/*
 * Much of the code here was borrowed from https://github.com/gamestabled/OoT3D_Randomizer/blob/main/code/src/grotto.c
 * It's been adapted for SoH to use our gPlayState vs their gGlobalContext and slightly different named properties.
 */

#include "randomizer_grotto.h"

#include "global.h"

extern PlayState* gPlayState;

// Information necessary for entering each grotto
static const GrottoLoadInfo grottoLoadTable[NUM_GROTTOS] = {
    {.entranceIndex = 0x05BC, .content = 0xFD, .scene = 0x5C}, // Desert Colossus -> Colossus Grotto
    {.entranceIndex = 0x05A4, .content = 0xEF, .scene = 0x57}, // Lake Hylia -> LH Grotto
    {.entranceIndex = 0x05BC, .content = 0xEB, .scene = 0x54}, // Zora River -> ZR Storms Grotto
    {.entranceIndex = 0x036D, .content = 0xE6, .scene = 0x54}, // Zora River -> ZR Fairy Grotto
    {.entranceIndex = 0x003F, .content = 0x29, .scene = 0x54}, // Zora River -> ZR Open Grotto
    {.entranceIndex = 0x05A4, .content = 0xF9, .scene = 0x61}, // DMC Lower Nearby -> DMC Hammer Grotto
    {.entranceIndex = 0x003F, .content = 0x7A, .scene = 0x61}, // DMC Upper Nearby -> DMC Upper Grotto
    {.entranceIndex = 0x05A4, .content = 0xFB, .scene = 0x62}, // GC Grotto Platform -> GC Grotto
    {.entranceIndex = 0x003F, .content = 0x57, .scene = 0x60}, // Death Mountain -> DMT Storms Grotto
    {.entranceIndex = 0x05FC, .content = 0xF8, .scene = 0x60}, // Death Mountain Summit -> DMT Cow Grotto
    {.entranceIndex = 0x003F, .content = 0x28, .scene = 0x52}, // Kak Backyard -> Kak Open Grotto
    {.entranceIndex = 0x05A0, .content = 0xE7, .scene = 0x52}, // Kakariko Village -> Kak Redead Grotto
    {.entranceIndex = 0x05B8, .content = 0xF6, .scene = 0x5F}, // Hyrule Castle Grounds -> HC Storms Grotto
    {.entranceIndex = 0x05C0, .content = 0xE1, .scene = 0x51}, // Hyrule Field -> HF Tektite Grotto
    {.entranceIndex = 0x0598, .content = 0xE5, .scene = 0x51}, // Hyrule Field -> HF Near Kak Grotto
    {.entranceIndex = 0x036D, .content = 0xFF, .scene = 0x51}, // Hyrule Field -> HF Fairy Grotto
    {.entranceIndex = 0x003F, .content = 0x00, .scene = 0x51}, // Hyrule Field -> HF Near Market Grotto
    {.entranceIndex = 0x05A8, .content = 0xE4, .scene = 0x51}, // Hyrule Field -> HF Cow Grotto
    {.entranceIndex = 0x059C, .content = 0xE6, .scene = 0x51}, // Hyrule Field -> HF Inside Fence Grotto
    {.entranceIndex = 0x003F, .content = 0x03, .scene = 0x51}, // Hyrule Field -> HF Open Grotto
    {.entranceIndex = 0x003F, .content = 0x22, .scene = 0x51}, // Hyrule Field -> HF Southeast Grotto
    {.entranceIndex = 0x05A4, .content = 0xFC, .scene = 0x63}, // Lon Lon Ranch -> LLR Grotto
    {.entranceIndex = 0x05B4, .content = 0xED, .scene = 0x56}, // SFM Entryway -> SFM Wolfos Grotto
    {.entranceIndex = 0x05BC, .content = 0xEE, .scene = 0x56}, // Sacred Forest Meadow -> SFM Storms Grotto
    {.entranceIndex = 0x036D, .content = 0xFF, .scene = 0x56}, // Sacred Forest Meadow -> SFM Fairy Grotto
    {.entranceIndex = 0x05B0, .content = 0xF5, .scene = 0x5B}, // LW Beyond Mido -> LW Scrubs Grotto
    {.entranceIndex = 0x003F, .content = 0x14, .scene = 0x5B}, // Lost Woods -> LW Near Shortcuts Grotto
    {.entranceIndex = 0x003F, .content = 0x2C, .scene = 0x55}, // Kokiri Forest -> KF Storms Grotto
    {.entranceIndex = 0x036D, .content = 0xFF, .scene = 0x58}, // Zoras Domain -> ZD Storms Grotto
    {.entranceIndex = 0x036D, .content = 0xFF, .scene = 0x5D}, // Gerudo Fortress -> GF Storms Grotto
    {.entranceIndex = 0x05BC, .content = 0xF0, .scene = 0x5A}, // GV Fortress Side -> GV Storms Grotto
    {.entranceIndex = 0x05AC, .content = 0xF2, .scene = 0x5A}, // GV Grotto Ledge -> GV Octorok Grotto
    {.entranceIndex = 0x05C4, .content = 0xF3, .scene = 0x5B}, // LW Beyond Mido -> Deku Theater
};

