#ifndef AM1001_H
#define AM1001_H

#include <stdint.h>

void initSensors();
int16_t readTemperature();
int16_t readHumidity();

#endif