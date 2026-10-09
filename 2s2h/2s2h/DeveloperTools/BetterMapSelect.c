
#include "BetterMapSelect.h"
#include "overlays/gamestates/ovl_file_choose/z_file_select.h"
#include "overlays/gamestates/ovl_select/z_select.h"
#include <libultraship/bridge/consolevariablebridge.h>

void BetterMapSelect_LoadGame(MapSelectState* mapSelectState, u32 entrance, s32 spawn);
void BetterMapSelect_LoadFileSelect(MapSelectState* mapSelectState);

// 2S2H Added columns to scene table: entranceSceneId, betterMapSelectIndex, humanName
#define DEFINE_SCENE(_name, _enumValue, _textId, _drawConfig, _restrictionFlags, _persistentCycleFlags, \
                     entranceSceneId, _betterMapSelectIndex, humanName)                                 \
    { humanName, BetterMapSelect_LoadGame, ENTRANCE(entranceSceneId, 0) },
#define DEFINE_SCENE_UNSET(_enumValue)

static SceneSelectEntry sBetterScenes[106] = {
#include "tables/scene_table.h"
    { "Chest Grottos", BetterMapSelect_LoadGame, ENTRANCE(GROTTOS, 4) },
    { "Cow Grottos", BetterMapSelect_LoadGame, ENTRANCE(GROTTOS, 10) },
    { "File Select", BetterMapSelect_LoadFileSelect, 0 },
    { "Title Screen", MapSelect_LoadConsoleLogo, 0 },
};

#undef DEFINE_SCENE
#undef DEFINE_SCENE_UNSET

// 2S2H Added columns to scene table: entranceSceneId, betterMapSelectIndex, humanName
#define DEFINE_SCENE(_name, _enumValue, _textId, _drawConfig, _restrictionFlags, _persistentCycleFlags, \
                     entranceSceneId, betterMapSelectIndex, humanName)                                  \
    { humanName, ENTRANCE(entranceSceneId, 0), betterMapSelectIndex },
#define DEFINE_SCENE_UNSET(_enumValue)

typedef struct {
    char* name;
    s32 entrance;
    s32 index;
} BetterMapSelectInfoEntry;

static BetterMapSelectInfoEntry sBetterMapSelectInfo[102] = {
#include "tables/scene_table.h"
};

#undef DEFINE_SCENE
#undef DEFINE_SCENE_UNSET

#define STAGE_CURRENT_TIME 0x9000

extern SceneSelectEntry sScenes[143];
extern SceneEntranceTableEntry sSceneEntranceTable[ENTR_SCENE_MAX];

static bool sIsBetterMapSelectEnabled = false;

typedef struct {
    u16 entrance;
    s16 roomIndex;
    s16 data;
    s16 yaw;
    s16 playerParams;
    Vec3f pos;
} BetterMapSelectRespawnInfo;

typedef struct {
    BetterMapSelectRespawnInfo grottoRespawn;
    BetterMapSelectRespawnInfo downRespawn;
} BetterMapSelectGrottoRespawnInfo;

#define UNUSED_GROTTO_RESPAWN                                           \
    {                                                                   \
        0, 0, 0xFF, 0, PLAYER_PARAMS(0xFF, PLAYER_START_MODE_GROTTO), { \
            0, 0, 0                                                     \
        }                                                               \
    }

