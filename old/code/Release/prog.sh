#!/bin/sh
echo "*** writing fuses for 8MHz external crystal"
avrdude -p atmega328p -c usbasp -U lfuse:w:0xdc:m 	-U hfuse:w:0xdf:m 	-U efuse:w:0xFf:m 	-U lock:w:0xFF:m 
echo "*** writting flash"
avrdude -p atmega328p -c usbasp -U flash:w:uradmonitor-KIT1.0.hex:i
