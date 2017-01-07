/**
 *	File:       	bmp180.cpp
 *	Version:  		1.0
 *	Date:       	2012
 *	License:		GPL v3
 *	Description:	AVR I2C Driver for Bosch BMP180, a MEMS sensor for Temperature Pressure
 *	Project:		uRADMonitor KIT1, a hackable digital sensor monitoring tool with network interface
 *
 *	Copyright 2012 by Radu Motisan, radu.motisan@gmail.com
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


#pragma  once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <avr/io.h>
#include <util/delay.h>
#include "../config.h"
#include "../i2c/i2c.h"

#define BMP180_ADDR (0x77<<1) //0x77 default I2C address

//registers
#define BMP180_REGAC1 0xAA
#define BMP180_REGAC2 0xAC
#define BMP180_REGAC3 0xAE
#define BMP180_REGAC4 0xB0
#define BMP180_REGAC5 0xB2
#define BMP180_REGAC6 0xB4
#define BMP180_REGB1 0xB6
#define BMP180_REGB2 0xB8
#define BMP180_REGMB 0xBA
#define BMP180_REGMC 0xBC
#define BMP180_REGMD 0xBE
#define BMP180_REGCONTROL 0xF4 //control
#define BMP180_REGCONTROLOUTPUT 0xF6 //output 0xF6=MSB, 0xF7=LSB, 0xF8=XLSB
#define BMP180_REGREADTEMPERATURE 0x2E //read temperature
#define BMP180_REGREADPRESSURE 0x34 //read pressure

//modes
#define BMP180_MODEULTRALOWPOWER 0 //oversampling=0, internalsamples=1, maxconvtimepressure=4.5ms, avgcurrent=3uA, RMSnoise_hPA=0.06, RMSnoise_m=0.5
#define BMP180_MODESTANDARD 1 //oversampling=1, internalsamples=2, maxconvtimepressure=7.5ms, avgcurrent=5uA, RMSnoise_hPA=0.05, RMSnoise_m=0.4
#define BMP180_MODEHIGHRES 2 //oversampling=2, internalsamples=4, maxconvtimepressure=13.5ms, avgcurrent=7uA, RMSnoise_hPA=0.04, RMSnoise_m=0.3
#define BMP180_MODEULTRAHIGHRES 3 //oversampling=3, internalsamples=8, maxconvtimepressure=25.5ms, avgcurrent=12uA, RMSnoise_hPA=0.03, RMSnoise_m=0.25

//setup parameters
#define BMP180_MODE BMP180_MODEULTRAHIGHRES //define a mode
#define BMP180_UNITPAOFFSET 0 //define a unit offset (pa)
#define BMP180_UNITMOFFSET 0 //define a unit offset (m)

class BMP180 {

	int16_t ac1, ac2, ac3, b1, b2, mb, mc, md;
	uint16_t ac4, ac5, ac6;

	void getcalibration();
	int32_t readRawTemperature(void);
	uint32_t readRawPressure(int32_t rawtemperature );
	float convertRawTemperature(int32_t rawtemperature);
	uint32_t convertRawPressure(uint32_t rawpressure) ;
	float convertAltitude(uint32_t pressure);
public:
	void	init();
	float	readTemperature();
	uint32_t readPressure();
	float	readAltitude();
	void readAll(float *temperature, uint32_t *pressure, float *altitude) ;
};
