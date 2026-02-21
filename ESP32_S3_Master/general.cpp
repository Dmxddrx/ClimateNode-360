#include "general.h"
#include <esp_sleep.h>

void initGeneral() {
    Serial.begin(115200);
    Serial.println("S3 Master Booting...");
}

void goDeepSleep(uint32_t seconds) {
    Serial.printf("Going to deep sleep for %d seconds...\n", seconds);
    esp_sleep_enable_timer_wakeup((uint64_t)seconds * 1000000ULL);
    esp_deep_sleep_start();
}