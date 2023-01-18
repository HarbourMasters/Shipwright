/*
 * File: z_select.c
 * Overlay: ovl_select
 * Description: Debug Scene Select Menu
 */

#include <libultraship/libultra.h>
#include "global.h"
#include "vt.h"
#include "alloca.h"

#include "soh/Enhancements/randomizer/randomizer_entrance.h"

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
        gSaveContext.magicFillTarget = gSaveContext.magic;
        gSaveContext.magic = 0;
        gSaveContext.magicCapacity = 0;
        gSaveContext.magicLevel = gSaveContext.magic;
    }
    for (int buttonIndex = 0; buttonIndex < ARRAY_COUNT(gSaveContext.buttonStatus); buttonIndex++) {
        gSaveContext.buttonStatus[buttonIndex] = BTN_ENABLED;
    }
    gSaveContext.unk_13E7 = gSaveContext.unk_13E8 = gSaveContext.unk_13EA = gSaveContext.unk_13EC = 0;
    Audio_QueueSeqCmd(SEQ_PLAYER_BGM_MAIN << 24 | NA_BGM_STOP);
    gSaveContext.entranceIndex = entranceIndex;

    // Check the entrance to see if the exit should be overriden to a grotto return point for entrance rando
    if (gSaveContext.n64ddFlag && Randomizer_GetSettingValue(RSK_SHUFFLE_ENTRANCES)) {
        // Ignore return value as we want to load into the entrance specified by the debug menu
        Grotto_OverrideSpecialEntrance(Entrance_GetOverride(entranceIndex));
    }

    if (CVarGetInteger("gBetterDebugWarpScreen", 0)) {
        CVarSetInteger("gBetterDebugWarpScreenCurrentScene", this->currentScene);
        CVarSetInteger("gBetterDebugWarpScreenTopDisplayedScene", this->topDisplayedScene);
        CVarSetInteger("gBetterDebugWarpScreenPageDownIndex", this->pageDownIndex);
        CVarSave();
    }

    gSaveContext.respawnFlag = 0;
    gSaveContext.respawn[RESPAWN_MODE_DOWN].entranceIndex = -1;
    gSaveContext.seqId = (u8)NA_BGM_DISABLED;
    gSaveContext.natureAmbienceId = 0xFF;
    gSaveContext.showTitleCard = true;
    gWeatherMode = 0;
    this->state.running = false;
    SET_NEXT_GAMESTATE(&this->state, Play_Init, PlayState);
}

