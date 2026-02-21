#include "sdcard.h"
#include <SPI.h>
#include <SD.h>
#include "espnow.h"
#include "debugmode.h"

#define CS_PIN 10

void initSD() {
    if(!SD.begin(CS_PIN)){
        DEBUG_PRINTLN("SD init failed!");
    } else {
        DEBUG_PRINTLN("SD initialized");
    }
}

void saveBufferedDataToSD() {
    if(receivedCount == 0) return;

    File file = SD.open("/log.csv", FILE_APPEND);
    if(!file){
        DEBUG_PRINTLN("Failed to open file!");
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
    DEBUG_PRINTF("%d records saved to SD\n", receivedCount);
    receivedCount = 0;
}