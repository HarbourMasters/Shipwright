/*
 * File: z_select.c
 * Overlay: ovl_select
 * Description: Debug Scene Select Menu
 */

#include "ultra64.h"
#include "global.h"
#include "vt.h"
#include "alloca.h"

void Select_LoadTitle(SelectContext* this) {
    this->state.running = false;
    SET_NEXT_GAMESTATE(&this->state, Title_Init, TitleContext);
}

void Select_LoadGame(SelectContext* this, s32 entranceIndex) {
    osSyncPrintf(VT_FGCOL(BLUE));
    osSyncPrintf("\n\n\nＦＩＬＥ＿ＮＯ＝%x\n\n\n", gSaveContext.fileNum);
    osSyncPrintf(VT_RST);
    if (gSaveContext.fileNum == 0xFF) {
        Sram_InitDebugSave();
        gSaveContext.unk_13F6 = gSaveContext.magic;
        gSaveContext.magic = 0;
        gSaveContext.unk_13F4 = 0;
        gSaveContext.magicLevel = gSaveContext.magic;
    }
    for (int buttonIndex = 0; buttonIndex < ARRAY_COUNT(gSaveContext.buttonStatus); buttonIndex++) {
        gSaveContext.buttonStatus[buttonIndex] = BTN_ENABLED;
    }
    gSaveContext.unk_13E7 = gSaveContext.unk_13E8 = gSaveContext.unk_13EA = gSaveContext.unk_13EC = 0;
    Audio_QueueSeqCmd(SEQ_PLAYER_BGM_MAIN << 24 | NA_BGM_STOP);
    gSaveContext.entranceIndex = entranceIndex;

    if (CVar_GetS32("gBetterDebugWarpScreen", 0)) {
        CVar_SetS32("gBetterDebugWarpScreenCurrentScene", this->currentScene);
        CVar_SetS32("gBetterDebugWarpScreenTopDisplayedScene", this->topDisplayedScene);
        CVar_SetS32("gBetterDebugWarpScreenPageDownIndex", this->pageDownIndex);
        CVar_Save();
    }

    gSaveContext.respawnFlag = 0;
    gSaveContext.respawn[RESPAWN_MODE_DOWN].entranceIndex = -1;
    gSaveContext.seqId = (u8)NA_BGM_DISABLED;
    gSaveContext.natureAmbienceId = 0xFF;
    gSaveContext.showTitleCard = true;
    gWeatherMode = 0;
    this->state.running = false;
    SET_NEXT_GAMESTATE(&this->state, Gameplay_Init, GlobalContext);
}

void Select_Grotto_LoadGame(SelectContext* this, s32 grottoIndex) {
    osSyncPrintf(VT_FGCOL(BLUE));
    osSyncPrintf("\n\n\nＦＩＬＥ＿ＮＯ＝%x\n\n\n", gSaveContext.fileNum);
    osSyncPrintf(VT_RST);
    if (gSaveContext.fileNum == 0xFF) {
        Sram_InitDebugSave();
        gSaveContext.unk_13F6 = gSaveContext.magic;
        gSaveContext.magic = 0;
        gSaveContext.unk_13F4 = 0;
        gSaveContext.magicLevel = gSaveContext.magic;
    }
    for (int buttonIndex = 0; buttonIndex < ARRAY_COUNT(gSaveContext.buttonStatus); buttonIndex++) {
        gSaveContext.buttonStatus[buttonIndex] = BTN_ENABLED;
    }
    gSaveContext.unk_13E7 = gSaveContext.unk_13E8 = gSaveContext.unk_13EA = gSaveContext.unk_13EC = 0;
    Audio_QueueSeqCmd(SEQ_PLAYER_BGM_MAIN << 24 | NA_BGM_STOP);
    // Entrance index and grotto content data to load the correct grotto and actors
    gSaveContext.entranceIndex = this->betterGrottos[grottoIndex].entranceIndex;
    gSaveContext.respawn[RESPAWN_MODE_RETURN].data = this->betterGrottos[grottoIndex].data;
    // Infomation to return to the correct grotto exit
    gSaveContext.respawn[RESPAWN_MODE_RETURN].entranceIndex = this->betterGrottos[grottoIndex].returnEntranceIndex;
    gSaveContext.respawn[RESPAWN_MODE_RETURN].roomIndex = this->betterGrottos[grottoIndex].roomIndex;
    gSaveContext.respawn[RESPAWN_MODE_RETURN].playerParams = 0x4ff;
    gSaveContext.respawn[RESPAWN_MODE_RETURN].pos = this->betterGrottos[grottoIndex].pos;

    if (CVar_GetS32("gBetterDebugWarpScreen", 0)) {
        CVar_SetS32("gBetterDebugWarpScreenCurrentScene", this->currentScene);
        CVar_SetS32("gBetterDebugWarpScreenTopDisplayedScene", this->topDisplayedScene);
        CVar_SetS32("gBetterDebugWarpScreenPageDownIndex", this->pageDownIndex);
        CVar_Save();
    }

    gSaveContext.respawnFlag = 0;
    gSaveContext.respawn[RESPAWN_MODE_DOWN].entranceIndex = -1;
    gSaveContext.seqId = (u8)NA_BGM_DISABLED;
    gSaveContext.natureAmbienceId = 0xFF;
    gSaveContext.showTitleCard = true;
    gWeatherMode = 0;
    this->state.running = false;
    SET_NEXT_GAMESTATE(&this->state, Gameplay_Init, GlobalContext);
}

