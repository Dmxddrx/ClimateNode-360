#ifndef GP2Y10_H
#define GP2Y10_H

#include <stdint.h>

// Initialize optical dust sensor (GP2Y10)
void initDustSensor();

// Read dust concentration, scaled x10 (e.g., 0.35V → 3)
int16_t readDust();

#endif