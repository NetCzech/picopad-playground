#ifndef MAPACR_MAIN_H
#define MAPACR_MAIN_H

#include "Arduino.h"

// Include laskakit logo
#include "img/laskakit.c"

// Wi-Fi credentials
const char* ssid = "your_ssid";
const char* password = "your_password";

// The number of items on the page
#define itemsPerPage 10

char Date[32];
char Time[32];
char ID[50];
char RGB[50];
char numberPage[10];

int currentPage = 0;
int maxPages = 0;

// Convert ID to city name
const char* cityNames[] = {
    "D���n",                            // ID 0
    "Liberec",                          // ID 1
    "Jablonec nad Nisou",               // ID 2
    "�st� nad Labem",                   // ID 3
    "�esk� L�pa",                       // ID 4
    "Semily",                           // ID 5
    "Teplice",                          // ID 6
    "Trutnov",                          // ID 7
    "Litom��ice",                       // ID 8
    "Most",                             // ID 9
    "Chomutov",                         // ID 10
    "Ji��n",                            // ID 11
    "N�chod",                           // ID 12
    "Mlad� Boleslav",                   // ID 13
    "M�ln�k",                           // ID 14
    "Louny",                            // ID 15
    "Karlovy Vary",                     // ID 16
    "Jesen�k",                          // ID 17
    "Hradec Kralove",                   // ID 18
    "Sokolov",                          // ID 19
    "Nymburk",                          // ID 20
    "Rychnov nad Kne�nou",              // ID 21
    "Kladno",                           // ID 22
    "Rakovn�k",                         // ID 23
    "Cheb",                             // ID 24
    "Brunt�l",                          // ID 25
    "Praha",                            // ID 26
    "Pardubice",                        // ID 27
    "Kolin",                            // ID 28
    "�st� nad Orlic�",                  // ID 29
    "Opava",                            // ID 30
    "�umperk",                          // ID 31
    "Beroun",                           // ID 32
    "Kutn� hora",                       // ID 33
    "Chrudim",                          // ID 34
    "Karvin�",                          // ID 35
    "Ostrava",                          // ID 36
    "Tachov",                           // ID 37
    "Svitavy",                          // ID 38
    "Bene�ov",                          // ID 39
    "Plzen",                            // ID 40
    "Rokycany",                         // ID 41
    "Fr�dek M�stek",                    // ID 42
    "P��bram",                          // ID 43
    "Nov� Ji��n",                       // ID 44
    "Olomouc",                          // ID 45
    "Havl��k�v Brod",                   // ID 46
    "Z��r nad S�zavou",                 // ID 47
    "P�erov",                           // ID 48
    "Prost�jov",                        // ID 49
    "Doma�lice",                        // ID 50
    "Pelh�imov",                        // ID 51
    "T�bor",                            // ID 52
    "Jihlava",                          // ID 53
    "Klatovy",                          // ID 54
    "Blansko",                          // ID 55
    "Vset�n",                           // ID 56
    "Krom���",                         // ID 57
    "P�sek",                            // ID 58
    "Vy�kov",                           // ID 59
    "Strakonice",                       // ID 60
    "Zl�n",                             // ID 61
    "T�eb��",                           // ID 62
    "Brno",                             // ID 63
    "Jind�ich�v Hradec",                // ID 64
    "Uhersk� Hradi�t�",                 // ID 65
    "Prachatice",                       // ID 66
    "�esk� Bud�jovice",                 // ID 67
    "Hodon�n",                          // ID 68
    "Znojmo",                           // ID 69
    "�esk� Krumlov",                    // ID 70
    "B�eclav"                           // ID 71
};

// Convert RGB color to rain type
struct RGBDescription {
    int r, g, b;
    const char* description;
};

const RGBDescription rgbDescriptions[] = {
    {48, 0, 168, "Mrholen�: < 0.5 mm/h, 8 dBZ"},
    {56, 0, 112, "Mrholen�: < 0.5 mm/h, 8 dBZ"},
    {0, 108, 192, "Mrholen�: < 0.5 mm/h, 8 dBZ"},
    {0, 0, 252, "Mrholen�: < 0.5 mm/h, 12 dBZ"},
    {0, 160, 0, "D隝: +/- 4 mm/h, 28 dBZ"},
    {0, 188 ,0, "D隝, +/- 4 mm/h, 28 dBZ"},
    {52, 216, 0, "D隝, +/- 4 mm/h, 28 dBZ"},
    {156, 220, 0, "D隝: +/- 4 mm/h, 32 dBZ"},
    {224, 220, 0, "Siln� d隝, +/- 10 mm/h, 40 dBZ"},
    {252, 176, 0, "Siln� d隝, +/- 10 mm/h, 40 dBZ"},
    {252, 132, 0, "Siln� d隝, +/- 10 mm/h, 44 dBZ"},
    {252, 88, 0, "Velmi siln� d�s�, +/- 70 mm/h, 48 dBZ"},
    {252, 0,0, "Velmi siln� d隝, +/- 70 mm/h, 48 dBZ"}
};

const char* getRGBDescription(int r, int g, int b) {
    for (unsigned int i = 0; i < sizeof(rgbDescriptions) / sizeof(RGBDescription); i++) {
        if (rgbDescriptions[i].r == r && rgbDescriptions[i].g == g && rgbDescriptions[i].b == b) {
            return rgbDescriptions[i].description;
        }
    }
    return "Nezn�m� intenzita de�t�";
}

#endif //MAPACR_MAIN_H