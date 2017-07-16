/*
** I2C master library using hardware TWI interface
** Version: 	0.1.0
** Date: 		July, 2015
** License:		LGPL v3
** Copyright: 	Peter Fleury <pfleury@gmx.ch>  http://jump.to/fleury
** Description:	any AVR device with hardware TWI
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


#pragma once

#include <avr/io.h>

/** defines the data direction (reading from I2C device) in i2c_start(),i2c_rep_start() */
#define I2C_READ    1

/** defines the data direction (writing to I2C device) in i2c_start(),i2c_rep_start() */
#define I2C_WRITE   0

/* I2C clock in Hz */
#define SCL_CLOCK  100000L

#define I2C_ERROR 1
#define I2C_SUCCESS 0

/**
 @brief initialize the I2C master interace. Need to be called only once
 @param  void
 @return none
 */
 void i2c_init(void);


/**
 @brief Terminates the data transfer and releases the I2C bus
 @param void
 @return none
 */
 void i2c_stop(void);


/**
 @brief Issues a start condition and sends address and transfer direction

 @param    addr address and transfer direction of I2C device
 @retval   0   device accessible
 @retval   1   failed to access device
 */
 uint8_t i2c_start(uint8_t addr);

/**
 @brief Issues a start condition and sends address and transfer direction

 If device is busy, use ack polling to wait until device ready
 @param    addr address and transfer direction of I2C device
 @retval   0  successful
 @retval   1  failed
 */
 uint8_t i2c_start_wait(uint8_t addr);


/**
 @brief Send one byte to I2C device
 @param    data  byte to be transfered
 @retval   0 write successful
 @retval   1 write failed
 */
 uint8_t i2c_write(uint8_t data);


/**
 @brief    read one byte from the I2C device, request more data from device
 @return   byte read from I2C device
 */
 uint8_t i2c_readAck(void);

/**
 @brief    read one byte from the I2C device, read is followed by a stop condition
 @return   byte read from I2C device
 */
 uint8_t i2c_readNak(void);

/**
 @brief    read one byte from the I2C device

 Implemented as a macro, which calls either i2c_readAck or i2c_readNak

 @param    ack 1 send ack, request more data from device<br>
               0 send nak, read is followed by a stop condition
 @return   byte read from I2C device
 */
//#define i2c_read(ack)  (ack) ? i2c_readAck() : i2c_readNak();

 uint8_t writemem(uint8_t address, uint8_t reg, uint8_t value);


 uint8_t readmem(uint8_t address, uint8_t reg, uint8_t buff[], uint8_t bytes);
