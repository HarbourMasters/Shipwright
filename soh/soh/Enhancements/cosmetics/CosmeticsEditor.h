#pragma once
#include <libultraship/libultraship.h>

#define PATCH_GFX(path, name, cvar, index, instruction) \
    if (CVarGetInteger(cvar, 0)) { \
        ResourceMgr_PatchGfxByName(path, name, index, instruction); \
    } else { \
        ResourceMgr_UnpatchGfxByName(path, name); \
    }

// Not to be confused with tabs, groups are 1:1 with the boxes shown in the UI, grouping them allows us to reset/randomize
// every item in a group at once. If you are looking for tabs they are rendered manually in ImGui in `DrawCosmeticsEditor`
typedef enum {
    GROUP_LINK,
    GROUP_MIRRORSHIELD,
    GROUP_SWORDS,
    GROUP_GLOVES,
    GROUP_EQUIPMENT,
    GROUP_CONSUMABLE,
    GROUP_HUD,
    GROUP_KALEIDO,
    GROUP_TITLE,
    GROUP_NPC,
    GROUP_WORLD,
    GROUP_MAGIC,
    GROUP_ARROWS,
    GROUP_SPIN_ATTACK,
    GROUP_TRAILS,
    GROUP_NAVI,
    GROUP_IVAN,
} CosmeticGroup;

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
void CosmeticsEditor_RandomizeGroup(CosmeticGroup group);
void CosmeticsEditor_ResetAll();
void CosmeticsEditor_ResetGroup(CosmeticGroup group);
void ApplyOrResetCustomGfxPatches(bool manualChange = true);

class CosmeticsEditorWindow : public LUS::GuiWindow {
  public:
    using GuiWindow::GuiWindow;

    void InitElement() override;
    void DrawElement() override;
    void UpdateElement() override {};
};