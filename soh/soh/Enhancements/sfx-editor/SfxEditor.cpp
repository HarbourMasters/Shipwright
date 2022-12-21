#include "SfxEditor.h"
#include "sequence.h"
#include <string>
#include <map>
#include <Cvar.h>
#include <ImGuiImpl.h>
#include <functions.h>
#include "../randomizer/3drando/random.hpp"
#include "../../OTRGlobals.h"
#include <Utils/StringHelper.h>
#include "../../UIWidgets.hpp"

Vec3f pos = { 0.0f, 0.0f, 0.0f };
f32 freqScale = 1.0f;
s8 reverbAdd = 0;

//  {originalSequenceId,           {label,                                  sfxKey,                          category},
std::map<u16, std::tuple<std::string, std::string, SeqType>> sfxEditorSequenceMap = {
    {NA_BGM_FIELD_LOGIC,           {"Hyrule Field",                        "NA_BGM_FIELD_LOGIC",             SEQ_BGM_WORLD}},
    {NA_BGM_DUNGEON,               {"Dodongo's Cavern",                    "NA_BGM_DUNGEON",                 SEQ_BGM_WORLD}},
    {NA_BGM_KAKARIKO_ADULT,        {"Kakariko Village (Adult)",            "NA_BGM_KAKARIKO_ADULT",          SEQ_BGM_WORLD}},
    {NA_BGM_ENEMY,                 {"Battle",                              "NA_BGM_ENEMY",                   SEQ_BGM_BATTLE}},
    {NA_BGM_BOSS,                  {"Boss Battle",                         "NA_BGM_BOSS",                    SEQ_BGM_BATTLE}},
    {NA_BGM_INSIDE_DEKU_TREE,      {"Inside the Deku Tree",                "NA_BGM_INSIDE_DEKU_TREE",        SEQ_BGM_WORLD}},
    {NA_BGM_MARKET,                {"Market",                              "NA_BGM_MARKET",                  SEQ_BGM_WORLD}},
    {NA_BGM_TITLE,                 {"Title Theme",                         "NA_BGM_TITLE",                   SEQ_BGM_WORLD}},
    {NA_BGM_LINK_HOUSE,            {"House",                               "NA_BGM_LINK_HOUSE",              SEQ_BGM_WORLD}},
    {NA_BGM_GAME_OVER,             {"Game Over",                           "NA_BGM_GAME_OVER",               SEQ_FANFARE}},
    {NA_BGM_BOSS_CLEAR,            {"Boss Clear",                          "NA_BGM_BOSS_CLEAR",              SEQ_FANFARE}},
    {NA_BGM_ITEM_GET,              {"Obtain Item",                         "NA_BGM_ITEM_GET",                SEQ_FANFARE}},
    {NA_BGM_OPENING_GANON,         {"Enter Ganondorf",                     "NA_BGM_OPENING_GANON",           SEQ_FANFARE}},
    {NA_BGM_HEART_GET,             {"Obtain Heart Container",              "NA_BGM_HEART_GET",               SEQ_FANFARE}},
    {NA_BGM_OCA_LIGHT,             {"Prelude of Light",                    "NA_BGM_OCA_LIGHT",               SEQ_OCARINA}},
    {NA_BGM_JABU_JABU,             {"Inside Jabu-Jabu's Belly",            "NA_BGM_JABU_JABU",               SEQ_BGM_WORLD}},
    {NA_BGM_KAKARIKO_KID,          {"Kakariko Village (Child)",            "NA_BGM_KAKARIKO_KID",            SEQ_BGM_WORLD}},
    {NA_BGM_GREAT_FAIRY,           {"Great Fairy's Fountain",              "NA_BGM_GREAT_FAIRY",             SEQ_BGM_EVENT}},
    {NA_BGM_ZELDA_THEME,           {"Zelda's Theme",                       "NA_BGM_ZELDA_THEME",             SEQ_BGM_EVENT}},
    {NA_BGM_FIRE_TEMPLE,           {"Fire Temple",                         "NA_BGM_FIRE_TEMPLE",             SEQ_BGM_WORLD}},
    {NA_BGM_OPEN_TRE_BOX,          {"Open Treasure Chest",                 "NA_BGM_OPEN_TRE_BOX",            SEQ_FANFARE}},
    {NA_BGM_FOREST_TEMPLE,         {"Forest Temple",                       "NA_BGM_FOREST_TEMPLE",           SEQ_BGM_WORLD}},
    {NA_BGM_COURTYARD,             {"Hyrule Castle Courtyard",             "NA_BGM_COURTYARD",               SEQ_BGM_WORLD}},
    {NA_BGM_GANON_TOWER,           {"Ganondorf's Theme",                   "NA_BGM_GANON_TOWER",             SEQ_BGM_WORLD}},
    {NA_BGM_LONLON,                {"Lon Lon Ranch",                       "NA_BGM_LONLON",                  SEQ_BGM_WORLD}},
    {NA_BGM_GORON_CITY,            {"Goron City",                          "NA_BGM_GORON_CITY",              SEQ_BGM_WORLD}},
    {NA_BGM_FIELD_MORNING,         {"Hyrule Field Morning Theme",          "NA_BGM_FIELD_MORNING",           SEQ_NOSHUFFLE}},
    {NA_BGM_SPIRITUAL_STONE,       {"Spiritual Stone Get",                 "NA_BGM_SPIRITUAL_STONE",         SEQ_FANFARE}},
    {NA_BGM_OCA_BOLERO,            {"Bolero of Fire",                      "NA_BGM_OCA_BOLERO",              SEQ_OCARINA}},
    {NA_BGM_OCA_MINUET,            {"Minuet of Forest",                    "NA_BGM_OCA_MINUET",              SEQ_OCARINA}},
    {NA_BGM_OCA_SERENADE,          {"Serenade of Water",                   "NA_BGM_OCA_SERENADE",            SEQ_OCARINA}},
    {NA_BGM_OCA_REQUIEM,           {"Requiem of Spirit",                   "NA_BGM_OCA_REQUIEM",             SEQ_OCARINA}},
    {NA_BGM_OCA_NOCTURNE,          {"Nocturne of Shadow",                  "NA_BGM_OCA_NOCTURNE",            SEQ_OCARINA}},
    {NA_BGM_MINI_BOSS,             {"Mini-Boss Battle",                    "NA_BGM_MINI_BOSS",               SEQ_BGM_BATTLE}},
    {NA_BGM_SMALL_ITEM_GET,        {"Obtain Small Item",                   "NA_BGM_SMALL_ITEM_GET",          SEQ_FANFARE}},
    {NA_BGM_TEMPLE_OF_TIME,        {"Temple of Time",                      "NA_BGM_TEMPLE_OF_TIME",          SEQ_BGM_WORLD}},
    {NA_BGM_EVENT_CLEAR,           {"Escape from Lon Lon Ranch",           "NA_BGM_EVENT_CLEAR",             SEQ_FANFARE}},
    {NA_BGM_KOKIRI,                {"Kokiri Forest",                       "NA_BGM_KOKIRI",                  SEQ_BGM_WORLD}},
    {NA_BGM_OCA_FAIRY_GET,         {"Obtain Fairy Ocarina",                "NA_BGM_OCA_FAIRY_GET",           SEQ_FANFARE}},
    {NA_BGM_SARIA_THEME,           {"Lost Woods",                          "NA_BGM_SARIA_THEME",             SEQ_BGM_WORLD}},
    {NA_BGM_SPIRIT_TEMPLE,         {"Spirit Temple",                       "NA_BGM_SPIRIT_TEMPLE",           SEQ_BGM_WORLD}},
    {NA_BGM_HORSE,                 {"Horse Race",                          "NA_BGM_HORSE",                   SEQ_BGM_EVENT}},
    {NA_BGM_HORSE_GOAL,            {"Horse Race Goal",                     "NA_BGM_HORSE_GOAL",              SEQ_FANFARE}},
    {NA_BGM_INGO,                  {"Ingo's Theme",                        "NA_BGM_INGO",                    SEQ_BGM_WORLD}},
    {NA_BGM_MEDALLION_GET,         {"Obtain Medallion",                    "NA_BGM_MEDALLION_GET",           SEQ_FANFARE}},
    {NA_BGM_OCA_SARIA,             {"Ocarina Saria's Song",                "NA_BGM_OCA_SARIA",               SEQ_OCARINA}},
    {NA_BGM_OCA_EPONA,             {"Ocarina Epona's Song",                "NA_BGM_OCA_EPONA",               SEQ_OCARINA}},
    {NA_BGM_OCA_ZELDA,             {"Ocarina Zelda's Lullaby",             "NA_BGM_OCA_ZELDA",               SEQ_OCARINA}},
    {NA_BGM_OCA_SUNS,              {"Ocarina Sun's Song",                  "NA_BGM_OCA_SUNS",                SEQ_OCARINA}},
    {NA_BGM_OCA_TIME,              {"Ocarina Song of Time",                "NA_BGM_OCA_TIME",                SEQ_OCARINA}},
    {NA_BGM_OCA_STORM,             {"Ocarina Song of Storms",              "NA_BGM_OCA_STORM",               SEQ_OCARINA}},
    {NA_BGM_NAVI_OPENING,          {"Fairy Flying",                        "NA_BGM_NAVI_OPENING",            SEQ_BGM_EVENT}},
    {NA_BGM_DEKU_TREE_CS,          {"Deku Tree",                           "NA_BGM_DEKU_TREE_CS",            SEQ_BGM_EVENT}},
    {NA_BGM_WINDMILL,              {"Windmill Hut",                        "NA_BGM_WINDMILL",                SEQ_BGM_WORLD}},
    {NA_BGM_HYRULE_CS,             {"Legend of Hyrule",                    "NA_BGM_HYRULE_CS",               SEQ_BGM_EVENT}},
    {NA_BGM_MINI_GAME,             {"Shooting Gallery",                    "NA_BGM_MINI_GAME",               SEQ_BGM_EVENT}},
    {NA_BGM_SHEIK,                 {"Sheik's Theme",                       "NA_BGM_SHEIK",                   SEQ_BGM_EVENT}},
    {NA_BGM_ZORA_DOMAIN,           {"Zora's Domain",                       "NA_BGM_ZORA_DOMAIN",             SEQ_BGM_WORLD}},
    {NA_BGM_APPEAR,                {"Enter Zelda",                         "NA_BGM_APPEAR",                  SEQ_FANFARE}},
    {NA_BGM_ADULT_LINK,            {"Goodbye to Zelda",                    "NA_BGM_ADULT_LINK",              SEQ_BGM_EVENT}},
    {NA_BGM_MASTER_SWORD,          {"Master Sword",                        "NA_BGM_MASTER_SWORD",            SEQ_FANFARE}},
    {NA_BGM_INTRO_GANON,           {"Ganon Intro",                         "NA_BGM_INTRO_GANON",             SEQ_BGM_EVENT}},
    {NA_BGM_SHOP,                  {"Shop",                                "NA_BGM_SHOP",                    SEQ_BGM_WORLD}},
    {NA_BGM_CHAMBER_OF_SAGES,      {"Chamber of the Sages",                "NA_BGM_CHAMBER_OF_SAGES",        SEQ_BGM_EVENT}},
    {NA_BGM_FILE_SELECT,           {"File Select",                         "NA_BGM_FILE_SELECT",             SEQ_BGM_WORLD}},
    {NA_BGM_ICE_CAVERN,            {"Ice Cavern",                          "NA_BGM_ICE_CAVERN",              SEQ_BGM_WORLD}},
    {NA_BGM_DOOR_OF_TIME,          {"Open Door of Temple of Time",         "NA_BGM_DOOR_OF_TIME",            SEQ_BGM_EVENT}},
    {NA_BGM_OWL,                   {"Kaepora Gaebora's Theme",             "NA_BGM_OWL",                     SEQ_BGM_EVENT}},
    {NA_BGM_SHADOW_TEMPLE,         {"Shadow Temple",                       "NA_BGM_SHADOW_TEMPLE",           SEQ_BGM_WORLD}},
    {NA_BGM_WATER_TEMPLE,          {"Water Temple",                        "NA_BGM_WATER_TEMPLE",            SEQ_BGM_WORLD}},
    {NA_BGM_BRIDGE_TO_GANONS,      {"Ganon's Castle Bridge",               "NA_BGM_BRIDGE_TO_GANONS",        SEQ_BGM_EVENT}},
    {NA_BGM_OCARINA_OF_TIME,       {"Ocarina of Time",                     "NA_BGM_OCARINA_OF_TIME",         SEQ_FANFARE}},
    {NA_BGM_GERUDO_VALLEY,         {"Gerudo Valley",                       "NA_BGM_GERUDO_VALLEY",           SEQ_BGM_WORLD}},
    {NA_BGM_POTION_SHOP,           {"Potion Shop",                         "NA_BGM_POTION_SHOP",             SEQ_BGM_WORLD}},
    {NA_BGM_KOTAKE_KOUME,          {"Kotake & Koume's Theme",              "NA_BGM_KOTAKE_KOUME",            SEQ_BGM_EVENT}},
    {NA_BGM_ESCAPE,                {"Escape from Ganon's Castle",          "NA_BGM_ESCAPE",                  SEQ_BGM_EVENT}},
    {NA_BGM_UNDERGROUND,           {"Ganon's Castle Under Ground",         "NA_BGM_UNDERGROUND",             SEQ_BGM_WORLD}},
    {NA_BGM_GANONDORF_BOSS,        {"Ganondorf Battle",                    "NA_BGM_GANONDORF_BOSS",          SEQ_BGM_BATTLE}},
    {NA_BGM_GANON_BOSS,            {"Ganon Battle",                        "NA_BGM_GANON_BOSS",              SEQ_BGM_BATTLE}},
    {NA_BGM_END_DEMO,              {"Seal of Six Sages",                   "NA_BGM_END_DEMO",                SEQ_NOSHUFFLE}},
    {NA_BGM_STAFF_1,               {"End Credits I",                       "NA_BGM_STAFF_1",                 SEQ_NOSHUFFLE}},
    {NA_BGM_STAFF_2,               {"End Credits II",                      "NA_BGM_STAFF_2",                 SEQ_NOSHUFFLE}},
    {NA_BGM_STAFF_3,               {"End Credits III",                     "NA_BGM_STAFF_3",                 SEQ_NOSHUFFLE}},
    {NA_BGM_STAFF_4,               {"End Credits IV",                      "NA_BGM_STAFF_4",                 SEQ_NOSHUFFLE}},
    {NA_BGM_FIRE_BOSS,             {"King Dodongo & Volvagia Boss Battle", "NA_BGM_FIRE_BOSS",               SEQ_BGM_BATTLE}},
    {NA_BGM_TIMED_MINI_GAME,       {"Mini-Game",                           "NA_BGM_TIMED_MINI_GAME",         SEQ_BGM_EVENT}},
    {INSTRUMENT_OFFSET + 1,        {"Ocarina",                             "OCARINA_INSTRUMENT_DEFAULT",     SEQ_INSTRUMENT}},
    {INSTRUMENT_OFFSET + 2,        {"Malon",                               "OCARINA_INSTRUMENT_MALON",       SEQ_INSTRUMENT}},
    {INSTRUMENT_OFFSET + 3,        {"Whistle",                             "OCARINA_INSTRUMENT_WHISTLE",     SEQ_INSTRUMENT}},
    {INSTRUMENT_OFFSET + 4,        {"Harp",                                "OCARINA_INSTRUMENT_HARP",        SEQ_INSTRUMENT}},
    {INSTRUMENT_OFFSET + 5,        {"Organ",                               "OCARINA_INSTRUMENT_GRIND_ORGAN", SEQ_INSTRUMENT}},
    {INSTRUMENT_OFFSET + 6,        {"Flute",                               "OCARINA_INSTRUMENT_FLUTE",       SEQ_INSTRUMENT}},
    {NA_SE_EV_SMALL_DOG_BARK,      {"Bark",                                "NA_SE_EV_SMALL_DOG_BARK",        SEQ_SFX}},
    {NA_SE_EN_AWA_BOUND,           {"Bomb Bounce",                         "NA_SE_EN_AWA_BOUND",             SEQ_SFX}},
    {NA_SE_EN_SHADEST_TAIKO_LOW,   {"Bongo Bongo Low",                     "NA_SE_EN_SHADEST_TAIKO_LOW",     SEQ_SFX}},
    {NA_SE_EN_NUTS_FAINT,          {"Business Scrub",                      "NA_SE_EN_NUTS_FAINT",            SEQ_SFX}},
    {NA_SE_SY_CARROT_RECOVER,      {"Carrot Refill",                       "NA_SE_SY_CARROT_RECOVER",        SEQ_SFX}},
    {NA_SE_EV_CHICKEN_CRY_N,       {"Cluck",                               "NA_SE_EV_CHICKEN_CRY_N",         SEQ_SFX}},
    {NA_SE_EV_BRIDGE_OPEN_STOP,    {"Drawbridge Set",                      "NA_SE_EV_BRIDGE_OPEN_STOP",      SEQ_SFX}},
    {NA_SE_EN_KAICHO_CRY,          {"Guay",                                "NA_SE_EN_KAICHO_CRY",            SEQ_SFX}},
    {NA_SE_SY_HITPOINT_ALARM,      {"Low HP Beep",                         "NA_SE_SY_HITPOINT_ALARM",        SEQ_SFX}},
    {NA_SE_SY_HP_RECOVER,          {"HP Recover",                          "NA_SE_SY_HP_RECOVER",            SEQ_SFX}},
    {NA_SE_EV_HORSE_RUN,           {"Horse Trot",                          "NA_SE_EV_HORSE_RUN",             SEQ_SFX}},
    {NA_SE_PL_WALK_HEAVYBOOTS,     {"Iron Boots",                          "NA_SE_PL_WALK_HEAVYBOOTS",       SEQ_SFX}},
    {NA_SE_EV_COW_CRY,             {"Moo",                                 "NA_SE_EV_COW_CRY",               SEQ_SFX}},
    {NA_SE_VO_KZ_MOVE,             {"Mweep!",                              "NA_SE_VO_KZ_MOVE",               SEQ_SFX}},
    {NA_SE_VO_NA_HELLO_2,          {"Navi Hey!",                           "NA_SE_VO_NA_HELLO_2",            SEQ_SFX}},
    {NA_SE_VO_SK_LAUGH,            {"Navi Listen!",                        "NA_SE_VO_SK_LAUGH",              SEQ_SFX}},
    {NA_SE_EV_POT_BROKEN,          {"Pot Shattering",                      "NA_SE_EV_POT_BROKEN",            SEQ_SFX}},
    {NA_SE_EV_FROG_CRY_0,          {"Ribbit",                              "NA_SE_EV_FROG_CRY_0",            SEQ_SFX}},
    {NA_SE_EV_FIVE_COUNT_LUPY,     {"Rupee (Silver)",                      "NA_SE_EV_FIVE_COUNT_LUPY",       SEQ_SFX}},
    {NA_SE_EV_FOOT_SWITCH,         {"Switch",                              "NA_SE_EV_FOOT_SWITCH",           SEQ_SFX}},
    {NA_SE_IT_WALL_HIT_SOFT,       {"Sword Bonk",                          "NA_SE_IT_WALL_HIT_SOFT",         SEQ_SFX}},
    {NA_SE_SY_METRONOME,           {"Tambourine",                          "NA_SE_SY_METRONOME",             SEQ_SFX}},
    {NA_SE_VO_Z1_SURPRISE,         {"Zelda Gasp (Adult)",                  "NA_SE_VO_Z1_SURPRISE",           SEQ_SFX}},
    {NA_SE_EN_AMOS_VOICE,          {"Armos",                               "NA_SE_EN_AMOS_VOICE",            SEQ_SFX}},
    {NA_SE_VO_LI_FALL_L_KID,       {"Child Scream",                        "NA_SE_VO_LI_FALL_L_KID",         SEQ_SFX}},
    {NA_SE_VO_FR_LAUGH_0,          {"Great Fairy",                         "NA_SE_VO_FR_LAUGH_0",            SEQ_SFX}},
    {NA_SE_EN_REDEAD_AIM,          {"Redead Scream",                       "NA_SE_EN_REDEAD_AIM",            SEQ_SFX}},
    {NA_SE_VO_RT_LAUGH_0,          {"Ruto Giggle",                         "NA_SE_VO_RT_LAUGH_0",            SEQ_SFX}},
    {NA_SE_EN_STALKID_ATTACK,      {"Stalchild Attack",                    "NA_SE_EN_STALKID_ATTACK",        SEQ_SFX}},
    {NA_SE_EV_CHICKEN_CRY_M,       {"Cockadoodiedoo",                      "NA_SE_EV_CHICKEN_CRY_M",         SEQ_SFX}},
    {NA_SE_SY_KINSTA_MARK_APPEAR,  {"Gold Skulltula Token",                "NA_SE_SY_KINSTA_MARK_APPEAR",    SEQ_SFX}},
    {NA_SE_EN_REDEAD_CRY,          {"Redead Moan",                         "NA_SE_EN_REDEAD_CRY",            SEQ_SFX}},
    {NA_SE_VO_TA_SLEEP,            {"Talon Snore",                         "NA_SE_VO_TA_SLEEP",              SEQ_SFX}},
    {NA_SE_EV_LIGHTNING,           {"Thunder",                             "NA_SE_EV_LIGHTNING",             SEQ_SFX}},
    {NA_SE_OC_ABYSS,               {"Cartoon Fall",                        "NA_SE_OC_ABYSS",                 SEQ_SFX}},
    {NA_SE_EN_FLAME_LAUGH,         {"Flare Dancer Laugh",                  "NA_SE_EN_FLAME_LAUGH",           SEQ_SFX}},
    {NA_SE_EN_AWA_BREAK,           {"Shabom Pop",                          "NA_SE_EN_AWA_BREAK",             SEQ_SFX}},
    {NA_SE_EN_SHADEST_TAIKO_HIGH,  {"Bongo Bongo High",                    "NA_SE_EN_SHADEST_TAIKO_HIGH",    SEQ_SFX}},
    {NA_SE_EV_BOTTLE_CAP_OPEN,     {"Bottle Cork",                         "NA_SE_EV_BOTTLE_CAP_OPEN",       SEQ_SFX}},
    {NA_SE_IT_BOW_FLICK,           {"Bow Twang",                           "NA_SE_IT_BOW_FLICK",             SEQ_SFX}},
    {NA_SE_EN_BUBLE_LAUGH,         {"Bubble Laugh",                        "NA_SE_EN_BUBLE_LAUGH",           SEQ_SFX}},
    {NA_SE_VO_LI_BREATH_DRINK_KID, {"Child Pant",                          "NA_SE_VO_LI_BREATH_DRINK_KID",   SEQ_SFX}},
    {NA_SE_EN_DEKU_JR_MOUTH,       {"Deku Baba",                           "NA_SE_EN_DEKU_JR_MOUTH",         SEQ_SFX}},
    {NA_SE_EV_DOG_CRY_EVENING,     {"Dusk Howl",                           "NA_SE_EV_DOG_CRY_EVENING",       SEQ_SFX}},
    {NA_SE_EN_FLAME_DAMAGE,        {"Flare Dancer Startled",               "NA_SE_EN_FLAME_DAMAGE",          SEQ_SFX}},
    {NA_SE_EN_GANON_AT_RETURN,     {"Ganondorf Teh!",                      "NA_SE_EN_GANON_AT_RETURN",       SEQ_SFX}},
    {NA_SE_EN_GOMA_JR_CRY,         {"Gohma Larva Croak",                   "NA_SE_EN_GOMA_JR_CRY",           SEQ_SFX}},
    {NA_SE_EN_GOLON_WAKE_UP,       {"Goron Wake",                          "NA_SE_EN_GOLON_WAKE_UP",         SEQ_SFX}},
    {NA_SE_SY_START_SHOT,          {"Gunshot",                             "NA_SE_SY_START_SHOT",            SEQ_SFX}},
    {NA_SE_IT_HAMMER_HIT,          {"Hammer Bonk",                         "NA_SE_IT_HAMMER_HIT",            SEQ_SFX}},
    {NA_SE_EN_IRONNACK_SWING_AXE,  {"Iron Knuckle",                        "NA_SE_EN_IRONNACK_SWING_AXE",    SEQ_SFX}},
    {NA_SE_EN_FANTOM_ST_LAUGH,     {"Phantom Ganon Laugh",                 "NA_SE_EN_FANTOM_ST_LAUGH",       SEQ_SFX}},
    {NA_SE_EV_PLANT_BROKEN,        {"Plant Explode",                       "NA_SE_EV_PLANT_BROKEN",          SEQ_SFX}},
    {NA_SE_SY_GET_RUPY,            {"Rupee",                               "NA_SE_SY_GET_RUPY",              SEQ_SFX}},
    {NA_SE_VO_RT_CRASH,            {"Ruto Crash",                          "NA_SE_VO_RT_CRASH",              SEQ_SFX}},
    {NA_SE_VO_RT_LIFT,             {"Ruto Lift",                           "NA_SE_VO_RT_LIFT",               SEQ_SFX}},
    {NA_SE_VO_RT_THROW,            {"Ruto Thrown",                         "NA_SE_VO_RT_THROW",              SEQ_SFX}},
    {NA_SE_EN_NUTS_UP,             {"Scrub Emerge",                        "NA_SE_EN_NUTS_UP",               SEQ_SFX}},
    {NA_SE_EN_AWA_BOUND,           {"Shabom Bounce",                       "NA_SE_EN_AWA_BOUND",             SEQ_SFX}},
    {NA_SE_EN_SHELL_MOUTH,         {"Shellblade",                          "NA_SE_EN_SHELL_MOUTH",           SEQ_SFX}},
    {NA_SE_EN_STALTU_DAMAGE,       {"Skulltula Damage",                    "NA_SE_EN_STALTU_DAMAGE",         SEQ_SFX}},
    {NA_SE_EN_NUTS_THROW,          {"Spit Nut",                            "NA_SE_EN_NUTS_THROW",            SEQ_SFX}},
    {NA_SE_VO_TA_CRY_0,            {"Talon Hmm",                           "NA_SE_VO_TA_CRY_0",              SEQ_SFX}},
    {NA_SE_VO_TA_SURPRISE,         {"Talon Surprised",                     "NA_SE_VO_TA_SURPRISE",           SEQ_SFX}},
    {NA_SE_SY_LOCK_ON,             {"Target Enemy",                        "NA_SE_SY_LOCK_ON",               SEQ_SFX}},
    {NA_SE_SY_LOCK_ON_HUMAN,       {"Target Neutral",                      "NA_SE_SY_LOCK_ON_HUMAN",         SEQ_SFX}},
    {NA_SE_SY_FSEL_CURSOR,         {"File Select Cursor",                  "NA_SE_SY_FSEL_CURSOR",           SEQ_SFX}},
    {NA_SE_SY_FSEL_DECIDE_L,       {"File Select Choose",                  "NA_SE_SY_FSEL_DECIDE_L",         SEQ_SFX}},
    {NA_SE_SY_FSEL_CLOSE,          {"File Select Back",                    "NA_SE_SY_FSEL_CLOSE",            SEQ_SFX}},
    {NA_SE_IT_BOMB_EXPLOSION,      {"Bomb Explosion",                      "NA_SE_IT_BOMB_EXPLOSION",        SEQ_SFX}},
    {NA_SE_EV_CHICKEN_CRY_A,       {"Chicken Cry",                         "NA_SE_EV_CHICKEN_CRY_A",         SEQ_SFX}},
};

