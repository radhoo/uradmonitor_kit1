/**
 *	File:       	uRADMonitor.cpp
 *	Version:  		1.0.121
 *	Date:       	2014
 *	License:		GPL v3
 *	Description:	uRADMonitor main code file
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

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/wdt.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/pgmspace.h>

#include "app/UI.h"
// local headers
#include "introscreen.h"
#include "lcd/5110.h"
#include "gpio/DigitalPin.h"
#include "time/timecounter.h"
#include "time/watchdog.h"
#include "config.h"
#include "geiger/detectors.h"
#include "misc/utils.h"
#include "misc/morse.h"
#include "misc/expProtocol.h"

// ethernet
#include "net/ip_arp_udp_tcp.h"
#include "net/enc28j60.h"
#include "net/net.h"
#include "net/dhcp_client.h"
#include "net/dnslkup.h"

#define				EEPROM_ADDR_DEVID		0x0		// address when the DEVID is being stored 1024Bytes available on atmega328p	: 4B
#define				EEPROM_ADDR_SENDINT		0x4		// address to store the send interval

/************************************************************************************************************************************************/
/* Global Objects                                                       																		*/
/************************************************************************************************************************************************/
char				buffer[120]		= { 0 };		// general purpose buffer

DigitalPin			speaker(&PORTD, PD3),			// used to create digital pulse on radiation event
					backlight(&PORTD, PD4),			// used to toggle LCD light on and off
					button(&PORTC, PC3, DigitalPin::INPUT), // user button, configured as input pin
					lcdReset(&PORTB, PB0), 			// lcd rst pin
					lcdCE(&PORTD, PD7),				// lcd CE pin
					lcdDC(&PORTD, PD6), 			// lcd DC pin
					lcdDATA(&PORTD, PD5), 			// lcd data pin
					lcdCLK(&PORTC, PC1);			// lcd CLK pin
LCD_5110			lcd(&lcdReset, &lcdCE, &lcdDC, &lcdDATA, &lcdCLK, &backlight);	// handle the LCD ops for drawing content onscreen
TimeCounter			time;
Data				data(&time);
UI					ui(&lcd, &speaker, &button, &data, buffer, sizeof(buffer));// lcd, speaker, button and data, form the GUI for user interaction
Watchdog			wd;								// used to protect device from code deadlocks by issueing an autoreset when needed
Morse				morse;							// used to transmit morse audio signals for communication or general purpose beeps

uint32_t			deviceNumber			= 0;//DEV_NUMBER;	 // If null, Dynamic ID  is enabled . Add a non null value to set number manually
volatile uint32_t 	geigerPulses 			= 0;	// geiger: total number of pulses detected
bool				cmdRefresh = 0;					// if true will refresh display

/************************************************************************************************************************************************/
/* Network interface                                                    																		*/
/************************************************************************************************************************************************/
#ifdef USE_ETHERNET
#define 			ARP_REF_GWMAC 			10		// just an unique request ID returned in the ARP callback
static uint8_t 		ethBuffer[600]			= { 0 };// the big bad ethernet buffer, very memory consuming
static char 		ethParams[120]			= { 0 };// general purpose buffer, will need to hold the upload parameters, careful when using many sensors!
#endif

// simple sound functions
inline void sdelay() { _delay_ms(40); }
inline void sbeep() { speaker = 1; sdelay(); speaker = 0; sdelay(); }
inline void lbeep() { speaker = 1; sdelay(); sdelay(); sdelay(); speaker = 0; sdelay(); }

// callback function called from the timecounter object when a full minute has elapsed
void callback_timeMinute() {}

