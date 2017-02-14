/**
 *	File:       	ringbuf.h
 *	Version:  		1.0
 *	Date:       	2016
 *	License:		GPL v3
 *	Description:	a ring buffer
 *	Project:		uRADMonitor KIT1, a hackable digital sensor monitoring tool with network interface
 *
 *	Copyright 2016 by Shawn Rutledge, s@ecloud.org
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

#include <stdint.h>
#include "../config.h"
#include "../lcd/5110.h"

#define RING_BUF_SIZE AVERAGE_SAMPLES // ((84 - CHAR_WIDTH * 3) / 2)  // enough to display on the LCD every other pixel, reserving 3 colums of text at right

/*
	Mini, memory-conserving ring buffer implementation.  RING_BUF_SIZE <= 255.
	Having all ring buffers be the same size means we do not need to store the
	size in the struct (it's a constant).  And offsets are cheaper than pointers too.
*/

typedef uint16_t RingBufEntry;

typedef struct _RingBuf
{
	uint8_t consumeOffset;		// Offset where next byte can be read.
	uint8_t loadOffset;			// Offset where next byte can be stored + 1
	bool empty;					// TRUE if rbCount must be 0, FALSE if rbCount > 0
	RingBufEntry buf[RING_BUF_SIZE];
} RingBuf;

/**
	Initialize the ring buffer (does not allocate memory - you do that by static declaration)
*/
void rbInit(RingBuf* rb);

/**
	@return the number of entries that are loaded into the ring buffer
*/
uint8_t rbCount(const RingBuf* rb);

/**
	@return the number of unused entries in the ring buffer
*/
uint8_t rbAvailable(const RingBuf* rb);

/**
	Look at the next available entry without consuming it.
	@return the next entry, or 0 if unavailable (better call rbCount first to find out)
*/
RingBufEntry rbPeek(const RingBuf* rb);

/**
	Look at the nth available entry without consuming it.
	@return the entry, or 0 if unavailable (better call rbCount first to find out)
*/
RingBufEntry rbRef(const RingBuf* rb, uint16_t idx);

/**
	Look at the nth entry (counting from the beginning of the buffer) without consuming it.
	@return the entry
*/
RingBufEntry rbRefAbs(const RingBuf* rb, uint16_t idx);

/**
	Discard count entries from the ring buffer.
	@return number of entries actually consumed
*/
uint8_t rbConsume(RingBuf* rb, uint8_t count);

/**
	Take one entry from the ring buffer.
	@return the consumed entry, or 0 if unavailable (better call rbCount first to find out)
*/
RingBufEntry rbTake(RingBuf* rb);

/**
	Put one entry into the ring buffer
	@return true if success, false if it's rejected (due to being full)
*/
bool rbLoadEntry(RingBuf* rb, RingBufEntry entry);
