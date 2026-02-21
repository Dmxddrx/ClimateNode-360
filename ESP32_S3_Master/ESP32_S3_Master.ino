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
    initDustSensor();
    initSD();
    initESPNow();

    // 1. Read local sensors
    SensorData localData;
    localData.nodeId = 0; // Master node
    localData.temperature = readTemperature();
    localData.humidity = readHumidity();
    localData.dust = readDust();

    storeLocalReading(localData);

    // 2. Wait for slaves
    waitForSlaves(8000); // 8 sec safe window

    // 3. Save all data to SD
    saveBufferedDataToSD();

    // 4. Upload batch if needed
    if(isTimeToUpload()){
        uploadBatch();
    }

    // 5. Go to deep sleep
    goDeepSleep(SAMPLE_INTERVAL_SEC);
}

void loop(){}