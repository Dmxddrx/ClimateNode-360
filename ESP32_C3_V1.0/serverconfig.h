#ifndef SERVERCONFIG_H
#define SERVERCONFIG_H

#include "datatypes.h"
#include <stdint.h>
#include <stddef.h>
#include <WiFi.h>
#include <HTTPClient.h>

// ================= WIFI =================
/*define WIFI_SSID "ENTGRA 2.5G"
#define WIFI_PASS "Entgra@110"*/

/*#define WIFI_SSID "Dmx's Note20 Ultra"
#define WIFI_PASS "11111129"*/

#define WIFI_SSID "Dialog-0429"
#define WIFI_PASS "hasith2001" 

// ================= LOCAL PHP SERVER =================
#define SERVER_URL "http://192.168.8.147/ClimateNode_360/log_data.php"

bool isNetworkAvailable();
bool uploadQueuedData(SensorData *dataArray, uint8_t count);

#endif