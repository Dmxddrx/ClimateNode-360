#include <WiFi.h>
#include <esp_wifi.h>

uint8_t mac[6];

void setup() {
  
  Serial.begin(115200);
  while(!Serial);   // Wait for serial connection
  delay(1000);
  Serial.println("Serial Ready");


  delay(1000);

  WiFi.mode(WIFI_STA);
  esp_wifi_get_mac(WIFI_IF_STA, mac);
}

void loop() {

  Serial.println("================================");

  Serial.printf("MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
                mac[0], mac[1], mac[2],
                mac[3], mac[4], mac[5]);

  Serial.printf("Array format: {0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X}\n",
                mac[0], mac[1], mac[2],
                mac[3], mac[4], mac[5]);

  Serial.println("================================\n");

  delay(2000);   // print every 2 seconds
}