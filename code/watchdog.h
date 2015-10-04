

class Watchdog {
	private:
		int eventsReset,
			wdt_counter;

	public:
		// called from section init3, before main(), to disable watchdog at startup and prevent auto-resetting endless loop
		static void wdt_first_disable(void);
		void wdt_init(int eventsToReset);
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
