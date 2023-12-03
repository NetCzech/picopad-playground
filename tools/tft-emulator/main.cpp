// Includes libraries necessary for the code
#include "emulator/src/lib_drawtft.cpp"
#include "stdio.h"
#include <SFML/Graphics.hpp>

char ssid[20];
const char* demoSSID[] = {"SSID no. 1", "SSID no. 2", "SSID no. 3", "SSID no. 4", "SSID no. 5"};

char rssi[15];
const char* demoRSSI[] = {"-91", "-62", "-88", "-45", "-67"};

char encType[10];
const char* demoENC[] = {"WPA2", "WPA2", "WPA2", "WPA2", "WPA2"};

char channel[5];
const char* demoCHANNEL[] = {"1", "4", "8", "3", "6"};

void sensor(bool connected) {

    // If the sensor is connected, display its readings
    if (connected) {
       	DrawClear(COL_BLACK);
        
        // Draw the header
        DrawClear(COL_BLACK);
        SelFont8x16();
        DrawText("Wi-Fi Scanner for Picopad", 65, 2, COL_YELLOW);
        DrawLine(0, 20, 320, 20, COL_WHITE);

        // Vytvoření cyklu pro zobrazení demonstračních Wi-Fi sítí
        for (int i = 0; i < std::min(5, static_cast<int>(sizeof(demoSSID) / sizeof(demoSSID[0]))); i++) {
            // Získání hodnot pro danou demonstrační síť
            strncpy(ssid, demoSSID[i], sizeof(ssid) - 1);
            ssid[sizeof(ssid) - 1] = '\0';

        // Convert rssi to a string
        snprintf(rssi, sizeof(rssi), "%s dB", demoRSSI[i]);

        // Convert rssi to a string
        snprintf(encType, sizeof(encType), "%s", demoENC[i]);

        // Convert rssi to a string
        snprintf(channel, sizeof(channel), "%s", demoCHANNEL[i]);

        // Show the values on the display
        SelFont8x8();
        DrawText(ssid, 5, 38 + i * 35, COL_YELLOW);
        DrawText("Encryption:", 150, 38 + i * 35, COL_WHITE);
        DrawText(encType, 250, 38 + i * 35, COL_WHITE);
        DrawText("Channel:", 55, 53 + i * 35, COL_WHITE);
        DrawText(channel, 125, 53 + i * 35, COL_WHITE);
        DrawText("RSSI:", 150, 53 + i * 35, COL_WHITE);
        DrawText(rssi, 250, 53 + i * 35, COL_WHITE);

        // Draw a footer
        SelFont8x8();
        DrawLine(0, 221, 320, 221, COL_WHITE);
        DrawText("A", 75, 230, COL_YELLOW);
        DrawText("channel spectrum", 105, 230, COL_WHITE);
        }

    // Updates the display with the drawn elements
    DispUpdate();
    }
}

int main() {
    // Creates a frame with given display width and height
    FrameTex.create(DispWidth, DispHeight);
    FrameSprite.setTexture(FrameTex);

    // Presumes sensor connection is established by default
    bool connected = true;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            // Check if the window was closed, if yes terminate the main loop
            if (event.type == sf::Event::Closed)
                window.close();

            // If a key was pressed
            if (event.type == sf::Event::KeyPressed) {
/*                 // If the 'C' key was pressed
                if (event.key.code == sf::Keyboard::C) {
                    // Toggle the connected variable
                    connected = !connected;
                } */
                if (event.key.code == sf::Keyboard::Y) {
                    window.close();
                    return 0;
                }
            }
        }

        sensor(connected);

        // Clears the window, draws the frame sprite, and displays the window
        window.clear();
        window.draw(FrameSprite);
        window.display();
    }

    return 0;
}
