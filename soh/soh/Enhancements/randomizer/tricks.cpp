#include "tricks.h"
#include "soh/SohGui/UIWidgets.hpp"
#include <unordered_map>

namespace Rando {
const std::unordered_map<RandomizerArea, std::string> rtAreaNames = {
    { RA_NONE, "General Tricks" },
    { RA_KOKIRI_FOREST, "Kokiri Forest" },
    { RA_THE_LOST_WOODS, "Lost Woods" },
    { RA_SACRED_FOREST_MEADOW, "Sacred Forest Meadow" },
    { RA_HYRULE_FIELD, "Hyrule Field" },
    { RA_LAKE_HYLIA, "Lake Hylia" },
    { RA_GERUDO_VALLEY, "Gerudo Valley" },
    { RA_GERUDO_FORTRESS, "Gerudo Fortress" },
    { RA_HAUNTED_WASTELAND, "Haunted Wasteland" },
    { RA_DESERT_COLOSSUS, "Desert Colossus" },
    { RA_THE_MARKET, "Hyrule Market" },
    { RA_HYRULE_CASTLE, "Hyrule Castle" },
    { RA_KAKARIKO_VILLAGE, "Kakariko Village" },
    { RA_THE_GRAVEYARD, "Graveyard" },
    { RA_DEATH_MOUNTAIN_TRAIL, "Death Mountain Trail" },
    { RA_GORON_CITY, "Goron City" },
    { RA_DEATH_MOUNTAIN_CRATER, "Death Mountain Crater" },
    { RA_ZORAS_RIVER, "Zora's River" },
    { RA_ZORAS_DOMAIN, "Zora's Domain" },
    { RA_ZORAS_FOUNTAIN, "Zora's Fountain" },
    { RA_LON_LON_RANCH, "Lon Lon Ranch" },
    { RA_DEKU_TREE, "Deku Tree" },
    { RA_DODONGOS_CAVERN, "Dodongo's Cavern" },
    { RA_JABU_JABUS_BELLY, "Jabu Jabu's Belly" },
    { RA_FOREST_TEMPLE, "Forest Temple" },
    { RA_FIRE_TEMPLE, "Fire Temple" },
    { RA_WATER_TEMPLE, "Water Temple" },
    { RA_SPIRIT_TEMPLE, "Spirit Temple" },
    { RA_SHADOW_TEMPLE, "Shadow Temple" },
    { RA_BOTTOM_OF_THE_WELL, "Bottom of the Well" },
    { RA_ICE_CAVERN, "Ice Cavern" },
    { RA_GERUDO_TRAINING_GROUND, "Gerudo Training Ground" },
    { RA_GANONS_CASTLE, "Ganon's Castle" },
    { RA_MAX, "Invalid" },
};

const std::string& Tricks::GetAreaName(const RandomizerArea area) {
    return rtAreaNames.at(area);
}

bool Tricks::CheckTags(const std::map<Tag, bool>& showTag, const std::set<Tag>& rtTags) {
    if (rtTags.empty()) {
        return false;
    }
    for (Tag rtTag : rtTags) {
        if (!showTag.at(rtTag)) {
            return false;
        }
    }
    return true;
}

const std::unordered_map<Tricks::Tag, std::string> rtTagNames = {
    { Tricks::Tag::NOVICE, "Novice" },     { Tricks::Tag::INTERMEDIATE, "Intermediate" },
    { Tricks::Tag::ADVANCED, "Advanced" }, { Tricks::Tag::EXPERT, "Expert" },
    { Tricks::Tag::EXTREME, "Extreme" },   { Tricks::Tag::EXPERIMENTAL, "Experimental" },
    { Tricks::Tag::GLITCH, "Glitch" },
};

const std::string Tricks::GetTagName(const Tag tag) {
    return rtTagNames.at(tag);
}

const ImVec4 Tricks::GetTextColor(const Tag tag) {
    switch (tag) {
        case Tag::GLITCH:
            return { 0.00f, 0.00f, 0.00f, 1.0f };
        default:
            return { 1.00f, 1.00f, 1.00f, 1.00f };
    }
}

const ImVec4 Tricks::GetTagColor(const Tag tag) {
    switch (tag) {
        case Tag::NOVICE:
            return UIWidgets::ColorValues.at(UIWidgets::Colors::Green);
        case Tag::INTERMEDIATE:
            return UIWidgets::ColorValues.at(UIWidgets::Colors::Orange);
        case Tag::ADVANCED:
            return UIWidgets::ColorValues.at(UIWidgets::Colors::Blue);
        case Tag::EXPERT:
            return UIWidgets::ColorValues.at(UIWidgets::Colors::Red);
        case Tag::EXTREME:
            return UIWidgets::ColorValues.at(UIWidgets::Colors::Purple);
        case Tag::EXPERIMENTAL:
            return UIWidgets::ColorValues.at(UIWidgets::Colors::LightBlue);
        case Tag::GLITCH:
            return UIWidgets::ColorValues.at(UIWidgets::Colors::White);
        default:
            assert(false);
            return UIWidgets::ColorValues.at(UIWidgets::Colors::Gray);
    }
}

void Tricks::DrawTagChips(const std::set<Tag>& rtTags, std::string trickName) {
    for (const Tag rtTag : rtTags) {
        std::string tagId = GetTagName(rtTag) + "##" + trickName;
        ImGui::SameLine();
        ImGui::BeginDisabled();
        UIWidgets::PushStyleButton(GetTagColor(rtTag));
        ImGui::SmallButton(tagId.c_str());
        UIWidgets::PopStyleButton();
        ImGui::EndDisabled();
    }
}
} // namespace Rando