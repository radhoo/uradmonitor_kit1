/**
 *	File:       	aux.cpp
 *	Version:  		1.0
 *	Date:       	2015
 *	License:		GPL v3
 *	Description:	general purpose code for AVR microcontrollers
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


#include "utils.h"

#include <util/delay.h>
#include <string.h>
#include <avr/pgmspace.h>


// Reset the microcontroller
void aux_softwarereset() {
	asm ("JMP 0");
	// http://forum.arduino.cc/index.php?topic=78020.0
}

// Proper reset using the watchdog: untested
void aux_wdreset() {
  wdt_enable(WDTO_15MS);
  while(1);
}

// Reads the ADC port specified by i
uint16_t aux_ADCRead(uint8_t i) {
	ADMUX = i;
	_delay_ms(1); //very important!
	ADCSRA = _BV(ADEN) | _BV(ADPS2) | _BV(ADSC);
	_delay_ms(1); //very important! allows ADEN to turn on ADC
	while (bit_is_set(ADCSRA,ADSC));
	uint16_t v = ADCL;
	v |= ADCH << 8;
	return v;
}

// return ADC port voltage computed against given VRef, with resistive divider
uint16_t aux_readDivVoltage(float vref, uint16_t divider_R1, uint16_t divider_R2, uint8_t i) {
	return (uint16_t) (aux_ADCRead(i) / 1024.0 * (divider_R1 + divider_R2) / divider_R2 * vref);
}

float aux_readVoltage(float vref, uint8_t i) {
	return aux_ADCRead(i) * vref / 1024.0;
}

// Read AVCC using internal 1.1V reference, for atmega168 and atmega328
uint16_t aux_readIntRefVcc() {
	ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1); // Read 1.1V reference against AVcc
	_delay_ms(1); //very important!
	ADCSRA = _BV(ADEN) | _BV(ADPS2) | _BV(ADSC);
	_delay_ms(1); //very important! allows ADEN to turn on ADC
	while (bit_is_set(ADCSRA,ADSC));
	uint16_t v = ADCL;
	v |= ADCH<<8;
	v = 1126400L / v; // Back-calculate AVcc in mV (1100 * 1024)
	return v;
}


/**
 * Encodes a small float (-127 .. 127) to an 16 bit integer, including sign and two decimals
 */
uint16_t float2int(float val) {
	int8_t sintp = (int8_t)val;				//signed integer part
	int8_t sdecp = (val - sintp ) * 100;	//signed decimal part
	uint8_t udecp = sdecp>0?sdecp:-1*sdecp; //removed decimal sign
	uint8_t uintp = sintp + 127;			//convert to unsigned
	uint16_t res = (udecp << 8) | (uint16_t)uintp;		//pack it together
	return res;
}

/**
 * copyBytes
 * copy specified src_bytes from src to dst at specified offset
 * this function is not safe, as dst is not checked for size. Make sure it is big enough!
 */
uint16_t copyBytes(void *dst, uint16_t dst_offset, void *src, uint8_t src_bytes) {
	if (!dst || !src_bytes) return dst_offset;

	for (int i=0; i< src_bytes;i++)
		((uint8_t *)dst)[dst_offset + i] = !src? 0 : ((uint8_t *)src)[src_bytes - i - 1]; // either 0 if empty source or most significant bytes go first
	return dst_offset + src_bytes;
}



/**
 * jsonKeyFind
 * finds a key and copies its value to the value output pointer
 */
bool jsonKeyFind(const char *response, const char *key, char *value, uint8_t size) {
	char *s1 = strstr_P(response, key);
	uint8_t len = strlen_P(key);
	if (s1 && len) {
		char *s2 = strstr(s1 + len + 3, "\"");
		if (s2) {
			strncpy(value, s1 + len + 3, MIN(s2 - s1 - len - 3, size) );
			return true;
		}
	}
	return false;
}

/**
 * hex2int
 * take a hex string and convert it to a 32bit number (max 8 hex digits)
 */
uint32_t hex2int(char *hex) {
    uint32_t val = 0;

     while (*hex) {
    	// get current character then increment
    	uint8_t byte = *hex++;
    	// transform hex character to the 4bit equivalent number, using the ascii table indexes
    	if (byte >= '0' && byte <= '9') byte = byte - '0';
    	else if (byte >= 'a' && byte <='f') byte = byte - 'a' + 10;
    	else if (byte >= 'A' && byte <='F') byte = byte - 'A' + 10;
    	// shift 4 to make space for new digit, and add the 4 bits of the new digit
    	val = (val << 4) | (byte & 0xF);
    }
    return val;
}
