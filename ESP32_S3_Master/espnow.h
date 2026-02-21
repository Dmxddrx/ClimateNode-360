#ifndef ESPNOW_H
#define ESPNOW_H

#include "datatypes.h"

#define MAX_NODES 20   // maximum number of C3 slaves + S3 local

// Buffer to store received data
extern SensorData receivedData[MAX_NODES];
extern uint8_t receivedCount;

// Initialize ESP-NOW and Wi-Fi in station mode
void initESPNow();

// Wait a few milliseconds for slaves to send data
void waitForSlaves(uint32_t ms);

// Store S3 local sensor readings in buffer
void storeLocalReading(SensorData &data);

#endif