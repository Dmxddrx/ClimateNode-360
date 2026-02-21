#ifndef AM1001_H
#define AM1001_H

#include <stdint.h>

// Initialize AM1001 temperature and humidity sensor
void initSensors();

// Read temperature in Celsius, scaled x100 (e.g., 25.34°C → 2534)
int16_t readTemperature();

// Read humidity in %, scaled x100 (e.g., 65.12% → 6512)
int16_t readHumidity();

#endif