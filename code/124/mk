CC=avr-gcc
CFLAGS=-fdata-sections -ffunction-sections -Wall -Wl,-u,vfprintf,--gc-sections -funsigned-char -Os -DF_CPU=8000000 -mmcu=atmega328p
OBJCOPY=avr-objcopy
LIBS=-lprintf_flt -lm

OBJS=uRADMonitor.o  data.o UI.o detectors.o inverter.o DigitalPin.o i2c.o 5110.o morse.o utils.o dhcp_client.o dnslkup.o enc28j60.o ip_arp_udp_tcp.o bme280.o bmp180.o timecounter.o watchdog.o

all:			uRADMonitor.hex

uRADMonitor.hex:	$(OBJS)
			$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o uRADMonitor.elf
			$(OBJCOPY) -O ihex uRADMonitor.elf uRADMonitor.hex

uRADMonitor.o:		uRADMonitor.cpp
			$(CC) $(CFLAGS) -c uRADMonitor.cpp

data.o:			app/data.cpp
			$(CC) $(CFLAGS) -c app/data.cpp

UI.o:			app/UI.cpp
			$(CC) $(CFLAGS) -c app/UI.cpp

detectors.o:		geiger/detectors.cpp
			$(CC) $(CFLAGS) -c geiger/detectors.cpp

inverter.o:		geiger/inverter.cpp
			$(CC) $(CFLAGS) -c geiger/inverter.cpp

DigitalPin.o:		gpio/DigitalPin.cpp
			$(CC) $(CFLAGS) -c gpio/DigitalPin.cpp

i2c.o:			i2c/i2c.cpp
			$(CC) $(CFLAGS) -c i2c/i2c.cpp

5110.o:			lcd/5110.cpp
			$(CC) $(CFLAGS) -c lcd/5110.cpp

morse.o:		misc/morse.cpp
			$(CC) $(CFLAGS) -c misc/morse.cpp

utils.o:		misc/utils.cpp
			$(CC) $(CFLAGS) -c misc/utils.cpp

dhcp_client.o:		net/dhcp_client.cpp
			$(CC) $(CFLAGS) -c net/dhcp_client.cpp

dnslkup.o:		net/dnslkup.cpp
			$(CC) $(CFLAGS) -c net/dnslkup.cpp

enc28j60.o:		net/enc28j60.cpp
			$(CC) $(CFLAGS) -c net/enc28j60.cpp

ip_arp_udp_tcp.o:	net/ip_arp_udp_tcp.cpp
			$(CC) $(CFLAGS) -c net/ip_arp_udp_tcp.cpp

bme280.o:		sensors/bme280.cpp
			$(CC) $(CFLAGS) -c sensors/bme280.cpp

bmp180.o:		sensors/bmp180.cpp
			$(CC) $(CFLAGS) -c sensors/bmp180.cpp

timecounter.o:		time/timecounter.cpp
			$(CC) $(CFLAGS) -c time/timecounter.cpp

watchdog.o:		time/watchdog.cpp
			$(CC) $(CFLAGS) -c time/watchdog.cpp

clean:
			rm -f *.o uRADMonitor.elf

realclean:		clean
			rm -f uRADMonitor.hex