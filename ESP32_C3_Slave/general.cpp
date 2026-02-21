#include "general.h"
#include <esp_sleep.h>

void initGeneral() {
  Serial.begin(115200);
}

void goDeepSleep(uint32_t seconds) {
  esp_sleep_enable_timer_wakeup((uint64_t)seconds * 1000000ULL);
  esp_deep_sleep_start();
}