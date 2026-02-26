#ifndef GENERAL_H
#define GENERAL_H

#include <Arduino.h>
#include "datatypes.h"

#define SAMPLE_INTERVAL_MS 5000   
#define UPLOAD_INTERVAL_MS 60000  

void initGeneral();
void feedWatchdog(); // Added fail-safe function
void collectSample(); 
void uploadAverage();

#endif