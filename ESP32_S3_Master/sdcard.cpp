#include "sdcard.h"
#include <SPI.h>
#include <SD.h>
#include "espnow.h"

#define CS_PIN 10

void initSD() {
    if(!SD.begin(CS_PIN)){
        Serial.println("SD init failed!");
    } else {
        Serial.println("SD initialized");
    }
}

void saveBufferedDataToSD() {
    if(receivedCount == 0) return;

    File file = SD.open("/log.csv", FILE_APPEND);
    if(!file){
        Serial.println("Failed to open file!");
        return;
    }

    for (int i = 0; i < receivedCount; i++) {
        file.printf("%d,%.2f,%.2f,%.1f\n",
            receivedData[i].nodeId,
            receivedData[i].temperature / 100.0,
            receivedData[i].humidity / 100.0,
            receivedData[i].dust / 10.0);
    }
    file.close();
    Serial.printf("%d records saved to SD\n", receivedCount);
    receivedCount = 0;
}