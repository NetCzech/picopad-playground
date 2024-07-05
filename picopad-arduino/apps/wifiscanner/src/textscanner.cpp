// ****************************************************************************
//
//                    Main code for Text Wi-Fi Scanner
//
// ****************************************************************************

#include "Arduino.h"
#include "include.h"

// Create a character array
char ssid[30];
char rssi[10];
char channel[5];

// Function to determine the encryption of Wi-Fi networks
const char* encType(int thisType) {
  switch (thisType) {
    case ENC_TYPE_NONE:
      return "None";
    case ENC_TYPE_AUTO:
      return "Auto";
    case ENC_TYPE_WEP:
      return "WEP";
    case ENC_TYPE_TKIP:
      return "WPA/TKIP";
    case ENC_TYPE_CCMP:
      return "WPA2";
    default:
      return "Unknown";
  }
}

void textScanner() {

  char ch;

  while (true) {
    // Draw the header
    DrawClear();
    SelFont8x16();
    DrawText("Wi-Fi Scanner for Picopad", 65, 2, COL_YELLOW);
    DrawLine(0, 20, 320, 20, COL_WHITE);
    
    // Draw a footer
    SelFont8x8();
    DrawLine(0, 221, 320, 221, COL_WHITE);
    DrawText("A", 75, 230, COL_YELLOW);
    DrawText("channel spectrum", 105, 230, COL_WHITE);

    // Find available Wi-Fi networks
    int numNetworks = WiFi.scanNetworks();
    if (numNetworks == 0) {
    DrawText("No Wi-Fi networks found", 5, 5, COL_WHITE);
    } else {
      // Set the number of Wi-Fi networks displayed
      for (int i = 0; i < min(5, numNetworks); i++) {
        // Convert SSID to a string
        strncpy(ssid, WiFi.SSID(i), sizeof(ssid) - 1);
        ssid[sizeof(ssid) - 1] = '\0';

        // Convert rssi to a string
        snprintf(rssi, sizeof(rssi), "%d", WiFi.RSSI(i));

        // Convert channel to a string
        snprintf(channel, sizeof(channel), "%d", WiFi.channel(i));

        // Variable to determine the WiFi network security type
        const char* secType = encType(WiFi.encryptionType(i));

        // Show the values on the display
        SelFont8x8();
        DrawText(WiFi.SSID(i), 5, 38 + i * 35, COL_YELLOW);
        DrawText("Encryption:", 150, 38 + i * 35, COL_WHITE);
        DrawText(encType(WiFi.encryptionType(i)), 250, 38 + i * 35, COL_WHITE);
        DrawText("Channel:", 55, 53 + i * 35, COL_WHITE);
        DrawText(String(WiFi.channel(i)).c_str(), 125, 53 + i * 35, COL_WHITE);
        DrawText("RSSI:", 150, 53 + i * 35, COL_WHITE);
        DrawText(String(WiFi.RSSI(i)).c_str(), 250, 53 + i * 35, COL_WHITE);
        DrawText("dB", 280, 53 + i * 35, COL_WHITE);
      }
    }

    // Updates the display
    DispUpdate();

    // Repeat scan after 3 seconds
    delay(3000);

    ch = KeyGet();
    switch (ch) {
      // Set the A key
      case KEY_A:
          delay(10);
          graphScanner();
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
