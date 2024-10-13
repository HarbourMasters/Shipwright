#include "SceneDB.h"

#include <assert.h>

#include "variables.h"
#include "textures/map_48x85_static/map_48x85_static.h"
#include "textures/map_i_static/map_i_static.h"
#include "textures/icon_item_nes_static/icon_item_nes_static.h"
#include "textures/icon_item_ger_static/icon_item_ger_static.h"
#include "textures/icon_item_fra_static/icon_item_fra_static.h"
#include "textures/map_grand_static/map_grand_static.h"

// TODO function comments

SceneDB* SceneDB::Instance;
EntranceDB* EntranceDB::Instance;

const SceneDBInit initDB[] = {
    { // SCENE_DEKU_TREE
        "ydan_scene",
        "Inside the Deku Tree",
        "g_pn_06",
        SDC_DEKU_TREE,
        SCENETYPE_DUNGEON,
        { false, false, false, false, false, false, false, false, false, false, false, false },
        { false, {0, 0, 0}, 0, {} },
        { 3, 3, 1070, -690 },
        {},
        {
            7,
            { 10, 1, 2, 10, 4, 5, 6, 7, 8, 10, 11 },
            std::string{gPauseDekuTitleENGTex},
            std::string{gPauseDekuTitleGERTex},
            std::string{gPauseDekuTitleFRATex},
            {
                {    0,  9999.0f, { }, std::string{}, std::string{}, {}, {}, },
                {    0,  9999.0f, { }, std::string{}, std::string{}, {}, {}, },
                {    0,  9999.0f, { }, std::string{}, std::string{}, {}, {}, },
                { F_3F,   760.0f, { 0, 10, }, std::string{gDekuTreePauseScreenMapFloor3LeftTex}, std::string{gDekuTreePauseScreenMapFloor3RightTex}, { { 2, 40.0f, -33.0f }, { 6, 49.0f, -42.0f } }, {}, },
                { F_2F,   360.0f, { 0, 1, 2, }, std::string{gDekuTreePauseScreenMapFloor2LeftTex}, std::string{gDekuTreePauseScreenMapFloor2RightTex}, { { 1, 48.0f, -63.0f }, { 5, 52.0f, -68.0f } }, {}, },
                { F_1F,   -40.0f, { 0, }, std::string{gDekuTreePauseScreenMapFloor1LeftTex}, std::string{gDekuTreePauseScreenMapFloor1RightTex}, { { 3, 84.0f, -39.0f } }, {}, },
                { F_B1, -1000.0f, { 3, 4, 5, 6, 7, 8, }, std::string{gDekuTreePauseScreenMapBasement1LeftTex}, std::string{gDekuTreePauseScreenMapBasement1RightTex}, { { 4, 77.0f, -26.0f } }, {}, },
                { F_B2, -2000.0f, { 9, }, std::string{gDekuTreePauseScreenMapBasement2LeftTex}, std::string{gDekuTreePauseScreenMapBasement2RightTex}, {}, { { -1, 55.0f, 0.0f } }, },
            },
            {
                { 1090, -660,  std::string{gDekuTreeRoom0Floor1MinimapTex}, { { 3, 71, 50 }, }, {} },
                { 1390, -570,  std::string{gDekuTreeRoom1MinimapTex}, {}, {} },
                { 1560, -410,  std::string{gDekuTreeRoom2MinimapTex}, { { 1, 64, 62 }, { 5, 71, 69 }, }, {} },
                { 1220, -690,  std::string{gDekuTreeRoom3MinimapTex}, { { 4, 76, 37 }, }, {} },
                { 1200, -500,  std::string{gDekuTreeRoom4MinimapTex}, {}, {} },
                { 1390, -380,  std::string{gDekuTreeRoom5MinimapTex}, {}, {} },
                { 1770, -470,  std::string{gDekuTreeRoom6MinimapTex}, {}, {} },
                { 1610, -630,  std::string{gDekuTreeRoom7MinimapTex}, {}, {} },
                { 2000, -990,  std::string{gDekuTreeRoom8MinimapTex}, {}, {} },
                { 1290, -870,  std::string{gDekuTreeRoom9MinimapTex}, {}, { { -1, 50, 23 }, } },
                { 1420, -720,  std::string{gDekuTreeRoom10MinimapTex}, { { 2, 46, 50 }, { 6, 58, 60 }, }, {} },
                { 1110, -630,  std::string{gDekuTreeRoom0Floor2MinimapTex}, {}, {} },
                { 1040, -660,  std::string{gDekuTreeRoom0Floor3MinimapTex}, {}, {} },
            },
            {
                { 11, 12, 3 },
                { 0, 11, 4 },
                { 0, 12, 3 },
                { 12, 11, 4 },
                { 11, 0, 5 },
            },
        },
        {}
    },
    { // SCENE_DODONGOS_CAVERN
        "ddan_scene",
        "Dodongo's Cavern",
        "g_pn_08",
        SDC_DODONGOS_CAVERN,
        SCENETYPE_DUNGEON,
        { false, false, false, false, false, false, false, false, false, false, false, false },
        { false, {0, 0, 0}, 0, {} },
        { 4, 4, 1070, -690 },
        {},
        {
            7,
            { 1, 3, 5, 6, 10, 3, 9, 2, 4, 2, 4, 7, 7, 8, 13, 11 },
            std::string{gPauseDodongoTitleENGTex},
            std::string{gPauseDodongoTitleGERTex},
            std::string{gPauseDodongoTitleFRATex},
            {
                {    0,  9999.0f, { }, std::string{}, std::string{}, {}, {}, },
                {    0,  9999.0f, { }, std::string{}, std::string{}, {}, {}, },
                {    0,  9999.0f, { }, std::string{}, std::string{}, {}, {}, },
                {    0,  9999.0f, { }, std::string{}, std::string{}, {}, {}, },
                {    0,  9999.0f, { }, std::string{}, std::string{}, {}, {}, },
                {    0,  9999.0f, { }, std::string{}, std::string{}, {}, {}, },
                { F_2F,   280.0f, { 0, 2, 3, 5, 6, 9, 10, 12, }, std::string{gDodongosCavernPauseScreenMapFloor2LeftTex}, std::string{gDodongosCavernPauseScreenMapFloor2RightTex}, { { 10, 25.0f, -41.0f }, { 4, 53.0f, -47.0f }, { 6, 58.0f, -59.0f } }, {}, },
                { F_1F,  -440.0f, { 0, 1, 2, 3, 4, 7, 8, 11, 13, 14, 15, }, std::string{gDodongosCavernPauseScreenMapFloor1LeftTex}, std::string{gDodongosCavernPauseScreenMapFloor1RightTex}, { { 5, 13.0f, -60.0f }, { 8, 20.0f, -49.0f } }, { { -1, 23.0f, -25.0f } }, },
            },
            {
                { 940, -780,  std::string{gDodongosCavernRoom0MinimapTex}, { { 8, 22, 32 }, }, {} },
                { 320, -800,  std::string{gDodongosCavernRoom1MinimapTex}, {}, {} },
                { 1500, -1090,  std::string{gDodongosCavernRoom2MinimapTex}, {}, {} },
                { 240, -1230,  std::string{gDodongosCavernRoom3MinimapTex}, {}, {} },
                { 580, -1140,  std::string{gDodongosCavernRoom4MinimapTex}, {}, {} },
                { 1510, -820,  std::string{gDodongosCavernRoom5MinimapTex}, {}, {} },
                { 720, -1210,  std::string{gDodongosCavernRoom6MinimapTex}, {}, {} },
                { 1030, -1430,  std::string{gDodongosCavernRoom7MinimapTex}, {}, { { -1, 37, 49 }, } },
                { 800, -1580,  std::string{gDodongosCavernRoom8MinimapTex}, {}, {} },
                { 660, -920,  std::string{gDodongosCavernRoom9MinimapTex}, { { 4, 58, 40 }, { 6, 65, 64 }, }, {} },
                { 180, -830,  std::string{gDodongosCavernRoom10MinimapTex}, {}, {} },
                { 520, -730,  std::string{gDodongosCavernRoom11MinimapTex}, {}, {} },
                { 310, -950,  std::string{gDodongosCavernRoom12MinimapTex}, {}, {} },
                { 550, -1580,  std::string{gDodongosCavernRoom13MinimapTex}, {}, {} },
                { 790, -1910,  std::string{gDodongosCavernRoom14MinimapTex}, {}, {} },
                { 1650, -950,  std::string{gDodongosCavernRoom15MinimapTex}, { { 5, 68, 64 }, }, {} },
                { 1000, -860,  std::string{gDodongosCavernRoom0Floor3MinimapTex}, { { 10, 31, 27 }, }, {} },
                { 1570, -1070,  std::string{gDodongosCavernRoom2Floor3MinimapTex}, {}, {} },
                { 80, -1070,  std::string{gDodongosCavernRoom3Floor3MinimapTex}, {}, {} },
            },
            {
                { 0, 16, 6 },
                { 2, 17, 6 },
                { 3, 18, 6 },
                { 16, 0, 7 },
                { 17, 2, 7 },
                { 18, 3, 7 },
            },
        },
        {}
    },
    { // SCENE_JABU_JABU
        "bdan_scene",
        "Inside Jabu-Jabu's Belly",
        "g_pn_07",
        SDC_JABU_JABU,
        SCENETYPE_DUNGEON,
        { false, false, false, false, false, false, false, false, false, false, false, false },
        { false, {0, 0, 0}, 0, {} },
        { 3, 3, 1070, -690 },
        {},
        {
            6,
            { 3, 1, 2, 2, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 3 },
            std::string{gPauseJabuTitleENGTex},
            std::string{gPauseJabuTitleGERTex},
            std::string{gPauseJabuTitleFRATex},
            {
                {    0,  9999.0f, { }, std::string{}, std::string{}, {}, {}, },
                {    0,  9999.0f, { }, std::string{}, std::string{}, {}, {}, },
                {    0,  9999.0f, { }, std::string{}, std::string{}, {}, {}, },
                {    0,  9999.0f, { }, std::string{}, std::string{}, {}, {}, },
                {    0,  9999.0f, { }, std::string{}, std::string{}, {}, {}, },
                {    0,  9999.0f, { }, std::string{}, std::string{}, {}, {}, },
                { F_1F,  -640.0f, { 0, 1, 2, 4, 5, 6, 7, 8, 9, 10, 11, 12, }, std::string{gJabuPauseScreenMapFloor1LeftTex}, std::string{gJabuPauseScreenMapFloor1RightTex}, { { 1, 67.0f, -13.0f }, { 2, 28.0f, -13.0f }, { 4, 38.0f, 0.0f } }, { { -1, 65.0f, -37.0f } }, },
                { F_B1, -3000.0f, { 1, 3, 6, 13, 14, }, std::string{gJabuPauseScreenMapBasement1LeftTex}, std::string{gJabuPauseScreenMapBasement1RightTex}, {}, {}, },
            },
            {
                { 1130, -770,  std::string{gJabuRoom0MinimapTex}, {}, {} },
                { 1070, -1320,  std::string{gJabuRoom1MinimapTex}, {}, {} },
                { 1090, -1750,  std::string{gJabuRoom2MinimapTex}, {}, {} },
                { 1160, -1800,  std::string{gJabuRoom3MinimapTex}, {}, {} },
                { 1500, -1360,  std::string{gJabuRoom4MinimapTex}, {}, {} },
                { 690, -1330,  std::string{gJabuRoom5MinimapTex}, {}, { { -1, 67, 32 }, } },
                { 1540, -1700,  std::string{gJabuRoom6MinimapTex}, {}, {} },
                { 920, -2280,  std::string{gJabuRoom7MinimapTex}, {}, {} },
                { 1160, -2700,  std::string{gJabuRoom8MinimapTex}, {}, {} },
                { 700, -2360,  std::string{gJabuRoom9MinimapTex}, { { 1, 74, 57 }, }, {} },
                { 1650, -2360,  std::string{gJabuRoom10MinimapTex}, { { 2, 59, 57 }, }, {} },
                { 950, -2650,  std::string{gJabuRoom11MinimapTex}, {}, {} },
                { 1380, -2650,  std::string{gJabuRoom12MinimapTex}, { { 4, 68, 51 }, }, {} },
                { 1460, -1600,  std::string{gJabuRoom13MinimapTex}, {}, {} },
                { 830, -1445,  std::string{gJabuRoom14MinimapTex}, {}, {} },
                { 1170, -1370,  std::string{gJabuRoom1Basement1MinimapTex}, {}, {} },
                { 1620, -1910,  std::string{gJabuRoom6Basement1MinimapTex}, {}, {} },
            },
            {
                { 1, 15, 7 },
                { 6, 16, 7 },
                { 15, 1, 6 },
                { 16, 6, 6 },
            },
        },
        {}
    },
    { // SCENE_FOREST_TEMPLE
        "Bmori1_scene",
        "Forest Temple",
        "g_pn_01",
        SDC_FOREST_TEMPLE,
        SCENETYPE_DUNGEON,
        { false, false, false, false, false, false, false, false, false, false, false, false },
        { false, {0, 0, 0}, 0, {} },
        { 4, 4, 1070, -690 },
        {},
        {
            7,
            { 9, 1, 2, 3, 6, 5, 6, 7, 8, 8, 5, 4, 3, 13, 11, 10, 11, 2, 13, 12, 10, 12, 13 },
            std::string{gPauseForestTitleENGTex},
            std::string{gPauseForestTitleGERTex},
            std::string{gPauseForestTitleFRATex},
            {
                {    0,  9999.0f, { }, std::string{}, std::string{}, {}, {}, },
                {    0,  9999.0f, { }, std::string{}, std::string{}, {}, {}, },
                {    0,  9999.0f, { }, std::string{}, std::string{}, {}, {}, },
                {    0,  9999.0f, { }, std::string{}, std::string{}, {}, {}, },
                { F_2F,   760.0f, { 0, 6, 7, 8, 10, 11, 12, 13, 14, 19, 20, }, std::string{gForestTemplePauseScreenMapFloor2LeftTex}, std::string{gForestTemplePauseScreenMapFloor2RightTex}, { { 0, 49.0f, -3.0f }, { 2, 12.0f, -26.0f }, { 5, 74.0f, -13.0f }, { 7, 82.0f, -22.0f } }, {}, },
                { F_1F,   -40.0f, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 11, 15, 16, 18, 21, }, std::string{gForestTemplePauseScreenMapFloor1LeftTex}, std::string{gForestTemplePauseScreenMapFloor1RightTex}, { { 0, 49.0f, -3.0f }, { 2, 12.0f, -26.0f }, { 5, 74.0f, -13.0f }, { 7, 82.0f, -22.0f } }, {}, },
                { F_B1,  -440.0f, { 9, }, std::string{gForestTemplePauseScreenMapBasement1LeftTex}, std::string{gForestTemplePauseScreenMapBasement1RightTex}, { { 9, 31.0f, -29.0f } }, {}, },
                { F_B2, -3000.0f, { 17, }, std::string{gForestTemplePauseScreenMapBasement2LeftTex}, std::string{gForestTemplePauseScreenMapBasement2RightTex}, { { 11, 40.0f, -41.0f } }, { { -1, 50.0f, -11.0f } }, },
            },
            {
                { 1130, -660,  std::string{gForestTempleRoom0Floor1MinimapTex}, {}, {} },
                { 1170, -900,  std::string{gForestTempleRoom1MinimapTex}, {}, {} },
                { 965, -1040,  std::string{gForestTempleRoom2MinimapTex}, {}, {} },
                { 890, -1210,  std::string{gForestTempleRoom3MinimapTex}, {}, {} },
                { 1170, -1520,  std::string{gForestTempleRoom4MinimapTex}, {}, {} },
                { 1460, -1190,  std::string{gForestTempleRoom5MinimapTex}, {}, {} },
                { 1170, -1670,  std::string{gForestTempleRoom6Floor1MinimapTex}, { { 0, 72, 60 }, }, {} },
                { 800, -1320,  std::string{gForestTempleRoom7Floor1MinimapTex}, { { 5, 76, 38 }, }, {} },
                { 1320, -1310,  std::string{gForestTempleRoom8Floor1MinimapTex}, { { 9, 59, 66 }, }, {} },
                { 880, -1260,  std::string{gForestTempleRoom9MinimapTex}, { { 9, 15, 58 }, }, {} },
                { 1130, -1500,  std::string{gForestTempleRoom10MinimapTex}, { { 1, 69, 56 }, }, {} },
                { 1590, -1080,  std::string{gForestTempleRoom11Floor1MinimapTex}, {}, {} },
                { 1390, -1650,  std::string{gForestTempleRoom12MinimapTex}, { { 13, 80, 54 }, }, {} },
                { 830, -1620,  std::string{gForestTempleRoom13MinimapTex}, { { 15, 49, 50 }, }, {} },
                { 610, -1090,  std::string{gForestTempleRoom14MinimapTex}, {}, {} },
                { 580, -1390,  std::string{gForestTempleRoom15MinimapTex}, { { 7, 61, 61 }, }, {} },
                { 710, -1220,  std::string{gForestTempleRoom16MinimapTex}, {}, {} },
                { 980, -1090,  std::string{gForestTempleRoom17MinimapTex}, { { 11, 39, 63 }, }, { { -1, 53, 5 }, } },
                { 1640, -1290,  std::string{gForestTempleRoom18MinimapTex}, { { 2, 66, 57 }, }, {} },
                { 1510, -1410,  std::string{gForestTempleRoom19MinimapTex}, { { 14, 64, 31 }, }, {} },
                { 590, -1390,  std::string{gForestTempleRoom20MinimapTex}, {}, {} },
                { 1610, -1600,  std::string{gForestTempleRoom21MinimapTex}, {}, {} },
                { 1130, -660,  std::string{gForestTempleRoom0Floor3MinimapTex}, { { 3, 63, 56 }, }, {} },
                { 1130, -1630,  std::string{gForestTempleRoom6Floor3MinimapTex}, { { 12, 69, 59 }, }, {} },
                { 820, -1290,  std::string{gForestTempleRoom7Floor3MinimapTex}, {}, {} },
                { 1320, -1290,  std::string{gForestTempleRoom8Floor3MinimapTex}, {}, {} },
                { 1620, -1080,  std::string{gForestTempleRoom11Floor3MinimapTex}, { { 4, 73, 54 }, }, {} },
            },
            {
                { 0, 22, 4 },
                { 6, 23, 4 },
                { 7, 24, 4 },
                { 8, 25, 4 },
                { 11, 26, 4 },
                { 22, 0, 5 },
                { 23, 6, 5 },
                { 24, 7, 5 },
                { 25, 8, 5 },
                { 26, 11, 5 },
            },
        },
        {}
    },
    { // SCENE_FIRE_TEMPLE
        "HIDAN_scene",
        "Fire Temple",
        "g_pn_03",
        SDC_FIRE_TEMPLE,
        SCENETYPE_DUNGEON,
        { false, false, false, false, false, false, false, false, false, false, false, false },
        { false, {0, 0, 0}, 0, {} },
        { 4, 4, 1070, -690 },
        {},
        {
            7,
            { 1, 6, 7, 2, 3, 4, 6, 10, 6, 10, 7, 9, 11, 1, 12, 3, 11, 4, 5, 8, 9, 10, 11, 12, 13, 2 },
            std::string{gPauseFireTitleENGTex},
            std::string{gPauseFireTitleGERTex},
            std::string{gPauseFireTitleFRATex},
            {
                {    0,  9999.0f, { }, std::string{}, std::string{}, {}, {}, },
                {    0,  9999.0f, { }, std::string{}, std::string{}, {}, {}, },
                {    0,  9999.0f, { }, std::string{}, std::string{}, {}, {}, },
                { F_5F,  4360.0f, { 7, 8, 12, 13, }, std::string{gFireTemplePauseScreenMapFloor5LeftTex}, std::string{gFireTemplePauseScreenMapFloor5RightTex}, { { 5, 22.0f, -41.0f }, { 13, 74.0f, -29.0f } }, {}, },
                { F_4F,  3640.0f, { 5, 6, 7, 10, 11, 12, 13, 14, 24, }, std::string{gFireTemplePauseScreenMapFloor4LeftTex}, std::string{gFireTemplePauseScreenMapFloor4RightTex}, {}, {}, },
                { F_3F,  2760.0f, { 4, 5, 6, 9, 10, 11, 16, 23, 24, 25, }, std::string{gFireTemplePauseScreenMapFloor3LeftTex}, std::string{gFireTemplePauseScreenMapFloor3RightTex}, { { 3, 76.0f, -48.0f }, { 6, 72.0f, -50.0f }, { 7, 44.0f, -17.0f }, { 8, 63.0f, -12.0f }, { 9, 30.0f, -34.0f }, { 10, 61.0f, -31.0f } }, {}, },
                { F_2F,  2020.0f, { 4, 6, 10, 21, }, std::string{gFireTemplePauseScreenMapFloor2LeftTex}, std::string{gFireTemplePauseScreenMapFloor2RightTex}, { { 11, 78.0f, -34.0f } }, {}, },
                { F_1F,  -140.0f, { 0, 1, 2, 3, 15, 17, 18, 19, 20, 21, 22, }, std::string{gFireTemplePauseScreenMapFloor1LeftTex}, std::string{gFireTemplePauseScreenMapFloor1RightTex}, { { 0, 48.0f, -17.0f }, { 1, 35.0f, -45.0f }, { 2, 67.0f, -58.0f }, { 4, 74.0f, -15.0f }, { 12, 47.0f, -27.0f } }, { { -1, 26.0f, -34.0f } }, },
            },
            {
                { 1160, -580,  std::string{gFireTempleRoom0MinimapTex}, {}, {} },
                { 620, -620,  std::string{gFireTempleRoom1MinimapTex}, {}, {} },
                { 1330, -700,  std::string{gFireTempleRoom2MinimapTex}, { { 1, 53, 70 }, }, { { -1, 40, 47 }, } },
                { 1280, -1150,  std::string{gFireTempleRoom3MinimapTex}, { { 0, 71, 60 }, }, {} },
                { 440, -710,  std::string{gFireTempleRoom4Floor1MinimapTex}, { { 11, 59, 46 }, }, {} },
                { 600, -670,  std::string{gFireTempleRoom5Floor1MinimapTex}, { { 6, 57, 73 }, { 3, 66, 71 }, }, {} },
                { 810, -850,  std::string{gFireTempleRoom6Floor1MinimapTex}, {}, {} },
                { 830, -1000,  std::string{gFireTempleRoom7Floor1MinimapTex}, {}, {} },
                { 720, -720,  std::string{gFireTempleRoom8MinimapTex}, { { 13, 72, 38 }, }, {} },
                { 1170, -840,  std::string{gFireTempleRoom9MinimapTex}, {}, {} },
                { 1490, -870,  std::string{gFireTempleRoom10Floor1MinimapTex}, {}, {} },
                { 1640, -840,  std::string{gFireTempleRoom11Floor1MinimapTex}, { { 9, 80, 61 }, }, {} },
                { 1870, -790,  std::string{gFireTempleRoom12Floor1MinimapTex}, {}, {} },
                { 1800, -1000,  std::string{gFireTempleRoom13Floor1MinimapTex}, {}, {} },
                { 1610, -980,  std::string{gFireTempleRoom14MinimapTex}, {}, {} },
                { 1130, -900,  std::string{gFireTempleRoom15MinimapTex}, {}, {} },
                { 860, -660,  std::string{gFireTempleRoom16MinimapTex}, { { 10, 71, 36 }, }, {} },
                { 1310, -910,  std::string{gFireTempleRoom17MinimapTex}, { { 12, 77, 58 }, }, {} },
                { 1140, -1130,  std::string{gFireTempleRoom18MinimapTex}, {}, {} },
                { 850, -1230,  std::string{gFireTempleRoom19MinimapTex}, {}, {} },
                { 760, -1300,  std::string{gFireTempleRoom20MinimapTex}, { { 4, 78, 59 }, }, {} },
                { 380, -810,  std::string{gFireTempleRoom21Floor1MinimapTex}, {}, {} },
                { 800, -380,  std::string{gFireTempleRoom22MinimapTex}, { { 2, 71, 68 }, }, {} },
                { 800, -1270,  std::string{gFireTempleRoom23MinimapTex}, { { 8, 66, 58 }, }, {} },
                { 1930, -820,  std::string{gFireTempleRoom24Floor1MinimapTex}, {}, {} },
                { 1410, -1120,  std::string{gFireTempleRoom25MinimapTex}, { { 7, 77, 54 }, }, {} },
                { 640, -880,  std::string{gFireTempleRoom4Floor3MinimapTex}, {}, {} },
                { 845, -1070,  std::string{gFireTempleRoom5Floor3MinimapTex}, {}, {} },
                { 810, -850,  std::string{gFireTempleRoom6Floor2MinimapTex}, {}, {} },
                { 810, -850,  std::string{gFireTempleRoom6Floor3MinimapTex}, {}, {} },
                { 850, -1060,  std::string{gFireTempleRoom7Floor3MinimapTex}, {}, {} },
                { 1390, -640,  std::string{gFireTempleRoom10Floor2MinimapTex}, {}, {} },
                { 1540, -900,  std::string{gFireTempleRoom10Floor3MinimapTex}, {}, {} },
                { 1650, -850,  std::string{gFireTempleRoom11Floor3MinimapTex}, {}, {} },
                { 1880, -890,  std::string{gFireTempleRoom12Floor3MinimapTex}, {}, {} },
                { 1530, -740,  std::string{gFireTempleRoom13Floor3MinimapTex}, { { 5, 49, 62 }, }, {} },
                { 420, -880,  std::string{gFireTempleRoom21Floor3MinimapTex}, {}, {} },
                { 1950, -840,  std::string{gFireTempleRoom24Floor3MinimapTex}, {}, {} },
            },
            {
                { 21, 36, 6 },
                { 4, 26, 5 },
                { 6, 28, 5 },
                { 10, 31, 5 },
                { 5, 27, 4 },
                { 11, 33, 4 },
                { 24, 37, 4 },
                { 28, 29, 4 },
                { 31, 32, 4 },
                { 10, 32, 4 },
                { 7, 30, 3 },
                { 12, 34, 3 },
                { 13, 35, 3 },
                { 30, 7, 4 },
                { 34, 12, 4 },
                { 35, 13, 4 },
                { 27, 5, 5 },
                { 33, 11, 5 },
                { 37, 24, 5 },
                { 29, 28, 5 },
                { 32, 31, 5 },
                { 26, 4, 6 },
                { 28, 6, 6 },
                { 31, 10, 6 },
                { 36, 21, 7 },
            },
        },
        {}
    },
    { // SCENE_WATER_TEMPLE
        "MIZUsin_scene",
        "Water Temple",
        "g_pn_04",
        SDC_WATER_TEMPLE,
        SCENETYPE_DUNGEON,
        { false, false, false, false, false, false, false, false, false, false, false, false },
        { false, {0, 0, 0}, 0, {} },
        { 4, 4, 900, -640 },
        {},
        {
            4,
            { 1, 2, 10, 3, 3, 4, 5, 13, 5, 6, 7, 8, 7, 9, 8, 9, 11, 10, 12, 11, 12, 13, 9, 7, 8, 12 },
            std::string{gPauseWaterTitleENGTex},
            std::string{gPauseWaterTitleGERTex},
            std::string{gPauseWaterTitleFRATex},
            {
                {    0,  9999.0f, { }, std::string{}, std::string{}, {}, {}, },
                {    0,  9999.0f, { }, std::string{}, std::string{}, {}, {}, },
                {    0,  9999.0f, { }, std::string{}, std::string{}, {}, {}, },
                {    0,  9999.0f, { }, std::string{}, std::string{}, {}, {}, },
                { F_3F,   690.0f, { 0, 1, 4, 5, 6, 7, 10, 11, 13, 17, 19, 20, }, std::string{gWaterTemplePauseScreenMapFloor3LeftTex}, std::string{gWaterTemplePauseScreenMapFloor3RightTex}, { { 2, 88.0f, -60.0f }, { 7, 23.0f, -2.0f }, { 9, 84.0f, -45.0f } }, { { -1, 62.0f, -23.0f } }, },
                { F_2F,   330.0f, { 0, 1, 4, 5, 6, 10, 17, 20, 21, }, std::string{gWaterTemplePauseScreenMapFloor2LeftTex}, std::string{gWaterTemplePauseScreenMapFloor2RightTex}, { { 0, 86.0f, -60.0f }, { 8, 76.0f, -72.0f } }, {}, },
                { F_1F,  -160.0f, { 0, 1, 3, 5, 8, 9, 12, 14, 15, 16, 17, 18, 21, }, std::string{gWaterTemplePauseScreenMapFloor1LeftTex}, std::string{gWaterTemplePauseScreenMapFloor1RightTex}, { { 1, 88.0f, -60.0f }, { 3, 42.0f, -21.0f }, { 5, 47.0f, -15.0f }, { 10, 33.0f, -31.0f } }, {}, },
                { F_B1, -3000.0f, { 0, 1, 2, 3, 5, 8, 9, 12, 14, 15, }, std::string{gWaterTemplePauseScreenMapBasement1LeftTex}, std::string{gWaterTemplePauseScreenMapBasement1RightTex}, { { 6, 77.0f, -66.0f } }, {}, },
            },
            {
                { 1120, -610,  std::string{gWaterTempleRoom0Floor3MinimapTex}, {}, {} },
                { 1290, -930,  std::string{gWaterTempleRoom1Floor3MinimapTex}, {}, {} },
                { 1120, -770,  std::string{gWaterTempleRoom2MinimapTex}, { { 6, 79, 68 }, }, {} },
                { 1380, -680,  std::string{gWaterTempleRoom3Floor3MinimapTex}, {}, {} },
                { 930, -1060,  std::string{gWaterTempleRoom4Floor3MinimapTex}, { { 9, 81, 62 }, }, {} },
                { 1520, -930,  std::string{gWaterTempleRoom5Floor3MinimapTex}, {}, {} },
                { 1980, -980,  std::string{gWaterTempleRoom6MinimapTex}, {}, {} },
                { 2010, -1860,  std::string{gWaterTempleRoom7MinimapTex}, { { 7, 74, 62 }, }, {} },
                { 1590, -1380,  std::string{gWaterTempleRoom8Floor3MinimapTex}, { { 10, 57, 66 }, }, {} },
                { 1510, -1050,  std::string{gWaterTempleRoom9Floor3MinimapTex}, {}, {} },
                { 1500, -830,  std::string{gWaterTempleRoom10Floor3MinimapTex}, {}, {} },
                { 1300, -1230,  std::string{gWaterTempleRoom11MinimapTex}, {}, { { -1, 77, 40 }, } },
                { 1240, -1380,  std::string{gWaterTempleRoom12Floor3MinimapTex}, {}, {} },
                { 1800, -1330,  std::string{gWaterTempleRoom13MinimapTex}, {}, {} },
                { 1290, -1750,  std::string{gWaterTempleRoom14Floor3MinimapTex}, {}, {} },
                { 1450, -1610,  std::string{gWaterTempleRoom15Floor3MinimapTex}, {}, {} },
                { 1560, -1600,  std::string{gWaterTempleRoom16MinimapTex}, { { 5, 76, 64 }, }, {} },
                { 880, -830,  std::string{gWaterTempleRoom17Floor3MinimapTex}, {}, {} },
                { 820, -800,  std::string{gWaterTempleRoom18MinimapTex}, { { 1, 74, 61 }, }, {} },
                { 820, -820,  std::string{gWaterTempleRoom19MinimapTex}, { { 2, 74, 63 }, }, {} },
                { 1060, -690,  std::string{gWaterTempleRoom20Floor3MinimapTex}, {}, {} },
                { 1670, -1540,  std::string{gWaterTempleRoom21Floor3MinimapTex}, {}, {} },
                { 1120, -620,  std::string{gWaterTempleRoom0Floor2MinimapTex}, {}, {} },
                { 1130, -790,  std::string{gWaterTempleRoom0Floor1MinimapTex}, {}, {} },
                { 1130, -780,  std::string{gWaterTempleRoom0Basement1MinimapTex}, {}, {} },
                { 1290, -930,  std::string{gWaterTempleRoom1Floor2MinimapTex}, {}, {} },
                { 1290, -930,  std::string{gWaterTempleRoom1Floor1MinimapTex}, {}, {} },
                { 1280, -870,  std::string{gWaterTempleRoom1Basement1MinimapTex}, {}, {} },
                { 1390, -690,  std::string{gWaterTempleRoom3Basement1MinimapTex}, {}, {} },
                { 940, -1050,  std::string{gWaterTempleRoom4Floor2MinimapTex}, {}, {} },
                { 1520, -930,  std::string{gWaterTempleRoom5Floor2MinimapTex}, {}, {} },
                { 1520, -930,  std::string{gWaterTempleRoom5Floor1MinimapTex}, {}, {} },
                { 1980, -980,  std::string{gWaterTempleRoom6Floor2MinimapTex}, {}, {} },
                { 1620, -1370,  std::string{gWaterTempleRoom8Basement1MinimapTex}, {}, {} },
                { 1510, -1050,  std::string{gWaterTempleRoom9Basement1MinimapTex}, {}, {} },
                { 1490, -800,  std::string{gWaterTempleRoom10Floor2MinimapTex}, {}, {} },
                { 1240, -1380,  std::string{gWaterTempleRoom12Basement1MinimapTex}, {}, {} },
                { 1290, -1750,  std::string{gWaterTempleRoom14Basement1MinimapTex}, {}, {} },
                { 1450, -1610,  std::string{gWaterTempleRoom15Basement1MinimapTex}, {}, {} },
                { 880, -840,  std::string{gWaterTempleRoom17Floor2MinimapTex}, { { 0, 74, 64 }, }, {} },
                { 880, -830,  std::string{gWaterTempleRoom17Floor1MinimapTex}, {}, {} },
                { 1060, -690,  std::string{gWaterTempleRoom20Floor2MinimapTex}, { { 8, 75, 67 }, }, {} },
                { 1670, -1540,  std::string{gWaterTempleRoom21Floor1MinimapTex}, { { 3, 82, 65 }, }, {} },
                { 1520, -930,  std::string{gWaterTempleRoom5Basement1MinimapTex}, {}, {} },
            },
            {
                { 0, 22, 5 },
                { 1, 25, 5 },
                { 4, 29, 5 },
                { 5, 30, 5 },
                { 6, 32, 5 },
                { 10, 35, 5 },
                { 17, 39, 5 },
                { 20, 41, 5 },
                { 0, 23, 6 },
                { 1, 26, 6 },
                { 22, 23, 6 },
                { 25, 26, 6 },
                { 30, 31, 6 },
                { 39, 40, 6 },
                { 21, 42, 6 },
                { 17, 40, 6 },
                { 0, 24, 7 },
                { 1, 27, 7 },
                { 23, 24, 7 },
                { 26, 27, 7 },
                { 3, 28, 7 },
                { 9, 34, 7 },
                { 12, 36, 7 },
                { 14, 37, 7 },
                { 15, 38, 7 },
                { 31, 43, 7 },
                { 8, 33, 7 },
                { 24, 23, 6 },
                { 27, 26, 6 },
                { 28, 3, 6 },
                { 33, 8, 6 },
                { 36, 12, 6 },
                { 37, 14, 6 },
                { 38, 15, 6 },
                { 43, 31, 6 },
                { 34, 9, 6 },
                { 23, 22, 5 },
                { 26, 25, 5 },
                { 31, 30, 5 },
                { 40, 39, 5 },
                { 42, 21, 5 },
                { 17, 39, 5 },
                { 22, 0, 4 },
                { 25, 1, 4 },
                { 29, 4, 4 },
                { 30, 5, 4 },
                { 32, 6, 4 },
                { 35, 10, 4 },
                { 39, 17, 4 },
                { 41, 20, 4 },
            },
        },
        {}
    },
    { // SCENE_SPIRIT_TEMPLE
        "jyasinzou_scene",
        "Spirit Temple",
        "g_pn_05",
        SDC_SPIRIT_TEMPLE,
        SCENETYPE_DUNGEON,
        { false, false, false, false, false, false, false, false, false, false, false, false },
        { false, {0, 0, 0}, 0, {} },
        { 3, 3, 900, -640 },
        {},
        {
            5,
            { 10, 1, 2, 3, 4, 5, 10, 7, 8, 9, 10, 6, 12, 13, 11, 8, 13, 1, 2, 3, 4, 11, 6, 12, 8, 5, 10, 9, 6 },
            std::string{gPauseSpiritTitleENGTex},
            std::string{gPauseSpiritTitleGERTex},
            std::string{gPauseSpiritTitleFRATex},
            {
                {    0,  9999.0f, { }, std::string{}, std::string{}, {}, {}, },
                {    0,  9999.0f, { }, std::string{}, std::string{}, {}, {}, },
                {    0,  9999.0f, { }, std::string{}, std::string{}, {}, {}, },
                {    0,  9999.0f, { }, std::string{}, std::string{}, {}, {}, },
                { F_4F,  1480.0f, { 22, 23, 24, 25, 26, }, std::string{gSpiritTemplePauseScreenMapFloor4LeftTex}, std::string{gSpiritTemplePauseScreenMapFloor4RightTex}, { { 10, 59.0f, -9.0f }, { 18, 32.0f, -20.0f } }, {}, },
                { F_3F,   760.0f, { 5, 7, 8, 9, 10, 11, 16, 17, 18, 19, 20, 21, 23, }, std::string{gSpiritTemplePauseScreenMapFloor3LeftTex}, std::string{gSpiritTemplePauseScreenMapFloor3RightTex}, { { 1, 20.0f, -43.0f }, { 5, 83.0f, -26.0f }, { 15, 57.0f, -14.0f }, { 20, 81.0f, -55.0f }, { 21, 87.0f, -55.0f } }, { { -1, 47.0f, 0.0f } }, },
                { F_2F,   280.0f, { 4, 5, 6, 15, }, std::string{gSpiritTemplePauseScreenMapFloor2LeftTex}, std::string{gSpiritTemplePauseScreenMapFloor2RightTex}, { { 2, 41.0f, -16.0f }, { 3, 47.0f, -17.0f }, { 6, 27.0f, -16.0f }, { 12, 29.0f, -20.0f }, { 13, 70.0f, -22.0f }, { 14, 70.0f, -25.0f } }, {}, },
                { F_1F, -3000.0f, { 0, 1, 2, 3, 4, 12, 13, 14, 15, 27, }, std::string{gSpiritTemplePauseScreenMapFloor1LeftTex}, std::string{gSpiritTemplePauseScreenMapFloor1RightTex}, { { 0, 38.0f, -17.0f }, { 4, 55.0f, -14.0f }, { 8, 15.0f, -14.0f }, { 7, 78.0f, -3.0f } }, {}, },
            },
            {
                { 800, -570,  std::string{gSpiritTempleRoom0MinimapTex}, {}, {} },
                { 1500, -900,  std::string{gSpiritTempleRoom1MinimapTex}, {}, {} },
                { 1370, -1070,  std::string{gSpiritTempleRoom2MinimapTex}, { { 0, 78, 43 }, }, {} },
                { 1730, -1090,  std::string{gSpiritTempleRoom3MinimapTex}, { { 8, 69, 40 }, }, {} },
                { 1590, -1280,  std::string{gSpiritTempleRoom4Floor1MinimapTex}, {}, {} },
                { 1020, -1160,  std::string{gSpiritTempleRoom5Floor2MinimapTex}, { { 2, 43, 50 }, { 3, 56, 54 }, }, {} },
                { 1060, -920,  std::string{gSpiritTempleRoom6MinimapTex}, {}, {} },
                { 1470, -980,  std::string{gSpiritTempleRoom7MinimapTex}, {}, {} },
                { 1600, -780,  std::string{gSpiritTempleRoom8MinimapTex}, { { 1, 70, 67 }, }, {} },
                { 1830, -530,  std::string{gSpiritTempleRoom9MinimapTex}, {}, {} },
                { 1630, -350,  std::string{gSpiritTempleRoom10MinimapTex}, {}, {} },
                { 2000, -1000,  std::string{gBlankSpiritTempleRoom11MinimapTex}, {}, {} },
                { 650, -1440,  std::string{gSpiritTempleRoom12MinimapTex}, { { 7, 70, 58 }, }, {} },
                { 660, -1080,  std::string{gSpiritTempleRoom13MinimapTex}, {}, {} },
                { 1020, -1100,  std::string{gSpiritTempleRoom14MinimapTex}, { { 4, 68, 42 }, }, {} },
                { 880, -1110,  std::string{gSpiritTempleRoom15Floor1MinimapTex}, {}, {} },
                { 940, -980,  std::string{gSpiritTempleRoom16MinimapTex}, {}, {} },
                { 720, -860,  std::string{gSpiritTempleRoom17MinimapTex}, {}, {} },
                { 570, -790,  std::string{gSpiritTempleRoom18MinimapTex}, {}, {} },
                { 620, -540,  std::string{gSpiritTempleRoom19MinimapTex}, { { 20, 75, 58 }, { 21, 83, 58 }, }, {} },
                { 570, -350,  std::string{gSpiritTempleRoom20MinimapTex}, {}, {} },
                { 550, -1000,  std::string{gSpiritTempleRoom21MinimapTex}, { { 5, 71, 55 }, }, {} },
                { 970, -1250,  std::string{gSpiritTempleRoom22MinimapTex}, { { 10, 72, 49 }, }, {} },
                { 920, -820,  std::string{gSpiritTempleRoom23Floor3MinimapTex}, {}, {} },
                { 1040, -1070,  std::string{gSpiritTempleRoom24MinimapTex}, {}, {} },
                { 1150, -1180,  std::string{gSpiritTempleRoom25MinimapTex}, {}, {} },
                { 1200, -950,  std::string{gSpiritTempleRoom26MinimapTex}, { { 18, 50, 41 }, }, {} },
                { 1550, -1380,  std::string{gSpiritTempleRoom27MinimapTex}, {}, {} },
                { 1520, -1160,  std::string{gSpiritTempleRoom4Floor2MinimapTex}, { { 6, 73, 49 }, { 12, 79, 55 }, }, {} },
                { 1020, -1100,  std::string{gSpiritTempleRoom5Floor3MinimapTex}, { { 15, 77, 42 }, }, { { -1, 57, 23 }, } },
                { 820, -1150,  std::string{gSpiritTempleRoom15Floor2MinimapTex}, { { 13, 77, 63 }, { 14, 77, 68 }, }, {} },
                { 1010, -830,  std::string{gSpiritTempleRoom23Floor4MinimapTex}, {}, {} },
            },
            {
                { 4, 28, 6 },
                { 15, 30, 6 },
                { 5, 29, 5 },
                { 23, 31, 4 },
                { 31, 23, 5 },
                { 29, 5, 6 },
                { 28, 4, 7 },
                { 30, 15, 7 },
            },
        },
        {}
    },
    { // SCENE_SHADOW_TEMPLE
        "HAKAdan_scene",
        "Shadow Temple",
        "g_pn_02",
        SDC_SHADOW_TEMPLE_AND_WELL,
        SCENETYPE_DUNGEON,
        { false, false, false, false, false, false, false, false, false, false, false, false },
        { false, {0, 0, 0}, 0, {} },
        { 10, 10, 900, -640 },
        {},
        {
            7,
            { 13, 1, 5, 4, 4, 5, 3, 7, 8, 9, 10, 11, 12, 13, 2, 5, 1, 1, 6, 7, 8, 3, 9, 7, 8, 12 },
            std::string{gPauseShadowTitleENGTex},
            std::string{gPauseShadowTitleGERTex},
            std::string{gPauseShadowTitleFRATex},
            {
                {    0,  9999.0f, { }, std::string{}, std::string{}, {}, {}, },
                {    0,  9999.0f, { }, std::string{}, std::string{}, {}, {}, },
                {    0,  9999.0f, { }, std::string{}, std::string{}, {}, {}, },
                {    0,  9999.0f, { }, std::string{}, std::string{}, {}, {}, },
                { F_B1,  -343.3f, { 0, 1, 2, 4, }, std::string{gShadowTemplePauseScreenMapBasement1LeftTex}, std::string{gShadowTemplePauseScreenMapBasement1RightTex}, { { 1, 41.0f, -17.0f }, { 7, 27.0f, -24.0f } }, {}, },
                { F_B2,  -863.3f, { 5, 6, 7, 8, }, std::string{gShadowTemplePauseScreenMapBasement2LeftTex}, std::string{gShadowTemplePauseScreenMapBasement2RightTex}, { { 2, 81.0f, -20.0f }, { 3, 74.0f, -37.0f } }, {}, },
                { F_B3, -1143.3f, { 8, 9, 12, 14, 16, 21, }, std::string{gShadowTemplePauseScreenMapBasement3LeftTex}, std::string{gShadowTemplePauseScreenMapBasement3RightTex}, { { 12, 96.0f, -51.0f }, { 22, 96.0f, -55.0f } }, {}, },
                { F_B4, -3000.0f, { 3, 9, 10, 11, 12, 13, 14, 15, 17, 18, 19, 20, 21, }, std::string{gShadowTemplePauseScreenMapBasement4LeftTex}, std::string{gShadowTemplePauseScreenMapBasement4RightTex}, { { 4, 43.0f, -66.0f }, { 5, 37.0f, -66.0f }, { 6, 33.0f, -72.0f }, { 8, 85.0f, -18.0f }, { 9, 61.0f, -42.0f }, { 10, 15.0f, -4.0f }, { 11, 25.0f, -4.0f }, { 13, 19.0f, -29.0f }, { 21, 92.0f, -29.0f }, { 20, 87.0f, -20.0f } }, { { -1, 31.0f, -45.0f } }, },
            },
            {
                { 1320, -940,  std::string{gShadowTempleRoom0MinimapTex}, {}, {} },
                { 1320, -960,  std::string{gShadowTempleRoom1MinimapTex}, { { 1, 77, 64 }, }, {} },
                { 1090, -860,  std::string{gShadowTempleRoom2MinimapTex}, {}, {} },
                { 1510, -870,  std::string{gShadowTempleRoom3MinimapTex}, {}, { { -1, 77, 76 }, } },
                { 1480, -920,  std::string{gShadowTempleRoom4MinimapTex}, { { 7, 76, 65 }, }, {} },
                { 940, -870,  std::string{gShadowTempleRoom5MinimapTex}, {}, {} },
                { 920, -980,  std::string{gShadowTempleRoom6MinimapTex}, { { 2, 83, 67 }, }, {} },
                { 910, -820,  std::string{gShadowTempleRoom7MinimapTex}, { { 3, 76, 67 }, }, {} },
                { 800, -860,  std::string{gShadowTempleRoom8Basement2MinimapTex}, {}, {} },
                { 820, -520,  std::string{gShadowTempleRoom9Basement3MinimapTex}, {}, {} },
                { 1150, -500,  std::string{gShadowTempleRoom10MinimapTex}, { { 4, 78, 62 }, { 5, 74, 62 }, { 6, 71, 68 }, }, {} },
                { 1000, -780,  std::string{gShadowTempleRoom11MinimapTex}, { { 9, 77, 64 }, }, {} },
                { 1800, -1050,  std::string{gShadowTempleRoom12Basement3MinimapTex}, {}, {} },
                { 1660, -1130,  std::string{gShadowTempleRoom13MinimapTex}, { { 10, 71, 65 }, { 11, 80, 65 }, }, {} },
                { 1090, -770,  std::string{gShadowTempleRoom14Basement3MinimapTex}, {}, {} },
                { 1630, -1010,  std::string{gShadowTempleRoom15MinimapTex}, {}, {} },
                { 710, -680,  std::string{gShadowTempleRoom16MinimapTex}, { { 12, 87, 64 }, { 22, 87, 68 }, }, {} },
                { 1670, -930,  std::string{gShadowTempleRoom17MinimapTex}, { { 13, 77, 66 }, }, {} },
                { 830, -880,  std::string{gShadowTempleRoom18MinimapTex}, {}, {} },
                { 770, -900,  std::string{gShadowTempleRoom19MinimapTex}, { { 21, 78, 66 }, }, {} },
                { 800, -980,  std::string{gShadowTempleRoom20MinimapTex}, { { 8, 76, 66 }, { 20, 78, 68 }, }, {} },
                { 850, -990,  std::string{gShadowTempleRoom21Basement3MinimapTex}, {}, {} },
                { 830, -770,  std::string{gShadowTempleRoom8Basement3MinimapTex}, {}, {} },
                { 820, -520,  std::string{gShadowTempleRoom9Basement4MinimapTex}, {}, {} },
                { 1800, -1050,  std::string{gShadowTempleRoom12Basement4MinimapTex}, {}, {} },
                { 1090, -770,  std::string{gShadowTempleRoom14Basement4MinimapTex}, {}, {} },
                { 850, -990,  std::string{gShadowTempleRoom21Basement4MinimapTex}, {}, {} },
            },
            {
                { 8, 22, 6 },
                { 9, 23, 7 },
                { 12, 24, 7 },
                { 14, 25, 7 },
                { 21, 26, 7 },
                { 23, 9, 6 },
                { 24, 12, 6 },
                { 25, 14, 6 },
                { 26, 21, 6 },
                { 22, 8, 5 },
            },
        },
        {}
    },
    { // SCENE_BOTTOM_OF_THE_WELL
        "HAKAdanCH_scene",
        "Bottom of the Well",
        "g_pn_54",
        SDC_SHADOW_TEMPLE_AND_WELL,
        SCENETYPE_DUNGEON,
        { false, false, false, false, false, false, false, false, false, false, false, false },
        { false, {0, 0, 0}, 0, {} },
        { 5, 5, 900, -640 },
        {},
        {
            -1,
            { 13, 1, 2, 3, 4, 5, 6 },
            std::string{gPauseBotWTitleENGTex},
            std::string{gPauseBotWTitleGERTex},
            std::string{gPauseBotWTitleFRATex},
            {
                {    0,  9999.0f, { }, std::string{}, std::string{}, {}, {}, },
                {    0,  9999.0f, { }, std::string{}, std::string{}, {}, {}, },
                {    0,  9999.0f, { }, std::string{}, std::string{}, {}, {}, },
                {    0,  9999.0f, { }, std::string{}, std::string{}, {}, {}, },
                {    0,  9999.0f, { }, std::string{}, std::string{}, {}, {}, },
                { F_B1,  -100.0f, { 0, 1, 2, 3, 4, 5, 6, }, std::string{gBottomOfTheWellPauseScreenMapBasement1LeftTex}, std::string{gBottomOfTheWellPauseScreenMapBasement1RightTex}, { { 1, 51.0f, -24.0f }, { 3, 84.0f, -38.0f }, { 4, 31.0f, -2.0f }, { 5, 67.0f, -27.0f }, { 8, 46.0f, -27.0f }, { 10, 82.0f, -12.0f }, { 12, 80.0f, -16.0f }, { 14, 62.0f, -24.0f }, { 20, 89.0f, -38.0f } }, {}, },
                { F_B2,  -520.0f, { 0, 1, }, std::string{gBottomOfTheWellPauseScreenMapBasement2LeftTex}, std::string{gBottomOfTheWellPauseScreenMapBasement2RightTex}, { { 2, 54.0f, -27.0f }, { 9, 28.0f, -17.0f }, { 16, 56.0f, -38.0f } }, {}, },
                { F_B3, -3000.0f, { 1, }, std::string{gBottomOfTheWellPauseScreenMapBasement3LeftTex}, std::string{gBottomOfTheWellPauseScreenMapBasement3RightTex}, { { 7, 71.0f, -33.0f } }, {}, },
            },
            {
                { 1080, -570,  std::string{gBottomOfTheWellRoom0Basement1MinimapTex}, { { 1, 56, 26 }, { 4, 39, 3 }, { 5, 69, 30 }, { 8, 53, 30 }, { 12, 83, 16 }, { 14, 67, 26 }, }, {} },
                { 1420, -930,  std::string{gBottomOfTheWellRoom1Basement1MinimapTex}, {}, {} },
                { 1620, -1040,  std::string{gBottomOfTheWellRoom2MinimapTex}, {}, {} },
                { 1040, -1100,  std::string{gBottomOfTheWellRoom3MinimapTex}, { { 10, 79, 67 }, }, {} },
                { 940, -800,  std::string{gBottomOfTheWellRoom4MinimapTex}, { { 3, 73, 62 }, { 20, 81, 62 }, }, {} },
                { 1190, -1100,  std::string{gBottomOfTheWellRoom5MinimapTex}, {}, {} },
                { 1310, -1100,  std::string{gBottomOfTheWellRoom6MinimapTex}, {}, {} },
                { 1090, -570,  std::string{gBottomOfTheWellRoom0Basement2MinimapTex}, { { 2, 59, 28 }, { 9, 36, 19 }, { 16, 62, 38 }, }, {} },
                { 1380, -930,  std::string{gBottomOfTheWellRoom1Basement2MinimapTex}, {}, {} },
                { 1080, -890,  std::string{gBottomOfTheWellRoom1Basement3MinimapTex}, { { 7, 75, 66 }, }, {} },
            },
            {
                { 0, 7, 6 },
                { 1, 8, 6 },
                { 8, 9, 7 },
                { 9, 8, 6 },
                { 7, 0, 5 },
                { 8, 1, 5 },
            },
        },
        {}
    },
    { // SCENE_ICE_CAVERN
        "ice_doukutu_scene",
        "Ice Cavern",
        "g_pn_10",
        SDC_ICE_CAVERN,
        SCENETYPE_DUNGEON,
        { false, false, false, false, false, false, false, false, true, false, false, false },
        { false, {0, 0, 0}, 0, {} },
        { 3, 3, 900, -640 },
        {},
        {
            -1,
            { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 },
            std::string{gPauseIceCavernTitleENGTex},
            std::string{gPauseIceCavernTitleGERTex},
            std::string{gPauseIceCavernTitleFRATex},
            {
                {    0,  9999.0f, { }, std::string{}, std::string{}, {}, {}, },
                {    0,  9999.0f, { }, std::string{}, std::string{}, {}, {}, },
                {    0,  9999.0f, { }, std::string{}, std::string{}, {}, {}, },
                {    0,  9999.0f, { }, std::string{}, std::string{}, {}, {}, },
                {    0,  9999.0f, { }, std::string{}, std::string{}, {}, {}, },
                {    0,  9999.0f, { }, std::string{}, std::string{}, {}, {}, },
                {    0,  9999.0f, { }, std::string{}, std::string{}, {}, {}, },
                { F_1F, -3000.0f, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, }, std::string{gIceCavernPauseScreenMapFloor1LeftTex}, std::string{gIceCavernPauseScreenMapFloor1RightTex}, { { 0, 66.0f, -2.0f }, { 1, 77.0f, -46.0f }, { 2, 27.0f, -45.0f } }, {}, },
            },
            {
                { 1070, 100,  std::string{gIceCavernRoom0MinimapTex}, {}, {} },
                { 1180, -280,  std::string{gIceCavernRoom1MinimapTex}, {}, {} },
                { 1270, -690,  std::string{gIceCavernRoom2MinimapTex}, {}, {} },
                { 990, -840,  std::string{gIceCavernRoom3MinimapTex}, {}, {} },
                { 1280, -1010,  std::string{gIceCavernRoom4MinimapTex}, {}, {} },
                { 1450, -950,  std::string{gIceCavernRoom5MinimapTex}, {}, {} },
                { 1680, -730,  std::string{gIceCavernRoom6MinimapTex}, {}, {} },
                { 1530, -470,  std::string{gIceCavernRoom7MinimapTex}, { { 2, 71, 59 }, }, {} },
                { 760, -1130,  std::string{gIceCavernRoom8MinimapTex}, {}, {} },
                { 860, -1440,  std::string{gIceCavernRoom9MinimapTex}, { { 0, 48, 36 }, }, {} },
                { 1500, -420,  std::string{gIceCavernRoom10MinimapTex}, {}, {} },
                { 800, -700,  std::string{gIceCavernRoom11MinimapTex}, { { 1, 73, 67 }, }, {} },
            },
            {},
        },
        {}
    },
    { // SCENE_GANONS_TOWER
        "ganon_scene",
        "Ganon's Tower",
        "none",
        SDC_DEFAULT,
        SCENETYPE_OTHER,
        { false, false, false, false, false, false, false, false, false, false, false, false },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        {},
        {},
        {}
    },
    { // SCENE_GERUDO_TRAINING_GROUND
        "men_scene",
        "Gerudo Training Ground",
        "g_pn_11",
        SDC_GERUDO_TRAINING_GROUND,
        SCENETYPE_OTHER,
        { false, false, false, false, false, false, false, true, false, true, false, false },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        {},
        {},
        {}
    },
    { // SCENE_THIEVES_HIDEOUT
        "gerudoway_scene",
        "Thieves' Hideout",
        "g_pn_49",
        SDC_THIEVES_HIDEOUT,
        SCENETYPE_OTHER,
        { false, false, false, false, false, false, false, false, false, true, false, false },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        {},
        {},
        {}
    },
    { // SCENE_INSIDE_GANONS_CASTLE
        "ganontika_scene",
        "Inside Ganon's Castle",
        "g_pn_09",
        SDC_INSIDE_GANONS_CASTLE,
        SCENETYPE_OTHER,
        { false, false, false, false, false, false, false, true, false, true, false, false },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        {},
        {},
        {}
    },
    { // SCENE_GANONS_TOWER_COLLAPSE_INTERIOR
        "ganon_sonogo_scene",
        "Ganon's Tower (Collapsing)",
        "none",
        SDC_GANONS_TOWER_COLLAPSE_INTERIOR,
        SCENETYPE_OTHER,
        { false, false, false, false, false, false, true, false, false, true, false, false },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        {},
        {},
        {}
    },
    { // SCENE_INSIDE_GANONS_CASTLE_COLLAPSE
        "ganontikasonogo_scene",
        "Inside Ganon's Castle (Collapsing)",
        "none",
        SDC_INSIDE_GANONS_CASTLE_COLLAPSE,
        SCENETYPE_OTHER,
        { false, false, false, false, false, false, true, false, false, true, false, false },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        {},
        {},
        {}
    },
    { // SCENE_TREASURE_BOX_SHOP
        "takaraya_scene",
        "Treasure Box Shop",
        "g_pn_51",
        SDC_DEFAULT,
        SCENETYPE_OTHER,
        { false, true, false, false, false, true, true, false, false, true, true, true },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        {},
        {},
        {}
    },
    { // SCENE_DEKU_TREE_BOSS
        "ydan_boss_scene",
        "Gohma's Lair",
        "none",
        SDC_DEKU_TREE_BOSS,
        SCENETYPE_BOSS,
        { false, false, false, false, true, false, true, false, false, true, false, false },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        { "ydan_scene" },
        {},
        {}
    },
    { // SCENE_DODONGOS_CAVERN_BOSS
        "ddan_boss_scene",
        "King Dodongo's Lair",
        "none",
        SDC_DEFAULT,
        SCENETYPE_BOSS,
        { false, false, false, false, true, false, true, false, false, true, false, false },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        { "ddan_scene" },
        {},
        {}
    },
    { // SCENE_JABU_JABU_BOSS
        "bdan_boss_scene",
        "Barinade's Lair",
        "none",
        SDC_JABU_JABU,
        SCENETYPE_BOSS,
        { false, false, false, false, true, false, true, false, false, true, false, false },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        { "bdan_scene" },
        {},
        {}
    },
    { // SCENE_FOREST_TEMPLE_BOSS
        "moribossroom_scene",
        "Phantom Ganon's Lair",
        "none",
        SDC_DEFAULT,
        SCENETYPE_BOSS,
        { false, false, false, false, true, false, true, false, false, true, false, false },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        { "Bmori1_scene" },
        {},
        {}
    },
    { // SCENE_FIRE_TEMPLE_BOSS
        "FIRE_bs_scene",
        "Volvagia's Lair",
        "none",
        SDC_FIRE_TEMPLE,
        SCENETYPE_BOSS,
        { false, false, false, false, true, false, true, false, false, true, false, false },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        { "HIDAN_scene" },
        {},
        {}
    },
    { // SCENE_WATER_TEMPLE_BOSS
        "MIZUsin_bs_scene",
        "Morpha's Lair",
        "none",
        SDC_WATER_TEMPLE_BOSS,
        SCENETYPE_BOSS,
        { false, false, false, false, true, false, true, false, false, true, false, false },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        { "MIZUsin_scene" },
        {},
        {}
    },
    { // SCENE_SPIRIT_TEMPLE_BOSS
        "jyasinboss_scene",
        "Twinrova's Lair & Nabooru's Mini-Boss Room",
        "none",
        SDC_DEFAULT,
        SCENETYPE_BOSS,
        { false, false, false, false, true, false, true, false, false, true, false, false },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        { "jyasinzou_scene" },
        {},
        {}
    },
    { // SCENE_SHADOW_TEMPLE_BOSS
        "HAKAdan_bs_scene",
        "Bongo Bongo's Lair",
        "none",
        SDC_SHADOW_TEMPLE_AND_WELL,
        SCENETYPE_BOSS,
        { false, false, false, false, true, false, true, false, false, true, false, false },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        { "HAKAdan_scene" },
        {},
        {}
    },
    { // SCENE_GANONDORF_BOSS
        "ganon_boss_scene",
        "Ganondorf's Lair",
        "none",
        SDC_DEFAULT,
        SCENETYPE_BOSS,
        { false, false, false, false, true, false, true, false, false, true, false, false },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        { "" },
        {},
        {}
    },
    { // SCENE_GANONS_TOWER_COLLAPSE_EXTERIOR
        "ganon_final_scene",
        "Tower Collapse Exterior",
        "none",
        SDC_GANONS_TOWER_COLLAPSE_EXTERIOR,
        SCENETYPE_BOSS,
        { false, false, false, false, false, false, true, false, false, true, false, false },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        { "" },
        {},
        {}
    },
    { // SCENE_MARKET_ENTRANCE_DAY
        "entra_scene",
        "Market Entrance (Child - Day)",
        "none",
        SDC_DEFAULT,
        SCENETYPE_OTHER,
        { false, false, false, false, false, true, false, false, false, true, true, true },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        {},
        {},
        {}
    },
    { // SCENE_MARKET_ENTRANCE_NIGHT
        "entra_n_scene",
        "Market Entrance (Child - Night)",
        "none",
        SDC_DEFAULT,
        SCENETYPE_OTHER,
        { false, false, false, false, false, true, false, false, false, true, true, true },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        {},
        {},
        {}
    },
    { // SCENE_MARKET_ENTRANCE_RUINS
        "enrui_scene",
        "Market Entrance (Ruins)",
        "none",
        SDC_DEFAULT,
        SCENETYPE_OTHER,
        { false, false, false, false, false, true, false, false, true, true, true, true },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        {},
        {},
        {}
    },
    { // SCENE_BACK_ALLEY_DAY
        "market_alley_scene",
        "Back Alley (Child - Day)",
        "g_pn_18",
        SDC_DEFAULT,
        SCENETYPE_OTHER,
        { false, false, false, false, false, true, false, false, false, true, true, true },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        {},
        {},
        {}
    },
    { // SCENE_BACK_ALLEY_NIGHT
        "market_alley_n_scene",
        "Back Alley (Child - Night)",
        "g_pn_18",
        SDC_DEFAULT,
        SCENETYPE_OTHER,
        { false, false, false, false, false, true, false, false, false, true, true, true },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        {},
        {},
        {}
    },
    { // SCENE_MARKET_DAY
        "market_day_scene",
        "Market (Child - Day)",
        "g_pn_17",
        SDC_DEFAULT,
        SCENETYPE_OTHER,
        { false, false, false, false, false, true, false, false, false, true, true, true },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        {},
        {},
        {}
    },
    { // SCENE_MARKET_NIGHT
        "market_night_scene",
        "Market (Child - Night)",
        "g_pn_17",
        SDC_DEFAULT,
        SCENETYPE_OTHER,
        { false, false, false, false, false, true, false, false, false, true, true, true },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        {},
        {},
        {}
    },
    { // SCENE_MARKET_RUINS
        "market_ruins_scene",
        "Market (Ruins)",
        "g_pn_17",
        SDC_DEFAULT,
        SCENETYPE_OTHER,
        { false, false, false, false, false, true, false, false, true, true, true, true },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        {},
        {},
        {}
    },
    { // SCENE_TEMPLE_OF_TIME_EXTERIOR_DAY
        "shrine_scene",
        "Temple of Time Exterior (Child - Day)",
        "none",
        SDC_DEFAULT,
        SCENETYPE_OTHER,
        { false, false, false, false, false, true, false, false, false, true, true, true },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        {},
        {},
        {}
    },
    { // SCENE_TEMPLE_OF_TIME_EXTERIOR_NIGHT
        "shrine_n_scene",
        "Temple of Time Exterior (Child - Night)",
        "none",
        SDC_DEFAULT,
        SCENETYPE_OTHER,
        { false, false, false, false, false, true, false, false, false, true, true, true },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        {},
        {},
        {}
    },
    { // SCENE_TEMPLE_OF_TIME_EXTERIOR_RUINS
        "shrine_r_scene",
        "Temple of Time Exterior (Ruins)",
        "none",
        SDC_DEFAULT,
        SCENETYPE_OTHER,
        { false, false, false, false, false, true, false, false, true, true, true, true },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        {},
        {},
        {}
    },
    { // SCENE_KNOW_IT_ALL_BROS_HOUSE
        "kokiri_home_scene",
        "Know-It-All Brothers' House",
        "none",
        SDC_DEFAULT,
        SCENETYPE_OTHER,
        { false, true, false, false, false, true, false, false, false, true, true, true },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        {},
        {},
        {}
    },
    { // SCENE_TWINS_HOUSE
        "kokiri_home3_scene",
        "House of Twins",
        "none",
        SDC_DEFAULT,
        SCENETYPE_OTHER,
        { false, true, false, false, false, true, false, false, false, true, true, true },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        {},
        {},
        {}
    },
    { // SCENE_MIDOS_HOUSE
        "kokiri_home4_scene",
        "Mido's House",
        "none",
        SDC_DEFAULT,
        SCENETYPE_OTHER,
        { false, true, false, false, false, true, false, false, false, true, true, true },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        {},
        {},
        {}
    },
    { // SCENE_SARIAS_HOUSE
        "kokiri_home5_scene",
        "Saria's House",
        "none",
        SDC_DEFAULT,
        SCENETYPE_OTHER,
        { false, true, false, false, false, true, false, false, false, true, true, true },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        {},
        {},
        {}
    },
    { // SCENE_KAKARIKO_CENTER_GUEST_HOUSE
        "kakariko_scene",
        "Carpenter Boss's House",
        "none",
        SDC_DEFAULT,
        SCENETYPE_OTHER,
        { false, true, false, false, false, true, false, false, false, true, true, true },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        {},
        {},
        {}
    },
    { // SCENE_BACK_ALLEY_HOUSE
        "kakariko3_scene",
        "Back Alley House (Man in Green)",
        "none",
        SDC_DEFAULT,
        SCENETYPE_OTHER,
        { false, true, false, false, false, true, false, false, false, true, true, true },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        {},
        {},
        {}
    },
    { // SCENE_BAZAAR
        "shop1_scene",
        "Bazaar",
        "g_pn_23",
        SDC_DEFAULT,
        SCENETYPE_OTHER,
        { false, true, false, false, false, true, false, false, false, true, true, true },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        {},
        {},
        {}
    },
    { // SCENE_KOKIRI_SHOP
        "kokiri_shop_scene",
        "Kokiri Shop",
        "g_pn_19",
        SDC_DEFAULT,
        SCENETYPE_OTHER,
        { false, true, false, false, false, true, false, false, false, true, true, true },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        {},
        {},
        {}
    },
    { // SCENE_GORON_SHOP
        "golon_scene",
        "Goron Shop",
        "g_pn_20",
        SDC_DEFAULT,
        SCENETYPE_OTHER,
        { false, true, false, false, false, true, false, false, false, true, true, true },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        {},
        {},
        {}
    },
    { // SCENE_ZORA_SHOP
        "zoora_scene",
        "Zora Shop",
        "g_pn_21",
        SDC_DEFAULT,
        SCENETYPE_OTHER,
        { false, true, false, false, false, true, false, false, false, true, true, true },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        {},
        {},
        {}
    },
    { // SCENE_POTION_SHOP_KAKARIKO
        "drag_scene",
        "Kakariko Potion Shop",
        "g_pn_24",
        SDC_DEFAULT,
        SCENETYPE_OTHER,
        { false, true, false, false, false, true, false, false, false, true, true, true },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        {},
        {},
        {}
    },
    { // SCENE_POTION_SHOP_MARKET
        "alley_shop_scene",
        "Market Potion Shop",
        "g_pn_24",
        SDC_DEFAULT,
        SCENETYPE_OTHER,
        { false, true, false, false, false, true, false, false, false, true, true, true },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        {},
        {},
        {}
    },
    { // SCENE_BOMBCHU_SHOP
        "night_shop_scene",
        "Bombchu Shop",
        "g_pn_56",
        SDC_DEFAULT,
        SCENETYPE_OTHER,
        { false, true, false, false, false, true, false, false, false, true, true, true },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        {},
        {},
        {}
    },
    { // SCENE_HAPPY_MASK_SHOP
        "face_shop_scene",
        "Happy Mask Shop",
        "g_pn_50",
        SDC_DEFAULT,
        SCENETYPE_OTHER,
        { false, true, false, false, false, true, false, false, false, true, true, true },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        {},
        {},
        {}
    },
    { // SCENE_LINKS_HOUSE
        "link_home_scene",
        "Link's House",
        "none",
        SDC_DEFAULT,
        SCENETYPE_OTHER,
        { false, true, false, false, false, true, false, false, false, true, true, true },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        {},
        {},
        {}
    },
    { // SCENE_DOG_LADY_HOUSE
        "impa_scene",
        "Back Alley House (Dog Lady)",
        "none",
        SDC_DEFAULT,
        SCENETYPE_OTHER,
        { false, true, false, false, false, true, false, false, false, true, true, true },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        {},
        {},
        {}
    },
    { // SCENE_STABLE
        "malon_stable_scene",
        "Stable",
        "g_pn_48",
        SDC_DEFAULT,
        SCENETYPE_OTHER,
        { false, true, false, false, false, true, false, false, false, true, true, true },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        {},
        {},
        {}
    },
    { // SCENE_IMPAS_HOUSE
        "labo_scene",
        "Impa's House",
        "none",
        SDC_DEFAULT,
        SCENETYPE_OTHER,
        { false, true, false, false, false, true, false, false, false, true, true, true },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        {},
        {},
        {}
    },
    { // SCENE_LAKESIDE_LABORATORY
        "hylia_labo_scene",
        "Lakeside Laboratory",
        "g_pn_26",
        SDC_LAKESIDE_LABORATORY,
        SCENETYPE_OTHER,
        { false, false, false, false, false, true, false, false, false, true, true, true },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        {},
        {},
        {}
    },
    { // SCENE_CARPENTERS_TENT
        "tent_scene",
        "Carpenters' Tent",
        "none",
        SDC_DEFAULT,
        SCENETYPE_OTHER,
        { false, true, false, false, false, true, false, false, false, true, true, true },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        {},
        {},
        {}
    },
    { // SCENE_GRAVEKEEPERS_HUT
        "hut_scene",
        "Gravekeeper's Hut",
        "g_pn_25",
        SDC_DEFAULT,
        SCENETYPE_OTHER,
        { false, true, false, false, false, true, false, false, false, true, true, true },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        {},
        {},
        {}
    },
    { // SCENE_GREAT_FAIRYS_FOUNTAIN_MAGIC
        "daiyousei_izumi_scene",
        "Great Fairy's Fountain (Upgrades)",
        "g_pn_13",
        SDC_GREAT_FAIRYS_FOUNTAIN,
        SCENETYPE_OTHER,
        { false, false, false, false, false, false, false, false, false, true, false, false },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        {},
        {},
        {}
    },
    { // SCENE_FAIRYS_FOUNTAIN
        "yousei_izumi_tate_scene",
        "Fairy's Fountain",
        "g_pn_45",
        SDC_FAIRYS_FOUNTAIN,
        SCENETYPE_OTHER,
        { false, false, false, false, false, false, false, false, true, true, false, false },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        {},
        {},
        {}
    },
    { // SCENE_GREAT_FAIRYS_FOUNTAIN_SPELLS
        "yousei_izumi_yoko_scene",
        "Great Fairy's Fountain (Spells)",
        "g_pn_13",
        SDC_GREAT_FAIRYS_FOUNTAIN,
        SCENETYPE_OTHER,
        { false, false, false, false, false, false, false, false, false, true, false, false },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        {},
        {},
        {}
    },
    { // SCENE_GROTTOS
        "kakusiana_scene",
        "Grottos",
        "none",
        SDC_GROTTOS,
        SCENETYPE_OTHER,
        { false, false, false, false, false, false, false, false, true, true, false, false },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        {},
        {},
        {}
    },
    { // SCENE_REDEAD_GRAVE
        "hakaana_scene",
        "Grave (Redead)",
        "none",
        SDC_GRAVE_EXIT_LIGHT_SHINING,
        SCENETYPE_OTHER,
        { false, false, false, false, false, false, false, false, true, true, false, false },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        {},
        {},
        {}
    },
    { // SCENE_GRAVE_WITH_FAIRYS_FOUNTAIN
        "hakaana2_scene",
        "Grave (Fairy's Fountain)",
        "none",
        SDC_FAIRYS_FOUNTAIN,
        SCENETYPE_OTHER,
        { false, false, false, false, false, false, false, false, true, true, false, false },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        {},
        {},
        {}
    },
    { // SCENE_ROYAL_FAMILYS_TOMB
        "hakaana_ouke_scene",
        "Royal Family's Tomb",
        "g_pn_44",
        SDC_ROYAL_FAMILYS_TOMB,
        SCENETYPE_OTHER,
        { false, false, false, false, false, false, false, false, true, true, false, false },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        {},
        {},
        {}
    },
    { // SCENE_SHOOTING_GALLERY
        "syatekijyou_scene",
        "Shooting Gallery",
        "g_pn_15",
        SDC_SHOOTING_GALLERY,
        SCENETYPE_OTHER,
        { false, true, false, true, true, true, true, false, false, true, true, true },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        {},
        {},
        {}
    },
    { // SCENE_TEMPLE_OF_TIME
        "tokinoma_scene",
        "Temple of Time",
        "g_pn_16",
        SDC_TEMPLE_OF_TIME,
        SCENETYPE_OTHER,
        { false, false, false, false, false, true, false, false, false, true, true, true },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        {},
        {},
        {}
    },
    { // SCENE_CHAMBER_OF_THE_SAGES
        "kenjyanoma_scene",
        "Chamber of the Sages",
        "g_pn_14",
        SDC_CHAMBER_OF_THE_SAGES,
        SCENETYPE_OTHER,
        { true, true, false, true, true, true, true, false, false, true, true, true },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        {},
        {},
        {}
    },
    { // SCENE_CASTLE_COURTYARD_GUARDS_DAY
        "hairal_niwa_scene",
        "Castle Hedge Maze (Day)",
        "g_pn_12",
        SDC_CASTLE_COURTYARD_GUARDS,
        SCENETYPE_OTHER,
        { false, true, false, true, true, true, true, false, false, true, true, true },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        {},
        {},
        {}
    },
    { // SCENE_CASTLE_COURTYARD_GUARDS_NIGHT
        "hairal_niwa_n_scene",
        "Castle Hedge Maze (Night)",
        "g_pn_12",
        SDC_CASTLE_COURTYARD_GUARDS,
        SCENETYPE_OTHER,
        { false, true, false, true, true, true, true, false, false, true, true, true },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        {},
        {},
        {}
    },
    { // SCENE_CUTSCENE_MAP
        "hiral_demo_scene",
        "Cutscene Map",
        "none",
        SDC_DEFAULT,
        SCENETYPE_OTHER,
        { false, false, false, false, false, false, false, false, false, false, false, false },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        {},
        {},
        {}
    },
    { // SCENE_WINDMILL_AND_DAMPES_GRAVE
        "hakasitarelay_scene",
        "Dampé's Grave & Windmill",
        "g_pn_57",
        SDC_GRAVE_EXIT_LIGHT_SHINING,
        SCENETYPE_OTHER,
        { false, false, false, false, false, false, false, true, false, true, true, false },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        {},
        {},
        {}
    },
    { // SCENE_FISHING_POND
        "turibori_scene",
        "Fishing Pond",
        "g_pn_46",
        SDC_FISHING_POND,
        SCENETYPE_OTHER,
        { false, true, false, true, true, true, true, false, false, true, true, true },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        {},
        {},
        {}
    },
    { // SCENE_CASTLE_COURTYARD_ZELDA
        "nakaniwa_scene",
        "Castle Courtyard",
        "g_pn_12",
        SDC_CALM_WATER,
        SCENETYPE_OTHER,
        { false, false, false, false, false, false, true, false, false, true, true, false },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        {},
        {},
        {}
    },
    { // SCENE_BOMBCHU_BOWLING_ALLEY
        "bowling_scene",
        "Bombchu Bowling Alley",
        "g_pn_47",
        SDC_BOMBCHU_BOWLING_ALLEY,
        SCENETYPE_OTHER,
        { false, true, false, true, true, true, true, false, false, true, true, true },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        {},
        {},
        {}
    },
    { // SCENE_LON_LON_BUILDINGS
        "souko_scene",
        "Ranch House & Silo",
        "none",
        SDC_LON_LON_BUILDINGS,
        SCENETYPE_OTHER,
        { false, false, false, false, false, true, false, false, false, true, true, true },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        {},
        {},
        {}
    },
    { // SCENE_MARKET_GUARD_HOUSE
        "miharigoya_scene",
        "Guard House",
        "none",
        SDC_MARKET_GUARD_HOUSE,
        SCENETYPE_OTHER,
        { false, false, false, false, false, true, false, false, false, true, true, false },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        {},
        {},
        {}
    },
    { // SCENE_POTION_SHOP_GRANNY
        "mahouya_scene",
        "Granny's Potion Shop",
        "g_pn_24",
        SDC_POTION_SHOP_GRANNY,
        SCENETYPE_OTHER,
        { false, true, false, false, false, true, true, false, false, true, true, true },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        {},
        {},
        {}
    },
    { // SCENE_GANON_BOSS
        "ganon_demo_scene",
        "Ganon's Tower Collapse & Battle Arena",
        "none",
        SDC_OUTSIDE_GANONS_CASTLE,
        SCENETYPE_OTHER,
        { false, false, false, false, true, false, true, false, false, true, false, false },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        {},
        {},
        {}
    },
    { // SCENE_HOUSE_OF_SKULLTULA
        "kinsuta_scene",
        "House of Skulltula",
        "g_pn_22",
        SDC_DEFAULT,
        SCENETYPE_OTHER,
        { false, false, false, false, false, true, false, false, false, true, true, true },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        {},
        {},
        {}
    },
    { // SCENE_HYRULE_FIELD
        "spot00_scene",
        "Spot 00 - Hyrule Field",
        "g_pn_27",
        SDC_HYRULE_FIELD,
        SCENETYPE_OVERWORLD,
        { false, false, false, false, false, false, false, false, false, true, false, false },
        { true, {-460, 100, 6640}, 0,
          { { 16, 0, 1341 }, { -1297, 0, 1459 }, { -5416, -300, 1296 }, { -4667, -300, 3620 }, { -3837, 81, 5537 }, { -5093, -226, 6661 }, { -6588, -79, 5053 }, { -7072, -500, 7538 }, { -6139, -500, 8910 }, { -8497, -300, 7802 }, { -5481, -499, 12127 }, { -4808, -700, 13583 }, { -3416, -490, 12092 }, { -2915, 100, 8339 }, { -2277, -500, 13247 }, { -1026, -500, 12101 }, { 1427, -500, 13341 }, { -200, -486, 10205 }, { -1469, 100, 7496 }, { -995, 168, 5652 }, { 1938, 89, 6232 }, { 1387, -105, 9206 }, { 1571, -223, 7701 }, { 3893, -121, 7068 }, { 3179, 373, 5221 }, { 4678, -20, 3869 }, { 3460, 246, 4207 }, { 3686, 128, 2366 }, { 1791, 18, 152 }, { 3667, -16, 1399 }, { 1827, -15, 983 }, { 1574, 399, 4318 }, { 716, 95, 3391 }, { -1189, -41, 4739 }, { -1976, -171, 4172 }, { 1314, 391, 5665 }, { 112, 0, 1959 }, { -3011, -111, 9397 }, { -5674, -270, 8585 }, { -8861, -300, 7836 }, { -6056, -500, 7810 }, { -7306, -500, 5994 }, { -7305, -500, 7605 }, { -7439, -300, 7600 }, { -7464, -300, 6268 }, { -8080, -300, 7553 }, { -8091, -300, 7349 }, { -8785, -300, 7383 }, { -8745, -300, 7508 }, { -8777, -300, 7788 }, { -8048, -299, 7738 }, { -7341, -184, 7730 }, { -6410, -288, 7824 }, { -6326, -290, 8205 }, { -6546, -292, 8400 }, { -7533, -180, 8459 }, { -8024, -295, 7903 }, { -8078, -308, 7994 }, { -9425, -287, 7696 }, { -9322, -292, 7577 }, { -9602, -199, 7160 }, { -9307, -300, 7758 }, { -9230, -300, 7642 }, { -7556, -499, 8695 }, { -6438, -500, 8606 }, { -6078, -500, 8258 }, { -6233, -500, 7613 }, { -5035, -205, 7814 }, { -5971, -500, 8501 }, { -5724, -498, 10123 }, { -5094, -392, 11106 }, { -5105, -393, 11312 }, { -4477, -314, 11132 }, { -3867, -380, 11419 }, { -2952, -500, 11944 }, { -2871, -488, 11743 }, { -3829, -372, 11327 }, { -4439, -293, 10989 }, { -5014, -381, 11086 }, { -5113, -188, 10968 }, { -5269, -188, 11156 }, { -5596, -178, 9972 }, { -5801, -288, 8518 }, { -4910, -178, 7931 }, { -3586, 73, 8140 }, { -4487, -106, 9362 }, { -4339, -112, 6412 }, { -3417, 105, 8194 }, { -4505, -20, 6608 }, { -5038, -199, 6603 }, { -4481, 1, 6448 }, { -5032, -168, 6418 }, { -5256, -700, 14329 }, { -5749, -820, 15380 }, { -3122, -700, 13608 }, { -3758, -525, 13228 }, { -3670, -500, 13123 }, { -2924, -500, 13526 }, { 1389, -115, 9370 }, { 548, -116, 8889 }, { -106, -107, 8530 }, { -1608, 85, 7646 }, { -5300, -700, 13772 }, { -5114, -700, 13400 }, { -4561, -700, 13700 }, { -4762, -700, 14084 }, { -2954, 100, 8216 }, { 1460, -104, 9246 }, { 629, -105, 8791 }, { -10, -90, 8419 }, { -1462, 100, 7504 }, { -3018, -500, 12493 }, { -2994, -311, 10331 }, { -4006, -700, 14152 }, { -4341, -500, 12743 }, { -5879, -497, 6799 }, { 22, -473, 10103 }, { -1389, -192, 8874 }, { -4, 92, 6866 }, { 483, 104, 6637 }, { 1580, 183, 5987 }, { 1548, 308, 5077 }, { 1511, 399, 4267 }, { 1358, 385, 4271 }, { 1379, 395, 5063 }, { 1360, 394, 5870 }, { 813, 283, 6194 }, { -57, 101, 6743 }, { 91, 325, 5143 }, { 1425, -214, 7659 }, { 3487, -19, 880 }, { 2933, 152, 2094 }, { 2888, -145, 6862 }, { 1511, 67, 6471 }, { 4051, -47, 1722 }, { -7335, -500, 8627 }, { -7728, -462, 8498 }, { -7791, -446, 8832 }, { -2915, -435, 11334 }, { 165, -278, 3352 } } },
        { 25, 25, 1080, -360 },
        {},
        {},
        { 216, 150, 80, 73, 1, 0, -1, std::string{gHyruleFieldMinimapTex} }
    },
    { // SCENE_KAKARIKO_VILLAGE
        "spot01_scene",
        "Spot 01 - Kakariko Village",
        "g_pn_28",
        SDC_KAKARIKO_VILLAGE,
        SCENETYPE_OVERWORLD,
        { false, false, false, false, false, false, false, false, false, true, false, false },
        { false, {0, 0, 0}, 0, {} },
        { 7, 6, 1000, -650 },
        {},
        {},
        { 216, 158, 80, 64, 269, -833, 8, std::string{gKakarikoVillageMinimapTex} }
    },
    { // SCENE_GRAVEYARD
        "spot02_scene",
        "Spot 02 - Graveyard",
        "g_pn_29",
        SDC_DEFAULT,
        SCENETYPE_OVERWORLD,
        { false, false, false, false, false, false, false, false, false, true, false, false },
        { false, {0, 0, 0}, 0, {} },
        { 6, 6, 890, -800 },
        {},
        {},
        { 218, 184, 80, 39, 1, 0, 7, std::string{gGraveyardMinimapTex} }
    },
    { // SCENE_ZORAS_RIVER
        "spot03_scene",
        "Spot 03 - Zora's River",
        "g_pn_30",
        SDC_ZORAS_RIVER,
        SCENETYPE_OVERWORLD,
        { false, false, false, false, false, false, false, false, false, true, false, false },
        { false, {0, 0, 0}, 0, {} },
        { 7, 7, 720, -730 },
        {},
        {},
        { 202, 164, 96, 58, 1, 0, -1, std::string{gZorasRiverMinimapTex} }
    },
    { // SCENE_KOKIRI_FOREST
        "spot04_scene",
        "Spot 04 - Kokiri Forest",
        "g_pn_31",
        SDC_KOKIRI_FOREST,
        SCENETYPE_OVERWORLD,
        { false, false, false, false, false, false, false, false, false, true, false, false },
        { false, {0, 0, 0}, 0, {} },
        { 8, 8, 660, -730 },
        {},
        {},
        { 202, 160, 96, 62, 273, -850, 0, std::string{gKokiriForestMinimapTex} }
    },
    { // SCENE_SACRED_FOREST_MEADOW
        "spot05_scene",
        "Spot 05 - Sacred Forest Meadow",
        "g_pn_52",
        SDC_CALM_WATER,
        SCENETYPE_OVERWORLD,
        { false, false, false, false, false, false, false, false, false, true, false, false },
        { false, {0, 0, 0}, 0, {} },
        { 5, 7, 1220, -660 },
        {},
        {},
        { 250, 138, 48, 85, 279, -889, 3, std::string{gSacredMeadowMinimapTex} }
    },
    { // SCENE_LAKE_HYLIA
        "spot06_scene",
        "Spot 06 - Lake Hylia",
        "g_pn_32",
        SDC_LAKE_HYLIA,
        SCENETYPE_OVERWORLD,
        { false, false, false, false, false, false, false, false, false, true, false, false },
        { true, {-1929, -1025, 768}, 0,
          { { -2109, -882, 1724 }, { -328, -1238, 2705 }, { -3092, -1033, 3527 } } },
        { 13, 13, 1080, -260 },
        {},
        {},
        { 216, 140, 80, 81, 259, -829, 5, std::string{gLakeHyliaMinimapTex} }
    },
    { // SCENE_ZORAS_DOMAIN
        "spot07_scene",
        "Spot 07 - Zora's Domain",
        "g_pn_33",
        SDC_ZORAS_DOMAIN,
        SCENETYPE_OVERWORLD,
        { false, false, false, false, false, false, false, false, false, true, false, false },
        { false, {0, 0, 0}, 0, {} },
        { 5, 5, 1120, -880 },
        {},
        {},
        { 234, 150, 64, 73, 1, 0, -1, std::string{gZorasDomainMinimapTex} }
    },
    { // SCENE_ZORAS_FOUNTAIN
        "spot08_scene",
        "Spot 08 - Zora's Fountain",
        "g_pn_34",
        SDC_ZORAS_FOUNTAIN,
        SCENETYPE_OVERWORLD,
        { false, false, false, false, false, false, false, false, false, true, false, false },
        { false, {0, 0, 0}, 0, {} },
        { 8, 8, 1150, -630 },
        {},
        {},
        { 234, 156, 64, 65, 260, -844, 2, std::string{gZorasFountainMinimapTex} }
    },
    { // SCENE_GERUDO_VALLEY
        "spot09_scene",
        "Spot 09 - Gerudo Valley",
        "g_pn_35",
        SDC_GERUDO_VALLEY,
        SCENETYPE_OVERWORLD,
        { false, false, false, false, false, false, false, false, false, true, false, false },
        { true, {2566, -259, 767}, 0,
          { { 2687, -269, 753 }, { 2066, -132, 317 }, { 523, -8, 635 }, { 558, 36, -323 }, { 615, 51, -839 }, { -614, 32, 29 }, { -639, -3, 553 }, { -540, 10, -889 }, { -1666, 58, 378 }, { -3044, 210, -648 } } },
        { 11, 11, 1060, -680 },
        {},
        {},
        { 216, 158, 80, 65, 1, 0, -1, std::string{gGerudoValleyMinimapTex} }
    },
    { // SCENE_LOST_WOODS
        "spot10_scene",
        "Spot 10 - Lost Woods",
        "g_pn_36",
        SDC_LOST_WOODS,
        SCENETYPE_OVERWORLD,
        { false, false, false, false, false, false, false, false, false, true, false, false },
        { false, {0, 0, 0}, 0, {} },
        { 12, 12, 1100, -720 },
        {},
        {},
        { 234, 168, 64, 56, 1, 0, -1, std::string{gHauntedWastelandMinimapTex} }
    },
    { // SCENE_DESERT_COLOSSUS
        "spot11_scene",
        "Spot 11 - Desert Colossus",
        "g_pn_55",
        SDC_DESERT_COLOSSUS,
        SCENETYPE_OVERWORLD,
        { false, false, false, false, false, false, false, false, false, true, false, false },
        { false, {0, 0, 0}, 0, {} },
        { 11, 11, 930, -710 },
        {},
        {},
        { 234, 162, 64, 59, 235, -836, 6, std::string{gDesertColossusMinimapTex} }
    },
    { // SCENE_GERUDOS_FORTRESS
        "spot12_scene",
        "Spot 12 - Gerudo's Fortress",
        "g_pn_53",
        SDC_GERUDOS_FORTRESS,
        SCENETYPE_OVERWORLD,
        { false, false, false, false, false, false, false, false, false, true, false, false },
        { true, {-328, 10, 953}, 0,
          { { -678, 21, -623 }, { 149, 333, -2499 }, { 499, 581, -547 }, { 3187, 1413, -3775 }, { 3198, 1413, 307 }, { 3380, 1413, -1200 }, { -966, 1, -56 }, { -966, 24, -761 }, { -694, 174, -2820 } } },
        { 9, 9, 850, -830 },
        {},
        {},
        { 216, 138, 80, 85, 1, 0, 11, std::string{gGerudosFortessMinimapTex} }
    },
    { // SCENE_HAUNTED_WASTELAND
        "spot13_scene",
        "Spot 13 - Haunted Wasteland",
        "g_pn_37",
        SDC_HAUNTED_WASTELAND,
        SCENETYPE_OVERWORLD,
        { false, false, false, false, false, false, false, false, false, true, false, false },
        { false, {0, 0, 0}, 0, {} },
        { 15, 15, 1100, -780 },
        {},
        {},
        { 234, 168, 64, 56, 1, 0, -1, std::string{gLostWoodsMinimapTex} }
    },
    { // SCENE_HYRULE_CASTLE
        "spot15_scene",
        "Spot 15 - Hyrule Castle",
        "g_pn_38",
        SDC_HYRULE_CASTLE,
        SCENETYPE_OVERWORLD,
        { false, false, false, false, false, false, false, false, false, true, false, false },
        { false, {0, 0, 0}, 0, {} },
        { 10, 10, 1030, -540 },
        {},
        {},
        { 234, 162, 64, 59, 1, 0, -1, std::string{gHyruleCastleAreaMinimapTex} }
    },
    { // SCENE_DEATH_MOUNTAIN_TRAIL
        "spot16_scene",
        "Spot 16 - Death Mountain Trail",
        "g_pn_39",
        SDC_DEATH_MOUNTAIN_TRAIL,
        SCENETYPE_OVERWORLD,
        { false, false, false, false, false, false, false, false, false, true, false, false },
        { false, {0, 0, 0}, 0, {} },
        { 10, 9, 1240, -700 },
        {},
        {},
        { 250, 138, 48, 85, 267, -852, 1, std::string{gDeathMountainTrailMinimapTex} }
    },
    { // SCENE_DEATH_MOUNTAIN_CRATER
        "spot17_scene",
        "Spot 17 - Death Mountain Crater",
        "g_pn_40",
        SDC_DEATH_MOUNTAIN_CRATER,
        SCENETYPE_OVERWORLD,
        { false, false, false, false, false, false, false, false, false, true, false, false },
        { false, {0, 0, 0}, 0, {} },
        { 6, 6, 1030, -620 },
        {},
        {},
        { 216, 146, 80, 78, 261, -873, 4, std::string{gDeathMountainCraterMinimapTex} }
    },
    { // SCENE_GORON_CITY
        "spot18_scene",
        "Spot 18 - Goron City",
        "g_pn_41",
        SDC_GORON_CITY,
        SCENETYPE_OVERWORLD,
        { false, false, false, false, false, false, false, false, false, true, false, false },
        { false, {0, 0, 0}, 0, {} },
        { 5, 5, 1100, -660 },
        {},
        {},
        { 234, 150, 64, 72, 1, 0, -1, std::string{gGoronCityMinimapTex} }
    },
    { // SCENE_LON_LON_RANCH
        "spot20_scene",
        "Spot 20 - Lon Lon Ranch",
        "g_pn_42",
        SDC_LON_LON_RANCH,
        SCENETYPE_OVERWORLD,
        { false, false, false, false, false, false, false, false, false, true, false, false },
        { true, {928, 0, -2280}, 0,
          { { 1039, 0, 2051 }, { -1443, 0, 1429 }, { 856, 0, -918 }, { 882, 0, -2256 }, { -1003, 0, -755 }, { -2254, 0, -629 }, { 907, 0, -2336 } } },
        { 11, 11, 1120, -750 },
        {},
        {},
        { 234, 154, 64, 68, 1, 0, -1, std::string{gLonLonRanchMinimapTex} }
    },
    { // SCENE_OUTSIDE_GANONS_CASTLE
        "ganon_tou_scene",
        "Ganon's Castle Exterior",
        "g_pn_43",
        SDC_OUTSIDE_GANONS_CASTLE,
        SCENETYPE_OTHER,
        { false, false, false, false, false, false, false, false, false, true, false, false },
        { false, {0, 0, 0}, 0, {} },
        { 10, 10, 1070, -580 },
        {},
        {},
        {}
    },
    { // SCENE_TEST01
        "test01_scene",
        "Jungle Gym",
        "none",
        SDC_CALM_WATER,
        SCENETYPE_OTHER,
        { false, false, false, false, false, false, false, false, false, false, false, false },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        {},
        {},
        {}
    },
    { // SCENE_BESITU
        "besitu_scene",
        "Ganondorf Test Room",
        "none",
        SDC_BESITU,
        SCENETYPE_OTHER,
        { false, false, false, false, false, false, false, false, false, false, false, false },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        {},
        {},
        {}
    },
    { // SCENE_DEPTH_TEST
        "depth_test_scene",
        "Depth Test",
        "none",
        SDC_DEFAULT,
        SCENETYPE_OTHER,
        { false, false, false, false, false, false, false, false, false, false, false, false },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        {},
        {},
        {}
    },
    { // SCENE_SYOTES
        "syotes_scene",
        "Stalfos Mini-Boss Room",
        "none",
        SDC_DEFAULT,
        SCENETYPE_OTHER,
        { false, false, false, false, false, false, false, false, false, false, false, false },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        {},
        {},
        {}
    },
    { // SCENE_SYOTES2
        "syotes2_scene",
        "Stalfos Boss Room",
        "none",
        SDC_DEFAULT,
        SCENETYPE_OTHER,
        { false, false, false, false, false, false, false, false, false, false, false, false },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        {},
        {},
        {}
    },
    { // SCENE_SUTARU
        "sutaru_scene",
        "Sutaru",
        "none",
        SDC_DEFAULT,
        SCENETYPE_OTHER,
        { false, false, false, false, false, false, false, false, false, false, false, false },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        {},
        {},
        {}
    },
    { // SCENE_HAIRAL_NIWA2
        "hairal_niwa2_scene",
        "Castle Hedge Maze (Early)",
        "g_pn_12",
        SDC_CASTLE_COURTYARD_GUARDS,
        SCENETYPE_OTHER,
        { false, false, false, false, false, false, false, false, false, false, false, false },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        {},
        {},
        {}
    },
    { // SCENE_SASATEST
        "sasatest_scene",
        "Sasa Test",
        "none",
        SDC_DEFAULT,
        SCENETYPE_OTHER,
        { false, false, false, false, false, false, false, false, false, false, false, false },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        {},
        {},
        {}
    },
    { // SCENE_TESTROOM
        "testroom_scene",
        "Treasure Chest Room",
        "none",
        SDC_DEFAULT,
        SCENETYPE_OTHER,
        { false, false, false, false, false, false, false, false, false, false, false, false },
        { false, {0, 0, 0}, 0, {} },
        { 0, 0, 0, 0 },
        {},
        {},
        {}
    },
};

