#include <WiFi.h>

void setup() {
    Serial.begin(115200);
    delay(2000); // Wait for serial to open

    Serial.println("\n--- ESP32-C3 WiFi Radar Scanner ---");

    // Set WiFi to station mode and disconnect from any saved networks
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    Serial.println("Scanning for nearby networks... (This takes about 5 seconds)");

    // Perform the scan
    int numNetworks = WiFi.scanNetworks();

    if (numNetworks == 0) {
        Serial.println("CRITICAL FAILURE: No WiFi networks found anywhere.");
        Serial.println("Possible causes: Bad USB cable, broken antenna, or metal enclosure blocking signal.");
    } else {
        Serial.printf("SUCCESS: %d networks found!\n", numNetworks);
        Serial.println("-----------------------------------------");
        for (int i = 0; i < numNetworks; ++i) {
            // Print SSID and Signal Strength
            Serial.printf("%2d: %-20s | Signal: %d dBm\n", i + 1, WiFi.SSID(i).c_str(), WiFi.RSSI(i));
            delay(10);
        }
        Serial.println("-----------------------------------------");
    }
}

void loop() {
    // Do nothing
}