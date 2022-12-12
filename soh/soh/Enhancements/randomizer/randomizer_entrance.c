/*
 * Much of the code here was borrowed from https://github.com/gamestabled/OoT3D_Randomizer/blob/main/code/src/entrance.c
 * It's been adapted for SoH to use our gPlayState vs their gGlobalContext with slightly different named properties, and our enums for some scenes/entrances.
 * 
 * Unlike 3DS rando, we need to be able to support the user loading up vanilla and rando saves, so the logic around
 * modifying the entrance table requires that we save the original table and reset whenever loading a vanilla save.
 * A modified dynamicExitList is manually included since we can't read it from addressing like 3ds rando.
 */

#include "randomizer_entrance.h"
#include "randomizer_grotto.h"
#include <string.h>

#include "global.h"

extern PlayState* gPlayState;

//Overwrite the dynamic exit for the OGC Fairy Fountain to be 0x3E8 instead
//of 0x340 (0x340 will stay as the exit for the HC Fairy Fountain -> Castle Grounds)
s16 dynamicExitList[] = { 0x045B, 0x0482, 0x03E8, 0x044B, 0x02A2, 0x0201, 0x03B8, 0x04EE, 0x03C0, 0x0463, 0x01CD, 0x0394, 0x0340, 0x057C };
//                                       OGC Fairy                                                                       HC Fairy

// Warp Song indices array : 0x53C33C = { 0x0600, 0x04F6, 0x0604, 0x01F1, 0x0568, 0x05F4 }

// Owl Flights : 0x492064 and 0x492080

static s16 entranceOverrideTable[ENTRANCE_TABLE_SIZE] = {0};

EntranceInfo originalEntranceTable[ENTRANCE_TABLE_SIZE] = {0};

//These variables store the new entrance indices for dungeons so that
//savewarping and game overs respawn players at the proper entrance.
//By default, these will be their vanilla values.
static s16 newDekuTreeEntrance              = DEKU_TREE_ENTRANCE;
static s16 newDodongosCavernEntrance        = DODONGOS_CAVERN_ENTRANCE;
static s16 newJabuJabusBellyEntrance        = JABU_JABUS_BELLY_ENTRANCE;
static s16 newForestTempleEntrance          = FOREST_TEMPLE_ENTRANCE;
static s16 newFireTempleEntrance            = FIRE_TEMPLE_ENTRANCE;
static s16 newWaterTempleEntrance           = WATER_TEMPLE_ENTRANCE;
static s16 newSpiritTempleEntrance          = SPIRIT_TEMPLE_ENTRANCE;
static s16 newShadowTempleEntrance          = SHADOW_TEMPLE_ENTRANCE;
static s16 newBottomOfTheWellEntrance       = BOTTOM_OF_THE_WELL_ENTRANCE;
static s16 newGerudoTrainingGroundsEntrance = GERUDO_TRAINING_GROUNDS_ENTRANCE;
static s16 newIceCavernEntrance             = ICE_CAVERN_ENTRANCE;

static s8 hasCopiedEntranceTable = 0;
static s8 hasModifiedEntranceTable = 0;

void Entrance_SetEntranceDiscovered(u16 entranceIndex);

u8 Entrance_EntranceIsNull(EntranceOverride* entranceOverride) {
    return entranceOverride->index == 0 && entranceOverride->destination == 0 && entranceOverride->blueWarp == 0
        && entranceOverride->override == 0 && entranceOverride->overrideDestination == 0;
}

static void Entrance_SeparateOGCFairyFountainExit(void) {
    //Overwrite unused entrance 0x03E8 with values from 0x0340 to use it as the
    //exit from OGC Great Fairy Fountain -> Castle Grounds
    for (size_t i = 0; i < 4; ++i) {
        gEntranceTable[0x3E8 + i] = gEntranceTable[0x340 + i];
    }
}

static void Entrance_SeparateAdultSpawnAndPrelude() {
    // Overwrite unused entrance 0x0282 with values from 0x05F4 to use it as the
    // Adult Spawn index and separate it from Prelude of Light
    for (size_t i = 0; i < 4; ++i) {
        gEntranceTable[0x282 + i] = gEntranceTable[0x5F4 + i];
    }
}

