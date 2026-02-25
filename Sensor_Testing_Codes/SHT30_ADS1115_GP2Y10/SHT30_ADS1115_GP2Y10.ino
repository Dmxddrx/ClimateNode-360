#include <Wire.h>
#include <Adafruit_SHT31.h>
#include <Adafruit_ADS1X15.h>

// Pin Definitions
#define I2C_SDA 6
#define I2C_SCL 7
#define DUST_LED 4

// Sensor Objects
Adafruit_SHT31 sht30 = Adafruit_SHT31();
Adafruit_ADS1115 ads; // Default address is 0x48

// Dust Sensor Timing (GP2Y10 specs)
const int samplingTime = 280; 
const int deltaTime = 40;
const int sleepTime = 9680;

void setup() {
  Serial.begin(115200);
  pinMode(DUST_LED, OUTPUT);
  digitalWrite(DUST_LED, HIGH); // Off (Active Low)

  // Initialize I2C on specific ESP32-C3 pins
  Wire.begin(I2C_SDA, I2C_SCL);

  // Initialize SHT30
  if (!sht30.begin(0x44)) {
    Serial.println("SHT30 not found!");
  }

  // Initialize ADS1115
  if (!ads.begin(0x48)) {
    Serial.println("ADS1115 not found!");
  }
  
  // Set Gain for ADS1115 (1x gain = +/- 4.096V)
  ads.setGain(GAIN_ONE); 

  Serial.println("Sensor Node Initialized...");
}

void loop() {
  // 1. Read SHT30 (Temp/Humidity)
  float t = sht30.readTemperature();
  float h = sht30.readHumidity();

  // 2. Read GP2Y10 via ADS1115 A0
  digitalWrite(DUST_LED, LOW); // Turn on LED
  delayMicroseconds(samplingTime);

  int16_t adc0 = ads.readADC_SingleEnded(0);
  
  delayMicroseconds(deltaTime);
  digitalWrite(DUST_LED, HIGH); // Turn off LED
  delayMicroseconds(sleepTime);

  // Convert ADC to Voltage
  // GAIN_ONE results in 0.125mV per bit
  float voltage = adc0 * 0.125 / 1000.0;

  // 3. Print Results
  Serial.println("--- Reading ---");
  Serial.print("Temp: "); Serial.print(t); Serial.println(" C");
  Serial.print("Hum:  "); Serial.print(h); Serial.println(" %");
  Serial.print("Dust Voltage: "); Serial.print(voltage); Serial.println(" V");
  
  delay(2000); 
}