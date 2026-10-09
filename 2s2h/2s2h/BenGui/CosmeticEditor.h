#ifndef COSMETIC_EDITOR_H
#define COSMETIC_EDITOR_H

#ifdef __cplusplus

#include <map>
#include <string>
#include <imgui.h>

class CosmeticEditorWindow : public Ship::GuiWindow {
  public:
    using GuiWindow::GuiWindow;

    void InitElement() override;
    void DrawElement() override;
    void UpdateElement() override{};
};

extern "C" {
#endif //__cplusplus

void gDPSetPrimColorOverrideEx(Gfx* pkt, u8 m, u8 l, u8 r, u8 g, u8 b, u8 a, const char* cosmeticId, u8 mode,
                               f32 modifier);
void gDPSetPrimColorOverride(Gfx* pkt, u8 m, u8 l, u8 r, u8 g, u8 b, u8 a, const char* cosmeticId);
void gDPSetEnvColorOverrideEx(Gfx* pkt, u8 r, u8 g, u8 b, u8 a, const char* cosmeticId, u8 mode, f32 modifier);
void gDPSetEnvColorOverride(Gfx* pkt, u8 r, u8 g, u8 b, u8 a, const char* cosmeticId);
Gfx* Gfx_DrawTexRectIA8_DropShadowOverride(Gfx* pkt, TexturePtr texture, s16 textureWidth, s16 textureHeight,
                                           s16 rectLeft, s16 rectTop, s16 rectWidth, s16 rectHeight, u16 dsdx, u16 dtdy,
                                           s16 r, s16 g, s16 b, s16 a, const char* cosmeticId);
Gfx* Gfx_DrawRect_DropShadowOverride(Gfx* pkt, s16 rectLeft, s16 rectTop, s16 rectWidth, s16 rectHeight, u16 dsdx,
                                     u16 dtdy, s16 r, s16 g, s16 b, s16 a, const char* cosmeticId);
Gfx* Gfx_DrawTexRectIA16_DropShadowOverride(Gfx* pkt, TexturePtr texture, s16 textureWidth, s16 textureHeight,
                                            s16 rectLeft, s16 rectTop, s16 rectWidth, s16 rectHeight, u16 dsdx,
                                            u16 dtdy, s16 r, s16 g, s16 b, s16 a, const char* cosmeticId);
Gfx* Gfx_DrawTexRectIA8_DropShadowOffsetOverride(Gfx* pkt, TexturePtr texture, s16 textureWidth, s16 textureHeight,
                                                 s16 rectLeft, s16 rectTop, s16 rectWidth, s16 rectHeight, u16 dsdx,
                                                 u16 dtdy, s16 r, s16 g, s16 b, s16 a, s32 masks, s32 rects,
                                                 const char* cosmeticId);
Color_RGBA8 CosmeticEditor_GetChangedColorEx(u8 r, u8 g, u8 b, u8 a, const char* cosmeticId, u8 mode, f32 modifier);
Color_RGBA8 CosmeticEditor_GetChangedColor(u8 r, u8 g, u8 b, u8 a, const char* cosmeticId);

const char* CosmeticEditor_GetDungeonCosmeticId(s32 dungeon);

Color_RGBA8 CosmeticEditor_GetPauseCursorGlow(s16 r, s16 g, s16 b, const s16* targetA, const s16* targetB);

void CosmeticEditor_SetStrayFairyMaterial(s32 area);
const char* CosmeticEditor_MatAnimPrimId(s32 segment);
const char* CosmeticEditor_MatAnimEnvId(void);
void CosmeticEditor_ClearMatAnimCosmetics(void);

void CosmeticEditor_ApplySillyMoonScale(void);
void CosmeticEditor_ApplySillyLimbScale(const char* cvar, Vec3f* jointPos);
void CosmeticEditor_SplitHeldItem(Gfx** dList, const char* cvar);
void CosmeticEditor_DrawSplitHeldItem(struct PlayState* play);
void CosmeticEditor_DrawBackShield(struct PlayState* play, Gfx* dList);

typedef enum {
    COSMETIC_COLOR_MODE_DEFAULT,
    COSMETIC_COLOR_MODE_MULTIPLY,
    COSMETIC_COLOR_MODE_DIVIDE,
    COSMETIC_COLOR_MODE_ADD,
    COSMETIC_COLOR_MODE_SUBTRACT,
    COSMETIC_COLOR_MODE_ROTATE,
} CosmeticEditorColorMode;

typedef enum {
    COSMETICS_GROUP_PLAYER,
    COSMETICS_GROUP_EFFECTS,
    COSMETICS_GROUP_TRAILS,
    COSMETICS_GROUP_HUD,
    COSMETICS_GROUP_BUTTONS,
    COSMETICS_GROUP_MENUS,
    COSMETICS_GROUP_COLLECTIBLES,
    COSMETICS_GROUP_DUNGEONS,
    COSMETICS_GROUP_WORLD,
    COSMETICS_GROUP_MAX,
} CosmeticGroup;

#define CVAR_PREFIX_COSMETIC "gCosmetic"
#define CVAR_COSMETIC(var) CVAR_PREFIX_COSMETIC "." var
#define COSMETIC_ID(id) id
#define CVAR_COSMETIC_COLOR(id) CVAR_COSMETIC(id ".Color")
#define CVAR_COSMETIC_RAINBOW(id) CVAR_COSMETIC(id ".Rainbow")
#define CVAR_COSMETIC_LOCKED(id) CVAR_COSMETIC(id ".Locked")
#define CVAR_COSMETIC_CHANGED(id) CVAR_COSMETIC(id ".Changed")
#define CVAR_SILLY_FLAT_SHADING CVAR_COSMETIC("Silly.FlatShading")

#ifdef __cplusplus

typedef struct {
    const char* cvar;
    const char* valuesCvar;
    const char* rainbowCvar;
    const char* lockedCvar;
    const char* changedCvar;
    const char* label;
    CosmeticGroup group;
    ImVec4 currentColor;
    Color_RGBA8 defaultColor;
    bool supportsAlpha;
    bool supportsRainbow;
    bool advancedOption;
    const char* colorCvar;
    const char* colorChangedCvar;
    const char* name;
    const char* parentName;
    int32_t defaultR;
    int32_t defaultG;
    int32_t defaultB;
    int32_t defaultA;
} CosmeticOption;

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
                           advancedOption,
                           valuesCvar,
                           changedCvar,
                           label,
                           nullptr,
                           defaultColor.r,
                           defaultColor.g,
                           defaultColor.b,
                           defaultColor.a };
}

