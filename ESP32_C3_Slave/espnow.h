#ifndef ESPNOW_H
#define ESPNOW_H

#include "datatypes.h"

#define MAX_NODES 20   // Maximum nodes (C3 slaves + master local)

extern SensorData receivedData[MAX_NODES];
extern uint8_t receivedCount;

// Initialize ESP-NOW and register receive callback
void initESPNow();

// Wait a window to allow slaves to send their data
void waitForSlaves(uint32_t ms);

// Add master local reading to the buffer
void storeLocalReading(SensorData &data);

#endif