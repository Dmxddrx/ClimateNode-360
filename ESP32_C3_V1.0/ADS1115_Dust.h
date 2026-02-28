#ifndef ADS1115_DUST_H
#define ADS1115_DUST_H

#include <stdint.h>

// Initialize GP2Y10 and ADS1115
void initDustSensor();

// Read dust concentration via ADS1115, scaled x10
int16_t readDust();

#endif