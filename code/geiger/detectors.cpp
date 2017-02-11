/**
 *	File:       	detectors.cpp
 *	Version:  		1.0
 *	Date:       	2015
 *	License:		GPL v3
 *	Description:	define Geiger tubes and conversion factors for CPM to equivalent dose
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

#include "detectors.h"
#include <avr/pgmspace.h>

// not more than 8 chars!
const char *aux_detectorName(uint8_t param) {
	static char ret[8];
	const char *val = 0;
	switch (param) {
		case GEIGER_TUBE_SBM20: val = PSTR("SBM20"); break;
		case GEIGER_TUBE_SI29BG:val = PSTR("SI29BG"); break;
		case GEIGER_TUBE_SBM19: val = PSTR("SBM19"); break;
		case GEIGER_TUBE_STS5:  val = PSTR(" STS5"); break;
		case GEIGER_TUBE_SI22G: val = PSTR("SI22G"); break;
		case GEIGER_TUBE_SI3BG: val = PSTR("SI3BG"); break;
		case GEIGER_TUBE_SBM21: val = PSTR("SBM21"); break;
		case GEIGER_TUBE_SI1G:  val = PSTR(" SI1G"); break;
		default: val = PSTR("unknown"); break;
	}
	strncpy_P(ret, val, sizeof(ret));
}

uint8_t getDetector(uint8_t param) {
	switch (param) {
		case 0x11: return GEIGER_TUBE_SBM20;
		case 0x12: return GEIGER_TUBE_SI29BG;
		case 0x41: return GEIGER_TUBE_SBM20;
		case 0x51: return GEIGER_TUBE_SBM20;
		case 0x82: return GEIGER_TUBE_SI29BG;
		default: return GEIGER_TUBE_UNKNOWN;
	}
}

float aux_detectorFactor(uint8_t param) {
	switch (param) {
		case GEIGER_TUBE_SBM20: 	return 0.006315;
		case GEIGER_TUBE_SI29BG: 	return 0.010000;
		case GEIGER_TUBE_SBM19: 	return 0.001500;
		case GEIGER_TUBE_STS5: 		return 0.006666;
		case GEIGER_TUBE_SI22G: 	return 0.001714;
		case GEIGER_TUBE_SI3BG: 	return 0.631578;
		case GEIGER_TUBE_SBM21: 	return 0.048000;
		case GEIGER_TUBE_SI1G:		return 0.006000;
		default: 0;
	}
}

// dose equivalent linear approximative conversion from CPM
float aux_CPM2uSVh(uint8_t tube, uint32_t cpm) {
	return aux_detectorFactor(tube) * cpm;
}