static SceneSelectEntry sScenes[] = {
    { " 1:Hyrule Field", Select_LoadGame, 0x00CD },
    { " 2:Kakariko Village", Select_LoadGame, 0x00DB },
    { " 3:Graveyard", Select_LoadGame, 0x00E4 },
    { " 4:Zora's River", Select_LoadGame, 0x00EA },
    { " 5:Kokiri Forest", Select_LoadGame, 0x00EE },
    { " 6:Sacred Forest Meadow", Select_LoadGame, 0x00FC },
    { " 7:Lake Hylia", Select_LoadGame, 0x0102 },
    { " 8:Zora's Domain", Select_LoadGame, 0x0108 },
    { " 9:Zora's Fountain", Select_LoadGame, 0x010E },
    { "10:Gerudo Valley", Select_LoadGame, 0x0117 },
    { "11:Lost Woods", Select_LoadGame, 0x011E },
    { "12:Desert Colossus", Select_LoadGame, 0x0123 },
    { "13:Gerudo's Fortress", Select_LoadGame, 0x0129 },
    { "14:Haunted Wasteland", Select_LoadGame, 0x0130 },
    { "15:Hyrule Castle", Select_LoadGame, 0x0138 },
    { "16:Death Mountain Trail", Select_LoadGame, 0x013D },
    { "17:Death Mountain Crater", Select_LoadGame, 0x0147 },
    { "18:Goron City", Select_LoadGame, 0x014D },
    { "19:Lon Lon Ranch", Select_LoadGame, 0x0157 },
    { "20:Temple Of Time", Select_LoadGame, 0x0053 },
    { "21:Chamber of Sages", Select_LoadGame, 0x006B },
    { "22:Shooting Gallery", Select_LoadGame, 0x003B },
    { "23:Castle Courtyard Game", Select_LoadGame, 0x007A },
    { "24:Grave 1", Select_LoadGame, 0x031C },
    { "25:Grave 2", Select_LoadGame, 0x004B },
    { "26:Royal Family's Tomb", Select_LoadGame, 0x002D },
    { "27:Great Fairy's Fountain (Upgrades)", Select_LoadGame, 0x0315 },
    { "28:Fairy's Fountain (Grotto)", Select_LoadGame, 0x036D },
    { "29:Great Fairy's Fountain (Magic)", Select_LoadGame, 0x0371 },
    { "30:Ganon's Tower - Collapsing", Select_LoadGame, 0x043F },
    { "31:Castle Courtyard", Select_LoadGame, 0x0400 },
    { "32:Fishing Pond", Select_LoadGame, 0x045F },
    { "33:Bombchu Bowling Alley", Select_LoadGame, 0x0507 },
    { "34:Lon Lon Ranch House", Select_LoadGame, 0x004F },
    { "35:Lon Lon Ranch Silo", Select_LoadGame, 0x05D0 },
    { "36:Guard House", Select_LoadGame, 0x007E },
    { "37:Potion Shop", Select_LoadGame, 0x0072 },
    { "38:Treasure Chest Game", Select_LoadGame, 0x0063 },
    { "39:House Of Skulltula", Select_LoadGame, 0x0550 },
    { "40:Entrance to Market", Select_LoadGame, 0x0033 },
    { "41:Market", Select_LoadGame, 0x00B1 },
    { "42:Back Alley", Select_LoadGame, 0x00AD },
    { "43:Temple of Time Exterior", Select_LoadGame, 0x0171 },
    { "44:Link's House", Select_LoadGame, 0x00BB },
    { "45:Kakariko House 1", Select_LoadGame, 0x02FD },
    { "46:Back Alley House 1", Select_LoadGame, 0x043B },
    { "47:House of the Know-it-All Brothers", Select_LoadGame, 0x00C9 },
    { "48:House of Twins", Select_LoadGame, 0x009C },
    { "49:Mido's House", Select_LoadGame, 0x0433 },
    { "50:Saria's House", Select_LoadGame, 0x0437 },
    { "51:Stable", Select_LoadGame, 0x02F9 },
    { "52:Grave Keeper's Hut", Select_LoadGame, 0x030D },
    { "53:Dog Lady's House", Select_LoadGame, 0x0398 },
    { "54:Impa's House", Select_LoadGame, 0x039C },
    { "55:Lakeside Laboratory", Select_LoadGame, 0x0043 },
    { "56:Running Man's Tent", Select_LoadGame, 0x03A0 },
    { "57:Bazaar", Select_LoadGame, 0x00B7 },
    { "58:Kokiri Shop", Select_LoadGame, 0x00C1 },
    { "59:Goron Shop", Select_LoadGame, 0x037C },
    { "60:Zora Shop", Select_LoadGame, 0x0380 },
    { "61:Closed Shop", Select_LoadGame, 0x0384 },
    { "62:Potion Shop", Select_LoadGame, 0x0388 },
    { "63:Bombchu Shop ", Select_LoadGame, 0x0390 },
    { "64:Happy Mask Shop", Select_LoadGame, 0x0530 },
    { "65:Gerudo Training Ground", Select_LoadGame, 0x0008 },
    { "66:Inside the Deku Tree", Select_LoadGame, 0x0000 },
    { "67:Gohma's Lair", Select_LoadGame, 0x040F },
    { "68:Dodongo's Cavern", Select_LoadGame, 0x0004 },
    { "69:King Dodongo's Lair", Select_LoadGame, 0x040B },
    { "70:Inside Jabu-Jabu's Belly", Select_LoadGame, 0x0028 },
    { "71:Barinade's Lair", Select_LoadGame, 0x0301 },
    { "72:Forest Temple", Select_LoadGame, 0x0169 },
    { "73:Phantom Ganon's Lair", Select_LoadGame, 0x000C },
    { "74:Bottom of the Well", Select_LoadGame, 0x0098 },
    { "75:Shadow Temple", Select_LoadGame, 0x0037 },
    { "76:Bongo Bongo's Lair", Select_LoadGame, 0x0413 },
    { "77:Fire Temple", Select_LoadGame, 0x0165 },
    { "78:Volvagia's Lair", Select_LoadGame, 0x0305 },
    { "79:Water Temple", Select_LoadGame, 0x0010 },
    { "80:Morpha's Lair", Select_LoadGame, 0x0417 },
    { "81:Spirit Temple", Select_LoadGame, 0x0082 },
    { "82:Iron Knuckle's Lair", Select_LoadGame, 0x008D },
    { "83:Twinrova's Lair", Select_LoadGame, 0x05EC },
    { "84:Stairs to Ganondorf's Lair", Select_LoadGame, 0x041B },
    { "85:Ganondorf's Lair", Select_LoadGame, 0x041F },
    { "86:Ice Cavern", Select_LoadGame, 0x0088 },
    { "87:Dampe Grave Relay Game", Select_LoadGame, 0x044F },
    { "88:Inside Ganon's Castle", Select_LoadGame, 0x0467 },
    { "89:Ganon's Lair", Select_LoadGame, 0x0517 },
    { "90:Escaping Ganon's Castle 1", Select_LoadGame, 0x0179 },
    { "91:Escaping Ganon's Castle 2", Select_LoadGame, 0x01B5 },
    { "92:Escaping Ganon's Castle 3", Select_LoadGame, 0x03DC },
    { "93:Escaping Ganon's Castle 4", Select_LoadGame, 0x03E4 },
    { "94:Escaping Ganon's Castle 5", Select_LoadGame, 0x056C },
    { "95:Thieves' Hideout 1-2", Select_LoadGame, 0x0486 },
    { "96:Thieves' Hideout 3-4 9-10", Select_LoadGame, 0x048E },
    { "97:Thieves' Hideout 5-6", Select_LoadGame, 0x0496 },
    { "98:Thieves' Hideout 7-8", Select_LoadGame, 0x049E },
    { "99:Thieves' Hideout 11-12", Select_LoadGame, 0x04AE },
    { "100:Thieves' Hideout 13", Select_LoadGame, 0x0570 },
    { "101:Grotto 0", Select_LoadGame, 0x003F },
    { "102:Grotto 1", Select_LoadGame, 0x0598 },
    { "103:Grotto 2", Select_LoadGame, 0x059C },
    { "104:Grotto 3", Select_LoadGame, 0x05A0 },
    { "105:Grotto 4", Select_LoadGame, 0x05A4 },
    { "106:Grotto 5", Select_LoadGame, 0x05A8 },
    { "107:Grotto 6", Select_LoadGame, 0x05AC },
    { "108:Grotto 7", Select_LoadGame, 0x05B0 },
    { "109:Grotto 8", Select_LoadGame, 0x05B4 },
    { "110:Grotto 9", Select_LoadGame, 0x05B8 },
    { "111:Grotto 10", Select_LoadGame, 0x05BC },
    { "112:Grotto 11", Select_LoadGame, 0x05C0 },
    { "113:Grotto 12", Select_LoadGame, 0x05C4 },
    { "114:Grotto 13", Select_LoadGame, 0x05FC },
    { "115:Goddess Cutscene Environment", Select_LoadGame, 0x00A0 },
    { "116:Test Room", Select_LoadGame, 0x0520 },
    { "117:SRD Map", Select_LoadGame, 0x0018 },
    { "118:Test Map", Select_LoadGame, 0x0094 },
    { "119:Treasure Chest Warp", Select_LoadGame, 0x0024 },
    { "120:Stalfos Miniboss Room", Select_LoadGame, 0x001C },
    { "121:Stalfos Boss Room", Select_LoadGame, 0x0020 },
    { "122:Dark Link Room", Select_LoadGame, 0x0047 },
    { "123:Shooting Gallery Duplicate", Select_LoadGame, 0x02EA },
    { "124:depth test", Select_LoadGame, 0x00B6 },
    { "125:Hyrule Garden Game (Broken)", Select_LoadGame, 0x0076 },
    { "title", Select_LoadTitle, 0x0000 },
};

