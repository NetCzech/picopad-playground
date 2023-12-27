#include <Arduino.h>
#include "picopad.h"

// Virtual keyboard height, rows a cols
const int kbHeight = 130;
const int kbRows = 4;
const int kbCols = 10;

// Offset of the virtual keyboard on the Y axis
int kbOffsetY = 15;

// Dynamic virtual keyboard key width
int keyWidths[kbRows];

// Key offset
const int keyMargin = 2;

// Text fiel parameters
const int txtFldHeight = 117;   // 9 rows, 13px line width => 9 * 13 = 117 px)
const int txtFldMargin = 2;     // Key border
String currentText = "";        // The current text in the text field

// The coordinates of the selected letter at first launch
int startRow = 0;
int startCol = 0;

// Monitor SHIFT key status
bool actShift = false;

// The number of keys in the virtual keyboard row
const int keyCols[kbRows] = {10, 10, 10, 9};

// Character layout for a virtual keyboard with small characters
String smallLabel[kbRows][kbCols] = {
    {"1", "2", "3", "4", "5", "6", "7", "8", "9", "0"},
    {"q", "w", "e", "r", "t", "z", "u", "i", "o", "p"},
    {"a", "s", "d", "f", "g", "h", "j", "k", "l", "BACKSPACE"},
    {"SHIFT", "y", "x", "c", "v", "b", "n", "m", "SPACE", ""}
};

// Character layout for a virtual keyboard with upper characters
String upperLabel[kbRows][kbCols] = {
    {"1", "2", "3", "4", "5", "6", "7", "8", "9", "0"},
    {"Q", "W", "E", "R", "T", "Z", "U", "I", "O", "P"},
    {"A", "S", "D", "F", "G", "H", "J", "K", "L", "BACKSPACE"},
    {"SHIFT", "Y", "X", "C", "V", "B", "N", "M", "SPACE", ""}
};

// Set the width of the keys
void setKeyWidth() {
    for (int i = 0; i < kbRows; i++) {
        keyWidths[i] = (WIDTH / 10) - (keyMargin * 2);
    }
}

// Draw an up arrow (placeholder for the SHIFT key)
void drawUpArrow(int x, int y, int width, int height, u16 color) {
    int centerX = x + width / 2;
    int centerY = y + height / 2;
    DrawLine(centerX, centerY - 5, centerX, centerY + 5, color);
    DrawLine(centerX, centerY - 5, centerX - 5, centerY, color);
    DrawLine(centerX, centerY - 5, centerX + 5, centerY, color);
}

// Draw an left arrow (placeholder for the BACKSPACE key)
void drawLeftArrow(int x, int y, int width, int height, u16 color) {
    int centerX = x + width / 2;
    int centerY = y + height / 2;
    DrawLine(centerX - 5, centerY, centerX + 5, centerY, color);
    DrawLine(centerX - 5, centerY, centerX, centerY - 5, color);
    DrawLine(centerX - 5, centerY, centerX, centerY + 5, color);
}

// Toggle the state of the SHIFT key
void toggleShift() {
    actShift = !actShift;
}

// Draw virtual keyboard keys
void drawKey(int row, int col, int keyWidth) {
    // Choose the keyboard layout based on the state of the SHIFT key
    String label = actShift ? upperLabel[row][col] : smallLabel[row][col];
    // Calculation horizontal and vertical position
    int x = col * (keyWidths[row] + keyMargin * 2) + keyMargin;
    int y = HEIGHT - kbHeight + row * ((kbHeight / kbRows) - (keyMargin * 2)) + keyMargin + kbOffsetY;
    // Set the color of the selected key
    u16 fillColor = (row == startRow && col == startCol) ? COL_DKYELLOW : COL_BLACK;
    // Draw a key square
    DrawRect(x, y, keyWidth, (kbHeight / kbRows) - (keyMargin * 2), COL_WHITE);
    DrawFillRect(x + 1, y + 1, keyWidth - 2, (kbHeight / kbRows) - (keyMargin * 2) - 2, fillColor);
    // Calculate the position of the text
    int textX = x + (keyWidth / 2) - (DrawFontWidth / 2);
    int textY = y + ((kbHeight / kbRows) / 2) - (DrawFontHeight / 2);
    // Draw placeholders for the SHIFT and BACKSPACE keys
    if (label == "SHIFT") {
        drawUpArrow(x, y, keyWidth, (kbHeight / kbRows) - (keyMargin * 2), COL_WHITE);
    } else if (label == "BACKSPACE") {
        drawLeftArrow(x, y, keyWidth, (kbHeight / kbRows) - (keyMargin * 2), COL_WHITE);
    } else {
        // Set the label position of the SPACE key
        if (label == "SPACE") {
            textX -= 16;
        }
        DrawText(label.c_str(), textX, textY, COL_WHITE);
    }
}

