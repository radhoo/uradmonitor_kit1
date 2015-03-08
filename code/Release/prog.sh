#!/bin/sh
echo "*** writing fuses for 8MHz external crystal"
avrdude -p atmega328p -c usbasp -U lfuse:w:0xdc:m 	-U hfuse:w:0xdf:m 	-U efuse:w:0xFf:m 	-U lock:w:0xFF:m 
echo "*** writing eeprom"
avrdude -p atmega328p -c usbasp -U eeprom:w:uradmonitor-dip-C101.eep:i
echo "*** writting flash"
avrdude -p atmega328p -c usbasp -U flash:w:uradmonitor-dip-C101.hex:i
