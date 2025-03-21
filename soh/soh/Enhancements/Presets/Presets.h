#pragma once

#include <map>
#include <string>
#include <vector>
#include <variant>
#include <cstdint>
#include "soh/Enhancements/randomizer/randomizerTypes.h"
#include "soh/Enhancements/enhancementTypes.h"
#include "soh/OTRGlobals.h"
#include "soh/cvar_prefixes.h"

enum PresetEntryType {
    PRESET_ENTRY_TYPE_S32,
    PRESET_ENTRY_TYPE_FLOAT,
    PRESET_ENTRY_TYPE_STRING,
    PRESET_ENTRY_TYPE_CPP_STRING,
};

enum PresetType {
    PRESET_TYPE_ENHANCEMENTS,
    PRESET_TYPE_RANDOMIZER,
};

enum EnhancementPreset {
    ENHANCEMENT_PRESET_DEFAULT,
    ENHANCEMENT_PRESET_VANILLA_PLUS,
    ENHANCEMENT_PRESET_ENHANCED,
    ENHANCEMENT_PRESET_RANDOMIZER,
};

enum RandomizerPreset {
    RANDOMIZER_PRESET_DEFAULT,
    RANDOMIZER_PRESET_BEGINNER,
    RANDOMIZER_PRESET_STANDARD,
    RANDOMIZER_PRESET_ADVANCED,
    RANDOMIZER_PRESET_HELL_MODE,
};

typedef struct PresetEntry {
    const char* cvar;
    PresetEntryType type;
    std::variant<int32_t, float, const char*, std::string> value;
} PresetEntry;

std::string FormatLocations(std::vector<RandomizerCheck> locs);

#define PRESET_ENTRY_S32(cvar, value) \
    { cvar,    PRESET_ENTRY_TYPE_S32, value }
#define PRESET_ENTRY_FLOAT(cvar, value) \
    { cvar,  PRESET_ENTRY_TYPE_FLOAT, value }
#define PRESET_ENTRY_STRING(cvar, value) \
    { cvar, PRESET_ENTRY_TYPE_STRING, value }
#define PRESET_ENTRY_CPP_STRING(cvar, value) \
    { cvar, PRESET_ENTRY_TYPE_CPP_STRING, value }

void DrawPresetSelector(PresetType presetType);
void clearCvars(std::vector<const char*> cvarsToClear);
void applyPreset(std::vector<PresetEntry> entries);

typedef struct PresetDefinition {
    const char* label;
    const char* description;
    std::vector<PresetEntry> entries;
} PresetDefinition;

typedef struct PresetTypeDefinition {
    std::vector<const char*> blocksToClear;
    std::map<uint16_t, PresetDefinition> presets;
} PresetTypeDefinition;

extern const std::map<PresetType, PresetTypeDefinition> presetTypes;