// Information necessary for setting up returning from a grotto
static const GrottoReturnInfo grottoReturnTable[NUM_GROTTOS] = {
    {.entranceIndex = 0x0123, .room = 0x00, .angle = 0xA71C, .pos = {.x =    62.5078f, .y =   -32.0f, .z = -1296.2f}},   // Colossus Grotto -> Desert Colossus
    {.entranceIndex = 0x0102, .room = 0x00, .angle = 0x0000, .pos = {.x = -3039.34f,   .y = -1033.0f, .z =  6080.74f}},  // LH Grotto -> Lake Hylia
    {.entranceIndex = 0x00EA, .room = 0x00, .angle = 0x0000, .pos = {.x = -1630.05f,   .y =   100.0f, .z =  -132.104f}}, // ZR Storms Grotto -> Zora River
    {.entranceIndex = 0x00EA, .room = 0x00, .angle = 0xE000, .pos = {.x =   649.507f,  .y =   570.0f, .z =  -346.853f}}, // ZR Fairy Grotto -> Zora River
    {.entranceIndex = 0x00EA, .room = 0x00, .angle = 0x8000, .pos = {.x =   362.29f,   .y =   570.0f, .z =   111.48f}},  // ZR Open Grotto -> Zora River
    {.entranceIndex = 0x0246, .room = 0x01, .angle = 0x31C7, .pos = {.x = -1666.73f,   .y =   721.0f, .z =  -459.21f}},  // DMC Hammer Grotto -> DMC Lower Local
    {.entranceIndex = 0x0147, .room = 0x01, .angle = 0x238E, .pos = {.x =    63.723f,  .y =  1265.0f, .z =  1791.39f}},  // DMC Upper Grotto -> DMC Upper Local
    {.entranceIndex = 0x014D, .room = 0x03, .angle = 0x0000, .pos = {.x =  1104.73f,   .y =   580.0f, .z = -1159.95f}},  // GC Grotto -> GC Grotto Platform
    {.entranceIndex = 0x01B9, .room = 0x00, .angle = 0x8000, .pos = {.x =  -387.584f,  .y =  1386.0f, .z = -1213.05f}},  // DMT Storms Grotto -> Death Mountain
    {.entranceIndex = 0x01B9, .room = 0x00, .angle = 0x8000, .pos = {.x =  -691.022f,  .y =  1946.0f, .z =  -312.969f}}, // DMT Cow Grotto -> Death Mountain Summit
    {.entranceIndex = 0x00DB, .room = 0x00, .angle = 0x0000, .pos = {.x =   855.238f,  .y =    80.0f, .z =  -234.095f}}, // Kak Open Grotto -> Kak Backyard
    {.entranceIndex = 0x00DB, .room = 0x00, .angle = 0x0000, .pos = {.x =  -401.873f,  .y =     0.0f, .z =   402.792f}}, // Kak Redead Grotto -> Kakariko Village
    {.entranceIndex = 0x0138, .room = 0x00, .angle = 0x9555, .pos = {.x =  1009.02f,   .y =  1571.0f, .z =   855.532f}}, // HC Storms Grotto -> Castle Grounds
    {.entranceIndex = 0x01F9, .room = 0x00, .angle = 0x1555, .pos = {.x = -4949.58f,   .y =  -300.0f, .z =  2837.59f}},  // HF Tektite Grotto -> Hyrule Field
    {.entranceIndex = 0x01F9, .room = 0x00, .angle = 0xC000, .pos = {.x =  2050.6f,    .y =    20.0f, .z =  -160.397f}}, // HF Near Kak Grotto -> Hyrule Field
    {.entranceIndex = 0x01F9, .room = 0x00, .angle = 0x0000, .pos = {.x = -4447.66f,   .y =  -300.0f, .z =  -393.191f}}, // HF Fairy Grotto -> Hyrule Field
    {.entranceIndex = 0x01F9, .room = 0x00, .angle = 0xE000, .pos = {.x = -1446.56f,   .y =     0.0f, .z =   830.775f}}, // HF Near Market Grotto -> Hyrule Field
    {.entranceIndex = 0x01F9, .room = 0x00, .angle = 0x0000, .pos = {.x = -7874.07f,   .y =  -300.0f, .z =  6921.31f}},  // HF Cow Grotto -> Hyrule Field
    {.entranceIndex = 0x01F9, .room = 0x00, .angle = 0xEAAB, .pos = {.x = -4989.13f,   .y =  -700.0f, .z = 13821.1f}},   // HF Inside Fence Grotto -> Hyrule Field
    {.entranceIndex = 0x01F9, .room = 0x00, .angle = 0x8000, .pos = {.x = -4032.61f,   .y =  -700.0f, .z = 13831.5f}},   // HF Open Grotto -> Hyrule Field
    {.entranceIndex = 0x01F9, .room = 0x00, .angle = 0x9555, .pos = {.x =  -288.313f,  .y =  -500.0f, .z = 12320.2f}},   // HF Southeast Grotto -> Hyrule Field
    {.entranceIndex = 0x0157, .room = 0x00, .angle = 0xAAAB, .pos = {.x =  1775.92f,   .y =     0.0f, .z =  1486.82f}},  // LLR Grotto -> Lon Lon Ranch
    {.entranceIndex = 0x00FC, .room = 0x00, .angle = 0x8000, .pos = {.x =  -189.861f,  .y =     0.0f, .z =  1898.09f}},  // SFM Wolfos Grotto -> SFM Entryway
    {.entranceIndex = 0x00FC, .room = 0x00, .angle = 0xAAAB, .pos = {.x =   314.853f,  .y =   480.0f, .z = -2300.39f}},  // SFM Storms Grotto -> Sacred Forest Meadow
    {.entranceIndex = 0x00FC, .room = 0x00, .angle = 0x0000, .pos = {.x =    55.034f,  .y =     0.0f, .z =   250.595f}}, // SFM Fairy Grotto -> Sacred Forest Meadow
    {.entranceIndex = 0x01A9, .room = 0x08, .angle = 0x2000, .pos = {.x =   691.994f,  .y =     0.0f, .z = -2502.2f}},   // LW Scrubs Grotto -> LW Beyond Mido
    {.entranceIndex = 0x011E, .room = 0x02, .angle = 0xE000, .pos = {.x =   905.755f,  .y =     0.0f, .z =  -901.43f}},  // LW Near Shortcuts Grotto -> Lost Woods
    {.entranceIndex = 0x0286, .room = 0x00, .angle = 0x4000, .pos = {.x =  -507.065f,  .y =   380.0f, .z = -1220.43f}},  // KF Storms Grotto -> Kokiri Forest
    {.entranceIndex = 0x0108, .room = 0x01, .angle = 0xD555, .pos = {.x =  -855.68f,   .y =    14.0f, .z =  -474.422f}}, // ZD Storms Grotto -> Zoras Domain
    {.entranceIndex = 0x0129, .room = 0x00, .angle = 0x4000, .pos = {.x =   380.521f,  .y =   333.0f, .z = -1560.74f}},  // GF Storms Grotto -> Gerudo Fortress
    {.entranceIndex = 0x022D, .room = 0x00, .angle = 0x9555, .pos = {.x = -1326.34f,   .y =    15.0f, .z =  -983.994f}}, // GV Storms Grotto -> GV Fortress Side
    {.entranceIndex = 0x0117, .room = 0x00, .angle = 0x8000, .pos = {.x =   291.513f,  .y =  -555.0f, .z =  1478.39f}},  // GV Octorok Grotto -> GV Grotto Ledge
    {.entranceIndex = 0x01A9, .room = 0x06, .angle = 0x4000, .pos = {.x =   109.281f,  .y =   -20.0f, .z = -1601.42f}},  // Deku Theater -> LW Beyond Mido
};

