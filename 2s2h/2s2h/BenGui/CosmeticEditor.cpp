#include "2s2h/BenGui/UIWidgets.hpp"
#include "2s2h/BenGui/BenGui.hpp"
#include "CosmeticEditor.h"
#include "CosmeticShading.h"
#include "2s2h/ShipInit.hpp"

#include <cstring>
#include "2s2h/GameInteractor/GameInteractor.h"

extern "C" {
#include "macros.h"
#include "functions.h"
#include "overlays/actors/ovl_En_Elforg/z_en_elforg.h"
#include "objects/object_link_boy/object_link_boy.h"
#include "objects/object_link_child/object_link_child.h"
extern PlayState* gPlayState;

void ResourceMgr_PatchGfxByName(const char* path, const char* patchName, int index, Gfx instruction);
void ResourceMgr_UnpatchGfxByName(const char* path, const char* patchName);
Gfx* ResourceMgr_LoadGfxByName(const char* path);
Gfx* Gfx_DrawTexRectIA8_DropShadow(Gfx* gfx, TexturePtr texture, s16 textureWidth, s16 textureHeight, s16 rectLeft,
                                   s16 rectTop, s16 rectWidth, s16 rectHeight, u16 dsdx, u16 dtdy, s16 r, s16 g, s16 b,
                                   s16 a);
Gfx* Gfx_DrawRect_DropShadow(Gfx* gfx, s16 rectLeft, s16 rectTop, s16 rectWidth, s16 rectHeight, u16 dsdx, u16 dtdy,
                             s16 r, s16 g, s16 b, s16 a);
Gfx* Gfx_DrawTexRectIA16_DropShadow(Gfx* gfx, TexturePtr texture, s16 textureWidth, s16 textureHeight, s16 rectLeft,
                                    s16 rectTop, s16 rectWidth, s16 rectHeight, u16 dsdx, u16 dtdy, s16 r, s16 g, s16 b,
                                    s16 a);
Gfx* Gfx_DrawTexRectIA8_DropShadowOffset(Gfx* gfx, TexturePtr texture, s16 textureWidth, s16 textureHeight,
                                         s16 rectLeft, s16 rectTop, s16 rectWidth, s16 rectHeight, u16 dsdx, u16 dtdy,
                                         s16 r, s16 g, s16 b, s16 a, s32 masks, s32 rects);
void Gfx_TextureCacheDelete(const uint8_t* texAddr);
}

Color_RGBA8 ColorRGBA8(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    Color_RGBA8 color = { r, g, b, a };
    return color;
}

static const std::map<CosmeticGroup, const char*> sCosmeticGroupLabels = {
    { COSMETICS_GROUP_PLAYER, "Player" },
    { COSMETICS_GROUP_EFFECTS, "Effects" },
    { COSMETICS_GROUP_TRAILS, "Trails" },
    { COSMETICS_GROUP_HUD, "HUD" },
    { COSMETICS_GROUP_BUTTONS, "Buttons" },
    { COSMETICS_GROUP_MENUS, "Menus" },
    { COSMETICS_GROUP_COLLECTIBLES, "Collectibles" },
    { COSMETICS_GROUP_DUNGEONS, "Dungeons" },
    { COSMETICS_GROUP_WORLD, "World" },
};

// clang-format off
std::map<std::string, CosmeticOption> cosmeticOptions = {
    COSMETIC_OPTION("HUD.Hearts",                   "Hearts",                           COSMETICS_GROUP_HUD,                ColorRGBA8(255,  70,  50, 255), false, true, false),
    COSMETIC_OPTION("HUD.HeartsDoubleDefense",      "Double Defense Hearts",            COSMETICS_GROUP_HUD,                ColorRGBA8(200,   0,   0, 255), false, true, false),
    COSMETIC_OPTION("HUD.Magic",                    "Magic",                            COSMETICS_GROUP_HUD,                ColorRGBA8(  0, 200,   0, 255), false, true, false),
    COSMETIC_OPTION("HUD.MagicChateau",             "Chateau Romani Magic",             COSMETICS_GROUP_HUD,                ColorRGBA8(  0,   0, 200, 255), false, true, false),
    COSMETIC_OPTION("HUD.SmallKey",                 "Small Key",                        COSMETICS_GROUP_HUD,                ColorRGBA8(  0, 200, 230, 255), false, true, false),
    COSMETIC_OPTION("HUD.RupeeIcon",                "Rupee Icon",                       COSMETICS_GROUP_HUD,                ColorRGBA8(200, 255, 100, 255), false, true, false),
    COSMETIC_OPTION("HUD.Minimap",                  "Minimap",                          COSMETICS_GROUP_HUD,                ColorRGBA8(  0, 255, 255, 160), false, true, false),
    COSMETIC_OPTION("HUD.TargetReticle",            "Lock-On Reticle",                  COSMETICS_GROUP_HUD,                ColorRGBA8(255, 255,   0, 255), false, true, false),
    COSMETIC_OPTION("HUD.TargetArrow",              "Lock-On Arrow",                    COSMETICS_GROUP_HUD,                ColorRGBA8(255, 255,   0, 255), false, true, false),
    COSMETIC_OPTION("HUD.LensOverlay",              "Lens Of Truth Overlay",            COSMETICS_GROUP_HUD,                ColorRGBA8( 74,   0,   0, 255), false, true, false),
    COSMETIC_OPTION("Effects.SpinSlashCharge",      "Spin Slash Charge",                COSMETICS_GROUP_EFFECTS,            ColorRGBA8(170, 255, 255, 255), false, true, false),
    COSMETIC_OPTION("Effects.SpinSlashBurst",       "Spin Slash Burst",                 COSMETICS_GROUP_EFFECTS,            ColorRGBA8(170, 255, 255, 255), false, true, false),
    COSMETIC_OPTION("Effects.GreatSpinCharge",      "Great Spin Charge",                COSMETICS_GROUP_EFFECTS,            ColorRGBA8(255, 255, 170, 255), false, true, false),
    COSMETIC_OPTION("Effects.GreatSpinBurst",       "Great Spin Burst",                 COSMETICS_GROUP_EFFECTS,            ColorRGBA8(255, 255, 170, 255), false, true, false),
    COSMETIC_OPTION("Effects.FireArrowPrim",        "Fire Arrow Primary",               COSMETICS_GROUP_EFFECTS,            ColorRGBA8(255, 200,   0, 255), false, true, false),
    COSMETIC_OPTION("Effects.FireArrowSec",         "Fire Arrow Secondary",             COSMETICS_GROUP_EFFECTS,            ColorRGBA8(255,   0,   0, 128), false, true, false),
    COSMETIC_OPTION("Effects.IceArrowPrim",         "Ice Arrow Primary",                COSMETICS_GROUP_EFFECTS,            ColorRGBA8(170, 255, 255, 255), false, true, false),
    COSMETIC_OPTION("Effects.IceArrowSec",          "Ice Arrow Secondary",              COSMETICS_GROUP_EFFECTS,            ColorRGBA8(  0,   0, 255, 128), false, true, false),
    COSMETIC_OPTION("Effects.LightArrowPrim",       "Light Arrow Primary",              COSMETICS_GROUP_EFFECTS,            ColorRGBA8(255, 255, 170, 255), false, true, false),
    COSMETIC_OPTION("Effects.LightArrowSec",        "Light Arrow Secondary",            COSMETICS_GROUP_EFFECTS,            ColorRGBA8(255, 255,   0, 128), false, true, false),
    COSMETIC_OPTION("Effects.SkullBubbleFlame",     "Skull Bubble Flame",               COSMETICS_GROUP_EFFECTS,            ColorRGBA8(  0,   0, 255, 255), false, true, false),
    COSMETIC_OPTION("Trails.KokiriSwordTrail",      "Kokiri Sword Trail",               COSMETICS_GROUP_TRAILS,             ColorRGBA8(255, 255, 255, 255), false, true, false),
    COSMETIC_OPTION("Trails.RazorSwordTrail",       "Razor Sword Trail",                COSMETICS_GROUP_TRAILS,             ColorRGBA8(255, 255, 255, 255), false, true, false),
    COSMETIC_OPTION("Trails.GildedSwordTrail",      "Gilded Sword Trail",               COSMETICS_GROUP_TRAILS,             ColorRGBA8(255, 255, 255, 255), false, true, false),
    COSMETIC_OPTION("Trails.GreatFairySwordTrail",  "Great Fairy Sword Trail",          COSMETICS_GROUP_TRAILS,             ColorRGBA8(255, 255, 255, 255), false, true, false),
    COSMETIC_OPTION("Trails.FierceDeitySwordTrail", "Fierce Deity Sword Trail",         COSMETICS_GROUP_TRAILS,             ColorRGBA8(255, 255, 255, 255), false, true, false),
    COSMETIC_OPTION("Trails.DekuStickTrail",        "Deku Stick Trail",                 COSMETICS_GROUP_TRAILS,             ColorRGBA8(255, 255, 255, 255), false, true, false),
    COSMETIC_OPTION("Trails.DekuSpinTrail",         "Deku Spin Trail",                  COSMETICS_GROUP_TRAILS,             ColorRGBA8(255, 255, 255, 255), false, true, false),
    COSMETIC_OPTION("Trails.ZoraPunchTrail",        "Zora Punch Trail",                 COSMETICS_GROUP_TRAILS,             ColorRGBA8(255, 255, 255, 255), false, true, false),
    COSMETIC_OPTION("Trails.ZoraKickTrail",         "Zora Kick Trail",                  COSMETICS_GROUP_TRAILS,             ColorRGBA8(255, 255, 255, 255), false, true, false),
    COSMETIC_OPTION("Trails.ZoraBoomerangTrail",    "Zora Boomerang Trail",             COSMETICS_GROUP_TRAILS,             ColorRGBA8(255, 255, 100, 255), false, true, false),
    COSMETIC_OPTION("Buttons.B",                    "B",                                COSMETICS_GROUP_BUTTONS,            ColorRGBA8(100, 255, 120, 255), false, true, false),
    COSMETIC_OPTION("Buttons.A",                    "A",                                COSMETICS_GROUP_BUTTONS,            ColorRGBA8(100, 200, 255, 255), false, true, false),
    COSMETIC_OPTION("Buttons.CLeft",                "C Left",                           COSMETICS_GROUP_BUTTONS,            ColorRGBA8(255, 240,   0, 255), false, true, false),
    COSMETIC_OPTION("Buttons.CDown",                "C Down",                           COSMETICS_GROUP_BUTTONS,            ColorRGBA8(255, 240,   0, 255), false, true, false),
    COSMETIC_OPTION("Buttons.CRight",               "C Right",                          COSMETICS_GROUP_BUTTONS,            ColorRGBA8(255, 240,   0, 255), false, true, false),
    COSMETIC_OPTION("Buttons.DPad",                 "D Pad",                            COSMETICS_GROUP_BUTTONS,            ColorRGBA8(255, 255, 255, 255), false, true, false),
    COSMETIC_OPTION("Buttons.Start",                "Start",                            COSMETICS_GROUP_BUTTONS,            ColorRGBA8(255, 130,  60, 255), false, true, false),
    COSMETIC_OPTION("Menus.FileWindow",             "File Select Window",               COSMETICS_GROUP_MENUS,              ColorRGBA8(100, 150, 255, 255), false, true, false),
    COSMETIC_OPTION("Menus.FilePlates",             "File Select Plates",               COSMETICS_GROUP_MENUS,              ColorRGBA8(100, 150, 255, 255), false, true, false),
    COSMETIC_OPTION("Menus.PausePanels",            "Pause Menu Panels",                COSMETICS_GROUP_MENUS,              ColorRGBA8(180, 180, 120, 255), false, true, false),
    COSMETIC_OPTION("Menus.PauseCursor",            "Pause Menu Cursor",                COSMETICS_GROUP_MENUS,              ColorRGBA8(255, 255, 255, 255), false, true, false),
    COSMETIC_OPTION("Menus.SceneTitleCard",         "Scene Title Card",                 COSMETICS_GROUP_MENUS,              ColorRGBA8(140,  40, 160, 255), false, true, false),
    COSMETIC_OPTION("Collectibles.RupeeGreen",      "Green Rupee",                      COSMETICS_GROUP_COLLECTIBLES,       ColorRGBA8( 82, 255, 107, 255), false, true, false),
    COSMETIC_OPTION("Collectibles.RupeeBlue",       "Blue Rupee",                       COSMETICS_GROUP_COLLECTIBLES,       ColorRGBA8(132, 181, 239, 255), false, true, false),
    COSMETIC_OPTION("Collectibles.RupeeRed",        "Red Rupee",                        COSMETICS_GROUP_COLLECTIBLES,       ColorRGBA8(239, 107, 107, 255), false, true, false),
    COSMETIC_OPTION("Collectibles.RupeePurple",     "Purple Rupee",                     COSMETICS_GROUP_COLLECTIBLES,       ColorRGBA8(255,  99, 231, 255), false, true, false),
    COSMETIC_OPTION("Collectibles.RupeeOrange",     "Orange Rupee",                     COSMETICS_GROUP_COLLECTIBLES,       ColorRGBA8(247, 148,  33, 255), false, true, false),
    COSMETIC_OPTION("Collectibles.RupeeSilver",     "Silver Rupee",                     COSMETICS_GROUP_COLLECTIBLES,       ColorRGBA8(189, 189, 198, 255), false, true, false),
    COSMETIC_OPTION("World.TatlInner",              "Tatl Inner",                       COSMETICS_GROUP_WORLD,              ColorRGBA8(255, 255, 230, 255), false, true, false),
    COSMETIC_OPTION("World.TatlOuter",              "Tatl Outer",                       COSMETICS_GROUP_WORLD,              ColorRGBA8(220, 160,  80, 255), false, true, false),
    COSMETIC_OPTION("World.RaceDogBlue",            "Blue Racedog",                     COSMETICS_GROUP_WORLD,              ColorRGBA8( 79,  79, 143, 255), false, true, false),
    COSMETIC_OPTION("World.GoldSkulltula",          "Gold Skulltula",                   COSMETICS_GROUP_WORLD,              ColorRGBA8(255, 255,   0, 255), false, true, false),
    COSMETIC_OPTION("World.TingleBalloon",          "Tingle's Balloon",                 COSMETICS_GROUP_WORLD,              ColorRGBA8(255,  60, 100, 255), false, true, false),
    COSMETIC_OPTION("World.EponaCoat",              "Epona's Coat",                     COSMETICS_GROUP_WORLD,              ColorRGBA8(214,  82,   8, 255), false, true, false),
    COSMETIC_OPTION("Player.HumanTunic",            "Human Tunic",                      COSMETICS_GROUP_PLAYER,             ColorRGBA8( 30, 105,  27, 255), false, true, false),
    COSMETIC_OPTION("Player.HumanHair",             "Human Hair",                       COSMETICS_GROUP_PLAYER,             ColorRGBA8(255, 240,   0, 255), false, true, false),
    COSMETIC_OPTION("Player.DekuTunic",             "Deku Tunic",                       COSMETICS_GROUP_PLAYER,             ColorRGBA8( 30, 105,  27, 255), false, true, false),
    COSMETIC_OPTION("Player.DekuHair",              "Deku Hair",                        COSMETICS_GROUP_PLAYER,             ColorRGBA8(255, 240,   0, 255), false, true, false),
    COSMETIC_OPTION("Player.GoronTunic",            "Goron Tunic",                      COSMETICS_GROUP_PLAYER,             ColorRGBA8( 30, 105,  27, 255), false, true, false),
    COSMETIC_OPTION("Player.ZoraTunic",             "Zora Tunic",                       COSMETICS_GROUP_PLAYER,             ColorRGBA8( 30, 105,  27, 255), false, true, false),
    COSMETIC_OPTION("Player.FierceDeityTunic",      "Fierce Deity Tunic",               COSMETICS_GROUP_PLAYER,             ColorRGBA8(174, 208, 207, 255), false, true, false),
    COSMETIC_OPTION("Player.KafeiHair",             "Kafei Hair",                       COSMETICS_GROUP_PLAYER,             ColorRGBA8( 64,   0, 163, 255), false, true, false),
    COSMETIC_OPTION("Player.MirrorShield",          "Mirror Shield",                    COSMETICS_GROUP_PLAYER,             ColorRGBA8(128,  90,  20, 255), false, true, false),
    COSMETIC_OPTION("Player.MirrorShieldMirror",    "Mirror Shield Reflection",         COSMETICS_GROUP_PLAYER,             ColorRGBA8(120, 120, 120, 255), false, true, false),
    COSMETIC_OPTION("Dungeon.Woodfall",             "Woodfall",                         COSMETICS_GROUP_DUNGEONS,           ColorRGBA8(255, 170, 246, 255), false, true, false),
    COSMETIC_OPTION("Dungeon.Snowhead",             "Snowhead",                         COSMETICS_GROUP_DUNGEONS,           ColorRGBA8(116, 226,  61, 255), false, true, false),
    COSMETIC_OPTION("Dungeon.GreatBay",             "Great Bay",                        COSMETICS_GROUP_DUNGEONS,           ColorRGBA8(143, 103, 226, 255), false, true, false),
    COSMETIC_OPTION("Dungeon.StoneTower",           "Stone Tower",                      COSMETICS_GROUP_DUNGEONS,           ColorRGBA8(226, 221,   0, 255), false, true, false),
};
// clang-format on

