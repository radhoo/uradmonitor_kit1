/**
 *	File:       	morse.cpp
 *	Version:  		1.0
 *	Date:       	2016
 *	License:		GPL v3
 *	Description:	international Morse implementation for AVR microcontrollers
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

#include "morse.h"

void Morse::binary(uint8_t decimal) {
	if (decimal) {
		binary(decimal/2);
		if (decimal != 1) decimal%2 ? dash() : dot();
	}
}

void Morse::encode(char c) {
	if (c == ' ') {
		end();end();end();end();end();end();end();
	} else {
		const char *latin = "**ETIANMSURWDKGOHVF*L*PJBXCYZQ**"; //32
		if (c >= 'a' && c <= 'z') c -= 32; // convert to uppercase
		if (c < 'A' || c > 'Z') return;
		uint8_t i = 0;
		while (latin[++i] != c);
		binary(i);
		end();end();end();
	}
}

void Morse::encode(char *s) {
	char *k = s;
	while (*k) encode(*k++);
}

void Morse::init(void (*callbackdot)(),void (*callbackdash)(), void (*callbackend)()) {
	dot = callbackdot;
	dash = callbackdash;
	end = callbackend;
}
