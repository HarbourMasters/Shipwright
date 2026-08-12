#pragma once

// Not to be confused with tabs, groups are 1:1 with the boxes shown in the UI, grouping them allows us to
// reset/randomize every item in a group at once. If you are looking for tabs they are rendered manually in ImGui in
// `DrawCosmeticsEditor`
typedef enum {
    COSMETICS_GROUP_LINK,
    COSMETICS_GROUP_MIRRORSHIELD,
    COSMETICS_GROUP_SWORDS,
    COSMETICS_GROUP_GLOVES,
    COSMETICS_GROUP_EQUIPMENT,
    COSMETICS_GROUP_KEYRING,
    COSMETICS_GROUP_SMALL_KEYS,
    COSMETICS_GROUP_BOSS_KEYS,
    COSMETICS_GROUP_CONSUMABLE,
    COSMETICS_GROUP_HUD,
    COSMETICS_GROUP_KALEIDO,
    COSMETICS_GROUP_TITLE,
    COSMETICS_GROUP_NPC,
    COSMETICS_GROUP_WORLD,
    COSMETICS_GROUP_MAGIC,
    COSMETICS_GROUP_ARROWS,
    COSMETICS_GROUP_SPIN_ATTACK,
    COSMETICS_GROUP_TRAILS,
    COSMETICS_GROUP_NAVI,
    COSMETICS_GROUP_IVAN,
    COSMETICS_GROUP_MESSAGE,
    COSMETICS_GROUP_MAX,
} CosmeticGroup;

#ifdef __cplusplus
#include <string>
#include <map>
#include <ship/window/gui/GuiWindow.h>
#include "soh/SohGui/UIWidgets.hpp"
extern "C" {
#endif //__cplusplus

Color_RGBA8 CosmeticsEditor_GetDefaultValue(const char* id);

#ifdef __cplusplus
}

#define COSMETIC_OPTION(id, label, group, defaultColor, supportsAlpha, supportsRainbow, advancedOption)               \
    {                                                                                                                 \
        id, {                                                                                                         \
            CVAR_COSMETIC(id), CVAR_COSMETIC(id ".Value"), CVAR_COSMETIC(id ".Rainbow"), CVAR_COSMETIC(id ".Locked"), \
                CVAR_COSMETIC(id ".Changed"), label, group,                                                           \
                ImVec4(defaultColor.r / 255.0f, defaultColor.g / 255.0f, defaultColor.b / 255.0f,                     \
                       defaultColor.a / 255.0f),                                                                      \
                defaultColor, supportsAlpha, supportsRainbow, advancedOption                                          \
        }                                                                                                             \
    }

typedef struct {
    const char* cvar;
    const char* valuesCvar;
    const char* rainbowCvar;
    const char* lockedCvar;
    const char* changedCvar;
    std::string label;
    CosmeticGroup group;
    ImVec4 currentColor;
    Color_RGBA8 defaultColor;
    bool supportsAlpha;
    bool supportsRainbow;
    bool advancedOption;
} CosmeticOption;

extern std::map<std::string, CosmeticOption> cosmeticOptions;