void Draw_SfxTab(const std::string& tabId, const std::map<u16, std::tuple<std::string, std::string, SeqType>>& map, SeqType type) {
    const std::string hiddenTabId = "##" + tabId;
    const std::string resetAllButton = "Reset All" + hiddenTabId;
    const std::string randomizeAllButton = "Randomize All" + hiddenTabId;
    if (ImGui::Button(resetAllButton.c_str())) {
        for (const auto& [defaultValue, seqData] : map) {
            const auto& [name, sfxKey, seqType] = seqData;
            if (seqType == type) {
                // Only save authentic sequence CVars
                if (seqType == SEQ_FANFARE && defaultValue >= MAX_AUTHENTIC_SEQID) {
                    continue;
                }
                const std::string cvarKey = "gSfxEditor_" + sfxKey;
                CVar_SetS32(cvarKey.c_str(), defaultValue);
            }
        }
        SohImGui::RequestCvarSaveOnNextTick();
    }
    ImGui::SameLine();
    if (ImGui::Button(randomizeAllButton.c_str())) {
        std::vector<u16> values;
        for (const auto& [value, seqData] : map) {
            if (std::get<2>(seqData) & type) {
                values.push_back(value);
            }
        }
        Shuffle(values);
        for (const auto& [defaultValue, seqData] : map) {
            const auto& [name, sfxKey, seqType] = seqData;
            const std::string cvarKey = "gSfxEditor_" + sfxKey;
            if (seqType & type) {
                // Only save authentic sequence CVars
                if (((seqType & SEQ_BGM_CUSTOM) || seqType == SEQ_FANFARE) && defaultValue >= MAX_AUTHENTIC_SEQID) {
                    continue;
                }
                const int randomValue = values.back();
                CVar_SetS32(cvarKey.c_str(), randomValue);
                values.pop_back();
            }
        }
        SohImGui::RequestCvarSaveOnNextTick();
    }

    ImGui::BeginTable(tabId.c_str(), 3, ImGuiTableFlags_SizingFixedFit);
    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 190.0f);
    for (const auto& [defaultValue, seqData] : map) {
        const auto& [name, sfxKey, seqType] = seqData;

        if (~(seqType) & type) {
            continue;
        }
        // Do not display custom sequences in the list
        if (((seqType & SEQ_BGM_CUSTOM) || seqType == SEQ_FANFARE) && defaultValue >= MAX_AUTHENTIC_SEQID) {
            continue;
        }

        const std::string cvarKey = "gSfxEditor_" + sfxKey;
        const std::string hiddenKey = "##" + cvarKey;
        const std::string stopButton = " Stop  " + hiddenKey;
        const std::string previewButton = "Preview" + hiddenKey;
        const std::string resetButton = "Reset" + hiddenKey;
        const std::string randomizeButton = "Randomize" + hiddenKey;
        const int currentValue = CVar_GetS32(cvarKey.c_str(), defaultValue);

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text(name.c_str());
        ImGui::TableNextColumn();
        ImGui::PushItemWidth(-FLT_MIN);
        const int initialValue = map.contains(currentValue) ? currentValue : defaultValue;
        if (ImGui::BeginCombo(hiddenKey.c_str(), std::get<0>(map.at(initialValue)).c_str())) {
            for (const auto& [value, seqData] : map) {
                const auto& [name, sfxKey, seqType] = seqData;
                if (~(seqType) & type) {
                    continue;
                }

                if (ImGui::Selectable(std::get<0>(seqData).c_str())) {
                    CVar_SetS32(cvarKey.c_str(), value);
                    SohImGui::RequestCvarSaveOnNextTick();
                }
            }

            ImGui::EndCombo();
        }
        ImGui::TableNextColumn();
        ImGui::PushItemWidth(-FLT_MIN);
        if (CVar_GetS32("gSfxEditor_playing", 0) == currentValue) {
            if (ImGui::Button(stopButton.c_str())) {
                func_800F5C2C();
                CVar_SetS32("gSfxEditor_playing", 0);
            }
        } else {
            if (ImGui::Button(previewButton.c_str())) {
                if  (CVar_GetS32("gSfxEditor_playing", 0) != 0) {
                    func_800F5C2C();
                    CVar_SetS32("gSfxEditor_playing", 0);
                } else {
                    if (type == SEQ_SFX) {
                        Audio_PlaySoundGeneral(defaultValue, &pos, 4, &freqScale, &freqScale, &reverbAdd);
                    } else if (type == SEQ_INSTRUMENT) {
                        Audio_OcaSetInstrument(defaultValue - INSTRUMENT_OFFSET);
                        Audio_OcaSetSongPlayback(9, 1);
                    } else {
                        // TODO: Cant do both here, so have to click preview button twice
                        func_800F5ACC(defaultValue);
                        CVar_SetS32("gSfxEditor_playing", currentValue);
                    }
                }
            }
        }
        ImGui::SameLine();
        ImGui::PushItemWidth(-FLT_MIN);
        if (ImGui::Button(resetButton.c_str())) {
            CVar_SetS32(cvarKey.c_str(), defaultValue);
            SohImGui::RequestCvarSaveOnNextTick();
        }
        ImGui::SameLine();
        ImGui::PushItemWidth(-FLT_MIN);
        if (ImGui::Button(randomizeButton.c_str())) {
            auto it = map.begin();
            while (true) {
                const auto& [value, seqData] = *std::next(it, rand() % map.size());
                const auto& [name, sfxKey, seqType] = seqData;
                if (seqType & type) {
                    CVar_SetS32(cvarKey.c_str(), value);
                    SohImGui::RequestCvarSaveOnNextTick();
                    break;
                }
            }
        }
    }
    ImGui::EndTable();
}

