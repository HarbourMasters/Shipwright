#include "AudioCollection.h"
#include "sequence.h"
#include <ship/utils/StringHelper.h>
#include <libultraship/bridge/consolevariablebridge.h>
#include <ship/window/Window.h>
#include <2s2h/BenPort.h>
#include <locale>
#include <filesystem>

#define SEQUENCE_MAP_ENTRY(sequenceId, label, sfxKey, type, categoryFlags, seqIdReplacements, canBeReplaced,         \
                           canBeUsedAsReplacement)                                                                   \
    {                                                                                                                \
        sequenceId, {                                                                                                \
            sequenceId, label, sfxKey, type, categoryFlags, seqIdReplacements, canBeReplaced, canBeUsedAsReplacement \
        }                                                                                                            \
    }

AudioCollection::AudioCollection() {
    //                    (originalSequenceId,                  label,                                      sfxKey,
    //                    type, categoryFlags, seqIdReplacements, canBeReplaced, canBeUsedAsReplacement),
    mSequenceMap = {
        SEQUENCE_MAP_ENTRY(NA_BGM_GENERAL_SFX, "General SFX", "SEQUENCE_MAP_ENTRY", SEQ_SFX, SEQ_CAT_NONE, nullptr,
                           false, false),
        SEQUENCE_MAP_ENTRY(NA_BGM_AMBIENCE, "Ambience", "NA_BGM_AMBIENCE", SEQ_BGM_WORLD, SEQ_CAT_NONE, nullptr, false,
                           false),
        SEQUENCE_MAP_ENTRY(NA_BGM_TERMINA_FIELD, "Termina Field", "NA_BGM_TERMINA_FIELD", SEQ_BGM_WORLD, SEQ_CAT_FIELD,
                           nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_CHASE, "Chase", "NA_BGM_CHASE", SEQ_BGM_BATTLE, SEQ_CAT_ACTION | SEQ_CAT_BOSS,
                           nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_MAJORAS_THEME, "Majora's Theme", "NA_BGM_MAJORAS_THEME", SEQ_BGM_WORLD, SEQ_CAT_CALM,
                           nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_CLOCK_TOWER, "Clock Tower", "NA_BGM_CLOCK_TOWER", SEQ_BGM_WORLD,
                           SEQ_CAT_INDOOR | SEQ_CAT_CALM, nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_STONE_TOWER_TEMPLE, "Stone Tower Temple", "NA_BGM_STONE_TOWER_TEMPLE", SEQ_BGM_WORLD,
                           SEQ_CAT_DUNGEON, nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_INV_STONE_TOWER_TEMPLE, "Inverted Stone Tower Temple",
                           "NA_BGM_INV_STONE_TOWER_TEMPLE", SEQ_BGM_WORLD, SEQ_CAT_DUNGEON, nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_FAILURE_0, "Missed Event 0", "NA_BGM_FAILURE_0", SEQ_BGM_EVENT, SEQ_CAT_FAN_GAMEOVER,
                           nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_FAILURE_1, "Missed Event 1", "NA_BGM_FAILURE_1", SEQ_BGM_EVENT, SEQ_CAT_FAN_GAMEOVER,
                           nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_HAPPY_MASK_SALESMAN, "Happy Mask Salesman's Theme", "NA_BGM_HAPPY_MASK_SALESMAN",
                           SEQ_BGM_WORLD, SEQ_CAT_CALM, nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_SONG_OF_HEALING, "Song Of Healing", "NA_BGM_SONG_OF_HEALING", SEQ_BGM_WORLD,
                           SEQ_CAT_CALM | SEQ_CAT_INDOOR, nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_SWAMP_REGION, "Southern Swamp", "NA_BGM_SWAMP_REGION", SEQ_BGM_WORLD,
                           SEQ_CAT_FIELD | SEQ_CAT_TOWN, nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_ALIEN_INVASION, "Alien Invasion", "NA_BGM_ALIEN_INVASION", SEQ_BGM_WORLD,
                           SEQ_CAT_ACTION | SEQ_CAT_CALM, nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_SWAMP_CRUISE, "Boat Cruise", "NA_BGM_SWAMP_CRUISE", SEQ_BGM_WORLD,
                           SEQ_CAT_MINIGAME | SEQ_CAT_INDOOR | SEQ_CAT_CALM, nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_SHARPS_CURSE, "Sharp's Curse", "NA_BGM_SHARPS_CURSE", SEQ_BGM_WORLD,
                           SEQ_CAT_INDOOR | SEQ_CAT_CALM | SEQ_CAT_ACTION, nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_GREAT_BAY_REGION, "Great Bay", "NA_BGM_GREAT_BAY_REGION", SEQ_BGM_WORLD,
                           SEQ_CAT_FIELD | SEQ_CAT_TOWN, nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_IKANA_REGION, "Ikana", "NA_BGM_IKANA_REGION", SEQ_BGM_WORLD,
                           SEQ_CAT_FIELD | SEQ_CAT_TOWN, nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_DEKU_PALACE, "Deku Palace", "NA_BGM_DEKU_PALACE", SEQ_BGM_WORLD,
                           SEQ_CAT_TOWN | SEQ_CAT_FIELD, nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_MOUNTAIN_REGION, "Mountain Region", "NA_BGM_MOUNTAIN_REGION", SEQ_BGM_WORLD,
                           SEQ_CAT_FIELD, nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_PIRATES_FORTRESS, "Pirates Fortress", "NA_BGM_PIRATES_FORTRESS", SEQ_BGM_WORLD,
                           SEQ_CAT_DUNGEON, nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_CLOCK_TOWN_DAY_1, "Clock Town Day 1", "NA_BGM_CLOCK_TOWN_DAY_1", SEQ_BGM_WORLD,
                           SEQ_CAT_TOWN, nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_CLOCK_TOWN_DAY_2, "Clock Town Day 2", "NA_BGM_CLOCK_TOWN_DAY_2", SEQ_BGM_WORLD,
                           SEQ_CAT_TOWN, nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_CLOCK_TOWN_DAY_3, "Clock Town Day 3", "NA_BGM_CLOCK_TOWN_DAY_3", SEQ_BGM_WORLD,
                           SEQ_CAT_TOWN, nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_FILE_SELECT, "File Select", "NA_BGM_FILE_SELECT", SEQ_BGM_WORLD,
                           SEQ_CAT_CALM | SEQ_CAT_INDOOR, nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_CLEAR_EVENT, "Clear Event", "NA_BGM_CLEAR_EVENT", SEQ_BGM_EVENT, SEQ_CAT_FAN_GETITEM,
                           nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_ENEMY, "Enemy", "NA_BGM_ENEMY", SEQ_BGM_BATTLE, SEQ_CAT_ACTION | SEQ_CAT_MINIGAME,
                           nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_BOSS, "Boss", "NA_BGM_BOSS", SEQ_BGM_BATTLE, SEQ_CAT_BOSS, nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_WOODFALL_TEMPLE, "Woodfall Temple", "NA_BGM_WOODFALL_TEMPLE", SEQ_BGM_WORLD,
                           SEQ_CAT_DUNGEON, nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_CLOCK_TOWN_MAIN_SEQUENCE, "Clock Town Main Sequence",
                           "NA_BGM_CLOCK_TOWN_MAIN_SEQUENCE", SEQ_BGM_WORLD, SEQ_CAT_NONE, nullptr, false, false),
        SEQUENCE_MAP_ENTRY(NA_BGM_OPENING, "Opening", "NA_BGM_OPENING", SEQ_BGM_WORLD, SEQ_CAT_NONE, nullptr, true,
                           true),
        SEQUENCE_MAP_ENTRY(NA_BGM_INSIDE_A_HOUSE, "Inside House", "NA_BGM_INSIDE_A_HOUSE", SEQ_BGM_WORLD,
                           SEQ_CAT_INDOOR, nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_GAME_OVER, "Game Over", "NA_BGM_GAME_OVER", SEQ_BGM_EVENT, SEQ_CAT_FAN_GAMEOVER,
                           nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_CLEAR_BOSS, "Clear Boss", "NA_BGM_CLEAR_BOSS", SEQ_BGM_EVENT,
                           SEQ_CAT_FAN_CLEAR | SEQ_CAT_FAN_GAMEOVER, nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_GET_ITEM, "Get Item", "NA_BGM_GET_ITEM", SEQ_FANFARE, SEQ_CAT_FAN_GETITEM, nullptr,
                           true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_CLOCK_TOWN_DAY_2_PTR, "Clock Town Day 2 (Alt)", "NA_BGM_CLOCK_TOWN_DAY_2_PTR",
                           SEQ_BGM_WORLD, SEQ_CAT_NONE, nullptr, false, false),
        SEQUENCE_MAP_ENTRY(NA_BGM_GET_HEART, "Get Heart", "NA_BGM_GET_HEART", SEQ_FANFARE, SEQ_CAT_FAN_GETITEM, nullptr,
                           true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_TIMED_MINI_GAME, "Timed Minigame", "NA_BGM_TIMED_MINI_GAME", SEQ_BGM_WORLD,
                           SEQ_CAT_MINIGAME | SEQ_CAT_ACTION, nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_GORON_RACE, "Goron Race", "NA_BGM_GORON_RACE", SEQ_BGM_WORLD, SEQ_CAT_MINIGAME,
                           nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_MUSIC_BOX_HOUSE, "Music Box House", "NA_BGM_MUSIC_BOX_HOUSE", SEQ_BGM_WORLD,
                           SEQ_CAT_INDOOR | SEQ_CAT_MINIGAME | SEQ_CAT_CALM, nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_FAIRY_FOUNTAIN, "Fairy Fountain", "NA_BGM_FAIRY_FOUNTAIN", SEQ_BGM_WORLD,
                           SEQ_CAT_CALM | SEQ_CAT_TOWN | SEQ_CAT_INDOOR, nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_ZELDAS_LULLABY, "Zelda's Lullaby", "NA_BGM_ZELDAS_LULLABY", SEQ_BGM_WORLD,
                           SEQ_CAT_CALM | SEQ_CAT_TOWN | SEQ_CAT_INDOOR, nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_ROSA_SISTERS, "Rosa Sisters", "NA_BGM_ROSA_SISTERS", SEQ_BGM_WORLD, SEQ_CAT_NONE,
                           nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_OPEN_CHEST, "Open Chest", "NA_BGM_OPEN_CHEST", SEQ_BGM_EVENT, SEQ_CAT_FAN_GETITEM,
                           nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_MARINE_RESEARCH_LAB, "Marine Research Lab", "NA_BGM_MARINE_RESEARCH_LAB",
                           SEQ_BGM_WORLD, SEQ_CAT_INDOOR | SEQ_CAT_CALM, nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_GIANTS_THEME, "Giants Theme", "NA_BGM_GIANTS_THEME", SEQ_BGM_WORLD,
                           SEQ_CAT_CALM | SEQ_CAT_INDOOR, nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_SONG_OF_STORMS, "Song Of Storms", "NA_BGM_SONG_OF_STORMS", SEQ_BGM_WORLD,
                           SEQ_CAT_INDOOR | SEQ_CAT_CALM, nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_ROMANI_RANCH, "Romani Ranch", "NA_BGM_ROMANI_RANCH", SEQ_BGM_WORLD,
                           SEQ_CAT_FIELD | SEQ_CAT_TOWN, nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_GORON_VILLAGE, "Goron Village", "NA_BGM_GORON_VILLAGE", SEQ_BGM_WORLD,
                           SEQ_CAT_TOWN | SEQ_CAT_INDOOR, nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_MAYORS_OFFICE, "Mayors Office", "NA_BGM_MAYORS_OFFICE", SEQ_BGM_WORLD,
                           SEQ_CAT_ACTION | SEQ_CAT_CALM | SEQ_CAT_INDOOR, nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_OCARINA_EPONA, "Epona's Song", "NA_BGM_OCARINA_EPONA", SEQ_OCARINA, SEQ_CAT_NONE,
                           nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_OCARINA_SUNS, "Sun's Song", "NA_BGM_OCARINA_SUNS", SEQ_OCARINA, SEQ_CAT_NONE, nullptr,
                           true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_OCARINA_TIME, "Song Of Time", "NA_BGM_OCARINA_TIME", SEQ_OCARINA, SEQ_CAT_NONE,
                           nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_OCARINA_STORM, "Song of Storms (Ocarina)", "NA_BGM_OCARINA_STORM", SEQ_OCARINA,
                           SEQ_CAT_NONE, nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_ZORA_HALL, "Zora Hall", "NA_BGM_ZORA_HALL", SEQ_BGM_WORLD,
                           SEQ_CAT_TOWN | SEQ_CAT_INDOOR, nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_GET_NEW_MASK, "Get Mask", "NA_BGM_GET_NEW_MASK", SEQ_FANFARE, SEQ_CAT_FAN_GETITEM,
                           nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_MINI_BOSS, "Mini Boss", "NA_BGM_MINI_BOSS", SEQ_BGM_BATTLE, SEQ_CAT_BOSS, nullptr,
                           true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_GET_SMALL_ITEM, "Get Small Item", "NA_BGM_GET_SMALL_ITEM", SEQ_FANFARE,
                           SEQ_CAT_FAN_GETITEM, nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_ASTRAL_OBSERVATORY, "Astral Observatory", "NA_BGM_ASTRAL_OBSERVATORY", SEQ_BGM_WORLD,
                           SEQ_CAT_INDOOR, nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_CAVERN, "Cavern", "NA_BGM_CAVERN", SEQ_BGM_WORLD, SEQ_CAT_DUNGEON, nullptr, true,
                           true),
        SEQUENCE_MAP_ENTRY(NA_BGM_MILK_BAR, "Milk Bar Day", "NA_BGM_MILK_BAR", SEQ_BGM_WORLD,
                           SEQ_CAT_INDOOR | SEQ_CAT_MINIGAME, nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_ZELDA_APPEAR, "Zelda Appear", "NA_BGM_ZELDA_APPEAR", SEQ_FANFARE, SEQ_CAT_FAN_GETITEM,
                           nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_SARIAS_SONG, "Saria's Song", "NA_BGM_SARIAS_SONG", SEQ_BGM_WORLD,
                           SEQ_CAT_CALM | SEQ_CAT_TOWN | SEQ_CAT_INDOOR | SEQ_CAT_MINIGAME | SEQ_CAT_FIELD, nullptr,
                           true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_GORON_GOAL, "Goron Race Goal", "NA_BGM_GORON_GOAL", SEQ_FANFARE, SEQ_CAT_FAN_GETITEM,
                           nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_HORSE, "Horse Race", "NA_BGM_HORSE", SEQ_BGM_WORLD, SEQ_CAT_MINIGAME, nullptr, true,
                           true),
        SEQUENCE_MAP_ENTRY(NA_BGM_HORSE_GOAL, "Horse Race Goal", "NA_BGM_HORSE_GOAL", SEQ_FANFARE, SEQ_CAT_FAN_GETITEM,
                           nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_INGO, "Ingo", "NA_BGM_INGO", SEQ_BGM_WORLD, SEQ_CAT_CALM | SEQ_CAT_TOWN, nullptr,
                           true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_KOTAKE_POTION_SHOP, "Potion Shop (Kotake)", "NA_BGM_KOTAKE_POTION_SHOP",
                           SEQ_BGM_WORLD, SEQ_CAT_CALM | SEQ_CAT_INDOOR, nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_SHOP, "Shop", "NA_BGM_SHOP", SEQ_BGM_WORLD, SEQ_CAT_INDOOR, nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_OWL, "Owl", "NA_BGM_OWL", SEQ_BGM_WORLD, SEQ_CAT_CALM, nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_SHOOTING_GALLERY, "Shooting Gallery", "NA_BGM_SHOOTING_GALLERY", SEQ_BGM_WORLD,
                           SEQ_CAT_INDOOR | SEQ_CAT_MINIGAME, nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_OCARINA_SOARING, "Song Of Soaring", "NA_BGM_OCARINA_SOARING", SEQ_OCARINA,
                           SEQ_CAT_NONE, nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_OCARINA_HEALING, "Song Of Healing", "NA_BGM_OCARINA_HEALING", SEQ_OCARINA,
                           SEQ_CAT_NONE, nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_INVERTED_SONG_OF_TIME, "Inverted Song Of Time", "NA_BGM_INVERTED_SONG_OF_TIME",
                           SEQ_OCARINA, SEQ_CAT_NONE, nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_SONG_OF_DOUBLE_TIME, "Song Of Double Time", "NA_BGM_SONG_OF_DOUBLE_TIME", SEQ_OCARINA,
                           SEQ_CAT_NONE, nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_SONATA_OF_AWAKENING, "Sonata Of Awakening", "NA_BGM_SONATA_OF_AWAKENING",
                           SEQ_BGM_SONGS, SEQ_CAT_NONE, nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_GORON_LULLABY, "Goron Lullaby", "NA_BGM_GORON_LULLABY", SEQ_BGM_SONGS, SEQ_CAT_NONE,
                           nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_NEW_WAVE_BOSSA_NOVA, "New Wave Bossa Nova", "NA_BGM_NEW_WAVE_BOSSA_NOVA",
                           SEQ_BGM_SONGS, SEQ_CAT_NONE, nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_ELEGY_OF_EMPTINESS, "Elegy Of Emptiness", "NA_BGM_ELEGY_OF_EMPTINESS", SEQ_BGM_SONGS,
                           SEQ_CAT_NONE, nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_OATH_TO_ORDER, "Oath To Order", "NA_BGM_OATH_TO_ORDER", SEQ_BGM_SONGS, SEQ_CAT_NONE,
                           nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_SWORD_TRAINING_HALL, "Sword Training", "NA_BGM_SWORD_TRAINING_HALL", SEQ_BGM_WORLD,
                           SEQ_CAT_INDOOR | SEQ_CAT_ACTION | SEQ_CAT_MINIGAME, nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_OCARINA_LULLABY_INTRO, "Lullaby Intro", "NA_BGM_OCARINA_LULLABY_INTRO", SEQ_OCARINA,
                           SEQ_CAT_NONE, nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_LEARNED_NEW_SONG, "Get Song", "NA_BGM_LEARNED_NEW_SONG", SEQ_FANFARE,
                           SEQ_CAT_FAN_GETITEM, nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_BREMEN_MARCH, "Bremen March", "NA_BGM_BREMEN_MARCH", SEQ_BGM_WORLD, SEQ_CAT_ACTION,
                           nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_BALLAD_OF_THE_WIND_FISH, "Ballad Of The Wind Fish", "NA_BGM_BALLAD_OF_THE_WIND_FISH",
                           SEQ_BGM_WORLD, SEQ_CAT_NONE, nullptr, false, false),
        SEQUENCE_MAP_ENTRY(NA_BGM_SONG_OF_SOARING, "Song Of Soaring", "NA_BGM_SONG_OF_SOARING", SEQ_BGM_SONGS,
                           SEQ_CAT_FAN_GETITEM | SEQ_CAT_FAN_GAMEOVER, nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_MILK_BAR_DUPLICATE, "Milk Bar Night", "NA_BGM_MILK_BAR_DUPLICATE", SEQ_BGM_WORLD,
                           SEQ_CAT_INDOOR, nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_FINAL_HOURS, "Final Hours", "NA_BGM_FINAL_HOURS", SEQ_BGM_WORLD, SEQ_CAT_ACTION,
                           nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_MIKAU_RIFF, "Mikau Riff", "NA_BGM_MIKAU_RIFF", SEQ_BGM_SONGS, SEQ_CAT_NONE, nullptr,
                           true, true), // Looping instrument
        SEQUENCE_MAP_ENTRY(NA_BGM_MIKAU_FINALE, "Mikau Finale", "NA_BGM_MIKAU_FINALE", SEQ_BGM_SONGS, SEQ_CAT_NONE,
                           nullptr, true, true), // Instrument finale
        SEQUENCE_MAP_ENTRY(NA_BGM_FROG_SONG, "Frog Song", "NA_BGM_FROG_SONG", SEQ_BGM_WORLD, SEQ_CAT_NONE, nullptr,
                           false, false), // Looping BGM
        SEQUENCE_MAP_ENTRY(NA_BGM_OCARINA_SONATA, "Sonata Of Awakening (Ocarina)", "NA_BGM_OCARINA_SONATA", SEQ_OCARINA,
                           SEQ_CAT_NONE, nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_OCARINA_LULLABY, "Goron Lullaby", "NA_BGM_OCARINA_LULLABY", SEQ_OCARINA, SEQ_CAT_NONE,
                           nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_OCARINA_NEW_WAVE, "New Wave Bossa Nova (Ocarina)", "NA_BGM_OCARINA_NEW_WAVE",
                           SEQ_OCARINA, SEQ_CAT_NONE, nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_OCARINA_ELEGY, "Elegy Of Emptiness (Ocarina)", "NA_BGM_OCARINA_ELEGY", SEQ_OCARINA,
                           SEQ_CAT_NONE, nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_OCARINA_OATH, "Oath To Order (Ocarina)", "NA_BGM_OCARINA_OATH", SEQ_OCARINA,
                           SEQ_CAT_NONE, nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_MAJORAS_LAIR, "Majora's Lair", "NA_BGM_MAJORAS_LAIR", SEQ_BGM_WORLD, SEQ_CAT_INDOOR,
                           nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_OCARINA_LULLABY_INTRO_PTR, "Lullaby Intro Pointer",
                           "NA_BGM_OCARINA_LULLABY_INTRO_PTR", SEQ_OCARINA, SEQ_CAT_NONE, nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_OCARINA_GUITAR_BASS_SESSION, "Jam Session Bass", "NA_BGM_OCARINA_GUITAR_BASS_SESSION",
                           SEQ_BGM_SONGS, SEQ_CAT_NONE, nullptr, true, true), // Instrument session
        SEQUENCE_MAP_ENTRY(NA_BGM_PIANO_SESSION, "Jam Session Piano", "NA_BGM_PIANO_SESSION", SEQ_BGM_SONGS,
                           SEQ_CAT_NONE, nullptr, true, true), // Instrument session
        SEQUENCE_MAP_ENTRY(NA_BGM_INDIGO_GO_SESSION, "Indigo Go Session (Credits)", "NA_BGM_INDIGO_GO_SESSION",
                           SEQ_BGM_WORLD, SEQ_CAT_NONE, nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_SNOWHEAD_TEMPLE, "Snowhead Temple", "NA_BGM_SNOWHEAD_TEMPLE", SEQ_BGM_WORLD,
                           SEQ_CAT_DUNGEON, nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_GREAT_BAY_TEMPLE, "Great Bay Temple", "NA_BGM_GREAT_BAY_TEMPLE", SEQ_BGM_WORLD,
                           SEQ_CAT_DUNGEON, nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_NEW_WAVE_SAXOPHONE, "New Wave Saxophone", "NA_BGM_NEW_WAVE_SAXOPHONE", SEQ_BGM_SONGS,
                           SEQ_CAT_NONE, nullptr, false, false), // Doesn't play outside the original cutscene
        SEQUENCE_MAP_ENTRY(NA_BGM_NEW_WAVE_VOCAL, "New Wave Vocal", "NA_BGM_NEW_WAVE_VOCAL", SEQ_BGM_SONGS,
                           SEQ_CAT_NONE, nullptr, true, true), // Vocal
        SEQUENCE_MAP_ENTRY(NA_BGM_MAJORAS_WRATH, "Majora's Wrath", "NA_BGM_MAJORAS_WRATH", SEQ_BGM_BATTLE, SEQ_CAT_BOSS,
                           nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_MAJORAS_INCARNATION, "Majora's Incarnation", "NA_BGM_MAJORAS_INCARNATION",
                           SEQ_BGM_BATTLE, SEQ_CAT_BOSS | SEQ_CAT_MINIGAME | SEQ_CAT_ACTION, nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_MAJORAS_MASK, "Majora's Mask", "NA_BGM_MAJORAS_MASK", SEQ_BGM_BATTLE, SEQ_CAT_BOSS,
                           nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_BASS_PLAY, "Bass Play", "NA_BGM_BASS_PLAY", SEQ_BGM_SONGS, SEQ_CAT_NONE, nullptr,
                           true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_DRUMS_PLAY, "Drums Play", "NA_BGM_DRUMS_PLAY", SEQ_BGM_SONGS, SEQ_CAT_NONE, nullptr,
                           true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_PIANO_PLAY, "Piano Play", "NA_BGM_PIANO_PLAY", SEQ_BGM_SONGS, SEQ_CAT_NONE, nullptr,
                           true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_IKANA_CASTLE, "Ikana Castle", "NA_BGM_IKANA_CASTLE", SEQ_BGM_WORLD, SEQ_CAT_DUNGEON,
                           nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_GATHERING_GIANTS, "Gathering Giants", "NA_BGM_GATHERING_GIANTS", SEQ_BGM_WORLD,
                           SEQ_CAT_TITLE, nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_KAMARO_DANCE, "Kamaro Dance", "NA_BGM_KAMARO_DANCE", SEQ_BGM_WORLD, SEQ_CAT_NONE,
                           nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_CREMIA_CARRIAGE, "Cremia Carriage", "NA_BGM_CREMIA_CARRIAGE", SEQ_BGM_WORLD,
                           SEQ_CAT_MINIGAME | SEQ_CAT_INDOOR | SEQ_CAT_CALM, nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_KEATON_QUIZ, "Keaton Quiz", "NA_BGM_KEATON_QUIZ", SEQ_BGM_WORLD, SEQ_CAT_CALM,
                           nullptr, true, true),
        // The credits cutscene uses the position in the sequence to advance. It should not be changed.
        SEQUENCE_MAP_ENTRY(NA_BGM_END_CREDITS, "Credits (First Half)", "NA_BGM_END_CREDITS", SEQ_BGM_WORLD,
                           SEQ_CAT_NONE, nullptr, false, false),
        SEQUENCE_MAP_ENTRY(NA_BGM_OPENING_LOOP, "Opening Loop", "NA_BGM_OPENING_LOOP", SEQ_BGM_WORLD, SEQ_CAT_NONE,
                           nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_TITLE_THEME, "Title Theme", "NA_BGM_TITLE_THEME", SEQ_BGM_WORLD, SEQ_CAT_TITLE,
                           nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_DUNGEON_APPEAR, "Dungeon Appear", "NA_BGM_DUNGEON_APPEAR", SEQ_BGM_EVENT,
                           SEQ_CAT_FAN_GAMEOVER | SEQ_CAT_FAN_GETITEM, nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_WOODFALL_CLEAR, "Woodfall Clear", "NA_BGM_WOODFALL_CLEAR", SEQ_BGM_EVENT,
                           SEQ_CAT_FAN_CLEAR, nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_SNOWHEAD_CLEAR, "Snowhead Clear", "NA_BGM_SNOWHEAD_CLEAR", SEQ_BGM_EVENT,
                           SEQ_CAT_FAN_CLEAR, nullptr, true, true),
        SEQUENCE_MAP_ENTRY(0x7A, "IDK But there is an entry missing", "SEND_HELP", SEQ_NOSHUFFLE, SEQ_CAT_NONE, nullptr,
                           false, false),
        SEQUENCE_MAP_ENTRY(NA_BGM_INTO_THE_MOON, "Enter Moon", "NA_BGM_INTO_THE_MOON", SEQ_BGM_WORLD, SEQ_CAT_ACTION,
                           nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_GOODBYE_GIANT, "Giants Leave", "NA_BGM_GOODBYE_GIANT", SEQ_BGM_EVENT,
                           SEQ_CAT_FAN_CLEAR, nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_TATL_AND_TAEL, "Tatl | Tale", "NA_BGM_TATL_AND_TAEL", SEQ_BGM_WORLD, SEQ_CAT_CALM,
                           nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_MOONS_DESTRUCTION, "Moon's Destruction", "NA_BGM_MOONS_DESTRUCTION", SEQ_BGM_EVENT,
                           SEQ_CAT_FAN_CLEAR, nullptr, true, true),
        SEQUENCE_MAP_ENTRY(NA_BGM_END_CREDITS_SECOND_HALF, "Credits (Second Half)", "NA_BGM_END_CREDITS_SECOND_HALF",
                           SEQ_BGM_WORLD, SEQ_CAT_NONE, nullptr, false, false),
    };

    // Initialize counts for each type
    for (const auto& [seqId, seqInfo] : mSequenceMap) {
        // Don't count sequences that are marked as custom BGM
        if (!(seqInfo.type == SEQ_BGM_CUSTOM)) {
            // Check each type flag individually
            if (seqInfo.type & SEQ_BGM_WORLD)
                mSequenceTypeCounts[SEQ_BGM_WORLD]++;
            if (seqInfo.type & SEQ_BGM_EVENT)
                mSequenceTypeCounts[SEQ_BGM_EVENT]++;
            if (seqInfo.type & SEQ_BGM_BATTLE)
                mSequenceTypeCounts[SEQ_BGM_BATTLE]++;
            if (seqInfo.type & SEQ_OCARINA)
                mSequenceTypeCounts[SEQ_OCARINA]++;
            if (seqInfo.type & SEQ_FANFARE)
                mSequenceTypeCounts[SEQ_FANFARE]++;
            if (seqInfo.type & SEQ_SFX)
                mSequenceTypeCounts[SEQ_SFX]++;
            if (seqInfo.type & SEQ_BGM_SONGS)
                mSequenceTypeCounts[SEQ_BGM_SONGS]++;
            if (seqInfo.type & SEQ_VOICE)
                mSequenceTypeCounts[SEQ_VOICE]++;
        }
    }
}
#define CVAR_AUDIO(var) CVAR_PREFIX_AUDIO "." var
std::string AudioCollection::GetCvarKey(std::string sfxKey) {
    auto prefix = CVAR_AUDIO("ReplacedSequences.");
    return prefix + sfxKey + ".value";
}

