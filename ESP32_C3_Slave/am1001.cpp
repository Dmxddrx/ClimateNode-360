#include "am1001.h"

#define TEMP_PIN 0
#define HUM_PIN  1

void initSensors() {
  pinMode(TEMP_PIN, INPUT);
  pinMode(HUM_PIN, INPUT);
}

float readTemperature() {
  int adc = analogRead(TEMP_PIN);
  return (adc * 3.3 / 4095.0) * 100.0;
}

float readHumidity() {
  int adc = analogRead(HUM_PIN);
  return (adc * 3.3 / 4095.0) * 100.0;
}