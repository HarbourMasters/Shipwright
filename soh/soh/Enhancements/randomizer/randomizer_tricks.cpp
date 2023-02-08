#include "randomizer_tricks.h"
#include <map>
#include <string>
#include <libultraship/bridge.h>
#include "z64.h"


//            RandomizerTrick,                                                RTVORMQ,         RTAREA,                        rt_glitch (is it a glitch)                 "Short name", "Description"
std::map<RandomizerTrick, RandomizerTrickObject> rtObjects = {
    RT_OBJECT(RT_ACUTE_ANGLE_CLIP,                                            RTVORMQ_BOTH,    RTAREA_GENERAL,                true,                                      "Acute angle clip", "Enables locations requiring jumpslash clips through walls which meet at an acute angle."),
    RT_OBJECT(RT_ADVANCED_CLIPS,                                              RTVORMQ_BOTH,    RTAREA_GENERAL,                true,                                      "Advanced clips", "Enables locations requiring clips through walls and objects requiring precise jumps or other tricks."),
    RT_OBJECT(RT_BLANK_A,                                                     RTVORMQ_BOTH,    RTAREA_GENERAL,                true,                                      "Blank A", "Enables locations requiring blank A button; NOTE: this requires the 'Quick Putaway' restoration."),
    RT_OBJECT(RT_DOOM_JUMP,                                                   RTVORMQ_BOTH,    RTAREA_GENERAL,                true,                                      "Doom Jump", "Enables locations requiring doom jumps."),
    RT_OBJECT(RT_EPG,                                                         RTVORMQ_BOTH,    RTAREA_GENERAL,                true,                                      "EPG", "Enables locations requiring use of the Entrance Point Glitch."),
    RT_OBJECT(RT_EQUIP_SWAP,                                                  RTVORMQ_BOTH,    RTAREA_GENERAL,                true,                                      "Equip Swap", "Enables locations requiring use of equip swap; NOTE: this may expect the 'Allow cursor to be over any slot' enhancement to be turned off."),
    RT_OBJECT(RT_EQUIP_SWAP_EXPECTS_DINS,                                     RTVORMQ_BOTH,    RTAREA_GENERAL,                true,                                      "Equip Swap Require's Din's Fire", "Enables locations requiring use of equip swap once din's fire is found."),
    RT_OBJECT(RT_FLAME_STORAGE,                                               RTVORMQ_BOTH,    RTAREA_GENERAL,                true,                                      "Flame Storage", "Enables locations requiring flame storage."),
    RT_OBJECT(RT_GROUND_CLIP,                                                 RTVORMQ_BOTH,    RTAREA_GENERAL,                true,                                      "Ground Clip", "Enables locations requiring ground clips."),
    RT_OBJECT(RT_GROUND_JUMP,                                                 RTVORMQ_BOTH,    RTAREA_GENERAL,                true,                                      "Ground Jump", "Enables locations requiring ground jumps."),
    RT_OBJECT(RT_HESS,                                                        RTVORMQ_BOTH,    RTAREA_GENERAL,                true,                                      "HESS", "Enables locations requiring a Hyper Extended Super Slide."),
    RT_OBJECT(RT_HOOKSHOT_CLIP,                                               RTVORMQ_BOTH,    RTAREA_GENERAL,                true,                                      "Hookshot Clip", "Enables locations requiring hookshot clips."),
    RT_OBJECT(RT_HOOKSHOT_JUMP,                                               RTVORMQ_BOTH,    RTAREA_GENERAL,                true,                                      "Hookshot Jump", "Enables locations requiring hookshot jumps."),
    RT_OBJECT(RT_ISG,                                                         RTVORMQ_BOTH,    RTAREA_GENERAL,                true,                                      "ISG", "Enables locations requiring use of the infinite sword glitch."),
    RT_OBJECT(RT_BUNNY_HOOD_JUMPS,                                            RTVORMQ_BOTH,    RTAREA_GENERAL,                false,                                     "Bunny Hood Jumps", "Enables locations requiring bunny hoods long jumps; NOTE: bunny hood jumps must be enabled in the enhancements."),
    RT_OBJECT(RT_DAMAGE_BOOST,                                                RTVORMQ_BOTH,    RTAREA_GENERAL,                false,                                     "Damage Boost", "Enables locations requiring a damage boost, excluding glitchless spirit hover."),
    RT_OBJECT(RT_NO_LENS,                                                     RTVORMQ_BOTH,    RTAREA_GENERAL,                false,                                     "No Lens of Truth", "Enables locations which normally require the lens of truth to be reached without it, excluding treasure minigame."),
    RT_OBJECT(RT_NO_STONE_OF_AGONY,                                           RTVORMQ_BOTH,    RTAREA_GENERAL,                false,                                     "No Stone of Agony", "Enables grotto locations which may need stone of agony to locate."),
    RT_OBJECT(RT_RECOIL_BOOST,                                                RTVORMQ_BOTH,    RTAREA_GENERAL,                false,                                     "Recoil Boost", "Enables locations requiring recoil boosts.")
};

