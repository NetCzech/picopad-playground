// ****************************************************************************
//
//                            Main code for keyboard
//
// ****************************************************************************

#include <Arduino.h>
#include "picopad.h"

// Virtual keyboard height, rows a cols
const int kbHeight = 130;
const int kbRows = 5;
const int kbCols = 10;

// Offset of the virtual keyboard on the Y axis
const int kbOffsetY = 15;

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

// Monitor SHIFT, CHAR and SMILE keys status
bool actShift = false;
bool actChar = false;
bool actSmile = false;

// The number of keys in the virtual keyboard row
const int keyCols[kbRows] = {5, 10, 10, 10, 9};

// Character layout for a virtual keyboard with small characters
String smallLabel[kbRows][kbCols] = {
    {"CHAR", "SMILE", "LABEL3", "LABEL4", "LABEL5"},
    {"1", "2", "3", "4", "5", "6", "7", "8", "9", "0"},
    {"q", "w", "e", "r", "t", "z", "u", "i", "o", "p"},
    {"a", "s", "d", "f", "g", "h", "j", "k", "l", "BACKSPACE"},
    {"SHIFT", "y", "x", "c", "v", "b", "n", "m", "SPACE", ""}
};

// Character layout for a virtual keyboard with upper characters
String upperLabel[kbRows][kbCols] = {
     {"CHAR", "SMILE", "LABEL3", "LABEL4", "LABEL5"},
    {"1", "2", "3", "4", "5", "6", "7", "8", "9", "0"},
    {"Q", "W", "E", "R", "T", "Z", "U", "I", "O", "P"},
    {"A", "S", "D", "F", "G", "H", "J", "K", "L", "BACKSPACE"},
    {"SHIFT", "Y", "X", "C", "V", "B", "N", "M", "SPACE", ""}
};

// Character layout for a virtual keyboard with special characters
String charLabel[kbRows][kbCols] = {
     {"CHAR", "SMILE", "LABEL3", "LABEL4", "LABEL5"},
    {"1", "2", "3", "4", "5", "6", "7", "8", "9", "0"},
    {"@", "#", "$", "_", "&", "-", "+", "(", ")", "/"},
    {"*", "'", ":", ";", "!", "?", "|", "^", "", "BACKSPACE"},
    {"{", "}", "~", "%", "<", ">", ",", ".", "SPACE", ""}
};

// Character layout for a virtual keyboard with smile characters
String smileLabel[kbRows][kbCols] = {
     {"CHAR", "SMILE", "LABEL3", "LABEL4", "LABEL5"},
    {":)", ":))", ";)", ":D", "xD", ":'D", ":P", "8)", ">:>", ":|"},
    {":(", ":'(", ":o", "8O", ":?", ":x", ":S", "*-)", "o_o", ">_<"},
    {":*", ":oO", "=)(=", ">]", "|)", ":a", ";>)", "<3", "@=", "BACKSPACE"},
    {"", "", "", "", "", "", "", "", "SPACE", ""}
};

// Toggle the state of the SHIFT key
void toggleShift() {
    actShift = !actShift;
}

// Toggle the state of the CHAR key
void toggleChar() {
    actChar = !actChar;
}

//Toggle the state of the SMILE key
void toggleSmile() {
    actSmile = !actSmile;
}

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

// Draw virtual keyboard keys
void drawKey(int row, int col, int keyWidth) {
    // Choose the keyboard layout based on the state of the SHIFT, CHAR or SMILE key
    String label;
    if (actChar) {
        label = charLabel[row][col];
    } else if (actSmile) {
        label = smileLabel[row][col];
    } else if (actShift) {
        label = upperLabel[row][col];
    } else {
        label = smallLabel[row][col];
    }
    
    int x, y, width;
    
    // Calculation horizontal and vertical position
    if (row == 0) {
        width = 2 * (WIDTH / kbCols) - (2 * keyMargin);
        x = col * (2 * (keyWidths[row] + keyMargin * 2)) + keyMargin;
    } else if (row == 4 && label == "SPACE") {
        width = 2 * (WIDTH / kbCols) - (2 * keyMargin);
        x = 8 * (keyWidths[row] + keyMargin * 2) + keyMargin;
    } else {
        width = keyWidths[row];
        x = col * (width + keyMargin * 2) + keyMargin;
    }

    y = HEIGHT - kbHeight + row * ((kbHeight / kbRows) - (keyMargin * 2)) + keyMargin + kbOffsetY;
    
    // Set the color for CHAR, SMILE, LABEL3, LABEL4 and LABEL5 key
    u16 fillColor;
    bool specialKey = (row == 0) && (label == "CHAR" || label == "SMILE" || label == "LABEL3" || label == "LABEL4" || label == "LABEL5");

    if (specialKey && !(row == startRow && col == startCol)) {
        fillColor = COL_MDKGRAY;
    } else {
        // Set the color for the selected key
        fillColor = (row == startRow && col == startCol) ? COL_DKYELLOW : COL_BLACK;
    }

    // Calculate the position of the text
    int textX = x + (width / 2) - (DrawFontWidth / 2 * label.length());
    int textY = y + ((kbHeight / kbRows) / 2) - (DrawFontHeight / 2);

    // Draw a key square
    DrawRect(x, y, width, (kbHeight / kbRows) - (keyMargin * 2), COL_WHITE);
    DrawFillRect(x + 1, y + 1, width - 2, (kbHeight / kbRows) - (keyMargin * 2) - 2, fillColor);

    // Draw placeholders for the SHIFT and BACKSPACE keys
    if (label == "SHIFT") {
        drawUpArrow(x, y, width, (kbHeight / kbRows) - (keyMargin * 2), COL_WHITE);
    } else if (label == "BACKSPACE") {
        drawLeftArrow(x, y, width, (kbHeight / kbRows) - (keyMargin * 2), COL_WHITE);
    } else {
        DrawText(label.c_str(), textX, textY, COL_WHITE);
    }
}