void Select_Grotto_LoadGame(SelectContext* this, s32 grottoIndex) {
    osSyncPrintf(VT_FGCOL(BLUE));
    osSyncPrintf("\n\n\nＦＩＬＥ＿ＮＯ＝%x\n\n\n", gSaveContext.fileNum);
    osSyncPrintf(VT_RST);
    if (gSaveContext.fileNum == 0xFF) {
        Sram_InitDebugSave();
        gSaveContext.magicFillTarget = gSaveContext.magic;
        gSaveContext.magic = 0;
        gSaveContext.magicCapacity = 0;
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

    // Check the entrance to see if the exit should be overriden to a grotto return point for entrance rando
    if (gSaveContext.n64ddFlag && Randomizer_GetSettingValue(RSK_SHUFFLE_ENTRANCES)) {
        // Use grotto content and parent scene num to identify the right grotto
        s16 grottoEntrance = Grotto_GetRenamedGrottoIndexFromOriginal(this->betterGrottos[grottoIndex].data, this->betterGrottos[grottoIndex].exitScene);
        // Ignore return value as we want to load into the entrance specified by the debug menu
        Grotto_OverrideSpecialEntrance(Entrance_GetOverride(grottoEntrance));
    }

    if (CVarGetInteger("gBetterDebugWarpScreen", 0)) {
        CVarSetInteger("gBetterDebugWarpScreenCurrentScene", this->currentScene);
        CVarSetInteger("gBetterDebugWarpScreenTopDisplayedScene", this->topDisplayedScene);
        CVarSetInteger("gBetterDebugWarpScreenPageDownIndex", this->pageDownIndex);
        CVarSave();
    }

    gSaveContext.respawnFlag = 0;
    gSaveContext.respawn[RESPAWN_MODE_DOWN].entranceIndex = -1;
    gSaveContext.seqId = (u8)NA_BGM_DISABLED;
    gSaveContext.natureAmbienceId = 0xFF;
    gSaveContext.showTitleCard = true;
    gWeatherMode = 0;
    this->state.running = false;
    SET_NEXT_GAMESTATE(&this->state, Play_Init, PlayState);
}

static SceneSelectEntry sScenes[] = {
    { " 1:SPOT00", " 1:Hyrule Field", " 1:Hylianische Steppe", " 1:Plaine d'Hyrule", Select_LoadGame, 0x00CD },
    { " 2:SPOT01", " 2:Kakariko Village", " 2:Kakariko", " 2:Village Cocorico", Select_LoadGame, 0x00DB },
    { " 3:SPOT02", " 3:Graveyard", " 3:Friedhof", " 3:Cimetiere", Select_LoadGame, 0x00E4 },
    { " 4:SPOT03", " 4:Zora's River", " 4:Zora-Fluß", " 4:Riviere Zora", Select_LoadGame, 0x00EA },
    { " 5:SPOT04", " 5:Kokiri Forest", " 5:Kokiri-Wald", " 5:Foret Kokiri", Select_LoadGame, 0x00EE },
    { " 6:SPOT05", " 6:Sacred Forest Meadow", " 6:Heilige Lichtung", " 6:Bosquet Sacre", Select_LoadGame, 0x00FC },
    { " 7:SPOT06", " 7:Lake Hylia", " 7:Hylia-See", " 7:Lac Hylia", Select_LoadGame, 0x0102 },
    { " 8:SPOT07", " 8:Zora's Domain", " 8:Zoras Reich", " 8:Domaine Zora", Select_LoadGame, 0x0108 },
    { " 9:SPOT08", " 9:Zora's Fountain", " 9:Zoras Quelle", " 9:Fontaine Zora", Select_LoadGame, 0x010E },
    { "10:SPOT09", "10:Gerudo Valley", "10:Gerudotal", "10:Vallee Gerudo", Select_LoadGame, 0x0117 },
    { "11:SPOT10", "11:Lost Woods", "11:Verlorene Walder", "11:Bois Perdus", Select_LoadGame, 0x011E },
    { "12:SPOT11", "12:Desert Colossus", "12:Wustenkoloss", "12:Colosse du Desert", Select_LoadGame, 0x0123 },
    { "13:SPOT12", "13:Gerudo's Fortress", "13:Gerudo-Festung", "13:Forteresse Gerudo", Select_LoadGame, 0x0129 },
    { "14:SPOT13", "14:Haunted Wasteland", "14:Gespensterwuste", "14:Desert Hante", Select_LoadGame, 0x0130 },
    { "15:SPOT15", "15:Hyrule Castle", "15:Schloß Hyrule", "15:Chateau d'Hyrule", Select_LoadGame, 0x0138 },
    { "16:SPOT16", "16:Death Mountain Trail", "16:Pfad zum Todesberg", "16:Chemin du Peril", Select_LoadGame, 0x013D },
    { "17:SPOT17", "17:Death Mountain Crater", "17:Todeskrater", "17:Cratere du Peril", Select_LoadGame, 0x0147 },
    { "18:SPOT18", "18:Goron City", "18:Goronia", "18:Village Goron", Select_LoadGame, 0x014D },
    { "19:SPOT20", "19:Lon Lon Ranch", "19:Lon Lon-Farm", "19:Ranch Lon Lon", Select_LoadGame, 0x0157 },
    { "20:" GFXP_HIRAGANA "ﾄｷﾉﾏ", "20:Temple Of Time", "20:Zitadelle der Zeit", "20:Temple du Temps", Select_LoadGame, 0x0053 },
    { "21:" GFXP_HIRAGANA "ｹﾝｼﾞｬﾉﾏ", "21:Chamber of Sages", "21:Halle der Weisen", "21:Sanctuaire des Sages", Select_LoadGame, 0x006B },
    { "22:" GFXP_HIRAGANA "ｼｬﾃｷｼﾞｮｳ", "22:Shooting Gallery", "22:Schießbude ", "22:Jeu d'adresse", Select_LoadGame, 0x003B },
    { "23:" GFXP_KATAKANA "ﾊｲﾗﾙ" GFXP_HIRAGANA "ﾆﾜ" GFXP_KATAKANA "ｹﾞｰﾑ", "23:Castle Courtyard Game", "23:Burghof", "23:Cour du Chateau (Infilration)", Select_LoadGame, 0x007A },
    { "24:" GFXP_HIRAGANA "ﾊｶｼﾀﾄﾋﾞｺﾐｱﾅ", "24:Grave 1", "24:Grave 1", "24:Tombe 1", Select_LoadGame, 0x031C },
    { "25:" GFXP_HIRAGANA "ﾊｶｼﾀﾄﾋﾞｺﾐｱﾅ 2", "25:Grave 2", "25:Grave 2", "25:Tombe 2", Select_LoadGame, 0x004B },
    { "26:" GFXP_HIRAGANA "ｵｳｹ ﾉ ﾊｶｱﾅ", "26:Royal Family's Tomb", "26:Royal Family's Tomb", "26:Tombe Royale", Select_LoadGame, 0x002D },
    { "27:" GFXP_HIRAGANA "ﾀﾞｲﾖｳｾｲﾉｲｽﾞﾐ", "27:Great Fairy's Fountain (Upgrades)", "27:Great Fairy's Fountain (Upgrades)", "27:Fontaine Royale des Fees (Amelioration)", Select_LoadGame, 0x0315 },
    { "28:" GFXP_HIRAGANA "ﾄﾋﾞｺﾐ ﾖｳｾｲ ｱﾅ", "28:Fairy's Fountain (Grotto)", "28:Fairy's Fountain (Grotto)", "28:Fontaines des Fees (Grotte)", Select_LoadGame, 0x036D },
    { "29:" GFXP_HIRAGANA "ﾏﾎｳｾｷ ﾖｳｾｲﾉｲｽﾞﾐ", "29:Great Fairy's Fountain (Magic)", "29:Great Fairy's Fountain (Magic)", "29:Fontaine Royale des Fees (Magie)", Select_LoadGame, 0x0371 },
    { "30:" GFXP_KATAKANA "ｶﾞﾉﾝ" GFXP_HIRAGANA "ｻｲｼｭｳｾﾝ", "30:Ganon's Tower - Collapsing", "30:Ganon's Tower - Collapsing", "30:Tour de Ganon - Effondrement", Select_LoadGame, 0x043F },
    { "31:" GFXP_KATAKANA "ﾊｲﾗﾙ" GFXP_HIRAGANA "ﾅｶﾆﾜ", "31:Castle Courtyard", "31:Castle Courtyard", "31:Cour du Chateau", Select_LoadGame, 0x0400 },
    { "32:" GFXP_HIRAGANA "ﾂﾘﾎﾞﾘ", "32:Fishing Pond", "32:Fishing Pond", "32:Etang", Select_LoadGame, 0x045F },
    { "33:" GFXP_KATAKANA "ﾎﾞﾑﾁｭｳﾎﾞｰﾘﾝｸﾞ", "33:Bombchu Bowling Alley", "33:Bombchu Bowling Alley", "33:Bowling Teigneux", Select_LoadGame, 0x0507 },
    { "34:" GFXP_KATAKANA "ﾛﾝﾛﾝ" GFXP_HIRAGANA "ﾎﾞｸｼﾞｮｳ ｿｳｺ 1", "34:Lon Lon Ranch House", "34:Lon Lon Ranch House", "34:Maison du Ranch Lon Lon", Select_LoadGame, 0x004F },
    { "35:" GFXP_KATAKANA "ﾛﾝﾛﾝ" GFXP_HIRAGANA "ﾎﾞｸｼﾞｮｳ ｿｳｺ 2", "35:Lon Lon Ranch Silo", "35:Lon Lon Ranch Silo", "35:Silo du Ranch Lon Lon", Select_LoadGame, 0x05D0 },
    { "36:" GFXP_HIRAGANA "ﾐﾊﾘ ｺﾞﾔ", "36:Guard House", "36:Guard House", "36:Maison de Garde", Select_LoadGame, 0x007E },
    { "37:" GFXP_HIRAGANA "ﾏﾎｳ ﾉ ｸｽﾘﾔ", "37:Potion Shop", "37:Potion Shop", "37:Apothicaire", Select_LoadGame, 0x0072 },
    { "38:" GFXP_HIRAGANA "ﾀｶﾗﾊﾞｺﾔ", "38:Treasure Chest Game", "38:Treasure Chest Game", "38:Chasse aux Tresors", Select_LoadGame, 0x0063 },
    { "39:" GFXP_HIRAGANA "ｷﾝ " GFXP_KATAKANA "ｽﾀﾙﾁｭﾗ ﾊｳｽ", "39:House Of Skulltula", "39:House Of Skulltula", "39:Maison des Skulltulas", Select_LoadGame, 0x0550 },
    { "40:" GFXP_HIRAGANA "ｼﾞｮｳｶﾏﾁ ｲﾘｸﾞﾁ", "40:Entrance to Market", "40:Entrance to Market", "40:Entree de la Place du Marche", Select_LoadGame, 0x0033 },
    { "41:" GFXP_HIRAGANA "ｼﾞｮｳｶﾏﾁ", "41:Market", "41:Market", "41:Place du Marche", Select_LoadGame, 0x00B1 },
    { "42:" GFXP_HIRAGANA "ｳﾗﾛｼﾞ", "42:Back Alley", "42:Back Alley", "42:Ruelle", Select_LoadGame, 0x00AD },
    { "43:" GFXP_HIRAGANA "ﾄｷﾉｼﾝﾃﾞﾝ ﾏｴ", "43:Temple of Time Exterior", "43:Temple of Time Exterior", "43:Exterieur du Temple du Temps", Select_LoadGame, 0x0171 },
    { "44:" GFXP_HIRAGANA "ﾘﾝｸﾉｲｴ", "44:Link's House", "44:Link's House", "44:Cabane de Link", Select_LoadGame, 0x00BB },
    { "45:" GFXP_KATAKANA "ｶｶﾘｺ" GFXP_HIRAGANA "ﾑﾗﾉﾅｶﾞﾔ", "45:Kakariko House 1", "45:Kakariko House 1", "45:Maison du Village Cocorico 1", Select_LoadGame, 0x02FD },
    { "46:" GFXP_HIRAGANA "ｳﾗﾛｼﾞﾉ ｲｴ", "46:Back Alley House 1", "46:Back Alley House 1", "46:Maison de la Ruelle 1", Select_LoadGame, 0x043B },
    { "47:" GFXP_HIRAGANA "ｺｷﾘﾉﾑﾗ ﾓﾉｼﾘｷｮｳﾀﾞｲﾉｲｴ", "47:House of the Know-it-All Brothers", "47:House of the Know-it-All Brothers", "47:Cabane des Freres Je-Sais-Tout", Select_LoadGame, 0x00C9 },
    { "48:" GFXP_HIRAGANA "ｺｷﾘﾉﾑﾗ ﾌﾀｺﾞﾉｲｴ", "48:House of Twins", "48:House of Twins", "48:Cabane des Jumeaux", Select_LoadGame, 0x009C },
    { "49:" GFXP_HIRAGANA "ｺｷﾘﾉﾑﾗ " GFXP_KATAKANA "ﾐﾄﾞ" GFXP_HIRAGANA "ﾉｲｴ", "49:Mido's House", "49:Mido's House", "49:Cabane du Grand Mido", Select_LoadGame, 0x0433 },
    { "50:" GFXP_HIRAGANA "ｺｷﾘﾉﾑﾗ " GFXP_KATAKANA "ｻﾘｱ" GFXP_HIRAGANA "ﾉｲｴ", "50:Saria's House", "50:Saria's House", "50:Cabane de Saria", Select_LoadGame, 0x0437 },
    { "51:" GFXP_HIRAGANA "ｳﾏｺﾞﾔ", "51:Stable", "51:Stable", "51:Etable", Select_LoadGame, 0x02F9 },
    { "52:" GFXP_HIRAGANA "ﾊｶﾓﾘﾉｲｴ", "52:Grave Keeper's Hut", "52:Grave Keeper's Hut", "52:Cabane du Fossoyeur", Select_LoadGame, 0x030D },
    { "53:" GFXP_HIRAGANA "ｳﾗﾛｼﾞ ｲﾇｵﾊﾞｻﾝﾉｲｴ", "53:Dog Lady's House", "53:Dog Lady's House", "53:Maison de la Dame du Chien", Select_LoadGame, 0x0398 },
    { "54:" GFXP_HIRAGANA "ｶｶﾘｺﾑﾗ " GFXP_KATAKANA "ｲﾝﾊﾟ" GFXP_HIRAGANA "ﾉｲｴ", "54:Impa's House", "54:Impa's House", "54:Maison d'Impa", Select_LoadGame, 0x039C },
    { "55:" GFXP_KATAKANA "ﾊｲﾘｱ" GFXP_HIRAGANA " ｹﾝｷｭｳｼﾞｮ", "55:Lakeside Laboratory", "55:Lakeside Laboratory", "55:Laboratoire du Lac", Select_LoadGame, 0x0043 },
    { "56:" GFXP_KATAKANA "ﾃﾝﾄ", "56:Running Man's Tent", "56:Running Man's Tent", "56:Tente du Marathonien", Select_LoadGame, 0x03A0 },
    { "57:" GFXP_HIRAGANA "ﾀﾃﾉﾐｾ", "57:Bazaar", "57:Bazaar", "57:Bazar", Select_LoadGame, 0x00B7 },
    { "58:" GFXP_HIRAGANA "ｺｷﾘｿﾞｸﾉﾐｾ", "58:Kokiri Shop", "58:Kokiri Shop", "58:Boutique Kokiri", Select_LoadGame, 0x00C1 },
    { "59:" GFXP_KATAKANA "ｺﾞﾛﾝ" GFXP_HIRAGANA "ﾉﾐｾ", "59:Goron Shop", "59:Goron Shop", "59:Boutique Goron", Select_LoadGame, 0x037C },
    { "60:" GFXP_KATAKANA "ｿﾞｰﾗ" GFXP_HIRAGANA "ﾉﾐｾ", "60:Zora Shop", "60:Zora Shop", "60:Boutique Zora", Select_LoadGame, 0x0380 },
    { "61:" GFXP_KATAKANA "ｶｶﾘｺ" GFXP_HIRAGANA "ﾑﾗ  ｸｽﾘﾔ", "61:Closed Shop", "61:Closed Shop", "61:Boutique Fermee", Select_LoadGame, 0x0384 },
    { "62:" GFXP_HIRAGANA "ｼﾞｮｳｶﾏﾁ ｸｽﾘﾔ", "62:Potion Shop", "62:Potion Shop", "62:Apothicaire (Boutique)", Select_LoadGame, 0x0388 },
    { "63:" GFXP_HIRAGANA "ｳﾗﾛｼﾞ ﾖﾙﾉﾐｾ", "63:Bombchu Shop", "63:Bombchu Shop", "63:Boutique de Missiles Teigneux", Select_LoadGame, 0x0390 },
    { "64:" GFXP_HIRAGANA "ｵﾒﾝﾔ", "64:Happy Mask Shop", "64:Happy Mask Shop", "64:Foire aux Masques", Select_LoadGame, 0x0530 },
    { "65:" GFXP_KATAKANA "ｹﾞﾙﾄﾞ" GFXP_HIRAGANA "ﾉｼｭｳﾚﾝｼﾞｮｳ", "65:Gerudo Training Ground", "65:Gerudo Training Ground", "65:Gymnase Gerudo", Select_LoadGame, 0x0008 },
    { "66:" GFXP_HIRAGANA "ﾖｳｾｲﾉｷﾉ " GFXP_KATAKANA "ﾀﾞﾝｼﾞｮﾝ", "66:Inside the Deku Tree", "66:Inside the Deku Tree", "66:Arbre Mojo", Select_LoadGame, 0x0000 },
    { "67:" GFXP_HIRAGANA "ﾖｳｾｲﾉｷﾉ " GFXP_KATAKANA "ﾀﾞﾝｼﾞｮﾝ ﾎﾞｽ", "67:Gohma's Lair", "67:Gohma's Lair", "67:Repaire de Gohma", Select_LoadGame, 0x040F },
    { "68:" GFXP_KATAKANA "ﾄﾞﾄﾞﾝｺﾞ ﾀﾞﾝｼﾞｮﾝ", "68:Dodongo's Cavern", "68:Dodongo's Cavern", "68:Caverne Dodongo", Select_LoadGame, 0x0004 },
    { "69:" GFXP_KATAKANA "ﾄﾞﾄﾞﾝｺﾞ ﾀﾞﾝｼﾞｮﾝ ﾎﾞｽ", "69:King Dodongo's Lair", "69:King Dodongo's Lair", "69:Repaire du Roi Dodongo", Select_LoadGame, 0x040B },
    { "70:" GFXP_HIRAGANA "ｷｮﾀﾞｲｷﾞｮ " GFXP_KATAKANA "ﾀﾞﾝｼﾞｮﾝ", "70:Inside Jabu-Jabu's Belly", "70:Inside Jabu-Jabu's Belly", "70:Ventre de Jabu-Jabu", Select_LoadGame, 0x0028 },
    { "71:" GFXP_HIRAGANA "ｷｮﾀﾞｲｷﾞｮ " GFXP_KATAKANA "ﾀﾞﾝｼﾞｮﾝ ﾎﾞｽ", "71:Barinade's Lair", "71:Barinade's Lair", "71:Repaire de Barinade", Select_LoadGame, 0x0301 },
    { "72:" GFXP_HIRAGANA "ﾓﾘﾉｼﾝﾃﾞﾝ", "72:Forest Temple", "72:Forest Temple", "72:Temple de la Foret", Select_LoadGame, 0x0169 },
    { "73:" GFXP_HIRAGANA "ﾓﾘﾉｼﾝﾃﾞﾝ " GFXP_KATAKANA "ﾎﾞｽ", "73:Phantom Ganon's Lair", "73:Phantom Ganon's Lair", "73:Repaire de Ganon Spectral", Select_LoadGame, 0x000C },
    { "74:" GFXP_HIRAGANA "ｲﾄﾞｼﾀ " GFXP_KATAKANA "ﾀﾞﾝｼﾞｮﾝ", "74:Bottom of the Well", "74:Bottom of the Well", "74:Puits", Select_LoadGame, 0x0098 },
    { "75:" GFXP_HIRAGANA "ﾊｶｼﾀ " GFXP_KATAKANA "ﾀﾞﾝｼﾞｮﾝ", "75:Shadow Temple", "75:Shadow Temple", "75:Temple de l'Ombre", Select_LoadGame, 0x0037 },
    { "76:" GFXP_HIRAGANA "ﾊｶｼﾀ " GFXP_KATAKANA "ﾀﾞﾝｼﾞｮﾝ ﾎﾞｽ", "76:Bongo Bongo's Lair", "76:Bongo Bongo's Lair", "76:Repaire de Bongo Bongo", Select_LoadGame, 0x0413 },
    { "77:" GFXP_HIRAGANA "ﾋﾉｼﾝﾃﾞﾝ", "77:Fire Temple", "77:Fire Temple", "77:Temple du Feu", Select_LoadGame, 0x0165 },
    { "78:" GFXP_HIRAGANA "ﾋﾉｼﾝﾃﾞﾝ " GFXP_KATAKANA "ﾎﾞｽ", "78:Volvagia's Lair", "78:Volvagia's Lair", "78:Repai de Volvagia", Select_LoadGame, 0x0305 },
    { "79:" GFXP_HIRAGANA "ﾐｽﾞﾉｼﾝﾃﾞﾝ", "79:Water Temple", "79:Water Temple", "79:Temple de l'Eau", Select_LoadGame, 0x0010 },
    { "80:" GFXP_HIRAGANA "ﾐｽﾞﾉｼﾝﾃﾞﾝ " GFXP_KATAKANA "ﾎﾞｽ", "80:Morpha's Lair", "80:Morpha's Lair", "80:Repaire de Morpha", Select_LoadGame, 0x0417 },
    { "81:" GFXP_HIRAGANA "ｼﾞｬｼﾝｿﾞｳ " GFXP_KATAKANA "ﾀﾞﾝｼﾞｮﾝ", "81:Spirit Temple", "81:Spirit Temple", "81:Temple de l'Esprit", Select_LoadGame, 0x0082 },
    { "82:" GFXP_HIRAGANA "ｼﾞｬｼﾝｿﾞｳ " GFXP_KATAKANA "ﾀﾞﾝｼﾞｮﾝ ｱｲｱﾝﾅｯｸ", "82:Iron Knuckle's Lair", "82:Iron Knuckle's Lair", "82:Repaire du Hache Viande", Select_LoadGame, 0x008D },
    { "83:" GFXP_HIRAGANA "ｼﾞｬｼﾝｿﾞｳ " GFXP_KATAKANA "ﾀﾞﾝｼﾞｮﾝ ﾎﾞｽ", "83:Twinrova's Lair", "83:Twinrova's Lair", "83:Repaire du Duo Malefique", Select_LoadGame, 0x05EC },
    { "84:" GFXP_KATAKANA "ｶﾞﾉﾝ" GFXP_HIRAGANA "ﾉﾄｳ", "84:Stairs to Ganondorf's Lair", "84:Stairs to Ganondorf's Lair", "84:Repaire de Ganondorf (Escaliers)", Select_LoadGame, 0x041B },
    { "85:" GFXP_KATAKANA "ｶﾞﾉﾝ" GFXP_HIRAGANA "ﾉﾄｳ" GFXP_KATAKANA "ﾎﾞｽ", "85:Ganondorf's Lair", "85:Ganondorf's Lair", "85:Repaire de Ganondorf", Select_LoadGame, 0x041F },
    { "86:" GFXP_HIRAGANA "ｺｵﾘﾉﾄﾞｳｸﾂ", "86:Ice Cavern", "86:Ice Cavern", "86:Caverne Polaire", Select_LoadGame, 0x0088 },
    { "87:" GFXP_HIRAGANA "ﾊｶｼﾀ" GFXP_KATAKANA "ﾘﾚｰ", "87:Dampe Grave Relay Game", "87:Dampe Grave Relay Game", "87:Dampe Grave Relay Game", Select_LoadGame, 0x044F },
    { "88:" GFXP_KATAKANA "ｶﾞﾉﾝ" GFXP_HIRAGANA "ﾁｶ " GFXP_KATAKANA "ﾀﾞﾝｼﾞｮﾝ", "88:Inside Ganon's Castle", "88:Inside Ganon's Castle", "88:Tour de Ganon", Select_LoadGame, 0x0467 },
    { "89:" GFXP_KATAKANA "ｶﾞﾉﾝ" GFXP_HIRAGANA "ｻｲｼｭｳｾﾝ " GFXP_KATAKANA "ﾃﾞﾓ & ﾊﾞﾄﾙ", "89:Ganon's Lair", "89:Ganon's Lair", "89:Repaire de Ganon", Select_LoadGame, 0x0517 },
    { "90:" GFXP_KATAKANA "ｶﾞﾉﾝ" GFXP_HIRAGANA "ﾉﾄｳ ｿﾉｺﾞ 1", "90:Escaping Ganon's Castle 1", "90:Escaping Ganon's Castle 1", "90:Fuite du Chateau de Ganon 1", Select_LoadGame, 0x0179 },
    { "91:" GFXP_KATAKANA "ｶﾞﾉﾝ" GFXP_HIRAGANA "ﾉﾄｳ ｿﾉｺﾞ 2", "91:Escaping Ganon's Castle 2", "91:Escaping Ganon's Castle 2", "91:Fuite du Chateau de Ganon 2", Select_LoadGame, 0x01B5 },
    { "92:" GFXP_KATAKANA "ｶﾞﾉﾝ" GFXP_HIRAGANA "ﾉﾄｳ ｿﾉｺﾞ 3", "92:Escaping Ganon's Castle 3", "92:Escaping Ganon's Castle 3", "92:Fuite du Chateau de Ganon 3", Select_LoadGame, 0x03DC },
    { "93:" GFXP_KATAKANA "ｶﾞﾉﾝ" GFXP_HIRAGANA "ﾉﾄｳ ｿﾉｺﾞ 4", "93:Escaping Ganon's Castle 4", "93:Escaping Ganon's Castle 4", "93:Fuite du Chateau de Ganon 4", Select_LoadGame, 0x03E4 },
    { "94:" GFXP_KATAKANA "ｶﾞﾉﾝ" GFXP_HIRAGANA "ﾁｶ ｿﾉｺﾞ", "94:Escaping Ganon's Castle 5", "94:Escaping Ganon's Castle 5", "94:Fuite du Chateau de Ganon 5", Select_LoadGame, 0x056C },
    { "95:" GFXP_KATAKANA "ｹﾞﾙﾄﾞ" GFXP_HIRAGANA "ﾂｳﾛ 1-2", "95:Thieves' Hideout 1-2", "95:Thieves' Hideout 1-2", "95:Repaire des Voleurs 1-2", Select_LoadGame, 0x0486 },
    { "96:" GFXP_KATAKANA "ｹﾞﾙﾄﾞ" GFXP_HIRAGANA "ﾂｳﾛ 3-4 9-10", "96:Thieves' Hideout 3-4 9-10", "96:Thieves' Hideout 3-4 9-10", "96:Repaire des Voleurs 3-4 9-10", Select_LoadGame, 0x048E },
    { "97:" GFXP_KATAKANA "ｹﾞﾙﾄﾞ" GFXP_HIRAGANA "ﾂｳﾛ 5-6", "97:Thieves' Hideout 5-6", "97:Thieves' Hideout 5-6", "97:Repaire des Voleurs 5-6", Select_LoadGame, 0x0496 },
    { "98:" GFXP_KATAKANA "ｹﾞﾙﾄﾞ" GFXP_HIRAGANA "ﾂｳﾛ 7-8", "98:Thieves' Hideout 7-8", "98:Thieves' Hideout 7-8", "98:Repaire des Voleurs 7-8", Select_LoadGame, 0x049E },
    { "99:" GFXP_KATAKANA "ｹﾞﾙﾄﾞ" GFXP_HIRAGANA "ﾂｳﾛ 11-12", "99:Thieves' Hideout 11-12", "99:Thieves' Hideout 11-12", "99:Repaire des Voleurs 11-12", Select_LoadGame, 0x04AE },
    { "100:" GFXP_KATAKANA "ｹﾞﾙﾄﾞ" GFXP_HIRAGANA "ﾂｳﾛ 13", "100:Thieves' Hideout 13", "100:Thieves' Hideout 13", "100:Repaire des Voleurs 13", Select_LoadGame, 0x0570 },
    { "101:" GFXP_HIRAGANA "ｶｸｼﾄﾋﾞｺﾐｱﾅ 0", "101:Grotto 0", "101:Grotto 0", "101:Grotte 0", Select_LoadGame, 0x003F },
    { "102:" GFXP_HIRAGANA "ｶｸｼﾄﾋﾞｺﾐｱﾅ 1", "102:Grotto 1", "102:Grotto 1", "102:Grotte 1", Select_LoadGame, 0x0598 },
    { "103:" GFXP_HIRAGANA "ｶｸｼﾄﾋﾞｺﾐｱﾅ 2", "103:Grotto 2", "103:Grotto 2", "103:Grotte 2", Select_LoadGame, 0x059C },
    { "104:" GFXP_HIRAGANA "ｶｸｼﾄﾋﾞｺﾐｱﾅ 3", "104:Grotto 3", "104:Grotto 3", "104:Grotte 3", Select_LoadGame, 0x05A0 },
    { "105:" GFXP_HIRAGANA "ｶｸｼﾄﾋﾞｺﾐｱﾅ 4", "105:Grotto 4", "105:Grotto 4", "105:Grotte 4", Select_LoadGame, 0x05A4 },
    { "106:" GFXP_HIRAGANA "ｶｸｼﾄﾋﾞｺﾐｱﾅ 5", "106:Grotto 5", "106:Grotto 5", "106:Grotte 5", Select_LoadGame, 0x05A8 },
    { "107:" GFXP_HIRAGANA "ｶｸｼﾄﾋﾞｺﾐｱﾅ 6", "107:Grotto 6", "107:Grotto 6", "107:Grotte 6", Select_LoadGame, 0x05AC },
    { "108:" GFXP_HIRAGANA "ｶｸｼﾄﾋﾞｺﾐｱﾅ 7", "108:Grotto 7", "108:Grotto 7", "108:Grotte 7", Select_LoadGame, 0x05B0 },
    { "109:" GFXP_HIRAGANA "ｶｸｼﾄﾋﾞｺﾐｱﾅ 8", "109:Grotto 8", "109:Grotto 8", "109:Grotte 8", Select_LoadGame, 0x05B4 },
    { "110:" GFXP_HIRAGANA "ｶｸｼﾄﾋﾞｺﾐｱﾅ 9", "110:Grotto 9", "110:Grotto 9", "110:Grotte 9", Select_LoadGame, 0x05B8 },
    { "111:" GFXP_HIRAGANA "ｶｸｼﾄﾋﾞｺﾐｱﾅ 10", "111:Grotto 10", "111:Grotto 10", "111:Grotte 10", Select_LoadGame, 0x05BC },
    { "112:" GFXP_HIRAGANA "ｶｸｼﾄﾋﾞｺﾐｱﾅ 11", "112:Grotto 11", "112:Grotto 11", "112:Grotte 11", Select_LoadGame, 0x05C0 },
    { "113:" GFXP_HIRAGANA "ｶｸｼﾄﾋﾞｺﾐｱﾅ 12", "113:Grotto 12", "113:Grotto 12", "113:Grotte 12", Select_LoadGame, 0x05C4 },
    { "114:" GFXP_HIRAGANA "ｶｸｼﾄﾋﾞｺﾐｱﾅ 13", "114:Grotto 13", "114:Grotto 13", "114:Grotte 13", Select_LoadGame, 0x05FC },
    { "115:" GFXP_KATAKANA "ﾊｲﾗﾙ ﾃﾞﾓ", "115:Goddess Cutscene Environment", "115:Goddess Cutscene Environment", "115:Goddess Cutscene Environment", Select_LoadGame, 0x00A0 },
    { "116:" GFXP_HIRAGANA "ﾍﾞｯｼﾂ (ﾀｶﾗﾊﾞｺ" GFXP_KATAKANA "ﾜｰﾌﾟ)", "116:Test Room", "116:Test Room", "116:Salle de Test", Select_LoadGame, 0x0520 },
    { "117:" GFXP_HIRAGANA "ｻｻ" GFXP_KATAKANA "ﾃｽﾄ", "117:SRD Map", "117:SRD Map", "117:Carte SRD", Select_LoadGame, 0x0018 },
    { "118:" GFXP_KATAKANA "ﾃｽﾄﾏｯﾌﾟ", "118:Test Map", "118:Test Map", "118:Carte de Test", Select_LoadGame, 0x0094 },
    { "119:" GFXP_KATAKANA "ﾃｽﾄﾙｰﾑ", "119:Treasure Chest Warp", "119:Treasure Chest Warp", "119:Treasure Chest Warp", Select_LoadGame, 0x0024 },
    { "120:" GFXP_HIRAGANA "ﾁｭｳ" GFXP_KATAKANA "ｽﾀﾛﾌｫｽ" GFXP_HIRAGANA "ﾍﾞﾔ", "120:Stalfos Miniboss Room", "120:Stalfos Miniboss Room", "120:Stalfos Miniboss Room", Select_LoadGame, 0x001C },
    { "121:" GFXP_KATAKANA "ﾎﾞｽｽﾀﾛﾌｫｽ" GFXP_HIRAGANA "ﾍﾞﾔ", "121:Stalfos Boss Room", "121:Stalfos Boss Room", "121:Stalfos Boss Room", Select_LoadGame, 0x0020 },
    { "122:Sutaru", "122:Dark Link Room", "122:Dark Link Room", "122:Dark Link Room", Select_LoadGame, 0x0047 },
    { "123:jikkenjyou", "123:Shooting Gallery Duplicate", "123:Shooting Gallery Duplicate", "123:Jeu d'adresse (Duplicatas)", Select_LoadGame, 0x02EA },
    { "124:depth" GFXP_KATAKANA "ﾃｽﾄ", "124:depth test", "124:depth test", "124:Test de Profondeur", Select_LoadGame, 0x00B6 },
    { "125:" GFXP_KATAKANA "ﾊｲﾗﾙ" GFXP_HIRAGANA "ﾆﾜ" GFXP_KATAKANA "ｹﾞｰﾑ2", "125:Hyrule Garden Game (Broken)", "125:Hyrule Garden Game (Broken)", "125:Hyrule Garden Game (Broken)", Select_LoadGame, 0x0076 },
    { "title", "title", "title", "Ecran-titre", Select_LoadTitle, 0x0000 },
};

static BetterSceneSelectEntry sBetterScenes[] = {
    { " 1:Hyrule Field", " 1:Hyrule Field", " 1:Plaine d'Hyrule", Select_LoadGame, 8, { 
        { "Near Drawbridge", "Near Drawbridge", "Pres du Pont-levis", 0x00CD },
        { "From Drawbridge", "From Drawbridge", "Depuis le Pont-levis", 0x01FD },
        { "From Kakariko Village", "From Kakariko Village", "Depuis le Village Cocorico", 0x017D },
        { "From Zora River", "From Zora River", "Depuis la Riviere Zora", 0x0181 },
        { "From Lost Woods", "From Lost Woods", "Depuis les Bois Perdus", 0x0185 },
        { "From Lake Hylia", "From Lake Hylia", "Depuis le Lac Hylia", 0x0189 },
        { "From Gerudo Valley", "From Gerudo Valley", "Depuis la Vallee Gerudo", 0x018D },
        { "From Lon Lon Ranch", "From Lon Lon Ranch", "Depuis le Ranch Lon Lon", 0x01F9 },
    }},
    { " 2:Kokiri Forest", " 2:Kokiri Forest", " 2:Foret Kokiri", Select_LoadGame, 9, {
        { "From Links House", "From Links House", "Depuis la Cabane de Link", 0x0211 },
        { "From Bridge", "From Bridge", "Depuis le Pont", 0x020D },
        { "From Lost Woods", "From Lost Woods", "Depuis les Bois Perdus", 0x0286 },
        { "From Deku Tree", "From Deku Tree", "Depuis l'Arbre Mojo", 0x0209 },
        { "From Kokiri Shop", "From Kokiri Shop", "Depuis la Boutique Kokiri", 0x0266 },
        { "From Know-It-All Brothers House", "From Know-It-All Brothers House", "Depuis la Cabane des Freres Je-Sais-Tout", 0x026A },
        { "From Twins House", "From Twins House", "Depuis la Cabane des Jumeaux", 0x033C },
        { "From Midos House", "From Midos House", "Depuis la Cabane du Grand Mido", 0x0443 },
        { "From Sarias House", "From Sarias House", "Depuis la Cabane de Saria", 0x0447 },
    }},
    { " 3:Kokiri Buildings", " 3:Kokiri Buildings", " 3:Cabanes des Kokiris", Select_LoadGame, 6, {
        { "Links Bed", "Links Bed", "Lit de Link", 0x00BB },
        { "Kokiri Shop", "Kokiri Shop", "Boutique Kokiri", 0x00C1 },
        { "Twins House", "Twins House", "Cabane des Jumeaux", 0x009C },
        { "Know-It-All Brothers House", "Know-It-All Brothers House", "Cabane des Freres Je-Sais-Tout", 0x00C9 },
        { "Midos House", "Midos House", "Cabane du Grand Mido", 0x0433 },
        { "Sarias House", "Sarias House", "Cabane de Sara", 0x0437 },
    }},
    { " 4:Lost Woods", " 4:Lost Woods", " 4:Bois Perdus", Select_LoadGame, 5, { 
        { "From Kokiri Forest", "From Kokiri Forest", "Depuis la Foret Kokiri", 0x011E },
        { "From Sacred Meadow", "From Sacred Meadow", "Depuis le Bosquet Sacre", 0x01A9 },
        { "From Goron City", "From Goron City", "Depuis le Village Goron", 0x04D6 },
        { "From Zora River", "From Zora River", "Depuis la Riviere Zora", 0x04DA },
        { "Bridge", "Bridge", "Pont", 0x05E0 },
    }},
    { " 5:Sacred Forest Meadow", " 5:Sacred Forest Meadow", " 5:Bosquet Sacre", Select_LoadGame, 3, { 
        { "From Lost Woods", "From Lost Woods", "Depuis les Bois Perdus", 0x00FC },
        { "From Forest Temple", "From Forest Temple", "Depuis le Temple de la Foret", 0x0215 },
        { "Minuet of Forest Warp", "Minuet of Forest Warp", "Teleporteur du Menuet des Bois", 0x0600 },
    }},
    { " 6:Castle Town Entrance", " 6:Castle Town Entrance", " 6:Entree du Bourg d'Hyrule", Select_LoadGame, 3, {
        { "From Hyrule Field", "From Hyrule Field", "Depuis la Plaine d'Hyrule", 0x0276 },
        { "From Market", "From Market", "Depuis la Place du Marche", 0x0033 },
        { "From Pot House", "From Pot House", "Depuis la Maison des Jarres", 0x026E },
    }},
    { " 7:Market", " 7:Market", " 7:Place du Marche", Select_LoadGame, 11, {
        { "From Castle Town Entrance", "From Castle Town Entrance", "Depuis l'Entree du Bourg d'Hyrule", 0x00B1 },
        { "From Shooting Gallery", "From Shooting Gallery", "Depuis le Jeu d'adresse", 0x01CD },
        { "From Happy Mask Shop", "From Happy Mask Shop", "Depuis la Foire aux Masques", 0x01D1 },
        { "From Treasure Box Minigame", "From Treasure Box Minigame", "Depuis le Bowling Teigneux", 0x01D5 },
        { "From Castle", "From Castle", "Depuis le Chateau d'Hyrule", 0x025A },
        { "From Temple of Time", "From Temple of Time", "Depuis le Temple du Temps", 0x025E },
        { "From Back Alley (Right)", "From Back Alley (Right)", "Depuis la Ruelle (Droite)", 0x0262 },
        { "From Back Alley (Left)", "From Back Alley (Left)", "Depuis la Ruelle (Gauche)", 0x029E },
        { "From Potion Shop", "From Potion Shop", "Depuis l'Apothicaire", 0x02A2 },
        { "From Bazaar Shop", "From Bazaar Shop", "Depuis le Bazar", 0x03B8 },
        { "From Bomchu Bowling Minigame", "From Bomchu Bowling Minigame", "Depuis le Bowling Teigneux", 0x03BC },
    }},
    { " 8:Castle Town Alley", " 8:Castle Town Alley", " 8:Ruelle du Bourg d'Hyrule", Select_LoadGame, 5, { 
        { "From Market (Right)", "From Market (Right)", "Depuis la Place du Marche (Droite)", 0x00AD },
        { "From Market (Left)", "From Market (Left)", "Depuis la Place du Marche (Gauche)", 0x029A }, 
        { "From Alley House", "From Alley House", "Depuis la Maison de la Ruelle", 0x0067 },
        { "From Dog House", "From Dog House", "Depuis la Maison du Chien", 0x038C }, 
        { "From Bombchu Shop", "From Bombchu Shop", "Depuis le Magasin de Missiles", 0x03C0 },
    }},
    { " 9:Castle Town Buildings", " 9:Castle Town Buildings", " 9:Batiments du Bourg d'Hyrule", Select_LoadGame, 10, { 
        { "Pot House", "Pot House", "Maison des Jarres", 0x007E },
        { "Shooting Gallery Minigame", "Shooting Gallery Minigame", "Jeu d'adresse", 0x016D },
        { "Treasure Box Minigame", "Treasure Box Minigame", "Chasse aux Tresors", 0x0063 },
        { "Potion Shop", "Potion Shop", "Apothicaire", 0x0388 },
        { "Bombchu Bowling Minigame", "Bombchu Bowling Minigame", "Bowling Teigneux", 0x0507 },
        { "Bazaar Shop", "Bazaar Shop", "Bazar", 0x052C },
        { "Happy Mask Shop", "Happy Mask Shop", "Foire aux Masques", 0x0530 },
        { "Bombchu Shop", "Bombchu Shop", "Boutique de Missiles", 0x0528 },
        { "Dog House", "Dog House", "Maison du Chien", 0x0398 },
        { "Alley House", "Alley House", "Maison de la Ruelle", 0x043B },
    }},
    { "10:Temple of Time", "10:Temple of Time", "10:Temple du Temps", Select_LoadGame, 5, { 
        { "From Outside", "From Outside", "Depuis l'Entree", 0x053 },
        { "From Master Sword Pedestal", "From Master Sword Pedestal", "Depuis le Piedestal de l'Epee de Legende", 0x02CA },
        { "Prelude of Light Warp", "Prelude of Light Warp", "Teleporteur du Prelude de la Lumiere", 0x05F4 },
        { "Outside Temple of Time - From Market", "Outside Temple of Time - From Market", "Exterieur du Temple - Depuis la Place du Marche", 0x0171 },
        { "Outside Temple of Time - From Temple of Time", "Outside Temple of Time - From Temple of Time", "Exterieur du Temple - Depuis le Temple", 0x0472 },
    }},
    { "11:Hyrule Castle", "11:Hyrule Castle", "11:Chateau d'Hyrule", Select_LoadGame, 5, {
        { "From Market", "From Market", "Depuis la Place du Marche", 0x0138 },
        { "From Castle Courtyard", "From Castle Courtyard", "Depuis la Cour du Chateau", 0x023D },
        { "From Great Fairy", "From Great Fairy", "Depuis la Grande Fee", 0x0340 },
        { "From Courtyard Guard Capture", "From Courtyard Guard Capture", "Depuis la capture d'un Garde de la Cour", 0x04FA },
        { "Great Fairy", "Great Fairy", "Great Fairy", 0x04C2 },
    }},
    { "12:Hyrule Castle Courtyard", "12:Hyrule Castle Courtyard", "12:Cour du Chateau", Select_LoadGame, 3, {
        { "From Crawlspace", "From Crawlspace", "Depuis l'Entree", 0x007A },
        { "From Zelda", "From Zelda", "Depuis Zelda", 0x0296 },
        { "Zeldas Courtyard", "Zeldas Courtyard", "Depuis la Cour de Zelda", 0x0400 },
    }},
    { "13:Lon Lon Ranch", "13:Lon Lon Ranch", "13:Ranch Lon Lon", Select_LoadGame, 4, { 
        { "From Hyrule Field", "From Hyrule Field", "Depuis la Plaine d'Hyrule", 0x0157 },
        { "From Ranch House", "From Ranch House", "Depuis la Maison du Ranch", 0x0378 },
        { "From Stables", "From Stables", "Depuis l'Etable", 0x042F },
        { "Epona Song Cutscene", "Epona Song Cutscene", "Cinematique du Chant d'Epona", 0x02AE },
    }},
    { "14:Lon Lon Ranch Buildings", "14:Lon Lon Ranch Buildings", "14:Batiments du Ranch Lon Lon", Select_LoadGame, 3, {
        { "Ranch House", "Ranch House", "Maison du Ranch", 0x004F },
        { "Stables", "Stables", "Etable du Ranch", 0x02F9 },
        { "Back Tower", "Back Tower", "Silo du Ranch", 0x05D0 },
    }},
    { "15:Kakariko Village", "15:Kakariko Village", "15:Village Cocorico", Select_LoadGame, 15, {
        { "From Hyrule Field", "From Hyrule Field", "Depuis la Plaine d'Hyrule", 0x00DB },
        { "From Death Mountain", "From Death Mountain", "Depuis le Mont du Peril", 0x0191 },
        { "From Graveyard", "From Graveyard", "Depuis le Cimetiere", 0x0195 },
        { "From Bazaar", "From Bazaar", "Depuis le Bazar", 0x0201 },
        { "From Bottom of Well", "From Bottom of Well", "Depuis le Puits", 0x02A6 },
        { "From Boss House", "From Boss House", "Depuis la Maison du Boss", 0x0349 },
        { "From Potion Shop", "From Potion Shop", "Depuis l'Apothicaire", 0x044B },
        { "From Potion Shop (Back Entrance)", "From Potion Shop (Back Entrance)", "Depuis l'Apothicaire (Entree Arriere)", 0x04FF },
        { "From Grannys Potion Shop", "From Grannys Potion Shop", "Depuis l'Apothicaire (Vieille Femme)", 0x034D },
        { "From Impas House", "From Impas House", "Depuis la Maison d'Impa", 0x0345 },
        { "From Impas House (Cow)", "From Impas House (Cow)", "Depuis la Maison d'Impa (Vache)", 0x05DC },
        { "From Windmill", "From Windmill", "Depuis le Moulin", 0x0351 },
        { "From Shooting Gallery", "From Shooting Gallery", "Depuis le Jeu d'adresse", 0x0463 },
        { "From Skulltula House", "From Skulltula House", "Depuis la Maison des Skulltulas", 0x04EE },
        { "Owl Drop Spot from Death Mountain", "Owl Drop Spot from Death Mountain", "Point de chute du Hibou depuis le Mont du Peril", 0x0554 },
    }},
    { "16:Kakariko Buildings", "16:Kakariko Buildings", "16:Batiments du Village Cocorico", Select_LoadGame, 9, {
        { "Shooting Gallery Minigame", "Shooting Gallery Minigame", "Jeu d'adresse", 0x003B },
        { "Grannys Potion Shop", "Grannys Potion Shop", "Apothicaire (Vieille Femme)", 0x0072 },
        { "Bazaar Shop", "Bazaar Shop", "Bazar", 0x00B7 },
        { "Potion Shop", "Potion Shop", "Apothicaire", 0x0384 },
        { "Impas House", "Impas House", "Maison d'Impa", 0x039C },
        { "Impas House (Near Cow)", "Impas House (Near Cow)", "Maison d'Impa (Vache)", 0x05C8 },
        { "Boss House", "Boss House", "Maison du Boss", 0x02FD },
        { "Windmill", "Windmill", "Moulin", 0x0453 },
        { "Skulltula House", "Skulltula House", "Maison des SKulltulas", 0x0550 },
    }}, 
    { "17:Graveyard", "17:Graveyard", "17:Cimetiere", Select_LoadGame, 9, {
        { "From Kakariko", "From Kakariko", "Depuis l'Apothicaire", 0x00E4 },
        { "From Shadow Temple", "From Shadow Temple", "Depuis le Temple de l'Ombre", 0x0205 },
        { "From Gravekeepers Hut", "From Gravekeepers Hut", "Depuis la Cabane du Fossoyeur", 0x0355 },
        { "From Dampes Grave", "From Dampes Grave", "Depuis la Tombe d'Igor", 0x0359 },
        { "From Shield Grave", "From Shield Grave", "Depuis la Tombe au Bouclier", 0x035D },
        { "From Redead Grave", "From Redead Grave", "Depuis la Tombe au Effrois", 0x0361 },
        { "From Royal Familys Tomb", "From Royal Familys Tomb", "Depuis la Tombe Royale", 0x050B },
        { "Inside Dampe's Hut", "Inside Dampe's Hut", "A l'interieur de la Cabane du Fossoyeur", 0x030D },
        { "Nocturne of Shadow Warp", "Nocturne of Shadow Warp", "Teleporteur du Nocturne de l'Ombre", 0x0568 },
    }},
    { "18:Graves", "18:Graves", "18:Tombes", Select_LoadGame, 5, {
        { "Dampes Grave Minigame", "Dampes Grave Minigame", "Tour du Cimetiere d'Igor", 0x044F },
        { "Royal Familys Tomb", "Royal Familys Tomb", "Tombe Royale", 0x002D },
        { "Royal Familys Tomb, Suns Song Cutscene", "Royal Familys Tomb, Suns Song Cutscene", "Tombe Royale, Cinematique du Chant du Soleil", 0x0574 },
        { "Treasure Chest Grave", "Treasure Chest Grave", "Tombe au Coffre", 0x004B },
        { "ReDead Grave", "ReDead Grave", "Tombe au Effrois", 0x031C },
    }},
    { "19:Death Mountain Trail", "19:Death Mountain Trail", "19:Mont du Peril", Select_LoadGame, 6, {
        { "From Kakariko Village", "From Kakariko Village", "Depuis le Village Cocorico", 0x013D },
        { "From Goron City", "From Goron City", "Depuis le Village Goron", 0x01B9 },
        { "From Death Mountain Crater", "From Death Mountain Crater", "Depuis le Cratere du Peril", 0x01BD },
        { "From Dodongos Cavern", "From Dodongos Cavern", "Depuis la Caverne Dodongo", 0x0242 },
        { "From Great Fairy", "From Great Fairy", "Depuis la Grande Fee", 0x045B },
        { "Great Fairy", "Great Fairy", "Grande Fee", 0x0315 },
    }},
    { "20:Goron City", "20:Goron City", "20:Village Goron", Select_LoadGame, 5, {
        { "From Death Mountain Trail", "From Death Mountain Trail", "Depuis le Mont du Peril", 0x014D },
        { "From Death Mountain Crater", "From Death Mountain Crater", "Depuis le Cratere du Peril", 0x01C1 },
        { "From Goron City Shop", "From Goron City Shop", "Depuis la Boutique Goron", 0x03FC },
        { "From Lost Woods", "From Lost Woods", "Depuis les Bois Perdus", 0x04E2 },
        { "Goron City Shop", "Goron City Shop", "Boutique Goron", 0x037C },
    }},
    { "21:Death Mountain Crater", "21:Death Mountain Crater", "21:Cratere du Peril", Select_LoadGame, 6, {
        { "From Death Mountain Trail", "From Death Mountain Trail", "Depuis le Mont du Peril", 0x0147 },
        { "From Goron City", "From Goron City", "Depuis le Village Goron", 0x0246 },
        { "From Fire Temple", "From Fire Temple", "Depuis le Temple du Feu", 0x024A },
        { "From Fairy Fountain", "From Fairy Fountain", "Depuis la Fontaine des Fees", 0x0482 },
        { "Great Fairy", "Great Fairy", "Depuis la Grande Fee", 0x04BE },
        { "Bolero of Fire Warp", "Bolero of Fire Warp", "Teleporteur du Bolero du Feu", 0x04F6 },
    }},
    { "22:Zora River", "22:Zora River", "22:Riviere Zora", Select_LoadGame, 3, {
        { "From Hyrule Field", "From Hyrule Field", "Depuis la Plaine d'Hyrule", 0x00EA },
        { "From Zoras Domain", "From Zoras Domain", "Depuis le Domaine Zora", 0x019D },
        { "From Lost Woods", "From Lost Woods", "Depuis les Bois Perdus", 0x01DD },
    }},
    { "23:Zoras Domain", "23:Zoras Domain", "23:Domaine Zora", Select_LoadGame, 5, {
        { "From Zora River", "From Zora River", "Depuis la Riviere Zora", 0x0108 },
        { "From Zoras Fountain", "From Zoras Fountain", "Depuis la Fontaine Zora", 0x01A1 },
        { "From Lake Hylia", "From Lake Hylia", "Depuis le Lac Hylia", 0x0328 },
        { "From Zora Shop", "From Zora Shop", "Depuis la Boutique Zora", 0x03C4 },
        { "Zora Shop", "Zora Shop", "Boutique Zora", 0x0380 },
    }},
    { "24:Zoras Fountain", "24:Zoras Fountain", "24:Fontaine Zora", Select_LoadGame, 5, {
        { "From Zoras Domain", "From Zoras Domain", "Depuis le Domaine Zora", 0x0225 },
        { "From Jabu Jabu", "From Jabu Jabu", "Depuis Jabu-Jabu", 0x0221 },
        { "From Ice Cavern", "From Ice Cavern", "Depuis la Caverne Polaire", 0x03D4 },
        { "From Fairy Fountain", "From Fairy Fountain", "Depuis la Fontaine des Fees", 0x0394 },
        { "Great Fairy", "Great Fairy", "Grande Fee", 0x0371 },
    }},
    { "25:Lake Hylia", "25:Lake Hylia", "25:Lac Hylia", Select_LoadGame, 7, {
        { "From Hyrule Field", "From Hyrule Field", "Depuis la Plaine d'Hyrule", 0x0102 },
        { "From Gerudo Valley", "From Gerudo Valley", "Depuis la Vallee Gerudo", 0x0219 },
        { "From Water Temple", "From Water Temple", "Depuis le Temple de l'Eau", 0x021D },
        { "From Fishing Pond", "From Fishing Pond", "Depuis l'Etang", 0x0309 },
        { "From Laboratory", "From Laboratory", "Depuis le Laboratoire du Lac", 0x03CC },
        { "From Zoras Domain", "From Zoras Domain", "Depuis le Domaine Zora", 0x0560 },
        { "Serenade Of Water Warp", "Serenade Of Water Warp", "Teleporteur de la Serenade de l'Eau", 0x0604 },
    }},
    { "26:Lake Hylia Buildings", "26:Lake Hylia Buildings", "26:Batiments du Lac Hylia", Select_LoadGame, 2, { 
        { "Laboratory", "Laboratory", "Laboratoire du Lac", 0x0043 }, 
        { "Fishing Pond Minigame", "Fishing Pond Minigame", "Etang", 0x045F },
    }},
    { "27:Gerudo Valley", "27:Gerudo Valley", "27:Vallee Gerudo", Select_LoadGame, 5, {
        { "From Hyrule Field", "From Hyrule Field", "Depuis la Plaine d'Hyrule", 0x0117 },
        { "From Gerudo Fortress", "From Gerudo Fortress", "Depuis la Forteresse Gerudo", 0x022D },
        { "From Carpenter's Tent", "From Carpenter's Tent", "Depuis la Tente du Charpentier", 0x03D0 },
        { "Carpenter's Tent/ Running Man Minigame", "Carpenter's Tent/ Running Man Minigame", "Tente du Charpentier/ Marathonien", 0x03A0 },
        { "Thrown out of Fortress", "Thrown out of Fortress", "Expulsé de la Forteresse", 0x01A5 },
    }},
    { "28:Gerudo Fortress", "28:Gerudo Fortress", "28:Forteresse Gerudo", Select_LoadGame, 18, {
        { "From Gerudo Valley", "From Gerudo Valley", "Depuis la Vallee Gerudo", 0x0129 },
        { "From Traning Grounds", "From Traning Grounds", "Depuis le Gymnase Gerudo", 0x03A8 },
        { "From Haunted Wasteland", "From Haunted Wasteland", "Depuis le Desert Hante", 0x03AC },
        { "Horseback Riding Minigame", "Horseback Riding Minigame", "Archerie Montee", 0x03B0 },
        { "Gerudo Fortress Jail", "Gerudo Fortress Jail", "Prison de la Forteresse Gerudo", 0x03B4 },
        { "From Thieves Hideout (1)", "From Thieves Hideout (1)", "Depuis le Repaire des Voleurs (1)", 0x0231 },
        { "From Thieves Hideout (2)", "From Thieves Hideout (2)", "Depuis le Repaire des Voleurs (2)", 0x0235 },
        { "From Thieves Hideout (3)", "From Thieves Hideout (3)", "Depuis le Repaire des Voleurs (3)", 0x0239 },
        { "From Thieves Hideout (4)", "From Thieves Hideout (4)", "Depuis le Repaire des Voleurs (4)", 0x02AA },
        { "From Thieves Hideout (5)", "From Thieves Hideout (5)", "Depuis le Repaire des Voleurs (5)", 0x02BA },
        { "From Thieves Hideout (6)", "From Thieves Hideout (6)", "Depuis le Repaire des Voleurs (6)", 0x02BE },
        { "From Thieves Hideout (7)", "From Thieves Hideout (7)", "Depuis le Repaire des Voleurs (7)", 0x02C2 },
        { "From Thieves Hideout (8)", "From Thieves Hideout (8)", "Depuis le Repaire des Voleurs (8)", 0x02C6 },
        { "From Thieves Hideout (9)", "From Thieves Hideout (9)", "Depuis le Repaire des Voleurs (9)", 0x02D2 },
        { "From Thieves Hideout (10)", "From Thieves Hideout (10)", "Depuis le Repaire des Voleurs (10)", 0x02D6 },
        { "From Thieves Hideout (11)", "From Thieves Hideout (11)", "Depuis le Repaire des Voleurs (11)", 0x02DA },
        { "From Thieves Hideout (12)", "From Thieves Hideout (12)", "Depuis le Repaire des Voleurs (12)", 0x02DE },
        { "From Thieves Hideout (13)", "From Thieves Hideout (13)", "Depuis le Repaire des Voleurs (13)", 0x03A4 },
    }},
    { "29:Thieves Hideout", "29:Thieves Hideout", "29:Repaire des Voleurs", Select_LoadGame, 13, {
        { "From Gerudo Fortress (1)", "From Gerudo Fortress (1)", "Depuis la Forteresse Gerudo (1)", 0x0486 },
        { "From Gerudo Fortress (2)", "From Gerudo Fortress (2)", "Depuis la Forteresse Gerudo (2)", 0x048A },
        { "From Gerudo Fortress (3)", "From Gerudo Fortress (3)", "Depuis la Forteresse Gerudo (3)", 0x048E },
        { "From Gerudo Fortress (4)", "From Gerudo Fortress (4)", "Depuis la Forteresse Gerudo (4)", 0x0492 },
        { "From Gerudo Fortress (5)", "From Gerudo Fortress (5)", "Depuis la Forteresse Gerudo (5)", 0x0496 },
        { "From Gerudo Fortress (6)", "From Gerudo Fortress (6)", "Depuis la Forteresse Gerudo (6)", 0x049A },
        { "From Gerudo Fortress (7)", "From Gerudo Fortress (7)", "Depuis la Forteresse Gerudo (7)", 0x049E },
        { "From Gerudo Fortress (8)", "From Gerudo Fortress (8)", "Depuis la Forteresse Gerudo (8)", 0x04A2 },
        { "From Gerudo Fortress (9)", "From Gerudo Fortress (9)", "Depuis la Forteresse Gerudo (9)", 0x04A6 },
        { "From Gerudo Fortress (10)", "From Gerudo Fortress (10)", "Depuis la Forteresse Gerudo (10)", 0x04AA },
        { "From Gerudo Fortress (11)", "From Gerudo Fortress (11)", "Depuis la Forteresse Gerudo (11)", 0x04AE },
        { "From Gerudo Fortress (12)", "From Gerudo Fortress (12)", "Depuis la Forteresse Gerudo (12)", 0x04B2 },
        { "From Gerudo Fortress (13)", "From Gerudo Fortress (13)", "Depuis la Forteresse Gerudo (13)", 0x0570 },
    }},
    { "30:Haunted Wasteland", "30:Haunted Wasteland", "30:Desert Hante", Select_LoadGame, 2, {
        { "From Gerudo Fortress", "From Gerudo Fortress", "Depuis la Forteresse Gerudo", 0x0130 },
        { "From Desert Colossus", "From Desert Colossus", "Depuis le Colosse du Desert", 0x0365 },
    }},
    { "31:Desert Colossus", "31:Desert Colossus", "31:Colosse du Desert", Select_LoadGame, 7, {
        { "From Haunted Wasteland", "From Haunted Wasteland", "Depuis le Desert Hante", 0x0123 },
        { "From Spirit Temple", "From Spirit Temple", "Depuis le Temple de l'Esprit", 0x01E1 },
        { "From Spirit Temple (Left Hand)", "From Spirit Temple (Left Hand)", "Depuis le Temple de l'Esprit (Main Gauche)", 0x01E5 },
        { "From Spirit Temple (Right Hand)", "From Spirit Temple (Right Hand)", "Depuis le Temple de l'Esprit (Main Droite)", 0x01E9 },
        { "From Fairy Fountain", "From Fairy Fountain", "Depuis la Fontaine des Fees", 0x057C },
        { "Great Fairy", "Great Fairy", "Grande Fee", 0x0588 },
        { "Requiem of Spirit Warp", "Requiem of Spirit Warp", "Teleporteur du Requiem de l'Esprit", 0x01F1 },
    }},
    { "32:Deku Tree", "32:Deku Tree", "32:Arbre Mojo", Select_LoadGame, 3, {
        { "Entrance", "Entrance", "Entree", 0x0001 },
        { "From Gohma's Lair", "From Gohma's Lair", "Depuis le Repaire de Gohma", 0x0252 },
        { "Gohma's Lair", "Gohma's Lair", "Repaire de Gohma", 0x040F },
    }},
    { "33:Dodongos Cavern", "33:Dodongos Cavern", "33:Caverne Dodongo", Select_LoadGame, 3, {
        { "Entrance", "Entrance", "Entree", 0x0004 },
        { "From King Dodongo's Lair", "From King Dodongo's Lair", "Depuis le Repaire du Roi Dodongo", 0x00C5 },
        { "King Dodongo's Lair", "King Dodongo's Lair", "Repaire du Roi Dodongo", 0x040B },
    }},
    { "34:Jabu Jabu", "34:Jabu Jabu", "34:Jabu-Jabu", Select_LoadGame, 2, {
        { "Entrance", "Entrance", "Entree", 0x0028 },
        { "Barinade's Lair", "Barinade's Lair", "Repaire de Barinade", 0x0301 },
    }},
    { "35:Forest Temple", "35:Forest Temple", "35:Temple de la Foret", Select_LoadGame, 4, {
        { "Entrance", "Entrance", "Entree", 0x0169 },
        { "Crushing Room", "Crushing Room", "Salle de Broyage", 0x0584 },
        { "From Phantom Ganon's Lair", "From Phantom Ganon's Lair", "Depuis le Repaire de Ganon Spectral", 0x024E },
        { "Phantom Ganon's Lair", "Phantom Ganon's Lair", "Repaire de Ganon Spectral", 0x000C },
    }},
    { "36:Fire Temple", "36:Fire Temple", "36:Temple du Feu", Select_LoadGame, 3, {
        { "Entrance", "Entrance", "Entrance", 0x0165 },
        { "From Volvagia's Lair", "From Volvagia's Lair", "Depuis le Repaire de Volvagia", 0x0175 },
        { "Volvagia's Lair", "Volvagia's Lair", "Repaire de Volvagia", 0x0305 },
    }},
    { "37:Water Temple", "37:Water Temple", "37:Temple de l'Eau", Select_LoadGame, 2, { 
        { "Entrance", "Entrance", "Entree", 0x0010 },
        { "Morpha's Lair", "Morpha's Lair", "Repaire de Morpha", 0x0417 },
    }},
    { "38:Shadow Temple", "38:Shadow Temple", "38:Temple de l'Ombre", Select_LoadGame, 3, {
        { "Entrance", "Entrance", "Entree", 0x0037 },
        { "From Bongo Bongo's Lair", "From Bongo Bongo's Lair", "Avant le Repaire de Bongo Bongo", 0x02B2 },
        { "Bongo Bongo's Lair", "Bongo Bongo's Lair", "Repaire de Bongo Bongo", 0x0413 },
    }},
    { "39:Spirit Temple", "39:Spirit Temple", "39:Temple de l'Esprit", Select_LoadGame, 6, {
        { "Entrance", "Entrance", "Entree", 0x0082 },
        { "From Left Hand", "From Left Hand", "Depuis la Main Gauche", 0x03F0 },
        { "From Right Hand", "From Right Hand", "Depuis la Main Droite", 0x03F4 },
        { "Before Twinrova's Lair", "Before Twinrov's Laira", "Avant le Repaire du Duo Malefique", 0x02F5 },
        { "Nabooru Fight", "Nabooru Fight", "Combat contre Nabooru", 0x008D },
        { "Twinrova's Lair", "Twinrova's Lair", "Repaire du Duo Malefique", 0x05EC },
    }},
    { "40:Ganons Castle", "40:Ganons Castle", "40:Chateau de Ganon", Select_LoadGame, 9, {
        { "Entrance", "Entrance", "Entree", 0x0467 },
        { "From Tower", "From Tower", "Depuis la Tour", 0x0534 },
        { "Stairs to Lair - From Castle", "Stairs to Lair - From Castle", "Escaliers vers Repaire - Depuis le Chateau", 0x041B },
        { "Stairs to Lair - From Ganondorf's Lair", "Stairs to Lair - From Ganondorf's Lair", "Escaliers vers Repaire - Depuis le Repaire de Ganondorf", 0x0427 },
        { "Ganondorf's Lair", "Ganondorf's Lair", "Repaire de Ganondorf", 0x041F },
        { "Ganondorf Defeated", "Ganondorf Defeated", "Ganondorf Vaincu", 0x01C9 },
        { "Ganondorf Defeated (2)", "Ganondorf Defeated (2)", "Ganondorf Vaincu (2)", 0x04BA },
        { "Ganon Battle", "Ganon Battle", "Repaire de Ganon", 0x0517 },
        { "Ganon Death Cutscene", "Ganon Death Cutscene", "Cinematique de la Mort de Ganon", 0x043F },
    }},
    { "41:Bottom of the Well", "41:Bottom of the Well", "41:Puits", Select_LoadGame, 1, {
        { "Entrance", "Entrance", "Entree", 0x0098 },
    }},
    { "42:Ice Cavern", "42:Ice Cavern", "42:Caverne Polaire", Select_LoadGame, 1, {
        { "Entrance", "Entrance", "Entree", 0x0088 },
    }},
    { "43:Gerudo Training Grounds", "43:Gerudo Training Grounds", "43:Gymnase Gerudo", Select_LoadGame, 1, {
        { "Entrance", "Entrance", "Entree", 0x0008 },
    }},
    { "44:Warps", "44:Warps", "44:Teleporteurs", Select_LoadGame, 6, {
        { "Prelude of Light Warp", "Prelude of Light Warp", "Teleporteur du Prelude de la Lumiere", 0x05F4 },
        { "Minuet of Forest Warp", "Minuet of Forest Warp", "Teleporteur du Menuet des Bois", 0x0600 },
        { "Bolero of Fire Warp", "Bolero of Fire Warp", "Teleporteur du Bolero du Feu", 0x04F6 },
        { "Serenade Of Water Warp", "Serenade Of Water Warp", "Teleporteur de la Serenade de l'Eau", 0x0604 },
        { "Nocturne of Shadow Warp", "Nocturne of Shadow Warp", "Teleporteur du Nocturne de l'Ombre", 0x0568 },
        { "Requiem of Spirit Warp", "Requiem of Spirit Warp", "Teleporteur du Requiem de l'Esprit", 0x01F1 },
    }},
    { "45:Shops", "45:Shops", "45:Boutiques", Select_LoadGame, 9, {
        { "Kokiri Shop", "Kokiri Shop", "Boutique Kokiri", 0x00C1 },
        { "Potion Shop (Market)", "Potion Shop (Market)", "Apothicaire (Place du Marche)", 0x0388 },
        { "Bazaar Shop (Market)", "Bazaar Shop (Market)", "Bazar (Place du Marche)", 0x052C },
        { "Happy Mask Shop", "Happy Mask Shop", "Foire aux Masques", 0x0530 },
        { "Bombchu Shop", "Bombchu Shop", "Boutique de Missiles", 0x0528 },
        { "Bazaar Shop (Kakariko)", "Bazaar Shop (Kakariko)", "Bazar (Village Cocorico)", 0x00B7 },
        { "Potion Shop (Kakariko)", "Potion Shop (Kakariko)", "Apothicaire (Village Cocorico)", 0x0384 },
        { "Goron City Shop", "Goron City Shop", "Boutique Goron", 0x037C },
        { "Zora Shop", "Zora Shop", "Boutique Zora", 0x0380 },
    }},
    { "46:Great Fairies", "46:Great Fairies", "46:Grandes Fees", Select_LoadGame, 6, {
        { "Hyrule Castle (Child)", "Hyrule Castle (Child)", "Chateau d'Hyrule (Enfant)", 0x0578 },
        { "Hyrule Castle (Adult)", "Hyrule Castle (Adult)", "Chateau d'Hyrule (Adult)", 0x04C2 },
        { "Death Mountain Trail", "Death Mountain Trail", "Mont du Peril", 0x0315 },
        { "Death Mountain Crater", "Death Mountain Crater", "Cratere du Peril", 0x04BE },
        { "Zoras Fountain", "Zoras Fountain", "Fontaine Zora", 0x0371 },
        { "Desert Colossus", "Desert Colossus", "Colosse du Desert", 0x0588 },
    }},
    { "47:Chest Grottos", "47:Chest Grottos", "47:Grottes a Coffres", Select_Grotto_LoadGame, 11, {
        { "Kokiri Forest (Song of Storms)", "Kokiri Forest (Song of Storms)", "Foret Kokiri (Chant des Tempetes)", 0x00 },
        { "Lost Woods", "Lost Woods", "Bois Perdus", 0x01 },
        { "Sacred Forest Meadow", "Sacred Forest Meadow", "Bosquet Sacre", 0x02 },
        { "Hyrule Field (Near Market)", "Hyrule Field (Near Market)", "Plaine d'Hyrule (Pres de la Place du Marche)", 0x03 },
        { "Hyrule Field (Open Near Lake)", "Hyrule Field (Open Near Lake)", "Plaine d'Hyrule (Deja ouverte pres du Lac)", 0x04 },
        { "Hyrule Field (SE Boulder)", "Hyrule Field (SE Boulder)", "Plaine d'Hyrule (Rocher a Gantelets)", 0x05 },
        { "Kakariko (Open)", "Kakariko (Open)", "Village Cocorico (Deja Ouverte)", 0x06 },
        { "Kakariko (Redead)", "Kakariko (Redead)", "Village Cocorico (Effrois)", 0x07 },
        { "Death Mountain Trail (Song of Storms)", "Death Mountain Trail (Song of Storms)", "Mont du Peril (Chant des Tempetes)", 0x08 },
        { "Death Mountain Crater", "Death Mountain Crater", "Cratere du Peril", 0x09 },
        { "Zora River (Open)", "Zora River (Open)", "Riviere Zora (Deja Ouverte)", 0x0A },
    }},
    { "48:Scrub Grottos", "48:Scrub Grottos", "48:Grottes des Pestes Marchandes", Select_Grotto_LoadGame, 10, {
        { "Hyrule Field (Near Lake)", "Hyrule Field (Near Lake)", "Plaine d'Hyrule (Pres du Lac)", 0x0B },
        { "Death Mountain Crater", "Death Mountain Crater", "Cratere du Peril", 0x0C },
        { "Goron City", "Goron City", "Village Goron", 0x0D },
        { "Lon Lon Ranch", "Lon Lon Ranch", "Ranch Lon Lon", 0x0E },
        { "Lake Hylia", "Lake Hylia", "Lac Hylia", 0x0F },
        { "Lost Woods", "Lost Woods", "Bois Perdus", 0x10 },
        { "Zora River (Song of Storms)", "Zora River (Song of Storms)", "Rivere Zora (Chant des Tempetes)", 0x11 },
        { "Sacred Forest Meadow (Song of Storms)", "Sacred Forest Meadow (Song of Storms)", "Bosquet Scare (Chant des Tempetes)", 0x12 },
        { "Gerudo Valley (Song of Storms)", "Gerudo Valley (Song of Storms)", "Vallee Gerudo (Chant des Tempetes)", 0x13 },
        { "Desert Colossus", "Desert Colossus", "Colosse du Desert", 0x14 },
    }},
    { "49:Other Grottos", "49:Other Grottos", "49:Autres Grottes", Select_Grotto_LoadGame, 7, {
        { "Scrub Theatre", "Scrub Theatre", "Theatre Mojo", 0x15 },
        { "Spider Grotto (Hyrule Field)", "Spider Grotto (Hyrule Field)", "Spider Grotto (Hyrule Field)", 0x16 },
        { "Spider Grotto (Hyrule Castle)", "Spider Grotto (Hyrule Castle)", "Spider Grotto (Hyrule Castle)", 0x17 },
        { "Cow Grotto (Hyrule Field)", "Cow Grotto (Hyrule Field)", "Cow Grotto (Hyrule Field)", 0x18 },
        { "Cow Grotto (Death Mountain Trail)", "Cow Grotto (Death Mountain Trail)", "Cow Grotto (Death Mountain Trail)", 0x19 },
        { "Flooded Grotto (Gerudo Valley)", "Flooded Grotto (Gerudo Valley)", "Flooded Grotto (Gerudo Valley)", 0x1A },
        { "Flooded Grotto (Hyrule Field)", "Flooded Grotto (Hyrule Field)", "Flooded Grotto (Hyrule Field)", 0x1B },
    }},
    { "50:Debug (Use with caution)", "50:Debug (Use with caution)", "50:Debug (A utiliser avec prudence)", Select_LoadGame, 10, {
        { "Test Room", "Test Room", "Test Room", 0x0520 },
        { "SRD Map", "SRD Map", "SRD Map", 0x0018 },
        { "Test Map", "Test Map", "Test Map", 0x0094 },
        { "Treasure Chest Warp", "Treasure Chest Warp", "Treasure Chest Warp", 0x0024 },
        { "Stalfos Miniboss Room", "Stalfos Miniboss Room", "Stalfos Miniboss Room", 0x001C },
        { "Stalfos Boss Room", "Stalfos Boss Room", "Stalfos Boss Room", 0x001C },
        { "Dark Link Room", "Dark Link Room", "Dark Link Room", 0x0047 },
        { "Shooting Gallery Duplicate", "Shooting Gallery Duplicate", "Shooting Gallery Duplicate", 0x02EA },
        { "Depth Test", "Depth Test", "Depth Test", 0x00B6 },
        { "Hyrule Garden Game (Broken)", "Hyrule Garden Game (Broken)", "Hyrule Garden Game (Broken)", 0x0076 },
    }},
};

static BetterSceneSelectGrottoData sBetterGrottos[] = {
    { 0x003F, 0x00EE, 0, 0x2C, 0x55, {  -504.0,   380.0, -1224.0 }}, // Kokiri Forest -> KF Storms Grotto
    { 0x003F, 0x04D6, 2, 0x14, 0x5B, {   922.0,     0.0,  -933.0 }}, // Lost Woods -> LW Near Shortcuts Grotto
    { 0x05B4, 0x00FC, 0, 0xED, 0x56, {  -201.0,     0.0,  1906.0 }}, // SFM Entryway -> SFM Wolfos Grotto
    { 0x003F, 0x00CD, 0, 0x00, 0x51, { -1428.0,     0.0,   790.0 }}, // Hyrule Field -> HF Near Market Grotto
    { 0x003F, 0x0189, 0, 0x03, 0x51, { -4026.0,  -700.0, 13858.0 }}, // Hyrule Field -> HF Open Grotto
    { 0x003F, 0x0189, 0, 0x22, 0x51, {  -259.0,  -500.0, 12356.0 }}, // Hyrule Field -> HF Southeast Grotto
    { 0x003F, 0x034D, 0, 0x28, 0x52, {   861.0,    80.0,  -253.0 }}, // Kak Backyard -> Kak Open Grotto
    { 0x05A0, 0x034D, 0, 0xE7, 0x52, {  -400.0,     0.0,   408.0 }}, // Kakariko Village -> Kak Redead Grotto
    { 0x003F, 0x01B9, 0, 0x57, 0x60, {  -389.0,  1386.0, -1202.0 }}, // Death Mountain -> DMT Storms Grotto
    { 0x003F, 0x0147, 1, 0x7A, 0x61, {    50.0,  1233.0,  1776.0 }}, // DMC Upper Nearby -> DMC Upper Grotto
    { 0x003F, 0x019D, 0, 0x29, 0x54, {   369.0,   570.0,   128.0 }}, // Zora River -> ZR Open Grotto
    { 0x059C, 0x0189, 0, 0xE6, 0x51, { -5002.0,  -700.0, 13823.0 }}, // Hyrule Field -> HF Inside Fence Grotto
    { 0x05A4, 0x0246, 1, 0xF9, 0x61, { -1703.0,   722.0,  -481.0 }}, // DMC Lower Nearby -> DMC Hammer Grotto
    { 0x05A4, 0x014D, 3, 0xFB, 0x62, {  1091.0,   580.0, -1192.0 }}, // GC Grotto Platform -> GC Grotto
    { 0x05A4, 0x05D4, 0, 0xFC, 0x63, {  1798.0,     0.0,  1498.0 }}, // Lon Lon Ranch -> LLR Grotto
    { 0x05A4, 0x021D, 0, 0xEF, 0x57, { -3044.0, -1033.0,  6070.0 }}, // Lake Hylia -> LH Grotto
    { 0x05B0, 0x01A9, 8, 0xF5, 0x5B, {   677.0,     0.0, -2515.0 }}, // LW Beyond Mido -> LW Scrubs Grotto
    { 0x05BC, 0x00EA, 0, 0xEB, 0x54, { -1632.0,   100.0,  -123.0 }}, // Zora River -> ZR Storms Grotto
    { 0x05BC, 0x0215, 0, 0xEE, 0x56, {   317.0,   480.0, -2303.0 }}, // Sacred Forest Meadow -> SFM Storms Grotto
    { 0x05BC, 0x03D0, 0, 0xF0, 0x5A, { -1321.0,    15.0,  -968.0 }}, // GV Fortress Side -> GV Storms Grotto
    { 0x05BC, 0x01F1, 0, 0xFD, 0x5C, {    71.0,   -32.0, -1303.0 }}, // Desert Colossus -> Colossus Grotto
    { 0x05C4, 0x04D6, 6, 0xF3, 0x5B, {    75.0,   -20.0, -1596.0 }}, // LW Beyond Mido -> Deku Theater
    { 0x0598, 0x017D, 0, 0xE5, 0x51, {  2059.0,    20.0,  -174.0 }}, // Hyrule Field -> HF Near Kak Grotto
    { 0x05B8, 0x023D, 0, 0xF6, 0x5F, {   986.0,  1571.0,   837.0 }}, // Hyrule Castle Grounds -> HC Storms Grotto
    { 0x05A8, 0x018D, 0, 0xE4, 0x51, { -7873.0,  -300.0,  6916.0 }}, // Hyrule Field -> HF Cow Grotto
    { 0x05FC, 0x01B9, 0, 0xF8, 0x60, {  -678.0,  1946.0,  -284.0 }}, // Death Mountain Summit -> DMT Cow Grotto
    { 0x05AC, 0x0117, 0, 0xF2, 0x5A, {   271.0,  -555.0,  1465.0 }}, // GV Grotto Ledge -> GV Octorok Grotto
    { 0x05C0, 0x00CD, 0, 0xE1, 0x51, { -4945.0,  -300.0,  2841.0 }}, // Hyrule Field -> HF Tektite Grotto
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

        if (CVarGetInteger("gDebugWarpScreenTranslation", 0)) {
            switch (gSaveContext.language) {
                case LANGUAGE_ENG:
                default:
                    name = this->scenes[scene].englishName;
                    break;
                case LANGUAGE_GER:
                    name = this->scenes[scene].germanName;
                    break;
                case LANGUAGE_FRA:
                    name = this->scenes[scene].frenchName;
                    break;
            }
        } else {
            name = this->scenes[scene].japaneseName;
        }
        //name = this->scenes[scene].name;
        if (name == NULL) {
            name = "**Null**";
        }

        GfxPrint_Printf(printer, "%s", name);
    };

    GfxPrint_SetColor(printer, 155, 55, 150, 255);

    // Small position hack of the OPT=X text since german Link's Age overlap if translated
    if (CVarGetInteger("gDebugWarpScreenTranslation", 0) && gSaveContext.language == LANGUAGE_GER) {
        GfxPrint_SetPos(printer, 26, 26);
    } else {
        GfxPrint_SetPos(printer, 20, 26);
    }
    
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
        
        if (CVarGetInteger("gDebugWarpScreenTranslation", 0)) {
            switch (gSaveContext.language) {
                case LANGUAGE_ENG:
                default:
                    name = this->betterScenes[scene].englishName;
                    break;
                case LANGUAGE_GER:
                    name = this->betterScenes[scene].germanName;
                    break;
                case LANGUAGE_FRA:
                    name = this->betterScenes[scene].frenchName;
                    break;
            }
        } else {
            name = this->betterScenes[scene].englishName;
        }
        if (name == NULL) {
            name = "**Null**";
        }

        GfxPrint_Printf(printer, "%s", name);
    };

    GfxPrint_SetColor(printer, 205, 100, 200, 255);
    GfxPrint_SetPos(printer, 3, 26);

    if (CVarGetInteger("gDebugWarpScreenTranslation", 0)) {
        switch (gSaveContext.language) {
            case LANGUAGE_ENG:
            default:
                GfxPrint_Printf(printer, "%s", this->betterScenes[this->currentScene].entrancePairs[this->pageDownIndex].englishName);
                break;
            case LANGUAGE_GER:
                GfxPrint_Printf(printer, "%s", this->betterScenes[this->currentScene].entrancePairs[this->pageDownIndex].germanName);
                break;
            case LANGUAGE_FRA:
                GfxPrint_Printf(printer, "%s", this->betterScenes[this->currentScene].entrancePairs[this->pageDownIndex].frenchName);
                break;
        }
    } else {
        GfxPrint_Printf(printer, "%s", this->betterScenes[this->currentScene].entrancePairs[this->pageDownIndex].englishName);
    }
}