void Entrance_CopyOriginalEntranceTable(void) {
    if (!hasCopiedEntranceTable) {
        memcpy(originalEntranceTable, gEntranceTable, sizeof(EntranceInfo) * ENTRANCE_TABLE_SIZE);
        hasCopiedEntranceTable = 1;
    }
}

void Entrance_ResetEntranceTable(void) {
    if (hasCopiedEntranceTable && hasModifiedEntranceTable) {
        memcpy(gEntranceTable, originalEntranceTable, sizeof(EntranceInfo) * ENTRANCE_TABLE_SIZE);
        hasModifiedEntranceTable = 0;
    }
}

void Entrance_Init(void) {
    s32 index;

    Entrance_CopyOriginalEntranceTable();

    // Skip Child Stealth if given by settings
    if (Randomizer_GetSettingValue(RSK_SKIP_CHILD_STEALTH)) {
        gEntranceTable[0x07A].scene = 0x4A;
        gEntranceTable[0x07A].spawn = 0x00;
        gEntranceTable[0x07A].field = 0x0183;
    }

    // Delete the title card and add a fade in for Hyrule Field from Ocarina of Time cutscene
    for (index = 0x50F; index < 0x513; ++index) {
        gEntranceTable[index].field = 0x010B;
    }

    Entrance_SeparateOGCFairyFountainExit();
    Entrance_SeparateAdultSpawnAndPrelude();

    // Initialize the entrance override table with each index leading to itself. An
    // index referring to itself means that the entrance is not currently shuffled.
    for (s16 i = 0; i < ENTRANCE_TABLE_SIZE; i++) {
        entranceOverrideTable[i] = i;
    }

    // Initialize the grotto exit and load lists
    Grotto_InitExitAndLoadLists();

    // Then overwrite the indices which are shuffled
    for (size_t i = 0; i < ENTRANCE_OVERRIDES_MAX_COUNT; i++) {

        if (Entrance_EntranceIsNull(&gSaveContext.entranceOverrides[i])) {
            break;
        }

        s16 originalIndex = gSaveContext.entranceOverrides[i].index;
        s16 blueWarpIndex = gSaveContext.entranceOverrides[i].blueWarp;
        s16 overrideIndex = gSaveContext.entranceOverrides[i].override;

        //Overwrite grotto related indices
        if (originalIndex >= ENTRANCE_RANDO_GROTTO_EXIT_START) {
            Grotto_SetExitOverride(originalIndex, overrideIndex);
            continue;
        }

        if (originalIndex >= ENTRANCE_RANDO_GROTTO_LOAD_START && originalIndex < ENTRANCE_RANDO_GROTTO_EXIT_START) {
            Grotto_SetLoadOverride(originalIndex, overrideIndex);
            continue;
        }

        // Overwrite the indices which we want to shuffle, leaving the rest as they are
        entranceOverrideTable[originalIndex] = overrideIndex;

        if (blueWarpIndex != 0) {
            entranceOverrideTable[blueWarpIndex] = overrideIndex;
        }

        //Override both land and water entrances for Hyrule Field -> ZR Front and vice versa
        if (originalIndex == 0x00EA) { //Hyrule Field -> ZR Front land entrance
            entranceOverrideTable[0x01D9] = overrideIndex;
        } else if (originalIndex == 0x0181) { //ZR Front -> Hyrule Field land entrance
            entranceOverrideTable[0x0311] = overrideIndex;
        }
    }

    // Stop playing background music during shuffled entrance transitions
    // so that we don't get duplicated or overlapping music tracks
    if (Randomizer_GetSettingValue(RSK_SHUFFLE_OVERWORLD_ENTRANCES)) {

        s16 indicesToSilenceBackgroundMusic[2] = {
            // The lost woods music playing near the GC Woods Warp keeps playing
            // in the next area if the bvackground music is allowed to keep playing
            entranceOverrideTable[0x04D6], // Goron City -> Lost Woods override

            // If Malon is singing at night, then her singing will be transferred
            // to the next area if it allows the background music to keep playing
            entranceOverrideTable[0x025A], // Castle Grounds -> Market override
        };

        for (size_t j = 0; j < sizeof(indicesToSilenceBackgroundMusic) / sizeof(s16); j++) {

            s16 override = indicesToSilenceBackgroundMusic[j];
            for (s16 i = 0; i < 4; i++) {
                // Zero out the bit in the field which tells the game to keep playing
                // background music for all four scene setups at each index
                gEntranceTable[override + i].field &= ~0x8000;
            }
        }
    }

    hasModifiedEntranceTable = 1;
}

