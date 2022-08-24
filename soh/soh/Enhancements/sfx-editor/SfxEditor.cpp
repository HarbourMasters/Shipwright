#include "SfxEditor.h"
#include "sequence.h"
#include <string>
#include <map>
#include <time.h>
#include <stdlib.h>
#include <Cvar.h>
#include "../libultraship/ImGuiImpl.h"
#include <functions.h>
extern "C" {
extern GlobalContext* gGlobalCtx;
}

const char* randoSFXLowHP[23] = { "Bark",
                                  "Bomb Bounce",
                                  "Bongo Bongo Low",
                                  "Business Scrub",
                                  "Carrot Refill",
                                  "Cluck",
                                  "Drawbridge Set",
                                  "Guay",
                                  "Low HP Beep",
                                  "HP Recover",
                                  "Horse Trot",
                                  "Iron Boots",
                                  "Moo",
                                  "Mweep!",
                                  "Navi Hey!",
                                  "Navi Listen!",
                                  "Pot Shattering",
                                  "Ribbit",
                                  "Rupee (Silver)",
                                  "Switch",
                                  "Sword Bonk",
                                  "Tambourine",
                                  "Zelda Gasp (Adult)" };

const char* randoSFXNightfall[9] = { "Dusk Howl", "Cockadoodiedoo", "Gold Skulltula Token", "Great Fairy", "Moo",
                                     "Mweep!", "Redead Moan", "Talon Snore", "Thunder" };

const char* randoSFXHorse[8] = { "Armos",  "Child Scream",  "Great Fairy", "Moo",
                                 "Mweep!", "Redead Scream", "Ruto Giggle", "Stalchild Attack" };

const char* randoSFXHoverBoots[6] = {
    "Bark", "Cartoon Fall", "Flare Dancer Laugh", "Mweep!", "Shabom Pop", "Tambourine"
};