static s16 grottoExitList[NUM_GROTTOS] = {0};
static s16 grottoLoadList[NUM_GROTTOS] = {0};
static s8 grottoId = 0xFF;
static s8 lastEntranceType = NOT_GROTTO;
static u8 overridingNextEntrance = false;

// Initialize both lists so that each index refers to itself. An index referring
// to itself means that the entrance is not shuffled. Indices will be overwritten
// later in Entrance_Init() in entrance.c if entrance shuffle is enabled.
// For the grotto load list, the entrance index is 0x0700 + the grotto id
// For the grotto exit list, the entrance index is 0x0800 + the grotto id
void Grotto_InitExitAndLoadLists(void) {
    for (u8 i = 0; i < NUM_GROTTOS; i++) {
        grottoLoadList[i] = ENTRANCE_RANDO_GROTTO_LOAD_START + i;
        grottoExitList[i] = ENTRANCE_RANDO_GROTTO_EXIT_START + i;
    }
}

void Grotto_SetExitOverride(s16 originalIndex, s16 overrideIndex) {
    s16 id = originalIndex & 0x00FF;
    grottoExitList[id] = overrideIndex;
}

void Grotto_SetLoadOverride(s16 originalIndex, s16 overrideIndex) {
    s16 id = originalIndex & 0x00FF;
    grottoLoadList[id] = overrideIndex;
}

