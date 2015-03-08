/*
    File:       uRADMonitor.cpp
    Version:    3.0.102 Hardware rev.100 Software rev.101
    Date:       March, 2015
	License:	GPL v2
    
	DIY Dosimeter Geiger Counter Kit with network capabilities
    
    ****************************************************************************
    Copyright (C) 2015 Radu Motisan  <radu.motisan@gmail.com>

	http://www.pocketmagic.net

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
    ****************************************************************************
 */


#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <string.h>
#include <stdio.h>
#include "timeout.h" // defines F_CPU
#include "misc/aux.h"
#include "misc/detectors.h"
#include "introscreen.h"
#include "lcd/5110.h"  
#include "io/DigitalOut.h"
#include "io/DigitalIn.h"
#include "time/timecounter.h"
#include "inverter/inverter.h"



#ifdef ETHERNET
#endif

/************************************************************************************************************************************************/
/* Constants                                                            																		*/
/************************************************************************************************************************************************/
#define				VER_HW					101
#define				VER_SW					102
#define				DEV_RAD_DETECTOR		GEIGER_TUBE_SBM20 	// 1=geiger_SBM20 , 2=geiger_SI29BG, 3=geiger_SBM19
#define 			PARTIAL_INTERVAL 		5					// seconds to display first results: must divide 60 for precise calculations
#define 			BACKLIGHT_TIMEOUT		10					// turn backlight in 10 seconds of inactivity to conserve power
#define				DEV_MODEL				0
#define				DEV_ID					0x1				 	// change here for new devices !!!


#define DOSE_THRESHOLD_LOW 0.10
#define DOSE_THRESHOLD_NORMAL 0.20
#define DOSE_THRESHOLD_HIGH 1.00
/************************************************************************************************************************************************/
/* Global Objects                                                       																		*/
/************************************************************************************************************************************************/
#define				MAX_UINT32				0xFFFFFFFF

uint32_t 			deviceID 				= ((uint32_t) ((DEV_MODEL << 4) | DEV_RAD_DETECTOR) << 24) | ((uint32_t) DEV_ID);
DigitalOut			speaker,									// used to create digital pulse on radiation event (pin PD4)
					backlight;									// used to toggle LCD light on and off
DigitalIn			button;
TIMECOUNTER			time;
INVERTER			inverter;									// handles the PWM on Timer1 to drive the Geiger tube inverter
LCD_5110			lcd;										// handle the LCD ops for drawing content onscreen

volatile uint32_t
					geigerPulses = 0,							// geiger: total number of pulses detected
					geigerCPM = 0,								// geiger: counts per minute
					geigerMaxCPM = 0,
					geigerTotalPulses = 0,
					geigerAVGCPM = 0;
bool				cmdPulse = 0,								// if true shows the pulse on the screen
					cmdRefresh = 0,								// if true will refresh display
					cmdAlarm = 0;								// if true , alarm will sound
float				geigerTotalDose = 0.0;

uint8_t				secTimeout = 1,
					page = 0;									// multiple content pages to display, toggle between them using the button


// callback function called from the timecounter object when a full minute has elapsed
void callback_timeMinute() {
	geigerCPM = geigerPulses;
	if (geigerCPM > geigerMaxCPM) geigerMaxCPM = geigerCPM;
	geigerPulses = 0;
	geigerTotalDose += aux_CPM2uSVh((uint8_t)DEV_RAD_DETECTOR, geigerCPM) / 60.0;
}

void callback_timeSecond() {

	// intermediary integration: every 5 sec
	if (time.getSec() % PARTIAL_INTERVAL == 0) {
		// not a full minute yet
		if (time.getSec() != 0) {
			int intervals = time.getSec() / PARTIAL_INTERVAL;
			if (geigerPulses > 0)
				geigerCPM = (60 * geigerPulses) / (PARTIAL_INTERVAL * intervals);
		}			
	}
	cmdRefresh = 1; // refresh display once per second

	if (cmdAlarm) {
		speaker.toggle();
		backlight.toggle();
	}
	secTimeout ++;
}

