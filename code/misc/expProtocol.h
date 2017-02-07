/**
 *	File:       	expProtocol.h
 *	Version:  		1.0
 *	Date:       	2016
 *	License:		GPL v3
 *	Description:	exp is a restful api compatible protocol for uRADMonitor KIT1 data upload
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

#define ID_TIME_SECONDS "01"			// compulsory: local time in seconds
#define ID_TEMPERATURE_CELSIUS "02"		// optional: temperature in degrees celsius
#define ID_PRESSURE_PASCALS "03"		// optional: barometric pressure in pascals
#define ID_HUMIDITY_RH "04"				// optional: humidity as relative humidity in percentage %
#define ID_LUMINOSITY_RL "05"			// optional: luminosity as relative luminosity in percentage ‰
#define ID_VOC_OHM "06"					// optional: volatile organic compounds in ohms
#define ID_CO2_PPM "07"					// optional: carbon dioxide in ppm
#define ID_CH2O_PPM "08"				// optional: formaldehyde in ppm
#define ID_PM25_UGCM "09"				// optional: particulate matter in micro grams per cubic meter
#define ID_TUBE_TYPE "10"				// optional: tube type
#define ID_BATTERY_VOLTS "0A"			// optional: device battery voltage in volts
#define ID_SBM20_CPM "0B"				// optional: radiation measured on SBM20 geiger tube in cpm
#define ID_INVERTERVOLTAGE_VOLTS "0C"	// optional: high voltage geiger tube inverter voltage in volts
#define ID_INVERTERDUTY_PM "0D"			// optional: high voltage geiger tube inverter duty in ‰
#define ID_VERSION_HW "0E"				// optional: hardware version
#define ID_VERSION_SW "0F"				// optional: software firmware version