s16 Entrance_GetOverride(s16 index) {

    // The game sometimes uses special indices from 0x7FF9 -> 0x7FFF for exiting
    // grottos and fairy fountains. These aren't handled here since the game
    // naturally handles them later.
    if (index >= ENTRANCE_TABLE_SIZE) {
        return index;
    }

    return entranceOverrideTable[index];
}

s16 Entrance_OverrideNextIndex(s16 nextEntranceIndex) {
    // When entering Spirit Temple, clear temp flags so they don't carry over to the randomized dungeon
    if (nextEntranceIndex == 0x0082 && Entrance_GetOverride(nextEntranceIndex) != nextEntranceIndex &&
        gPlayState != NULL) {
        gPlayState->actorCtx.flags.tempSwch = 0;
        gPlayState->actorCtx.flags.tempCollect = 0;
    }

    // Exiting through the crawl space from Hyrule Castle courtyard is the same exit as leaving Ganon's castle
    // If we came from the Castle courtyard, then don't override the entrance to keep Link in Hyrule Castle area
    if (gPlayState != NULL && gPlayState->sceneNum == 69 && nextEntranceIndex == 0x023D) {
        return nextEntranceIndex;
    }

    Entrance_SetEntranceDiscovered(nextEntranceIndex);
    EntranceTracker_SetLastEntranceOverride(nextEntranceIndex);
    return Grotto_OverrideSpecialEntrance(Entrance_GetOverride(nextEntranceIndex));
}

s16 Entrance_OverrideDynamicExit(s16 dynamicExitIndex) {
    Entrance_SetEntranceDiscovered(dynamicExitList[dynamicExitIndex]);
    EntranceTracker_SetLastEntranceOverride(dynamicExitList[dynamicExitIndex]);
    return Grotto_OverrideSpecialEntrance(Entrance_GetOverride(dynamicExitList[dynamicExitIndex]));
}

u32 Entrance_SceneAndSpawnAre(u8 scene, u8 spawn) {
    s16 computedEntranceIndex;

    // Adjust the entrance to acount for the exact scene/spawn combination for child/adult and day/night
    if (!IS_DAY) {
        if (!LINK_IS_ADULT) {
            computedEntranceIndex = gSaveContext.entranceIndex + 1;
        } else {
            computedEntranceIndex = gSaveContext.entranceIndex + 3;
        }
    } else {
        if (!LINK_IS_ADULT) {
            computedEntranceIndex = gSaveContext.entranceIndex;
        } else {
            computedEntranceIndex = gSaveContext.entranceIndex + 2;
        }
    }

    EntranceInfo currentEntrance = gEntranceTable[computedEntranceIndex];
    return currentEntrance.scene == scene && currentEntrance.spawn == spawn;
}

