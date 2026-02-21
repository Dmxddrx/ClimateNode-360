#ifndef ESPNOW_H
#define ESPNOW_H

#include "general.h"

void initESPNow();
void sendToMaster(SensorData &data);

#endif