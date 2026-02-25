// Function to read sensors and add to the total
void collectSample() {
    float currentTemp = readTemperature();
    float currentHum = readHumidity();
    float currentDust = readDust();

    tempSum += currentTemp;
    delay(10);
    humSum += currentHum;
    delay(10);
    dustSum += currentDust;
    sampleCount++;

    // Added node_id and current values to the print statement
    // Note: nodeId 0 is used as default here
    DEBUG_PRINTF("[Node 0] Sample #%d captured -> Temp: %.2f, Hum: %.2f, Dust: %.2f\n", 
                 sampleCount, currentTemp / 100.0, currentHum / 100.0, currentDust / 10.0);
}

// Function to average and upload
void uploadAverage() {
    if (sampleCount == 0) return;

    DEBUG_PRINTLN("Processing 60s average and uploading...");

    SensorData localData;
    localData.nodeId = 0; // Ensure this matches your node's physical ID
    
    // Calculate average
    localData.temperature = (int32_t)(tempSum / sampleCount);
    localData.humidity    = (int32_t)(humSum / sampleCount);
    localData.dust        = (int32_t)(dustSum / sampleCount);
    localData.timestamp   = 0;

    // Enhanced debug print to show Node ID with averaged data
    DEBUG_PRINTF("[Node %d] Avg Data -> Temp: %.2f, Hum: %.2f, Dust: %.2f\n",
                  localData.nodeId,
                  localData.temperature / 100.0,
                  localData.humidity / 100.0,
                  localData.dust / 10.0);

    if (!isNetworkAvailable()) {
        DEBUG_PRINTLN("Network unavailable. Upload aborted.");
    } else {
        uint32_t now = getValidTimestamp(10000); 
        if (now != 0) {
            localData.timestamp = now;
            SensorData allData[1];
            allData[0] = localData;

            int retryCount = 0;
            const int maxRetries = 3;
            bool success = false;

            while (retryCount < maxRetries && !success) {
                success = uploadQueuedData(allData, 1);
                if (!success) {
                    retryCount++;
                    DEBUG_PRINTF("Retry %d/3...\n", retryCount);
                    delay(2000); 
                }
            }
            if (success) DEBUG_PRINTF("[Node %d] Upload successful to ClimateNode_360.\n", localData.nodeId);
        }

    // Reset accumulators for next 60s cycle
    tempSum = 0;
    humSum = 0;
    dustSum = 0;
    sampleCount = 0;
}