#include "soh/Enhancements/custom-message/GameText.h"
#include "soh/Enhancements/Lang/Lang.h"
#include "soh/Enhancements/Lang/TranslationCatalog.h"
#include <string.h>
#include <vector>
#include <algorithm>
#include <array>
#include <assert.h>

#include <spdlog/spdlog.h>

#include "util.h"

extern "C" {
#include <variables.h>
}

std::string invalidString = "";

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
    "Twinrova's Lair",
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
    "Dampe's Grave & Windmill",
    "Fishing Pond",
    "Castle Courtyard",
    "Bombchu Bowling Alley",
    "Ranch House & Silo",
    "Guard House",
    "Granny's Potion Shop",
    "Ganon's Tower Collapse & Arena",
    "House of Skulltula",
    "Hyrule Field",
    "Kakariko Village",
    "Graveyard",
    "Zora's River",
    "Kokiri Forest",
    "Sacred Forest Meadow",
    "Lake Hylia",
    "Zora's Domain",
    "Zora's Fountain",
    "Gerudo Valley",
    "Lost Woods",
    "Desert Colossus",
    "Gerudo's Fortress",
    "Haunted Wasteland",
    "Hyrule Castle",
    "Death Mountain Trail",
    "Death Mountain Crater",
    "Goron City",
    "Lon Lon Ranch",
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
    "Unknown",
};

static const std::vector<std::string>& GetItemNames(uint8_t language) {
    static const auto english = CustomMessages::GetGameTextPool("en_US", "vanilla.item_names");
    static const auto german = CustomMessages::GetGameTextPool("de_DE", "vanilla.item_names");
    static const auto french = CustomMessages::GetGameTextPool("fr_FR", "vanilla.item_names");
    switch (language) {
        case LANGUAGE_GER:
            return german;
        case LANGUAGE_FRA:
            return french;
        default:
            return english;
    }
}

std::vector<std::string> questItemNamesEng = {
    "Forest Medallion",   "Fire Medallion",   "Water Medallion", "Spirit Medallion",     "Shadow Medallion",
    "Light Medallion",    "Minuet of Forest", "Bolero of Fire",  "Serenade of Water",    "Requiem of Spirit",
    "Nocturne of Shadow", "Prelude of Light", "Zelda's Lullaby", "Epona's Song",         "Saria's Song",
    "Sun's Song",         "Song of Time",     "Song of Storms",  "Kokiri's Emerald",     "Goron's Ruby",
    "Zora's Sapphire",    "Stone of Agony",   "Gerudo's Card",   "Gold Skulltula Token",
};

std::vector<std::string> questItemNamesFra = {
    "Médaillon de la Forêt", "Médaillon du Feu",        "Médaillon de l'Eau",  "Médaillon de l'Esprit",
    "Médaillon de l'Ombre",  "Médaillon de la Lumière", "Menuet des Bois",     "Boléro du Feu",
    "Sérénade de l'Eau",     "Requiem de l'Esprit",     "Nocturne de l'Ombre", "Prélude de la Lumière",
    "Berceuse de Zelda",     "Chant d'Epona",           "Chant de Saria",      "Chant du Soleil",
    "Chant du Temps",        "Chant des Tempêtes",      "Émeraude Kokiri",     "Rubis Goron",
    "Saphir Zora",           "Pierre de Souffrance",    "Carte Gerudo",        "Symbole de Skulltula d'Or",
};

std::vector<std::string> questItemNamesGer = {
    "Amulett des Waldes",
    "Amulett des Feuers",
    "Amulett des Wassers",
    "Amulett der Geister",
    "Amulett des Schattens",
    "Amulett des Lichts",
    "Menuett des Waldes",
    "Bolero des Feuers",
    "Serenade des Wassers",
    "Requiem der Geister",
    "Nocturne des Schattens",
    "Kantate des Lichts",
    "Zeldas Wiegenlied",
    "Eponas Lied",
    "Salias Lied",
    "Hymne der Sonne",
    "Hymne der Zeit",
    "Hymne des Sturms",
    "Kokiri-Smaragd",
    "Goronen-Rubin",
    "Zora-Saphir",
    "Stein des Wissens",
    "Gerudo-Paß",
    "Goldenes Skulltula-Symbol",
};

std::array<std::string, RA_MAX> rcareaPrefixes = {
    "KF",
    "LW",
    "SFM",
    "HF",
    "LH",
    "GV",
    "GF",
    "Wasteland",
    "Colossus",
    "Market",
    "HC",
    "Kak",
    "Graveyard",
    "DMT",
    "GC",
    "DMC",
    "ZR",
    "ZD",
    "ZF",
    "LLR",
    "Deku Tree",
    "Dodongos Cavern",
    "Jabu Jabus Belly",
    "Forest Temple",
    "Fire Temple",
    "Water Temple",
    "Spirit Temple",
    "Shadow Temple",
    "Bottom of the Well",
    "Ice Cavern",
    "Gerudo Training Ground",
    "Ganon's Castle",
};