const std::map<u16, std::tuple<std::string, std::string, SeqType>> sequenceMap = {
    { NA_BGM_FIELD_LOGIC, { "Hyrule Field", "NA_BGM_FIELD_LOGIC", SEQ_BGM_WORLD } },
    { NA_BGM_DUNGEON, { "Dodongo's Cavern", "NA_BGM_DUNGEON", SEQ_BGM_WORLD } },
    { NA_BGM_KAKARIKO_ADULT, { "Kakariko Village (Adult)", "NA_BGM_KAKARIKO_ADULT", SEQ_BGM_WORLD } },
    { NA_BGM_ENEMY, { "Battle", "NA_BGM_ENEMY", SEQ_NOSHUFFLE } },
    { NA_BGM_BOSS, { "Boss Battle", "NA_BGM_BOSS", SEQ_BGM_WORLD } },
    { NA_BGM_INSIDE_DEKU_TREE, { "Inside the Deku Tree", "NA_BGM_INSIDE_DEKU_TREE", SEQ_BGM_WORLD } },
    { NA_BGM_MARKET, { "Market", "NA_BGM_MARKET", SEQ_BGM_WORLD } },
    { NA_BGM_TITLE, { "Title Theme", "NA_BGM_TITLE", SEQ_BGM_WORLD } },
    { NA_BGM_LINK_HOUSE, { "House", "NA_BGM_LINK_HOUSE", SEQ_BGM_WORLD } },
    { NA_BGM_GAME_OVER, { "Game Over", "NA_BGM_GAME_OVER", SEQ_FANFARE } },
    { NA_BGM_BOSS_CLEAR, { "Boss Clear", "NA_BGM_BOSS_CLEAR", SEQ_FANFARE } },
    { NA_BGM_ITEM_GET, { "Obtain Item", "NA_BGM_ITEM_GET", SEQ_FANFARE } },
    { NA_BGM_OPENING_GANON, { "Enter Ganondorf", "NA_BGM_OPENING_GANON", SEQ_FANFARE } },
    { NA_BGM_HEART_GET, { "Obtain Heart Container", "NA_BGM_HEART_GET", SEQ_FANFARE } },
    { NA_BGM_OCA_LIGHT, { "Prelude of Light", "NA_BGM_OCA_LIGHT", SEQ_FANFARE } },
    { NA_BGM_JABU_JABU, { "Inside Jabu-Jabu's Belly", "NA_BGM_JABU_JABU", SEQ_BGM_WORLD } },
    { NA_BGM_KAKARIKO_KID, { "Kakariko Village (Child)", "NA_BGM_KAKARIKO_KID", SEQ_BGM_WORLD } },
    { NA_BGM_GREAT_FAIRY, { "Great Fairy's Fountain", "NA_BGM_GREAT_FAIRY", SEQ_BGM_WORLD } },
    { NA_BGM_ZELDA_THEME, { "Zelda's Theme", "NA_BGM_ZELDA_THEME", SEQ_BGM_WORLD } },
    { NA_BGM_FIRE_TEMPLE, { "Fire Temple", "NA_BGM_FIRE_TEMPLE", SEQ_BGM_WORLD } },
    { NA_BGM_OPEN_TRE_BOX, { "Open Treasure Chest", "NA_BGM_OPEN_TRE_BOX", SEQ_FANFARE } },
    { NA_BGM_FOREST_TEMPLE, { "Forest Temple", "NA_BGM_FOREST_TEMPLE", SEQ_BGM_WORLD } },
    { NA_BGM_COURTYARD, { "Hyrule Castle Courtyard", "NA_BGM_COURTYARD", SEQ_BGM_WORLD } },
    { NA_BGM_GANON_TOWER, { "Ganondorf's Theme", "NA_BGM_GANON_TOWER", SEQ_BGM_WORLD } },
    { NA_BGM_LONLON, { "Lon Lon Ranch", "NA_BGM_LONLON", SEQ_BGM_WORLD } },
    { NA_BGM_GORON_CITY, { "Goron City", "NA_BGM_GORON_CITY", SEQ_BGM_WORLD } },
    { NA_BGM_SPIRITUAL_STONE, { "Spiritual Stone Get", "NA_BGM_SPIRITUAL_STONE", SEQ_FANFARE } },
    { NA_BGM_OCA_BOLERO, { "Bolero of Fire", "NA_BGM_OCA_BOLERO", SEQ_FANFARE } },
    { NA_BGM_OCA_MINUET, { "Minuet of Forest", "NA_BGM_OCA_MINUET", SEQ_FANFARE } },
    { NA_BGM_OCA_SERENADE, { "Serenade of Water", "NA_BGM_OCA_SERENADE", SEQ_FANFARE } },
    { NA_BGM_OCA_REQUIEM, { "Requiem of Spirit", "NA_BGM_OCA_REQUIEM", SEQ_FANFARE } },
    { NA_BGM_OCA_NOCTURNE, { "Nocturne of Shadow", "NA_BGM_OCA_NOCTURNE", SEQ_FANFARE } },
    { NA_BGM_MINI_BOSS, { "Mini-Boss Battle", "NA_BGM_MINI_BOSS", SEQ_BGM_WORLD } },
    { NA_BGM_SMALL_ITEM_GET, { "Obtain Small Item", "NA_BGM_SMALL_ITEM_GET", SEQ_FANFARE } },
    { NA_BGM_TEMPLE_OF_TIME, { "Temple of Time", "NA_BGM_TEMPLE_OF_TIME", SEQ_BGM_WORLD } },
    { NA_BGM_EVENT_CLEAR, { "Escape from Lon Lon Ranch", "NA_BGM_EVENT_CLEAR", SEQ_FANFARE } },
    { NA_BGM_KOKIRI, { "Kokiri Forest", "NA_BGM_KOKIRI", SEQ_BGM_WORLD } },
    { NA_BGM_OCA_FAIRY_GET, { "Obtain Fairy Ocarina", "NA_BGM_OCA_FAIRY_GET", SEQ_FANFARE } },
    { NA_BGM_SARIA_THEME, { "Lost Woods", "NA_BGM_SARIA_THEME", SEQ_BGM_WORLD } },
    { NA_BGM_SPIRIT_TEMPLE, { "Spirit Temple", "NA_BGM_SPIRIT_TEMPLE", SEQ_BGM_WORLD } },
    { NA_BGM_HORSE, { "Horse Race", "NA_BGM_HORSE", SEQ_BGM_WORLD } },
    { NA_BGM_HORSE_GOAL, { "Horse Race Goal", "NA_BGM_HORSE_GOAL", SEQ_FANFARE } },
    { NA_BGM_INGO, { "Ingo's Theme", "NA_BGM_INGO", SEQ_BGM_WORLD } },
    { NA_BGM_MEDALLION_GET, { "Obtain Medallion", "NA_BGM_MEDALLION_GET", SEQ_FANFARE } },
    { NA_BGM_OCA_SARIA, { "Ocarina Saria's Song", "NA_BGM_OCA_SARIA", SEQ_FANFARE } },
    { NA_BGM_OCA_EPONA, { "Ocarina Epona's Song", "NA_BGM_OCA_EPONA", SEQ_FANFARE } },
    { NA_BGM_OCA_ZELDA, { "Ocarina Zelda's Lullaby", "NA_BGM_OCA_ZELDA", SEQ_FANFARE } },
    { NA_BGM_OCA_SUNS, { "Ocarina Sun's Song", "NA_BGM_OCA_SUNS", SEQ_FANFARE } },
    { NA_BGM_OCA_TIME, { "Ocarina Song of Time", "NA_BGM_OCA_TIME", SEQ_FANFARE } },
    { NA_BGM_OCA_STORM, { "Ocarina Song of Storms", "NA_BGM_OCA_STORM", SEQ_FANFARE } },
    { NA_BGM_NAVI_OPENING, { "Fairy Flying", "NA_BGM_NAVI_OPENING", SEQ_BGM_WORLD } },
    { NA_BGM_DEKU_TREE_CS, { "Deku Tree", "NA_BGM_DEKU_TREE_CS", SEQ_BGM_WORLD } },
    { NA_BGM_WINDMILL, { "Windmill Hut", "NA_BGM_WINDMILL", SEQ_BGM_WORLD } },
    { NA_BGM_HYRULE_CS, { "Legend of Hyrule", "NA_BGM_HYRULE_CS", SEQ_NOSHUFFLE } },
    { NA_BGM_MINI_GAME, { "Shooting Gallery", "NA_BGM_MINI_GAME", SEQ_BGM_WORLD } },
    { NA_BGM_SHEIK, { "Sheik's Theme", "NA_BGM_SHEIK", SEQ_BGM_WORLD } },
    { NA_BGM_ZORA_DOMAIN, { "Zora's Domain", "NA_BGM_ZORA_DOMAIN", SEQ_BGM_WORLD } },
    { NA_BGM_APPEAR, { "Enter Zelda", "NA_BGM_APPEAR", SEQ_FANFARE } },
    { NA_BGM_ADULT_LINK, { "Goodbye to Zelda", "NA_BGM_ADULT_LINK", SEQ_BGM_WORLD } },
    { NA_BGM_MASTER_SWORD, { "Master Sword", "NA_BGM_MASTER_SWORD", SEQ_FANFARE } },
    { NA_BGM_INTRO_GANON, { "Ganon Intro", "NA_BGM_INTRO_GANON", SEQ_BGM_WORLD } },
    { NA_BGM_SHOP, { "Shop", "NA_BGM_SHOP", SEQ_BGM_WORLD } },
    { NA_BGM_CHAMBER_OF_SAGES, { "Chamber of the Sages", "NA_BGM_CHAMBER_OF_SAGES", SEQ_BGM_WORLD } },
    { NA_BGM_FILE_SELECT, { "File Select", "NA_BGM_FILE_SELECT", SEQ_NOSHUFFLE } },
    { NA_BGM_ICE_CAVERN, { "Ice Cavern", "NA_BGM_ICE_CAVERN", SEQ_BGM_WORLD } },
    { NA_BGM_DOOR_OF_TIME, { "Open Door of Temple of Time", "NA_BGM_DOOR_OF_TIME", SEQ_FANFARE } },
    { NA_BGM_OWL, { "Kaepora Gaebora's Theme", "NA_BGM_OWL", SEQ_BGM_WORLD } },
    { NA_BGM_SHADOW_TEMPLE, { "Shadow Temple", "NA_BGM_SHADOW_TEMPLE", SEQ_BGM_WORLD } },
    { NA_BGM_WATER_TEMPLE, { "Water Temple", "NA_BGM_WATER_TEMPLE", SEQ_BGM_WORLD } },
    { NA_BGM_BRIDGE_TO_GANONS, { "Ganon's Castle Bridge", "NA_BGM_BRIDGE_TO_GANONS", SEQ_FANFARE } },
    { NA_BGM_OCARINA_OF_TIME, { "Ocarina of Time", "NA_BGM_OCARINA_OF_TIME", SEQ_NOSHUFFLE } },
    { NA_BGM_GERUDO_VALLEY, { "Gerudo Valley", "NA_BGM_GERUDO_VALLEY", SEQ_BGM_WORLD } },
    { NA_BGM_POTION_SHOP, { "Potion Shop", "NA_BGM_POTION_SHOP", SEQ_BGM_WORLD } },
    { NA_BGM_KOTAKE_KOUME, { "Kotake & Koume's Theme", "NA_BGM_KOTAKE_KOUME", SEQ_BGM_WORLD } },
    { NA_BGM_ESCAPE, { "Escape from Ganon's Castle", "NA_BGM_ESCAPE", SEQ_BGM_WORLD } },
    { NA_BGM_UNDERGROUND, { "Ganon's Castle Under Ground", "NA_BGM_UNDERGROUND", SEQ_BGM_WORLD } },
    { NA_BGM_GANONDORF_BOSS, { "Ganondorf Battle", "NA_BGM_GANONDORF_BOSS", SEQ_BGM_WORLD } },
    { NA_BGM_GANON_BOSS, { "Ganon Battle", "NA_BGM_GANON_BOSS", SEQ_BGM_WORLD } },
    { NA_BGM_END_DEMO, { "Seal of Six Sages", "NA_BGM_END_DEMO", SEQ_NOSHUFFLE } },
    { NA_BGM_STAFF_1, { "End Credits I", "NA_BGM_STAFF_1", SEQ_NOSHUFFLE } },
    { NA_BGM_STAFF_2, { "End Credits II", "NA_BGM_STAFF_2", SEQ_NOSHUFFLE } },
    { NA_BGM_STAFF_3, { "End Credits III", "NA_BGM_STAFF_3", SEQ_NOSHUFFLE } },
    { NA_BGM_STAFF_4, { "End Credits IV", "NA_BGM_STAFF_4", SEQ_NOSHUFFLE } },
    { NA_BGM_FIRE_BOSS, { "King Dodongo & Volvagia Boss Battle", "NA_BGM_FIRE_BOSS", SEQ_BGM_WORLD } },
    { NA_BGM_TIMED_MINI_GAME, { "Mini-Game", "NA_BGM_TIMED_MINI_GAME", SEQ_BGM_ERROR } },
    { NA_SE_EV_SMALL_DOG_BARK, { "Bark", "0x28D8", SFX_SOUND } },
    { NA_SE_EN_AWA_BOUND, { "Bomb Bounce", "0x3948", SFX_SOUND } },
    { NA_SE_EN_SHADEST_TAIKO_LOW, { "Bongo Bongo Low", "0x3950", SFX_SOUND } },
    { NA_SE_EN_NUTS_FAINT, { "Business Scrub", "0x3882", SFX_SOUND } },
    { NA_SE_SY_CARROT_RECOVER, { "Carrot Refill", "0x4845", SFX_SOUND } },
    { NA_SE_EV_CHICKEN_CRY_N, { "Cluck", "0x2811", SFX_SOUND } },
    { NA_SE_EV_BRIDGE_OPEN_STOP, { "Drawbridge Set", "0x280E", SFX_SOUND } },
    { NA_SE_EN_KAICHO_CRY, { "Guay", "0x38B6", SFX_SOUND } },
    { NA_SE_SY_HITPOINT_ALARM, { "Low HP Beep", "0x481B", SFX_SOUND } },
    { NA_SE_SY_HP_RECOVER, { "HP Recover", "0x480B", SFX_SOUND } },
    { NA_SE_EV_HORSE_RUN, { "Horse Trot", "0x2804", SFX_SOUND } },
    { NA_SE_PL_WALK_HEAVYBOOTS, { "Iron Boots", "0x80D", SFX_SOUND } },
    { NA_SE_EV_COW_CRY, { "Moo", "0x28DF", SFX_SOUND } },
    { NA_SE_VO_KZ_MOVE, { "Mweep!", "0x687A", SFX_SOUND } },
    { NA_SE_VO_NA_HELLO_2, { "Navi Hey!", "0x685F", SFX_SOUND } },
    { NA_SE_VO_SK_LAUGH, { "Navi Listen!", "0x6873", SFX_SOUND } },
    { NA_SE_EV_POT_BROKEN, { "Pot Shattering", "0x2887", SFX_SOUND } },
    { NA_SE_EV_FROG_CRY_0, { "Ribbit", "0x28E3", SFX_SOUND } },
    { NA_SE_EV_FIVE_COUNT_LUPY, { "Rupee (Silver)", "0x28E8", SFX_SOUND } },
    { NA_SE_EV_FOOT_SWITCH, { "Switch", "0x2815", SFX_SOUND } },
    { NA_SE_IT_WALL_HIT_SOFT, { "Sword Bonk", "0x181", SFX_SOUND } },
    { NA_SE_SY_METRONOME, { "Tambourine", "0x483", SFX_SOUND } },
    { NA_SE_VO_Z1_SURPRISE, { "Zelda Gasp (Adult)", "0x6878", SFX_SOUND } },
    { NA_SE_EN_AMOS_VOICE, { "Armos", "0x3848", SFX_SOUND } },
    { NA_SE_VO_LI_FALL_L_KID, { "Child Scream", "0x6828", SFX_SOUND } },
    { NA_SE_VO_FR_LAUGH_0, { "Great Fairy", "0x6858", SFX_SOUND } },
    { NA_SE_EN_REDEAD_AIM, { "Redead Scream", "0x38E5", SFX_SOUND } },
    { NA_SE_VO_RT_LAUGH_0, { "Ruto Giggle", "0x6863", SFX_SOUND } },
    { NA_SE_EN_STALKID_ATTACK, { "Stalchild Attack", "0x3831", SFX_SOUND } },
    { NA_SE_EV_CHICKEN_CRY_M, { "Cockadoodiedoo", "0x2813", SFX_SOUND } },
    { NA_SE_SY_KINSTA_MARK_APPEAR, { "Gold Skulltula Token", "0x4843", SFX_SOUND } },
    { NA_SE_EN_REDEAD_CRY, { "Redead Moan", "0x38E4", SFX_SOUND } },
    { NA_SE_VO_TA_SLEEP, { "Talon Snore", "0x6850", SFX_SOUND } },
    { NA_SE_EV_LIGHTNING, { "Thunder", "0x282E", SFX_SOUND } },
    { NA_SE_OC_ABYSS, { "Cartoon Fall", "0x5801", SFX_SOUND } },
    { NA_SE_EN_FLAME_LAUGH, { "Flare Dancer Laugh", "0x3981", SFX_SOUND } },
    { NA_SE_EN_AWA_BREAK, { "Shabom Pop", "0x3949", SFX_SOUND } },
    { NA_SE_EN_SHADEST_TAIKO_HIGH, { "Bongo Bongo High", "0x3951", SFX_SOUND } },
    { NA_SE_EV_BOTTLE_CAP_OPEN, { "Bottle Cork", "0x286C", SFX_SOUND } },
    { NA_SE_IT_BOW_FLICK, { "Bow Twang", "0x1830", SFX_SOUND } },
    { NA_SE_EN_BUBLE_LAUGH, { "Bubble Laugh", "0x38CA", SFX_SOUND } },
    { NA_SE_VO_LI_BREATH_DRINK_KID, { "Child Pant", "0x682A", SFX_SOUND } },
    { NA_SE_EN_DEKU_JR_MOUTH, { "Deku Baba", "0x3860", SFX_SOUND } },
    { NA_SE_EV_DOG_CRY_EVENING, { "Dusk Howl", "0x28AE", SFX_SOUND } },
    { NA_SE_EN_FLAME_DAMAGE, { "Flare Dancer Startled", "0x397A", SFX_SOUND } },
    { NA_SE_EN_GANON_AT_RETURN, { "Ganondorf Teh!", "0x39CA", SFX_SOUND } },
    { NA_SE_EN_GOMA_JR_CRY, { "Gohma Larva Croak", "0x395D", SFX_SOUND } },
    { NA_SE_EN_GOLON_WAKE_UP, { "Goron Wake", "0x38FC", SFX_SOUND } },
    { NA_SE_SY_START_SHOT, { "Gunshot", "0x4835", SFX_SOUND } },
    { NA_SE_IT_HAMMER_HIT, { "Hammer Bonk", "0x180A", SFX_SOUND } },
    { NA_SE_EN_IRONNACK_SWING_AXE, { "Iron Knuckle", "0x3929", SFX_SOUND } },
    { NA_SE_EN_FANTOM_ST_LAUGH, { "Phantom Ganon Laugh", "0x39D6", SFX_SOUND } },
    { NA_SE_EV_PLANT_BROKEN, { "Plant Explode", "0x284E", SFX_SOUND } },
    { NA_SE_SY_GET_RUPY, { "Rupee", "0x4803", SFX_SOUND } }, 
    { NA_SE_VO_RT_CRASH, { "Ruto Crash", "0x6860", SFX_SOUND } },
    { NA_SE_VO_RT_LIFT, { "Ruto Lift", "0x6864", SFX_SOUND } },
    { NA_SE_VO_RT_THROW, { "Ruto Thrown", "0x6865", SFX_SOUND } },
    { NA_SE_EN_NUTS_UP, { "Scrub Emerge", "0x387C", SFX_SOUND } },
    { NA_SE_EN_AWA_BOUND, { "Shabom Bounce", "0x3948", SFX_SOUND } },
    { NA_SE_EN_SHELL_MOUTH, { "Shellblade", "0x3849", SFX_SOUND } },
    { NA_SE_EN_STALTU_DAMAGE, { "Skulltula Damage", "0x386B", SFX_SOUND } },
    { NA_SE_EN_NUTS_THROW, { "Spit Nut", "0x387E", SFX_SOUND } },
    { NA_SE_VO_TA_CRY_0, { "Talon Hmm", "0x6852", SFX_SOUND } },
    { NA_SE_VO_TA_SURPRISE, { "Talon Surprised", "0x6851", SFX_SOUND } },
    { NA_SE_SY_LOCK_ON, { "Target Enemy", "0x4830", SFX_SOUND } },
    { NA_SE_SY_LOCK_ON_HUMAN, { "Target Neutral", "0x4810", SFX_SOUND } }
};

