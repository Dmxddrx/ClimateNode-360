#ifndef GENERAL_H
#define GENERAL_H

#include <Arduino.h>
#include "datatypes.h"

// Interval for master deep sleep
#define SAMPLE_INTERVAL_SEC 60

// Initialize serial, debug, etc.
void initGeneral();

// Main logic routine called from setup()
void generalRun();

// Go to deep sleep for given seconds
void goDeepSleep(uint32_t seconds);

#endif