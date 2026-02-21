#ifndef GENERAL_H
#define GENERAL_H

#include <Arduino.h>

#define SAMPLE_INTERVAL_SEC 60   // Master deep sleep interval

void initGeneral();
void goDeepSleep(uint32_t seconds);

#endif