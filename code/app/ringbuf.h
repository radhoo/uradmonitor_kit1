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
#include "../misc/utils.h"

/*
	Mini, memory-conserving ring buffer implementation.  Size <= 255;
	different sized ring buffers are different types.
*/
template <class RingBufEntry, int Size>
class RingBuf
{
public:
    uint8_t consumeOffset;		// Offset where next byte can be read.
	uint8_t loadOffset;			// Offset where next byte can be stored + 1
	bool empty;					// TRUE if rbCount must be 0, FALSE if rbCount > 0
	RingBufEntry buf[Size];

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

public:
	/**
		Constructor
	*/
    RingBuf() :
        consumeOffset(0),
        loadOffset(0),
        empty(true)
    {
    }

	/**
		@return the number of entries that are loaded into the ring buffer
	*/
    uint8_t count() const
    {
        int16_t ret = loadOffset - consumeOffset;
        if (ret < 0)
            ret += Size;
        if (!ret && !empty)
            return Size;
        return (uint8_t)ret;
    }

	/**
		@return the number of unused entries in the ring buffer
	*/
    uint8_t available() const
    {
        return Size - count();
    }

	/**
		Look at the next available entry without consuming it.
		@return the next entry, or 0 if unavailable (better call rbCount first to find out)
	*/
    RingBufEntry peek() const
    {
        return buf[consumeOffset];
    }

	/**
		Look at the nth available entry without consuming it.
		@return the entry, or 0 if unavailable (better call rbCount first to find out)
	*/
    RingBufEntry at(uint16_t idx) const
    {
        uint16_t offset;
        offset = consumeOffset + idx;
        if (offset >= Size)
            offset -= Size;
        return buf[offset];
    }

	/**
		Look at the nth entry (counting from the beginning of the buffer) without consuming it.
		@return the entry
	*/
    RingBufEntry atAbs(uint16_t idx) const
    {
        uint16_t offset = idx;
        if (offset >= Size)
            offset -= Size;
        return buf[offset];
    }

	/**
		Discard count entries from the ring buffer.
		@return number of entries actually consumed
	*/
    uint8_t remove(uint8_t c)
    {
        uint8_t countWas = count();
        if (count() == 1)
            empty = true;
        uint16_t newConsume = (uint16_t)(consumeOffset) + (uint16_t)(MIN(c, count()));
        if (newConsume >= Size)
            newConsume -= Size;
        consumeOffset = newConsume;
        return countWas - count();
    }

	/**
		Take one entry from the ring buffer.
		@return the consumed entry, or 0 if unavailable (better call rbCount first to find out)
	*/
    RingBufEntry take()
    {
        uint16_t offset = consumeOffset;
        RingBufEntry ret = buf[offset];
        offset++;
        if (offset >= Size)
            offset -= Size;
        consumeOffset = offset;
        return ret;
    }

	/**
		Put one entry into the ring buffer
		@return true if success, false if it's rejected (due to being full)
	*/
    bool insert(RingBufEntry entry)
    {
        if (!available())
            return false;
        uint8_t lo = loadOffset;
        // Wrap around just prior to insertion, if we need to.
        if (lo >= Size)
            lo = 0;
        // It's OK to store one more.
        buf[lo] = entry;
        // Update loadOffset for next time.
        ++lo;
        loadOffset = lo;
        empty = false;
        return true;
    }
};
