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

void Watchdog::wdt_init(int eventsToReset) {
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





