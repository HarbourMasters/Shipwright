#include "CosmeticsEditor.h"
#include <libultraship/ImGuiImpl.h>

#include <string>
#include <libultraship/Cvar.h>
#include <random>
#include <math.h>
#include <algorithm>
#include <ultra64/types.h>
#include "../randomizer/3drando/random.hpp"

#include "../../UIWidgets.hpp"

extern "C" {
#include <z64.h>
#include "macros.h"
extern GlobalContext* gGlobalCtx;
#include "objects/object_link_boy/object_link_boy.h"
#include "objects/object_link_child/object_link_child.h"
#include "objects/object_gi_shield_3/object_gi_shield_3.h"
#include "objects/object_gi_heart/object_gi_heart.h"
#include "objects/object_gi_bow/object_gi_bow.h"
#include "objects/object_gi_bracelet/object_gi_bracelet.h"
#include "objects/object_gi_rupy/object_gi_rupy.h"
#include "objects/object_gi_magicpot/object_gi_magicpot.h"
#include "objects/object_gi_gloves/object_gi_gloves.h"
#include "objects/object_gi_hammer/object_gi_hammer.h"
#include "objects/object_gi_sutaru/object_gi_sutaru.h"
#include "objects/object_st/object_st.h"
#include "objects/object_gi_boomerang/object_gi_boomerang.h"
#include "objects/object_gi_liquid/object_gi_liquid.h"
#include "objects/object_gi_bow/object_gi_bow.h"
#include "objects/object_gi_hearts/object_gi_hearts.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/object_gi_sword_1/object_gi_sword_1.h"
#include "objects/object_gi_longsword/object_gi_longsword.h"
#include "objects/object_gi_clothes/object_gi_clothes.h"
#include "objects/object_toki_objects/object_toki_objects.h"
void ResourceMgr_PatchGfxByName(const char* path, const char* patchName, int index, Gfx instruction);
void ResourceMgr_UnpatchGfxByName(const char* path, const char* patchName);
}

void ApplyOrResetCustomGfxPatches(bool rainbowTick);

typedef enum {
    BOX_LINK,
    BOX_MIRRORSHIELD,
    BOX_SWORDS,
    BOX_GLOVES,
    BOX_EQUIPMENT,
    BOX_CONSUMABLE,
    BOX_HUD,
    BOX_NPC,
    BOX_WORLD,
    BOX_EFFECTS,
} CosmeticBox;

std::map<CosmeticBox, const char*> boxLabels = {
    { BOX_LINK, "Link" },
    { BOX_MIRRORSHIELD, "Mirror Shield" },
    { BOX_SWORDS, "Swords" },
    { BOX_GLOVES, "Gloves" },
    { BOX_EQUIPMENT, "Equipment" },
    { BOX_CONSUMABLE, "Consumables" },
    { BOX_HUD, "HUD" },
    { BOX_NPC, "NPCs" },
    { BOX_WORLD, "World" },
    { BOX_EFFECTS, "Effects" },
};

typedef struct {
    const char* cvar;
    const char* rainbowCvar;
    const char* lockedCvar;
    const char* changedCvar;
    std::string label;
    CosmeticBox box;
    ImVec4 currentColor;
    ImVec4 defaultColor;
    bool supportsAlpha;
    bool supportsRainbow;
    bool advancedOption;
} CosmeticOption;

#define COSMETIC_OPTION(id, label, box, defaultColor, supportsAlpha, supportsRainbow, advancedOption) \
    { id, {"gCosmetics." id, "gCosmetics." id ".Rainbow", "gCosmetics." id ".Locked", "gCosmetics." id ".Changed", label, box, defaultColor, defaultColor, supportsAlpha, supportsRainbow, advancedOption} }

std::map<const char*, CosmeticOption> cosmeticOptions = {
    COSMETIC_OPTION("Link_KokiriTunic",              "Kokiri Tunic",         BOX_LINK,         ImVec4( 30, 105,  27, 255), false, true, false),
    COSMETIC_OPTION("Link_GoronTunic",               "Goron Tunic",          BOX_LINK,         ImVec4(100,  20,   0, 255), false, true, false),
    COSMETIC_OPTION("Link_ZoraTunic",                "Zora Tunic",           BOX_LINK,         ImVec4(  0,  60, 100, 255), false, true, false),
    COSMETIC_OPTION("Link_Hair",                     "Hair",                 BOX_LINK,         ImVec4(255, 173,  27, 255), false, true, true),
    COSMETIC_OPTION("Link_Linen",                    "Linen",                BOX_LINK,         ImVec4(255, 255, 255, 255), false, true, true),
    COSMETIC_OPTION("Link_Boots",                    "Boots",                BOX_LINK,         ImVec4( 93,  44,  18, 255), false, true, true),

    COSMETIC_OPTION("MirrorShield_Body",             "Body",                 BOX_MIRRORSHIELD, ImVec4(215,   0,   0, 255), false, true, false),
    COSMETIC_OPTION("MirrorShield_Mirror",           "Mirror",               BOX_MIRRORSHIELD, ImVec4(255, 255, 255, 255), false, true, true),
    COSMETIC_OPTION("MirrorShield_Emblem",           "Emblem",               BOX_MIRRORSHIELD, ImVec4(205, 225, 255, 255), false, true, true),

    COSMETIC_OPTION("Swords_KokiriBlade",            "Kokiri Sword Blade",   BOX_SWORDS,       ImVec4(255, 255, 255, 255), false, true, false),
    COSMETIC_OPTION("Swords_MasterBlade",            "Master Sword Blade",   BOX_SWORDS,       ImVec4(255, 255, 255, 255), false, true, false),
    COSMETIC_OPTION("Swords_MasterHilt",             "Master Sword Hilt",    BOX_SWORDS,       ImVec4( 80,  80, 168, 255), false, true, true),
    COSMETIC_OPTION("Swords_BiggoronBlade",          "Biggoron Sword Blade", BOX_SWORDS,       ImVec4(255, 255, 255, 255), false, true, false),
    COSMETIC_OPTION("Swords_BiggoronHilt",           "Biggoron Sword Hilt",  BOX_SWORDS,       ImVec4( 80,  80, 168, 255), false, true, true),

    COSMETIC_OPTION("Gloves_GoronBracelet",          "Goron Bracelet",       BOX_GLOVES,       ImVec4(255, 255, 170, 255), false, true, false),
    COSMETIC_OPTION("Gloves_SilverGauntlets",        "Silver Gauntlets",     BOX_GLOVES,       ImVec4(255, 255, 255, 255), false, true, false),
    COSMETIC_OPTION("Gloves_GoldenGauntlets",        "Golden Gauntlets",     BOX_GLOVES,       ImVec4(254, 207,  15, 255), false, true, false),
    COSMETIC_OPTION("Gloves_GauntletsGem",           "Gauntlets Gem",        BOX_GLOVES,       ImVec4(255,  60, 100, 255), false, true, true),
    
    COSMETIC_OPTION("Equipment_BoomerangBody",       "Boomerang Body",       BOX_EQUIPMENT,    ImVec4(160, 100,   0, 255), false, true, false),
    COSMETIC_OPTION("Equipment_BoomerangGem",        "Boomerang Gem",        BOX_EQUIPMENT,    ImVec4(255,  50, 150, 255), false, true, true),
    COSMETIC_OPTION("Equipment_HammerHead",          "Hammer Head",          BOX_EQUIPMENT,    ImVec4(155, 192, 201, 255), false, true, false),
    COSMETIC_OPTION("Equipment_HammerHandle",        "Hammer Handle",        BOX_EQUIPMENT,    ImVec4(110,  60,   0, 255), false, true, true),
    COSMETIC_OPTION("Equipment_HookshotChain",       "Hookshot Chain",       BOX_EQUIPMENT,    ImVec4(255, 255, 255, 255), false, true, true),
    COSMETIC_OPTION("Equipment_HookshotReticle",     "Hookshot Reticle",     BOX_EQUIPMENT,    ImVec4(255, 255, 255, 255), false, true, true),
    COSMETIC_OPTION("Equipment_HookshotTip",         "Hookshot Tip",         BOX_EQUIPMENT,    ImVec4(255, 255, 255, 255), false, true, false),
    COSMETIC_OPTION("Equipment_BowTips",             "Bow Tips",             BOX_EQUIPMENT,    ImVec4(200,   0,   0, 255), false, true, true),
    COSMETIC_OPTION("Equipment_BowString",           "Bow String",           BOX_EQUIPMENT,    ImVec4(255, 255, 255, 255), false, true, true),
    COSMETIC_OPTION("Equipment_BowBody",             "Bow Body",             BOX_EQUIPMENT,    ImVec4(140,  90,  10, 255), false, true, false),
    COSMETIC_OPTION("Equipment_BowHandle",           "Bow Handle",           BOX_EQUIPMENT,    ImVec4( 50, 150, 255, 255), false, true, true),
    // Todo: Slingshot
    // Todo: Hookshot

    COSMETIC_OPTION("Consumable_Hearts",            "Hearts",                BOX_CONSUMABLE,   ImVec4(255,  70,  50, 255), false, true, false),
    COSMETIC_OPTION("Consumable_DDHearts",          "DD Hearts",             BOX_CONSUMABLE,   ImVec4(200,   0,   0, 255), false, true, false),
    COSMETIC_OPTION("Consumable_DDBorder",          "DD Border",             BOX_CONSUMABLE,   ImVec4(255, 255, 255, 255), false, true, true),
    COSMETIC_OPTION("Consumable_Magic",             "Magic",                 BOX_CONSUMABLE,   ImVec4(  0, 200,   0, 255), false, true, false),
    COSMETIC_OPTION("Consumable_MagicActive",       "Magic Active",          BOX_CONSUMABLE,   ImVec4(250, 250,   0, 255), false, true, true),
    COSMETIC_OPTION("Consumable_MagicBorder",       "Magic Border",          BOX_CONSUMABLE,   ImVec4(255, 255, 255, 255), false, false, true),
    COSMETIC_OPTION("Consumable_MagicBorderActive", "Magic Border Active",   BOX_CONSUMABLE,   ImVec4(255, 255, 255, 255), false, false, true),
    COSMETIC_OPTION("Consumable_GreenRupee",        "Green Rupee",           BOX_CONSUMABLE,   ImVec4( 50, 255,  50, 255), false, true, true),
    COSMETIC_OPTION("Consumable_BlueRupee",         "Blue Rupee",            BOX_CONSUMABLE,   ImVec4( 50,  50, 255, 255), false, true, true),
    COSMETIC_OPTION("Consumable_RedRupee",          "Red Rupee",             BOX_CONSUMABLE,   ImVec4(255,  50,  50, 255), false, true, true),
    COSMETIC_OPTION("Consumable_PurpleRupee",       "Purple Rupee",          BOX_CONSUMABLE,   ImVec4(150,  50, 255, 255), false, true, true),
    COSMETIC_OPTION("Consumable_GoldRupee",         "Gold Rupee",            BOX_CONSUMABLE,   ImVec4(255, 190,  55, 255), false, true, true),
    COSMETIC_OPTION("Consumable_SilverRupee",       "Silver Rupee",          BOX_CONSUMABLE,   ImVec4(255, 255, 255, 255), false, true, true),

    COSMETIC_OPTION("Hud_AButton",                  "A Button",              BOX_HUD,          ImVec4(  0, 200,  50, 255), false, true, false), // gCCABtnPrim
    COSMETIC_OPTION("Hud_BButton",                  "B Button",              BOX_HUD,          ImVec4(255,  30,  30, 255), false, true, false), // gCCBBtnPrim
    COSMETIC_OPTION("Hud_CButtons",                 "C Buttons",             BOX_HUD,          ImVec4(255, 160,   0, 255), false, true, false), // gCCCBtnPrim
    COSMETIC_OPTION("Hud_CUpButton",                "C Up Button",           BOX_HUD,          ImVec4(255, 160,   0, 255), false, true, true), // gCCCUBtnPrim
    COSMETIC_OPTION("Hud_CDownButton",              "C Down Button",         BOX_HUD,          ImVec4(255, 160,   0, 255), false, true, true), // gCCCDBtnPrim
    COSMETIC_OPTION("Hud_CLeftButton",              "C Left Button",         BOX_HUD,          ImVec4(255, 160,   0, 255), false, true, true), // gCCCLBtnPrim
    COSMETIC_OPTION("Hud_CRightButton",             "C Right Button",        BOX_HUD,          ImVec4(255, 160,   0, 255), false, true, true), // gCCCRBtnPrim
    COSMETIC_OPTION("Hud_StartButton",              "Start Button",          BOX_HUD,          ImVec4(200,   0,   0, 255), false, true, false), // gCCStartBtnPrim
    COSMETIC_OPTION("Hud_Dpad",                     "Dpad",                  BOX_HUD,          ImVec4(255, 255, 255, 255), false, true, false), // gCCDpadPrim
    COSMETIC_OPTION("Hud_KeyCount",                 "Key Count",             BOX_HUD,          ImVec4(200, 230, 255, 255), false, true, true), // gCCKeysPrim
    COSMETIC_OPTION("Hud_StoneOfAgony",             "Stone of Agony",        BOX_HUD,          ImVec4(255, 255, 255, 255), false, true, true), // gCCVSOAPrim
    COSMETIC_OPTION("Hud_Minimap",                  "Minimap",               BOX_HUD,          ImVec4(  0, 255, 255, 255), false, true, false), // gCCMinimapPrim / gCCMinimapDGNPrim
    COSMETIC_OPTION("Hud_MinimapPosition",          "Minimap Position",      BOX_HUD,          ImVec4(200, 255,   0, 255), false, true, true), // gCCMinimapCPPrim
    COSMETIC_OPTION("Hud_MinimapEntrance",          "Minimap Entrance",      BOX_HUD,          ImVec4(200,   0,   0, 255), false, true, true), // gCCMinimapLEPrim
    COSMETIC_OPTION("Hud_FileChoose",               "File Choose",           BOX_HUD,          ImVec4(100, 150, 255, 255), false, true, true), // gCCFileChoosePrim

    /* Magic/Effects */

    // Fire Arrows
    // Ice Arrows
    // Light Arrows

    // Dins Fire
    // Farore's Wind
    // Nayru's Love

    // Spin Attack
    // Sword Trails

    /* World */
    COSMETIC_OPTION("World_BlockOfTime",            "Block of Time",         BOX_WORLD,        ImVec4(255, 255, 255, 255), false, true, true),
    COSMETIC_OPTION("World_Moon",                   "Moon",                  BOX_WORLD,        ImVec4(240, 255, 180, 255), false, true, true),

    /* NPCs */
    // Navi
    // Keese Fire
    // Keese Ice
    COSMETIC_OPTION("NPC_Dog1",                     "Dog 1",                 BOX_NPC,          ImVec4(255, 255, 200, 255), false, true, true),
    COSMETIC_OPTION("NPC_Dog2",                     "Dog 2",                 BOX_NPC,          ImVec4(150, 100,  50, 255), false, true, true),
    COSMETIC_OPTION("NPC_GoldenSkulltula",          "Golden Skulltula",      BOX_NPC,          ImVec4(255, 255, 255, 255), false, true, true),
    COSMETIC_OPTION("NPC_Kokiri",                   "Kokiri",                BOX_NPC,          ImVec4(  0, 130,  70, 255), false, true, true),
    // Cucco
    // Gerudo Guards
};