static void CosmeticEditorInitializeCompatFields() {
    static bool initialized = false;
    if (initialized) {
        return;
    }

    for (auto& [id, option] : cosmeticOptions) {
        option.parentName = sCosmeticGroupLabels.at(option.group);
    }

    initialized = true;
}

static CosmeticOption& CosmeticEditor_GetOptionMutable(const char* id) {
    CosmeticEditorInitializeCompatFields();
    return cosmeticOptions.at(id);
}

static CosmeticOption& kHumanTunicOption = cosmeticOptions.at("Player.HumanTunic");
static CosmeticOption& kHumanHairOption = cosmeticOptions.at("Player.HumanHair");
static CosmeticOption& kDekuTunicOption = cosmeticOptions.at("Player.DekuTunic");
static CosmeticOption& kDekuHairOption = cosmeticOptions.at("Player.DekuHair");
static CosmeticOption& kKafeiHairOption = cosmeticOptions.at("Player.KafeiHair");
static CosmeticOption& kGoronTunicOption = cosmeticOptions.at("Player.GoronTunic");
static CosmeticOption& kZoraTunicOption = cosmeticOptions.at("Player.ZoraTunic");
static CosmeticOption& kFierceDeityTunicOption = cosmeticOptions.at("Player.FierceDeityTunic");
static CosmeticOption& kHeartsOption = cosmeticOptions.at("HUD.Hearts");
static CosmeticOption& kMagicOption = cosmeticOptions.at("HUD.Magic");
static CosmeticOption& kMirrorShieldOption = cosmeticOptions.at("Player.MirrorShield");
static CosmeticOption& kMirrorShieldMirrorOption = cosmeticOptions.at("Player.MirrorShieldMirror");

static bool CosmeticEditorIsSuppressed(const char* cosmeticId) {
    return IsCustomModelActiveForCosmeticId(cosmeticId);
}

static bool CosmeticEditorIsSuppressed(const CosmeticOption& option) {
    if (&option == &kHumanTunicOption || &option == &kHumanHairOption) {
        return IsCustomModelActiveForCosmeticId("Player.HumanTunic");
    }
    if (&option == &kDekuTunicOption || &option == &kDekuHairOption) {
        return IsCustomModelActiveForCosmeticId("Player.DekuTunic");
    }
    if (&option == &kGoronTunicOption) {
        return IsCustomModelActiveForCosmeticId("Player.GoronTunic");
    }
    if (&option == &kZoraTunicOption) {
        return IsCustomModelActiveForCosmeticId("Player.ZoraTunic");
    }
    if (&option == &kFierceDeityTunicOption) {
        return IsCustomModelActiveForCosmeticId("Player.FierceDeityTunic");
    }
    if (&option == &kKafeiHairOption) {
        return IsCustomModelActiveForCosmeticId("Player.KafeiHair");
    }

    return false;
}

Gfx backToWhite[] = {
    gsDPSetPrimColor(0, 0x80, 255, 255, 255, 255),
    gsDPPipeSync(),
    gsSPEndDisplayList(),
};

Gfx enableGrayscale[] = {
    gsSPGrayscale(1),
    gsDPPipeSync(),
    gsSPEndDisplayList(),
};

Gfx disableGrayscale[] = {
    gsSPGrayscale(0),
    gsSPEndDisplayList(),
};

extern "C" Color_RGBA8 CosmeticEditor_GetChangedColorEx(u8 r, u8 g, u8 b, u8 a, const char* cosmeticId, u8 mode,
                                                        f32 modifier) {
    const CosmeticOption& option = CosmeticEditor_GetOptionMutable(cosmeticId);

    Color_RGBA8 returnedColor;

    if (CosmeticEditorIsSuppressed(cosmeticId)) {
        returnedColor.r = r;
        returnedColor.g = g;
        returnedColor.b = b;
        returnedColor.a = a;
    } else if (CVarGetInteger(option.changedCvar, false)) {
        Color_RGBA8 changedColor = CVarGetColor(option.valuesCvar, option.defaultColor);
        returnedColor.r = static_cast<uint8_t>(changedColor.r);
        returnedColor.g = static_cast<uint8_t>(changedColor.g);
        returnedColor.b = static_cast<uint8_t>(changedColor.b);
        returnedColor.a = static_cast<uint8_t>(option.supportsAlpha ? changedColor.a : a);

        switch (mode) {
            case COSMETIC_COLOR_MODE_DEFAULT:
                break;
            case COSMETIC_COLOR_MODE_MULTIPLY:
                returnedColor.r = CLAMP_MAX(returnedColor.r * modifier, 255);
                returnedColor.g = CLAMP_MAX(returnedColor.g * modifier, 255);
                returnedColor.b = CLAMP_MAX(returnedColor.b * modifier, 255);
                break;
            case COSMETIC_COLOR_MODE_DIVIDE:
                if (modifier != 0) {
                    returnedColor.r = CLAMP(returnedColor.r / modifier, 0, 255);
                    returnedColor.g = CLAMP(returnedColor.g / modifier, 0, 255);
                    returnedColor.b = CLAMP(returnedColor.b / modifier, 0, 255);
                }
                break;
            case COSMETIC_COLOR_MODE_ADD:
                returnedColor.r = CLAMP(returnedColor.r + modifier, 0, 255);
                returnedColor.g = CLAMP(returnedColor.g + modifier, 0, 255);
                returnedColor.b = CLAMP(returnedColor.b + modifier, 0, 255);
                break;
            case COSMETIC_COLOR_MODE_SUBTRACT:
                returnedColor.r = CLAMP(returnedColor.r - modifier, 0, 255);
                returnedColor.g = CLAMP(returnedColor.g - modifier, 0, 255);
                returnedColor.b = CLAMP(returnedColor.b - modifier, 0, 255);
                break;
            case COSMETIC_COLOR_MODE_ROTATE: {
                // Rotate hue by modifier degrees (0-360). Example: green (0,200,0) + 120 => blue (0,0,200)
                float rf = returnedColor.r / 255.0f;
                float gf = returnedColor.g / 255.0f;
                float bf = returnedColor.b / 255.0f;

                float maxC = std::max({ rf, gf, bf });
                float minC = std::min({ rf, gf, bf });
                float delta = maxC - minC;

                float h = 0.0f, s = 0.0f, v = maxC;
                if (delta > 0.0f) {
                    s = delta / maxC;
                    if (maxC == rf) {
                        h = 60.0f * fmodf((gf - bf) / delta, 6.0f);
                    } else if (maxC == gf) {
                        h = 60.0f * ((bf - rf) / delta + 2.0f);
                    } else {
                        h = 60.0f * ((rf - gf) / delta + 4.0f);
                    }
                    if (h < 0.0f)
                        h += 360.0f;
                }

                h = fmodf(h + modifier, 360.0f);
                if (h < 0.0f)
                    h += 360.0f;

                float c = v * s;
                float x = c * (1.0f - fabsf(fmodf(h / 60.0f, 2.0f) - 1.0f));
                float m = v - c;
                float r1 = 0.0f, g1 = 0.0f, b1 = 0.0f;
                if (h < 60.0f) {
                    r1 = c;
                    g1 = x;
                    b1 = 0;
                } else if (h < 120.0f) {
                    r1 = x;
                    g1 = c;
                    b1 = 0;
                } else if (h < 180.0f) {
                    r1 = 0;
                    g1 = c;
                    b1 = x;
                } else if (h < 240.0f) {
                    r1 = 0;
                    g1 = x;
                    b1 = c;
                } else if (h < 300.0f) {
                    r1 = x;
                    g1 = 0;
                    b1 = c;
                } else {
                    r1 = c;
                    g1 = 0;
                    b1 = x;
                }

                returnedColor.r = static_cast<uint8_t>((r1 + m) * 255.0f);
                returnedColor.g = static_cast<uint8_t>((g1 + m) * 255.0f);
                returnedColor.b = static_cast<uint8_t>((b1 + m) * 255.0f);
                break;
            }
        }
    } else {
        returnedColor.r = r;
        returnedColor.g = g;
        returnedColor.b = b;
        returnedColor.a = a;
    }

    return returnedColor;
}

extern "C" Color_RGBA8 CosmeticEditor_GetChangedColor(u8 r, u8 g, u8 b, u8 a, const char* cosmeticId) {
    return CosmeticEditor_GetChangedColorEx(r, g, b, a, cosmeticId, COSMETIC_COLOR_MODE_DEFAULT, 0.0f);
}

extern "C" const char* CosmeticEditor_GetDungeonCosmeticId(s32 dungeon) {
    static const char* dungeonCosmeticIds[] = {
        COSMETIC_ID("Dungeon.Woodfall"),
        COSMETIC_ID("Dungeon.Snowhead"),
        COSMETIC_ID("Dungeon.GreatBay"),
        COSMETIC_ID("Dungeon.StoneTower"),
    };

    if (dungeon < 0 || dungeon >= (s32)ARRAY_COUNT(dungeonCosmeticIds)) {
        return nullptr;
    }

    return dungeonCosmeticIds[dungeon];
}

extern "C" Color_RGBA8 CosmeticEditor_GetPauseCursorGlow(s16 r, s16 g, s16 b, const s16* targetA, const s16* targetB) {
    Color_RGBA8 envColor = { (u8)r, (u8)g, (u8)b, 255 };

    if (!CVarGetInteger(CVAR_COSMETIC_CHANGED("Menus.PauseCursor"), 0)) {
        return envColor;
    }

    s16 targetMax = MAX(MAX(MAX(targetA[0], targetA[1]), targetA[2]), MAX(MAX(targetB[0], targetB[1]), targetB[2]));
    if (targetMax <= 0) {
        return envColor;
    }

    Color_RGBA8 glow = CosmeticEditor_GetChangedColorEx(r, g, b, 255, COSMETIC_ID("Menus.PauseCursor"),
                                                        COSMETIC_COLOR_MODE_ADD, 70.0f);
    f32 pulse = CLAMP_MAX((f32)MAX(MAX(r, g), b) / targetMax, 1.0f);

    envColor.r = glow.r * pulse;
    envColor.g = glow.g * pulse;
    envColor.b = glow.b * pulse;

    return envColor;
}

static const char* sStrayFairyCosmeticId = nullptr;

extern "C" void CosmeticEditor_SetStrayFairyMaterial(s32 area) {
    sStrayFairyCosmeticId = CosmeticEditor_GetDungeonCosmeticId(area - STRAY_FAIRY_AREA_WOODFALL);
}

extern "C" const char* CosmeticEditor_MatAnimPrimId(s32 segment) {
    return segment == 9 ? sStrayFairyCosmeticId : nullptr;
}

extern "C" const char* CosmeticEditor_MatAnimEnvId(void) {
    return sStrayFairyCosmeticId;
}

extern "C" void CosmeticEditor_ClearMatAnimCosmetics(void) {
    sStrayFairyCosmeticId = nullptr;
}

extern "C" void gDPSetEnvColorOverrideEx(Gfx* pkt, u8 r, u8 g, u8 b, u8 a, const char* cosmeticId, u8 mode,
                                         f32 modifier) {
    if (cosmeticId == nullptr) {
        gDPSetEnvColor(pkt, r, g, b, a);
        return;
    }

    Color_RGBA8 setColor = CosmeticEditor_GetChangedColorEx(r, g, b, a, cosmeticId, mode, modifier);
    gDPSetEnvColor(pkt, setColor.r, setColor.g, setColor.b, a);
}

extern "C" void gDPSetEnvColorOverride(Gfx* pkt, u8 r, u8 g, u8 b, u8 a, const char* cosmeticId) {
    return gDPSetEnvColorOverrideEx(pkt, r, g, b, a, cosmeticId, COSMETIC_COLOR_MODE_DEFAULT, 0.0f);
}

