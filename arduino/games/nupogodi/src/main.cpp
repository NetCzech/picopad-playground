// ****************************************************************************
//
//                        Main code for Nu, Pogodi
//
// ****************************************************************************

#include <Arduino.h>
#include "picopad.h"
#include "main.h"
#include <stdlib.h>

struct Point {
    int x, y;
};

// Nastavení počátku hry
int totalEggs = 0;
int caughtEggs = 0;
int lives = 3;
bool gameOver = false;

enum WolfState { LEFT_BOTTOM, RIGHT_BOTTOM, LEFT_TOP, RIGHT_TOP };
WolfState wolfState = LEFT_TOP; // Výchozí pozice vlka

// Struktura pro padající vajíčko
struct FallingEgg {
    const Point* path;
    int pathIndex;
    bool active;
    bool broken;
    bool isLeft;
};

FallingEgg fallingEgg;
FallingEgg secondaryEgg;

// Definování drah pro vajíčka
const Point leftTopPath[4] = {{20, 79}, {30, 85}, {40, 91}, {55, 100}};
const Point leftBottomPath[4] = {{20, 126}, {30, 132}, {40, 138}, {55, 147}};
const Point rightTopPath[4] = {{290, 79}, {280, 85}, {270, 91}, {255, 100}};
const Point rightBottomPath[4] = {{290, 126}, {280, 132}, {270, 138}, {255, 147}};

const Point* allPaths[4] = {leftTopPath, leftBottomPath, rightTopPath, rightBottomPath};

// Časovače pro vajíčka
unsigned long lastMoveTime = 0; // Poslední čas pohybu vajíčka
unsigned long brokenEggStartTime = 0; // Čas, kdy se začalo zobrazovat rozbité vajíčko
unsigned long secondaryEggLastMoveTime = 0; // Poslední čas pohybu druhého vajíčka
unsigned long chickStartTime = 0; // Čas, kdy se začalo zobrazovat kuřátko
unsigned long lastEggSpawnTime = 0; // čas posledního generování vajíčka
unsigned long eggMoveDuration = 800; // Čas mezi posuny vajíčka v ms (rychlost padání vajec)
bool showBrokenEgg = false; // Stav pro zobrazení rozbitého vajíčka
bool showChick = false; // Stav pro zobrazení kuřátka
int transitionDelay = 350; // Pauza mezi zobrazením rozbitého vajíčka a kuřátka

// Inicializace vajíčka
void initFallingEgg(FallingEgg &egg, bool isSecondary = false) {
    if (gameOver) return;
    egg.active = true;
    egg.broken = false;
    if (!isSecondary) {
        showBrokenEgg = false;
        showChick = false;
    }
    egg.pathIndex = 0;

    // Nastavení náhodné dráhy
    int randomPath = random (0, 4);
    egg.path = allPaths[randomPath];
    egg.isLeft = (randomPath == 0 || randomPath == 1);
    if (!isSecondary) {
        lastMoveTime = millis();
        Serial.println("New egg initialized");
    } else {
        secondaryEggLastMoveTime = millis();
        Serial.println("Secondary egg initialized");
    }
}

// Rychlost generování vajec
unsigned long getEggInterval() {
    unsigned long baseInterval = 4000; // interval začátku hry
    long interval = (long)baseInterval - (totalEggs * 25); // každé vejce zkrátí čas o 30 ms
    if (interval < 800) interval = 800; // maximální rychlost hry
    return (unsigned long)interval;
}

// Zrychlování padání vajec podle počtu generovaných vajec
void increaseSpeed() {
    if (eggMoveDuration > 400) { // nejrychlejší pád = 400 ms
        eggMoveDuration -= 5; // každé vejce zrychlí pád o 5 ms
    }
}

// Aktualizace pozice vajíčka
void updateFallingEgg(FallingEgg &egg, unsigned long &lastMoveTime, unsigned long MoveDuration) {
    unsigned long currentTime = millis();
    if (egg.active && currentTime - lastMoveTime > MoveDuration) {
        lastMoveTime = currentTime;
        egg.pathIndex++;
        if (egg.pathIndex >= 4) {
            if ((egg.path == leftTopPath && wolfState == LEFT_TOP) || 
                (egg.path == leftBottomPath && wolfState == LEFT_BOTTOM) || 
                (egg.path == rightTopPath && wolfState == RIGHT_TOP) || 
                (egg.path == rightBottomPath && wolfState == RIGHT_BOTTOM)) { // Pokud je vlk ve správné pozici
                // Vlk chytil vejce -> zvýšení počtu chycených vajec
                caughtEggs++;
                Serial.println("Egg caught");
            } else {
                // Vlk nechytil vejce -> odečtení životu nebo konec hry
                egg.broken = true;
                brokenEggStartTime = millis();
                showBrokenEgg = true;
                showChick = false;
                lives--;
                Serial.println("Egg broken");
                if (lives <= 0) {
                    gameOver = true;
                    Serial.println("Game Over");
                }
            }
            egg.active = false;
            totalEggs++;
            increaseSpeed();
        }
    }
}

// Vytváření vajíčka podle dynamického intervalu
void createEgg() {
    unsigned long currentTime = millis();
    unsigned long eggInterval = getEggInterval();
    // Pokud uplynul čas od posledního generování
    if ((currentTime - lastEggSpawnTime >= eggInterval) && !fallingEgg.active && !gameOver) {
        initFallingEgg(fallingEgg);
        lastEggSpawnTime = currentTime;
    }
    // Po 10 bodech aktivuj druhé vejce
    if (caughtEggs >= 10 && !secondaryEgg.active && !gameOver) {
        if (currentTime - lastEggSpawnTime >= eggInterval / 2) {
            initFallingEgg(secondaryEgg, true);
        }
    }
}

