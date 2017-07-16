/**
 *	File:       	DigitalPin.cpp
 *	Version:  		1.0
 *	Date:       	2015
 *	License:		GPL v3
 *	Description:	I/O control general class for AVR microcontrollers
 *	Project:		uRADMonitor KIT1, a hackable digital sensor monitoring tool with network interface
 *
 *	Copyright 2015 by Radu Motisan, radu.motisan@gmail.com
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

#include "DigitalPin.h"

void DigitalPin::config(Type type) {
	if (type == INPUT) {
		*PORT2DDR(m_pport) &= mask0;
	} else {
		*PORT2DDR(m_pport) |= mask1;
	}
}

// Set the output, specified as 0 or 1 (int)
void DigitalPin::write(bool state) {
	state? *m_pport |= mask1 : *m_pport &= mask0;
	m_state = state;
}

// Return the output setting, represented as 0 or 1 (bool)
bool DigitalPin::read() {
	if (m_type == OUTPUT)
		return m_state;
	else
		return *PORT2PIN(m_pport) & mask1;
}

// negate state
bool DigitalPin::toggle() {
	bool state = !read();
	write(state);
	return state;
}