static SceneSelectLoadingMessages sLoadingMessages[] = {
    { GFXP_HIRAGANA "ｼﾊﾞﾗｸｵﾏﾁｸﾀﾞｻｲ", "Please wait a minute", "Please wait a minute", "Veuillez patienter une minute" },
    { GFXP_HIRAGANA "ﾁｮｯﾄ ﾏｯﾃﾈ", "Hold on a sec", "Hold on a sec" "Une seconde, ça arrive" },
    { GFXP_KATAKANA "ｳｪｲﾄ ｱ ﾓｰﾒﾝﾄ", "Wait a moment", "Wait a moment", "Patientez un instant" },
    { GFXP_KATAKANA "ﾛｰﾄﾞ" GFXP_HIRAGANA "ﾁｭｳ", "Loading", "Loading", "Chargement" },
    { GFXP_HIRAGANA "ﾅｳ ﾜｰｷﾝｸﾞ", "Now working", "Now working", "Au travail" },
    { GFXP_HIRAGANA "ｲﾏ ﾂｸｯﾃﾏｽ", "Now creating", "Now creating", "En cours de creation" },
    { GFXP_HIRAGANA "ｺｼｮｳｼﾞｬﾅｲﾖ", "It's not broken", "It's not broken", "C'est pas casse!" },
    { GFXP_KATAKANA "ｺｰﾋｰ ﾌﾞﾚｲｸ", "Coffee Break", "Coffee Break", "Pause Cafe" },
    { GFXP_KATAKANA "Bﾒﾝｦｾｯﾄｼﾃｸﾀﾞｻｲ", "Please set B side", "Please set B side", "Please set B side" },
    { GFXP_HIRAGANA "ｼﾞｯﾄ" GFXP_KATAKANA "ｶﾞﾏﾝ" GFXP_HIRAGANA "ﾉ" GFXP_KATAKANA "ｺ" GFXP_HIRAGANA "ﾃﾞｱｯﾀ", "Be patient, now", "", "Veuillez patientez" },
    { GFXP_HIRAGANA "ｲﾏｼﾊﾞﾗｸｵﾏﾁｸﾀﾞｻｲ", "Please wait just a minute", "", "Patientez un peu" },
    { GFXP_HIRAGANA "ｱﾜﾃﾅｲｱﾜﾃﾅｲ｡ﾋﾄﾔｽﾐﾋﾄﾔｽﾐ｡", "Don't panic, don't panic. Take a break, take a break.", "", "Pas de panique. Prenez une pause." },
    { "Enough! My ship sails in the morning!", "Enough! My ship sails in the morning!", "Enough! My ship sails in the morning!", "Enough! My ship sails in the morning!" },
};

