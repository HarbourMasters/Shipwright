#include "randomizer_entrance.h"
#include "randomizer_grotto.h"
#include <string.h>

#include "global.h"

GlobalContext* gGlobalCtx;

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

typedef void (*SetNextEntrance_proc)(struct GlobalContext* globalCtx, s16 entranceIndex, u32 sceneLoadFlag, u32 transition);
#define SetNextEntrance_addr 0x3716F0
#define SetNextEntrance ((SetNextEntrance_proc)SetNextEntrance_addr)

#define dynamicExitList_addr 0x53C094
#define dynamicExitList ((s16*)dynamicExitList_addr) // = { 0x045B, 0x0482, 0x0340, 0x044B, 0x02A2, 0x0201, 0x03B8, 0x04EE, 0x03C0, 0x0463, 0x01CD, 0x0394, 0x0340, 0x057C }

// Warp Song indices array : 0x53C33C = { 0x0600, 0x04F6, 0x0604, 0x01F1, 0x0568, 0x05F4 }

// Owl Flights : 0x492064 and 0x492080

EntranceOverride rEntranceOverrides[ENTRANCE_OVERRIDES_MAX_COUNT] = {0};
EntranceOverride destList[ENTRANCE_OVERRIDES_MAX_COUNT] = {0};
EntranceTrackingData gEntranceTrackingData = {0};
static s16 entranceOverrideTable[ENTRANCE_TABLE_SIZE] = {0};

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

u8 Entrance_EntranceIsNull(EntranceOverride* entranceOverride) {
    return entranceOverride->index == 0 && entranceOverride->destination == 0 && entranceOverride->blueWarp == 0
        && entranceOverride->override == 0 && entranceOverride->overrideDestination == 0;
}

// void Scene_Init(void) {
//     memcpy(&gSceneTable[0],  gSettingsContext.dekuTreeDungeonMode              == DUNGEONMODE_MQ ? &gMQDungeonSceneTable[0]  : &gDungeonSceneTable[0],  sizeof(Scene));
//     memcpy(&gSceneTable[1],  gSettingsContext.dodongosCavernDungeonMode        == DUNGEONMODE_MQ ? &gMQDungeonSceneTable[1]  : &gDungeonSceneTable[1],  sizeof(Scene));
//     memcpy(&gSceneTable[2],  gSettingsContext.jabuJabusBellyDungeonMode        == DUNGEONMODE_MQ ? &gMQDungeonSceneTable[2]  : &gDungeonSceneTable[2],  sizeof(Scene));
//     memcpy(&gSceneTable[3],  gSettingsContext.forestTempleDungeonMode          == DUNGEONMODE_MQ ? &gMQDungeonSceneTable[3]  : &gDungeonSceneTable[3],  sizeof(Scene));
//     memcpy(&gSceneTable[4],  gSettingsContext.fireTempleDungeonMode            == DUNGEONMODE_MQ ? &gMQDungeonSceneTable[4]  : &gDungeonSceneTable[4],  sizeof(Scene));
//     memcpy(&gSceneTable[5],  gSettingsContext.waterTempleDungeonMode           == DUNGEONMODE_MQ ? &gMQDungeonSceneTable[5]  : &gDungeonSceneTable[5],  sizeof(Scene));
//     memcpy(&gSceneTable[6],  gSettingsContext.spiritTempleDungeonMode          == DUNGEONMODE_MQ ? &gMQDungeonSceneTable[6]  : &gDungeonSceneTable[6],  sizeof(Scene));
//     memcpy(&gSceneTable[7],  gSettingsContext.shadowTempleDungeonMode          == DUNGEONMODE_MQ ? &gMQDungeonSceneTable[7]  : &gDungeonSceneTable[7],  sizeof(Scene));
//     memcpy(&gSceneTable[8],  gSettingsContext.bottomOfTheWellDungeonMode       == DUNGEONMODE_MQ ? &gMQDungeonSceneTable[8]  : &gDungeonSceneTable[8],  sizeof(Scene));
//     memcpy(&gSceneTable[9],  gSettingsContext.iceCavernDungeonMode             == DUNGEONMODE_MQ ? &gMQDungeonSceneTable[9]  : &gDungeonSceneTable[9],  sizeof(Scene));
//     memcpy(&gSceneTable[11], gSettingsContext.gerudoTrainingGroundsDungeonMode == DUNGEONMODE_MQ ? &gMQDungeonSceneTable[11] : &gDungeonSceneTable[11], sizeof(Scene));
//     memcpy(&gSceneTable[13], gSettingsContext.ganonsCastleDungeonMode          == DUNGEONMODE_MQ ? &gMQDungeonSceneTable[13] : &gDungeonSceneTable[13], sizeof(Scene));

//     gRestrictionFlags[72].flags2 = 0; // Allows warp songs in GTG
//     //gRestrictionFlags[93].flags2 = 0; // Allows warp songs in Windmill / Dampe's grave
//     gRestrictionFlags[94].flags2 = 0; // Allows warp songs in Ganon's Castle

//     gRestrictionFlags[72].flags3 = 0; // Allows farore's wind in GTG
//     gRestrictionFlags[94].flags3 = 0; // Allows farore's wind in Ganon's Castle
// }

static void Entrance_SeparateOGCFairyFountainExit(void) {
    //Overwrite unused entrance 0x03E8 with values from 0x0340 to use it as the
    //exit from OGC Great Fairy Fountain -> Castle Grounds
    for (size_t i = 0; i < 4; ++i) {
        gEntranceTable[0x3E8 + i] = gEntranceTable[0x340 + i];
    }

    //Overwrite the dynamic exit for the OGC Fairy Fountain to be 0x3E8 instead
    //of 0x340 (0x340 will stay as the exit for the HC Fairy Fountain -> Castle Grounds)
    // dynamicExitList[2] = 0x03E8;
}

