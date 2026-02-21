#include "am1001.h"
#include <Arduino.h>

// GPIO pin assignment for S3/C3
#define TEMP_PIN 7
#define HUM_PIN  6

// Initialize sensor pins
void initSensors() {
    pinMode(TEMP_PIN, INPUT);
    pinMode(HUM_PIN, INPUT);
}

// Read temperature from sensor
int16_t readTemperature() {
    int adc = analogRead(TEMP_PIN);             // ADC raw value
    float voltage = adc * 3.3 / 4095.0;        // Convert to voltage
    return (int16_t)(voltage * 100);           // Scale x100
}

// Read humidity from sensor
int16_t readHumidity() {
    int adc = analogRead(HUM_PIN);
    float voltage = adc * 3.3 / 4095.0;
    return (int16_t)(voltage * 100);           // Scale x100
}