const std::map<u16, const std::string> ocarinaSFXMap = { { 1, "Ocarina" }, { 2, "Voice" },     { 3, "Whistle" },
                                                         { 4, "Harp" },    { 5, "MusicBox"}, { 6, "Flute" } };

void Draw_BgmTab(const std::string& tabKey, const std::map<u16, std::tuple<std::string, std::string, SeqType>>& map,
                 SeqType type) {
    srand(time(NULL));
    if (ImGui::Button("Reset All")) {
        for (const auto& [defaultValue, nameAndStorageKeySuffix] : map) {
            const auto& [name, storageKeySuffix, seqType] = nameAndStorageKeySuffix;
            if (seqType == type) {
                const std::string storageKey = "gSfxEditor_" + storageKeySuffix;
                CVar_SetS32(storageKey.c_str(), defaultValue);
            }
        }
        SohImGui::needs_save = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("Randomize All")) {
        std::vector<u16> values;
        for (const auto& [value, nameAndStorageKeySuffix] : map) {
            const auto& [name, storageKeySuffix, seqType] = nameAndStorageKeySuffix;

            if (seqType == type) {
                values.push_back(value);
            }
        }
        for (const auto& [defaultValue, nameAndStorageKeySuffix] : map) {
            const auto& [name, storageKeySuffix, seqType] = nameAndStorageKeySuffix;
            const std::string storageKey = "gSfxEditor_" + storageKeySuffix;
            if (seqType == type) {
                const int randomIndex = rand() % values.size();
                const int randomValue = values[randomIndex];
                values.erase(values.begin() + randomIndex);
                CVar_SetS32(storageKey.c_str(), randomValue);
            }
        }
        SohImGui::needs_save = true;
    }
    if (type == SFX_SOUND) {
        for (int i = 0; i < 4; i++) {
            char sound[35] = "";
            int max = 0;
            
            if (i == 0) {
                strcat(sound, "Randomize Sound: Low HP Beep");
                max = 22;   
            }
            if (i == 1) {
                strcat(sound, "Randomize Sound: Nightfall Howl");
                max = 8;
            }
            if (i == 2) {
                strcat(sound, "Randomize Sound: Horse Trot");
                max = 7;
            }
            if (i == 3) {
                strcat(sound, "Randomize Sound: Hover Boots");
                max = 5;
            } 
            if (ImGui::Button(sound)) {
                std::vector<u16> values;
                for (const auto& [value, nameAndStorageKeySuffix] : map) {
                    const auto& [name, storageKeySuffix, seqType] = nameAndStorageKeySuffix;
                    for (int j = 0; j < max; j++) {
                        if (i == 0) {
                            if (name == randoSFXLowHP[j]) {
                                values.push_back(value);
                            }
                        } else if (i == 1) {
                            if (name == randoSFXNightfall[j]) {
                                values.push_back(value);
                            }
                        } else if (i == 2) {
                            if (name == randoSFXHorse[j]) {
                                values.push_back(value);
                            }
                        } else {
                            if (name == randoSFXHoverBoots[j]) {
                                values.push_back(value);
                            }
                        }
                    }
                }
                const int randomIndex = rand() % values.size();
                const int randomValue = values[randomIndex];
                if (i == 0) {
                    CVar_SetS32("gSfxEditor_0x481B", randomValue);
                } else if (i == 1) {
                    CVar_SetS32("gSfxEditor_0x28AE", randomValue);
                } else if (i == 2) {
                    CVar_SetS32("gSfxEditor_0x2804", randomValue);
                } else {
                    CVar_SetS32("gSfxEditor_0x8C9", randomValue);
                }
                SohImGui::needs_save = true; 
            }
        }
    }

    ImGui::BeginTable(tabKey.c_str(), 3, ImGuiTableFlags_SizingFixedFit);
    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 110.0f);
    for (const auto& [defaultValue, nameAndStorageKeySuffix] : map) {
        const auto& [name, storageKeySuffix, seqType] = nameAndStorageKeySuffix;

        if (seqType != type) {
            continue;
        }

        const std::string storageKey = "gSfxEditor_" + storageKeySuffix;
        const std::string hiddenKey = "##" + storageKey;
        const std::string stopButton = " Stop  " + hiddenKey;
        const std::string previewButton = "Preview" + hiddenKey;
        const int currentValue = CVar_GetS32(storageKey.c_str(), defaultValue);

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text(name.c_str());
        ImGui::TableNextColumn();
        ImGui::PushItemWidth(-FLT_MIN);
        if (ImGui::BeginCombo(hiddenKey.c_str(), std::get<0>(map.at(static_cast<u16>(currentValue))).c_str())) {
            for (const auto& [value, nameAndStorageKeySuffix] : map) {
                const auto& [name, storageKeySuffix, seqType] = nameAndStorageKeySuffix;
                if (seqType != type) {
                    continue;
                }

                if (ImGui::Selectable(std::get<0>(nameAndStorageKeySuffix).c_str())) {
                    CVar_SetS32(storageKey.c_str(), static_cast<int>(value));
                    SohImGui::needs_save = true;
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
                SohImGui::needs_save = true;
            }
        } else {
            if (ImGui::Button(previewButton.c_str())) {
                if (type == SFX_SOUND) {
                    Player* player = (Player*)(gGlobalCtx)->actorCtx.actorLists[ACTORCAT_PLAYER].head;
                    Audio_PlayActorSound2(&player->actor, currentValue);
                } else {
                    if (CVar_GetS32("gSfxEditor_playing", 0) != 0) {
                        func_800F5C2C();
                        CVar_SetS32("gSfxEditor_playing", 0);
                        SohImGui::needs_save = true;
                    } else {
                        // TODO: Cant do both here, so have to click preview button twice
                        func_800F5ACC(currentValue);
                        CVar_SetS32("gSfxEditor_playing", currentValue);
                        SohImGui::needs_save = true;
                    }
                }
            }
        }
        ImGui::SameLine();
        ImGui::PushItemWidth(-FLT_MIN);
        if (ImGui::Button("Reset")) {
            CVar_SetS32(storageKey.c_str(), defaultValue);
            SohImGui::needs_save = true;
        }
    }
    ImGui::EndTable();
}

