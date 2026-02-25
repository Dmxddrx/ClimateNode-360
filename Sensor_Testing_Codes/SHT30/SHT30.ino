#include <Wire.h>
#include "Adafruit_SHT31.h"

// Define your custom I2C pins
#define I2C_SDA 6
#define I2C_SCL 7

Adafruit_SHT31 sht30 = Adafruit_SHT31();

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10); 

  Serial.println("\n--- SHT30 Custom Pin Test ---");

  // Initialize I2C with your specific pins
  Wire.begin(I2C_SDA, I2C_SCL);

  // Initialize sensor at address 0x44
  if (!sht30.begin(0x44)) {
    Serial.println("SHT30 not found! Check wiring on GPIO 6/7.");
    while (1) delay(1);
  }

  Serial.println("SHT30 detected successfully!");
}

void loop() {
  float t = sht30.readTemperature();
  float h = sht30.readHumidity();

  if (!isnan(t) && !isnan(h)) {
    Serial.print("Temp: "); Serial.print(t, 2); Serial.print(" °C | ");
    Serial.print("Hum: "); Serial.print(h, 2); Serial.println(" %");
  } else {
    Serial.println("Error: Failed to read from sensor");
  }

  delay(2000);
}