inline void ResetColor(CosmeticOption& cosmeticOption) {
    Color_RGBA8 defaultColor = { cosmeticOption.defaultColor.r, cosmeticOption.defaultColor.g,
                                 cosmeticOption.defaultColor.b, cosmeticOption.defaultColor.a };
    cosmeticOption.currentColor.x = defaultColor.r / 255.0f;
    cosmeticOption.currentColor.y = defaultColor.g / 255.0f;
    cosmeticOption.currentColor.z = defaultColor.b / 255.0f;
    cosmeticOption.currentColor.w = defaultColor.a / 255.0f;

    CVarClear(cosmeticOption.changedCvar);
    CVarClear(cosmeticOption.rainbowCvar);
    CVarClear(cosmeticOption.lockedCvar);
    CVarClear(cosmeticOption.valuesCvar);
    CVarClear((std::string(cosmeticOption.valuesCvar) + ".R").c_str());
    CVarClear((std::string(cosmeticOption.valuesCvar) + ".G").c_str());
    CVarClear((std::string(cosmeticOption.valuesCvar) + ".B").c_str());
    CVarClear((std::string(cosmeticOption.valuesCvar) + ".A").c_str());
    CVarClear((std::string(cosmeticOption.valuesCvar) + ".Type").c_str());

    if (cosmeticOption.label == "Bow Body") {
        ResetColor(cosmeticOptions.at("Equipment.BowTips"));
        ResetColor(cosmeticOptions.at("Equipment.BowHandle"));
    } else if (cosmeticOption.label == "Idle Primary") {
        ResetColor(cosmeticOptions.at("Navi.IdleSecondary"));
    } else if (cosmeticOption.label == "Enemy Primary") {
        ResetColor(cosmeticOptions.at("Navi.EnemySecondary"));
    } else if (cosmeticOption.label == "NPC Primary") {
        ResetColor(cosmeticOptions.at("Navi.NPCSecondary"));
    } else if (cosmeticOption.label == "Props Primary") {
        ResetColor(cosmeticOptions.at("Navi.PropsSecondary"));
    } else if (cosmeticOption.label == "Level 1 Secondary") {
        ResetColor(cosmeticOptions.at("SpinAttack.Level1Primary"));
    } else if (cosmeticOption.label == "Level 2 Secondary") {
        ResetColor(cosmeticOptions.at("SpinAttack.Level2Primary"));
    } else if (cosmeticOption.label == "Item Select Color") {
        ResetColor(cosmeticOptions.at("Kaleido.ItemSelB"));
        ResetColor(cosmeticOptions.at("Kaleido.ItemSelC"));
        ResetColor(cosmeticOptions.at("Kaleido.ItemSelD"));
    } else if (cosmeticOption.label == "Equip Select Color") {
        ResetColor(cosmeticOptions.at("Kaleido.EquipSelB"));
        ResetColor(cosmeticOptions.at("Kaleido.EquipSelC"));
        ResetColor(cosmeticOptions.at("Kaleido.EquipSelD"));
    } else if (cosmeticOption.label == "Map Dungeon Color") {
        ResetColor(cosmeticOptions.at("Kaleido.MapSelDunB"));
        ResetColor(cosmeticOptions.at("Kaleido.MapSelDunC"));
        ResetColor(cosmeticOptions.at("Kaleido.MapSelDunD"));
    } else if (cosmeticOption.label == "Quest Status Color") {
        ResetColor(cosmeticOptions.at("Kaleido.QuestStatusB"));
        ResetColor(cosmeticOptions.at("Kaleido.QuestStatusC"));
        ResetColor(cosmeticOptions.at("Kaleido.QuestStatusD"));
    } else if (cosmeticOption.label == "Map Color") {
        ResetColor(cosmeticOptions.at("Kaleido.MapSelectB"));
        ResetColor(cosmeticOptions.at("Kaleido.MapSelectC"));
        ResetColor(cosmeticOptions.at("Kaleido.MapSelectD"));
    } else if (cosmeticOption.label == "Save Color") {
        ResetColor(cosmeticOptions.at("Kaleido.SaveB"));
        ResetColor(cosmeticOptions.at("Kaleido.SaveC"));
        ResetColor(cosmeticOptions.at("Kaleido.SaveD"));
    }
    ShipInit::Init(cosmeticOption.valuesCvar);
}

inline CosmeticOption MakeCosmeticOption(const char* cvar, const char* valuesCvar, const char* rainbowCvar,
                                         const char* lockedCvar, const char* changedCvar, const char* label,
                                         CosmeticGroup group, Color_RGBA8 defaultColor, bool supportsAlpha,
                                         bool supportsRainbow, bool advancedOption) {
    return CosmeticOption{ cvar,
                           valuesCvar,
                           rainbowCvar,
                           lockedCvar,
                           changedCvar,
                           label,
                           group,
                           ImVec4(defaultColor.r / 255.0f, defaultColor.g / 255.0f, defaultColor.b / 255.0f,
                                  defaultColor.a / 255.0f),
                           defaultColor,
                           supportsAlpha,
                           supportsRainbow,
                           advancedOption };
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
static ImGuiTableColumnFlags FlagsCell =
    ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort;

void CosmeticsEditor_RandomizeAll();
void CosmeticsEditor_AutoRandomizeAll();
void CosmeticsEditor_RandomizeGroup(CosmeticGroup group);
void CosmeticsEditor_ResetAll();
void CosmeticsEditor_ResetGroup(CosmeticGroup group);
void ApplyOrResetCustomGfxPatches(bool manualChange = true);
void ScanCustomCosmetics();
bool HasCustomCosmetics();
void DrawCustomCosmetics();
void ApplyCustomCosmetics();
void UpdateCustomCosmeticsRainbow(int hue, float rainbowSpeed, int& index);

class CosmeticsEditorWindow final : public Ship::GuiWindow {
  public:
    using GuiWindow::GuiWindow;

    void InitElement() override;
    void DrawElement() override;
    void ApplyDungeonKeyColors();
    void UpdateElement() override{};
};
#endif //__cplusplus