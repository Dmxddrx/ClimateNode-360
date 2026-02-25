#include "gp2y10.h"
#include <Arduino.h>

// GPIO pin assignment for your C3
#define DUST_LED 3
#define DUST_PIN 4

// Constants for Sharp GP2Y10
const float VOC = 0.6;          // No-dust offset voltage (0.6V typical)
const float SENSITIVITY = 0.17; // Sensitivity (V per 100ug/m3)

void initDustSensor() {
    pinMode(DUST_LED, OUTPUT);
    pinMode(DUST_PIN, INPUT);
    digitalWrite(DUST_LED, HIGH); // LED off initially (active LOW circuit)
}

int16_t readDust() {
    // 1. Precise timing for measurement pulse
    // Note: RISC-V C3 handles microseconds timing very accurately
    digitalWrite(DUST_LED, LOW);       // Turn LED on
    delayMicroseconds(280);            // Wait 0.28ms

    int adc = analogRead(DUST_PIN);    // Measure at exactly 0.28ms

    delayMicroseconds(40);             // Remaining pulse width (total 0.32ms)
    digitalWrite(DUST_LED, HIGH);      // Turn LED off
    delayMicroseconds(9680);           // Off time (total cycle 10ms)

    // 2. Convert ADC to Voltage
    // C3 ADC is 12-bit (0-4095) by default
    float voltage = adc * 3.3 / 4095.0;

    // 3. Convert Voltage to Dust Density (ug/m3)
    float dustDensity = (voltage - VOC) * (100.0 / SENSITIVITY);

    // 4. Cleanup values
    if (dustDensity < 0) dustDensity = 0; 
    
    // 5. Return scaled x10 for compatibility with general.cpp
    return (int16_t)(dustDensity * 10); 
}