std::string AudioCollection::GetCvarLockKey(std::string sfxKey) {
    auto prefix = std::string(CVAR_AUDIO("ReplacedSequences."));
    return prefix + sfxKey + ".locked";
}

void AudioCollection::AddToCollection(char* otrPath, uint16_t seqNum) {
    std::string fileName = std::filesystem::path(otrPath).filename().string();
    size_t underscorePos = fileName.find_last_of('_') + 1;
    SeqType type = SEQ_BGM_CUSTOM;
    // Generic category placement
    int compositeCategory = SEQ_CAT_BGM;
    // More specific placement by sequence ID.
    std::vector<int> seqIdReplacements;
    if (underscorePos != std::string::npos) {
        std::string typeString = fileName.substr(underscorePos);
        std::locale loc;
        for (size_t i = 0; i < typeString.length(); i++) {
            typeString[i] = std::tolower(typeString[i], loc);
        }
        if (typeString == "fanfare") {
            type = SEQ_BGM_CUSTOM_FANFARE;
            // Selects categories 7, 8, and 9
            compositeCategory = SEQ_CAT_FAN;
        } else if (typeString != "bgm") {
            compositeCategory = SEQ_CAT_NONE;
            size_t dashPos;
            std::string category;
            // typeString can be assumed to be dash separated list of categories
            try {
                while ((dashPos = typeString.find('-')) != std::string::npos) {
                    category = typeString.substr(0, dashPos);
                    ParseSequenceCategory(category, compositeCategory, seqIdReplacements);
                    typeString = typeString.substr(dashPos + 1);
                }
                ParseSequenceCategory(typeString, compositeCategory, seqIdReplacements);
            } catch (...) { compositeCategory = SEQ_CAT_BGM; }
            // If any fanfare categories are used, make this a fanfare sequence
            if (compositeCategory & SEQ_CAT_FAN) {
                type = SEQ_BGM_CUSTOM_FANFARE;
            }
        }
    }
    std::string sequenceName = fileName.substr(0, underscorePos - 1);
    std::shared_ptr<std::vector<int>> sirPtr = nullptr;
    if (!seqIdReplacements.empty()) {
        // Catch fanfares that lack generic categories
        if (compositeCategory == SEQ_CAT_NONE) {
            for (int& seqId : seqIdReplacements) {
                if (mSequenceMap[seqId].type & SEQ_BGM_CUSTOM_FANFARE) {
                    type = SEQ_BGM_CUSTOM_FANFARE;
                    break;
                }
            }
        }
        sirPtr = std::make_shared<std::vector<int>>(seqIdReplacements);
    }
    SequenceInfo info = { seqNum,
                          sequenceName,
                          StringHelper::Replace(
                              StringHelper::Replace(StringHelper::Replace(sequenceName, " ", "_"), "~", "-"), ".", ""),
                          type,
                          compositeCategory,
                          sirPtr,
                          false,
                          true };
    mSequenceMap.emplace(seqNum, info);
}

