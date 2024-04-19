#include "util.h"

#include <string.h>
#include <vector>
#include <algorithm>

#include <stdio.h>

std::vector<std::string> sceneNames = {
    "Inside the Deku Tree",
    "Dodongo's Cavern",
    "Inside Jabu-Jabu's Belly",
    "Forest Temple",
    "Fire Temple",
    "Water Temple",
    "Spirit Temple",
    "Shadow Temple",
    "Bottom of the Well",
    "Ice Cavern",
    "Ganon's Tower",
    "Gerudo Training Ground",
    "Thieves' Hideout",
    "Inside Ganon's Castle",
    "Ganon's Tower (Collapsing)",
    "Inside Ganon's Castle (Collapsing)",
    "Treasure Box Shop",
    "Gohma's Lair",
    "King Dodongo's Lair",
    "Barinade's Lair",
    "Phantom Ganon's Lair",
    "Volvagia's Lair",
    "Morpha's Lair",
    "Twinrova's Lair & Nabooru's Mini-Boss Room",
    "Bongo Bongo's Lair",
    "Ganondorf's Lair",
    "Tower Collapse Exterior",
    "Market Entrance (Child - Day)",
    "Market Entrance (Child - Night)",
    "Market Entrance (Ruins)",
    "Back Alley (Child - Day)",
    "Back Alley (Child - Night)",
    "Market (Child - Day)",
    "Market (Child - Night)",
    "Market (Ruins)",
    "Temple of Time Exterior (Child - Day)",
    "Temple of Time Exterior (Child - Night)",
    "Temple of Time Exterior (Ruins)",
    "Know-It-All Brothers' House",
    "House of Twins",
    "Mido's House",
    "Saria's House",
    "Carpenter Boss's House",
    "Back Alley House (Man in Green)",
    "Bazaar",
    "Kokiri Shop",
    "Goron Shop",
    "Zora Shop",
    "Kakariko Potion Shop",
    "Market Potion Shop",
    "Bombchu Shop",
    "Happy Mask Shop",
    "Link's House",
    "Back Alley House (Dog Lady)",
    "Stable",
    "Impa's House",
    "Lakeside Laboratory",
    "Carpenters' Tent",
    "Gravekeeper's Hut",
    "Great Fairy's Fountain (Upgrades)",
    "Fairy's Fountain",
    "Great Fairy's Fountain (Spells)",
    "Grottos",
    "Grave (Redead)",
    "Grave (Fairy's Fountain)",
    "Royal Family's Tomb",
    "Shooting Gallery",
    "Temple of Time",
    "Chamber of the Sages",
    "Castle Hedge Maze (Day)",
    "Castle Hedge Maze (Night)",
    "Cutscene Map",
    "Damp�'s Grave & Windmill",
    "Fishing Pond",
    "Castle Courtyard",
    "Bombchu Bowling Alley",
    "Ranch House & Silo",
    "Guard House",
    "Granny's Potion Shop",
    "Ganon's Tower Collapse & Battle Arena",
    "House of Skulltula",
    "Spot 00 - Hyrule Field",
    "Spot 01 - Kakariko Village",
    "Spot 02 - Graveyard",
    "Spot 03 - Zora's River",
    "Spot 04 - Kokiri Forest",
    "Spot 05 - Sacred Forest Meadow",
    "Spot 06 - Lake Hylia",
    "Spot 07 - Zora's Domain",
    "Spot 08 - Zora's Fountain",
    "Spot 09 - Gerudo Valley",
    "Spot 10 - Lost Woods",
    "Spot 11 - Desert Colossus",
    "Spot 12 - Gerudo's Fortress",
    "Spot 13 - Haunted Wasteland",
    "Spot 15 - Hyrule Castle",
    "Spot 16 - Death Mountain Trail",
    "Spot 17 - Death Mountain Crater",
    "Spot 18 - Goron City",
    "Spot 20 - Lon Lon Ranch",
    "Ganon's Castle Exterior",
    "Jungle Gym",
    "Ganondorf Test Room",
    "Depth Test",
    "Stalfos Mini-Boss Room",
    "Stalfos Boss Room",
    "Sutaru",
    "Castle Hedge Maze (Early)",
    "Sasa Test",
    "Treasure Chest Room",
};

