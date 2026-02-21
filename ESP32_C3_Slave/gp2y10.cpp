#include <Arduino.h>

#define DUST_LED 3
#define DUST_PIN 2

int16_t readDust() {
  digitalWrite(DUST_LED, LOW);
  delayMicroseconds(280);

  int adc = analogRead(DUST_PIN);

  delayMicroseconds(40);
  digitalWrite(DUST_LED, HIGH);

  float voltage = adc * 3.3 / 4095.0;
  return (int16_t)(voltage * 1000); // scaled x10
}