uint16_t AudioCollection::GetReplacementSequence(uint16_t seqId) {
    if (mSequenceMap.find(seqId) == mSequenceMap.end()) {
        return seqId;
    }

    const auto& sequenceInfo = mSequenceMap.at(seqId);
    const std::string cvarKey = GetCvarKey(sequenceInfo.sfxKey);
    int replacementSeq = CVarGetInteger(cvarKey.c_str(), seqId);
    if (!mSequenceMap.contains(replacementSeq)) {
        replacementSeq = seqId;
    }
    return static_cast<uint16_t>(replacementSeq);
}

// For custom sequences, we need to get the original sequence ID for sequence flag lookups
uint16_t AudioCollection::GetOriginalSequence(uint16_t seqId) {
    // BENTODO there is probably a better way to do this.
    // There are 127 original sequences. If the ID is less than that we don't need to do
    // any lookups
    if (seqId <= GetMaxOriginalSeqId()) {
        return seqId;
    }

    for (const auto& a : mSequenceMap) {
        const std::string cvarKey = GetCvarKey(a.second.sfxKey);
        int replacementSeq = CVarGetInteger(cvarKey.c_str(), NA_BGM_DISABLED);
        if (replacementSeq == seqId) {
            return a.first;
        }
    }
    return 0;
}

