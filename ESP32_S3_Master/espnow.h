#ifndef ESPNOW_H
#define ESPNOW_H

#include <Arduino.h>
#include "general.h"

#define MAX_NODES 20

extern SensorData receivedData[MAX_NODES];
extern uint8_t receivedCount;

void initESPNow();
void waitForSlaves(uint32_t ms);

#endif