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
#include <avr/io.h>
#include <avr/interrupt.h>
#include <compat/twi.h>

#include "i2c.h"


/*************************************************************************
 Initialization of the I2C bus interface. Need to be called only once
*************************************************************************/
void i2c_init(void) {
  /* initialize TWI clock: 100 kHz clock, TWPS = 0 => prescaler = 1 */
  TWSR = 0;                         /* no prescaler */
  TWBR = ((F_CPU/SCL_CLOCK)-16)/2;  /* must be > 10 for stable operation */
}/* i2c_init */


/*************************************************************************
  Issues a start condition and sends address and transfer direction.
  return 0 = device accessible, 1= failed to access device
*************************************************************************/
uint8_t i2c_start(uint8_t address) {
    uint8_t   twst;

	// send START condition
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);

	// wait until transmission completed
	uint16_t timeout = 1002;
	while(!(TWCR & (1<<TWINT))) { timeout--; if (!timeout) return I2C_ERROR; }

	// check value of TWI Status Register. Mask prescaler bits.
	twst = TW_STATUS & 0xF8;
	if ( (twst != TW_START) && (twst != TW_REP_START)) return I2C_ERROR;

	// send device address
	TWDR = address;
	TWCR = (1<<TWINT) | (1<<TWEN);

	// wail until transmission completed and ACK/NACK has been received
	timeout = 1002;
	while(!(TWCR & (1<<TWINT))) { timeout--; if (!timeout) return I2C_ERROR; }

	// check value of TWI Status Register. Mask prescaler bits.
	twst = TW_STATUS & 0xF8;
	if ( (twst != TW_MT_SLA_ACK) && (twst != TW_MR_SLA_ACK) ) return I2C_ERROR;

	return I2C_SUCCESS;

}/* i2c_start */


/*************************************************************************
 Issues a start condition and sends address and transfer direction.
 If device is busy, use ack polling to wait until device is ready

 Input:   address and transfer direction of I2C device
*************************************************************************/
/*void i2c_start_wait(uint8_t address) {
    uint8_t   twst;
    while ( 1) {
	    // send START condition
	    TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
    	// wait until transmission completed
	    while (!(TWCR & (1<<TWINT)));
    	// check value of TWI Status Register. Mask prescaler bits.
    	twst = TW_STATUS & 0xF8;
    	if ( (twst != TW_START) && (twst != TW_REP_START)) continue;
    	// send device address
    	TWDR = address;
    	TWCR = (1<<TWINT) | (1<<TWEN);
    	// wait until transmission completed
    	while (!(TWCR & (1<<TWINT)));
    	// check value of TWI Status Register. Mask prescaler bits.
    	twst = TW_STATUS & 0xF8;
    	if ( (twst == TW_MT_SLA_NACK )||(twst ==TW_MR_DATA_NACK) )  {
    	    // device busy, send stop condition to terminate write operation
	        TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
	        // wait until stop condition is executed and bus released
	        while ((TWCR & (1<<TWSTO)));
    	    continue;
    	}
    	//if( twst != TW_MT_SLA_ACK) return 1;
    	break;
     }
}*/

/*************************************************************************
 Issues a start condition and sends address and transfer direction.
 If device is busy, use ack polling to wait until device is ready

 Input:   address and transfer direction of I2C device
 @retval   0  successful
 @retval   1  failed
*************************************************************************/
uint8_t i2c_start_wait(uint8_t address) {
	do {
		//Put Start Condition on TWI Bus
		TWCR=(1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
		//Poll Till Done
		uint16_t timeout = 1002;
		while(!(TWCR & (1<<TWINT))) { timeout--; if (!timeout) return I2C_ERROR; }
		//Check status
		if((TWSR & 0xF8) != 0x08) return 1;
		//Now write SLA+W
		TWDR=address;
		//Initiate Transfer
		TWCR=(1<<TWINT)|(1<<TWEN);
		//Poll Till Done
		timeout = 1002;
		while(!(TWCR & (1<<TWINT))) { timeout--; if (!timeout) return I2C_ERROR; }
	} while((TWSR & 0xF8) != 0x18);

	return 0;
}


/*************************************************************************
 Terminates the data transfer and releases the I2C bus
*************************************************************************/
void i2c_stop(void) {
    /* send stop condition */
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
	// wait until stop condition is executed and bus released
	uint16_t timeout = 1002;
	while( (TWCR & (1<<TWSTO))) { timeout--; if (!timeout) return; };
}


/*************************************************************************
  Send one byte to I2C device
  Input:    byte to be transfered
  Return:   0 write successful
            1 write failed
*************************************************************************/
uint8_t i2c_write( uint8_t data ) {
    uint8_t   twst;
	// send data to the previously addressed device
	TWDR = data;
	TWCR = (1<<TWINT) | (1<<TWEN);
	// wait until transmission completed or timeout
	uint16_t timeout = 1002;
	while(!(TWCR & (1<<TWINT))) { timeout--; if (!timeout) return I2C_ERROR; };
	// check value of TWI Status Register. Mask prescaler bits
	twst = TW_STATUS & 0xF8;
	if( twst != TW_MT_DATA_ACK) return I2C_ERROR;
	return I2C_SUCCESS;
}


/*************************************************************************
 Read one byte from the I2C device, request more data from device
 Return:  byte read from I2C device
*************************************************************************/
uint8_t i2c_readAck(void) {
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWEA);
	while(!(TWCR & (1<<TWINT)));
    return TWDR;
}

/*************************************************************************
 Read one byte from the I2C device, read is followed by a stop condition
 Return:  byte read from I2C device
*************************************************************************/
uint8_t i2c_readNak(void) {
	TWCR = (1<<TWINT) | (1<<TWEN);
	uint16_t timeout = 1002;
	while(!(TWCR & (1<<TWINT))) { timeout--; if (!timeout) return I2C_ERROR; };
    return TWDR;
}


uint8_t writemem(uint8_t address, uint8_t reg, uint8_t value) {
	if (i2c_start_wait(address | I2C_WRITE))  return I2C_ERROR;
	if (i2c_write(reg)) return I2C_ERROR;
	if (i2c_write(value)) return I2C_ERROR;
	i2c_stop();
	return I2C_SUCCESS;
}

// i2c read
uint8_t readmem(uint8_t address, uint8_t reg, uint8_t buff[], uint8_t bytes) {
	if (i2c_start_wait(address | I2C_WRITE)) return I2C_ERROR;
	if (i2c_write(reg)) return I2C_ERROR;
	if (i2c_start(address | I2C_READ)) return I2C_ERROR;
	for(uint8_t i=0; i<bytes; i++)
		buff[i] = i2c_readAck();
	i2c_readNak();
	i2c_stop();
	return I2C_SUCCESS;
}