const std::string& SohUtils::GetSceneName(int32_t scene) {
    if (scene < 0 || static_cast<size_t>(scene) >= sceneNames.size()) {
        SPDLOG_WARN("Passed invalid scene id to SohUtils::GetSceneName: ({})", scene);
        assert(false);
        return invalidString;
    }

    return sceneNames[scene];
}

const std::string& SohUtils::GetItemName(int32_t item) {
    if (item >= 0) {
        const auto* text = CustomMessages::GetGameTextCatalog().Find(
            Lang::GetUILanguage(), "messages.vanilla.item_names." + std::to_string(item));
        if (text != nullptr) {
            return *text;
        }
    }
    return GetGameItemName(item, Lang::GetUIResourceLanguage());
}

const std::string& SohUtils::GetGameItemName(int32_t item, uint8_t language) {
    const auto* currentItemNames = &GetItemNames(language);

    if (item < 0 || static_cast<size_t>(item) >= currentItemNames->size()) {
        SPDLOG_WARN("Passed invalid item id to SohUtils::GetItemName: ({})", item);
        assert(false);
        return invalidString;
    }

    return (*currentItemNames)[item];
}

const std::string& SohUtils::GetQuestItemName(int32_t item) {
    const std::vector<std::string>* currentQuestItemNames = nullptr;

    switch (Lang::GetUIResourceLanguage()) {
        case LANGUAGE_FRA:
            currentQuestItemNames = &questItemNamesFra;
            break;
        case LANGUAGE_GER:
            currentQuestItemNames = &questItemNamesGer;
            break;
        case LANGUAGE_ENG:
        default:
            currentQuestItemNames = &questItemNamesEng;
            break;
    }
    if (item < 0 || static_cast<size_t>(item) >= questItemNamesEng.size()) {
        SPDLOG_WARN("Passed invalid quest item id to SohUtils::GetQuestItemName: ({})", item);
        assert(false);
        return invalidString;
    }

    return (*currentQuestItemNames)[item];
}

const std::string& SohUtils::GetRandomizerCheckAreaPrefix(int32_t rcarea) {
    if (rcarea < 0 || static_cast<size_t>(rcarea) >= rcareaPrefixes.size()) {
        SPDLOG_WARN("Passed invalid rcarea to SohUtils::GetRandomizerCheckAreaPrefix: ({})", rcarea);
        assert(false);
        return invalidString;
    }

    return rcareaPrefixes[rcarea];
}

void SohUtils::CopyStringToCharArray(char* destination, std::string source, size_t size) {
    if (size > 0) {
        strncpy(destination, source.c_str(), size - 1);
        destination[size - 1] = '\0';
    }
}

std::string SohUtils::Sanitize(std::string stringValue) {
    stringValue.erase(std::remove_if(stringValue.begin(), stringValue.end(),
                                     [](char const c) { return '\n' == c || '\r' == c || '\0' == c || '\x1A' == c; }),
                      stringValue.end());

    return stringValue;
}

size_t SohUtils::CopyStringToCharBuffer(char* buffer, const std::string& source, const size_t maxBufferSize) {
    if (!source.empty() && maxBufferSize > 0) {
        memset(buffer, 0, maxBufferSize);
        const size_t copiedCharLen = std::min<size_t>(maxBufferSize - 1, source.length());
        memcpy(buffer, source.c_str(), copiedCharLen);
        return copiedCharLen;
    }

    return 0;
}

bool SohUtils::IsStringEmpty(std::string str) {
    // Remove spaces at the beginning of the string
    std::string::size_type start = str.find_first_not_of(' ');
    // Remove spaces at the end of the string
    std::string::size_type end = str.find_last_not_of(' ');

    // Check if the string is empty after stripping spaces
    return start == std::string::npos || end == std::string::npos;
}

uint32_t SohUtils::Hash(std::string str) {
    // FNV-1a
    const size_t len = str.size();
    uint32_t hval = 0x811c9dc5;
    for (size_t pos = 0; pos < len; pos++) {
        hval ^= (uint32_t)str[pos];
        hval *= 0x01000193;
    }
    return hval;
}

std::vector<std::string> SohUtils::StringSplit(const std::string& str, const std::string& delimiter) {
    std::vector<std::string> tokens;
    size_t pos = str.find(delimiter, 0);
    size_t prevpos = 0;

    while (pos != std::string::npos) {
        std::string token = str.substr(prevpos, pos - prevpos);
        tokens.push_back(token);
        prevpos = pos + 1;
        pos = str.find(delimiter, prevpos);
    }

    tokens.push_back(str.substr(prevpos));

    return tokens;
}