extern "C" void gDPSetPrimColorOverrideEx(Gfx* pkt, u8 m, u8 l, u8 r, u8 g, u8 b, u8 a, const char* cosmeticId, u8 mode,
                                          f32 modifier) {
    if (cosmeticId == nullptr) {
        gDPSetPrimColor(pkt, m, l, r, g, b, a);
        return;
    }

    Color_RGBA8 setColor = CosmeticEditor_GetChangedColorEx(r, g, b, a, cosmeticId, mode, modifier);
    gDPSetPrimColor(pkt, m, l, setColor.r, setColor.g, setColor.b, a);
}

extern "C" void gDPSetPrimColorOverride(Gfx* pkt, u8 m, u8 l, u8 r, u8 g, u8 b, u8 a, const char* cosmeticId) {
    return gDPSetPrimColorOverrideEx(pkt, m, l, r, g, b, a, cosmeticId, COSMETIC_COLOR_MODE_DEFAULT, 0.0f);
}

extern "C" Gfx* Gfx_DrawTexRectIA8_DropShadowOverride(Gfx* pkt, TexturePtr texture, s16 textureWidth, s16 textureHeight,
                                                      s16 rectLeft, s16 rectTop, s16 rectWidth, s16 rectHeight,
                                                      u16 dsdx, u16 dtdy, s16 r, s16 g, s16 b, s16 a,
                                                      const char* cosmeticId) {
    Color_RGBA8 setColor = CosmeticEditor_GetChangedColor(r, g, b, a, cosmeticId);
    return Gfx_DrawTexRectIA8_DropShadow(pkt, texture, textureWidth, textureHeight, rectLeft, rectTop, rectWidth,
                                         rectHeight, dsdx, dtdy, setColor.r, setColor.g, setColor.b, a);
}

extern "C" Gfx* Gfx_DrawRect_DropShadowOverride(Gfx* pkt, s16 rectLeft, s16 rectTop, s16 rectWidth, s16 rectHeight,
                                                u16 dsdx, u16 dtdy, s16 r, s16 g, s16 b, s16 a,
                                                const char* cosmeticId) {
    Color_RGBA8 setColor = CosmeticEditor_GetChangedColor(r, g, b, a, cosmeticId);
    return Gfx_DrawRect_DropShadow(pkt, rectLeft, rectTop, rectWidth, rectHeight, dsdx, dtdy, setColor.r, setColor.g,
                                   setColor.b, a);
}
extern "C" Gfx* Gfx_DrawTexRectIA16_DropShadowOverride(Gfx* pkt, TexturePtr texture, s16 textureWidth,
                                                       s16 textureHeight, s16 rectLeft, s16 rectTop, s16 rectWidth,
                                                       s16 rectHeight, u16 dsdx, u16 dtdy, s16 r, s16 g, s16 b, s16 a,
                                                       const char* cosmeticId) {
    Color_RGBA8 setColor = CosmeticEditor_GetChangedColor(r, g, b, a, cosmeticId);
    return Gfx_DrawTexRectIA16_DropShadow(pkt, texture, textureWidth, textureHeight, rectLeft, rectTop, rectWidth,
                                          rectHeight, dsdx, dtdy, setColor.r, setColor.g, setColor.b, a);
}
extern "C" Gfx* Gfx_DrawTexRectIA8_DropShadowOffsetOverride(Gfx* pkt, TexturePtr texture, s16 textureWidth,
                                                            s16 textureHeight, s16 rectLeft, s16 rectTop, s16 rectWidth,
                                                            s16 rectHeight, u16 dsdx, u16 dtdy, s16 r, s16 g, s16 b,
                                                            s16 a, s32 masks, s32 rects, const char* cosmeticId) {
    Color_RGBA8 setColor = CosmeticEditor_GetChangedColor(r, g, b, a, cosmeticId);
    return Gfx_DrawTexRectIA8_DropShadowOffset(pkt, texture, textureWidth, textureHeight, rectLeft, rectTop, rectWidth,
                                               rectHeight, dsdx, dtdy, setColor.r, setColor.g, setColor.b, a, masks,
                                               rects);
}

const char* kCosmeticRainbowSyncCvar = CVAR_COSMETIC("RainbowSync");
const char* kCosmeticRainbowSpeedCvar = CVAR_COSMETIC("RainbowSpeed");
const char* kCosmeticRandomizeOnSeedGenCvar = CVAR_COSMETIC("RandomizeOnSeedGen");
int sCosmeticRainbowHue = 0;

void CosmeticEditorResetSillyOptions();

void CosmeticEditorSave() {
    Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
}

Color_RGBA8 CosmeticEditorGetDefaultColor(const CosmeticOption& option) {
    return option.defaultColor;
}

void CosmeticEditorRefreshElement(const CosmeticOption& option) {
    ShipInit::Init(option.valuesCvar);
    ShipInit::Init(option.changedCvar);
    ShipInit::Init(option.rainbowCvar);
}

void CosmeticEditorSetRainbowEnabled(const CosmeticOption& option, bool enabled, bool save = true) {
    CVarSetInteger(option.rainbowCvar, enabled);
    if (enabled) {
        CVarSetInteger(option.changedCvar, 1);
    }
    CosmeticEditorRefreshElement(option);
    if (save) {
        CosmeticEditorSave();
    }
}

void CosmeticEditorSetLocked(const CosmeticOption& option, bool locked, bool save = true) {
    CVarSetInteger(option.lockedCvar, locked);
    if (save) {
        CosmeticEditorSave();
    }
}

void CosmeticEditorResetElement(CosmeticOption& option, bool save) {
    if (CosmeticEditorIsSuppressed(option)) {
        return;
    }
    if (CVarGetInteger(option.lockedCvar, 0)) {
        return;
    }
    CVarClear(option.valuesCvar);
    CVarClear(option.changedCvar);
    CVarClear(option.rainbowCvar);
    option.currentColor = ImVec4(option.defaultColor.r / 255.0f, option.defaultColor.g / 255.0f,
                                 option.defaultColor.b / 255.0f, option.defaultColor.a / 255.0f);
    CosmeticEditorRefreshElement(option);
    if (save) {
        CosmeticEditorSave();
    }
}

void CosmeticEditorRandomizeElement(CosmeticOption& option, bool save = true) {
    if (CosmeticEditorIsSuppressed(option)) {
        return;
    }
    if (CVarGetInteger(option.lockedCvar, 0)) {
        return;
    }
    ImVec4 colorVec = GetRandomValue();
    Color_RGBA8 color = { static_cast<uint8_t>(colorVec.x * 255.0f), static_cast<uint8_t>(colorVec.y * 255.0f),
                          static_cast<uint8_t>(colorVec.z * 255.0f),
                          static_cast<uint8_t>(option.supportsAlpha ? option.currentColor.w * 255.0f : 255) };
    option.currentColor = ImVec4(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f);
    CVarSetColor(option.valuesCvar, color);
    CVarSetInteger(option.changedCvar, 1);
    CVarSetInteger(option.rainbowCvar, 0);
    CosmeticEditorRefreshElement(option);
    if (save) {
        CosmeticEditorSave();
    }
}

void CosmeticEditorRandomizeAllElements() {
    for (auto& [id, option] : cosmeticOptions) {
        CosmeticEditorRandomizeElement(option, false);
    }
    RandomizeAllDynamicCosmetics();
    CosmeticEditorSave();
}

void CosmeticEditorResetAllElements() {
    for (auto& [id, option] : cosmeticOptions) {
        CosmeticEditorResetElement(option, false);
    }
    ResetAllDynamicCosmetics();
    CosmeticEditorResetSillyOptions();
    CosmeticEditorSave();
}

void CosmeticEditorSetAllLocked(bool locked) {
    for (auto& [id, option] : cosmeticOptions) {
        if (CosmeticEditorIsSuppressed(option)) {
            continue;
        }
        CosmeticEditorSetLocked(option, locked, false);
    }
    CosmeticEditorSave();
}

void CosmeticEditorSetAllRainbow(bool enabled) {
    for (auto& [id, option] : cosmeticOptions) {
        if (CosmeticEditorIsSuppressed(option) || !option.supportsRainbow || CVarGetInteger(option.lockedCvar, 0)) {
            continue;
        }
        CosmeticEditorSetRainbowEnabled(option, enabled, false);
    }
    SetAllDynamicCosmeticsRainbow(enabled);
    CosmeticEditorSave();
}

bool CosmeticEditorMatchesGroup(const CosmeticOption& option, CosmeticGroup group) {
    return option.group == group;
}

void CosmeticEditorRandomizeGroup(CosmeticGroup group) {
    for (auto& [id, option] : cosmeticOptions) {
        if (CosmeticEditorMatchesGroup(option, group)) {
            CosmeticEditorRandomizeElement(option, false);
        }
    }
    CosmeticEditorSave();
}

void CosmeticEditorResetGroup(CosmeticGroup group) {
    for (auto& [id, option] : cosmeticOptions) {
        if (CosmeticEditorMatchesGroup(option, group)) {
            CosmeticEditorResetElement(option, false);
        }
    }
    CosmeticEditorSave();
}

static const char* kSillyLinkSizeCvar = CVAR_COSMETIC("Silly.LinkSize");
static const char* kSillyLinkHeadScaleCvar = CVAR_COSMETIC("Silly.LinkHeadScale");
static const char* kSillyBunnyHoodEarLengthCvar = CVAR_COSMETIC("Silly.BunnyHoodEarLength");
static const char* kSillyBunnyHoodEarSpreadCvar = CVAR_COSMETIC("Silly.BunnyHoodEarSpread");
static const char* kSillyFairySizeCvar = CVAR_COSMETIC("Silly.FairySize");
static const char* kSillyLockOnReticleScaleCvar = CVAR_COSMETIC("Silly.LockOnReticleScale");
static const char* kSillyLockOnReticleSpinCvar = CVAR_COSMETIC("Silly.LockOnReticleSpin");
static const char* kSillyClockTowerSpeedCvar = CVAR_COSMETIC("Silly.ClockTowerSpeed");
static const char* kSillyLikeLikeWidthCvar = CVAR_COSMETIC("Silly.LikeLikeWidth");
static const char* kSillyMoonSizeCvar = CVAR_COSMETIC("Silly.MoonSize");
static const char* kSillyLinkSwordScaleCvar = CVAR_COSMETIC("Silly.LinkSwordScale");
static const char* kSillyLinkShieldScaleCvar = CVAR_COSMETIC("Silly.LinkShieldScale");
static const char* kSillyGreatFairyMaskHairLengthCvar = CVAR_COSMETIC("Silly.GreatFairyMaskHairLength");
static const char* kSillyGreatFairyMaskHairSpreadCvar = CVAR_COSMETIC("Silly.GreatFairyMaskHairSpread");
static const char* kSillyGoronNeckLengthCvar = CVAR_COSMETIC("Silly.GoronNeckLength");
static const char* kSillyFlatShadingCvar = CVAR_SILLY_FLAT_SHADING;
static const char* kSillyDullRupeesCvar = CVAR_COSMETIC("Silly.DullRupees");
static const char* kSillyRainEverywhereCvar = CVAR_COSMETIC("Silly.RainEverywhere");

// this is a little weird but we'll probably replace it with my sail PR later
static void CosmeticEditorApplyLinkSize(Actor* actor) {
    static f32 lastMultiplier = 1.0f;

    f32 multiplier = CVarGetFloat(kSillyLinkSizeCvar, 1.0f);
    if ((multiplier == 1.0f) && (lastMultiplier == 1.0f)) {
        return;
    }

    Player* player = (Player*)actor;
    f32 baseScale = (player->transformation == PLAYER_FORM_FIERCE_DEITY) ? 0.015f : 0.01f;

    Actor_SetScale(actor, baseScale * multiplier);
    lastMultiplier = multiplier;
}

extern "C" void CosmeticEditor_ApplySillyMoonScale(void) {
    f32 scale = CVarGetFloat(kSillyMoonSizeCvar, 1.0f);

    if (scale != 1.0f) {
        Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
    }
}

static void CosmeticEditorScaleAbout(f32 scale, f32 anchorX, f32 anchorY, f32 anchorZ) {
    Matrix_Translate(anchorX, anchorY, anchorZ, MTXMODE_APPLY);
    Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
    Matrix_Translate(-anchorX, -anchorY, -anchorZ, MTXMODE_APPLY);
}

extern "C" void CosmeticEditor_ApplySillyLimbScale(const char* cvar, Vec3f* jointPos) {
    f32 scale = CVarGetFloat(cvar, 1.0f);

    if (scale == 1.0f) {
        return;
    }

    CosmeticEditorScaleAbout(scale, jointPos->x, jointPos->y, jointPos->z);
}

#define HELD_ITEM_GRIP_HUMAN 210.0f
#define HELD_ITEM_GRIP_FIERCE_DEITY 390.0f

typedef struct {
    const char* combined;
    const char* hand;
    const char* item;
    const char* extraItem; // gilded sword splits into a handle and a blade
    f32 gripHeight;
} HeldItemSplit;

static const HeldItemSplit sHeldItemSplits[] = {
    { gLinkHumanLeftHandHoldingKokiriSwordDL, gLinkHumanLeftHandClosedDL, gKokiriSwordDL, nullptr,
      HELD_ITEM_GRIP_HUMAN },
    { gLinkHumanLeftHandHoldingRazorSwordDL, gLinkHumanLeftHandClosedDL, gRazorSwordDL, nullptr, HELD_ITEM_GRIP_HUMAN },
    { gLinkHumanLeftHandHoldingGildedSwordDL, gLinkHumanLeftHandClosedDL, gLinkHumanGildedSwordHandleDL,
      gLinkHumanGildedSwordBladeDL, HELD_ITEM_GRIP_HUMAN },
    { gLinkHumanLeftHandHoldingGreatFairysSwordDL, gLinkHumanLeftHandClosedDL, gLinkHumanGreatFairysSwordDL, nullptr,
      HELD_ITEM_GRIP_HUMAN },
    { gLinkFierceDeityLeftHandHoldingSwordDL, gLinkFierceDeityLeftHandDL, gLinkFierceDeitySwordDL, nullptr,
      HELD_ITEM_GRIP_FIERCE_DEITY },
    { gLinkHumanRightHandHoldingHerosShieldDL, gLinkHumanRightHandClosedDL, gLinkHumanHerosShieldDL, nullptr,
      HELD_ITEM_GRIP_HUMAN },
    { gLinkHumanRightHandHoldingMirrorShieldDL, gLinkHumanRightHandClosedDL, gLinkHumanMirrorShieldDL, nullptr,
      HELD_ITEM_GRIP_HUMAN },
};

