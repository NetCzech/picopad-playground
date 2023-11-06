#include "Arduino.h"
#include "picopad.h"
#include <WiFi.h>

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  delay(100);
  Serial.println("WiFi Scanner application starting...");

  // Initialize the device and display
  device_init();
  DrawClear();
  DispUpdate();
  KeyFlush();
}

void loop() {
  // Clear display
  DrawClear();

  // Sets the current font to 8x8.
  SelFont8x8();

  Serial.println("Scanning WiFi networks...");
  int numNetworks = WiFi.scanNetworks();

  if (numNetworks == 0) {
    Serial.println("No WiFi networks found");
    DrawText("No WiFi networks found", 10, 60, COL_WHITE);
  } else {
    Serial.println("WiFi networks found:");
    for (int i = 0; i < min(4, numNetworks); i++) {
      
      // Variables for SSID
      char ssid[30];
      strncpy(ssid, WiFi.SSID(i), sizeof(ssid) - 1);
      ssid[sizeof(ssid) - 1] = '\0';
      
      // Variables for RSSI
      char rssi[5];
      itoa(WiFi.RSSI(i), rssi, 10);
      
      // Variables for channel
      char channel[5];
      snprintf(channel, sizeof(channel), "%d", WiFi.channel(i));

      Serial.print(ssid);
      Serial.print(" - Signal strength: ");
      Serial.println(WiFi.RSSI(i));
    
      // Show the values on the display
      DrawText("SSID:", 5, 5 + i * 60, COL_WHITE);
      DrawText(ssid, 50, 5 + i * 60, COL_WHITE);
      DrawText("RSSI:", 5, 20 + i * 60, COL_WHITE);
      DrawText(rssi, 50, 20 + i * 60, COL_WHITE);
      DrawText("dBm", 80, 20 + i * 60, COL_WHITE);
      DrawText("CHANNEL:", 5, 35 + i * 60, COL_WHITE);
      DrawText(channel, 75, 35 + i * 60, COL_WHITE);
    }
  }

  // Updates the display
  DispUpdate();

  // Repeat scan after xy ms
  delay(5000);
}
