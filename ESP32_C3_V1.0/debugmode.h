#ifndef DEBUGMODE_H
#define DEBUGMODE_H

#include <Arduino.h>

// If DEBUG_MODE is defined, Serial prints will be enabled
// Otherwise, all debug prints are disabled
extern bool DEBUG_MODE;

#define DEBUG_PRINT(x)     do { if(DEBUG_MODE) Serial.print(x); } while(0)
#define DEBUG_PRINTLN(x)   do { if(DEBUG_MODE) Serial.println(x); } while(0)
#define DEBUG_PRINTF(fmt, ...) do { if(DEBUG_MODE) Serial.printf(fmt, ##__VA_ARGS__); } while(0)

#endif