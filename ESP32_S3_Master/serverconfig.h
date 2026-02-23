#ifndef SERVERCONFIG_H
#define SERVERCONFIG_H

#include "datatypes.h"
#include <stdint.h>
#include <stddef.h>

// Wi-Fi and server checks
bool isNetworkAvailable();

// Queue system
void addToServerQueue(SensorData &data);
size_t getQueueSize();
SensorData getQueuedData(uint8_t index);
void clearQueue();

// Upload function
bool uploadQueuedData();

#endif