// int0 interrupt handler, triggered each time a rising edge is detected on INT0, which on a ATmega8 is PD2
// we have a top limit of 2^32-1 pulses. We don't go over it.
ISR (INT0_vect) {
	if (geigerPulses < MAX_UINT32)
		geigerPulses++; // count this pulse
	geigerTotalPulses ++;

	if (time.getTotalSec() > 0)
		geigerAVGCPM = (60 * geigerTotalPulses) / time.getTotalSec();

	cmdPulse = 1;	// signal a new pulse, to be handled in the main loop
}

// timer0 overflow interrupt   event to be executed every  2.048ms here when on 8MHz, and every 1.024ms for 16MHz */
ISR (TIMER0_OVF_vect) { time.TimerEvent(); }
  
// return 4 char LCD battery symbol
 char *batSymbol(int voltage) {
	 static char battery[4][4] = { {0x81,0x82,0x83,0},{0x84,0x85,0x86,0},{0x87,0x88,0x89,0},{0x8A,0x8B,0x8C,0}};
	 if (voltage > 3100) return battery[0]; //charging
	 else if (voltage > 2800) return battery[1]; //full
	 else if (voltage > 2500) return battery[2]; //half
	 else return battery[3]; //empty
}

 // to save display space, we use multipliers
 float getDoseMulFactor(float dose) {
	 if (dose < 10) return 1; // show in micros
	 else if (dose < 10000) return 0.001; // show in milis
	 else return 0.000001; // show in units
 }

 char getDoseMulSym(float dose) {
	 if (dose < 10) return 0x8D; // show in micros
	 else if (dose < 10000) return 'm'; // show in milis
	 else return ' '; // show in units
 }

 uint16_t getCPMAppliedFactor(uint32_t cpm) {
	 if (cpm < 1000) return (uint16_t)cpm;
	 else if (cpm < 1000000) return cpm / 1000;
	 else return cpm / 1000000UL;
 }

 char getCPMMulSym(uint32_t cpm) {
	 if (cpm < 1000) return ' ';
	 else if (cpm < 1000000) return 'K';
	 else return 'M';
 }

 char *getDoseLabel(float dose) {
	 static char label[10];

	 if (dose < DOSE_THRESHOLD_LOW) strcpy(label, "   LOW    ");
	 else if (dose < DOSE_THRESHOLD_NORMAL) strcpy(label, "  NORMAL  ");
	 else if (dose < DOSE_THRESHOLD_HIGH) strcpy(label, "   HIGH   ");
	 else strcpy(label, "  DANGER  ");

	 return label;
 }

