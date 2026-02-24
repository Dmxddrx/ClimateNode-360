#ifndef SERVERCONFIG_H
#define SERVERCONFIG_H

#include "datatypes.h"
#include <stdint.h>
#include <stddef.h>

// ================= WIFI =================
#define WIFI_SSID "ENTGRA 2.5G"
#define WIFI_PASS "Entgra@110"

// ================= INFLUXDB CLOUD =================
#define INFLUXDB_URL "https://us-east-1-1.aws.cloud2.influxdata.com/api/v2/write?org=DMX&bucket=ClimateNode_360&precision=s"
#define INFLUXDB_TOKEN "XEv_RQ7JVhHuBt61WLM3cADQxkzFnCENXm8-PAhG20e3zSPzxuYkzjh-8gvukZEBKoVlbj4eFKXhsFm_aDDlLQ=="

// Network check
bool isNetworkAvailable();

// Upload function (takes array of SensorData)
bool uploadQueuedData(SensorData *dataArray, uint8_t count);

#endif