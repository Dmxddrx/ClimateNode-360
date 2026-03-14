#ifndef SERVERCONFIG_H
#define SERVERCONFIG_H

#include "datatypes.h"
#include <stdint.h>
#include <stddef.h>
#include <WiFi.h>
#include <HTTPClient.h>

// ================= WIFI LIST =================
struct WifiNetwork {
    const char* ssid;
    const char* pass;
};

static const WifiNetwork wifiList[] = {
    {"ENTGRA 2.5G", "Entgra@110"},        // Home
    {"Dmx's Note20 Ultra", "11111129"},   // Phone hotspot
    {"Dialog 4G 208", "Hasith2001"}       // Workshop
};

static const uint8_t WIFI_COUNT = sizeof(wifiList) / sizeof(wifiList[0]);

// ================= LOCAL PHP SERVER =================
#define SERVER_URL "http://192.168.8.147/ClimateNode_360/log_data.php"

bool isNetworkAvailable();
bool uploadQueuedData(SensorData *dataArray, uint8_t count);

#endif