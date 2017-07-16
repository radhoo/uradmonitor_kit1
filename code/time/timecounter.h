/**
 *	File:       	timecounter.h
 *	Version:  		1.0
 *	Date:       	2013
 *	License:		GPL v3
 *	Description:	time handling subsystem for AVR microcontrollers - implement a time counter
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
#include <avr/interrupt.h>
#include <avr/io.h>


typedef void (*TimeCallback)();

#define MAX_COUNTERS 3


class TimeCounter {
	uint16_t		milis;		// time: counts the intervals of 2.048ms (on 8MHz F_CPU) elapsed

	uint32_t 		total_sec,
					total_min,
					counter[MAX_COUNTERS];

	TimeCallback	ptrFuncSec,	// called when a full second has elapsed
					ptrFuncMin;	// a full minute has elapsed
	public:
	enum {
		TIME_COUNTER_1,
		TIME_COUNTER_2,
		TIME_COUNTER_3
	};
	TimeCounter() {
		milis = 0;
		total_sec = 0; total_min = 0;
		ptrFuncSec = 0; ptrFuncMin = 0;
	}

	void init(TimeCallback callSec, TimeCallback callMin);
	void TimerEvent();
	inline uint32_t getTotalSec() { return total_sec; };
	inline uint32_t getTotalMin() { return total_min; };

	void startCount(uint8_t index);
	uint32_t getCount(uint8_t index);
	uint8_t getSec();
	uint8_t getMin();
	uint8_t getHour();
};


