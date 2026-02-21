#include "sdcard.h"
#include <SPI.h>
#include <SD.h>
#include "espnow.h"

#define CS_PIN 10

void initSD() {
  SD.begin(CS_PIN);
}

void saveBufferedDataToSD() {
  File file = SD.open("/log.csv", FILE_APPEND);

  for (int i = 0; i < receivedCount; i++) {
    file.printf("%d,%.2f,%.2f,%.2f\n",
      receivedData[i].nodeId,
      receivedData[i].temperature,
      receivedData[i].humidity,
      receivedData[i].dust);
  }

  file.close();
  receivedCount = 0;
}