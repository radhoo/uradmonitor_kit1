/**
 *	File:       	aux.h
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

#pragma once

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

#define MIN(x,y) (x<y)?x:y

// Reset the microcontroller
void aux_softwarereset();

// Proper reset using the watchdog: untested
void aux_wdreset() ;

// Reads the ADC port specified by i
uint16_t aux_ADCRead(uint8_t i);

// return ADC port voltage computed against given VRef, with resistive divider
uint16_t aux_readDivVoltage(float vref, uint16_t divider_R1, uint16_t divider_R2, uint8_t i);

float aux_readVoltage(float vref, uint8_t i);

// Read AVCC using internal 1.1V reference, for atmega168 and atmega328
uint16_t aux_readIntRefVcc();

/**
 * Encodes a small float (-127 .. 127) to an 16 bit integer, including sign and two decimals
 */
uint16_t float2int(float val);

/**
 * copyBytes
 * copy specified src_bytes from src to dst at specified offset
 * this function is not safe, as dst is not checked for size. Make sure it is big enough!
 */
uint16_t copyBytes(void *dst, uint16_t dst_offset, void *src, uint8_t src_bytes);

/**
 * jsonKeyFind
 * finds a key and copies its value to the value output pointer
 */
bool jsonKeyFind(const char *response, const char *key, char *value, uint8_t size);

/**
 * hex2int
 * take a hex string and convert it to a 32bit number (max 8 hex digits)
 */
uint32_t hex2int(char *hex);
