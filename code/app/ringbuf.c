/**
 *	File:       	ringbuf.c
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
#include "ringbuf.h"
#include "../misc/utils.h"

void rbInit(RingBuf* rb)
{
	rb->consumeOffset = 0;
	rb->loadOffset = 0;
	rb->empty = true;
}

/*
	https://en.wikipedia.org/wiki/Circular_buffer#Circular_buffer_mechanics
	"When they are equal, the buffer is empty, and
	when the start is one greater than the end, the buffer is full"
	Here consumeOffset is the "start" and loadOffset is the "end":
	where _would_ the next element be consumed from, and
	where _would_ the next element be inserted, respectively.
	But loadOffset == consumeOffset == 0 doesn't always means "empty"
	in this implementation, so we need to maintain the empty flag.
*/
uint8_t rbCount(const RingBuf* rb)
{
	int16_t ret = rb->loadOffset - rb->consumeOffset;
	if (ret < 0)
		ret += RING_BUF_SIZE;
	if (!ret && !rb->empty)
		return RING_BUF_SIZE;
	return (uint8_t)ret;
}

uint8_t rbAvailable(const RingBuf* rb)
{
	return RING_BUF_SIZE - rbCount(rb);
}

RingBufEntry rbPeek(const RingBuf* rb)
{
	return rb->buf[rb->consumeOffset];
}

RingBufEntry rbRef(const RingBuf* rb, uint16_t idx)
{
	uint16_t offset;
	offset = rb->consumeOffset + idx;
	if (offset >= RING_BUF_SIZE)
		offset -= RING_BUF_SIZE;
	return rb->buf[offset];
}

RingBufEntry rbRefAbs(const RingBuf* rb, uint16_t idx)
{
	uint16_t offset = idx;
	if (offset >= RING_BUF_SIZE)
		offset -= RING_BUF_SIZE;
	return rb->buf[offset];
}

uint8_t rbConsume(RingBuf* rb, uint8_t count)
{
	uint8_t countWas = rbCount(rb);
	if (rbCount(rb) == 1)
		rb->empty = true;
	uint16_t newConsume = (uint16_t)(rb->consumeOffset) + (uint16_t)(MIN(count, rbCount(rb)));
	if (newConsume >= RING_BUF_SIZE)
		newConsume -= RING_BUF_SIZE;
	rb->consumeOffset = newConsume;
	return countWas - rbCount(rb);
}

RingBufEntry rbTake(RingBuf* rb)
{
	uint16_t offset = rb->consumeOffset;
	RingBufEntry ret = rb->buf[offset];
	offset++;
	if (offset >= RING_BUF_SIZE)
		offset -= RING_BUF_SIZE;
	rb->consumeOffset = offset;
	return ret;
}

bool rbLoadEntry(RingBuf* rb, RingBufEntry entry)
{
	if (!rbAvailable(rb))
		return false;
	uint8_t loadOffset = rb->loadOffset;
	// Wrap around just prior to insertion, if we need to.
	if (loadOffset >= RING_BUF_SIZE)
		loadOffset = 0;
	// It's OK to store one more.
	rb->buf[loadOffset] = entry;
	// Update loadOffset for next time.
	++loadOffset;
	rb->loadOffset = loadOffset;
	rb->empty = false;
	return true;
}
