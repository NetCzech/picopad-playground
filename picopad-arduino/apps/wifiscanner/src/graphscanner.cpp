// ****************************************************************************
//
//                    Main code for Graphics Wi-Fi Scanner
//
// ****************************************************************************

#include "Arduino.h"
#include "include.h"

static bool scanDone = false;

int numNetworks = 0;

// Set width, min. height and max. height of signal strenght column
int rectWidth = 14;
int minHeight = 0;
int maxHeight = 100;

// Function for drawing a rectangle
void drawSignalRect(int x, int y, int width, int height, int color) {
  DrawRect(x, y - height, width, height, color);
}

// Definition X-axis
void drawXaxis(int startX, int endX, int numLabelsX) {
  int yStep = (startX - endX) / numLabelsX;
  // Draw X-axis labels and line
  for (int i = 0; i < numLabelsX; i++) {
    int channelX = map(i + 1, 1, numLabelsX, 10, 310);
    DrawLine(channelX, startX + 2, channelX, startX - 2, COL_WHITE);
    // Draw the X-axis coordinates
    SelFont8x8();
    DrawText(String(i + 1).c_str(), channelX - 3, startX + 10, COL_WHITE);
  }
}

void graphScanner() {
  
  char ch;

  // Display on first startup
  // Draw the header
  DrawClear();
  SelFont8x16();
  DrawText("Wi-Fi Scanner for Picopad", 65, 2, COL_YELLOW);
  DrawLine(0, 20, 320, 20, COL_WHITE);
  
  // Draw X-axis
  drawXaxis(200, 5, 14);
  
  // Draw a footer
  SelFont8x8();
  DrawLine(0, 221, 320, 221, COL_WHITE);
  DrawText("X", 5, 230, COL_YELLOW);
  DrawText("scan", 30, 230, COL_WHITE);
  DrawText("B", 110, 230, COL_YELLOW);
  DrawText("list of Wi-Fi networks", 135, 230, COL_WHITE);
  
  // Display update
  DispUpdate();

  // Graphics scanner loop
  while (true) {
    ch = KeyGet();
    switch (ch) {
      // Set the X key
      case KEY_X:
          DrawClear();
          // Find available Wi-Fi networks
          numNetworks = WiFi.scanNetworks();
          if (numNetworks == 0) {
          DrawText("No Wi-Fi networks found", 5, 5, COL_WHITE);
          } else {
            // Set the number of Wi-Fi networks displayed
            for (int i = 0; i < min(5, numNetworks); i++) {
              // Draw rectangles of Wi-Fi signal strength
              int rectX = map(WiFi.channel(i), 1, 14, 4, 304);
              int rectY = 203;
              
              //Set the color of the rectangle according to the strength of the Wi-Fi signal
              int signalStrength = WiFi.RSSI(i);
              int rectColor;
              if (signalStrength >= -30) {
                rectColor = COL_GREEN;
              } else if (signalStrength >= -60) {
                rectColor = COL_YELLOW;
              } else if (signalStrength >= -70) {
                rectColor = COL_ORANGE;
              } else {
                rectColor = COL_RED;
              }
              // Calculate the height of the Wi-Fi signal strenght rectangle
              int barHeight = map(signalStrength, -90, -30, minHeight, maxHeight);
              
              // Draw a rectangle of Wi-Fi signal strength
              drawSignalRect(rectX, rectY, rectWidth, barHeight, rectColor);
              
              // Draw the Wi-Fi signal strength column numbers
              SelFont8x8();
              DrawText(String(i + 1).c_str(), rectX + 3, rectY - barHeight - 15, COL_WHITE);
              
              // Draw the header
              SelFont8x16();
              DrawText("Wi-Fi Scanner for Picopad", 65, 2, COL_YELLOW);
              DrawLine(0, 20, 320, 20, COL_WHITE);
              
              // Display SSID of detected Wi-Fi networks
              DrawText("1: ", 5, 30, COL_WHITE);
              DrawText(WiFi.SSID(0), 30, 30, COL_WHITE);
              DrawText("2: ", 5, 45, COL_WHITE);
              DrawText(WiFi.SSID(1), 30, 45, COL_WHITE);
              DrawText("3: ", 5, 60, COL_WHITE);
              DrawText(WiFi.SSID(2), 30, 60, COL_WHITE);
              DrawText("4: ", 160, 30, COL_WHITE);
              DrawText(WiFi.SSID(3), 185, 30, COL_WHITE);
              DrawText("5: ", 160, 45, COL_WHITE);
              DrawText(WiFi.SSID(4), 185, 45, COL_WHITE);
              
              // Draw X-axis
              drawXaxis(200, 5, 14);
              
              // Draw a footer
              SelFont8x8();
              DrawLine(0, 221, 320, 221, COL_WHITE);
              DrawText("X", 5, 230, COL_YELLOW);
              DrawText("scan", 30, 230, COL_WHITE);
              DrawText("B", 110, 230, COL_YELLOW);
              DrawText("list of Wi-Fi networks", 135, 230, COL_WHITE);
            }
          }

          // Display update
          DispUpdate();
          scanDone = true;
          break;

      // Set the B key
      case KEY_B:
          delay(10);
          textScanner();
          break;

      // Set the Y key
      case KEY_Y:
          delay(10);
          Serial.end();
          reset_to_boot_loader();
          break;
    }
  }
}
