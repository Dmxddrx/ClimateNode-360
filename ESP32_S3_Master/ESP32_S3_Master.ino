#include "general.h"
#include "espnow.h"
#include "sdcard.h"
#include "serverconfig.h"

void setup() {
  initGeneral();
  initSD();
  initESPNow();

  waitForSlaves(5000);   // wait 5 sec for C3 data

  saveBufferedDataToSD();

  if (isTimeToUpload()) {
    uploadBatch();
  }

  goDeepSleep(60);
}

void loop() {}