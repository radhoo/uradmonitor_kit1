/**
 *	File:       	gui.cpp
 *	Version:  		1.0
 *	Date:       	2016
 *	License:		GPL v3
 *	Description:	uRADMonitor user interaction file for the KIT1
 *	Project:		uRADMonitor KIT1, a hackable digital sensor monitoring tool with network interface
 *
 *	Copyright 2016 by Radu Motisan, radu.motisan@gmail.com
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

#include "UI.h"

#include <avr/pgmspace.h>

UI::UI(LCD_5110 *lcd, DigitalPin *speaker, DigitalPin *button, Data *data, char *buffer, uint16_t size) {
	m_page = UI_PAGE_HOME;
	m_lcd = lcd;
	m_speaker = speaker;
	m_button = button;
	m_data = data;
	m_buffer = buffer;
	m_size = size;
}

// return 4 char LCD battery symbol
 char* UI::getSymbolBattery(uint16_t voltage) {
	 static char battery[4][4] = { {0x81,0x82,0x83,0},{0x84,0x85,0x86,0},{0x87,0x88,0x89,0},{0x8A,0x8B,0x8C,0}};
	 if (voltage > 3100) return battery[0]; //charging
	 else if (voltage > 2800) return battery[1]; //full
	 else if (voltage > 2500) return battery[2]; //half
	 else return battery[3]; //empty
}

 // to save display space, we use multipliers
 float UI::factorDose(float dose) {
	 if (dose < 10) return dose; // show in micros
	 else if (dose < 10000) return dose / 1000.0; // show in milis
	 else return dose / 1000000.0; // show in units
 }

 // get multiplier symbol
 char UI::factorDoseSymbol(float dose) {
	 if (dose < 10) return 0x8D; // show in micros
	 else if (dose < 10000) return 'm'; // show in milis
	 else return ' '; // show in units
 }

 // apply multiplication factor to CPM
 uint16_t UI::factorCPM(uint32_t cpm) {
	 if (cpm < 1000) return (uint16_t)cpm;
	 else if (cpm < 1000000) return cpm / 1000;
	 else return cpm / 1000000UL;
 }

 // get multiplication factor symbol for CPM
 char UI::factorCPMSymbol(uint32_t cpm) {
	 if (cpm < 1000) return ' ';
	 else if (cpm < 1000000) return 'K';
	 else return 'M';
 }

 // get string label for various dose thresholds
 char* UI::labelDose(float dose) {
	 static char label[10];
	 if (dose < DOSE_THRESHOLD_LOW) strcpy_P(label, PSTR("   LOW    "));
	 else if (dose < DOSE_THRESHOLD_NORMAL) strcpy_P(label, PSTR("  NORMAL  "));
	 else if (dose < DOSE_THRESHOLD_HIGH) strcpy_P(label, PSTR("   HIGH   "));
	 else strcpy_P(label, PSTR("  DANGER  "));
	 return label;
 }

 void UI::drawPage() {
	 m_lcd->goto_xy(0,0);
	 switch (m_page) {
	 	 case UI_PAGE_HOME:
	 		m_lcd->send(m_buffer, m_size,
				PSTR("%02d:%02d:%02d%c %c%s"
#ifdef USE_BME280_SENSOR
					"%05.1f %04lu %03u"
					"%c%cC%c%c%c%ch%c%c%cRH%c"

#else
					"              "
					"%c%c%c%c%c%c%c%c%c%c%c%c%c%c"
#endif

					"%c %5.2f %c%s%c"
					"%c%c%s%c%c"
					"%3dV %02d%% %03d%c%c"),
				// line 1: time, state symbols and battery
	 			m_data->getTimeCounter()->getHour(), m_data->getTimeCounter()->getMin(), m_data->getTimeCounter()->getSec(),
				(m_data->getStateSpeaker() == Data::ENABLED)?0x97:0x98,
				(m_data->getStateNetwork() == Data::ENABLED)?0x99:0x9A,
				getSymbolBattery(m_data->getBatteryVoltage()),
				// line 2 extra sensor
#ifdef USE_BME280_SENSOR
				m_data->getTemperature(), m_data->getPressure() / 100, m_data->getHumidity(),
				// line 3: separator

				0x90,0x9B,0x94,0x94,0x94,0x94,0x9C,0x94,0x94,0x91,
#else
				// line 3: separator
				0x90,0x94,0x94,0x94,0x94,0x94,0x94,0x94,0x94,0x94,0x94,0x94,0x94,0x91,
#endif
				// line 4: dose
	 			0x92, factorDose(m_data->getGeigerDose()), (m_data->getGeigerDose() > 0 ? factorDoseSymbol(m_data->getGeigerDose()):' '),(m_data->getGeigerDose() > 0?"Sv/h":"wait"),0x93,
				// line 5: dose label
	 			0x95,0x94, labelDose(m_data->getGeigerDose()), 0x94,0x96,
				// line 6: voltage, duty, CPM
	 			m_data->getInverterVoltage(), m_data->getInverterDuty()/10, factorCPM(m_data->getGeigerCPM()), factorCPMSymbol(m_data->getGeigerCPM()), 0x8F);
	 		break;
	 	 case UI_PAGE_STATS:
	 		m_lcd->send(m_buffer, m_size, PSTR(
					"KIT1  %08lX" \
					"Time: %02d:%02d:%02d" \
					"              " \
					"              " \
					"              " \
					"%s tube %d"),
	 			// line 1: id
	 			m_data->getDeviceID(),
				// line 2: time
				m_data->getTimeCounter()->getHour(), m_data->getTimeCounter()->getMin(), m_data->getTimeCounter()->getSec(),
				// line 3: total pulses from power on

				// line 4: absolute average CPM

				// line 5: maximum CPM

				// line 6: geiger tube and firmware version
				aux_detectorName(GEIGER_TUBE), VER_SW);
			break;
	 	 case UI_PAGE_NETWORK:
	 		if (m_data->getStateNetwork() == Data::ENABLED) {
				// 14chars
				m_lcd->send(m_buffer, m_size, PSTR(
					"%3u.%3u.%3u%3u" \
					"%3u.%3u.%3u%3u" \
					"%3u.%3u.%3u%3u" \
					"%3u.%3u.%3u%3u" \
					"%4u/%4u/%4u" \
					"Upload %04dsec"),
				m_data->getIPLocal()[0],m_data->getIPLocal()[1],m_data->getIPLocal()[2],m_data->getIPLocal()[3],
				m_data->getIPGateway()[0],m_data->getIPGateway()[1],m_data->getIPGateway()[2],m_data->getIPGateway()[3],
				m_data->getIPNetmask()[0],m_data->getIPNetmask()[1],m_data->getIPNetmask()[2],m_data->getIPNetmask()[3],
				m_data->getIPRemote()[0],m_data->getIPRemote()[1],m_data->getIPRemote()[2],m_data->getIPRemote()[3],
				m_data->getNetworkPings(), m_data->getNetworkPacketsOK(), m_data->getNetworkPacketsTotal(),
				m_data->getNetworkSendInterval());
			} else {
				m_lcd->send(PSTR("No Ethernet.\nConnect cable and restart."));
			}
	 		break;
	 }
}

void UI::loop(bool *refresh) {
	// read sensors fast do directly, slow only on refresh (default once per second)
	m_data->readSensorsFast();
	if (m_data->getStateRead()) {
		m_data->readSensorsSlow();
		// check result and disable reading if done
		m_data->setStateRead(Data::DISABLED);
	}

	// threshold to sound alarm reached, enable alarm and jump to page 0
	if (m_data->getGeigerDose() >= DOSE_THRESHOLD_HIGH) {
		m_data->setStateAlarm(Data::ENABLED);
		m_page = UI_PAGE_HOME;
	} else {
		m_data->setStateAlarm(Data::DISABLED);
		*m_speaker = Data::DISABLED;
	}

	// handle button press
	uint8_t button = BUTTON_NOPRESS;
	if (!m_data->getStatePressed() && !*m_button) { // button down
		m_data->setStatePressed(Data::ENABLED);
		m_data->getTimeCounter()->startCount(TimeCounter::TIME_COUNTER_1);
	} else if (m_data->getStatePressed() && *m_button) { // button up
		m_data->setStatePressed(Data::DISABLED);
		// check for how long the button was pressed
		if (m_data->getTimeCounter()->getCount(TimeCounter::TIME_COUNTER_1) > LONG_PRESS)
			button = BUTTON_PRESS_LONG;
		else
			button = BUTTON_PRESS_SHORT;
	}
	switch (button) {
		case BUTTON_PRESS_SHORT:
			// if button is pressed while alarm is on, stop alarm
			if (m_data->getStateAlarm()) {
				m_data->setStateAlarm(Data::DISABLED);
				*m_speaker = Data::DISABLED;
				m_data->setGeigerCPM(0); // reset readings, alarm might sound again
			}
			// turn screen on
			m_lcd->setBacklight(Data::ENABLED);
			m_data->getTimeCounter()->startCount(TimeCounter::TIME_COUNTER_2);
			// change page
			m_page = (m_page + 1) %3;
			m_lcd->clear();
		break;
		case BUTTON_PRESS_LONG:
			// toggle speaker
			m_data->setStateSpeaker(!m_data->getStateSpeaker());
			// if speaker turned off, stop speaker
			if (m_data->getStateSpeaker() == Data::DISABLED) *m_speaker = 0;
		break;
		default: break;
	}

	// handle GUI display
	if (*refresh) {
		// draw GUI
		drawPage();
	}

	// turn backlight off when timeout is reached, only if alarm is off, because in alarm mode screen is flickering
	if (BACKLIGHT_TIMEOUT > 0 && !m_data->getStateAlarm() && m_data->getTimeCounter()->getCount(TimeCounter::TIME_COUNTER_2) > BACKLIGHT_TIMEOUT) {
		m_lcd->setBacklight(Data::DISABLED);
	}

	if (m_data->getStateBeep()) {
		if (m_page == 0) {
			m_lcd->goto_xy(9,0); m_lcd->send(0x8E); // show radiation sign on page 0
			// do a beep if alarm if off but speaker is not muted
			if (!m_data->getStateAlarm() && m_data->getStateSpeaker()) {
				*m_speaker = Data::ENABLED; _delay_ms(10); *m_speaker = Data::DISABLED;
			}
		}
		// done, disable state
		m_data->setStateBeep(Data::DISABLED);
	}

	// done until next refresh event (next second)
	*refresh = 0;
}