void Draw_SfxTab(const std::string& tabKey, const std::map<u16, const std::string>& map) {
    srand(time(NULL));
    if (ImGui::Button("Reset All")) {
        for (const auto& [value, name] : map) {
            const std::string storageKey = "g" + name + "SFX";    
            CVar_SetS32(storageKey.c_str(), value);
        }
        SohImGui::needs_save = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("Randomize All")) {
        std::vector<u16> values;
        for (const auto& [value, name] : map) {

        values.push_back(value);
        }
        for (const auto& [value, name] : map) {
            const std::string storageKey = "g" + name + "SFX";           
            const int randomIndex = rand() % values.size();
            const int randomValue = values[randomIndex];
            values.erase(values.begin() + randomIndex);
            CVar_SetS32(storageKey.c_str(), randomValue);
        }
        SohImGui::needs_save = true;
    }

    ImGui::BeginTable(tabKey.c_str(), 3, ImGuiTableFlags_SizingFixedFit);
    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 110.0f);
    for (const auto& [value, name] : map) {

        const std::string storageKey = "g" + name + "SFX";
        const std::string hiddenKey = "##" + storageKey;
        const std::string stopButton = " Stop  " + hiddenKey;
        const std::string previewButton = "Preview" + hiddenKey;
        const int currentValue = CVar_GetS32(storageKey.c_str(), value);

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text(name.c_str());
        ImGui::TableNextColumn();
        ImGui::PushItemWidth(-FLT_MIN);
        if (ImGui::BeginCombo(hiddenKey.c_str(), (map.at(static_cast<u16>(currentValue))).c_str())) {
            for (const auto& [value, name] : map) {

                if (ImGui::Selectable(name.c_str())) {
                    CVar_SetS32(storageKey.c_str(), static_cast<int>(value));
                    SohImGui::needs_save = true;
                }
            }

            ImGui::EndCombo();
        }
        ImGui::TableNextColumn();
        ImGui::PushItemWidth(-FLT_MIN);
        if (ImGui::Button("Reset")) {
            CVar_SetS32(storageKey.c_str(), value);
            SohImGui::needs_save = true;
        }
    }
    ImGui::EndTable();
}

