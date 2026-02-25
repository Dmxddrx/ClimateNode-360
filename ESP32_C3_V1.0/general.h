#ifndef GENERAL_H
#define GENERAL_H

#include <Arduino.h>
#include "datatypes.h"

// No changes needed here; intervals remain identical
#define SAMPLE_INTERVAL_MS 5000   // 5 seconds
#define UPLOAD_INTERVAL_MS 60000  // 60 seconds

void initGeneral();
void collectSample(); 
void uploadAverage();
void goDeepSleep(uint32_t seconds);

#endif