#include "general.h"
#include "am1001.h"
#include "gp2y10.h"
#include "espnow.h"
#include "sdcard.h"
#include "serverconfig.h"
#include "datatypes.h"

void setup() {

  initGeneral();
  initSensors();
  initSD();
  initESPNow();

  // --------- 1. Read S3 Local Sensors ----------
  SensorData localData;
  localData.nodeId = 0;   // Master ID = 0
  localData.temperature = readTemperature();
  localData.humidity = readHumidity();
  localData.dust = readDust();

  storeLocalReading(localData);

  // --------- 2. Wait for C3 Slaves ----------
  waitForSlaves(5000);

  // --------- 3. Save Everything ----------
  saveBufferedDataToSD();

  // --------- 4. Upload every 15 cycles ----------
  if (isTimeToUpload()) {
    uploadBatch();
  }

  // --------- 5. Sleep ----------
  goDeepSleep(60);
}

void loop() {}