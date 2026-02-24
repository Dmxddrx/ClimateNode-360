#include "general.h"
#include "am1001.h"
#include "gp2y10.h"
#include "espnow.h"
#include "debugmode.h"
#include <esp_sleep.h>

void initGeneral() {
    Serial.begin(115200);
    delay(200);
    DEBUG_MODE = true;

    DEBUG_PRINTLN("Node Booting...");
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

    // Debug output: sensor values + send result
    DEBUG_PRINTF("Sent -> Node: %d, Temp: %.2f, Hum: %.2f, Dust: %.2f\n",
                data.nodeId,
                data.temperature,
                data.humidity,
                data.dust);
}

void goDeepSleep(uint32_t seconds) {
    DEBUG_PRINTF("Going to deep sleep for %d seconds...\n", seconds);
    //Serial.flush();  // wait until all prints are transmitted
    //esp_sleep_enable_timer_wakeup((uint64_t)seconds * 1000000ULL);
    //esp_deep_sleep_start();
}