/**
 *	File:       	gui.h
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

#pragma once

#include <stdint.h>
#include "../lcd/5110.h"
#include "../geiger/detectors.h"
#include "../config.h"
#include "data.h"

class UI {
	enum {
		UI_PAGE_HOME = 0,
		UI_PAGE_STATS = 1,
		UI_PAGE_NETWORK = 2
	};
	enum {
		BUTTON_NOPRESS = 0,
		BUTTON_PRESS_SHORT = 1,
		BUTTON_PRESS_LONG = 2,
	};
	LCD_5110 *m_lcd;
	DigitalPin *m_speaker, *m_button;
	Data *m_data;
	char *m_buffer;
	uint8_t m_page;
	uint16_t m_size;

public:
	// The GUI is an interface between user via the LCD, and Data
	UI(LCD_5110 *lcd, DigitalPin *speaker, DigitalPin *button, Data *data, char *buffer, uint16_t size);



	// returns the battery symbol based on voltage level, given in milivolts
	char *getSymbolBattery(uint16_t voltage);

	// to save display space, we use multipliers
	float factorDose(float dose);

	// get multiplier symbol
	char factorDoseSymbol(float dose);

	// apply multiplication factor to CPM
	uint16_t factorCPM(uint32_t cpm);

	// get multiplication factor symbol for CPM
	char factorCPMSymbol(uint32_t cpm);

	// get string label for various dose thresholds
	char* labelDose(float dose);

	void drawPage();

	void loop(bool *refresh);
};
