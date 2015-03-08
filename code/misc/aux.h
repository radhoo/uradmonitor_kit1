#pragma once

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include "../timeout.h"


// Reset the microcontroller
void aux_softwarereset();

// Proper reset using the watchdog: untested
void aux_wdreset() ;

// Reads the ADC port specified by i
uint16_t aux_ADCRead(uint8_t i);

// Read AVCC using internal 1.1V reference, for atmega168 and atmega328
uint16_t aux_readIntRefVcc();

// return ADC port voltage computed against given VRef, with resistive divider
uint16_t aux_readDivVoltage(float vref, uint16_t divider_R1, uint16_t divider_R2, uint8_t i);

// dose equivalent linear approximative conversion from CPM
float aux_CPM2uSVh(uint8_t tube, uint32_t cpm);


/**
 * Encodes a small float (-127 .. 127) to an 16 bit integer, including sign and two decimals
 */
uint16_t float2int(float val);

/**
 * copyBytes
 * copy specified src_bytes from src to dst at specified offset
 * this function is not safe, as dst is not checked for size. Make sure it is big enough!
 */
uint16_t copyBytes(void *dst, uint16_t dst_offset, void *src, uint8_t src_bytes);