// Draw virtual keyboard
void drawKeyboard() {
    int colIndex = 0;
    for (int row = 0; row < kbRows; row++) {
        for (int col = 0; col < keyCols[row]; col++) {
            // Set the layout of the virtual keyboard according to the state of the SHIFT key
            String label = actShift ? upperLabel[row][col] : smallLabel[row][col];
            int currentKeyWidth = keyWidths[row];
            // Calculate the width of the SPACE key
            if (strcmp(label.c_str(), "SPACE") == 0 && row == 3) {
                currentKeyWidth = keyWidths[row] * 2 + keyMargin * 2;
            }
            drawKey(row, colIndex, currentKeyWidth);
            if (strcmp(label.c_str(), "SPACE") == 0 && row == 3) {
                colIndex += 2;
            } else {
                colIndex++;
            }
        }
        colIndex = 0;
    }
}

// Draw the text field
void drawTextField() {
    // Define the coordinates and dimensions of the text field
    int x = txtFldMargin;
    int y = txtFldMargin;
    int width = WIDTH - 2 * txtFldMargin;
    int height = txtFldHeight;
    // Draw the text field and its border
    DrawRect(x, y, width, height, COL_WHITE);
    DrawFillRect(x + 1, y + 1, width - 2, height - 2, COL_BLACK);
    // Set the max. lines, displayed lines and the position of the text
    int maxLines = 9;
    int linesShown = 0;
    int startPos = 0;
    // Display text in text field
    while (linesShown < maxLines && startPos < currentText.length()) {
        int endPos = currentText.indexOf('\n', startPos);
        if (endPos == -1) {
            endPos = currentText.length();
        }
        String lineText = currentText.substring(startPos, endPos);
        DrawText(lineText.c_str(), x + 5, y + 5 + linesShown * 13, COL_WHITE);
        startPos = endPos + 1;
        linesShown++;
    }
}

void setup() {
    // Device initialization
    device_init();
    DrawClear();
    // Draw text field, set width of keys and draw keyboard
    drawTextField();
    setKeyWidth();
    drawKeyboard();
    // Device update
    DispUpdate();
    KeyFlush();
}

void loop() {
    KeyScan();

    // Check the status of the SHIFT key
    if (KeyPressed(KEY_A) && (actShift ? upperLabel[startRow][startCol] : smallLabel[startRow][startCol]) == "SHIFT") {
        toggleShift();
        drawKeyboard();
        DispUpdate();
        return;
    }

    // Set cursor movement
    if (KeyPressed(KEY_UP) && startRow > 0) {
        startRow--;
    } else if (KeyPressed(KEY_DOWN) && startRow < kbRows - 1) {
        startRow++;
        if (startRow == 3 && (startCol == 8 || startCol == 9)) {
            startCol = 8;
        }
    } else if (KeyPressed(KEY_LEFT) && startCol > 0) {
        startCol--;
    } else if (KeyPressed(KEY_RIGHT) && startCol < keyCols[startRow] - 1) {
        startCol++;
    }

    // Set character insertion function
    if (KeyPressed(KEY_A)) {
        String selectedKey = actShift ? upperLabel[startRow][startCol] : smallLabel[startRow][startCol];

        if (selectedKey == "BACKSPACE") {
            if (currentText.length() > 0) {
                currentText.remove(currentText.length() - 1);
            }
        } else if (selectedKey == "SPACE") {
            currentText += " ";
        } else {
            currentText += selectedKey;
        }
        
        drawTextField();
        DispUpdate();
        delay(200);
    }

    // Set the cursor to the next line
    if (KeyPressed(KEY_B)) {
        currentText += "\n";
        drawTextField();
        DispUpdate();
    }

    drawKeyboard();
    DispUpdate();

    delay(110);

    // Reset to bootloader
    if (KeyPressed(KEY_Y))
        reset_to_boot_loader();
}