// called on elapsed second: keep code here light!
void callback_timeSecond() {
	// bigger than 0
	if (time.getTotalSec() > 0 && time.getTotalSec() >= WARMUP) {
		if (time.getTotalSec() % READ_INTERVAL == 0) {
			// compute partial CPM, using time counter 3, that keeps all seconds since last geigerPulses reset to 0
			if (time.getCount(TimeCounter::TIME_COUNTER_3) > 0) {
				data.setGeigerCPM((60 * geigerPulses) / time.getCount(TimeCounter::TIME_COUNTER_3));
				data.setGeigerDose(aux_CPM2uSVh(GEIGER_TUBE, data.getGeigerCPM()));
			}
			// issue READ command every given interval
			data.setStateRead(Data::ENABLED);
		}
		// issue SEND command every given interval
		// if no Ethernet, reset watchdog every time, but if we have Internet, reset it only in the HTTP Client callback
		if (data.getNetworkSendInterval() > 0 && time.getTotalSec() % data.getNetworkSendInterval() == 0) {
			data.setGeigerCPM((60 * geigerPulses) / data.getNetworkSendInterval());
			data.setGeigerDose(aux_CPM2uSVh(GEIGER_TUBE, data.getGeigerCPM()));
			geigerPulses = 0;
			// reinit counter 3, to compute the partical CPM
			time.startCount(TimeCounter::TIME_COUNTER_3);

			if (!data.getStateNetwork())
				wd.wdt_my_reset();
			else {
				// re-set HTTP code
				data.setNetworkHTTPCode(0);
				// ask to send data online
				data.setStateSend(Data::ENABLED);
			}
		}
	}

	// alarm condition
	if (data.getStateAlarm()) {
		if (data.getStateSpeaker()) speaker.toggle(); // // dangerous code to do here
		backlight.toggle();
	}

	// enabled GUI display, that is refresh every second
	cmdRefresh = Data::ENABLED;
}

// watchdog overflow interrupt, set to 1sec
ISR (WDT_vect) { wd.timerEvent(); }

// timer0 overflow interrupt   event to be executed every  2.048ms here when on 8MHz, and every 1.024ms for 16MHz */
ISR (TIMER0_OVF_vect) { time.TimerEvent(); }

// int0 interrupt handler, triggered each time a rising edge is detected on INT0, which on a ATmega8 is PD2
// we have a top limit of 2^32-1 pulses. We don't go over it.
ISR (INT0_vect) {
	geigerPulses++; // count this pulse
	data.setStateBeep(Data::ENABLED);
}

#ifdef USE_ETHERNET
// Callback function for client HTTP connection result
// eg. server reply: {"success":"ok"}
// {"beep":""}
// {"alarm":""}
// {"reset":""}
// {"morse":"msg"}
// {"setid":"82000090"}
// {"sendint":"60"}
void callback_browser(uint16_t webstatuscode, uint16_t datapos, uint16_t len) {
	// 200 means success in connecting to server script: watchdog will NOT reset
	data.setNetworkHTTPCode(webstatuscode);
	// if the server replied OK, we reset the watchdog to prevent auto-reset. This is currently the only point in code where we do this.
	if (webstatuscode == 200) {
		data.incNetworkPacketsOK();
		wd.wdt_my_reset();
		// check reply
		char value[10] = {0}; // this has to be inited, or when parsing we could have extra junk screwing results
		char *serverAnswer = (char *)&(ethBuffer[datapos]);
		if (strstr_P(serverAnswer,PSTR("beep")))
			data.setStateBeep(Data::ENABLED);
		else if (strstr_P(serverAnswer,PSTR("alarm")))
			data.setStateAlarm(Data::ENABLED);
		else if (jsonKeyFind(serverAnswer, PSTR("setid"), value, 10)) {
			eeprom_busy_wait();
			eeprom_write_dword((uint32_t *) EEPROM_ADDR_DEVID, hex2int(value));
			eeprom_busy_wait();
			aux_softwarereset();
		} else if (jsonKeyFind(serverAnswer, PSTR("sendint"), value, 10)) {
			data.setNetworkSendInterval(atoi(value));
			eeprom_busy_wait();
			eeprom_write_word((uint16_t *) EEPROM_ADDR_SENDINT, data.getNetworkSendInterval());
			// set WDT reset interval against sendInterval
			wd.wdt_setRebootSeconds(data.getNetworkSendInterval() + WARMUP + 180);
		} else if (jsonKeyFind(serverAnswer, PSTR("morse"), value, 10)) {
			morse.encode(value);
		} else if (strstr_P(serverAnswer,PSTR("reset"))) {
			aux_softwarereset();
		}
	}
}

