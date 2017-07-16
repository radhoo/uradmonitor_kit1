/**
 *	File:       	bmp180.cpp
 *	Version:  		1.0
 *	Date:       	2012
 *	License:		GPL v3
 *	Description:	AVR I2C Driver for Bosch BMP180, a MEMS sensor for Temperature and Pressure
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


#include "bmp180.h"


// read the calibration data from sensor memory
void BMP180::getcalibration() {
	uint8_t buff[2];
	memset(buff, 0, sizeof(buff));
	//ac1 = read16(BMP180_CAL_AC1);
	readmem(BMP180_ADDR, BMP180_REGAC1, buff, 2);
	ac1 = ((int)buff[0] <<8 | ((int)buff[1]));
	readmem(BMP180_ADDR, BMP180_REGAC2, buff, 2);
	ac2 = ((int)buff[0] <<8 | ((int)buff[1]));
	readmem(BMP180_ADDR, BMP180_REGAC3, buff, 2);
	ac3 = ((int)buff[0] <<8 | ((int)buff[1]));
	readmem(BMP180_ADDR, BMP180_REGAC4, buff, 2);
	ac4 = ((unsigned int)buff[0] <<8 | ((unsigned int)buff[1]));
	readmem(BMP180_ADDR, BMP180_REGAC5, buff, 2);
	ac5 = ((unsigned int)buff[0] <<8 | ((unsigned int)buff[1]));
	readmem(BMP180_ADDR, BMP180_REGAC6, buff, 2);
	ac6 = ((unsigned int)buff[0] <<8 | ((unsigned int)buff[1]));
	readmem(BMP180_ADDR, BMP180_REGB1, buff, 2);
	b1 = ((int)buff[0] <<8 | ((int)buff[1]));
	readmem(BMP180_ADDR, BMP180_REGB2, buff, 2);
	b2 = ((int)buff[0] <<8 | ((int)buff[1]));
	readmem(BMP180_ADDR, BMP180_REGMB, buff, 2);
	mb = ((int)buff[0] <<8 | ((int)buff[1]));
	readmem(BMP180_ADDR, BMP180_REGMC, buff, 2);
	mc = ((int)buff[0] <<8 | ((int)buff[1]));
	readmem(BMP180_ADDR, BMP180_REGMD, buff, 2);
	md = ((int)buff[0] <<8 | ((int)buff[1]));
}


int32_t BMP180::readRawTemperature(void) {
	uint8_t buff[2];
	memset(buff, 0, sizeof(buff));
	//read raw temperature
	writemem(BMP180_ADDR, BMP180_REGCONTROL, BMP180_REGREADTEMPERATURE);
	_delay_ms(5); // min. 4.5ms read Temp delay
	readmem(BMP180_ADDR, BMP180_REGCONTROLOUTPUT, buff, 2);

	int32_t ut,x1,x2;
	//uncompensated temperature value
	ut = ((int32_t)buff[0] << 8 | ((int32_t)buff[1]));
	//compensate
	x1 = ((int32_t)ut - ac6) * ac5 >> 15;
	x2 = ((int32_t)mc << 11) / (x1 + md);
	int32_t comp_ut = x1 + x2;

	return comp_ut;
}

uint32_t BMP180::readRawPressure(int32_t rawtemperature ) {
	uint8_t buff[3];
	memset(buff, 0, sizeof(buff));
	writemem(BMP180_ADDR, BMP180_REGCONTROL, BMP180_REGREADPRESSURE + (BMP180_MODE << 6));
	_delay_ms(2 + (3<<BMP180_MODE));
	readmem(BMP180_ADDR, BMP180_REGCONTROLOUTPUT, buff, 3);

	int32_t up,x1,x2,x3,b3,b6,p;
	uint32_t b4,b7;
	// uncompensated pressure value
	up = ((((int32_t)buff[0] <<16) | ((int32_t)buff[1] <<8) | ((int32_t)buff[2])) >> (8-BMP180_MODE));

	//calculate raw pressure, compensated
	b6 = rawtemperature - 4000;
	x1 = (b2* (b6 * b6) >> 12) >> 11;
	x2 = (ac2 * b6) >> 11;
	x3 = x1 + x2;
	b3 = (((((int32_t)ac1) * 4 + x3) << BMP180_MODE) + 2) >> 2;
	x1 = (ac3 * b6) >> 13;
	x2 = (b1 * ((b6 * b6) >> 12)) >> 16;
	x3 = ((x1 + x2) + 2) >> 2;
	b4 = (ac4 * (uint32_t)(x3 + 32768)) >> 15;
	b7 = ((uint32_t)up - b3) * (50000 >> BMP180_MODE);
	p = b7 < 0x80000000 ? (b7 << 1) / b4 : (b7 / b4) << 1;
	x1 = (p >> 8) * (p >> 8);
	x1 = (x1 * 3038) >> 16;
	x2 = (-7357 * p) >> 16;
	return p + ((x1 + x2 + 3791) >> 4);
}

void  BMP180::init() {
	// read calibration data
	getcalibration();
}

float BMP180::convertRawTemperature(int32_t rawtemperature) {
	return ((rawtemperature + 8)>>4) / 10.0;
}

uint32_t BMP180::convertRawPressure(uint32_t rawpressure) {
	return rawpressure + BMP180_UNITPAOFFSET;
}

// we assume sea level pressure is 101325 Pa
float BMP180::convertAltitude(uint32_t pressure) {
	return ((1 - pow(pressure/(double)101325, 0.1903 )) / 0.0000225577) + BMP180_UNITMOFFSET;
}

// -------------------------------------------------------------------------- //
float BMP180::readTemperature() {
	return convertRawTemperature(readRawTemperature());
}

uint32_t BMP180::readPressure() {
	return convertRawPressure(readRawPressure(readRawTemperature()));
}

float BMP180::readAltitude() {
	return convertAltitude(readPressure());
}

void BMP180::readAll(float *temperature, uint32_t *pressure, float *altitude) {
	int32_t rawtemperature = readRawTemperature();
	int32_t rawpressure = readRawPressure(rawtemperature);
	//
	*temperature = convertRawTemperature(rawtemperature);
	*pressure = convertRawPressure(rawpressure);
	*altitude = convertAltitude(*pressure);
}
