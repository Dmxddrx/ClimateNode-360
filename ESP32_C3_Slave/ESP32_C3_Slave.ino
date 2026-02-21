#include "general.h"
#include "am1001.h"
#include "gp2y10.h"
#include "espnow.h"
#include "datatypes.h"

void setup() {

  initGeneral();
  initSensors();
  initESPNow();

  SensorData data;

  data.nodeId = NODE_ID;
  data.temperature = readTemperature();
  data.humidity = readHumidity();
  data.dust = readDust();

  sendToMaster(data);

  delay(200);

  goDeepSleep(SAMPLE_INTERVAL_SEC);
}

void loop(){}