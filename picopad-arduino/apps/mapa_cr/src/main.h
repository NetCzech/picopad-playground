#ifndef MAPACR_MAIN_H
#define MAPACR_MAIN_H

#include "Arduino.h"

// Include laskakit logo
#include "img/laskakit.c"

// Wi-Fi credentials
const char* ssid = "your_ssid";
const char* password = "your_wifi_password";

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
    "Dìèín",                            // ID 0
    "Liberec",                          // ID 1
    "Jablonec nad Nisou",               // ID 2
    "Ústí nad Labem",                   // ID 3
    "Èeská Lípa",                       // ID 4
    "Semily",                           // ID 5
    "Teplice",                          // ID 6
    "Trutnov",                          // ID 7
    "Litomìøice",                       // ID 8
    "Most",                             // ID 9
    "Chomutov",                         // ID 10
    "Jièín",                            // ID 11
    "Náchod",                           // ID 12
    "Mladá Boleslav",                   // ID 13
    "Mìlník",                           // ID 14
    "Louny",                            // ID 15
    "Karlovy Vary",                     // ID 16
    "Jeseník",                          // ID 17
    "Hradec Kralove",                   // ID 18
    "Sokolov",                          // ID 19
    "Nymburk",                          // ID 20
    "Rychnov nad Knežnou",              // ID 21
    "Kladno",                           // ID 22
    "Rakovník",                         // ID 23
    "Cheb",                             // ID 24
    "Bruntál",                          // ID 25
    "Praha",                            // ID 26
    "Pardubice",                        // ID 27
    "Kolin",                            // ID 28
    "Ústí nad Orlicí",                  // ID 29
    "Opava",                            // ID 30
    "Šumperk",                          // ID 31
    "Beroun",                           // ID 32
    "Kutná hora",                       // ID 33
    "Chrudim",                          // ID 34
    "Karviná",                          // ID 35
    "Ostrava",                          // ID 36
    "Tachov",                           // ID 37
    "Svitavy",                          // ID 38
    "Benešov",                          // ID 39
    "Plzen",                            // ID 40
    "Rokycany",                         // ID 41
    "Frýdek Místek",                    // ID 42
    "Pøíbram",                          // ID 43
    "Nový Jièín",                       // ID 44
    "Olomouc",                          // ID 45
    "Havlíèkùv Brod",                   // ID 46
    "Zïár nad Sázavou",                 // ID 47
    "Pøerov",                           // ID 48
    "Prostìjov",                        // ID 49
    "Domažlice",                        // ID 50
    "Pelhøimov",                        // ID 51
    "Tábor",                            // ID 52
    "Jihlava",                          // ID 53
    "Klatovy",                          // ID 54
    "Blansko",                          // ID 55
    "Vsetín",                           // ID 56
    "Kromìøíž",                         // ID 57
    "Písek",                            // ID 58
    "Vyškov",                           // ID 59
    "Strakonice",                       // ID 60
    "Zlín",                             // ID 61
    "Tøebíè",                           // ID 62
    "Brno",                             // ID 63
    "Jindøichùv Hradec",                // ID 64
    "Uherské Hradištì",                 // ID 65
    "Prachatice",                       // ID 66
    "Èeské Budìjovice",                 // ID 67
    "Hodonín",                          // ID 68
    "Znojmo",                           // ID 69
    "Èeský Krumlov",                    // ID 70
    "Bøeclav"                           // ID 71
};

// Convert RGB color to rain type
struct RGBDescription {
    int r, g, b;
    const char* description;
};

const RGBDescription rgbDescriptions[] = {
    {48, 0, 168, "Mrholení: < 0.5 mm/h, 8 dBZ"},
    {56, 0, 112, "Mrholení: < 0.5 mm/h, 8 dBZ"},
    {0, 108, 192, "Mrholení: < 0.5 mm/h, 8 dBZ"},
    {0, 0, 252, "Mrholení: < 0.5 mm/h, 12 dBZ"},
    {0, 160, 0, "Déš: +/- 4 mm/h, 28 dBZ"},
    {0, 188 ,0, "Déš, +/- 4 mm/h, 28 dBZ"},
    {52, 216, 0, "Déš, +/- 4 mm/h, 28 dBZ"},
    {156, 220, 0, "Déš: +/- 4 mm/h, 32 dBZ"},
    {224, 220, 0, "Silný déš, +/- 10 mm/h, 40 dBZ"},
    {252, 176, 0, "Silný déš, +/- 10 mm/h, 40 dBZ"},
    {252, 132, 0, "Silný déš, +/- 10 mm/h, 44 dBZ"},
    {252, 88, 0, "Velmi silný dés, +/- 70 mm/h, 48 dBZ"},
    {252, 0,0, "Velmi silný déš, +/- 70 mm/h, 48 dBZ"}
};

const char* getRGBDescription(int r, int g, int b) {
    for (unsigned int i = 0; i < sizeof(rgbDescriptions) / sizeof(RGBDescription); i++) {
        if (rgbDescriptions[i].r == r && rgbDescriptions[i].g == g && rgbDescriptions[i].b == b) {
            return rgbDescriptions[i].description;
        }
    }
    return "Neznámá intenzita deštì";
}

#endif //MAPACR_MAIN_H