// clang-format off
// Unique grottos (and only one instance of the reused grottos)
BetterMapSelectGrottoRespawnInfo sBetterMapSelectGrottoInfo[] = {
    /*  0 */ { { ENTRANCE(TERMINA_FIELD, 0), 0, 0x1F, -5644, PLAYER_PARAMS(0xFF, PLAYER_START_MODE_GROTTO), { -2782, 48, -1654 } },
               { ENTRANCE(TERMINA_FIELD, 0), 0, 0x01, -16384, PLAYER_PARAMS(0xFF, PLAYER_START_MODE_D), { -2400, 68, -400 } } },
    /*  1 */ { { ENTRANCE(TERMINA_FIELD, 0), 0, 0x1F, -21118, PLAYER_PARAMS(0xFF, PLAYER_START_MODE_GROTTO), { -1592, -222, 4622 } },
               { ENTRANCE(TERMINA_FIELD, 6), 0, 0x01, 0, PLAYER_PARAMS(0xFF, PLAYER_START_MODE_D), { -412, -77, 1681 } } },
    /*  2 */ { { ENTRANCE(TERMINA_FIELD, 0), 0, 0x1F, -27125, PLAYER_PARAMS(0xFF, PLAYER_START_MODE_GROTTO), { 4450, 254, 925 } },
               { ENTRANCE(TERMINA_FIELD, 7), 0, 0x01, 16384, PLAYER_PARAMS(0xFF, PLAYER_START_MODE_D), { 1672, 68, -394 } } },
    /*  3 */ { { ENTRANCE(TERMINA_FIELD, 0), 0, 0x1F, -11287, PLAYER_PARAMS(0xFF, PLAYER_START_MODE_GROTTO), { 192, 48, -3138 } },
               { ENTRANCE(TERMINA_FIELD, 8), 0, 0x01, -32768, PLAYER_PARAMS(0xFF, PLAYER_START_MODE_D), { -400, 48, -2520 } } },
    /*  4 */ { { ENTRANCE(TERMINA_FIELD, 0), 0, 0x3F, -22028, PLAYER_PARAMS(0xFF, PLAYER_START_MODE_GROTTO), { 1012, -221, 3642 } }, // Reused chest
               { ENTRANCE(TERMINA_FIELD, 6), 0, 0x01, 0, PLAYER_PARAMS(0xFF, PLAYER_START_MODE_D), { -412, -77, 1681 } } },
    /*  5 */ { { ENTRANCE(PATH_TO_GORON_VILLAGE_WINTER, 0), 0, 0xFF, -21846, PLAYER_PARAMS(0xFF, PLAYER_START_MODE_GROTTO), { 589, 195, 53 } },
               { ENTRANCE(PATH_TO_GORON_VILLAGE_WINTER, 0), 0, 0x01, 20024, PLAYER_PARAMS(0xFF, PLAYER_START_MODE_D), { -2044, 200, 1288 } } },
    /*  6 */ { UNUSED_GROTTO_RESPAWN,
               { ENTRANCE(DEKU_PALACE, 2), 0, 0x01, 0, PLAYER_PARAMS(0xFF, PLAYER_START_MODE_D), { 5, 0, 1433 } } },
    /*  7 */ { { ENTRANCE(TERMINA_FIELD, 0), 0, 0x1F, 10558, PLAYER_PARAMS(0xFF, PLAYER_START_MODE_GROTTO), { -2425, -281, -3291 } },
               { ENTRANCE(TERMINA_FIELD, 8), 0, 0x01, -32768, PLAYER_PARAMS(0xFF, PLAYER_START_MODE_D), { -400, 48, -2520 } } },
    /*  8 */ { UNUSED_GROTTO_RESPAWN,
               { ENTRANCE(DEKU_PALACE, 2), 0, 0x01, 0, PLAYER_PARAMS(0xFF, PLAYER_START_MODE_D), { 5, 0, 1433 } } },
    /*  9 */ { { ENTRANCE(TERMINA_FIELD, 0), 0, 0x1F, -19479, PLAYER_PARAMS(0xFF, PLAYER_START_MODE_GROTTO), { 3223, 219, 1417 } },
               { ENTRANCE(TERMINA_FIELD, 7), 0, 0x01, 16384, PLAYER_PARAMS(0xFF, PLAYER_START_MODE_D), { 1672, 68, -394 } } },
    /* 10 */ { { ENTRANCE(TERMINA_FIELD, 0), 0, 0x1F, -32768, PLAYER_PARAMS(0xFF, PLAYER_START_MODE_GROTTO), { -375, -222, 3976 } }, // Reused cow
               { ENTRANCE(TERMINA_FIELD, 6), 0, 0x01, 0, PLAYER_PARAMS(0xFF, PLAYER_START_MODE_D), { -412, -77, 1681 } } },
    /* 11 */ { { ENTRANCE(TERMINA_FIELD, 0), 0, 0x1F, 0, PLAYER_PARAMS(0xFF, PLAYER_START_MODE_GROTTO), { -5159, -281, -571 } },
               { ENTRANCE(TERMINA_FIELD, 0), 0, 0x01, -16384, PLAYER_PARAMS(0xFF, PLAYER_START_MODE_D), { -2400, 68, -400 } } },
    /* 12 */ { UNUSED_GROTTO_RESPAWN,
               { ENTRANCE(DEKU_PALACE, 2), 0, 0x01, 0, PLAYER_PARAMS(0xFF, PLAYER_START_MODE_D), { 5, 0, 1433 } } },
    /* 13 */ { { ENTRANCE(TERMINA_FIELD, 0), 0, 0x1F, 21663, PLAYER_PARAMS(0xFF, PLAYER_START_MODE_GROTTO), { -2317, -221, 3418 } },
               { ENTRANCE(TERMINA_FIELD, 6), 0, 0x01, 0, PLAYER_PARAMS(0xFF, PLAYER_START_MODE_D), { -412, -77, 1681 } } },
    /* 14 */ { UNUSED_GROTTO_RESPAWN,
               { ENTRANCE(DEKU_PALACE, 2), 0, 0x01, 0, PLAYER_PARAMS(0xFF, PLAYER_START_MODE_D), { 5, 0, 1433 } } },
    /* 15 */ { UNUSED_GROTTO_RESPAWN,
               { ENTRANCE(DEKU_PALACE, 2), 0, 0x01, 0, PLAYER_PARAMS(0xFF, PLAYER_START_MODE_D), { 5, 0, 1433 } } },
    /* 16 */ { UNUSED_GROTTO_RESPAWN,
               { ENTRANCE(GORON_VILLAGE_WINTER, 3), 0, 0x01, -12015, PLAYER_PARAMS(0xFF, PLAYER_START_MODE_D), { 2621, -200, -1389 } } },
};

