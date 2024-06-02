// ****************************************************************************
//
//                            Main code for mapa_cr
//
// ****************************************************************************

#include <Arduino.h>
#include "picopad.h"
#include "main.h"

#include "WiFi.h"

#include <HTTPClient.h>
#include <ArduinoJson.h>

DynamicJsonDocument doc(8192);

// Function to print WiFi status to the Serial port
void printWifiStatus() {
	Serial.print("SSID: ");
	Serial.println(WiFi.SSID());
	IPAddress localIo = WiFi.localIP();
	Serial.print("IP Address: ");
	Serial.println(localIo);
	long rssi = WiFi.RSSI();
	Serial.print("Signal strength (RSSI):");
	Serial.print(rssi);
	Serial.println(" dBm");
}

// Function to test the Wi-Fi connection
void testWiFi() {
  HTTPClient http;
  http.begin("http://www.google.com");
  int httpCode = http.GET();

  if (httpCode > 0) {
    Serial.println("Internet connection is OK");
  } else {
    Serial.print("Internet connection error, HTTP code: ");
    Serial.println(httpCode);
  }
  http.end();
}

// Function to load a JSON file
void loadJson() {
  HTTPClient http;
  http.begin("http://oracle-ams.kloboukuv.cloud/radarmapa/?chcu=posledni_v2.json");
  int httpCode = http.GET();

  if (httpCode == 200) {
    DeserializationError error = deserializeJson(doc, http.getString());

    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }
  } else {
    Serial.print("Error loading data from API, HTTP code: ");
    Serial.println(httpCode);
    return;
  }
  http.end();
}

// Function to get date and time from a JSON file
void getDateTime() {
  JsonObject utc_datum = doc["utc_datum"];
  int rok = utc_datum["rok"];
  int mesic = utc_datum["mesic"];
  int den = utc_datum["den"];
  int hodina = utc_datum["hodina"];
  int minuta = utc_datum["minuta"];

  snprintf(Date, sizeof(Date), "Datum: %02d.%02d.%d", den, mesic, rok);
  snprintf(Time, sizeof(Time), "Èas: %02d:%02d", hodina, minuta);
  DrawText(Date, 20, 20, COL_WHITE);
  DrawText(Time, 220, 20, COL_WHITE);
}

// Function to get ID from JSON file
void getID() {
  JsonArray seznam = doc["seznam"];
  int startIndex = currentPage * itemsPerPage / 2;
  int endIndex = min(startIndex + itemsPerPage / 2, (int) seznam.size());
  // Set the ID display position
  int yPos = 45;
  
  for(int i = startIndex; i < endIndex; i++) {
    JsonObject obj = seznam[i];
    int id = obj["id"];
    const char* cityName = id >= 0 && id < sizeof(cityNames)/sizeof(cityNames[0]) ? cityNames[id] : "Neznámé ID";
    
    // Adjust so that the values do not overlap the footer
    if (yPos + 35 > 250) break;
    snprintf(ID, sizeof(ID), "%s", cityName);
    DrawText(ID, 0, yPos, COL_ORANGE);
    // Set the next ID display position
    yPos += 35;
  }
}

// Function to get RGB color from JSON files
void getRGB() {
  JsonArray seznam = doc["seznam"];
  int startIndex = currentPage * itemsPerPage / 2;
  int endIndex = min(startIndex + itemsPerPage / 2, (int) seznam.size());
  // Set the RGB color display position
  int yPos = 60;
  
  for(int i = startIndex; i < endIndex; i++) {
    JsonObject obj = seznam[i];
    int r = obj["r"];
    int g = obj["g"];
    int b = obj["b"];
    
    // Adjust so that the values do not overlap the footer
    if (yPos + 35 > 250) break;
    // Assign an RGB color description
    const char* description = getRGBDescription(r, g, b);
    snprintf(RGB, sizeof(RGB), "%s", description);
    
    // If you want to display only RGB values, comment out lines 122 and 123 and uncomment the line 126
    //snprintf(RGB, sizeof(RGB), "R:%d G:%d B:%d", r, g, b);
    
    DrawText(RGB, 0, yPos, COL_WHITE);
    // Set the next RGB color display position
    yPos += 35;
  }
}

// Function to display the header
void drawHeader() {
  SelFontCz8x16();
  DrawText("LÁSKAKIT.CZ - Mapa Èeské republiky", 20, 3, COL_YELLOW);
  getDateTime();
  DrawLine(0, 40, 320, 40, COL_WHITE);
}

// Function to display footer
void drawFooter() {
  DrawLine(0, 220, 320, 220, COL_WHITE);
  DrawText("Další: Pg Dn", 5, 225, COL_YELLOW);
  DrawText("Pøedchozí: Pg Up", 185, 225, COL_YELLOW);
  snprintf(numberPage, sizeof(numberPage), "%d/%d", currentPage + 1, maxPages);
  DrawText(numberPage, 135, 225, COL_YELLOW);
}

void setup() {
  Serial.begin(115200);
    
  device_init();
  DrawClear();

  Serial.printf("Connecting to %s ", ssid);
	unsigned long wifi_start = millis();
	WiFi.begin(ssid, password);
	while (WiFi.status() != WL_CONNECTED) {
    if (KeyGet() == KEY_Y) reset_to_boot_loader();
		delay(500);
		Serial.print(".");
	}
	printWifiStatus();

  // If necessary, uncomment the Wi-Fi connection test
  //testWiFi();

  loadJson();
  drawHeader();
  DrawImgRle(LaskakitImg_RLE, LaskakitImg_Pal, 250, 55, 60, 49);
  getID();
  getRGB();
  drawFooter();
  DispUpdate();
}

void loop() {
  char key = KeyGet();

  // Find total item from JSON file
  int totalItems = doc["seznam"].size();
  // Calculate the number of pages
  maxPages = ceil((float)totalItems / (itemsPerPage / 2));

  Serial.print("Total items: ");
  Serial.println(totalItems);
  Serial.print("Max pages: ");
  Serial.println(maxPages);

  // Show the values on the display
  DrawClear();
  drawHeader();
  DrawImgRle(LaskakitImg_RLE, LaskakitImg_Pal, 250, 55, 60, 49);
  getID();
  getRGB();
  drawFooter();
  DispUpdate();

  if (key == KEY_DOWN) {
    if (currentPage < maxPages - 1) {
      currentPage++;
      Serial.print(F("Page down, actual page: "));
      Serial.println(currentPage);
    }
  } else if (key == KEY_UP) {
    if (currentPage > 0) {
      currentPage--;
      Serial.print(F("Page up, actual page: "));
      Serial.println(currentPage);
    }
  } else if (key == KEY_Y) {
    reset_to_boot_loader();
  }

  delay(100);
}