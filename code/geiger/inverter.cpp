/**
 *	File:       	inverter.cpp
 *	Version:  		1.0
 *	Date:       	2013
 *	License:		GPL v3
 *	Description:	high voltage inverter class to drive a Geiger tube for AVR microcontrollers
 *	Project:		uRADMonitor KIT1, a hackable digital sensor monitoring tool with network interface
 *
 *	Copyright 2013 by Radu Motisan, radu.motisan@gmail.com
 *	Copyright 2016 by Magnasci SRL, www.magnasci.com
 *
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 * 	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 * 	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * 	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "../geiger/inverter.h"

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