// Grotto 4 re-use
BetterMapSelectGrottoRespawnInfo sBetterMapSelectChestGrottoInfo[] = {
    // Termina Field near swamp grass grotto
    /*  0 */ { { ENTRANCE(TERMINA_FIELD, 0), 0, 0x3F, -22028, PLAYER_PARAMS(0xFF, PLAYER_START_MODE_GROTTO), { 1012, -221, 3642 } },
               { ENTRANCE(TERMINA_FIELD, 6), 0, 0x01, 0, PLAYER_PARAMS(0xFF, PLAYER_START_MODE_D), { -412, -77, 1681 } } },
    // Termina Field near Ikana pillar grotto
    /*  1 */ { { ENTRANCE(TERMINA_FIELD, 0), 0, 0x9A, 0, PLAYER_PARAMS(0xFF, PLAYER_START_MODE_GROTTO), { 2367, 315, -192 } },
               { ENTRANCE(TERMINA_FIELD, 7), 0, 0x01, 16384, PLAYER_PARAMS(0xFF, PLAYER_START_MODE_D), { 1672, 68, -394 } } },
    // Road to Southern Swamp near heart piece tree grotto
    /*  2 */ { { ENTRANCE(ROAD_TO_SOUTHERN_SWAMP, 0), 0, 0x3E, 4187, PLAYER_PARAMS(0xFF, PLAYER_START_MODE_GROTTO), { 104, -182, 2202 } },
               { ENTRANCE(ROAD_TO_SOUTHERN_SWAMP, 0), 0, 0x01, -3641, PLAYER_PARAMS(0xFF, PLAYER_START_MODE_D), { 331, -143, 245 } } },
    // Woods of Mystery Day 2 path grotto
    /*  3 */ { { ENTRANCE(WOODS_OF_MYSTERY, 0), 2, 0x5C, 0, PLAYER_PARAMS(0xFF, PLAYER_START_MODE_GROTTO), { 2, 0, -889 } },
               { ENTRANCE(WOODS_OF_MYSTERY, 0), 1, 0x01, -16384, PLAYER_PARAMS(0xFF, PLAYER_START_MODE_D), { 274, 0, 0 } } },
    // Southern Swamp behind spider house grotto
    /*  4 */ { { ENTRANCE(SOUTHERN_SWAMP_POISONED, 0), 1, 0x3D, -5462, PLAYER_PARAMS(0xFF, PLAYER_START_MODE_GROTTO), { -1700, 38, 1800 } },
               { ENTRANCE(SOUTHERN_SWAMP_POISONED, 8), 1, 0x01, -2731, PLAYER_PARAMS(0xFF, PLAYER_START_MODE_D), { -1049, 12, 2042 } } },
    // Mountain Village Spring ramps near Darmani grave grotto
    /*  5 */ { { ENTRANCE(MOUNTAIN_VILLAGE_SPRING, 0), 1, 0x3B, -10377, PLAYER_PARAMS(0xFF, PLAYER_START_MODE_GROTTO), { 2406, 1168, -1197 } },
               { ENTRANCE(MOUNTAIN_VILLAGE_SPRING, 2), 0, 0x01, -22210, PLAYER_PARAMS(0xFF, PLAYER_START_MODE_D), { 2089, 15, 939 } } },
    // Path to Goron Village ramp hidden grotto
    /*  6 */ { { ENTRANCE(PATH_TO_GORON_VILLAGE_WINTER, 0), 0, 0x99, 12379, PLAYER_PARAMS(0xFF, PLAYER_START_MODE_GROTTO), { -1309, 320, 143 } },
               { ENTRANCE(PATH_TO_GORON_VILLAGE_WINTER, 0), 0, 0x01, 20024, PLAYER_PARAMS(0xFF, PLAYER_START_MODE_D), { -2044, 200, 1288 } } },
    // Path to Snowhead snow triangle hidden grotto
    /*  7 */ { { ENTRANCE(PATH_TO_SNOWHEAD, 0), 0, 0x33, -19479, PLAYER_PARAMS(0xFF, PLAYER_START_MODE_GROTTO), { -987, 360, -2339 } },
               { ENTRANCE(PATH_TO_SNOWHEAD, 1), 0, 0x01, 8192, PLAYER_PARAMS(0xFF, PLAYER_START_MODE_D), { -2518, 550, -3441 } } },
    // Great Bay behind Fisherman hut grotto
    /*  8 */ { { ENTRANCE(GREAT_BAY_COAST, 0), 0, 0x37, -11287, PLAYER_PARAMS(0xFF, PLAYER_START_MODE_GROTTO), { 1359, 80, 5018 } },
               { ENTRANCE(GREAT_BAY_COAST, 4), 0, 0x01, -28217, PLAYER_PARAMS(0xFF, PLAYER_START_MODE_D), { 1137, 92, 4635 } } },
    // Zora Cape under rock grotto
    /*  9 */ { { ENTRANCE(ZORA_CAPE, 0), 0, 0x95, -14018, PLAYER_PARAMS(0xFF, PLAYER_START_MODE_GROTTO), { -562, 80, 2707 } },
               { ENTRANCE(ZORA_CAPE, 0), 0, 0x01, 3583, PLAYER_PARAMS(0xFF, PLAYER_START_MODE_D), { 92, 12, 333 } } },
    // Road to Ikana under rock grotto
    /* 10 */ { { ENTRANCE(ROAD_TO_IKANA, 0), 0, 0x96, -19479, PLAYER_PARAMS(0xFF, PLAYER_START_MODE_GROTTO), { -428, 200, -335 } },
               { ENTRANCE(ROAD_TO_IKANA, 0), 0, 0x01, 14563, PLAYER_PARAMS(0xFF, PLAYER_START_MODE_D), { -3006, 0, -305 } } },
    // Ikana Graveyard rock circle hidden grotto
    /* 11 */ { { ENTRANCE(IKANA_GRAVEYARD, 0), 1, 0xB8, -28217, PLAYER_PARAMS(0xFF, PLAYER_START_MODE_GROTTO), { 106, 314, -1777 } },
               { ENTRANCE(IKANA_GRAVEYARD, 0), 0, 0x01, -32768, PLAYER_PARAMS(0xFF, PLAYER_START_MODE_D), { -45, -49, 864 } } },
    // Ikana Canyon near Secret Shrine grotto
    /* 12 */ { { ENTRANCE(IKANA_CANYON, 0), 2, 0xB4, -28217, PLAYER_PARAMS(0xFF, PLAYER_START_MODE_GROTTO), { -2475, -505, 2475 } },
               { ENTRANCE(IKANA_CANYON, 12), 2, 0x01, 13653, PLAYER_PARAMS(0xFF, PLAYER_START_MODE_D), { -3068, -505, 2690 } } },
};

