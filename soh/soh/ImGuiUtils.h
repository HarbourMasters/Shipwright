#ifndef IM_GUI_UTILS_H
#define IM_GUI_UTILS_H

#pragma once

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include <imgui.h>
#include <array>
#include <map>
#include <string>
#include "Enhancements/randomizer/randomizerTypes.h"
#include "variables.h" //only for gItemIcons

extern "C" {
#include "textures/icon_item_static/icon_item_static.h"
#include "textures/icon_item_24_static/icon_item_24_static.h"
#include "textures/parameter_static/parameter_static.h"
}

const char* GetTextureForItemId(uint32_t itemId);
void RegisterImGuiItemIcons();

typedef struct {
    uint32_t id;
    std::string name;
    std::string nameFaded;
    std::string texturePath;
} ItemMapEntry;

#define ITEM_MAP_ENTRY(id) \
    {                      \
        id, {              \
            id, #id, #id "_Faded", static_cast<char*>(gItemIcons[id]) \
        }                  \
    }

// Maps items ids to info for use in ImGui
extern std::map<uint32_t, ItemMapEntry> itemMapping;

extern std::map<uint32_t, ItemMapEntry> gregMapping;

typedef struct {
    uint32_t id;
    std::string name;
    std::string nameFaded;
    std::string texturePath;
} QuestMapEntry;

#define QUEST_MAP_ENTRY(id, tex)       \
    {                                  \
        id, {                          \
            id, #id, #id "_Faded", tex \
        }                              \
    }

// Maps quest items ids to info for use in ImGui
extern std::map<uint32_t, QuestMapEntry> questMapping;

typedef struct {
    uint32_t id;
    std::string name;
    std::string nameFaded;
    ImVec4 color;
} SongMapEntry;

#define SONG_MAP_ENTRY(id, r, g, b) \
    {                               \
            id, #id, #id "_Faded", ImVec4(r / 255.0f, g / 255.0f, b / 255.0f, 1.0f) \
    }

// Maps song ids to info for use in ImGui
extern std::array<SongMapEntry, 12> songMapping;

#define VANILLA_SONG_MAP_ENTRY(id, r, g, b) \
    {                                       \
            id, #id "_Vanilla", #id "_Vanilla_Faded", ImVec4(r / 255.0f, g / 255.0f, b / 255.0f, 1.0f) \
    }

// Maps song ids to info for use in ImGui
extern std::array<SongMapEntry, 12> vanillaSongMapping;

#endif //IM_GUI_UTILS_H