// Callback function for ARP requests
void callback_arp(uint8_t *ip __attribute__((unused)), uint8_t reference_number, uint8_t *mac) {
	if (reference_number == ARP_REF_GWMAC)
		memcpy(data.getMACGateway(), mac, 6);
}

// Callback function for incoming pings
void callback_ping(uint8_t *srcip) {
	data.incNetworkPings();
}
#endif

// This function is called upon a HARDWARE RESET, before main() :
void early_run(void) __attribute__((naked)) __attribute__((section(".init3")));

// disable WDT on reboot, to avoid any deadlocks
void early_run(void) {
	Watchdog::wdt_first_disable(); // call as static: object might not be ready yet
}

/************************************************************************************************************************************************/
/* Main entry point                                                    																			*/
/************************************************************************************************************************************************/
 int main(void) {
	// 1.setup watchdog, EEPROM params/Dev ID/Send interval
	wd.wdt_init(WATCHDOG_INTERVAL); // 10min reboot
	eeprom_busy_wait();
	deviceNumber = eeprom_read_dword((uint32_t *) EEPROM_ADDR_DEVID);
	data.setNetworkSendInterval(eeprom_read_word((uint16_t *) EEPROM_ADDR_SENDINT));
	if (data.getNetworkSendInterval() == 0xFFFF || data.getNetworkSendInterval() < 5 || data.getNetworkSendInterval() > 3600)
		data.setNetworkSendInterval(60); // default to 1 min send interval

	// set WDT reset interval against sendInterval
	wd.wdt_setRebootSeconds(data.getNetworkSendInterval() + WARMUP + 180);
	// configure device ID and MAC
	data.setDeviceID_Mac(((uint32_t) DEV_CLASS << 24) | ((uint32_t) deviceNumber & 0x00FFFFFF));

	// 2.CONFIGURE INTERRUPT INT0  to count pulses from Geiger Counter, connected on PIN PD2
	EICRA |= _BV(ISC00) | _BV(ISC01);// Configure INT0 to trigger on RISING EDGE - Instead of MCUCR, you need to use EICRA. mega8:MCUCR , mega328p: EICR
	EIMSK |= _BV(INT0); // Configure INT0 to fire interrupts - Instead of GICR, you need to use EIMSK mega8:GICR , mega328p: EIMSK

	// 3.CREATE Timer T0 to count seconds
	time.init(callback_timeSecond, callback_timeMinute);
	// start couting seconds for the partial CPM
	time.startCount(TimeCounter::TIME_COUNTER_3);

	// 4.CREATE Timer T1 PWM to drive inverter for regulated Geiger tube voltage
	data.getInverter().initPWM();

	// 5. init sensors
	morse.init(sbeep, lbeep, sdelay);
	data.initSensors();


	// 6. lcd init goes here
	lcd.init();
	lcd.clear(); lcd.printPictureOnLCD(introScreen); _delay_ms(1000); lcd.clear();
	lcd.send(PSTR("Init ETHERNET\n--------------Connect cable\nand wait15s or press button\nto go offline."));
	lcd.setBacklight(Data::ENABLED);

#ifdef USE_ETHERNET
	// 7.Init ETHERNET
	uint16_t plen = 0, dat_p = 0;
	enc28j60Init(data.getMACLocal()); //initialize enc28j60
	enc28j60PhyWrite(PHLCON, 0x476); // 0x476 is PHLCON LEDA (green)=links status, LEDB (yellow) =receive/transmit (enc28j60 datasheet, page 11)
	// DHCP handling. Get the initial IP. At this point we'll know if the network is ok: cable connected, router ok
	uint8_t rval = 0;
	init_mac(data.getMACLocal());
	//assume net is on unless we cancel
	data.setStateNetwork(Data::ENABLED);
	// DHCP lease loop
	while (rval == 0) {
		plen = enc28j60PacketReceive(sizeof(ethBuffer), ethBuffer);
		sprintf_P(ethParams, PSTR("uRADMonitor-%02X"), data.getMACLocal()[5]);
		rval = packetloop_dhcp_initial_ip_assignment(ethBuffer, plen, data.getMACLocal()[5], ethParams); 	// blocking, can take up to 11sec in a single cycle
		if (!button) {
			data.setStateNetwork(Data::DISABLED); break; // user canceled DHCP lease loop and wants offline mode
		}
	}
	// continue network init
	if (data.getStateNetwork()) {
		// we have an IP, read it
		dhcp_get_my_ip(data.getIPLocal(), data.getIPNetmask(), data.getIPGateway());
		client_ifconfig(data.getIPLocal(), data.getIPNetmask());
		if (data.getIPGateway()[0] == 0)
			data.setStateNetwork(Data::DISABLED);
		else {
			// we have a gateway, find the mac address of the gateway (e.g your dsl router).
			get_mac_with_arp(data.getIPGateway(), ARP_REF_GWMAC, &callback_arp);
			while (get_mac_with_arp_wait()) {
				// to process the ARP reply we must call the packetloop
				plen = enc28j60PacketReceive(sizeof(ethBuffer), ethBuffer);
				packetloop_arp_icmp_tcp(ethBuffer, plen);
				// this takes almost 0sec, in a single cycle
			}
			// count the pings
			register_ping_rec_callback(&callback_ping);
		}
	} else {
		// if network init is not OK, simply power down the enc28j60 module
		powerDown();
	}
#endif
	// we don't check if failed (data.getStateNetwork() = (rval != 0), we assume we're online, and will fail later and reboot via watchdog
	// the only offline scenario is considered when the user specifically pressed the button in the DHCP resolve wait screen
	// if net is on, speaker goes off by default
	data.setStateSpeaker(!data.getStateNetwork());

	// turn LCD light on
	lcd.setBacklight(Data::ENABLED);

	// 8.Main program loop
	while (1) {

		// read sensors, listen to button presses and refresh screen
		ui.loop(&cmdRefresh);

#ifdef USE_ETHERNET

		if (data.getStateNetwork()) {
			// get packets, handle ping and wait for a tcp packet
			plen = enc28j60PacketReceive(sizeof(ethBuffer), ethBuffer);
			// return the position  of the tcp data if there is tcp data part
			dat_p = packetloop_arp_icmp_tcp(ethBuffer, plen);

			if (plen == 0) {
				// First, DNS Request
				if (data.getStateDNS() != Data::DNS_RESOLVED) {
					// before anything else, handle DNS requests
					if (data.getStateDNS() == Data::DNS_IDLE) {
						// only for dnslkup_request we have to check if the link is up.
						if (!enc28j60linkup()) continue;
						data.setStateDNS(Data::DNS_RESOLVING);
						dnslkup_request(ethBuffer, URADMONITOR_SERVER, data.getMACGateway());
						continue;
					}
					if (data.getStateDNS() == Data::DNS_RESOLVING && dnslkup_haveanswer()) {
						data.setStateDNS(Data::DNS_RESOLVED);
						dnslkup_get_ip(data.getIPRemote());
					}
					// retry every half minute if dns-lookup failed:
					if (time.getSec() == 60) data.setStateDNS(Data::DNS_IDLE);
					continue;
				}
				// Next, handle CLIENT Data in case we need to send
				if (data.getStateSend()) {
					// uploading to uRADMonitor network requires authentification with a valid account
					// to get a valid account, register on www.uradmonitor.com/dashboard , then enter your details in config.h
					// do not modify the code below
					sprintf_P(buffer, PSTR("X-User-id:%s\r\nX-User-hash:%s\r\nX-Device-id:%08lX\r\n"), USER_ID, USER_KEY, data.getDeviceID());

					// when sending data, make sure you include the timestamp with each packet, or the server will reject your data
					// see expProtocol.h for the possible sensors supported by the server
#ifdef USE_BME280_SENSOR
					sprintf_P(ethParams, PSTR(ID_TIME_SECONDS"/%lu/"ID_VERSION_HW"/%u/"ID_VERSION_SW"/%u/"
							ID_SBM20_CPM"/%lu/"ID_INVERTERVOLTAGE_VOLTS"/%u/"ID_INVERTERDUTY_PM"/%u/"
							ID_TEMPERATURE_CELSIUS"/%.2f/"ID_PRESSURE_PASCALS"/%lu/"ID_HUMIDITY_RH"/%u"),
							time.getTotalSec(), (uint8_t)VER_HW, (uint8_t)VER_SW,
							data.getGeigerCPM(),data.getInverterVoltage(), data.getInverterDuty(),
							data.getTemperature(), data.getPressure(), data.getHumidity()
						);
#else
					sprintf_P(ethParams, PSTR(ID_TIME_SECONDS"/%lu/"ID_VERSION_HW"/%u/"ID_VERSION_SW"/%u/"
							ID_SBM20_CPM"/%lu/"ID_INVERTERVOLTAGE_VOLTS"/%u/"ID_INVERTERDUTY_PM"/%u"),
							time.getTotalSec(), (uint8_t)VER_HW, (uint8_t)VER_SW,
							data.getGeigerCPM(),data.getInverterVoltage(), data.getInverterDuty()
						);

#endif
					// send data as HTTP POST
					client_http_post(PSTR(URADMONITOR_SCRIPT), ethParams, PSTR(URADMONITOR_SERVER), buffer, NULL, &callback_browser, data.getIPRemote(), data.getMACGateway() );
					// count all outbound packets
					data.incNetworkPacketsTotal();
					// mark sending job done
					data.setStateSend(Data::DISABLED); // stop sending data until next minute
				}
				continue;
			} //plen check

			// check for incomming messages not processed
			// as part of packetloop_arp_icmp_tcp, e.g udp messages
			if (dat_p == 0) { //plen!=0
				udp_client_check_for_dns_answer(ethBuffer, plen);
				continue;
			}

			// server: serve JSON
			if (strncmp("GET /j", (char *)&(ethBuffer[dat_p]), 6) == 0) {
				dat_p = fill_tcp_data_p(ethBuffer,0,PSTR("HTTP/1.0 200 OK\r\nContent-Type: text/html\r\nPragma: no-cache\r\n\r\n"));
				// create webserver page: pay attention to buffers used: buffer and ethBuffer as small in size!
				sprintf_P(buffer, PSTR("{\"data\":{ \"id\":\"%08lX\","), data.getDeviceID());
				dat_p = fill_tcp_data_len(ethBuffer,dat_p, (uint8_t *)buffer, strlen(buffer));
				sprintf_P(buffer, PSTR("\"type\":\"%X\",\"detector\":\"%s\",\"cpm\":%lu,"),DEV_CLASS, aux_detectorName(GEIGER_TUBE), data.getGeigerCPM());
				dat_p = fill_tcp_data_len(ethBuffer,dat_p, (uint8_t *)buffer, strlen(buffer));
#ifdef USE_BME280_SENSOR
				sprintf_P(buffer, PSTR("\"temperature\":%.2f,\"pressure\":%lu,\"humidity\":%.2f,"), data.getTemperature(), data.getPressure(), data.getHumidity());
				dat_p = fill_tcp_data_len(ethBuffer,dat_p, (uint8_t *)buffer, strlen(buffer));
#endif
				sprintf_P(buffer, PSTR("\"uptime\": %lu}}"), time.getTotalSec());
				dat_p = fill_tcp_data_len(ethBuffer,dat_p, (uint8_t *)buffer, strlen(buffer));
			}
			// server: serve mini webpage
			else if (strncmp("GET ",(char *)&(ethBuffer[dat_p]),4)!=0 || strncmp("/ ",(char *)&(ethBuffer[dat_p+4]),2)==0) {
				dat_p = fill_tcp_data_p(ethBuffer,0,PSTR("HTTP/1.0 200 OK\r\nContent-Type: text/html\r\nPragma: no-cache\r\n\r\n"));
				// create webserver page: pay attention to buffers used: buffer and ethBuffer as small in size!
				sprintf_P(buffer, PSTR("<b>uRADMonitor %08lX</b><br>"), data.getDeviceID());
				dat_p = fill_tcp_data_len(ethBuffer,dat_p, (uint8_t *)buffer, strlen(buffer));
				sprintf_P(buffer, PSTR("type:%X hw:%u sw:%u %s<hr>"), DEV_CLASS, VER_HW, VER_SW, aux_detectorName(GEIGER_TUBE));
				dat_p = fill_tcp_data_len(ethBuffer,dat_p, (uint8_t *)buffer, strlen(buffer));
				sprintf_P(buffer, PSTR("radiation:%luCPM<br>"), data.getGeigerCPM());
				dat_p = fill_tcp_data_len(ethBuffer,dat_p, (uint8_t *)buffer, strlen(buffer));
#ifdef USE_BME280_SENSOR
				if (time.getTotalSec() < WARMUP) {
					sprintf_P(buffer, PSTR("Ready in %ds<br><br>"), WARMUP - time.getTotalSec());
					dat_p = fill_tcp_data_len(ethBuffer,dat_p, (uint8_t *)buffer, strlen(buffer));
				} else {
					sprintf_P(buffer, PSTR("temperature:%.2fC<br>pressure:%luPa<br>"), data.getTemperature(), data.getPressure());
					dat_p = fill_tcp_data_len(ethBuffer,dat_p, (uint8_t *)buffer, strlen(buffer));
					sprintf_P(buffer, PSTR("humidty:%.2fRH<br>"), data.getHumidity());
				}
#endif
				sprintf_P(buffer, PSTR("voltage:%uV<br>duty:%u%%<br>frequency:%.2fkHz<br>"),data.getInverterVoltage(), data.getInverterDuty() /10, INVERTER_FREQUENCY / 1000.0);
				dat_p = fill_tcp_data_len(ethBuffer,dat_p, (uint8_t *)buffer, strlen(buffer));
				sprintf_P(buffer, PSTR("uptime:%lus wdt:%us/%us<br><br>"), time.getTotalSec(), wd.wdt_get_counter(), wd.wdt_getRebootSeconds());
				dat_p = fill_tcp_data_len(ethBuffer,dat_p, (uint8_t *)buffer, strlen(buffer));
				sprintf_P(buffer,PSTR("ip:%u.%u.%u.%u<br>server:%u.%u.%u.%u<br>"),
						data.getIPLocal()[0],data.getIPLocal()[1],data.getIPLocal()[2],data.getIPLocal()[3],
						data.getIPRemote()[0],data.getIPRemote()[1],data.getIPRemote()[2],data.getIPRemote()[3]);
				dat_p = fill_tcp_data_len(ethBuffer,dat_p, (uint8_t *)buffer, strlen(buffer));
				sprintf_P(buffer,PSTR("interval:%u http:%u %u/%u"), data.getNetworkSendInterval(), data.getNetworkHTTPCode(), data.getNetworkPacketsOK(), data.getNetworkPacketsTotal());
				dat_p = fill_tcp_data_len(ethBuffer,dat_p, (uint8_t *)buffer, strlen(buffer));
				sprintf_P(buffer,PSTR("<hr><a href=\"/j\">JSON</a> [%03u]"), dat_p + 31);
				dat_p = fill_tcp_data_len(ethBuffer,dat_p, (uint8_t *)buffer, strlen(buffer));
				// tcp port 80 end
			} else
				dat_p=fill_tcp_data_p(ethBuffer,0,PSTR("HTTP/1.0 401 Unauthorized\r\nContent-Type: text/html\r\n\r\n<h1>401 Unauthorized</h1>"));
			www_server_reply(ethBuffer, dat_p); // send web page data to client
		}
#endif // USE_ETHERNET
	} // if network data.getStateNetwork() is ok

	return (0);
}