//Properly respawn the player after a game over, accounding for dungeon entrance
//randomizer. It's easier to rewrite this entirely compared to performing an ASM
//dance for just the boss rooms. Entrance Indexes can be found here:
//https://wiki.cloudmodding.com/oot/Entrance_Table_(Data)
void Entrance_SetGameOverEntrance(void) {

    //Set the current entrance depending on which entrance the player last came through
    switch (gSaveContext.entranceIndex) {
        case 0x040F : //Deku Tree Boss Room
            gSaveContext.entranceIndex = newDekuTreeEntrance;
            return;
        case 0x040B : //Dodongos Cavern Boss Room
            gSaveContext.entranceIndex = newDodongosCavernEntrance;
            return;
        case 0x0301 : //Jabu Jabus Belly Boss Room
            gSaveContext.entranceIndex = newJabuJabusBellyEntrance;
            return;
        case 0x000C : //Forest Temple Boss Room
            gSaveContext.entranceIndex = newForestTempleEntrance;
            return;
        case 0x0305 : //Fire Temple Boss Room
            gSaveContext.entranceIndex = newFireTempleEntrance;
            return;
        case 0x0417 : //Water Temple Boss Room
            gSaveContext.entranceIndex = newWaterTempleEntrance;
            return;
        case 0x008D : //Spirit Temple Boss Room
            gSaveContext.entranceIndex = newSpiritTempleEntrance;
            return;
        case 0x0413 : //Shadow Temple Boss Room
            gSaveContext.entranceIndex = newShadowTempleEntrance;
            return;
        case 0x041F : //Ganondorf Boss Room
            gSaveContext.entranceIndex = 0x041B; // Inside Ganon's Castle -> Ganon's Tower Climb
            return;
    }
}

//Properly savewarp the player accounting for dungeon entrance randomizer.
//It's easier to rewrite this entirely compared to performing an ASM
//dance for just the boss rooms.
//https://wiki.cloudmodding.com/oot/Entrance_Table_(Data)
void Entrance_SetSavewarpEntrance(void) {

    s16 scene = gSaveContext.savedSceneNum;

    if (scene == SCENE_YDAN || scene == SCENE_YDAN_BOSS) {
        gSaveContext.entranceIndex = newDekuTreeEntrance;
    } else if (scene == SCENE_DDAN || scene == SCENE_DDAN_BOSS) {
        gSaveContext.entranceIndex = newDodongosCavernEntrance;
    } else if (scene == SCENE_BDAN || scene == SCENE_BDAN_BOSS) {
        gSaveContext.entranceIndex = newJabuJabusBellyEntrance;
    } else if (scene == SCENE_BMORI1 || scene == SCENE_MORIBOSSROOM) { //Forest Temple Boss Room
        gSaveContext.entranceIndex = newForestTempleEntrance;
    } else if (scene == SCENE_HIDAN || scene == SCENE_FIRE_BS) { //Fire Temple Boss Room
        gSaveContext.entranceIndex = newFireTempleEntrance;
    } else if (scene == SCENE_MIZUSIN || scene == SCENE_MIZUSIN_BS) { //Water Temple Boss Room
        gSaveContext.entranceIndex = newWaterTempleEntrance;
    } else if (scene == SCENE_JYASINZOU || scene == SCENE_JYASINBOSS) { //Spirit Temple Boss Room
        gSaveContext.entranceIndex = newSpiritTempleEntrance;
    } else if (scene == SCENE_HAKADAN || scene == SCENE_HAKADAN_BS) { //Shadow Temple Boss Room
        gSaveContext.entranceIndex = newShadowTempleEntrance;
    } else if (scene == SCENE_HAKADANCH) { // BOTW
        gSaveContext.entranceIndex = newBottomOfTheWellEntrance;
    } else if (scene == SCENE_MEN) { // GTG
        gSaveContext.entranceIndex = newGerudoTrainingGroundsEntrance;
    } else if (scene == SCENE_ICE_DOUKUTO) { // Ice cavern
        gSaveContext.entranceIndex = newIceCavernEntrance;
    } else if (scene == SCENE_GANONTIKA) {
        gSaveContext.entranceIndex = GANONS_CASTLE_ENTRANCE;
    } else if (scene == SCENE_GANON || scene == SCENE_GANONTIKA_SONOGO || scene == SCENE_GANON_SONOGO || scene == SCENE_GANON_DEMO || scene == SCENE_GANON_FINAL) {
        gSaveContext.entranceIndex = 0x041B; // Inside Ganon's Castle -> Ganon's Tower Climb
    } else if (scene == SCENE_GERUDOWAY) { // Theives hideout
        gSaveContext.entranceIndex = 0x0486; // Gerudo Fortress -> Thieve's Hideout spawn 0
    } else if (scene == SCENE_LINK_HOME) {
        gSaveContext.entranceIndex = Entrance_OverrideNextIndex(LINK_HOUSE_SAVEWARP_ENTRANCE);
    } else if (LINK_IS_CHILD) {
        gSaveContext.entranceIndex = Entrance_OverrideNextIndex(LINK_HOUSE_SAVEWARP_ENTRANCE); // Child Overworld Spawn
    } else {
        gSaveContext.entranceIndex = Entrance_OverrideNextIndex(0x0282); // Adult Overworld Spawn (Normally 0x5F4, but 0x282 has been repurposed to differentiate from Prelude which also uses 0x5F4)
    }
}