void Select_PrintLoadingMessage(SelectContext* this, GfxPrint* printer) {
    s32 randomMsg;

    GfxPrint_SetPos(printer, 10, 15);
    GfxPrint_SetColor(printer, 255, 255, 255, 255);
    randomMsg = Rand_ZeroOne() * ARRAY_COUNT(sLoadingMessages);
    if (CVarGetInteger("gDebugWarpScreenTranslation", 0)) {
        switch (gSaveContext.language) {
            case LANGUAGE_ENG:
            default:
                GfxPrint_Printf(printer, "%s", sLoadingMessages[randomMsg].englishMessage);
                break;
            case LANGUAGE_GER:
                GfxPrint_Printf(printer, "%s", sLoadingMessages[randomMsg].germanMessage);
                break;
            case LANGUAGE_FRA:
                GfxPrint_Printf(printer, "%s", sLoadingMessages[randomMsg].frenchMessage);
                break;
        }
    } else {
        GfxPrint_Printf(printer, "%s", sLoadingMessages[randomMsg].japaneseMessage);
    }
}

static SceneSelectAgeLabels sAgeLabels[] = {
    { GFXP_HIRAGANA "17(ﾜｶﾓﾉ)", "17(Adult)", "17(Erwachsener)", "17(Adulte)" }, // "17(young)
    { GFXP_HIRAGANA "5(ﾜｶｽｷﾞ)", "10(Child)", "10(Kind)", "10(Enfant)" }, // "5(very young), for the translation I decided to use the canonical age
};

