#ifndef ESPNOW_H
#define ESPNOW_H

#include "datatypes.h"

#define MAX_NODES 20 

// Change this line to match the .cpp
extern volatile uint8_t receivedCount; 
extern SensorData receivedData[MAX_NODES];

void initESPNow();
void waitForSlaves(uint32_t ms);
void storeLocalReading(SensorData &data);

#endif