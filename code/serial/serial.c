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

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdarg.h>

#include "serial.h"

/**
    Serial port implementation
*/

//---------------------------------------------------------
// Module vars

static RingBuf<uint8_t,64> serialSendBuf;

//---------------------------------------------------------
// Public functions

void serialInit()
{
	// set baud rate
	//~ const uint16_t ubrr = 51; // 8000000 / (16 * UART_BAUD_RATE) - 1
	//~ UBRR0H = (uint8_t)(ubrr>>8);
	//~ UBRR0L = (uint8_t)ubrr;
	UBRR0H = 0;
	UBRR0L = 51; // 9600 

	// Enable receiver and transmitter; enable RX interrupt
	//~ UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
	// Enable transmitter only
	UCSR0B = (1 << TXEN0);

	// asynchronous 8N1
	UCSR0C = (3 << UCSZ00);
	
	DDRD |= _BV(PD0); // Set PD0 (RX) as output pin so we can use it as another "ground"
}

uint8_t serialSend(const uint8_t* buf, uint8_t len)
{
	serialSendBuf.remove(serialSendBuf.count()); // bug workaround
	for (int i = 0; i < len; ++i)
		if (!serialSendByte(buf[i]))
			return i;
	return len;
}

bool serialSendByte(uint8_t byte)
{
	return serialSendBuf.insert(byte);
}

/**
    Check if there are any pending bytes to be sent on the UART.
*/
void checkSerialSend()
{
	if (serialSendBuf.count() > 0 && (UCSR0A & _BV(UDRE0)))
		UDR0 = serialSendBuf.take();
}
