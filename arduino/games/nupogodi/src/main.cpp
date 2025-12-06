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

// Nastavení hry
int totalEggs = 0;
int caughtEggs = 0;
int lives = 3;
bool gameOver = false;

enum GameState { STATE_INTRO, STATE_PLAYING, STATE_SCORE };
GameState gameState = STATE_INTRO;

enum WolfState { LEFT_BOTTOM, RIGHT_BOTTOM, LEFT_TOP, RIGHT_TOP };
WolfState wolfState = LEFT_TOP;

#define SCORE_FILE "nupogodi.cfg"
int highScore[4] = {0, 0, 0, 0};

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

// Časovače
unsigned long lastMoveTime = 0; // Poslední čas pohybu vajíčka
unsigned long brokenEggStartTime = 0; // Čas, kdy se začalo zobrazovat rozbité vajíčko
unsigned long secondaryEggLastMoveTime = 0; // Poslední čas pohybu druhého vajíčka
unsigned long chickStartTime = 0; // Čas, kdy se začalo zobrazovat kuřátko
unsigned long lastEggSpawnTime = 0; // čas posledního generování vajíčka
unsigned long eggMoveDuration = 800; // Čas mezi posuny vajíčka v ms (rychlost padání vajec)
unsigned long gameOverTime = 0; // Čas, kdy nastal stav Game Over

// Stavy
bool showBrokenEgg = false; // Stav pro zobrazení rozbitého vajíčka
bool showChick = false; // Stav pro zobrazení kuřátka
int transitionDelay = 350; // Pauza mezi zobrazením rozbitého vajíčka a kuřátka

// Načtení skóre z SD karty
void loadScore() {
    // Ujisti se, že je disk připojený
    if (!sd_mount()) {
        // SD není dostupná → necháme default 0,0,0,0
        highScore[0] = highScore[1] = highScore[2] = highScore[3]= 0;
        return;
    }

    FIL file;
    
    // Pokud soubor neexistuje, vytvoříme ho s nulami
    if (!file_exist(SCORE_FILE)) {
        if (file_create(&file, SCORE_FILE)) {
            file_write(&file, highScore, sizeof(highScore));
            file_close(&file);
        }
        return;
    }

    // Soubor existuje → otevřít a přečíst
    if (!file_open(&file, SCORE_FILE, FA_READ)) {
        // Nelze otevřít → necháme 0,0,0,0
        highScore[0] = highScore[1] = highScore[2] = highScore[3]= 0;
        return;
    }

    u32 readBytes = file_read(&file, highScore, sizeof(highScore));
    file_close(&file);

    // Pokud velikost nesedí, raději reset
    if (readBytes != sizeof(highScore)) {
        highScore[0] = highScore[1] = highScore[2] = highScore[3]= 0;
    }
}

// Uložení skóre na SD kartu
void saveScore() {
    if (!sd_mount()) return;

    FIL file;

    // Přepíšeme soubor novým obsahem
    if (!file_create(&file, SCORE_FILE)) return;

    file_write(&file, highScore, sizeof(highScore));
    file_close(&file);
}

// Aktualizace pole skóre
void updateScore(int newScore) {
    // 0 = nejlepší, 1 = druhé, 2 = třetí, 3 = čtvrté
    if (newScore > highScore[0]) {
        highScore[3] = highScore[2];
        highScore[2] = highScore[1];
        highScore[1] = highScore[0];
        highScore[0] = newScore;
    } else if (newScore > highScore[1]) {
        highScore[3] = highScore[2];
        highScore[2] = highScore[1];
        highScore[1] = newScore;
    } else if (newScore > highScore[2]) {
        highScore[3] = highScore[2];
        highScore[2] = newScore;
    } else if (newScore > highScore[3]) {
        highScore[3] = newScore;
    }
}

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