static BetterSceneSelectAgeLabels sBetterAgeLabels[] = {
    { "Adult", "Erwachsener", "Adulte" }, 
    { "Child", "Kind", "Enfant" },
};

void Select_PrintAgeSetting(SelectContext* this, GfxPrint* printer, s32 age) {
    GfxPrint_SetPos(printer, 4, 26);
    GfxPrint_SetColor(printer, 255, 255, 55, 255);
    if (CVarGetInteger("gDebugWarpScreenTranslation", 0)) {
        switch (gSaveContext.language) {
            case LANGUAGE_ENG:
            default:
                GfxPrint_Printf(printer, "Age:%s", sAgeLabels[age].englishAge);
                break;
            case LANGUAGE_GER:
                GfxPrint_Printf(printer, "Alter:%s", sAgeLabels[age].germanAge);
                break;
            case LANGUAGE_FRA:
                GfxPrint_Printf(printer, "Age:%s", sAgeLabels[age].frenchAge);
                break;
        }
    } else {
        GfxPrint_Printf(printer, "Age:%s", sAgeLabels[age].japaneseAge);
    }
}

void Better_Select_PrintAgeSetting(SelectContext* this, GfxPrint* printer, s32 age) {
    GfxPrint_SetPos(printer, 25, 25);
    GfxPrint_SetColor(printer, 100, 100, 100, 255);
    if (CVarGetInteger("gDebugWarpScreenTranslation", 0)) {
        switch (gSaveContext.language) {
            case LANGUAGE_ENG:
            case LANGUAGE_FRA:
            default:
                GfxPrint_Printf(printer, "(B)Age:");
                break;
            case LANGUAGE_GER:
                GfxPrint_Printf(printer, "(B)Alter:");
                break;
        }
    } else {
        GfxPrint_Printf(printer, "(B)Age:");
    }
    
    GfxPrint_SetColor(printer, 55, 200, 50, 255);
    if (CVarGetInteger("gDebugWarpScreenTranslation", 0)) {
        switch (gSaveContext.language) {
            case LANGUAGE_ENG:
            default:
                GfxPrint_Printf(printer, "%s", sBetterAgeLabels[age].englishAge);
                break;
            case LANGUAGE_GER:
                GfxPrint_Printf(printer, "%s", sBetterAgeLabels[age].germanAge);
                break;
            case LANGUAGE_FRA:
                GfxPrint_Printf(printer, "%s", sBetterAgeLabels[age].frenchAge);
                break;
        }
    } else {
        GfxPrint_Printf(printer, "%s", sBetterAgeLabels[age].englishAge);
    }
}

