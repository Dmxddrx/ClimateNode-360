#include "serverconfig.h"
#include <WiFi.h>
#include <HTTPClient.h>

#define WIFI_SSID "ssid"
#define WIFI_PASS "pass"
#define SERVER_URL "http://server/api/upload"

// RTC counter survives deep sleep
RTC_DATA_ATTR uint8_t uploadCounter = 0;

bool isTimeToUpload() {
    uploadCounter++;
    if(uploadCounter >= 15){
        uploadCounter = 0;
        return true;
    }
    return false;
}

void uploadBatch() {
    DEBUG_PRINTLN("Connecting to Wi-Fi...");
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while(WiFi.status() != WL_CONNECTED) delay(300);

    HTTPClient http;
    http.begin(SERVER_URL);
    http.addHeader("Content-Type", "application/json");

    String payload = "{\"status\":\"batch\"}";
    int httpCode = http.POST(payload);

    if(httpCode > 0){
        DEBUG_PRINTF("Upload success: %d\n", httpCode);
    } else {
        DEBUG_PRINTLN("Upload failed");
    }

    http.end();
    WiFi.disconnect(true);
}