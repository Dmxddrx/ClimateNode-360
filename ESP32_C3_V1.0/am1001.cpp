#include "am1001.h"
#include <Arduino.h>
#include <math.h> 

// Changed for C3 Compatibility: 
// GPIO 6/7 are often reserved for Flash on C3. 
// Using GPIO 0 and 1 which are ADC1 enabled.
#define TEMP_PIN 0
#define HUM_PIN  1

// Circuit Constants
const float VCC = 3.3;
const float R_FIXED = 6200.0; 
const float ADC_MAX = 4095.0;

// NTC Constants
const float NTC_NOMINAL = 10000.0; 
const float TEMP_NOMINAL = 25.0;   
const float B_COEFFICIENT = 3950.0; 

void initSensors() {
    // Note: On C3, analogRead handles pinMode internally, 
    // but keeping this for consistency.
    pinMode(TEMP_PIN, INPUT);
    pinMode(HUM_PIN, INPUT);
}

int16_t readTemperature() {
    int adc = analogRead(TEMP_PIN);
    if (adc == 0) return -27300; 

    // 1. Calculate Voltage
    float voltage = (adc / ADC_MAX) * VCC;

    // 2. Calculate NTC Resistance
    float r_ntc = R_FIXED * (VCC / voltage - 1.0);

    // 3. Steinhart-Hart Equation
    float steinhart;
    steinhart = r_ntc / NTC_NOMINAL;          
    steinhart = log(steinhart);               
    steinhart /= B_COEFFICIENT;               
    steinhart += 1.0 / (TEMP_NOMINAL + 273.15); 
    steinhart = 1.0 / steinhart;              
    steinhart -= 273.15;                      

    return (int16_t)(steinhart * 100);
}

int16_t readHumidity() {
    int adc = analogRead(HUM_PIN);
    float voltage = (adc / ADC_MAX) * VCC;
    
    // AM1001 Standard Formula
    float humidity = (voltage - 0.6) * 47.619; 
    
    // Your specific calibration offset
    humidity -= 8.46; 

    if (humidity > 100.0) humidity = 100.0;
    if (humidity < 0.0) humidity = 0.0;

    return (int16_t)(humidity * 100);
}