/************************************************************************************************************************************************/
/* Main entry point                                                    																			*/
/************************************************************************************************************************************************/
 int main(void) {
	// 1. init speaker pin for output, button for input, and LCD screen
	speaker.init( &PORTD, PD3);
	button.init( &PORTC, PC3);
	backlight.init( &PORTD, PD4);
	lcd.init(&PORTB, PB0, //rst
			&PORTD, PD7, //ce
			&PORTD, PD6, //dc
			&PORTD, PD5, //data
			&PORTC, PC1);//clk
	// show splash screen for a second
	lcd.clear(); lcd.printPictureOnLCD(introScreen); _delay_ms(1000); lcd.clear();

	// 2.CONFIGURE INTERRUPT INT0  to count pulses from Geiger Counter, connected on PIN PD2
	// atmega8
	//MCUCR |= _BV(ISC00) | _BV(ISC01);// Configure INT0 to trigger on RISING EDGE
	//GICR |= _BV(INT0); // Configure INT0 to fire interrupts
	// atmega328p
	EICRA |= _BV(ISC00) | _BV(ISC01);// Configure INT0 to trigger on RISING EDGE - Instead of MCUCR, you need to use EICRA. mega8:MCUCR , mega328p: EICR
	EIMSK |= _BV(INT0); // Configure INT0 to fire interrupts - Instead of GICR, you need to use EIMSK mega8:GICR , mega328p: EIMSK
	
	// 3.CREATE Timer T0 to count seconds
	time.init(callback_timeSecond, callback_timeMinute);

	// 4.CREATE Timer T1 PWM to drive inverter for regulated Geiger tube voltage
	inverter.initPWM();

	// 5.Init other
	// 6.Init ETHERNET

	// turn LCD light on
	backlight.set(1);
	secTimeout = 0;

	// 7.Main program loop
	while (1) {
		// read sensors  and parameters once per second ?
		int batVol = aux_readIntRefVcc(); // battery (AVCC) voltage in mv
		int tubeVol = aux_readDivVoltage(batVol / 1000.0, 10000, 47, PC2); // read inverter voltage, via 10M/47K resistive divider, connected to pin ADC2
		// adjust duty cycle to regulate inverter output voltage, close to 	INVERTER_THRESHOLD	
		inverter.adjustDutyCycle(tubeVol);
		
		bool pressed = !button.get();

		float dose = aux_CPM2uSVh((uint8_t)DEV_RAD_DETECTOR, geigerCPM);

		if (cmdRefresh) {
			cmdRefresh = 0;
			lcd.goto_xy(0,0);
			if (page == 0) {
				lcd.send_format_string(
					"%02d:%02d:%02d %c %s" \
					"%1.3f%cSv   %1.1f" \
					"%c%c%c%c%c%c%c%c%c%c%c%c%c%c" \
					"%c %.2f %c%s %c" \
					"%c%c%s%c%c" \
					"%3dV %02d%% %03d%c%c"
					,
					time.getHour(),time.getMin(), time.getSec(), (pressed?'x':' '), batSymbol(batVol),
					geigerTotalDose * getDoseMulFactor(geigerTotalDose), getDoseMulSym(geigerTotalDose), batVol / 1000.0,
					0x90,0x94,0x94,0x94,0x94,0x94,0x94,0x94,0x94,0x94,0x94,0x94,0x94,0x91,
					0x92, dose * getDoseMulFactor(dose), (dose>0?getDoseMulSym(dose):' '),(dose > 0?"Sv/h":"wait"),0x93,
					0x95,0x94,getDoseLabel(dose),0x94,0x96,
					tubeVol, inverter.getDuty()/10, getCPMAppliedFactor(geigerCPM), getCPMMulSym(geigerCPM), 0x8F
					);
			} else if (page == 1) {
				if (geigerAVGCPM > geigerMaxCPM) geigerMaxCPM = geigerAVGCPM;
				lcd.send_format_string("Abs.  readings" \
					"Time: %02d:%02d:%02d" \
					"Pu: %010lu" \
					"Avg%c: %08lu" \
					"Max%c: %08lu" \
					"%s tube ",
					time.getHour(),time.getMin(), time.getSec(),
					geigerTotalPulses,
					0x8F, geigerAVGCPM,
					0x8F, geigerMaxCPM,
					aux_detectorName(DEV_RAD_DETECTOR));
			} else if (page == 2) {
				lcd.send_string("No Ethernet.");
			}
		}


		// turn backlight off when timeout is reached
		if (secTimeout > BACKLIGHT_TIMEOUT) {
			backlight.set(0);
			secTimeout = 0;
		}

		// if button pressed, change page and turn backlight on
		if (pressed) {
			// if button is pressed while alarm is on, stop alarm
			if (cmdAlarm) {
				cmdAlarm = 0;
				speaker.set(0);
				geigerCPM = 0; // reset readings, alarm might sound again
			}
			// turn screen on
			backlight.set(1);
			secTimeout = 0;
			// change page
			page = (page + 1) %3;
			lcd.clear();
		}

		if (dose >= DOSE_THRESHOLD_HIGH) {
			// threshold to sound alarm reached
			cmdAlarm = 1;
			// on alarm, move to page 0
			page = 0;
		} else {
			// alarm should be turned off
			cmdAlarm = 0;
			speaker.set(0);
		}


		if (cmdPulse) {
			if (page == 0) {
				lcd.goto_xy(9,0); lcd.send_chr(0x8E); // show radiation sign on page 0
				// do a beep if alarm if off
				if (!cmdAlarm) {
					speaker.set(1); _delay_ms(10); speaker.set(0);
				}
			}
			cmdPulse = false;
		}
	}
	return (0);
}  
