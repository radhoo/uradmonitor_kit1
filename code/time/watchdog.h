/**
 *	File:       	watchdog.h
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

class Watchdog {
	private:
	uint32_t eventsReset,
			wdt_counter;

	public:
		// called from section init3, before main(), to disable watchdog at startup and prevent auto-resetting endless loop
		static void wdt_first_disable(void);
		void wdt_init(uint32_t secondsToReset);
		void wdt_setRebootSeconds(uint32_t secondsToReset);
		uint32_t wdt_getRebootSeconds();
		void wdt_my_reset();
		inline void timerEvent();
		int wdt_get_counter();
		static void wdt_hw_reboot();
};

void Watchdog::timerEvent() {
	wdt_counter++;
	if (wdt_counter < eventsReset) {
		wdt_reset();					// start timer again (we are still in interrupt-only mode)
	} else {
		// go for reset
		wdt_hw_reboot();
	}
}
