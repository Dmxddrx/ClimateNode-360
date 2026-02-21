#include "sdcard.h"
#include <SPI.h>
#include <SD.h>

#define CS_PIN 10   // Chip Select pin for SD card

// Initialize SD card
void initSD() {
    SD.begin(CS_PIN);   // Returns false if initialization fails, but we skip printing
}

// Save an array of SensorData to SD
void saveBufferedDataToSD(SensorData *data, uint8_t count) {
    if (count == 0 || data == nullptr) return;

    File file = SD.open("/log.csv", FILE_APPEND);
    if (!file) return;   // Fail silently

    for (uint8_t i = 0; i < count; i++) {
        // Format: nodeId,temperature,humidity,dust
        // Temperature/humidity scaled x100, dust scaled x10
        file.printf("%d,%d,%d,%d\n",
            data[i].nodeId,
            data[i].temperature,
            data[i].humidity,
            data[i].dust
        );
    }

    file.close();
}