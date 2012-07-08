#ifndef __SETTINGS_H__
#define __SETTINGS_H__
#include "Scales.h"
class Settings {
	
	unsigned int duration; //in midi clock ticks
	uint8_t velocity;
	uint8_t row_scale[];
	uint8_t column_scale[];
	uint8_t root_note;

}
	

#endif// __SETTINGS_H__
