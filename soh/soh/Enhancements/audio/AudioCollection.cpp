#include "AudioCollection.h"
#include "sequence.h"
#include "sfx.h"
#include <vector>
#include <Utils/StringHelper.h>
#include <libultraship/bridge.h>
#include <libultraship/classes.h>
#include <locale>
#include <filesystem>

#define SEQUENCE_MAP_ENTRY(sequenceId, label, sfxKey, category, canBeReplaced, canBeUsedAsReplacement) \
    { sequenceId, { sequenceId, label, sfxKey, category, canBeReplaced, canBeUsedAsReplacement } }

AudioCollection::AudioCollection() {
    //                    (originalSequenceId,                  label,                                      sfxKey,                           category,    canBeReplaced, canBeUsedAsReplacement),
    sequenceMap = {

        // SEQ_BGM_WORLD
        SEQUENCE_MAP_ENTRY(NA_BGM_FIELD_LOGIC,                  "Hyrule Field",                             "NA_BGM_FIELD_LOGIC",             SEQ_BGM_WORLD,    true,     true),
        SEQUENCE_MAP_ENTRY(NA_BGM_DUNGEON,                      "Dodongo's Cavern",                         "NA_BGM_DUNGEON",                 SEQ_BGM_WORLD,    true,     true),
        SEQUENCE_MAP_ENTRY(NA_BGM_KAKARIKO_ADULT,               "Kakariko Village (Adult)",                 "NA_BGM_KAKARIKO_ADULT",          SEQ_BGM_WORLD,    true,     true),
        SEQUENCE_MAP_ENTRY(NA_BGM_INSIDE_DEKU_TREE,             "Inside the Deku Tree",                     "NA_BGM_INSIDE_DEKU_TREE",        SEQ_BGM_WORLD,    true,     true),
        SEQUENCE_MAP_ENTRY(NA_BGM_MARKET,                       "Market",                                   "NA_BGM_MARKET",                  SEQ_BGM_WORLD,    true,     true),
        SEQUENCE_MAP_ENTRY(NA_BGM_TITLE,                        "Title Theme",                              "NA_BGM_TITLE",                   SEQ_BGM_WORLD,    true,     true),
        SEQUENCE_MAP_ENTRY(NA_BGM_LINK_HOUSE,                   "House",                                    "NA_BGM_LINK_HOUSE",              SEQ_BGM_WORLD,    true,     true),
        SEQUENCE_MAP_ENTRY(NA_BGM_JABU_JABU,                    "Inside Jabu-Jabu's Belly",                 "NA_BGM_JABU_JABU",               SEQ_BGM_WORLD,    true,     true),
        SEQUENCE_MAP_ENTRY(NA_BGM_KAKARIKO_KID,                 "Kakariko Village (Child)",                 "NA_BGM_KAKARIKO_KID",            SEQ_BGM_WORLD,    true,     true),
        SEQUENCE_MAP_ENTRY(NA_BGM_FIRE_TEMPLE,                  "Fire Temple",                              "NA_BGM_FIRE_TEMPLE",             SEQ_BGM_WORLD,    true,     true),
        SEQUENCE_MAP_ENTRY(NA_BGM_FOREST_TEMPLE,                "Forest Temple",                            "NA_BGM_FOREST_TEMPLE",           SEQ_BGM_WORLD,    true,     true),
        SEQUENCE_MAP_ENTRY(NA_BGM_COURTYARD,                    "Hyrule Castle Courtyard",                  "NA_BGM_COURTYARD",               SEQ_BGM_WORLD,    true,     true),
        SEQUENCE_MAP_ENTRY(NA_BGM_GANON_TOWER,                  "Ganondorf's Theme",                        "NA_BGM_GANON_TOWER",             SEQ_BGM_WORLD,    true,     true),
        SEQUENCE_MAP_ENTRY(NA_BGM_LONLON,                       "Lon Lon Ranch",                            "NA_BGM_LONLON",                  SEQ_BGM_WORLD,    true,     true),
        SEQUENCE_MAP_ENTRY(NA_BGM_GORON_CITY,                   "Goron City",                               "NA_BGM_GORON_CITY",              SEQ_BGM_WORLD,    true,     true),
        SEQUENCE_MAP_ENTRY(NA_BGM_SARIA_THEME,                  "Lost Woods",                               "NA_BGM_SARIA_THEME",             SEQ_BGM_WORLD,    true,     true),
        SEQUENCE_MAP_ENTRY(NA_BGM_SPIRIT_TEMPLE,                "Spirit Temple",                            "NA_BGM_SPIRIT_TEMPLE",           SEQ_BGM_WORLD,    true,     true),
        SEQUENCE_MAP_ENTRY(NA_BGM_INGO,                         "Ingo's Theme",                             "NA_BGM_INGO",                    SEQ_BGM_WORLD,    true,     true),
        SEQUENCE_MAP_ENTRY(NA_BGM_KOKIRI,                       "Kokiri Forest",                            "NA_BGM_KOKIRI",                  SEQ_BGM_WORLD,    true,     true),
        SEQUENCE_MAP_ENTRY(NA_BGM_TEMPLE_OF_TIME,               "Temple of Time",                           "NA_BGM_TEMPLE_OF_TIME",          SEQ_BGM_WORLD,    true,     true),
        SEQUENCE_MAP_ENTRY(NA_BGM_WINDMILL,                     "Windmill Hut",                             "NA_BGM_WINDMILL",                SEQ_BGM_WORLD,    true,     true),
        SEQUENCE_MAP_ENTRY(NA_BGM_FILE_SELECT,                  "File Select",                              "NA_BGM_FILE_SELECT",             SEQ_BGM_WORLD,    true,     true),
        SEQUENCE_MAP_ENTRY(NA_BGM_ICE_CAVERN,                   "Ice Cavern",                               "NA_BGM_ICE_CAVERN",              SEQ_BGM_WORLD,    true,     true),
        SEQUENCE_MAP_ENTRY(NA_BGM_ZORA_DOMAIN,                  "Zora's Domain",                            "NA_BGM_ZORA_DOMAIN",             SEQ_BGM_WORLD,    true,     true),
        SEQUENCE_MAP_ENTRY(NA_BGM_SHOP,                         "Shop",                                     "NA_BGM_SHOP",                    SEQ_BGM_WORLD,    true,     true),
        SEQUENCE_MAP_ENTRY(NA_BGM_SHADOW_TEMPLE,                "Shadow Temple",                            "NA_BGM_SHADOW_TEMPLE",           SEQ_BGM_WORLD,    true,     true),
        SEQUENCE_MAP_ENTRY(NA_BGM_WATER_TEMPLE,                 "Water Temple",                             "NA_BGM_WATER_TEMPLE",            SEQ_BGM_WORLD,    true,     true),
        SEQUENCE_MAP_ENTRY(NA_BGM_UNDERGROUND,                  "Ganon's Castle Under Ground",              "NA_BGM_UNDERGROUND",             SEQ_BGM_WORLD,    true,     true),
        SEQUENCE_MAP_ENTRY(NA_BGM_GERUDO_VALLEY,                "Gerudo Valley",                            "NA_BGM_GERUDO_VALLEY",           SEQ_BGM_WORLD,    true,     true),
        SEQUENCE_MAP_ENTRY(NA_BGM_POTION_SHOP,                  "Potion Shop",                              "NA_BGM_POTION_SHOP",             SEQ_BGM_WORLD,    true,     true),

        // SEQ_BGM_BATTLE
        SEQUENCE_MAP_ENTRY(NA_BGM_ENEMY,                        "Battle",                                   "NA_BGM_ENEMY",                   SEQ_BGM_BATTLE,   true,     true),
        SEQUENCE_MAP_ENTRY(NA_BGM_BOSS,                         "Boss Battle",                              "NA_BGM_BOSS",                    SEQ_BGM_BATTLE,   true,     true),
        SEQUENCE_MAP_ENTRY(NA_BGM_MINI_BOSS,                    "Mini-Boss Battle",                         "NA_BGM_MINI_BOSS",               SEQ_BGM_BATTLE,   true,     true),
        SEQUENCE_MAP_ENTRY(NA_BGM_GANONDORF_BOSS,               "Ganondorf Battle",                         "NA_BGM_GANONDORF_BOSS",          SEQ_BGM_BATTLE,   true,     true),
        SEQUENCE_MAP_ENTRY(NA_BGM_GANON_BOSS,                   "Ganon Battle",                             "NA_BGM_GANON_BOSS",              SEQ_BGM_BATTLE,   true,     true),
        SEQUENCE_MAP_ENTRY(NA_BGM_FIRE_BOSS,                    "King Dodongo & Volvagia Boss Battle",      "NA_BGM_FIRE_BOSS",               SEQ_BGM_BATTLE,   true,     true),

        // SEQ_BGM_FANFARE
        SEQUENCE_MAP_ENTRY(NA_BGM_GAME_OVER,                    "Game Over",                                "NA_BGM_GAME_OVER",               SEQ_FANFARE,      true,     true),
        SEQUENCE_MAP_ENTRY(NA_BGM_BOSS_CLEAR,                   "Boss Clear",                               "NA_BGM_BOSS_CLEAR",              SEQ_FANFARE,      true,     true),
        SEQUENCE_MAP_ENTRY(NA_BGM_ITEM_GET,                     "Obtain Item",                              "NA_BGM_ITEM_GET",                SEQ_FANFARE,      true,     true),
        SEQUENCE_MAP_ENTRY(NA_BGM_OPENING_GANON,                "Enter Ganondorf",                          "NA_BGM_OPENING_GANON",           SEQ_FANFARE,      true,     true),
        SEQUENCE_MAP_ENTRY(NA_BGM_HEART_GET,                    "Obtain Heart Container",                   "NA_BGM_HEART_GET",               SEQ_FANFARE,      true,     true),
        SEQUENCE_MAP_ENTRY(NA_BGM_OPEN_TRE_BOX,                 "Open Treasure Chest",                      "NA_BGM_OPEN_TRE_BOX",            SEQ_FANFARE,      true,     true),
        SEQUENCE_MAP_ENTRY(NA_BGM_SPIRITUAL_STONE,              "Spiritual Stone Get",                      "NA_BGM_SPIRITUAL_STONE",         SEQ_FANFARE,      true,     true),
        SEQUENCE_MAP_ENTRY(NA_BGM_SMALL_ITEM_GET,               "Obtain Small Item",                        "NA_BGM_SMALL_ITEM_GET",          SEQ_FANFARE,      true,     true),
        SEQUENCE_MAP_ENTRY(NA_BGM_EVENT_CLEAR,                  "Escape from Lon Lon Ranch",                "NA_BGM_EVENT_CLEAR",             SEQ_FANFARE,      true,     true),
        SEQUENCE_MAP_ENTRY(NA_BGM_OCA_FAIRY_GET,                "Obtain Fairy Ocarina",                     "NA_BGM_OCA_FAIRY_GET",           SEQ_FANFARE,      true,     true),
        SEQUENCE_MAP_ENTRY(NA_BGM_HORSE_GOAL,                   "Horse Race Goal",                          "NA_BGM_HORSE_GOAL",              SEQ_FANFARE,      true,     true),
        SEQUENCE_MAP_ENTRY(NA_BGM_MEDALLION_GET,                "Obtain Medallion",                         "NA_BGM_MEDALLION_GET",           SEQ_FANFARE,      true,     true),
        SEQUENCE_MAP_ENTRY(NA_BGM_APPEAR,                       "Enter Zelda",                              "NA_BGM_APPEAR",                  SEQ_FANFARE,      true,     true),
        SEQUENCE_MAP_ENTRY(NA_BGM_MASTER_SWORD,                 "Master Sword",                             "NA_BGM_MASTER_SWORD",            SEQ_FANFARE,      true,     true),
        SEQUENCE_MAP_ENTRY(NA_BGM_OCARINA_OF_TIME,              "Ocarina of Time",                          "NA_BGM_OCARINA_OF_TIME",         SEQ_FANFARE,      true,     true),

        // SEQ_OCARINA
        SEQUENCE_MAP_ENTRY(NA_BGM_OCA_LIGHT,                    "Prelude of Light",                         "NA_BGM_OCA_LIGHT",               SEQ_OCARINA,      true,     true),
        SEQUENCE_MAP_ENTRY(NA_BGM_OCA_BOLERO,                   "Bolero of Fire",                           "NA_BGM_OCA_BOLERO",              SEQ_OCARINA,      true,     true),
        SEQUENCE_MAP_ENTRY(NA_BGM_OCA_MINUET,                   "Minuet of Forest",                         "NA_BGM_OCA_MINUET",              SEQ_OCARINA,      true,     true),
        SEQUENCE_MAP_ENTRY(NA_BGM_OCA_SERENADE,                 "Serenade of Water",                        "NA_BGM_OCA_SERENADE",            SEQ_OCARINA,      true,     true),
        SEQUENCE_MAP_ENTRY(NA_BGM_OCA_REQUIEM,                  "Requiem of Spirit",                        "NA_BGM_OCA_REQUIEM",             SEQ_OCARINA,      true,     true),
        SEQUENCE_MAP_ENTRY(NA_BGM_OCA_NOCTURNE,                 "Nocturne of Shadow",                       "NA_BGM_OCA_NOCTURNE",            SEQ_OCARINA,      true,     true),
        SEQUENCE_MAP_ENTRY(NA_BGM_OCA_SARIA,                    "Ocarina Saria's Song",                     "NA_BGM_OCA_SARIA",               SEQ_OCARINA,      true,     true),
        SEQUENCE_MAP_ENTRY(NA_BGM_OCA_EPONA,                    "Ocarina Epona's Song",                     "NA_BGM_OCA_EPONA",               SEQ_OCARINA,      true,     true),
        SEQUENCE_MAP_ENTRY(NA_BGM_OCA_ZELDA,                    "Ocarina Zelda's Lullaby",                  "NA_BGM_OCA_ZELDA",               SEQ_OCARINA,      true,     true),
        SEQUENCE_MAP_ENTRY(NA_BGM_OCA_SUNS,                     "Ocarina Sun's Song",                       "NA_BGM_OCA_SUNS",                SEQ_OCARINA,      true,     true),
        SEQUENCE_MAP_ENTRY(NA_BGM_OCA_TIME,                     "Ocarina Song of Time",                     "NA_BGM_OCA_TIME",                SEQ_OCARINA,      true,     true),
        SEQUENCE_MAP_ENTRY(NA_BGM_OCA_STORM,                    "Ocarina Song of Storms",                   "NA_BGM_OCA_STORM",               SEQ_OCARINA,      true,     true),

        // SEQ_BGM_EVENT
        SEQUENCE_MAP_ENTRY(NA_BGM_GREAT_FAIRY,                  "Great Fairy's Fountain",                   "NA_BGM_GREAT_FAIRY",             SEQ_BGM_EVENT,    true,     true),
        SEQUENCE_MAP_ENTRY(NA_BGM_ZELDA_THEME,                  "Zelda's Theme",                            "NA_BGM_ZELDA_THEME",             SEQ_BGM_EVENT,    true,     true),
        SEQUENCE_MAP_ENTRY(NA_BGM_HORSE,                        "Horse Race",                               "NA_BGM_HORSE",                   SEQ_BGM_EVENT,    true,     true),
        SEQUENCE_MAP_ENTRY(NA_BGM_NAVI_OPENING,                 "Fairy Flying",                             "NA_BGM_NAVI_OPENING",            SEQ_BGM_EVENT,    true,     true),
        SEQUENCE_MAP_ENTRY(NA_BGM_DEKU_TREE_CS,                 "Deku Tree",                                "NA_BGM_DEKU_TREE_CS",            SEQ_BGM_EVENT,    true,     true),
        SEQUENCE_MAP_ENTRY(NA_BGM_HYRULE_CS,                    "Legend of Hyrule",                         "NA_BGM_HYRULE_CS",               SEQ_BGM_EVENT,    true,     true),
        SEQUENCE_MAP_ENTRY(NA_BGM_MINI_GAME,                    "Shooting Gallery",                         "NA_BGM_MINI_GAME",               SEQ_BGM_EVENT,    true,     true),
        SEQUENCE_MAP_ENTRY(NA_BGM_SHEIK,                        "Sheik's Theme",                            "NA_BGM_SHEIK",                   SEQ_BGM_EVENT,    true,     true),
        SEQUENCE_MAP_ENTRY(NA_BGM_ADULT_LINK,                   "Goodbye to Zelda",                         "NA_BGM_ADULT_LINK",              SEQ_BGM_EVENT,    true,     true),
        SEQUENCE_MAP_ENTRY(NA_BGM_INTRO_GANON,                  "Ganon Intro",                              "NA_BGM_INTRO_GANON",             SEQ_BGM_EVENT,    true,     true),
        SEQUENCE_MAP_ENTRY(NA_BGM_CHAMBER_OF_SAGES,             "Chamber of the Sages",                     "NA_BGM_CHAMBER_OF_SAGES",        SEQ_BGM_EVENT,    true,     true),
        SEQUENCE_MAP_ENTRY(NA_BGM_DOOR_OF_TIME,                 "Open Door of Temple of Time",              "NA_BGM_DOOR_OF_TIME",            SEQ_BGM_EVENT,    true,     true),
        SEQUENCE_MAP_ENTRY(NA_BGM_OWL,                          "Kaepora Gaebora's Theme",                  "NA_BGM_OWL",                     SEQ_BGM_EVENT,    true,     true),
        SEQUENCE_MAP_ENTRY(NA_BGM_BRIDGE_TO_GANONS,             "Ganon's Castle Bridge",                    "NA_BGM_BRIDGE_TO_GANONS",        SEQ_BGM_EVENT,    true,     true),
        SEQUENCE_MAP_ENTRY(NA_BGM_KOTAKE_KOUME,                 "Kotake & Koume's Theme",                   "NA_BGM_KOTAKE_KOUME",            SEQ_BGM_EVENT,    true,     true),
        SEQUENCE_MAP_ENTRY(NA_BGM_ESCAPE,                       "Escape from Ganon's Castle",               "NA_BGM_ESCAPE",                  SEQ_BGM_EVENT,    true,     true),
        SEQUENCE_MAP_ENTRY(NA_BGM_TIMED_MINI_GAME,              "Mini-Game",                                "NA_BGM_TIMED_MINI_GAME",         SEQ_BGM_EVENT,    true,     true),

        // Previously SEQ_NOSHUFFLE
        SEQUENCE_MAP_ENTRY(NA_BGM_FIELD_MORNING,                "Hyrule Field Morning Theme",               "NA_BGM_FIELD_MORNING",           SEQ_BGM_EVENT,    false,    false), // Previously SEQ_UNUSED, so not shown anywhere?
        SEQUENCE_MAP_ENTRY(NA_BGM_END_DEMO,                     "Seal of Six Sages",                        "NA_BGM_END_DEMO",                SEQ_BGM_EVENT,    false,    false), // Previously SEQ_UNUSED, so not shown anywhere?
        SEQUENCE_MAP_ENTRY(NA_BGM_STAFF_1,                      "End Credits I",                            "NA_BGM_STAFF_1",                 SEQ_BGM_EVENT,    false,    false), // Previously SEQ_UNUSED, so not shown anywhere?
        SEQUENCE_MAP_ENTRY(NA_BGM_STAFF_2,                      "End Credits II",                           "NA_BGM_STAFF_2",                 SEQ_BGM_EVENT,    false,    false), // Previously SEQ_UNUSED, so not shown anywhere?
        SEQUENCE_MAP_ENTRY(NA_BGM_STAFF_3,                      "End Credits III",                          "NA_BGM_STAFF_3",                 SEQ_BGM_EVENT,    false,    false), // Previously SEQ_UNUSED, so not shown anywhere?
        SEQUENCE_MAP_ENTRY(NA_BGM_STAFF_4,                      "End Credits IV",                           "NA_BGM_STAFF_4",                 SEQ_BGM_EVENT,    false,    false), // Previously SEQ_UNUSED, so not shown anywhere?
        
        // SEQ_INSTRUMENT
        SEQUENCE_MAP_ENTRY(INSTRUMENT_OFFSET + 1,               "Ocarina",                                  "OCARINA_INSTRUMENT_DEFAULT",     SEQ_INSTRUMENT,   true,     true),
        SEQUENCE_MAP_ENTRY(INSTRUMENT_OFFSET + 2,               "Malon",                                    "OCARINA_INSTRUMENT_MALON",       SEQ_INSTRUMENT,   true,     true),
        SEQUENCE_MAP_ENTRY(INSTRUMENT_OFFSET + 3,               "Whistle",                                  "OCARINA_INSTRUMENT_WHISTLE",     SEQ_INSTRUMENT,   true,     true),
        SEQUENCE_MAP_ENTRY(INSTRUMENT_OFFSET + 4,               "Harp",                                     "OCARINA_INSTRUMENT_HARP",        SEQ_INSTRUMENT,   true,     true),
        SEQUENCE_MAP_ENTRY(INSTRUMENT_OFFSET + 5,               "Organ",                                    "OCARINA_INSTRUMENT_GRIND_ORGAN", SEQ_INSTRUMENT,   true,     true),
        SEQUENCE_MAP_ENTRY(INSTRUMENT_OFFSET + 6,               "Flute",                                    "OCARINA_INSTRUMENT_FLUTE",       SEQ_INSTRUMENT,   true,     true),

        // SEQ_SFX
        SEQUENCE_MAP_ENTRY(NA_SE_EV_SMALL_DOG_BARK,             "Bark",                                     "NA_SE_EV_SMALL_DOG_BARK",        SEQ_SFX,          true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_EN_AWA_BOUND,                  "Bomb Bounce",                              "NA_SE_EN_AWA_BOUND",             SEQ_SFX,          true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_EN_SHADEST_TAIKO_LOW,          "Bongo Bongo Low",                          "NA_SE_EN_SHADEST_TAIKO_LOW",     SEQ_SFX,          true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_EN_NUTS_FAINT,                 "Business Scrub",                           "NA_SE_EN_NUTS_FAINT",            SEQ_SFX,          true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_SY_CARROT_RECOVER,             "Carrot Refill",                            "NA_SE_SY_CARROT_RECOVER",        SEQ_SFX,          true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_EV_CHICKEN_CRY_N,              "Cluck",                                    "NA_SE_EV_CHICKEN_CRY_N",         SEQ_SFX,          true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_EV_BRIDGE_OPEN_STOP,           "Drawbridge Set",                           "NA_SE_EV_BRIDGE_OPEN_STOP",      SEQ_SFX,          true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_EN_KAICHO_CRY,                 "Guay",                                     "NA_SE_EN_KAICHO_CRY",            SEQ_SFX,          true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_SY_HITPOINT_ALARM,             "Low HP Beep",                              "NA_SE_SY_HITPOINT_ALARM",        SEQ_SFX,          true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_SY_HP_RECOVER,                 "HP Recover",                               "NA_SE_SY_HP_RECOVER",            SEQ_SFX,          true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_EV_HORSE_RUN,                  "Horse Trot",                               "NA_SE_EV_HORSE_RUN",             SEQ_SFX,          true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_PL_WALK_HEAVYBOOTS,            "Iron Boots",                               "NA_SE_PL_WALK_HEAVYBOOTS",       SEQ_SFX,          true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_PL_HOBBERBOOTS_LV - SFX_FLAG,  "Hover Boots",                              "NA_SE_PL_HOBBERBOOTS_LV",        SEQ_SFX,          true,     false),
        SEQUENCE_MAP_ENTRY(NA_SE_EV_COW_CRY,                    "Moo",                                      "NA_SE_EV_COW_CRY",               SEQ_SFX,          true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_EV_POT_BROKEN,                 "Pot Shattering",                           "NA_SE_EV_POT_BROKEN",            SEQ_SFX,          true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_EV_FROG_CRY_0,                 "Ribbit",                                   "NA_SE_EV_FROG_CRY_0",            SEQ_SFX,          true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_EV_FIVE_COUNT_LUPY,            "Rupee (Silver)",                           "NA_SE_EV_FIVE_COUNT_LUPY",       SEQ_SFX,          true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_EV_FOOT_SWITCH,                "Switch",                                   "NA_SE_EV_FOOT_SWITCH",           SEQ_SFX,          true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_IT_WALL_HIT_SOFT,              "Sword Bonk",                               "NA_SE_IT_WALL_HIT_SOFT",         SEQ_SFX,          true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_SY_METRONOME,                  "Tambourine",                               "NA_SE_SY_METRONOME",             SEQ_SFX,          true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_EN_AMOS_VOICE,                 "Armos",                                    "NA_SE_EN_AMOS_VOICE",            SEQ_SFX,          true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_EN_REDEAD_AIM,                 "Redead Scream",                            "NA_SE_EN_REDEAD_AIM",            SEQ_SFX,          true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_EN_STALKID_ATTACK,             "Stalchild Attack",                         "NA_SE_EN_STALKID_ATTACK",        SEQ_SFX,          true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_EV_CHICKEN_CRY_M,              "Cockadoodiedoo",                           "NA_SE_EV_CHICKEN_CRY_M",         SEQ_SFX,          true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_SY_KINSTA_MARK_APPEAR,         "Gold Skulltula Token",                     "NA_SE_SY_KINSTA_MARK_APPEAR",    SEQ_SFX,          true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_EN_REDEAD_CRY,                 "Redead Moan",                              "NA_SE_EN_REDEAD_CRY",            SEQ_SFX,          true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_EV_LIGHTNING,                  "Thunder",                                  "NA_SE_EV_LIGHTNING",             SEQ_SFX,          true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_OC_ABYSS,                      "Cartoon Fall",                             "NA_SE_OC_ABYSS",                 SEQ_SFX,          true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_EN_FLAME_LAUGH,                "Flare Dancer Laugh",                       "NA_SE_EN_FLAME_LAUGH",           SEQ_SFX,          true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_EN_AWA_BREAK,                  "Shabom Pop",                               "NA_SE_EN_AWA_BREAK",             SEQ_SFX,          true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_EN_SHADEST_TAIKO_HIGH,         "Bongo Bongo High",                         "NA_SE_EN_SHADEST_TAIKO_HIGH",    SEQ_SFX,          true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_EV_BOTTLE_CAP_OPEN,            "Bottle Cork",                              "NA_SE_EV_BOTTLE_CAP_OPEN",       SEQ_SFX,          true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_IT_BOW_FLICK,                  "Bow Twang",                                "NA_SE_IT_BOW_FLICK",             SEQ_SFX,          true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_EN_BUBLE_LAUGH,                "Bubble Laugh",                             "NA_SE_EN_BUBLE_LAUGH",           SEQ_SFX,          true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_EN_DEKU_JR_MOUTH,              "Deku Baba",                                "NA_SE_EN_DEKU_JR_MOUTH",         SEQ_SFX,          true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_EV_DOG_CRY_EVENING,            "Dusk Howl",                                "NA_SE_EV_DOG_CRY_EVENING",       SEQ_SFX,          true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_EN_FLAME_DAMAGE,               "Flare Dancer Startled",                    "NA_SE_EN_FLAME_DAMAGE",          SEQ_SFX,          true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_EN_GANON_AT_RETURN,            "Ganondorf Teh!",                           "NA_SE_EN_GANON_AT_RETURN",       SEQ_SFX,          true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_EN_GOMA_JR_CRY,                "Gohma Larva Croak",                        "NA_SE_EN_GOMA_JR_CRY",           SEQ_SFX,          true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_EN_GOLON_WAKE_UP,              "Goron Wake",                               "NA_SE_EN_GOLON_WAKE_UP",         SEQ_SFX,          true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_SY_START_SHOT,                 "Gunshot",                                  "NA_SE_SY_START_SHOT",            SEQ_SFX,          true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_IT_HAMMER_HIT,                 "Hammer Bonk",                              "NA_SE_IT_HAMMER_HIT",            SEQ_SFX,          true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_EN_IRONNACK_SWING_AXE,         "Iron Knuckle",                             "NA_SE_EN_IRONNACK_SWING_AXE",    SEQ_SFX,          true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_EN_FANTOM_ST_LAUGH,            "Phantom Ganon Laugh",                      "NA_SE_EN_FANTOM_ST_LAUGH",       SEQ_SFX,          true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_EV_PLANT_BROKEN,               "Plant Explode",                            "NA_SE_EV_PLANT_BROKEN",          SEQ_SFX,          true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_SY_GET_RUPY,                   "Rupee",                                    "NA_SE_SY_GET_RUPY",              SEQ_SFX,          true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_EN_NUTS_UP,                    "Scrub Emerge",                             "NA_SE_EN_NUTS_UP",               SEQ_SFX,          true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_EN_AWA_BOUND,                  "Shabom Bounce",                            "NA_SE_EN_AWA_BOUND",             SEQ_SFX,          true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_EN_SHELL_MOUTH,                "Shellblade",                               "NA_SE_EN_SHELL_MOUTH",           SEQ_SFX,          true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_EN_STALTU_DAMAGE,              "Skulltula Damage",                         "NA_SE_EN_STALTU_DAMAGE",         SEQ_SFX,          true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_EN_NUTS_THROW,                 "Spit Nut",                                 "NA_SE_EN_NUTS_THROW",            SEQ_SFX,          true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_SY_LOCK_ON,                    "Target Enemy",                             "NA_SE_SY_LOCK_ON",               SEQ_SFX,          true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_SY_LOCK_ON_HUMAN,              "Target Neutral",                           "NA_SE_SY_LOCK_ON_HUMAN",         SEQ_SFX,          true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_SY_FSEL_CURSOR,                "File Select Cursor",                       "NA_SE_SY_FSEL_CURSOR",           SEQ_SFX,          true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_SY_FSEL_DECIDE_L,              "File Select Choose",                       "NA_SE_SY_FSEL_DECIDE_L",         SEQ_SFX,          true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_SY_FSEL_CLOSE,                 "File Select Back",                         "NA_SE_SY_FSEL_CLOSE",            SEQ_SFX,          true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_IT_BOMB_EXPLOSION,             "Bomb Explosion",                           "NA_SE_IT_BOMB_EXPLOSION",        SEQ_SFX,          true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_EV_CHICKEN_CRY_A,              "Chicken Cry",                              "NA_SE_EV_CHICKEN_CRY_A",         SEQ_SFX,          true,     true),

        // SEQ_VOICE
        SEQUENCE_MAP_ENTRY(NA_SE_VO_LI_SWORD_N,                 "Adult Link - Slash",                       "NA_SE_VO_LI_SWORD_N",            SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_LI_SWORD_L,                 "Adult Link - Big Slash",                   "NA_SE_VO_LI_SWORD_L",            SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_LI_LASH,                    "Adult Link - Hookshot Latch Yell",         "NA_SE_VO_LI_LASH",               SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_LI_HANG,                    "Adult Link - Dangling Gasp",               "NA_SE_VO_LI_HANG",               SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_LI_CLIMB_END,               "Adult Link - Climb Edge",                  "NA_SE_VO_LI_CLIMB_END",          SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_LI_DAMAGE_S,                "Adult Link - Small Damage",                "NA_SE_VO_LI_DAMAGE_S",           SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_LI_FREEZE,                  "Adult Link - Freeze",                      "NA_SE_VO_LI_FREEZE",             SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_LI_FALL_S,                  "Adult Link - Fall Gasp",                   "NA_SE_VO_LI_FALL_S",             SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_LI_FALL_L,                  "Adult Link - Fall Scream",                 "NA_SE_VO_LI_FALL_L",             SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_LI_BREATH_REST,             "Adult Link - Low Health Sigh",             "NA_SE_VO_LI_BREATH_REST",        SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_LI_BREATH_DRINK,            "Adult Link - Bottle Sigh",                 "NA_SE_VO_LI_BREATH_DRINK",       SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_LI_DOWN,                    "Adult Link - Death",                       "NA_SE_VO_LI_DOWN",               SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_LI_TAKEN_AWAY,              "Adult Link - Taken away by Wallmaster",    "NA_SE_VO_LI_TAKEN_AWAY",         SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_LI_HELD,                    "Adult Link - Grabbed by Wallmaster",       "NA_SE_VO_LI_HELD",               SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_LI_SNEEZE,                  "Adult Link - Sneeze",                      "NA_SE_VO_LI_SNEEZE",             SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_LI_SWEAT,                   "Adult Link - Gasp (Hot Room)",             "NA_SE_VO_LI_SWEAT",              SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_LI_RELAX,                   "Adult Link - Yawn",                        "NA_SE_VO_LI_RELAX",              SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_LI_SWORD_PUTAWAY,           "Adult Link - Yell (Sword Putaway)",        "NA_SE_VO_LI_SWORD_PUTAWAY",      SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_LI_AUTO_JUMP,               "Adult Link - Jump/Swing Bottle",           "NA_SE_VO_LI_AUTOd_JUMP",         SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_LI_MAGIC_NALE,              "Adult Link - Use Nayru's Love",            "NA_SE_VO_LI_MAGIC_NALE",         SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_LI_SURPRISE,                "Adult Link - Suprised Gasp",               "NA_SE_VO_LI_SURPRISE",           SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_LI_MAGIC_FROL,              "Adult Link - Use Farore's Wind",           "NA_SE_VO_LI_MAGIC_FROL",         SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_LI_PUSH,                    "Adult Link - Push",                        "NA_SE_VO_LI_PUSH",               SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_LI_HOOKSHOT_HANG,           "Adult Link - Hookshot hang",               "NA_SE_VO_LI_HOOKSHOT_HANG",      SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_LI_LAND_DAMAGE_S,           "Adult Link - Fall damage",                 "NA_SE_VO_LI_LAND_DAMAGE_S",      SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_LI_NULL_0x1b,               "Adult Link - Small grunt (unused?)",       "NA_SE_VO_LI_NULL_0x1b",          SEQ_VOICE,        true,     true), // Unused? But unique so keeping it
        SEQUENCE_MAP_ENTRY(NA_SE_VO_LI_MAGIC_ATTACK,            "Adult Link - Din's Fire",                  "NA_SE_VO_LI_MAGIC_ATTACK",       SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_BL_DOWN,                    "Adult Link - Scream (Nocturne?)",          "NA_SE_VO_BL_DOWN",               SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_LI_DEMO_DAMAGE,             "Adult Link - Pained Land (Nocturne?)",     "NA_SE_VO_LI_DEMO_DAMAGE",        SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_LI_SWORD_N_KID,             "Child Link - Slash",                       "NA_SE_VO_LI_SWORD_N_KID",        SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_LI_ROLLING_CUT_KID,         "Child Link - Big Slash",                   "NA_SE_VO_LI_ROLLING_CUT_KID",    SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_LI_LASH_KID,                "Child Link - Hootshot Latch",              "NA_SE_VO_LI_LASH_KID",           SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_LI_HANG_KID,                "Child Link - Dangling Gasp",               "NA_SE_VO_LI_HANG_KID",           SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_LI_CLIMB_END_KID,           "Child Link - Climb Edge",                  "NA_SE_VO_LI_CLIMB_END_KID",      SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_LI_DAMAGE_S_KID,            "Child Link - Small Damage",                "NA_SE_VO_LI_DAMAGE_S_KID",       SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_LI_FREEZE_KID,              "Child Link - Freeze",                      "NA_SE_VO_LI_FREEZE_KID",         SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_LI_FALL_S_KID,              "Child Link - Fall Gasp",                   "NA_SE_VO_LI_FALL_S_KID",         SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_LI_FALL_L_KID,              "Child Link - Fall Scream",                 "NA_SE_VO_LI_FALL_L_KID",         SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_LI_BREATH_REST_KID,         "Child Link - Low Health Sigh",             "NA_SE_VO_LI_BREATH_REST_KID",    SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_LI_BREATH_DRINK_KID,        "Child Link - Bottle Sigh",                 "NA_SE_VO_LI_BREATH_DRINK_KID",   SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_LI_DOWN_KID,                "Child Link - Death",                       "NA_SE_VO_LI_DOWN_KID",           SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_LI_TAKEN_AWAY_KID,          "Child Link - Taken away by Wallmaster",    "NA_SE_VO_LI_TAKEN_AWAY_KID",     SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_LI_HELD_KID,                "Child Link - Grabbed by Wallmaster",       "NA_SE_VO_LI_HELD_KID",           SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_LI_SNEEZE_KID,              "Child Link - Sneeze",                      "NA_SE_VO_LI_SNEEZE_KID",         SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_LI_SWEAT_KID,               "Child Link - Gasp (Hot Room)",             "NA_SE_VO_LI_SWEAT_KID",          SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_LI_RELAX_KID,               "Child Link - Yawn",                        "NA_SE_VO_LI_RELAX_KID",          SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_LI_SWORD_PUTAWAY_KID,       "Child Link - Yell (Sword Putaway)",        "NA_SE_VO_LI_SWORD_PUTAWAY_KID",  SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_LI_AUTO_JUMP_KID,           "Child Link - Jump/Swing Bottle",           "NA_SE_VO_LI_AUTO_JUMP_KID",      SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_LI_MAGIC_NALE_KID,          "Child Link - Use Nayru's Love",            "NA_SE_VO_LI_MAGIC_NALE_KID",     SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_LI_SURPRISE_KID,            "Child Link - Surprised Gasp",              "NA_SE_VO_LI_SURPRISE_KID",       SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_LI_MAGIC_FROL_KID,          "Child Link - Use Farore's Wind",           "NA_SE_VO_LI_MAGIC_FROL_KID",     SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_LI_PUSH_KID,                "Child Link - Push",                        "NA_SE_VO_LI_PUSH_KID",           SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_LI_HOOKSHOT_HANG_KID,       "Child Link - Hookshot Hang",               "NA_SE_VO_LI_HOOKSHOT_HANG_KID",  SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_LI_LAND_DAMAGE_S_KID,       "Child Link - Fall Damage",                 "NA_SE_VO_LI_LAND_DAMAGE_S_KID",  SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_LI_NULL_0x1b_KID,           "Child Link - Small Gasp (unused?)",        "NA_SE_VO_LI_NULL_0x1b_KID",      SEQ_VOICE,        true,     true), // Unused? But unique so keeping it
        SEQUENCE_MAP_ENTRY(NA_SE_VO_LI_MAGIC_ATTACK_KID,        "Child Link - Din's Fire",                  "NA_SE_VO_LI_MAGIC_ATTACK_KID",   SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_BL_DOWN_KID,                "Child Link - Scream (Nocturne?)",          "NA_SE_VO_BL_DOWN_KID",           SEQ_VOICE,        true,     true), // ...
        SEQUENCE_MAP_ENTRY(NA_SE_VO_LI_DEMO_DAMAGE_KID,         "Child Link - Scream 2 (Nocturne?)",        "NA_SE_VO_LI_DEMO_DAMAGE_KID",    SEQ_VOICE,        true,     true), // Unused, matches with adult nocturne stuff
        SEQUENCE_MAP_ENTRY(NA_SE_VO_NAVY_ENEMY,                 "Navi - Unused Enemy Target",               "NA_SE_VO_NAVY_ENEMY",            SEQ_VOICE,        true,     true), // Has no sound but is played when targetting enemies, unsure to keep or not
        SEQUENCE_MAP_ENTRY(NA_SE_VO_NAVY_HELLO,                 "Navi - Unused NPC Hello",                  "NA_SE_VO_NAVY_HELLO",            SEQ_VOICE,        true,     true), // Has no sound but is played when targetting npcs, unsure to keep or not
        SEQUENCE_MAP_ENTRY(NA_SE_VO_NAVY_HEAR,                  "Navi - Unused Other Target",               "NA_SE_VO_NAVY_HEAR",             SEQ_VOICE,        true,     true), // Has no sound but is played when targetting npcs, unsure to keep or not
        SEQUENCE_MAP_ENTRY(NA_SE_VO_NAVY_CALL,                  "Navi - Look/Hey/Watchout (Target Enemy)",  "NA_SE_VO_NAVY_CALL",             SEQ_VOICE,        true,     true), // Has no sound but is played when targetting signs and other things, unsure to keep or not
        SEQUENCE_MAP_ENTRY(NA_SE_VO_NA_HELLO_3,                 "Navi - Hello",                             "NA_SE_VO_NA_HELLO_3",            SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_TA_SLEEP,                   "Talon - Snore",                            "NA_SE_VO_TA_SLEEP",              SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_TA_SURPRISE,                "Talon - Surprised",                        "NA_SE_VO_TA_SURPRISE",           SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_TA_CRY_0,                   "Talon - Hmm",                              "NA_SE_VO_TA_CRY_0",              SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_TA_CRY_1,                   "Talon - Scream",                           "NA_SE_VO_TA_CRY_1",              SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_IN_CRY_0,                   "Ingo - WAAAH!",                            "NA_SE_VO_IN_CRY_0",              SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_IN_LOST,                    "Ingo - KAAAAH! (Lost)",                    "NA_SE_VO_IN_LOST",               SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_IN_LASH_0,                  "Ingo - Heyeah (Horse 1)",                  "NA_SE_VO_IN_LASH_0",             SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_IN_LASH_1,                  "Ingo - Ha (Horse 2)",                      "NA_SE_VO_IN_LASH_1",             SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_FR_LAUGH_0,                 "Great Fairy - Big Laugh",                  "NA_SE_VO_FR_LAUGH_0",            SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_FR_SMILE_0,                 "Great Fairy - Small Laugh",                "NA_SE_VO_FR_SMILE_0",            SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_NB_AGONY,                   "Nabooru - Pained Gasp",                    "NA_SE_VO_NB_AGONY",              SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_NB_CRY_0,                   "Nabooru - Scream",                         "NA_SE_VO_NB_CRY_0",              SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_NB_NOTICE,                  "Nabooru - Hmm?",                           "NA_SE_VO_NB_NOTICE",             SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_NA_HELLO_0,                 "Navi - Watchout!",                         "NA_SE_VO_NA_HELLO_0",            SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_NA_HELLO_1,                 "Navi - Look!",                             "NA_SE_VO_NA_HELLO_1",            SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_NA_HELLO_2,                 "Navi - Hey! (C-up Sound)",                 "NA_SE_VO_NA_HELLO_2",            SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_RT_CRASH,                   "Ruto - Crash",                             "NA_SE_VO_RT_CRASH",              SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_RT_DISCOVER,                "Ruto - Found",                             "NA_SE_VO_RT_DISCOVER",           SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_RT_FALL,                    "Ruto - Fall",                              "NA_SE_VO_RT_FALL",               SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_RT_LAUGH_0,                 "Ruto - Giggle",                            "NA_SE_VO_RT_LAUGH_0",            SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_RT_LIFT,                    "Ruto - Lifted/Navi - Intro Bonk",          "NA_SE_VO_RT_LIFT",               SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_RT_THROW,                   "Ruto - Thrown",                            "NA_SE_VO_RT_THROW",              SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_RT_UNBALLANCE,              "Ruto - Held Tantrum",                      "NA_SE_VO_RT_UNBALLANCE",         SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_ST_DAMAGE,                  "Cursed Man - Scream",                      "NA_SE_VO_ST_DAMAGE",             SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_ST_ATTACK,                  "Cursed Man - Gasp",                        "NA_SE_VO_ST_ATTACK",             SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_Z0_HURRY,                   "Child Zelda - Hurry",                      "NA_SE_VO_Z0_HURRY",              SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_Z0_MEET,                    "Child Zelda - Meeting Link Gasp",          "NA_SE_VO_Z0_MEET",               SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_Z0_QUESTION,                "Child Zelda - Question",                   "NA_SE_VO_Z0_QUESTION",           SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_Z0_SIGH_0,                  "Child Zelda - Sigh",                       "NA_SE_VO_Z0_SIGH_0",             SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_Z0_SMILE_0,                 "Child Zelda - Laugh",                      "NA_SE_VO_Z0_SMILE_0",            SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_Z0_SURPRISE,                "Child Zelda - Sees Ganon Gasp",            "NA_SE_VO_Z0_SURPRISE",           SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_Z0_THROW,                   "Child Zelda - Throws Ocarina",             "NA_SE_VO_Z0_THROW",              SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_SK_CRY_0,                   "Sheik - Pained Gasp (Nocturne)",           "NA_SE_VO_SK_CRY_0",              SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_SK_CRY_1,                   "Sheik - Pained Scream (Nocturne)",         "NA_SE_VO_SK_CRY_1",              SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_SK_CRASH,                   "Sheik - Pained Landing (Nocturne)",        "NA_SE_VO_SK_CRASH",              SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_SK_LAUGH,                   "Navi - Listen!",                           "NA_SE_VO_SK_LAUGH",              SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_SK_SHOUT,                   "Sheik - Shout (Throwing Deku Nut)",        "NA_SE_VO_SK_SHOUT",              SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_Z1_CRY_0,                   "Adult Zelda - Scream 1",                   "NA_SE_VO_Z1_CRY_0",              SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_Z1_CRY_1,                   "Adult Zelda - Scream 2",                   "NA_SE_VO_Z1_CRY_1",              SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_Z1_OPENDOOR,                "Adult Zelda - Open Seseme Magic Gasp",     "NA_SE_VO_Z1_OPENDOOR",           SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_Z1_SURPRISE,                "Adult Zelda - Gasp",                       "NA_SE_VO_Z1_SURPRISE",           SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_Z1_PAIN,                    "Adult Zelda - Pained Gasp",                "NA_SE_VO_Z1_PAIN",               SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_KZ_MOVE,                    "King Zora - Mweep!",                       "NA_SE_VO_KZ_MOVE",               SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_NB_LAUGH,                   "Navi - Hello!",                            "NA_SE_VO_NB_LAUGH",              SEQ_VOICE,        true,     true),
        SEQUENCE_MAP_ENTRY(NA_SE_VO_LI_DRINK - SFX_FLAG,        "Adult Link - Drinking",                    "NA_SE_VO_LI_DRINK",              SEQ_VOICE,        true,     false),

        //SEQUENCE_MAP_ENTRY(NA_SE_VO_LI_DRINK, "Adult Link - Drinking",                        "NA_SE_VO_LI_DRINK",                    SEQ_VOICE, true, false), // Doesn't work due to SFX_FLAG
        //SEQUENCE_MAP_ENTRY(NA_SE_VO_LI_DRINK_KID, "Child Link - Drinking",                    "NA_SE_VO_LI_DRINK_KID",                SEQ_VOICE, true, false), // Doesn't work due to SFX_FLAG
        //SEQUENCE_MAP_ENTRY(NA_SE_VO_LI_GROAN, "Adult Link - Groan (Unused)",                  "NA_SE_VO_LI_GROAN",                    SEQ_VOICE, true, false),
        //SEQUENCE_MAP_ENTRY("Adult Link - Unused Sound 1?","NA_SE_VO_LI_ELECTRIC_SHOCK_LV",    "NA_SE_VO_LI_ELECTRIC_SHOCK_LV",        SEQ_VOICE, true, false),
        //SEQUENCE_MAP_ENTRY(NA_SE_VO_LI_GROAN_KID, "Child Link - Groan (Unused)",              "NA_SE_VO_LI_GROAN_KID",                SEQ_VOICE, true, false),
        //SEQUENCE_MAP_ENTRY(NA_SE_VO_LI_ELECTRIC_SHOCK_LV_KID, "Child Link - Unused Sound 1?", "NA_SE_VO_LI_ELECTRIC_SHOCK_LV_KID",    SEQ_VOICE, true, false),
        
        // Following group of Dummies are all duplicate entries for Navi saying Look/Hey/Watchout
        //SEQUENCE_MAP_ENTRY(NA_SE_VO_DUMMY_0x45,          "NA_SE_VO_DUMMY_0x45",                 "NA_SE_VO_DUMMY_0x45",            SEQ_VOICE, true, false),
        //SEQUENCE_MAP_ENTRY(NA_SE_VO_DUMMY_0x46,          "NA_SE_VO_DUMMY_0x46",                 "NA_SE_VO_DUMMY_0x46",            SEQ_VOICE, true, false),
        //SEQUENCE_MAP_ENTRY(NA_SE_VO_DUMMY_0x47,          "NA_SE_VO_DUMMY_0x47",                 "NA_SE_VO_DUMMY_0x47",            SEQ_VOICE, true, false),
        //SEQUENCE_MAP_ENTRY(NA_SE_VO_DUMMY_0x48,          "NA_SE_VO_DUMMY_0x48",                 "NA_SE_VO_DUMMY_0x48",            SEQ_VOICE, true, false),
        //SEQUENCE_MAP_ENTRY(NA_SE_VO_DUMMY_0x49,          "NA_SE_VO_DUMMY_0x49",                 "NA_SE_VO_DUMMY_0x49",            SEQ_VOICE, true, false),
        //SEQUENCE_MAP_ENTRY(NA_SE_VO_DUMMY_0x4a           "NA_SE_VO_DUMMY_0x4a",                 "NA_SE_VO_DUMMY_0x4a",            SEQ_VOICE, true, false),
        //SEQUENCE_MAP_ENTRY(NA_SE_VO_DUMMY_0x4b,          "NA_SE_VO_DUMMY_0x4b",                 "NA_SE_VO_DUMMY_0x4b",            SEQ_VOICE, true, false),
        //SEQUENCE_MAP_ENTRY(NA_SE_VO_DUMMY_0x4c,          "NA_SE_VO_DUMMY_0x4c",                 "NA_SE_VO_DUMMY_0x4c",            SEQ_VOICE, true, false),
        //SEQUENCE_MAP_ENTRY(NA_SE_VO_DUMMY_0x4d,          "NA_SE_VO_DUMMY_0x4d",                 "NA_SE_VO_DUMMY_0x4d",            SEQ_VOICE, true, false),
        //SEQUENCE_MAP_ENTRY(NA_SE_VO_DUMMY_0x4e,          "NA_SE_VO_DUMMY_0x4e",                 "NA_SE_VO_DUMMY_0x4e",            SEQ_VOICE, true, false),
        //SEQUENCE_MAP_ENTRY(NA_SE_VO_DUMMY_0x4f,          "NA_SE_VO_DUMMY_0x4f",                 "NA_SE_VO_DUMMY_0x4f",            SEQ_VOICE, true, false),

        //SEQUENCE_MAP_ENTRY(NA_SE_VO_IN_LAUGH,            "NA_SE_VO_IN_LAUGH",                   "NA_SE_VO_IN_LAUGH",              SEQ_VOICE, true, false), // Crashes
        //SEQUENCE_MAP_ENTRY(NA_SE_VO_DUMMY_0x7d,          "NA_SE_VO_DUMMY_0x7d",                 "NA_SE_VO_DUMMY_0x7d",            SEQ_VOICE, true, false), // Unused weird Child Link sound
        //SEQUENCE_MAP_ENTRY(NA_SE_VO_DUMMY_0x7e,          "NA_SE_VO_DUMMY_0x7e",                 "NA_SE_VO_DUMMY_0x7e",            SEQ_VOICE, true, false), // Unused weird Child Link sound
        //SEQUENCE_MAP_ENTRY(NA_SE_VO_DUMMY_0x7f,          "NA_SE_VO_DUMMY_0x7f",                 "NA_SE_VO_DUMMY_0x7f",            SEQ_VOICE, true, false), // Unused weird Child Link sound
        //SEQUENCE_MAP_ENTRY(NA_SE_VO_DUMMY_0x80,          "NA_SE_VO_DUMMY_0x80",                 "NA_SE_VO_DUMMY_0x80",            SEQ_VOICE, true, false), // Unused and empty
        //SEQUENCE_MAP_ENTRY(NA_SE_VO_LI_AUTO_JUMP_DARKLINK,"NA_SE_VO_LI_AUTO_JUMP_DARKLINK",     "NA_SE_VO_LI_AUTO_JUMP_DARKLINK", SEQ_VOICE, true, false), // Implies Dark Link but isn't used in anything
        //SEQUENCE_MAP_ENTRY(NA_SE_VO_DUMMY_0x82_YOBI,     "NA_SE_VO_DUMMY_0x82_YOBI",            "NA_SE_VO_DUMMY_0x82_YOBI",       SEQ_VOICE, true, false), // Rest are unused and empty
        //SEQUENCE_MAP_ENTRY(NA_SE_VO_DUMMY_0x83_YOBI,     "NA_SE_VO_DUMMY_0x83_YOBI",            "NA_SE_VO_DUMMY_0x83_YOBI",       SEQ_VOICE, true, false), // ..
        //SEQUENCE_MAP_ENTRY(NA_SE_VO_DUMMY_0x84_YOBI,     "NA_SE_VO_DUMMY_0x84_YOBI",            "NA_SE_VO_DUMMY_0x84_YOBI",       SEQ_VOICE, true, false), // ..
        //SEQUENCE_MAP_ENTRY(NA_SE_VO_DUMMY_0x85_YOBI,     "NA_SE_VO_DUMMY_0x85_YOBI",            "NA_SE_VO_DUMMY_0x85_YOBI",       SEQ_VOICE, true, false), // ..
        //SEQUENCE_MAP_ENTRY(NA_SE_VO_DUMMY_0x86_YOBI,     "NA_SE_VO_DUMMY_0x86_YOBI",            "NA_SE_VO_DUMMY_0x86_YOBI",       SEQ_VOICE, true, false), // ..
        //SEQUENCE_MAP_ENTRY(NA_SE_VO_DUMMY_0x87_YOBI,     "NA_SE_VO_DUMMY_0x87_YOBI",            "NA_SE_VO_DUMMY_0x87_YOBI",       SEQ_VOICE, true, false), // ..
        //SEQUENCE_MAP_ENTRY(NA_SE_VO_DUMMY_0x88_YOBI,     "NA_SE_VO_DUMMY_0x88_YOBI",            "NA_SE_VO_DUMMY_0x88_YOBI",       SEQ_VOICE, true, false), // ..
        //SEQUENCE_MAP_ENTRY(NA_SE_VO_DUMMY_0x89_YOBI,     "NA_SE_VO_DUMMY_0x89_YOBI",            "NA_SE_VO_DUMMY_0x89_YOBI",       SEQ_VOICE, true, false), // ..
    };

}

