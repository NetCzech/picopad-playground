// Includes libraries necessary for the code
#include "emulator/src/lib_drawtft.cpp"
#include "stdio.h"
#include <SFML/Graphics.hpp>

// Draw a segmented line with vertical dividers at the bottom of the screen
void drawXaxis() {
    int startX = 0;
    int endX = 320;
    int numSegments = 14;
    int segmentWidth = (endX - startX) / numSegments;

    for (int i = 0; i < numSegments; i++) {
        int segmentStartX = startX + i * segmentWidth;
        int segmentEndX = segmentStartX + segmentWidth;
        int segmentCenterX = (segmentStartX + segmentEndX) / 2;

        // Draw the segment
        DrawLine(segmentStartX, 200, segmentEndX, 200, COL_WHITE);

        // Draw the vertical divider
        DrawLine(segmentCenterX, 195, segmentCenterX, 200, COL_WHITE);

        // Draw the segment number
        DrawText(std::to_string(i + 1).c_str(), segmentCenterX - 3, 205, COL_WHITE);
    }
}

void sensor(bool connected) {

    // If the sensor is connected, display its readings
    if (connected) {
       	DrawClear(COL_BLACK);
        
        // Draw the header
        SelFont8x16();
        DrawText("Wi-Fi Scanner for Picopad", 65, 2, COL_YELLOW);
        DrawLine(0, 20, 320, 20, COL_WHITE);

        // Draw the signal strength rectangle
        DrawRect(25, 100, 14, 100, COL_GREEN);
        DrawRect(70, 160, 14, 40, COL_RED);
        DrawRect(93, 135, 14, 65, COL_ORANGE);
        DrawRect(202, 110, 14, 90, COL_RED);
        
        // Draw the Wi-Fi signal strength column numbers
        SelFont8x8();
        DrawText("1", 28, 85, COL_WHITE);
        DrawText("2", 73, 147, COL_WHITE);
        DrawText("3", 96, 122, COL_WHITE);
        DrawText("4", 205, 97, COL_WHITE);

        // Draw SSID
        DrawText("1: SSID no. 1", 5, 30, COL_WHITE);
        DrawText("2: SSID no. 2", 5, 45, COL_WHITE);
        DrawText("3: SSID no. 3", 5, 60, COL_WHITE);
        DrawText("4: SSID no. 4", 160, 30, COL_WHITE);
        // Draw X-axis
        drawXaxis();

        // Draw a footer
        SelFont8x8();
        DrawLine(0, 221, 320, 221, COL_WHITE);
        DrawText("X", 5, 230, COL_YELLOW);
        DrawText("scan", 30, 230, COL_WHITE);
        DrawText("B", 110, 230, COL_YELLOW);
        DrawText("list of Wi-Fi networks", 135, 230, COL_WHITE);
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