// Grotto 10 re-use
BetterMapSelectGrottoRespawnInfo sBetterMapSelectCowGrottoInfo[] = {
    // Termina Field Hollow Log hidden grotto
    /* 0 */ { { ENTRANCE(TERMINA_FIELD, 0), 0, 0x1F, -32768, PLAYER_PARAMS(0xFF, PLAYER_START_MODE_GROTTO), { -375, -222, 3976 } },
              { ENTRANCE(TERMINA_FIELD, 6), 0, 0x01, 0, PLAYER_PARAMS(0xFF, PLAYER_START_MODE_D), { -412, -77, 1681 } } },
    // Great Bay Cliffs near Gerudo Fortress grotto
    /* 1 */ { { ENTRANCE(GREAT_BAY_COAST, 0), 0, 0xFF, -13654, PLAYER_PARAMS(0xFF, PLAYER_START_MODE_GROTTO), { 2077, 333, -215 } },
              { ENTRANCE(GREAT_BAY_COAST, 6), 0, 0x01, 16019, PLAYER_PARAMS(0xFF, PLAYER_START_MODE_D), { 1321, -16, -1029 } } },
};

// clang-format on

void BetterMapSelect_LoadGame(MapSelectState* mapSelectState, u32 entrance, s32 spawn) {
    CVarSetInteger("gDeveloperTools.BetterMapSelect.CurrentScene", mapSelectState->currentScene);
    CVarSetInteger("gDeveloperTools.BetterMapSelect.Opt", mapSelectState->opt);
    CVarSetInteger("gDeveloperTools.BetterMapSelect.TopDisplayedScene", mapSelectState->topDisplayedScene);
    CVarSetInteger("gDeveloperTools.BetterMapSelect.PageDownIndex", mapSelectState->pageDownIndex);
    CVarSave();
    MapSelect_LoadGame(mapSelectState, entrance, spawn);

    // Remove dummy cutscene index for retaining current time
    if (gSaveContext.save.cutsceneIndex == STAGE_CURRENT_TIME) {
        gSaveContext.save.cutsceneIndex = 0;
    }

    // Handle Grotto return locations
    if (Entrance_GetSceneIdAbsolute(entrance) == SCENE_KAKUSIANA) {
        BetterMapSelectGrottoRespawnInfo grotto;

        if (entrance == ENTRANCE(GROTTOS, 0)) {
            if (spawn >= 0 && spawn < ARRAY_COUNT(sBetterMapSelectGrottoInfo)) {
                grotto = sBetterMapSelectGrottoInfo[spawn];
            }
        } else {
            // Chest/cow grotto selection uses a special list and changes the meaning of the `spawn` value,
            // so we need to set entrance back to the value one
            gSaveContext.save.entrance = entrance;

            if (entrance == ENTRANCE(GROTTOS, 4)) { // Chests
                if (spawn >= 0 && spawn < ARRAY_COUNT(sBetterMapSelectChestGrottoInfo)) {
                    grotto = sBetterMapSelectChestGrottoInfo[spawn];
                }
            } else if (entrance == ENTRANCE(GROTTOS, 10)) { // Cows
                if (spawn >= 0 && spawn < ARRAY_COUNT(sBetterMapSelectCowGrottoInfo)) {
                    grotto = sBetterMapSelectCowGrottoInfo[spawn];
                }
            }
        }

        // Set player void out location
        gSaveContext.respawn[RESPAWN_MODE_DOWN].data = grotto.downRespawn.data;
        gSaveContext.respawn[RESPAWN_MODE_DOWN].roomIndex = grotto.downRespawn.roomIndex;
        gSaveContext.respawn[RESPAWN_MODE_DOWN].entrance = grotto.downRespawn.entrance;
        gSaveContext.respawn[RESPAWN_MODE_DOWN].pos = grotto.downRespawn.pos;
        gSaveContext.respawn[RESPAWN_MODE_DOWN].yaw = grotto.downRespawn.yaw;
        gSaveContext.respawn[RESPAWN_MODE_DOWN].playerParams = grotto.downRespawn.playerParams;

        // Copy void out to top
        gSaveContext.respawn[RESPAWN_MODE_TOP] = gSaveContext.respawn[RESPAWN_MODE_DOWN];

        // Set grotto respawn info
        gSaveContext.respawn[RESPAWN_MODE_UNK_3].data = grotto.grottoRespawn.data;
        gSaveContext.respawn[RESPAWN_MODE_UNK_3].roomIndex = grotto.grottoRespawn.roomIndex;
        gSaveContext.respawn[RESPAWN_MODE_UNK_3].entrance = grotto.grottoRespawn.entrance;
        gSaveContext.respawn[RESPAWN_MODE_UNK_3].pos = grotto.grottoRespawn.pos;
        gSaveContext.respawn[RESPAWN_MODE_UNK_3].yaw = grotto.grottoRespawn.yaw;
        gSaveContext.respawn[RESPAWN_MODE_UNK_3].playerParams = grotto.grottoRespawn.playerParams;
    }
}