std::map<RandomizerTrickArea, std::string> rtAreaNames = {
    { RTAREA_GENERAL, "General Tricks"},
    { RTAREA_EXPLOSIVES, "Common Tricks With Explosives"},
    { RTAREA_BK_SKIPS, "Boss Key Skips"},
    { RTAREA_KOKIRI_FOREST, "Kokiri Forest"},
    { RTAREA_LOST_WOODS, "Lost Woods"},
    { RTAREA_SACRED_FOREST_MEADOW, "Sacred Forest Meadow"},
    { RTAREA_HYRULE_FIELD, "Hyrule Field"},
    { RTAREA_LAKE_HYLIA, "Lake Hylia"},
    { RTAREA_GERUDO_VALLEY, "Gerudo Valley"},
    { RTAREA_GERUDO_FORTRESS, "Gerudo Fortress"},
    { RTAREA_WASTELAND, "Desert Wasteland"},   
    { RTAREA_DESERT_COLOSSUS, "Desert Colossus"},
    { RTAREA_MARKET, "Hyrule Market"},
    { RTAREA_HYRULE_CASTLE, "Hyrule Castle"},
    { RTAREA_KAKARIKO_VILLAGE, "Kakariko Village"},    
    { RTAREA_GRAVEYARD, "Graveyard"},
    { RTAREA_DEATH_MOUNTAIN_TRAIL, "Death Mountain Trail"},
    { RTAREA_GORON_CITY, "Goron City"},
    { RTAREA_DEATH_MOUNTAIN_CRATER, "Death Mountain Crater"},
    { RTAREA_ZORAS_RIVER, "Zora's River"},
    { RTAREA_ZORAS_DOMAIN, "Zora's Domain"},
    { RTAREA_ZORAS_FOUNTAIN, "Zora's Fountain"},
    { RTAREA_LON_LON_RANCH, "Lon Lon Ranch"},
    { RTAREA_DEKU_TREE, "Deku Tree"},
    { RTAREA_DODONGOS_CAVERN, "Dodongo's Cavern"},
    { RTAREA_JABU_JABUS_BELLY, "Jabu Jabu's Belly"},    
    { RTAREA_FOREST_TEMPLE, "Forest Temple"},    
    { RTAREA_FIRE_TEMPLE, "Fire Temple"},    
    { RTAREA_WATER_TEMPLE, "Water Temple"},
    { RTAREA_SPIRIT_TEMPLE, "Spirit Temple"},
    { RTAREA_SHADOW_TEMPLE, "Shadow Temple"},
    { RTAREA_BOTTOM_OF_THE_WELL, "Bottom of the Well"},
    { RTAREA_ICE_CAVERN, "Ice Cavern"},
    { RTAREA_GERUDO_TRAINING_GROUND, "Gerudo Training Grounds"},
    { RTAREA_GANONS_CASTLE, "Ganon's Castle"},
    { RTAREA_EXTREME, "Extreme tricks"},
    { RTAREA_INVALID, "Invalid"},
 };

std::map<RandomizerTrickArea, std::map<RandomizerTrick, RandomizerTrickObject>> rtObjectsByArea = {};

std::map<RandomizerTrickArea, std::map<RandomizerTrick,RandomizerTrickObject>> RandomizerTricks::GetAllRTObjectsByArea() {
    if (rtObjectsByArea.size() == 0) {
        for (auto& [randomizerTrick, rtObject] : rtObjects) {
            rtObjectsByArea[rtObject.rtArea][randomizerTrick] = rtObject;
        }
    }

    return rtObjectsByArea;
}

std::string RandomizerTricks::GetRTAreaName(RandomizerTrickArea area) {
    return rtAreaNames[area];
}