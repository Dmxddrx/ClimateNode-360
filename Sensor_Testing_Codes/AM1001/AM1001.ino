/*
 * AM1001 Raw ADC Test for ESP32-S3
 * Reads pins 6 and 7 directly
 */

const int tempPin = 7;
const int humPin = 6;

void setup() {
  Serial.begin(115200);

  // Sets the ADC to 12-bit (0 to 4095)
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db); // Ensures full 0-3.3V range

  Serial.println("Reading Raw ADC Values...");
  Serial.println("Humidity_Raw(6)  Temperature_Raw(7)");
  Serial.println("------------------------------------");
}

void loop() {
  int rawHum = analogRead(humPin);
  int rawTemp = analogRead(tempPin);

  // Print values separated by a tab for easy reading
  Serial.print(rawHum);
  Serial.print("\t\t");
  Serial.println(rawTemp);

  delay(500);  // Faster updates for real-time testing
}