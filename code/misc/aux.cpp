#include "aux.h"
#include "detectors.h"

// Reset the microcontroller
void aux_softwarereset() {
	asm ("JMP 0");
	// http://forum.arduino.cc/index.php?topic=78020.0
}

// Proper reset using the watchdog: untested
void aux_wdreset() {
  wdt_enable(WDTO_15MS);
  while(1);
}

// Reads the ADC port specified by i
uint16_t aux_ADCRead(uint8_t i) {
	ADMUX = i;
	_delay_ms(1); //very important!
	ADCSRA = _BV(ADEN) | _BV(ADPS2) | _BV(ADSC);
	_delay_ms(1); //very important! allows ADEN to turn on ADC
	while (bit_is_set(ADCSRA,ADSC));
	uint16_t v = ADCL;
	v |= ADCH << 8;
	return v;
}

// Read AVCC using internal 1.1V reference, for atmega168 and atmega328
uint16_t aux_readIntRefVcc() {
	ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1); // Read 1.1V reference against AVcc
	_delay_ms(1); //very important!
	ADCSRA = _BV(ADEN) | _BV(ADPS2) | _BV(ADSC);
	_delay_ms(1); //very important! allows ADEN to turn on ADC
	while (bit_is_set(ADCSRA,ADSC));
	uint16_t v = ADCL;
	v |= ADCH<<8;
	v = 1126400L / v; // Back-calculate AVcc in mV (1100 * 1024)
	return v;
}


// return ADC port voltage computed against given VRef, with resistive divider
uint16_t aux_readDivVoltage(float vref, uint16_t divider_R1, uint16_t divider_R2, uint8_t i) {
	return (uint16_t) (aux_ADCRead(i) / 1024.0 * (divider_R1 + divider_R2) / divider_R2 * vref);
}


// dose equivalent linear approximative conversion from CPM
float aux_CPM2uSVh(uint8_t tube, uint32_t cpm) {
	return aux_detectorFactor(tube) * cpm;
}

/**
 * Encodes a small float (-127 .. 127) to an 16 bit integer, including sign and two decimals
 */
uint16_t float2int(float val) {
	int8_t sintp = (int8_t)val;				//signed integer part
	int8_t sdecp = (val - sintp ) * 100;	//signed decimal part
	uint8_t udecp = sdecp>0?sdecp:-1*sdecp; //removed decimal sign
	uint8_t uintp = sintp + 127;			//convert to unsigned
	uint16_t res = (udecp << 8) | (uint16_t)uintp;		//pack it together
	return res;
}

/**
 * copyBytes
 * copy specified src_bytes from src to dst at specified offset
 * this function is not safe, as dst is not checked for size. Make sure it is big enough!
 */
uint16_t copyBytes(void *dst, uint16_t dst_offset, void *src, uint8_t src_bytes) {
	if (!dst || !src_bytes) return dst_offset;

	for (int i=0; i< src_bytes;i++)
		((uint8_t *)dst)[dst_offset + i] = !src? 0 : ((uint8_t *)src)[src_bytes - i - 1]; // either 0 if empty source or most significant bytes go first
	return dst_offset + src_bytes;
}
