/**
 *	File:       	config.cpp
 *	Version:  		1.0
 *	Date:       	2016
 *	License:		GPL v3
 *	Description:	config file for uRADMonitor code
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

// USER CONFIGURATION SPACE
// additional sensors sample
// #define USE_BME280_SENSOR

#define USE_ETHERNET

// user upload credetials, create your account on www.uradmonitor.com/dashboard
// the Dashboard shows the user-id and the user-key
#define				USER_ID					""
#define				USER_KEY				""

// PRIVATE CONFIGURATION SPACE, don't change the values below unless you know what you are doing
// device identification
#define				DEV_CLASS				0x13
#define				VER_SW					122
#define				VER_HW					105

// constants for this device
#define				P_TUBEVOLFB_PIN			PC2		// voltage divider to measure high voltage
#define				P_TUBEVOLFB_R1			10000	// voltage divider to measure high voltage
#define				P_TUBEVOLFB_R2			47		// voltage divider to measure high voltage
#define				GEIGER_TUBE				GEIGER_TUBE_SBM20
#define				WARMUP					0		// initial warmup interval before sensors are ready, use only if needed
#define				READ_INTERVAL			5		// interval in seconds to read sensors

// user interaction
#define				LONG_PRESS				2		// 2 seconds on central button count as a long press
#define 			BACKLIGHT_TIMEOUT		10		// turn backlight in 10 seconds of inactivity to conserve power
#define				WATCHDOG_INTERVAL		600		// seconds to wait until a reboot IF the server doesn't answer

// geiger tube radiation dose thresholds
#define 			DOSE_THRESHOLD_LOW 		0.10
#define 			DOSE_THRESHOLD_NORMAL 	0.20
#define 			DOSE_THRESHOLD_HIGH 	1.00

// uradmonitor upload api
#define				URADMONITOR_SERVER 		"data.uradmonitor.com"
#define 			URADMONITOR_SCRIPT 		"/api/v1/upload/exp/"



