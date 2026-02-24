#include "general.h"
#include "am1001.h"
#include "gp2y10.h"
#include "espnow.h"

#include <esp_sleep.h>

void initGeneral() {
    Serial.begin(115200);
    Serial.println("Node Booting...");
}

void readAndSendData() {
    // Read all sensors
    SensorData data;
    data.nodeId = NODE_ID;

    // Temperature + Humidity (AM1001)
    data.temperature = readTemperature();
    delay(50);
    data.humidity = readHumidity();
    delay(50);

    // Dust (GP2Y10)
    data.dust = readDust();
    delay(50);

    // Send via ESP-NOW
    sendToMaster(data);

    // Optional: print for debug
    Serial.printf("Sent -> Node: %d, Temp: %.2f, Hum: %.2f, Dust: %.2f\n",
                  data.nodeId, data.temperature, data.humidity, data.dust);
}

void goDeepSleep(uint32_t seconds) {
    Serial.printf("Going to deep sleep for %d seconds...\n", seconds);
    esp_sleep_enable_timer_wakeup((uint64_t)seconds * 1000000ULL);
    esp_deep_sleep_start();
}