void Entrance_SetWarpSongEntrance(void) {
    gPlayState->sceneLoadFlag = 0x14;
    gPlayState->fadeTransition = 5;
    switch (gPlayState->msgCtx.lastPlayedSong) {
        case 0:
            gPlayState->nextEntranceIndex = Entrance_OverrideNextIndex(0x0600); // Minuet
            break;
        case 1:
            gPlayState->nextEntranceIndex = Entrance_OverrideNextIndex(0x04F6); // Bolero
            break;
        case 2:
            gPlayState->nextEntranceIndex = Entrance_OverrideNextIndex(0x0604); // Serenade
            break;
        case 3:
            gPlayState->nextEntranceIndex = Entrance_OverrideNextIndex(0x01F1); // Requiem
            break;
        case 4:
            gPlayState->nextEntranceIndex = Entrance_OverrideNextIndex(0x0568); // Nocturne
            break;
        case 5:
            gPlayState->nextEntranceIndex = Entrance_OverrideNextIndex(0x05F4); // Prelude
            break;
        default:
            gPlayState->sceneLoadFlag = 0; // if something goes wrong, the animation plays normally
    }

    // If one of the warp songs happens to lead to a grotto return, then we
    // have to force the grotto return afterwards
    Grotto_ForceGrottoReturnOnSpecialEntrance();

    if (gSaveContext.gameMode != 0) {
        // During DHWW the cutscene must play at the destination
        gSaveContext.respawnFlag = -3;
    } else if (gSaveContext.respawnFlag == -3) {
        // Unset Zoneout Type -3 to avoid cutscene at destination (technically it's not needed)
        gSaveContext.respawnFlag = 0;
    }
}

void Entrance_OverrideBlueWarp(void) {
    // Set nextEntranceIndex as a flag so that Grotto_CheckSpecialEntrance
    // won't return index 0x7FFF, which can't work to override blue warps.
    gPlayState->nextEntranceIndex = 0;

    switch (gPlayState->sceneNum) {
        case SCENE_YDAN_BOSS: // Ghoma boss room
            gPlayState->nextEntranceIndex = Entrance_OverrideNextIndex(0x0457);
            return;
        case SCENE_DDAN_BOSS: // King Dodongo boss room
            gPlayState->nextEntranceIndex = Entrance_OverrideNextIndex(0x047A);
            return;
        case SCENE_BDAN_BOSS: // Barinade boss room
            gPlayState->nextEntranceIndex = Entrance_OverrideNextIndex(0x010E);
            return;
        case SCENE_MORIBOSSROOM: // Phantom Ganon boss room
            gPlayState->nextEntranceIndex = Entrance_OverrideNextIndex(0x0608);
            return;
        case SCENE_FIRE_BS: // Volvagia boss room
            gPlayState->nextEntranceIndex = Entrance_OverrideNextIndex(0x0564);
            return;
        case SCENE_MIZUSIN_BS: // Morpha boss room
            gPlayState->nextEntranceIndex = Entrance_OverrideNextIndex(0x060C);
            return;
        case SCENE_JYASINBOSS: // Bongo-Bongo boss room
            gPlayState->nextEntranceIndex = Entrance_OverrideNextIndex(0x0610);
            return;
        case SCENE_HAKADAN_BS: // Twinrova boss room
            gPlayState->nextEntranceIndex = Entrance_OverrideNextIndex(0x0580);
            return;
    }
}

void Entrance_OverrideCutsceneEntrance(u16 cutsceneCmd) {
    switch (cutsceneCmd) {
        case 24: // Dropping a fish for Jabu Jabu
            gPlayState->nextEntranceIndex = Entrance_OverrideNextIndex(newJabuJabusBellyEntrance);
            gPlayState->sceneLoadFlag = 0x14;
            gPlayState->fadeTransition = 2;
            // In case Jabu's mouth leads to a grotto return
            Grotto_ForceGrottoReturnOnSpecialEntrance();
            break;
    }
}