static BetterSceneSelectEntry sBetterScenes[] = {
    { " 1:Hyrule Field", Select_LoadGame, 8, { 
        { "Near Drawbridge", 0x00CD },
        { "From Drawbridge", 0x01FD },
        { "From Kakariko Village", 0x017D },
        { "From Zora River", 0x0181 },
        { "From Lost Woods", 0x0185 },
        { "From Lake Hylia", 0x0189 },
        { "From Gerudo Valley", 0x018D },
        { "From Lon Lon Ranch", 0x01F9 },
    }},
    { " 2:Kokiri Forest", Select_LoadGame, 9, {
        { "From Links House", 0x0211 },
        { "From Bridge", 0x020D },
        { "From Lost Woods", 0x0286 },
        { "From Deku Tree", 0x0209 },
        { "From Kokiri Shop", 0x0266 },
        { "From Know-It-All Brothers House", 0x026A },
        { "From Twins House", 0x033C },
        { "From Midos House", 0x0443 },
        { "From Sarias House", 0x0447 },
    }},
    { " 3:Kokiri Buildings", Select_LoadGame, 6, {
        { "Links Bed", 0x00BB },
        { "Kokiri Shop", 0x00C1 },
        { "Twins House", 0x009C },
        { "Know-It-All Brothers House", 0x00C9 },
        { "Midos House", 0x0433 },
        { "Sarias House", 0x0437 },
    }},
    { " 4:Lost Woods", Select_LoadGame, 5, { 
        { "From Kokiri Forest", 0x011E },
        { "From Sacred Meadow", 0x01A9 },
        { "From Goron City", 0x04D6 },
        { "From Zora River", 0x04DA },
        { "Bridge", 0x05E0 },
    }},
    { " 5:Sacred Forest Meadow", Select_LoadGame, 3, { 
        { "From Lost Woods", 0x00FC },
        { "From Forest Temple", 0x0215 },
        { "Minuet of Forest Warp", 0x0600 },
    }},
    { " 6:Castle Town Entrance", Select_LoadGame, 3, {
        { "From Hyrule Field", 0x0276 },
        { "From Market", 0x0033 },
        { "From Pot House", 0x026E },
    }},
    { " 7:Market", Select_LoadGame, 11, {
        { "From Castle Town Entrance", 0x00B1 },
        { "From Shooting Gallery", 0x01CD },
        { "From Happy Mask Shop", 0x01D1 },
        { "From Treasure Box Minigame", 0x01D5 },
        { "From Castle", 0x025A },
        { "From Temple of Time", 0x025E },
        { "From Back Alley (Right)", 0x0262 },
        { "From Back Alley (Left)", 0x029E },
        { "From Potion Shop", 0x02A2 },
        { "From Bazaar Shop", 0x03B8 },
        { "From Bomchu Bowling Minigame", 0x03BC },
    }},
    { " 8:Castle Town Alley", Select_LoadGame, 5, { 
        { "From Market (Right)", 0x00AD },
        { "From Market (Left)", 0x029A }, 
        { "From Alley House", 0x0067 },
        { "From Dog House", 0x038C }, 
        { "From Bombchu Shop", 0x03C0 },
    }},
    { " 9:Castle Town Buildings", Select_LoadGame, 10, { 
        { "Pot House", 0x007E },
        { "Shooting Gallery Minigame", 0x016D },
        { "Treasure Box Minigame", 0x0063 },
        { "Potion Shop", 0x0388 },
        { "Bombchu Bowling Minigame", 0x0507 },
        { "Bazaar Shop", 0x052C },
        { "Happy Mask Shop", 0x0530 },
        { "Bombchu Shop", 0x0528 },
        { "Dog House", 0x0398 },
        { "Alley House", 0x043B },
    }},
    { "10:Temple of Time", Select_LoadGame, 5, { 
        { "From Outside", 0x053 },
        { "From Master Sword Pedestal", 0x02CA },
        { "Prelude of Light Warp", 0x05F4 },
        { "Outside Temple of Time - From Market", 0x0171 },
        { "Outside Temple of Time - From Temple of Time", 0x0472 },
    }},
    { "11:Hyrule Castle", Select_LoadGame, 5, {
        { "From Market", 0x0138 },
        { "From Castle Courtyard", 0x023D },
        { "From Great Fairy", 0x0340 },
        { "From Courtyard Guard Capture", 0x04FA },
        { "Great Fairy", 0x04C2 },
    }},
    { "12:Hyrule Castle Courtyard", Select_LoadGame, 3, {
        { "From Crawlspace", 0x007A },
        { "From Zelda", 0x0296 },
        { "Zeldas Courtyard", 0x0400 },
    }},
    { "13:Lon Lon Ranch", Select_LoadGame, 4, { 
        { "From Hyrule Field", 0x0157 },
        { "From Ranch House", 0x0378 },
        { "From Stables", 0x042F },
        { "Epona Song Cutscene", 0x02AE },
    }},
    { "14:Lon Lon Ranch Buildings", Select_LoadGame, 3, {
        { "Ranch House", 0x004F },
        { "Stables", 0x02F9 },
        { "Back Tower", 0x05D0 },
    }},
    { "15:Kakariko Village", Select_LoadGame, 15, {
        { "From Hyrule Field", 0x00DB },
        { "From Death Mountain", 0x0191 },
        { "From Graveyard", 0x0195 },
        { "From Bazaar", 0x0201 },
        { "From Bottom of Well", 0x02A6 },
        { "From Boss House", 0x0349 },
        { "From Potion Shop", 0x044B },
        { "From Potion Shop (Back Entrance)", 0x04FF },
        { "From Grannys Potion Shop", 0x034D },
        { "From Impas House", 0x0345 },
        { "From Impas House (Cow)", 0x05DC },
        { "From Windmill", 0x0351 },
        { "From Shooting Gallery", 0x0463 },
        { "From Skulltula House", 0x04EE },
        { "Owl Drop Spot from Death Mountain", 0x0554 },
    }},
    { "16:Kakariko Buildings", Select_LoadGame, 9, {
        { "Shooting Gallery Minigame", 0x003B },
        { "Grannys Potion Shop", 0x0072 },
        { "Bazaar Shop", 0x00B7 },
        { "Potion Shop", 0x0384 },
        { "Impas House", 0x039C },
        { "Impas House (Near Cow)", 0x05C8 },
        { "Boss House", 0x02FD },
        { "Windmill", 0x0453 },
        { "Skulltula House", 0x0550 },
    }}, 
    { "17:Graveyard", Select_LoadGame, 9, {
        { "From Kakariko", 0x00E4 },
        { "From Shadow Temple", 0x0205 },
        { "From Gravekeepers Hut", 0x0355 },
        { "From Dampes Grave", 0x0359 },
        { "From Shield Grave", 0x035D },
        { "From Redead Grave", 0x0361 },
        { "From Royal Familys Tomb", 0x050B },
        { "Inside Dampe's Hut", 0x030D },
        { "Nocturne of Shadow Warp", 0x0568 },
    }},
    { "18:Graves", Select_LoadGame, 5, {
        { "Dampes Grave Minigame", 0x044F },
        { "Royal Familys Tomb", 0x002D },
        { "Royal Familys Tomb, Suns Song Cutscene", 0x0574 },
        { "Treasure Chest Grave", 0x004B },
        { "ReDead Grave", 0x031C },
    }},
    { "19:Death Mountain Trail", Select_LoadGame, 6, {
        { "From Kakariko Village", 0x013D },
        { "From Goron City", 0x01B9 },
        { "From Death Mountain Crater", 0x01BD },
        { "From Dodongos Cavern", 0x0242 },
        { "From Great Fairy", 0x045B },
        { "Great Fairy", 0x0315 },
    }},
    { "20:Goron City", Select_LoadGame, 5, {
        { "From Death Mountain Trail", 0x014D },
        { "From Death Mountain Crater", 0x01C1 },
        { "From Goron City Shop", 0x03FC },
        { "From Lost Woods", 0x04E2 },
        { "Goron City Shop", 0x037C },
    }},
    { "21:Death Mountain Crater", Select_LoadGame, 6, {
        { "From Death Mountain Trail", 0x0147 },
        { "From Goron City", 0x0246 },
        { "From Fire Temple", 0x024A },
        { "From Fairy Fountain", 0x0482 },
        { "Great Fairy", 0x04BE },
        { "Bolero of Fire Warp", 0x04F6 },
    }},
    { "22:Zora River", Select_LoadGame, 3, {
        { "From Hyrule Field", 0x00EA },
        { "From Zoras Domain", 0x019D },
        { "From Lost Woods", 0x01DD },
    }},
    { "23:Zoras Domain", Select_LoadGame, 5, {
        { "From Zora River", 0x0108 },
        { "From Zoras Fountain", 0x01A1 },
        { "From Lake Hylia", 0x0328 },
        { "From Zora Shop", 0x03C4 },
        { "Zora Shop", 0x0380 },
    }},
    { "24:Zoras Fountain", Select_LoadGame, 5, {
        { "From Zoras Domain", 0x0225 },
        { "From Jabu Jabu", 0x0221 },
        { "From Ice Cavern", 0x03D4 },
        { "From Fairy Fountain", 0x0394 },
        { "Great Fairy", 0x0371 },
    }},
    { "25:Lake Hylia", Select_LoadGame, 7, {
        { "From Hyrule Field", 0x0102 },
        { "From Gerudo Valley", 0x0219 },
        { "From Water Temple", 0x021D },
        { "From Fishing Pond", 0x0309 },
        { "From Laboratory", 0x03CC },
        { "From Zoras Domain", 0x0560 },
        { "Serenade Of Water Warp", 0x0604 },
    }},
    { "26:Lake Hylia Buildings", Select_LoadGame, 2, { 
        { "Laboratory", 0x0043 }, 
        { "Fishing Pond Minigame", 0x045F },
    }},
    { "27:Gerudo Valley", Select_LoadGame, 5, {
        { "From Hyrule Field", 0x0117 },
        { "From Gerudo Fortress", 0x022D },
        { "From Carpenter's Tent", 0x03D0 },
        { "Carpenter's Tent/ Running Man Minigame", 0x03A0 },
        { "Thrown out of Fortress", 0x01A5 },
    }},
    { "28:Gerudo Fortress", Select_LoadGame, 18, {
        { "From Gerudo Valley", 0x0129 },
        { "From Traning Grounds", 0x03A8 },
        { "From Haunted Wasteland", 0x03AC },
        { "Horseback Riding Minigame", 0x03B0 },
        { "Gerudo Fortress Jail", 0x03B4 },
        { "From Thieves Hideout (1)", 0x0231 },
        { "From Thieves Hideout (2)", 0x0235 },
        { "From Thieves Hideout (3)", 0x0239 },
        { "From Thieves Hideout (4)", 0x02AA },
        { "From Thieves Hideout (5)", 0x02BA },
        { "From Thieves Hideout (6)", 0x02BE },
        { "From Thieves Hideout (7)", 0x02C2 },
        { "From Thieves Hideout (8)", 0x02C6 },
        { "From Thieves Hideout (9)", 0x02D2 },
        { "From Thieves Hideout (10)", 0x02D6 },
        { "From Thieves Hideout (11)", 0x02DA },
        { "From Thieves Hideout (12)", 0x02DE },
        { "From Thieves Hideout (13)", 0x03A4 },
    }},
    { "29:Thieves Hideout", Select_LoadGame, 13, {
        { "From Gerudo Fortress (1)", 0x0486 },
        { "From Gerudo Fortress (2)", 0x048A },
        { "From Gerudo Fortress (3)", 0x048E },
        { "From Gerudo Fortress (4)", 0x0492 },
        { "From Gerudo Fortress (5)", 0x0496 },
        { "From Gerudo Fortress (6)", 0x049A },
        { "From Gerudo Fortress (7)", 0x049E },
        { "From Gerudo Fortress (8)", 0x04A2 },
        { "From Gerudo Fortress (9)", 0x04A6 },
        { "From Gerudo Fortress (10)", 0x04AA },
        { "From Gerudo Fortress (11)", 0x04AE },
        { "From Gerudo Fortress (12)", 0x04B2 },
        { "From Gerudo Fortress (13)", 0x0570 },
    }},
    { "30:Haunted Wasteland", Select_LoadGame, 2, {
        { "From Gerudo Fortress", 0x0130 },
        { "From Desert Colossus", 0x0365 },
    }},
    { "31:Desert Colossus", Select_LoadGame, 7, {
        { "From Haunted Wasteland", 0x0123 },
        { "From Spirit Temple", 0x01E1 },
        { "From Spirit Temple (Left Hand)", 0x01E5 },
        { "From Spirit Temple (Right Hand)", 0x01E9 },
        { "From Fairy Fountain", 0x057C },
        { "Great Fairy", 0x0588 },
        { "Requiem of Spirit Warp", 0x01F1 },
    }},
    { "32:Deku Tree", Select_LoadGame, 3, {
        { "Entrance", 0x0001 },
        { "From Gohma Fight", 0x0252 },
        { "Gohma Fight", 0x040F },
    }},
    { "33:Dodongos Cavern", Select_LoadGame, 3, {
        { "Entrance", 0x0004 },
        { "From King Dodongo", 0x00C5 },
        { "King Dodongo", 0x040B },
    }},
    { "34:Jabu Jabu", Select_LoadGame, 2, {
        { "Entrance", 0x0028 },
        { "Barinade Fight", 0x0301 },
    }},
    { "35:Forest Temple", Select_LoadGame, 4, {
        { "Entrance", 0x0169 },
        { "Crushing Room", 0x0584 },
        { "Before Phantom Ganon", 0x024E },
        { "Phantom Ganon Fight", 0x000C },
    }},
    { "36:Fire Temple", Select_LoadGame, 3, {
        { "Entrance", 0x0165 },
        { "Before Volvagia", 0x0175 },
        { "Volvagia", 0x0305 },
    }},
    { "37:Water Temple", Select_LoadGame, 2, { 
        { "Entrance", 0x0010 },
        { "Barinade", 0x0417 },
    }},
    { "38:Shadow Temple", Select_LoadGame, 3, {
        { "Entrance", 0x0037 },
        { "Outside Bongo Bongo", 0x02B2 },
        { "Bongo Bongo", 0x0413 },
    }},
    { "39:Spirit Temple", Select_LoadGame, 6, {
        { "Entrance", 0x0082 },
        { "From Left Hand", 0x03F0 },
        { "From Right Hand", 0x03F4 },
        { "Before Twinrova", 0x02F5 },
        { "Naboora Fight", 0x008D },
        { "Twinrova", 0x05EC },
    }},
    { "40:Ganons Castle", Select_LoadGame, 9, {
        { "Entrance", 0x0467 },
        { "From Tower", 0x0534 },
        { "Stairs to Lair - From Castle", 0x041B },
        { "Stairs to Lair - From Ganondorf's Lair", 0x0427 },
        { "Ganondorf's Lair", 0x041F },
        { "Ganondorf Defeated", 0x01C9 },
        { "Ganondorf Defeated (2)", 0x04BA },
        { "Ganon Battle", 0x0517 },
        { "Ganon Death Cutscene", 0x043F },
    }},
    { "41:Bottom of the Well", Select_LoadGame, 1, {
        { "Entrance", 0x0098 },
    }},
    { "42:Ice Cavern", Select_LoadGame, 1, {
        { "Entrance", 0x0088 },
    }},
    { "43:Gerudo Training Grounds", Select_LoadGame, 1, {
        { "Entrance", 0x0008 },
    }},
    { "44:Warps", Select_LoadGame, 6, {
        { "Prelude of Light Warp", 0x05F4 },
        { "Minuet of Forest Warp", 0x0600 },
        { "Bolero of Fire Warp", 0x04F6 },
        { "Serenade Of Water Warp", 0x0604 },
        { "Nocturne of Shadow Warp", 0x0568 },
        { "Requiem of Spirit Warp", 0x01F1 },
    }},
    { "45:Shops", Select_LoadGame, 9, {
        { "Kokiri Shop", 0x00C1 },
        { "Potion Shop (Market)", 0x0388 },
        { "Bazaar Shop (Market)", 0x052C },
        { "Happy Mask Shop", 0x0530 },
        { "Bombchu Shop", 0x0528 },
        { "Bazaar Shop (Kakariko)", 0x00B7 },
        { "Potion Shop (Kakariko)", 0x0384 },
        { "Goron City Shop", 0x037C },
        { "Zora Shop", 0x0380 },
    }},
    { "46:Great Fairies", Select_LoadGame, 6, {
        { "Hyrule Castle (Child)", 0x0578 },
        { "Hyrule Castle (Adult)", 0x04C2 },
        { "Death Mountain Trail", 0x0315 },
        { "Death Mountain Crater", 0x04BE },
        { "Zoras Fountain", 0x0371 },
        { "Desert Colossus", 0x0588 },
    }},
    { "47:Chest Grottos", Select_Grotto_LoadGame, 11, {
        { "Kokiri Forest (Song of Storms)", 0x00 },
        { "Lost Woods", 0x01 },
        { "Sacred Forest Meadow", 0x02 },
        { "Hyrule Field (Near Market)", 0x03 },
        { "Hyrule Field (Open Near Lake)", 0x04 },
        { "Hyrule Field (SE Boulder)", 0x05 },
        { "Kakariko (Open)", 0x06 },
        { "Kakariko (Redead)", 0x07 },
        { "Death Mountain Trail (Song of Storms)", 0x08 },
        { "Death Mountain Crater", 0x09 },
        { "Zora River (Open)", 0x0A },
    }},
    { "48:Scrub Grottos", Select_Grotto_LoadGame, 10, {
        { "Hyrule Field (Near Lake)", 0x0B },
        { "Death Mountain Crater", 0x0C },
        { "Goron City", 0x0D },
        { "Lon Lon Ranch", 0x0E },
        { "Lake Hylia", 0x0F },
        { "Lost Woods", 0x10 },
        { "Zora River (Song of Storms)", 0x11 },
        { "Sacred Forest Meadow (Song of Storms)", 0x12 },
        { "Gerudo Valley (Song of Storms)", 0x13 },
        { "Desert Colossus", 0x14 },
    }},
    { "49:Other Grottos", Select_Grotto_LoadGame, 7, {
        { "Scrub Theatre", 0x15 },
        { "Spider Grotto (Hyrule Field)", 0x16 },
        { "Spider Grotto (Hyrule Castle)", 0x17 },
        { "Cow Grotto (Hyrule Field)", 0x18 },
        { "Cow Grotto (Death Mountain Trail)", 0x19 },
        { "Flooded Grotto (Gerudo Valley)", 0x1A },
        { "Flooded Grotto (Hyrule Field)", 0x1B },
    }},
    { "50:Debug (Use with caution)", Select_LoadGame, 10, {
        { "Test Room", 0x0520 },
        { "SRD Map", 0x0018 },
        { "Test Map", 0x0094 },
        { "Treasure Chest Warp", 0x0024 },
        { "Stalfos Miniboss Room", 0x001C },
        { "Stalfos Boss Room", 0x001C },
        { "Dark Link Room", 0x0047 },
        { "Shooting Gallery Duplicate", 0x02EA },
        { "Depth Test", 0x00B6 },
        { "Hyrule Garden Game (Broken)", 0x0076 },
    }},
};