struct EntranceTableInit {
    std::string name;
    std::string desc;
    s32 scene;
    s32 spawn;
    bool continueBgm;
    bool displayTitleCard;
    u8 endTransition;
    u8 startTransition;
};

#define DEFINE_ENTRANCE(enumName, sceneId, spawn, continueBgm, displayTitleCard, endTransType, startTransType) \
    { #enumName, "", sceneId, spawn, continueBgm, displayTitleCard, endTransType, startTransType },

// TODO consistent name with initDB
static EntranceTableInit sEntranceTableInit[] = {
#include "tables/entrance_table.h"
};

#undef DEFINE_ENTRANCE

SceneDB::SceneDB() {
    db.reserve(SCENE_ID_MAX); // reserve size for all initial entries so we don't do it for each
    for (const SceneDBInit& init : initDB) {
        AddEntry(init);
    }

    SceneDB::Entry& entry = RetrieveEntry(SCENE_BOTTOM_OF_THE_WELL);
    entry.entry.epona.allowed = true;
    entry.SetEponaSpawnPos({ { -630, 0, 53} });
}

// Adds an scene at the given index. The name must be unique.
SceneDB::Entry& SceneDB::AddEntry(const std::string& name, const std::string& desc, size_t index) {
    assert(!nameTable.contains(name)); // TODO this should maybe throw instead. We'll need to think about error handling
                                       // for mods that try to declare the same scene.

    if (db.size() < (index + 1)) {
        db.resize(index + 1);
    }
    Entry& newEntry = db.at(index);
    newEntry.entry.id = index;

    assert(!newEntry.entry.valid);

    nextFreeId = std::max(nextFreeId, index + 1);

    nameTable[name] = newEntry.entry.id;
    newEntry.SetName(name);
    newEntry.SetDesc(desc);

    newEntry.entry.valid = true;

    return newEntry;
}

// Adds an scene with the new SceneDBInit struct. The id assigned to the scene is dynamic. Use the returned Entry or
// RetrieveId to get it.
SceneDB::Entry& SceneDB::AddEntry(const SceneDBInit& init) {
    Entry& entry = AddEntry(init.name, init.desc, nextFreeId);

    entry.SetTitleCard(init.titleCard);

    entry.entry.sceneDrawConfig = init.sceneDrawConfig;
    entry.entry.type = init.type;
    entry.entry.restrictions.hGauge = init.restrictions.hGauge;
    entry.entry.restrictions.bButton = init.restrictions.bButton;
    entry.entry.restrictions.aButton = init.restrictions.aButton;
    entry.entry.restrictions.bottles = init.restrictions.bottles;
    entry.entry.restrictions.tradeItems = init.restrictions.tradeItems;
    entry.entry.restrictions.hookshot = init.restrictions.hookshot;
    entry.entry.restrictions.ocarina = init.restrictions.ocarina;
    entry.entry.restrictions.warpSongs = init.restrictions.warpSongs;
    entry.entry.restrictions.sunsSong = init.restrictions.sunsSong;
    entry.entry.restrictions.farores = init.restrictions.farores;
    entry.entry.restrictions.dinsNayrus = init.restrictions.dinsNayrus;
    entry.entry.restrictions.all = init.restrictions.all;
    entry.entry.epona.allowed = init.epona.allowed;
    entry.entry.epona.startingPos = init.epona.startingPos;
    entry.entry.epona.angle = init.epona.angle;
    entry.SetEponaSpawnPos(init.epona.spawnPos);

    entry.entry.compassInfo.scaleX = init.compassInfo.scaleX;
    entry.entry.compassInfo.scaleY = init.compassInfo.scaleY;
    entry.entry.compassInfo.x = init.compassInfo.x;
    entry.entry.compassInfo.y = init.compassInfo.y;

    if (init.bossData.mapScene != "") {
        int id = RetrieveId(init.bossData.mapScene);
        assert(id != -1); // TODO error of some kind
        entry.entry.bossData.mapScene = id;
    } else {
        entry.entry.bossData.mapScene = -1;
    }
    entry.entry.dungeonData.bossFloor = init.dungeonData.bossFloor;
    entry.SetDungeonPalettes(init.dungeonData.palettes);
    entry.SetDungeonNameTextures(init.dungeonData.nameEngTexture, init.dungeonData.nameGerTexture, init.dungeonData.nameFraTexture);
    entry.SetDungeonFloors(init.dungeonData.floors);
    entry.SetDungeonRooms(init.dungeonData.rooms);
    entry.SetDungeonIntraRoomTransitions(init.dungeonData.intraRoomTransitions);

    entry.entry.worldData.minimapX = init.worldData.minimapX;
    entry.entry.worldData.minimapY = init.worldData.minimapY;
    entry.entry.worldData.minimapWidth = init.worldData.minimapWidth;
    entry.entry.worldData.minimapHeight = init.worldData.minimapHeight;
    entry.entry.worldData.iconX = init.worldData.iconX;
    entry.entry.worldData.iconY = init.worldData.iconY;
    entry.entry.worldData.entranceFlag = init.worldData.entranceFlag;
    entry.SetWorldMinimapTexture(init.worldData.minimapTexture);

    return entry;
}

// Get the SceneDB::Entry for the given scene id.
SceneDB::Entry& SceneDB::RetrieveEntry(const int id) {
    static Entry invalid;
    if ((id < 0) || (id >= db.size())) {
        return invalid;
    }
    return db[id];
}

// Get the id for a given scene by name.
int SceneDB::RetrieveId(const std::string& name) {
    auto entry = nameTable.find(name);
    if (entry == nameTable.end()) {
        return -1;
    }

    return entry->second;
}

// Get the number of scene entries
size_t SceneDB::GetNumEntries() {
    return db.size();
}

SceneDB::Entry::Entry() {
    entry.name = nullptr;
    entry.desc = nullptr;
    entry.valid = false;
    entry.id = 0;
    entry.titleCard = nullptr;
    entry.sceneDrawConfig = SDC_DEFAULT;
    entry.type = SCENETYPE_OTHER;
    entry.restrictions.hGauge = false;
    entry.restrictions.bButton = false;
    entry.restrictions.aButton = false;
    entry.restrictions.bottles = false;
    entry.restrictions.tradeItems = false;
    entry.restrictions.hookshot = false;
    entry.restrictions.ocarina = false;
    entry.restrictions.warpSongs = false;
    entry.restrictions.sunsSong = false;
    entry.restrictions.farores = false;
    entry.restrictions.dinsNayrus = false;
    entry.restrictions.all = false;
    entry.epona.allowed = false;
    entry.epona.startingPos = { 0, 0, 0 };
    entry.epona.spawnPos = {};
    entry.epona.angle = 0;
    entry.compassInfo.scaleX = 0;
    entry.compassInfo.scaleY = 0;
    entry.compassInfo.x = 0;
    entry.compassInfo.y = 0;
    entry.bossData.mapScene = -1;
    entry.dungeonData.bossFloor = -1;
    entry.dungeonData.palettes = nullptr;
    entry.dungeonData.numPalettes = 0;
    for (size_t i = 0; i < 8; i++) {
        entry.dungeonData.floors[i].id = static_cast<FloorID>(0);
        entry.dungeonData.floors[i].height = 0.0f;
        entry.dungeonData.floors[i].rooms = nullptr;
        entry.dungeonData.floors[i].numRooms = 0;
        entry.dungeonData.floors[i].mapLeftTexture = nullptr;
        entry.dungeonData.floors[i].mapRightTexture = nullptr;
    }
    entry.dungeonData.rooms = nullptr;
    entry.dungeonData.numRooms = 0;
    entry.dungeonData.intraRoomTransitions = nullptr;
    entry.dungeonData.numIntraRoomTransitions = 0;
    entry.worldData.minimapX = 0;
    entry.worldData.minimapY = 0;
    entry.worldData.minimapWidth = 0;
    entry.worldData.minimapHeight = 0;
    entry.worldData.iconX = 0;
    entry.worldData.iconY = 0;
    entry.worldData.entranceFlag = 0;
    entry.worldData.minimapTexture = nullptr;
}

SceneDB::Entry::Entry(const Entry& other) {
    entry = other.entry;
    SetName(other.name);
    SetDesc(other.desc);
    SetTitleCard(other.titleCard);
    SetEponaSpawnPos(other.eponaSpawnPos);
    SetDungeonPalettes(other.dungeonPalettes);
    SetDungeonNameTextures(other.nameEngTexture, other.nameGerTexture, other.nameFraTexture);
    SetDungeonFloors(other.dungeonFloors, other.dungeonFloorInfo);
    SetDungeonRooms(other.dungeonRooms, other.dungeonRoomInfo);
    SetDungeonIntraRoomTransitions(other.dungeonIntraRoomTransitions);
    SetWorldMinimapTexture(other.worldMinimapTexture);
}

SceneDB::Entry& SceneDB::Entry::operator=(const Entry& other) {
    entry = other.entry;
    SetName(other.name);
    SetDesc(other.desc);
    SetTitleCard(other.titleCard);
    SetEponaSpawnPos(other.eponaSpawnPos);
    SetDungeonPalettes(other.dungeonPalettes);
    SetDungeonNameTextures(other.nameEngTexture, other.nameGerTexture, other.nameFraTexture);
    SetDungeonFloors(other.dungeonFloors, other.dungeonFloorInfo);
    SetDungeonRooms(other.dungeonRooms, other.dungeonRoomInfo);
    SetDungeonIntraRoomTransitions(other.dungeonIntraRoomTransitions);
    SetWorldMinimapTexture(other.worldMinimapTexture);
    return *this;
}

void SceneDB::Entry::SetName(const std::string& newName) {
    name = newName;
    entry.name = name.c_str();
}

void SceneDB::Entry::SetDesc(const std::string& newDesc) {
    desc = newDesc;
    entry.desc = desc.c_str();
}

void SceneDB::Entry::SetTitleCard(const std::string& newTitleCard) {
    titleCard = newTitleCard;
    entry.titleCard = titleCard.c_str();
}

void SceneDB::Entry::SetEponaSpawnPos(const std::vector<Vec3s>& newSpawnPos) {
    eponaSpawnPos = newSpawnPos;
    entry.epona.spawnPos = eponaSpawnPos.data();
    entry.epona.numSpawns = eponaSpawnPos.size();
}

void SceneDB::Entry::SetDungeonPalettes(const std::vector<s16>& newDungeonPalettes) {
    dungeonPalettes = newDungeonPalettes;
    entry.dungeonData.palettes = dungeonPalettes.data();
    entry.dungeonData.numPalettes = dungeonPalettes.size();
}

void SceneDB::Entry::SetDungeonNameTextures(const std::string& newNameEngTexture, const std::string& newNameGerTexture, const std::string& newNameFraTexture) {
    nameEngTexture = newNameEngTexture;
    nameGerTexture = newNameGerTexture;
    nameFraTexture = newNameFraTexture;
    entry.dungeonData.nameEngTexture = nameEngTexture.c_str();
    entry.dungeonData.nameGerTexture = nameGerTexture.c_str();
    entry.dungeonData.nameFraTexture = nameFraTexture.c_str();
}

void SceneDB::Entry::SetDungeonFloors(const std::vector<SceneDBInit::FloorInit>& newDungeonFloors) {
    assert(newDungeonFloors.size() == 8 || newDungeonFloors.size() == 0);
    dungeonFloors.reserve(newDungeonFloors.size());
    dungeonFloorInfo.reserve(newDungeonFloors.size());

    for (size_t i = 0; i < newDungeonFloors.size(); i++) {
        dungeonFloors.emplace_back(static_cast<FloorID>(newDungeonFloors[i].id), newDungeonFloors[i].height, nullptr, 0, nullptr, nullptr, nullptr, 0, nullptr, 0);
        dungeonFloorInfo.emplace_back(newDungeonFloors[i].palettes, newDungeonFloors[i].mapLeftTexture, newDungeonFloors[i].mapRightTexture, newDungeonFloors[i].chestMarks, newDungeonFloors[i].bossMarks);
    }

    for (size_t i = 0; i < dungeonFloors.size(); i++) {
        dungeonFloors[i].rooms = dungeonFloorInfo[i].rooms.data();
        dungeonFloors[i].numRooms = dungeonFloorInfo[i].rooms.size();
        dungeonFloors[i].mapLeftTexture = dungeonFloorInfo[i].mapLeftTexture.c_str();
        dungeonFloors[i].mapRightTexture = dungeonFloorInfo[i].mapRightTexture.c_str();
        dungeonFloors[i].numChestMarks = dungeonFloorInfo[i].chestMarks.size();
        dungeonFloors[i].chestMarks = dungeonFloorInfo[i].chestMarks.data();
        dungeonFloors[i].numBossMarks = dungeonFloorInfo[i].bossMarks.size();
        dungeonFloors[i].bossMarks = dungeonFloorInfo[i].bossMarks.data();
    }
    std::copy(dungeonFloors.begin(), dungeonFloors.end(), entry.dungeonData.floors);
}

void SceneDB::Entry::SetDungeonFloors(const std::vector<SceneDBFloor>& newDungeonFloors, const std::vector<FloorInfo>& newDungeonFloorInfo) {
    assert(newDungeonFloors.size() == 8 || newDungeonFloors.size() == 0);
    assert(newDungeonFloorInfo.size() == 8 || newDungeonFloorInfo.size() == 0);
    assert(newDungeonFloors.size() == newDungeonFloorInfo.size());
    dungeonFloors = newDungeonFloors;
    dungeonFloorInfo = newDungeonFloorInfo;
    for (size_t i = 0; i < dungeonFloors.size(); i++) {
        dungeonFloors[i].rooms = dungeonFloorInfo[i].rooms.data();
        dungeonFloors[i].numRooms = dungeonFloorInfo[i].rooms.size();
        dungeonFloors[i].mapLeftTexture = dungeonFloorInfo[i].mapLeftTexture.c_str();
        dungeonFloors[i].mapRightTexture = dungeonFloorInfo[i].mapRightTexture.c_str();
        dungeonFloors[i].numChestMarks = dungeonFloorInfo[i].chestMarks.size();
        dungeonFloors[i].chestMarks = dungeonFloorInfo[i].chestMarks.data();
        dungeonFloors[i].numBossMarks = dungeonFloorInfo[i].bossMarks.size();
        dungeonFloors[i].bossMarks = dungeonFloorInfo[i].bossMarks.data();
    }
    std::copy(dungeonFloors.begin(), dungeonFloors.end(), entry.dungeonData.floors);
}

void SceneDB::Entry::SetDungeonRooms(const std::vector<SceneDBInit::RoomInit>& newDungeonRooms) {
    dungeonRooms.reserve(newDungeonRooms.size());
    dungeonRoomInfo.reserve(newDungeonRooms.size());

    for (size_t i = 0; i < newDungeonRooms.size(); i++) {
        dungeonRooms.emplace_back(newDungeonRooms[i].compassOffsetX, newDungeonRooms[i].compassOffsetY, nullptr, nullptr, 0, nullptr, 0);
        dungeonRoomInfo.emplace_back(newDungeonRooms[i].minimapTexture, newDungeonRooms[i].chestMarks, newDungeonRooms[i].bossMarks);
    }

    for (size_t i = 0; i < dungeonRooms.size(); i++) {
        dungeonRooms[i].minimapTexture = dungeonRoomInfo[i].minimapTexture.c_str();
        dungeonRooms[i].chestMarks = dungeonRoomInfo[i].chestMarks.data();
        dungeonRooms[i].numChestMarks = dungeonRoomInfo[i].chestMarks.size();
        dungeonRooms[i].bossMarks = dungeonRoomInfo[i].bossMarks.data();
        dungeonRooms[i].numBossMarks = dungeonRoomInfo[i].bossMarks.size();
    }
    entry.dungeonData.rooms = dungeonRooms.data();
    entry.dungeonData.numRooms = dungeonRooms.size();
}

void SceneDB::Entry::SetDungeonRooms(const std::vector<SceneDBRoom>& newDungeonRooms, const std::vector<RoomInfo>& newDungeonRoomInfo) {
    dungeonRooms = newDungeonRooms;
    dungeonRoomInfo = newDungeonRoomInfo;
    for (size_t i = 0; i < dungeonRooms.size(); i++) {
        dungeonRooms[i].minimapTexture = dungeonRoomInfo[i].minimapTexture.c_str();
        dungeonRooms[i].chestMarks = dungeonRoomInfo[i].chestMarks.data();
        dungeonRooms[i].numChestMarks = dungeonRoomInfo[i].chestMarks.size();
        dungeonRooms[i].bossMarks = dungeonRoomInfo[i].bossMarks.data();
        dungeonRooms[i].numBossMarks = dungeonRoomInfo[i].bossMarks.size();
    }
    entry.dungeonData.rooms = dungeonRooms.data();
    entry.dungeonData.numRooms = dungeonRooms.size();
}

void SceneDB::Entry::SetDungeonIntraRoomTransitions(const std::vector<SceneDBInit::IntraRoomTransitionInit>& newDungeonIntraRoomTransitions) {
    dungeonIntraRoomTransitions.reserve(newDungeonIntraRoomTransitions.size());
    for (size_t i = 0; i < newDungeonIntraRoomTransitions.size(); i++) {
        dungeonIntraRoomTransitions.emplace_back(newDungeonIntraRoomTransitions[i].fromRoom, newDungeonIntraRoomTransitions[i].toRoom, newDungeonIntraRoomTransitions[i].toFloor);
    }
    entry.dungeonData.intraRoomTransitions = dungeonIntraRoomTransitions.data();
    entry.dungeonData.numIntraRoomTransitions = dungeonIntraRoomTransitions.size();
}

void SceneDB::Entry::SetDungeonIntraRoomTransitions(const std::vector<SceneDBIntraRoomTransition>& newDungeonIntraRoomTransitions) {
    dungeonIntraRoomTransitions = newDungeonIntraRoomTransitions;
    entry.dungeonData.intraRoomTransitions = dungeonIntraRoomTransitions.data();
    entry.dungeonData.numIntraRoomTransitions = dungeonIntraRoomTransitions.size();
}

void SceneDB::Entry::SetWorldMinimapTexture(const std::string& newWorldMinimapTexture) {
    worldMinimapTexture = newWorldMinimapTexture;
    entry.worldData.minimapTexture = worldMinimapTexture == "" ? nullptr : worldMinimapTexture.c_str(); // TODO all?
}

bool SceneDB::Entry::isBoss() {
    return this->entry.bossData.mapScene != -1;
}

bool SceneDB::Entry::isDungeon() {
    return this->entry.dungeonData.numRooms != 0;
}

bool SceneDB::Entry::isOverworld() {
    return this->entry.worldData.minimapTexture != nullptr;
}


EntranceDB::EntranceDB() {
    db.reserve(ENTR_MAX); // reserve size for all initial entries so we don't do it for each
    for (const EntranceTableInit& init : sEntranceTableInit) {
        // TODO remove code duplication
        Entry& entry = AddEntry(init.name, init.desc, nextFreeId);
        // TODO handle errors
        entry.entry.sceneId = SceneDB::Instance->RetrieveEntry(init.scene).entry.id;
        entry.entry.spawn = init.spawn;
        entry.entry.continueBgm = init.continueBgm;
        entry.entry.displayTitleCard = init.displayTitleCard;
        entry.entry.endTransition = init.endTransition;
        entry.entry.startTransition = init.startTransition;

        // TODO comment
        NextLayerLookupKey nextLayerKey{ entry.entry.sceneId, entry.entry.spawn };
        auto nextLayer = nextLayerLookupTable.find(nextLayerKey);
        if (nextLayer == nextLayerLookupTable.end()) {
            entry.entry.layer = 0;
            nextLayerLookupTable.insert({ nextLayerKey, 1 });
        } else {
            entry.entry.layer = nextLayer->second++;
        }

        idLookupTable.insert({ { entry.entry.sceneId, entry.entry.spawn, entry.entry.layer }, entry.entry.id });
    }
}

EntranceDB::Entry& EntranceDB::AddEntry(const EntranceDBInit& init) {
    Entry& entry = AddEntry(init.name, init.desc, nextFreeId);
    // TODO handle errors
    entry.entry.sceneId = SceneDB::Instance->RetrieveEntry(SceneDB::Instance->RetrieveId(init.scene)).entry.id;
    entry.entry.spawn = init.spawn;
    entry.entry.continueBgm = init.continueBgm;
    entry.entry.displayTitleCard = init.displayTitleCard;
    entry.entry.endTransition = init.endTransition;
    entry.entry.startTransition = init.startTransition;

    // TODO comment
    NextLayerLookupKey nextLayerKey{ entry.entry.sceneId, entry.entry.spawn };
    auto nextLayer = nextLayerLookupTable.find(nextLayerKey);
    if (nextLayer == nextLayerLookupTable.end()) {
        entry.entry.layer = 0;
        nextLayerLookupTable.insert({ nextLayerKey, 1 });
    } else {
        entry.entry.layer = nextLayer->second++;
    }

    idLookupTable.insert({ { entry.entry.sceneId, entry.entry.spawn, entry.entry.layer }, entry.entry.id });

    return entry;
}

EntranceDB::Entry& EntranceDB::RetrieveEntry(const int id) {
    static Entry invalid;
    if ((id < 0) || (id >= db.size())) {
        return invalid;
    }
    return db[id];
}

EntranceDB::Entry& EntranceDB::RetrieveEntry(const int id, const int layer) {
    Entry& entry = RetrieveEntry(id);
    // TODO assert
    return RetrieveEntry(CalcId(entry.entry.sceneId, entry.entry.spawn, layer));
}

int EntranceDB::RetrieveId(const std::string& name) {
    auto entry = nameTable.find(name);
    if (entry == nameTable.end()) {
        return -1;
    }

    return entry->second;
}

size_t EntranceDB::GetNumEntries() {
    return db.size();
}

s32 EntranceDB::CalcId(const s32 sceneId, const s32 spawn, const s32 layer) {
    auto entrance = idLookupTable.find({ sceneId, spawn, layer });
    // TODO assert
    return entrance->second;
}

s32 EntranceDB::CalcId(const s32 entrance, const s32 newLayer) {
    Entry& entry = RetrieveEntry(entrance);
    auto newEntrance = idLookupTable.find({ entry.entry.sceneId, entry.entry.spawn, newLayer });
    // TODO assert
    return newEntrance->second;
}

EntranceDB::Entry& EntranceDB::AddEntry(const std::string& name, const std::string& desc, size_t index) {
    assert(!nameTable.contains(name)); // TODO this should maybe throw instead. We'll need to think about error handling
                                       // for mods that try to declare the same scene.

    if (db.size() < (index + 1)) {
        db.resize(index + 1);
    }
    Entry& newEntry = db.at(index);
    newEntry.entry.id = index;

    assert(!newEntry.entry.valid);

    nextFreeId = std::max(nextFreeId, index + 1);

    nameTable[name] = newEntry.entry.id;
    newEntry.SetName(name);
    newEntry.SetDesc(desc);

    newEntry.entry.valid = true;

    return newEntry;
}

EntranceDB::Entry::Entry() {
    entry.name = nullptr;
    entry.desc = nullptr;
    entry.valid = false;
    entry.id = 0;
    entry.sceneId = 0;
    entry.spawn = 0;
    entry.layer = 0;
    entry.continueBgm = false;
    entry.displayTitleCard = false;
    entry.endTransition = 0;
    entry.startTransition = 0;
}

EntranceDB::Entry::Entry(const Entry& other) {
    entry = other.entry;
    SetName(other.name);
    SetDesc(other.desc);
}

EntranceDB::Entry& EntranceDB::Entry::operator=(const Entry& other) {
    entry = other.entry;
    SetName(other.name);
    SetDesc(other.desc);
    return *this;
}

void EntranceDB::Entry::SetName(const std::string& newName) {
    name = newName;
    entry.name = name.c_str();
}

void EntranceDB::Entry::SetDesc(const std::string& newDesc) {
    desc = newDesc;
    entry.desc = desc.c_str();
}

extern "C" SceneDBEntry * SceneDB_Retrieve(const int id) {
    return &SceneDB::Instance->RetrieveEntry(id).entry;
}

extern "C" int SceneDB_RetrieveId(const char* name) {
    return SceneDB::Instance->RetrieveId(name);
}

extern "C" int SceneRB_IsBoss(const int id) {
    return SceneDB::Instance->RetrieveEntry(id).isBoss();
}

extern "C" int SceneRB_IsDungeon(const int id) {
    return SceneDB::Instance->RetrieveEntry(id).isDungeon();
}

extern "C" int SceneRB_IsOverworld(const int id) {
    return SceneDB::Instance->RetrieveEntry(id).isOverworld();
}

extern "C" EntranceDBEntry* EntranceDB_Retrieve(const int id) {
    return &EntranceDB::Instance->RetrieveEntry(id).entry;
}

extern "C" EntranceDBEntry* EntranceDB_RetrieveLayer(const int id, const int layer) {
    return &EntranceDB::Instance->RetrieveEntry(id, layer).entry;
}

extern "C" int EntranceDB_RetrieveId(const char* name) {
    return EntranceDB::Instance->RetrieveId(name);
}

extern "C" int EntranceDB_CalcId(const int sceneId, const int spawn, const int layer) {
    return EntranceDB::Instance->CalcId(sceneId, spawn, layer);
}

extern "C" int EntranceDB_CalcIdWithEntrance(const int entrance, const int newLayer) {
    return EntranceDB::Instance->CalcId(entrance, newLayer);
}