void Entrance_EnableFW(void) {
    Player* player = GET_PLAYER(gPlayState);
    // Leave restriction in Tower Collapse Interior, Castle Collapse, Treasure Box Shop, Tower Collapse Exterior,
    // Grottos area, Fishing Pond, Ganon Battle and for states that disable buttons.
    if (!false /* farores wind anywhere */ ||
        gPlayState->sceneNum == 14 || gPlayState->sceneNum == 15 || (gPlayState->sceneNum == 16 && !false /* shuffled chest mini game */) ||
        gPlayState->sceneNum == 26 || gPlayState->sceneNum == 62 || gPlayState->sceneNum == 73 ||
        gPlayState->sceneNum == 79 ||
        gSaveContext.eventInf[0] & 0x1 ||   // Ingo's Minigame state
        player->stateFlags1 & 0x08A02000 || // Swimming, riding horse, Down A, hanging from a ledge
        player->stateFlags2 & 0x00040000    // Blank A
        // Shielding, spinning and getting skull tokens still disable buttons automatically
        ) {
        return;
    }

    for (int i = 1; i < 5; i++) {
        if (gSaveContext.equips.buttonItems[i] == 13) {
            gSaveContext.buttonStatus[i] = BTN_ENABLED;
        }
    }
}

// Check if Link should still be riding epona after changing entrances
void Entrance_HandleEponaState(void) {
    s32 entrance = gPlayState->nextEntranceIndex;
    Player* player = GET_PLAYER(gPlayState);
    //If Link is riding Epona but he's about to go through an entrance where she can't spawn,
    //unset the Epona flag to avoid Master glitch, and restore temp B.
    if (Randomizer_GetSettingValue(RSK_SHUFFLE_OVERWORLD_ENTRANCES) && (player->stateFlags1 & PLAYER_STATE1_23)) {

        static const s16 validEponaEntrances[] = {
            0x0102, // Hyrule Field -> Lake Hylia
            0x0189, // Lake Hylia -> Hyrule Field
            0x0309, // LH Fishing Hole -> LH Fishing Island
            0x03CC, // LH Lab -> Lake Hylia
            0x0117, // Hyrule Field -> Gerudo Valley
            0x018D, // Gerudo Valley -> Hyrule Field
            0x0157, // Hyrule Field -> Lon Lon Ranch
            0x01F9, // Lon Lon Ranch -> Hyrule Field
            0x01FD, // Market Entrance -> Hyrule Field
            0x00EA, // ZR Front -> Hyrule Field
            0x0181, // LW Bridge -> Hyrule Field
            0x0129, // GV Fortress Side -> Gerudo Fortress
            0x022D, // Gerudo Fortress -> GV Fortress Side
            0x03D0, // GV Carpenter Tent -> GV Fortress Side
            0x0496, // Gerudo Fortress -> Thieves Hideout
            0x042F, // LLR Stables -> Lon Lon Ranch
            0x05D4, // LLR Tower -> Lon Lon Ranch
            0x0378, // LLR Talons House -> Lon Lon Ranch
            0x028A, // LLR Southern Fence Jump
            0x028E, // LLR Western Fence Jump
            0x0292, // LLR Eastern Fence Jump
            0x0476, // LLR Front Gate Jump
            // The following indices currently aren't randomized, but we'll list
            // them in case they ever are. They're all Theives Hideout -> Gerudo Fortress
            0x231,
            0x235,
            0x239,
            0x2BA,
        };
        for (size_t i = 0; i < ARRAY_COUNT(validEponaEntrances); i++) {
            // If the entrance is equal to any of the valid ones, return and
            // don't change anything
            if (entrance == validEponaEntrances[i]) {
                return;
            }
        }

        // Update Link's status to no longer be riding epona so that the next time link
        // enters an epona supported area, he isn't automatically placed on epona
        player->stateFlags1 &= ~PLAYER_STATE1_23;
        player->actor.parent = NULL;
        AREG(6) = 0;
        gSaveContext.equips.buttonItems[0] = gSaveContext.buttonStatus[0]; //"temp B"
    }
}