static void Grotto_SetupReturnInfo(GrottoReturnInfo grotto, RespawnMode respawnMode) {
    // Set necessary grotto return data to the Entrance Point, so that voiding out and setting FW work correctly
    gSaveContext.respawn[respawnMode].entranceIndex = grotto.entranceIndex;
    gSaveContext.respawn[respawnMode].roomIndex = grotto.room;
    gSaveContext.respawn[respawnMode].playerParams = 0x04FF; // exiting grotto with no initial camera focus
    gSaveContext.respawn[respawnMode].yaw = grotto.angle;
    gSaveContext.respawn[respawnMode].pos = grotto.pos;
    // If Mixed Entrance Pools or decoupled entrances are active, set these flags to 0 instead of restoring them
    if (Randomizer_GetSettingValue(RSK_MIX_GROTTO_ENTRANCES) || Randomizer_GetSettingValue(RSK_DECOUPLED_ENTRANCES)) {
        gSaveContext.respawn[respawnMode].tempSwchFlags = 0;
        gSaveContext.respawn[respawnMode].tempCollectFlags = 0;
    } else {
        gSaveContext.respawn[respawnMode].tempSwchFlags = gSaveContext.respawn[RESPAWN_MODE_RETURN].tempSwchFlags;
        gSaveContext.respawn[respawnMode].tempCollectFlags = gSaveContext.respawn[RESPAWN_MODE_RETURN].tempCollectFlags;
    }
}

