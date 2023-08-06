#!/bin/sh
echo "*** writing fuses for 8MHz external crystal"
# preserve EEPROM set with hfuse:0xD7 . To delete EEPROM, hfuse: 0xDF
avrdude -p atmega328p -c usbasp -U lfuse:w:0xDC:m -U hfuse:w:0xD7:m  

echo "*** writting flash"
avrdude -V -p atmega328p -c usbasp -U flash:w:uradmonitor-KIT1-EXP.hex:i