// Certain weather states don't clear normally in entrance rando, so we need to reset and re-apply
// the correct weather state based on the current entrance and scene
void Entrance_OverrideWeatherState() {
    gWeatherMode = 0;
    gPlayState->envCtx.gloomySkyMode = 0;

    // Weather only applyies to adult link
    if (LINK_IS_CHILD || gSaveContext.sceneSetupIndex >= 4) {
        return;
    }

    // Hyrule Market
    if (gSaveContext.entranceIndex == 0x01FD) { // Hyrule Field by Market Entrance
        gWeatherMode = 1;
        return;
    }
    // Lon Lon Ranch (No Epona)
    if (!Flags_GetEventChkInf(0x18)){ // if you don't have Epona
        switch (gSaveContext.entranceIndex) {
            case 0x0157: // Lon Lon Ranch from HF
            case 0x01F9: // Hyrule Field from LLR
                gWeatherMode = 2;
                return;
        }
    }
    // Water Temple
    if (!(gSaveContext.eventChkInf[4] & 0x0400)) { // have not beaten Water Temple
        switch (gSaveContext.entranceIndex) {
            case 0x019D: // Zora River from behind waterfall
            case 0x01DD: // Zora River from LW water shortcut
            case 0x04DA: // Lost Woods water shortcut from ZR
                gWeatherMode = 3;
                return;
        }
        switch (gPlayState->sceneNum) {
            case 88: // Zora's Domain
            case 89: // Zora's Fountain
                gWeatherMode = 3;
                return;
        }
    }
    // Kakariko Thunderstorm
    if (((gSaveContext.inventory.questItems & 0x7) == 0x7) && // Have forest, fire, and water medallion
        !(gSaveContext.sceneFlags[24].clear & 0x02)) { // have not beaten Bongo Bongo
        switch (gPlayState->sceneNum) {
            case 82: // Kakariko
            case 83: // Graveyard
                gPlayState->envCtx.gloomySkyMode = 2;
                switch (gSaveContext.entranceIndex) {
                    case 0x00DB: // Kakariko from HF
                    case 0x0191: // Kakariko from Death Mountain Trail
                    case 0x0205: // Graveyard from Shadow Temple
                        break;
                    default:
                        gWeatherMode = 5;
                        return;
                }
        }
    }
    // Death Mountain Cloudy
    if (!(gSaveContext.eventChkInf[4] & 0x0200)) { // have not beaten Fire Temple
        if (gPlayState->nextEntranceIndex == 0x04D6) { // Lost Woods Goron City Shortcut
            gWeatherMode = 2;
            return;
        }
        switch (gPlayState->sceneNum) {
            case 82: // Kakariko
            case 83: // Graveyard
            case 96: // Death Mountain Trail
            case 97: // Death Mountain Crater
                if (!gPlayState->envCtx.gloomySkyMode) {
                    gPlayState->envCtx.gloomySkyMode = 1;
                }
                switch (gSaveContext.entranceIndex) {
                    case 0x00DB: // Kakariko from HF
                    case 0x0195: // Kakariko from Graveyard
                        break;
                    default:
                        gWeatherMode = 2;
                        return;
                }
        }
    }
}

// Rectify the "Getting Caught By Gerudo" entrance index if necessary, based on the age and current scene
// In ER, Adult should be placed at the fortress entrance when getting caught in the fortress without a hookshot, instead of being thrown in the valley
// Child should always be thrown in the stream when caught in the valley, and placed at the fortress entrance from valley when caught in the fortress
void Entrance_OverrideGeurdoGuardCapture(void) {
    if (LINK_IS_CHILD) {
        gPlayState->nextEntranceIndex = 0x1A5;
    }

    if ((LINK_IS_CHILD || Randomizer_GetSettingValue(RSK_SHUFFLE_OVERWORLD_ENTRANCES)) &&
        gPlayState->nextEntranceIndex == 0x1A5) { // Geurdo Valley thrown out
        if (gPlayState->sceneNum != 0x5A) { // Geurdo Valley
            gPlayState->nextEntranceIndex = 0x129; // Gerudo Fortress
        }
    }
}

