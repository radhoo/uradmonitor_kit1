For those of you that want to avoid the hassle of downloading and compiling the code, I have included the compiled code below. 
One variant is a simple, offline code that works without the Ethernet module: uradmonitor-kit1-local.hex 
while the second adds support for Ethernet as well: uradmonitor-kit1-eth.hex

Some instructions on how to burn a hex firmware file to the microcontroller are provided here: 
http://www.pocketmagic.net/how-to-set-the-avr-fusebits/

To correctly burn any of the two hex files into your microcontroller, see the prog.sh script that invokes avrdude. It also sets the fuses for the 8MHz crystal.

Important note:
Code for joining the uRADMonitor network is currently available only on request due to sensitive security details involved. With this code a valid uRADMonitor device ID is also provided.