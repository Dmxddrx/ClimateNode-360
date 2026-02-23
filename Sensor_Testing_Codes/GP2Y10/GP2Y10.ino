/*
 * GP2Y10 Dust Sensor for ESP32-S3
 * Vout Pin: GPIO 4 (Analog)
 * LED Pin:  GPIO 5 (Digital)
 */

const int measurePin = 4; 
const int ledPin = 5;    

// Timing (from Sharp datasheet)
const int samplingTime = 280; // Pulse width (microseconds)
const int deltaTime = 40;     // Measurement window
const int sleepTime = 9680;   // Wait time for 10ms cycle

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH); // Start with LED OFF (Active Low)

  // Configure ADC for ESP32-S3
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db); // Range up to ~3.1V

  Serial.println("GP2Y10 Sensor Test Initialized...");
}

void loop() {
  // 1. Trigger Pulse
  digitalWrite(ledPin, LOW); // Turn on LED
  delayMicroseconds(samplingTime);

  // 2. Sample Voltage
  int rawValue = analogRead(measurePin);

  // 3. Complete Pulse Cycle
  delayMicroseconds(deltaTime);
  digitalWrite(ledPin, HIGH); // Turn off LED
  delayMicroseconds(sleepTime);

  // 4. Calculations
  // Assuming 3.3V reference for ESP32-S3
  float voltage = rawValue * (3.3 / 4095.0);
  
  // Standard Sharp conversion: 
  // Dust Density (mg/m3) = 0.17 * voltage - 0.1
  float dustDensity = (0.17 * voltage) - 0.1;

  // Clean up display for low values
  if (dustDensity < 0) dustDensity = 0;

  // 5. Output
  Serial.print("Raw: ");
  Serial.print(rawValue);
  Serial.print(" \tVoltage: ");
  Serial.print(voltage, 3);
  Serial.print("V \tDensity: ");
  Serial.print(dustDensity, 4);
  Serial.println(" mg/m^3");

  delay(1000); // Wait 1 second between updates
}