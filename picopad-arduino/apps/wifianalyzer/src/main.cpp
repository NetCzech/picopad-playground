#include "Arduino.h"
#include "picopad.h"
#include <WiFi.h>

// String to display the scanner value
char ssid[30];
char rssi[10];
char channel[5];

const char* encType(int thisType) {
  switch (thisType) {
    case ENC_TYPE_NONE:
      return "None";
    case ENC_TYPE_AUTO:
      return "Auto";
    case ENC_TYPE_WEP:
      return "WEP";
    case ENC_TYPE_TKIP:
      return "WPA";
    case ENC_TYPE_CCMP:
      return "WPA2";
    default:
      return "Unknown";
  }
}

// Draw the signal strength bar
void drawBar(int x, int y, int width, int color) {
  DrawRect(x, y, width, 5, color);
}

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

  int numNetworks = WiFi.scanNetworks();

  if (numNetworks == 0) {
    DrawText("No WiFi networks found", 5, 5, COL_WHITE);
  } else {
    for (int i = 0; i < min(5, numNetworks); i++) {

      // Variable to determine the WiFi network security type
      const char* secType = encType(WiFi.encryptionType(i));

      // Convert SSID to a string
      strncpy(ssid, WiFi.SSID(i), sizeof(ssid) - 1);
      ssid[sizeof(ssid) - 1] = '\0';

      // Calculate the bar width based on signal strength
      int barWidth = map(WiFi.RSSI(i), -90, 0, 10, 320);

      // Convert RSSI to a string
      snprintf(rssi, sizeof(rssi), "%d dBm", WiFi.RSSI(i));

      // Convert channel to a string
      snprintf(channel, sizeof(channel), "%d", WiFi.channel(i));

      // Draw the signal strength bar
      drawBar(0, 15 + i * 45, barWidth, COL_GREEN);

      // Show the values on the display
      DrawText("SSID:", 5, 5 + i * 45, COL_WHITE);
      DrawText(ssid, 55, 5 + i * 45, COL_WHITE);
      DrawText("RSSI:", 5, 25 + i * 45, COL_WHITE);
      DrawText(rssi, 55, 25 + i * 45, COL_WHITE);
      DrawText("Channel:", 150, 25 + i * 45, COL_WHITE);
      DrawText(channel, 240, 25 + i * 45, COL_WHITE);
      DrawText("Security:", 150, 35 + i * 45, COL_WHITE);
      DrawText(secType, 240, 35 + i * 45, COL_WHITE);

    }
  }

  // Updates the display
  DispUpdate();

  // Repeat scan after xy ms
  delay(5000);

  // Press key Y for reset to bootloader
  if (KeyGet() == KEY_Y) {
    delay(10);
    Serial.end();
    reset_to_boot_loader();
  }
}
