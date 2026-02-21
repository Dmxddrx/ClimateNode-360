#ifndef GENERAL_H
#define GENERAL_H

#include <Arduino.h>
#include "datatypes.h"

#define NODE_ID 1            // Change per node
#define SAMPLE_INTERVAL_SEC 60  // Sleep interval

void initGeneral();
void goDeepSleep(uint32_t seconds);

#endif