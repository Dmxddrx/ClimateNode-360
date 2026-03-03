#ifndef LED_H
#define LED_H

#include <Arduino.h>

// ESP32-C3 LED pin
#define STATUS_LED 10

// Initialize LED
void initLED();

// Pattern triggers
void ledWifiConnect();     // WiFi connecting
void ledServerResponse();  // Server response OK
void ledSampleTaken();     // Sample captured
void ledUpload();          // Upload in progress

// Non-blocking update (call in loop)
void ledUpdate();

#endif