void BetterMapSelect_LoadFileSelect(MapSelectState* mapSelectState) {
    CVarSetInteger("gDeveloperTools.BetterMapSelect.CurrentScene", mapSelectState->currentScene);
    CVarSetInteger("gDeveloperTools.BetterMapSelect.Opt", mapSelectState->opt);
    CVarSetInteger("gDeveloperTools.BetterMapSelect.TopDisplayedScene", mapSelectState->topDisplayedScene);
    CVarSetInteger("gDeveloperTools.BetterMapSelect.PageDownIndex", mapSelectState->pageDownIndex);
    CVarSave();
    gSaveContext.gameMode = GAMEMODE_FILE_SELECT;
    STOP_GAMESTATE(&mapSelectState->state);
    SET_NEXT_GAMESTATE(&mapSelectState->state, FileSelect_Init, sizeof(FileSelectState));
}

void BetterMapSelect_Init(MapSelectState* mapSelectState) {
    static bool sIsInitialized = false;
    sIsBetterMapSelectEnabled = CVarGetInteger("gDeveloperTools.BetterMapSelect.Enabled", 0);

    if (sIsBetterMapSelectEnabled) {
        if (!sIsInitialized) {
            for (s32 i = 0; i < ARRAY_COUNT(sBetterMapSelectInfo); i++) {
                sBetterScenes[sBetterMapSelectInfo[i].index].name = sBetterMapSelectInfo[i].name;
                sBetterScenes[sBetterMapSelectInfo[i].index].entrance = sBetterMapSelectInfo[i].entrance;
            }
            sIsInitialized = true;
        }

        mapSelectState->scenes = sBetterScenes;
        mapSelectState->count = ARRAY_COUNT(sBetterScenes);
        mapSelectState->currentScene = CVarGetInteger("gDeveloperTools.BetterMapSelect.CurrentScene", 0);
        mapSelectState->opt = CVarGetInteger("gDeveloperTools.BetterMapSelect.Opt", 0);
        mapSelectState->topDisplayedScene = CVarGetInteger("gDeveloperTools.BetterMapSelect.TopDisplayedScene", 0);
        mapSelectState->pageDownIndex = CVarGetInteger("gDeveloperTools.BetterMapSelect.PageDownIndex", 0);

        gSaveContext.save.cutsceneIndex = STAGE_CURRENT_TIME;
    } else {
        mapSelectState->scenes = sScenes;
        mapSelectState->count = ARRAY_COUNT(sScenes);
        mapSelectState->currentScene = 0;
        mapSelectState->opt = 0;
        mapSelectState->topDisplayedScene = 0;
        mapSelectState->pageDownIndex = 0;

        if (gSaveContext.save.cutsceneIndex == STAGE_CURRENT_TIME) {
            gSaveContext.save.cutsceneIndex = 0;
        }
    }
}