void Select_PrintCutsceneSetting(SelectContext* this, GfxPrint* printer, u16 csIndex) {
    char* cutsceneLabels[13][4] = {
        { GFXP_HIRAGANA " ﾖﾙ " GFXP_KATAKANA "ｺﾞﾛﾝ", "Day", "Nacht", "Jour" },
        { GFXP_HIRAGANA "ｵﾋﾙ " GFXP_KATAKANA "ｼﾞｬﾗ", "Night", "Tag", "Nuit" },
        { "ﾃﾞﾓ00", "Demo00", "Demo00", "Demo00" },
        { "ﾃﾞﾓ01", "Demo01", "Demo01", "Demo01" },
        { "ﾃﾞﾓ02", "Demo02", "Demo02", "Demo02" },
        { "ﾃﾞﾓ03", "Demo03", "Demo03", "Demo03" },
        { "ﾃﾞﾓ04", "Demo04", "Demo04", "Demo04" },
        { "ﾃﾞﾓ05", "Demo05", "Demo05", "Demo05" },
        { "ﾃﾞﾓ06", "Demo06", "Demo06", "Demo06" },
        { "ﾃﾞﾓ07", "Demo07", "Demo07", "Demo07" },
        { "ﾃﾞﾓ08", "Demo08", "Demo08", "Demo08" },
        { "ﾃﾞﾓ09", "Demo09", "Demo09", "Demo09" },
        { "ﾃﾞﾓ0A", "Demo0A", "Demo0A", "Demo0A" },
    };
    
    char* label;
    int lang = CVarGetInteger("gDebugWarpScreenTranslation", 0) ? gSaveContext.language + 1 : 0;

    GfxPrint_SetPos(printer, 4, 25);
    GfxPrint_SetColor(printer, 255, 255, 55, 255);

    switch (csIndex) {
        case 0:
            label = cutsceneLabels[1][lang];
            gSaveContext.dayTime = 0;
            break;
        case 0x8000:
            // clang-format off
            gSaveContext.dayTime = 0x8000; label = cutsceneLabels[0][lang];
            // clang-format on
            break;
        case 0xFFF0:
            // clang-format off
            gSaveContext.dayTime = 0x8000; label = cutsceneLabels[2][lang];
            // clang-format on
            break;
        case 0xFFF1:
            label = cutsceneLabels[3][lang];
            break;
        case 0xFFF2:
            label = cutsceneLabels[4][lang];
            break;
        case 0xFFF3:
            label = cutsceneLabels[5][lang];
            break;
        case 0xFFF4:
            label = cutsceneLabels[6][lang];
            break;
        case 0xFFF5:
            label = cutsceneLabels[7][lang];
            break;
        case 0xFFF6:
            label = cutsceneLabels[8][lang];
            break;
        case 0xFFF7:
            label = cutsceneLabels[9][lang];
            break;
        case 0xFFF8:
            label = cutsceneLabels[10][lang];
            break;
        case 0xFFF9:
            label = cutsceneLabels[11][lang];
            break;
        case 0xFFFA:
            label = cutsceneLabels[12][lang];
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
        if (CVarGetInteger("gDebugWarpScreenTranslation", 0)) {
            switch (gSaveContext.language) {
                case LANGUAGE_ENG:
                default:
                    label = "Night";
                    break;
                case LANGUAGE_GER:
                    label = "Nacht";
                    break;
                case LANGUAGE_FRA:
                    label = "Nuit";
                    break;
            }
        } else {
            label = "Night";
        }
    } else {
        if (CVarGetInteger("gDebugWarpScreenTranslation", 0)) {
            switch (gSaveContext.language) {
                case LANGUAGE_ENG:
                default:
                    label = "Day";
                    break;
                case LANGUAGE_GER:
                    label = "Tag";
                    break;
                case LANGUAGE_FRA:
                    label = "Jour";
                    break;
            }
        } else {
            label = "Day";
        }
    }
    if (CVarGetInteger("gDebugWarpScreenTranslation", 0)) {
        switch (gSaveContext.language) {
            case LANGUAGE_ENG:
            default:
                GfxPrint_Printf(printer, "(Z/R)Time:");
                break;
            case LANGUAGE_GER:
                GfxPrint_Printf(printer, "(Z/R)Zeit:");
                break;
            case LANGUAGE_FRA:
                GfxPrint_Printf(printer, "(Z/R)Temps:");
                break;
        }
    } else {
        GfxPrint_Printf(printer, "(Z/R)Time:");
    }

    GfxPrint_SetColor(printer, 200, 200, 55, 255);
    GfxPrint_Printf(printer, "%s", label);
}

