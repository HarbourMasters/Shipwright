#pragma once
#include <ImGuiImpl.h>

#define PATCH_GFX(path, name, cvar, index, instruction) \
    if (CVarGetInteger(cvar, 0)) { \
        ResourceMgr_PatchGfxByName(path, name, index, instruction); \
    } else { \
        ResourceMgr_UnpatchGfxByName(path, name); \
    }

typedef struct {
    const std::string Name;
    const std::string ToolTip;
    const std::string CvarName;
    ImVec4 ModifiedColor;
    ImVec4 DefaultColor;
    bool canRainbow;
    bool hasAlpha;
    bool sameLine;
} CosmeticsColorIndividual;

static float TablesCellsWidth = 300.0f;
static ImGuiTableColumnFlags FlagsTable = ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV;
static ImGuiTableColumnFlags FlagsCell = ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort;

void InitCosmeticsEditor();//Init the menu itself
ImVec4 GetRandomValue(int MaximumPossible);
void CosmeticsEditor_RandomizeAll();
void CosmeticsEditor_ResetAll();
void ApplyOrResetCustomGfxPatches(bool manualChange = true);
