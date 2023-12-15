// Includes libraries necessary for the code
#include "emulator/src/lib_drawtft.cpp"
#include <SFML/Graphics.hpp>
#include "stdio.h"
#include "connector.h"
#include "logo.h"

void sensor(bool connected) {

    // If the sensor is connected, display its readings
    if (connected) {
       	DrawClear(COL_BLACK);

        // Set system font
        SelFont6x8();

        //Draw connector image and description
        DrawImgRle(ConnectorImg_RLE, ConnectorImg_Pal, 160, 10, 142, 78);
        DrawText("PIN 1", 152, 70, COL_GRAY);

        // Set system font
        pDrawFont = FontBold8x8;
        DrawFontHeight = 8;

        // Draw logo and description
        DrawImgRle(LogoImg_RLE, LogoImg_Pal, 40, 20, 80, 34);
        DrawText("CONNECTOR", 51, 55, COL_WHITE);

        // Set system font
        SelFont6x8();

        // Draw PIN and description
        DrawText("1", 15, 80, COL_WHITE);
        DrawText("GND", 40, 80, COL_GRAY);
        DrawText("2", 15, 92, COL_WHITE);
        DrawText("GND", 40, 92, COL_GRAY);
        DrawText("3", 15, 104, COL_WHITE);
        DrawText("3V3", 40, 104, COL_DKRED);
        DrawText("4", 15, 116, COL_WHITE);
        DrawText("VBAT", 40, 116, COL_DKRED);
        DrawText("5", 15, 128, COL_WHITE);
        DrawText("GP28", 40, 128, COL_DKGREEN);
        DrawText("ADC2", 75, 128, COL_JUNGLE);
        DrawText("6", 15, 140, COL_WHITE);
        DrawText("ADC_VREF", 40, 140, COL_JUNGLE);
        DrawText("7", 15, 152, COL_WHITE);
        DrawText("GP27", 40, 152, COL_DKGREEN);
        DrawText("ADC1", 75, 152, COL_JUNGLE);
        DrawText("I2C1 SCL", 135, 152, COL_LTBLUE);
        DrawText("8", 15, 164, COL_WHITE);
        DrawText("GND", 40, 164, COL_GRAY);
        DrawText("AGND", 75, 164, COL_JUNGLE);
        DrawText("9", 15, 176, COL_WHITE);
        DrawText("GP0", 40, 176, COL_DKGREEN);
        DrawText("SPIO RX", 75, 176, COL_PINK);
        DrawText("I2C0 SDA", 135, 176, COL_LTBLUE);
        DrawText("UART0 TX", 195, 176, COL_DKMAGENTA);
        DrawText("10", 15, 188, COL_WHITE);
        DrawText("GP26", 40, 188, COL_DKGREEN);
        DrawText("ADC0", 75, 188, COL_JUNGLE);
        DrawText("I2C1 SDA", 135, 188, COL_LTBLUE);
        DrawText("11", 15, 200, COL_WHITE);
        DrawText("GP1", 40, 200, COL_DKGREEN);
        DrawText("SPI0 CSn", 75, 200, COL_PINK);
        DrawText("I2C0 SCL", 135, 200, COL_LTBLUE);
        DrawText("UART0 RX", 195, 200, COL_DKMAGENTA);
        DrawText("12", 15, 212, COL_WHITE);
        DrawText("GP14", 40, 212, COL_DKGREEN);
        DrawText("SPI1 SCK", 75, 212, COL_PINK);
        DrawText("I2C1 SDA", 135, 212, COL_LTBLUE);
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
