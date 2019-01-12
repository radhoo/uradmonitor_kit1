/**
 *	Copyright 2019 by Shawn Rutledge, s@ecloud.org
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
 #ifndef _SERIAL_H_
#define _SERIAL_H_

#include <stdint.h>

#include "../app/ringbuf.h"

#define UART_BAUD_RATE 9600

/**
    Init the send/receive buffers and the UART hardware
*/
void serialInit();

/**
    Send one byte
    @return 1 if success, 0 if the send buffer is full
*/
bool serialSendByte(uint8_t byte);

/**
    Send len bytes
    @return number of bytes successfully inserted into the send buffer
*/
uint8_t serialSend(const uint8_t* buf, uint8_t len);

/**
    Check if there are any pending bytes to be sent on the UART.
*/
void checkSerialSend();

#endif // _SERIAL_H_
