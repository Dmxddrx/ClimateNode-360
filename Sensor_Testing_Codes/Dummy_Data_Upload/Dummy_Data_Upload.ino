#include <WiFi.h>
#include <HTTPClient.h>

// ================= CONFIGURATION =================
#define WIFI_SSID "ENTGRA 2.5G"
#define WIFI_PASS "Entgra@110"
#define SERVER_URL "http://192.168.8.147/ClimateNode_360/log_data.php"

// ================= TEST VALUES =================
const int test_node_id = 5;
const float test_temp  = 10.4;
const float test_hum   = 20.0;
const float test_dust  = 30.0;

void setup() {
    Serial.begin(115200);
    delay(2000);
    
    Serial.println("\n--- ESP32-C3 SERVER TEST START ---");
    
    // 1. Connect to WiFi
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    Serial.print("Connecting to WiFi");
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    
    Serial.println("\nWiFi Connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    // 2. Prepare and Send Data
    sendTestData();
}

void loop() {
    // We only send once in setup for testing. 
    // Reset the ESP32 to send again.
}

void sendTestData() {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        
        Serial.println("Attempting to send data...");
        http.begin(SERVER_URL);
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");

        // Construct the POST string matching your PHP variables
        // node_id, temp, hum, dust
        String httpRequestData = "node_id=" + String(test_node_id) +
                                 "&temp="    + String(test_temp) +
                                 "&hum="     + String(test_hum) +
                                 "&dust="    + String(test_dust);

        Serial.print("POST Data: ");
        Serial.println(httpRequestData);

        int httpResponseCode = http.POST(httpRequestData);

        if (httpResponseCode > 0) {
            Serial.print("HTTP Response code: ");
            Serial.println(httpResponseCode);
            
            // Print the response from the server (very helpful for debugging PHP)
            String payload = http.getString();
            Serial.println("Server says: " + payload);
        } else {
            Serial.print("Error code: ");
            Serial.println(httpResponseCode);
            Serial.println("Check if Server IP is correct and XAMPP is running.");
        }
        
        http.end();
    } else {
        Serial.println("WiFi Disconnected");
    }
}