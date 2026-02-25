#ifndef SHT30_H
#define SHT30_H

#include <stdint.h>

// Initialize SHT30 and I2C
void initSHT30();

// Read temperature in Celsius, scaled x100
int16_t readTemperature();

// Read humidity in %, scaled x100
int16_t readHumidity();

#endif