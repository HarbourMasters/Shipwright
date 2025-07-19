/*
 * File: z_select.c
 * Overlay: ovl_select
 * Description: Debug Scene Select Menu
 */

#include <libultraship/libultra.h>
#include "global.h"
#include "vt.h"

#include "soh/Enhancements/enhancementTypes.h"
#include "soh/Enhancements/randomizer/randomizer_entrance.h"
#include "soh/Enhancements/randomizer/randomizer_grotto.h"
#include "soh/OTRGlobals.h"
#include "soh/ResourceManagerHelpers.h"

void Select_SwitchBetterWarpMode(SelectContext* this, u8 isBetterWarpMode);
void Sram_InitDebugSave(void);

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
    gSaveContext.forceRisingButtonAlphas = gSaveContext.unk_13E8 = gSaveContext.unk_13EA = gSaveContext.unk_13EC = 0;
    Audio_QueueSeqCmd(SEQ_PLAYER_BGM_MAIN << 24 | NA_BGM_STOP);
    gSaveContext.entranceIndex = entranceIndex;

    // Check the entrance to see if the exit should be overridden to a grotto return point for entrance rando
    if (IS_RANDO && Randomizer_GetSettingValue(RSK_SHUFFLE_ENTRANCES)) {
        // Ignore return value as we want to load into the entrance specified by the debug menu
        Grotto_OverrideSpecialEntrance(Entrance_GetOverride(entranceIndex));
    }

    if (this->isBetterWarp) {
        CVarSetInteger(CVAR_GENERAL("BetterDebugWarpScreenCurrentScene"), this->currentScene);
        CVarSetInteger(CVAR_GENERAL("BetterDebugWarpScreenTopDisplayedScene"), this->topDisplayedScene);
        CVarSetInteger(CVAR_GENERAL("BetterDebugWarpScreenPageDownIndex"), this->pageDownIndex);
        CVarSave();

        if (ResourceMgr_GameHasMasterQuest() && ResourceMgr_GameHasOriginal()) {
            BetterSceneSelectEntrancePair entrancePair =
                this->betterScenes[this->currentScene].entrancePairs[this->pageDownIndex];
            // Check to see if the scene/entrance we just picked can be MQ'd
            if (entrancePair.canBeMQ) {
                s16 scene = gEntranceTable[entrancePair.entranceIndex].scene;
                u8 isEntranceDefaultMQ = ResourceMgr_IsSceneMasterQuest(scene);
                if (!isEntranceDefaultMQ && this->opt) { // Force vanilla for default MQ scene
                    CVarSetInteger(CVAR_GENERAL("BetterDebugWarpScreenMQMode"), WARP_MODE_OVERRIDE_MQ_AS_VANILLA);
                    CVarSetInteger(CVAR_GENERAL("BetterDebugWarpScreenMQModeScene"), scene);
                } else if (isEntranceDefaultMQ && !this->opt) { // Force MQ for default vanilla scene
                    CVarSetInteger(CVAR_GENERAL("BetterDebugWarpScreenMQMode"), WARP_MODE_OVERRIDE_VANILLA_AS_MQ);
                    CVarSetInteger(CVAR_GENERAL("BetterDebugWarpScreenMQModeScene"), scene);
                }
            }
        }
    }

    gSaveContext.respawnFlag = 0;
    gSaveContext.respawn[RESPAWN_MODE_DOWN].entranceIndex = ENTR_LOAD_OPENING;
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
    gSaveContext.forceRisingButtonAlphas = gSaveContext.unk_13E8 = gSaveContext.unk_13EA = gSaveContext.unk_13EC = 0;
    Audio_QueueSeqCmd(SEQ_PLAYER_BGM_MAIN << 24 | NA_BGM_STOP);
    // Entrance index and grotto content data to load the correct grotto and actors
    gSaveContext.entranceIndex = this->betterGrottos[grottoIndex].entranceIndex;
    gSaveContext.respawn[RESPAWN_MODE_RETURN].data = this->betterGrottos[grottoIndex].data;
    // Infomation to return to the correct grotto exit
    gSaveContext.respawn[RESPAWN_MODE_RETURN].entranceIndex = this->betterGrottos[grottoIndex].returnEntranceIndex;
    gSaveContext.respawn[RESPAWN_MODE_RETURN].roomIndex = this->betterGrottos[grottoIndex].roomIndex;
    gSaveContext.respawn[RESPAWN_MODE_RETURN].playerParams = 0x4ff;
    gSaveContext.respawn[RESPAWN_MODE_RETURN].pos = this->betterGrottos[grottoIndex].pos;

    // Check the entrance to see if the exit should be overridden to a grotto return point for entrance rando
    if (IS_RANDO && Randomizer_GetSettingValue(RSK_SHUFFLE_ENTRANCES)) {
        // Use grotto content and parent scene num to identify the right grotto
        s16 grottoEntrance = Grotto_GetRenamedGrottoIndexFromOriginal(this->betterGrottos[grottoIndex].data,
                                                                      this->betterGrottos[grottoIndex].exitScene);
        // Ignore return value as we want to load into the entrance specified by the debug menu
        Grotto_OverrideSpecialEntrance(Entrance_GetOverride(grottoEntrance));
    }

    if (this->isBetterWarp) {
        CVarSetInteger(CVAR_GENERAL("BetterDebugWarpScreenCurrentScene"), this->currentScene);
        CVarSetInteger(CVAR_GENERAL("BetterDebugWarpScreenTopDisplayedScene"), this->topDisplayedScene);
        CVarSetInteger(CVAR_GENERAL("BetterDebugWarpScreenPageDownIndex"), this->pageDownIndex);
        CVarSave();
    }

    gSaveContext.respawnFlag = 0;
    gSaveContext.respawn[RESPAWN_MODE_DOWN].entranceIndex = ENTR_LOAD_OPENING;
    gSaveContext.seqId = (u8)NA_BGM_DISABLED;
    gSaveContext.natureAmbienceId = 0xFF;
    gSaveContext.showTitleCard = true;
    gWeatherMode = 0;
    this->state.running = false;
    SET_NEXT_GAMESTATE(&this->state, Play_Init, PlayState);
}

