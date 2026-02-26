#ifndef GENERAL_H
#define GENERAL_H
#include <Arduino.h>

void initGeneral();
void feedWatchdog(); 
void collectSample(); 
void uploadAverage();
#endif