#pragma once


#include <stdint.h>

// define Radiation DETECTORS
#define GEIGER_TUBE_UNKNOWN 0x0
#define GEIGER_TUBE_SBM20 0x1
#define GEIGER_TUBE_SI29BG 0x2
#define GEIGER_TUBE_SBM19 0x3
#define GEIGER_TUBE_STS5 0x4
#define GEIGER_TUBE_SI22G 0x5
#define GEIGER_TUBE_SI3BG 0x6
#define GEIGER_TUBE_SBM21 0x7
#define GEIGER_TUBE_SI1G 0x8


char *aux_detectorName(uint8_t param);
float aux_detectorFactor(uint8_t param);