// Draw virtual keyboard
void drawKeyboard() {
    for (int row = 0; row < kbRows; row++) {
        for (int col = 0; col < keyCols[row]; col++) {
            
            // Set the layout of the virtual keyboard according to the state of the SHIFT key
            String label = actShift ? upperLabel[row][col] : smallLabel[row][col];
            drawKey(row, col, keyWidths[row]);
        }
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
    
    //Select font
    SelFont8x8();
    
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

    // Check the status of the SHIFT, CHAR and SMILE key
    if (KeyPressed(KEY_A)) {
        String selectedKey = (actShift ? upperLabel[startRow][startCol] : smallLabel[startRow][startCol]);

        if (selectedKey == "SHIFT") {
            toggleShift();
            drawKeyboard();
            DispUpdate();
            delay(200);
            return;
        }

        if (selectedKey == "CHAR") {
            toggleChar();
            drawKeyboard();
            DispUpdate();
            delay(200);
            return;
        }

        if (selectedKey == "SMILE") {
            toggleSmile();
            drawKeyboard();
            DispUpdate();
            delay(200);
            return;
        }
    }

    // Set cursor movement
    if (KeyPressed(KEY_UP) && startRow > 0) {
        if (startRow == 1) {
            if (startCol < 2) startCol = 0; // CHAR key
            else if (startCol < 4) startCol = 1; // SMILE key
            else if (startCol < 6) startCol = 2; // LABEL3 key
            else if (startCol < 8) startCol = 3; // LABEL4 key
            else startCol = 4; // LABEL5 key
        }
        startRow--;
    } else if (KeyPressed(KEY_DOWN) && startRow < kbRows - 1) {
        if (startRow == 0) {
            if (startCol == 0) startCol = 1; // Move from the CHAR key to the 2 key
            else if (startCol == 1) startCol = 3; // Move from the SMILE key to the 4 key
            else if (startCol == 2) startCol = 5; // Move from the LABEL3 key to the 6 key
            else if (startCol == 3) startCol = 7; // Move from the LABEL4 key to the 8 key
            else if (startCol == 4) startCol = 9; // Move from the LEBEL5 key to the 0 key
        }
        startRow++;
        // Set move to SPACE key
        if (startRow == 4 && startCol > 7) {
            startCol = 8;
        }
    } else if (KeyPressed(KEY_LEFT) && startCol > 0) {
        startCol--;
    } else if (KeyPressed(KEY_RIGHT) && startCol < keyCols[startRow] - 1) {
        startCol++;
    }

    // Set character insertion function
if (KeyPressed(KEY_A)) {
    String selectedKey;
    if (actChar) {
        selectedKey = charLabel[startRow][startCol];
    } else if (actSmile) {
        selectedKey = smileLabel[startRow][startCol];
    } else if (actShift) {
        selectedKey = upperLabel[startRow][startCol];
    } else {
        selectedKey = smallLabel[startRow][startCol];
    }

    int lastNewLine = currentText.lastIndexOf('\n');
    if (lastNewLine == -1) lastNewLine = 0;
    else lastNewLine++;

    int LineLength = currentText.length() - lastNewLine;
    int maxCharsLine = (WIDTH / DrawFontWidth) - 1;

    if (LineLength >= maxCharsLine - 1 && selectedKey != "BACKSPACE") {
        currentText += "\n";
    }

    if (selectedKey == "BACKSPACE") {
        if (currentText.length() > 0) {
            currentText.remove(currentText.length() - 1);
        }
    } else if (selectedKey != "SHIFT" && selectedKey != "CHAR" && selectedKey != "SMILE") {
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