#include "SHT30.h"
#include <Wire.h>
#include <Adafruit_SHT31.h>

#define I2C_SDA 6
#define I2C_SCL 7

Adafruit_SHT31 sht30 = Adafruit_SHT31();

void initSHT30() {  
    sht30.begin(0x44);
}

int16_t readTemperature() {
    float t = sht30.readTemperature();
    return isnan(t) ? 0 : (int16_t)(t * 100);
}

int16_t readHumidity() {
    float h = sht30.readHumidity();
    return isnan(h) ? 0 : (int16_t)(h * 100);
}