void Select_DrawMenu(SelectContext* this) {
    GraphicsContext* gfxCtx = this->state.gfxCtx;
    GfxPrint* printer;

    OPEN_DISPS(gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x00, NULL);
    Gfx_SetupFrame(gfxCtx, 0, 0, 0);
    SET_FULLSCREEN_VIEWPORT(&this->view);
    func_800AAA50(&this->view, 0xF);
    Gfx_SetupDL_28Opa(gfxCtx);

    printer = alloca(sizeof(GfxPrint));
    GfxPrint_Init(printer);
    GfxPrint_Open(printer, POLY_OPA_DISP);
    if (CVarGetInteger("gBetterDebugWarpScreen", 0)) {
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
    Gfx_SetupFrame(gfxCtx, 0, 0, 0);
    SET_FULLSCREEN_VIEWPORT(&this->view);
    func_800AAA50(&this->view, 0xF);
    Gfx_SetupDL_28Opa(gfxCtx);

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
    Gfx_SetupFrame(gfxCtx, 0, 0, 0);
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

    if (CVarGetInteger("gBetterDebugWarpScreen", 0)) {
        Better_Select_UpdateMenu(this);
    } else {
        Select_UpdateMenu(this);
    }
    Select_Draw(this);
}

void Select_Destroy(GameState* thisx) {
    osSyncPrintf("%c", BEL);
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
    this->count = CVarGetInteger("gBetterDebugWarpScreen", 0) ? ARRAY_COUNT(sBetterScenes) : ARRAY_COUNT(sScenes);
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
    if (CVarGetInteger("gBetterDebugWarpScreen", 0)) {
        this->currentScene = CVarGetInteger("gBetterDebugWarpScreenCurrentScene", 0);
        this->topDisplayedScene = CVarGetInteger("gBetterDebugWarpScreenTopDisplayedScene", 0);
        this->pageDownIndex = CVarGetInteger("gBetterDebugWarpScreenPageDownIndex", 0);
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
