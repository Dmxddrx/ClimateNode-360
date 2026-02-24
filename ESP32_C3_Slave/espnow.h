
#ifndef ESPNOW_H
#define ESPNOW_H

#include "datatypes.h"

void initESPNow();
void sendToMaster(SensorData &data);

#endif
