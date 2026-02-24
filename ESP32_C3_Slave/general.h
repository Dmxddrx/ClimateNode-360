#ifndef GENERAL_H
#define GENERAL_H

#include <Arduino.h>
#include "datatypes.h"

// Node settings
#define NODE_ID 2
#define SAMPLE_INTERVAL_SEC 5  // deep sleep interval

// Functions
void initGeneral();
void readAndSendData();
void goDeepSleep(uint32_t seconds);

#endif