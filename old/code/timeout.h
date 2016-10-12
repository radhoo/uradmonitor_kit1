/*
    File:       timeout.h
    Version:    0.1.0
    Date:       Feb. 21, 2013
	License:	GPL v2
    
	define the F_CPU and includes the delay.h headers, to provide easy access 
	to delay functions _delay_ms and _delay_us
	change F_CPU to match your configuration
    
    ****************************************************************************
    Copyright (C) 2013 Radu Motisan  <radu.motisan@gmail.com>
	
	http://www.pocketmagic.net

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
    ****************************************************************************
 */


// Change to match your board! //
#define F_CPU 8000000UL  


#ifndef ALIBC_OLD
	#include <util/delay.h>
#else
	#include <avr/delay.h>
#endif

