// Includes libraries necessary for the code
#include "emulator/src/lib_drawtft.cpp"
#include "stdio.h"
#include <SFML/Graphics.hpp>
#include "laskakit.c"

void getDateTime() {
        DrawText("Datum: 23.05.2024", 20, 20, COL_WHITE);
        DrawText("Èas: 17:50", 220, 20, COL_WHITE);
}

// Function to get ID from JSON file
void getID() {
    DrawText("Jeseník", 0, 45, COL_ORANGE);
    DrawText("Dìèín", 0, 75, COL_ORANGE);
    DrawText("Rumburk", 0, 105, COL_ORANGE);
    DrawText("Bílina", 0, 140, COL_ORANGE);
    DrawText("Èeské Budìjovice", 0, 175, COL_ORANGE);
}

void getRGB() {
    DrawText("Mrholení: < 0.5 mm/h, 8 dBZ", 0, 60, COL_WHITE);
    DrawText("Déš: +/- 4 mm/h, 28 dBZ", 0, 90, COL_WHITE);
    DrawText("Mrholení: < 0.5 mm/h, 8 dBZ", 0, 120, COL_WHITE);
    DrawText("Déš: +/- 4 mm/h, 28 dBZ", 0, 155, COL_WHITE);
    DrawText("Silný déš, +/- 10 mm/h, 40 dBZ", 0, 190, COL_WHITE);    
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
    SelFontCz8x16();
    DrawLine(0, 220, 320, 220, COL_WHITE);
    DrawText("Další: Pg Dn", 5, 225, COL_YELLOW);
    DrawText("Pøedchozí: Pg Up", 185, 225, COL_YELLOW);
    DrawText("1/3", 135, 225, COL_YELLOW);
}

void sensor(bool connected) {

    // If the sensor is connected, display its readings
    if (connected) {
       	DrawClear(COL_BLACK);
        
        // Draw the header
        DrawClear(COL_BLACK);
        drawHeader();
        DrawImgRle(LaskakitImg_RLE, LaskakitImg_Pal, 250, 55, 60, 49);
        getID();
        getRGB();
        drawFooter();
        }

    // Updates the display with the drawn elements
    DispUpdate();
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