void Entrance_Init(void) {
    s32 index;

    // Skip Child Stealth if given by settings
    if (Randomizer_GetSettingValue(RSK_SKIP_CHILD_STEALTH)) {
        gEntranceTable[0x07A].scene = 0x4A;
        gEntranceTable[0x07A].spawn = 0x00;
        gEntranceTable[0x07A].field = 0x0183;
    }

    // Skip Tower Escape Sequence if given by settings
    if (Randomizer_GetSettingValue(RSK_SKIP_TOWER_ESCAPE)) {
        gEntranceTable[0x43F].scene = 0x4F;
        gEntranceTable[0x43F].spawn = 0x01;
        gEntranceTable[0x43F].field = 0x4183;
    }

    // Delete the title card and add a fade in for Hyrule Field from Ocarina of Time cutscene
    for (index = 0x50F; index < 0x513; ++index) {
        gEntranceTable[index].field = 0x010B;
    }

    Entrance_SeparateOGCFairyFountainExit();

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
        if (originalIndex >= 0x0800) {
            Grotto_SetExitOverride(originalIndex, overrideIndex);
            continue;
        }

        if (originalIndex >= 0x0700 && originalIndex < 0x0800) {
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
    if (nextEntranceIndex == 0x0082 && Entrance_GetOverride(nextEntranceIndex) != nextEntranceIndex) {
        gGlobalCtx->actorCtx.flags.tempSwch = 0;
        gGlobalCtx->actorCtx.flags.tempCollect = 0;
    }
    // SaveFile_SetEntranceDiscovered(nextEntranceIndex);
    return Grotto_CheckSpecialEntrance(Entrance_GetOverride(nextEntranceIndex));
}

void Entrance_OverrideDynamicExit(void) {
    // SaveFile_SetEntranceDiscovered(gGlobalCtx->nextEntranceIndex);
    gGlobalCtx->nextEntranceIndex = Grotto_CheckSpecialEntrance(Entrance_GetOverride(gGlobalCtx->nextEntranceIndex));
}

void Entrance_DeathInGanonBattle(void) {
    if ((gGlobalCtx->sceneNum == 0x004F) && (Randomizer_GetSettingValue(RSK_SKIP_TOWER_ESCAPE))) {
        SetNextEntrance(gGlobalCtx, 0x517, 0x14, 2);
    } else {
        SetNextEntrance(gGlobalCtx, 0x43F, 0x14, 2);
    }
}

u32 Entrance_SceneAndSpawnAre(u8 scene, u8 spawn) {
    EntranceInfo currentEntrance = gEntranceTable[gSaveContext.entranceIndex];
    return currentEntrance.scene == scene && currentEntrance.spawn == spawn;
}

u32 Entrance_IsLostWoodsBridge(void) {
    //  Kokiri Forest -> LW Bridge, index 05E0   Hyrule Field -> LW Bridge, index 04DE
    if (Entrance_SceneAndSpawnAre(0x5B, 0x09) || Entrance_SceneAndSpawnAre(0x5B, 0x08)) {
      return 1;
    } else {
      return 0;
    }
}

u8 IsInGame(void) {
    s32 entr = gSaveContext.entranceIndex;
    s32 mode = gSaveContext.gameMode;
    return mode == 0 ||
        (mode == 1 && (gSaveContext.cutsceneIndex < 0xFFF0 ||
                       (entr != 0x0629 && entr != 0x0147 && entr != 0x00A0 && entr != 0x008D)));
}

void Entrance_EnteredLocation(void) {
    if (!IsInGame()) {
        return;
    }
    // SaveFile_SetSceneDiscovered(gGlobalCtx->sceneNum);
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
        gSaveContext.entranceIndex = LINK_HOUSE_SAVEWARP_ENTRANCE;
    } else if (gSaveContext.linkAge == 1) { // child
        gSaveContext.entranceIndex = Entrance_GetOverride(LINK_HOUSE_SAVEWARP_ENTRANCE);
    } else {
        gSaveContext.entranceIndex = Entrance_GetOverride(0x05F4); // Temple of Time Adult Spawn
    }
}