static BetterSceneSelectGrottoData sBetterGrottos[] = {
    { 0x003F, 0x00EE, 0, 0x2C,       {  -504.0,   380.0, -1224.0 }},
    { 0x003F, 0x04D6, 2, 0x14,       {   922.0,     0.0,  -933.0 }},
    { 0x05B4, 0x00FC, 0, 0xFFFFFFED, {  -201.0,     0.0,  1906.0 }},
    { 0x003F, 0x00CD, 0, 0x00,       { -1428.0,     0.0,   790.0 }},
    { 0x003F, 0x0189, 0, 0x03,        { -4026.0, -700.0, 13858.0 }},
    { 0x003F, 0x0189, 0, 0x22,       {  -259.0,  -500.0, 12356.0 }},
    { 0x003F, 0x034D, 0, 0x28,       {   861.0,    80.0,  -253.0 }},
    { 0x05A0, 0x034D, 0, 0xFFFFFFE7, {  -400.0,     0.0,   408.0 }},
    { 0x003F, 0x01B9, 0, 0x57,       {  -389.0,  1386.0, -1202.0 }},
    { 0x003F, 0x0147, 1, 0x7A,       {    50.0,  1233.0,  1776.0 }},
    { 0x003F, 0x019D, 0, 0x29,       {   369.0,   570.0,   128.0 }},
    { 0x059C, 0x0189, 0, 0xFFFFFFE6, { -5002.0,  -700.0, 13823.0 }},
    { 0x05A4, 0x0246, 1, 0xFFFFFFF9, { -1703.0,   722.0,  -481.0 }},
    { 0x05A4, 0x014D, 3, 0xFFFFFFFB, {  1091.0,   580.0, -1192.0 }},
    { 0x05A4, 0x05D4, 0, 0xFFFFFFFC, {  1798.0,     0.0,  1498.0 }},
    { 0x05A4, 0x021D, 0, 0xFFFFFFEF, { -3044.0, -1033.0,  6070.0 }},
    { 0x05B0, 0x01A9, 8, 0xFFFFFFF5, {   677.0,     0.0, -2515.0 }},
    { 0x05BC, 0x00EA, 0, 0xFFFFFFEB, { -1632.0,   100.0,  -123.0 }},
    { 0x05BC, 0x0215, 0, 0xFFFFFFEE, {   317.0,   480.0, -2303.0 }},
    { 0x05BC, 0x03D0, 0, 0xFFFFFFF0, { -1321.0,    15.0,  -968.0 }},
    { 0x05BC, 0x01F1, 0, 0xFFFFFFFD, {    71.0,   -32.0, -1303.0 }},
    { 0x05C4, 0x04D6, 6, 0xFFFFFFF3, {    75.0,   -20.0, -1596.0 }},
    { 0x0598, 0x017D, 0, 0xFFFFFFE5, {  2059.0,    20.0,  -174.0 }},
    { 0x05B8, 0x023D, 0, 0xFFFFFFF6, {   986.0,  1571.0,   837.0 }},
    { 0x05A8, 0x018D, 0, 0xFFFFFFE4, { -7873.0,  -300.0,  6916.0 }},
    { 0x05FC, 0x01B9, 0, 0xFFFFFFF8, {  -678.0,  1946.0,  -284.0 }},
    { 0x05AC, 0x0117, 0, 0xFFFFFFF2, {   271.0,  -555.0,  1465.0 }},
    { 0x05C0, 0x00CD, 0, 0xFFFFFFE1, { -4945.0,  -300.0,  2841.0 }},
};

