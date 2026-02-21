#ifndef SDCARD_H
#define SDCARD_H

#include "datatypes.h"

// Initialize SD card
void initSD();

// Save an array of SensorData to SD card
void saveBufferedDataToSD(SensorData *data, uint8_t count);

#endif