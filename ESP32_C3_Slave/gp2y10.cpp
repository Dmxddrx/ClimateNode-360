#include "gp2y10.h"
#include <Arduino.h>

// GPIO pin assignment
#define DUST_LED 3
#define DUST_PIN 4

// Initialize the dust sensor pins
void initDustSensor() {
    pinMode(DUST_LED, OUTPUT);
    pinMode(DUST_PIN, INPUT);
    digitalWrite(DUST_LED, HIGH); // LED off initially
}

// Read dust sensor value
int16_t readDust() {
    digitalWrite(DUST_LED, LOW);       // Turn LED on for measurement
    delayMicroseconds(280);            // Wait for sensor response

    int adc = analogRead(DUST_PIN);    // Read ADC value

    delayMicroseconds(40);             // Hold time
    digitalWrite(DUST_LED, HIGH);      // Turn LED off

    float voltage = adc * 3.3 / 4095.0;  // Convert to voltage
    return (int16_t)(voltage * 10);      // Scale x10
}