static SceneSelectEntry sScenes[] = {
    { " 1:SPOT00", " 1:Hyrule Field", " 1:Hylianische Steppe", " 1:Plaine d'Hyrule", Select_LoadGame,
      ENTR_HYRULE_FIELD_PAST_BRIDGE_SPAWN },
    { " 2:SPOT01", " 2:Kakariko Village", " 2:Kakariko", " 2:Village Cocorico", Select_LoadGame,
      ENTR_KAKARIKO_VILLAGE_FRONT_GATE },
    { " 3:SPOT02", " 3:Graveyard", " 3:Friedhof", " 3:Cimetiere", Select_LoadGame, ENTR_GRAVEYARD_ENTRANCE },
    { " 4:SPOT03", " 4:Zora's River", " 4:Zora-Fluss", " 4:Riviere Zora", Select_LoadGame, ENTR_ZORAS_RIVER_WEST_EXIT },
    { " 5:SPOT04", " 5:Kokiri Forest", " 5:Kokiri-Wald", " 5:Foret Kokiri", Select_LoadGame, ENTR_KOKIRI_FOREST_0 },
    { " 6:SPOT05", " 6:Sacred Forest Meadow", " 6:Heilige Lichtung", " 6:Bosquet Sacre", Select_LoadGame,
      ENTR_SACRED_FOREST_MEADOW_SOUTH_EXIT },
    { " 7:SPOT06", " 7:Lake Hylia", " 7:Hylia-See", " 7:Lac Hylia", Select_LoadGame, ENTR_LAKE_HYLIA_NORTH_EXIT },
    { " 8:SPOT07", " 8:Zora's Domain", " 8:Zoras Reich", " 8:Domaine Zora", Select_LoadGame,
      ENTR_ZORAS_DOMAIN_ENTRANCE },
    { " 9:SPOT08", " 9:Zora's Fountain", " 9:Zoras Quelle", " 9:Fontaine Zora", Select_LoadGame,
      ENTR_ZORAS_FOUNTAIN_JABU_JABU_BLUE_WARP },
    { "10:SPOT09", "10:Gerudo Valley", "10:Gerudotal", "10:Vallee Gerudo", Select_LoadGame,
      ENTR_GERUDO_VALLEY_EAST_EXIT },
    { "11:SPOT10", "11:Lost Woods", "11:Verlorene Waelder", "11:Bois Perdus", Select_LoadGame,
      ENTR_LOST_WOODS_SOUTH_EXIT },
    { "12:SPOT11", "12:Desert Colossus", "12:Wuestenkoloss", "12:Colosse du Desert", Select_LoadGame,
      ENTR_DESERT_COLOSSUS_EAST_EXIT },
    { "13:SPOT12", "13:Gerudo's Fortress", "13:Gerudo-Festung", "13:Forteresse Gerudo", Select_LoadGame,
      ENTR_GERUDOS_FORTRESS_EAST_EXIT },
    { "14:SPOT13", "14:Haunted Wasteland", "14:Gespensterwueste", "14:Desert Hante", Select_LoadGame,
      ENTR_HAUNTED_WASTELAND_EAST_EXIT },
    { "15:SPOT15", "15:Hyrule Castle", "15:Schloss Hyrule", "15:Chateau d'Hyrule", Select_LoadGame,
      ENTR_CASTLE_GROUNDS_SOUTH_EXIT },
    { "16:SPOT16", "16:Death Mountain Trail", "16:Pfad zum Todesberg", "16:Chemin du Peril", Select_LoadGame,
      ENTR_DEATH_MOUNTAIN_TRAIL_BOTTOM_EXIT },
    { "17:SPOT17", "17:Death Mountain Crater", "17:Todeskrater", "17:Cratere du Peril", Select_LoadGame,
      ENTR_DEATH_MOUNTAIN_CRATER_UPPER_EXIT },
    { "18:SPOT18", "18:Goron City", "18:Goronia", "18:Village Goron", Select_LoadGame, ENTR_GORON_CITY_UPPER_EXIT },
    { "19:SPOT20", "19:Lon Lon Ranch", "19:Lon Lon-Farm", "19:Ranch Lon Lon", Select_LoadGame,
      ENTR_LON_LON_RANCH_ENTRANCE },
    { "20:" GFXP_HIRAGANA "ﾄｷﾉﾏ", "20:Temple Of Time", "20:Zitadelle der Zeit", "20:Temple du Temps", Select_LoadGame,
      ENTR_TEMPLE_OF_TIME_ENTRANCE },
    { "21:" GFXP_HIRAGANA "ｹﾝｼﾞｬﾉﾏ", "21:Chamber of Sages", "21:Halle der Weisen", "21:Sanctuaire des Sages",
      Select_LoadGame, ENTR_CHAMBER_OF_THE_SAGES_0 },
    { "22:" GFXP_HIRAGANA "ｼｬﾃｷｼﾞｮｳ", "22:Shooting Gallery", "22:Schiessbude", "22:Jeu d'adresse", Select_LoadGame,
      ENTR_SHOOTING_GALLERY_0 },
    { "23:" GFXP_KATAKANA "ﾊｲﾗﾙ" GFXP_HIRAGANA "ﾆﾜ" GFXP_KATAKANA "ｹﾞｰﾑ", "23:Castle Courtyard Game",
      "23:Burghof - Wachen", "23:Cour du Chateau (Infilration)", Select_LoadGame, ENTR_CASTLE_COURTYARD_GUARDS_DAY_0 },
    { "24:" GFXP_HIRAGANA "ﾊｶｼﾀﾄﾋﾞｺﾐｱﾅ", "24:Grave 1", "24:Grab 1", "24:Tombe 1", Select_LoadGame,
      ENTR_REDEAD_GRAVE_0 },
    { "25:" GFXP_HIRAGANA "ﾊｶｼﾀﾄﾋﾞｺﾐｱﾅ 2", "25:Grave 2", "25:Grab 2", "25:Tombe 2", Select_LoadGame,
      ENTR_GRAVE_WITH_FAIRYS_FOUNTAIN_0 },
    { "26:" GFXP_HIRAGANA "ｵｳｹ ﾉ ﾊｶｱﾅ", "26:Royal Family's Tomb", "26:Koenigsgrab", "26:Tombe Royale", Select_LoadGame,
      ENTR_ROYAL_FAMILYS_TOMB_0 },
    { "27:" GFXP_HIRAGANA "ﾀﾞｲﾖｳｾｲﾉｲｽﾞﾐ", "27:Great Fairy's Fountain (Upgrades)", "27:Feen-Quelle (Upgrades)",
      "27:Fontaine Royale des Fees (Amel.)", Select_LoadGame, ENTR_GREAT_FAIRYS_FOUNTAIN_MAGIC_DMT },
    { "28:" GFXP_HIRAGANA "ﾄﾋﾞｺﾐ ﾖｳｾｲ ｱﾅ", "28:Fairy's Fountain (Grotto)", "28:Feen-Brunnen (Grotte)",
      "28:Fontaines des Fees (Grotte)", Select_LoadGame, ENTR_FAIRYS_FOUNTAIN_0 },
    { "29:" GFXP_HIRAGANA "ﾏﾎｳｾｷ ﾖｳｾｲﾉｲｽﾞﾐ", "29:Great Fairy's Fountain (Magic)", "29:Feen-Quelle (Magie)",
      "29:Fontaine Royale des Fees (Magie)", Select_LoadGame, ENTR_GREAT_FAIRYS_FOUNTAIN_SPELLS_FARORES_ZF },
    { "30:" GFXP_KATAKANA "ｶﾞﾉﾝ" GFXP_HIRAGANA "ｻｲｼｭｳｾﾝ", "30:Ganon's Tower - Collapsing", "30:Ganons Turm - Einsturz",
      "30:Tour de Ganon - Effondrement", Select_LoadGame, ENTR_GANONS_TOWER_COLLAPSE_EXTERIOR_0 },
    { "31:" GFXP_KATAKANA "ﾊｲﾗﾙ" GFXP_HIRAGANA "ﾅｶﾆﾜ", "31:Castle Courtyard", "31:Burghof - Zelda",
      "31:Cour du Chateau", Select_LoadGame, ENTR_CASTLE_COURTYARD_ZELDA_0 },
    { "32:" GFXP_HIRAGANA "ﾂﾘﾎﾞﾘ", "32:Fishing Pond", "32:Fischweiher", "32:Etang", Select_LoadGame,
      ENTR_FISHING_POND_0 },
    { "33:" GFXP_KATAKANA "ﾎﾞﾑﾁｭｳﾎﾞｰﾘﾝｸﾞ", "33:Bombchu Bowling Alley", "33:Minenbowlingbahn", "33:Bowling Teigneux",
      Select_LoadGame, ENTR_BOMBCHU_BOWLING_ALLEY_0 },
    { "34:" GFXP_KATAKANA "ﾛﾝﾛﾝ" GFXP_HIRAGANA "ﾎﾞｸｼﾞｮｳ ｿｳｺ 1", "34:Lon Lon Ranch House", "34:Lon Lon-Farm Haus",
      "34:Maison du Ranch Lon Lon", Select_LoadGame, ENTR_LON_LON_BUILDINGS_TALONS_HOUSE },
    { "35:" GFXP_KATAKANA "ﾛﾝﾛﾝ" GFXP_HIRAGANA "ﾎﾞｸｼﾞｮｳ ｿｳｺ 2", "35:Lon Lon Ranch Silo", "35:Lon Lon-Farm Silo",
      "35:Silo du Ranch Lon Lon", Select_LoadGame, ENTR_LON_LON_BUILDINGS_TOWER },
    { "36:" GFXP_HIRAGANA "ﾐﾊﾘ ｺﾞﾔ", "36:Guard House", "36:Wachposten", "36:Maison de Garde", Select_LoadGame,
      ENTR_MARKET_GUARD_HOUSE_0 },
    { "37:" GFXP_HIRAGANA "ﾏﾎｳ ﾉ ｸｽﾘﾔ", "37:Potion Shop", "37:Magie-Laden", "37:Apothicaire", Select_LoadGame,
      ENTR_POTION_SHOP_GRANNY_0 },
    { "38:" GFXP_HIRAGANA "ﾀｶﾗﾊﾞｺﾔ", "38:Treasure Chest Game", "38:Truhenlotterie", "38:Chasse aux Tresors",
      Select_LoadGame, ENTR_TREASURE_BOX_SHOP_0 },
    { "39:" GFXP_HIRAGANA "ｷﾝ " GFXP_KATAKANA "ｽﾀﾙﾁｭﾗ ﾊｳｽ", "39:House Of Skulltula", "39:Skulltulas Haus",
      "39:Maison des Skulltulas", Select_LoadGame, ENTR_HOUSE_OF_SKULLTULA_0 },
    { "40:" GFXP_HIRAGANA "ｼﾞｮｳｶﾏﾁ ｲﾘｸﾞﾁ", "40:Entrance to Market", "40:Eingang zum Marktplatz",
      "40:Entree de la Place du Marche", Select_LoadGame, ENTR_MARKET_ENTRANCE_NORTH_EXIT },
    { "41:" GFXP_HIRAGANA "ｼﾞｮｳｶﾏﾁ", "41:Market", "41:Marktplatz", "41:Place du Marche", Select_LoadGame,
      ENTR_MARKET_SOUTH_EXIT },
    { "42:" GFXP_HIRAGANA "ｳﾗﾛｼﾞ", "42:Back Alley", "42:Seitenstrasse", "42:Ruelle", Select_LoadGame,
      ENTR_BACK_ALLEY_DAY_0 },
    { "43:" GFXP_HIRAGANA "ﾄｷﾉｼﾝﾃﾞﾝ ﾏｴ", "43:Temple of Time Exterior", "43:Vor der Zitadelle der Zeit",
      "43:Exterieur du Temple du Temps", Select_LoadGame, ENTR_TEMPLE_OF_TIME_EXTERIOR_DAY_GOSSIP_STONE_EXIT },
    { "44:" GFXP_HIRAGANA "ﾘﾝｸﾉｲｴ", "44:Link's House", "44:Links Haus", "44:Cabane de Link", Select_LoadGame,
      ENTR_LINKS_HOUSE_CHILD_SPAWN },
    { "45:" GFXP_KATAKANA "ｶｶﾘｺ" GFXP_HIRAGANA "ﾑﾗﾉﾅｶﾞﾔ", "45:Kakariko House 1", "45:Kakariko Haus 1",
      "45:Maison du Village Cocorico 1", Select_LoadGame, ENTR_KAKARIKO_CENTER_GUEST_HOUSE_0 },
    { "46:" GFXP_HIRAGANA "ｳﾗﾛｼﾞﾉ ｲｴ", "46:Back Alley House 1", "46:Seitenstrasse Haus 1", "46:Maison de la Ruelle 1",
      Select_LoadGame, ENTR_BACK_ALLEY_MAN_IN_GREEN_HOUSE },
    { "47:" GFXP_HIRAGANA "ｺｷﾘﾉﾑﾗ ﾓﾉｼﾘｷｮｳﾀﾞｲﾉｲｴ", "47:House of the Know-it-All Brothers",
      "47:Haus der Allwissenden Brueder", "47:Cabane des Freres Je-Sais-Tout", Select_LoadGame,
      ENTR_KNOW_IT_ALL_BROS_HOUSE_0 },
    { "48:" GFXP_HIRAGANA "ｺｷﾘﾉﾑﾗ ﾌﾀｺﾞﾉｲｴ", "48:House of Twins", "48:Haus der Zwillinge", "48:Cabane des Jumeaux",
      Select_LoadGame, ENTR_TWINS_HOUSE_0 },
    { "49:" GFXP_HIRAGANA "ｺｷﾘﾉﾑﾗ " GFXP_KATAKANA "ﾐﾄﾞ" GFXP_HIRAGANA "ﾉｲｴ", "49:Mido's House", "49:Midos Haus",
      "49:Cabane du Grand Mido", Select_LoadGame, ENTR_MIDOS_HOUSE_0 },
    { "50:" GFXP_HIRAGANA "ｺｷﾘﾉﾑﾗ " GFXP_KATAKANA "ｻﾘｱ" GFXP_HIRAGANA "ﾉｲｴ", "50:Saria's House", "50:Salias Haus",
      "50:Cabane de Saria", Select_LoadGame, ENTR_SARIAS_HOUSE_0 },
    { "51:" GFXP_HIRAGANA "ｳﾏｺﾞﾔ", "51:Stable", "51:Stall", "51:Etable", Select_LoadGame, ENTR_STABLE_0 },
    { "52:" GFXP_HIRAGANA "ﾊｶﾓﾘﾉｲｴ", "52:Grave Keeper's Hut", "52:Huette des Totengraebers", "52:Cabane du Fossoyeur",
      Select_LoadGame, ENTR_GRAVEKEEPERS_HUT_0 },
    { "53:" GFXP_HIRAGANA "ｳﾗﾛｼﾞ ｲﾇｵﾊﾞｻﾝﾉｲｴ", "53:Dog Lady's House", "53:Haus der Hunde-Dame",
      "53:Maison de la Dame du Chien", Select_LoadGame, ENTR_DOG_LADY_HOUSE_0 },
    { "54:" GFXP_HIRAGANA "ｶｶﾘｺﾑﾗ " GFXP_KATAKANA "ｲﾝﾊﾟ" GFXP_HIRAGANA "ﾉｲｴ", "54:Impa's House", "54:Impas Haus",
      "54:Maison d'Impa", Select_LoadGame, ENTR_IMPAS_HOUSE_FRONT },
    { "55:" GFXP_KATAKANA "ﾊｲﾘｱ" GFXP_HIRAGANA " ｹﾝｷｭｳｼﾞｮ", "55:Lakeside Laboratory", "55:Hylia-See Laboratorium",
      "55:Laboratoire du Lac", Select_LoadGame, ENTR_LAKESIDE_LABORATORY_0 },
    { "56:" GFXP_KATAKANA "ﾃﾝﾄ", "56:Running Man's Tent", "56:Zelt des Rennlaeufers", "56:Tente du Marathonien",
      Select_LoadGame, ENTR_CARPENTERS_TENT_0 },
    { "57:" GFXP_HIRAGANA "ﾀﾃﾉﾐｾ", "57:Bazaar", "57:Basar", "57:Bazar", Select_LoadGame, ENTR_BAZAAR_0 },
    { "58:" GFXP_HIRAGANA "ｺｷﾘｿﾞｸﾉﾐｾ", "58:Kokiri Shop", "58:Kokiri-Laden", "58:Boutique Kokiri", Select_LoadGame,
      ENTR_KOKIRI_SHOP_0 },
    { "59:" GFXP_KATAKANA "ｺﾞﾛﾝ" GFXP_HIRAGANA "ﾉﾐｾ", "59:Goron Shop", "59:Goronen-Laden", "59:Boutique Goron",
      Select_LoadGame, ENTR_GORON_SHOP_0 },
    { "60:" GFXP_KATAKANA "ｿﾞｰﾗ" GFXP_HIRAGANA "ﾉﾐｾ", "60:Zora Shop", "60:Zora-Laden", "60:Boutique Zora",
      Select_LoadGame, ENTR_ZORA_SHOP_0 },
    { "61:" GFXP_KATAKANA "ｶｶﾘｺ" GFXP_HIRAGANA "ﾑﾗ  ｸｽﾘﾔ", "61:Closed Shop", "61:Geschlossener Laden",
      "61:Boutique Fermee", Select_LoadGame, ENTR_POTION_SHOP_KAKARIKO_FRONT },
    { "62:" GFXP_HIRAGANA "ｼﾞｮｳｶﾏﾁ ｸｽﾘﾔ", "62:Potion Shop", "62:Magie-Laden", "62:Apothicaire (Boutique)",
      Select_LoadGame, ENTR_POTION_SHOP_MARKET_0 },
    { "63:" GFXP_HIRAGANA "ｳﾗﾛｼﾞ ﾖﾙﾉﾐｾ", "63:Bombchu Shop", "63:Krabbelminen-Laden", "63:Boutique de Missiles Teigneux",
      Select_LoadGame, ENTR_BOMBCHU_SHOP_0 },
    { "64:" GFXP_HIRAGANA "ｵﾒﾝﾔ", "64:Happy Mask Shop", "64:Maskenhaendler", "64:Foire aux Masques", Select_LoadGame,
      ENTR_HAPPY_MASK_SHOP_0 },
    { "65:" GFXP_KATAKANA "ｹﾞﾙﾄﾞ" GFXP_HIRAGANA "ﾉｼｭｳﾚﾝｼﾞｮｳ", "65:Gerudo Training Ground", "65:Gerudo-Trainingsarena",
      "65:Gymnase Gerudo", Select_LoadGame, ENTR_GERUDO_TRAINING_GROUND_ENTRANCE },
    { "66:" GFXP_HIRAGANA "ﾖｳｾｲﾉｷﾉ " GFXP_KATAKANA "ﾀﾞﾝｼﾞｮﾝ", "66:Inside the Deku Tree", "66:Im Deku-Baum",
      "66:Arbre Mojo", Select_LoadGame, ENTR_DEKU_TREE_ENTRANCE },
    { "67:" GFXP_HIRAGANA "ﾖｳｾｲﾉｷﾉ " GFXP_KATAKANA "ﾀﾞﾝｼﾞｮﾝ ﾎﾞｽ", "67:Gohma's Lair", "67:Gohmas Verlies",
      "67:Repaire de Gohma", Select_LoadGame, ENTR_DEKU_TREE_BOSS_ENTRANCE },
    { "68:" GFXP_KATAKANA "ﾄﾞﾄﾞﾝｺﾞ ﾀﾞﾝｼﾞｮﾝ", "68:Dodongo's Cavern", "68:Dodongos Hoehle", "68:Caverne Dodongo",
      Select_LoadGame, ENTR_DODONGOS_CAVERN_ENTRANCE },
    { "69:" GFXP_KATAKANA "ﾄﾞﾄﾞﾝｺﾞ ﾀﾞﾝｼﾞｮﾝ ﾎﾞｽ", "69:King Dodongo's Lair", "69:Koenig Dodongos Verlies",
      "69:Repaire du Roi Dodongo", Select_LoadGame, ENTR_DODONGOS_CAVERN_BOSS_ENTRANCE },
    { "70:" GFXP_HIRAGANA "ｷｮﾀﾞｲｷﾞｮ " GFXP_KATAKANA "ﾀﾞﾝｼﾞｮﾝ", "70:Inside Jabu-Jabu's Belly", "70:Jabu-Jabus Bauch",
      "70:Ventre de Jabu-Jabu", Select_LoadGame, ENTR_JABU_JABU_ENTRANCE },
    { "71:" GFXP_HIRAGANA "ｷｮﾀﾞｲｷﾞｮ " GFXP_KATAKANA "ﾀﾞﾝｼﾞｮﾝ ﾎﾞｽ", "71:Barinade's Lair", "71:Barinades Verlies",
      "71:Repaire de Barinade", Select_LoadGame, ENTR_JABU_JABU_BOSS_ENTRANCE },
    { "72:" GFXP_HIRAGANA "ﾓﾘﾉｼﾝﾃﾞﾝ", "72:Forest Temple", "72:Waldtempel", "72:Temple de la Foret", Select_LoadGame,
      ENTR_FOREST_TEMPLE_ENTRANCE },
    { "73:" GFXP_HIRAGANA "ﾓﾘﾉｼﾝﾃﾞﾝ " GFXP_KATAKANA "ﾎﾞｽ", "73:Phantom Ganon's Lair", "73:Phantom-Ganons Verlies",
      "73:Repaire de Ganon Spectral", Select_LoadGame, ENTR_FOREST_TEMPLE_BOSS_ENTRANCE },
    { "74:" GFXP_HIRAGANA "ｲﾄﾞｼﾀ " GFXP_KATAKANA "ﾀﾞﾝｼﾞｮﾝ", "74:Bottom of the Well", "74:Grund des Brunnens",
      "74:Puits", Select_LoadGame, ENTR_BOTTOM_OF_THE_WELL_ENTRANCE },
    { "75:" GFXP_HIRAGANA "ﾊｶｼﾀ " GFXP_KATAKANA "ﾀﾞﾝｼﾞｮﾝ", "75:Shadow Temple", "75:Schattentempel",
      "75:Temple de l'Ombre", Select_LoadGame, ENTR_SHADOW_TEMPLE_ENTRANCE },
    { "76:" GFXP_HIRAGANA "ﾊｶｼﾀ " GFXP_KATAKANA "ﾀﾞﾝｼﾞｮﾝ ﾎﾞｽ", "76:Bongo Bongo's Lair", "76:Bongo Bongos Verlies",
      "76:Repaire de Bongo Bongo", Select_LoadGame, ENTR_SHADOW_TEMPLE_BOSS_ENTRANCE },
    { "77:" GFXP_HIRAGANA "ﾋﾉｼﾝﾃﾞﾝ", "77:Fire Temple", "77:Feuertempel", "77:Temple du Feu", Select_LoadGame,
      ENTR_FIRE_TEMPLE_ENTRANCE },
    { "78:" GFXP_HIRAGANA "ﾋﾉｼﾝﾃﾞﾝ " GFXP_KATAKANA "ﾎﾞｽ", "78:Volvagia's Lair", "78:Volvagias Verlies",
      "78:Repaire de Volcania", Select_LoadGame, ENTR_FIRE_TEMPLE_BOSS_ENTRANCE },
    { "79:" GFXP_HIRAGANA "ﾐｽﾞﾉｼﾝﾃﾞﾝ", "79:Water Temple", "79:Wassertempel", "79:Temple de l'Eau", Select_LoadGame,
      ENTR_WATER_TEMPLE_ENTRANCE },
    { "80:" GFXP_HIRAGANA "ﾐｽﾞﾉｼﾝﾃﾞﾝ " GFXP_KATAKANA "ﾎﾞｽ", "80:Morpha's Lair", "80:Morphas Verlies",
      "80:Repaire de Morpha", Select_LoadGame, ENTR_WATER_TEMPLE_BOSS_ENTRANCE },
    { "81:" GFXP_HIRAGANA "ｼﾞｬｼﾝｿﾞｳ " GFXP_KATAKANA "ﾀﾞﾝｼﾞｮﾝ", "81:Spirit Temple", "81:Geistertempel",
      "81:Temple de l'Esprit", Select_LoadGame, ENTR_SPIRIT_TEMPLE_ENTRANCE },
    { "82:" GFXP_HIRAGANA "ｼﾞｬｼﾝｿﾞｳ " GFXP_KATAKANA "ﾀﾞﾝｼﾞｮﾝ ｱｲｱﾝﾅｯｸ", "82:Iron Knuckle's Lair",
      "82:Eisenprinz' Verlies", "82:Repaire du Hache Viande", Select_LoadGame, ENTR_SPIRIT_TEMPLE_BOSS_ENTRANCE },
    { "83:" GFXP_HIRAGANA "ｼﾞｬｼﾝｿﾞｳ " GFXP_KATAKANA "ﾀﾞﾝｼﾞｮﾝ ﾎﾞｽ", "83:Twinrova's Lair", "83:Killa Ohmaz' Verlies",
      "83:Repaire du Duo Malefique", Select_LoadGame, ENTR_SPIRIT_TEMPLE_BOSS_2 },
    { "84:" GFXP_KATAKANA "ｶﾞﾉﾝ" GFXP_HIRAGANA "ﾉﾄｳ", "84:Stairs to Ganondorf's Lair",
      "84:Treppen zu Ganondorfs Verlies", "84:Repaire de Ganondorf (Escaliers)", Select_LoadGame, ENTR_GANONS_TOWER_0 },
    { "85:" GFXP_KATAKANA "ｶﾞﾉﾝ" GFXP_HIRAGANA "ﾉﾄｳ" GFXP_KATAKANA "ﾎﾞｽ", "85:Ganondorf's Lair",
      "85:Ganondorfs Verlies", "85:Repaire de Ganondorf", Select_LoadGame, ENTR_GANONDORF_BOSS_0 },
    { "86:" GFXP_HIRAGANA "ｺｵﾘﾉﾄﾞｳｸﾂ", "86:Ice Cavern", "86:Eishoehle", "86:Caverne Polaire", Select_LoadGame,
      ENTR_ICE_CAVERN_ENTRANCE },
    { "87:" GFXP_HIRAGANA "ﾊｶｼﾀ" GFXP_KATAKANA "ﾘﾚｰ", "87:Dampe Grave Relay Game", "87:Boris' Grab Staffellauf",
      "87:Tombe d'Igor", Select_LoadGame, ENTR_WINDMILL_AND_DAMPES_GRAVE_GRAVE },
    { "88:" GFXP_KATAKANA "ｶﾞﾉﾝ" GFXP_HIRAGANA "ﾁｶ " GFXP_KATAKANA "ﾀﾞﾝｼﾞｮﾝ", "88:Inside Ganon's Castle",
      "88:In Ganons Schloss", "88:Tour de Ganon", Select_LoadGame, ENTR_INSIDE_GANONS_CASTLE_ENTRANCE },
    { "89:" GFXP_KATAKANA "ｶﾞﾉﾝ" GFXP_HIRAGANA "ｻｲｼｭｳｾﾝ " GFXP_KATAKANA "ﾃﾞﾓ & ﾊﾞﾄﾙ", "89:Ganon's Lair",
      "89:Ganons Verlies", "89:Repaire de Ganon", Select_LoadGame, ENTR_GANON_BOSS_0 },
    { "90:" GFXP_KATAKANA "ｶﾞﾉﾝ" GFXP_HIRAGANA "ﾉﾄｳ ｿﾉｺﾞ 1", "90:Escaping Ganon's Castle 1",
      "90:Flucht aus Ganons Schloss 1", "90:Fuite du Chateau de Ganon 1", Select_LoadGame,
      ENTR_GANONS_TOWER_COLLAPSE_INTERIOR_0 },
    { "91:" GFXP_KATAKANA "ｶﾞﾉﾝ" GFXP_HIRAGANA "ﾉﾄｳ ｿﾉｺﾞ 2", "91:Escaping Ganon's Castle 2",
      "91:Flucht aus Ganons Schloss 2", "91:Fuite du Chateau de Ganon 2", Select_LoadGame,
      ENTR_GANONS_TOWER_COLLAPSE_INTERIOR_2 },
    { "92:" GFXP_KATAKANA "ｶﾞﾉﾝ" GFXP_HIRAGANA "ﾉﾄｳ ｿﾉｺﾞ 3", "92:Escaping Ganon's Castle 3",
      "92:Flucht aus Ganons Schloss 3", "92:Fuite du Chateau de Ganon 3", Select_LoadGame,
      ENTR_GANONS_TOWER_COLLAPSE_INTERIOR_4 },
    { "93:" GFXP_KATAKANA "ｶﾞﾉﾝ" GFXP_HIRAGANA "ﾉﾄｳ ｿﾉｺﾞ 4", "93:Escaping Ganon's Castle 4",
      "93:Flucht aus Ganons Schloss 4", "93:Fuite du Chateau de Ganon 4", Select_LoadGame,
      ENTR_GANONS_TOWER_COLLAPSE_INTERIOR_6 },
    { "94:" GFXP_KATAKANA "ｶﾞﾉﾝ" GFXP_HIRAGANA "ﾁｶ ｿﾉｺﾞ", "94:Escaping Ganon's Castle 5",
      "94:Flucht aus Ganons Schloss 5", "94:Fuite du Chateau de Ganon 5", Select_LoadGame,
      ENTR_INSIDE_GANONS_CASTLE_COLLAPSE_0 },
    { "95:" GFXP_KATAKANA "ｹﾞﾙﾄﾞ" GFXP_HIRAGANA "ﾂｳﾛ 1-2", "95:Thieves' Hideout 1-2", "95:Diebesversteck 1-2",
      "95:Repaire des Voleurs 1-2", Select_LoadGame, ENTR_THIEVES_HIDEOUT_0 },
    { "96:" GFXP_KATAKANA "ｹﾞﾙﾄﾞ" GFXP_HIRAGANA "ﾂｳﾛ 3-4 9-10", "96:Thieves' Hideout 3-4 9-10",
      "96:Diebesversteck 3-4 9-10", "96:Repaire des Voleurs 3-4 9-10", Select_LoadGame, ENTR_THIEVES_HIDEOUT_2 },
    { "97:" GFXP_KATAKANA "ｹﾞﾙﾄﾞ" GFXP_HIRAGANA "ﾂｳﾛ 5-6", "97:Thieves' Hideout 5-6", "97:Diebesversteck 5-6",
      "97:Repaire des Voleurs 5-6", Select_LoadGame, ENTR_THIEVES_HIDEOUT_4 },
    { "98:" GFXP_KATAKANA "ｹﾞﾙﾄﾞ" GFXP_HIRAGANA "ﾂｳﾛ 7-8", "98:Thieves' Hideout 7-8", "98:Diebesversteck 7-8",
      "98:Repaire des Voleurs 7-8", Select_LoadGame, ENTR_THIEVES_HIDEOUT_6 },
    { "99:" GFXP_KATAKANA "ｹﾞﾙﾄﾞ" GFXP_HIRAGANA "ﾂｳﾛ 11-12", "99:Thieves' Hideout 11-12", "99:Diebesversteck 11-12",
      "99:Repaire des Voleurs 11-12", Select_LoadGame, ENTR_THIEVES_HIDEOUT_10 },
    { "100:" GFXP_KATAKANA "ｹﾞﾙﾄﾞ" GFXP_HIRAGANA "ﾂｳﾛ 13", "100:Thieves' Hideout 13", "100:Diebesversteck 13",
      "100:Repaire des Voleurs 13", Select_LoadGame, ENTR_THIEVES_HIDEOUT_12 },
    { "101:" GFXP_HIRAGANA "ｶｸｼﾄﾋﾞｺﾐｱﾅ 0", "101:Grotto 0", "101:Grotte 0", "101:Grotte 0", Select_LoadGame,
      ENTR_GROTTOS_0 },
    { "102:" GFXP_HIRAGANA "ｶｸｼﾄﾋﾞｺﾐｱﾅ 1", "102:Grotto 1", "102:Grotte 1", "102:Grotte 1", Select_LoadGame,
      ENTR_GROTTOS_1 },
    { "103:" GFXP_HIRAGANA "ｶｸｼﾄﾋﾞｺﾐｱﾅ 2", "103:Grotto 2", "103:Grotte 2", "103:Grotte 2", Select_LoadGame,
      ENTR_GROTTOS_2 },
    { "104:" GFXP_HIRAGANA "ｶｸｼﾄﾋﾞｺﾐｱﾅ 3", "104:Grotto 3", "104:Grotte 3", "104:Grotte 3", Select_LoadGame,
      ENTR_GROTTOS_3 },
    { "105:" GFXP_HIRAGANA "ｶｸｼﾄﾋﾞｺﾐｱﾅ 4", "105:Grotto 4", "105:Grotte 4", "105:Grotte 4", Select_LoadGame,
      ENTR_GROTTOS_4 },
    { "106:" GFXP_HIRAGANA "ｶｸｼﾄﾋﾞｺﾐｱﾅ 5", "106:Grotto 5", "106:Grotte 5", "106:Grotte 5", Select_LoadGame,
      ENTR_GROTTOS_5 },
    { "107:" GFXP_HIRAGANA "ｶｸｼﾄﾋﾞｺﾐｱﾅ 6", "107:Grotto 6", "107:Grotte 6", "107:Grotte 6", Select_LoadGame,
      ENTR_GROTTOS_6 },
    { "108:" GFXP_HIRAGANA "ｶｸｼﾄﾋﾞｺﾐｱﾅ 7", "108:Grotto 7", "108:Grotte 7", "108:Grotte 7", Select_LoadGame,
      ENTR_GROTTOS_7 },
    { "109:" GFXP_HIRAGANA "ｶｸｼﾄﾋﾞｺﾐｱﾅ 8", "109:Grotto 8", "109:Grotte 8", "109:Grotte 8", Select_LoadGame,
      ENTR_GROTTOS_8 },
    { "110:" GFXP_HIRAGANA "ｶｸｼﾄﾋﾞｺﾐｱﾅ 9", "110:Grotto 9", "110:Grotte 9", "110:Grotte 9", Select_LoadGame,
      ENTR_GROTTOS_9 },
    { "111:" GFXP_HIRAGANA "ｶｸｼﾄﾋﾞｺﾐｱﾅ 10", "111:Grotto 10", "111:Grotte 10", "111:Grotte 10", Select_LoadGame,
      ENTR_GROTTOS_10 },
    { "112:" GFXP_HIRAGANA "ｶｸｼﾄﾋﾞｺﾐｱﾅ 11", "112:Grotto 11", "112:Grotte 11", "112:Grotte 11", Select_LoadGame,
      ENTR_GROTTOS_11 },
    { "113:" GFXP_HIRAGANA "ｶｸｼﾄﾋﾞｺﾐｱﾅ 12", "113:Grotto 12", "113:Grotte 12", "113:Grotte 12", Select_LoadGame,
      ENTR_GROTTOS_12 },
    { "114:" GFXP_HIRAGANA "ｶｸｼﾄﾋﾞｺﾐｱﾅ 13", "114:Grotto 13", "114:Grotte 13", "114:Grotte 13", Select_LoadGame,
      ENTR_GROTTOS_13 },
    { "115:" GFXP_KATAKANA "ﾊｲﾗﾙ ﾃﾞﾓ", "115:Goddess Cutscene Environment", "115:Goettinnen Cutscene Umgebung",
      "115:Carte de la scene des Deesses", Select_LoadGame, ENTR_CUTSCENE_MAP_0 },
    { "116:" GFXP_HIRAGANA "ﾍﾞｯｼﾂ (ﾀｶﾗﾊﾞｺ" GFXP_KATAKANA "ﾜｰﾌﾟ)", "116:Test Room", "116:Test Raum", "116:Salle de Test",
      Select_LoadGame, ENTR_BESITU_0 },
    { "117:" GFXP_HIRAGANA "ｻｻ" GFXP_KATAKANA "ﾃｽﾄ", "117:SRD Map", "117:SRD Karte", "117:Carte SRD", Select_LoadGame,
      ENTR_SASATEST_0 },
    { "118:" GFXP_KATAKANA "ﾃｽﾄﾏｯﾌﾟ", "118:Test Map", "118:Test Karte", "118:Carte de Test", Select_LoadGame,
      ENTR_TEST01_0 },
    { "119:" GFXP_KATAKANA "ﾃｽﾄﾙｰﾑ", "119:Treasure Chest Warp", "119:Schatzkisten Teleport", "119:Coffres debug",
      Select_LoadGame, ENTR_TESTROOM_0 },
    { "120:" GFXP_HIRAGANA "ﾁｭｳ" GFXP_KATAKANA "ｽﾀﾛﾌｫｽ" GFXP_HIRAGANA "ﾍﾞﾔ", "120:Stalfos Miniboss Room",
      "120:Stalfos-Ritter Miniboss Raum", "120:Salle de Miniboss Stalfos", Select_LoadGame, ENTR_SYOTES_0 },
    { "121:" GFXP_KATAKANA "ﾎﾞｽｽﾀﾛﾌｫｽ" GFXP_HIRAGANA "ﾍﾞﾔ", "121:Stalfos Boss Room", "121:Stalfos-Ritter Boss Raum",
      "121:Salle de Boss Stalfos", Select_LoadGame, ENTR_SYOTES2_0 },
    { "122:Sutaru", "122:Dark Link Room", "122:Schwarzer Link Raum", "122:Salle de Dark Link", Select_LoadGame,
      ENTR_SUTARU_0 },
    { "123:jikkenjyou", "123:Shooting Gallery Duplicate", "123:Schiessbude (Duplikat)", "123:Jeu d'adresse (Duplicata)",
      Select_LoadGame, ENTR_TEST_SHOOTING_GALLERY_0 },
    { "124:depth" GFXP_KATAKANA "ﾃｽﾄ", "124:depth test", "124:Tiefen Test", "124:Test de Profondeur", Select_LoadGame,
      ENTR_DEPTH_TEST_0 },
    { "125:" GFXP_KATAKANA "ﾊｲﾗﾙ" GFXP_HIRAGANA "ﾆﾜ" GFXP_KATAKANA "ｹﾞｰﾑ2", "125:Hyrule Garden Game (Broken)",
      "125:Burghof - Wachen-Minispiel (Kaputt)", "125:Cour du Chateau (Non fonctionnel)", Select_LoadGame,
      ENTR_HAIRAL_NIWA2_0 },
    { "title", "title", "Titelbildschirm", "Ecran-titre", Select_LoadTitle, 0 },
};