#define COSMETIC_OPTION(id, label, group, defaultColor, supportsAlpha, supportsRainbow, advancedOption)               \
    {                                                                                                                 \
        id, {                                                                                                         \
            CVAR_COSMETIC(id), CVAR_COSMETIC(id ".Color"), CVAR_COSMETIC(id ".Rainbow"), CVAR_COSMETIC(id ".Locked"), \
                CVAR_COSMETIC(id ".Changed"), label, group,                                                           \
                ImVec4(defaultColor.r / 255.0f, defaultColor.g / 255.0f, defaultColor.b / 255.0f,                     \
                       defaultColor.a / 255.0f),                                                                      \
                defaultColor, supportsAlpha, supportsRainbow, advancedOption, CVAR_COSMETIC(id ".Color"),             \
                CVAR_COSMETIC(id ".Changed"), label, nullptr, defaultColor.r, defaultColor.g, defaultColor.b,         \
                defaultColor.a                                                                                        \
        }                                                                                                             \
    }

extern std::map<std::string, CosmeticOption> cosmeticOptions;
}

void CosmeticEditorResetElement(CosmeticOption& option, bool save = true);
void ScanDynamicCosmetics();
void DrawDynamicCosmetics();
void ApplyDynamicCosmetics();
void RandomizeAllDynamicCosmetics();
void ResetAllDynamicCosmetics();
void SetAllDynamicCosmeticsRainbow(bool enabled);
bool HasCustomCosmetics();
bool UpdateCustomCosmeticsRainbow(int hue, float rainbowSpeed, int& index);
void RefreshDynamicCosmeticsStateIfNeeded();
bool IsCustomModelActiveForCosmeticId(const char* cosmeticId);
bool IsCustomHumanModelActive();
bool IsCustomDekuModelActive();
bool IsCustomGoronModelActive();
bool IsCustomZoraModelActive();
bool IsCustomFierceDeityModelActive();
bool IsCustomKafeiModelActive();
#endif //__cplusplus

#endif