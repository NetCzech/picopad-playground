// ****************************************************************************
//
//                            Main code for Snake
//
// ****************************************************************************

#ifndef _MAIN_H
#define _MAIN_H

// Sound files
#include "snd/eatSnd.h"
#include "snd/deadSnd.h"

// Image files
#include "img/deadPict.h"

// Define the size of the playing area
#define WIDTH_AREA 80
#define HEIGHT_AREA 60

//Score area definition
#define SCORE_AREA 6

// Define the size of the snake
#define SNAKE_SIZE 4

// Sound definition
extern const u8 EatSnd[13848];
extern const u8 deadSnd[85328];

// Graphic definition
#define COL_BACKGROUND	RGBTO16(135, 165, 5)
#define COL_BODY        RGBTO16(33, 45, 22)
#define COL_SCORE       RGBTO16(33, 45, 22)

//Variables for score
int score = 0;
char scoreText[20];

//Variables for pause
bool pause = false;
int isPause = 0;

#endif // _MAIN_H