static const HeldItemSplit* sPendingHeldItemSplit = nullptr;
static f32 sPendingHeldItemScale = 1.0f;

extern "C" void CosmeticEditor_SplitHeldItem(Gfx** dList, const char* cvar) {
    f32 scale = CVarGetFloat(cvar, 1.0f);

    if ((*dList == nullptr) || (scale == 1.0f)) {
        return;
    }

    for (const HeldItemSplit& split : sHeldItemSplits) {
        if (strcmp((const char*)*dList, split.combined) == 0) {
            *dList = (Gfx*)split.hand;
            sPendingHeldItemSplit = &split;
            sPendingHeldItemScale = scale;
            return;
        }
    }
}

extern "C" void CosmeticEditor_DrawSplitHeldItem(PlayState* play) {
    const HeldItemSplit* split = sPendingHeldItemSplit;
    f32 scale = sPendingHeldItemScale;

    sPendingHeldItemSplit = nullptr;

    if (split == nullptr) {
        return;
    }

    OPEN_DISPS(play->state.gfxCtx);

    Matrix_Push();
    CosmeticEditorScaleAbout(scale, 0.0f, split->gripHeight, 0.0f);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, (Gfx*)split->item);
    if (split->extraItem != nullptr) {
        gSPDisplayList(POLY_OPA_DISP++, (Gfx*)split->extraItem);
    }
    Matrix_Pop();

    CLOSE_DISPS(play->state.gfxCtx);
}