const char* MarginCvarList[] {
    "gHearts", "gHeartsCount", "gMagicBar", "gVSOA", "gBBtn", "gABtn", "gStartBtn", 
    "gCBtnU", "gCBtnD", "gCBtnL", "gCBtnR", "gDPad", "gMinimap", 
    "gSKC", "gRC", "gCarrots",  "gTimers", "gAS", "gTCM", "gTCB"
};

ImVec4 GetRandomValue(int MaximumPossible){
    ImVec4 NewColor;
    unsigned long range = 255 - 0;
#if !defined(__SWITCH__) && !defined(__WIIU__)
    std::random_device rd;
    std::mt19937 rng(rd());
#else
    size_t seed = std::hash<std::string>{}(std::to_string(rand()));
    std::mt19937_64 rng(seed);
#endif
    std::uniform_int_distribution<int> dist(0, 255 - 1);
    
    NewColor.x = (float)(dist(rng)) / 255;
    NewColor.y = (float)(dist(rng)) / 255;
    NewColor.z = (float)(dist(rng)) / 255;
    return NewColor;
}

void SetMarginAll(const char* ButtonName, bool SetActivated) {
    if (ImGui::Button(ButtonName)) {
        u8 arrayLength = sizeof(MarginCvarList) / sizeof(*MarginCvarList);
        //MarginCvarNonAnchor is an array that list every element that has No anchor by default, because if that the case this function will not touch it with pose type 0.
        const char* MarginCvarNonAnchor[] { "gCarrots", "gTimers", "gAS", "gTCM","gTCB" };
        u8 arrayLengthNonMargin = sizeof(MarginCvarNonAnchor) / sizeof(*MarginCvarNonAnchor);
        for (u8 s = 0; s < arrayLength; s++) {
            std::string cvarName = MarginCvarList[s];
            std::string cvarPosType = cvarName+"PosType";
            std::string cvarNameMargins = cvarName+"UseMargins";
            if (CVar_GetS32(cvarPosType.c_str(),0) <= 2 && SetActivated) { //Our element is not Hidden or Non anchor
                for(int i = 0; i < arrayLengthNonMargin; i++){
                    if(MarginCvarNonAnchor[i] == cvarName && CVar_GetS32(cvarPosType.c_str(),0) == 0){ //Our element is both in original position and do not have anchor by default so we skip it.
                        CVar_SetS32(cvarNameMargins.c_str(), false); //force set off
                    } else if(MarginCvarNonAnchor[i] == cvarName && CVar_GetS32(cvarPosType.c_str(),0) != 0){ //Our element is not in original position regarless it has no anchor by default since player made it anchored we can toggle margins
                        CVar_SetS32(cvarNameMargins.c_str(), SetActivated);
                    } else if(MarginCvarNonAnchor[i] != cvarName){ //Our elements has an anchor by default so regarless of it's position right now that okay to toggle margins.
                        CVar_SetS32(cvarNameMargins.c_str(), SetActivated);
                    }
                }
            } else { //Since the user requested to turn all margin off no need to do any check there.
                CVar_SetS32(cvarNameMargins.c_str(), SetActivated);
            }
        }
    }
}
void ResetPositionAll() {
    if (ImGui::Button("Reset all positions")) {
        u8 arrayLength = sizeof(MarginCvarList) / sizeof(*MarginCvarList);
        for (u8 s = 0; s < arrayLength; s++) {
            std::string cvarName = MarginCvarList[s];
            std::string cvarPosType = cvarName+"PosType";
            std::string cvarNameMargins = cvarName+"UseMargins";
            CVar_SetS32(cvarPosType.c_str(), 0);
            CVar_SetS32(cvarNameMargins.c_str(), false); //Turn margin off to everythings as that original position.
        }
    }
}

int hue = 0;

void LoadRainbowColor(bool& open) {
    int index = 0;
    for (auto& [id, cosmeticOption] : cosmeticOptions) {
        if (cosmeticOption.supportsRainbow && CVar_GetS32(cosmeticOption.rainbowCvar, 0)) {
            float frequency = CVar_GetFloat("gCosmetics.RainbowSpeed", 0.3f);
            Color_RGBA8 newColor;
            newColor.r = sin(frequency * ((hue + index) % 360) + 0) * 127 + 128;
            newColor.g = sin(frequency * ((hue + index) % 360) + 2) * 127 + 128;
            newColor.b = sin(frequency * ((hue + index) % 360) + 4) * 127 + 128;
            newColor.a = 255;

            cosmeticOption.currentColor.x = newColor.r / 255.0;
            cosmeticOption.currentColor.y = newColor.g / 255.0;
            cosmeticOption.currentColor.z = newColor.b / 255.0;
            cosmeticOption.currentColor.w = newColor.a / 255.0;

            CVar_SetRGBA(cosmeticOption.cvar, newColor);
        }
        index+=40;
    }
    ApplyOrResetCustomGfxPatches(true);
    hue++;
    if (hue >= 360) hue = 0;
}