std::vector<std::string> itemNames = {
    "Deku Stick",
    "Deku Nut",
    "Bomb",
    "Fairy Bow",
    "Fire Arrow",
    "Din's Fire",
    "Fairy Slingshot",
    "Fairy Ocarina",
    "Ocarina of Time",
    "Bombchu",
    "Hookshot",
    "Longshot",
    "Ice Arrow",
    "Farore's Wind",
    "Boomerang",
    "Lens of Truth",
    "Magic Bean",
    "Megaton Hammer",
    "Light Arrow",
    "Nayru's Love",
    "Empty Bottle",
    "Red Potion",
    "Green Potion",
    "Blue Potion",
    "Bottled Fairy",
    "Fish",
    "Lon Lon Milk & Bottle",
    "Ruto's Letter",
    "Blue Fire",
    "Bug",
    "Big Poe",
    "Lon Lon Milk (Half)",
    "Poe",
    "Weird Egg",
    "Chicken",
    "Zelda's Letter",
    "Keaton Mask",
    "Skull Mask",
    "Spooky Mask",
    "Bunny Hood",
    "Goron Mask",
    "Zora Mask",
    "Gerudo Mask",
    "Mask of Truth",
    "SOLD OUT",
    "Pocket Egg",
    "Pocket Cucco",
    "Cojiro",
    "Odd Mushroom",
    "Odd Potion",
    "Poacher's Saw",
    "Goron's Sword (Broken)",
    "Prescription",
    "Eyeball Frog",
    "Eye Drops",
    "Claim Check",
    "Fairy Bow & Fire Arrow",
    "Fairy Bow & Ice Arrow",
    "Fairy Bow & Light Arrow",
    "Kokiri Sword",
    "Master Sword",
    "Giant's Knife & Biggoron's Sword",
    "Deku Shield",
    "Hylian Shield",
    "Mirror Shield",
    "Kokiri Tunic",
    "Goron Tunic",
    "Zora Tunic",
    "Kokiri Boots",
    "Iron Boots",
    "Hover Boots",
    "Bullet Bag (30)",
    "Bullet Bag (40)",
    "Bullet Bag (50)",
    "Quiver (30)",
    "Big Quiver (40)",
    "Biggest Quiver (50)",
    "Bomb Bag (20)",
    "Big Bomb Bag (30)",
    "Biggest Bomb Bag (40)",
    "Goron's Bracelet",
    "Silver Gauntlets",
    "Golden Gauntlets",
    "Silver Scale",
    "Golden Scale",
    "Giant's Knife (Broken)",
    "Adult's Wallet",
    "Giant's Wallet",
    "Deku Seeds (5)",
    "Fishing Pole",
    "Minuet of Forest",
    "Bolero of Fire",
    "Serenade of Water",
    "Requiem of Spirit",
    "Nocturne of Shadow",
    "Prelude of Light",
    "Zelda's Lullaby",
    "Epona's Song",
    "Saria's Song",
    "Sun's Song",
    "Song of Time",
    "Song of Storms",
    "Forest Medallion",
    "Fire Medallion",
    "Water Medallion",
    "Spirit Medallion",
    "Shadow Medallion",
    "Light Medallion",
    "Kokiri's Emerald",
    "Goron's Ruby",
    "Zora's Sapphire",
    "Stone of Agony",
    "Gerudo's Card",
    "Gold Skulltula Token",
    "Heart Container",
    "Piece of Heart",
    "Boss Key",
    "Compass",
    "Dungeon Map",
    "Small Key",
    "Small Magic Jar",
    "Large Magic Jar",
    "Piece of Heart",
    "[Removed]",
    "[Removed]",
    "[Removed]",
    "[Removed]",
    "[Removed]",
    "[Removed]",
    "[Removed]",
    "Lon Lon Milk",
    "Recovery Heart",
    "Green Rupee",
    "Blue Rupee",
    "Red Rupee",
    "Purple Rupee",
    "Huge Rupee",
    "[Removed]",
    "Deku Sticks (5)",
    "Deku Sticks (10)",
    "Deku Nuts (5)",
    "Deku Nuts (10)",
    "Bombs (5)",
    "Bombs (10)",
    "Bombs (20)",
    "Bombs (30)",
    "Arrows (Small)",
    "Arrows (Medium)",
    "Arrows (Large)",
    "Deku Seeds (30)",
    "Bombchu (5)",
    "Bombchu (20)",
    "Deku Stick Upgrade (20)",
    "Deku Stick Upgrade (30)",
    "Deku Nut Upgrade (30)",
    "Deku Nut Upgrade (40)",
};