void BetterMapSelect_Update(MapSelectState* mapSelectState) {
    if (sIsBetterMapSelectEnabled != CVarGetInteger("gDeveloperTools.BetterMapSelect.Enabled", 0)) {
        BetterMapSelect_Init(mapSelectState);
    }

    if (!sIsBetterMapSelectEnabled) {
        return;
    }

    static s32 sPrevScene = -1;
    Input* controller1 = CONTROLLER1(&mapSelectState->state);

    // Clamp and wrap around the spawn value based on the supported entrances for that scene
    if (mapSelectState->currentScene < ARRAY_COUNT(sBetterMapSelectInfo)) {
        // Scenes from scene_table.h can be checked directly against `sSceneEntranceTable`
        s32 entrSceneId = sBetterScenes[mapSelectState->currentScene].entrance >> 9;
        SceneEntranceTableEntry entry = sSceneEntranceTable[entrSceneId];

        if (mapSelectState->opt >= entry.tableCount) {
            mapSelectState->opt = 0;
        } else if (mapSelectState->opt < 0) {
            mapSelectState->opt = entry.tableCount - 1;
        }
    } else if (mapSelectState->currentScene == 102 || mapSelectState->currentScene == 103) {
        // Cheset/Cow special entries
        s32 count = mapSelectState->currentScene == 102 ? ARRAY_COUNT(sBetterMapSelectChestGrottoInfo)
                                                        : ARRAY_COUNT(sBetterMapSelectCowGrottoInfo);
        if (mapSelectState->opt >= count) {
            mapSelectState->opt = 0;
        } else if (mapSelectState->opt < 0) {
            mapSelectState->opt = count - 1;
        }
    } else { // File Select/Title Screen
        mapSelectState->opt = 0;
    }

    // Update to a normal stage value, then standard MapSelect_Update handles the rest
    if (gSaveContext.save.cutsceneIndex == STAGE_CURRENT_TIME &&
        CHECK_BTN_ANY(controller1->press.button, BTN_R | BTN_Z)) {
        gSaveContext.save.cutsceneIndex = 0;
    }
}