void ApplyOrResetCustomGfxPatches(bool rainbowTick = false) {
    CosmeticOption linkGoronTunic = cosmeticOptions.at("Link_GoronTunic");
    if (rainbowTick == false || CVar_GetS32(linkGoronTunic.rainbowCvar, 0)) {
        Color_RGB8 defaultColor = {linkGoronTunic.defaultColor.w, linkGoronTunic.defaultColor.x, linkGoronTunic.defaultColor.y};
        Color_RGB8 color = CVar_GetRGB(linkGoronTunic.cvar, defaultColor);
        PATCH_GFX(gGiGoronTunicColorDL,                           "Link_GoronTunic1",         linkGoronTunic.changedCvar,           6, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGiGoronCollarColorDL,                          "Link_GoronTunic2",         linkGoronTunic.changedCvar,           6, gsDPSetPrimColor(0, 0, color.r / 2, color.g / 2, color.b / 2, 255));
        PATCH_GFX(gGiGoronTunicColorDL,                           "Link_GoronTunic3",         linkGoronTunic.changedCvar,           8, gsDPSetEnvColor(color.r / 2, color.g / 2, color.b / 2, 255));
        PATCH_GFX(gGiGoronCollarColorDL,                          "Link_GoronTunic4",         linkGoronTunic.changedCvar,           8, gsDPSetEnvColor(color.r / 2, color.g / 2, color.b / 2, 255));
    }

    CosmeticOption linkZoraTunic = cosmeticOptions.at("Link_ZoraTunic");
    if (rainbowTick == false || CVar_GetS32(linkZoraTunic.rainbowCvar, 0)) {
        Color_RGB8 defaultColor = {linkZoraTunic.defaultColor.w, linkZoraTunic.defaultColor.x, linkZoraTunic.defaultColor.y};
        Color_RGB8 color = CVar_GetRGB(linkZoraTunic.cvar, defaultColor);
        PATCH_GFX(gGiZoraTunicColorDL,                            "Link_ZoraTunic1",          linkZoraTunic.changedCvar,            6, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGiZoraCollarColorDL,                           "Link_ZoraTunic2",          linkZoraTunic.changedCvar,            6, gsDPSetPrimColor(0, 0, color.r / 2, color.g / 2, color.b / 2, 255));
        PATCH_GFX(gGiZoraTunicColorDL,                            "Link_ZoraTunic3",          linkZoraTunic.changedCvar,            8, gsDPSetEnvColor(color.r / 2, color.g / 2, color.b / 2, 255));
        PATCH_GFX(gGiZoraCollarColorDL,                           "Link_ZoraTunic4",          linkZoraTunic.changedCvar,            8, gsDPSetEnvColor(color.r / 2, color.g / 2, color.b / 2, 255));
    }

    CosmeticOption linkHair = cosmeticOptions.at("Link_Hair");
    if (rainbowTick == false || CVar_GetS32(linkHair.rainbowCvar, 0)) {
        Color_RGB8 defaultColor = {linkHair.defaultColor.w, linkHair.defaultColor.x, linkHair.defaultColor.y};
        Color_RGB8 color = CVar_GetRGB(linkHair.cvar, defaultColor);
        PATCH_GFX(gLinkChildHeadNearDL,                           "Link_Hair1",               linkHair.changedCvar,                20, gDPSetGrayscaleColor(color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkChildHeadFarDL,                            "Link_Hair2",               linkHair.changedCvar,                20, gDPSetGrayscaleColor(color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultHeadNearDL,                           "Link_Hair3",               linkHair.changedCvar,                20, gDPSetGrayscaleColor(color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultHeadFarDL,                            "Link_Hair4",               linkHair.changedCvar,                20, gDPSetGrayscaleColor(color.r, color.g, color.b, 255));
    
        if (!rainbowTick) {
        PATCH_GFX(gLinkChildHeadNearDL,                           "Link_Hair5",               linkHair.changedCvar,                92, gsDPGrayscale(true));
        PATCH_GFX(gLinkChildHeadNearDL,                           "Link_Hair6",               linkHair.changedCvar,               108, gsDPGrayscale(false));
        PATCH_GFX(gLinkChildHeadNearDL,                           "Link_Hair7",               linkHair.changedCvar,               272, gsDPGrayscale(true));
        PATCH_GFX(gLinkChildHeadNearDL,                           "Link_Hair8",               linkHair.changedCvar,               324, gsDPGrayscale(false));
        PATCH_GFX(gLinkChildHeadFarDL,                            "Link_Hair9",               linkHair.changedCvar,               202, gsDPGrayscale(true));
        PATCH_GFX(gLinkChildHeadFarDL,                            "Link_Hair10",              linkHair.changedCvar,               236, gsDPGrayscale(false));
        PATCH_GFX(gLinkAdultHeadNearDL,                           "Link_Hair11",              linkHair.changedCvar,               250, gsDPGrayscale(true));
        PATCH_GFX(gLinkAdultHeadNearDL,                           "Link_Hair12",              linkHair.changedCvar,               318, gsDPGrayscale(false));
        PATCH_GFX(gLinkAdultHeadFarDL,                            "Link_Hair13",              linkHair.changedCvar,               204, gsDPGrayscale(true));
        PATCH_GFX(gLinkAdultHeadFarDL,                            "Link_Hair14",              linkHair.changedCvar,               244, gsDPGrayscale(false));
        }
    }

    CosmeticOption linkLinen = cosmeticOptions.at("Link_Linen");
    if (rainbowTick == false || CVar_GetS32(linkLinen.rainbowCvar, 0)) {
        Color_RGB8 defaultColor = {linkLinen.defaultColor.w, linkLinen.defaultColor.x, linkLinen.defaultColor.y};
        Color_RGB8 color = CVar_GetRGB(linkLinen.cvar, defaultColor);
        PATCH_GFX(gLinkAdultLeftArmNearDL,                        "Link_Linen1",              linkLinen.changedCvar,               60, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultLeftArmNearDL,                        "Link_Linen2",              linkLinen.changedCvar,              166, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultLeftArmOutNearDL,                     "Link_Linen3",              linkLinen.changedCvar,               50, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultLeftArmFarDL,                         "Link_Linen4",              linkLinen.changedCvar,               60, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultLeftArmFarDL,                         "Link_Linen5",              linkLinen.changedCvar,              140, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultRightArmFarDL,                        "Link_Linen6",              linkLinen.changedCvar,               60, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultRightArmFarDL,                        "Link_Linen7",              linkLinen.changedCvar,              140, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultRightArmNearDL,                       "Link_Linen8",              linkLinen.changedCvar,               60, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultLeftShoulderFarDL,                    "Link_Linen9",              linkLinen.changedCvar,              110, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultLeftShoulderNearDL,                   "Link_Linen10",             linkLinen.changedCvar,              114, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultRightShoulderNearDL,                  "Link_Linen11",             linkLinen.changedCvar,              114, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultRightShoulderFarDL,                   "Link_Linen12",             linkLinen.changedCvar,              110, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultTorsoNearDL,                          "Link_Linen13",             linkLinen.changedCvar,              132, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultTorsoFarDL,                           "Link_Linen14",             linkLinen.changedCvar,              114, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultRightThighNearDL,                     "Link_Linen15",             linkLinen.changedCvar,              106, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultLeftThighNearDL,                      "Link_Linen16",             linkLinen.changedCvar,              106, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultRightThighFarDL,                      "Link_Linen17",             linkLinen.changedCvar,              108, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultLeftThighFarDL,                       "Link_Linen18",             linkLinen.changedCvar,              108, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultRightLegNearDL,                       "Link_Linen19",             linkLinen.changedCvar,               60, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultLeftLegNearDL,                        "Link_Linen20",             linkLinen.changedCvar,               60, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultRightLegFarDL,                        "Link_Linen21",             linkLinen.changedCvar,               60, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultLeftLegFarDL,                         "Link_Linen22",             linkLinen.changedCvar,               60, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));

        if (!rainbowTick) {
        PATCH_GFX(gLinkAdultLeftArmFarDL,                         "Link_Linen23",             linkLinen.changedCvar,               70, gsDPSetPrimColor(0, 0, 255, 255, 255, 255));
        PATCH_GFX(gLinkAdultLeftArmOutNearDL,                     "Link_Linen24",             linkLinen.changedCvar,               90, gsDPSetPrimColor(0, 0, 255, 255, 255, 255));
        PATCH_GFX(gLinkAdultLeftArmNearDL,                        "Link_Linen25",             linkLinen.changedCvar,               80, gsDPSetPrimColor(0, 0, 255, 255, 255, 255));
        PATCH_GFX(gLinkAdultLeftArmFarDL,                         "Link_Linen26",             linkLinen.changedCvar,              154, gsDPSetPrimColor(0, 0, 255, 255, 255, 255));
        PATCH_GFX(gLinkAdultRightArmFarDL,                        "Link_Linen27",             linkLinen.changedCvar,               70, gsDPSetPrimColor(0, 0, 255, 255, 255, 255));
        PATCH_GFX(gLinkAdultRightArmFarDL,                        "Link_Linen28",             linkLinen.changedCvar,              154, gsDPSetPrimColor(0, 0, 255, 255, 255, 255));
        PATCH_GFX(gLinkAdultRightArmNearDL,                       "Link_Linen29",             linkLinen.changedCvar,               84, gsDPSetPrimColor(0, 0, 255, 255, 255, 255));
        PATCH_GFX(gLinkAdultRightLegNearDL,                       "Link_Linen30",             linkLinen.changedCvar,               86, gsDPSetPrimColor(0, 0, 255, 255, 255, 255));
        PATCH_GFX(gLinkAdultLeftLegNearDL,                        "Link_Linen31",             linkLinen.changedCvar,               86, gsDPSetPrimColor(0, 0, 255, 255, 255, 255));
        PATCH_GFX(gLinkAdultRightLegFarDL,                        "Link_Linen32",             linkLinen.changedCvar,               76, gsDPSetPrimColor(0, 0, 255, 255, 255, 255));
        PATCH_GFX(gLinkAdultLeftLegFarDL,                         "Link_Linen33",             linkLinen.changedCvar,               76, gsDPSetPrimColor(0, 0, 255, 255, 255, 255));
        }
    }

    CosmeticOption linkBoots = cosmeticOptions.at("Link_Boots");
    if (rainbowTick == false || CVar_GetS32(linkBoots.rainbowCvar, 0)) {
        Color_RGB8 defaultColor = {linkBoots.defaultColor.w, linkBoots.defaultColor.x, linkBoots.defaultColor.y};
        Color_RGB8 color = CVar_GetRGB(linkBoots.cvar, defaultColor);
        PATCH_GFX(gLinkChildRightShinNearDL,                      "Link_Boots1",              linkBoots.changedCvar,               20, gDPSetGrayscaleColor(color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkChildRightShinFarDL,                       "Link_Boots2",              linkBoots.changedCvar,               20, gDPSetGrayscaleColor(color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultRightLegNearDL,                       "Link_Boots3",              linkBoots.changedCvar,               20, gDPSetGrayscaleColor(color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultRightLegFarDL,                        "Link_Boots4",              linkBoots.changedCvar,               20, gDPSetGrayscaleColor(color.r, color.g, color.b, 255));

        if (!rainbowTick) {
        PATCH_GFX(gLinkChildRightShinNearDL,                      "Link_Boots5",              linkBoots.changedCvar,              106, gsDPGrayscale(true));
        PATCH_GFX(gLinkChildRightShinNearDL,                      "Link_Boots6",              linkBoots.changedCvar,              138, gsDPGrayscale(false));
        PATCH_GFX(gLinkChildRightShinFarDL,                       "Link_Boots7",              linkBoots.changedCvar,              104, gsDPGrayscale(true));
        PATCH_GFX(gLinkChildRightShinFarDL,                       "Link_Boots8",              linkBoots.changedCvar,              122, gsDPGrayscale(false));
        PATCH_GFX(gLinkChildLeftShinNearDL,                       "Link_Boots9",              linkBoots.changedCvar,              106, gsDPGrayscale(true));
        PATCH_GFX(gLinkChildLeftShinNearDL,                       "Link_Boots10",             linkBoots.changedCvar,              138, gsDPGrayscale(false));
        PATCH_GFX(gLinkChildLeftShinFarDL,                        "Link_Boots11",             linkBoots.changedCvar,              104, gsDPGrayscale(true));
        PATCH_GFX(gLinkChildLeftShinFarDL,                        "Link_Boots12",             linkBoots.changedCvar,              122, gsDPGrayscale(false));
        PATCH_GFX(gLinkChildRightFootNearDL,                      "Link_Boots13",             linkBoots.changedCvar,               60, gsDPGrayscale(true));
        PATCH_GFX(gLinkChildRightFootFarDL,                       "Link_Boots14",             linkBoots.changedCvar,               60, gsDPGrayscale(true));
        PATCH_GFX(gLinkChildLeftFootNearDL,                       "Link_Boots15",             linkBoots.changedCvar,               60, gsDPGrayscale(true));
        PATCH_GFX(gLinkChildLeftFootFarDL,                        "Link_Boots16",             linkBoots.changedCvar,               60, gsDPGrayscale(true));
        PATCH_GFX(gLinkChildLeftThighNearDL,                      "Link_Boots17",             linkBoots.changedCvar,               20, gsDPGrayscale(false));
        PATCH_GFX(gLinkChildLeftThighFarDL,                       "Link_Boots18",             linkBoots.changedCvar,               20, gsDPGrayscale(false));
        PATCH_GFX(gLinkChildHeadNearDL,                           "Link_Boots19",             linkBoots.changedCvar,               40, gsDPGrayscale(false));
        PATCH_GFX(gLinkChildHeadFarDL,                            "Link_Boots20",             linkBoots.changedCvar,               40, gsDPGrayscale(false));
        PATCH_GFX(gLinkAdultRightLegNearDL,                       "Link_Boots21",             linkBoots.changedCvar,              114, gsDPGrayscale(true));
        PATCH_GFX(gLinkAdultRightLegFarDL,                        "Link_Boots22",             linkBoots.changedCvar,              104, gsDPGrayscale(true));
        PATCH_GFX(gLinkAdultLeftLegNearDL,                        "Link_Boots23",             linkBoots.changedCvar,              114, gsDPGrayscale(true));
        PATCH_GFX(gLinkAdultLeftLegFarDL,                         "Link_Boots24",             linkBoots.changedCvar,              104, gsDPGrayscale(true));
        PATCH_GFX(gLinkAdultLeftThighNearDL,                      "Link_Boots25",             linkBoots.changedCvar,               20, gsDPGrayscale(false));
        PATCH_GFX(gLinkAdultLeftThighFarDL,                       "Link_Boots26",             linkBoots.changedCvar,               20, gsDPGrayscale(false));
        PATCH_GFX(gLinkAdultHeadNearDL,                           "Link_Boots27",             linkBoots.changedCvar,               40, gsDPGrayscale(false));
        PATCH_GFX(gLinkAdultHeadFarDL,                            "Link_Boots28",             linkBoots.changedCvar,               40, gsDPGrayscale(false));
        }
    }

    CosmeticOption mirrorShieldBody = cosmeticOptions.at("MirrorShield_Body");
    if (rainbowTick == false || CVar_GetS32(mirrorShieldBody.rainbowCvar, 0)) {
        Color_RGB8 defaultColor = {mirrorShieldBody.defaultColor.w, mirrorShieldBody.defaultColor.x, mirrorShieldBody.defaultColor.y};
        Color_RGB8 color = CVar_GetRGB(mirrorShieldBody.cvar, defaultColor);
        PATCH_GFX(gGiMirrorShieldDL,                              "MirrorShield_Body1",       mirrorShieldBody.changedCvar,        10, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGiMirrorShieldDL,                              "MirrorShield_Body2",       mirrorShieldBody.changedCvar,        12, gsDPSetEnvColor(color.r / 3, color.g / 3, color.b / 3, 255));
        PATCH_GFX(gLinkAdultMirrorShieldSwordAndSheathNearDL,     "MirrorShield_Body3",       mirrorShieldBody.changedCvar,        56, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultMirrorShieldSwordAndSheathFarDL,      "MirrorShield_Body4",       mirrorShieldBody.changedCvar,        34, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultMirrorShieldAndSheathNearDL,          "MirrorShield_Body5",       mirrorShieldBody.changedCvar,        56, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultMirrorShieldAndSheathFarDL,           "MirrorShield_Body6",       mirrorShieldBody.changedCvar,        34, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultRightHandHoldingMirrorShieldNearDL,   "MirrorShield_Body7",       mirrorShieldBody.changedCvar,        56, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultRightHandHoldingMirrorShieldFarDL,    "MirrorShield_Body8",       mirrorShieldBody.changedCvar,       190, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
    }
    CosmeticOption mirrorShieldMirror = cosmeticOptions.at("MirrorShield_Mirror");
    if (rainbowTick == false || CVar_GetS32(mirrorShieldMirror.rainbowCvar, 0)) {
        Color_RGB8 defaultColor = {mirrorShieldMirror.defaultColor.w, mirrorShieldMirror.defaultColor.x, mirrorShieldMirror.defaultColor.y};
        Color_RGB8 color = CVar_GetRGB(mirrorShieldMirror.cvar, defaultColor);
        PATCH_GFX(gGiMirrorShieldDL,                              "MirrorShield_Mirror1",     mirrorShieldMirror.changedCvar,      94, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGiMirrorShieldDL,                              "MirrorShield_Mirror2",     mirrorShieldMirror.changedCvar,      96, gsDPSetEnvColor(color.r / 3, color.g / 3, color.b / 3, 255));
        PATCH_GFX(gLinkAdultMirrorShieldSwordAndSheathNearDL,     "MirrorShield_Mirror3",     mirrorShieldMirror.changedCvar,      34, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultMirrorShieldSwordAndSheathFarDL,      "MirrorShield_Mirror4",     mirrorShieldMirror.changedCvar,      66, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultMirrorShieldAndSheathNearDL,          "MirrorShield_Mirror5",     mirrorShieldMirror.changedCvar,      34, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultMirrorShieldAndSheathFarDL,           "MirrorShield_Mirror6",     mirrorShieldMirror.changedCvar,      66, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultRightHandHoldingMirrorShieldNearDL,   "MirrorShield_Mirror7",     mirrorShieldMirror.changedCvar,      34, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultRightHandHoldingMirrorShieldFarDL,    "MirrorShield_Mirror8",     mirrorShieldMirror.changedCvar,     222, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
    }
    CosmeticOption mirrorShieldEmblem = cosmeticOptions.at("MirrorShield_Emblem");
    if (rainbowTick == false || CVar_GetS32(mirrorShieldEmblem.rainbowCvar, 0)) {
        Color_RGB8 defaultColor = {mirrorShieldEmblem.defaultColor.w, mirrorShieldEmblem.defaultColor.x, mirrorShieldEmblem.defaultColor.y};
        Color_RGB8 color = CVar_GetRGB(mirrorShieldEmblem.cvar, defaultColor);
        PATCH_GFX(gGiMirrorShieldSymbolDL,                        "MirrorShield_Emblem1",     mirrorShieldEmblem.changedCvar,      10, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 140));
        PATCH_GFX(gGiMirrorShieldSymbolDL,                        "MirrorShield_Emblem2",     mirrorShieldEmblem.changedCvar,      12, gsDPSetEnvColor(color.r / 3, color.g / 3, color.b / 3, 255));
        PATCH_GFX(gLinkAdultMirrorShieldSwordAndSheathNearDL,     "MirrorShield_Emblem3",     mirrorShieldEmblem.changedCvar,     330, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultMirrorShieldSwordAndSheathFarDL,      "MirrorShield_Emblem4",     mirrorShieldEmblem.changedCvar,     270, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultMirrorShieldAndSheathNearDL,          "MirrorShield_Emblem5",     mirrorShieldEmblem.changedCvar,     258, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultMirrorShieldAndSheathFarDL,           "MirrorShield_Emblem6",     mirrorShieldEmblem.changedCvar,     206, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultRightHandHoldingMirrorShieldNearDL,   "MirrorShield_Emblem7",     mirrorShieldEmblem.changedCvar,     324, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultRightHandHoldingMirrorShieldFarDL,    "MirrorShield_Emblem8",     mirrorShieldEmblem.changedCvar,     266, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
    }

    CosmeticOption swordsKokiriBlade = cosmeticOptions.at("Swords_KokiriBlade");
    if (rainbowTick == false || CVar_GetS32(swordsKokiriBlade.rainbowCvar, 0)) {
        Color_RGB8 defaultColor = {swordsKokiriBlade.defaultColor.w, swordsKokiriBlade.defaultColor.x, swordsKokiriBlade.defaultColor.y};
        Color_RGB8 color = CVar_GetRGB(swordsKokiriBlade.cvar, defaultColor);
        PATCH_GFX(gLinkChildLeftFistAndKokiriSwordNearDL,         "Swords_KokiriBlade1",      swordsKokiriBlade.changedCvar,      158, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkChildLeftFistAndKokiriSwordFarDL,          "Swords_KokiriBlade2",      swordsKokiriBlade.changedCvar,      150, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGiKokiriSwordDL,                               "Swords_KokiriBlade3",      swordsKokiriBlade.changedCvar,       10, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGiKokiriSwordDL,                               "Swords_KokiriBlade4",      swordsKokiriBlade.changedCvar,       12, gsDPSetEnvColor(color.r / 3, color.g / 3, color.b / 3, 255));
    }
    CosmeticOption swordsMasterBlade = cosmeticOptions.at("Swords_MasterBlade");
    if (rainbowTick == false || CVar_GetS32(swordsMasterBlade.rainbowCvar, 0)) {
        Color_RGB8 defaultColor = {swordsMasterBlade.defaultColor.w, swordsMasterBlade.defaultColor.x, swordsMasterBlade.defaultColor.y};
        Color_RGB8 color = CVar_GetRGB(swordsMasterBlade.cvar, defaultColor);
        PATCH_GFX(gLinkAdultLeftHandHoldingMasterSwordFarDL,      "Swords_MasterBlade1",      swordsMasterBlade.changedCvar,      120, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultLeftHandHoldingMasterSwordNearDL,     "Swords_MasterBlade2",      swordsMasterBlade.changedCvar,       34, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(object_toki_objects_DL_001BD0,                  "Swords_MasterBlade3",      swordsMasterBlade.changedCvar,       26, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(object_toki_objects_DL_001BD0,                  "Swords_MasterBlade4",      swordsMasterBlade.changedCvar,       28, gsDPSetEnvColor(color.r / 2, color.g / 2, color.b / 2, 255));
    }
    CosmeticOption swordsMasterHilt = cosmeticOptions.at("Swords_MasterHilt");
    if (rainbowTick == false || CVar_GetS32(swordsMasterHilt.rainbowCvar, 0)) {
        Color_RGB8 defaultColor = {swordsMasterHilt.defaultColor.w, swordsMasterHilt.defaultColor.x, swordsMasterHilt.defaultColor.y};
        Color_RGB8 color = CVar_GetRGB(swordsMasterHilt.cvar, defaultColor);
        PATCH_GFX(gLinkAdultLeftHandHoldingMasterSwordNearDL,     "Swords_MasterHilt1",       swordsMasterHilt.changedCvar,        20, gDPSetGrayscaleColor(color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultLeftHandHoldingMasterSwordFarDL,      "Swords_MasterHilt2",       swordsMasterHilt.changedCvar,        20, gDPSetGrayscaleColor(color.r, color.g, color.b, 255));
        PATCH_GFX(object_toki_objects_DL_001BD0,                  "Swords_MasterHilt3",       swordsMasterHilt.changedCvar,        16, gDPSetGrayscaleColor(color.r, color.g, color.b, 255));

        if (!rainbowTick) {
        PATCH_GFX(gLinkAdultLeftHandHoldingMasterSwordFarDL,      "Swords_MasterHilt4",       swordsMasterHilt.changedCvar,        38, gsDPGrayscale(true));
        PATCH_GFX(gLinkAdultLeftHandHoldingMasterSwordFarDL,      "Swords_MasterHilt5",       swordsMasterHilt.changedCvar,       112, gsDPGrayscale(false));
        PATCH_GFX(gLinkAdultLeftHandHoldingMasterSwordNearDL,     "Swords_MasterHilt6",       swordsMasterHilt.changedCvar,        86, gsDPGrayscale(true));
        PATCH_GFX(gLinkAdultLeftHandHoldingMasterSwordNearDL,     "Swords_MasterHilt7",       swordsMasterHilt.changedCvar,       208, gsDPGrayscale(false));
        PATCH_GFX(object_toki_objects_DL_001BD0,                  "Swords_MasterHilt8",       swordsMasterHilt.changedCvar,       112, gsDPGrayscale(true));
        PATCH_GFX(object_toki_objects_DL_001BD0,                  "Swords_MasterHilt9",       swordsMasterHilt.changedCvar,       278, gsDPGrayscale(false));
        PATCH_GFX(object_toki_objects_DL_001BD0,                  "Swords_MasterHilt10",      swordsMasterHilt.changedCvar,       280, gsSPEndDisplayList());
        }
    }
    CosmeticOption swordsBiggoronBlade = cosmeticOptions.at("Swords_BiggoronBlade");
    if (rainbowTick == false || CVar_GetS32(swordsBiggoronBlade.rainbowCvar, 0)) {
        Color_RGB8 defaultColor = {swordsBiggoronBlade.defaultColor.w, swordsBiggoronBlade.defaultColor.x, swordsBiggoronBlade.defaultColor.y};
        Color_RGB8 color = CVar_GetRGB(swordsBiggoronBlade.cvar, defaultColor);
        PATCH_GFX(gLinkAdultLeftHandHoldingBgsFarDL,              "Swords_BiggoronBlade1",    swordsBiggoronBlade.changedCvar,    216, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultLeftHandHoldingBgsNearDL,             "Swords_BiggoronBlade2",    swordsBiggoronBlade.changedCvar,    126, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGiBiggoronSwordDL,                             "Swords_BiggoronBlade3",    swordsBiggoronBlade.changedCvar,     10, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGiBiggoronSwordDL,                             "Swords_BiggoronBlade4",    swordsBiggoronBlade.changedCvar,     12, gsDPSetEnvColor(color.r / 3, color.g / 3, color.b / 3, 255));
    }
    CosmeticOption swordsBiggoronHilt = cosmeticOptions.at("Swords_BiggoronHilt");
    if (rainbowTick == false || CVar_GetS32(swordsBiggoronHilt.rainbowCvar, 0)) {
        Color_RGB8 defaultColor = {swordsBiggoronHilt.defaultColor.w, swordsBiggoronHilt.defaultColor.x, swordsBiggoronHilt.defaultColor.y};
        Color_RGB8 color = CVar_GetRGB(swordsBiggoronHilt.cvar, defaultColor);
        PATCH_GFX(gLinkAdultLeftHandHoldingBgsNearDL,             "Swords_BiggoronHilt1",     swordsBiggoronHilt.changedCvar,      20, gDPSetGrayscaleColor(color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultLeftHandHoldingBgsFarDL,              "Swords_BiggoronHilt2",     swordsBiggoronHilt.changedCvar,      20, gDPSetGrayscaleColor(color.r, color.g, color.b, 255));
        PATCH_GFX(gGiBiggoronSwordDL,                             "Swords_BiggoronHilt3",     swordsBiggoronHilt.changedCvar,      74, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGiBiggoronSwordDL,                             "Swords_BiggoronHilt4",     swordsBiggoronHilt.changedCvar,      76, gsDPSetEnvColor(color.r / 3, color.g / 3, color.b / 3, 255));
        PATCH_GFX(gGiBiggoronSwordDL,                             "Swords_BiggoronHilt5",     swordsBiggoronHilt.changedCvar,     154, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGiBiggoronSwordDL,                             "Swords_BiggoronHilt6",     swordsBiggoronHilt.changedCvar,     156, gsDPSetEnvColor(color.r / 3, color.g / 3, color.b / 3, 255));

        if (!rainbowTick) {
        PATCH_GFX(gLinkAdultLeftHandHoldingBgsFarDL,              "Swords_BiggoronHilt7",     swordsBiggoronHilt.changedCvar,     278, gsDPGrayscale(true));
        PATCH_GFX(gLinkAdultLeftHandHoldingBgsFarDL,              "Swords_BiggoronHilt8",     swordsBiggoronHilt.changedCvar,     332, gsDPGrayscale(false));
        PATCH_GFX(gLinkAdultLeftHandHoldingBgsFarDL,              "Swords_BiggoronHilt9",     swordsBiggoronHilt.changedCvar,     334, gsSPEndDisplayList());
        PATCH_GFX(gLinkAdultLeftHandHoldingBgsNearDL,             "Swords_BiggoronHilt10",    swordsBiggoronHilt.changedCvar,      38, gsDPGrayscale(true));
        PATCH_GFX(gLinkAdultLeftHandHoldingBgsNearDL,             "Swords_BiggoronHilt11",    swordsBiggoronHilt.changedCvar,     118, gsDPGrayscale(false));
        }
    }   

    CosmeticOption glovesGoronBracelet = cosmeticOptions.at("Gloves_GoronBracelet");
    if (rainbowTick == false || CVar_GetS32(glovesGoronBracelet.rainbowCvar, 0)) {
        Color_RGB8 defaultColor = {glovesGoronBracelet.defaultColor.w, glovesGoronBracelet.defaultColor.x, glovesGoronBracelet.defaultColor.y};
        Color_RGB8 color = CVar_GetRGB(glovesGoronBracelet.cvar, defaultColor);
        PATCH_GFX(gGiGoronBraceletDL,                             "Gloves_GoronBracelet1",    glovesGoronBracelet.changedCvar,     10, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGiGoronBraceletDL,                             "Gloves_GoronBracelet2",    glovesGoronBracelet.changedCvar,     12, gsDPSetEnvColor(color.r / 3, color.g / 3, color.b / 3, 255));
        PATCH_GFX(gLinkChildGoronBraceletDL,                      "Gloves_GoronBracelet3",    glovesGoronBracelet.changedCvar,      6, gDPSetGrayscaleColor(color.r, color.g, color.b, 255));

        if (!rainbowTick) {
        PATCH_GFX(gLinkChildGoronBraceletDL,                      "Gloves_GoronBracelet4",    glovesGoronBracelet.changedCvar,     22, gsDPGrayscale(true));
        PATCH_GFX(gLinkChildGoronBraceletDL,                      "Gloves_GoronBracelet5",    glovesGoronBracelet.changedCvar,     78, gsDPGrayscale(false));
        }
    }
    CosmeticOption glovesSilverGauntlets = cosmeticOptions.at("Gloves_SilverGauntlets");
    if (rainbowTick == false || CVar_GetS32(glovesSilverGauntlets.rainbowCvar, 0)) {
        Color_RGB8 defaultColor = {glovesSilverGauntlets.defaultColor.w, glovesSilverGauntlets.defaultColor.x, glovesSilverGauntlets.defaultColor.y};
        Color_RGB8 color = CVar_GetRGB(glovesSilverGauntlets.cvar, defaultColor);
        PATCH_GFX(gGiSilverGauntletsColorDL,                      "Gloves_SilverGauntlets1",  glovesSilverGauntlets.changedCvar,    6, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGiSilverGauntletsColorDL,                      "Gloves_SilverGauntlets2",  glovesSilverGauntlets.changedCvar,    8, gsDPSetEnvColor(color.r / 3, color.g / 3, color.b / 3, 255));
    }
    CosmeticOption glovesGoldenGauntlets = cosmeticOptions.at("Gloves_GoldenGauntlets");
    if (rainbowTick == false || CVar_GetS32(glovesGoldenGauntlets.rainbowCvar, 0)) {
        Color_RGB8 defaultColor = {glovesGoldenGauntlets.defaultColor.w, glovesGoldenGauntlets.defaultColor.x, glovesGoldenGauntlets.defaultColor.y};
        Color_RGB8 color = CVar_GetRGB(glovesGoldenGauntlets.cvar, defaultColor);
        PATCH_GFX(gGiGoldenGauntletsColorDL,                      "Gloves_GoldenGauntlets1",  glovesGoldenGauntlets.changedCvar,    6, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGiGoldenGauntletsColorDL,                      "Gloves_GoldenGauntlets2",  glovesGoldenGauntlets.changedCvar,    8, gsDPSetEnvColor(color.r / 3, color.g / 3, color.b / 3, 255));
    }
    CosmeticOption glovesGauntletsGem = cosmeticOptions.at("Gloves_GauntletsGem");
    if (rainbowTick == false || CVar_GetS32(glovesGauntletsGem.rainbowCvar, 0)) {
        Color_RGB8 defaultColor = {glovesGauntletsGem.defaultColor.w, glovesGauntletsGem.defaultColor.x, glovesGauntletsGem.defaultColor.y};
        Color_RGB8 color = CVar_GetRGB(glovesGauntletsGem.cvar, defaultColor);
        PATCH_GFX(gGiGauntletsDL,                                 "Gloves_GauntletsGem1",     glovesGauntletsGem.changedCvar,     168, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGiGauntletsDL,                                 "Gloves_GauntletsGem2",     glovesGauntletsGem.changedCvar,     170, gsDPSetEnvColor(color.r / 3, color.g / 3, color.b / 3, 255));
        PATCH_GFX(gLinkAdultLeftGauntletPlate2DL,                 "Gloves_GauntletsGem3",     glovesGauntletsGem.changedCvar,      84, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultRightGauntletPlate2DL,                "Gloves_GauntletsGem4",     glovesGauntletsGem.changedCvar,      84, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultLeftGauntletPlate3DL,                 "Gloves_GauntletsGem5",     glovesGauntletsGem.changedCvar,      84, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultRightGauntletPlate3DL,                "Gloves_GauntletsGem6",     glovesGauntletsGem.changedCvar,      84, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
    }

    CosmeticOption equipmentBoomerangBody = cosmeticOptions.at("Equipment_BoomerangBody");
    if (rainbowTick == false || CVar_GetS32(equipmentBoomerangBody.rainbowCvar, 0)) {
        Color_RGB8 defaultColor = {equipmentBoomerangBody.defaultColor.w, equipmentBoomerangBody.defaultColor.x, equipmentBoomerangBody.defaultColor.y};
        Color_RGB8 color = CVar_GetRGB(equipmentBoomerangBody.cvar, defaultColor);
        PATCH_GFX(gGiBoomerangDL,                                 "Equipment_BoomerangBody1", equipmentBoomerangBody.changedCvar,  10, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGiBoomerangDL,                                 "Equipment_BoomerangBody2", equipmentBoomerangBody.changedCvar,  12, gsDPSetEnvColor(color.r / 3, color.g / 3, color.b / 3, 255));
        PATCH_GFX(gLinkChildLeftFistAndBoomerangNearDL,           "Equipment_BoomerangBody3", equipmentBoomerangBody.changedCvar,  68, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkChildLeftFistAndBoomerangFarDL,            "Equipment_BoomerangBody4", equipmentBoomerangBody.changedCvar,  18, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gBoomerangDL,                                   "Equipment_BoomerangBody5", equipmentBoomerangBody.changedCvar,  78, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
    }
    CosmeticOption equipmentBoomerangGem = cosmeticOptions.at("Equipment_BoomerangGem");
    if (rainbowTick == false || CVar_GetS32(equipmentBoomerangGem.rainbowCvar, 0)) {
        Color_RGB8 defaultColor = {equipmentBoomerangGem.defaultColor.w, equipmentBoomerangGem.defaultColor.x, equipmentBoomerangGem.defaultColor.y};
        Color_RGB8 color = CVar_GetRGB(equipmentBoomerangGem.cvar, defaultColor);
        PATCH_GFX(gGiBoomerangDL,                                 "Equipment_BoomerangGem1",  equipmentBoomerangGem.changedCvar,  168, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGiBoomerangDL,                                 "Equipment_BoomerangGem2",  equipmentBoomerangGem.changedCvar,  170, gsDPSetEnvColor(color.r / 3, color.g / 3, color.b / 3, 255));
        PATCH_GFX(gLinkChildLeftFistAndBoomerangNearDL,           "Equipment_BoomerangGem3",  equipmentBoomerangGem.changedCvar,   32, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gBoomerangDL,                                   "Equipment_BoomerangGem4",  equipmentBoomerangGem.changedCvar,   46, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        // No gem rendered on far?
        // PATCH_GFX(gLinkChildLeftFistAndBoomerangFarDL,  "Equipment_BoomerangGem5",  equipmentBoomerangGem.changedCvar,  32, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
    }

    CosmeticOption equipmentBowTips = cosmeticOptions.at("Equipment_BowTips");
    if (rainbowTick == false || CVar_GetS32(equipmentBowTips.rainbowCvar, 0)) {
        Color_RGB8 defaultColor = {equipmentBowTips.defaultColor.w, equipmentBowTips.defaultColor.x, equipmentBowTips.defaultColor.y};
        Color_RGB8 color = CVar_GetRGB(equipmentBowTips.cvar, defaultColor);
        PATCH_GFX(gGiBowDL,                                       "Equipment_BowTips1",       equipmentBowTips.changedCvar,       172, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGiBowDL,                                       "Equipment_BowTips2",       equipmentBowTips.changedCvar,       174, gsDPSetEnvColor(color.r / 3, color.g / 3, color.b / 3, 255));
        PATCH_GFX(gLinkAdultRightHandHoldingBowFirstPersonDL,     "Equipment_BowTips3",       equipmentBowTips.changedCvar,        68, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultRightHandHoldingBowNearDL,            "Equipment_BowTips4",       equipmentBowTips.changedCvar,        52, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultRightHandHoldingBowFarDL,             "Equipment_BowTips5",       equipmentBowTips.changedCvar,        50, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
    }
    CosmeticOption equipmentBowString = cosmeticOptions.at("Equipment_BowString");
    if (rainbowTick == false || CVar_GetS32(equipmentBowString.rainbowCvar, 0)) {
        Color_RGB8 defaultColor = {equipmentBowString.defaultColor.w, equipmentBowString.defaultColor.x, equipmentBowString.defaultColor.y};
        Color_RGB8 color = CVar_GetRGB(equipmentBowString.cvar, defaultColor);
        PATCH_GFX(gGiBowDL,                                       "Equipment_BowString1",     equipmentBowString.changedCvar,     210, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGiBowDL,                                       "Equipment_BowString2",     equipmentBowString.changedCvar,     212, gsDPSetEnvColor(color.r / 3, color.g / 3, color.b / 3, 255));
        PATCH_GFX(gLinkAdultBowStringDL,                          "Equipment_BowString3",     equipmentBowString.changedCvar,      18, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
    }
    CosmeticOption equipmentBowBody = cosmeticOptions.at("Equipment_BowBody");
    if (rainbowTick == false || CVar_GetS32(equipmentBowBody.rainbowCvar, 0)) {
        Color_RGB8 defaultColor = {equipmentBowBody.defaultColor.w, equipmentBowBody.defaultColor.x, equipmentBowBody.defaultColor.y};
        Color_RGB8 color = CVar_GetRGB(equipmentBowBody.cvar, defaultColor);
        PATCH_GFX(gGiBowDL,                                       "Equipment_BowBody1",       equipmentBowBody.changedCvar,        10, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGiBowDL,                                       "Equipment_BowBody2",       equipmentBowBody.changedCvar,        12, gsDPSetEnvColor(color.r / 3, color.g / 3, color.b / 3, 255));
        PATCH_GFX(gLinkAdultRightHandHoldingBowFirstPersonDL,     "Equipment_BowBody3",       equipmentBowBody.changedCvar,        84, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultRightHandHoldingBowNearDL,            "Equipment_BowBody4",       equipmentBowBody.changedCvar,        66, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultRightHandHoldingBowFarDL,             "Equipment_BowBody5",       equipmentBowBody.changedCvar,        62, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
    }
    CosmeticOption equipmentBowHandle = cosmeticOptions.at("Equipment_BowHandle");
    if (rainbowTick == false || CVar_GetS32(equipmentBowHandle.rainbowCvar, 0)) {
        Color_RGB8 defaultColor = {equipmentBowHandle.defaultColor.w, equipmentBowHandle.defaultColor.x, equipmentBowHandle.defaultColor.y};
        Color_RGB8 color = CVar_GetRGB(equipmentBowHandle.cvar, defaultColor);
        PATCH_GFX(gGiBowDL,                                       "Equipment_BowHandle1",     equipmentBowHandle.changedCvar,     102, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGiBowDL,                                       "Equipment_BowHandle2",     equipmentBowHandle.changedCvar,     104, gsDPSetEnvColor(color.r / 3, color.g / 3, color.b / 3, 255));
        PATCH_GFX(gLinkAdultRightHandHoldingBowFirstPersonDL,     "Equipment_BowHandle3",     equipmentBowHandle.changedCvar,      36, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultRightHandHoldingBowNearDL,            "Equipment_BowHandle4",     equipmentBowHandle.changedCvar,      36, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultRightHandHoldingBowFarDL,             "Equipment_BowHandle5",     equipmentBowHandle.changedCvar,      36, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
    }

    CosmeticOption equipmentHammerHead = cosmeticOptions.at("Equipment_HammerHead");
    if (rainbowTick == false || CVar_GetS32(equipmentHammerHead.rainbowCvar, 0)) {
        Color_RGB8 defaultColor = {equipmentHammerHead.defaultColor.w, equipmentHammerHead.defaultColor.x, equipmentHammerHead.defaultColor.y};
        Color_RGB8 color = CVar_GetRGB(equipmentHammerHead.cvar, defaultColor);
        PATCH_GFX(gGiHammerDL,                                    "Equipment_HammerHead1",    equipmentHammerHead.changedCvar,     10, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGiHammerDL,                                    "Equipment_HammerHead2",    equipmentHammerHead.changedCvar,     12, gsDPSetEnvColor(color.r / 5, color.g / 5, color.b / 5, 255));
        PATCH_GFX(gGiHammerDL,                                    "Equipment_HammerHead3",    equipmentHammerHead.changedCvar,    136, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGiHammerDL,                                    "Equipment_HammerHead4",    equipmentHammerHead.changedCvar,    138, gsDPSetEnvColor(color.r / 5, color.g / 5, color.b / 5, 255));
        PATCH_GFX(gLinkAdultLeftHandHoldingHammerNearDL,          "Equipment_HammerHead5",    equipmentHammerHead.changedCvar,     76, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultLeftHandHoldingHammerFarDL,           "Equipment_HammerHead6",    equipmentHammerHead.changedCvar,     76, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
    }
    CosmeticOption equipmentHammerHandle = cosmeticOptions.at("Equipment_HammerHandle");
    if (rainbowTick == false || CVar_GetS32(equipmentHammerHandle.rainbowCvar, 0)) {
        Color_RGB8 defaultColor = {equipmentHammerHandle.defaultColor.w, equipmentHammerHandle.defaultColor.x, equipmentHammerHandle.defaultColor.y};
        Color_RGB8 color = CVar_GetRGB(equipmentHammerHandle.cvar, defaultColor);
        PATCH_GFX(gGiHammerDL,                                    "Equipment_HammerHandle1",  equipmentHammerHandle.changedCvar,  168, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGiHammerDL,                                    "Equipment_HammerHandle2",  equipmentHammerHandle.changedCvar,  170, gsDPSetEnvColor(color.r / 2, color.g / 2, color.b / 2, 255));
        PATCH_GFX(gLinkAdultLeftHandHoldingHammerNearDL,          "Equipment_HammerHandle5",  equipmentHammerHandle.changedCvar,   36, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultLeftHandHoldingHammerFarDL,           "Equipment_HammerHandle6",  equipmentHammerHandle.changedCvar,   36, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
    }

    CosmeticOption consumableGreenRupee = cosmeticOptions.at("Consumable_GreenRupee");
    if (rainbowTick == false || CVar_GetS32(consumableGreenRupee.rainbowCvar, 0)) {
        Color_RGB8 defaultColor = {consumableGreenRupee.defaultColor.w, consumableGreenRupee.defaultColor.x, consumableGreenRupee.defaultColor.y};
        Color_RGB8 color = CVar_GetRGB(consumableGreenRupee.cvar, defaultColor);
        PATCH_GFX(gGiGreenRupeeInnerColorDL,                      "Consumable_GreenRupee1",   consumableGreenRupee.changedCvar,     6, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGiGreenRupeeInnerColorDL,                      "Consumable_GreenRupee2",   consumableGreenRupee.changedCvar,     8, gsDPSetEnvColor(color.r / 5, color.g / 5, color.b / 5, 255));
        PATCH_GFX(gGiGreenRupeeOuterColorDL,                      "Consumable_GreenRupee3",   consumableGreenRupee.changedCvar,     6, gsDPSetPrimColor(0, 0, 255, 255, 255, 255));
        PATCH_GFX(gGiGreenRupeeOuterColorDL,                      "Consumable_GreenRupee4",   consumableGreenRupee.changedCvar,     8, gsDPSetEnvColor(color.r * 0.75f, color.g * 0.75f, color.b * 0.75f, 255));
    }
    CosmeticOption consumableBlueRupee = cosmeticOptions.at("Consumable_BlueRupee");
    if (rainbowTick == false || CVar_GetS32(consumableBlueRupee.rainbowCvar, 0)) {
        Color_RGB8 defaultColor = {consumableBlueRupee.defaultColor.w, consumableBlueRupee.defaultColor.x, consumableBlueRupee.defaultColor.y};
        Color_RGB8 color = CVar_GetRGB(consumableBlueRupee.cvar, defaultColor);
        PATCH_GFX(gGiBlueRupeeInnerColorDL,                       "Consumable_BlueRupee1",    consumableBlueRupee.changedCvar,      6, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGiBlueRupeeInnerColorDL,                       "Consumable_BlueRupee2",    consumableBlueRupee.changedCvar,      8, gsDPSetEnvColor(color.r / 5, color.g / 5, color.b / 5, 255));
        PATCH_GFX(gGiBlueRupeeOuterColorDL,                       "Consumable_BlueRupee3",    consumableBlueRupee.changedCvar,      6, gsDPSetPrimColor(0, 0, 255, 255, 255, 255));
        PATCH_GFX(gGiBlueRupeeOuterColorDL,                       "Consumable_BlueRupee4",    consumableBlueRupee.changedCvar,      8, gsDPSetEnvColor(color.r * 0.75f, color.g * 0.75f, color.b * 0.75f, 255));
    }
    CosmeticOption consumableRedRupee = cosmeticOptions.at("Consumable_RedRupee");
    if (rainbowTick == false || CVar_GetS32(consumableRedRupee.rainbowCvar, 0)) {
        Color_RGB8 defaultColor = {consumableRedRupee.defaultColor.w, consumableRedRupee.defaultColor.x, consumableRedRupee.defaultColor.y};
        Color_RGB8 color = CVar_GetRGB(consumableRedRupee.cvar, defaultColor);
        PATCH_GFX(gGiRedRupeeInnerColorDL,                        "Consumable_RedRupee1",     consumableRedRupee.changedCvar,       6, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGiRedRupeeInnerColorDL,                        "Consumable_RedRupee2",     consumableRedRupee.changedCvar,       8, gsDPSetEnvColor(color.r / 5, color.g / 5, color.b / 5, 255));
        PATCH_GFX(gGiRedRupeeOuterColorDL,                        "Consumable_RedRupee3",     consumableRedRupee.changedCvar,       6, gsDPSetPrimColor(0, 0, 255, 255, 255, 255));
        PATCH_GFX(gGiRedRupeeOuterColorDL,                        "Consumable_RedRupee4",     consumableRedRupee.changedCvar,       8, gsDPSetEnvColor(color.r * 0.75f, color.g * 0.75f, color.b * 0.75f, 255));
    }
    CosmeticOption consumablePurpleRupee = cosmeticOptions.at("Consumable_PurpleRupee");
    if (rainbowTick == false || CVar_GetS32(consumablePurpleRupee.rainbowCvar, 0)) {
        Color_RGB8 defaultColor = {consumablePurpleRupee.defaultColor.w, consumablePurpleRupee.defaultColor.x, consumablePurpleRupee.defaultColor.y};
        Color_RGB8 color = CVar_GetRGB(consumablePurpleRupee.cvar, defaultColor);
        PATCH_GFX(gGiPurpleRupeeInnerColorDL,                     "Consumable_PurpleRupee1",  consumablePurpleRupee.changedCvar,    6, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGiPurpleRupeeInnerColorDL,                     "Consumable_PurpleRupee2",  consumablePurpleRupee.changedCvar,    8, gsDPSetEnvColor(color.r / 5, color.g / 5, color.b / 5, 255));
        PATCH_GFX(gGiPurpleRupeeOuterColorDL,                     "Consumable_PurpleRupee3",  consumablePurpleRupee.changedCvar,    6, gsDPSetPrimColor(0, 0, 255, 255, 255, 255));
        PATCH_GFX(gGiPurpleRupeeOuterColorDL,                     "Consumable_PurpleRupee4",  consumablePurpleRupee.changedCvar,    8, gsDPSetEnvColor(color.r * 0.75f, color.g * 0.75f, color.b * 0.75f, 255));
    }
    CosmeticOption consumableGoldRupee = cosmeticOptions.at("Consumable_GoldRupee");
    if (rainbowTick == false || CVar_GetS32(consumableGoldRupee.rainbowCvar, 0)) {
        Color_RGB8 defaultColor = {consumableGoldRupee.defaultColor.w, consumableGoldRupee.defaultColor.x, consumableGoldRupee.defaultColor.y};
        Color_RGB8 color = CVar_GetRGB(consumableGoldRupee.cvar, defaultColor);
        PATCH_GFX(gGiGoldRupeeInnerColorDL,                       "Consumable_GoldRupee1",    consumableGoldRupee.changedCvar,      6, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGiGoldRupeeInnerColorDL,                       "Consumable_GoldRupee2",    consumableGoldRupee.changedCvar,      8, gsDPSetEnvColor(color.r / 5, color.g / 5, color.b / 5, 255));
        PATCH_GFX(gGiGoldRupeeOuterColorDL,                       "Consumable_GoldRupee3",    consumableGoldRupee.changedCvar,      6, gsDPSetPrimColor(0, 0, 255, 255, 255, 255));
        PATCH_GFX(gGiGoldRupeeOuterColorDL,                       "Consumable_GoldRupee4",    consumableGoldRupee.changedCvar,      8, gsDPSetEnvColor(color.r * 0.75f, color.g * 0.75f, color.b * 0.75f, 255));
    }

    CosmeticOption consumableHearts = cosmeticOptions.at("Consumable_Hearts");
    if (rainbowTick == false || CVar_GetS32(consumableHearts.rainbowCvar, 0)) {
        Color_RGB8 defaultColor = {consumableHearts.defaultColor.w, consumableHearts.defaultColor.x, consumableHearts.defaultColor.y};
        Color_RGB8 color = CVar_GetRGB(consumableHearts.cvar, defaultColor);
        PATCH_GFX(gGiRecoveryHeartDL,                             "Consumable_Hearts1",       consumableHearts.changedCvar,         4, gDPSetGrayscaleColor(color.r, color.g, color.b, 255));
        PATCH_GFX(gGiRecoveryHeartDL,                             "Consumable_Hearts2",       consumableHearts.changedCvar,        26, gsDPGrayscale(true));
        PATCH_GFX(gGiRecoveryHeartDL,                             "Consumable_Hearts3",       consumableHearts.changedCvar,        72, gsDPGrayscale(false));
        PATCH_GFX(gGiRecoveryHeartDL,                             "Consumable_Hearts4",       consumableHearts.changedCvar,        74, gsSPEndDisplayList());
        PATCH_GFX(gGiHeartPieceDL,                                "Consumable_Hearts5",       consumableHearts.changedCvar,         4, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGiHeartPieceDL,                                "Consumable_Hearts6",       consumableHearts.changedCvar,        12, gsDPSetEnvColor(color.r / 2, color.g / 2, color.b / 2, 255));
        PATCH_GFX(gGiHeartContainerDL,                            "Consumable_Hearts7",       consumableHearts.changedCvar,         4, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGiHeartContainerDL,                            "Consumable_Hearts8",       consumableHearts.changedCvar,        12, gsDPSetEnvColor(color.r / 2, color.g / 2, color.b / 2, 255));
        PATCH_GFX(gGiRedPotColorDL,                               "Consumable_Hearts9",       consumableHearts.changedCvar,         6, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGiRedPotColorDL,                               "Consumable_Hearts10",      consumableHearts.changedCvar,         8, gsDPSetEnvColor(color.r / 2, color.g / 2, color.b / 2, 255));
    }
    CosmeticOption consumableMagic = cosmeticOptions.at("Consumable_Magic");
    if (rainbowTick == false || CVar_GetS32(consumableMagic.rainbowCvar, 0)) {
        Color_RGB8 defaultColor = {consumableMagic.defaultColor.w, consumableMagic.defaultColor.x, consumableMagic.defaultColor.y};
        Color_RGB8 color = CVar_GetRGB(consumableMagic.cvar, defaultColor);
        PATCH_GFX(gGiMagicJarSmallDL,                             "Consumable_Magic1",        consumableMagic.changedCvar,         62, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGiMagicJarSmallDL,                             "Consumable_Magic2",        consumableMagic.changedCvar,         64, gsDPSetEnvColor(color.r / 2, color.g / 2, color.b / 2, 255));
        PATCH_GFX(gGiMagicJarLargeDL,                             "Consumable_Magic3",        consumableMagic.changedCvar,         62, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGiMagicJarLargeDL,                             "Consumable_Magic4",        consumableMagic.changedCvar,         64, gsDPSetEnvColor(color.r / 2, color.g / 2, color.b / 2, 255));
        PATCH_GFX(gGiGreenPotColorDL,                             "Consumable_Magic5",        consumableMagic.changedCvar,          6, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGiGreenPotColorDL,                             "Consumable_Magic6",        consumableMagic.changedCvar,          8, gsDPSetEnvColor(color.r / 2, color.g / 2, color.b / 2, 255));
    }

    CosmeticOption npcGoldenSkulltula = cosmeticOptions.at("NPC_GoldenSkulltula");
    if (rainbowTick == false || CVar_GetS32(npcGoldenSkulltula.rainbowCvar, 0)) {
        Color_RGB8 defaultColor = {npcGoldenSkulltula.defaultColor.w, npcGoldenSkulltula.defaultColor.x, npcGoldenSkulltula.defaultColor.y};
        Color_RGB8 color = CVar_GetRGB(npcGoldenSkulltula.cvar, defaultColor);
        PATCH_GFX(gGiSkulltulaTokenDL,                            "NPC_GoldenSkulltula1",     npcGoldenSkulltula.changedCvar,      10, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGiSkulltulaTokenDL,                            "NPC_GoldenSkulltula2",     npcGoldenSkulltula.changedCvar,      12, gsDPSetEnvColor(color.r / 2, color.g / 2, color.b / 2, 255));
        PATCH_GFX(gGiSkulltulaTokenFlameDL,                       "NPC_GoldenSkulltula3",     npcGoldenSkulltula.changedCvar,      64, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGiSkulltulaTokenFlameDL,                       "NPC_GoldenSkulltula4",     npcGoldenSkulltula.changedCvar,      66, gsDPSetEnvColor(color.r / 2, color.g / 2, color.b / 2, 255));
        PATCH_GFX(object_st_DL_003FB0,                            "NPC_GoldenSkulltula5",     npcGoldenSkulltula.changedCvar,     236, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(object_st_DL_003FB0,                            "NPC_GoldenSkulltula6",     npcGoldenSkulltula.changedCvar,     238, gsDPSetEnvColor(color.r / 4, color.g / 4, color.b / 4, 255));
    }
}

void Table_InitHeader(bool has_header = true) {
    if (has_header) {
        ImGui::TableHeadersRow();
    }
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::AlignTextToFramePadding(); //This is to adjust Vertical pos of item in a cell to be normlized.
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 2);
    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x-60);
}
void DrawUseMarginsSlider(const std::string ElementName, const std::string CvarName){
    std::string CvarLabel = CvarName + "UseMargins";
    std::string Label = ElementName + " use margins";
    UIWidgets::EnhancementCheckbox(Label.c_str(), CvarLabel.c_str());
    UIWidgets::Tooltip("Using this allow you move the element with General margins sliders");
}
void DrawPositionsRadioBoxes(const std::string CvarName, bool NoAnchorEnabled = true){
    std::string CvarLabel = CvarName + "PosType";
    UIWidgets::EnhancementRadioButton("Original position", CvarLabel.c_str(), 0);
    UIWidgets::Tooltip("This will use original intended elements position");
    UIWidgets::EnhancementRadioButton("Anchor to the left", CvarLabel.c_str(), 1);
    UIWidgets::Tooltip("This will make your elements follow the left side of your game window");
    UIWidgets::EnhancementRadioButton("Anchor to the right", CvarLabel.c_str(), 2);
    UIWidgets::Tooltip("This will make your elements follow the right side of your game window");
    if (NoAnchorEnabled) {
        UIWidgets::EnhancementRadioButton("No anchors", CvarLabel.c_str(), 3);
        UIWidgets::Tooltip("This will make your elements to not follow any side\nBetter used for center elements");
    }
    UIWidgets::EnhancementRadioButton("Hidden", CvarLabel.c_str(), 4);
    UIWidgets::Tooltip("This will make your elements hidden");
}
void DrawPositionSlider(const std::string CvarName, int MinY, int MaxY, int MinX, int MaxX){
    std::string PosXCvar = CvarName+"PosX";
    std::string PosYCvar = CvarName+"PosY";
    std::string InvisibleLabelX = "##"+PosXCvar;
    std::string InvisibleLabelY = "##"+PosYCvar;
    UIWidgets::EnhancementSliderInt("Up <-> Down : %d", InvisibleLabelY.c_str(), PosYCvar.c_str(), MinY, MaxY, "", 0, true);
    UIWidgets::Tooltip("This slider is used to move Up and Down your elements.");
    UIWidgets::EnhancementSliderInt("Left <-> Right : %d", InvisibleLabelX.c_str(), PosXCvar.c_str(), MinX, MaxX, "", 0, true);
    UIWidgets::Tooltip("This slider is used to move Left and Right your elements.");
}
void DrawScaleSlider(const std::string CvarName,float DefaultValue){
    std::string InvisibleLabel = "##"+CvarName;
    std::string CvarLabel = CvarName+"Scale";
    //Disabled for now. feature not done and several fixes needed to be merged.
    //UIWidgets::EnhancementSliderFloat("Scale : %dx", InvisibleLabel.c_str(), CvarLabel.c_str(), 0.1f, 3.0f,"",DefaultValue,true,true);
}
void Draw_Placements(){
    if (ImGui::BeginTable("tableMargins", 1, FlagsTable)) {
        ImGui::TableSetupColumn("General margins settings", FlagsCell, TablesCellsWidth);
        Table_InitHeader();
        UIWidgets::EnhancementSliderInt("Top : %dx", "##UIMARGINT", "gHUDMargin_T", (ImGui::GetWindowViewport()->Size.y/2)*-1, 25, "", 0, true);
        UIWidgets::EnhancementSliderInt("Left: %dx", "##UIMARGINL", "gHUDMargin_L", -25, ImGui::GetWindowViewport()->Size.x, "", 0, true);
        UIWidgets::EnhancementSliderInt("Right: %dx", "##UIMARGINR", "gHUDMargin_R", (ImGui::GetWindowViewport()->Size.x)*-1, 25, "", 0, true);
        UIWidgets::EnhancementSliderInt("Bottom: %dx", "##UIMARGINB", "gHUDMargin_B", (ImGui::GetWindowViewport()->Size.y/2)*-1, 25, "", 0, true);
        SetMarginAll("All margins on",true);
        UIWidgets::Tooltip("Set most of the element to use margin\nSome elements with default position will not be affected\nElements without Archor or Hidden will not be turned on");
        ImGui::SameLine();
        SetMarginAll("All margins off",false);
        UIWidgets::Tooltip("Set all of the element to not use margin");
        ImGui::SameLine();
        ResetPositionAll();
        UIWidgets::Tooltip("Revert every element to use their original position and no margins");
        ImGui::NewLine();
        ImGui::EndTable();
    }
    if (ImGui::CollapsingHeader("Hearts count position")) {
        if (ImGui::BeginTable("tableHeartsCounts", 1, FlagsTable)) {
            ImGui::TableSetupColumn("Hearts counts settings", FlagsCell, TablesCellsWidth);
            Table_InitHeader(false);
            DrawUseMarginsSlider("Hearts counts", "gHearts");
            DrawPositionsRadioBoxes("gHeartsCount");
            DrawPositionSlider("gHeartsCount",-22,ImGui::GetWindowViewport()->Size.y,-125,ImGui::GetWindowViewport()->Size.x);
            DrawScaleSlider("gHeartsCount",0.7f);
            UIWidgets::EnhancementSliderInt("Heart line length : %d", "##HeartLineLength", "gHeartsLineLength", 0, 20, "", 10, true);
            UIWidgets::Tooltip("This will set the length of a row of hearts. Set to 0 for unlimited length.");
            ImGui::NewLine();
            ImGui::EndTable();
        }
    }
    if (ImGui::CollapsingHeader("Magic Meter position")) {
        if (ImGui::BeginTable("tablemmpos", 1, FlagsTable)) {
            ImGui::TableSetupColumn("Magic meter settings", FlagsCell, TablesCellsWidth);
            Table_InitHeader(false);
            DrawUseMarginsSlider("Magic meter", "gMagicBar");
            DrawPositionsRadioBoxes("gMagicBar");
            UIWidgets::EnhancementRadioButton("Anchor to life bar", "gMagicBarPosType", 5);
            UIWidgets::Tooltip("This will make your elements follow the bottom of the life meter");
            DrawPositionSlider("gMagicBar", 0, ImGui::GetWindowViewport()->Size.y/2, -5, ImGui::GetWindowViewport()->Size.x/2);
            DrawScaleSlider("gMagicBar",1.0f);
            ImGui::NewLine();
            ImGui::EndTable();
        }
    }
    if (CVar_GetS32("gVisualAgony",0) && ImGui::CollapsingHeader("Visual stone of agony position")) {
        if (ImGui::BeginTable("tabledvisualstoneofagony", 1, FlagsTable)) {
            ImGui::TableSetupColumn("Visual stone of agony settings", FlagsCell, TablesCellsWidth);
            Table_InitHeader(false);
            DrawUseMarginsSlider("Visual stone of agony", "gVSOA");
            DrawPositionsRadioBoxes("gVSOA");
            s16 Min_X_VSOA = 0;
            s16 Max_X_VSOA = ImGui::GetWindowViewport()->Size.x/2;
            if(CVar_GetS32("gVSOAPosType",0) == 2){
                Max_X_VSOA = 290;
            } else if(CVar_GetS32("gVSOAPosType",0) == 4){
                Min_X_VSOA = (ImGui::GetWindowViewport()->Size.x/2)*-1;
            }
            DrawPositionSlider("gVSOA", 0, ImGui::GetWindowViewport()->Size.y/2, Min_X_VSOA, Max_X_VSOA);
            DrawScaleSlider("gVSOA",1.0f);
            ImGui::NewLine();
            ImGui::EndTable();
        }
    }
    if (ImGui::CollapsingHeader("B Button position")) {
        if (ImGui::BeginTable("tablebbtn", 1, FlagsTable)) {
            ImGui::TableSetupColumn("B Button settings", FlagsCell, TablesCellsWidth);
            Table_InitHeader(false);
            DrawUseMarginsSlider("B Button", "gBBtn");
            DrawPositionsRadioBoxes("gBBtn");
            DrawPositionSlider("gBBtn", 0, ImGui::GetWindowViewport()->Size.y/4+50, -1, ImGui::GetWindowViewport()->Size.x-50);
            DrawScaleSlider("gBBtn",0.95f);
            ImGui::NewLine();
            ImGui::EndTable();
        }
    }
    if (ImGui::CollapsingHeader("A Button position")) {
        if (ImGui::BeginTable("tableabtn", 1, FlagsTable)) {
            ImGui::TableSetupColumn("A Button settings", FlagsCell, TablesCellsWidth);
            Table_InitHeader(false);
            DrawUseMarginsSlider("A Button", "gABtn");
            DrawPositionsRadioBoxes("gABtn");
            DrawPositionSlider("gABtn", -10, ImGui::GetWindowViewport()->Size.y/4+50, -20, ImGui::GetWindowViewport()->Size.x-50);
            DrawScaleSlider("gABtn",0.95f);
            ImGui::NewLine();
            ImGui::EndTable();
        }
    }
    if (ImGui::CollapsingHeader("Start Button position")) {
        if (ImGui::BeginTable("tablestartbtn", 1, FlagsTable)) {
            ImGui::TableSetupColumn("Start Button settings", FlagsCell, TablesCellsWidth);
            Table_InitHeader(false);
            DrawUseMarginsSlider("Start Button", "gStartBtn");
            DrawPositionsRadioBoxes("gStartBtn");
            DrawPositionSlider("gStartBtn", 0, ImGui::GetWindowViewport()->Size.y/2, 0, ImGui::GetWindowViewport()->Size.x/2+70);
            DrawScaleSlider("gStartBtn",0.75f);
            ImGui::NewLine();
            ImGui::EndTable();
        }
    }
    if (ImGui::CollapsingHeader("C Button Up position")) {
        if (ImGui::BeginTable("tablecubtn", 1, FlagsTable)) {
            ImGui::TableSetupColumn("C Button Up settings", FlagsCell, TablesCellsWidth);
            Table_InitHeader(false);
            DrawUseMarginsSlider("C Button Up", "gCBtnU");
            DrawPositionsRadioBoxes("gCBtnU");
            s16 Min_X_CU = 0;
            s16 Max_X_CU = ImGui::GetWindowViewport()->Size.x/2;
            if(CVar_GetS32("gCBtnUPosType",0) == 2){
                Max_X_CU = 294;
            } else if(CVar_GetS32("gCBtnUPosType",0) == 3){
                Max_X_CU = ImGui::GetWindowViewport()->Size.x/2;
            } else if(CVar_GetS32("gCBtnUPosType",0) == 4){
                Min_X_CU = (ImGui::GetWindowViewport()->Size.x/2)*-1;
            }
            DrawPositionSlider("gCBtnU", 0, ImGui::GetWindowViewport()->Size.y/2, Min_X_CU, Max_X_CU);
            DrawScaleSlider("gCBtnU",0.5f);
            ImGui::NewLine();
            ImGui::EndTable();
        }
    }
    if (ImGui::CollapsingHeader("C Button Down position")) {
        if (ImGui::BeginTable("tablecdbtn", 1, FlagsTable)) {
            ImGui::TableSetupColumn("C Button Down settings", FlagsCell, TablesCellsWidth);
            Table_InitHeader(false);
            DrawUseMarginsSlider("C Button Down", "gCBtnD");
            DrawPositionsRadioBoxes("gCBtnD");
            s16 Min_X_CD = 0;
            s16 Max_X_CD = ImGui::GetWindowViewport()->Size.x/2;
            if(CVar_GetS32("gCBtnDPosType",0) == 2){
                Max_X_CD = 294;
            } else if(CVar_GetS32("gCBtnDPosType",0) == 3){
                Max_X_CD = ImGui::GetWindowViewport()->Size.x/2;
            } else if(CVar_GetS32("gCBtnDPosType",0) == 4){
                Min_X_CD = (ImGui::GetWindowViewport()->Size.x/2)*-1;
            }
            DrawPositionSlider("gCBtnD", 0, ImGui::GetWindowViewport()->Size.y/2, Min_X_CD, Max_X_CD);
            DrawScaleSlider("gCBtnD",0.87f);
            ImGui::NewLine();
            ImGui::EndTable();
        }
    }
    if (ImGui::CollapsingHeader("C Button Left position")) {
        if (ImGui::BeginTable("tableclbtn", 1, FlagsTable)) {
            ImGui::TableSetupColumn("C Button Left settings", FlagsCell, TablesCellsWidth);
            Table_InitHeader(false);
            DrawUseMarginsSlider("C Button Left", "gCBtnL");
            DrawPositionsRadioBoxes("gCBtnL");
            s16 Min_X_CL = 0;
            s16 Max_X_CL = ImGui::GetWindowViewport()->Size.x/2;
            if(CVar_GetS32("gCBtnLPosType",0) == 2){
                Max_X_CL = 294;
            } else if(CVar_GetS32("gCBtnLPosType",0) == 3){
                Max_X_CL = ImGui::GetWindowViewport()->Size.x/2;
            } else if(CVar_GetS32("gCBtnLPosType",0) == 4){
                Min_X_CL = (ImGui::GetWindowViewport()->Size.x/2)*-1;
            }
            DrawPositionSlider("gCBtnL", 0, ImGui::GetWindowViewport()->Size.y/2, Min_X_CL, Max_X_CL);
            DrawScaleSlider("gCBtnL",0.87f);
            ImGui::NewLine();
            ImGui::EndTable();
        }
    }
    if (ImGui::CollapsingHeader("C Button Right position")) {
        if (ImGui::BeginTable("tablecrnbtn", 1, FlagsTable)) {
            ImGui::TableSetupColumn("C Button Right settings", FlagsCell, TablesCellsWidth);
            Table_InitHeader(false);
            DrawUseMarginsSlider("C Button Right", "gCBtnR");
            DrawPositionsRadioBoxes("gCBtnR");
            s16 Min_X_CR = 0;
            s16 Max_X_CR = ImGui::GetWindowViewport()->Size.x/2;
            if(CVar_GetS32("gCBtnRPosType",0) == 2){
                Max_X_CR = 294;
            } else if(CVar_GetS32("gCBtnRPosType",0) == 3){
                Max_X_CR = ImGui::GetWindowViewport()->Size.x/2;
            } else if(CVar_GetS32("gCBtnRPosType",0) == 4){
                Min_X_CR = (ImGui::GetWindowViewport()->Size.x/2)*-1;
            }
            DrawPositionSlider("gCBtnR", 0, ImGui::GetWindowViewport()->Size.y/2, Min_X_CR, Max_X_CR);
            DrawScaleSlider("gCBtnR",0.87f);
            ImGui::NewLine();
            ImGui::EndTable();
        }
    }
    if (CVar_GetS32("gDpadEquips",0) && ImGui::CollapsingHeader("DPad items position")) {
        if (ImGui::BeginTable("tabledpaditems", 1, FlagsTable)) {
            ImGui::TableSetupColumn("DPad items settings", FlagsCell, TablesCellsWidth);
            Table_InitHeader(false);
            DrawUseMarginsSlider("DPad items", "gDPad");
            DrawPositionsRadioBoxes("gDPad");
            s16 Min_X_Dpad = 0;
            s16 Max_X_Dpad = ImGui::GetWindowViewport()->Size.x/2;
            if(CVar_GetS32("gDPadPosType",0) == 2){
                Max_X_Dpad = 290;
            } else if(CVar_GetS32("gDPadPosType",0) == 4){
                Min_X_Dpad = (ImGui::GetWindowViewport()->Size.x/2)*-1;
            }
            DrawPositionSlider("gDPad", 0, ImGui::GetWindowViewport()->Size.y/2, Min_X_Dpad, Max_X_Dpad);
            DrawScaleSlider("gDPad",1.0f);
            ImGui::NewLine();
            ImGui::EndTable();
        }
    }
    if (ImGui::CollapsingHeader("Minimaps position")) {
        if (ImGui::BeginTable("tableminimapspos", 1, FlagsTable)) {
            ImGui::TableSetupColumn("minimaps settings", FlagsCell, TablesCellsWidth);
            Table_InitHeader(false);
            DrawUseMarginsSlider("Minimap", "gMinimap");
            DrawPositionsRadioBoxes("gMinimap", false);
            DrawPositionSlider("gMinimap", (ImGui::GetWindowViewport()->Size.y/3)*-1, ImGui::GetWindowViewport()->Size.y/3, ImGui::GetWindowViewport()->Size.x*-1, ImGui::GetWindowViewport()->Size.x/2);
            DrawScaleSlider("gMinimap",1.0f);
            ImGui::NewLine();
            ImGui::EndTable();
        }
    }
    if (ImGui::CollapsingHeader("Small Keys counter position")) {
        if (ImGui::BeginTable("tablesmolekeys", 1, FlagsTable)) {
            ImGui::TableSetupColumn("Small Keys counter settings", FlagsCell, TablesCellsWidth);
            Table_InitHeader(false);
            DrawUseMarginsSlider("Small Keys counter", "gSKC");
            DrawPositionsRadioBoxes("gSKC");
            DrawPositionSlider("gSKC", 0, ImGui::GetWindowViewport()->Size.y/3, -1, ImGui::GetWindowViewport()->Size.x/2);
            DrawScaleSlider("gSKC",1.0f);
            ImGui::NewLine();
            ImGui::EndTable();
        }
    }
    if (ImGui::CollapsingHeader("Rupee counter position")) {
        if (ImGui::BeginTable("tablerupeecount", 1, FlagsTable)) {
            ImGui::TableSetupColumn("Rupee counter settings", FlagsCell, TablesCellsWidth);
            Table_InitHeader(false);
            DrawUseMarginsSlider("Rupee counter", "gRC");
            DrawPositionsRadioBoxes("gRC");
            DrawPositionSlider("gRC", -2, ImGui::GetWindowViewport()->Size.y/3, -3, ImGui::GetWindowViewport()->Size.x/2);
            DrawScaleSlider("gRC",1.0f);
            ImGui::NewLine();
            ImGui::EndTable();
        }
    }
    if (ImGui::CollapsingHeader("Carrots position")) {
        if (ImGui::BeginTable("tableCarrots", 1, FlagsTable)) {
            ImGui::TableSetupColumn("Carrots settings", FlagsCell, TablesCellsWidth);
            Table_InitHeader(false);
            DrawUseMarginsSlider("Carrots", "gCarrots");
            DrawPositionsRadioBoxes("gCarrots");
            DrawPositionSlider("gCarrots", 0, ImGui::GetWindowViewport()->Size.y/2, -50, ImGui::GetWindowViewport()->Size.x/2+25);
            DrawScaleSlider("gCarrots",1.0f);
            ImGui::NewLine();
            ImGui::EndTable();
        }
    }
    if (ImGui::CollapsingHeader("Timers position")) {
        if (ImGui::BeginTable("tabletimers", 1, FlagsTable)) {
            ImGui::TableSetupColumn("Timers settings", FlagsCell, TablesCellsWidth);
            Table_InitHeader(false);
            DrawUseMarginsSlider("Timers", "gTimers");
            DrawPositionsRadioBoxes("gTimers");
            DrawPositionSlider("gTimers", 0, ImGui::GetWindowViewport()->Size.y/2, -50, ImGui::GetWindowViewport()->Size.x/2-50);
            DrawScaleSlider("gTimers",1.0f);
            ImGui::NewLine();
            ImGui::EndTable();
        }
    }
    if (ImGui::CollapsingHeader("Archery Scores position")) {
        if (ImGui::BeginTable("tablearchery", 1, FlagsTable)) {
            ImGui::TableSetupColumn("Archery Scores settings", FlagsCell, TablesCellsWidth);
            Table_InitHeader(false);
            DrawUseMarginsSlider("Archery scores", "gAS");
            DrawPositionsRadioBoxes("gAS", false);
            DrawPositionSlider("gAS", 0, ImGui::GetWindowViewport()->Size.y/2, -50, ImGui::GetWindowViewport()->Size.x/2-50);
            DrawScaleSlider("gAS",1.0f);
            ImGui::NewLine();
            ImGui::EndTable();
        }
    }
    if (ImGui::CollapsingHeader("Title cards (Maps) position")) {
        if (ImGui::BeginTable("tabletcmaps", 1, FlagsTable)) {
            ImGui::TableSetupColumn("Titlecard maps settings", FlagsCell, TablesCellsWidth);
            Table_InitHeader(false);
            DrawUseMarginsSlider("Title cards (overworld)", "gTCM");
            DrawPositionsRadioBoxes("gTCM");
            DrawPositionSlider("gTCM", 0, ImGui::GetWindowViewport()->Size.y/2, -50, ImGui::GetWindowViewport()->Size.x/2+10);
            DrawScaleSlider("gTCM",1.0f);
            ImGui::NewLine();
            ImGui::EndTable();
        }
    }
    if (ImGui::CollapsingHeader("Title cards (Bosses) position")) {
        if (ImGui::BeginTable("tabletcbosses", 1, FlagsTable)) {
            ImGui::TableSetupColumn("Title cards (Bosses) settings", FlagsCell, TablesCellsWidth);
            Table_InitHeader(false);
            DrawUseMarginsSlider("Title cards (Bosses)", "gTCB");
            DrawPositionsRadioBoxes("gTCB");
            DrawPositionSlider("gTCB", 0, ImGui::GetWindowViewport()->Size.y/2, -50, ImGui::GetWindowViewport()->Size.x/2+10);
            DrawScaleSlider("gTCB",1.0f);
            ImGui::NewLine();
            ImGui::EndTable();
        }
    }
}

void RandomizeColor(CosmeticOption& cosmeticOption) {
    Color_RGBA8 newColor;
    newColor.r = Random(0, 255);
    newColor.g = Random(0, 255);
    newColor.b = Random(0, 255);
    newColor.a = 255;

    cosmeticOption.currentColor.x = newColor.r / 255.0;
    cosmeticOption.currentColor.y = newColor.g / 255.0;
    cosmeticOption.currentColor.z = newColor.b / 255.0;
    cosmeticOption.currentColor.w = newColor.a / 255.0;

    CVar_SetRGBA(cosmeticOption.cvar, newColor);
    CVar_SetS32((cosmeticOption.rainbowCvar), 0);
    CVar_SetS32((cosmeticOption.changedCvar), 1);
}

void ResetColor(CosmeticOption& cosmeticOption) {
    Color_RGBA8 defaultColor;
    defaultColor.r = cosmeticOption.defaultColor.x;
    defaultColor.g = cosmeticOption.defaultColor.y;
    defaultColor.b = cosmeticOption.defaultColor.z;
    defaultColor.a = cosmeticOption.defaultColor.w;

    cosmeticOption.currentColor.x = defaultColor.r / 255.0;
    cosmeticOption.currentColor.y = defaultColor.g / 255.0;
    cosmeticOption.currentColor.z = defaultColor.b / 255.0;
    cosmeticOption.currentColor.w = defaultColor.a / 255.0;

    CVar_SetRGBA(cosmeticOption.cvar, defaultColor);
    CVar_SetS32((cosmeticOption.rainbowCvar), 0);
    CVar_SetS32((cosmeticOption.changedCvar), 0);
}

void DrawCosmeticRow(CosmeticOption& cosmeticOption) {
    if (ImGui::ColorEdit3(cosmeticOption.label.c_str(), (float*)&cosmeticOption.currentColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel)) {
        Color_RGBA8 color;
        color.r = cosmeticOption.currentColor.x * 255.0;
        color.g = cosmeticOption.currentColor.y * 255.0;
        color.b = cosmeticOption.currentColor.z * 255.0;
        color.a = cosmeticOption.currentColor.w * 255.0;

        CVar_SetRGBA(cosmeticOption.cvar, color);
        CVar_SetS32((cosmeticOption.rainbowCvar), 0);
        CVar_SetS32((cosmeticOption.changedCvar), 1);
        ApplyOrResetCustomGfxPatches();
        SohImGui::RequestCvarSaveOnNextTick();
    }
    ImGui::SameLine();
    ImGui::Text(cosmeticOption.label.c_str());
    ImGui::SameLine((ImGui::CalcTextSize("Mirror Shield Mirror").x * 1.0f) + 60.0f);
    if (ImGui::Button(("Random##" + cosmeticOption.label).c_str())) {
        RandomizeColor(cosmeticOption);
        ApplyOrResetCustomGfxPatches();
        SohImGui::RequestCvarSaveOnNextTick();
    }
    ImGui::SameLine();
    bool isRainbow = (bool)CVar_GetS32((cosmeticOption.rainbowCvar), 0);
    if (ImGui::Checkbox(("Rainbow##" + cosmeticOption.label).c_str(), &isRainbow)) {
        CVar_SetS32((cosmeticOption.rainbowCvar), isRainbow);
        CVar_SetS32((cosmeticOption.changedCvar), 1);
        ApplyOrResetCustomGfxPatches();
        SohImGui::RequestCvarSaveOnNextTick();
    }
    ImGui::SameLine();
    bool isLocked = (bool)CVar_GetS32((cosmeticOption.lockedCvar), 0);
    if (ImGui::Checkbox(("Locked##" + cosmeticOption.label).c_str(), &isLocked)) {
        CVar_SetS32((cosmeticOption.lockedCvar), isLocked);
        SohImGui::RequestCvarSaveOnNextTick();
    }
    if (CVar_GetS32((cosmeticOption.changedCvar), 0)) {
        ImGui::SameLine();
        if (ImGui::Button(("Reset##" + cosmeticOption.label).c_str())) {
            ResetColor(cosmeticOption);
            ApplyOrResetCustomGfxPatches();
            SohImGui::RequestCvarSaveOnNextTick();
        }
    }
}

void DrawCosmeticBox(CosmeticBox cosmeticBox) {
    ImGui::Text(boxLabels.at(cosmeticBox));
    for (auto& [id, cosmeticOption] : cosmeticOptions) {
        if (cosmeticOption.box == cosmeticBox && (!cosmeticOption.advancedOption || CVar_GetS32("gCosmetics.AdvancedMode", 0))) {
            DrawCosmeticRow(cosmeticOption);
        }
    }
}

const char* colorSchemes[2] = {
    "N64",
    "Gamecube",
};

void DrawCosmeticsEditor(bool& open) {
    if (!open) {
        CVar_SetS32("gCosmeticsEditorEnabled", 0);
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(480, 520), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Cosmetics Editor", &open)) {
        ImGui::End();
        return;
    }

    ImGui::Text("Color Scheme");
    ImGui::SameLine();
    UIWidgets::EnhancementCombobox("gCosmetics.DefaultColorScheme", colorSchemes, 2, 0);
    UIWidgets::EnhancementCheckbox("Advanced Mode", "gCosmetics.AdvancedMode");
    if (ImGui::Button("Randomize All", ImVec2(ImGui::GetContentRegionAvail().x / 2, 30.0f))) {
        for (auto& [id, cosmeticOption] : cosmeticOptions) {
            if (!CVar_GetS32(cosmeticOption.lockedCvar, 0) && (!cosmeticOption.advancedOption || CVar_GetS32("gCosmetics.AdvancedMode", 0))) {
                RandomizeColor(cosmeticOption);
            }
        }
        ApplyOrResetCustomGfxPatches();
        SohImGui::RequestCvarSaveOnNextTick();
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset All", ImVec2(ImGui::GetContentRegionAvail().x, 30.0f))) {
        for (auto& [id, cosmeticOption] : cosmeticOptions) {
            if (!CVar_GetS32(cosmeticOption.lockedCvar, 0)) {
                ResetColor(cosmeticOption);
            }
        }
        ApplyOrResetCustomGfxPatches();
        SohImGui::RequestCvarSaveOnNextTick();
    }

    if (ImGui::BeginTabBar("CosmeticsContextTabBar", ImGuiTabBarFlags_NoCloseWithMiddleMouseButton)) {
        if (ImGui::BeginTabItem("Link & Items")) {
            DrawCosmeticBox(BOX_LINK);
            DrawCosmeticBox(BOX_GLOVES);
            DrawCosmeticBox(BOX_MIRRORSHIELD);
            DrawCosmeticBox(BOX_EQUIPMENT);
            DrawCosmeticBox(BOX_SWORDS);
            DrawCosmeticBox(BOX_CONSUMABLE);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Effects")) {
            DrawCosmeticBox(BOX_EFFECTS);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("World")) {
            DrawCosmeticBox(BOX_WORLD);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("NPCs")) {
            DrawCosmeticBox(BOX_NPC);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Silly")) {
            ImGui::Text("Link");
            if (UIWidgets::EnhancementSliderFloat("Scale: %f", "##Link_Scale", "gCosmetics.Link_Scale", 0.001f, 0.025f, "", 0.01f, false)) {
                Player* player = GET_PLAYER(gGlobalCtx);
                player->actor.scale.x = CVar_GetFloat("gCosmetics.Link_Scale", 0.01f);
                player->actor.scale.y = CVar_GetFloat("gCosmetics.Link_Scale", 0.01f);
                player->actor.scale.z = CVar_GetFloat("gCosmetics.Link_Scale", 0.01f);
            }
            ImGui::Text("Bunny Hood");
            UIWidgets::EnhancementSliderFloat("Length: %f", "##BunnyHood_EarLength", "gCosmetics.BunnyHood_EarLength", -300.0f, 1000.0f, "", 0.0f, false);
            UIWidgets::EnhancementSliderFloat("Spread: %f", "##BunnyHood_EarSpread", "gCosmetics.BunnyHood_EarSpread", -300.0f, 500.0f, "", 0.0f, false);
            ImGui::Text("Gorons");
            UIWidgets::EnhancementSliderFloat("Neck Length: %f", "##Goron_NeckLength", "gCosmetics.Goron_NeckLength", 0.0f, 1000.0f, "", 0.0f, false);
            UIWidgets::EnhancementCheckbox("Unfix Goron Spin", "gUnfixGoronSpin");
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("HUD")) {
            DrawCosmeticBox(BOX_HUD);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("HUD Placement")) {
            Draw_Placements();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    ImGui::End();
}

void InitCosmeticsEditor() {
    //This allow to hide a window without disturbing the player nor adding things in menu
    //LoadRainbowColor() will this way run in background once it's window is activated
    //ImGui::SetNextItemWidth(0.0f);
    SohImGui::AddWindow("Enhancements", "Rainbowfunction", LoadRainbowColor, true, true);
    //Draw the bar in the menu.
    SohImGui::AddWindow("Enhancements", "Cosmetics Editor", DrawCosmeticsEditor);
    for (auto& [id, cosmeticOption] : cosmeticOptions) {
        Color_RGBA8 defaultColors;  
        defaultColors.r = cosmeticOption.defaultColor.x;
        defaultColors.g = cosmeticOption.defaultColor.y;
        defaultColors.b = cosmeticOption.defaultColor.z;
        defaultColors.a = cosmeticOption.defaultColor.w;

        Color_RGBA8 cvarColor = CVar_GetRGBA(cosmeticOption.cvar, defaultColors);

        cosmeticOption.currentColor.x = cvarColor.r / 255.0;
        cosmeticOption.currentColor.y = cvarColor.g / 255.0;
        cosmeticOption.currentColor.z = cvarColor.b / 255.0;
        cosmeticOption.currentColor.w = cvarColor.a / 255.0;
    }
    SohImGui::RequestCvarSaveOnNextTick();
    ApplyOrResetCustomGfxPatches();
}