// Translates and overrides the passed in entrance index if it corresponds to a
// special grotto entrance (grotto load or returnpoint)
s16 Grotto_OverrideSpecialEntrance(s16 nextEntranceIndex) {

    // Don't change anything unless grotto shuffle has been enabled
    if (!Randomizer_GetSettingValue(RSK_SHUFFLE_GROTTO_ENTRANCES) && !Randomizer_GetSettingValue(RSK_SHUFFLE_OVERWORLD_SPAWNS) && !Randomizer_GetSettingValue(RSK_SHUFFLE_WARP_SONGS)) {
        return nextEntranceIndex;
    }

    // If Link hits a grotto exit, load the entrance index from the grotto exit list
    // based on the current grotto ID
    if (nextEntranceIndex == 0x7FFF) {
        Entrance_SetEntranceDiscovered(ENTRANCE_RANDO_GROTTO_EXIT_START + grottoId);
        EntranceTracker_SetLastEntranceOverride(ENTRANCE_RANDO_GROTTO_EXIT_START + grottoId);
        nextEntranceIndex = grottoExitList[grottoId];
    }

    // Get the new grotto id from the next entrance
    grottoId = nextEntranceIndex & 0x00FF;

    // Grotto Returns
    if (nextEntranceIndex >= ENTRANCE_RANDO_GROTTO_EXIT_START && nextEntranceIndex < ENTRANCE_RANDO_GROTTO_EXIT_START + NUM_GROTTOS) {

        GrottoReturnInfo grotto = grottoReturnTable[grottoId];
        Grotto_SetupReturnInfo(grotto, RESPAWN_MODE_RETURN);
        Grotto_SetupReturnInfo(grotto, RESPAWN_MODE_DOWN);

        // When the nextEntranceIndex is determined by a dynamic exit,
        // or set by Entrance_OverrideBlueWarp to mark a blue warp entrance,
        // we have to set the respawn information and nextEntranceIndex manually
        if (gPlayState != NULL && gPlayState->nextEntranceIndex != -1) {
            gSaveContext.respawnFlag = 2;
            nextEntranceIndex = grotto.entranceIndex;
            gPlayState->fadeTransition = 3;
            gSaveContext.nextTransitionType = 3;
        } else if (gPlayState == NULL) { // Handle spawn position when loading from a save file
            gSaveContext.respawnFlag = 2;
            nextEntranceIndex = grotto.entranceIndex;
            gSaveContext.nextTransitionType = 3;
        // Otherwise return 0x7FFF and let the game handle it
        } else {
            nextEntranceIndex = 0x7FFF;
        }

        lastEntranceType = GROTTO_RETURN;
    // Grotto Loads
    } else if (nextEntranceIndex >= ENTRANCE_RANDO_GROTTO_LOAD_START && nextEntranceIndex < ENTRANCE_RANDO_GROTTO_EXIT_START) {

        // Set the respawn data to load the correct grotto
        GrottoLoadInfo grotto = grottoLoadTable[grottoId];
        gSaveContext.respawn[RESPAWN_MODE_RETURN].data = grotto.content;
        nextEntranceIndex = grotto.entranceIndex;

        EntranceTracker_SetCurrentGrottoID(grottoId);

        lastEntranceType = NOT_GROTTO;
    // Otherwise just unset the current grotto ID
    } else {
        grottoId = 0xFF;
        lastEntranceType = NOT_GROTTO;
    }

    overridingNextEntrance = true;
    return nextEntranceIndex;
}

// Override the entrance index when entering into a grotto actor
// thisx - pointer to the grotto actor
void Grotto_OverrideActorEntrance(Actor* thisx) {

    // Vanilla Behavior if there's no possibility of ending up in a grotto randomly
    if (!Randomizer_GetSettingValue(RSK_SHUFFLE_GROTTO_ENTRANCES) && !Randomizer_GetSettingValue(RSK_SHUFFLE_OVERWORLD_SPAWNS) && !Randomizer_GetSettingValue(RSK_SHUFFLE_WARP_SONGS)) {
        return;
    }

    s8 grottoContent = thisx->params & 0x00FF;

    // Loop through the grotto load table until we find the matching index based
    // on content and scene
    for (s16 index = 0; index < NUM_GROTTOS; index++) {

        if (grottoContent == grottoLoadTable[index].content && gPlayState->sceneNum == grottoLoadTable[index].scene) {
            // Find the override for the matching index from the grotto Load List
            Entrance_SetEntranceDiscovered(ENTRANCE_RANDO_GROTTO_LOAD_START + index);
            EntranceTracker_SetLastEntranceOverride(ENTRANCE_RANDO_GROTTO_LOAD_START + index);
            index = grottoLoadList[index];

            // Run the index through the special entrances override check
            lastEntranceType = GROTTO_LOAD;
            gPlayState->nextEntranceIndex = Grotto_OverrideSpecialEntrance(index);
            return;
        }
    }
}

// Set necessary flags for when warp songs/overworld spawns are shuffled to grotto return points
void Grotto_ForceGrottoReturnOnSpecialEntrance(void) {
    if (lastEntranceType == GROTTO_RETURN && (Randomizer_GetSettingValue(RSK_SHUFFLE_GROTTO_ENTRANCES) || Randomizer_GetSettingValue(RSK_SHUFFLE_OVERWORLD_SPAWNS) || Randomizer_GetSettingValue(RSK_SHUFFLE_WARP_SONGS))) {
        gSaveContext.respawnFlag = 2;
        gSaveContext.respawn[RESPAWN_MODE_RETURN].playerParams = 0x4FF;
        gSaveContext.respawn[RESPAWN_MODE_RETURN].pos = grottoReturnTable[grottoId].pos;
        // Clear current temp flags
        gSaveContext.respawn[RESPAWN_MODE_RETURN].tempSwchFlags = 0;
        gSaveContext.respawn[RESPAWN_MODE_RETURN].tempCollectFlags = 0;
    }
}

