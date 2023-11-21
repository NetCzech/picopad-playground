#include "Arduino.h"
#include "picopad.h"
#include <WiFi.h>

// String to display the scanner value
// char ssid[30];
// char rssi[10];

int rectWidth = 14;
int minHeight = 0;  // Minimum height of the bar
int maxHeight = 240;  // Maximum height of the bar

// Define colors for signal strength
#define COL_EXCELLENT COL_DKGREEN
#define COL_GOOD      COL_DKYELLOW
#define COL_FAIR      COL_ORANGE
#define COL_WEAK      COL_DKRED

// const char* encType(int thisType) {
//   switch (thisType) {
//     case ENC_TYPE_NONE:
//       return "None";
//     case ENC_TYPE_AUTO:
//       return "Auto";
//     case ENC_TYPE_WEP:
//       return "WEP";
//     case ENC_TYPE_TKIP:
//       return "WPA";
//     case ENC_TYPE_CCMP:
//       return "WPA2";
//     default:
//       return "Unknown";
//   }
// }

// Draw the signal strength rectangle
void drawSignalRect(int x, int y, int width, int height, int color) {
  DrawRect(x, y - height, width, height, color);
}

// Draw the Y-axis labels and line
void drawAxisY(int startY, int endY, int numLabelsY) {
  int yStep = (startY - endY) / numLabelsY;

  // Draw Y-axis labels and line
  for (int i = 0; i < numLabelsY; i++) {
    // Adjust this value based on your needs
    int channelX = map(i + 1, 1, numLabelsY, 10, 310);
    DrawLine(channelX, startY + 2, channelX, startY - 2, COL_WHITE);
    DrawText(String(i + 1).c_str(), channelX - 3, startY + 10, COL_WHITE);
  }
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
    for (int i = 0; i < min(10, numNetworks); i++) {
      // Calculate position for each rectangle based on channel
      int rectX = map(WiFi.channel(i), 1, 14, 4, 304);
      int rectY = 223;

      // Convert SSID to a string
      // strncpy(ssid, WiFi.SSID(i), sizeof(ssid) - 1);
      // ssid[sizeof(ssid) - 1] = '\0';

      // Calculate the rectangle color based on signal strength
      int signalStrength = WiFi.RSSI(i);
      int rectColor;
      if (signalStrength >= -30) {
        rectColor = COL_EXCELLENT;
      } else if (signalStrength >= -60) {
        rectColor = COL_GOOD;
      } else if (signalStrength >= -70) {
        rectColor = COL_FAIR;
      } else {
        rectColor = COL_WEAK;
      }

      // Calculate the height of the bar based on signal strength
      int barHeight = map(signalStrength, -90, -30, minHeight, maxHeight);

      // Draw the signal strength rectangle
      drawSignalRect(rectX, rectY, rectWidth, barHeight, rectColor);

      DrawText("WiFi Scanner for Picopad", 65, 5, COL_WHITE);

      // Draw Y-axis
      drawAxisY(220, 5, 14);

      // Show the SSID on the display
      // DrawText(ssid, rectX + rectWidth / 2, rectY + 10, COL_WHITE);
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
