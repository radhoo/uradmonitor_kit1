/**
 *	File:       	watchdog.cpp
 *	Version:  		1.0
 *	Date:       	2014
 *	License:		GPL v3
 *	Description:	watchdog implementation, including stretched configurable time interval
 *	Project:		uRADMonitor KIT1, a hackable digital sensor monitoring tool with network interface
 *
 *	Copyright 2014 by Radu Motisan, radu.motisan@gmail.com
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

#include <avr/interrupt.h>
#include <avr/wdt.h>
#include "watchdog.h"

// Clear SREG_I on hardware reset.
void Watchdog::wdt_first_disable(void) {
	// Note that for newer devices (any AVR that has the option to also
	// generate WDT interrupts), the watchdog timer remains active even
	// after a system reset (except a power-on condition), using the fastest
	// prescaler value (approximately 15 ms). It is therefore required
	// to turn off the watchdog early during program startup.
	MCUSR = 0; // clear reset flags
	wdt_disable();
	// http://www.atmel.com/webdoc/AVRLibcReferenceManual/FAQ_1faq_softreset.html
}

void Watchdog::wdt_init(uint32_t eventsToReset) {
	wdt_counter = 0;
	// the unit will autoreset, if wdt_my_reset() is not called before this number of seconds
	eventsReset = eventsToReset;
	// disable interrupts
	//cli();
	// configure watchdog for interrupt only , 1second prescale
	MCUSR  &= ~_BV(WDRF);
	WDTCSR = (1<<WDCE) | (1<<WDE);   				// Enable the WD Change Bit - configure mode
	WDTCSR = (1<<WDIE) | (1<<WDP2) | (1<<WDP1);	// Enable WDT Interrupt , Set Timeout to ~1 second

	// enable interrupts
	sei();
}

void Watchdog::wdt_setRebootSeconds(uint32_t eventToReset) {
	eventsReset = eventToReset;
}

uint32_t Watchdog::wdt_getRebootSeconds() {
	return eventsReset;
}

int Watchdog::wdt_get_counter() {
	return wdt_counter;
}

void Watchdog::wdt_my_reset() {
	wdt_counter = 0;
	wdt_reset();
}

void Watchdog::wdt_hw_reboot() {
	WDTCSR = (1<<WDCE) | (1<<WDE);	// Enable the WD Change Bit - configure mode
	WDTCSR = (1<<WDE) | (1<<WDP0);	// set reset flag (WDE) and 16ms (WDP0)
}





