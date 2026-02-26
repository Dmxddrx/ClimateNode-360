#include "serverconfig.h"
#include "debugmode.h"
#include <esp_task_wdt.h>

bool isNetworkAvailable() {
    if (WiFi.status() == WL_CONNECTED) return true;

    DEBUG_PRINTLN("Activating WiFi Stack...");
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    uint32_t start = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start < 10000) {
        delay(500);
        DEBUG_PRINT(".");
        esp_task_wdt_reset(); 
    }

    if (WiFi.status() == WL_CONNECTED) {
        DEBUG_PRINTF("\nConnected! IP: %s\n", WiFi.localIP().toString().c_str());
        return true;
    } else {
        DEBUG_PRINTF("\nWiFi Failed. Status: %d\n", WiFi.status());
        return false;
    }
}

bool uploadQueuedData(SensorData *dataArray, uint8_t count) {
    if (count == 0 || dataArray == nullptr) return false;
    if (!isNetworkAvailable()) return false;

    bool success = true;
    HTTPClient http;

    for (uint8_t i = 0; i < count; i++) {
        http.begin(SERVER_URL);
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");

        // SQL Column Names: node_id, temp, hum, dust
        String httpRequestData = "node_id=" + String(dataArray[i].nodeId) +
                                 "&temp="    + String(dataArray[i].temperature / 100.0) +
                                 "&hum="     + String(dataArray[i].humidity / 100.0) +
                                 "&dust="    + String(dataArray[i].dust / 10.0);

        int httpResponseCode = http.POST(httpRequestData);

        if (httpResponseCode > 0) {
            DEBUG_PRINTF("Server Response: %d\n", httpResponseCode);
        } else {
            DEBUG_PRINTF("Post Failed: %s\n", http.errorToString(httpResponseCode).c_str());
            success = false;
        }
        http.end();
    }

    WiFi.disconnect(true);  
    return success;
}