/*
 * BMP180 Sensor Library for AVR Microcontrollers
 * Copyright (C) 2012, Radu Motisan , radu.motisan@gmail.com , All rights reserved.
 * http://www.pocketmagic.net/
 */


#pragma  once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <avr/io.h>

//#include <util/delay.h>
#include "../i2c/i2c.h"

//#define BMP180_ADDR 0xEF
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
	
	//void writemem(uint8_t reg, uint8_t value);
	//void readmem(uint8_t reg, uint8_t buff[], uint8_t bytes);
	void getcalibration();
	int32_t readRawTemperature(void);
	int32_t readRawPressure(int32_t rawtemperature );
	double convertRawTemperature(int32_t rawtemperature);
	int32_t convertRawPressure(int32_t rawpressure) ;
	double convertAltitude(int32_t pressure);
public:
	void	begin();
	double	readTemperature();
	int32_t readPressure();
	double	readAltitude();
	void readSensors(double *temperature, uint32_t *pressure, double *altitude) ;
};
