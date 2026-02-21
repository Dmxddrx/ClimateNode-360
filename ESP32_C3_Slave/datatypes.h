#ifndef DATATYPES_H
#define DATATYPES_H

#include <Arduino.h>

// Packed struct for ESP-NOW transmission
struct __attribute__((packed)) SensorData {
    uint8_t nodeId;       // Node ID
    int16_t temperature;  // x100 (25.34°C = 2534)
    int16_t humidity;     // x100
    int16_t dust;         // x10
};

#endif