// ****************************************************************************
//
//                        Main code for Wi-Fi Scanner
//
// ****************************************************************************

#include <Arduino.h>
#include "include.h"

char ch;

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  delay(100);
  Serial.println("WiFi Scanner application starting...");

  // Initialize the device and display
  device_init();
  DrawClear();
  // Set graphic scanner as default
  graphScanner();
  DispUpdate();
  KeyFlush();
}

void loop() {
  while (true)
	{
		ch = KeyGet();
    switch (ch)
		{
		// Set the A key
		case KEY_A:
			graphScanner();
      break;
    
		// Set the B key
		case KEY_B:
		 	textScanner();
      break;
    
    // Set the Y key
		case KEY_Y:
		 	reset_to_boot_loader();
		 	break;
    }
    delay(10);
  }
}