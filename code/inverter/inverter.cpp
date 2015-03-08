/*
    File:       inverter.cpp
    Version:    0.1.0
    Date:       July 10, 2013
	License:	GPL v2
    
	class code, handling the ferrite core inverter for a geiger tube (400V)
    
    ****************************************************************************
    Copyright (C) 2013 Radu Motisan  <radu.motisan@gmail.com>
	
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

#include "../timeout.h"
#include "inverter.h"
#include <avr/io.h>
#include <avr/interrupt.h>

//uint32_t INVERTER_FREQUENCY = 1000UL;

void INVERTER::setDuty(uint16_t d) {
	duty = d;
	OCR1A = (uint16_t)( (float)ICR1 * (float) duty  / 1000.0);
}

// CREATE Timer T1 PWM to drive inverter for regulated Geiger tube voltage 
void INVERTER::initPWM() {
	TCCR1A = 0;     // disable all PWM on Timer1 whilst we set it up
	DDRB |= _BV(PB1); // Set PB1 as output (PB1 is OC1A)
	ICR1 = F_CPU / INVERTER_FREQUENCY; // set the frequency FCPU/(ICR1 * PRESCALLING) Hz . Prescalling set to 1X
	setDuty(INVERTER_DEFAULT_DUTY);
	TCCR1B = (1 << WGM13) | (1<<WGM12) | (1 << CS10); //Fast PWM mode via ICR1, with no prescaling (CS11 = 8x, CS10 = 1x)
	TCCR1A |= (1 << COM1A1) | (1<< CS11); // set none-inverting mode and start PWM
}

// check tube voltage and adjust duty cycle to match tube given threshold level (400V)
bool INVERTER::adjustDutyCycle(uint16_t measuredVoltage, uint16_t targetVoltage) {
	if ( (measuredVoltage >= targetVoltage + INVERTER_TOLERANCE) && (duty > INVERTER_DUTY_MIN)) {
		setDuty(duty - 1); // we need to decrease duty cycle to decrease voltage
	}
	if ( (measuredVoltage <= targetVoltage - INVERTER_TOLERANCE) && (duty < INVERTER_DUTY_MAX)) {
		setDuty(duty + 1); // we need to increase duty cycle to increase voltage
	}
	if (duty == INVERTER_DUTY_MAX) return false; // error
	else return true;
}

uint16_t INVERTER::getDuty() {
	return duty;
}
