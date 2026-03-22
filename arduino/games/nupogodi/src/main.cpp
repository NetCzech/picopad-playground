// ****************************************************************************
//
//                        Main code for Nu, Pogodi
//
// ****************************************************************************

#include <Arduino.h>
#include "picopad.h"
#include "main.h"
#include <stdlib.h>

// Povolení debug výpisů
//#define DEBUG

// Herní konstanty
const int MAX_LIVES = 3;                                // počet životů
const int MAX_SCORE = 999;                              // max. skóre
const int MAX_PENALTIES = MAX_LIVES * 2;                // počet trestných bodů
const int SEC_EGG_THRESHOLD = 10;                       // počet vajec, po kterých padají dvě vejce současně

const unsigned long EGG_MOVE_BASE = 800;                // počáteční rychlost hry
const unsigned long EGG_MOVE_SPEEDUP = 5;               // zrychlení hry
const unsigned long EGG_MOVE_MIN = 400;                 // max. rychlost hry

const unsigned long EGG_SPAWN_BASE = 4000;              // interval generování vajec na počátku hry
const long EGG_SPAWN_SPEEDUP = 25;                      // časový úbytek intervalu mezi generováním vajec
const long EGG_SPAWN_MIN = 800;                         // nejkratší možný inteval mezi generováním vajec

const unsigned long RABBIT_MIN_INTERVAL = 5000;         // minimální interval mezi zobrazováním zajíce
const unsigned long RABBIT_MAX_INTERVAL = 15000;        // maximální interval mezi zobrazováním zajíce
const unsigned long RABBIT_SHOW_DURATION = 1000;        // doba zobrazení zajíce
const unsigned long RABBIT_ANIM_SPEED = 250;            // rychlost animace

const int FORGIVE_FIRST = 200;                          // odpuštění trestných bodů po dosažení skóre 200
const int FORGIVE_SECOND = 500;                         // odpuštění trestných bodů po dosažení skóre 500

const int BROKENEGG_CHICK_DELAY= 350;                   // interval mezi zobrazením rozbitého vajíčka a kuřátka
const unsigned long GAME_OVER_DELAY = 3000;             // doba zobrazení GAME OVER obrazovky

// Herní proměnné
int totalEggs = 0;
int caughtEggs = 0;
int penalties = 0;

bool gameOver = false;
bool forgiveFirstDone = false;
bool forgiveSecondDone = false;
bool rabbitVisible = false;

unsigned long rabbitShowTime = 0;
unsigned long rabbitNextAppear = 0;

// Herní stavy
bool showBrokenEgg = false;                             // stav pro zobrazení rozbitého vajíčka
bool showChick = false;                                 // stav pro zobrazení kuřátka

enum GameState { STATE_INTRO, STATE_PLAYING, STATE_SCORE };
GameState gameState = STATE_INTRO;

enum WolfState { LEFT_BOTTOM, RIGHT_BOTTOM, LEFT_TOP, RIGHT_TOP };
WolfState wolfState = LEFT_TOP;

// Herní časovače
unsigned long lastMoveTime = 0;                         // poslední čas pohybu vajíčka
unsigned long brokenEggStartTime = 0;                   // čas, kdy se začalo zobrazovat rozbité vajíčko
unsigned long secondaryEggLastMoveTime = 0;             // poslední čas pohybu druhého vajíčka
unsigned long chickStartTime = 0;                       // čas, kdy se začalo zobrazovat kuřátko
unsigned long lastEggSpawnTime = 0;                     // čas posledního generování vajíčka
unsigned long eggMoveDuration = EGG_MOVE_BASE;          // čas mezi posuny vajíčka v ms (rychlost padání vajec)
unsigned long gameOverTime = 0;                         // čas, kdy nastal stav Game Over

struct Point {
    int x, y;
};

#define SCORE_FILE "nupogodi.cfg"
int highScore[4] = {0, 0, 0, 0};

// Struktura pro padající vajíčko
struct FallingEgg {
    const Point* path;
    int pathIndex;
    bool active;
};

FallingEgg fallingEgg;
FallingEgg secondaryEgg;

// Definování drah pro vajíčka
const Point leftTopPath[4] = {{20, 79}, {30, 85}, {40, 91}, {55, 100}};
const Point leftBottomPath[4] = {{20, 126}, {30, 132}, {40, 138}, {55, 147}};
const Point rightTopPath[4] = {{290, 79}, {280, 85}, {270, 91}, {255, 100}};
const Point rightBottomPath[4] = {{290, 126}, {280, 132}, {270, 138}, {255, 147}};

const Point* allPaths[4] = {leftTopPath, leftBottomPath, rightTopPath, rightBottomPath};
const Point* lastBrokenEggPath = nullptr;