void AudioCollection::RemoveFromShufflePool(SequenceInfo* seqInfo) {
    const std::string cvarKey = std::string(CVAR_AUDIO("Excluded.")) + seqInfo->sfxKey;
    excludedSequences.insert(seqInfo);
    includedSequences.erase(seqInfo);
    CVarSetInteger(cvarKey.c_str(), 1);
    Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
}

void AudioCollection::AddToShufflePool(SequenceInfo* seqInfo) {
    const std::string cvarKey = std::string(CVAR_AUDIO("Excluded.")) + seqInfo->sfxKey;
    includedSequences.insert(seqInfo);
    excludedSequences.erase(seqInfo);
    CVarClear(cvarKey.c_str());
    Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
}

void AudioCollection::InitializeShufflePool() {
    if (shufflePoolInitialized)
        return;

    for (auto& [seqId, seqInfo] : mSequenceMap) {
        if (!seqInfo.canBeUsedAsReplacement)
            continue;
        const std::string cvarKey = std::string(CVAR_AUDIO("Excluded.")) + seqInfo.sfxKey;
        if (CVarGetInteger(cvarKey.c_str(), 0)) {
            excludedSequences.insert(&seqInfo);
        } else {
            includedSequences.insert(&seqInfo);
        }
    }

    shufflePoolInitialized = true;
};