// Set the respawn flag for when we want to return from a grotto entrance
// Used for Sun's Song and Game Over, which usually don't restore saved position data
void Grotto_ForceGrottoReturn(void) {
    if (lastEntranceType == GROTTO_RETURN && (Randomizer_GetSettingValue(RSK_SHUFFLE_GROTTO_ENTRANCES) || Randomizer_GetSettingValue(RSK_SHUFFLE_OVERWORLD_SPAWNS) || Randomizer_GetSettingValue(RSK_SHUFFLE_WARP_SONGS))) {
        gSaveContext.respawnFlag = 2;
        gSaveContext.respawn[RESPAWN_MODE_RETURN].playerParams = 0x0DFF;
        gSaveContext.respawn[RESPAWN_MODE_RETURN].pos = grottoReturnTable[grottoId].pos;
        //Save the current temp flags in the grotto return point, so they'll properly keep their values.
        gSaveContext.respawn[RESPAWN_MODE_RETURN].tempSwchFlags = gPlayState->actorCtx.flags.tempSwch;
        gSaveContext.respawn[RESPAWN_MODE_RETURN].tempCollectFlags = gPlayState->actorCtx.flags.tempCollect;
    }
}

// Used for the DMT special voids, which usually don't restore saved position data
void Grotto_ForceRegularVoidOut(void) {
    if (lastEntranceType == GROTTO_RETURN && (Randomizer_GetSettingValue(RSK_SHUFFLE_GROTTO_ENTRANCES) || Randomizer_GetSettingValue(RSK_SHUFFLE_OVERWORLD_SPAWNS) || Randomizer_GetSettingValue(RSK_SHUFFLE_WARP_SONGS))) {
        gSaveContext.respawn[RESPAWN_MODE_DOWN] = gSaveContext.respawn[RESPAWN_MODE_RETURN];
        gSaveContext.respawn[RESPAWN_MODE_DOWN].playerParams = 0x0DFF;
        gSaveContext.respawn[RESPAWN_MODE_DOWN].pos = grottoReturnTable[grottoId].pos;
        gSaveContext.respawnFlag = 1;
    }
}

// If returning to a FW point saved at a grotto exit, copy the FW data to the Grotto Return Point
// so that Sun's Song and Game Over will behave correctly
void Grotto_SetupReturnInfoOnFWReturn(void) {
    if (Randomizer_GetSettingValue(RSK_SHUFFLE_GROTTO_ENTRANCES) || Randomizer_GetSettingValue(RSK_SHUFFLE_OVERWORLD_SPAWNS) || Randomizer_GetSettingValue(RSK_SHUFFLE_WARP_SONGS) &&
        gSaveContext.fw.playerParams == 0x4FF) {
        gSaveContext.respawn[RESPAWN_MODE_RETURN] = gSaveContext.respawn[RESPAWN_MODE_TOP];
        gSaveContext.respawn[RESPAWN_MODE_RETURN].playerParams = 0x0DFF;
        lastEntranceType = GROTTO_RETURN;
    } else {
        lastEntranceType = NOT_GROTTO;
    }
}

// If a scene transition is not overridden at all (i.e. guards throwing Link out / quitting game)
// the lastEntranceType must be cleared to avoid messing up savewarps and deathwarps.
// This does not apply to void out and other respawns, which should keep the lastEntranceType.
void Grotto_SanitizeEntranceType(void) {
    if (!overridingNextEntrance && gSaveContext.respawnFlag == 0) {
        lastEntranceType = NOT_GROTTO;
    }
    overridingNextEntrance = false;
}

// Get the renamed entrance index based on the grotto contents and exit scene number
s16 Grotto_GetRenamedGrottoIndexFromOriginal(s8 content, s8 scene) {
    for (s16 index = 0; index < NUM_GROTTOS; index++) {
        if (content == grottoLoadTable[index].content && scene == grottoLoadTable[index].scene) {
            return ENTRANCE_RANDO_GROTTO_LOAD_START | index;
        }
    }

    return ENTRANCE_RANDO_GROTTO_LOAD_START;
}
