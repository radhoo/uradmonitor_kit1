/**
 *	File:       	inverter.h
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

#pragma once

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

/************************************************************************/
/* Configuration                                                        */
/************************************************************************/

#define INVERTER_DEFAULT_DUTY	11			// start with low duty to allow powering the unit from higher voltages as well
#define INVERTER_DUTY_MIN		10			// inverter duty cycle will not go below 1%
#define INVERTER_DUTY_MAX		900			// inverter duty cycle will not go above 49% - without discharge transistor!
#define INVERTER_FREQUENCY		13000UL		// inverter frequency in Hertz

#define INVERTER_THRESHOLD		380			// inverter output voltage target
#define INVERTER_TOLERANCE		5			// tolerance for adjusting the duty cycle to control the voltage

class INVERTER {
	uint16_t duty;								// used to set the duty cycle to adjust the output voltage

	void setDuty(uint16_t d);

	public:

	// CREATE Timer T1 PWM to drive inverter for regulated Geiger tube voltage
	void initPWM();

	// check tube voltage and adjust duty cycle to match tube given threshold level (400V)
	// return false on critical error (when we hit the duty max), true if all ok
	bool adjustDutyCycle(uint16_t measuredVoltage, uint16_t targetVoltage = INVERTER_THRESHOLD);

	uint16_t getDuty();
};
