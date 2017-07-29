/**
 *	File:       	data.cpp
 *	Version:  		1.0
 *	Date:       	2016
 *	License:		GPL v3
 *	Description:	uRADMonitor sensor and data interface
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
#include "data.h"

Data::Data(TimeCounter *time) {
	m_time = time;
	stateAlarm = DISABLED;
	stateBeep = DISABLED;
	stateNetwork = DISABLED;
	stateRead = DISABLED;
	stateSend = DISABLED;
	stateSpeaker = DISABLED;
	statePressed = DISABLED;
	geigerCPM = 0;
	geigerDose = 0;
	batteryVoltage = 0;
	inverterVoltage = 0;
	inverterDuty = 0;
	deviceID = 0;
	networkPacketsTotal = 0;
	networkPacketsOK = 0;
	networkPings = 0;
}

// return objects
TimeCounter* Data::getTimeCounter() { return m_time; }
INVERTER Data::getInverter() { return m_inverter; }

// Device ID access calls
uint32_t Data::getDeviceID() { return deviceID; }
void Data::setDeviceID_Mac(uint32_t id) {
	deviceID = id;
	macLocal[0] = 0x2; // locally administered
	macLocal[1] = 0x0;
	macLocal[2] = (uint8_t)((deviceID >> 24) & 0xFF);
	macLocal[3] = (uint8_t)((deviceID >> 16) & 0xFF);
	macLocal[4] = (uint8_t)((deviceID >>  8) & 0xFF);
	macLocal[5] = (uint8_t)((deviceID      ) & 0xFF);
}

// state access calls
bool Data::getStateSpeaker() { return stateSpeaker; }
void Data::setStateSpeaker(bool state) { stateSpeaker = state; }
bool Data::getStateBeep() { return stateBeep; }
void Data::setStateBeep(bool state) { stateBeep = state; }
bool Data::getStateNetwork() { return stateNetwork; }
void Data::setStateNetwork(bool state) { stateNetwork = state; }
bool Data::getStatePressed() { return statePressed; }
void Data::setStatePressed(bool state) { statePressed = state; }
bool Data::getStateAlarm() { return stateAlarm; }
void Data::setStateAlarm(bool state) { stateAlarm = state; }
bool Data::getStateRead() { return stateRead; };
void Data::setStateRead(bool state) { stateRead = state; }
bool Data::getStateSend() { return stateSend; }
void Data::setStateSend(bool state) { stateSend = state; }
void Data::setStateDNS(uint8_t state) { stateDNS = state; }
uint8_t Data::getStateDNS() { return stateDNS; }

// access battery voltage in milivolts
uint16_t Data::getBatteryVoltage() { return batteryVoltage; }
void Data::setBatteryVoltage(uint16_t milivolts) { batteryVoltage = milivolts; }

// geiger varios dose& stats access calls
float Data::getGeigerDose() { return geigerDose; }
void Data::setGeigerDose(float dose) { geigerDose = dose; }
uint32_t Data::getGeigerCPM() { return geigerCPM; }
void Data::setGeigerCPM(uint32_t cpm) { geigerCPM = cpm; }

// inverter access functions
uint16_t Data::getInverterVoltage() { return inverterVoltage; }
void Data::setInverterVoltage(uint16_t volts) { inverterVoltage = volts; }
uint16_t Data::getInverterDuty() { return inverterDuty; }
void Data::setInverterDuty(uint16_t duty) { inverterDuty = duty; }

// network settings access functions
uint8_t* Data::getIPLocal() { return ipLocal; }
uint8_t* Data::getIPRemote() { return ipRemote; }
uint8_t* Data::getIPGateway() { return ipGateway; }
uint8_t* Data::getIPNetmask() { return ipNetmask; }
uint8_t* Data::getMACLocal() { return macLocal; }
uint8_t* Data::getMACGateway() { return macGateway; }
uint16_t Data::getNetworkPings() { return networkPings; }
uint16_t Data::getNetworkPacketsOK() { return networkPacketsOK; }
uint16_t Data::getNetworkPacketsTotal() { return networkPacketsTotal; }
uint16_t Data::getNetworkSendInterval() { return networkSendInterval; }
void Data::setNetworkSendInterval(uint16_t interval) { networkSendInterval = interval; }
uint16_t Data::getNetworkHTTPCode() { return networkHTTPCode; }
void Data::setNetworkHTTPCode(uint16_t code) { networkHTTPCode = code; }
void Data::incNetworkPings() { networkPings++; }
void Data::incNetworkPacketsOK() { networkPacketsOK++; }
void Data::incNetworkPacketsTotal() { networkPacketsTotal++; }

bool Data::initSensors() {
#ifdef USE_BME280_SENSOR
	i2c_init();
	return bme.begin();
#endif
}

// read sensors Fast: called with a very high frequency
void Data::readSensorsFast() {
	// battery (AVCC) voltage in mv
	setBatteryVoltage(aux_readIntRefVcc());
	// read inverter voltage, via 10M/47K resistive divider, connected to pin ADC2, we use battery reference voltage
	setInverterVoltage(aux_readDivVoltage(getBatteryVoltage() / 1000.0, P_TUBEVOLFB_R1, P_TUBEVOLFB_R2, P_TUBEVOLFB_PIN));
	// adjust duty cycle to regulate inverter output voltage, close to 	INVERTER_THRESHOLD
	m_inverter.adjustDutyCycle(getInverterVoltage());
	setInverterDuty(m_inverter.getDuty());
}

// read sensors Slow: called only when refresh is set, default: once per seconds
void Data::readSensorsSlow() {
#ifdef USE_BME280_SENSOR
	bme.readSensors(&temperature, &pressure, &humidity);
#endif
}
