/*
** BME280 Sensor Library for AVR Microcontrollers
** Version: 	0.1.0
** Date: 		September, 2015
** License:		LGPL v3
** Copyright: 	(C) 2009 - 2012 Radu Motisan, radu.motisan@gmail.com
** Description:	The BME280 from Bosch is a temperature and pressure sensor with a I2C interface
**
** www.pocketmagic.net
**
** This file is a part of "Portable Environmental Monitor" open source project presented on
** https://hackaday.io/project/4977-portable-environmental-monitor
**
** This project is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published
** by the Free Software Foundation; either version 3 of the License,
** or (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#pragma  once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <avr/io.h>
#include "../i2c/i2c.h"

// Connecting SDO to GND results in slave
// address 1110110 (0x76); connection it to VDDIO results in slave address 1110111 (0x77), which
// is the same as BMP280’s I²C address
#define BME280_ADDR (0x76<<1) //0x77 default I2C address



class BME280 {
    enum
    {
      BME280_REGISTER_T1              = 0x88,
      BME280_REGISTER_T2              = 0x8A,
      BME280_REGISTER_T3              = 0x8C,

      BME280_REGISTER_P1              = 0x8E,
      BME280_REGISTER_P2              = 0x90,
      BME280_REGISTER_P3              = 0x92,
      BME280_REGISTER_P4              = 0x94,
      BME280_REGISTER_P5              = 0x96,
      BME280_REGISTER_P6              = 0x98,
      BME280_REGISTER_P7              = 0x9A,
      BME280_REGISTER_P8              = 0x9C,
      BME280_REGISTER_P9              = 0x9E,

      BME280_REGISTER_H1              = 0xA1,
      BME280_REGISTER_H2              = 0xE1,
      BME280_REGISTER_H3              = 0xE3,
      BME280_REGISTER_H4              = 0xE4,
      BME280_REGISTER_H5              = 0xE5,
      BME280_REGISTER_H6              = 0xE7,

      BME280_REGISTER_CHIPID             = 0xD0,
      BME280_REGISTER_VERSION            = 0xD1,
      BME280_REGISTER_SOFTRESET          = 0xE0,

      BME280_REGISTER_CAL26              = 0xE1,  // R calibration stored in 0xE1-0xF0

      BME280_REGISTER_CONTROLHUMID       = 0xF2,
      BME280_REGISTER_CONTROL            = 0xF4,
      BME280_REGISTER_CONFIG             = 0xF5,
      BME280_REGISTER_PRESSUREDATA       = 0xF7,
      BME280_REGISTER_TEMPDATA           = 0xFA,
      BME280_REGISTER_HUMIDDATA          = 0xFD,
    };

public:

	bool  	begin(uint8_t addr = BME280_ADDR);
	void 	readSensors(double *temperature, uint32_t *pressure, double *humidity);
private:
	uint16_t reg_T1;
	int16_t  reg_T2;
	int16_t  reg_T3;

	uint16_t reg_P1;
	int16_t  reg_P2;
	int16_t  reg_P3;
	int16_t  reg_P4;
	int16_t  reg_P5;
	int16_t  reg_P6;
	int16_t  reg_P7;
	int16_t  reg_P8;
	int16_t  reg_P9;

	uint8_t  reg_H1;
	int16_t  reg_H2;
	uint8_t  reg_H3;
	int16_t  reg_H4;
	int16_t  reg_H5;
	int8_t   reg_H6;

	int32_t t_fine;

	void getcalibration();

	uint8_t uread8(uint8_t reg);
	uint16_t uread16(uint8_t reg);
	int16_t read16(uint8_t reg);
	uint16_t uread16LE(uint8_t reg);
	int16_t read16LE(uint8_t reg);


};
