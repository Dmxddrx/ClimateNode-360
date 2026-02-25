#include <Wire.h>

// ESP32-C3 Super Mini / Nebula Default Pins
#define I2C_SDA 6
#define I2C_SCL 7

void setup() {
  // Use a high baud rate for the ESP32-C3's native USB
  Serial.begin(115200);
  while (!Serial); // Wait for Serial Monitor to open
  
  Serial.println("\n--- I2C Address Scanner ---");
   
  // Initialize I2C with the specific C3 pins
  Wire.begin(I2C_SDA, I2C_SCL);
}

void loop() {
  byte error, address;
  int nDevices = 0;

  Serial.println("Scanning...");

  for (address = 1; address < 127; address++) {
    // The i2c_scanner uses the return value of
    // the Write.endTransmission to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address < 16) Serial.print("0");
      Serial.print(address, HEX);
      Serial.println("  !");

      nDevices++;
    } 
    else if (error == 4) {
      Serial.print("Unknown error at address 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
    }
  }

  if (nDevices == 0) {
    Serial.println("No I2C devices found\n");
  } else {
    Serial.println("Scan complete.\n");
  }

  delay(5000); // Wait 5 seconds before scanning again
}