// Note about Japanese scene names:
// * SoH originally lacked the ability to render Hiragana characters using GFXP_HIRAGANA macro.
//   This is not the case anymore, but Hiragana characters are still written directly here
//   for the sake of readability and editability.
// * Dakuten (濁点) characters must be explicitly written using '゛' or '゜'.
//   Combined characters (such as 'が', 'げ', 'ば', etc) won't render.
static BetterSceneSelectEntry sBetterScenes[] = {
    // clang-format off
    { "1:ハイラルへいけ゛ん", " 1:Hyrule Field", " 1:Hylianische Steppe", " 1:Plaine d'Hyrule", Select_LoadGame, 8, {
        { "はねは゛しのちかく", "Near Drawbridge", "Nahe der Zugbruecke", "Pres du Pont-levis", ENTR_HYRULE_FIELD_PAST_BRIDGE_SPAWN, 0 },
        { "はねは゛し　から", "From Drawbridge", "Von der Zugbruecke", "Depuis le Pont-levis", ENTR_HYRULE_FIELD_ON_BRIDGE_SPAWN, 0 },
        { "カカリコむら　から", "From Kakariko Village", "Von Kakariko", "Depuis le Village Cocorico", ENTR_HYRULE_FIELD_STAIRS_EXIT, 0 },
        { "ソ゛ーラかわ　から", "From Zora River", "Vom Zora-Fluss", "Depuis la Riviere Zora", ENTR_HYRULE_FIELD_RIVER_EXIT, 0 },
        { "まよいのもり　から", "From Lost Woods", "Von den verlorenen Waeldern", "Depuis les Bois Perdus", ENTR_HYRULE_FIELD_WOODED_EXIT, 0 },
        { "ハイリアこはん　から", "From Lake Hylia", "Vom Hylia-See", "Depuis le Lac Hylia", ENTR_HYRULE_FIELD_FENCE_EXIT, 0 },
        { "ケ゛ルト゛のたに　から", "From Gerudo Valley", "Vom Gerudotal", "Depuis la Vallee Gerudo", ENTR_HYRULE_FIELD_ROCKY_PATH, 0 },
        { "ロンロンほ゛くし゛ょう　から", "From Lon Lon Ranch", "Von der Lon Lon-Farm", "Depuis le Ranch Lon Lon", ENTR_HYRULE_FIELD_CENTER_EXIT, 0 },
    }},
    { "2:コキリのもり", " 2:Kokiri Forest", " 2:Kokiri-Wald", " 2:Foret Kokiri", Select_LoadGame, 9, {
        { "リンクのおうち　から", "From Links House", "Von Links Haus", "Depuis la Cabane de Link", ENTR_KOKIRI_FOREST_OUTSIDE_LINKS_HOUSE, 0 },
        { "はし　から", "From Bridge", "Von der Bruecke", "Depuis le Pont", ENTR_KOKIRI_FOREST_LOWER_EXIT, 0 },
        { "まよいのもり　から", "From Lost Woods", "Von den verlorenen Waeldern", "Depuis les Bois Perdus", ENTR_KOKIRI_FOREST_UPPER_EXIT, 0 },
        { "テ゛クのきサマ　から", "From Deku Tree", "Vom Deku-Baum", "Depuis l'Arbre Mojo", ENTR_KOKIRI_FOREST_OUTSIDE_DEKU_TREE, 0 },
        { "コキリのおみせ　から", "From Kokiri Shop", "Vom Kokiri-Laden", "Depuis la Boutique Kokiri", ENTR_KOKIRI_FOREST_OUTSIDE_SHOP, 0 },
        { "ものしりきょうた゛いのおうち　から", "From Know-It-All Brothers House", "Vom Haus der Allwissenden Brueder", "Depuis la Cabane des Freres Je-Sais-Tout", ENTR_KOKIRI_FOREST_OUTSIDE_KNOW_IT_ALL_HOUSE, 0 },
        { "ふたこ゛のおうち　から", "From Twins House", "Vom Haus der Zwillinge", "Depuis la Cabane des Jumeaux", ENTR_KOKIRI_FOREST_OUTSIDE_TWINS_HOUSE, 0 },
        { "ミト゛さまのおやしき　から", "From Midos House", "Von Midos Haus", "Depuis la Cabane du Grand Mido", ENTR_KOKIRI_FOREST_OUTSIDE_MIDOS_HOUSE, 0 },
        { "サリアのおうち　から", "From Sarias House", "Von Salias Haus", "Depuis la Cabane de Saria", ENTR_KOKIRI_FOREST_OUTSIDE_SARIAS_HOUSE, 0 },
    }},
    { "3:コキリのたてもの", " 3:Kokiri Buildings", " 3:Kokiri Gebaeude", " 3:Cabanes des Kokiris", Select_LoadGame, 6, {
        { "リンクのおうち", "Links Bed", "Links Bett", "Lit de Link", ENTR_LINKS_HOUSE_CHILD_SPAWN, 0 },
        { "コキリのおみせ", "Kokiri Shop", "Kokiri-Laden", "Boutique Kokiri", ENTR_KOKIRI_SHOP_0, 0 },
        { "ふたこ゛のおうち", "Twins House", "Haus der Zwillinge", "Cabane des Jumeaux", ENTR_TWINS_HOUSE_0, 0 },
        { "ものしりきょうた゛いのおうち", "Know-It-All Brothers House", "Haus der Allwissenden Brueder", "Cabane des Freres Je-Sais-Tout", ENTR_KNOW_IT_ALL_BROS_HOUSE_0, 0 },
        { "ミト゛さまのおやしき", "Midos House", "Midos Haus", "Cabane du Grand Mido", ENTR_MIDOS_HOUSE_0, 0 },
        { "サリアのおうち", "Sarias House", "Salias Haus", "Cabane de Sara", ENTR_SARIAS_HOUSE_0, 0 },
    }},
    { "4:まよいのもり", " 4:Lost Woods", " 4:Verlorene Waelder", " 4:Bois Perdus", Select_LoadGame, 5, {
        { "コキリのもり　から", "From Kokiri Forest", "Vom Kokiri-Wald", "Depuis la Foret Kokiri", ENTR_LOST_WOODS_SOUTH_EXIT, 0 },
        { "もりのせいいき　から", "From Sacred Meadow", "Von der Waldlichtung", "Depuis le Bosquet Sacre", ENTR_LOST_WOODS_NORTH_EXIT, 0 },
        { "コ゛ロンシティ　から", "From Goron City", "Vom Goronia", "Depuis le Village Goron", ENTR_LOST_WOODS_TUNNEL_SHORTCUT, 0 },
        { "ソ゛ーラかわ　から", "From Zora River", "Vom Zora-Fluss", "Depuis la Riviere Zora", ENTR_LOST_WOODS_UNDERWATER_SHORTCUT, 0 },
        { "はし", "Bridge", "Bruecke", "Pont", ENTR_LOST_WOODS_BRIDGE_EAST_EXIT, 0 },
    }},
    { "5:もりのせいいき", " 5:Sacred Forest Meadow", " 5:Waldlichtung", " 5:Bosquet Sacre", Select_LoadGame, 3, {
        { "まよいのもり　から", "From Lost Woods", "Von den Verlorenen Waeldern", "Depuis les Bois Perdus", ENTR_SACRED_FOREST_MEADOW_SOUTH_EXIT, 0 },
        { "もりのしんて゛ん　から", "From Forest Temple", "Vom Waldtempel", "Depuis le Temple de la Foret", ENTR_SACRED_FOREST_MEADOW_OUTSIDE_TEMPLE, 0 },
        { "もりのメヌエット　ワーフ゜", "Minuet of Forest Warp", "Menuett des Waldes Teleport", "Teleporteur du Menuet des Bois", ENTR_SACRED_FOREST_MEADOW_WARP_PAD, 0 },
    }},
    { "6:し゛ょうかまち　いりく゛ち", " 6:Castle Town Entrance", " 6:Eingang zum Marktplatz", " 6:Entree du Bourg d'Hyrule", Select_LoadGame, 3, {
        { "ハイラルへいけ゛ん　から", "From Hyrule Field", "Von der Hylianischen Steppe", "Depuis la Plaine d'Hyrule", ENTR_MARKET_ENTRANCE_NEAR_GUARD_EXIT, 0 },
        { "し゛ょうかまち　から", "From Market", "Vom Marktplatz", "Depuis la Place du Marche", ENTR_MARKET_ENTRANCE_NORTH_EXIT, 0 },
        { "つほ゛のこや　から", "From Pot House", "Vom Wachposten", "Depuis la Maison des Jarres", ENTR_MARKET_ENTRANCE_OUTSIDE_GUARD_HOUSE, 0 },
    }},
    { "7:し゛ょうかまち", " 7:Market", " 7:Marktplatz", " 7:Place du Marche", Select_LoadGame, 11, {
        { "し゛ょうかまち　いりく゛ち　から", "From Castle Town Entrance", "Vom Eingang zum Marktplatz", "Depuis l'Entree du Bourg d'Hyrule", ENTR_MARKET_SOUTH_EXIT, 0 },
        { "まとあてや　から", "From Shooting Gallery", "Von der Schiessbude", "Depuis le Jeu d'adresse", ENTR_MARKET_DAY_OUTSIDE_SHOOTING_GALLERY, 0 },
        { "おめんや　から", "From Happy Mask Shop", "Vom Maskenhaendler", "Depuis la Foire aux Masques", ENTR_MARKET_DAY_OUTSIDE_HAPPY_MASK_SHOP, 0 },
        { "たからは゛こや　から", "From Treasure Box Minigame", "Von der Truhenlotterie", "Depuis le Bowling Teigneux", ENTR_MARKET_DAY_OUTSIDE_TREASURE_BOX_SHOP, 0 },
        { "おしろ　から", "From Castle", "Vom Schloss", "Depuis le Chateau d'Hyrule", ENTR_MARKET_DAY_CASTLE_EXIT, 0 },
        { "ときのしんて゛ん　から", "From Temple of Time", "Von der Zitadelle der Zeit", "Depuis le Temple du Temps", ENTR_MARKET_DAY_TEMPLE_EXIT, 0 },
        { "ろし゛うら　から　(みき゛)", "From Back Alley (Right)", "Von der Seitenstrasse (Rechts)", "Depuis la Ruelle (Droite)", ENTR_MARKET_DAY_3, 0 },
        { "ろし゛うら　から　(ひた゛り)", "From Back Alley (Left)", "Von der Seitenstrasse (Links)", "Depuis la Ruelle (Gauche)", ENTR_MARKET_DAY_4, 0 },
        { "くすりや　から", "From Potion Shop", "Vom Magie-Laden", "Depuis l'Apothicaire", ENTR_MARKET_DAY_OUTSIDE_POTION_SHOP, 0 },
        { "なンて゛もや　から", "From Bazaar Shop", "Vom Basar", "Depuis le Bazar", ENTR_MARKET_DAY_OUTSIDE_BAZAAR, 0 },
        { "ホ゛ムチュウホ゛ウリンク゛から", "From Bomchu Bowling Minigame", "Von der Minenbowlingbahn", "Depuis le Bowling Teigneux", ENTR_MARKET_DAY_OUTSIDE_BOMBCHU_BOWLING, 0 },
    }},
    { "8:し゛ょうかまち　ろし゛うら", " 8:Castle Town Alley", " 8:Seitenstrasse", " 8:Ruelle du Bourg d'Hyrule", Select_LoadGame, 5, {
        { "し゛ょうかまち　から　(みき゛)", "From Market (Right)", "Vom Marktplatz (Rechts)", "Depuis la Place du Marche (Droite)", ENTR_BACK_ALLEY_DAY_0, 0 },
        { "し゛ょうかまち　から　(ひた゛り)", "From Market (Left)", "Vom Marktplatz (Links)", "Depuis la Place du Marche (Gauche)", ENTR_BACK_ALLEY_DAY_1, 0 },
        { "ろし゛うらのいえ　から", "From Alley House", "Vom Seitenstrassenhaus", "Depuis la Maison de la Ruelle", ENTR_BACK_ALLEY_DAY_OUTSIDE_MAN_IN_GREEN_HOUSE, 0 },
        { "いぬのいえ　から", "From Dog House", "Vom Haus der Hunde-Dame", "Depuis la Maison du Chien", ENTR_BACK_ALLEY_DAY_4, 0 },
        { "ホ゛ムチュウや　から", "From Bombchu Shop", "Vom Krabbelminen-Laden", "Depuis le Magasin de Missiles", ENTR_BACK_ALLEY_DAY_OUTSIDE_BOMBCHU_SHOP, 0 },
    }},
    { "9:し゛ょうかまちのたてもの", " 9:Castle Town Buildings", " 9:Marktplatz Gebaeude", " 9:Batiments du Bourg d'Hyrule", Select_LoadGame, 10, {
        { "つほ゛のこや", "Pot House", "Wachposten", "Maison des Jarres", ENTR_MARKET_GUARD_HOUSE_0, 0 },
        { "まとあてや", "Shooting Gallery Minigame", "Schiessbude", "Jeu d'adresse", ENTR_SHOOTING_GALLERY_1, 0 },
        { "たからは゛こや", "Treasure Box Minigame", "Truhenlotterie", "Chasse aux Tresors", ENTR_TREASURE_BOX_SHOP_0, 0 },
        { "くすりや", "Potion Shop", "Magie-Laden", "Apothicaire", ENTR_POTION_SHOP_MARKET_0, 0 },
        { "ホ゛ムチュウホ゛ウリンク゛", "Bombchu Bowling Minigame", "Minenbowlingbahn", "Bowling Teigneux", ENTR_BOMBCHU_BOWLING_ALLEY_0, 0 },
        { "なンて゛もや", "Bazaar Shop", "Basar", "Bazar", ENTR_BAZAAR_1, 0 },
        { "おめんや", "Happy Mask Shop", "Maskenhaendler", "Foire aux Masques", ENTR_HAPPY_MASK_SHOP_0, 0 },
        { "ホ゛ムチュウや", "Bombchu Shop", "Krabbelminen-Laden", "Boutique de Missiles", ENTR_BOMBCHU_SHOP_1, 0 },
        { "いぬのいえ", "Dog House", "Haus der Hunde-Dame", "Maison du Chien", ENTR_DOG_LADY_HOUSE_0, 0 },
        { "ろし゛うらのいえ", "Alley House", "Seitenstrassenhaus", "Maison de la Ruelle", ENTR_BACK_ALLEY_MAN_IN_GREEN_HOUSE, 0 },
    }},
    { "10:ときのしんて゛ん", "10:Temple of Time", "10:Zitadelle der Zeit", "10:Temple du Temps", Select_LoadGame, 5, {
        { "そと　から", "From Outside", "Von draussen", "Depuis l'Entree", ENTR_TEMPLE_OF_TIME_ENTRANCE, 0 },
        { "マスターソート゛のた゛いさ゛　から", "From Master Sword Pedestal", "Vom Podest des Master-Schwerts", "Depuis le Piedestal de l'Epee de Legende", ENTR_TEMPLE_OF_TIME_2, 0 },
        { "ひかりのフ゜レリュート゛　ワーフ゜", "Prelude of Light Warp", "Kantate des Lichts Teleport", "Teleporteur du Prelude de la Lumiere", ENTR_TEMPLE_OF_TIME_WARP_PAD, 0 },
        { "ときのしんて゛んまえ - し゛ょうかまち　から", "Outside Temple of Time - From Market", "Vor der Zitadelle der Zeit - Vom Marktplatz", "Exterieur du Temple - Depuis la Place du Marche", ENTR_TEMPLE_OF_TIME_EXTERIOR_DAY_GOSSIP_STONE_EXIT, 0 },
        { "ときのしんて゛んまえ - ときのしんて゛ん　から", "Outside Temple of Time - From Temple of Time", "Vor der Zitadelle der Zeit - Von der Zitadelle der Zeit", "Exterieur du Temple - Depuis le Temple", ENTR_TEMPLE_OF_TIME_EXTERIOR_DAY_OUTSIDE_TEMPLE, 0 },
    }},
    { "11:ハイラルし゛ょう", "11:Hyrule Castle", "11:Schloss Hyrule", "11:Chateau d'Hyrule", Select_LoadGame, 5, {
        { "し゛ょうかまち　から", "From Market", "Vom Marktplatz", "Depuis la Place du Marche", ENTR_CASTLE_GROUNDS_SOUTH_EXIT, 0 },
        { "なかにわ　から", "From Castle Courtyard", "Vom Burghof", "Depuis la Cour du Chateau", ENTR_CASTLE_GROUNDS_RAINBOW_BRIDGE_EXIT, 0 },
        { "た゛いようせい　から", "From Great Fairy", "Von der Feen-Quelle", "Depuis la Grande Fee", ENTR_CASTLE_GROUNDS_GREAT_FAIRY_EXIT, 0 },
        { "つかまったとき", "From Courtyard Guard Capture", "Von der Wachen-Festnahme", "Depuis la capture d'un Garde de la Cour", ENTR_HYRULE_CASTLE_3, 0 },
        { "た゛いようせい", "Great Fairy", "Feen-Quelle", "Grande Fee", ENTR_GREAT_FAIRYS_FOUNTAIN_SPELLS_DINS_HC, 0 },
    }},
    { "12:ハイラルし゛ょう　なかにわ", "12:Hyrule Castle Courtyard", "12:Burghof", "12:Cour du Chateau", Select_LoadGame, 3, {
        { "ちいさなあな　から", "From Crawlspace", "Vom Kriechtunnel", "Depuis l'Entree", ENTR_CASTLE_COURTYARD_GUARDS_DAY_0, 0 },
        { "セ゛ルタ゛ひめの　なかにわ　から", "From Zelda", "Von Zelda", "Depuis Zelda", ENTR_CASTLE_COURTYARD_GUARDS_DAY_1, 0 },
        { "セ゛ルタ゛ひめの　なかにわ", "Zeldas Courtyard", "Zeldas Burghof", "Depuis la Cour de Zelda", ENTR_CASTLE_COURTYARD_ZELDA_0, 0 },
    }},
    { "13:ロンロンほ゛くし゛ょう", "13:Lon Lon Ranch", "13:Lon Lon-Farm", "13:Ranch Lon Lon", Select_LoadGame, 5, {
        { "ハイラルへいけ゛ん　から", "From Hyrule Field", "Von der Hylianischen Steppe", "Depuis la Plaine d'Hyrule", ENTR_LON_LON_RANCH_ENTRANCE, 0 },
        { "タロンのいえ　から", "From Ranch House", "Vom Farmhaus", "Depuis la Maison du Ranch", ENTR_LON_LON_RANCH_OUTSIDE_TALONS_HOUSE, 0 },
        { "うまこ゛や　から", "From Stables", "Vom Stall", "Depuis l'Etable", ENTR_LON_LON_RANCH_OUTSIDE_STABLES, 0 },
        { "なや　から", "From Back Tower", "Vom Silo", "Depuis Silo du Ranch", ENTR_LON_LON_RANCH_OUTSIDE_TOWER, 0 },
        { "エホ゜ナのうた　カットシーン", "Epona Song Cutscene", "Eponas Song Sequenz", "Cinematique du Chant d'Epona", ENTR_LON_LON_RANCH_1, 0 },
    }},
    { "14:ロンロンほ゛くし゛ょうのたてもの", "14:Lon Lon Ranch Buildings", "14:Lon Lon-Farm Gebaeude", "14:Batiments du Ranch Lon Lon", Select_LoadGame, 3, {
        { "タロンのいえ", "Ranch House", "Farmhaus", "Maison du Ranch", ENTR_LON_LON_BUILDINGS_TALONS_HOUSE, 0 },
        { "うまこ゛や", "Stables", "Stall", "Etable du Ranch", ENTR_STABLE_0, 0 },
        { "なや", "Back Tower", "Silo", "Silo du Ranch", ENTR_LON_LON_BUILDINGS_TOWER, 0 },
    }},
    { "15:カカリコむら", "15:Kakariko Village", "15:Kakariko", "15:Village Cocorico", Select_LoadGame, 15, {
        { "ハイラルへいけ゛ん　から", "From Hyrule Field", "Von der Hylianischen Steppe", "Depuis la Plaine d'Hyrule", ENTR_KAKARIKO_VILLAGE_FRONT_GATE, 0 },
        { "テ゛スマウンテン　から", "From Death Mountain", "Vom Todesberg", "Depuis le Mont du Peril", ENTR_KAKARIKO_VILLAGE_GUARD_GATE, 0 },
        { "ほ゛ち　から", "From Graveyard", "Vom Friedhof", "Depuis le Cimetiere", ENTR_KAKARIKO_VILLAGE_SOUTHEAST_EXIT, 0 },
        { "なンて゛もや　から", "From Bazaar", "Vom Basar", "Depuis le Bazar", ENTR_KAKARIKO_VILLAGE_OUTSIDE_BAZAAR, 0 },
        { "いと゛のそこ　から", "From Bottom of Well", "Vom Grund des Brunnens", "Depuis le Puits", ENTR_KAKARIKO_VILLAGE_OUTSIDE_BOTTOM_OF_THE_WELL, 0 },
        { "なか゛や　から", "From Boss House", "Vom Haus des Bosses", "Depuis la Maison du Boss", ENTR_KAKARIKO_VILLAGE_OUTSIDE_CENTER_GUEST_HOUSE, 0 },
        { "くすりや　から", "From Potion Shop", "Vom Magie-Laden", "Depuis l'Apothicaire", ENTR_KAKARIKO_VILLAGE_OUTSIDE_POTION_SHOP_FRONT, 0 },
        { "くすりや　から(うらく゛ち)", "From Potion Shop (Back Entrance)", "Vom Magie-Laden (Hintereingang)", "Depuis l'Apothicaire (Entree Arriere)", ENTR_KAKARIKO_VILLAGE_OUTSIDE_POTION_SHOP_BACK, 0 },
        { "オハ゛ハ゛のくすりや　から", "From Grannys Potion Shop", "Von Omas Magie-Laden", "Depuis l'Apothicaire (Vieille Femme)", ENTR_KAKARIKO_VILLAGE_OUTSIDE_SHOP_GRANNY, 0 },
        { "インハ゜のおやしき　から", "From Impas House", "Von Impas Haus", "Depuis la Maison d'Impa", ENTR_KAKARIKO_VILLAGE_OUTSIDE_IMPAS_HOUSE_FRONT, 0 },
        { "インハ゜のおやしき(ウシ)　から", "From Impas House (Cow)", "Von Impas Haus (Kuh)", "Depuis la Maison d'Impa (Vache)", ENTR_KAKARIKO_VILLAGE_OUTSIDE_IMPAS_HOUSE_BACK, 0 },
        { "ふうしゃ　から", "From Windmill", "Von der Windmuehle", "Depuis le Moulin", ENTR_KAKARIKO_VILLAGE_OUTSIDE_WINDMILL, 0 },
        { "まとあてや　から", "From Shooting Gallery", "Von der Schiessbude", "Depuis le Jeu d'adresse", ENTR_KAKARIKO_VILLAGE_OUTSIDE_SHOOTING_GALLERY, 0 },
        { "スタルチュラハウス　から", "From Skulltula House", "Vom Haus der Skulltulas", "Depuis la Maison des Skulltulas", ENTR_KAKARIKO_VILLAGE_OUTSIDE_SKULKLTULA_HOUSE, 0 },
        { "テ゛スマウンテンから　フクロウて゛　とんて゛くるは゛しょ", "Owl Drop Spot from Death Mountain", "Eulen-Absetzpunkt vom Todesberg", "Point de chute du Hibou depuis le Mont du Peril", ENTR_KAKARIKO_VILLAGE_OWL_DROP, 0 },
    }},
    { "16:カカリコむらのたてもの", "16:Kakariko Buildings", "16:Kakariko Gebaeude", "16:Batiments du Village Cocorico", Select_LoadGame, 9, {
        { "まとあてや", "Shooting Gallery Minigame", "Schiessbude", "Jeu d'adresse", ENTR_SHOOTING_GALLERY_0, 0 },
        { "オハ゛ハ゛のくすりや", "Grannys Potion Shop", "Omas Magie-Laden", "Apothicaire (Vieille Femme)", ENTR_POTION_SHOP_GRANNY_0, 0 },
        { "なンて゛もや", "Bazaar Shop", "Basar", "Bazar", ENTR_BAZAAR_0, 0 },
        { "くすりや", "Potion Shop", "Magie-Laden", "Apothicaire", ENTR_POTION_SHOP_KAKARIKO_FRONT, 0 },
        { "インハ゜のおやしき", "Impas House", "Impas Haus", "Maison d'Impa", ENTR_IMPAS_HOUSE_FRONT, 0 },
        { "インハ゜のおやしき(ウシのちかく)", "Impas House (Near Cow)", "Impas Haus (Kuh)", "Maison d'Impa (Vache)", ENTR_IMPAS_HOUSE_BACK, 0 },
        { "なか゛や", "Boss House", "Haus des Bosses", "Maison du Boss", ENTR_KAKARIKO_CENTER_GUEST_HOUSE_0, 0 },
        { "ふうしゃ", "Windmill", "Windmuehle", "Moulin", ENTR_WINDMILL_AND_DAMPES_GRAVE_WINDMILL, 0 },
        { "スタルチュラハウス", "Skulltula House", "Haus der Skulltulas", "Maison des Skulltulas", ENTR_HOUSE_OF_SKULLTULA_0, 0 },
    }}, 
    { "17:ほ゛ち", "17:Graveyard", "17:Friedhof", "17:Cimetiere", Select_LoadGame, 9, {
        { "カカリコむら　から", "From Kakariko", "Von Kakariko", "Depuis l'Apothicaire", ENTR_GRAVEYARD_ENTRANCE, 0 },
        { "やみのしんて゛ん　から", "From Shadow Temple", "Vom Schattentempel", "Depuis le Temple de l'Ombre", ENTR_GRAVEYARD_OUTSIDE_TEMPLE, 0 },
        { "はかもりのこや　から", "From Gravekeepers Hut", "Von der Huette des Totengraebers", "Depuis la Cabane du Fossoyeur", ENTR_GRAVEYARD_OUTSIDE_DAMPES_HUT, 0 },
        { "タ゛ンヘ゜イのはか　から", "From Dampes Grave", "Von Boris' Grab", "Depuis la Tombe d'Igor", ENTR_GRAVEYARD_DAMPES_GRAVE_EXIT, 0 },
        { "たてのはか　から", "From Shield Grave", "Vom Schild-Grab", "Depuis la Tombe au Bouclier", ENTR_GRAVEYARD_SHIELD_GRAVE_EXIT, 0 },
        { "リーテ゛ットのはか　から", "From Redead Grave", "Vom Zombie-Grab", "Depuis la Tombe au Effrois", ENTR_GRAVEYARD_HEART_PIECE_GRAVE_EXIT, 0 },
        { "おうけのはかあな　から", "From Royal Familys Tomb", "Vom Koenigsgrab", "Depuis la Tombe Royale", ENTR_GRAVEYARD_ROYAL_TOMB_EXIT, 0 },
        { "はかもりのこや", "Inside Dampe's Hut", "Huette des Totengraebers", "A l'interieur de la Cabane du Fossoyeur", ENTR_GRAVEKEEPERS_HUT_0, 0 },
        { "やみのノクターン　ワーフ゜", "Nocturne of Shadow Warp", "Nocturne des Schattens Teleport", "Teleporteur du Nocturne de l'Ombre", ENTR_GRAVEYARD_WARP_PAD, 0 },
    }},
    { "18:はかあな", "18:Graves", "18:Graeber", "18:Tombes", Select_LoadGame, 5, {
        { "タ゛ンヘ゜イのはか", "Dampes Grave Minigame", "Boris' Grab-Minispiel", "Tour du Cimetiere d'Igor", ENTR_WINDMILL_AND_DAMPES_GRAVE_GRAVE, 0 },
        { "おうけのはかあな", "Royal Familys Tomb", "Koenigsgrab", "Tombe Royale", ENTR_ROYAL_FAMILYS_TOMB_0, 0 },
        { "おうけのはかあな　たいようのうたカットシーン", "Royal Familys Tomb, Suns Song Cutscene", "Koenigsgrab, Hymne der Sonne Sequenz", "Tombe Royale, Cinematique du Chant du Soleil", ENTR_ROYAL_FAMILYS_TOMB_1, 0 },
        { "たからは゛このはかあな", "Treasure Chest Grave", "Schatzkisten Grab", "Tombe au Coffre", ENTR_GRAVE_WITH_FAIRYS_FOUNTAIN_0, 0 },
        { "リーテ゛ットのはか", "ReDead Grave", "Zombie Grab", "Tombe au Effrois", ENTR_REDEAD_GRAVE_0, 0 },
    }},
    { "19:テ゛スマウンテンとさ゛んと゛う", "19:Death Mountain Trail", "19:Gebirgspfad", "19:Mont du Peril", Select_LoadGame, 6, {
        { "カカリコむら　から", "From Kakariko Village", "Von Kakariko", "Depuis le Village Cocorico", ENTR_DEATH_MOUNTAIN_TRAIL_BOTTOM_EXIT, 0 },
        { "コ゛ロンシティ　から", "From Goron City", "Von Goronia", "Depuis le Village Goron", ENTR_DEATH_MOUNTAIN_TRAIL_GC_EXIT, 0 },
        { "テ゛スマウンテンかこう　から", "From Death Mountain Crater", "Vom Todeskrater", "Depuis le Cratere du Peril", ENTR_DEATH_MOUNTAIN_TRAIL_SUMMIT_EXIT, 0 },
        { "ト゛ト゛ンコ゛のと゛うくつ　から", "From Dodongos Cavern", "Von Dodongos Hoehle", "Depuis la Caverne Dodongo", ENTR_DEATH_MOUNTAIN_TRAIL_OUTSIDE_DODONGOS_CAVERN, 0 },
        { "た゛いようせい　から", "From Great Fairy", "Von der Feen-Quelle", "Depuis la Grande Fee", ENTR_DEATH_MOUNTAIN_TRAIL_GREAT_FAIRY_EXIT, 0 },
        { "た゛いようせい", "Great Fairy", "Feen-Quelle", "Grande Fee", ENTR_GREAT_FAIRYS_FOUNTAIN_MAGIC_DMT, 0 },
    }},
    { "20:コ゛ロンシティ", "20:Goron City", "20:Goronia", "20:Village Goron", Select_LoadGame, 5, {
        { "テ゛スマウンテンとさ゛んと゛う　から", "From Death Mountain Trail", "Vom Gebirgspfad", "Depuis le Mont du Peril", ENTR_GORON_CITY_UPPER_EXIT, 0 },
        { "テ゛スマウンテンかこう　から", "From Death Mountain Crater", "Vom Todeskrater", "Depuis le Cratere du Peril", ENTR_GORON_CITY_DARUNIA_ROOM_EXIT, 0 },
        { "コ゛ロンのおみせ　から", "From Goron City Shop", "Vom Goronen-Laden", "Depuis la Boutique Goron", ENTR_GORON_CITY_OUTSIDE_SHOP, 0 },
        { "まよいのもり　から", "From Lost Woods", "Von den Verlorenen Waeldern", "Depuis les Bois Perdus", ENTR_GORON_CITY_TUNNEL_SHORTCUT, 0 },
        { "コ゛ロンのおみせ", "Goron City Shop", "Goronen-Laden", "Boutique Goron", ENTR_GORON_SHOP_0, 0 },
    }},
    { "21:テ゛スマウンテンかこう", "21:Death Mountain Crater", "21:Todeskrater", "21:Cratere du Peril", Select_LoadGame, 6, {
        { "テ゛スマウンテンとさ゛んと゛う　から", "From Death Mountain Trail", "Vom Gebirgspfad", "Depuis le Mont du Peril", ENTR_DEATH_MOUNTAIN_CRATER_UPPER_EXIT, 0 },
        { "コ゛ロンシティ　から", "From Goron City", "Von Goronia", "Depuis le Village Goron", ENTR_DEATH_MOUNTAIN_CRATER_GC_EXIT, 0 },
        { "ほのおのしんて゛ん　から", "From Fire Temple", "Vom Feuertempel", "Depuis le Temple du Feu", ENTR_DEATH_MOUNTAIN_CRATER_OUTSIDE_TEMPLE, 0 },
        { "た゛いようせい　から", "From Fairy Fountain", "Von der Feen-Quelle", "Depuis la Fontaine des Fees", ENTR_DEATH_MOUNTAIN_CRATER_GREAT_FAIRY_EXIT, 0 },
        { "た゛いようせい", "Great Fairy", "Feen-Quelle", "Depuis la Grande Fee", ENTR_GREAT_FAIRYS_FOUNTAIN_MAGIC_DMC, 0 },
        { "ほのおのホ゛レロ　ワーフ゜", "Bolero of Fire Warp", "Bolero des Feuers Teleport", "Teleporteur du Bolero du Feu", ENTR_DEATH_MOUNTAIN_CRATER_WARP_PAD, 0 },
    }},
    { "22:ソ゛ーラかわ", "22:Zora River", "22:Zora-Fluss", "22:Riviere Zora", Select_LoadGame, 3, {
        { "ハイラルへいけ゛ん　から", "From Hyrule Field", "Von der Hylianischen Steppe", "Depuis la Plaine d'Hyrule", ENTR_ZORAS_RIVER_WEST_EXIT, 0 },
        { "ソ゛ーラのさと　から", "From Zoras Domain", "Von Zoras Reich", "Depuis le Domaine Zora", ENTR_ZORAS_RIVER_WATERFALL_EXIT, 0 },
        { "まよいのもり　から", "From Lost Woods", "Von den Verlorenen Waeldern", "Depuis les Bois Perdus", ENTR_ZORAS_RIVER_UNDERWATER_SHORTCUT, 0 },
    }},
    { "23:ソ゛ーラのさと", "23:Zoras Domain", "23:Zoras Reich", "23:Domaine Zora", Select_LoadGame, 5, {
        { "ソ゛ーラかわ　から", "From Zora River", "Vom Zora-Fluss", "Depuis la Riviere Zora", ENTR_ZORAS_DOMAIN_ENTRANCE, 0 },
        { "ソ゛ーラのいす゛み　から", "From Zoras Fountain", "Von Zoras Quelle", "Depuis la Fontaine Zora", ENTR_ZORAS_DOMAIN_KING_ZORA_EXIT, 0 },
        { "ハイリアこはん　から", "From Lake Hylia", "Vom Hylia-See", "Depuis le Lac Hylia", ENTR_ZORAS_DOMAIN_UNDERWATER_SHORTCUT, 0 },
        { "ソ゛ーラのおみせ　から", "From Zora Shop", "Vom Zora-Laden", "Depuis la Boutique Zora", ENTR_ZORAS_DOMAIN_OUTSIDE_SHOP, 0 },
        { "ソ゛ーラのおみせ", "Zora Shop", "Zora-Laden", "Boutique Zora", ENTR_ZORA_SHOP_0, 0 },
    }},
    { "24:ソ゛ーラのいす゛み", "24:Zoras Fountain", "24:Zoras Quelle", "24:Fontaine Zora", Select_LoadGame, 5, {
        { "ソ゛ーラのさと　から", "From Zoras Domain", "Von Zoras Reich", "Depuis le Domaine Zora", ENTR_ZORAS_FOUNTAIN_TUNNEL_EXIT, 0 },
        { "シ゛ャフ゛シ゛ャフ゛さま　から", "From Jabu Jabu", "Von Jabu-Jabu", "Depuis Jabu-Jabu", ENTR_ZORAS_FOUNTAIN_OUTSIDE_JABU_JABU, 0 },
        { "こおりのと゛うくつ　から", "From Ice Cavern", "Von der Eishoehle", "Depuis la Caverne Polaire", ENTR_ZORAS_FOUNTAIN_OUTSIDE_ICE_CAVERN, 0 },
        { "た゛いようせい　から", "From Fairy Fountain", "Von der Feen-Quelle", "Depuis la Fontaine des Fees", ENTR_ZORAS_FOUNTAIN_OUTSIDE_GREAT_FAIRY, 0 },
        { "た゛いようせい", "Great Fairy", "Feen-Quelle", "Grande Fee", ENTR_GREAT_FAIRYS_FOUNTAIN_SPELLS_FARORES_ZF, 0 },
    }},
    { "25:ハイリアこはん", "25:Lake Hylia", "25:Hylia-See", "25:Lac Hylia", Select_LoadGame, 7, {
        { "ハイラルへいけ゛ん　から", "From Hyrule Field", "Von der Hylianischen Steppe", "Depuis la Plaine d'Hyrule", ENTR_LAKE_HYLIA_NORTH_EXIT, 0 },
        { "ケ゛ルト゛のたに　から", "From Gerudo Valley", "Vom Gerudotal", "Depuis la Vallee Gerudo", ENTR_LAKE_HYLIA_RIVER_EXIT, 0 },
        { "みす゛のしんて゛ん　から", "From Water Temple", "Vom Wassertempel", "Depuis le Temple de l'Eau", ENTR_LAKE_HYLIA_OUTSIDE_TEMPLE, 0 },
        { "つりほ゛り　から", "From Fishing Pond", "Vom Fischweiher", "Depuis l'Etang", ENTR_LAKE_HYLIA_OUTSIDE_FISHING_POND, 0 },
        { "みす゛うみけんきゅうし゛ょ　から", "From Laboratory", "Vom Laboratorium", "Depuis le Laboratoire du Lac", ENTR_LAKE_HYLIA_OUTSIDE_LAB, 0 },
        { "ソ゛ーラのさと　から", "From Zoras Domain", "Von Zoras Reich", "Depuis le Domaine Zora", ENTR_LAKE_HYLIA_UNDERWATER_SHORTCUT, 0 },
        { "みす゛のセレナーテ゛　ワーフ゜", "Serenade Of Water Warp", "Serenade des Wassers Teleport", "Teleporteur de la Serenade de l'Eau", ENTR_LAKE_HYLIA_WARP_PAD, 0 },
    }},
    { "26:ハイリアこはんのたてもの", "26:Lake Hylia Buildings", "26:Hylia-See Gebaeude", "26:Batiments du Lac Hylia", Select_LoadGame, 2, {
        { "みす゛うみけんきゅうし゛ょ", "Laboratory", "Laboratorium", "Laboratoire du Lac", ENTR_LAKESIDE_LABORATORY_0, 0 },
        { "つりほ゛り", "Fishing Pond Minigame", "Fischweiher", "Etang", ENTR_FISHING_POND_0, 0 },
    }},
    { "27:ケ゛ルト゛のたに", "27:Gerudo Valley", "27:Gerudotal", "27:Vallee Gerudo", Select_LoadGame, 5, {
        { "ハイラルへいけ゛ん　から", "From Hyrule Field", "Von der Hylianischen Steppe", "Depuis la Plaine d'Hyrule", ENTR_GERUDO_VALLEY_EAST_EXIT, 0 },
        { "ケ゛ルト゛のとりて゛　から", "From Gerudo Fortress", "Von der Gerudo-Festung", "Depuis la Forteresse Gerudo", ENTR_GERUDO_VALLEY_WEST_EXIT, 0 },
        { "た゛いくのテント　から", "From Carpenter's Tent", "Vom Zelt der Zimmerleute", "Depuis la Tente du Charpentier", ENTR_GERUDO_VALLEY_OUTSIDE_TENT, 0 },
        { "た゛いくのテント/マラソンマンミニケ゛ーム", "Carpenter's Tent/ Running Man Minigame", "Zelt der Zimmerleute/ Rennlaeufer Minispiel", "Tente du Charpentier/ Marathonien", ENTR_CARPENTERS_TENT_0, 0 },
        { "とりて゛からほうりた゛されたとき", "Thrown out of Fortress", "Aus der Festung geworfen", "Expulse de la Forteresse", ENTR_GERUDO_VALLEY_1, 0 },
    }},
    { "28:ケ゛ルト゛のとりて゛", "28:Gerudo Fortress", "28:Gerudo-Festung", "28:Forteresse Gerudo", Select_LoadGame, 18, {
        { "ケ゛ルト゛のたに　から", "From Gerudo Valley", "Vom Gerudotal", "Depuis la Vallee Gerudo", ENTR_GERUDOS_FORTRESS_EAST_EXIT, 0 },
        { "ケ゛ルト゛のしゅうれんし゛ょう　から", "From Gerudo Training Ground", "Von der Gerudo-Trainingsarena", "Depuis le Gymnase Gerudo", ENTR_GERUDOS_FORTRESS_OUTSIDE_GERUDO_TRAINING_GROUND, 0 },
        { "け゛んえいのさは゛く　から", "From Haunted Wasteland", "Von der Gespensterwueste", "Depuis le Desert Hante", ENTR_GERUDOS_FORTRESS_GATE_EXIT, 0 },
        { "やふ゛さめ", "Horseback Riding Minigame", "Bogen zu Pferde Minispiel", "Archerie Montee", ENTR_GERUDOS_FORTRESS_16, 0 },
        { "ろうや", "Gerudo Fortress Jail", "Gerudo-Festung Gefaengnis", "Prison de la Forteresse Gerudo", ENTR_GERUDOS_FORTRESS_17, 0 },
        { "アシ゛ト　から(1)", "From Thieves Hideout (1)", "Vom Diebesversteck (1)", "Depuis le Repaire des Voleurs (1)", ENTR_GERUDOS_FORTRESS_1, 0 },
        { "アシ゛ト　から(2)", "From Thieves Hideout (2)", "Vom Diebesversteck (2)", "Depuis le Repaire des Voleurs (2)", ENTR_GERUDOS_FORTRESS_2, 0 },
        { "アシ゛ト　から(3)", "From Thieves Hideout (3)", "Vom Diebesversteck (3)", "Depuis le Repaire des Voleurs (3)", ENTR_GERUDOS_FORTRESS_3, 0 },
        { "アシ゛ト　から(4)", "From Thieves Hideout (4)", "Vom Diebesversteck (4)", "Depuis le Repaire des Voleurs (4)", ENTR_GERUDOS_FORTRESS_4, 0 },
        { "アシ゛ト　から(5)", "From Thieves Hideout (5)", "Vom Diebesversteck (5)", "Depuis le Repaire des Voleurs (5)", ENTR_GERUDOS_FORTRESS_5, 0 },
        { "アシ゛ト　から(6)", "From Thieves Hideout (6)", "Vom Diebesversteck (6)", "Depuis le Repaire des Voleurs (6)", ENTR_GERUDOS_FORTRESS_6, 0 },
        { "アシ゛ト　から(7)", "From Thieves Hideout (7)", "Vom Diebesversteck (7)", "Depuis le Repaire des Voleurs (7)", ENTR_GERUDOS_FORTRESS_7, 0 },
        { "アシ゛ト　から(8)", "From Thieves Hideout (8)", "Vom Diebesversteck (8)", "Depuis le Repaire des Voleurs (8)", ENTR_GERUDOS_FORTRESS_8, 0 },
        { "アシ゛ト　から(9)", "From Thieves Hideout (9)", "Vom Diebesversteck (9)", "Depuis le Repaire des Voleurs (9)", ENTR_GERUDOS_FORTRESS_9, 0 },
        { "アシ゛ト　から(10)", "From Thieves Hideout (10)", "Vom Diebesversteck (10)", "Depuis le Repaire des Voleurs (10)", ENTR_GERUDOS_FORTRESS_10, 0 },
        { "アシ゛ト　から(11)", "From Thieves Hideout (11)", "Vom Diebesversteck (11)", "Depuis le Repaire des Voleurs (11)", ENTR_GERUDOS_FORTRESS_11, 0 },
        { "アシ゛ト　から(12)", "From Thieves Hideout (12)", "Vom Diebesversteck (12)", "Depuis le Repaire des Voleurs (12)", ENTR_GERUDOS_FORTRESS_12, 0 },
        { "アシ゛ト　から(13)", "From Thieves Hideout (13)", "Vom Diebesversteck (13)", "Depuis le Repaire des Voleurs (13)", ENTR_GERUDOS_FORTRESS_13, 0 },
    }},
    { "29:とうそ゛くた゛んのアシ゛ト", "29:Thieves Hideout", "29:Diebesversteck", "29:Repaire des Voleurs", Select_LoadGame, 13, {
        { "ケ゛ルト゛のとりて゛　から(1)", "From Gerudo Fortress (1)", "Von der Gerudo-Festung (1)", "Depuis la Forteresse Gerudo (1)", ENTR_THIEVES_HIDEOUT_0, 0 },
        { "ケ゛ルト゛のとりて゛　から(2)", "From Gerudo Fortress (2)", "Von der Gerudo-Festung (2)", "Depuis la Forteresse Gerudo (2)", ENTR_THIEVES_HIDEOUT_1, 0 },
        { "ケ゛ルト゛のとりて゛　から(3)", "From Gerudo Fortress (3)", "Von der Gerudo-Festung (3)", "Depuis la Forteresse Gerudo (3)", ENTR_THIEVES_HIDEOUT_2, 0 },
        { "ケ゛ルト゛のとりて゛　から(4)", "From Gerudo Fortress (4)", "Von der Gerudo-Festung (4)", "Depuis la Forteresse Gerudo (4)", ENTR_THIEVES_HIDEOUT_3, 0 },
        { "ケ゛ルト゛のとりて゛　から(5)", "From Gerudo Fortress (5)", "Von der Gerudo-Festung (5)", "Depuis la Forteresse Gerudo (5)", ENTR_THIEVES_HIDEOUT_4, 0 },
        { "ケ゛ルト゛のとりて゛　から(6)", "From Gerudo Fortress (6)", "Von der Gerudo-Festung (6)", "Depuis la Forteresse Gerudo (6)", ENTR_THIEVES_HIDEOUT_5, 0 },
        { "ケ゛ルト゛のとりて゛　から(7)", "From Gerudo Fortress (7)", "Von der Gerudo-Festung (7)", "Depuis la Forteresse Gerudo (7)", ENTR_THIEVES_HIDEOUT_6, 0 },
        { "ケ゛ルト゛のとりて゛　から(8)", "From Gerudo Fortress (8)", "Von der Gerudo-Festung (8)", "Depuis la Forteresse Gerudo (8)", ENTR_THIEVES_HIDEOUT_7, 0 },
        { "ケ゛ルト゛のとりて゛　から(9)", "From Gerudo Fortress (9)", "Von der Gerudo-Festung (9)", "Depuis la Forteresse Gerudo (9)", ENTR_THIEVES_HIDEOUT_8, 0 },
        { "ケ゛ルト゛のとりて゛　から(10)", "From Gerudo Fortress (10)", "Von der Gerudo-Festung (10)", "Depuis la Forteresse Gerudo (10)", ENTR_THIEVES_HIDEOUT_9, 0 },
        { "ケ゛ルト゛のとりて゛　から(11)", "From Gerudo Fortress (11)", "Von der Gerudo-Festung (11)", "Depuis la Forteresse Gerudo (11)", ENTR_THIEVES_HIDEOUT_10, 0 },
        { "ケ゛ルト゛のとりて゛　から(12)", "From Gerudo Fortress (12)", "Von der Gerudo-Festung (12)", "Depuis la Forteresse Gerudo (12)", ENTR_THIEVES_HIDEOUT_11, 0 },
        { "ケ゛ルト゛のとりて゛　から(13)", "From Gerudo Fortress (13)", "Von der Gerudo-Festung (13)", "Depuis la Forteresse Gerudo (13)", ENTR_THIEVES_HIDEOUT_12, 0 },
    }},
    { "30:け゛んえいのさは゛く", "30:Haunted Wasteland", "30:Geisterwueste", "30:Desert Hante", Select_LoadGame, 2, {
        { "ケ゛ルト゛のとりて゛　から", "From Gerudo Fortress", "Von der Gerudo-Festung", "Depuis la Forteresse Gerudo", ENTR_HAUNTED_WASTELAND_EAST_EXIT, 0 },
        { "きょた゛いし゛ゃしんそ゛う　から", "From Desert Colossus", "Vom Wuestenkoloss", "Depuis le Colosse du Desert", ENTR_HAUNTED_WASTELAND_WEST_EXIT, 0 },
    }},
    { "31:きょた゛いし゛ゃしんそ゛う", "31:Desert Colossus", "31:Wuestenkoloss", "31:Colosse du Desert", Select_LoadGame, 7, {
        { "け゛んえいのさは゛く　から", "From Haunted Wasteland", "Von der Geisterwueste", "Depuis le Desert Hante", ENTR_DESERT_COLOSSUS_EAST_EXIT, 0 },
        { "たましいのしんて゛ん　から", "From Spirit Temple", "Vom Geistertempel", "Depuis le Temple de l'Esprit", ENTR_DESERT_COLOSSUS_OUTSIDE_TEMPLE, 0 },
        { "たましいのしんて゛ん　から(ひた゛りて)", "From Spirit Temple (Left Hand)", "Vom Geistertempel (Linke Hand)", "Depuis le Temple de l'Esprit (Main Gauche)", ENTR_DESERT_COLOSSUS_2, 0 },
        { "たましいのしんて゛ん　から(みき゛て)", "From Spirit Temple (Right Hand)", "Vom Geistertempel (Rechte Hand)", "Depuis le Temple de l'Esprit (Main Droite)", ENTR_DESERT_COLOSSUS_3, 0 },
        { "た゛いようせい　から", "From Fairy Fountain", "Von der Feen-Quelle", "Depuis la Fontaine des Fees", ENTR_DESERT_COLOSSUS_GREAT_FAIRY_EXIT, 0 },
        { "た゛いようせい", "Great Fairy", "Feen-Quelle", "Grande Fee", ENTR_GREAT_FAIRYS_FOUNTAIN_SPELLS_NAYRUS_COLOSSUS, 0 },
        { "たましいのレクイエム　ワーフ゜", "Requiem of Spirit Warp", "Requiem der Geister Teleport", "Teleporteur du Requiem de l'Esprit", ENTR_DESERT_COLOSSUS_WARP_PAD, 0 },
    }},
    { "32:テ゛クのきサマのなか", "32:Deku Tree", "32:Deku-Baum", "32:Arbre Mojo", Select_LoadGame, 3, {
        { "いりく゛ち", "Entrance", "Eingang", "Entree", ENTR_DEKU_TREE_ENTRANCE, 1 },
        { "コ゛ーマのへや　から", "From Gohma's Lair", "Von Gohmas Kampf", "Depuis le Repaire de Gohma", ENTR_DEKU_TREE_BOSS_DOOR, 1 },
        { "コ゛ーマのへや", "Gohma's Lair", "Gohmas Kampf", "Repaire de Gohma", ENTR_DEKU_TREE_BOSS_ENTRANCE, 0 },
    }},
    { "33:ト゛ト゛ンコ゛のと゛うくつ", "33:Dodongos Cavern", "33:Dodongos Hoehle", "33:Caverne Dodongo", Select_LoadGame, 3, {
        { "いりく゛ち", "Entrance", "Eingang", "Entree", ENTR_DODONGOS_CAVERN_ENTRANCE, 1 },
        { "キンク゛ト゛ト゛ンコ゛のへや　から", "From King Dodongo", "Von Koenig Dodongo", "Depuis le Repaire du Roi Dodongo", ENTR_DODONGOS_CAVERN_BOSS_DOOR, 1 },
        { "キンク゛ト゛ト゛ンコ゛のへや", "King Dodongo's Lair", "Koenig Dodongos Kampf", "Repaire du Roi Dodongo", ENTR_DODONGOS_CAVERN_BOSS_ENTRANCE, 0 },
    }},
    { "34:シ゛ャフ゛シ゛ャフ゛さまのおなか", "34:Jabu Jabu", "34:Jabu-Jabu", "34:Jabu-Jabu", Select_LoadGame, 2, {
        { "いりく゛ち", "Entrance", "Eingang", "Entree", ENTR_JABU_JABU_ENTRANCE, 1 },
        { "ハ゛リネート゛のへや", "Barinade's Lair", "Barinades Kampf", "Repaire de Barinade", ENTR_JABU_JABU_BOSS_ENTRANCE, 0 },
    }},
    { "35:もりのしんて゛ん", "35:Forest Temple", "35:Waldtempel", "35:Temple de la Foret", Select_LoadGame, 4, {
        { "いりく゛ち", "Entrance", "Eingang", "Entree", ENTR_FOREST_TEMPLE_ENTRANCE, 1 },
        { "てんし゛ょうか゛おちてくるへや", "Crushing Room", "Der Fallende Decke Raum", "Salle de Broyage", ENTR_FOREST_TEMPLE_2, 1 },
        { "ファントムカ゛ノンまえ", "Before Phantom Ganon", "Vor Phantom-Ganon", "Avant Ganon Spectral", ENTR_FOREST_TEMPLE_BOSS_DOOR, 1 },
        { "ファントムカ゛ノンのへや", "Phantom Ganon's Lair", "Phantom-Ganons Kampf", "Repaire de Ganon Spectral", ENTR_FOREST_TEMPLE_BOSS_ENTRANCE, 0 },
    }},
    { "36:ほのおのしんて゛ん", "36:Fire Temple", "36:Feuertempel", "36:Temple du Feu", Select_LoadGame, 3, {
        { "いりく゛ち", "Entrance", "Eingang", "Entrance", ENTR_FIRE_TEMPLE_ENTRANCE, 1 },
        { "ウ゛ァルハ゛シ゛アまえ", "Before Volvagia", "Vor Volvagia", "Avant Volvagia", ENTR_FIRE_TEMPLE_BOSS_DOOR, 1 },
        { "ウ゛ァルハ゛シ゛アのへや", "Volvagia's Lair", "Volvagias Kampf", "Repaire de Volcania", ENTR_FIRE_TEMPLE_BOSS_ENTRANCE, 0 },
    }},
    { "37:みす゛のしんて゛ん", "37:Water Temple", "37:Wassertempel", "37:Temple de l'Eau", Select_LoadGame, 2, {
        { "いりく゛ち", "Entrance", "Eingang", "Entree", ENTR_WATER_TEMPLE_ENTRANCE, 1 },
        { "モーファのへや", "Morpha's Lair", "Morphas Kampf", "Repaire de Morpha", ENTR_WATER_TEMPLE_BOSS_ENTRANCE, 0 },
    }},
    { "38:やみのしんて゛ん", "38:Shadow Temple", "38:Schattentempel", "38:Temple de l'Ombre", Select_LoadGame, 3, {
        { "いりく゛ち", "Entrance", "Eingang", "Entree", ENTR_SHADOW_TEMPLE_ENTRANCE, 1 },
        { "ホ゛ンコ゛ホ゛ンコ゛まえ", "Outside Bongo Bongo", "Vor Bongo Bongo", "Avant Bongo Bongo", ENTR_SHADOW_TEMPLE_BOSS_DOOR, 1 },
        { "ホ゛ンコ゛ホ゛ンコ゛のへや", "Bongo Bongo's Lair", "Bongo Bongos Kampf", "Repaire de Bongo Bongo", ENTR_SHADOW_TEMPLE_BOSS_ENTRANCE, 1 },
    }},
    { "39:たましいのしんて゛ん", "39:Spirit Temple", "39:Geistertempel", "39:Temple de l'Esprit", Select_LoadGame, 6, {
        { "いりく゛ち", "Entrance", "Eingang", "Entree", ENTR_SPIRIT_TEMPLE_ENTRANCE, 1 },
        { "ひた゛りて　から", "From Left Hand", "Von der linken Hand", "Depuis la Main Gauche", ENTR_SPIRIT_TEMPLE_2, 1 },
        { "みき゛て　から", "From Right Hand", "Von der rechten Hand", "Depuis la Main Droite", ENTR_SPIRIT_TEMPLE_3, 1 },
        { "ツインローハ゛まえ", "Before Twinrova", "Vor den Killa Ohmaz", "Avant le Duo Malefique", ENTR_SPIRIT_TEMPLE_BOSS_DOOR, 1 },
        { "ナホ゛ールせん", "Nabooru Fight", "Naborus Kampf", "Combat contre Nabooru", ENTR_SPIRIT_TEMPLE_BOSS_ENTRANCE, 0 },
        { "ツインローハ゛のへや", "Twinrova's Lair", "Killa Ohmaz' Kampf", "Repaire du Duo Malefique", ENTR_SPIRIT_TEMPLE_BOSS_2, 0 },
    }},
    { "40:カ゛ノンし゛ょう", "40:Ganons Castle", "40:Ganons Schloss", "40:Chateau de Ganon", Select_LoadGame, 9, {
        { "いりく゛ち", "Entrance", "Eingang", "Entree", ENTR_INSIDE_GANONS_CASTLE_ENTRANCE, 1 },
        { "カ゛ノンのとう　から", "From Tower", "Vom Tower", "Depuis la Tour", ENTR_INSIDE_GANONS_CASTLE_1, 1 },
        { "カ゛ノンへのかいた゛ん - しろから", "Stairs to Lair - From Castle", "Stufen zum Verlies - Vom Schloss", "Escaliers vers Repaire - Depuis le Chateau", ENTR_GANONS_TOWER_0, 0 },
        { "カ゛ノンへのかいた゛ん - カ゛ノント゛ロフのへやから", "Stairs to Lair - From Ganondorf's Lair", "Stufen zum Verlies - Von Ganondorfs Verlies", "Escaliers vers Repaire - Depuis le Repaire de Ganondorf", ENTR_GANONS_TOWER_1, 0 },
        { "カ゛ノント゛ロフのへや", "Ganondorf's Lair", "Ganondorfs Verlies", "Repaire de Ganondorf", ENTR_GANONDORF_BOSS_0, 0 },
        { "カ゛ノント゛ロフをたおしたあと", "Ganondorf Defeated", "Ganondorf Besiegt", "Ganondorf Vaincu", ENTR_GANONS_TOWER_COLLAPSE_EXTERIOR_5, 0 },
        { "カ゛ノント゛ロフをたおしたあと(2)", "Ganondorf Defeated (2)", "Ganondorf Besiegt (2)", "Ganondorf Vaincu (2)", ENTR_GANONS_TOWER_COLLAPSE_EXTERIOR_1, 0 },
        { "カ゛ノンのへや", "Ganon's Lair", "Ganons Kampf", "Repaire de Ganon", ENTR_GANON_BOSS_0, 0 },
        { "カ゛ノンのさいこ゛カットシーン", "Ganon Death Cutscene", "Ganons Todessequenz", "Cinematique de la Mort de Ganon", ENTR_GANONS_TOWER_COLLAPSE_EXTERIOR_0, 0 },
    }},
    { "41:いと゛のそこ", "41:Bottom of the Well", "41:Grund des Brunnens", "41:Puits", Select_LoadGame, 1, {
        { "いりく゛ち", "Entrance", "Eingang", "Entree", ENTR_BOTTOM_OF_THE_WELL_ENTRANCE, 1 },
    }},
    { "42:こおりのと゛うくつ", "42:Ice Cavern", "42:Eishoehle", "42:Caverne Polaire", Select_LoadGame, 1, {
        { "いりく゛ち", "Entrance", "Eingang", "Entree", ENTR_ICE_CAVERN_ENTRANCE, 1 },
    }},
    { "43:ケ゛ルト゛のしゅうれんし゛ょう", "43:Gerudo Training Ground", "43:Gerudo-Trainingsarena", "43:Gymnase Gerudo", Select_LoadGame, 1, {
        { "いりく゛ち", "Entrance", "Eingang", "Entree", ENTR_GERUDO_TRAINING_GROUND_ENTRANCE, 1 },
    }},
    { "44:ワーフ゜", "44:Warps", "44:Teleportpunkte", "44:Teleporteurs", Select_LoadGame, 6, {
        { "ひかりのフ゜レリュート゛　ワーフ゜", "Prelude of Light Warp", "Kantate des Lichts Teleport", "Teleporteur du Prelude de la Lumiere", ENTR_TEMPLE_OF_TIME_WARP_PAD, 0 },
        { "もりのメヌエット　ワーフ゜", "Minuet of Forest Warp", "Menuett des Waldes Teleport", "Teleporteur du Menuet des Bois", ENTR_SACRED_FOREST_MEADOW_WARP_PAD, 0 },
        { "ほのおのホ゛レロ　ワーフ゜", "Bolero of Fire Warp", "Bolero des Feuers Teleport", "Teleporteur du Bolero du Feu", ENTR_DEATH_MOUNTAIN_CRATER_WARP_PAD, 0 },
        { "みす゛のセレナーテ゛　ワーフ゜", "Serenade Of Water Warp", "Serenade des Wassers Teleport", "Teleporteur de la Serenade de l'Eau", ENTR_LAKE_HYLIA_WARP_PAD, 0 },
        { "やみのノクターン　ワーフ゜", "Nocturne of Shadow Warp", "Nocturne des Schattens Teleport", "Teleporteur du Nocturne de l'Ombre", ENTR_GRAVEYARD_WARP_PAD, 0 },
        { "たましいのレクイエム　ワーフ゜", "Requiem of Spirit Warp", "Requiem der Geister Teleport", "Teleporteur du Requiem de l'Esprit", ENTR_DESERT_COLOSSUS_WARP_PAD, 0 },
    }},
    { "45:おみせ", "45:Shops", "45:Laeden", "45:Boutiques", Select_LoadGame, 9, {
        { "コキリのおみせ", "Kokiri Shop", "Kokiri-Laden", "Boutique Kokiri", ENTR_KOKIRI_SHOP_0 },
        { "くすりや(し゛ょうかまち)", "Potion Shop (Market)", "Magie-Laden (Marktplatz)", "Apothicaire (Place du Marche)", ENTR_POTION_SHOP_MARKET_0, 0 },
        { "なンて゛もや(し゛ょうかまち)", "Bazaar Shop (Market)", "Basar (Marktplatz)", "Bazar (Place du Marche)", ENTR_BAZAAR_1, 0 },
        { "おめんや", "Happy Mask Shop", "Maskenhaendler", "Foire aux Masques", ENTR_HAPPY_MASK_SHOP_0, 0 },
        { "ホ゛ムチュウや", "Bombchu Shop", "Krabbelminen-Laden", "Boutique de Missiles", ENTR_BOMBCHU_SHOP_1, 0 },
        { "なンて゛もや(カカリコむら)", "Bazaar Shop (Kakariko)", "Basar (Kakariko)", "Bazar (Village Cocorico)", ENTR_BAZAAR_0, 0 },
        { "くすりや(カカリコむら)", "Potion Shop (Kakariko)", "Magie-Laden (Kakariko)", "Apothicaire (Village Cocorico)", ENTR_POTION_SHOP_KAKARIKO_FRONT, 0 },
        { "コ゛ロンのおみせ", "Goron City Shop", "Goronen-Laden", "Boutique Goron", ENTR_GORON_SHOP_0, 0 },
        { "ソ゛ーラのおみせ", "Zora Shop", "Zora-Laden", "Boutique Zora", ENTR_ZORA_SHOP_0, 0 },
    }},
    { "46:た゛いようせい", "46:Great Fairies", "46:Feen-Quellen", "46:Grandes Fees", Select_LoadGame, 6, {
        { "ハイラルし゛ょう(こと゛も)", "Hyrule Castle (Child)", "Schloss Hyrule (Kind)", "Chateau d'Hyrule (Enfant)", ENTR_GREAT_FAIRYS_FOUNTAIN_SPELLS_DINS_HC, 0 },
        { "ハイラルし゛ょう(おとな)", "Hyrule Castle (Adult)", "Schloss Hyrule (Erwachsener)", "Chateau d'Hyrule (Adult)", ENTR_GREAT_FAIRYS_FOUNTAIN_MAGIC_OGC_DD, 0 },
        { "テ゛スマウンテンとさ゛んと゛う", "Death Mountain Trail", "Gebirgspfad", "Mont du Peril", ENTR_GREAT_FAIRYS_FOUNTAIN_MAGIC_DMT, 0 },
        { "テ゛スマウンテンかこう", "Death Mountain Crater", "Todeskrater", "Cratere du Peril", ENTR_GREAT_FAIRYS_FOUNTAIN_MAGIC_DMC, 0 },
        { "ソ゛ーラのいす゛み", "Zoras Fountain", "Zoras Quelle", "Fontaine Zora", ENTR_GREAT_FAIRYS_FOUNTAIN_SPELLS_FARORES_ZF, 0 },
        { "きょた゛いし゛ゃしんそ゛う", "Desert Colossus", "Wuestenkoloss", "Colosse du Desert", ENTR_GREAT_FAIRYS_FOUNTAIN_SPELLS_NAYRUS_COLOSSUS, 0 },
    }},
    { "47:たからは゛この　あな", "47:Chest Grottos", "47:Truhengrotten", "47:Grottes a Coffres", Select_Grotto_LoadGame, 11, {
        { "コキリのもり(あらしのうた)", "Kokiri Forest (Song of Storms)", "Kokiri-Wald (Hymne des Sturms)", "Foret Kokiri (Chant des Tempetes)", 0x00, 0 },
        { "まよいのもり", "Lost Woods", "Verlorene Waelder", "Bois Perdus", 0x01, 0 },
        { "もりのせいいき", "Sacred Forest Meadow", "Waldlichtung", "Bosquet Sacre", 0x02, 0 },
        { "ハイラルへいけ゛ん(し゛ょうかまちのちかく)", "Hyrule Field (Near Market)", "Hylianische Steppe (Nahe Marktplatz)", "Plaine d'Hyrule (Pres de la Place du Marche)", 0x03, 0 },
        { "ハイラルへいけ゛ん(オーフ゜ン いけちかく)", "Hyrule Field (Open Near Lake)", "Hylianische Steppe (Offen, beim Hylia-See)", "Plaine d'Hyrule (Deja ouverte pres du Lac)", 0x04, 0 },
        { "ハイラルへいけ゛ん(なんとうのいわ)", "Hyrule Field (SE Boulder)", "Hylianische Steppe (Felsen, S-O)", "Plaine d'Hyrule (Rocher a Gantelets)", 0x05, 0 },
        { "カカリコむら(オーフ゜ン)", "Kakariko (Open)", "Kakariko (Offen)", "Village Cocorico (Deja Ouverte)", 0x06, 0 },
        { "カカリコむら(リーテ゛ット)", "Kakariko (Redead)", "Kakariko (Zombies)", "Village Cocorico (Effrois)", 0x07, 0 },
        { "テ゛スマウンテンとさ゛んと゛う(あらしのうた)", "Death Mountain Trail (Song of Storms)", "Gebirgspfad (Hymne des Sturms)", "Mont du Peril (Chant des Tempetes)", 0x08, 0 },
        { "テ゛スマウンテンかこう", "Death Mountain Crater", "Todeskrater", "Cratere du Peril", 0x09, 0 },
        { "ソ゛ーラかわ(オーフ゜ン)", "Zora River (Open)", "Zora-Fluss (Offen)", "Riviere Zora (Deja Ouverte)", 0x0A, 0 },
    }},
    { "48:アキント゛ナッツの　あな", "48:Scrub Grottos", "48:Laubkerlgrotten", "48:Grottes des Pestes Marchandes", Select_Grotto_LoadGame, 10, {
        { "ハイラルへいけ゛ん(いけちかく)", "Hyrule Field (Near Lake)", "Hylianische Steppe (beim Hylia-See)", "Plaine d'Hyrule (Pres du Lac)", 0x0B, 0 },
        { "テ゛スマウンテンかこう", "Death Mountain Crater", "Todeskrater", "Cratere du Peril", 0x0C, 0 },
        { "コ゛ロンシティ", "Goron City", "Goronia", "Village Goron", 0x0D, 0 },
        { "ロンロンほ゛くし゛ょう", "Lon Lon Ranch", "Lon Lon-Farm", "Ranch Lon Lon", 0x0E, 0 },
        { "ハイリアこはん", "Lake Hylia", "Hylia-See", "Lac Hylia", 0x0F, 0 },
        { "まよいのもり", "Lost Woods", "Verlorene Waelder", "Bois Perdus", 0x10, 0 },
        { "ソ゛ーラかわ(あらしのうた)", "Zora River (Song of Storms)", "Zora-Fluss (Hymne des Sturms)", "Rivere Zora (Chant des Tempetes)", 0x11, 0 },
        { "もりのせいいき(あらしのうた)", "Sacred Forest Meadow (Song of Storms)", "Waldlichtung (Hymne des Sturms)", "Bosquet Scare (Chant des Tempetes)", 0x12, 0 },
        { "ケ゛ルト゛のたに(あらしのうた)", "Gerudo Valley (Song of Storms)", "Gerudotal (Hymne des Sturms)", "Vallee Gerudo (Chant des Tempetes)", 0x13, 0 },
        { "きょた゛いし゛ゃしんそ゛う", "Desert Colossus", "Wuestenkoloss", "Colosse du Desert", 0x14, 0 },
    }},
    { "49:そのたの　あな", "49:Other Grottos", "49:Andere Grotten", "49:Autres Grottes", Select_Grotto_LoadGame, 7, {
        { "もりのステーシ゛", "Scrub Theatre", "Waldbuehne", "Theatre Mojo", 0x15, 0 },
        { "クモのあな(ハイラルへいけ゛ん)", "Spider Grotto (Hyrule Field)", "Spinnen-Grotte (Hylianische Steppe)", "Spider Araignee (Plaine d'Hyrule)", 0x16, 0 },
        { "クモのあな(ハイラルし゛ょう)", "Spider Grotto (Hyrule Castle)", "Spinnen-Grotte (Schloss Hyrule)", "Grotte Araignee (Chateau d'Hyrule)", 0x17, 0 },
        { "ウシのあな(ハイラルへいけ゛ん)", "Cow Grotto (Hyrule Field)", "Kuh-Grotte (Hylianische Steppe)", "Grotte a Vache (Plaine d'Hyrule)", 0x18, 0 },
        { "ウシのあな(テ゛スマウンテンとさ゛んと゛う)", "Cow Grotto (Death Mountain Trail)", "Kuh-Grotte (Gebirgspfad)", "Grotte a Vache (Chemin du Peril)", 0x19, 0 },
        { "みす゛のあな(ケ゛ルト゛のたに)", "Flooded Grotto (Gerudo Valley)", "Geflutete Grotte (Gerudotal)", "Grotte Inondee (Vallee Gerudo)", 0x1A, 0 },
        { "みす゛のあな(ハイラルへいけ゛ん)", "Flooded Grotto (Hyrule Field)", "Geflutete Grotte (Hylianische Steppe)", "Grotte Inondee (Plaine d'Hyrule)", 0x1B, 0 },
    }},
    { "50:テ゛ハ゛ック゛(しようちゅうい)", "50:Debug (Use with caution)", "50:Debug (Mit Vorsicht benutzen)", "50:Debug (A utiliser avec prudence)", Select_LoadGame, 10, {
        { "テストルーム", "Test Room", "Test Raum", "Salle de Test", ENTR_BESITU_0, 0 },
        { "SRDマッフ゜(ささテスト)", "SRD Map", "SRD Karte", "Carte SRD", ENTR_SASATEST_0, 0 },
        { "テストマッフ゜", "Test Map", "Test Karte", "Carte de Test", ENTR_TEST01_0, 0 },
        { "たからは゛こワーフ゜", "Treasure Chest Warp", "Schatzkisten Teleport", "Salle de Test - Objets", ENTR_TESTROOM_0, 0 },
        { "ちゅうスタルフォスへ゛や", "Stalfos Miniboss Room", "Stalfos-Ritter Miniboss-Raum", "Salle du Minoboss Stalfos", ENTR_SYOTES_0, 0 },
        { "ホ゛ススタルフォスへ゛や", "Stalfos Boss Room", "Stalfos-Ritter Boss-Raum", "Salle de Boss Stalfos", ENTR_SYOTES2_0, 0 },
        { "タ゛ークリンクへ゛や(Sutaru)", "Dark Link Room", "Schwarzer Link Raum", "Salle de Dark Link", ENTR_SUTARU_0, 0 },
        { "まとあてやのコヒ゜ー(jikkenjyou)", "Shooting Gallery Duplicate", "Schiessbude (Duplikat)", "Jeu d'Adresse (Duplicata)", ENTR_TEST_SHOOTING_GALLERY_0, 0 },
        { "depthテスト", "Depth Test", "Tiefen Test", "Test de Profondeur", ENTR_DEPTH_TEST_0, 0 },
        { "ハイラルにわケ゛ーム2(こわれてます)", "Hyrule Garden Game (Broken)", "Burghof - Wachen-Minispiel (Kaputt)", "Cour du chateau (Non Fonctionnel)", ENTR_HAIRAL_NIWA2_0, 0 },
    }},
    // clang-format on
};

