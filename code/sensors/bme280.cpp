/**
 *	File:       	bme280.cpp
 *	Version:  		1.0
 *	Date:       	2015
 *	License:		GPL v3
 *	Description:	AVR I2C Driver for Bosch BME280, a MEMS sensor for Temperature, Pressure and Humidity
 *	Project:		uRADMonitor KIT1, a hackable digital sensor monitoring tool with network interface
 *
 *	Copyright 2015 by Radu Motisan, radu.motisan@gmail.com
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

#include "bme280.h"


uint8_t BME280::uread8(uint8_t reg) {
	uint8_t buf;
	readmem(address, reg, &buf, 1);
	return buf;
}

uint16_t BME280::uread16(uint8_t reg) {
	uint8_t buf[2];
	readmem(address, reg, buf, 2);
	uint16_t ret = (buf[0] << 8) | buf[1];
	return ret;
}


int16_t BME280::read16(uint8_t reg) {
	return (int16_t)uread16(reg);
}

uint16_t BME280::uread16LE(uint8_t reg) {
  uint16_t temp = uread16(reg);
  return (temp >> 8) | (temp << 8);

}

int16_t BME280::read16LE(uint8_t reg) {
  return (int16_t)uread16LE(reg);
}


bool BME280::begin(uint8_t addr) {
	address = addr;
	uint8_t id = 0;
	readmem(address, BME280_REGISTER_CHIPID, &id, 1);
	if (id != 0x60) return false;

	getcalibration();
	writemem(address, BME280_REGISTER_CONTROLHUMID, 0x03); // Set before CONTROL (DS 5.4.3)
	writemem(address, BME280_REGISTER_CONTROL, 0x3F);
	return true;
}

void BME280::getcalibration() {
	uint8_t buffer[26];
	readmem(address, BME280_REGISTER_T1, buffer, 26);

	reg_T1 = buffer[0] | (buffer[1]<<8) ;
	reg_T2 = buffer[2] | (buffer[3]<<8) ;
	reg_T3 = buffer[4] | (buffer[5]<<8) ;
	reg_P1 = buffer[6] | (buffer[7]<<8) ;
	reg_P2 = buffer[8] | (buffer[9]<<8) ;
	reg_P3 = buffer[10] | (buffer[11]<<8) ;
	reg_P4 = buffer[12] | (buffer[13]<<8) ;
	reg_P5 = buffer[14] | (buffer[15]<<8) ;
	reg_P6 = buffer[16] | (buffer[17]<<8) ;
	reg_P7 = buffer[18] | (buffer[19]<<8) ;
	reg_P8 = buffer[20] | (buffer[21]<<8) ;
	reg_P9 = buffer[22] | (buffer[23]<<8) ;

	reg_H1 = buffer[25];

	readmem(address, BME280_REGISTER_H2, buffer, 7);
	reg_H2 = buffer[0] | (buffer[1]<<8) ;
	reg_H3 = buffer[2] ;
	reg_H4 = (buffer[3]<<4) | (buffer[4]&0x0f) ;
	reg_H5 = ((buffer[4]&0xf0) >> 4) | (buffer[5]<<4);
	reg_H6 = buffer[6] ;
}

void BME280::readSensors(float *temperature, uint32_t *pressure, uint8_t *humidity) {
	unsigned char buffer[8];
	readmem(address, BME280_REGISTER_PRESSUREDATA, buffer, 8);

	int32_t t_fine, adc_T;
	adc_T = ((int32_t)buffer[3] << 12) | ((int32_t)buffer[4] << 4) | (buffer[5] >> 4) ;

	int32_t var1, var2;
	var1 = ((((adc_T >> 3) - ((int32_t)reg_T1 << 1))) * ((int32_t)reg_T2)) >> 11;
	var2 = (((((adc_T >> 4) - ((int32_t)reg_T1)) * ((adc_T >> 4) - ((int32_t)reg_T1))) >> 12) * ((int32_t)reg_T3)) >> 14;
	t_fine = var1 + var2;
	*temperature = ((float)((t_fine * 5 + 128) >> 8)) / 100.0;

	// pressure
	int32_t adc_P = ((int32_t)buffer[0] << 12) | ((int32_t)buffer[1] << 4) | ((int32_t)buffer[2] >> 4) ;

	int32_t p_var1, p_var2;
	uint32_t p;
	p_var1 = (((int32_t)t_fine) >> 1) - (int32_t)64000;
	p_var2 = (((p_var1 >> 2) * (p_var1 >> 2)) >> 11 ) * ((int32_t)reg_P6);
	p_var2 = p_var2 + ((p_var1 * ((int32_t)reg_P5)) << 1);
	p_var2 = (p_var2 >> 2) + (((int32_t)reg_P4) << 16);
	p_var1 = (((reg_P3 * (((p_var1 >> 2) * (p_var1 >> 2)) >> 13 )) >> 3) + ((((int32_t)reg_P2) * p_var1) >> 1)) >> 18;
	p_var1 = ((((32768 + p_var1)) * ((int32_t)reg_P1)) >> 15);
	if (p_var1 != 0) {
		p = (((uint32_t)(((int32_t) 1048576) - adc_P) - (p_var2 >> 12))) * 3125;
		if (p < 0x80000000) {
			p = (p << 1) / ((uint32_t)p_var1);
		} else {
			p = (p / (uint32_t)p_var1) * 2;
		}
		p_var1 = (((int32_t)reg_P9) * ((int32_t)(((p>>3) * (p >> 3)) >> 13))) >> 12;
		p_var2 = (((int32_t)(p >> 2)) * ((int32_t)reg_P8)) >> 13;
		p = (uint32_t)((int32_t)p + ((p_var1 + p_var2 + reg_P7) >> 4));
		*pressure = (uint32_t)p;
	}

	// humidity
	int32_t adc_H = ((int32_t)buffer[6] << 8) | (buffer[7] );

	int32_t v_x1_u32r;
	v_x1_u32r = (t_fine - ((int32_t) 76800));
	v_x1_u32r = (((((adc_H << 14) - (((int32_t)reg_H4) << 20) - (((int32_t)reg_H5) * v_x1_u32r)) + ((int32_t)16384)) >> 15) *
			(((((((v_x1_u32r * ((int32_t)reg_H6)) >> 10) * (((v_x1_u32r * ((int32_t)reg_H3)) >> 11) + ((int32_t)32768))) >> 10) + ((int32_t)2097152)) * ((int32_t)reg_H2) + 8192) >> 14));
	v_x1_u32r = (v_x1_u32r-(((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) * ((int32_t)reg_H1)) >> 4));
	v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
	v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);
	*humidity = (uint8_t) (v_x1_u32r >> 22);
}