static const char* betterFormLabels[] = {
    "Deity", "Goron", "Zora", "Deku", "Child",
};

void BetterMapSelect_PrintMenu(MapSelectState* mapSelectState, GfxPrint* printer) {
    s32 i;
    s32 sceneIndex;
    char* sceneName;
    char* stageName;
    char* dayName;

    // Header
    GfxPrint_SetColor(printer, 255, 255, 255, 255);
    GfxPrint_SetPos(printer, 12, 2);
    GfxPrint_Printf(printer, "Scene Selection");
    GfxPrint_SetColor(printer, 255, 255, 255, 255);

    // Scenes
    for (i = 0; i < 20; i++) {
        GfxPrint_SetPos(printer, 3, i + 4);

        sceneIndex = (mapSelectState->topDisplayedScene + i + mapSelectState->count) % mapSelectState->count;
        if (sceneIndex == mapSelectState->currentScene) {
            GfxPrint_SetColor(printer, 255, 100, 100, 255);
        } else {
            GfxPrint_SetColor(printer, 175, 175, 175, 255);
        }

        sceneName = sBetterScenes[sceneIndex].name;
        GfxPrint_Printf(printer, "%3d %s", sceneIndex, sceneName);
    };

    // Entrance
    GfxPrint_SetColor(printer, 100, 100, 100, 255);
    GfxPrint_SetPos(printer, 28, 26);
    GfxPrint_Printf(printer, "Entrance:");
    GfxPrint_SetColor(printer, 200, 200, 50, 255);
    GfxPrint_Printf(printer, "%2d", mapSelectState->opt);

    // Form
    GfxPrint_SetPos(printer, 26, 25);
    GfxPrint_SetColor(printer, 100, 100, 100, 255);
    GfxPrint_Printf(printer, "(B)Form:");
    GfxPrint_SetColor(printer, 55, 200, 50, 255);
    GfxPrint_Printf(printer, "%s", betterFormLabels[GET_PLAYER_FORM]);

    // Day
    GfxPrint_SetPos(printer, 1, 25);
    GfxPrint_SetColor(printer, 100, 100, 100, 255);
    GfxPrint_Printf(printer, "Day:");
    GfxPrint_SetColor(printer, 100, 100, 200, 255);
    switch (gSaveContext.save.day) {
        case 1:
            dayName = "First Day";
            break;
        case 2:
            dayName = "Second Day";
            break;
        case 3:
            dayName = "Final Day";
            break;
        case 4:
            dayName = "Clear Day";
            break;
        default:
            gSaveContext.save.day = 1;
            dayName = "First Day";
            break;
    }
    GfxPrint_Printf(printer, "%s", dayName);

    // Stage
    GfxPrint_SetPos(printer, 1, 26);
    GfxPrint_SetColor(printer, 100, 100, 100, 255);
    GfxPrint_Printf(printer, "(Z/R)Stage:");
    GfxPrint_SetColor(printer, 200, 100, 200, 255);
    switch (gSaveContext.save.cutsceneIndex) {
        case 0:
            gSaveContext.save.time = CLOCK_TIME(12, 0);
            stageName = "Afternoon";
            break;
        case 0x8000:
            gSaveContext.save.time = CLOCK_TIME(6, 0) + 1;
            stageName = "Morning";
            break;
        case 0x8800:
            gSaveContext.save.time = CLOCK_TIME(18, 1);
            stageName = "Night";
            break;
        case 0xFFF0:
            gSaveContext.save.time = CLOCK_TIME(12, 0);
            stageName = "0";
            break;
        case 0xFFF1:
            stageName = "1";
            break;
        case 0xFFF2:
            stageName = "2";
            break;
        case 0xFFF3:
            stageName = "3";
            break;
        case 0xFFF4:
            stageName = "4";
            break;
        case 0xFFF5:
            stageName = "5";
            break;
        case 0xFFF6:
            stageName = "6";
            break;
        case 0xFFF7:
            stageName = "7";
            break;
        case 0xFFF8:
            stageName = "8";
            break;
        case 0xFFF9:
            stageName = "9";
            break;
        case 0xFFFA:
            stageName = "A";
            break;
        default:
            stageName = "???";
            break;
    }

    if (gSaveContext.save.cutsceneIndex != STAGE_CURRENT_TIME) {
        GfxPrint_Printf(printer, "%s", stageName);
    } else {
        u16 curMinutes = (s32)TIME_TO_MINUTES_F(CURRENT_TIME) % 60;
        u16 curHours = (s32)TIME_TO_MINUTES_F(CURRENT_TIME) / 60;
        char* ampm = "";
        char* hourPrefix = "";
        char* minutePrefix = curMinutes < 10 ? "0" : "";

        // Handle 24 or 12 hour time
        if (CVarGetInteger("gEnhancements.Graphics.24HoursClock", 0)) {
            if (curHours < 10) {
                hourPrefix = "0";
            }
        } else {
            ampm = curHours >= 12 ? "pm" : "am";
            curHours = curHours % 12 ? curHours % 12 : 12;
        }

        GfxPrint_Printf(printer, "%s%d:%s%d %s", hourPrefix, curHours, minutePrefix, curMinutes, ampm);
    }
};
