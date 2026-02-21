#include "am1001.h"

#define TEMP_PIN 0
#define HUM_PIN  1

void initSensors() {
  pinMode(TEMP_PIN, INPUT);
  pinMode(HUM_PIN, INPUT);
}

float readTemperatureRaw() {
  int adc = analogRead(TEMP_PIN);
  float voltage = adc * 3.3 / 4095.0;
  return voltage * 100.0;
}

int16_t readTemperature() {
  return (int16_t)(readTemperatureRaw() * 100);
}

int16_t readHumidity() {
  int adc = analogRead(HUM_PIN);
  float voltage = adc * 3.3 / 4095.0;
  return (int16_t)(voltage * 10000);
}