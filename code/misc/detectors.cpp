#include "detectors.h"

// not more than 10 chars!
char *aux_detectorName(uint8_t param) {
	switch (param) {
		case GEIGER_TUBE_SBM20: return "SBM20";
		case GEIGER_TUBE_SI29BG:return "SI29BG";
		case GEIGER_TUBE_SBM19: return "SBM19";
		case GEIGER_TUBE_STS5:  return " STS5";
		case GEIGER_TUBE_SI22G: return "SI22G";
		case GEIGER_TUBE_SI3BG: return "SI3BG";
		case GEIGER_TUBE_SBM21: return "SBM21";
		case GEIGER_TUBE_SI1G:  return " SI1G";
		default: return "unknown";
	}
}

float aux_detectorFactor(uint8_t param) {
	switch (param) {
		case GEIGER_TUBE_SBM20: 	return 0.006315;
		case GEIGER_TUBE_SI29BG: 	return 0.010000;
		case GEIGER_TUBE_SBM19: 	return 0.001500;
		case GEIGER_TUBE_STS5: 		return 0.006666;
		case GEIGER_TUBE_SI22G: 	return 0.001714;
		case GEIGER_TUBE_SI3BG: 	return 0.631578;
		case GEIGER_TUBE_SBM21: 	return 0.048000;
		case GEIGER_TUBE_SI1G:		return 0.006000;
		default: 0;
	}
}
