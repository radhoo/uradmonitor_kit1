/**
 *	File:       	data.h
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
#pragma once

#include "../config.h"
#include "../misc/utils.h"
#include "../time/timecounter.h"
#include "../geiger/inverter.h"
#include "ringbuf.h"


// By default, the KIT1 has only a Geiger counter tube
// You can add additional sensors via the extension port, and handle them here, see the BME280 example
#ifdef USE_BME280_SENSOR
#include "../i2c/i2c.h"
#include "../sensors/bme280.h"

#endif


class Data {
	uint32_t 			deviceID;						// KIT1 unique identifier in the uRADMonitor network
	TimeCounter			*m_time;						// clock
	INVERTER			m_inverter;						// handles the PWM on Timer1 to drive the Geiger tube inverter
	bool				stateSpeaker : 1,					// if true the speaker is functional, if false the speaker is muted
						stateNetwork : 1,					// if true we have network up and running, if false we're offline
						statePressed : 1,					// true if button is pressed
						stateAlarm : 1,						// if true will trigger alarm
						stateBeep : 1,						// if true will trigger a beep
						stateRead : 1,						// if true will read sensors
						stateSend : 1;						// if true will trigger sending data online
	uint8_t				ipLocal[4],						// network local IP4
						ipRemote[4],					// network server IP4
						ipGateway[4],					// network gateway IP4 in current LAN
						ipNetmask[4],					// network netmask IP4in current LAN
						macLocal[6],					// network local MAC address
						macGateway[6],					// network gateway MAC address
						stateDNS;

	uint16_t 			batteryVoltage,					// voltage on battery in millivolts
						inverterVoltage,				// high voltage produced by the inverter in volts
						inverterDuty,					// inverter driver signal duty cycle in percents per mille
						networkPings,					// pings received
						networkPacketsOK,				// packets sent ok
						networkPacketsTotal,				// packets failed to send
						networkSendInterval,			// interval in seconds to send
						networkHTTPCode;				// server returned HTTP code
	uint16_t			geigerCPMHigh;					// observed max CPM
	uint16_t			geigerCPMLow;					// observed min CPM
	uint16_t			geigerIntervalCount;			// 5-second intervals which have elapsed
	uint32_t			geigerCPM;						// radiation dose as CPM
	RingBuf				geigerCPMHistory;				// recent history of geigerCPM

	float				geigerDose;						// radiation dose approximated to equivalent dose based on geiger tube factor

	// Add any new sensors here
#ifdef USE_BME280_SENSOR
	BME280 				bme;
	float				temperature;
	uint32_t			pressure;
	uint8_t				humidity;
public:
	float				getTemperature() { return temperature; }
	uint32_t			getPressure() { return pressure; }
	uint8_t				getHumidity() { return humidity; }
#endif

public:
	enum {
		DISABLED = 0,
		ENABLED = 1
	};
	enum {
		DNS_IDLE,
		DNS_RESOLVING,
		DNS_RESOLVED
	};
	Data(TimeCounter *time);

	// return objects
	TimeCounter* getTimeCounter();
	INVERTER getInverter();

	// Device ID access calls
	uint32_t getDeviceID();
	void setDeviceID_Mac(uint32_t id);
	// state access calls
	bool getStateSpeaker();
	void setStateSpeaker(bool state);
	bool getStateBeep();
	void setStateBeep(bool state);
	bool getStateNetwork();
	void setStateNetwork(bool state);
	bool getStatePressed();
	void setStatePressed(bool state);
	bool getStateAlarm();
	void setStateAlarm(bool state);
	bool getStateRead();
	void setStateRead(bool state);
	bool getStateSend();
	void setStateSend(bool state);
	void setStateDNS(uint8_t state);
	uint8_t getStateDNS();

	// access battery voltage in millivolts
	uint16_t getBatteryVoltage();
	void setBatteryVoltage(uint16_t millivolts);

	// geiger varios dose& stats access calls
	float getGeigerDose();
	void setGeigerDose(float dose);
	uint32_t getGeigerCPM();
	void setGeigerCPM(uint32_t cpm);
	uint16_t getGeigerCPMHigh();
	uint16_t getGeigerCPMLow();
	uint16_t getGeigerCPMRecentAverage();
	uint16_t getGeigerIntervalCount();
	const RingBuf *getHistory();

	// inverter access functions
	uint16_t getInverterVoltage();
	void setInverterVoltage(uint16_t volts);
	uint16_t getInverterDuty();
	void setInverterDuty(uint16_t duty);

	// network settings access functions
	uint8_t* getIPLocal();
	uint8_t* getIPRemote();
	uint8_t* getIPGateway();
	uint8_t* getIPNetmask();
	uint8_t* getMACLocal();
	uint8_t* getMACGateway();
	uint16_t getNetworkPings();
	uint16_t getNetworkPacketsOK();
	uint16_t getNetworkPacketsTotal();
	uint16_t getNetworkSendInterval();
	void setNetworkSendInterval(uint16_t interval);
	uint16_t getNetworkHTTPCode();
	void setNetworkHTTPCode(uint16_t code);
	void incNetworkPings();
	void incNetworkPacketsOK();
	void incNetworkPacketsTotal();

	bool initSensors();

	// read sensors Fast: called with a very high frequency
	void readSensorsFast();

	// read sensors Slow: called only when refresh is set, default: once per seconds
	void readSensorsSlow();
};