extern "C" u16 SfxEditor_GetReplacementSeq(u16 seqId) {
    // if Hyrule Field Morning is about to play, but Hyrule Field is swapped, get the replacement sequence
    // for Hyrule Field instead. Otherwise, leave it alone, so that without any sfx editor modifications we will
    // play the normal track as usual.
    if (seqId == NA_BGM_FIELD_MORNING) {
        if (CVar_GetS32("gSfxEditor_NA_BGM_FIELD_LOGIC", NA_BGM_FIELD_LOGIC) != NA_BGM_FIELD_LOGIC) {
            seqId = NA_BGM_FIELD_LOGIC;
        }
    }
    
    if (sfxEditorSequenceMap.find(seqId) == sfxEditorSequenceMap.end()) {
        return seqId;
    }

    const auto& [name, sfxKey, seqType] = sfxEditorSequenceMap.at(seqId);
    const std::string cvarKey = "gSfxEditor_" + sfxKey;
    int replacementSeq = CVar_GetS32(cvarKey.c_str(), seqId);
    if (!sfxEditorSequenceMap.contains(replacementSeq)) {
        replacementSeq = seqId;
    }
    return static_cast<u16>(replacementSeq);
}

extern "C" u16 SfxEditor_GetReverseReplacementSeq(u16 seqId) {
    for (const auto& [id, nameAndsfxKey] : sfxEditorSequenceMap) {
        const auto& [name, sfxKey, seqType] = sfxEditorSequenceMap.at(id);
        const std::string cvarKey = "gSfxEditor_" + sfxKey;
        if (CVar_GetS32(cvarKey.c_str(), id) == seqId){
            return static_cast<u16>(id);
        }
    }

    return static_cast<u16>(seqId);
}

