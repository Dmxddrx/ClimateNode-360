#ifndef GENERAL_H
#define GENERAL_H

#include <Arduino.h>

#define NODE_ID 1
#define SAMPLE_INTERVAL_SEC 60

struct SensorData {
  uint8_t nodeId;
  float temperature;
  float humidity;
  float dust;
};

void initGeneral();
void goDeepSleep(uint32_t seconds);

#endif