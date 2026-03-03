#include "ADS1115_Dust.h"
#include <Adafruit_ADS1X15.h>
#include <Wire.h>

#define DUST_LED 4

Adafruit_ADS1115 ads;

void initDustSensor() {
    pinMode(DUST_LED, OUTPUT);
    digitalWrite(DUST_LED, HIGH); 
    ads.begin(0x48);
    ads.setGain(GAIN_ONE); 
}

int16_t readDust() {
    digitalWrite(DUST_LED, LOW); 
    delayMicroseconds(1000); //minimum is 280 micro seconds

    int16_t adc0 = ads.readADC_SingleEnded(3);

    delayMicroseconds(40);
    digitalWrite(DUST_LED, HIGH); 
    delayMicroseconds(9680);

    float voltage = adc0 * 0.125 / 1000.0;
    float dustDensity = (voltage - 0.6) * (100.0 / 0.17);
    if (dustDensity < 0) dustDensity = 0;

    return (int16_t)(dustDensity * 10);
}