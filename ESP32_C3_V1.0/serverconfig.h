#ifndef SERVERCONFIG_H
#define SERVERCONFIG_H

#include "datatypes.h"
#include <stdint.h>
#include <stddef.h>
#include <WiFi.h>

// ================= WIFI =================
#define WIFI_SSID "ENTGRA 2.5G"
#define WIFI_PASS "Entgra@110"

//#define WIFI_SSID "Dmx's Note20 Ultra"
//#define WIFI_PASS "11111129"

// ================= LOCAL SERVER =================
// Replace with your laptop's IP address
#define SERVER_URL "http://192.168.8.147/ClimateNode_360/log_data.php" //Wifi Router Entgra 2.5G
//#define SERVER_URL "http://192.168.8.2/ClimateNode_360/log_data.php" //Dmx's Note20 Ultra


// Network check
bool isNetworkAvailable();

// Upload function (takes array of SensorData) - words kept same for compatibility
bool uploadQueuedData(SensorData *dataArray, uint8_t count);

#endif