extern "C" void AudioCollection_AddToCollection(char* otrPath, uint16_t seqNum) {
    AudioCollection::Instance->AddToCollection(otrPath, seqNum);
}

bool AudioCollection::HasSequenceNum(uint16_t seqId) {
    return mSequenceMap.contains(seqId);
}

const char* AudioCollection::GetSequenceName(uint16_t seqId) {
    auto seqIt = mSequenceMap.find(seqId);
    if (seqIt != mSequenceMap.end()) {
        return seqIt->second.label.c_str();
    }
    return nullptr;
}

size_t AudioCollection::SequenceMapSize() {
    return mSequenceMap.size();
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

size_t AudioCollection::CountSequencesByType(SeqType type) {
    auto it = mSequenceTypeCounts.find(type);
    return it != mSequenceTypeCounts.end() ? it->second : 0;
}

uint16_t AudioCollection::GetMaxOriginalSeqId() const {
    return 0x7F;
    // uint16_t maxId = 0;
    // for (const auto& [seqId, seqInfo] : mSequenceMap) {
    //     if (!(seqInfo.type & SEQ_BGM_CUSTOM) && seqId > maxId) {
    //         maxId = seqId;
    //     }
    // }
    // return maxId;
}

void AudioCollection::ParseSequenceCategory(std::string token, int& compositeCategory, std::vector<int>& seqIds) {
    // Categories 10 -> 16 are written as decimal numbers. Everything else can be read as hexidecimal.
    int numCategory = std::stoi(token, 0, token.length() == 2 ? 10 : 16);
    // Quietly omits categories in the range [0x11, 0xff]
    if (numCategory >= SEQUENCE_ID_REPLACEMENT_OFFSET) {
        int seqId = numCategory - SEQUENCE_ID_REPLACEMENT_OFFSET;
        // Sequences can't replace custom sequences
        if (seqId <= GetMaxOriginalSeqId()) {
            seqIds.push_back(seqId);
        }
    } else if (numCategory <= 16) {
        compositeCategory |= 1 << numCategory;
    }
}