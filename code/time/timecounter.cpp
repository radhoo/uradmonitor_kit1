/*
    File:       timecounter.cpp
    Version:    0.1.0
    Date:       July 10, 2013
	License:	GPL v2
    
	class code, defining a time handling subsystem
    
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




#include "timecounter.h"
#include "../timeout.h"
#include <avr/interrupt.h>
#include <avr/io.h>

// init function used to set a callback to be called every new minute
void  TIMECOUNTER::init(TimeCallback callSec, TimeCallback callMin) {
	ptrFuncSec = callSec;
	ptrFuncMin = callMin;
	
	TIMSK0 |= (1 << TOIE0); //atmega168
	sei(); // enable interrupts
	TCCR0B |= _BV(CS01) | _BV(CS00); // set prescaler to 64 and start the timer

}


uint8_t TIMECOUNTER::getSec() { return total_sec % 60; }
uint8_t TIMECOUNTER::getMin() { return (total_sec / 60) % 60; }
uint8_t TIMECOUNTER::getHour() { return (total_sec / 3600) % 24; }
//int TIMECOUNTER::getDay() { return day; }
//int TIMECOUNTER::getYear() { return year; }