extern "C" void CosmeticEditor_DrawBackShield(PlayState* play, Gfx* dList) {
    f32 scale = CVarGetFloat(kSillyLinkShieldScaleCvar, 1.0f);

    OPEN_DISPS(play->state.gfxCtx);

    if (scale != 1.0f) {
        Matrix_Push();
        CosmeticEditorScaleAbout(scale, 552.0f, 0.0f, 0.0f);
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_OPA_DISP++, dList);
        Matrix_Pop();
    } else {
        gSPDisplayList(POLY_OPA_DISP++, dList);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void CosmeticEditorUpdateTick() {
    RefreshDynamicCosmeticsStateIfNeeded();

    int index = 0;
    float rainbowSpeed = CVarGetFloat(kCosmeticRainbowSpeedCvar, 0.6f);
    if (rainbowSpeed <= 0.0f) {
        rainbowSpeed = 0.6f;
    }

    bool hasRainbowEntries = false;
    bool syncRainbow = CVarGetInteger(kCosmeticRainbowSyncCvar, 0);

    for (auto& [id, option] : cosmeticOptions) {
        if (CosmeticEditorIsSuppressed(option)) {
            continue;
        }
        if (!option.supportsRainbow || !CVarGetInteger(option.rainbowCvar, 0)) {
            if (!syncRainbow) {
                index += static_cast<int>(60 * rainbowSpeed);
            }
            continue;
        }

        hasRainbowEntries = true;

        double frequency = 2 * M_PI / (360 * rainbowSpeed);
        Color_RGBA8 color = {
            static_cast<uint8_t>(sin(frequency * (sCosmeticRainbowHue + index) + 0) * 127 + 128),
            static_cast<uint8_t>(sin(frequency * (sCosmeticRainbowHue + index) + (2 * M_PI / 3)) * 127 + 128),
            static_cast<uint8_t>(sin(frequency * (sCosmeticRainbowHue + index) + (4 * M_PI / 3)) * 127 + 128),
            static_cast<uint8_t>(option.supportsAlpha ? option.currentColor.w * 255.0f : 255),
        };

        option.currentColor = ImVec4(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f);
        CVarSetColor(option.valuesCvar, color);

        ShipInit::Init(option.valuesCvar);
        if (!CVarGetInteger(option.changedCvar, 0)) {
            CVarSetInteger(option.changedCvar, 1);
            ShipInit::Init(option.changedCvar);
        }

        if (!syncRainbow) {
            index += static_cast<int>(60 * rainbowSpeed);
        }
    }

    const bool hasCustomRainbowEntries = UpdateCustomCosmeticsRainbow(sCosmeticRainbowHue, rainbowSpeed, index);
    if (hasCustomRainbowEntries) {
        ApplyDynamicCosmetics();
    }

    if (!hasRainbowEntries && !hasCustomRainbowEntries) {
        return;
    }

    sCosmeticRainbowHue++;
    if (sCosmeticRainbowHue >= (360 * rainbowSpeed)) {
        sCosmeticRainbowHue = 0;
    }
}

void CosmeticEditorDrawRow(CosmeticOption& option) {
    if (CosmeticEditorIsSuppressed(option)) {
        return;
    }

    Color_RGBA8 defaultColor = CosmeticEditorGetDefaultColor(option);

    if (UIWidgets::CVarColorPicker(option.label, option.valuesCvar, defaultColor, option.supportsAlpha,
                                   option.lockedCvar, THEME_COLOR)) {
        Color_RGBA8 changedColor = CVarGetColor(option.valuesCvar, defaultColor);
        option.currentColor =
            ImVec4(changedColor.r / 255.0f, changedColor.g / 255.0f, changedColor.b / 255.0f, changedColor.a / 255.0f);
        CVarSetInteger(option.changedCvar, 1);
        CosmeticEditorRefreshElement(option);
        CosmeticEditorSave();
    }

    ImGui::SameLine((ImGui::CalcTextSize("Message Light Blue (None No Shadow)").x * 1.0f) + 60.0f);
    if (UIWidgets::Button(
            ("Random##" + std::string(option.label)).c_str(),
            UIWidgets::ButtonOptions().Size(ImVec2(80, 31)).Padding(ImVec2(2.0f, 0.0f)).Color(THEME_COLOR))) {
        CosmeticEditorRandomizeElement(option);
    }

    ImGui::SameLine();
    bool rainbowEnabled = CVarGetInteger(option.rainbowCvar, 0);
    if (UIWidgets::Checkbox(("Rainbow##" + std::string(option.label)).c_str(), &rainbowEnabled,
                            UIWidgets::CheckboxOptions().Color(THEME_COLOR))) {
        CosmeticEditorSetRainbowEnabled(option, rainbowEnabled);
    }

    ImGui::SameLine();
    bool locked = CVarGetInteger(option.lockedCvar, 0);
    if (UIWidgets::Checkbox(("Locked##" + std::string(option.label)).c_str(), &locked,
                            UIWidgets::CheckboxOptions().Color(THEME_COLOR))) {
        CosmeticEditorSetLocked(option, locked);
    }

    if (CVarGetInteger(option.changedCvar, 0)) {
        ImGui::SameLine();
        if (UIWidgets::Button(("Reset##" + std::string(option.label)).c_str(),
                              UIWidgets::ButtonOptions().Size(ImVec2(80, 31)).Padding(ImVec2(2.0f, 0.0f)))) {
            CosmeticEditorResetElement(option);
        }
    }
}

static void CosmeticEditorDrawSillySlider(const char* label, const char* cvar, float min, float max, float step,
                                          float defaultValue, const char* format, const char* tooltip = nullptr) {
    UIWidgets::FloatSliderOptions options = UIWidgets::FloatSliderOptions()
                                                .Format(format)
                                                .Min(min)
                                                .Max(max)
                                                .DefaultValue(defaultValue)
                                                .Step(step)
                                                .Size(ImVec2(300.0f, 0.0f))
                                                .Color(THEME_COLOR);
    if (tooltip != nullptr) {
        options = options.Tooltip(tooltip);
    }

    if (UIWidgets::CVarSliderFloat(label, cvar, options)) {
        CosmeticEditorSave();
    }
}

void CosmeticEditorResetSillyOptions() {
    CVarClear(kSillyLinkSizeCvar);
    CVarClear(kSillyLinkHeadScaleCvar);
    CVarClear(kSillyBunnyHoodEarLengthCvar);
    CVarClear(kSillyBunnyHoodEarSpreadCvar);
    CVarClear(kSillyFairySizeCvar);
    CVarClear(kSillyLockOnReticleScaleCvar);
    CVarClear(kSillyLockOnReticleSpinCvar);
    CVarClear(kSillyClockTowerSpeedCvar);
    CVarClear(kSillyLikeLikeWidthCvar);
    CVarClear(kSillyMoonSizeCvar);
    CVarClear(kSillyLinkSwordScaleCvar);
    CVarClear(kSillyLinkShieldScaleCvar);
    CVarClear(kSillyGreatFairyMaskHairLengthCvar);
    CVarClear(kSillyGreatFairyMaskHairSpreadCvar);
    CVarClear(kSillyGoronNeckLengthCvar);
    CVarClear(kSillyFlatShadingCvar);
    ShipInit::Init(kSillyFlatShadingCvar);
    CVarClear(kSillyDullRupeesCvar);
    ShipInit::Init(kSillyDullRupeesCvar);
    CVarClear(kSillyRainEverywhereCvar);
    ShipInit::Init(kSillyRainEverywhereCvar);
}

void CosmeticEditorDrawSillyTab() {
    if (UIWidgets::Button("Reset Silly Options",
                          UIWidgets::ButtonOptions().Size(ImVec2(250.0f, 0.0f)).Color(THEME_COLOR))) {
        CosmeticEditorResetSillyOptions();
        CosmeticEditorSave();
    }

    UIWidgets::Separator(true, true, 2.0f, 2.0f);

    UIWidgets::CVarCheckbox("Average Textures", kSillyFlatShadingCvar, UIWidgets::CheckboxOptions().Color(THEME_COLOR));
    UIWidgets::CVarCheckbox(
        "Dull Rupees", kSillyDullRupeesCvar,
        UIWidgets::CheckboxOptions()
            .Color(THEME_COLOR)
            .Tooltip("Drops the glassy shell that draws over rupees, leaving just the solid inside."));
    UIWidgets::CVarCheckbox("Rain Everywhere", kSillyRainEverywhereCvar,
                            UIWidgets::CheckboxOptions().Color(THEME_COLOR));

    UIWidgets::Separator(true, true, 2.0f, 2.0f);

    CosmeticEditorDrawSillySlider("Link Size", kSillyLinkSizeCvar, 0.1f, 5.0f, 0.1f, 1.0f, "%.1fx");
    CosmeticEditorDrawSillySlider("Link Head Scale", kSillyLinkHeadScaleCvar, 0.1f, 5.0f, 0.1f, 1.0f, "%.1fx");
    CosmeticEditorDrawSillySlider("Link Sword Scale", kSillyLinkSwordScaleCvar, 0.1f, 5.0f, 0.1f, 1.0f, "%.1fx");
    CosmeticEditorDrawSillySlider("Link Shield Scale", kSillyLinkShieldScaleCvar, 0.1f, 5.0f, 0.1f, 1.0f, "%.1fx");

    UIWidgets::Separator(true, true, 2.0f, 2.0f);

    CosmeticEditorDrawSillySlider("Bunny Hood Ear Length", kSillyBunnyHoodEarLengthCvar, -300.0f, 1000.0f, 10.0f, 0.0f,
                                  "%.0f");
    CosmeticEditorDrawSillySlider("Bunny Hood Ear Spread", kSillyBunnyHoodEarSpreadCvar, -300.0f, 500.0f, 10.0f, 0.0f,
                                  "%.0f");

    UIWidgets::Separator(true, true, 2.0f, 2.0f);

    CosmeticEditorDrawSillySlider("Great Fairy's Mask Hair Length", kSillyGreatFairyMaskHairLengthCvar, 0.1f, 5.0f,
                                  0.1f, 1.0f, "%.1fx");
    CosmeticEditorDrawSillySlider("Great Fairy's Mask Hair Spread", kSillyGreatFairyMaskHairSpreadCvar, -3.0f, 5.0f,
                                  0.1f, 1.0f, "%.1fx");

    UIWidgets::Separator(true, true, 2.0f, 2.0f);

    CosmeticEditorDrawSillySlider("Fairy Size", kSillyFairySizeCvar, 0.1f, 5.0f, 0.1f, 1.0f, "%.1fx");
    CosmeticEditorDrawSillySlider("Goron Neck Length", kSillyGoronNeckLengthCvar, -1000.0f, 12000.0f, 100.0f, 0.0f,
                                  "%.0f");

    UIWidgets::Separator(true, true, 2.0f, 2.0f);

    CosmeticEditorDrawSillySlider("Lock-On Reticle Size", kSillyLockOnReticleScaleCvar, 0.1f, 5.0f, 0.1f, 1.0f,
                                  "%.1fx");
    CosmeticEditorDrawSillySlider("Lock-On Reticle Spin", kSillyLockOnReticleSpinCvar, -5.0f, 5.0f, 0.1f, 1.0f,
                                  "%.1fx");

    UIWidgets::Separator(true, true, 2.0f, 2.0f);

    CosmeticEditorDrawSillySlider(
        "Clock Tower Speed", kSillyClockTowerSpeedCvar, -50.0f, 50.0f, 1.0f, 1.0f, "%.1fx",
        "This isn't the actual clock but like the thing on top, idk what it's supposed to be lol");
    CosmeticEditorDrawSillySlider("Moon Size", kSillyMoonSizeCvar, 0.1f, 2.2f, 0.1f, 1.0f, "%.1fx");
    CosmeticEditorDrawSillySlider("Like Like Width", kSillyLikeLikeWidthCvar, 0.1f, 5.0f, 0.1f, 1.0f, "%.1fx");
}

void CosmeticEditorDrawGroup(CosmeticGroup group, const char* displayName = nullptr) {
    bool hasVisibleOptions = false;
    for (auto& [id, option] : cosmeticOptions) {
        if (CosmeticEditorMatchesGroup(option, group) && !CosmeticEditorIsSuppressed(option)) {
            hasVisibleOptions = true;
            break;
        }
    }

    if (!hasVisibleOptions) {
        return;
    }

    std::string label = displayName != nullptr ? displayName : sCosmeticGroupLabels.at(group);
    ImGui::Text("%s", label.c_str());
    ImGui::SameLine((ImGui::CalcTextSize("Message Light Blue (None No Shadow)").x * 1.0f) + 60.0f);
    if (UIWidgets::Button(
            ("Random##" + label).c_str(),
            UIWidgets::ButtonOptions().Size(ImVec2(80, 31)).Padding(ImVec2(2.0f, 0.0f)).Color(THEME_COLOR))) {
        CosmeticEditorRandomizeGroup(group);
    }
    ImGui::SameLine();
    if (UIWidgets::Button(("Reset##" + label).c_str(),
                          UIWidgets::ButtonOptions().Size(ImVec2(80, 31)).Padding(ImVec2(2.0f, 0.0f)))) {
        CosmeticEditorResetGroup(group);
    }
    UIWidgets::Spacer();

    for (auto& [id, option] : cosmeticOptions) {
        if (CosmeticEditorMatchesGroup(option, group) && !CosmeticEditorIsSuppressed(option)) {
            CosmeticEditorDrawRow(option);
        }
    }

    UIWidgets::Separator(true, true, 2.0f, 2.0f);
}

void CosmeticEditorWindow::DrawElement() {
    RefreshDynamicCosmeticsStateIfNeeded();

    UIWidgets::CVarCheckbox("Sync Rainbow colors", kCosmeticRainbowSyncCvar,
                            UIWidgets::CheckboxOptions()
                                .Color(THEME_COLOR)
                                .Tooltip("Keeps all rainbow-enabled cosmetics on the same hue cycle."));
    UIWidgets::CVarSliderFloat("Rainbow Speed", kCosmeticRainbowSpeedCvar,
                               UIWidgets::FloatSliderOptions()
                                   .Format("%.2f")
                                   .Min(0.01f)
                                   .Max(1.0f)
                                   .DefaultValue(0.6f)
                                   .Step(0.01f)
                                   .Size(ImVec2(300.0f, 0.0f))
                                   .Color(THEME_COLOR));
    UIWidgets::CVarCheckbox(
        "Randomize all Cosmetics on Randomizer Generation", kCosmeticRandomizeOnSeedGenCvar,
        UIWidgets::CheckboxOptions()
            .Color(THEME_COLOR)
            .Tooltip("Randomizes every unlocked cosmetic entry when a new randomizer seed is generated."));

    if (UIWidgets::Button("Randomize All", UIWidgets::ButtonOptions().Size(ImVec2(250.0f, 0.0f)).Color(THEME_COLOR))) {
        CosmeticEditorRandomizeAllElements();
    }
    ImGui::SameLine();
    if (UIWidgets::Button("Reset All", UIWidgets::ButtonOptions().Size(ImVec2(250.0f, 0.0f)).Color(THEME_COLOR))) {
        CosmeticEditorResetAllElements();
    }

    if (UIWidgets::Button("Lock All", UIWidgets::ButtonOptions().Size(ImVec2(250.0f, 0.0f)).Color(THEME_COLOR))) {
        CosmeticEditorSetAllLocked(true);
    }
    ImGui::SameLine();
    if (UIWidgets::Button("Unlock All", UIWidgets::ButtonOptions().Size(ImVec2(250.0f, 0.0f)).Color(THEME_COLOR))) {
        CosmeticEditorSetAllLocked(false);
    }

    if (UIWidgets::Button("Rainbow All", UIWidgets::ButtonOptions().Size(ImVec2(250.0f, 0.0f)).Color(THEME_COLOR))) {
        CosmeticEditorSetAllRainbow(true);
    }
    ImGui::SameLine();
    if (UIWidgets::Button("Un-Rainbow All", UIWidgets::ButtonOptions().Size(ImVec2(250.0f, 0.0f)).Color(THEME_COLOR))) {
        CosmeticEditorSetAllRainbow(false);
    }

    UIWidgets::Spacer(3.0f);

    UIWidgets::PushStyleTabs(THEME_COLOR);
    if (ImGui::BeginTabBar("CosmeticsContextTabBar", ImGuiTabBarFlags_NoCloseWithMiddleMouseButton)) {
        if (ImGui::BeginTabItem("Link & Items")) {
            UIWidgets::Separator(true, true, 2.0f, 2.0f);
            CosmeticEditorDrawGroup(COSMETICS_GROUP_PLAYER, "Link");
            ImGui::EndTabItem();
        }

        if (HasCustomCosmetics() && ImGui::BeginTabItem("Mods")) {
            UIWidgets::Separator(true, true, 2.0f, 2.0f);
            DrawDynamicCosmetics();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Effects")) {
            UIWidgets::Separator(true, true, 2.0f, 2.0f);
            CosmeticEditorDrawGroup(COSMETICS_GROUP_EFFECTS);
            CosmeticEditorDrawGroup(COSMETICS_GROUP_TRAILS);
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("HUD")) {
            UIWidgets::Separator(true, true, 2.0f, 2.0f);
            CosmeticEditorDrawGroup(COSMETICS_GROUP_HUD);
            CosmeticEditorDrawGroup(COSMETICS_GROUP_BUTTONS);
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("World")) {
            UIWidgets::Separator(true, true, 2.0f, 2.0f);
            CosmeticEditorDrawGroup(COSMETICS_GROUP_COLLECTIBLES);
            CosmeticEditorDrawGroup(COSMETICS_GROUP_DUNGEONS);
            CosmeticEditorDrawGroup(COSMETICS_GROUP_WORLD);
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Pause Menu")) {
            UIWidgets::Separator(true, true, 2.0f, 2.0f);
            CosmeticEditorDrawGroup(COSMETICS_GROUP_MENUS);
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Silly")) {
            UIWidgets::Separator(true, true, 2.0f, 2.0f);
            CosmeticEditorDrawSillyTab();
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }
    UIWidgets::PopStyleTabs();
}

void CosmeticEditorWindow::InitElement() {
    static bool sHooksRegistered = false;
    if (sHooksRegistered) {
        return;
    }

    for (auto& [id, option] : cosmeticOptions) {
        Color_RGBA8 cvarColor = CVarGetColor(option.valuesCvar, option.defaultColor);
        option.currentColor =
            ImVec4(cvarColor.r / 255.0f, cvarColor.g / 255.0f, cvarColor.b / 255.0f, cvarColor.a / 255.0f);
        CosmeticEditorRefreshElement(option);
    }
    CosmeticEditorSave();

    RefreshDynamicCosmeticsStateIfNeeded();
    ApplyDynamicCosmetics();

    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnRandoSeedGeneration>([]() {
        if (CVarGetInteger(kCosmeticRandomizeOnSeedGenCvar, 0)) {
            CosmeticEditorRandomizeAllElements();
        }
    });
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnGameStateUpdate>([]() { CosmeticEditorUpdateTick(); });
    GameInteractor::Instance->RegisterGameHookForID<GameInteractor::OnActorUpdate>(
        ACTOR_PLAYER, [](Actor* actor) { CosmeticEditorApplyLinkSize(actor); });

    sHooksRegistered = true;
}

// Player.HumanTunic

Gfx humanTunic[] = {
    gsDPSetPrimColor(0, 0, 0, 0, 0, 0),
    gsDPPipeSync(),
    gsSPEndDisplayList(),
};

static RegisterShipInitFunc humanTunicPatch(
    []() {
        if (!IsCustomHumanModelActive() && CVarGetInteger(kHumanTunicOption.colorChangedCvar, 0)) {
            ResourceMgr_PatchGfxByName("objects/object_link_child/gLinkHumanWaistDL", "setPrim", 5,
                                       gsSPDisplayList(humanTunic));
            ResourceMgr_PatchGfxByName("objects/object_link_child/gLinkHumanRightThighDL", "setPrim", 10,
                                       gsSPDisplayList(humanTunic));
            ResourceMgr_PatchGfxByName("objects/object_link_child/gLinkHumanLeftThighDL", "setPrim", 10,
                                       gsSPDisplayList(humanTunic));
            ResourceMgr_PatchGfxByName("objects/object_link_child/gLinkHumanHeadDL", "setPrim", 92,
                                       gsSPDisplayList(humanTunic));
            ResourceMgr_PatchGfxByName("objects/object_link_child/gLinkHumanHatDL", "setPrim", 10,
                                       gsSPDisplayList(humanTunic));
            ResourceMgr_PatchGfxByName("objects/object_link_child/gLinkHumanCollarDL", "setPrim", 5,
                                       gsSPDisplayList(humanTunic));
            ResourceMgr_PatchGfxByName("objects/object_link_child/gLinkHumanLeftShoulderDL", "setPrim1", 10,
                                       gsSPDisplayList(humanTunic));
            ResourceMgr_PatchGfxByName("objects/object_link_child/gLinkHumanLeftShoulderDL", "setPrim2", 65,
                                       gsSPDisplayList(humanTunic));
            ResourceMgr_PatchGfxByName("objects/object_link_child/gLinkHumanRightShoulderDL", "setPrim1", 10,
                                       gsSPDisplayList(humanTunic));
            ResourceMgr_PatchGfxByName("objects/object_link_child/gLinkHumanRightShoulderDL", "setPrim2", 65,
                                       gsSPDisplayList(humanTunic));
            ResourceMgr_PatchGfxByName("objects/object_link_child/gLinkHumanTorsoDL", "setPrim", 5,
                                       gsSPDisplayList(humanTunic));
        } else {
            ResourceMgr_UnpatchGfxByName("objects/object_link_child/gLinkHumanWaistDL", "setPrim");
            ResourceMgr_UnpatchGfxByName("objects/object_link_child/gLinkHumanRightThighDL", "setPrim");
            ResourceMgr_UnpatchGfxByName("objects/object_link_child/gLinkHumanLeftThighDL", "setPrim");
            ResourceMgr_UnpatchGfxByName("objects/object_link_child/gLinkHumanHeadDL", "setPrim");
            ResourceMgr_UnpatchGfxByName("objects/object_link_child/gLinkHumanHatDL", "setPrim");
            ResourceMgr_UnpatchGfxByName("objects/object_link_child/gLinkHumanCollarDL", "setPrim");
            ResourceMgr_UnpatchGfxByName("objects/object_link_child/gLinkHumanLeftShoulderDL", "setPrim1");
            ResourceMgr_UnpatchGfxByName("objects/object_link_child/gLinkHumanLeftShoulderDL", "setPrim2");
            ResourceMgr_UnpatchGfxByName("objects/object_link_child/gLinkHumanRightShoulderDL", "setPrim1");
            ResourceMgr_UnpatchGfxByName("objects/object_link_child/gLinkHumanRightShoulderDL", "setPrim2");
            ResourceMgr_UnpatchGfxByName("objects/object_link_child/gLinkHumanTorsoDL", "setPrim");
        }
    },
    { kHumanTunicOption.colorChangedCvar });

static RegisterShipInitFunc humanTunicColor(
    []() {
        Color_RGBA8 changedColor = CVarGetColor(kHumanTunicOption.colorCvar, {});
        humanTunic[0] = gsDPSetPrimColor(0, 0, changedColor.r, changedColor.g, changedColor.b, 255);
    },
    { kHumanTunicOption.colorCvar });

// Player.HumanHair

Gfx humanHair[] = {
    gsDPSetPrimColor(0, 0, 0, 0, 0, 0),
    gsDPPipeSync(),
    gsSPEndDisplayList(),
};

static RegisterShipInitFunc humanHairPatch(
    []() {
        if (!IsCustomHumanModelActive() && CVarGetInteger(kHumanHairOption.colorChangedCvar, 0)) {
            ResourceMgr_PatchGfxByName("objects/object_link_child/gLinkHumanHeadDL", "setPrim1", 162,
                                       gsSPDisplayList(humanHair));
            ResourceMgr_PatchGfxByName("objects/object_link_child/gLinkHumanHeadDL", "setPrim2", 201,
                                       gsSPDisplayList(backToWhite));

            ShadePaletteWhite("objects/object_link_child/object_link_child_Tex_005400", 0, 127, MODE_AVG);
        } else {
            ResourceMgr_UnpatchGfxByName("objects/object_link_child/gLinkHumanHeadDL", "setPrim1");
            ResourceMgr_UnpatchGfxByName("objects/object_link_child/gLinkHumanHeadDL", "setPrim2");

            ShadePaletteRevert("objects/object_link_child/object_link_child_Tex_005400", 0, 127);
        }
    },
    { kHumanHairOption.colorChangedCvar });

static RegisterShipInitFunc humanHairColor(
    []() {
        Color_RGBA8 changedColor = CVarGetColor(kHumanHairOption.colorCvar, {});
        humanHair[0] = gsDPSetPrimColor(0, 0, changedColor.r, changedColor.g, changedColor.b, 255);
    },
    { kHumanHairOption.colorCvar });

// Player.DekuTunic

Gfx dekuTunic[] = {
    gsDPSetPrimColor(0, 0, 0, 0, 0, 0),
    gsDPPipeSync(),
    gsSPEndDisplayList(),
};

static RegisterShipInitFunc dekuTunicPatch(
    []() {
        if (!IsCustomDekuModelActive() && CVarGetInteger(kDekuTunicOption.colorChangedCvar, 0)) {
            ResourceMgr_PatchGfxByName("objects/object_link_nuts/gLinkDekuWaistDL", "setPrim", 22,
                                       gsSPDisplayList(dekuTunic));
            ResourceMgr_PatchGfxByName("objects/object_link_nuts/gLinkDekuHeadDL", "setPrim1", 55,
                                       gsSPDisplayList(dekuTunic));
            ResourceMgr_PatchGfxByName("objects/object_link_nuts/gLinkDekuHeadDL", "setPrim2", 76,
                                       gsSPDisplayList(backToWhite));
            ResourceMgr_PatchGfxByName("objects/object_link_nuts/gLinkDekuHatDL", "setPrim", 29,
                                       gsSPDisplayList(dekuTunic));

            ShadePaletteWhite("objects/object_link_nuts/object_link_nuts_TLUT_003EB0", 243, 254, MODE_MAX);
        } else {
            ResourceMgr_UnpatchGfxByName("objects/object_link_nuts/gLinkDekuWaistDL", "setPrim");
            ResourceMgr_UnpatchGfxByName("objects/object_link_nuts/gLinkDekuHeadDL", "setPrim1");
            ResourceMgr_UnpatchGfxByName("objects/object_link_nuts/gLinkDekuHeadDL", "setPrim2");
            ResourceMgr_UnpatchGfxByName("objects/object_link_nuts/gLinkDekuHatDL", "setPrim");

            ShadePaletteRevert("objects/object_link_nuts/object_link_nuts_TLUT_003EB0", 243, 254);
        }
    },
    { kDekuTunicOption.colorChangedCvar });

static RegisterShipInitFunc dekuTunicColor(
    []() {
        Color_RGBA8 changedColor = CVarGetColor(kDekuTunicOption.colorCvar, {});
        dekuTunic[0] = gsDPSetPrimColor(0, 0, changedColor.r, changedColor.g, changedColor.b, 255);
    },
    { kDekuTunicOption.colorCvar });

// Player.DekuHair

Gfx dekuHair[] = {
    gsDPSetPrimColor(0, 0, 0, 0, 0, 0),
    gsDPPipeSync(),
    gsSPEndDisplayList(),
};

static RegisterShipInitFunc dekuHairPatch(
    []() {
        if (!IsCustomDekuModelActive() && CVarGetInteger(kDekuHairOption.colorChangedCvar, 0)) {
            ResourceMgr_PatchGfxByName("objects/object_link_nuts/gLinkDekuHeadDL", "setPrim3", 22,
                                       gsSPDisplayList(dekuHair));
            ResourceMgr_PatchGfxByName("objects/object_link_nuts/gLinkDekuHeadDL", "setPrim4", 42,
                                       gsSPDisplayList(backToWhite));
            ResourceMgr_PatchGfxByName("objects/object_link_nuts/object_link_nuts_DL_009C48", "setPrim", 22,
                                       gsSPDisplayList(dekuHair));

            ShadePaletteWhite("objects/object_link_nuts/object_link_nuts_TLUT_003EB0", 109, 122, MODE_MAX);
            ShadePaletteWhite("objects/object_link_nuts/object_link_nuts_TLUT_003EB0", 124, 242, MODE_MAX);
        } else {
            ResourceMgr_UnpatchGfxByName("objects/object_link_nuts/gLinkDekuHeadDL", "setPrim3");
            ResourceMgr_UnpatchGfxByName("objects/object_link_nuts/gLinkDekuHeadDL", "setPrim4");
            ResourceMgr_UnpatchGfxByName("objects/object_link_nuts/object_link_nuts_DL_009C48", "setPrim");

            ShadePaletteRevert("objects/object_link_nuts/object_link_nuts_TLUT_003EB0", 109, 122);
            ShadePaletteRevert("objects/object_link_nuts/object_link_nuts_TLUT_003EB0", 124, 242);
        }
    },
    { kDekuHairOption.colorChangedCvar });

static RegisterShipInitFunc dekuHairColor(
    []() {
        Color_RGBA8 changedColor = CVarGetColor(kDekuHairOption.colorCvar, {});
        dekuHair[0] = gsDPSetPrimColor(0, 0, changedColor.r, changedColor.g, changedColor.b, 255);
    },
    { kDekuHairOption.colorCvar });

// Player.KafeiHair

Gfx kafeiHair[] = {
    gsDPSetPrimColor(0, 0, 0, 0, 0, 0),
    gsDPPipeSync(),
    gsSPEndDisplayList(),
};

static const char* kKafeiHeadDlPath = "objects/object_test3/gKafeiHeadDL";

static RegisterShipInitFunc kafeiHairPatch(
    []() {
        if (!IsCustomKafeiModelActive() && CVarGetInteger(kKafeiHairOption.colorChangedCvar, 0)) {
            ResourceMgr_PatchGfxByName(kKafeiHeadDlPath, "setPrim1", 101, gsSPDisplayList(kafeiHair));
            ResourceMgr_PatchGfxByName(kKafeiHeadDlPath, "setPrim2", 163, gsSPDisplayList(backToWhite));
            ResourceMgr_PatchGfxByName(kKafeiHeadDlPath, "setPrim3", 200, gsSPDisplayList(kafeiHair));
            ResourceMgr_PatchGfxByName(kKafeiHeadDlPath, "setPrim4", 236, gsSPDisplayList(backToWhite));

            uint8_t* hairTlut = ShadeKafeiHairTlut();
            if (hairTlut != nullptr) {
                ResourceMgr_PatchGfxByName(kKafeiHeadDlPath, "hairTlut1", 89,
                                           gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, hairTlut));
                ResourceMgr_PatchGfxByName(kKafeiHeadDlPath, "hairTlut1Hash", 90, gsDPNoOp());
                ResourceMgr_PatchGfxByName(kKafeiHeadDlPath, "hairTlut2", 194,
                                           gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, hairTlut));
                ResourceMgr_PatchGfxByName(kKafeiHeadDlPath, "hairTlut2Hash", 195, gsDPNoOp());
            }
        } else {
            ResourceMgr_UnpatchGfxByName(kKafeiHeadDlPath, "setPrim1");
            ResourceMgr_UnpatchGfxByName(kKafeiHeadDlPath, "setPrim2");
            ResourceMgr_UnpatchGfxByName(kKafeiHeadDlPath, "setPrim3");
            ResourceMgr_UnpatchGfxByName(kKafeiHeadDlPath, "setPrim4");
            ResourceMgr_UnpatchGfxByName(kKafeiHeadDlPath, "hairTlut1");
            ResourceMgr_UnpatchGfxByName(kKafeiHeadDlPath, "hairTlut1Hash");
            ResourceMgr_UnpatchGfxByName(kKafeiHeadDlPath, "hairTlut2");
            ResourceMgr_UnpatchGfxByName(kKafeiHeadDlPath, "hairTlut2Hash");

            ShadeKafeiHairTlutRevert();
        }
    },
    { kKafeiHairOption.colorChangedCvar });