// Start hry
void startGame() {
    totalEggs = 0;
    caughtEggs = 0;
    lives = 3;
    gameOver = false;

    fallingEgg.active = false;
    fallingEgg.broken = false;
    fallingEgg.isLeft = false;
    fallingEgg.pathIndex = 0;

    secondaryEgg.active = false;
    secondaryEgg.broken = false;
    secondaryEgg.isLeft = false;
    secondaryEgg.pathIndex = 0;

    showBrokenEgg = false;
    brokenEggStartTime = 0;
    showChick = false;
    chickStartTime = 0;

    wolfState = LEFT_TOP;

    lastMoveTime = millis();
    secondaryEggLastMoveTime = millis();
    lastEggSpawnTime = millis();
    eggMoveDuration = 800; // počáteční rychlost hry
    
    initFallingEgg(fallingEgg);
    lastEggSpawnTime = millis();
    
    gameState = STATE_PLAYING; // přepnout ze zobrazení intra do hry
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
                if (lives <= 0 && !gameOver) {
                    gameOver = true;
                    gameOverTime = millis();
                    Serial.println("Game Over");
                    updateScore(caughtEggs);
                    saveScore();
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

// Vykreslení úvodní obrazovky
void drawIntroScreen() {
    DrawClear();
    DrawImgRle(intro, intro_Pal, 0, 0, 320, 240);

    SelFont8x16();

    DrawText("A", 13, 223, COL_SAFFRONGOLD);
    DrawText("Mode A", 29, 223, COL_SAFFRONGOLD);

    DrawText("B", 97, 223, COL_SAFFRONGOLD);
    DrawText("Mode B", 113, 223, COL_SAFFRONGOLD);

    DrawText("X", 182, 223, COL_SAFFRONGOLD);
    DrawText("Score", 198, 223, COL_SAFFRONGOLD);

    DrawText("Y", 259, 223, COL_SAFFRONGOLD);
    DrawText("Exit", 275, 223, COL_SAFFRONGOLD);

    DispUpdate();
}

void drawScoreScreen () {
    DrawClear();
    DrawImgRle (score, score_Pal, 0, 0, 320, 240);

    SelFont8x16();

    DrawText("A", 13, 223, COL_SAFFRONGOLD);
    DrawText("Mode A", 37, 223, COL_SAFFRONGOLD);

    DrawText("B", 136, 223, COL_SAFFRONGOLD);
    DrawText("Mode B", 160, 223, COL_SAFFRONGOLD);

    DrawText("Y", 251, 223, COL_SAFFRONGOLD);
    DrawText("Exit", 275, 223, COL_SAFFRONGOLD);

    SelFont8x16();
    char buf[16];

    DrawText("1.", 13, 195, COL_SAFFRONGOLD);
    snprintf(buf, sizeof(buf), "%03d", highScore[0]);
    DrawText(buf, 51, 195, COL_SAFFRONGOLD);
    
    DrawText("2.", 96, 195, COL_SAFFRONGOLD);
    snprintf(buf, sizeof(buf), "%03d", highScore[1]);
    DrawText(buf, 118, 195, COL_SAFFRONGOLD);
    
    DrawText("3.", 178, 195, COL_SAFFRONGOLD);
    snprintf(buf, sizeof(buf), "%03d", highScore[2]);
    DrawText(buf, 200, 195, COL_SAFFRONGOLD);

    DrawText("4.", 261, 195, COL_SAFFRONGOLD);
    snprintf(buf, sizeof(buf), "%03d", highScore[3]);
    DrawText(buf, 283, 195, COL_SAFFRONGOLD);

    DispUpdate();
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

// Vykreslení aktuálního skóre
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
    gameState = STATE_INTRO;
 
    drawIntroScreen();
 
    sd_init();
    sd_mount();
    loadScore();
}

void loop() {
    char ch = KeyGet();
    
    // Zpět / ukončení hry
    if (ch == KEY_Y) {
        if (gameState == STATE_INTRO) {
            reset_to_boot_loader();
        }
        else if (gameState == STATE_PLAYING) {
            gameOver = false;
            gameState = STATE_INTRO;
            drawIntroScreen();
            return;    
        }
        else if (gameState == STATE_SCORE) {
            gameState = STATE_INTRO;
            drawIntroScreen();
            return;
        }
    }

        // Intro
        if (gameState == STATE_INTRO) {
            if (ch == KEY_A) {
            startGame();    
        } else if (ch == KEY_X) {
            gameState = STATE_SCORE;
            drawScoreScreen();
        } else {
            drawIntroScreen();
        }
        return;
        }

    // Score
    if (gameState == STATE_SCORE) {
        if (ch == KEY_A) {
            startGame();
        } else {
            drawScoreScreen();
        }
        return;
    }

    // Hra (ovládání vlka)
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

    // Stav hry Game Over
    if (gameOver) {
        if (millis() - gameOverTime >= 3000) { // 3 vteřinová pauza mezi GameOver a Intro
            gameState = STATE_INTRO;
            gameOver = false;
            drawIntroScreen();
        } else {
            drawScene();
        }
        return;
    }

    // Aktualizace pohybu vajec
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