std::string AudioCollection::GetCvarKey(std::string sfxKey) {
    return "gAudioEditor.ReplacedSequences." + sfxKey + ".value";
}

std::string AudioCollection::GetCvarLockKey(std::string sfxKey) {
    return "gAudioEditor.ReplacedSequences." + sfxKey + ".locked";
}

void AudioCollection::AddToCollection(char* otrPath, uint16_t seqNum) {
    std::string fileName = std::filesystem::path(otrPath).filename().string();
    std::vector<std::string> splitFileName = StringHelper::Split(fileName, "_");
    std::string sequenceName = splitFileName[0];
    SeqType type = SEQ_BGM_CUSTOM;
    std::string typeString = splitFileName[splitFileName.size() - 1];
    std::locale loc;
    for (size_t i = 0; i < typeString.length(); i++) {
        typeString[i] = std::tolower(typeString[i], loc);
    }
    if (typeString == "fanfare") {
        type = SEQ_FANFARE;
    }
    SequenceInfo info = {seqNum,
                         sequenceName,
                         StringHelper::Replace(StringHelper::Replace(StringHelper::Replace(sequenceName, " ", "_"), "~", "-"),".", ""),
                         type, false, true};
    sequenceMap.emplace(seqNum, info);
}

uint16_t AudioCollection::GetReplacementSequence(uint16_t seqId) {
    // if Hyrule Field Morning is about to play, but Hyrule Field is swapped, get the replacement sequence
    // for Hyrule Field instead. Otherwise, leave it alone, so that without any sfx editor modifications we will
    // play the normal track as usual.
    if (seqId == NA_BGM_FIELD_MORNING) {
        if (CVarGetInteger("gAudioEditor.ReplacedSequences.NA_BGM_FIELD_LOGIC.value", NA_BGM_FIELD_LOGIC) != NA_BGM_FIELD_LOGIC) {
            seqId = NA_BGM_FIELD_LOGIC;
        }
    }

    if (sequenceMap.find(seqId) == sequenceMap.end()) {
        return seqId;
    }

    const auto& sequenceInfo = sequenceMap.at(seqId);
    const std::string cvarKey = GetCvarKey(sequenceInfo.sfxKey);
    int replacementSeq = CVarGetInteger(cvarKey.c_str(), seqId);
    if (!sequenceMap.contains(replacementSeq)) {
        replacementSeq = seqId;
    }
    return static_cast<uint16_t>(replacementSeq);
}

