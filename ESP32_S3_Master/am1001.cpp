#include "am1001.h"
#include <Arduino.h>
#include <math.h> // Required for log()

#define TEMP_PIN 7
#define HUM_PIN  6

// Circuit Constants
const float VCC = 3.3;
const float R_FIXED = 6200.0; // 6.2k resistor
const float ADC_MAX = 4095.0;

// NTC Constants
const float NTC_NOMINAL = 10000.0; // 10k at 25C
const float TEMP_NOMINAL = 25.0;   // 25C
const float B_COEFFICIENT = 3950.0; // Typical Beta for 10k NTC (adjust if needed)

void initSensors() {
    pinMode(TEMP_PIN, INPUT);
    pinMode(HUM_PIN, INPUT);
}

int16_t readTemperature() {
    int adc = analogRead(TEMP_PIN);
    if (adc == 0) return -27300; // Error: pin grounded

    // 1. Calculate Voltage at the ADC pin
    float voltage = (adc / ADC_MAX) * VCC;

    // 2. Calculate NTC Resistance
    // Formula for divider where R_FIXED is to GND: Vout = Vcc * (R_fixed / (R_ntc + R_fixed))
    // Rearranged: R_ntc = R_fixed * (Vcc / Vout - 1)
    float r_ntc = R_FIXED * (VCC / voltage - 1.0);

    // 3. Steinhart-Hart Equation (Beta Formula)
    float steinhart;
    steinhart = r_ntc / NTC_NOMINAL;          // (R/Ro)
    steinhart = log(steinhart);               // ln(R/Ro)
    steinhart /= B_COEFFICIENT;               // 1/B * ln(R/Ro)
    steinhart += 1.0 / (TEMP_NOMINAL + 273.15); // + (1/To)
    steinhart = 1.0 / steinhart;              // Invert
    steinhart -= 273.15;                      // Convert Kelvin to Celsius

    // Return Celsius scaled by 100 for general.cpp
    return (int16_t)(steinhart * 100);
}

int16_t readHumidity() {
    int adc = analogRead(HUM_PIN);
    float voltage = (adc / ADC_MAX) * VCC;
    
    // For AM1001, humidity is usually 0.1V to 1.0V for 0-100% RH
    // If your specific module outputs 0-3V, change 1.0 to 3.0 below
    float humidity = (voltage / 1.0) * 100.0; 
    
    if (humidity > 100.0) humidity = 100.0;
    if (humidity < 0.0) humidity = 0.0;

    return (int16_t)(humidity * 100);
}