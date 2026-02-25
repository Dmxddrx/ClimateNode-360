#ifndef GENERAL_H
#define GENERAL_H

#include <Arduino.h>
#include "datatypes.h"

// New intervals for averaging logic
#define SAMPLE_INTERVAL_MS 5000   // 5 seconds
#define UPLOAD_INTERVAL_MS 60000  // 60 seconds

// Initialize serial, debug, etc.
void initGeneral();

// New functions for averaging logic
void collectSample(); 
void uploadAverage();

// Go to deep sleep for given seconds
void goDeepSleep(uint32_t seconds);

#endif