void Entrance_OverrideSpawnScene(s32 sceneNum, s32 spawn) {
    if (Randomizer_GetSettingValue(RSK_SHUFFLE_DUNGEON_ENTRANCES) == RO_DUNGEON_ENTRANCE_SHUFFLE_ON_PLUS_GANON) {
        // Move Hyrule's Castle Courtyard exit spawn to be before the crates so players don't skip Talon
        if (sceneNum == 95 && spawn == 1) {
            gPlayState->linkActorEntry->pos.x = 0x033A;
            gPlayState->linkActorEntry->pos.y = 0x0623;
            gPlayState->linkActorEntry->pos.z = 0xFF22;
        }

        // Move Ganon's Castle exit spawn to be on the small ledge near the castle and not over the void
        if (sceneNum == 100 && spawn == 1) {
            gPlayState->linkActorEntry->pos.x = 0xFEA8;
            gPlayState->linkActorEntry->pos.y = 0x065C;
            gPlayState->linkActorEntry->pos.z = 0x0290;
            gPlayState->linkActorEntry->rot.y = 0x0700;
            gPlayState->linkActorEntry->params = 0x0DFF; // stationary spawn
        }
    }
}

u8 Entrance_GetIsSceneDiscovered(u8 sceneNum) {
    u16 bitsPerIndex = sizeof(u32) * 8;
    u32 idx = sceneNum / bitsPerIndex;
    if (idx < SAVEFILE_SCENES_DISCOVERED_IDX_COUNT) {
        u32 sceneBit = 1 << (sceneNum - (idx * bitsPerIndex));
        return (gSaveContext.sohStats.scenesDiscovered[idx] & sceneBit) != 0;
    }
    return 0;
}

void Entrance_SetSceneDiscovered(u8 sceneNum) {
    if (Entrance_GetIsSceneDiscovered(sceneNum)) {
        return;
    }

    u16 bitsPerIndex = sizeof(u32) * 8;
    u32 idx = sceneNum / bitsPerIndex;
    if (idx < SAVEFILE_SCENES_DISCOVERED_IDX_COUNT) {
        u32 sceneBit = 1 << (sceneNum - (idx * bitsPerIndex));
        gSaveContext.sohStats.scenesDiscovered[idx] |= sceneBit;
    }
}

u8 Entrance_GetIsEntranceDiscovered(u16 entranceIndex) {
    u16 bitsPerIndex = sizeof(u32) * 8;
    u32 idx = entranceIndex / bitsPerIndex;
    if (idx < SAVEFILE_ENTRANCES_DISCOVERED_IDX_COUNT) {
        u32 entranceBit = 1 << (entranceIndex - (idx * bitsPerIndex));
        return (gSaveContext.sohStats.entrancesDiscovered[idx] & entranceBit) != 0;
    }
    return 0;
}

void Entrance_SetEntranceDiscovered(u16 entranceIndex) {
    // Skip if already set to save time from setting the connected entrance or
    // if this entrance is outside of the randomized entrance range (i.e. is a dynamic entrance)
    if (entranceIndex > MAX_ENTRANCE_RANDO_USED_INDEX || Entrance_GetIsEntranceDiscovered(entranceIndex)) {
        return;
    }

    u16 bitsPerIndex = sizeof(u32) * 8;
    u32 idx = entranceIndex / bitsPerIndex;
    if (idx < SAVEFILE_ENTRANCES_DISCOVERED_IDX_COUNT) {
        u32 entranceBit = 1 << (entranceIndex - (idx * bitsPerIndex));
        gSaveContext.sohStats.entrancesDiscovered[idx] |= entranceBit;
        // Set connected
        for (size_t i = 0; i < ENTRANCE_OVERRIDES_MAX_COUNT; i++) {
            if (entranceIndex == gSaveContext.entranceOverrides[i].index) {
                Entrance_SetEntranceDiscovered(gSaveContext.entranceOverrides[i].overrideDestination);
                break;
            }
        }
    }
}