void Select_UpdateMenu(SelectContext* this) {
    Input* input = &this->state.input[0];
    s32 pad;
    SceneSelectEntry* selectedScene;

    if (this->verticalInputAccumulator == 0) {
        if (CHECK_BTN_ALL(input->press.button, BTN_A) || CHECK_BTN_ALL(input->press.button, BTN_START)) {
            selectedScene = &this->scenes[this->currentScene];
            if (selectedScene->loadFunc != NULL) {
                selectedScene->loadFunc(this, selectedScene->entranceIndex);
            }
        }

        if (CHECK_BTN_ALL(input->press.button, BTN_B)) {
            if (LINK_AGE_IN_YEARS == YEARS_ADULT) {
                gSaveContext.linkAge = 1;
            } else {
                gSaveContext.linkAge = 0;
            }
        }

        if (CHECK_BTN_ALL(input->press.button, BTN_Z)) {
            if (gSaveContext.cutsceneIndex == 0x8000) {
                gSaveContext.cutsceneIndex = 0;
            } else if (gSaveContext.cutsceneIndex == 0) {
                gSaveContext.cutsceneIndex = 0xFFF0;
            } else if (gSaveContext.cutsceneIndex == 0xFFF0) {
                gSaveContext.cutsceneIndex = 0xFFF1;
            } else if (gSaveContext.cutsceneIndex == 0xFFF1) {
                gSaveContext.cutsceneIndex = 0xFFF2;
            } else if (gSaveContext.cutsceneIndex == 0xFFF2) {
                gSaveContext.cutsceneIndex = 0xFFF3;
            } else if (gSaveContext.cutsceneIndex == 0xFFF3) {
                gSaveContext.cutsceneIndex = 0xFFF4;
            } else if (gSaveContext.cutsceneIndex == 0xFFF4) {
                gSaveContext.cutsceneIndex = 0xFFF5;
            } else if (gSaveContext.cutsceneIndex == 0xFFF5) {
                gSaveContext.cutsceneIndex = 0xFFF6;
            } else if (gSaveContext.cutsceneIndex == 0xFFF6) {
                gSaveContext.cutsceneIndex = 0xFFF7;
            } else if (gSaveContext.cutsceneIndex == 0xFFF7) {
                gSaveContext.cutsceneIndex = 0xFFF8;
            } else if (gSaveContext.cutsceneIndex == 0xFFF8) {
                gSaveContext.cutsceneIndex = 0xFFF9;
            } else if (gSaveContext.cutsceneIndex == 0xFFF9) {
                gSaveContext.cutsceneIndex = 0xFFFA;
            } else if (gSaveContext.cutsceneIndex == 0xFFFA) {
                gSaveContext.cutsceneIndex = 0x8000;
            }
        } else if (CHECK_BTN_ALL(input->press.button, BTN_R)) {
            if (gSaveContext.cutsceneIndex == 0x8000) {
                gSaveContext.cutsceneIndex = 0xFFFA;
            } else if (gSaveContext.cutsceneIndex == 0) {
                gSaveContext.cutsceneIndex = 0x8000;
            } else if (gSaveContext.cutsceneIndex == 0xFFF0) {
                gSaveContext.cutsceneIndex = 0;
            } else if (gSaveContext.cutsceneIndex == 0xFFF1) {
                gSaveContext.cutsceneIndex = 0xFFF0;
            } else if (gSaveContext.cutsceneIndex == 0xFFF2) {
                gSaveContext.cutsceneIndex = 0xFFF1;
            } else if (gSaveContext.cutsceneIndex == 0xFFF3) {
                gSaveContext.cutsceneIndex = 0xFFF2;
            } else if (gSaveContext.cutsceneIndex == 0xFFF4) {
                gSaveContext.cutsceneIndex = 0xFFF3;
            } else if (gSaveContext.cutsceneIndex == 0xFFF5) {
                gSaveContext.cutsceneIndex = 0xFFF4;
            } else if (gSaveContext.cutsceneIndex == 0xFFF6) {
                gSaveContext.cutsceneIndex = 0xFFF5;
            } else if (gSaveContext.cutsceneIndex == 0xFFF7) {
                gSaveContext.cutsceneIndex = 0xFFF6;
            } else if (gSaveContext.cutsceneIndex == 0xFFF8) {
                gSaveContext.cutsceneIndex = 0xFFF7;
            } else if (gSaveContext.cutsceneIndex == 0xFFF9) {
                gSaveContext.cutsceneIndex = 0xFFF8;
            } else if (gSaveContext.cutsceneIndex == 0xFFFA) {
                gSaveContext.cutsceneIndex = 0xFFF9;
            }
        }

        gSaveContext.nightFlag = 0;
        if (gSaveContext.cutsceneIndex == 0) {
            gSaveContext.nightFlag = 1;
        }

        // user can change "opt", but it doesn't do anything
        if (CHECK_BTN_ALL(input->press.button, BTN_CUP)) {
            this->opt--;
        }
        if (CHECK_BTN_ALL(input->press.button, BTN_CDOWN)) {
            this->opt++;
        }

        if (CHECK_BTN_ALL(input->press.button, BTN_DUP)) {
            if (this->lockUp == true) {
                this->timerUp = 0;
            }
            if (this->timerUp == 0) {
                this->timerUp = 20;
                this->lockUp = true;
                Audio_PlaySoundGeneral(NA_SE_IT_SWORD_IMPACT, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                this->verticalInput = R_UPDATE_RATE;
            }
        }

        if (CHECK_BTN_ALL(input->cur.button, BTN_DUP) && this->timerUp == 0) {
            Audio_PlaySoundGeneral(NA_SE_IT_SWORD_IMPACT, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            this->verticalInput = R_UPDATE_RATE * 3;
        }

        if (CHECK_BTN_ALL(input->press.button, BTN_DDOWN)) {
            if (this->lockDown == true) {
                this->timerDown = 0;
            }
            if (this->timerDown == 0) {
                this->timerDown = 20;
                this->lockDown = true;
                Audio_PlaySoundGeneral(NA_SE_IT_SWORD_IMPACT, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                this->verticalInput = -R_UPDATE_RATE;
            }
        }

        if (CHECK_BTN_ALL(input->cur.button, BTN_DDOWN) && (this->timerDown == 0)) {
            Audio_PlaySoundGeneral(NA_SE_IT_SWORD_IMPACT, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            this->verticalInput = -R_UPDATE_RATE * 3;
        }

        if (CHECK_BTN_ALL(input->press.button, BTN_DLEFT) || CHECK_BTN_ALL(input->cur.button, BTN_DLEFT)) {
            Audio_PlaySoundGeneral(NA_SE_IT_SWORD_IMPACT, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            this->verticalInput = R_UPDATE_RATE;
        }

        if (CHECK_BTN_ALL(input->press.button, BTN_DRIGHT) || CHECK_BTN_ALL(input->cur.button, BTN_DRIGHT)) {
            Audio_PlaySoundGeneral(NA_SE_IT_SWORD_IMPACT, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            this->verticalInput = -R_UPDATE_RATE;
        }
    }

    if (CHECK_BTN_ALL(input->press.button, BTN_L)) {
        this->pageDownIndex++;
        this->pageDownIndex =
            (this->pageDownIndex + ARRAY_COUNT(this->pageDownStops)) % ARRAY_COUNT(this->pageDownStops);
        this->currentScene = this->topDisplayedScene = this->pageDownStops[this->pageDownIndex];
    }

    this->verticalInputAccumulator += this->verticalInput;

    if (this->verticalInputAccumulator < -7) {
        this->verticalInput = 0;
        this->verticalInputAccumulator = 0;

        this->currentScene++;
        this->currentScene = (this->currentScene + this->count) % this->count;

        if (this->currentScene == ((this->topDisplayedScene + this->count + 19) % this->count)) {
            this->topDisplayedScene++;
            this->topDisplayedScene = (this->topDisplayedScene + this->count) % this->count;
        }
    }

    if (this->verticalInputAccumulator > 7) {
        this->verticalInput = 0;
        this->verticalInputAccumulator = 0;

        if (this->currentScene == this->topDisplayedScene) {
            this->topDisplayedScene -= 2;
            this->topDisplayedScene = (this->topDisplayedScene + this->count) % this->count;
        }

        this->currentScene--;
        this->currentScene = (this->currentScene + this->count) % this->count;

        if (this->currentScene == ((this->topDisplayedScene + this->count) % this->count)) {
            this->topDisplayedScene--;
            this->topDisplayedScene = (this->topDisplayedScene + this->count) % this->count;
        }
    }

    this->currentScene = (this->currentScene + this->count) % this->count;
    this->topDisplayedScene = (this->topDisplayedScene + this->count) % this->count;

    dREG(80) = this->currentScene;
    dREG(81) = this->topDisplayedScene;
    dREG(82) = this->pageDownIndex;

    if (this->timerUp != 0) {
        this->timerUp--;
    }

    if (this->timerUp == 0) {
        this->lockUp = false;
    }

    if (this->timerDown != 0) {
        this->timerDown--;
    }

    if (this->timerDown == 0) {
        this->lockDown = false;
    }
}

void Better_Select_UpdateMenu(SelectContext* this) {
    Input* input = &this->state.input[0];
    s32 pad;
    BetterSceneSelectEntry* selectedScene;

    if (this->verticalInputAccumulator == 0) {
        if (CHECK_BTN_ALL(input->press.button, BTN_A) || CHECK_BTN_ALL(input->press.button, BTN_START)) {
            selectedScene = &this->betterScenes[this->currentScene];
            if (selectedScene->loadFunc != NULL) {
                selectedScene->loadFunc(this, selectedScene->entrancePairs[this->pageDownIndex].entranceIndex);
            }
        }

        if (CHECK_BTN_ALL(input->press.button, BTN_B)) {
            if (LINK_AGE_IN_YEARS == YEARS_ADULT) {
                gSaveContext.linkAge = 1;
                Audio_PlaySoundGeneral(NA_SE_VO_LI_SWORD_N_KID, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            } else {
                gSaveContext.linkAge = 0;
                Audio_PlaySoundGeneral(NA_SE_VO_LI_SWORD_N, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            }
        }

        if (CHECK_BTN_ALL(input->press.button, BTN_Z) || CHECK_BTN_ALL(input->press.button, BTN_R)) {
            if (gSaveContext.dayTime > 0xC000 || gSaveContext.dayTime < 0x4555) {
                gSaveContext.nightFlag = 0;
                gSaveContext.dayTime = 0x8000;
                Audio_PlaySoundGeneral(NA_SE_EV_CHICKEN_CRY_M, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            } else {
                gSaveContext.nightFlag = 1;
                gSaveContext.dayTime = 0x0000;
                Audio_PlaySoundGeneral(NA_SE_EV_DOG_CRY_EVENING, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            }
        }

        if (CHECK_BTN_ALL(input->press.button, BTN_CLEFT) || CHECK_BTN_ALL(input->press.button, BTN_DLEFT)) {
            this->pageDownIndex--;
            Audio_PlaySoundGeneral(NA_SE_IT_SWORD_SWING, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            if (this->pageDownIndex < 0) {
                this->pageDownIndex = this->betterScenes[this->currentScene].count - 1;
            }
        }
        if (CHECK_BTN_ALL(input->press.button, BTN_CRIGHT) || CHECK_BTN_ALL(input->press.button, BTN_DRIGHT)) {
            this->pageDownIndex++;
            Audio_PlaySoundGeneral(NA_SE_IT_SWORD_SWING, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            if (this->pageDownIndex > this->betterScenes[this->currentScene].count - 1) {
                this->pageDownIndex = 0;
            }
        }

        if (CHECK_BTN_ALL(input->press.button, BTN_DUP) || CHECK_BTN_ALL(input->press.button, BTN_CUP)) {
            if (this->lockUp == true) {
                this->timerUp = 0;
            }
            if (this->timerUp == 0) {
                this->timerUp = 20;
                this->lockUp = true;
                Audio_PlaySoundGeneral(NA_SE_IT_SWORD_IMPACT, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                this->verticalInput = R_UPDATE_RATE;
            }
        }

        if ((CHECK_BTN_ALL(input->cur.button, BTN_DUP) || CHECK_BTN_ALL(input->cur.button, BTN_CUP)) && this->timerUp == 0) {
            Audio_PlaySoundGeneral(NA_SE_IT_SWORD_IMPACT, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            this->verticalInput = R_UPDATE_RATE * 3;
        }

        if (CHECK_BTN_ALL(input->press.button, BTN_DDOWN) || CHECK_BTN_ALL(input->press.button, BTN_CDOWN)) {
            if (this->lockDown == true) {
                this->timerDown = 0;
            }
            if (this->timerDown == 0) {
                this->timerDown = 20;
                this->lockDown = true;
                Audio_PlaySoundGeneral(NA_SE_IT_SWORD_IMPACT, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                this->verticalInput = -R_UPDATE_RATE;
            }
        }

        if ((CHECK_BTN_ALL(input->cur.button, BTN_DDOWN) || CHECK_BTN_ALL(input->cur.button, BTN_CDOWN)) && (this->timerDown == 0)) {
            Audio_PlaySoundGeneral(NA_SE_IT_SWORD_IMPACT, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            this->verticalInput = -R_UPDATE_RATE * 3;
        }
    }

    this->verticalInputAccumulator += this->verticalInput;

    if (this->verticalInputAccumulator < -7) {
        this->verticalInput = 0;
        this->verticalInputAccumulator = 0;

        this->currentScene++;
        this->currentScene = (this->currentScene + this->count) % this->count;
        this->pageDownIndex = 0;

        if (this->currentScene == ((this->topDisplayedScene + this->count + 19) % this->count)) {
            this->topDisplayedScene++;
            this->topDisplayedScene = (this->topDisplayedScene + this->count) % this->count;
        }
    }

    if (this->verticalInputAccumulator > 7) {
        this->verticalInput = 0;
        this->verticalInputAccumulator = 0;

        if (this->currentScene == this->topDisplayedScene) {
            this->topDisplayedScene -= 2;
            this->topDisplayedScene = (this->topDisplayedScene + this->count) % this->count;
        }

        this->currentScene--;
        this->currentScene = (this->currentScene + this->count) % this->count;
        this->pageDownIndex = 0;

        if (this->currentScene == ((this->topDisplayedScene + this->count) % this->count)) {
            this->topDisplayedScene--;
            this->topDisplayedScene = (this->topDisplayedScene + this->count) % this->count;
        }
    }

    this->currentScene = (this->currentScene + this->count) % this->count;
    this->topDisplayedScene = (this->topDisplayedScene + this->count) % this->count;

    dREG(80) = this->currentScene;
    dREG(81) = this->topDisplayedScene;
    dREG(82) = this->pageDownIndex;

    if (this->timerUp != 0) {
        this->timerUp--;
    }

    if (this->timerUp == 0) {
        this->lockUp = false;
    }

    if (this->timerDown != 0) {
        this->timerDown--;
    }

    if (this->timerDown == 0) {
        this->lockDown = false;
    }
}

void Select_PrintMenu(SelectContext* this, GfxPrint* printer) {
    s32 scene;
    s32 i;
    char* name;

    GfxPrint_SetColor(printer, 255, 155, 150, 255);
    GfxPrint_SetPos(printer, 12, 2);
    GfxPrint_Printf(printer, "ZELDA MAP SELECT");
    GfxPrint_SetColor(printer, 255, 255, 255, 255);

    for (i = 0; i < 20; i++) {
        GfxPrint_SetPos(printer, 9, i + 4);

        scene = (this->topDisplayedScene + i + this->count) % this->count;
        if (scene == this->currentScene) {
            GfxPrint_SetColor(printer, 255, 20, 20, 255);
        } else {
            GfxPrint_SetColor(printer, 200, 200, 55, 255);
        }

        name = this->scenes[scene].name;
        if (name == NULL) {
            name = "**Null**";
        }

        GfxPrint_Printf(printer, "%s", name);
    };

    GfxPrint_SetColor(printer, 155, 55, 150, 255);
    GfxPrint_SetPos(printer, 20, 26);
    GfxPrint_Printf(printer, "OPT=%d", this->opt);
}

void Better_Select_PrintMenu(SelectContext* this, GfxPrint* printer) {
    s32 scene;
    s32 i;
    char* name;

    GfxPrint_SetColor(printer, 255, 255, 255, 255);
    GfxPrint_SetPos(printer, 12, 2);
    GfxPrint_Printf(printer, "Scene Selection");
    GfxPrint_SetColor(printer, 255, 255, 255, 255);

    for (i = 0; i < 20; i++) {
        GfxPrint_SetPos(printer, 3, i + 4);

        scene = (this->topDisplayedScene + i + this->count) % this->count;
        if (scene == this->currentScene) {
            GfxPrint_SetColor(printer, 255, 100, 100, 255);
        } else {
            GfxPrint_SetColor(printer, 175, 175, 175, 255);
        }

        name = this->betterScenes[scene].name;
        if (name == NULL) {
            name = "**Null**";
        }

        GfxPrint_Printf(printer, "%s", name);
    };

    GfxPrint_SetColor(printer, 205, 100, 200, 255);
    GfxPrint_SetPos(printer, 3, 26);
    GfxPrint_Printf(printer, "%s", this->betterScenes[this->currentScene].entrancePairs[this->pageDownIndex].name);
}

static const char* sLoadingMessages[] = {
    // "Please wait a minute"
    GFXP_HIRAGANA "ｼﾊﾞﾗｸｵﾏﾁｸﾀﾞｻｲ",
    // "Hold on a sec"
    GFXP_HIRAGANA "ﾁｮｯﾄ ﾏｯﾃﾈ",
    // "Wait a moment"
    GFXP_KATAKANA "ｳｪｲﾄ ｱ ﾓｰﾒﾝﾄ",
    // "Loading"
    GFXP_KATAKANA "ﾛｰﾄﾞ" GFXP_HIRAGANA "ﾁｭｳ",
    // "Now working"
    GFXP_HIRAGANA "ﾅｳ ﾜｰｷﾝｸﾞ",
    // "Now creating"
    GFXP_HIRAGANA "ｲﾏ ﾂｸｯﾃﾏｽ",
    // "It's not broken"
    GFXP_HIRAGANA "ｺｼｮｳｼﾞｬﾅｲﾖ",
    // "Coffee Break"
    GFXP_KATAKANA "ｺｰﾋｰ ﾌﾞﾚｲｸ",
    // "Please set B side"
    GFXP_KATAKANA "Bﾒﾝｦｾｯﾄｼﾃｸﾀﾞｻｲ",
    // "Be patient, now"
    GFXP_HIRAGANA "ｼﾞｯﾄ" GFXP_KATAKANA "ｶﾞﾏﾝ" GFXP_HIRAGANA "ﾉ" GFXP_KATAKANA "ｺ" GFXP_HIRAGANA "ﾃﾞｱｯﾀ",
    // "Please wait just a minute"
    GFXP_HIRAGANA "ｲﾏｼﾊﾞﾗｸｵﾏﾁｸﾀﾞｻｲ",
    // "Don't worry, don't worry. Take a break, take a break."
    GFXP_HIRAGANA "ｱﾜﾃﾅｲｱﾜﾃﾅｲ｡ﾋﾄﾔｽﾐﾋﾄﾔｽﾐ｡",
};

void Select_PrintLoadingMessage(SelectContext* this, GfxPrint* printer) {
    s32 randomMsg;

    GfxPrint_SetPos(printer, 10, 15);
    GfxPrint_SetColor(printer, 255, 255, 255, 255);
    randomMsg = Rand_ZeroOne() * ARRAY_COUNT(sLoadingMessages);
    GfxPrint_Printf(printer, "%s", sLoadingMessages[randomMsg]);
}

static const char* sAgeLabels[] = {
    GFXP_HIRAGANA "17(ﾜｶﾓﾉ)", // "17(young)"
    GFXP_HIRAGANA "5(ﾜｶｽｷﾞ)", // "5(very young)"
};
static const char* sBetterAgeLabels[] = {
    "Adult",
    "Child",
};

void Select_PrintAgeSetting(SelectContext* this, GfxPrint* printer, s32 age) {
    GfxPrint_SetPos(printer, 4, 26);
    GfxPrint_SetColor(printer, 255, 255, 55, 255);
    GfxPrint_Printf(printer, "Age:%s", sAgeLabels[age]);
}

void Better_Select_PrintAgeSetting(SelectContext* this, GfxPrint* printer, s32 age) {
    GfxPrint_SetPos(printer, 25, 25);
    GfxPrint_SetColor(printer, 100, 100, 100, 255);
    GfxPrint_Printf(printer, "(B)Age:");
    GfxPrint_SetColor(printer, 55, 200, 50, 255);
    GfxPrint_Printf(printer, "%s", sBetterAgeLabels[age]);
}

void Select_PrintCutsceneSetting(SelectContext* this, GfxPrint* printer, u16 csIndex) {
    char* label;

    GfxPrint_SetPos(printer, 4, 25);
    GfxPrint_SetColor(printer, 255, 255, 55, 255);

    switch (csIndex) {
        case 0:
            label = GFXP_HIRAGANA " ﾖﾙ " GFXP_KATAKANA "ｺﾞﾛﾝ";
            gSaveContext.dayTime = 0;
            break;
        case 0x8000:
            // clang-format off
            gSaveContext.dayTime = 0x8000; label = GFXP_HIRAGANA "ｵﾋﾙ " GFXP_KATAKANA "ｼﾞｬﾗ";
            // clang-format on
            break;
        case 0xFFF0:
            // clang-format off
            gSaveContext.dayTime = 0x8000; label = "ﾃﾞﾓ00";
            // clang-format on
            break;
        case 0xFFF1:
            label = "ﾃﾞﾓ01";
            break;
        case 0xFFF2:
            label = "ﾃﾞﾓ02";
            break;
        case 0xFFF3:
            label = "ﾃﾞﾓ03";
            break;
        case 0xFFF4:
            label = "ﾃﾞﾓ04";
            break;
        case 0xFFF5:
            label = "ﾃﾞﾓ05";
            break;
        case 0xFFF6:
            label = "ﾃﾞﾓ06";
            break;
        case 0xFFF7:
            label = "ﾃﾞﾓ07";
            break;
        case 0xFFF8:
            label = "ﾃﾞﾓ08";
            break;
        case 0xFFF9:
            label = "ﾃﾞﾓ09";
            break;
        case 0xFFFA:
            label = "ﾃﾞﾓ0A";
            break;
    };

    gSaveContext.skyboxTime = gSaveContext.dayTime;
    GfxPrint_Printf(printer, "Stage:" GFXP_KATAKANA "%s", label);
}

void Better_Select_PrintTimeSetting(SelectContext* this, GfxPrint* printer) {
    char* label;

    GfxPrint_SetPos(printer, 22, 24);
    GfxPrint_SetColor(printer, 100, 100, 100, 255);

    if (gSaveContext.dayTime > 0xC000 || gSaveContext.dayTime < 0x4555) {
        label = "Night";
    } else {
        label = "Day";
    }
    GfxPrint_Printf(printer, "(Z/R)Time:");
    GfxPrint_SetColor(printer, 200, 200, 55, 255);
    GfxPrint_Printf(printer, "%s", label);
}

void Select_DrawMenu(SelectContext* this) {
    GraphicsContext* gfxCtx = this->state.gfxCtx;
    GfxPrint* printer;

    OPEN_DISPS(gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x00, NULL);
    func_80095248(gfxCtx, 0, 0, 0);
    SET_FULLSCREEN_VIEWPORT(&this->view);
    func_800AAA50(&this->view, 0xF);
    func_80094140(gfxCtx);

    printer = alloca(sizeof(GfxPrint));
    GfxPrint_Init(printer);
    GfxPrint_Open(printer, POLY_OPA_DISP);
    if (CVar_GetS32("gBetterDebugWarpScreen", 0)) {
        Better_Select_PrintMenu(this, printer);
        Better_Select_PrintAgeSetting(this, printer, ((void)0, gSaveContext.linkAge));
        Better_Select_PrintTimeSetting(this, printer);
    } else {
        Select_PrintMenu(this, printer);
        Select_PrintAgeSetting(this, printer, ((void)0, gSaveContext.linkAge));
        Select_PrintCutsceneSetting(this, printer, ((void)0, gSaveContext.cutsceneIndex));
    }
    POLY_OPA_DISP = GfxPrint_Close(printer);
    GfxPrint_Destroy(printer);

    CLOSE_DISPS(gfxCtx);
}

void Select_DrawLoadingScreen(SelectContext* this) {
    GraphicsContext* gfxCtx = this->state.gfxCtx;
    GfxPrint* printer;

    OPEN_DISPS(gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x00, NULL);
    func_80095248(gfxCtx, 0, 0, 0);
    SET_FULLSCREEN_VIEWPORT(&this->view);
    func_800AAA50(&this->view, 0xF);
    func_80094140(gfxCtx);

    printer = alloca(sizeof(GfxPrint));
    GfxPrint_Init(printer);
    GfxPrint_Open(printer, POLY_OPA_DISP);
    Select_PrintLoadingMessage(this, printer);
    POLY_OPA_DISP = GfxPrint_Close(printer);
    GfxPrint_Destroy(printer);

    CLOSE_DISPS(gfxCtx);
}

void Select_Draw(SelectContext* this) {
    GraphicsContext* gfxCtx = this->state.gfxCtx;

    OPEN_DISPS(gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x00, NULL);
    func_80095248(gfxCtx, 0, 0, 0);
    SET_FULLSCREEN_VIEWPORT(&this->view);
    func_800AAA50(&this->view, 0xF);

    if (!this->state.running) {
        Select_DrawLoadingScreen(this);
    } else {
        Select_DrawMenu(this);
    }

    CLOSE_DISPS(gfxCtx);
}

void Select_Main(GameState* thisx) {
    SelectContext* this = (SelectContext*)thisx;

    if (CVar_GetS32("gBetterDebugWarpScreen", 0)) {
        Better_Select_UpdateMenu(this);
    } else {
        Select_UpdateMenu(this);
    }
    Select_Draw(this);
}

void Select_Destroy(GameState* thisx) {
    osSyncPrintf("%c", '\a'); // ASCII BEL character, plays an alert tone
    // "view_cleanup will hang, so it won't be called"
    osSyncPrintf("*** view_cleanupはハングアップするので、呼ばない ***\n");
}

void Select_Init(GameState* thisx) {
    SelectContext* this = (SelectContext*)thisx;
    size_t size;
    s32 pad;

    this->state.main = Select_Main;
    this->state.destroy = Select_Destroy;
    this->scenes = sScenes;
    this->betterScenes = sBetterScenes;
    this->betterGrottos = sBetterGrottos;
    this->topDisplayedScene = 0;
    this->currentScene = 0;
    this->pageDownStops[0] = 0;  // Hyrule Field
    this->pageDownStops[1] = 19; // Temple Of Time
    this->pageDownStops[2] = 37; // Treasure Chest Game
    this->pageDownStops[3] = 51; // Gravekeeper's Hut
    this->pageDownStops[4] = 59; // Zora Shop
    this->pageDownStops[5] = 73; // Bottom of the Well
    this->pageDownStops[6] = 91; // Escaping Ganon's Tower 3
    this->pageDownIndex = 0;
    this->opt = 0;
    this->count = CVar_GetS32("gBetterDebugWarpScreen", 0) ? ARRAY_COUNT(sBetterScenes) : ARRAY_COUNT(sScenes);
    View_Init(&this->view, this->state.gfxCtx);
    this->view.flags = (0x08 | 0x02);
    this->verticalInputAccumulator = 0;
    this->verticalInput = 0;
    this->timerUp = 0;
    this->timerDown = 0;
    this->lockUp = 0;
    this->lockDown = 0;
    this->unk_234 = 0;

    size = (uintptr_t)_z_select_staticSegmentRomEnd - (uintptr_t)_z_select_staticSegmentRomStart;

    if ((dREG(80) >= 0) && (dREG(80) < this->count)) {
        this->currentScene = dREG(80);
        this->topDisplayedScene = dREG(81);
        this->pageDownIndex = dREG(82);
    }
    if (CVar_GetS32("gBetterDebugWarpScreen", 0)) {
        this->currentScene = CVar_GetS32("gBetterDebugWarpScreenCurrentScene", 0);
        this->topDisplayedScene = CVar_GetS32("gBetterDebugWarpScreenTopDisplayedScene", 0);
        this->pageDownIndex = CVar_GetS32("gBetterDebugWarpScreenPageDownIndex", 0);
    }
    R_UPDATE_RATE = 1;
#if !defined(_MSC_VER) && !defined(__GNUC__)
    this->staticSegment = GAMESTATE_ALLOC_MC(&this->state, size);
    DmaMgr_SendRequest1(this->staticSegment, _z_select_staticSegmentRomStart, size, __FILE__, __LINE__);
#endif
    gSaveContext.cutsceneIndex = 0x8000;
    gSaveContext.linkAge = 1;
    gSaveContext.nightFlag = 0;
    gSaveContext.dayTime = 0x8000;
}