// Vykreslení vlka
void drawWolf() {
    if (wolfState == LEFT_BOTTOM) {
        DrawImgRle(leftBottom, leftBottom_Pal, 60, 100, 98, 104);
    } else if (wolfState == RIGHT_BOTTOM) {
        DrawImgRle(rightBottom, rightBottom_Pal, 163, 100, 98, 104);
    } else if (wolfState == LEFT_TOP) {
        DrawImgRle(leftTop, leftTop_Pal, 60, 100, 98, 104);
    } else if (wolfState == RIGHT_TOP) {
        DrawImgRle(rightTop, rightTop_Pal, 163, 100, 98, 104);
    }
}

// Vykreslení vajíčka
void drawFallingEgg(const FallingEgg &egg) {
    if (egg.active) {
        Point currentPos = egg.path[egg.pathIndex];
        if (egg.path == leftTopPath || egg.path == leftBottomPath) {
            DrawImgRle(eggLeft, eggLeft_Pal, currentPos.x, currentPos.y, 10, 9);
        } else {
            DrawImgRle(eggRight, eggRight_Pal, currentPos.x, currentPos.y, 10, 9);
        }
    }
}

// Vykreslení rozbitého vajíčka
void drawBrokenEgg(bool isLeft) {
    if (isLeft) {
        DrawImgRle(brokenEgg, brokenEgg_Pal, 55, 190, 25, 13);
    } else {
        DrawImgRle(brokenEgg, brokenEgg_Pal, 240, 190, 25, 13);
    }
}

// Vykreslení kuřátka
void drawChick(bool isLeft) {
    if (isLeft) {
        DrawImgRle(leftChick, leftChick_Pal, 55, 182, 20, 22);
    } else {
        DrawImgRle(rightChick, rightChick_Pal, 240, 182, 20, 22);
    }
}

// Vykreslení životů
void drawLives() {
    for (int i = 0; i < 3; i++) {
        if (i < 3 - lives) {
            DrawImgRle(minusLife, minusLife_Pal, 195 + i * 20, 30, 20, 20);
        }
    }
}

// Vykreslení Game Over
void drawGameOver() {
    SelFont8x8();
    DrawText2("GAME OVER", 90, 70, COL_MDKRED);
}

// Vykreslení skóre
void drawScore() {
    char scoreText[20];
    SelFont8x8();
    snprintf(scoreText, sizeof(scoreText), "%03d", caughtEggs);
    DrawText2(scoreText, 200, 10, COL_BLACK);
}

// Vykreslení scény
void drawScene() {
    DrawClear();
    DrawImgRle(Background, Background_Pal, 0, 0, 320, 240);
    drawWolf();
    drawFallingEgg(fallingEgg);
    drawFallingEgg(secondaryEgg);
    if (showBrokenEgg) {
        drawBrokenEgg(fallingEgg.isLeft);
    }
    if (showChick) {
        drawChick(fallingEgg.isLeft);
    }
    drawScore();
    drawLives();
    if (gameOver) {
        drawGameOver();
    }
    DispUpdate();
}

void setup() {
    device_init();
    Serial.begin(9600);
    randomSeed(millis());
    DrawClear();
    DrawImgRle(Background, Background_Pal, 0, 0, 320, 240);
    initFallingEgg(fallingEgg);
    drawScene();
    DispUpdate();
    lastMoveTime = millis();
    lastEggSpawnTime = millis();
}

void loop() {
    char ch = KeyGet();
    if (!gameOver) {
        if (ch == KEY_LEFT) {
            if (wolfState == RIGHT_TOP) {
                wolfState = LEFT_TOP;
            } else if (wolfState == RIGHT_BOTTOM) {
                wolfState = LEFT_BOTTOM;
            }
        } else if (ch == KEY_RIGHT) {
            if (wolfState == LEFT_TOP) {
                wolfState = RIGHT_TOP;        
            } else if (wolfState == LEFT_BOTTOM) {
                wolfState = RIGHT_BOTTOM;
            }
        } else if (ch == KEY_UP) {
            if (wolfState == LEFT_BOTTOM) {
                wolfState = LEFT_TOP;
            } else if (wolfState == RIGHT_BOTTOM) {
                wolfState = RIGHT_TOP;
            }
        } else if (ch == KEY_DOWN) {
            if (wolfState == LEFT_TOP) {
                wolfState = LEFT_BOTTOM;
            } else if (wolfState == RIGHT_TOP) {
                wolfState = RIGHT_BOTTOM;
            }
        }
    }
    if (ch == KEY_Y) {
        reset_to_boot_loader();
    }
    updateFallingEgg(fallingEgg, lastMoveTime, eggMoveDuration);
    updateFallingEgg(secondaryEgg, secondaryEggLastMoveTime, eggMoveDuration);

    // Aktualizace stavu rozbitého vajíčka a kuřátka
    if (showBrokenEgg && (millis() - brokenEggStartTime > transitionDelay)) {
        showBrokenEgg = false;
        showChick = true;
        chickStartTime = millis();
    }
    if (showChick && (millis() - chickStartTime > transitionDelay)) {
        showChick = false;
    }

    createEgg();
    drawScene();
}