void DrawSfxEditor(bool& open) {
    if (!open) {
        CVar_SetS32("gSfxEditor", 0);
        return;
    }
    ImGui::SetNextWindowSize(ImVec2(900, 630), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("SFX Editor", &open)) {
        ImGui::End();
        return;
    }

    if (ImGui::BeginTabBar("SfxContextTabBar", ImGuiTabBarFlags_NoCloseWithMiddleMouseButton)) {
        if (ImGui::BeginTabItem("Background Music")) {
            Draw_SfxTab("backgroundMusic", sfxEditorSequenceMap, SEQ_BGM_WORLD);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Fanfares")) {
            Draw_SfxTab("fanfares", sfxEditorSequenceMap, SEQ_FANFARE);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Events")) {
            Draw_SfxTab("event", sfxEditorSequenceMap, SEQ_BGM_EVENT);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Battle Music")) {
            Draw_SfxTab("battleMusic", sfxEditorSequenceMap, SEQ_BGM_BATTLE);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Ocarina")) {
            Draw_SfxTab("instrument", sfxEditorSequenceMap, SEQ_INSTRUMENT);
            Draw_SfxTab("ocarina", sfxEditorSequenceMap, SEQ_OCARINA);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Sound Effects")) {
            Draw_SfxTab("sfx", sfxEditorSequenceMap, SEQ_SFX);
            ImGui::EndTabItem();
        }

        static ImVec2 cellPadding(8.0f, 8.0f);
        if (ImGui::BeginTabItem("Options")) {
            ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, cellPadding);
            ImGui::BeginTable("Options", 1, ImGuiTableFlags_SizingStretchSame);
            ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            if (ImGui::BeginChild("SfxOptions", ImVec2(0, -8))) {
                ImGui::PushItemWidth(-FLT_MIN);
                UIWidgets::EnhancementCheckbox("Disable Enemy Proximity Music", "gEnemyBGMDisable");
                UIWidgets::InsertHelpHoverText(
                    "Disables the music change when getting close to enemies. Useful for hearing "
                    "your custom music for each scene more often.");
                UIWidgets::PaddedSeparator();
                UIWidgets::PaddedText("The following options are experimental and may cause music\nto sound odd or have other undesireable effects.");
                UIWidgets::EnhancementCheckbox("Lower Octaves of Unplayable High Notes", "gExperimentalOctaveDrop");
                UIWidgets::InsertHelpHoverText("Some custom sequences may have notes that are too high for the game's audio "
                                            "engine to play. Enabling this checkbox will cause these notes to drop a "
                                            "couple of octaves so they can still harmonize with the other notes of the "
                                            "sequence.");
                ImGui::PopItemWidth();
            }
            ImGui::EndChild();
            ImGui::EndTable();
            ImGui::PopStyleVar(1);
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    ImGui::End();
}

void InitSfxEditor() {
    //Draw the bar in the menu.
    SohImGui::AddWindow("Enhancements", "SFX Editor", DrawSfxEditor);
}

extern "C" void SfxEditor_AddSequence(char *otrPath, uint16_t seqNum) {
    std::vector<std::string> splitName = StringHelper::Split(otrPath, "/");
    std::string fileName = splitName[splitName.size() - 1];
    std::vector<std::string> splitFileName = StringHelper::Split(fileName, "_");
    std::string sequenceName = splitFileName[0];
    SeqType type = SEQ_BGM_CUSTOM;
    std::string typeString = splitFileName[splitFileName.size() - 1];
    std::locale loc;
    for (int i = 0; i < typeString.length(); i++) {
        typeString[i] = std::tolower(typeString[i], loc);
    }
    if (typeString == "fanfare") {
        type = SEQ_FANFARE;
    }
    auto tuple = std::make_tuple(
        sequenceName, StringHelper::Replace(StringHelper::Replace(sequenceName, " ", "_"), "~", "-"), type);
    sfxEditorSequenceMap.emplace(seqNum, tuple);
}
