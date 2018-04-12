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

// not more than 10 chars!
char *aux_detectorName(uint8_t param) {
	switch (param) {
		case GEIGER_TUBE_SBM20: return "SBM20";
		case GEIGER_TUBE_SI29BG: return "SI29BG";
		case GEIGER_TUBE_SBM19: return "SBM19";
		case GEIGER_TUBE_LND712: return "LND712";
		case GEIGER_TUBE_SBM20M: return "SBM20M";
		case GEIGER_TUBE_SI22G: return "SI22G";
		case GEIGER_TUBE_STS5: return "STS5";
		case GEIGER_TUBE_SI3BG: return "SI3BG";
		case GEIGER_TUBE_SBM21: return "SBM21";
		case GEIGER_TUBE_SBT9: return "SBT9";
		case GEIGER_TUBE_SI1G: return "SI1G";
		case GEIGER_TUBE_SI8B: return "SI8B";
		case GEIGER_TUBE_SBT10A: return "SBT10A";
		default: return "unknown";
	}
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
		case GEIGER_TUBE_SBM20: 	return 0.006315; // CPM 19
		case GEIGER_TUBE_SI29BG: 	return 0.010000; // CPM 12
		case GEIGER_TUBE_SBM19: 	return 0.001500; // CPM 80
		case GEIGER_TUBE_LND712: 	return 0.005940; // CPM 20.20
		case GEIGER_TUBE_SBM20M:	return 0.013333; // CPM 9
		case GEIGER_TUBE_SI22G: 	return 0.001714; // CPM 70
		case GEIGER_TUBE_STS5: 		return 0.006666; // CPM 18
		case GEIGER_TUBE_SI3BG: 	return 0.631578; // CPM 0.19
		case GEIGER_TUBE_SBM21: 	return 0.048000; // CPM 2.5
		case GEIGER_TUBE_SBT9: 		return 0.010900; // CPM 11
		case GEIGER_TUBE_SI1G:		return 0.006000; // CPM 20
		case GEIGER_TUBE_SI8B:		return 0.001108; // 
		case GEIGER_TUBE_SBT10A:	return 0.001105; // 
		default: 0;
	}
}

// dose equivalent linear approximative conversion from CPM
float aux_CPM2uSVh(uint8_t tube, uint32_t cpm) {
	return aux_detectorFactor(tube) * cpm;
}
