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

/**
  * Suggestion to disable unused sensors:
  * - Add logic in the backend that if the provided unused sensor value is e.g. Not Applicable -> "NA" then the graph is no longer displayed/ rendered.
  * - If it has a value greater or equal 0 it renders the graph with the uploaded value.
  * This prevents unnecessary graph loads for the webserver.
*/

#define ID_TIME_SECONDS "01"			// compulsory: local time in seconds
#define ID_TEMPERATURE_CELSIUS "02"		// optional: temperature in degrees celsius
#define ID_PRESSURE_PASCALS "03"		// optional: barometric pressure in pascals
#define ID_HUMIDITY_RH "04"				// optional: humidity as relative humidity in percentage %
#define ID_LUMINOSITY_RL "05"			// optional: luminosity as relative luminosity in percentage ‰
#define ID_VOC_OHM "06"					// optional: volatile organic compounds in ohms
#define ID_CO2_PPM "07"					// optional: carbon dioxide in ppm
#define ID_CH2O_PPM "08"				// optional: formaldehyde in ppm
#define ID_PM25_UGCM "09"				// optional: PM2.5 particulate matter in micro grams per cubic meter
#define ID_TUBE "10"					// optional: tube type ID
#define ID_PM01_UGCM "11"       // optional: PM1 particulate matter in micro grams per cubic meter
#define ID_PM10_UGCM "12"       // optional: PM10 particulate matter in micro grams per cubic meter
#define ID_NOISE_DB "13"        // optional: Noise in dB
#define ID_NOX_PPB "14"         // optional: Nitrogen dioxide in parts per billion (ppb)
#define ID_O3_PPB "15"          // optional: Ozone in parts per billion (ppb)
#define ID_SO2_UGCM "16"        // optional: Sulfur dioxide in micro grams per cubic meter
#define ID_CO_PPM "17"          // optional: carbon monoxide in parts per million (ppm)
#define ID_H2S_PPM "18"         // optional: Hydrogen Sulfide in parts per million (ppm)
#define ID_PhH_PPM "19"         // optional: Benzene in parts per million (ppm)
#define ID_Rn_CPM "20"          // optional: Radon measured on geiger tube in cpm
#define ID_BATTERY_VOLTS "0A"			// optional: device battery voltage in volts
#define ID_GEIGER_CPM "0B"				// optional: average radiation measured on geiger tube in cpm
#define ID_GEIGER_CPM "0B1"				// optional: average radiation measured on geiger tube 01 in cpm
#define ID_GEIGER_CPM "0B2"				// optional: average radiation measured on geiger tube 02 in cpm
#define ID_INVERTERVOLTAGE_VOLTS "0C"	// optional: high voltage geiger tube inverter voltage in volts
#define ID_INVERTERDUTY_PM "0D"			// optional: high voltage geiger tube inverter duty in ‰
#define ID_VERSION_HW "0E"				// optional: hardware version
#define ID_VERSION_SW "0F"				// optional: software firmware version
