#include <WiFi.h>

// Your network credentials
const char* ssid = "ENTGRA 2.5G";
const char* password = "Entgra@110";

void setup() {
    // Start serial communication
    Serial.begin(115200);
    delay(2000); // Give the C3 serial port time to initialize
    
    Serial.println("\n\n--- ESP32-C3 WiFi Diagnostic Test ---");

    // 1. Hard reset the WiFi hardware
    Serial.println("Resetting WiFi radio...");
    WiFi.disconnect(true);
    delay(1000); 

    // 2. Set to Station mode (Client)
    WiFi.mode(WIFI_STA);
    
    // Print the MAC Address (useful to check if your router is blocking it)
    Serial.print("ESP32-C3 MAC Address: ");
    Serial.println(WiFi.macAddress());

    // 3. Start connection
    Serial.printf("Attempting to connect to SSID: %s\n", ssid);
    WiFi.begin(ssid, password);

    // 4. Wait for connection with a 15-second timeout
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 30) {
        delay(500);
        Serial.print(".");
        attempts++;
    }
    Serial.println(); // New line

    // 5. Evaluate the result
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("SUCCESS! Connected to WiFi.");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
        Serial.print("Gateway: ");
        Serial.println(WiFi.gatewayIP());
        Serial.print("Signal Strength (RSSI): ");
        Serial.print(WiFi.RSSI());
        Serial.println(" dBm");
    } else {
        Serial.println("FAILED to connect!");
        Serial.printf("Final Error Status Code: %d\n", WiFi.status());
        
        // Print a helpful translation of the error code
        switch(WiFi.status()) {
            case 1: Serial.println("Diagnosis: Network not found (Out of range or router is 5GHz only)."); break;
            case 4: Serial.println("Diagnosis: Connection failed (Wrong password or MAC blocked by router)."); break;
            case 6: Serial.println("Diagnosis: Disconnected (Module is idle/stuck)."); break;
            default: Serial.println("Diagnosis: Unknown error."); break;
        }
    }
}

void loop() {
    // Just monitor the connection every 5 seconds to see if it drops
    if (WiFi.status() == WL_CONNECTED) {
        Serial.print("Network Stable. Ping RSSI: ");
        Serial.println(WiFi.RSSI());
    } else {
        Serial.println("WARNING: Network connection lost!");
    }
    delay(5000);
}