// Načtení skóre z SD karty
void loadScore() {
    if (!sd_mount()) {
        highScore[0] = highScore[1] = highScore[2] = highScore[3]= 0;
        return;
    }

    FIL file;
    
    if (!file_exist(SCORE_FILE)) {
        if (file_create(&file, SCORE_FILE)) {
            file_write(&file, highScore, sizeof(highScore));
            file_close(&file);
        }
        return;
    }

    if (!file_open(&file, SCORE_FILE, FA_READ)) {
        highScore[0] = highScore[1] = highScore[2] = highScore[3]= 0;
        return;
    }

    int readBytes = file_read(&file, highScore, sizeof(highScore));
    file_close(&file);

    if (readBytes != (int)sizeof(highScore)) {
        highScore[0] = highScore[1] = highScore[2] = highScore[3]= 0;
    }
}

// Uložení skóre na SD kartu
void saveScore() {
    if (!sd_mount()) return;

    FIL file;

    if (!file_create(&file, SCORE_FILE)) return;

    file_write(&file, highScore, sizeof(highScore));
    file_close(&file);
}

// Aktualizace pole skóre
void updateScore(int newScore) {
    if (newScore > highScore[0]) {                      // 0 = nejlepší, 1 = druhé, 2 = třetí, 3 = čtvrté
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
    PLAYSOUND(eggSnd);
    if (!isSecondary) {
        showBrokenEgg = false;
        showChick = false;
    }
    egg.pathIndex = 0;

    // Nastavení náhodné dráhy
    int randomPath = random (0, 4);
    egg.path = allPaths[randomPath];
    if (!isSecondary) {
        lastMoveTime = millis();
        
        #ifdef DEBUG
        Serial.println("New egg initialized");
        #endif

    } else {
        secondaryEggLastMoveTime = millis();
        
        #ifdef DEBUG
        Serial.println("Secondary egg initialized");
        #endif
    }
}

// Start hry
void startGame() {
    totalEggs = 0;
    caughtEggs = 0;
    penalties = 0;
    
    lastBrokenEggPath = nullptr;

    forgiveFirstDone = false;
    forgiveSecondDone = false;

    secondaryEgg.active = false;
    secondaryEgg.pathIndex = 0;
    secondaryEggLastMoveTime = millis();
    eggMoveDuration = EGG_MOVE_BASE;

    showBrokenEgg = false;
    brokenEggStartTime = 0;

    showChick = false;
    chickStartTime = 0;

    rabbitVisible = false;
    rabbitShowTime = 0;
    rabbitNextAppear = millis() + random(RABBIT_MIN_INTERVAL, RABBIT_MAX_INTERVAL);

    initFallingEgg(fallingEgg);
    lastEggSpawnTime = millis();
    lastMoveTime = millis();
    
    wolfState = LEFT_TOP;
    
    gameState = STATE_PLAYING;
    gameOver = false;
}

// Rychlost generování vajec
unsigned long getEggInterval() {
    long interval = (long)EGG_SPAWN_BASE - (totalEggs * EGG_SPAWN_SPEEDUP);
    if (interval < EGG_SPAWN_MIN) interval = EGG_SPAWN_MIN;
    return (unsigned long)interval;
}

// Zrychlování padání vajec podle počtu generovaných vajec
void increaseSpeed() {
    if (eggMoveDuration > EGG_MOVE_MIN) {
        eggMoveDuration -= EGG_MOVE_SPEEDUP;
    }
}

// Aktualizace pozice vajíčka
void updateFallingEgg(FallingEgg &egg, unsigned long &lastMoveTime, unsigned long MoveDuration) {
    unsigned long currentTime = millis();
    if (egg.active && currentTime - lastMoveTime > MoveDuration) {
        lastMoveTime = currentTime;
        egg.pathIndex++;
        PLAYSOUND(eggSnd);
        if (egg.pathIndex >= 4) {
            if ((egg.path == leftTopPath && wolfState == LEFT_TOP) || 
                (egg.path == leftBottomPath && wolfState == LEFT_BOTTOM) || 
                (egg.path == rightTopPath && wolfState == RIGHT_TOP) || 
                (egg.path == rightBottomPath && wolfState == RIGHT_BOTTOM)) {
                caughtEggs++;
                if (caughtEggs > MAX_SCORE) caughtEggs = MAX_SCORE;
                PLAYSOUND(collectSnd);

                if (caughtEggs >= FORGIVE_FIRST && !forgiveFirstDone) {
                    forgiveFirstDone = true;
                    penalties = 0;
                }
                if (caughtEggs >= FORGIVE_SECOND && !forgiveSecondDone) {
                    forgiveSecondDone = true;
                    penalties = 0;
                }
                if (rabbitVisible && penalties > 0) {
                    penalties--;
                }

                #ifdef DEBUG
                Serial.println("Egg caught");
                #endif

            } else {
                lastBrokenEggPath = egg.path;
                brokenEggStartTime = millis();
                showBrokenEgg = true;
                showChick = false;
                PLAYSOUND(lifeDownSnd);
                if (penalties % 2 == 1) {
                    penalties += 1;
                } else {
                    penalties += 2;
                }
                
                #ifdef DEBUG
                Serial.println("Egg broken");
                #endif

                if (penalties >= MAX_PENALTIES && !gameOver) {
                    gameOver = true;
                    gameOverTime = millis();
                    PLAYSOUND(gameOverSnd);
                    fallingEgg.active = false;
                    secondaryEgg.active = false;
                    
                    #ifdef DEBUG
                    Serial.println("Game Over");
                    #endif

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

    if ((currentTime - lastEggSpawnTime >= eggInterval) && !fallingEgg.active && !gameOver) {
        initFallingEgg(fallingEgg);
        lastEggSpawnTime = currentTime;
    }

    if (caughtEggs >= SEC_EGG_THRESHOLD && !secondaryEgg.active && !gameOver) {
        if (currentTime - lastEggSpawnTime >= eggInterval / 2) {
            initFallingEgg(secondaryEgg, true);
        }
    }
}

// Vykreslení úvodní obrazovky
void drawIntroScreen() {
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
    char buf[16];

    DrawImgRle (score, score_Pal, 0, 0, 320, 240);
    SelFont8x16();

    DrawText("A", 13, 223, COL_SAFFRONGOLD);
    DrawText("Mode A", 37, 223, COL_SAFFRONGOLD);

    DrawText("B", 136, 223, COL_SAFFRONGOLD);
    DrawText("Mode B", 160, 223, COL_SAFFRONGOLD);

    DrawText("Y", 251, 223, COL_SAFFRONGOLD);
    DrawText("Exit", 275, 223, COL_SAFFRONGOLD);

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

// Vykreslí zajíce
void drawRabbit() {
    if (rabbitVisible) {
        unsigned long elapsed = millis() - rabbitShowTime;
        int frame = elapsed / RABBIT_ANIM_SPEED;
        
        if (frame % 2 == 0) {
            DrawImgRle(rabbitTop, rabbitTop_Pal, 0, 0, 82, 83);
        } else {
            DrawImgRle(rabbitBottom, rabbitBottom_Pal, 0, 0, 74, 83);
        }
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
    int fullIcons = penalties / 2;
    bool halfIcon = (penalties % 2);
    
    for (int i = 0; i < fullIcons && i < MAX_LIVES; i++) {                                  // Neblikající ikona (celý život)
        DrawImgRle(minusLife, minusLife_Pal, 195 + i * 20, 30, 20, 20);
    }
    
    if (halfIcon && fullIcons < MAX_LIVES) {                                                // Blikající ikona (1/2 života)
        if ((millis() / 250) % 2) {
            DrawImgRle(minusLife, minusLife_Pal, 195 + fullIcons * 20, 30, 20, 20);
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
    char scoreText[8];
    SelFont8x8();
    snprintf(scoreText, sizeof(scoreText), "%03d", caughtEggs);
    DrawText2(scoreText, 200, 10, COL_BLACK);
}

// Vykreslení scény
void drawScene() {
    DrawImgRle(Background, Background_Pal, 0, 0, 320, 240);
    drawRabbit();
    drawWolf();
    drawFallingEgg(fallingEgg);
    drawFallingEgg(secondaryEgg);
    
    if (showBrokenEgg && lastBrokenEggPath != nullptr) {
        bool isLeft = (lastBrokenEggPath == leftTopPath || lastBrokenEggPath == leftBottomPath);
        drawBrokenEgg(isLeft);
    }
    if (showChick && lastBrokenEggPath != nullptr) {
        bool isLeft = (lastBrokenEggPath == leftTopPath || lastBrokenEggPath == leftBottomPath);
        drawChick(isLeft);
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
    
    #ifdef DEBUG
    Serial.begin(9600);
    #endif
 
    randomSeed(millis());
    gameState = STATE_INTRO;
 
    drawIntroScreen();
 
    sd_init();
    sd_mount();
    loadScore();
}

void loop() {
    char ch = KeyGet();
  
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

    if (gameState == STATE_INTRO) {
        if (ch == KEY_A) {
            startGame();
        } else if (ch == KEY_X) {
            gameState = STATE_SCORE;
            drawScoreScreen();
        }
        return;
    }

    if (gameState == STATE_SCORE) {
        if (ch == KEY_A) {
            startGame();
        }
        return;
    }

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

    if (gameOver) {
        if (millis() - gameOverTime >= GAME_OVER_DELAY) {
            gameState = STATE_INTRO;
            gameOver = false;
            drawIntroScreen();
        } else {
            drawScene();
        }
        return;
    }

    updateFallingEgg(fallingEgg, lastMoveTime, eggMoveDuration);
    updateFallingEgg(secondaryEgg, secondaryEggLastMoveTime, eggMoveDuration);

    if (showBrokenEgg && (millis() - brokenEggStartTime > BROKENEGG_CHICK_DELAY)) {
        showBrokenEgg = false;
        showChick = true;
        chickStartTime = millis();
    }
    if (showChick && (millis() - chickStartTime > BROKENEGG_CHICK_DELAY)) {
        showChick = false;
    }

    if (!gameOver) {
        if (!rabbitVisible && millis() >= rabbitNextAppear) {
            rabbitVisible = true;
            rabbitShowTime = millis();
        }
        if (rabbitVisible && millis() - rabbitShowTime >= RABBIT_SHOW_DURATION) {
            rabbitVisible = false;
            rabbitNextAppear = millis() + random(RABBIT_MIN_INTERVAL, RABBIT_MAX_INTERVAL);
        }
    }

    createEgg();
    drawScene();
}
