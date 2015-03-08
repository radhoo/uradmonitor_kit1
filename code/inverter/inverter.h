/*
    File:       inverter.h
    Version:    0.1.0
    Date:       July 10, 2013
	License:	GPL v2
    
	header file, handling the ferrite core inverter for a geiger tube (400V)
    
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

#pragma once

// This class assumes the inverter signal is connected to pin PB1 on an ATmega uC

/************************************************************************/
/* Configuration                                                        */
/************************************************************************/

#define INVERTER_DEFAULT_DUTY	11			// start with low duty to allow powering the unit from higher voltages as well
#define INVERTER_DUTY_MIN		10			// inverter duty cycle will not go below 1%
#define INVERTER_DUTY_MAX		490			// inverter duty cycle will not go above 49% - without discharge transistor!
#define INVERTER_FREQUENCY		18000UL		// inverter frequency in Hertz
//extern uint32_t INVERTER_FREQUENCY;
// SBM20:350 - 475V
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