static RegisterShipInitFunc kafeiHairColor(
    []() {
        Color_RGBA8 changedColor = CVarGetColor(kKafeiHairOption.colorCvar, {});
        kafeiHair[0] = gsDPSetPrimColor(0, 0, changedColor.r, changedColor.g, changedColor.b, 255);
    },
    { kKafeiHairOption.colorCvar });

// Player.GoronTunic

Gfx goronTunic[] = {
    gsDPSetPrimColor(0, 0, 0, 0, 0, 0),
    gsDPPipeSync(),
    gsSPEndDisplayList(),
};

static RegisterShipInitFunc goronTunicPatch(
    []() {
        if (!IsCustomGoronModelActive() && CVarGetInteger(kGoronTunicOption.colorChangedCvar, 0)) {
            ResourceMgr_PatchGfxByName("objects/object_link_goron/gLinkGoronWaistDL", "setPrim", 16,
                                       gsSPDisplayList(goronTunic));
            ResourceMgr_PatchGfxByName("objects/object_link_goron/gLinkGoronHatDL", "setPrim", 17,
                                       gsSPDisplayList(goronTunic));

            ShadePaletteWhite("objects/object_link_goron/object_link_goron_Tex_002780", 0, 127, MODE_MAX);
        } else {
            ResourceMgr_UnpatchGfxByName("objects/object_link_goron/gLinkGoronWaistDL", "setPrim");
            ResourceMgr_UnpatchGfxByName("objects/object_link_goron/gLinkGoronHatDL", "setPrim");

            ShadePaletteRevert("objects/object_link_goron/object_link_goron_Tex_002780", 0, 127);
            ShadePaletteRevert("objects/object_link_goron/object_link_goron_Tex_00CEB8", 0, 127);
        }
    },
    { kGoronTunicOption.colorChangedCvar });

static RegisterShipInitFunc goronTunicColor(
    []() {
        Color_RGBA8 changedColor = CVarGetColor(kGoronTunicOption.colorCvar, {});
        goronTunic[0] = gsDPSetPrimColor(0, 0, changedColor.r, changedColor.g, changedColor.b, 255);

        if (IsCustomGoronModelActive() || !CVarGetInteger(kGoronTunicOption.colorChangedCvar, 0)) {
            return;
        }

        ShadePaletteNewBase("objects/object_link_goron/object_link_goron_Tex_00CEB8", 0, 127, changedColor, MODE_MAX);
    },
    { kGoronTunicOption.colorCvar });

// Player.ZoraTunic
static const Color_RGBA8 zoraSkinColor = { 197, 247, 247, 255 };
static const Color_RGBA8 zoraTunicBaseColor = { 0, 74, 16, 255 };

static RegisterShipInitFunc zoraTunicColor(
    []() {
        if (IsCustomZoraModelActive() || !CVarGetInteger(kZoraTunicOption.colorChangedCvar, 0)) {
            return;
        }

        /*
         * Zora works differently from the other color changes. Other forms apply a grayscale to the green tunic
         * textures and then alter the Gfx commands to set the color. That works because those textures are one basic
         * color. Zora, however, gradually transitions from green to the bluish Zora skin. A further complication is
         * that relevant colors in the TLUTs are not contiguous, so the brightness calculation will not work as
         * intended. Instead of using the palette approach, here we directly apply the custom color to the textures and
         * TLUTs.
         */
        Color_RGBA8 changedColor = CVarGetColor(kZoraTunicOption.colorCvar, {});
        // Arms
        ShadePaletteGradient("objects/object_link_zora/object_link_zora_TLUT_00C578", 151, 177, zoraTunicBaseColor,
                             changedColor, zoraSkinColor);
        ShadePaletteGradient("objects/object_link_zora/object_link_zora_TLUT_00C578", 179, 180, zoraTunicBaseColor,
                             changedColor, zoraSkinColor);
        ShadePaletteGradient("objects/object_link_zora/object_link_zora_TLUT_00C578", 183, 183, zoraTunicBaseColor,
                             changedColor, zoraSkinColor);

        // Hat/head and pants
        ShadePaletteGradient("objects/object_link_zora/object_link_zora_TLUT_005000", 151, 177, zoraTunicBaseColor,
                             changedColor, zoraSkinColor);
        ShadePaletteGradient("objects/object_link_zora/object_link_zora_TLUT_005000", 179, 180, zoraTunicBaseColor,
                             changedColor, zoraSkinColor);
        ShadePaletteGradient("objects/object_link_zora/object_link_zora_TLUT_005000", 183, 183, zoraTunicBaseColor,
                             changedColor, zoraSkinColor);

        // Shield
        ShadePaletteGradient("objects/object_link_zora/object_link_zora_Tex_010228", 80, 511, zoraTunicBaseColor,
                             changedColor, zoraSkinColor);

        // Boomerangs
        ShadePaletteGradient("objects/gameplay_keep/gameplay_keep_Tex_0700B0", 80, 511, zoraTunicBaseColor,
                             changedColor, zoraSkinColor);
    },
    { kZoraTunicOption.colorCvar });

static RegisterShipInitFunc zoraTunicPatch(
    []() {
        if (!IsCustomZoraModelActive() && CVarGetInteger(kZoraTunicOption.colorChangedCvar, 0)) {
            return;
        }

        ShadePaletteRevert("objects/object_link_zora/object_link_zora_TLUT_00C578", 151, 177);
        ShadePaletteRevert("objects/object_link_zora/object_link_zora_TLUT_00C578", 179, 180);
        ShadePaletteRevert("objects/object_link_zora/object_link_zora_TLUT_00C578", 183, 183);

        ShadePaletteRevert("objects/object_link_zora/object_link_zora_TLUT_005000", 151, 177);
        ShadePaletteRevert("objects/object_link_zora/object_link_zora_TLUT_005000", 179, 180);
        ShadePaletteRevert("objects/object_link_zora/object_link_zora_TLUT_005000", 183, 183);

        ShadePaletteRevert("objects/object_link_zora/object_link_zora_Tex_010228", 80, 511);
        ShadePaletteRevert("objects/gameplay_keep/gameplay_keep_Tex_0700B0", 80, 511);
    },
    { kZoraTunicOption.colorChangedCvar });

// Player.FierceDeityTunic

Gfx fierceDeityTunic[] = {
    gsDPSetPrimColor(0, 0x80, 0, 0, 0, 255),
    gsDPPipeSync(),
    gsSPEndDisplayList(),
};

static const char* kFierceDeityClothTlutPath = "objects/object_link_boy/object_link_boy_TLUT_008128";
static const char* kFierceDeityHeadDlPath = "objects/object_link_boy/gLinkFierceDeityHeadDL";

static RegisterShipInitFunc fierceDeityTunicPatch(
    []() {
        if (!IsCustomFierceDeityModelActive() && CVarGetInteger(kFierceDeityTunicOption.colorChangedCvar, 0)) {
            ResourceMgr_PatchGfxByName("objects/object_link_boy/gLinkFierceDeityHatDL", "setPrim", 32,
                                       gsSPDisplayList(fierceDeityTunic));
            ResourceMgr_PatchGfxByName(kFierceDeityHeadDlPath, "setPrim", 87, gsSPDisplayList(fierceDeityTunic));
            ResourceMgr_PatchGfxByName("objects/object_link_boy/gLinkFierceDeityRightShoulderDL", "setPrim", 30,
                                       gsSPDisplayList(fierceDeityTunic));
            ResourceMgr_PatchGfxByName("objects/object_link_boy/gLinkFierceDeityLeftShoulderDL", "setPrim", 30,
                                       gsSPDisplayList(fierceDeityTunic));
            ResourceMgr_PatchGfxByName("objects/object_link_boy/gLinkFierceDeityRightThighDL", "setPrim", 30,
                                       gsSPDisplayList(fierceDeityTunic));
            ResourceMgr_PatchGfxByName("objects/object_link_boy/gLinkFierceDeityLeftThighDL", "setPrim1", 36,
                                       gsSPDisplayList(fierceDeityTunic));
            ResourceMgr_PatchGfxByName("objects/object_link_boy/gLinkFierceDeityLeftThighDL", "setPrim2", 118,
                                       gsSPDisplayList(fierceDeityTunic));
            ResourceMgr_PatchGfxByName(kFierceDeityHeadDlPath, "setPrimBackToWhite", 129, gsSPDisplayList(backToWhite));

            ShadePaletteWhite(kFierceDeityClothTlutPath, 0, 12, MODE_MAX);
        } else {
            ResourceMgr_UnpatchGfxByName("objects/object_link_boy/gLinkFierceDeityHatDL", "setPrim");
            ResourceMgr_UnpatchGfxByName(kFierceDeityHeadDlPath, "setPrim");
            ResourceMgr_UnpatchGfxByName("objects/object_link_boy/gLinkFierceDeityRightShoulderDL", "setPrim");
            ResourceMgr_UnpatchGfxByName("objects/object_link_boy/gLinkFierceDeityLeftShoulderDL", "setPrim");
            ResourceMgr_UnpatchGfxByName("objects/object_link_boy/gLinkFierceDeityRightThighDL", "setPrim");
            ResourceMgr_UnpatchGfxByName("objects/object_link_boy/gLinkFierceDeityLeftThighDL", "setPrim1");
            ResourceMgr_UnpatchGfxByName("objects/object_link_boy/gLinkFierceDeityLeftThighDL", "setPrim2");
            ResourceMgr_UnpatchGfxByName(kFierceDeityHeadDlPath, "setPrimBackToWhite");

            ShadePaletteRevert(kFierceDeityClothTlutPath, 0, 12);
        }
    },
    { kFierceDeityTunicOption.colorChangedCvar });

