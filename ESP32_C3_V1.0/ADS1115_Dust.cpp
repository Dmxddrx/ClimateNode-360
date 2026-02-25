#include "ADS1115_Dust.h"
#include <Arduino.h>
#include <Adafruit_ADS1X15.h>

#define DUST_LED 4

Adafruit_ADS1115 ads;

const int samplingTime = 280; 
const int deltaTime = 40;
const int sleepTime = 9680;

const float VOC = 0.6;          
const float SENSITIVITY = 0.17; 

void initDustSensor() {
    pinMode(DUST_LED, OUTPUT);
    digitalWrite(DUST_LED, HIGH); 

    // Silent initialization
    ads.begin(0x48);
    ads.setGain(GAIN_ONE); 
}

int16_t readDust() {
    digitalWrite(DUST_LED, LOW); 
    delayMicroseconds(samplingTime);

    int16_t adc0 = ads.readADC_SingleEnded(0);
    
    delayMicroseconds(deltaTime);
    digitalWrite(DUST_LED, HIGH); 
    delayMicroseconds(sleepTime);

    float voltage = adc0 * 0.125 / 1000.0;
    float dustDensity = (voltage - VOC) * (100.0 / SENSITIVITY);
    
    if (dustDensity < 0) dustDensity = 0;

    return (int16_t)(dustDensity * 10); 
}