void AudioCollection::RemoveFromShufflePool(SequenceInfo* seqInfo) {
    const std::string cvarKey = "gAudioEditor.Excluded." + seqInfo->sfxKey;
    excludedSequences.insert(seqInfo);
    includedSequences.erase(seqInfo);
    CVarSetInteger(cvarKey.c_str(), 1);
    LUS::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
}

void AudioCollection::AddToShufflePool(SequenceInfo* seqInfo) {
    const std::string cvarKey = "gAudioEditor.Excluded." + seqInfo->sfxKey;
    includedSequences.insert(seqInfo);
    excludedSequences.erase(seqInfo);
    CVarClear(cvarKey.c_str());
    LUS::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
}

void AudioCollection::InitializeShufflePool() {
    if (shufflePoolInitialized) return;
    
    for (auto& [seqId, seqInfo] : sequenceMap) {
        const std::string cvarKey = "gAudioEditor.Excluded." + seqInfo.sfxKey;
        if (CVarGetInteger(cvarKey.c_str(), 0) && !seqInfo.canBeUsedAsReplacement) {
            excludedSequences.insert(&seqInfo);
        } else {
            includedSequences.insert(&seqInfo);
        }
    }

    shufflePoolInitialized = true;
};

extern "C" void AudioCollection_AddToCollection(char *otrPath, uint16_t seqNum) {
    AudioCollection::Instance->AddToCollection(otrPath, seqNum);
}

bool AudioCollection::HasSequenceNum(uint16_t seqId) {
    return sequenceMap.contains(seqId);
}

const char* AudioCollection::GetSequenceName(uint16_t seqId) {
    auto seqIt = sequenceMap.find(seqId);
    if (seqIt != sequenceMap.end()) {
        return seqIt->second.label.c_str();
    }
    return nullptr;
}

size_t AudioCollection::SequenceMapSize() {
    return sequenceMap.size();
}

extern "C" const char* AudioCollection_GetSequenceName(uint16_t seqId) {
    return AudioCollection::Instance->GetSequenceName(seqId);
}

extern "C" bool AudioCollection_HasSequenceNum(uint16_t seqId) {
    return AudioCollection::Instance->HasSequenceNum(seqId);
}

extern "C" size_t AudioCollection_SequenceMapSize() {
    return AudioCollection::Instance->SequenceMapSize();
}