std::vector<std::string> questItemNames = {
        "Forest Medallion",
        "Fire Medallion",
        "Water Medallion",
        "Spirit Medallion",
        "Shadow Medallion",
        "Light Medallion",
        "Minuet of Forest",
        "Bolero of Fire",
        "Serenade of Water",
        "Requiem of Spirit",
        "Nocturne of Shadow",
        "Prelude of Light",
        "Zelda's Lullaby",
        "Epona's Song",
        "Saria's Song",
        "Sun's Song",
        "Song of Time",
        "Song of Storms",
        "Kokiri's Emerald",
        "Goron's Ruby",
        "Zora's Sapphire",
        "Stone of Agony",
        "Gerudo's Card",
        "Gold Skulltula Token",
};

const std::string& SohUtils::GetSceneName(int32_t scene) {
    return sceneNames[scene];
}

const std::string& SohUtils::GetItemName(int32_t item) {
    return itemNames[item];
}

const std::string& SohUtils::GetQuestItemName(int32_t item) {
    return questItemNames[item];
}

void SohUtils::CopyStringToCharArray(char* destination, std::string source, size_t size) {
    strncpy(destination, source.c_str(), size - 1);
    destination[size - 1] = '\0';
}

std::string SohUtils::Sanitize(std::string stringValue) {
    // Add backslashes.
    for (auto i = stringValue.begin();;) {
        auto const pos = std::find_if(i, stringValue.end(), [](char const c) { return '\\' == c || '\'' == c || '"' == c; });
        if (pos == stringValue.end()) {
            break;
        }
        i = std::next(stringValue.insert(pos, '\\'), 2);
    }

    // Removes others.
    stringValue.erase(std::remove_if(stringValue.begin(), stringValue.end(), [](char const c) {
        return '\n' == c || '\r' == c || '\0' == c || '\x1A' == c; }), stringValue.end());

    return stringValue;
}

size_t SohUtils::CopyStringToCharBuffer(char* buffer, const std::string& source, const size_t maxBufferSize) {
    if (!source.empty()) {
        // Prevent potential horrible overflow due to implicit conversion of maxBufferSize to an unsigned. Prevents negatives.
        memset(buffer, 0, std::max<size_t>(0, maxBufferSize));
        // Gaurentee that this value will be greater than 0, regardless of passed variables.
        const size_t copiedCharLen = std::min<size_t>(std::max<size_t>(0, maxBufferSize - 1), source.length());
        memcpy(buffer, source.c_str(), copiedCharLen);
        return copiedCharLen;
    }

    return 0;
}

extern "C" void UpdateColor(const char* trailName, Color_RGBA8* color, int* changed, int* reset) {
    Color_RGBA8 defaultColor = { 255, 255, 255, 255 };
    char cvarName[50];
    if (CVarGetInteger(cvarName,0)) {
        snprintf(cvarName, sizeof(cvarName), "%s.Value", trailName);
        *color = CVarGetColor(cvarName, defaultColor);
        *changed = 1;
    } else if (*changed) {
        *color = defaultColor;
        *reset = 1;
    }
}

extern "C" void UpdateParticleColors(Color_RGBA8* color, Color_RGBA8 p1StartColor, Color_RGBA8 p2StartColor,
                          Color_RGBA8 p1EndColor, Color_RGBA8 p2EndColor) {
    p1StartColor.r = color->r;
    p2StartColor.r = color->r * 0.8f;
    p1EndColor.r = color->r * 0.6f;
    p2EndColor.r = color->r * 0.4f;

    p1StartColor.g = color->g;
    p2StartColor.g = color->g * 0.8f;
    p1EndColor.g = color->g * 0.6f;
    p2EndColor.g = color->g * 0.4f;

    p1StartColor.b = color->b;
    p2StartColor.b = color->b * 0.8f;
    p1EndColor.b = color->b * 0.6f;
    p2EndColor.b = color->b * 0.4f;
}

