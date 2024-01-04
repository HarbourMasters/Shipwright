#ifndef TRICKS_H
#define TRICKS_H

#include <imgui.h>
#include <set>

#include "randomizerTypes.h"

#include <unordered_map>
#include <string>


namespace Rando::Tricks {
    static const std::unordered_map<RandomizerArea, std::string> rtAreaNames = {
        { RA_NONE, "General Tricks"},
        //{ RTAREA_BK_SKIPS, "Boss Key Skips"},
        { RA_KOKIRI_FOREST, "Kokiri Forest"},
        { RA_THE_LOST_WOODS, "Lost Woods"},
        { RA_SACRED_FOREST_MEADOW, "Sacred Forest Meadow"},
        { RA_HYRULE_FIELD, "Hyrule Field"},
        { RA_LAKE_HYLIA, "Lake Hylia"},
        { RA_GERUDO_VALLEY, "Gerudo Valley"},
        { RA_GERUDO_FORTRESS, "Gerudo Fortress"},
        { RA_HAUNTED_WASTELAND, "Haunted Wasteland"},
        { RA_DESERT_COLOSSUS, "Desert Colossus"},
        { RA_THE_MARKET, "Hyrule Market"},
        { RA_HYRULE_CASTLE, "Hyrule Castle"},
        { RA_KAKARIKO_VILLAGE, "Kakariko Village"},
        { RA_THE_GRAVEYARD, "Graveyard"},
        { RA_DEATH_MOUNTAIN_TRAIL, "Death Mountain Trail"},
        { RA_GORON_CITY, "Goron City"},
        { RA_DEATH_MOUNTAIN_CRATER, "Death Mountain Crater"},
        { RA_ZORAS_RIVER, "Zora's River"},
        { RA_ZORAS_DOMAIN, "Zora's Domain"},
        { RA_ZORAS_FOUNTAIN, "Zora's Fountain"},
        { RA_LON_LON_RANCH, "Lon Lon Ranch"},
        { RA_DEKU_TREE, "Deku Tree"},
        { RA_DODONGOS_CAVERN, "Dodongo's Cavern"},
        { RA_JABU_JABUS_BELLY, "Jabu Jabu's Belly"},
        { RA_FOREST_TEMPLE, "Forest Temple"},
        { RA_FIRE_TEMPLE, "Fire Temple"},
        { RA_WATER_TEMPLE, "Water Temple"},
        { RA_SPIRIT_TEMPLE, "Spirit Temple"},
        { RA_SHADOW_TEMPLE, "Shadow Temple"},
        { RA_BOTTOM_OF_THE_WELL, "Bottom of the Well"},
        { RA_ICE_CAVERN, "Ice Cavern"},
        { RA_GERUDO_TRAINING_GROUND, "Gerudo Training Grounds"},
        { RA_GANONS_CASTLE, "Ganon's Castle"},
        { RA_MAX, "Invalid"},
    };

    enum class Tag {
        NOVICE,
        INTERMEDIATE,
        ADVANCED,
        EXPERT,
        EXTREME,
        /*LENS,
         *BKSKIP,
         *EXPERIMENTAL*/
    };

    static std::unordered_map<Tag, std::string> rtTagNames = {
        {Tag::NOVICE, "Novice"},
        {Tag::INTERMEDIATE, "Intermediate"},
        {Tag::ADVANCED, "Advanced"},
        {Tag::EXPERT, "Expert"},
        {Tag::EXTREME, "Extreme"},
        /*{Rando::Tag::LENS, "Lens"},
        {Rando::Tag::BKSKIP, "Boss Key Skip"},
        {Rando::Tag::EXPERIMENTAL, "Experimental"}*/
    };

    const std::string& GetRTAreaName(RandomizerArea area);
    bool CheckRTTags(std::unordered_map<Tag, bool> &showTag, const std::set<Tag> &rtTags);
    std::string GetRTTagName(Tag tag);
    ImVec4 GetRTTagColor(Tag tag);
    void DrawTagChips(const std::set<Tag> &rtTags);

}


#endif //TRICKS_H