static RegisterShipInitFunc fierceDeityTunicColor(
    []() {
        Color_RGBA8 changedColor = CVarGetColor(kFierceDeityTunicOption.colorCvar, {});
        fierceDeityTunic[0] = gsDPSetPrimColor(0, 0x80, changedColor.r, changedColor.g, changedColor.b, 255);
    },
    { kFierceDeityTunicOption.colorCvar });

// HUD.Hearts

Gfx heartsColorDL[] = {
    gsDPSetPrimColor(0, 0, 0, 0, 0, 0),
    gsDPSetGrayscaleColor(0, 0, 0, 0),
    gsSPEndDisplayList(),
};

Gfx heartsEnvColorDL[] = {
    gsDPSetEnvColor(0, 0, 0, 0),
    gsSPEndDisplayList(),
};

static RegisterShipInitFunc heartsColorDLPatch(
    []() {
        if (CVarGetInteger(kHeartsOption.colorChangedCvar, 0)) {
            ResourceMgr_PatchGfxByName("objects/object_gi_heart/gGiRecoveryHeartDL", "enableGrayscale", 2,
                                       gsSPDisplayList(enableGrayscale));
            ResourceMgr_PatchGfxByName("objects/object_gi_heart/gGiRecoveryHeartDL", "setPrim", 5,
                                       gsSPDisplayList(heartsColorDL));
            ResourceMgr_PatchGfxByName("objects/object_gi_heart/gGiRecoveryHeartDL", "disableGrayscale", 36,
                                       gsSPBranchList(disableGrayscale));
            ResourceMgr_PatchGfxByName("objects/gameplay_keep/gHeartPieceInteriorDL", "setPrim", 56,
                                       gsSPDisplayList(heartsColorDL));
            ResourceMgr_PatchGfxByName("objects/object_gi_hearts/gGiHeartPieceDL", "setPrim", 2,
                                       gsSPDisplayList(heartsColorDL));
            ResourceMgr_PatchGfxByName("objects/object_gi_hearts/gGiHeartPieceDL", "setEnv", 6,
                                       gsSPDisplayList(heartsEnvColorDL));
            ResourceMgr_PatchGfxByName("objects/object_gi_hearts/gGiHeartContainerDL", "setPrim", 2,
                                       gsSPDisplayList(heartsColorDL));
            ResourceMgr_PatchGfxByName("objects/object_gi_hearts/gGiHeartContainerDL", "setEnv", 6,
                                       gsSPDisplayList(heartsEnvColorDL));
            ResourceMgr_PatchGfxByName("objects/object_gi_bottle_red/gGiRedPotionBottleDL", "setPrim", 5,
                                       gsSPDisplayList(heartsColorDL));
            ResourceMgr_PatchGfxByName("objects/object_gi_bottle_red/gGiRedPotionBottleDL", "setEnv", 6,
                                       gsSPDisplayList(heartsEnvColorDL));
            ResourceMgr_PatchGfxByName("objects/object_gi_liquid/gGiPotionContainerRedPotColorDL", "setPrim", 3,
                                       gsSPDisplayList(heartsColorDL));
            ResourceMgr_PatchGfxByName("objects/object_gi_liquid/gGiPotionContainerRedPotColorDL", "setEnv", 4,
                                       gsSPDisplayList(heartsEnvColorDL));
            ResourceMgr_PatchGfxByName("objects/object_gi_liquid/gGiPotionContainerRedPatternColorDL", "setPrim", 3,
                                       gsSPDisplayList(heartsColorDL));
            ResourceMgr_PatchGfxByName("objects/object_gi_liquid/gGiPotionContainerRedPatternColorDL", "setEnv", 4,
                                       gsSPDisplayList(heartsEnvColorDL));
        } else {
            ResourceMgr_UnpatchGfxByName("objects/object_gi_heart/gGiRecoveryHeartDL", "enableGrayscale");
            ResourceMgr_UnpatchGfxByName("objects/object_gi_heart/gGiRecoveryHeartDL", "setPrim");
            ResourceMgr_UnpatchGfxByName("objects/object_gi_heart/gGiRecoveryHeartDL", "disableGrayscale");
            ResourceMgr_UnpatchGfxByName("objects/gameplay_keep/gHeartPieceInteriorDL", "setPrim");
            ResourceMgr_UnpatchGfxByName("objects/object_gi_hearts/gGiHeartPieceDL", "setPrim");
            ResourceMgr_UnpatchGfxByName("objects/object_gi_hearts/gGiHeartPieceDL", "setEnv");
            ResourceMgr_UnpatchGfxByName("objects/object_gi_hearts/gGiHeartContainerDL", "setPrim");
            ResourceMgr_UnpatchGfxByName("objects/object_gi_hearts/gGiHeartContainerDL", "setEnv");
            ResourceMgr_UnpatchGfxByName("objects/object_gi_bottle_red/gGiRedPotionBottleDL", "setPrim");
            ResourceMgr_UnpatchGfxByName("objects/object_gi_bottle_red/gGiRedPotionBottleDL", "setEnv");
            ResourceMgr_UnpatchGfxByName("objects/object_gi_liquid/gGiPotionContainerRedPotColorDL", "setPrim");
            ResourceMgr_UnpatchGfxByName("objects/object_gi_liquid/gGiPotionContainerRedPotColorDL", "setEnv");
            ResourceMgr_UnpatchGfxByName("objects/object_gi_liquid/gGiPotionContainerRedPatternColorDL", "setPrim");
            ResourceMgr_UnpatchGfxByName("objects/object_gi_liquid/gGiPotionContainerRedPatternColorDL", "setEnv");

            ShadeRGBA16Revert("objects/gameplay_keep/gDropRecoveryHeartTex", 0, 1023);
        }
    },
    { kHeartsOption.colorChangedCvar });

static RegisterShipInitFunc heartsColorDLUpdate(
    []() {
        Color_RGBA8 primColor = CosmeticEditor_GetChangedColor(0, 0, 0, 0, "HUD.Hearts");
        heartsColorDL[0] = gsDPSetPrimColor(0, 0, primColor.r, primColor.g, primColor.b, 255);
        heartsColorDL[1] = gsDPSetGrayscaleColor(primColor.r, primColor.g, primColor.b, 255);

        Color_RGBA8 envColor =
            CosmeticEditor_GetChangedColorEx(0, 0, 0, 0, "HUD.Hearts", COSMETIC_COLOR_MODE_DIVIDE, 2.0f);
        heartsEnvColorDL[0] = gsDPSetEnvColor(envColor.r, envColor.g, envColor.b, 255);

        if (!CVarGetInteger(kHeartsOption.colorChangedCvar, 0)) {
            return;
        }

        Color_RGBA8 changedColor = CVarGetColor(kHeartsOption.colorCvar, {});
        ShadeRGBA16NewBase("objects/gameplay_keep/gDropRecoveryHeartTex", 0, 1023, changedColor, MODE_AVG);
    },
    { kHeartsOption.colorCvar });

// HUD.Magic

Gfx magicColorDL[] = {
    gsDPSetPrimColor(0, 0, 0, 0, 0, 0),
    gsDPSetGrayscaleColor(0, 0, 0, 0),
    gsSPEndDisplayList(),
};

Gfx magicEnvColorDL[] = {
    gsDPSetEnvColor(0, 0, 0, 0),
    gsSPEndDisplayList(),
};

static RegisterShipInitFunc magicColorDLPatch(
    []() {
        if (CVarGetInteger(kMagicOption.colorChangedCvar, 0)) {
            ResourceMgr_PatchGfxByName("objects/object_gi_magicpot/gGiMagicJarLargeDL", "setPrim", 31,
                                       gsSPDisplayList(magicColorDL));
            ResourceMgr_PatchGfxByName("objects/object_gi_magicpot/gGiMagicJarLargeDL", "setEnv", 32,
                                       gsSPDisplayList(magicEnvColorDL));
            ResourceMgr_PatchGfxByName("objects/object_gi_magicpot/gGiMagicJarSmallDL", "setPrim", 31,
                                       gsSPDisplayList(magicColorDL));
            ResourceMgr_PatchGfxByName("objects/object_gi_magicpot/gGiMagicJarSmallDL", "setEnv", 32,
                                       gsSPDisplayList(magicEnvColorDL));
            ResourceMgr_PatchGfxByName("objects/object_gi_liquid/gGiPotionContainerGreenPotColorDL", "setPrim", 3,
                                       gsSPDisplayList(magicColorDL));
            ResourceMgr_PatchGfxByName("objects/object_gi_liquid/gGiPotionContainerGreenPotColorDL", "setEnv", 4,
                                       gsSPDisplayList(magicEnvColorDL));
            ResourceMgr_PatchGfxByName("objects/object_gi_liquid/gGiPotionContainerGreenPatternColorDL", "setPrim", 3,
                                       gsSPDisplayList(magicColorDL));
            ResourceMgr_PatchGfxByName("objects/object_gi_liquid/gGiPotionContainerGreenPatternColorDL", "setEnv", 4,
                                       gsSPDisplayList(magicEnvColorDL));
        } else {
            ResourceMgr_UnpatchGfxByName("objects/object_gi_magicpot/gGiMagicJarLargeDL", "setPrim");
            ResourceMgr_UnpatchGfxByName("objects/object_gi_magicpot/gGiMagicJarLargeDL", "setEnv");
            ResourceMgr_UnpatchGfxByName("objects/object_gi_magicpot/gGiMagicJarSmallDL", "setPrim");
            ResourceMgr_UnpatchGfxByName("objects/object_gi_magicpot/gGiMagicJarSmallDL", "setEnv");
            ResourceMgr_UnpatchGfxByName("objects/object_gi_liquid/gGiPotionContainerGreenPotColorDL", "setPrim");
            ResourceMgr_UnpatchGfxByName("objects/object_gi_liquid/gGiPotionContainerGreenPotColorDL", "setEnv");
            ResourceMgr_UnpatchGfxByName("objects/object_gi_liquid/gGiPotionContainerGreenPatternColorDL", "setPrim");
            ResourceMgr_UnpatchGfxByName("objects/object_gi_liquid/gGiPotionContainerGreenPatternColorDL", "setEnv");

            ShadeRGBA16Revert("objects/gameplay_keep/gDropMagicSmallTex", 0, 1023);
            ShadeRGBA16Revert("objects/gameplay_keep/gDropMagicLargeTex", 0, 1023);
        }
    },
    { kMagicOption.colorChangedCvar });

static RegisterShipInitFunc magicColorDLUpdate(
    []() {
        Color_RGBA8 primColor = CosmeticEditor_GetChangedColor(0, 0, 0, 0, "HUD.Magic");
        magicColorDL[0] = gsDPSetPrimColor(0, 0, primColor.r, primColor.g, primColor.b, 255);
        magicColorDL[1] = gsDPSetGrayscaleColor(primColor.r, primColor.g, primColor.b, 255);

        Color_RGBA8 envColor =
            CosmeticEditor_GetChangedColorEx(0, 0, 0, 0, "HUD.Magic", COSMETIC_COLOR_MODE_DIVIDE, 2.0f);
        magicEnvColorDL[0] = gsDPSetEnvColor(envColor.r, envColor.g, envColor.b, 255);

        if (!CVarGetInteger(kMagicOption.colorChangedCvar, 0)) {
            return;
        }

        Color_RGBA8 changedColor = CVarGetColor(kMagicOption.colorCvar, {});
        ShadeRGBA16NewBase("objects/gameplay_keep/gDropMagicSmallTex", 0, 1023, changedColor, MODE_AVG);
        ShadeRGBA16NewBase("objects/gameplay_keep/gDropMagicLargeTex", 0, 1023, changedColor, MODE_AVG);
    },
    { kMagicOption.colorCvar });

// Player.MirrorShield

static const char* sMirrorShieldTrimDLs[] = {
    "objects/object_link_child/object_link_child_DL_0161C8",
    "objects/object_link_child/object_link_child_DL_016270",
    "objects/object_link_child/object_link_child_DL_016320",
};

static const char* sMirrorShieldMirrorDL = "objects/object_link_child/object_link_child_DL_0163D0";
static const int kMirrorShieldPrimIndex = 5;
// With metallic materials, when the prim goes too high it overflows causing some really ugly artifacts,
// this is also an issue on OoT with the master sword, we should probably back port this fix
static const uint8_t kMetallicPrimCeiling = 128;

static Color_RGBA8 ScaleToMetallicCeiling(Color_RGBA8 color) {
    uint8_t maxChannel = MAX(MAX(color.r, color.g), color.b);
    if (maxChannel <= kMetallicPrimCeiling) {
        return color;
    }
    f32 scale = (f32)kMetallicPrimCeiling / maxChannel;
    return { (uint8_t)(color.r * scale + 0.5f), (uint8_t)(color.g * scale + 0.5f), (uint8_t)(color.b * scale + 0.5f),
             color.a };
}

Gfx mirrorShieldTrim[] = {
    gsDPSetPrimColor(0, 0xFF, 0, 0, 0, 255),
    gsSPEndDisplayList(),
};

Gfx mirrorShieldMirror[] = {
    gsDPSetPrimColor(0, 0xFF, 0, 0, 0, 255),
    gsSPEndDisplayList(),
};

static RegisterShipInitFunc mirrorShieldPatch(
    []() {
        for (const char* path : sMirrorShieldTrimDLs) {
            if (CVarGetInteger(kMirrorShieldOption.colorChangedCvar, 0)) {
                ResourceMgr_PatchGfxByName(path, "setPrim", kMirrorShieldPrimIndex, gsSPDisplayList(mirrorShieldTrim));
            } else {
                ResourceMgr_UnpatchGfxByName(path, "setPrim");
            }
        }
    },
    { kMirrorShieldOption.colorChangedCvar });

static RegisterShipInitFunc mirrorShieldColor(
    []() {
        Color_RGBA8 changedColor = ScaleToMetallicCeiling(CVarGetColor(kMirrorShieldOption.colorCvar, {}));
        mirrorShieldTrim[0] = gsDPSetPrimColor(0, 0xFF, changedColor.r, changedColor.g, changedColor.b, 255);
    },
    { kMirrorShieldOption.colorCvar });

// Player.MirrorShieldMirror

static RegisterShipInitFunc mirrorShieldMirrorPatch(
    []() {
        if (CVarGetInteger(kMirrorShieldMirrorOption.colorChangedCvar, 0)) {
            ResourceMgr_PatchGfxByName(sMirrorShieldMirrorDL, "setPrim", kMirrorShieldPrimIndex,
                                       gsSPDisplayList(mirrorShieldMirror));
        } else {
            ResourceMgr_UnpatchGfxByName(sMirrorShieldMirrorDL, "setPrim");
        }
    },
    { kMirrorShieldMirrorOption.colorChangedCvar });