extern "C" u16 getReplacementSeq(u16 seqId) {
    if (sequenceMap.find(seqId) == sequenceMap.end()) {
        return seqId;
    }

    const auto& [name, storageKeySuffix, seqType] = sequenceMap.at(seqId);
        const std::string storageKey = "gSfxEditor_" + storageKeySuffix;
        const int replacementSeq = CVar_GetS32(storageKey.c_str(), seqId);

        return static_cast<u16>(replacementSeq);
}

void DrawSfxEditor(bool& open) {
    if (!open) {
        CVar_SetS32("gSfxEditor", 0);
        return;
    }
    ImGui::SetNextWindowSize(ImVec2(465, 430), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("SFX Editor", &open)) {
        ImGui::End();
        return;
    }

    if (ImGui::BeginTabBar("SfxContextTabBar", ImGuiTabBarFlags_NoCloseWithMiddleMouseButton)) {
        if (ImGui::BeginTabItem("Background Music")) {
            Draw_BgmTab("backgroundMusic", sequenceMap, SEQ_BGM_WORLD);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Fanfares")) {
            Draw_BgmTab("fanfares", sequenceMap, SEQ_FANFARE);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("SFX")) {
            Draw_BgmTab("sfx", sequenceMap, SFX_SOUND);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Ocarina SFX")) {
            Draw_SfxTab("ocarinaSfx", ocarinaSFXMap);
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
