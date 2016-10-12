#!/bin/sh
avrdude -p atmega328p -c usbasp -U lfuse:w:0xDC:m -U hfuse:w:0xD7:m  
avrdude -p atmega328p -c usbasp -U flash:w:uradmonitor-KIT1-EXP.hex:i