static RegisterShipInitFunc mirrorShieldMirrorColor(
    []() {
        Color_RGBA8 changedColor = ScaleToMetallicCeiling(CVarGetColor(kMirrorShieldMirrorOption.colorCvar, {}));
        mirrorShieldMirror[0] = gsDPSetPrimColor(0, 0xFF, changedColor.r, changedColor.g, changedColor.b, 255);
    },
    { kMirrorShieldMirrorOption.colorCvar });

// Collectibles.Rupee*

static Color_RGBA8 RupeeEnvWithoutLodBoost(Color_RGBA8 prim, Color_RGBA8 env) {
    auto fold = [](uint8_t prim8, uint8_t env8) {
        f32 p = prim8 / 255.0f;
        f32 e = env8 / 255.0f;
        return (uint8_t)CLAMP((e - 0.5f * p * (p - e)) * 255.0f + 0.5f, 0.0f, 255.0f);
    };
    return { fold(prim.r, env.r), fold(prim.g, env.g), fold(prim.b, env.b), env.a };
}

typedef struct {
    const char* cosmeticId;
    const char* texture;
    const char* innerColorDL;
    const char* outerColorDL;
} RupeeCosmetic;

static const RupeeCosmetic sRupeeCosmetics[] = {
    { "Collectibles.RupeeGreen", "objects/gameplay_keep/gRupeeGreenTex",
      "objects/object_gi_rupy/gGiGreenRupeeInnerColorDL", "objects/object_gi_rupy/gGiGreenRupeeOuterColorDL" },
    { "Collectibles.RupeeBlue", "objects/gameplay_keep/gRupeeBlueTex",
      "objects/object_gi_rupy/gGiBlueRupeeInnerColorDL", "objects/object_gi_rupy/gGiBlueRupeeOuterColorDL" },
    { "Collectibles.RupeeRed", "objects/gameplay_keep/gRupeeRedTex", "objects/object_gi_rupy/gGiRedRupeeInnerColorDL",
      "objects/object_gi_rupy/gGiRedRupeeOuterColorDL" },
    { "Collectibles.RupeePurple", "objects/gameplay_keep/gRupeePurpleTex",
      "objects/object_gi_rupy/gGiPurpleRupeeInnerColorDL", "objects/object_gi_rupy/gGiPurpleRupeeOuterColorDL" },
    { "Collectibles.RupeeOrange", "objects/gameplay_keep/gRupeeOrangeTex",
      "objects/object_gi_rupy/gGiGoldRupeeInnerColorDL", "objects/object_gi_rupy/gGiGoldRupeeOuterColorDL" },
    { "Collectibles.RupeeSilver", "objects/gameplay_keep/gRupeeSilverTex",
      "objects/object_gi_rupy/gGiSilverRupeeInnerColorDL", "objects/object_gi_rupy/gGiSilverRupeeOuterColorDL" },
};

static void ApplyRupeeCosmetic(const RupeeCosmetic& rupee) {
    const CosmeticOption& option = cosmeticOptions.at(rupee.cosmeticId);

    if (!CVarGetInteger(option.colorChangedCvar, 0)) {
        ShadeRGBA16Revert(rupee.texture, 0, 15);
        ResourceMgr_UnpatchGfxByName(rupee.innerColorDL, "setPrim");
        ResourceMgr_UnpatchGfxByName(rupee.innerColorDL, "setEnv");
        ResourceMgr_UnpatchGfxByName(rupee.outerColorDL, "setPrim");
        ResourceMgr_UnpatchGfxByName(rupee.outerColorDL, "setEnv");
        return;
    }

    Color_RGBA8 changedColor = CVarGetColor(option.colorCvar, option.defaultColor);
    ShadeRGBA16NewBase(rupee.texture, 0, 15, changedColor, MODE_AVG);

    Color_RGBA8 innerPrim =
        CosmeticEditor_GetChangedColorEx(0, 0, 0, 255, rupee.cosmeticId, COSMETIC_COLOR_MODE_ADD, 55.0f);
    Color_RGBA8 innerEnv = RupeeEnvWithoutLodBoost(
        innerPrim, CosmeticEditor_GetChangedColorEx(0, 0, 0, 255, rupee.cosmeticId, COSMETIC_COLOR_MODE_DIVIDE, 2.0f));
    Color_RGBA8 outerPrim =
        CosmeticEditor_GetChangedColorEx(0, 0, 0, 255, rupee.cosmeticId, COSMETIC_COLOR_MODE_ADD, 120.0f);
    Color_RGBA8 outerEnv = RupeeEnvWithoutLodBoost(outerPrim, changedColor);

    ResourceMgr_PatchGfxByName(rupee.innerColorDL, "setPrim", 3,
                               gsDPSetPrimColor(0, 0, innerPrim.r, innerPrim.g, innerPrim.b, 255));
    ResourceMgr_PatchGfxByName(rupee.innerColorDL, "setEnv", 4,
                               gsDPSetEnvColor(innerEnv.r, innerEnv.g, innerEnv.b, 255));
    ResourceMgr_PatchGfxByName(rupee.outerColorDL, "setPrim", 3,
                               gsDPSetPrimColor(0, 0, outerPrim.r, outerPrim.g, outerPrim.b, 255));
    ResourceMgr_PatchGfxByName(rupee.outerColorDL, "setEnv", 4,
                               gsDPSetEnvColor(outerEnv.r, outerEnv.g, outerEnv.b, 255));
}

// World.TingleBalloon

static const uint32_t kTingleBalloonLastPixel = (16 * 16) - 1;

static RegisterShipInitFunc tingleBalloonColor(
    []() {
        const CosmeticOption& option = cosmeticOptions.at("World.TingleBalloon");

        if (CVarGetInteger(option.colorChangedCvar, 0)) {
            Color_RGBA8 changedColor = CVarGetColor(option.colorCvar, option.defaultColor);
            ShadeRGBA16NewBase("objects/object_bal/gTingleBalloonTex", 0, kTingleBalloonLastPixel, changedColor,
                               MODE_AVG);
        } else {
            ShadeRGBA16Revert("objects/object_bal/gTingleBalloonTex", 0, kTingleBalloonLastPixel);
        }
    },
    { CVAR_COSMETIC_COLOR("World.TingleBalloon"), CVAR_COSMETIC_CHANGED("World.TingleBalloon") });

// World.EponaCoat

// saturation difference between the hair and the body
static const f32 kEponaCoatMinSaturation = 0.35f;

static RegisterShipInitFunc eponaCoatColorFunc(
    []() {
        const CosmeticOption& option = cosmeticOptions.at("World.EponaCoat");

        if (CVarGetInteger(option.colorChangedCvar, 0)) {
            Color_RGBA8 changedColor = CVarGetColor(option.colorCvar, option.defaultColor);

            ShadeRGBA16Recolor("objects/object_horse_link_child/object_horse_link_child_Tex_00DCF0", 0, 1023,
                               changedColor, kEponaCoatMinSaturation); // neck
            ShadeRGBA16Recolor("objects/object_horse_link_child/object_horse_link_child_Tex_00DAF0", 0, 255,
                               changedColor, kEponaCoatMinSaturation);
            ShadeRGBA16Recolor("objects/object_horse_link_child/object_horse_link_child_Tex_002788", 0, 255,
                               changedColor, kEponaCoatMinSaturation); // head
            ShadeRGBA16Recolor("objects/object_horse_link_child/object_horse_link_child_Tex_002168", 0, 255,
                               changedColor, kEponaCoatMinSaturation); // eyes
            ShadeRGBA16Recolor("objects/object_horse_link_child/object_horse_link_child_Tex_002568", 0, 15,
                               changedColor, kEponaCoatMinSaturation); // neck strip
            ShadeRGBA16Recolor("objects/object_horse_link_child/object_horse_link_child_Tex_002588", 0, 255,
                               changedColor, kEponaCoatMinSaturation);
            ShadeRGBA16Recolor("objects/object_horse_link_child/object_horse_link_child_Tex_002368", 0, 255,
                               changedColor, kEponaCoatMinSaturation);

            ShadePaletteTintMatchingHue("objects/object_horse_link_child/gEponaTLUT", 0, 255, { 214, 82, 8, 255 },
                                        changedColor);
        } else {
            ShadeRGBA16Revert("objects/object_horse_link_child/object_horse_link_child_Tex_00DCF0", 0, 1023);
            ShadeRGBA16Revert("objects/object_horse_link_child/object_horse_link_child_Tex_00DAF0", 0, 255);
            ShadeRGBA16Revert("objects/object_horse_link_child/object_horse_link_child_Tex_002788", 0, 255);
            ShadeRGBA16Revert("objects/object_horse_link_child/object_horse_link_child_Tex_002168", 0, 255);
            ShadeRGBA16Revert("objects/object_horse_link_child/object_horse_link_child_Tex_002568", 0, 15);
            ShadeRGBA16Revert("objects/object_horse_link_child/object_horse_link_child_Tex_002588", 0, 255);
            ShadeRGBA16Revert("objects/object_horse_link_child/object_horse_link_child_Tex_002368", 0, 255);

            ShadePaletteTintRevert("objects/object_horse_link_child/gEponaTLUT", 0, 255);
        }
    },
    { CVAR_COSMETIC_COLOR("World.EponaCoat"), CVAR_COSMETIC_CHANGED("World.EponaCoat") });

// World.GoldSkulltula

static RegisterShipInitFunc goldSkulltulaColor(
    []() {
        const CosmeticOption& option = cosmeticOptions.at("World.GoldSkulltula");

        if (CVarGetInteger(option.colorChangedCvar, 0)) {
            Color_RGBA8 prim = CVarGetColor(option.colorCvar, option.defaultColor);
            Color_RGBA8 env = CosmeticEditor_GetChangedColorEx(0, 0, 0, 255, "World.GoldSkulltula",
                                                               COSMETIC_COLOR_MODE_DIVIDE, 12.0f);

            ResourceMgr_PatchGfxByName("objects/object_st/object_st_DL_003FB0", "setPrim", 118,
                                       gsDPSetPrimColor(0, 0, prim.r, prim.g, prim.b, 255));
            ResourceMgr_PatchGfxByName("objects/object_st/object_st_DL_003FB0", "setEnv", 119,
                                       gsDPSetEnvColor(env.r, env.g, env.b, 255));
            ResourceMgr_PatchGfxByName("objects/object_st/object_st_DL_0043D8", "setPrim", 61,
                                       gsDPSetPrimColor(0, 0, prim.r, prim.g, prim.b, 255));
            ResourceMgr_PatchGfxByName("objects/object_st/object_st_DL_0043D8", "setEnv", 62,
                                       gsDPSetEnvColor(env.r, env.g, env.b, 255));
            ResourceMgr_PatchGfxByName("objects/object_st/object_st_DL_001C30", "setPrim", 22,
                                       gsDPSetPrimColor(0, 0, prim.r, prim.g, prim.b, 255));
            ResourceMgr_PatchGfxByName("objects/object_st/object_st_DL_001A40", "setPrim", 17,
                                       gsDPSetPrimColor(0, 0, prim.r, prim.g, prim.b, 255));
        } else {
            ResourceMgr_UnpatchGfxByName("objects/object_st/object_st_DL_003FB0", "setPrim");
            ResourceMgr_UnpatchGfxByName("objects/object_st/object_st_DL_003FB0", "setEnv");
            ResourceMgr_UnpatchGfxByName("objects/object_st/object_st_DL_0043D8", "setPrim");
            ResourceMgr_UnpatchGfxByName("objects/object_st/object_st_DL_0043D8", "setEnv");
            ResourceMgr_UnpatchGfxByName("objects/object_st/object_st_DL_001C30", "setPrim");
            ResourceMgr_UnpatchGfxByName("objects/object_st/object_st_DL_001A40", "setPrim");
        }
    },
    { CVAR_COSMETIC_COLOR("World.GoldSkulltula"), CVAR_COSMETIC_CHANGED("World.GoldSkulltula") });

static RegisterShipInitFunc rupeeShine(
    []() {
        if (CVarGetInteger(kSillyDullRupeesCvar, 0)) {
            ResourceMgr_PatchGfxByName("objects/object_gi_rupy/gGiRupeeOuterDL", "dullRupees", 0, gsSPEndDisplayList());
        } else {
            ResourceMgr_UnpatchGfxByName("objects/object_gi_rupy/gGiRupeeOuterDL", "dullRupees");
        }
    },
    { kSillyDullRupeesCvar });

static RegisterShipInitFunc rupeeGreenColor([]() { ApplyRupeeCosmetic(sRupeeCosmetics[0]); },
                                            { CVAR_COSMETIC_COLOR("Collectibles.RupeeGreen"),
                                              CVAR_COSMETIC_CHANGED("Collectibles.RupeeGreen") });

static RegisterShipInitFunc rupeeBlueColor([]() { ApplyRupeeCosmetic(sRupeeCosmetics[1]); },
                                           { CVAR_COSMETIC_COLOR("Collectibles.RupeeBlue"),
                                             CVAR_COSMETIC_CHANGED("Collectibles.RupeeBlue") });

static RegisterShipInitFunc rupeeRedColor([]() { ApplyRupeeCosmetic(sRupeeCosmetics[2]); },
                                          { CVAR_COSMETIC_COLOR("Collectibles.RupeeRed"),
                                            CVAR_COSMETIC_CHANGED("Collectibles.RupeeRed") });

static RegisterShipInitFunc rupeePurpleColor([]() { ApplyRupeeCosmetic(sRupeeCosmetics[3]); },
                                             { CVAR_COSMETIC_COLOR("Collectibles.RupeePurple"),
                                               CVAR_COSMETIC_CHANGED("Collectibles.RupeePurple") });

static RegisterShipInitFunc rupeeOrangeColor([]() { ApplyRupeeCosmetic(sRupeeCosmetics[4]); },
                                             { CVAR_COSMETIC_COLOR("Collectibles.RupeeOrange"),
                                               CVAR_COSMETIC_CHANGED("Collectibles.RupeeOrange") });

static RegisterShipInitFunc rupeeSilverColor([]() { ApplyRupeeCosmetic(sRupeeCosmetics[5]); },
                                             { CVAR_COSMETIC_COLOR("Collectibles.RupeeSilver"),
                                               CVAR_COSMETIC_CHANGED("Collectibles.RupeeSilver") });
