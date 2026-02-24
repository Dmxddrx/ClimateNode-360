#ifndef SERVERCONFIG_H
#define SERVERCONFIG_H

#include "datatypes.h"
#include <stdint.h>
#include <stddef.h>
#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>

// ================= WIFI =================
#define WIFI_SSID "ENTGRA 2.5G"
#define WIFI_PASS "Entgra@110"

// ================= INFLUXDB CLOUD =================
#define INFLUXDB_URL    "https://us-east-1-1.aws.cloud2.influxdata.com"
#define INFLUXDB_TOKEN  "ITVjF59iKk2ohIJcqf-H85OD-M_IG0lhCXXrLttbXsIspzBHnkPGFC61MDzavHEbQMaomygOAM68lpksk8H0Kw=="
#define INFLUXDB_ORG    "d241431ab76edfa9"      // Org ID, not name
#define INFLUXDB_BUCKET "ClimateNodeData"

// Declare InfluxDB client instance (uses Cloud certificate)
extern InfluxDBClient client;

// Network check
bool isNetworkAvailable();

// Upload function (takes array of SensorData)
bool uploadQueuedData(SensorData *dataArray, uint8_t count);

#endif