static BetterSceneSelectGrottoData sBetterGrottos[] = {
    // clang-format off
    // Kokiri Forest -> KF Storms Grotto
    { ENTR_GROTTOS_0, ENTR_KOKIRI_FOREST_0, 0, 0x2C, SCENE_KOKIRI_FOREST, { -504.0, 380.0, -1224.0 } },
    // Lost Woods -> LW Near Shortcuts Grotto
    { ENTR_GROTTOS_0, ENTR_LOST_WOODS_TUNNEL_SHORTCUT, 2, 0x14, SCENE_LOST_WOODS, { 922.0, 0.0, -933.0 } },
    // SFM Entryway -> SFM Wolfos Grotto
    { ENTR_GROTTOS_8, ENTR_SACRED_FOREST_MEADOW_SOUTH_EXIT, 0, 0xED, SCENE_SACRED_FOREST_MEADOW, { -201.0, 0.0, 1906.0 } },
    // Hyrule Field -> HF Near Market Grotto
    { ENTR_GROTTOS_0, ENTR_HYRULE_FIELD_PAST_BRIDGE_SPAWN, 0, 0x00, SCENE_HYRULE_FIELD, { -1428.0, 0.0, 790.0 } },
    // Hyrule Field -> HF Open Grotto
    { ENTR_GROTTOS_0, ENTR_HYRULE_FIELD_FENCE_EXIT, 0, 0x03, SCENE_HYRULE_FIELD, { -4026.0, -700.0, 13858.0 } },
    // Hyrule Field -> HF Southeast Grotto
    { ENTR_GROTTOS_0, ENTR_HYRULE_FIELD_FENCE_EXIT, 0, 0x22, SCENE_HYRULE_FIELD, { -259.0, -500.0, 12356.0 } },
    // Kak Backyard -> Kak Open Grotto
    { ENTR_GROTTOS_0, ENTR_KAKARIKO_VILLAGE_OUTSIDE_SHOP_GRANNY, 0, 0x28, SCENE_KAKARIKO_VILLAGE, { 861.0, 80.0, -253.0 } },
    // Kakariko Village -> Kak Redead Grotto
    { ENTR_GROTTOS_3, ENTR_KAKARIKO_VILLAGE_OUTSIDE_SHOP_GRANNY, 0, 0xE7, SCENE_KAKARIKO_VILLAGE, { -400.0, 0.0, 408.0 } },
    // Death Mountain -> DMT Storms Grotto
    { ENTR_GROTTOS_0, ENTR_DEATH_MOUNTAIN_TRAIL_GC_EXIT, 0, 0x57, SCENE_DEATH_MOUNTAIN_TRAIL, { -389.0, 1386.0, -1202.0 } },
    // DMC Upper Nearby -> DMC Upper Grotto
    { ENTR_GROTTOS_0, ENTR_DEATH_MOUNTAIN_CRATER_UPPER_EXIT, 1, 0x7A, SCENE_DEATH_MOUNTAIN_CRATER, { 50.0, 1233.0, 1776.0 } },
    // Zora River -> ZR Open Grotto
    { ENTR_GROTTOS_0, ENTR_ZORAS_RIVER_WATERFALL_EXIT, 0, 0x29, SCENE_ZORAS_RIVER, { 369.0, 570.0, 128.0 } },
    // Hyrule Field -> HF Inside Fence Grotto
    { ENTR_GROTTOS_2, ENTR_HYRULE_FIELD_FENCE_EXIT, 0, 0xE6, SCENE_HYRULE_FIELD, { -5002.0, -700.0, 13823.0 } },
    // DMC Lower Nearby -> DMC Hammer Grotto
    { ENTR_GROTTOS_4, ENTR_DEATH_MOUNTAIN_CRATER_GC_EXIT, 1, 0xF9, SCENE_DEATH_MOUNTAIN_CRATER, { -1703.0, 722.0, -481.0 } },
    // GC Grotto Platform -> GC Grotto
    { ENTR_GROTTOS_4, ENTR_GORON_CITY_UPPER_EXIT, 3, 0xFB, SCENE_GORON_CITY, { 1091.0, 580.0, -1192.0 } },
    // Lon Lon Ranch -> LLR Grotto
    { ENTR_GROTTOS_4, ENTR_LON_LON_RANCH_OUTSIDE_TOWER, 0, 0xFC, SCENE_LON_LON_RANCH, { 1798.0, 0.0, 1498.0 } },
    // Lake Hylia -> LH Grotto
    { ENTR_GROTTOS_4, ENTR_LAKE_HYLIA_OUTSIDE_TEMPLE, 0, 0xEF, SCENE_LAKE_HYLIA, { -3044.0, -1033.0, 6070.0 } },
    // LW Beyond Mido -> LW Scrubs Grotto
    { ENTR_GROTTOS_7, ENTR_LOST_WOODS_NORTH_EXIT, 8, 0xF5, SCENE_LOST_WOODS, { 677.0, 0.0, -2515.0 } },
    // Zora River -> ZR Storms Grotto
    { ENTR_GROTTOS_10, ENTR_ZORAS_RIVER_WEST_EXIT, 0, 0xEB, SCENE_ZORAS_RIVER, { -1632.0, 100.0, -123.0 } },
    // Sacred Forest Meadow -> SFM Storms Grotto
    { ENTR_GROTTOS_10, ENTR_SACRED_FOREST_MEADOW_OUTSIDE_TEMPLE, 0, 0xEE, SCENE_SACRED_FOREST_MEADOW, { 317.0, 480.0, -2303.0 } },
    // GV Fortress Side -> GV Storms Grotto
    { ENTR_GROTTOS_10, ENTR_GERUDO_VALLEY_OUTSIDE_TENT, 0, 0xF0, SCENE_GERUDO_VALLEY, { -1321.0, 15.0, -968.0 } },
    // Desert Colossus -> Colossus Grotto
    { ENTR_GROTTOS_10, ENTR_DESERT_COLOSSUS_WARP_PAD, 0, 0xFD, SCENE_DESERT_COLOSSUS, { 71.0, -32.0, -1303.0 } },
    // LW Beyond Mido -> Deku Theater
    { ENTR_GROTTOS_12, ENTR_LOST_WOODS_TUNNEL_SHORTCUT, 6, 0xF3, SCENE_LOST_WOODS, { 75.0, -20.0, -1596.0 } },
    // Hyrule Field -> HF Near Kak Grotto
    { ENTR_GROTTOS_1, ENTR_HYRULE_FIELD_STAIRS_EXIT, 0, 0xE5, SCENE_HYRULE_FIELD, { 2059.0, 20.0, -174.0 } },
    // Hyrule Castle Grounds -> HC Storms Grotto
    { ENTR_GROTTOS_9, ENTR_CASTLE_GROUNDS_RAINBOW_BRIDGE_EXIT, 0, 0xF6, SCENE_HYRULE_CASTLE, { 986.0, 1571.0, 837.0 } },
    // Hyrule Field -> HF Cow Grotto
    { ENTR_GROTTOS_5, ENTR_HYRULE_FIELD_ROCKY_PATH, 0, 0xE4, SCENE_HYRULE_FIELD, { -7873.0, -300.0, 6916.0 } },
    // Death Mountain Summit -> DMT Cow Grotto
    { ENTR_GROTTOS_13, ENTR_DEATH_MOUNTAIN_TRAIL_GC_EXIT, 0, 0xF8, SCENE_DEATH_MOUNTAIN_TRAIL, { -678.0, 1946.0, -284.0 } },
    // GV Grotto Ledge -> GV Octorok Grotto
    { ENTR_GROTTOS_6, ENTR_GERUDO_VALLEY_EAST_EXIT, 0, 0xF2, SCENE_GERUDO_VALLEY, { 271.0, -555.0, 1465.0 } },
    // Hyrule Field -> HF Tektite Grotto
    { ENTR_GROTTOS_11, ENTR_HYRULE_FIELD_PAST_BRIDGE_SPAWN, 0, 0xE1, SCENE_HYRULE_FIELD, { -4945.0, -300.0, 2841.0 } },
    // clang-format on
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
                Audio_PlaySoundGeneral(NA_SE_IT_SWORD_IMPACT, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                       &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
                this->verticalInput = R_UPDATE_RATE;
            }
        }

        if (CHECK_BTN_ALL(input->cur.button, BTN_DUP) && this->timerUp == 0) {
            Audio_PlaySoundGeneral(NA_SE_IT_SWORD_IMPACT, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                   &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
            this->verticalInput = R_UPDATE_RATE * 3;
        }

        if (CHECK_BTN_ALL(input->press.button, BTN_DDOWN)) {
            if (this->lockDown == true) {
                this->timerDown = 0;
            }
            if (this->timerDown == 0) {
                this->timerDown = 20;
                this->lockDown = true;
                Audio_PlaySoundGeneral(NA_SE_IT_SWORD_IMPACT, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                       &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
                this->verticalInput = -R_UPDATE_RATE;
            }
        }

        if (CHECK_BTN_ALL(input->cur.button, BTN_DDOWN) && (this->timerDown == 0)) {
            Audio_PlaySoundGeneral(NA_SE_IT_SWORD_IMPACT, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                   &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
            this->verticalInput = -R_UPDATE_RATE * 3;
        }

        if (CHECK_BTN_ALL(input->press.button, BTN_DLEFT) || CHECK_BTN_ALL(input->cur.button, BTN_DLEFT)) {
            Audio_PlaySoundGeneral(NA_SE_IT_SWORD_IMPACT, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                   &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
            this->verticalInput = R_UPDATE_RATE;
        }

        if (CHECK_BTN_ALL(input->press.button, BTN_DRIGHT) || CHECK_BTN_ALL(input->cur.button, BTN_DRIGHT)) {
            Audio_PlaySoundGeneral(NA_SE_IT_SWORD_IMPACT, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                   &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
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
    uint8_t sceneChanged = 0;

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
                Audio_PlaySoundGeneral(NA_SE_VO_LI_SWORD_N_KID, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                       &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
            } else {
                gSaveContext.linkAge = 0;
                Audio_PlaySoundGeneral(NA_SE_VO_LI_SWORD_N, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                       &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
            }
        }

        if (CHECK_BTN_ALL(input->press.button, BTN_Z) || CHECK_BTN_ALL(input->press.button, BTN_R)) {
            if (gSaveContext.dayTime > 0xC000 || gSaveContext.dayTime < 0x4555) {
                gSaveContext.nightFlag = 0;
                gSaveContext.dayTime = 0x8000;
                Audio_PlaySoundGeneral(NA_SE_EV_CHICKEN_CRY_M, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                       &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
            } else {
                gSaveContext.nightFlag = 1;
                gSaveContext.dayTime = 0x0000;
                Audio_PlaySoundGeneral(NA_SE_EV_DOG_CRY_EVENING, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                       &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
            }
        }

        if (CHECK_BTN_ALL(input->press.button, BTN_L)) {
            // Hijacking opt as the "MQ" option for better select. Only change opt/play sound if displayed
            if (ResourceMgr_GameHasMasterQuest() && ResourceMgr_GameHasOriginal() &&
                this->betterScenes[this->currentScene].entrancePairs[this->pageDownIndex].canBeMQ) {
                this->opt = this->opt ? 0 : 1;
                if (this->opt) {
                    Audio_PlaySoundGeneral(NA_SE_IT_SWORD_PICKOUT, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                           &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
                } else {
                    Audio_PlaySoundGeneral(NA_SE_IT_SWORD_PUTAWAY, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                           &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
                }
            }
        }

        if (CHECK_BTN_ALL(input->press.button, BTN_CLEFT) || CHECK_BTN_ALL(input->press.button, BTN_DLEFT)) {
            this->pageDownIndex--;
            Audio_PlaySoundGeneral(NA_SE_IT_SWORD_SWING, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                   &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
            if (this->pageDownIndex < 0) {
                this->pageDownIndex = this->betterScenes[this->currentScene].entranceCount - 1;
            }
        }
        if (CHECK_BTN_ALL(input->press.button, BTN_CRIGHT) || CHECK_BTN_ALL(input->press.button, BTN_DRIGHT)) {
            this->pageDownIndex++;
            Audio_PlaySoundGeneral(NA_SE_IT_SWORD_SWING, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                   &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
            if (this->pageDownIndex > this->betterScenes[this->currentScene].entranceCount - 1) {
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
                Audio_PlaySoundGeneral(NA_SE_IT_SWORD_IMPACT, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                       &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
                this->verticalInput = R_UPDATE_RATE;
            }
        }

        if ((CHECK_BTN_ALL(input->cur.button, BTN_DUP) || CHECK_BTN_ALL(input->cur.button, BTN_CUP)) &&
            this->timerUp == 0) {
            Audio_PlaySoundGeneral(NA_SE_IT_SWORD_IMPACT, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                   &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
            this->verticalInput = R_UPDATE_RATE * 3;
        }

        if (CHECK_BTN_ALL(input->press.button, BTN_DDOWN) || CHECK_BTN_ALL(input->press.button, BTN_CDOWN)) {
            if (this->lockDown == true) {
                this->timerDown = 0;
            }
            if (this->timerDown == 0) {
                this->timerDown = 20;
                this->lockDown = true;
                Audio_PlaySoundGeneral(NA_SE_IT_SWORD_IMPACT, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                       &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
                this->verticalInput = -R_UPDATE_RATE;
            }
        }

        if ((CHECK_BTN_ALL(input->cur.button, BTN_DDOWN) || CHECK_BTN_ALL(input->cur.button, BTN_CDOWN)) &&
            (this->timerDown == 0)) {
            Audio_PlaySoundGeneral(NA_SE_IT_SWORD_IMPACT, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                   &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
            this->verticalInput = -R_UPDATE_RATE * 3;
        }
    }

    this->verticalInputAccumulator += this->verticalInput;

    if (this->verticalInputAccumulator < -7) {
        sceneChanged = 1;
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
        sceneChanged = 1;
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

    if (sceneChanged) {
        BetterSceneSelectEntrancePair entrancePair =
            this->betterScenes[this->currentScene].entrancePairs[this->pageDownIndex];
        // Update the MQ status to match the new scene
        if (entrancePair.canBeMQ && ResourceMgr_IsSceneMasterQuest(gEntranceTable[entrancePair.entranceIndex].scene)) {
            this->opt = 1;
        } else {
            this->opt = 0;
        }
    }

    this->currentScene = (this->currentScene + this->count) % this->count;
    this->topDisplayedScene = (this->topDisplayedScene + this->count) % this->count;

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

        if (CVarGetInteger(CVAR_DEVELOPER_TOOLS("DebugWarpScreenTranslation"), 1)) {
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
                case LANGUAGE_JPN:
                    name = this->scenes[scene].japaneseName;
                    break;
            }
        } else {
            name = this->scenes[scene].japaneseName;
        }
        // name = this->scenes[scene].name;
        if (name == NULL) {
            name = "**Null**";
        }

        GfxPrint_Printf(printer, "%s", name);
    };

    GfxPrint_SetColor(printer, 155, 55, 150, 255);

    // Small position hack of the OPT=X text since german Link's Age overlap if translated
    if (CVarGetInteger(CVAR_DEVELOPER_TOOLS("DebugWarpScreenTranslation"), 1) &&
        gSaveContext.language == LANGUAGE_GER) {
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
    if (CVarGetInteger(CVAR_DEVELOPER_TOOLS("DebugWarpScreenTranslation"), 1)) {
        switch (gSaveContext.language) {
            case LANGUAGE_ENG:
            default:
                GfxPrint_Printf(printer, "Scene Selection");
                break;
            case LANGUAGE_GER:
                GfxPrint_Printf(printer, "Szenenauswahl");
                break;
            case LANGUAGE_FRA:
                GfxPrint_Printf(printer, "Selection de Scene");
                break;
            case LANGUAGE_JPN:
                GfxPrint_Printf(printer, "シーンセレクト");
                break;
        }
    } else {
        GfxPrint_Printf(printer, "Scene Selection");
    }
    GfxPrint_SetColor(printer, 255, 255, 255, 255);

    for (i = 0; i < 20; i++) {
        GfxPrint_SetPos(printer, 3, i + 4);

        scene = (this->topDisplayedScene + i + this->count) % this->count;
        if (scene == this->currentScene) {
            GfxPrint_SetColor(printer, 255, 100, 100, 255);
        } else {
            GfxPrint_SetColor(printer, 175, 175, 175, 255);
        }

        if (CVarGetInteger(CVAR_DEVELOPER_TOOLS("DebugWarpScreenTranslation"), 1)) {
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
                case LANGUAGE_JPN:
                    name = this->betterScenes[scene].japaneseName;
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

    if (CVarGetInteger(CVAR_DEVELOPER_TOOLS("DebugWarpScreenTranslation"), 1)) {
        switch (gSaveContext.language) {
            case LANGUAGE_ENG:
            default:
                GfxPrint_Printf(printer, "%s",
                                this->betterScenes[this->currentScene].entrancePairs[this->pageDownIndex].englishName);
                break;
            case LANGUAGE_GER:
                GfxPrint_Printf(printer, "%s",
                                this->betterScenes[this->currentScene].entrancePairs[this->pageDownIndex].germanName);
                break;
            case LANGUAGE_FRA:
                GfxPrint_Printf(printer, "%s",
                                this->betterScenes[this->currentScene].entrancePairs[this->pageDownIndex].frenchName);
                break;
            case LANGUAGE_JPN:
                GfxPrint_Printf(printer, "%s",
                                this->betterScenes[this->currentScene].entrancePairs[this->pageDownIndex].japaneseName);
                break;
        }
    } else {
        GfxPrint_Printf(printer, "%s",
                        this->betterScenes[this->currentScene].entrancePairs[this->pageDownIndex].englishName);
    }
}

static SceneSelectLoadingMessages sLoadingMessages[] = {
    { GFXP_HIRAGANA "ｼﾊﾞﾗｸｵﾏﾁｸﾀﾞｻｲ", "Please wait a minute", "Bitte warte eine Minute",
      "Veuillez patienter une minute" },
    { GFXP_HIRAGANA "ﾁｮｯﾄ ﾏｯﾃﾈ", "Hold on a sec",
      "Warte mal 'ne Sekunde"
      "Une seconde, ca arrive" },
    { GFXP_KATAKANA "ｳｪｲﾄ ｱ ﾓｰﾒﾝﾄ", "Wait a moment", "Warte einen Moment", "Patientez un instant" },
    { GFXP_KATAKANA "ﾛｰﾄﾞ" GFXP_HIRAGANA "ﾁｭｳ", "Loading", "Ladevorgang", "Chargement" },
    { GFXP_HIRAGANA "ﾅｳ ﾜｰｷﾝｸﾞ", "Now working", "Verarbeite", "Au travail" },
    { GFXP_HIRAGANA "ｲﾏ ﾂｸｯﾃﾏｽ", "Now creating", "Erstelle...", "En cours de creation..." },
    { GFXP_HIRAGANA "ｺｼｮｳｼﾞｬﾅｲﾖ", "It's not broken", "Es ist nicht kaputt", "C'est pas casse!" },
    { GFXP_KATAKANA "ｺｰﾋｰ ﾌﾞﾚｲｸ", "Coffee Break", "Kaffee-Pause", "Pause Cafe" },
    { GFXP_KATAKANA "Bﾒﾝｦｾｯﾄｼﾃｸﾀﾞｻｲ", "Please set B side", "Please set B side", "Please set B side" },
    { GFXP_HIRAGANA "ｼﾞｯﾄ" GFXP_KATAKANA "ｶﾞﾏﾝ" GFXP_HIRAGANA "ﾉ" GFXP_KATAKANA "ｺ" GFXP_HIRAGANA "ﾃﾞｱｯﾀ",
      "Be patient, now", "Ube dich in Geduld", "Veuillez patientez" },
    { GFXP_HIRAGANA "ｲﾏｼﾊﾞﾗｸｵﾏﾁｸﾀﾞｻｲ", "Please wait just a minute", "Bitte warte noch eine Minute",
      "Patientez un peu" },
    { GFXP_HIRAGANA "ｱﾜﾃﾅｲｱﾜﾃﾅｲ｡ﾋﾄﾔｽﾐﾋﾄﾔｽﾐ｡", "Don't panic, don't panic. Take a break, take a break.",
      "Keine Panik! Nimm dir eine Auszeit", "Pas de panique. Prenez une pause." },
    { "Enough! My ship sails in the morning!", "Enough! My ship sails in the morning!",
      "Enough! My ship sails in the morning!", "Enough! My ship sails in the morning!" },
};

void Select_PrintLoadingMessage(SelectContext* this, GfxPrint* printer) {
    s32 randomMsg;

    GfxPrint_SetPos(printer, 10, 15);
    GfxPrint_SetColor(printer, 255, 255, 255, 255);
    randomMsg = Rand_ZeroOne() * ARRAY_COUNT(sLoadingMessages);
    if (CVarGetInteger(CVAR_DEVELOPER_TOOLS("DebugWarpScreenTranslation"), 1)) {
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
            case LANGUAGE_JPN:
                GfxPrint_Printf(printer, "%s", sLoadingMessages[randomMsg].japaneseMessage);
                break;
        }
    } else {
        GfxPrint_Printf(printer, "%s", sLoadingMessages[randomMsg].japaneseMessage);
    }
}

static SceneSelectAgeLabels sAgeLabels[] = {
    { GFXP_HIRAGANA "17(ﾜｶﾓﾉ)", "17(Adult)", "17(Erwachsener)", "17(Adulte)" }, // "17(young)
    { GFXP_HIRAGANA "5(ﾜｶｽｷﾞ)", "5(Child)", "5(Kind)", "5(Enfant)" },           // "5(very young)
};

static BetterSceneSelectAgeLabels sBetterAgeLabels[] = {
    { "おとな", "Adult", "Erwachsener", "Adulte" },
    { "こと゛も", "Child", "Kind", "Enfant" },
};

void Select_PrintAgeSetting(SelectContext* this, GfxPrint* printer, s32 age) {
    GfxPrint_SetPos(printer, 4, 26);
    GfxPrint_SetColor(printer, 255, 255, 55, 255);
    if (CVarGetInteger(CVAR_DEVELOPER_TOOLS("DebugWarpScreenTranslation"), 1)) {
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
            case LANGUAGE_JPN:
                GfxPrint_Printf(printer, "Age:%s", sAgeLabels[age].japaneseAge);
                break;
        }
    } else {
        GfxPrint_Printf(printer, "Age:%s", sAgeLabels[age].japaneseAge);
    }
}

void Better_Select_PrintAgeSetting(SelectContext* this, GfxPrint* printer, s32 age) {
    if (gSaveContext.language == LANGUAGE_JPN) {
        GfxPrint_SetPos(printer, 22, 25); // For better alignment for Japanese menu
    } else {
        GfxPrint_SetPos(printer, 25, 25);
    }
    GfxPrint_SetColor(printer, 100, 100, 100, 255);
    if (CVarGetInteger(CVAR_DEVELOPER_TOOLS("DebugWarpScreenTranslation"), 1)) {
        switch (gSaveContext.language) {
            case LANGUAGE_ENG:
            case LANGUAGE_FRA:
            default:
                GfxPrint_Printf(printer, "(B)Age:");
                break;
            case LANGUAGE_GER:
                GfxPrint_Printf(printer, "(B)Alter:");
                break;
            case LANGUAGE_JPN:
                GfxPrint_Printf(printer, "(B)し゛た゛い:");
                break;
        }
    } else {
        GfxPrint_Printf(printer, "(B)Age:");
    }

    GfxPrint_SetColor(printer, 55, 200, 50, 255);
    if (CVarGetInteger(CVAR_DEVELOPER_TOOLS("DebugWarpScreenTranslation"), 1)) {
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
            case LANGUAGE_JPN:
                GfxPrint_Printf(printer, "%s", sBetterAgeLabels[age].japaneseAge);
                break;
        }
    } else {
        GfxPrint_Printf(printer, "%s", sBetterAgeLabels[age].englishAge);
    }
}

void Select_PrintCutsceneSetting(SelectContext* this, GfxPrint* printer, u16 csIndex) {
    char* cutsceneLabels[13][4] = {
        { GFXP_HIRAGANA " ﾖﾙ " GFXP_KATAKANA "ｺﾞﾛﾝ", "Day", "Tag", "Jour" },
        { GFXP_HIRAGANA "ｵﾋﾙ " GFXP_KATAKANA "ｼﾞｬﾗ", "Night", "Nacht", "Nuit" },
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
    int lang =
        CVarGetInteger(CVAR_DEVELOPER_TOOLS("DebugWarpScreenTranslation"), 1) ? (gSaveContext.language + 1) % 4 : 0;

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
        if (CVarGetInteger(CVAR_DEVELOPER_TOOLS("DebugWarpScreenTranslation"), 1)) {
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
                case LANGUAGE_JPN:
                    label = "よる";
                    break;
            }
        } else {
            label = "Night";
        }
    } else {
        if (CVarGetInteger(CVAR_DEVELOPER_TOOLS("DebugWarpScreenTranslation"), 1)) {
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
                case LANGUAGE_JPN:
                    label = "ひる";
                    break;
            }
        } else {
            label = "Day";
        }
    }
    if (CVarGetInteger(CVAR_DEVELOPER_TOOLS("DebugWarpScreenTranslation"), 1)) {
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
            case LANGUAGE_JPN:
                GfxPrint_Printf(printer, "(Z/R)し゛かん:");
                break;
        }
    } else {
        GfxPrint_Printf(printer, "(Z/R)Time:");
    }

    GfxPrint_SetColor(printer, 200, 200, 55, 255);
    GfxPrint_Printf(printer, "%s", label);
}

void Better_Select_PrintMQSetting(SelectContext* this, GfxPrint* printer) {
    char* label;

    if (this->betterScenes[this->currentScene].entrancePairs[this->pageDownIndex].canBeMQ) {
        GfxPrint_SetColor(printer, 100, 100, 100, 255);
        GfxPrint_SetPos(printer, 3, 25);

        // MQ can be toggled
        if (ResourceMgr_GameHasMasterQuest() && ResourceMgr_GameHasOriginal()) {
            GfxPrint_Printf(printer, "(L)MQ:");
        } else {
            GfxPrint_Printf(printer, "MQ:");
        }

        if (CVarGetInteger(CVAR_DEVELOPER_TOOLS("DebugWarpScreenTranslation"), 1)) {
            switch (gSaveContext.language) {
                case LANGUAGE_ENG:
                case LANGUAGE_JPN:
                default:
                    label = this->opt ? "ON" : "OFF";
                    break;
                case LANGUAGE_GER:
                    label = this->opt ? "AN" : "AUS";
                    break;
                case LANGUAGE_FRA:
                    label = this->opt ? "ACTIVE" : "DESACTIVE";
                    break;
            }
        } else {
            label = this->opt ? "ON" : "OFF";
        }

        GfxPrint_SetColor(printer, 0, 150, 194, 255);
        GfxPrint_Printf(printer, "%s", label);
    }
}

void Select_DrawMenu(SelectContext* this) {
    GraphicsContext* gfxCtx = this->state.gfxCtx;
    GfxPrint printer;

    OPEN_DISPS(gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x00, NULL);
    Gfx_SetupFrame(gfxCtx, 0, 0, 0);
    SET_FULLSCREEN_VIEWPORT(&this->view);
    func_800AAA50(&this->view, 0xF);
    Gfx_SetupDL_28Opa(gfxCtx);

    // printer = alloca(sizeof(GfxPrint));
    GfxPrint_Init(&printer);
    GfxPrint_Open(&printer, POLY_OPA_DISP);
    if (this->isBetterWarp) {
        Better_Select_PrintTimeSetting(this, &printer);
        Better_Select_PrintAgeSetting(this, &printer, ((void)0, gSaveContext.linkAge));
        Better_Select_PrintMQSetting(this, &printer);
        Better_Select_PrintMenu(this, &printer);
    } else {
        Select_PrintMenu(this, &printer);
        Select_PrintAgeSetting(this, &printer, ((void)0, gSaveContext.linkAge));
        Select_PrintCutsceneSetting(this, &printer, ((void)0, gSaveContext.cutsceneIndex));
    }
    POLY_OPA_DISP = GfxPrint_Close(&printer);
    GfxPrint_Destroy(&printer);

    CLOSE_DISPS(gfxCtx);
}

void Select_DrawLoadingScreen(SelectContext* this) {
    GraphicsContext* gfxCtx = this->state.gfxCtx;
    GfxPrint printer;

    OPEN_DISPS(gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x00, NULL);
    Gfx_SetupFrame(gfxCtx, 0, 0, 0);
    SET_FULLSCREEN_VIEWPORT(&this->view);
    func_800AAA50(&this->view, 0xF);
    Gfx_SetupDL_28Opa(gfxCtx);

    // printer = alloca(sizeof(GfxPrint));
    GfxPrint_Init(&printer);
    GfxPrint_Open(&printer, POLY_OPA_DISP);
    Select_PrintLoadingMessage(this, &printer);
    POLY_OPA_DISP = GfxPrint_Close(&printer);
    GfxPrint_Destroy(&printer);

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

    if (this->isBetterWarp != CVarGetInteger(CVAR_DEVELOPER_TOOLS("BetterDebugWarpScreen"), 1)) {
        Select_SwitchBetterWarpMode(this, !this->isBetterWarp);
    }

    if (this->isBetterWarp) {
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

// Switch better warp mode and re-init the list
void Select_SwitchBetterWarpMode(SelectContext* this, u8 isBetterWarpMode) {
    this->isBetterWarp = isBetterWarpMode;
    this->opt = 0;
    this->currentScene = 0;
    this->topDisplayedScene = 0;
    this->pageDownIndex = 0;
    gSaveContext.cutsceneIndex = 0x8000;
    gSaveContext.linkAge = 1;
    gSaveContext.nightFlag = 0;
    gSaveContext.dayTime = 0x8000;

    if (isBetterWarpMode) {
        s32 currScene = CVarGetInteger(CVAR_GENERAL("BetterDebugWarpScreenCurrentScene"), 0);
        this->count = ARRAY_COUNT(sBetterScenes);

        if (currScene >= 0 && currScene < this->count) {
            this->currentScene = currScene;
            this->topDisplayedScene = CVarGetInteger(CVAR_GENERAL("BetterDebugWarpScreenTopDisplayedScene"), 0);
            this->pageDownIndex = CVarGetInteger(CVAR_GENERAL("BetterDebugWarpScreenPageDownIndex"), 0);

            BetterSceneSelectEntrancePair entrancePair =
                this->betterScenes[this->currentScene].entrancePairs[this->pageDownIndex];
            if (entrancePair.canBeMQ &&
                ResourceMgr_IsSceneMasterQuest(gEntranceTable[entrancePair.entranceIndex].scene)) {
                this->opt = 1;
            }
        }
    } else {
        this->count = ARRAY_COUNT(sScenes);

        if ((dREG(80) >= 0) && (dREG(80) < this->count)) {
            this->currentScene = dREG(80);
            this->topDisplayedScene = dREG(81);
            this->pageDownIndex = dREG(82);
        }
    }
}

void Select_Init(GameState* thisx) {
    SelectContext* this = (SelectContext*)thisx;
    size_t size;

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
    this->count = ARRAY_COUNT(sScenes);
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

    R_UPDATE_RATE = 1;
#if !defined(_MSC_VER) && !defined(__GNUC__)
    this->staticSegment = GAMESTATE_ALLOC_MC(&this->state, size);
    DmaMgr_SendRequest1(this->staticSegment, _z_select_staticSegmentRomStart, size, __FILE__, __LINE__);
#endif
    gSaveContext.cutsceneIndex = 0x8000;
    gSaveContext.linkAge = 1;
    gSaveContext.nightFlag = 0;
    gSaveContext.dayTime = 0x8000;

    CVarClear(CVAR_GENERAL("BetterDebugWarpScreenMQMode"));
    CVarClear(CVAR_GENERAL("BetterDebugWarpScreenMQModeScene"));
    Select_SwitchBetterWarpMode(this, CVarGetInteger(CVAR_DEVELOPER_TOOLS("BetterDebugWarpScreen"), 1));
}
