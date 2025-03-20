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

// TODO: Ideally everything below this point will come from one/many JSON files

// Enhancement presets
const std::vector<PresetEntry> vanillaPlusPresetEntries = {
    //PRESET_ENTRY_S32(CVAR_SETTING("DpadInText"), 1),
};

const std::vector<PresetEntry> enhancedPresetEntries = {
    
};

const std::vector<PresetEntry> randomizerPresetEntries = {
    
};

// Randomizer presets
const std::vector<PresetEntry> randomizerBeginnerPresetEntries = {
    
};

const std::vector<PresetEntry> randomizerStandardPresetEntries = {
    
};

const std::vector<PresetEntry> randomizerAdvancedPresetEntries = {
    
};

const std::vector<PresetEntry> hellModePresetEntries = {
    
};

typedef struct PresetDefinition {
    const char* label;
    const char* description;
    std::vector<PresetEntry> entries;
} PresetDefinition;

typedef struct PresetTypeDefinition {
    std::vector<const char*> blocksToClear;
    std::map<uint16_t, PresetDefinition> presets;
} PresetTypeDefinition;

const std::map<PresetType, PresetTypeDefinition> presetTypes = {
    { PRESET_TYPE_ENHANCEMENTS,
      { { CVAR_PREFIX_ENHANCEMENT, CVAR_PREFIX_CHEAT },
        {
            { ENHANCEMENT_PRESET_DEFAULT,
              {
                  "Default",
                  "Reset all options to their default values.",
                  {},
              } },
            { ENHANCEMENT_PRESET_VANILLA_PLUS,
              {
                  "Vanilla Plus",
                  "Adds quality of life features that enhance your experience, but don't alter gameplay. Recommended "
                  "for a first playthrough of OoT.",
                  vanillaPlusPresetEntries,
              } },
            { ENHANCEMENT_PRESET_ENHANCED,
              { "Enhanced",
                "The \"Vanilla Plus\" preset, but with more quality of life enhancements that might alter gameplay "
                "slightly. Recommended for returning players going through the vanilla game again.",
                enhancedPresetEntries } },
            { ENHANCEMENT_PRESET_RANDOMIZER,
              { "Randomizer",
                "A baseline set of enhancements for playing randomizer. Includes many quality of life options and "
                "options to speed up gameplay.",
                randomizerPresetEntries } },
        } } },
    { PRESET_TYPE_RANDOMIZER,
      { { CVAR_PREFIX_RANDOMIZER_SETTING, CVAR_PREFIX_RANDOMIZER_ENHANCEMENT },
        {
            { RANDOMIZER_PRESET_DEFAULT,
              {
                  "Default",
                  "Reset all options to their default values.",
                  {},
              } },
            { RANDOMIZER_PRESET_BEGINNER,
              {
                  "Beginner",
                  "A simpler set of options and shuffled items meant for players new to the randomizer. ",
                  randomizerBeginnerPresetEntries,
              } },
            { RANDOMIZER_PRESET_STANDARD,
              {
                  "Standard",
                  "A set of options meant as a baseline for both newer and experienced randomizer players.",
                  randomizerStandardPresetEntries,
              } },
            { RANDOMIZER_PRESET_ADVANCED,
              {
                  "Advanced",
                  "Includes many more shuffled items and introduces some entrance shuffle options. Meant for advanced "
                  "randomizer players.",
                  randomizerAdvancedPresetEntries,
              } },
            { RANDOMIZER_PRESET_HELL_MODE,
              { "Hell Mode", "All settings maxed but still using glitchless logic. Expect pain.",
                hellModePresetEntries } },
        } } }
};