void EnableFW(void) {
    Player* player = GET_PLAYER(gGlobalCtx);
    // Leave restriction in Tower Collapse Interior, Castle Collapse, Treasure Box Shop, Tower Collapse Exterior,
    // Grottos area, Fishing Pond, Ganon Battle and for states that disable buttons.
    if (!false /* farores wind anywhere */ ||
        gGlobalCtx->sceneNum == 14 || gGlobalCtx->sceneNum == 15 || (gGlobalCtx->sceneNum == 16 && !false /* shuffled chest mini game */) ||
        gGlobalCtx->sceneNum == 26 || gGlobalCtx->sceneNum == 62 || gGlobalCtx->sceneNum == 73 ||
        gGlobalCtx->sceneNum == 79 ||
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

u8 EntranceCutscene_ShouldPlay(u8 flag) {
    if (gSaveContext.gameMode != 0 || flag == 0x18 || flag == 0xAD || (flag >= 0xBB && flag <= 0xBF)) {
        if (flag == 0xC0) {
            gSaveContext.eventChkInf[0x3] &= ~0x0800; // clear "began Nabooru battle"
        }
        return 1; // cutscene will play normally in DHWW, or always if it's freeing Epona or clearing a Trial
    }
    // EventSet(flag);
    return 0; //cutscene will not play
}

// Check if Link should still be riding epona after changing entrances
void Entrance_CheckEpona(void) {
    s32 entrance = gGlobalCtx->nextEntranceIndex;
    Player* player = GET_PLAYER(gGlobalCtx);
    //If Link is riding Epona but he's about to go through an entrance where she can't spawn,
    //unset the Epona flag to avoid Master glitch, and restore temp B.
    if (Randomizer_GetSettingValue(RSK_SHUFFLE_OVERWORLD_ENTRANCES) && (player->stateFlags1 & 0x00800000)) {

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
        for (size_t i = 0; i < ARRAY_SIZE(validEponaEntrances); i++) {
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

const EntranceData entranceData[] = {
    { 0x0000, "KF" /* > Deku Tree */, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_DUNGEON },
    { 0x0209, "Deku Tree" /* > KF */, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_DUNGEON, 1 },
    { 0x0004, "DMT" /* > Dodongo's Cavern */, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_TYPE_DUNGEON },
    { 0x0242, "Dodongo's Cavern" /* > DMT */, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_TYPE_DUNGEON, 1 },
    { 0x0028, "ZF" /* > Jabu Jabu's Belly */, ENTRANCE_GROUP_ZORAS_DOMAIN, ENTRANCE_TYPE_DUNGEON },
    { 0x0221, "Jabu Jabu's Belly" /* > ZF */, ENTRANCE_GROUP_ZORAS_DOMAIN, ENTRANCE_TYPE_DUNGEON, 1 },
    { 0x0169, "SFM" /* > Forest Temple */, ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_TYPE_DUNGEON },
    { 0x0215, "Forest Temple" /* > SFM */, ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_TYPE_DUNGEON, 1 },
    { 0x0165, "DMC" /* > Fire Temple */, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DUNGEON },
    { 0x024A, "Fire Temple" /* > DMC */, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_DUNGEON, 1 },
    { 0x0010, "LH" /* > Water Temple */, ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_TYPE_DUNGEON },
    { 0x021D, "Water Temple" /* > LH */, ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_TYPE_DUNGEON, 1 },
    { 0x0082, "Desert Colossus" /* > Spirit Temple */, ENTRANCE_GROUP_HAUNTED_WASTELAND, ENTRANCE_TYPE_DUNGEON },
    { 0x01E1, "Spirit Temple" /* > Desert Colossus */, ENTRANCE_GROUP_HAUNTED_WASTELAND, ENTRANCE_TYPE_DUNGEON, 1 },
    { 0x0037, "Graveyard" /* > Shadow Temple */, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_DUNGEON },
    { 0x0205, "Shadow Temple" /* > Graveyard */, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_DUNGEON, 1 },
    { 0x0098, "Kakariko" /* > Bottom of the Well */, ENTRANCE_GROUP_KAKARIKO, ENTRANCE_TYPE_DUNGEON },
    { 0x02A6, "Bottom of the Well" /* > Kakariko */, ENTRANCE_GROUP_KAKARIKO, ENTRANCE_TYPE_DUNGEON, 1 },
    { 0x0088, "ZF" /* > Ice Cavern */, ENTRANCE_GROUP_ZORAS_DOMAIN, ENTRANCE_TYPE_DUNGEON },
    { 0x03D4, "Ice Cavern" /* > ZF */, ENTRANCE_GROUP_ZORAS_DOMAIN, ENTRANCE_TYPE_DUNGEON, 1 },
    { 0x0008, "GF" /* > Gerudo Training Grounds */, ENTRANCE_GROUP_GERUDO_VALLEY, ENTRANCE_TYPE_DUNGEON },
    { 0x03A8, "Gerudo Training Grounds" /* > GF */, ENTRANCE_GROUP_GERUDO_VALLEY, ENTRANCE_TYPE_DUNGEON, 1 },
    { 0x0467, "OGC" /* > Ganon's Castle */, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_DUNGEON },
    { 0x023D, "Ganon's Castle" /* > OGC */, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_DUNGEON, 1 },

    { 0x0272, "KF" /* > Link's House */, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_INTERIOR },
    { 0x0433, "KF" /* > Mido's House */, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_INTERIOR },
    { 0x0437, "KF" /* > Saria's House */, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_INTERIOR },
    { 0x009C, "KF" /* > House of Twins */, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_INTERIOR },
    { 0x00C9, "KF" /* > Know-It-All House */, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_INTERIOR },
    { 0x00C1, "KF" /* > KF Shop */, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_INTERIOR },
    { 0x0211, "Link's House" /* > KF */, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_INTERIOR, 1 },
    { 0x0443, "Mido's House" /* > KF */, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_INTERIOR, 1 },
    { 0x0447, "Saria's House" /* > KF */, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_INTERIOR, 1 },
    { 0x033C, "House of Twins" /* > KF */, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_INTERIOR, 1 },
    { 0x026A, "Know-It-All House" /* > KF */, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_INTERIOR, 1 },
    { 0x0266, "KF Shop" /* > KF */, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_INTERIOR, 1 },
    { 0x02FD, "Kakariko" /* > Carpenter Boss House */, ENTRANCE_GROUP_KAKARIKO, ENTRANCE_TYPE_INTERIOR },
    { 0x0550, "Kakariko" /* > House of Skulltula */, ENTRANCE_GROUP_KAKARIKO, ENTRANCE_TYPE_INTERIOR },
    { 0x039C, "Kakariko" /* > Impa's House Front */, ENTRANCE_GROUP_KAKARIKO, ENTRANCE_TYPE_INTERIOR },
    { 0x05C8, "Kakariko" /* > Impa's House Back */, ENTRANCE_GROUP_KAKARIKO, ENTRANCE_TYPE_INTERIOR },
    { 0x0453, "Kakariko" /* > Windmill */, ENTRANCE_GROUP_KAKARIKO, ENTRANCE_TYPE_INTERIOR },
    { 0x003B, "Kakariko" /* > Kak Shooting Gallery */, ENTRANCE_GROUP_KAKARIKO, ENTRANCE_TYPE_INTERIOR },
    { 0x0072, "Kakariko" /* > Granny's Potion Shop */, ENTRANCE_GROUP_KAKARIKO, ENTRANCE_TYPE_INTERIOR },
    { 0x00B7, "Kakariko" /* > Kak Bazaar */, ENTRANCE_GROUP_KAKARIKO, ENTRANCE_TYPE_INTERIOR },
    { 0x0384, "Kakariko" /* > Kak Potion Shop Front */, ENTRANCE_GROUP_KAKARIKO, ENTRANCE_TYPE_INTERIOR },
    { 0x03EC, "Kakariko" /* > Kak Potion Shop Back */, ENTRANCE_GROUP_KAKARIKO, ENTRANCE_TYPE_INTERIOR },
    { 0x0349, "Carpenter Boss House" /* > Kakariko */, ENTRANCE_GROUP_KAKARIKO, ENTRANCE_TYPE_INTERIOR, 1 },
    { 0x04EE, "House of Skulltula" /* > Kakariko */, ENTRANCE_GROUP_KAKARIKO, ENTRANCE_TYPE_INTERIOR, 1 },
    { 0x0345, "Impa's House Front" /* > Kakariko */, ENTRANCE_GROUP_KAKARIKO, ENTRANCE_TYPE_INTERIOR, 1 },
    { 0x05DC, "Impa's House Back" /* > Kakariko */, ENTRANCE_GROUP_KAKARIKO, ENTRANCE_TYPE_INTERIOR, 1 },
    { 0x0351, "Windmill" /* > Kakariko */, ENTRANCE_GROUP_KAKARIKO, ENTRANCE_TYPE_INTERIOR, 1 },
    { 0x0463, "Kak Shooting Gallery" /* > Kakariko */, ENTRANCE_GROUP_KAKARIKO, ENTRANCE_TYPE_INTERIOR, 1 },
    { 0x034D, "Granny's Potion Shop" /* > Kakariko */, ENTRANCE_GROUP_KAKARIKO, ENTRANCE_TYPE_INTERIOR, 1 },
    { 0x0201, "Kak Bazaar" /* > Kakariko */, ENTRANCE_GROUP_KAKARIKO, ENTRANCE_TYPE_INTERIOR, 1 },
    { 0x044B, "Kak Potion Shop Front" /* > Kakariko */, ENTRANCE_GROUP_KAKARIKO, ENTRANCE_TYPE_INTERIOR, 1 },
    { 0x04FF, "Kak Potion Shop Back" /* > Kakariko */, ENTRANCE_GROUP_KAKARIKO, ENTRANCE_TYPE_INTERIOR, 1 },
    { 0x030D, "Graveyard" /* > Dampe's Shack */, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_INTERIOR },
    { 0x0355, "Dampe's Shack" /* > Graveyard */, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_INTERIOR, 1 },
    { 0x0315, "DMT" /* > DMT Great Fairy Fountain */, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_TYPE_INTERIOR },
    { 0x045B, "DMT Great Fairy Fountain" /* > DMT */, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_TYPE_INTERIOR, 1 },
    { 0x04BE, "DMC" /* > DMC Great Fairy Fountain */, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_INTERIOR },
    { 0x0482, "DMC Great Fairy Fountain" /* > DMC */, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_INTERIOR, 1 },
    { 0x037C, "Goron City" /* > Goron Shop */, ENTRANCE_GROUP_GORON_CITY, ENTRANCE_TYPE_INTERIOR },
    { 0x03FC, "Goron Shop" /* > Goron City */, ENTRANCE_GROUP_GORON_CITY, ENTRANCE_TYPE_INTERIOR, 1 },
    { 0x0380, "Zora's Domain" /* > Zora Shop */, ENTRANCE_GROUP_ZORAS_DOMAIN, ENTRANCE_TYPE_INTERIOR },
    { 0x03C4, "Zora Shop" /* > Zora's Domain */, ENTRANCE_GROUP_ZORAS_DOMAIN, ENTRANCE_TYPE_INTERIOR, 1 },
    { 0x0371, "ZF" /* > ZF Great Fairy Fountain */, ENTRANCE_GROUP_ZORAS_DOMAIN, ENTRANCE_TYPE_INTERIOR },
    { 0x0394, "ZF Great Fairy Fountain" /* > ZF */, ENTRANCE_GROUP_ZORAS_DOMAIN, ENTRANCE_TYPE_INTERIOR, 1 },
    { 0x004F, "Lon Lon Ranch" /* > Talon's House */, ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_TYPE_INTERIOR },
    { 0x02F9, "Lon Lon Ranch" /* > LLR Stables */, ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_TYPE_INTERIOR },
    { 0x05D0, "Lon Lon Ranch" /* > LLR Tower */, ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_TYPE_INTERIOR },
    { 0x0378, "Talon's House" /* > Lon Lon Ranch */, ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_TYPE_INTERIOR, 1 },
    { 0x042F, "LLR Stables" /* > Lon Lon Ranch */, ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_TYPE_INTERIOR, 1 },
    { 0x05D4, "LLR Tower" /* > Lon Lon Ranch */, ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_TYPE_INTERIOR, 1 },
    { 0x0043, "Lake Hylia" /* > LH Lab */, ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_TYPE_INTERIOR },
    { 0x045F, "Lake Hylia" /* > Fishing Hole */, ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_TYPE_INTERIOR },
    { 0x03CC, "LH Lab" /* > Lake Hylia */, ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_TYPE_INTERIOR, 1 },
    { 0x0309, "Fishing Hole" /* > Lake Hylia */, ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_TYPE_INTERIOR, 1 },
    { 0x03A0, "GV" /* > Carpenters' Tent */, ENTRANCE_GROUP_GERUDO_VALLEY, ENTRANCE_TYPE_INTERIOR },
    { 0x03D0, "Carpenters' Tent" /* > GV */, ENTRANCE_GROUP_GERUDO_VALLEY, ENTRANCE_TYPE_INTERIOR, 1 },
    { 0x0588, "Colossus" /* > Colossus Great Fairy Fountain */, ENTRANCE_GROUP_HAUNTED_WASTELAND, ENTRANCE_TYPE_INTERIOR },
    { 0x057C, "Colossus Great Fairy Fountain" /* > Colossus */, ENTRANCE_GROUP_HAUNTED_WASTELAND, ENTRANCE_TYPE_INTERIOR, 1 },
    { 0x007E, "Market Entrance" /* > Guard House */, ENTRANCE_GROUP_MARKET, ENTRANCE_TYPE_INTERIOR },
    { 0x016D, "Market" /* > MK Shooting Gallery */, ENTRANCE_GROUP_MARKET, ENTRANCE_TYPE_INTERIOR },
    { 0x0507, "Market" /* > Bombchu Bowling */, ENTRANCE_GROUP_MARKET, ENTRANCE_TYPE_INTERIOR },
    { 0x0063, "Market" /* > Treasure Chest Game */, ENTRANCE_GROUP_MARKET, ENTRANCE_TYPE_INTERIOR },
    { 0x043B, "Market" /* > Man-in-Green's House */, ENTRANCE_GROUP_MARKET, ENTRANCE_TYPE_INTERIOR },
    { 0x0530, "Market" /* > Mask Shop */, ENTRANCE_GROUP_MARKET, ENTRANCE_TYPE_INTERIOR },
    { 0x052C, "Market" /* > MK Bazaar */, ENTRANCE_GROUP_MARKET, ENTRANCE_TYPE_INTERIOR },
    { 0x0388, "Market" /* > MK Potion Shop */, ENTRANCE_GROUP_MARKET, ENTRANCE_TYPE_INTERIOR },
    { 0x0528, "Market" /* > Bombchu Shop */, ENTRANCE_GROUP_MARKET, ENTRANCE_TYPE_INTERIOR },
    { 0x0053, "Outside Temple of Time" /* > Temple of Time */, ENTRANCE_GROUP_MARKET, ENTRANCE_TYPE_INTERIOR },
    { 0x026E, "Guard House" /* > Market Entrance */, ENTRANCE_GROUP_MARKET, ENTRANCE_TYPE_INTERIOR, 1 },
    { 0x01CD, "MK Shooting Gallery" /* > Market */, ENTRANCE_GROUP_MARKET, ENTRANCE_TYPE_INTERIOR, 1 },
    { 0x03BC, "Bombchu Bowling" /* > Market */, ENTRANCE_GROUP_MARKET, ENTRANCE_TYPE_INTERIOR, 1 },
    { 0x01D5, "Treasure Chest Game" /* > Market */, ENTRANCE_GROUP_MARKET, ENTRANCE_TYPE_INTERIOR, 1 },
    { 0x0067, "Man-in-Green's House" /* > Market */, ENTRANCE_GROUP_MARKET, ENTRANCE_TYPE_INTERIOR, 1 },
    { 0x01D1, "Mask Shop" /* > Market */, ENTRANCE_GROUP_MARKET, ENTRANCE_TYPE_INTERIOR, 1 },
    { 0x03B8, "MK Bazaar" /* > Market */, ENTRANCE_GROUP_MARKET, ENTRANCE_TYPE_INTERIOR, 1 },
    { 0x02A2, "MK Potion Shop" /* > Market */, ENTRANCE_GROUP_MARKET, ENTRANCE_TYPE_INTERIOR, 1 },
    { 0x03C0, "Bombchu Shop" /* > Market */, ENTRANCE_GROUP_MARKET, ENTRANCE_TYPE_INTERIOR, 1 },
    { 0x0472, "Temple of Time" /* > Outside Temple of Time */, ENTRANCE_GROUP_MARKET, ENTRANCE_TYPE_INTERIOR, 1 },
    { 0x0578, "HC Grounds" /* > HC Great Fairy Fountain */, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_INTERIOR },
    { 0x0340, "HC Great Fairy Fountain" /* > HC Grounds */, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_INTERIOR, 1 },
    { 0x04C2, "OGC" /* > OGC Great Fairy Fountain */, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_INTERIOR },
    { 0x03E8, "OGC Great Fairy Fountain" /* > OGC */, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_INTERIOR, 1 },

    { 0x071B, "KF" /* > KF Storms Grotto */, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_GROTTO },
    { 0x081B, "KF Storms Grotto" /* > KF */, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_GROTTO, 1 },
    { 0x071A, "Lost Woods" /* > LW Near Shortcuts Grotto */, ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_TYPE_GROTTO },
    { 0x0719, "Lost Woods" /* > LW Scrubs Grotto */, ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_TYPE_GROTTO },
    { 0x0720, "Lost Woods" /* > Deku Theater */, ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_TYPE_GROTTO },
    { 0x0716, "SFM" /* > SFM Wolfos Grotto */, ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_TYPE_GROTTO },
    { 0x0718, "SFM" /* > SFM Fairy Grotto */, ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_TYPE_GROTTO },
    { 0x0717, "SFM" /* > SFM Storms Grotto */, ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_TYPE_GROTTO },
    { 0x081A, "LW Near Shortcuts Grotto" /* > Lost Woods */, ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_TYPE_GROTTO, 1 },
    { 0x0819, "LW Scrubs Grotto" /* > Lost Woods */, ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_TYPE_GROTTO, 1 },
    { 0x0820, "Deku Theater" /* > Lost Woods */, ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_TYPE_GROTTO, 1 },
    { 0x0816, "SFM Wolfos Grotto" /* > SFM */, ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_TYPE_GROTTO, 1 },
    { 0x0818, "SFM Fairy Grotto" /* > SFM */, ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_TYPE_GROTTO, 1 },
    { 0x0817, "SFM Storms Grotto" /* > SFM */, ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_TYPE_GROTTO, 1 },
    { 0x070A, "Kakariko" /* > Kak Open Grotto */, ENTRANCE_GROUP_KAKARIKO, ENTRANCE_TYPE_GROTTO },
    { 0x070B, "Kakariko" /* > Kak Redead Grotto */, ENTRANCE_GROUP_KAKARIKO, ENTRANCE_TYPE_GROTTO },
    { 0x080A, "Kak Open Grotto" /* > Kakariko */, ENTRANCE_GROUP_KAKARIKO, ENTRANCE_TYPE_GROTTO, 1 },
    { 0x080B, "Kak Redead Grotto" /* > Kakariko */, ENTRANCE_GROUP_KAKARIKO, ENTRANCE_TYPE_GROTTO, 1 },
    { 0x004B, "Graveyard" /* > Shield Grave */, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_GROTTO },
    { 0x031C, "Graveyard" /* > Heart Piece Grave */, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_GROTTO },
    { 0x002D, "Graveyard" /* > Composer's Grave */, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_GROTTO },
    { 0x044F, "Graveyard" /* > Dampe's Grave */, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_GROTTO },
    { 0x035D, "Shield Grave" /* > Graveyard */, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_GROTTO, 1 },
    { 0x0361, "Heart Piece Grave" /* > Graveyard */, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_GROTTO, 1 },
    { 0x050B, "Composer's Grave" /* > Graveyard */, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_GROTTO, 1 },
    { 0x0359, "Dampe's Grave" /* > Graveyard */, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_GROTTO, 1 },
    { 0x0708, "DMT" /* > DMT Storms Grotto */, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_TYPE_GROTTO },
    { 0x0709, "DMT" /* > DMT Cow Grotto */, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_TYPE_GROTTO },
    { 0x0808, "DMT Storms Grotto" /* > DMT */, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_TYPE_GROTTO, 1 },
    { 0x0809, "DMT Cow Grotto" /* > DMT */, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_TYPE_GROTTO, 1 },
    { 0x0706, "DMC" /* > DMC Upper Grotto */, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_GROTTO },
    { 0x0705, "DMC" /* > DMC Hammer Grotto */, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_GROTTO },
    { 0x0806, "DMC Upper Grotto" /* > DMC */, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_GROTTO, 1 },
    { 0x0805, "DMC Hammer Grotto" /* > DMC */, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_GROTTO, 1 },
    { 0x0707, "Goron City" /* > Goron City Grotto */, ENTRANCE_GROUP_GORON_CITY, ENTRANCE_TYPE_GROTTO },
    { 0x0807, "Goron City Grotto" /* > Goron City */, ENTRANCE_GROUP_GORON_CITY, ENTRANCE_TYPE_GROTTO, 1 },
    { 0x0702, "ZR" /* > ZR Storms Grotto */, ENTRANCE_GROUP_ZORAS_DOMAIN, ENTRANCE_TYPE_GROTTO },
    { 0x0703, "ZR" /* > ZR Fairy Grotto */, ENTRANCE_GROUP_ZORAS_DOMAIN, ENTRANCE_TYPE_GROTTO },
    { 0x0704, "ZR" /* > ZR Open Grotto */, ENTRANCE_GROUP_ZORAS_DOMAIN, ENTRANCE_TYPE_GROTTO },
    { 0x0802, "ZR Storms Grotto" /* > ZR */, ENTRANCE_GROUP_ZORAS_DOMAIN, ENTRANCE_TYPE_GROTTO, 1 },
    { 0x0803, "ZR Fairy Grotto" /* > ZR */, ENTRANCE_GROUP_ZORAS_DOMAIN, ENTRANCE_TYPE_GROTTO, 1 },
    { 0x0804, "ZR Open Grotto" /* > ZR */, ENTRANCE_GROUP_ZORAS_DOMAIN, ENTRANCE_TYPE_GROTTO, 1 },
    { 0x071C, "Zora's Domain" /* > ZD Storms Grotto */, ENTRANCE_GROUP_ZORAS_DOMAIN, ENTRANCE_TYPE_GROTTO },
    { 0x081C, "ZD Storms Grotto" /* > Zora's Domain */, ENTRANCE_GROUP_ZORAS_DOMAIN, ENTRANCE_TYPE_GROTTO, 1 },
    { 0x0710, "Hyrule Field" /* > HF Near Market Grotto */, ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_TYPE_GROTTO },
    { 0x070E, "Hyrule Field" /* > HF Near Kak Grotto */, ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_TYPE_GROTTO },
    { 0x070D, "Hyrule Field" /* > HF Tektite Grotto */, ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_TYPE_GROTTO },
    { 0x070F, "Hyrule Field" /* > HF Fairy Grotto */, ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_TYPE_GROTTO },
    { 0x0711, "Hyrule Field" /* > HF Cow Grotto */, ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_TYPE_GROTTO },
    { 0x0713, "Hyrule Field" /* > HF Open Grotto */, ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_TYPE_GROTTO },
    { 0x0712, "Hyrule Field" /* > HF Inside Fence Grotto */, ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_TYPE_GROTTO },
    { 0x0714, "Hyrule Field" /* > HF Southeast Grotto */, ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_TYPE_GROTTO },
    { 0x0810, "HF Near Market Grotto" /* > Hyrule Field */, ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_TYPE_GROTTO, 1 },
    { 0x080E, "HF Near Kak Grotto" /* > Hyrule Field */, ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_TYPE_GROTTO, 1 },
    { 0x080D, "HF Tektite Grotto" /* > Hyrule Field */, ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_TYPE_GROTTO, 1 },
    { 0x080F, "HF Fairy Grotto" /* > Hyrule Field */, ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_TYPE_GROTTO, 1 },
    { 0x0811, "HF Cow Grotto" /* > Hyrule Field */, ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_TYPE_GROTTO, 1 },
    { 0x0813, "HF Open Grotto" /* > Hyrule Field */, ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_TYPE_GROTTO, 1 },
    { 0x0812, "HF Inside Fence Grotto" /* > Hyrule Field */, ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_TYPE_GROTTO, 1 },
    { 0x0814, "HF Southeast Grotto" /* > Hyrule Field */, ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_TYPE_GROTTO, 1 },
    { 0x0715, "Lon Lon Ranch" /* > LLR Grotto */, ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_TYPE_GROTTO },
    { 0x0815, "LLR Grotto" /* > Lon Lon Ranch */, ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_TYPE_GROTTO, 1 },
    { 0x0701, "Lake Hylia" /* > LH Grotto */, ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_TYPE_GROTTO },
    { 0x0801, "LH Grotto" /* > Lake Hylia */, ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_TYPE_GROTTO, 1 },
    { 0x071F, "GV" /* > GV Octorok Grotto */, ENTRANCE_GROUP_GERUDO_VALLEY, ENTRANCE_TYPE_GROTTO },
    { 0x071E, "GV" /* > GV Storms Grotto */, ENTRANCE_GROUP_GERUDO_VALLEY, ENTRANCE_TYPE_GROTTO },
    { 0x071D, "GF" /* > GF Storms Grotto */, ENTRANCE_GROUP_GERUDO_VALLEY, ENTRANCE_TYPE_GROTTO },
    { 0x081F, "GV Octorok Grotto" /* > GF */, ENTRANCE_GROUP_GERUDO_VALLEY, ENTRANCE_TYPE_GROTTO, 1 },
    { 0x081E, "GV Storms Grotto" /* > GV */, ENTRANCE_GROUP_GERUDO_VALLEY, ENTRANCE_TYPE_GROTTO, 1 },
    { 0x081D, "GF Storms Grotto" /* > GF */, ENTRANCE_GROUP_GERUDO_VALLEY, ENTRANCE_TYPE_GROTTO, 1 },
    { 0x0700, "Desert Colossus" /* > Colossus Grotto */, ENTRANCE_GROUP_HAUNTED_WASTELAND, ENTRANCE_TYPE_GROTTO },
    { 0x0800, "Colossus Grotto" /* > Desert Colossus */, ENTRANCE_GROUP_HAUNTED_WASTELAND, ENTRANCE_TYPE_GROTTO, 1 },
    { 0x070C, "HC Grounds" /* > HC Storms Grotto */, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_GROTTO },
    { 0x080C, "HC Storms Grotto" /* > HC Grounds */, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_GROTTO, 1 },

    { 0x05E0, "KF" /* > Lost Woods Bridge */, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_OVERWORLD },
    { 0x011E, "KF" /* > Lost Woods */, ENTRANCE_GROUP_KOKIRI_FOREST, ENTRANCE_TYPE_OVERWORLD },
    { 0x020D, "Lost Woods Bridge" /* > KF */, ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_TYPE_OVERWORLD },
    { 0x0185, "Lost Woods Bridge" /* > Hyrule Field */, ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_TYPE_OVERWORLD },
    { 0x0286, "Lost Woods" /* > KF */, ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_TYPE_OVERWORLD },
    { 0x04E2, "Lost Woods" /* > Goron City */, ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_TYPE_OVERWORLD },
    { 0x01DD, "Lost Woods" /* > ZR */, ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_TYPE_OVERWORLD },
    { 0x00FC, "Lost Woods" /* > SFM */, ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_TYPE_OVERWORLD },
    { 0x01A9, "SFM" /* > Lost Woods */, ENTRANCE_GROUP_LOST_WOODS, ENTRANCE_TYPE_OVERWORLD },
    { 0x017D, "Kakariko" /* > Hyrule Field */, ENTRANCE_GROUP_KAKARIKO, ENTRANCE_TYPE_OVERWORLD },
    { 0x00E4, "Kakariko" /* > Graveyard */, ENTRANCE_GROUP_KAKARIKO, ENTRANCE_TYPE_OVERWORLD },
    { 0x013D, "Kakariko" /* > DMT */, ENTRANCE_GROUP_KAKARIKO, ENTRANCE_TYPE_OVERWORLD },
    { 0x0195, "Graveyard" /* > Kakariko */, ENTRANCE_GROUP_GRAVEYARD, ENTRANCE_TYPE_OVERWORLD },
    { 0x0191, "DMT" /* > Kakariko */, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_TYPE_OVERWORLD },
    { 0x014D, "DMT" /* > Goron City */, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_TYPE_OVERWORLD },
    { 0x0147, "DMT" /* > DMC */, ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL, ENTRANCE_TYPE_OVERWORLD },
    { 0x01C1, "DMC" /* > Goron City */, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_OVERWORLD },
    { 0x01BD, "DMC" /* > DMT */, ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER, ENTRANCE_TYPE_OVERWORLD },
    { 0x01B9, "Goron City" /* > DMT */, ENTRANCE_GROUP_GORON_CITY, ENTRANCE_TYPE_OVERWORLD },
    { 0x0246, "Goron City" /* > DMC */, ENTRANCE_GROUP_GORON_CITY, ENTRANCE_TYPE_OVERWORLD },
    { 0x04D6, "Goron City" /* > Lost Woods */, ENTRANCE_GROUP_GORON_CITY, ENTRANCE_TYPE_OVERWORLD },
    { 0x0181, "ZR" /* > Hyrule Field */, ENTRANCE_GROUP_ZORAS_DOMAIN, ENTRANCE_TYPE_OVERWORLD },
    { 0x04DA, "ZR" /* > Lost Woods */, ENTRANCE_GROUP_ZORAS_DOMAIN, ENTRANCE_TYPE_OVERWORLD },
    { 0x0108, "ZR" /* > Zora's Domain */, ENTRANCE_GROUP_ZORAS_DOMAIN, ENTRANCE_TYPE_OVERWORLD },
    { 0x019D, "Zora's Domain" /* > ZR */, ENTRANCE_GROUP_ZORAS_DOMAIN, ENTRANCE_TYPE_OVERWORLD },
    { 0x0560, "Zora's Domain" /* > Lake Hylia */, ENTRANCE_GROUP_ZORAS_DOMAIN, ENTRANCE_TYPE_OVERWORLD },
    { 0x0225, "Zora's Domain" /* > ZF */, ENTRANCE_GROUP_ZORAS_DOMAIN, ENTRANCE_TYPE_OVERWORLD },
    { 0x01A1, "ZF" /* > Zora's Domain */, ENTRANCE_GROUP_ZORAS_DOMAIN, ENTRANCE_TYPE_OVERWORLD },
    { 0x04DE, "Hyrule Field" /* > Lost Woods Bridge */, ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_TYPE_OVERWORLD },
    { 0x0276, "Hyrule Field" /* > Market Entrance */, ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_TYPE_OVERWORLD },
    { 0x0157, "Hyrule Field" /* > Lon Lon Ranch */, ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_TYPE_OVERWORLD },
    { 0x00DB, "Hyrule Field" /* > Kakariko */, ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_TYPE_OVERWORLD },
    { 0x00EA, "Hyrule Field" /* > ZR */, ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_TYPE_OVERWORLD },
    { 0x0102, "Hyrule Field" /* > Lake Hylia */, ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_TYPE_OVERWORLD },
    { 0x0117, "Hyrule Field" /* > GV */, ENTRANCE_GROUP_HYRULE_FIELD, ENTRANCE_TYPE_OVERWORLD },
    { 0x01F9, "Lon Lon Ranch" /* > Hyrule Field */, ENTRANCE_GROUP_LON_LON_RANCH, ENTRANCE_TYPE_OVERWORLD },
    { 0x0189, "Lake Hylia" /* > Hyrule Field */, ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_TYPE_OVERWORLD },
    { 0x0328, "Lake Hylia" /* > Zora's Domain */, ENTRANCE_GROUP_LAKE_HYLIA, ENTRANCE_TYPE_OVERWORLD },
    { 0x018D, "GV" /* > Hyrule Field */, ENTRANCE_GROUP_GERUDO_VALLEY, ENTRANCE_TYPE_OVERWORLD },
    { 0x0129, "GV" /* > GF */, ENTRANCE_GROUP_GERUDO_VALLEY, ENTRANCE_TYPE_OVERWORLD },
    { 0x022D, "GF" /* > GV */, ENTRANCE_GROUP_GERUDO_VALLEY, ENTRANCE_TYPE_OVERWORLD },
    { 0x0130, "GF" /* > Haunted Wasteland */, ENTRANCE_GROUP_GERUDO_VALLEY, ENTRANCE_TYPE_OVERWORLD },
    { 0x03AC, "Haunted Wasteland" /* > GF */, ENTRANCE_GROUP_HAUNTED_WASTELAND, ENTRANCE_TYPE_OVERWORLD },
    { 0x0123, "Haunted Wasteland" /* > Desert Colossus */, ENTRANCE_GROUP_HAUNTED_WASTELAND, ENTRANCE_TYPE_OVERWORLD },
    { 0x0365, "Desert Colossus" /* > Haunted Wasteland */, ENTRANCE_GROUP_HAUNTED_WASTELAND, ENTRANCE_TYPE_OVERWORLD },
    { 0x01FD, "Market Entrance" /* > Hyrule Field */, ENTRANCE_GROUP_MARKET, ENTRANCE_TYPE_OVERWORLD },
    { 0x00B1, "Market Entrance" /* > Market */, ENTRANCE_GROUP_MARKET, ENTRANCE_TYPE_OVERWORLD },
    { 0x0033, "Market" /* > Market Entrance */, ENTRANCE_GROUP_MARKET, ENTRANCE_TYPE_OVERWORLD },
    { 0x0138, "Market" /* > HC Grounds / OGC */, ENTRANCE_GROUP_MARKET, ENTRANCE_TYPE_OVERWORLD },
    { 0x0171, "Market" /* > Outside Temple of Time */, ENTRANCE_GROUP_MARKET, ENTRANCE_TYPE_OVERWORLD },
    { 0x025E, "Outside Temple of Time" /* > Market */, ENTRANCE_GROUP_MARKET, ENTRANCE_TYPE_OVERWORLD },
    { 0x025A, "HC Grounds / OGC" /* > Market */, ENTRANCE_GROUP_HYRULE_CASTLE, ENTRANCE_TYPE_OVERWORLD },
};

const EntranceData* GetEntranceData(s16 index) {
    for (size_t i = 0; i < ARRAY_SIZE(entranceData); i++) {
        if (index == entranceData[i].index) {
            return &entranceData[i];
        }
    }
    // Shouldn't be reached
    return 0;
}

s16 Entrance_GetReplacementIndex(s16 index) {
    for (size_t i = 0; i < ENTRANCE_OVERRIDES_MAX_COUNT; i++) {
        if (Entrance_EntranceIsNull(&gSaveContext.entranceOverrides[i])) {
            break;
        }
        if (index == gSaveContext.entranceOverrides[i].override) {
            return gSaveContext.entranceOverrides[i].index;
        }
    }
    // Entrance wasn't replaced
    return index;
}

void SortEntranceList(EntranceOverride* entranceList, u8 byDest) {
    EntranceOverride tempList[ENTRANCE_OVERRIDES_MAX_COUNT] = { 0 };

    // Store to temp
    for (size_t i = 0; i < ENTRANCE_OVERRIDES_MAX_COUNT; i++) {
        tempList[i] = entranceList[i];
    }

    // Sort
    int idx = 0;
    for (size_t i = 1; i < SPOILER_ENTRANCE_GROUP_COUNT; i++) {
        for (size_t j = 0; j < ENTRANCE_TYPE_COUNT; j++) {
            for (size_t k = 0; k < ARRAY_SIZE(entranceData); k++) {
                if (entranceData[k].group != i || entranceData[k].type != j) {
                    continue;
                }
                for (size_t l = 0; l < ENTRANCE_OVERRIDES_MAX_COUNT; l++) {
                    if (Entrance_EntranceIsNull(&tempList[l])) {
                        break;
                    }
                    if ((byDest ? tempList[l].overrideDestination : tempList[l].index) == entranceData[k].index) {
                        entranceList[idx] = tempList[l];
                        idx++;
                        break;
                    }
                }
            }
        }
    }
}

void InitEntranceTrackingData(void) {
    // Check if entrance randomization is disabled
    if (Entrance_EntranceIsNull(&gSaveContext.entranceOverrides[0])) {
        return;
    }

    // Set total and group counts
    for (size_t i = 0; i < ENTRANCE_OVERRIDES_MAX_COUNT; i++) {
        if (Entrance_EntranceIsNull(&gSaveContext.entranceOverrides[i])) {
            break;
        }
        gEntranceTrackingData.GroupEntranceCounts[GetEntranceData(gSaveContext.entranceOverrides[i].index)->group]++;
        gEntranceTrackingData.EntranceCount++;
    }

    // Set offset
    u16 offsetTotal = 0;
    for (size_t i = 1; i < SPOILER_ENTRANCE_GROUP_COUNT; i++) {
        offsetTotal += gEntranceTrackingData.GroupEntranceCounts[i - 1];
        gEntranceTrackingData.GroupOffsets[i] = offsetTotal;
    }

    // Fill destList which will have the entrances reordered based on the destination instead of the source
    for (size_t i = 0; i < ENTRANCE_OVERRIDES_MAX_COUNT; i++) {
        destList[i] = gSaveContext.entranceOverrides[i];
    }

    // Sort entrances by entrance type
    // SortEntranceList(gSaveContext.entranceOverrides, 0);
    // SortEntranceList(destList, 1);
}
