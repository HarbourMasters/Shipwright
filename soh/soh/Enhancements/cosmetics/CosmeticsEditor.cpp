#include "CosmeticsEditor.h"
#include "cosmeticsTypes.h"
#include "authenticGfxPatches.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"

#include <string>
#include <libultraship/bridge.h>
#include <random>
#include <math.h>
#include <algorithm>
#include <libultraship/libultra/types.h>
#include "soh/Enhancements/randomizer/3drando/random.hpp"
#include <libultraship/libultraship.h>

#include "soh/UIWidgets.hpp"
#include "soh/OTRGlobals.h"

extern "C" {
#include <z64.h>
#include "macros.h"
extern PlayState* gPlayState;
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
#include "objects/object_gi_bomb_2/object_gi_bomb_2.h"
#include "objects/object_gla/object_gla.h"
#include "objects/object_toki_objects/object_toki_objects.h"
#include "objects/object_gi_pachinko/object_gi_pachinko.h"
#include "objects/object_trap/object_trap.h"
#include "overlays/ovl_Boss_Ganon2/ovl_Boss_Ganon2.h"
#include "objects/object_gjyo_objects/object_gjyo_objects.h"
#include "textures/nintendo_rogo_static/nintendo_rogo_static.h"
#include "objects/object_gi_rabit_mask/object_gi_rabit_mask.h"
void ResourceMgr_PatchGfxByName(const char* path, const char* patchName, int index, Gfx instruction);
void ResourceMgr_PatchGfxCopyCommandByName(const char* path, const char* patchName, int destinationIndex, int sourceIndex);
void ResourceMgr_UnpatchGfxByName(const char* path, const char* patchName);
u8 Randomizer_GetSettingValue(RandomizerSettingKey randoSettingKey);
}

// This is used for the greg bridge
#define dgEndGrayscaleAndEndDlistDL "__OTR__helpers/cosmetics/gEndGrayscaleAndEndDlistDL"
static const ALIGN_ASSET(2) char gEndGrayscaleAndEndDlistDL[] = dgEndGrayscaleAndEndDlistDL;

std::map<CosmeticGroup, const char*> groupLabels = {
    { COSMETICS_GROUP_LINK, "Link" },
    { COSMETICS_GROUP_MIRRORSHIELD, "Mirror Shield" },
    { COSMETICS_GROUP_SWORDS, "Swords" },
    { COSMETICS_GROUP_GLOVES, "Gloves" },
    { COSMETICS_GROUP_EQUIPMENT, "Equipment" },
    { COSMETICS_GROUP_CONSUMABLE, "Consumables" },
    { COSMETICS_GROUP_HUD, "HUD" },
    { COSMETICS_GROUP_KALEIDO, "Pause Menu" },
    { COSMETICS_GROUP_TITLE, "Title Screen" },
    { COSMETICS_GROUP_NPC, "NPCs" },
    { COSMETICS_GROUP_WORLD, "World" },
    { COSMETICS_GROUP_MAGIC, "Magic Effects" },
    { COSMETICS_GROUP_ARROWS, "Arrow Effects" },
    { COSMETICS_GROUP_SPIN_ATTACK, "Spin Attack" },
    { COSMETICS_GROUP_TRAILS, "Trails" },
    { COSMETICS_GROUP_NAVI, "Navi" },
    { COSMETICS_GROUP_IVAN, "Ivan" } 
};

typedef struct {
    const char* cvar;
    const char* rainbowCvar;
    const char* lockedCvar;
    const char* changedCvar;
    std::string label;
    CosmeticGroup group;
    ImVec4 currentColor;
    ImVec4 defaultColor;
    bool supportsAlpha;
    bool supportsRainbow;
    bool advancedOption;
} CosmeticOption;

#define COSMETIC_OPTION(id, label, group, defaultColor, supportsAlpha, supportsRainbow, advancedOption) \
    { id, { \
        CVAR_COSMETIC(id ".Value"), CVAR_COSMETIC(id ".Rainbow"), CVAR_COSMETIC(id ".Locked"), CVAR_COSMETIC(id ".Changed"), label, group, \
        defaultColor, defaultColor, \
        supportsAlpha, supportsRainbow, advancedOption \
    } }

/*
    So, you would like to add a new cosmetic option? BUCKLE UP

    To preface this, if you have any questions or concerns ping @ProxySaw on discord, if I'm no longer available ask around in the #development-chat

    # Silly Options
    Lets get this one out of the way, probably the only thing that will be consistent between silly options is how they are rendered
    on the ImGui tab. So when adding one just make sure it follows the same general pattern as the rest. Notably:
    - Make sure to SaveConsoleVariablesOnNextTick(), forgetting this will not persist your changes
    - Make sure reset properly resets the value
    - Depending on your use case you may or may not have to split the cvar into two values (cvar.Changed & cvar.Value)

    # Finding your color
    So the first order of business is finding the source of the color you are trying to change. There are four scenarios to be aware
    of, in order of difficulty from easiest to hardest:
    1. Color is in code
    2. Color is in DList with gsDPSetPrimColor/gsDPSetEnvColor commands
    3. Color is embedded in a TLUT, which is applied to the texture
    4. Color is embedded in the texture itself

    I would recommend first finding the draw function for whatever you are looking for. In most cases this will be an actor, and the actor's draw
    func will be at the bottom of their overlay file, `EnCow_Draw` for ACTOR_EN_COW is one example. There can also be additional nested draw methods
    like drawing each limb of an actor for instance that you will also want to inspect. What you are looking for is any sort of RGB values, or calls
    directly to gDPSetPrimColor/gDPSetEnvColor in code. If you find one, try changing the arguments and see if that's what you are looking for.

    If this fails, and you aren't able to find any colors within the source of the actor/whatever you will now need to investigate the DLists 
    that are being rendered. The easiest way to do this is to use the experimental Display List Viewer in the developer tools options. An
    alternative to this is to dig through the source of the DLists after you have built the zeldaret/oot repository, but this will be much more
    manual, and I can't provide instructions for it.

    Assuming you are planning on using the Display List Viewer, you need to find the name of the DList to inspect. In the same areas you were looking
    for RGB values you now want to look for calls to gSPDisplayList, or variables that end in "DL". Once you have this name start typing parts of 
    it into the dlist-viewer (in the developer dropdown) and select the desired dlist in the dropdown, there may be many. You will now see a
    list of commands associated with the DList you have selected. If you are lucky, there will be calls to gsDPSetPrimColor/gsDPSetEnvColor with
    the RGB values editable, and you can edit those to determine if that is the DList you are looking for. If it is, make note of the name and
    the index of the DList command you just edited, as you will need that going forward.

    If you are unlucky, this means it is very likely the color you want to change is embedded in a TLUT or the texture itself. We can work around
    this by using grayscale coloring, but this is advanced and I won't be providing a walkthrough for it here, you'll just have to read through
    the existing cosmetic options to get an understanding of how to do this.

    # Add your option to the editor
    This step should be as simple as adding a single line in the map below, ensure you add it to the appropriate place and with the default colors

    # Applying your color
    If you have determined your color is in code, this should just be as simple as replacing it, or the call it's used in if and only if it has
    been changed. Example from the moon cosmetic option:

    ```cpp
    // original
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 240, 255, 180, alpha);
    gDPSetEnvColor(POLY_OPA_DISP++, 80, 70, 20, alpha);

    // with cosmetics change
    if (CVarGetInteger(CVAR_COSMETIC("World.Moon.Changed"), 0)) {
        Color_RGB8 moonColor = CVarGetColor24(CVAR_COSMETIC("World.Moon.Value"), (Color_RGB8){ 0, 0, 240 });
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, moonColor.r, moonColor.g, moonColor.b, alpha);
        gDPSetEnvColor(POLY_OPA_DISP++, moonColor.r / 2, moonColor.g / 2, moonColor.b / 2, alpha);
    } else {
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 240, 255, 180, alpha);
        gDPSetEnvColor(POLY_OPA_DISP++, 80, 70, 20, alpha);
    }
    ```

    If instead what you found was that your color was set via a gsDPSetPrimColor command within a DList, you will need to follow the pattern 
    displayed in `ApplyOrResetCustomGfxPatches`, using the name of the Dlist, and index of the command you want to replace appropriately.

    # Applying variants of the same color
    This applies to all of the different kinds of cosmetics, in some cases you will need to apply multiple shades of the same color, otherwise
    you end up with a weird color between the original color and the new color for example. One demonstration on how to handle this is shown above
    in the moon cosmetic, where for the gDPSetEnvColor color we are halving the RGB values, to make them a bit darker similar to how the original
    colors were darker than the gDPSetPrimColor. You will see many more examples of this below in the `ApplyOrResetCustomGfxPatches` method
*/
static std::map<std::string, CosmeticOption> cosmeticOptions = {
    COSMETIC_OPTION("Link.KokiriTunic",              "Kokiri Tunic",         COSMETICS_GROUP_LINK,         ImVec4( 30, 105,  27, 255), false, true, false),
    COSMETIC_OPTION("Link.GoronTunic",               "Goron Tunic",          COSMETICS_GROUP_LINK,         ImVec4(100,  20,   0, 255), false, true, false),
    COSMETIC_OPTION("Link.ZoraTunic",                "Zora Tunic",           COSMETICS_GROUP_LINK,         ImVec4(  0,  60, 100, 255), false, true, false),
    COSMETIC_OPTION("Link.Hair",                     "Hair",                 COSMETICS_GROUP_LINK,         ImVec4(255, 173,  27, 255), false, true, true),
    COSMETIC_OPTION("Link.Linen",                    "Linen",                COSMETICS_GROUP_LINK,         ImVec4(255, 255, 255, 255), false, true, true),
    COSMETIC_OPTION("Link.Boots",                    "Boots",                COSMETICS_GROUP_LINK,         ImVec4( 93,  44,  18, 255), false, true, true),
    
    COSMETIC_OPTION("MirrorShield.Body",             "Body",                 COSMETICS_GROUP_MIRRORSHIELD, ImVec4(215,   0,   0, 255), false, true, false),
    COSMETIC_OPTION("MirrorShield.Mirror",           "Mirror",               COSMETICS_GROUP_MIRRORSHIELD, ImVec4(255, 255, 255, 255), false, true, true),
    COSMETIC_OPTION("MirrorShield.Emblem",           "Emblem",               COSMETICS_GROUP_MIRRORSHIELD, ImVec4(205, 225, 255, 255), false, true, true),

    COSMETIC_OPTION("Swords.KokiriBlade",            "Kokiri Sword Blade",   COSMETICS_GROUP_SWORDS,       ImVec4(255, 255, 255, 255), false, true, false),
    // COSMETIC_OPTION("Swords.KokiriHilt",             "Kokiri Sword Hilt",    COSMETICS_GROUP_SWORDS,       ImVec4(160, 100,  15, 255), false, true, true), // Todo (Cosmetics): Broken, need a better way to grayscale
    COSMETIC_OPTION("Swords.MasterBlade",            "Master Sword Blade",   COSMETICS_GROUP_SWORDS,       ImVec4(255, 255, 255, 255), false, true, false),
    // COSMETIC_OPTION("Swords.MasterHilt",             "Master Sword Hilt",    COSMETICS_GROUP_SWORDS,       ImVec4( 80,  80, 168, 255), false, true, true), // Todo (Cosmetics): Broken, need a better way to grayscale
    COSMETIC_OPTION("Swords.BiggoronBlade",          "Biggoron Sword Blade", COSMETICS_GROUP_SWORDS,       ImVec4(255, 255, 255, 255), false, true, false),
    // COSMETIC_OPTION("Swords.BiggoronHilt",           "Biggoron Sword Hilt",  COSMETICS_GROUP_SWORDS,       ImVec4( 80,  80, 168, 255), false, true, true), // Todo (Cosmetics): Broken, need a better way to grayscale

    COSMETIC_OPTION("Gloves.GoronBracelet",          "Goron Bracelet",       COSMETICS_GROUP_GLOVES,       ImVec4(255, 255, 170, 255), false, true, false),
    COSMETIC_OPTION("Gloves.SilverGauntlets",        "Silver Gauntlets",     COSMETICS_GROUP_GLOVES,       ImVec4(255, 255, 255, 255), false, true, false),
    COSMETIC_OPTION("Gloves.GoldenGauntlets",        "Golden Gauntlets",     COSMETICS_GROUP_GLOVES,       ImVec4(254, 207,  15, 255), false, true, false),
    COSMETIC_OPTION("Gloves.GauntletsGem",           "Gauntlets Gem",        COSMETICS_GROUP_GLOVES,       ImVec4(255,  60, 100, 255), false, true, true),
    
    COSMETIC_OPTION("Equipment.BoomerangBody",       "Boomerang Body",       COSMETICS_GROUP_EQUIPMENT,    ImVec4(160, 100,   0, 255), false, true, false),
    COSMETIC_OPTION("Equipment.BoomerangGem",        "Boomerang Gem",        COSMETICS_GROUP_EQUIPMENT,    ImVec4(255,  50, 150, 255), false, true, true),
    // COSMETIC_OPTION("Equipment.SlingshotBody",       "Slingshot Body",       COSMETICS_GROUP_EQUIPMENT,    ImVec4(160, 100,   0, 255), false, true, true), // Todo (Cosmetics): Broken, need a better way to grayscale
    COSMETIC_OPTION("Equipment.SlingshotString",     "Slingshot String",     COSMETICS_GROUP_EQUIPMENT,    ImVec4(255, 255, 255, 255), false, true, true),
    COSMETIC_OPTION("Equipment.HammerHead",          "Hammer Head",          COSMETICS_GROUP_EQUIPMENT,    ImVec4(155, 192, 201, 255), false, true, false),
    COSMETIC_OPTION("Equipment.HammerHandle",        "Hammer Handle",        COSMETICS_GROUP_EQUIPMENT,    ImVec4(110,  60,   0, 255), false, true, true),
    // COSMETIC_OPTION("Equipment.HookshotChain",       "Hookshot Chain",       COSMETICS_GROUP_EQUIPMENT,    ImVec4(255, 255, 255, 255), false, true, true), // Todo (Cosmetics): Implement
    // COSMETIC_OPTION("Equipment.HookshotTip",         "Hookshot Tip",         COSMETICS_GROUP_EQUIPMENT,    ImVec4(255, 255, 255, 255), false, true, false), // Todo (Cosmetics): Implement
    COSMETIC_OPTION("HookshotReticle.Target",        "Hookshotable Reticle", COSMETICS_GROUP_EQUIPMENT,         ImVec4(  0, 255,   0, 255), false, false, false),
    COSMETIC_OPTION("HookshotReticle.NonTarget",     "Non-Hookshotable Reticle", COSMETICS_GROUP_EQUIPMENT,     ImVec4(255,   0,   0, 255), false, false, false),
    COSMETIC_OPTION("Equipment.BowTips",             "Bow Tips",             COSMETICS_GROUP_EQUIPMENT,    ImVec4(200,   0,   0, 255), false, true, true),
    COSMETIC_OPTION("Equipment.BowString",           "Bow String",           COSMETICS_GROUP_EQUIPMENT,    ImVec4(255, 255, 255, 255), false, true, true),
    COSMETIC_OPTION("Equipment.BowBody",             "Bow Body",             COSMETICS_GROUP_EQUIPMENT,    ImVec4(140,  90,  10, 255), false, true, false),
    COSMETIC_OPTION("Equipment.BowHandle",           "Bow Handle",           COSMETICS_GROUP_EQUIPMENT,    ImVec4( 50, 150, 255, 255), false, true, true),
    COSMETIC_OPTION("Equipment.ChuFace",             "Bombchu Face",         COSMETICS_GROUP_EQUIPMENT,    ImVec4(  0, 100, 150, 255), false, true, true),
    COSMETIC_OPTION("Equipment.ChuBody",             "Bombchu Body",         COSMETICS_GROUP_EQUIPMENT,    ImVec4(180, 130,  50, 255), false, true, true), 
    COSMETIC_OPTION("Equipment.BunnyHood",           "Bunny Hood",           COSMETICS_GROUP_EQUIPMENT,    ImVec4(255, 235,  109, 255), false, true, true), 

    COSMETIC_OPTION("Consumable.Hearts",             "Hearts",               COSMETICS_GROUP_CONSUMABLE,   ImVec4(255,  70,  50, 255), false, true, false),
    COSMETIC_OPTION("Consumable.HeartBorder",        "Heart Border",         COSMETICS_GROUP_CONSUMABLE,   ImVec4( 50,  40,  60, 255), false, true, true),
    COSMETIC_OPTION("Consumable.DDHearts",           "DD Hearts",            COSMETICS_GROUP_CONSUMABLE,   ImVec4(200,   0,   0, 255), false, true, false),
    COSMETIC_OPTION("Consumable.DDHeartBorder",      "DD Heart Border",      COSMETICS_GROUP_CONSUMABLE,   ImVec4(255, 255, 255, 255), false, true, true),
    COSMETIC_OPTION("Consumable.Magic",              "Magic",                COSMETICS_GROUP_CONSUMABLE,   ImVec4(  0, 200,   0, 255), false, true, false),
    COSMETIC_OPTION("Consumable.MagicActive",        "Magic Active",         COSMETICS_GROUP_CONSUMABLE,   ImVec4(250, 250,   0, 255), false, true, true),
    COSMETIC_OPTION("Consumable_MagicInfinite",      "Infinite Magic",       COSMETICS_GROUP_CONSUMABLE,   ImVec4(  0,   0, 200, 255), false, true, true),
    COSMETIC_OPTION("Consumable.MagicBorder",        "Magic Border",         COSMETICS_GROUP_CONSUMABLE,   ImVec4(255, 255, 255, 255), false, false, true),
    COSMETIC_OPTION("Consumable.MagicBorderActive",  "Magic Border Active",  COSMETICS_GROUP_CONSUMABLE,   ImVec4(255, 255, 255, 255), false, false, true),
    COSMETIC_OPTION("Consumable.GreenRupee",         "Green Rupee",          COSMETICS_GROUP_CONSUMABLE,   ImVec4( 50, 255,  50, 255), false, true, true),
    COSMETIC_OPTION("Consumable.BlueRupee",          "Blue Rupee",           COSMETICS_GROUP_CONSUMABLE,   ImVec4( 50,  50, 255, 255), false, true, true),
    COSMETIC_OPTION("Consumable.RedRupee",           "Red Rupee",            COSMETICS_GROUP_CONSUMABLE,   ImVec4(255,  50,  50, 255), false, true, true),
    COSMETIC_OPTION("Consumable.PurpleRupee",        "Purple Rupee",         COSMETICS_GROUP_CONSUMABLE,   ImVec4(150,  50, 255, 255), false, true, true),
    COSMETIC_OPTION("Consumable.GoldRupee",          "Gold Rupee",           COSMETICS_GROUP_CONSUMABLE,   ImVec4(255, 190,  55, 255), false, true, true),
    COSMETIC_OPTION("Consumable.SilverRupee",        "Silver Rupee",         COSMETICS_GROUP_CONSUMABLE,   ImVec4(255, 255, 255, 255), false, true, true),

    COSMETIC_OPTION("HUD.AButton",                   "A Button",             COSMETICS_GROUP_HUD,          ImVec4( 90,  90, 255, 255), false, true, false),
    COSMETIC_OPTION("HUD.BButton",                   "B Button",             COSMETICS_GROUP_HUD,          ImVec4(  0, 150,   0, 255), false, true, false),
    COSMETIC_OPTION("HUD.CButtons",                  "C Buttons",            COSMETICS_GROUP_HUD,          ImVec4(255, 160,   0, 255), false, true, false),
    COSMETIC_OPTION("HUD.CUpButton",                 "C Up Button",          COSMETICS_GROUP_HUD,          ImVec4(255, 160,   0, 255), false, true, true),
    COSMETIC_OPTION("HUD.CDownButton",               "C Down Button",        COSMETICS_GROUP_HUD,          ImVec4(255, 160,   0, 255), false, true, true),
    COSMETIC_OPTION("HUD.CLeftButton",               "C Left Button",        COSMETICS_GROUP_HUD,          ImVec4(255, 160,   0, 255), false, true, true),
    COSMETIC_OPTION("HUD.CRightButton",              "C Right Button",       COSMETICS_GROUP_HUD,          ImVec4(255, 160,   0, 255), false, true, true),
    COSMETIC_OPTION("HUD.StartButton",               "Start Button",         COSMETICS_GROUP_HUD,          ImVec4(200,   0,   0, 255), false, true, false),
    COSMETIC_OPTION("HUD.Dpad",                      "Dpad",                 COSMETICS_GROUP_HUD,          ImVec4(255, 255, 255, 255), false, true, false),
    COSMETIC_OPTION("HUD.KeyCount",                  "Key Count",            COSMETICS_GROUP_HUD,          ImVec4(200, 230, 255, 255), false, true, true),
    COSMETIC_OPTION("HUD.StoneOfAgony",              "Stone of Agony",       COSMETICS_GROUP_HUD,          ImVec4(255, 255, 255, 255), false, true, true),
    COSMETIC_OPTION("HUD.Minimap",                   "Minimap",              COSMETICS_GROUP_HUD,          ImVec4(  0, 255, 255, 255), false, true, false),
    COSMETIC_OPTION("HUD.MinimapPosition",           "Minimap Position",     COSMETICS_GROUP_HUD,          ImVec4(200, 255,   0, 255), false, true, true),
    COSMETIC_OPTION("HUD.MinimapEntrance",           "Minimap Entrance",     COSMETICS_GROUP_HUD,          ImVec4(200,   0,   0, 255), false, true, true),
    COSMETIC_OPTION("HUD.EnemyHealthBar",            "Enemy Health Bar",     COSMETICS_GROUP_HUD,          ImVec4(255,   0,   0, 255), true, true, false),
    COSMETIC_OPTION("HUD.EnemyHealthBorder",         "Enemy Health Border",  COSMETICS_GROUP_HUD,          ImVec4(255, 255, 255, 255), true, false, true),
    COSMETIC_OPTION("HUD.NameTagActorText",          "Nametag Text",         COSMETICS_GROUP_HUD,          ImVec4(255, 255, 255, 255), true, true, false),
    COSMETIC_OPTION("HUD.NameTagActorBackground",    "Nametag Background",   COSMETICS_GROUP_HUD,          ImVec4(0,     0,   0,  80), true, false, true),
    // Todo (Cosmetics): re-implement title card colors

    COSMETIC_OPTION("Kaleido.ItemSelA",                  "Item Select Color",    COSMETICS_GROUP_KALEIDO,      ImVec4(10,   50,  80, 255), false, true, false),
    COSMETIC_OPTION("Kaleido.ItemSelB",                  "Item Select Color B",  COSMETICS_GROUP_KALEIDO,      ImVec4(70,  100, 130, 255), false, true, true),
    COSMETIC_OPTION("Kaleido.ItemSelC",                  "Item Select Color C",  COSMETICS_GROUP_KALEIDO,      ImVec4(70,  100, 130, 255), false, true, true),
    COSMETIC_OPTION("Kaleido.ItemSelD",                  "Item Select Color D",  COSMETICS_GROUP_KALEIDO,      ImVec4(10,   50,  80, 255), false, true, true),

    COSMETIC_OPTION("Kaleido.EquipSelA",                 "Equip Select Color",   COSMETICS_GROUP_KALEIDO,      ImVec4(10,   50,  40, 255), false, true, false),
    COSMETIC_OPTION("Kaleido.EquipSelB",                 "Equip Select Color B", COSMETICS_GROUP_KALEIDO,      ImVec4(90,  100,  60, 255), false, true, true),
    COSMETIC_OPTION("Kaleido.EquipSelC",                 "Equip Select Color C", COSMETICS_GROUP_KALEIDO,      ImVec4(90,  100,  60, 255), false, true, true),
    COSMETIC_OPTION("Kaleido.EquipSelD",                 "Equip Select Color D", COSMETICS_GROUP_KALEIDO,      ImVec4(10,   50,  80, 255), false, true, true),

    COSMETIC_OPTION("Kaleido.MapSelDunA",                "Map Dungeon Color",    COSMETICS_GROUP_KALEIDO,      ImVec4(80,   40,  30, 255), false, true, true),
    COSMETIC_OPTION("Kaleido.MapSelDunB",                "Map Dungeon Color B",  COSMETICS_GROUP_KALEIDO,      ImVec4(140,  60,  60, 255), false, true, true),
    COSMETIC_OPTION("Kaleido.MapSelDunC",                "Map Dungeon Color C",  COSMETICS_GROUP_KALEIDO,      ImVec4(140,  60,  60, 255), false, true, true),
    COSMETIC_OPTION("Kaleido.MapSelDunD",                "Map Dungeon Color D",  COSMETICS_GROUP_KALEIDO,      ImVec4(80,   40,  30, 255), false, true, true),

    COSMETIC_OPTION("Kaleido.QuestStatusA",              "Quest Status Color",   COSMETICS_GROUP_KALEIDO,      ImVec4(80, 80, 50, 255),    false, true, false),
    COSMETIC_OPTION("Kaleido.QuestStatusB",              "Quest Status Color B", COSMETICS_GROUP_KALEIDO,      ImVec4(120, 120, 70, 255),  false, true, true),
    COSMETIC_OPTION("Kaleido.QuestStatusC",              "Quest Status Color C", COSMETICS_GROUP_KALEIDO,      ImVec4(120, 120, 70, 255),  false, true, true),
    COSMETIC_OPTION("Kaleido.QuestStatusD",              "Quest Status Color D", COSMETICS_GROUP_KALEIDO,      ImVec4(80, 80, 50, 255),    false, true, true),

    COSMETIC_OPTION("Kaleido.MapSelectA",                "Map Color",            COSMETICS_GROUP_KALEIDO,      ImVec4(80, 40, 30, 255),    false, true, false),
    COSMETIC_OPTION("Kaleido.MapSelectB",                "Map Color B",          COSMETICS_GROUP_KALEIDO,      ImVec4(140, 60, 60, 255),   false, true, true),
    COSMETIC_OPTION("Kaleido.MapSelectC",                "Map Color C",          COSMETICS_GROUP_KALEIDO,      ImVec4(140, 60, 60, 255),   false, true, true),
    COSMETIC_OPTION("Kaleido.MapSelectD",                "Map Color D",          COSMETICS_GROUP_KALEIDO,      ImVec4(80, 40, 30, 255),    false, true, true),

    COSMETIC_OPTION("Kaleido.SaveA",                     "Save Color",           COSMETICS_GROUP_KALEIDO,      ImVec4(50, 50, 50, 255),    false, true, false),
    COSMETIC_OPTION("Kaleido.SaveB",                     "Save Color B",         COSMETICS_GROUP_KALEIDO,      ImVec4(110, 110, 110, 255), false, true, true),
    COSMETIC_OPTION("Kaleido.SaveC",                     "Save Color C",         COSMETICS_GROUP_KALEIDO,      ImVec4(110, 110, 110, 255), false, true, true),
    COSMETIC_OPTION("Kaleido.SaveD",                     "Save Color D",         COSMETICS_GROUP_KALEIDO,      ImVec4(50, 50, 50, 255),    false, true, true),

    COSMETIC_OPTION("Kaleido.NamePanel",                 "Name Panel",           COSMETICS_GROUP_KALEIDO,      ImVec4(90,100,130,255),     true, true, false),

    COSMETIC_OPTION("Title.FileChoose",              "File Choose",          COSMETICS_GROUP_TITLE,        ImVec4(100, 150, 255, 255), false, true, false),
    COSMETIC_OPTION("Title.NintendoLogo",            "Nintendo Logo",        COSMETICS_GROUP_TITLE,        ImVec4(  0,   0, 255, 255), false, true, true),
    COSMETIC_OPTION("Title.N64LogoRed",              "N64 Red",              COSMETICS_GROUP_TITLE,        ImVec4(150,   0,   0, 255), false, true, true),
    COSMETIC_OPTION("Title.N64LogoBlue",             "N64 Blue",             COSMETICS_GROUP_TITLE,        ImVec4(  0,  50, 150, 255), false, true, true),
    COSMETIC_OPTION("Title.N64LogoGreen",            "N64 Green",            COSMETICS_GROUP_TITLE,        ImVec4( 50, 100,   0, 255), false, true, true),
    COSMETIC_OPTION("Title.N64LogoYellow",           "N64 Yellow",           COSMETICS_GROUP_TITLE,        ImVec4(200, 150,   0, 255), false, true, true),
    // COSMETIC_OPTION("Title.FirePrimary",             "Title Fire Primary",   COSMETICS_GROUP_TITLE,        ImVec4(255, 255, 170, 255), false, true, false), // Todo (Cosmetics): Kinda complicated
    // COSMETIC_OPTION("Title.FireSecondary",           "Title Fire Secondary", COSMETICS_GROUP_TITLE,        ImVec4(255, 100,   0, 255), false, true, true), // Todo (Cosmetics): Kinda complicated

    COSMETIC_OPTION("Arrows.NormalPrimary",          "Normal Primary",       COSMETICS_GROUP_ARROWS,       ImVec4(  0, 150,   0,   0), false, true, false),
    COSMETIC_OPTION("Arrows.NormalSecondary",        "Normal Secondary",     COSMETICS_GROUP_ARROWS,       ImVec4(255, 255, 170, 255), false, true, true),
    COSMETIC_OPTION("Arrows.FirePrimary",            "Fire Primary",         COSMETICS_GROUP_ARROWS,       ImVec4(255, 200,   0,   0), false, true, false),
    COSMETIC_OPTION("Arrows.FireSecondary",          "Fire Secondary",       COSMETICS_GROUP_ARROWS,       ImVec4(255,   0,   0, 255), false, true, true),
    COSMETIC_OPTION("Arrows.IcePrimary",             "Ice Primary",          COSMETICS_GROUP_ARROWS,       ImVec4(  0,   0, 255, 255), false, true, false),
    COSMETIC_OPTION("Arrows.IceSecondary",           "Ice Secondary",        COSMETICS_GROUP_ARROWS,       ImVec4(255, 255, 255, 255), false, true, true),
    COSMETIC_OPTION("Arrows.LightPrimary",           "Light Primary",        COSMETICS_GROUP_ARROWS,       ImVec4(255, 255,   0, 255), false, true, false),
    COSMETIC_OPTION("Arrows.LightSecondary",         "Light Secondary",      COSMETICS_GROUP_ARROWS,       ImVec4(255, 255, 170,   0), false, true, true),

    // COSMETIC_OPTION("Magic.DinsPrimary",             "Din's Primary",        COSMETICS_GROUP_MAGIC,        ImVec4(255, 255, 255, 255), false, true, false),
    // COSMETIC_OPTION("Magic.DinsSecondary",           "Din's Secondary",      COSMETICS_GROUP_MAGIC,        ImVec4(255, 255, 255, 255), false, true, true),
    // COSMETIC_OPTION("Magic.FaroresPrimary",          "Farore's Primary",     COSMETICS_GROUP_MAGIC,        ImVec4(255, 255, 255, 255), false, true, false), // Todo (Cosmetics): Implement
    // COSMETIC_OPTION("Magic.FaroresSecondary",        "Farore's Secondary",   COSMETICS_GROUP_MAGIC,        ImVec4(255, 255, 255, 255), false, true, true), // Todo (Cosmetics): Implement
    // COSMETIC_OPTION("Magic.NayrusPrimary",           "Nayru's Primary",      COSMETICS_GROUP_MAGIC,        ImVec4(255, 255, 255, 255), false, true, false),
    // COSMETIC_OPTION("Magic.NayrusSecondary",         "Nayru's Secondary",    COSMETICS_GROUP_MAGIC,        ImVec4(255, 255, 255, 255), false, true, true),

    COSMETIC_OPTION("SpinAttack.Level1Primary",      "Level 1 Primary",      COSMETICS_GROUP_SPIN_ATTACK,  ImVec4(170, 255, 255, 255), false, true, true),
    COSMETIC_OPTION("SpinAttack.Level1Secondary",    "Level 1 Secondary",    COSMETICS_GROUP_SPIN_ATTACK,  ImVec4(  0, 100, 255, 255), false, true, false),
    COSMETIC_OPTION("SpinAttack.Level2Primary",      "Level 2 Primary",      COSMETICS_GROUP_SPIN_ATTACK,  ImVec4(255, 255, 170, 255), false, true, true),
    COSMETIC_OPTION("SpinAttack.Level2Secondary",    "Level 2 Secondary",    COSMETICS_GROUP_SPIN_ATTACK,  ImVec4(255, 100,   0, 255), false, true, false),

    COSMETIC_OPTION("Trails.Bombchu",                "Bombchu",              COSMETICS_GROUP_TRAILS,       ImVec4(250,   0,   0, 255), false, true, true),
    COSMETIC_OPTION("Trails.Boomerang",              "Boomerang",            COSMETICS_GROUP_TRAILS,       ImVec4(255, 255, 100, 255), false, true, true),
    COSMETIC_OPTION("Trails.KokiriSword",            "Kokiri Sword",         COSMETICS_GROUP_TRAILS,       ImVec4(255, 255, 255, 255), false, true, false),
    COSMETIC_OPTION("Trails.MasterSword",            "Master Sword",         COSMETICS_GROUP_TRAILS,       ImVec4(255, 255, 255, 255), false, true, false),
    COSMETIC_OPTION("Trails.BiggoronSword",          "Biggoron Sword",       COSMETICS_GROUP_TRAILS,       ImVec4(255, 255, 255, 255), false, true, true),
    COSMETIC_OPTION("Trails.Stick",                  "Stick",                COSMETICS_GROUP_TRAILS,       ImVec4(255, 255, 255, 255), false, true, true),
    COSMETIC_OPTION("Trails.Hammer",                 "Hammer",               COSMETICS_GROUP_TRAILS,       ImVec4(255, 255, 255, 255), false, true, true),

    COSMETIC_OPTION("World.BlockOfTime",             "Block of Time",        COSMETICS_GROUP_WORLD,        ImVec4(255, 255, 255, 255), false, true, true),
    COSMETIC_OPTION("World.Moon",                    "Moon",                 COSMETICS_GROUP_WORLD,        ImVec4(240, 255, 180, 255), false, true, true),
    COSMETIC_OPTION("World.GossipStone",             "Gossip Stone",         COSMETICS_GROUP_WORLD,        ImVec4(200, 200, 200, 255), false, true, true),
    COSMETIC_OPTION("World.RedIce",                  "Red Ice",              COSMETICS_GROUP_WORLD,        ImVec4(255,   0,   0, 255), false, true, false),
    COSMETIC_OPTION("World.MysteryItem",             "Mystery Item",         COSMETICS_GROUP_WORLD,        ImVec4(0,    60, 100, 255), false, true, false),

    COSMETIC_OPTION("Navi.IdlePrimary",              "Idle Primary",         COSMETICS_GROUP_NAVI,         ImVec4(255, 255, 255, 255), false, true, false),
    COSMETIC_OPTION("Navi.IdleSecondary",            "Idle Secondary",       COSMETICS_GROUP_NAVI,         ImVec4(  0,   0, 255,   0), false, true, true),
    COSMETIC_OPTION("Navi.NPCPrimary",               "NPC Primary",          COSMETICS_GROUP_NAVI,         ImVec4(150, 150, 255, 255), false, true, false),
    COSMETIC_OPTION("Navi.NPCSecondary",             "NPC Secondary",        COSMETICS_GROUP_NAVI,         ImVec4(150, 150, 255,   0), false, true, true),
    COSMETIC_OPTION("Navi.EnemyPrimary",             "Enemy Primary",        COSMETICS_GROUP_NAVI,         ImVec4(255, 255,   0, 255), false, true, false),
    COSMETIC_OPTION("Navi.EnemySecondary",           "Enemy Secondary",      COSMETICS_GROUP_NAVI,         ImVec4(200, 155,   0,   0), false, true, true),
    COSMETIC_OPTION("Navi.PropsPrimary",             "Props Primary",        COSMETICS_GROUP_NAVI,         ImVec4(  0, 255,   0, 255), false, true, false),
    COSMETIC_OPTION("Navi.PropsSecondary",           "Props Secondary",      COSMETICS_GROUP_NAVI,         ImVec4(  0, 255,   0,   0), false, true, true),
    
    COSMETIC_OPTION("Ivan.IdlePrimary",              "Ivan Idle Primary",    COSMETICS_GROUP_IVAN,         ImVec4(255, 255, 255, 255), false, true, false),
    COSMETIC_OPTION("Ivan.IdleSecondary",            "Ivan Idle Secondary",  COSMETICS_GROUP_IVAN,         ImVec4(  0, 255,   0, 255), false, true, true),

    COSMETIC_OPTION("NPC.FireKeesePrimary",          "Fire Keese Primary",   COSMETICS_GROUP_NPC,          ImVec4(255, 255, 255, 255), false, true, false),
    COSMETIC_OPTION("NPC.FireKeeseSecondary",        "Fire Keese Secondary", COSMETICS_GROUP_NPC,          ImVec4(255, 255, 255, 255), false, true, true),
    COSMETIC_OPTION("NPC.IceKeesePrimary",           "Ice Keese Primary",    COSMETICS_GROUP_NPC,          ImVec4(255, 255, 255, 255), false, true, false),
    COSMETIC_OPTION("NPC.IceKeeseSecondary",         "Ice Keese Secondary",  COSMETICS_GROUP_NPC,          ImVec4(255, 255, 255, 255), false, true, true),
    // Todo (Cosmetics): Health fairy
    COSMETIC_OPTION("NPC.Dog1",                      "Dog 1",                COSMETICS_GROUP_NPC,          ImVec4(255, 255, 200, 255), false, true, true),
    COSMETIC_OPTION("NPC.Dog2",                      "Dog 2",                COSMETICS_GROUP_NPC,          ImVec4(150, 100,  50, 255), false, true, true),
    COSMETIC_OPTION("NPC.GoldenSkulltula",           "Golden Skulltula",     COSMETICS_GROUP_NPC,          ImVec4(255, 255, 255, 255), false, true, false),
    COSMETIC_OPTION("NPC.Kokiri",                    "Kokiri",               COSMETICS_GROUP_NPC,          ImVec4(  0, 130,  70, 255), false, true, false),
    COSMETIC_OPTION("NPC.Gerudo",                    "Gerudo",               COSMETICS_GROUP_NPC,          ImVec4( 90,   0, 140, 255), false, true, false),
    COSMETIC_OPTION("NPC.MetalTrap",                 "Metal Trap",           COSMETICS_GROUP_NPC,          ImVec4(255, 255, 255, 255), false, true, true),
    COSMETIC_OPTION("NPC.IronKnuckles",              "Iron Knuckles",        COSMETICS_GROUP_NPC,          ImVec4(245, 255, 205, 255), false, true, false),
};

static const char* MarginCvarList[] {
    CVAR_COSMETIC("HUD.Hearts"), CVAR_COSMETIC("HUD.HeartsCount"), CVAR_COSMETIC("HUD.MagicBar"), CVAR_COSMETIC("HUD.VisualSoA"), CVAR_COSMETIC("HUD.BButton"), CVAR_COSMETIC("HUD.AButton"), CVAR_COSMETIC("HUD.StartButton"), 
    CVAR_COSMETIC("HUD.CUpButton"), CVAR_COSMETIC("HUD.CDownButton"), CVAR_COSMETIC("HUD.CLeftButton"), CVAR_COSMETIC("HUD.CRightButton"), CVAR_COSMETIC("HUD.Dpad"), CVAR_COSMETIC("HUD.Minimap"), 
    CVAR_COSMETIC("HUD.SmallKey"), CVAR_COSMETIC("HUD.Rupees"), CVAR_COSMETIC("HUD.Carrots"),  CVAR_COSMETIC("HUD.Timers"), CVAR_COSMETIC("HUD.ArcheryScore"), CVAR_COSMETIC("HUD.TitleCard.Map"), CVAR_COSMETIC("HUD.TitleCard.Boss"), CVAR_COSMETIC("HUD.IGT")
};
static const char* MarginCvarNonAnchor[]{ CVAR_COSMETIC("HUD.Carrots"), CVAR_COSMETIC("HUD.Timers"), CVAR_COSMETIC("HUD.ArcheryScore"), CVAR_COSMETIC("HUD.TitleCard.Map"),CVAR_COSMETIC("HUD.TitleCard.Boss") };

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
        //MarginCvarNonAnchor is an array that list every element that has No anchor by default, because if that the case this function will not touch it with pose type 0.
        u8 arrayLengthNonMargin = sizeof(MarginCvarNonAnchor) / sizeof(*MarginCvarNonAnchor);
        for (auto cvarName : MarginCvarList) {
            std::string cvarPosType = std::string(cvarName).append(".PosType");
            std::string cvarNameMargins = std::string(cvarName).append(".UseMargins");
            if (CVarGetInteger(cvarPosType.c_str(),0) <= 2 && SetActivated) { //Our element is not Hidden or Non anchor
                for (int i = 0; i < arrayLengthNonMargin; i++){
                    if ((strcmp(cvarName, MarginCvarNonAnchor[i]) == 0) && (CVarGetInteger(cvarPosType.c_str(), 0) == 0)) { //Our element is both in original position and do not have anchor by default so we skip it.
                        CVarSetInteger(cvarNameMargins.c_str(), false); //force set off
                    } 
                    else if ((strcmp(cvarName, MarginCvarNonAnchor[i]) == 0) && (CVarGetInteger(cvarPosType.c_str(), 0) != 0)) { //Our element is not in original position regarless it has no anchor by default since player made it anchored we can toggle margins
                        CVarSetInteger(cvarNameMargins.c_str(), SetActivated);
                    } 
                    else if (strcmp(cvarName, MarginCvarNonAnchor[i]) != 0) { //Our elements has an anchor by default so regarless of it's position right now that okay to toggle margins.
                        CVarSetInteger(cvarNameMargins.c_str(), SetActivated);
                    }
                }
            } 
            else { //Since the user requested to turn all margin off no need to do any check there.
                CVarSetInteger(cvarNameMargins.c_str(), SetActivated);
            }
        }
    }
}
void ResetPositionAll() {
    if (ImGui::Button("Reset all positions")) {
        for (auto cvarName : MarginCvarList) {
            std::string cvarPosType = std::string(cvarName).append(".PosType");
            std::string cvarNameMargins = std::string(cvarName).append(".UseMargins");
            CVarSetInteger(cvarPosType.c_str(), 0);
            CVarSetInteger(cvarNameMargins.c_str(), false); //Turn margin off to everythings as that original position.
        }
    }
}

int hue = 0;

// Runs every frame to update rainbow hue, a potential future optimization is to only run this a once or twice a second and increase the speed of the rainbow hue rotation.
void CosmeticsUpdateTick() {
    int index = 0;
    float rainbowSpeed = CVarGetFloat(CVAR_COSMETIC("RainbowSpeed"), 0.6f);
    for (auto& [id, cosmeticOption] : cosmeticOptions) {
        if (cosmeticOption.supportsRainbow && CVarGetInteger(cosmeticOption.rainbowCvar, 0)) {
            float frequency = 2 * M_PI / (360 * rainbowSpeed);
            Color_RGBA8 newColor;
            newColor.r = sin(frequency * (hue + index) + 0) * 127 + 128;
            newColor.g = sin(frequency * (hue + index) + (2 * M_PI / 3)) * 127 + 128;
            newColor.b = sin(frequency * (hue + index) + (4 * M_PI / 3)) * 127 + 128;
            newColor.a = 255;
            // For alpha supported options, retain the last set alpha instead of overwriting
            if (cosmeticOption.supportsAlpha) {
                newColor.a = cosmeticOption.currentColor.w * 255;
            }

            cosmeticOption.currentColor.x = newColor.r / 255.0;
            cosmeticOption.currentColor.y = newColor.g / 255.0;
            cosmeticOption.currentColor.z = newColor.b / 255.0;
            cosmeticOption.currentColor.w = newColor.a / 255.0;

            CVarSetColor(cosmeticOption.cvar, newColor);
        }
        // If we don't want the rainbow color on items to be synced, offset the index for each item in the loop.
        // Technically this would work if you replaced "60" with 1 but the hue would be so close it's 
        // indistinguishable, 60 gives us a big enough gap to notice the difference.
        if (!CVarGetInteger(CVAR_COSMETIC("RainbowSync"), 0)) {
            index+= (60 * rainbowSpeed);
        }
    }
    ApplyOrResetCustomGfxPatches(false);
    hue++;
    if (hue >= (360 * rainbowSpeed)) hue = 0;
}

/* 
    This is called every time a color is changed in the UI, and every frame to update colors that have rainbow mode enabled
    The columns/arguments for PATCH_GFX are as follows:
    1. Display List: This should be a valid display list pointer, if you have errors listing one here make sure to include the appropriate header file up top
    2. Patch Name: Each of these MUST be unique, used for restoring the original DList command, if these are not unique bugs WILL happen
    3. Changed Cvar: What determines if a patch should be applied or reset.
    4. GFX Command Index: Index of the GFX command you want to replace, the instructions on finding this are in the giant comment block above the cosmeticOptions map
    5. GFX Command: The GFX command you want to insert
*/
void ApplyOrResetCustomGfxPatches(bool manualChange) {
    static CosmeticOption& linkGoronTunic = cosmeticOptions.at("Link.GoronTunic");
    if (manualChange || CVarGetInteger(linkGoronTunic.rainbowCvar, 0)) {
        static Color_RGBA8 defaultColor = {linkGoronTunic.defaultColor.x, linkGoronTunic.defaultColor.y, linkGoronTunic.defaultColor.z, linkGoronTunic.defaultColor.w};
        Color_RGBA8 color = CVarGetColor(linkGoronTunic.cvar, defaultColor);
        PATCH_GFX(gGiGoronTunicColorDL,                           "Link_GoronTunic1",         linkGoronTunic.changedCvar,           3, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGiGoronCollarColorDL,                          "Link_GoronTunic2",         linkGoronTunic.changedCvar,           3, gsDPSetPrimColor(0, 0, color.r / 2, color.g / 2, color.b / 2, 255));
        PATCH_GFX(gGiGoronTunicColorDL,                           "Link_GoronTunic3",         linkGoronTunic.changedCvar,           4, gsDPSetEnvColor(color.r / 2, color.g / 2, color.b / 2, 255));
        PATCH_GFX(gGiGoronCollarColorDL,                          "Link_GoronTunic4",         linkGoronTunic.changedCvar,           4, gsDPSetEnvColor(color.r / 2, color.g / 2, color.b / 2, 255));
    }

    static CosmeticOption& linkZoraTunic = cosmeticOptions.at("Link.ZoraTunic");
    if (manualChange || CVarGetInteger(linkZoraTunic.rainbowCvar, 0)) {
        static Color_RGBA8 defaultColor = {linkZoraTunic.defaultColor.x, linkZoraTunic.defaultColor.y, linkZoraTunic.defaultColor.z, linkZoraTunic.defaultColor.w};
        Color_RGBA8 color = CVarGetColor(linkZoraTunic.cvar, defaultColor);
        PATCH_GFX(gGiZoraTunicColorDL,                            "Link_ZoraTunic1",          linkZoraTunic.changedCvar,            3, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGiZoraCollarColorDL,                           "Link_ZoraTunic2",          linkZoraTunic.changedCvar,            3, gsDPSetPrimColor(0, 0, color.r / 2, color.g / 2, color.b / 2, 255));
        PATCH_GFX(gGiZoraTunicColorDL,                            "Link_ZoraTunic3",          linkZoraTunic.changedCvar,            4, gsDPSetEnvColor(color.r / 2, color.g / 2, color.b / 2, 255));
        PATCH_GFX(gGiZoraCollarColorDL,                           "Link_ZoraTunic4",          linkZoraTunic.changedCvar,            4, gsDPSetEnvColor(color.r / 2, color.g / 2, color.b / 2, 255));
    }

    static CosmeticOption& linkHair = cosmeticOptions.at("Link.Hair");
    if (manualChange || CVarGetInteger(linkHair.rainbowCvar, 0)) {
        static Color_RGBA8 defaultColor = {linkHair.defaultColor.x, linkHair.defaultColor.y, linkHair.defaultColor.z, linkHair.defaultColor.w};
        Color_RGBA8 color = CVarGetColor(linkHair.cvar, defaultColor);
        PATCH_GFX(gLinkChildHeadNearDL,                           "Link_Hair1",               linkHair.changedCvar,                10, gsDPSetGrayscaleColor(color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkChildHeadFarDL,                            "Link_Hair2",               linkHair.changedCvar,                10, gsDPSetGrayscaleColor(color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultHeadNearDL,                           "Link_Hair3",               linkHair.changedCvar,                10, gsDPSetGrayscaleColor(color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultHeadFarDL,                            "Link_Hair4",               linkHair.changedCvar,                10, gsDPSetGrayscaleColor(color.r, color.g, color.b, 255));
    
        if (manualChange) {
        PATCH_GFX(gLinkChildHeadNearDL,                           "Link_Hair5",               linkHair.changedCvar,                46, gsSPGrayscale(true));
        PATCH_GFX(gLinkChildHeadNearDL,                           "Link_Hair6",               linkHair.changedCvar,               54, gsSPGrayscale(false));
        PATCH_GFX(gLinkChildHeadNearDL,                           "Link_Hair7",               linkHair.changedCvar,               136, gsSPGrayscale(true));
        PATCH_GFX(gLinkChildHeadNearDL,                           "Link_Hair8",               linkHair.changedCvar,               162, gsSPGrayscale(false));
        PATCH_GFX(gLinkChildHeadFarDL,                            "Link_Hair9",               linkHair.changedCvar,               101, gsSPGrayscale(true));
        PATCH_GFX(gLinkChildHeadFarDL,                            "Link_Hair10",              linkHair.changedCvar,               118, gsSPGrayscale(false));
        PATCH_GFX(gLinkAdultHeadNearDL,                           "Link_Hair11",              linkHair.changedCvar,               125, gsSPGrayscale(true));
        PATCH_GFX(gLinkAdultHeadNearDL,                           "Link_Hair12",              linkHair.changedCvar,               159, gsSPGrayscale(false));
        PATCH_GFX(gLinkAdultHeadFarDL,                            "Link_Hair13",              linkHair.changedCvar,               102, gsSPGrayscale(true));
        PATCH_GFX(gLinkAdultHeadFarDL,                            "Link_Hair14",              linkHair.changedCvar,               122, gsSPGrayscale(false));
        }
    }

    static CosmeticOption& linkLinen = cosmeticOptions.at("Link.Linen");
    if (manualChange || CVarGetInteger(linkLinen.rainbowCvar, 0)) {
        static Color_RGBA8 defaultColor = {linkLinen.defaultColor.x, linkLinen.defaultColor.y, linkLinen.defaultColor.z, linkLinen.defaultColor.w};
        Color_RGBA8 color = CVarGetColor(linkLinen.cvar, defaultColor);
        PATCH_GFX(gLinkAdultLeftArmNearDL,                        "Link_Linen1",              linkLinen.changedCvar,               30, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultLeftArmNearDL,                        "Link_Linen2",              linkLinen.changedCvar,               83, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultLeftArmOutNearDL,                     "Link_Linen3",              linkLinen.changedCvar,               25, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultLeftArmFarDL,                         "Link_Linen4",              linkLinen.changedCvar,               30, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultLeftArmFarDL,                         "Link_Linen5",              linkLinen.changedCvar,               70, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultRightArmFarDL,                        "Link_Linen6",              linkLinen.changedCvar,               30, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultRightArmFarDL,                        "Link_Linen7",              linkLinen.changedCvar,               70, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultRightArmNearDL,                       "Link_Linen8",              linkLinen.changedCvar,               30, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultLeftShoulderFarDL,                    "Link_Linen9",              linkLinen.changedCvar,               55, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultLeftShoulderNearDL,                   "Link_Linen10",             linkLinen.changedCvar,               57, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultRightShoulderNearDL,                  "Link_Linen11",             linkLinen.changedCvar,               57, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultRightShoulderFarDL,                   "Link_Linen12",             linkLinen.changedCvar,               55, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultTorsoNearDL,                          "Link_Linen13",             linkLinen.changedCvar,               66, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultTorsoFarDL,                           "Link_Linen14",             linkLinen.changedCvar,               57, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultRightThighNearDL,                     "Link_Linen15",             linkLinen.changedCvar,               53, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultLeftThighNearDL,                      "Link_Linen16",             linkLinen.changedCvar,               53, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultRightThighFarDL,                      "Link_Linen17",             linkLinen.changedCvar,               54, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultLeftThighFarDL,                       "Link_Linen18",             linkLinen.changedCvar,               54, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultRightLegNearDL,                       "Link_Linen19",             linkLinen.changedCvar,               30, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultLeftLegNearDL,                        "Link_Linen20",             linkLinen.changedCvar,               30, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultRightLegFarDL,                        "Link_Linen21",             linkLinen.changedCvar,               30, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultLeftLegFarDL,                         "Link_Linen22",             linkLinen.changedCvar,               30, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));

        if (manualChange) {
        PATCH_GFX(gLinkAdultLeftArmFarDL,                         "Link_Linen23",             linkLinen.changedCvar,               35, gsDPSetPrimColor(0, 0, 255, 255, 255, 255));
        PATCH_GFX(gLinkAdultLeftArmOutNearDL,                     "Link_Linen24",             linkLinen.changedCvar,               45, gsDPSetPrimColor(0, 0, 255, 255, 255, 255));
        PATCH_GFX(gLinkAdultLeftArmNearDL,                        "Link_Linen25",             linkLinen.changedCvar,               40, gsDPSetPrimColor(0, 0, 255, 255, 255, 255));
        PATCH_GFX(gLinkAdultLeftArmFarDL,                         "Link_Linen26",             linkLinen.changedCvar,               77, gsDPSetPrimColor(0, 0, 255, 255, 255, 255));
        PATCH_GFX(gLinkAdultRightArmFarDL,                        "Link_Linen27",             linkLinen.changedCvar,               35, gsDPSetPrimColor(0, 0, 255, 255, 255, 255));
        PATCH_GFX(gLinkAdultRightArmFarDL,                        "Link_Linen28",             linkLinen.changedCvar,               77, gsDPSetPrimColor(0, 0, 255, 255, 255, 255));
        PATCH_GFX(gLinkAdultRightArmNearDL,                       "Link_Linen29",             linkLinen.changedCvar,               42, gsDPSetPrimColor(0, 0, 255, 255, 255, 255));
        PATCH_GFX(gLinkAdultRightLegNearDL,                       "Link_Linen30",             linkLinen.changedCvar,               43, gsDPSetPrimColor(0, 0, 255, 255, 255, 255));
        PATCH_GFX(gLinkAdultLeftLegNearDL,                        "Link_Linen31",             linkLinen.changedCvar,               43, gsDPSetPrimColor(0, 0, 255, 255, 255, 255));
        PATCH_GFX(gLinkAdultRightLegFarDL,                        "Link_Linen32",             linkLinen.changedCvar,               38, gsDPSetPrimColor(0, 0, 255, 255, 255, 255));
        PATCH_GFX(gLinkAdultLeftLegFarDL,                         "Link_Linen33",             linkLinen.changedCvar,               38, gsDPSetPrimColor(0, 0, 255, 255, 255, 255));
        }
    }

    static CosmeticOption& linkBoots = cosmeticOptions.at("Link.Boots");
    if (manualChange || CVarGetInteger(linkBoots.rainbowCvar, 0)) {
        static Color_RGBA8 defaultColor = {linkBoots.defaultColor.x, linkBoots.defaultColor.y, linkBoots.defaultColor.z, linkBoots.defaultColor.w};
        Color_RGBA8 color = CVarGetColor(linkBoots.cvar, defaultColor);
        PATCH_GFX(gLinkChildRightShinNearDL,                      "Link_Boots1",              linkBoots.changedCvar,               10, gsDPSetGrayscaleColor(color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkChildRightShinFarDL,                       "Link_Boots2",              linkBoots.changedCvar,               10, gsDPSetGrayscaleColor(color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultRightLegNearDL,                       "Link_Boots3",              linkBoots.changedCvar,               10, gsDPSetGrayscaleColor(color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultRightLegFarDL,                        "Link_Boots4",              linkBoots.changedCvar,               10, gsDPSetGrayscaleColor(color.r, color.g, color.b, 255));

        if (manualChange) {
        PATCH_GFX(gLinkChildRightShinNearDL,                      "Link_Boots5",              linkBoots.changedCvar,               53, gsSPGrayscale(true));
        PATCH_GFX(gLinkChildRightShinNearDL,                      "Link_Boots6",              linkBoots.changedCvar,               69, gsSPGrayscale(false));
        PATCH_GFX(gLinkChildRightShinFarDL,                       "Link_Boots7",              linkBoots.changedCvar,               52, gsSPGrayscale(true));
        PATCH_GFX(gLinkChildRightShinFarDL,                       "Link_Boots8",              linkBoots.changedCvar,               61, gsSPGrayscale(false));
        PATCH_GFX(gLinkChildLeftShinNearDL,                       "Link_Boots9",              linkBoots.changedCvar,               53, gsSPGrayscale(true));
        PATCH_GFX(gLinkChildLeftShinNearDL,                       "Link_Boots10",             linkBoots.changedCvar,               69, gsSPGrayscale(false));
        PATCH_GFX(gLinkChildLeftShinFarDL,                        "Link_Boots11",             linkBoots.changedCvar,               52, gsSPGrayscale(true));
        PATCH_GFX(gLinkChildLeftShinFarDL,                        "Link_Boots12",             linkBoots.changedCvar,               61, gsSPGrayscale(false));
        PATCH_GFX(gLinkChildRightFootNearDL,                      "Link_Boots13",             linkBoots.changedCvar,               30, gsSPGrayscale(true));
        PATCH_GFX(gLinkChildRightFootFarDL,                       "Link_Boots14",             linkBoots.changedCvar,               30, gsSPGrayscale(true));
        PATCH_GFX(gLinkChildLeftFootNearDL,                       "Link_Boots15",             linkBoots.changedCvar,               30, gsSPGrayscale(true));
        PATCH_GFX(gLinkChildLeftFootFarDL,                        "Link_Boots16",             linkBoots.changedCvar,               30, gsSPGrayscale(true));
        PATCH_GFX(gLinkChildLeftThighNearDL,                      "Link_Boots17",             linkBoots.changedCvar,               10, gsSPGrayscale(false));
        PATCH_GFX(gLinkChildLeftThighFarDL,                       "Link_Boots18",             linkBoots.changedCvar,               10, gsSPGrayscale(false));
        PATCH_GFX(gLinkChildHeadNearDL,                           "Link_Boots19",             linkBoots.changedCvar,               20, gsSPGrayscale(false));
        PATCH_GFX(gLinkChildHeadFarDL,                            "Link_Boots20",             linkBoots.changedCvar,               20, gsSPGrayscale(false));
        PATCH_GFX(gLinkAdultRightLegNearDL,                       "Link_Boots21",             linkBoots.changedCvar,               57, gsSPGrayscale(true));
        PATCH_GFX(gLinkAdultRightLegFarDL,                        "Link_Boots22",             linkBoots.changedCvar,               52, gsSPGrayscale(true));
        PATCH_GFX(gLinkAdultLeftLegNearDL,                        "Link_Boots23",             linkBoots.changedCvar,               57, gsSPGrayscale(true));
        PATCH_GFX(gLinkAdultLeftLegFarDL,                         "Link_Boots24",             linkBoots.changedCvar,               52, gsSPGrayscale(true));
        PATCH_GFX(gLinkAdultLeftThighNearDL,                      "Link_Boots25",             linkBoots.changedCvar,               10, gsSPGrayscale(false));
        PATCH_GFX(gLinkAdultLeftThighFarDL,                       "Link_Boots26",             linkBoots.changedCvar,               10, gsSPGrayscale(false));
        PATCH_GFX(gLinkAdultHeadNearDL,                           "Link_Boots27",             linkBoots.changedCvar,               20, gsSPGrayscale(false));
        PATCH_GFX(gLinkAdultHeadFarDL,                            "Link_Boots28",             linkBoots.changedCvar,               20, gsSPGrayscale(false));
        }
    }

    static CosmeticOption& mirrorShieldBody = cosmeticOptions.at("MirrorShield.Body");
    if (manualChange || CVarGetInteger(mirrorShieldBody.rainbowCvar, 0)) {
        static Color_RGBA8 defaultColor = {mirrorShieldBody.defaultColor.x, mirrorShieldBody.defaultColor.y, mirrorShieldBody.defaultColor.z, mirrorShieldBody.defaultColor.w};
        Color_RGBA8 color = CVarGetColor(mirrorShieldBody.cvar, defaultColor);
        PATCH_GFX(gGiMirrorShieldDL,                              "MirrorShield_Body1",       mirrorShieldBody.changedCvar,         5, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGiMirrorShieldDL,                              "MirrorShield_Body2",       mirrorShieldBody.changedCvar,         6, gsDPSetEnvColor(color.r / 3, color.g / 3, color.b / 3, 255));
        PATCH_GFX(gLinkAdultMirrorShieldSwordAndSheathNearDL,     "MirrorShield_Body3",       mirrorShieldBody.changedCvar,        28, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultMirrorShieldSwordAndSheathFarDL,      "MirrorShield_Body4",       mirrorShieldBody.changedCvar,        17, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultMirrorShieldAndSheathNearDL,          "MirrorShield_Body5",       mirrorShieldBody.changedCvar,        28, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultMirrorShieldAndSheathFarDL,           "MirrorShield_Body6",       mirrorShieldBody.changedCvar,        17, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultRightHandHoldingMirrorShieldNearDL,   "MirrorShield_Body7",       mirrorShieldBody.changedCvar,        28, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultRightHandHoldingMirrorShieldFarDL,    "MirrorShield_Body8",       mirrorShieldBody.changedCvar,        95, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
    }
    static CosmeticOption& mirrorShieldMirror = cosmeticOptions.at("MirrorShield.Mirror");
    if (manualChange || CVarGetInteger(mirrorShieldMirror.rainbowCvar, 0)) {
        static Color_RGBA8 defaultColor = {mirrorShieldMirror.defaultColor.x, mirrorShieldMirror.defaultColor.y, mirrorShieldMirror.defaultColor.z, mirrorShieldMirror.defaultColor.w};
        Color_RGBA8 color = CVarGetColor(mirrorShieldMirror.cvar, defaultColor);
        PATCH_GFX(gGiMirrorShieldDL,                              "MirrorShield_Mirror1",     mirrorShieldMirror.changedCvar,      47, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGiMirrorShieldDL,                              "MirrorShield_Mirror2",     mirrorShieldMirror.changedCvar,      48, gsDPSetEnvColor(color.r / 3, color.g / 3, color.b / 3, 255));
        PATCH_GFX(gLinkAdultMirrorShieldSwordAndSheathNearDL,     "MirrorShield_Mirror3",     mirrorShieldMirror.changedCvar,      17, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultMirrorShieldSwordAndSheathFarDL,      "MirrorShield_Mirror4",     mirrorShieldMirror.changedCvar,      33, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultMirrorShieldAndSheathNearDL,          "MirrorShield_Mirror5",     mirrorShieldMirror.changedCvar,      17, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultMirrorShieldAndSheathFarDL,           "MirrorShield_Mirror6",     mirrorShieldMirror.changedCvar,      33, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultRightHandHoldingMirrorShieldNearDL,   "MirrorShield_Mirror7",     mirrorShieldMirror.changedCvar,      17, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultRightHandHoldingMirrorShieldFarDL,    "MirrorShield_Mirror8",     mirrorShieldMirror.changedCvar,     111, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
    }
    static CosmeticOption& mirrorShieldEmblem = cosmeticOptions.at("MirrorShield.Emblem");
    if (manualChange || CVarGetInteger(mirrorShieldEmblem.rainbowCvar, 0)) {
        static Color_RGBA8 defaultColor = {mirrorShieldEmblem.defaultColor.x, mirrorShieldEmblem.defaultColor.y, mirrorShieldEmblem.defaultColor.z, mirrorShieldEmblem.defaultColor.w};
        Color_RGBA8 color = CVarGetColor(mirrorShieldEmblem.cvar, defaultColor);
        PATCH_GFX(gGiMirrorShieldSymbolDL,                        "MirrorShield_Emblem1",     mirrorShieldEmblem.changedCvar,       5, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 140));
        PATCH_GFX(gGiMirrorShieldSymbolDL,                        "MirrorShield_Emblem2",     mirrorShieldEmblem.changedCvar,       6, gsDPSetEnvColor(color.r / 3, color.g / 3, color.b / 3, 255));
        PATCH_GFX(gLinkAdultMirrorShieldSwordAndSheathNearDL,     "MirrorShield_Emblem3",     mirrorShieldEmblem.changedCvar,     165, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultMirrorShieldSwordAndSheathFarDL,      "MirrorShield_Emblem4",     mirrorShieldEmblem.changedCvar,     135, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultMirrorShieldAndSheathNearDL,          "MirrorShield_Emblem5",     mirrorShieldEmblem.changedCvar,     129, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultMirrorShieldAndSheathFarDL,           "MirrorShield_Emblem6",     mirrorShieldEmblem.changedCvar,     103, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultRightHandHoldingMirrorShieldNearDL,   "MirrorShield_Emblem7",     mirrorShieldEmblem.changedCvar,     162, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultRightHandHoldingMirrorShieldFarDL,    "MirrorShield_Emblem8",     mirrorShieldEmblem.changedCvar,     133, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
    }

    static CosmeticOption& swordsKokiriBlade = cosmeticOptions.at("Swords.KokiriBlade");
    if (manualChange || CVarGetInteger(swordsKokiriBlade.rainbowCvar, 0)) {
        static Color_RGBA8 defaultColor = {swordsKokiriBlade.defaultColor.x, swordsKokiriBlade.defaultColor.y, swordsKokiriBlade.defaultColor.z, swordsKokiriBlade.defaultColor.w};
        Color_RGBA8 color = CVarGetColor(swordsKokiriBlade.cvar, defaultColor);
        PATCH_GFX(gLinkChildLeftFistAndKokiriSwordNearDL,         "Swords_KokiriBlade1",      swordsKokiriBlade.changedCvar,       79, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkChildLeftFistAndKokiriSwordFarDL,          "Swords_KokiriBlade2",      swordsKokiriBlade.changedCvar,       75, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGiKokiriSwordDL,                               "Swords_KokiriBlade3",      swordsKokiriBlade.changedCvar,        5, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGiKokiriSwordDL,                               "Swords_KokiriBlade4",      swordsKokiriBlade.changedCvar,        6, gsDPSetEnvColor(color.r / 4, color.g / 4, color.b / 4, 255));
    }
    // static CosmeticOption& swordsKokiriHilt = cosmeticOptions.at("Swords.KokiriHilt");
    // if (manualChange || CVarGetInteger(swordsKokiriHilt.rainbowCvar, 0)) {
    //     static Color_RGBA8 defaultColor = {swordsKokiriHilt.defaultColor.x, swordsKokiriHilt.defaultColor.y, swordsKokiriHilt.defaultColor.z, swordsKokiriHilt.defaultColor.w};
    //     Color_RGBA8 color = CVarGetColor(swordsKokiriHilt.cvar, defaultColor);
    //     PATCH_GFX(gLinkChildLeftFistAndKokiriSwordNearDL,         "Swords_KokiriHilt1",       swordsKokiriHilt.changedCvar,         4, gsDPSetGrayscaleColor(color.r, color.g, color.b, 255));
    //     PATCH_GFX(gLinkChildLeftFistAndKokiriSwordFarDL,          "Swords_KokiriHilt2",       swordsKokiriHilt.changedCvar,         4, gsDPSetGrayscaleColor(color.r, color.g, color.b, 255));
    //     PATCH_GFX(gLinkChildSwordAndSheathNearDL,                 "Swords_KokiriHilt3",       swordsKokiriHilt.changedCvar,         4, gsDPSetGrayscaleColor(color.r, color.g, color.b, 255));
    //     PATCH_GFX(gLinkChildSwordAndSheathFarDL,                  "Swords_KokiriHilt4",       swordsKokiriHilt.changedCvar,         4,  gsDPSetGrayscaleColor(color.r, color.g, color.b, 255));
    //     PATCH_GFX(gLinkChildDekuShieldSwordAndSheathNearDL,       "Swords_KokiriHilt5",       swordsKokiriHilt.changedCvar,         4,  gsDPSetGrayscaleColor(color.r, color.g, color.b, 255));
    //     PATCH_GFX(gLinkChildDekuShieldSwordAndSheathFarDL,        "Swords_KokiriHilt6",       swordsKokiriHilt.changedCvar,         4,  gsDPSetGrayscaleColor(color.r, color.g, color.b, 255));
    //     PATCH_GFX(gLinkChildHylianShieldSwordAndSheathNearDL,     "Swords_KokiriHilt7",       swordsKokiriHilt.changedCvar,         4,  gsDPSetGrayscaleColor(color.r, color.g, color.b, 255));
    //     PATCH_GFX(gLinkChildHylianShieldSwordAndSheathFarDL,      "Swords_KokiriHilt8",       swordsKokiriHilt.changedCvar,         4,  gsDPSetGrayscaleColor(color.r, color.g, color.b, 255));
    //     PATCH_GFX(gGiKokiriSwordDL,                               "Swords_KokiriHilt9",       swordsKokiriHilt.changedCvar,        64,  gsDPSetPrimColor(0, 0, MAX(color.r - 50, 0), MAX(color.g - 50, 0), MAX(color.b - 50, 0), 255));
    //     PATCH_GFX(gGiKokiriSwordDL,                               "Swords_KokiriHilt10",      swordsKokiriHilt.changedCvar,        66,  gsDPSetEnvColor(MAX(color.r - 50, 0) / 3, MAX(color.g - 50, 0) / 3, MAX(color.b - 50, 0) / 3, 255));
    //     PATCH_GFX(gGiKokiriSwordDL,                               "Swords_KokiriHilt11",      swordsKokiriHilt.changedCvar,       162,  gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
    //     PATCH_GFX(gGiKokiriSwordDL,                               "Swords_KokiriHilt12",      swordsKokiriHilt.changedCvar,       164,  gsDPSetEnvColor(color.r / 3, color.g / 3, color.b / 3, 255));

    //     if (manualChange) {
    //     PATCH_GFX(gLinkChildLeftFistAndKokiriSwordNearDL,         "Swords_KokiriHilt13",      swordsKokiriHilt.changedCvar,       108, gsSPGrayscale(true));
    //     PATCH_GFX(gLinkChildLeftFistAndKokiriSwordNearDL,         "Swords_KokiriHilt14",      swordsKokiriHilt.changedCvar,       134, gsSPGrayscale(false));
    //     PATCH_GFX(gLinkChildLeftFistAndKokiriSwordFarDL,          "Swords_KokiriHilt15",      swordsKokiriHilt.changedCvar,       106, gsSPGrayscale(true));
    //     PATCH_GFX(gLinkChildLeftFistAndKokiriSwordFarDL,          "Swords_KokiriHilt16",      swordsKokiriHilt.changedCvar,       126, gsSPGrayscale(false));
    //     PATCH_GFX(gLinkChildSwordAndSheathNearDL,                 "Swords_KokiriHilt17",      swordsKokiriHilt.changedCvar,       100, gsSPGrayscale(true));
    //     PATCH_GFX(gLinkChildSwordAndSheathNearDL,                 "Swords_KokiriHilt18",      swordsKokiriHilt.changedCvar,       126, gsSPGrayscale(false));
    //     PATCH_GFX(gLinkChildSwordAndSheathNearDL,                 "Swords_KokiriHilt19",      swordsKokiriHilt.changedCvar,       128, gsSPEndDisplayList());
    //     PATCH_GFX(gLinkChildSwordAndSheathFarDL,                  "Swords_KokiriHilt20",      swordsKokiriHilt.changedCvar,        98, gsSPGrayscale(true));
    //     PATCH_GFX(gLinkChildSwordAndSheathFarDL,                  "Swords_KokiriHilt21",      swordsKokiriHilt.changedCvar,       118, gsSPGrayscale(false));
    //     PATCH_GFX(gLinkChildSwordAndSheathFarDL,                  "Swords_KokiriHilt22",      swordsKokiriHilt.changedCvar,       120, gsSPEndDisplayList());
    //     PATCH_GFX(gLinkChildDekuShieldSwordAndSheathNearDL,       "Swords_KokiriHilt23",      swordsKokiriHilt.changedCvar,       166, gsSPGrayscale(true));
    //     PATCH_GFX(gLinkChildDekuShieldSwordAndSheathNearDL,       "Swords_KokiriHilt24",      swordsKokiriHilt.changedCvar,       192, gsSPGrayscale(false));
    //     PATCH_GFX(gLinkChildDekuShieldSwordAndSheathNearDL,       "Swords_KokiriHilt25",      swordsKokiriHilt.changedCvar,       194, gsSPEndDisplayList());
    //     PATCH_GFX(gLinkChildDekuShieldSwordAndSheathFarDL,        "Swords_KokiriHilt26",      swordsKokiriHilt.changedCvar,       156, gsSPGrayscale(true));
    //     PATCH_GFX(gLinkChildDekuShieldSwordAndSheathFarDL,        "Swords_KokiriHilt27",      swordsKokiriHilt.changedCvar,       176, gsSPGrayscale(false));
    //     PATCH_GFX(gLinkChildDekuShieldSwordAndSheathFarDL,        "Swords_KokiriHilt28",      swordsKokiriHilt.changedCvar,       178, gsSPEndDisplayList());
    //     PATCH_GFX(gLinkChildHylianShieldSwordAndSheathNearDL,     "Swords_KokiriHilt29",      swordsKokiriHilt.changedCvar,       162, gsSPGrayscale(true));
    //     PATCH_GFX(gLinkChildHylianShieldSwordAndSheathNearDL,     "Swords_KokiriHilt30",      swordsKokiriHilt.changedCvar,       188, gsSPGrayscale(false));
    //     PATCH_GFX(gLinkChildHylianShieldSwordAndSheathNearDL,     "Swords_KokiriHilt31",      swordsKokiriHilt.changedCvar,       190, gsSPEndDisplayList());
    //     PATCH_GFX(gLinkChildHylianShieldSwordAndSheathFarDL,      "Swords_KokiriHilt32",      swordsKokiriHilt.changedCvar,        98, gsSPGrayscale(true));
    //     PATCH_GFX(gLinkChildHylianShieldSwordAndSheathFarDL,      "Swords_KokiriHilt33",      swordsKokiriHilt.changedCvar,       118, gsSPGrayscale(false));
    //     }
    // }
    static CosmeticOption& swordsMasterBlade = cosmeticOptions.at("Swords.MasterBlade");
    if (manualChange || CVarGetInteger(swordsMasterBlade.rainbowCvar, 0)) {
        static Color_RGBA8 defaultColor = {swordsMasterBlade.defaultColor.x, swordsMasterBlade.defaultColor.y, swordsMasterBlade.defaultColor.z, swordsMasterBlade.defaultColor.w};
        Color_RGBA8 color = CVarGetColor(swordsMasterBlade.cvar, defaultColor);
        PATCH_GFX(gLinkAdultLeftHandHoldingMasterSwordFarDL,      "Swords_MasterBlade1",      swordsMasterBlade.changedCvar,       60, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultLeftHandHoldingMasterSwordNearDL,     "Swords_MasterBlade2",      swordsMasterBlade.changedCvar,       17, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(object_toki_objects_DL_001BD0,                  "Swords_MasterBlade3",      swordsMasterBlade.changedCvar,       13, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(object_toki_objects_DL_001BD0,                  "Swords_MasterBlade4",      swordsMasterBlade.changedCvar,       14, gsDPSetEnvColor(color.r / 2, color.g / 2, color.b / 2, 255));
        PATCH_GFX(gGanonMasterSwordDL,                            "Swords_MasterBlade5",      swordsMasterBlade.changedCvar,       13, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGanonMasterSwordDL,                            "Swords_MasterBlade6",      swordsMasterBlade.changedCvar,       14, gsDPSetEnvColor(color.r / 2, color.g / 2, color.b / 2, 255));
    }
    // static CosmeticOption& swordsMasterHilt = cosmeticOptions.at("Swords.MasterHilt");
    // if (manualChange || CVarGetInteger(swordsMasterHilt.rainbowCvar, 0)) {
    //     static Color_RGBA8 defaultColor = {swordsMasterHilt.defaultColor.x, swordsMasterHilt.defaultColor.y, swordsMasterHilt.defaultColor.z, swordsMasterHilt.defaultColor.w};
    //     Color_RGBA8 color = CVarGetColor(swordsMasterHilt.cvar, defaultColor);
    //     PATCH_GFX(gLinkAdultLeftHandHoldingMasterSwordNearDL,     "Swords_MasterHilt1",       swordsMasterHilt.changedCvar,        20, gsDPSetGrayscaleColor(color.r, color.g, color.b, 255));
    //     PATCH_GFX(gLinkAdultLeftHandHoldingMasterSwordFarDL,      "Swords_MasterHilt2",       swordsMasterHilt.changedCvar,        20, gsDPSetGrayscaleColor(color.r, color.g, color.b, 255));
    //     PATCH_GFX(object_toki_objects_DL_001BD0,                  "Swords_MasterHilt3",       swordsMasterHilt.changedCvar,        16, gsDPSetGrayscaleColor(color.r, color.g, color.b, 255));
    //     PATCH_GFX(gLinkAdultMasterSwordAndSheathNearDL,           "Swords_MasterHilt4",       swordsMasterHilt.changedCvar,         4, gsDPSetGrayscaleColor(color.r, color.g, color.b, 255));
    //     PATCH_GFX(gLinkAdultMasterSwordAndSheathFarDL,            "Swords_MasterHilt5",       swordsMasterHilt.changedCvar,         4, gsDPSetGrayscaleColor(color.r, color.g, color.b, 255));
    //     PATCH_GFX(gLinkAdultMirrorShieldSwordAndSheathNearDL,     "Swords_MasterHilt6",       swordsMasterHilt.changedCvar,         4, gsDPSetGrayscaleColor(color.r, color.g, color.b, 255));
    //     PATCH_GFX(gLinkAdultMirrorShieldSwordAndSheathFarDL,      "Swords_MasterHilt7",       swordsMasterHilt.changedCvar,         4, gsDPSetGrayscaleColor(color.r, color.g, color.b, 255));
    //     PATCH_GFX(gLinkAdultHylianShieldSwordAndSheathNearDL,     "Swords_MasterHilt8",       swordsMasterHilt.changedCvar,         4, gsDPSetGrayscaleColor(color.r, color.g, color.b, 255));
    //     PATCH_GFX(gLinkAdultHylianShieldSwordAndSheathFarDL,      "Swords_MasterHilt9",       swordsMasterHilt.changedCvar,         4, gsDPSetGrayscaleColor(color.r, color.g, color.b, 255));
    //     PATCH_GFX(gGanonMasterSwordDL,                            "Swords_MasterHilt10",      swordsMasterHilt.changedCvar,        16, gsDPSetGrayscaleColor(color.r, color.g, color.b, 255));

    //     if (manualChange) {
    //     PATCH_GFX(gLinkAdultMasterSwordAndSheathFarDL,            "Swords_MasterHilt11",      swordsMasterHilt.changedCvar,        38, gsSPGrayscale(true));
    //     PATCH_GFX(gLinkAdultMasterSwordAndSheathFarDL,            "Swords_MasterHilt12",      swordsMasterHilt.changedCvar,        64, gsSPGrayscale(false));
    //     PATCH_GFX(gLinkAdultMasterSwordAndSheathFarDL,            "Swords_MasterHilt13",      swordsMasterHilt.changedCvar,       106, gsSPGrayscale(true));
    //     PATCH_GFX(gLinkAdultMasterSwordAndSheathFarDL,            "Swords_MasterHilt14",      swordsMasterHilt.changedCvar,       120, gsSPGrayscale(false));
    //     PATCH_GFX(gLinkAdultMasterSwordAndSheathNearDL,           "Swords_MasterHilt15",      swordsMasterHilt.changedCvar,       104, gsSPGrayscale(true));
    //     PATCH_GFX(gLinkAdultMasterSwordAndSheathNearDL,           "Swords_MasterHilt16",      swordsMasterHilt.changedCvar,       182, gsSPGrayscale(false));
    //     PATCH_GFX(gLinkAdultMasterSwordAndSheathNearDL,           "Swords_MasterHilt17",      swordsMasterHilt.changedCvar,       184, gsSPEndDisplayList());
    //     PATCH_GFX(gLinkAdultHylianShieldSwordAndSheathFarDL,      "Swords_MasterHilt18",      swordsMasterHilt.changedCvar,        80, gsSPGrayscale(true));
    //     PATCH_GFX(gLinkAdultHylianShieldSwordAndSheathFarDL,      "Swords_MasterHilt19",      swordsMasterHilt.changedCvar,        94, gsSPGrayscale(false));
    //     PATCH_GFX(gLinkAdultHylianShieldSwordAndSheathFarDL,      "Swords_MasterHilt20",      swordsMasterHilt.changedCvar,       162, gsSPGrayscale(true));
    //     PATCH_GFX(gLinkAdultHylianShieldSwordAndSheathFarDL,      "Swords_MasterHilt21",      swordsMasterHilt.changedCvar,       180, gsSPGrayscale(false));
    //     PATCH_GFX(gLinkAdultHylianShieldSwordAndSheathNearDL,     "Swords_MasterHilt22",      swordsMasterHilt.changedCvar,       154, gsSPGrayscale(true));
    //     PATCH_GFX(gLinkAdultHylianShieldSwordAndSheathNearDL,     "Swords_MasterHilt23",      swordsMasterHilt.changedCvar,       232, gsSPGrayscale(false));
    //     PATCH_GFX(gLinkAdultMirrorShieldSwordAndSheathFarDL,      "Swords_MasterHilt24",      swordsMasterHilt.changedCvar,       112, gsSPGrayscale(true));
    //     PATCH_GFX(gLinkAdultMirrorShieldSwordAndSheathFarDL,      "Swords_MasterHilt25",      swordsMasterHilt.changedCvar,       130, gsSPGrayscale(false));
    //     PATCH_GFX(gLinkAdultMirrorShieldSwordAndSheathFarDL,      "Swords_MasterHilt26",      swordsMasterHilt.changedCvar,       172, gsSPGrayscale(true));
    //     PATCH_GFX(gLinkAdultMirrorShieldSwordAndSheathFarDL,      "Swords_MasterHilt27",      swordsMasterHilt.changedCvar,       186, gsSPGrayscale(false));
    //     PATCH_GFX(gLinkAdultMirrorShieldSwordAndSheathNearDL,     "Swords_MasterHilt28",      swordsMasterHilt.changedCvar,       220, gsSPGrayscale(true));
    //     PATCH_GFX(gLinkAdultMirrorShieldSwordAndSheathNearDL,     "Swords_MasterHilt29",      swordsMasterHilt.changedCvar,       298, gsSPGrayscale(false));
    //     PATCH_GFX(gLinkAdultLeftHandHoldingMasterSwordFarDL,      "Swords_MasterHilt30",      swordsMasterHilt.changedCvar,        38, gsSPGrayscale(true));
    //     PATCH_GFX(gLinkAdultLeftHandHoldingMasterSwordFarDL,      "Swords_MasterHilt31",      swordsMasterHilt.changedCvar,       112, gsSPGrayscale(false));
    //     PATCH_GFX(gLinkAdultLeftHandHoldingMasterSwordNearDL,     "Swords_MasterHilt32",      swordsMasterHilt.changedCvar,        86, gsSPGrayscale(true));
    //     PATCH_GFX(gLinkAdultLeftHandHoldingMasterSwordNearDL,     "Swords_MasterHilt33",      swordsMasterHilt.changedCvar,       208, gsSPGrayscale(false));
    //     PATCH_GFX(object_toki_objects_DL_001BD0,                  "Swords_MasterHilt34",      swordsMasterHilt.changedCvar,       112, gsSPGrayscale(true));
    //     PATCH_GFX(object_toki_objects_DL_001BD0,                  "Swords_MasterHilt35",      swordsMasterHilt.changedCvar,       278, gsSPGrayscale(false));
    //     PATCH_GFX(object_toki_objects_DL_001BD0,                  "Swords_MasterHilt36",      swordsMasterHilt.changedCvar,       280, gsSPEndDisplayList());
    //     PATCH_GFX(gGanonMasterSwordDL,                            "Swords_MasterHilt37",      swordsMasterHilt.changedCvar,       112, gsSPGrayscale(true));
    //     PATCH_GFX(gGanonMasterSwordDL,                            "Swords_MasterHilt38",      swordsMasterHilt.changedCvar,       278, gsSPGrayscale(false));
    //     PATCH_GFX(gGanonMasterSwordDL,                            "Swords_MasterHilt39",      swordsMasterHilt.changedCvar,       280, gsSPEndDisplayList());
    //     }
    // }
    static CosmeticOption& swordsBiggoronBlade = cosmeticOptions.at("Swords.BiggoronBlade");
    if (manualChange || CVarGetInteger(swordsBiggoronBlade.rainbowCvar, 0)) {
        static Color_RGBA8 defaultColor = {swordsBiggoronBlade.defaultColor.x, swordsBiggoronBlade.defaultColor.y, swordsBiggoronBlade.defaultColor.z, swordsBiggoronBlade.defaultColor.w};
        Color_RGBA8 color = CVarGetColor(swordsBiggoronBlade.cvar, defaultColor);
        PATCH_GFX(gLinkAdultLeftHandHoldingBgsFarDL,              "Swords_BiggoronBlade1",    swordsBiggoronBlade.changedCvar,    108, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultLeftHandHoldingBgsNearDL,             "Swords_BiggoronBlade2",    swordsBiggoronBlade.changedCvar,     63, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGiBiggoronSwordDL,                             "Swords_BiggoronBlade3",    swordsBiggoronBlade.changedCvar,      5, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGiBiggoronSwordDL,                             "Swords_BiggoronBlade4",    swordsBiggoronBlade.changedCvar,      6, gsDPSetEnvColor(color.r / 3, color.g / 3, color.b / 3, 255));
    }
    // static CosmeticOption& swordsBiggoronHilt = cosmeticOptions.at("Swords.BiggoronHilt");
    // if (manualChange || CVarGetInteger(swordsBiggoronHilt.rainbowCvar, 0)) {
    //     static Color_RGBA8 defaultColor = {swordsBiggoronHilt.defaultColor.x, swordsBiggoronHilt.defaultColor.y, swordsBiggoronHilt.defaultColor.z, swordsBiggoronHilt.defaultColor.w};
    //     Color_RGBA8 color = CVarGetColor(swordsBiggoronHilt.cvar, defaultColor);
    //     PATCH_GFX(gLinkAdultLeftHandHoldingBgsNearDL,             "Swords_BiggoronHilt1",     swordsBiggoronHilt.changedCvar,      20, gsDPSetGrayscaleColor(color.r, color.g, color.b, 255));
    //     PATCH_GFX(gLinkAdultLeftHandHoldingBgsFarDL,              "Swords_BiggoronHilt2",     swordsBiggoronHilt.changedCvar,      20, gsDPSetGrayscaleColor(color.r, color.g, color.b, 255));
    //     PATCH_GFX(gGiBiggoronSwordDL,                             "Swords_BiggoronHilt3",     swordsBiggoronHilt.changedCvar,      74, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
    //     PATCH_GFX(gGiBiggoronSwordDL,                             "Swords_BiggoronHilt4",     swordsBiggoronHilt.changedCvar,      76, gsDPSetEnvColor(color.r / 3, color.g / 3, color.b / 3, 255));
    //     PATCH_GFX(gGiBiggoronSwordDL,                             "Swords_BiggoronHilt5",     swordsBiggoronHilt.changedCvar,     154, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
    //     PATCH_GFX(gGiBiggoronSwordDL,                             "Swords_BiggoronHilt6",     swordsBiggoronHilt.changedCvar,     156, gsDPSetEnvColor(color.r / 3, color.g / 3, color.b / 3, 255));

    //     if (manualChange) {
    //     PATCH_GFX(gLinkAdultLeftHandHoldingBgsFarDL,              "Swords_BiggoronHilt7",     swordsBiggoronHilt.changedCvar,     278, gsSPGrayscale(true));
    //     PATCH_GFX(gLinkAdultLeftHandHoldingBgsFarDL,              "Swords_BiggoronHilt8",     swordsBiggoronHilt.changedCvar,     332, gsSPGrayscale(false));
    //     PATCH_GFX(gLinkAdultLeftHandHoldingBgsFarDL,              "Swords_BiggoronHilt9",     swordsBiggoronHilt.changedCvar,     334, gsSPEndDisplayList());
    //     PATCH_GFX(gLinkAdultLeftHandHoldingBgsNearDL,             "Swords_BiggoronHilt10",    swordsBiggoronHilt.changedCvar,      38, gsSPGrayscale(true));
    //     PATCH_GFX(gLinkAdultLeftHandHoldingBgsNearDL,             "Swords_BiggoronHilt11",    swordsBiggoronHilt.changedCvar,     118, gsSPGrayscale(false));
    //     }
    // }   

    static CosmeticOption& glovesGoronBracelet = cosmeticOptions.at("Gloves.GoronBracelet");
    if (manualChange || CVarGetInteger(glovesGoronBracelet.rainbowCvar, 0)) {
        static Color_RGBA8 defaultColor = {glovesGoronBracelet.defaultColor.x, glovesGoronBracelet.defaultColor.y, glovesGoronBracelet.defaultColor.z, glovesGoronBracelet.defaultColor.w};
        Color_RGBA8 color = CVarGetColor(glovesGoronBracelet.cvar, defaultColor);
        PATCH_GFX(gGiGoronBraceletDL,                             "Gloves_GoronBracelet1",    glovesGoronBracelet.changedCvar,      5, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGiGoronBraceletDL,                             "Gloves_GoronBracelet2",    glovesGoronBracelet.changedCvar,      6, gsDPSetEnvColor(color.r / 3, color.g / 3, color.b / 3, 255));
        PATCH_GFX(gLinkChildGoronBraceletDL,                      "Gloves_GoronBracelet3",    glovesGoronBracelet.changedCvar,      3, gsDPSetGrayscaleColor(color.r, color.g, color.b, 255));

        if (manualChange) {
        PATCH_GFX(gLinkChildGoronBraceletDL,                      "Gloves_GoronBracelet4",    glovesGoronBracelet.changedCvar,     11, gsSPGrayscale(true));
        PATCH_GFX(gLinkChildGoronBraceletDL,                      "Gloves_GoronBracelet5",    glovesGoronBracelet.changedCvar,     39, gsSPGrayscale(false));
        }
    }
    static CosmeticOption& glovesSilverGauntlets = cosmeticOptions.at("Gloves.SilverGauntlets");
    if (manualChange || CVarGetInteger(glovesSilverGauntlets.rainbowCvar, 0)) {
        static Color_RGBA8 defaultColor = {glovesSilverGauntlets.defaultColor.x, glovesSilverGauntlets.defaultColor.y, glovesSilverGauntlets.defaultColor.z, glovesSilverGauntlets.defaultColor.w};
        Color_RGBA8 color = CVarGetColor(glovesSilverGauntlets.cvar, defaultColor);
        PATCH_GFX(gGiSilverGauntletsColorDL,                      "Gloves_SilverGauntlets1",  glovesSilverGauntlets.changedCvar,    3, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGiSilverGauntletsColorDL,                      "Gloves_SilverGauntlets2",  glovesSilverGauntlets.changedCvar,    4, gsDPSetEnvColor(color.r / 3, color.g / 3, color.b / 3, 255));
    }
    static CosmeticOption& glovesGoldenGauntlets = cosmeticOptions.at("Gloves.GoldenGauntlets");
    if (manualChange || CVarGetInteger(glovesGoldenGauntlets.rainbowCvar, 0)) {
        static Color_RGBA8 defaultColor = {glovesGoldenGauntlets.defaultColor.x, glovesGoldenGauntlets.defaultColor.y, glovesGoldenGauntlets.defaultColor.z, glovesGoldenGauntlets.defaultColor.w};
        Color_RGBA8 color = CVarGetColor(glovesGoldenGauntlets.cvar, defaultColor);
        PATCH_GFX(gGiGoldenGauntletsColorDL,                      "Gloves_GoldenGauntlets1",  glovesGoldenGauntlets.changedCvar,    3, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGiGoldenGauntletsColorDL,                      "Gloves_GoldenGauntlets2",  glovesGoldenGauntlets.changedCvar,    4, gsDPSetEnvColor(color.r / 3, color.g / 3, color.b / 3, 255));
    }
    static CosmeticOption& glovesGauntletsGem = cosmeticOptions.at("Gloves.GauntletsGem");
    if (manualChange || CVarGetInteger(glovesGauntletsGem.rainbowCvar, 0)) {
        static Color_RGBA8 defaultColor = {glovesGauntletsGem.defaultColor.x, glovesGauntletsGem.defaultColor.y, glovesGauntletsGem.defaultColor.z, glovesGauntletsGem.defaultColor.w};
        Color_RGBA8 color = CVarGetColor(glovesGauntletsGem.cvar, defaultColor);
        PATCH_GFX(gGiGauntletsDL,                                 "Gloves_GauntletsGem1",     glovesGauntletsGem.changedCvar,      84, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGiGauntletsDL,                                 "Gloves_GauntletsGem2",     glovesGauntletsGem.changedCvar,      85, gsDPSetEnvColor(color.r / 3, color.g / 3, color.b / 3, 255));
        PATCH_GFX(gLinkAdultLeftGauntletPlate2DL,                 "Gloves_GauntletsGem3",     glovesGauntletsGem.changedCvar,      42, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultRightGauntletPlate2DL,                "Gloves_GauntletsGem4",     glovesGauntletsGem.changedCvar,      42, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultLeftGauntletPlate3DL,                 "Gloves_GauntletsGem5",     glovesGauntletsGem.changedCvar,      42, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultRightGauntletPlate3DL,                "Gloves_GauntletsGem6",     glovesGauntletsGem.changedCvar,      42, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
    }

    static CosmeticOption& equipmentBoomerangBody = cosmeticOptions.at("Equipment.BoomerangBody");
    if (manualChange || CVarGetInteger(equipmentBoomerangBody.rainbowCvar, 0)) {
        static Color_RGBA8 defaultColor = {equipmentBoomerangBody.defaultColor.x, equipmentBoomerangBody.defaultColor.y, equipmentBoomerangBody.defaultColor.z, equipmentBoomerangBody.defaultColor.w};
        Color_RGBA8 color = CVarGetColor(equipmentBoomerangBody.cvar, defaultColor);
        PATCH_GFX(gGiBoomerangDL,                                 "Equipment_BoomerangBody1", equipmentBoomerangBody.changedCvar,   5, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGiBoomerangDL,                                 "Equipment_BoomerangBody2", equipmentBoomerangBody.changedCvar,   6, gsDPSetEnvColor(color.r / 3, color.g / 3, color.b / 3, 255));
        PATCH_GFX(gLinkChildLeftFistAndBoomerangNearDL,           "Equipment_BoomerangBody3", equipmentBoomerangBody.changedCvar,  34, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkChildLeftFistAndBoomerangFarDL,            "Equipment_BoomerangBody4", equipmentBoomerangBody.changedCvar,   9, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gBoomerangDL,                                   "Equipment_BoomerangBody5", equipmentBoomerangBody.changedCvar,  39, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
    }
    static CosmeticOption& equipmentBoomerangGem = cosmeticOptions.at("Equipment.BoomerangGem");
    if (manualChange || CVarGetInteger(equipmentBoomerangGem.rainbowCvar, 0)) {
        static Color_RGBA8 defaultColor = {equipmentBoomerangGem.defaultColor.x, equipmentBoomerangGem.defaultColor.y, equipmentBoomerangGem.defaultColor.z, equipmentBoomerangGem.defaultColor.w};
        Color_RGBA8 color = CVarGetColor(equipmentBoomerangGem.cvar, defaultColor);
        PATCH_GFX(gGiBoomerangDL,                                 "Equipment_BoomerangGem1",  equipmentBoomerangGem.changedCvar,   84, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGiBoomerangDL,                                 "Equipment_BoomerangGem2",  equipmentBoomerangGem.changedCvar,   85, gsDPSetEnvColor(color.r / 3, color.g / 3, color.b / 3, 255));
        PATCH_GFX(gLinkChildLeftFistAndBoomerangNearDL,           "Equipment_BoomerangGem3",  equipmentBoomerangGem.changedCvar,   16, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gBoomerangDL,                                   "Equipment_BoomerangGem4",  equipmentBoomerangGem.changedCvar,   23, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        // There appears to be no gem rendered on the far LOD variant, not sure if this is an SOH bug or what.
        // PATCH_GFX(gLinkChildLeftFistAndBoomerangFarDL,  "Equipment_BoomerangGem5",  equipmentBoomerangGem.changedCvar,  32, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
    }

    // static CosmeticOption& equipmentSlingshotBody = cosmeticOptions.at("Equipment.SlingshotBody");
    // if (manualChange || CVarGetInteger(equipmentSlingshotBody.rainbowCvar, 0)) {
    //     static Color_RGBA8 defaultColor = {equipmentSlingshotBody.defaultColor.x, equipmentSlingshotBody.defaultColor.y, equipmentSlingshotBody.defaultColor.z, equipmentSlingshotBody.defaultColor.w};
    //     Color_RGBA8 color = CVarGetColor(equipmentSlingshotBody.cvar, defaultColor);
    //     PATCH_GFX(gGiSlingshotDL,                                 "Equipment_SlingshotBody1", equipmentSlingshotBody.changedCvar,  10, gsDPSetPrimColor(0, 0, MAX(color.r - 100, 0), MAX(color.g - 100, 0), MAX(color.b - 100, 0), 255));
    //     PATCH_GFX(gGiSlingshotDL,                                 "Equipment_SlingshotBody2", equipmentSlingshotBody.changedCvar,  12, gsDPSetEnvColor(MAX(color.r - 100, 0) / 3, MAX(color.g - 100, 0) / 3, MAX(color.b - 100, 0) / 3, 255));
    //     PATCH_GFX(gGiSlingshotDL,                                 "Equipment_SlingshotBody3", equipmentSlingshotBody.changedCvar,  74, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
    //     PATCH_GFX(gGiSlingshotDL,                                 "Equipment_SlingshotBody4", equipmentSlingshotBody.changedCvar,  76, gsDPSetEnvColor(color.r / 3, color.g / 3, color.b / 3, 255));
    //     PATCH_GFX(gGiSlingshotDL,                                 "Equipment_SlingshotBody5", equipmentSlingshotBody.changedCvar, 128, gsDPSetPrimColor(0, 0, MAX(color.r - 100, 0), MAX(color.g - 100, 0), MAX(color.b - 100, 0), 255));
    //     PATCH_GFX(gGiSlingshotDL,                                 "Equipment_SlingshotBody6", equipmentSlingshotBody.changedCvar, 130, gsDPSetEnvColor(MAX(color.r - 100, 0) / 3, MAX(color.g - 100, 0) / 3, MAX(color.b - 100, 0) / 3, 255));
    //     PATCH_GFX(gLinkChildRightArmStretchedSlingshotDL,         "Equipment_SlingshotBody7", equipmentSlingshotBody.changedCvar,   4, gsDPSetGrayscaleColor(color.r, color.g, color.b, 255));
    //     PATCH_GFX(gLinkChildRightHandHoldingSlingshotNearDL,      "Equipment_SlingshotBody8", equipmentSlingshotBody.changedCvar,   4, gsDPSetGrayscaleColor(color.r, color.g, color.b, 255));
    //     PATCH_GFX(gLinkChildRightHandHoldingSlingshotFarDL,       "Equipment_SlingshotBody9", equipmentSlingshotBody.changedCvar,   4, gsDPSetGrayscaleColor(color.r, color.g, color.b, 255));

    //     if (manualChange) {
    //     PATCH_GFX(gLinkChildRightArmStretchedSlingshotDL,         "Equipment_SlingshotBody10",equipmentSlingshotBody.changedCvar,  20, gsSPGrayscale(true));
    //     PATCH_GFX(gLinkChildRightArmStretchedSlingshotDL,         "Equipment_SlingshotBody11",equipmentSlingshotBody.changedCvar,  74, gsSPGrayscale(false));
    //     PATCH_GFX(gLinkChildRightHandHoldingSlingshotFarDL,       "Equipment_SlingshotBody12",equipmentSlingshotBody.changedCvar,  20, gsSPGrayscale(true));
    //     PATCH_GFX(gLinkChildRightHandHoldingSlingshotFarDL,       "Equipment_SlingshotBody13",equipmentSlingshotBody.changedCvar,  66, gsSPGrayscale(false));
    //     PATCH_GFX(gLinkChildRightHandHoldingSlingshotNearDL,      "Equipment_SlingshotBody14",equipmentSlingshotBody.changedCvar,  96, gsSPGrayscale(true));
    //     PATCH_GFX(gLinkChildRightHandHoldingSlingshotNearDL,      "Equipment_SlingshotBody15",equipmentSlingshotBody.changedCvar, 136, gsSPGrayscale(false));
    //     PATCH_GFX(gLinkChildRightHandHoldingSlingshotNearDL,      "Equipment_SlingshotBody16",equipmentSlingshotBody.changedCvar, 138, gsSPEndDisplayList());
    //     }
    // }
    static CosmeticOption& equipmentSlingshotString = cosmeticOptions.at("Equipment.SlingshotString");
    if (manualChange || CVarGetInteger(equipmentSlingshotString.rainbowCvar, 0)) {
        static Color_RGBA8 defaultColor = {equipmentSlingshotString.defaultColor.x, equipmentSlingshotString.defaultColor.y, equipmentSlingshotString.defaultColor.z, equipmentSlingshotString.defaultColor.w};
        Color_RGBA8 color = CVarGetColor(equipmentSlingshotString.cvar, defaultColor);
        PATCH_GFX(gGiSlingshotDL,                                 "Equipment_SlingshotString1",equipmentSlingshotString.changedCvar, 75, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGiSlingshotDL,                                 "Equipment_SlingshotString2",equipmentSlingshotString.changedCvar, 76, gsDPSetEnvColor(color.r / 2, color.g / 2, color.b / 2, 255));
        PATCH_GFX(gLinkChildSlinghotStringDL,                     "Equipment_SlingshotString3",equipmentSlingshotString.changedCvar,  9, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
    }

    static CosmeticOption& equipmentBowTips = cosmeticOptions.at("Equipment.BowTips");
    if (manualChange || CVarGetInteger(equipmentBowTips.rainbowCvar, 0)) {
        static Color_RGBA8 defaultColor = {equipmentBowTips.defaultColor.x, equipmentBowTips.defaultColor.y, equipmentBowTips.defaultColor.z, equipmentBowTips.defaultColor.w};
        Color_RGBA8 color = CVarGetColor(equipmentBowTips.cvar, defaultColor);
        PATCH_GFX(gGiBowDL,                                       "Equipment_BowTips1",       equipmentBowTips.changedCvar,        86, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGiBowDL,                                       "Equipment_BowTips2",       equipmentBowTips.changedCvar,        87, gsDPSetEnvColor(color.r / 3, color.g / 3, color.b / 3, 255));
        PATCH_GFX(gLinkAdultRightHandHoldingBowFirstPersonDL,     "Equipment_BowTips3",       equipmentBowTips.changedCvar,        34, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultRightHandHoldingBowNearDL,            "Equipment_BowTips4",       equipmentBowTips.changedCvar,        26, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultRightHandHoldingBowFarDL,             "Equipment_BowTips5",       equipmentBowTips.changedCvar,        25, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
    }
    static CosmeticOption& equipmentBowString = cosmeticOptions.at("Equipment.BowString");
    if (manualChange || CVarGetInteger(equipmentBowString.rainbowCvar, 0)) {
        static Color_RGBA8 defaultColor = {equipmentBowString.defaultColor.x, equipmentBowString.defaultColor.y, equipmentBowString.defaultColor.z, equipmentBowString.defaultColor.w};
        Color_RGBA8 color = CVarGetColor(equipmentBowString.cvar, defaultColor);
        PATCH_GFX(gGiBowDL,                                       "Equipment_BowString1",     equipmentBowString.changedCvar,     105, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGiBowDL,                                       "Equipment_BowString2",     equipmentBowString.changedCvar,     106, gsDPSetEnvColor(color.r / 3, color.g / 3, color.b / 3, 255));
        PATCH_GFX(gLinkAdultBowStringDL,                          "Equipment_BowString3",     equipmentBowString.changedCvar,       9, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
    }
    static CosmeticOption& equipmentBowBody = cosmeticOptions.at("Equipment.BowBody");
    if (manualChange || CVarGetInteger(equipmentBowBody.rainbowCvar, 0)) {
        static Color_RGBA8 defaultColor = {equipmentBowBody.defaultColor.x, equipmentBowBody.defaultColor.y, equipmentBowBody.defaultColor.z, equipmentBowBody.defaultColor.w};
        Color_RGBA8 color = CVarGetColor(equipmentBowBody.cvar, defaultColor);
        PATCH_GFX(gGiBowDL,                                       "Equipment_BowBody1",       equipmentBowBody.changedCvar,         5, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGiBowDL,                                       "Equipment_BowBody2",       equipmentBowBody.changedCvar,         6, gsDPSetEnvColor(color.r / 3, color.g / 3, color.b / 3, 255));
        PATCH_GFX(gLinkAdultRightHandHoldingBowFirstPersonDL,     "Equipment_BowBody3",       equipmentBowBody.changedCvar,        42, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultRightHandHoldingBowNearDL,            "Equipment_BowBody4",       equipmentBowBody.changedCvar,        33, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultRightHandHoldingBowFarDL,             "Equipment_BowBody5",       equipmentBowBody.changedCvar,        31, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
    }
    static CosmeticOption& equipmentBowHandle = cosmeticOptions.at("Equipment.BowHandle");
    if (manualChange || CVarGetInteger(equipmentBowHandle.rainbowCvar, 0)) {
        static Color_RGBA8 defaultColor = {equipmentBowHandle.defaultColor.x, equipmentBowHandle.defaultColor.y, equipmentBowHandle.defaultColor.z, equipmentBowHandle.defaultColor.w};
        Color_RGBA8 color = CVarGetColor(equipmentBowHandle.cvar, defaultColor);
        PATCH_GFX(gGiBowDL,                                       "Equipment_BowHandle1",     equipmentBowHandle.changedCvar,      51, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGiBowDL,                                       "Equipment_BowHandle2",     equipmentBowHandle.changedCvar,      52, gsDPSetEnvColor(color.r / 3, color.g / 3, color.b / 3, 255));
        PATCH_GFX(gLinkAdultRightHandHoldingBowFirstPersonDL,     "Equipment_BowHandle3",     equipmentBowHandle.changedCvar,      18, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultRightHandHoldingBowNearDL,            "Equipment_BowHandle4",     equipmentBowHandle.changedCvar,      18, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultRightHandHoldingBowFarDL,             "Equipment_BowHandle5",     equipmentBowHandle.changedCvar,      18, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
    }

    static CosmeticOption& equipmentHammerHead = cosmeticOptions.at("Equipment.HammerHead");
    if (manualChange || CVarGetInteger(equipmentHammerHead.rainbowCvar, 0)) {
        static Color_RGBA8 defaultColor = {equipmentHammerHead.defaultColor.x, equipmentHammerHead.defaultColor.y, equipmentHammerHead.defaultColor.z, equipmentHammerHead.defaultColor.w};
        Color_RGBA8 color = CVarGetColor(equipmentHammerHead.cvar, defaultColor);
        PATCH_GFX(gGiHammerDL,                                    "Equipment_HammerHead1",    equipmentHammerHead.changedCvar,      5, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGiHammerDL,                                    "Equipment_HammerHead2",    equipmentHammerHead.changedCvar,      6, gsDPSetEnvColor(color.r / 5, color.g / 5, color.b / 5, 255));
        PATCH_GFX(gGiHammerDL,                                    "Equipment_HammerHead3",    equipmentHammerHead.changedCvar,     68, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGiHammerDL,                                    "Equipment_HammerHead4",    equipmentHammerHead.changedCvar,     69, gsDPSetEnvColor(color.r / 5, color.g / 5, color.b / 5, 255));
        PATCH_GFX(gLinkAdultLeftHandHoldingHammerNearDL,          "Equipment_HammerHead5",    equipmentHammerHead.changedCvar,     38, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultLeftHandHoldingHammerFarDL,           "Equipment_HammerHead6",    equipmentHammerHead.changedCvar,     38, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
    }
    static CosmeticOption& equipmentHammerHandle = cosmeticOptions.at("Equipment.HammerHandle");
    if (manualChange || CVarGetInteger(equipmentHammerHandle.rainbowCvar, 0)) {
        static Color_RGBA8 defaultColor = {equipmentHammerHandle.defaultColor.x, equipmentHammerHandle.defaultColor.y, equipmentHammerHandle.defaultColor.z, equipmentHammerHandle.defaultColor.w};
        Color_RGBA8 color = CVarGetColor(equipmentHammerHandle.cvar, defaultColor);
        PATCH_GFX(gGiHammerDL,                                    "Equipment_HammerHandle1",  equipmentHammerHandle.changedCvar,   84, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGiHammerDL,                                    "Equipment_HammerHandle2",  equipmentHammerHandle.changedCvar,   85, gsDPSetEnvColor(color.r / 2, color.g / 2, color.b / 2, 255));
        PATCH_GFX(gLinkAdultLeftHandHoldingHammerNearDL,          "Equipment_HammerHandle5",  equipmentHammerHandle.changedCvar,   18, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gLinkAdultLeftHandHoldingHammerFarDL,           "Equipment_HammerHandle6",  equipmentHammerHandle.changedCvar,   18, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
    }

    static CosmeticOption& equipmentChuFace = cosmeticOptions.at("Equipment.ChuFace");
    if (manualChange || CVarGetInteger(equipmentChuFace.rainbowCvar, 0)) {
        static Color_RGBA8 defaultColor = {equipmentChuFace.defaultColor.x, equipmentChuFace.defaultColor.y, equipmentChuFace.defaultColor.z, equipmentChuFace.defaultColor.w};
        Color_RGBA8 color = CVarGetColor(equipmentChuFace.cvar, defaultColor);
        PATCH_GFX(gGiBombchuDL,                                   "Equipment_ChuFace1",       equipmentChuFace.changedCvar,         5, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGiBombchuDL,                                   "Equipment_ChuFace2",       equipmentChuFace.changedCvar,         6, gsDPSetEnvColor(color.r / 3, color.g / 3, color.b / 3, 255));
        PATCH_GFX(gBombchuDL,                                     "Equipment_ChuFace3",       equipmentChuFace.changedCvar,         2, gsDPSetGrayscaleColor(color.r, color.g, color.b, 255));

        if (manualChange) {
        PATCH_GFX(gBombchuDL,                                     "Equipment_ChuFace4",       equipmentChuFace.changedCvar,        10, gsSPGrayscale(true));
        PATCH_GFX(gBombchuDL,                                     "Equipment_ChuFace5",       equipmentChuFace.changedCvar,        27, gsSPGrayscale(false));
        }
    }
    static CosmeticOption& equipmentChuBody = cosmeticOptions.at("Equipment.ChuBody");
    if (manualChange || CVarGetInteger(equipmentChuBody.rainbowCvar, 0)) {
        static Color_RGBA8 defaultColor = {equipmentChuBody.defaultColor.x, equipmentChuBody.defaultColor.y, equipmentChuBody.defaultColor.z, equipmentChuBody.defaultColor.w};
        Color_RGBA8 color = CVarGetColor(equipmentChuBody.cvar, defaultColor);
        PATCH_GFX(gGiBombchuDL,                                   "Equipment_ChuBody1",       equipmentChuBody.changedCvar,        39, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGiBombchuDL,                                   "Equipment_ChuBody2",       equipmentChuBody.changedCvar,        40, gsDPSetEnvColor(color.r / 3, color.g / 3, color.b / 3, 255));
        PATCH_GFX(gGiBombchuDL,                                   "Equipment_ChuBody3",       equipmentChuBody.changedCvar,        60, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGiBombchuDL,                                   "Equipment_ChuBody4",       equipmentChuBody.changedCvar,        61, gsDPSetEnvColor(color.r / 3, color.g / 3, color.b / 3, 255));
        PATCH_GFX(gBombchuDL,                                     "Equipment_ChuBody5",       equipmentChuBody.changedCvar,        46, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
    }

    static CosmeticOption& equipmentBunnyHood = cosmeticOptions.at("Equipment.BunnyHood");
    if (manualChange || CVarGetInteger(equipmentBunnyHood.rainbowCvar, 0)) {
        static Color_RGBA8 defaultColor = {equipmentBunnyHood.defaultColor.x, equipmentBunnyHood.defaultColor.y, equipmentBunnyHood.defaultColor.z, equipmentBunnyHood.defaultColor.w};
        Color_RGBA8 color = CVarGetColor(equipmentBunnyHood.cvar, defaultColor);
        PATCH_GFX(gGiBunnyHoodDL,                                   "Equipment_BunnyHood1",       equipmentBunnyHood.changedCvar,        5, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGiBunnyHoodDL,                                   "Equipment_BunnyHood2",       equipmentBunnyHood.changedCvar,        6, gsDPSetEnvColor(color.r / 3, color.g / 3, color.b / 3, 255));
        PATCH_GFX(gGiBunnyHoodDL,                                   "Equipment_BunnyHood3",       equipmentBunnyHood.changedCvar,        83, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGiBunnyHoodDL,                                   "Equipment_BunnyHood4",       equipmentBunnyHood.changedCvar,        84, gsDPSetEnvColor(color.r / 3, color.g / 3, color.b / 3, 255));
        PATCH_GFX(gLinkChildBunnyHoodDL,                            "Equipment_BunnyHood5",       equipmentBunnyHood.changedCvar,         4, gsDPSetGrayscaleColor(color.r, color.g, color.b, 255));

        if (manualChange) {
        PATCH_GFX(gLinkChildBunnyHoodDL,                            "Equipment_BunnyHood6",       equipmentBunnyHood.changedCvar,        13, gsSPGrayscale(true));
            if (CVarGetInteger(equipmentBunnyHood.changedCvar, 0)) {
                ResourceMgr_PatchGfxByName(gLinkChildBunnyHoodDL,   "Equipment_BunnyHood7",                                             125, gsSPBranchListOTRFilePath(gEndGrayscaleAndEndDlistDL));
            } else {
                ResourceMgr_UnpatchGfxByName(gLinkChildBunnyHoodDL, "Equipment_BunnyHood7");
            }
        }
    }

    static CosmeticOption& consumableGreenRupee = cosmeticOptions.at("Consumable.GreenRupee");
    if (manualChange || CVarGetInteger(consumableGreenRupee.rainbowCvar, 0)) {
        static Color_RGBA8 defaultColor = {consumableGreenRupee.defaultColor.x, consumableGreenRupee.defaultColor.y, consumableGreenRupee.defaultColor.z, consumableGreenRupee.defaultColor.w};
        Color_RGBA8 color = CVarGetColor(consumableGreenRupee.cvar, defaultColor);
        PATCH_GFX(gGiGreenRupeeInnerColorDL,                      "Consumable_GreenRupee1",   consumableGreenRupee.changedCvar,     3, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGiGreenRupeeInnerColorDL,                      "Consumable_GreenRupee2",   consumableGreenRupee.changedCvar,     4, gsDPSetEnvColor(color.r / 5, color.g / 5, color.b / 5, 255));
        PATCH_GFX(gGiGreenRupeeOuterColorDL,                      "Consumable_GreenRupee3",   consumableGreenRupee.changedCvar,     3, gsDPSetPrimColor(0, 0, MIN(color.r + 100, 255), MIN(color.g + 100, 255), MIN(color.b + 100, 255), 255));
        PATCH_GFX(gGiGreenRupeeOuterColorDL,                      "Consumable_GreenRupee4",   consumableGreenRupee.changedCvar,     4, gsDPSetEnvColor(color.r * 0.75f, color.g * 0.75f, color.b * 0.75f, 255));
    
        // Greg Bridge
        if (Randomizer_GetSettingValue(RSK_RAINBOW_BRIDGE) == RO_BRIDGE_GREG) {
            ResourceMgr_PatchGfxByName(gRainbowBridgeDL, "RainbowBridge_StartGrayscale", 2, gsSPGrayscale(true));
            ResourceMgr_PatchGfxByName(gRainbowBridgeDL, "RainbowBridge_MakeGreen", 10, gsDPSetGrayscaleColor(color.r, color.g, color.b, color.a));
            ResourceMgr_PatchGfxByName(gRainbowBridgeDL, "RainbowBridge_EndGrayscaleAndEndDlist", 79, gsSPBranchListOTRFilePath(gEndGrayscaleAndEndDlistDL));
        } else {
            ResourceMgr_UnpatchGfxByName(gRainbowBridgeDL, "RainbowBridge_StartGrayscale");
            ResourceMgr_UnpatchGfxByName(gRainbowBridgeDL, "RainbowBridge_MakeGreen");
            ResourceMgr_UnpatchGfxByName(gRainbowBridgeDL, "RainbowBridge_EndGrayscaleAndEndDlist");
        }
    }
    static CosmeticOption& consumableBlueRupee = cosmeticOptions.at("Consumable.BlueRupee");
    if (manualChange || CVarGetInteger(consumableBlueRupee.rainbowCvar, 0)) {
        static Color_RGBA8 defaultColor = {consumableBlueRupee.defaultColor.x, consumableBlueRupee.defaultColor.y, consumableBlueRupee.defaultColor.z, consumableBlueRupee.defaultColor.w};
        Color_RGBA8 color = CVarGetColor(consumableBlueRupee.cvar, defaultColor);
        PATCH_GFX(gGiBlueRupeeInnerColorDL,                       "Consumable_BlueRupee1",    consumableBlueRupee.changedCvar,      3, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGiBlueRupeeInnerColorDL,                       "Consumable_BlueRupee2",    consumableBlueRupee.changedCvar,      4, gsDPSetEnvColor(color.r / 5, color.g / 5, color.b / 5, 255));
        PATCH_GFX(gGiBlueRupeeOuterColorDL,                       "Consumable_BlueRupee3",    consumableBlueRupee.changedCvar,      3, gsDPSetPrimColor(0, 0, MIN(color.r + 100, 255), MIN(color.g + 100, 255), MIN(color.b + 100, 255), 255));
        PATCH_GFX(gGiBlueRupeeOuterColorDL,                       "Consumable_BlueRupee4",    consumableBlueRupee.changedCvar,      4, gsDPSetEnvColor(color.r * 0.75f, color.g * 0.75f, color.b * 0.75f, 255));
    }
    static CosmeticOption& consumableRedRupee = cosmeticOptions.at("Consumable.RedRupee");
    if (manualChange || CVarGetInteger(consumableRedRupee.rainbowCvar, 0)) {
        static Color_RGBA8 defaultColor = {consumableRedRupee.defaultColor.x, consumableRedRupee.defaultColor.y, consumableRedRupee.defaultColor.z, consumableRedRupee.defaultColor.w};
        Color_RGBA8 color = CVarGetColor(consumableRedRupee.cvar, defaultColor);
        PATCH_GFX(gGiRedRupeeInnerColorDL,                        "Consumable_RedRupee1",     consumableRedRupee.changedCvar,       3, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGiRedRupeeInnerColorDL,                        "Consumable_RedRupee2",     consumableRedRupee.changedCvar,       4, gsDPSetEnvColor(color.r / 5, color.g / 5, color.b / 5, 255));
        PATCH_GFX(gGiRedRupeeOuterColorDL,                        "Consumable_RedRupee3",     consumableRedRupee.changedCvar,       3, gsDPSetPrimColor(0, 0, MIN(color.r + 100, 255), MIN(color.g + 100, 255), MIN(color.b + 100, 255), 255));
        PATCH_GFX(gGiRedRupeeOuterColorDL,                        "Consumable_RedRupee4",     consumableRedRupee.changedCvar,       4, gsDPSetEnvColor(color.r * 0.75f, color.g * 0.75f, color.b * 0.75f, 255));
    }
    static CosmeticOption& consumablePurpleRupee = cosmeticOptions.at("Consumable.PurpleRupee");
    if (manualChange || CVarGetInteger(consumablePurpleRupee.rainbowCvar, 0)) {
        static Color_RGBA8 defaultColor = {consumablePurpleRupee.defaultColor.x, consumablePurpleRupee.defaultColor.y, consumablePurpleRupee.defaultColor.z, consumablePurpleRupee.defaultColor.w};
        Color_RGBA8 color = CVarGetColor(consumablePurpleRupee.cvar, defaultColor);
        PATCH_GFX(gGiPurpleRupeeInnerColorDL,                     "Consumable_PurpleRupee1",  consumablePurpleRupee.changedCvar,    3, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGiPurpleRupeeInnerColorDL,                     "Consumable_PurpleRupee2",  consumablePurpleRupee.changedCvar,    4, gsDPSetEnvColor(color.r / 5, color.g / 5, color.b / 5, 255));
        PATCH_GFX(gGiPurpleRupeeOuterColorDL,                     "Consumable_PurpleRupee3",  consumablePurpleRupee.changedCvar,    3, gsDPSetPrimColor(0, 0, MIN(color.r + 100, 255), MIN(color.g + 100, 255), MIN(color.b + 100, 255), 255));
        PATCH_GFX(gGiPurpleRupeeOuterColorDL,                     "Consumable_PurpleRupee4",  consumablePurpleRupee.changedCvar,    4, gsDPSetEnvColor(color.r * 0.75f, color.g * 0.75f, color.b * 0.75f, 255));
    }
    static CosmeticOption& consumableGoldRupee = cosmeticOptions.at("Consumable.GoldRupee");
    if (manualChange || CVarGetInteger(consumableGoldRupee.rainbowCvar, 0)) {
        static Color_RGBA8 defaultColor = {consumableGoldRupee.defaultColor.x, consumableGoldRupee.defaultColor.y, consumableGoldRupee.defaultColor.z, consumableGoldRupee.defaultColor.w};
        Color_RGBA8 color = CVarGetColor(consumableGoldRupee.cvar, defaultColor);
        PATCH_GFX(gGiGoldRupeeInnerColorDL,                       "Consumable_GoldRupee1",    consumableGoldRupee.changedCvar,      3, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGiGoldRupeeInnerColorDL,                       "Consumable_GoldRupee2",    consumableGoldRupee.changedCvar,      4, gsDPSetEnvColor(color.r / 5, color.g / 5, color.b / 5, 255));
        PATCH_GFX(gGiGoldRupeeOuterColorDL,                       "Consumable_GoldRupee3",    consumableGoldRupee.changedCvar,      3, gsDPSetPrimColor(0, 0, MIN(color.r + 100, 255), MIN(color.g + 100, 255), MIN(color.b + 100, 255), 255));
        PATCH_GFX(gGiGoldRupeeOuterColorDL,                       "Consumable_GoldRupee4",    consumableGoldRupee.changedCvar,      4, gsDPSetEnvColor(color.r * 0.75f, color.g * 0.75f, color.b * 0.75f, 255));
    }

    static CosmeticOption& consumableHearts = cosmeticOptions.at("Consumable.Hearts");
    if (manualChange || CVarGetInteger(consumableHearts.rainbowCvar, 0)) {
        static Color_RGBA8 defaultColor = {consumableHearts.defaultColor.x, consumableHearts.defaultColor.y, consumableHearts.defaultColor.z, consumableHearts.defaultColor.w};
        Color_RGBA8 color = CVarGetColor(consumableHearts.cvar, defaultColor);
        // PATCH_GFX(gGiRecoveryHeartDL,                             "Consumable_Hearts1",       consumableHearts.changedCvar,         4, gsDPSetGrayscaleColor(color.r, color.g, color.b, 255));
        // PATCH_GFX(gGiRecoveryHeartDL,                             "Consumable_Hearts2",       consumableHearts.changedCvar,        26, gsSPGrayscale(true));
        // PATCH_GFX(gGiRecoveryHeartDL,                             "Consumable_Hearts3",       consumableHearts.changedCvar,        72, gsSPGrayscale(false));
        // PATCH_GFX(gGiRecoveryHeartDL,                             "Consumable_Hearts4",       consumableHearts.changedCvar,        74, gsSPEndDisplayList());
        PATCH_GFX(gGiHeartPieceDL,                                "Consumable_Hearts5",       consumableHearts.changedCvar,         2, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGiHeartPieceDL,                                "Consumable_Hearts6",       consumableHearts.changedCvar,         6, gsDPSetEnvColor(color.r / 2, color.g / 2, color.b / 2, 255));
        PATCH_GFX(gGiHeartContainerDL,                            "Consumable_Hearts7",       consumableHearts.changedCvar,         2, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGiHeartContainerDL,                            "Consumable_Hearts8",       consumableHearts.changedCvar,         6, gsDPSetEnvColor(color.r / 2, color.g / 2, color.b / 2, 255));
        PATCH_GFX(gGiRedPotColorDL,                               "Consumable_Hearts9",       consumableHearts.changedCvar,         3, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGiRedPotColorDL,                               "Consumable_Hearts10",      consumableHearts.changedCvar,         4, gsDPSetEnvColor(color.r / 2, color.g / 2, color.b / 2, 255));
    }
    static CosmeticOption& consumableMagic = cosmeticOptions.at("Consumable.Magic");
    if (manualChange || CVarGetInteger(consumableMagic.rainbowCvar, 0)) {
        static Color_RGBA8 defaultColor = {consumableMagic.defaultColor.x, consumableMagic.defaultColor.y, consumableMagic.defaultColor.z, consumableMagic.defaultColor.w};
        Color_RGBA8 color = CVarGetColor(consumableMagic.cvar, defaultColor);
        PATCH_GFX(gGiMagicJarSmallDL,                             "Consumable_Magic1",        consumableMagic.changedCvar,         31, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGiMagicJarSmallDL,                             "Consumable_Magic2",        consumableMagic.changedCvar,         32, gsDPSetEnvColor(color.r / 2, color.g / 2, color.b / 2, 255));
        PATCH_GFX(gGiMagicJarLargeDL,                             "Consumable_Magic3",        consumableMagic.changedCvar,         31, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGiMagicJarLargeDL,                             "Consumable_Magic4",        consumableMagic.changedCvar,         32, gsDPSetEnvColor(color.r / 2, color.g / 2, color.b / 2, 255));
        PATCH_GFX(gGiGreenPotColorDL,                             "Consumable_Magic5",        consumableMagic.changedCvar,          3, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGiGreenPotColorDL,                             "Consumable_Magic6",        consumableMagic.changedCvar,          4, gsDPSetEnvColor(color.r / 2, color.g / 2, color.b / 2, 255));
    }

    static CosmeticOption& npcGoldenSkulltula = cosmeticOptions.at("NPC.GoldenSkulltula");
    if (manualChange || CVarGetInteger(npcGoldenSkulltula.rainbowCvar, 0)) {
        static Color_RGBA8 defaultColor = {npcGoldenSkulltula.defaultColor.x, npcGoldenSkulltula.defaultColor.y, npcGoldenSkulltula.defaultColor.z, npcGoldenSkulltula.defaultColor.w};
        Color_RGBA8 color = CVarGetColor(npcGoldenSkulltula.cvar, defaultColor);
        PATCH_GFX(gSkulltulaTokenDL,                              "NPC_GoldenSkulltula1",     npcGoldenSkulltula.changedCvar,       5, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gSkulltulaTokenDL,                              "NPC_GoldenSkulltula2",     npcGoldenSkulltula.changedCvar,       6, gsDPSetEnvColor(color.r / 2, color.g / 2, color.b / 2, 255));
        PATCH_GFX(gSkulltulaTokenFlameDL,                         "NPC_GoldenSkulltula3",     npcGoldenSkulltula.changedCvar,      32, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gSkulltulaTokenFlameDL,                         "NPC_GoldenSkulltula4",     npcGoldenSkulltula.changedCvar,      33, gsDPSetEnvColor(color.r / 2, color.g / 2, color.b / 2, 255));
        PATCH_GFX(gGiSkulltulaTokenDL,                            "NPC_GoldenSkulltula5",     npcGoldenSkulltula.changedCvar,       5, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGiSkulltulaTokenDL,                            "NPC_GoldenSkulltula6",     npcGoldenSkulltula.changedCvar,       6, gsDPSetEnvColor(color.r / 2, color.g / 2, color.b / 2, 255));
        PATCH_GFX(gGiSkulltulaTokenFlameDL,                       "NPC_GoldenSkulltula7",     npcGoldenSkulltula.changedCvar,      32, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGiSkulltulaTokenFlameDL,                       "NPC_GoldenSkulltula8",     npcGoldenSkulltula.changedCvar,      33, gsDPSetEnvColor(color.r / 2, color.g / 2, color.b / 2, 255));
        PATCH_GFX(object_st_DL_003FB0,                            "NPC_GoldenSkulltula9",     npcGoldenSkulltula.changedCvar,     118, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(object_st_DL_003FB0,                            "NPC_GoldenSkulltula10",    npcGoldenSkulltula.changedCvar,     119, gsDPSetEnvColor(color.r / 4, color.g / 4, color.b / 4, 255));
    }

    static CosmeticOption& npcGerudo = cosmeticOptions.at("NPC.Gerudo");
    if (manualChange || CVarGetInteger(npcGerudo.rainbowCvar, 0)) {
        static Color_RGBA8 defaultColor = {npcGerudo.defaultColor.x, npcGerudo.defaultColor.y, npcGerudo.defaultColor.z, npcGerudo.defaultColor.w};
        Color_RGBA8 color = CVarGetColor(npcGerudo.cvar, defaultColor);
        PATCH_GFX(gGerudoPurpleTorsoDL,                           "NPC_Gerudo1",              npcGerudo.changedCvar,              139, gsDPSetEnvColor( color.r, color.g, color.b, 255));
        PATCH_GFX(gGerudoPurpleRightThighDL,                      "NPC_Gerudo2",              npcGerudo.changedCvar,               11, gsDPSetEnvColor(color.r, color.g, color.b, 255));
        PATCH_GFX(gGerudoPurpleLeftThighDL,                       "NPC_Gerudo3",              npcGerudo.changedCvar,               11, gsDPSetEnvColor(color.r, color.g, color.b, 255));
        PATCH_GFX(gGerudoPurpleVeilDL,                            "NPC_Gerudo4",              npcGerudo.changedCvar,               17, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGerudoPurpleLeftShinDL,                        "NPC_Gerudo5",              npcGerudo.changedCvar,               11, gsDPSetEnvColor(color.r, color.g, color.b, 255));
        PATCH_GFX(gGerudoPurpleRightShinDL,                       "NPC_Gerudo6",              npcGerudo.changedCvar,               11, gsDPSetEnvColor(color.r, color.g, color.b, 255));
        PATCH_GFX(gGerudoPurpleLeftHandDL,                        "NPC_Gerudo7",              npcGerudo.changedCvar,               17, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
        PATCH_GFX(gGerudoPurpleRightHandDL,                       "NPC_Gerudo8",              npcGerudo.changedCvar,               17, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
    }

    static CosmeticOption& npcMetalTrap = cosmeticOptions.at("NPC.MetalTrap");
    if (manualChange || CVarGetInteger(npcMetalTrap.rainbowCvar, 0)) {
        static Color_RGBA8 defaultColor = {npcMetalTrap.defaultColor.x, npcMetalTrap.defaultColor.y, npcMetalTrap.defaultColor.z, npcMetalTrap.defaultColor.w};
        Color_RGBA8 color = CVarGetColor(npcMetalTrap.cvar, defaultColor);
        PATCH_GFX(gSlidingBladeTrapDL,                            "NPC_MetalTrap1",           npcMetalTrap.changedCvar,            59, gsDPSetPrimColor(0, 0, color.r, color.g, color.b, 255));
    }

    static CosmeticOption& n64LogoRed = cosmeticOptions.at("Title.N64LogoRed");
    if (manualChange || CVarGetInteger(n64LogoRed.rainbowCvar, 0)) {
        static Color_RGBA8 defaultColor = {n64LogoRed.defaultColor.x, n64LogoRed.defaultColor.y, n64LogoRed.defaultColor.z, n64LogoRed.defaultColor.w};
        Color_RGBA8 color = CVarGetColor(n64LogoRed.cvar, defaultColor);
        PATCH_GFX(gNintendo64LogoDL,                              "Title_N64LogoRed1",          n64LogoRed.changedCvar,              17, gsDPSetPrimColor(0, 0, 255, 255, 255, 255))
        PATCH_GFX(gNintendo64LogoDL,                              "Title_N64LogoRed2",          n64LogoRed.changedCvar,              18, gsDPSetEnvColor(color.r, color.g, color.b, 128));
    }
    static CosmeticOption& n64LogoBlue = cosmeticOptions.at("Title.N64LogoBlue");
    if (manualChange || CVarGetInteger(n64LogoBlue.rainbowCvar, 0)) {
        static Color_RGBA8 defaultColor = {n64LogoBlue.defaultColor.x, n64LogoBlue.defaultColor.y, n64LogoBlue.defaultColor.z, n64LogoBlue.defaultColor.w};
        Color_RGBA8 color = CVarGetColor(n64LogoBlue.cvar, defaultColor);
        PATCH_GFX(gNintendo64LogoDL,                              "Title_N64LogoBlue1",         n64LogoBlue.changedCvar,             29, gsDPSetPrimColor(0, 0, 255, 255, 255, 255))
        PATCH_GFX(gNintendo64LogoDL,                              "Title_N64LogoBlue2",         n64LogoBlue.changedCvar,             30, gsDPSetEnvColor(color.r, color.g, color.b, 128));
    }
    static CosmeticOption& n64LogoGreen = cosmeticOptions.at("Title.N64LogoGreen");
    if (manualChange || CVarGetInteger(n64LogoGreen.rainbowCvar, 0)) {
        static Color_RGBA8 defaultColor = {n64LogoGreen.defaultColor.x, n64LogoGreen.defaultColor.y, n64LogoGreen.defaultColor.z, n64LogoGreen.defaultColor.w};
        Color_RGBA8 color = CVarGetColor(n64LogoGreen.cvar, defaultColor);
        PATCH_GFX(gNintendo64LogoDL,                              "Title_N64LogoGreen1",        n64LogoGreen.changedCvar,            56, gsDPSetPrimColor(0, 0, 255, 255, 255, 255))
        PATCH_GFX(gNintendo64LogoDL,                              "Title_N64LogoGreen2",        n64LogoGreen.changedCvar,            57, gsDPSetEnvColor(color.r, color.g, color.b, 128));
    }
    static CosmeticOption& n64LogoYellow = cosmeticOptions.at("Title.N64LogoYellow");
    if (manualChange || CVarGetInteger(n64LogoYellow.rainbowCvar, 0)) {
        static Color_RGBA8 defaultColor = {n64LogoYellow.defaultColor.x, n64LogoYellow.defaultColor.y, n64LogoYellow.defaultColor.z, n64LogoYellow.defaultColor.w};
        Color_RGBA8 color = CVarGetColor(n64LogoYellow.cvar, defaultColor);
        PATCH_GFX(gNintendo64LogoDL,                              "Title_N64LogoYellow1",       n64LogoYellow.changedCvar,           81, gsDPSetPrimColor(0, 0, 255, 255, 255, 255))
        PATCH_GFX(gNintendo64LogoDL,                              "Title_N64LogoYellow2",       n64LogoYellow.changedCvar,           82, gsDPSetEnvColor(color.r, color.g, color.b, 128));
    }

    if (gPlayState != nullptr) {
        if (CVarGetInteger(CVAR_COSMETIC("Link.BodyScale.Changed"), 0)) {
            static Player* player = GET_PLAYER(gPlayState);
            float scale = CVarGetFloat(CVAR_COSMETIC("Link.BodyScale.Value"), 0.01f);
            player->actor.scale.x = scale;
            player->actor.scale.y = scale;
            player->actor.scale.z = scale;
        }
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
    std::string CvarLabel = CvarName + ".UseMargins";
    std::string Label = ElementName + " use margins";
    UIWidgets::EnhancementCheckbox(Label.c_str(), CvarLabel.c_str());
    UIWidgets::Tooltip("Using this allow you move the element with General margins sliders");
}
void DrawPositionsRadioBoxes(const std::string CvarName, bool NoAnchorEnabled = true){
    std::string CvarLabel = CvarName + ".PosType";
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
    std::string PosXCvar = CvarName + ".PosX";
    std::string PosYCvar = CvarName + ".PosY";
    std::string InvisibleLabelX = "##"+PosXCvar;
    std::string InvisibleLabelY = "##"+PosYCvar;
    UIWidgets::EnhancementSliderInt("Up <-> Down : %d", InvisibleLabelY.c_str(), PosYCvar.c_str(), MinY, MaxY, "", 0);
    UIWidgets::Tooltip("This slider is used to move Up and Down your elements.");
    UIWidgets::EnhancementSliderInt("Left <-> Right : %d", InvisibleLabelX.c_str(), PosXCvar.c_str(), MinX, MaxX, "", 0);
    UIWidgets::Tooltip("This slider is used to move Left and Right your elements.");
}
void DrawScaleSlider(const std::string CvarName,float DefaultValue){
    std::string InvisibleLabel = "##"+CvarName;
    std::string CvarLabel = CvarName + ".Scale";
    //Disabled for now. feature not done and several fixes needed to be merged.
    //UIWidgets::EnhancementSliderFloat("Scale : %dx", InvisibleLabel.c_str(), CvarLabel.c_str(), 0.1f, 3.0f,"",DefaultValue,true);
}
void Draw_Table_Dropdown(const char* Header_Title, const char* Table_ID, const char* Column_Title, const char* Slider_Title, const char* Slider_ID, int MinY, int MaxY, int MinX, int MaxX, float Default_Value) {
    if (ImGui::CollapsingHeader(Header_Title)) {
        if (ImGui::BeginTable(Table_ID, 1, FlagsTable)) {
            ImGui::TableSetupColumn(Column_Title, FlagsCell, TablesCellsWidth);
            Table_InitHeader(false);
            DrawUseMarginsSlider(Slider_Title, Slider_ID);
            DrawPositionsRadioBoxes(Slider_ID);
            DrawPositionSlider(Slider_ID, MinY, MaxY, MinX, MaxX);
            DrawScaleSlider(Slider_ID, Default_Value);
            ImGui::NewLine();
            ImGui::EndTable();
        }
    }
}
void C_Button_Dropdown(const char* Header_Title, const char* Table_ID, const char* Column_Title, const char* Slider_Title, const char* Slider_ID, const char* Int_Type, float Slider_Scale_Value) {
    if (ImGui::CollapsingHeader(Header_Title)) {
        if (ImGui::BeginTable(Table_ID, 1, FlagsTable)) {
            ImGui::TableSetupColumn(Column_Title, FlagsCell, TablesCellsWidth);
            Table_InitHeader(false);
            DrawUseMarginsSlider(Slider_Title, Slider_ID);
            DrawPositionsRadioBoxes(Slider_ID);
            s16 Min_X_CU = 0;
            s16 Max_X_CU = ImGui::GetWindowViewport()->Size.x/2;
            if(CVarGetInteger(Int_Type,0) == 2){
                Max_X_CU = 294;
            } else if(CVarGetInteger(Int_Type,0) == 3){
                Max_X_CU = ImGui::GetWindowViewport()->Size.x/2;
            } else if(CVarGetInteger(Int_Type,0) == 4){
                Min_X_CU = (ImGui::GetWindowViewport()->Size.x/2)*-1;
            }
            DrawPositionSlider(Slider_ID, 0, ImGui::GetWindowViewport()->Size.y/2, Min_X_CU, Max_X_CU);
            DrawScaleSlider(Slider_ID, Slider_Scale_Value);
            ImGui::NewLine();
            ImGui::EndTable();
        }
        std::shared_ptr<Ship::Controller> controller = Ship::Context::GetInstance()->GetControlDeck()->GetControllerByPort(0);
        for (auto [id, mapping] : controller->GetButton(BTN_DDOWN)->GetAllButtonMappings()) {
            controller->GetButton(BTN_CUSTOM_OCARINA_NOTE_F4)->AddButtonMapping(mapping);
        }
        for (auto [id, mapping] : controller->GetButton(BTN_DRIGHT)->GetAllButtonMappings()) {
            controller->GetButton(BTN_CUSTOM_OCARINA_NOTE_A4)->AddButtonMapping(mapping);
        }
        for (auto [id, mapping] : controller->GetButton(BTN_DLEFT)->GetAllButtonMappings()) {
            controller->GetButton(BTN_CUSTOM_OCARINA_NOTE_B4)->AddButtonMapping(mapping);
        }
        for (auto [id, mapping] : controller->GetButton(BTN_DUP)->GetAllButtonMappings()) {
            controller->GetButton(BTN_CUSTOM_OCARINA_NOTE_D5)->AddButtonMapping(mapping);
        }
    }
}
void Draw_Placements(){
    if (ImGui::BeginTable("tableMargins", 1, FlagsTable)) {
        ImGui::TableSetupColumn("General margins settings", FlagsCell, TablesCellsWidth);
        Table_InitHeader();
        UIWidgets::EnhancementSliderInt("Top : %dx", "##UIMARGINT", CVAR_COSMETIC("HUD.Margin.T"), (ImGui::GetWindowViewport()->Size.y/2)*-1, 25, "", 0);
        UIWidgets::EnhancementSliderInt("Left: %dx", "##UIMARGINL", CVAR_COSMETIC("HUD.Margin.L"), -25, ImGui::GetWindowViewport()->Size.x, "", 0);
        UIWidgets::EnhancementSliderInt("Right: %dx", "##UIMARGINR", CVAR_COSMETIC("HUD.Margin.R"), (ImGui::GetWindowViewport()->Size.x)*-1, 25, "", 0);
        UIWidgets::EnhancementSliderInt("Bottom: %dx", "##UIMARGINB", CVAR_COSMETIC("HUD.Margin.B"), (ImGui::GetWindowViewport()->Size.y/2)*-1, 25, "", 0);
        SetMarginAll("All margins on",true);
        UIWidgets::Tooltip("Set most of the elements to use margins\nSome elements with default position will not be affected\nElements without Anchor or Hidden will not be turned on");
        ImGui::SameLine();
        SetMarginAll("All margins off",false);
        UIWidgets::Tooltip("Set all of the elements to not use margins");
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
            DrawUseMarginsSlider("Hearts counts", CVAR_COSMETIC("HUD.Hearts"));
            DrawPositionsRadioBoxes(CVAR_COSMETIC("HUD.HeartsCount"));
            DrawPositionSlider(CVAR_COSMETIC("HUD.HeartsCount"),-22,ImGui::GetWindowViewport()->Size.y,-125,ImGui::GetWindowViewport()->Size.x);
            DrawScaleSlider(CVAR_COSMETIC("HUD.HeartsCount"),0.7f);
            UIWidgets::EnhancementSliderInt("Heart line length : %d", "##HeartLineLength", CVAR_COSMETIC("HUD.Hearts.LineLength"), 0, 20, "", 10);
            UIWidgets::Tooltip("This will set the length of a row of hearts. Set to 0 for unlimited length.");
            ImGui::NewLine();
            ImGui::EndTable();
        }
    }
    if (ImGui::CollapsingHeader("Magic Meter position")) {
        if (ImGui::BeginTable("tablemmpos", 1, FlagsTable)) {
            ImGui::TableSetupColumn("Magic meter settings", FlagsCell, TablesCellsWidth);
            Table_InitHeader(false);
            DrawUseMarginsSlider("Magic meter", CVAR_COSMETIC("HUD.MagicBar"));
            DrawPositionsRadioBoxes(CVAR_COSMETIC("HUD.MagicBar"));
            UIWidgets::EnhancementRadioButton("Anchor to life bar", CVAR_COSMETIC("HUD.MagicBar.PosType"), 5);
            UIWidgets::Tooltip("This will make your elements follow the bottom of the life meter");
            DrawPositionSlider(CVAR_COSMETIC("HUD.MagicBar"), 0, ImGui::GetWindowViewport()->Size.y/2, -5, ImGui::GetWindowViewport()->Size.x/2);
            DrawScaleSlider(CVAR_COSMETIC("HUD.MagicBar"),1.0f);
            ImGui::NewLine();
            ImGui::EndTable();
        }
    }
    if (CVarGetInteger(CVAR_ENHANCEMENT("VisualAgony"),0) && ImGui::CollapsingHeader("Visual stone of agony position")) {
        if (ImGui::BeginTable("tabledvisualstoneofagony", 1, FlagsTable)) {
            ImGui::TableSetupColumn("Visual stone of agony settings", FlagsCell, TablesCellsWidth);
            Table_InitHeader(false);
            DrawUseMarginsSlider("Visual stone of agony", CVAR_COSMETIC("HUD.VisualSoA"));
            DrawPositionsRadioBoxes(CVAR_COSMETIC("HUD.VisualSoA"));
            s16 Min_X_VSOA = 0;
            s16 Max_X_VSOA = ImGui::GetWindowViewport()->Size.x/2;
            if(CVarGetInteger(CVAR_COSMETIC("HUD.VisualSoA.PosType"),0) == 2){
                Max_X_VSOA = 290;
            } else if(CVarGetInteger(CVAR_COSMETIC("HUD.VisualSoA.PosType"),0) == 4){
                Min_X_VSOA = (ImGui::GetWindowViewport()->Size.x/2)*-1;
            }
            DrawPositionSlider(CVAR_COSMETIC("HUD.VisualSoA"), 0, ImGui::GetWindowViewport()->Size.y/2, Min_X_VSOA, Max_X_VSOA);
            DrawScaleSlider(CVAR_COSMETIC("HUD.VisualSoA"),1.0f);
            ImGui::NewLine();
            ImGui::EndTable();
        }
    }
    Draw_Table_Dropdown("B Button position", "tablebbtn", "B Button settings", "B Button", CVAR_COSMETIC("HUD.BButton"), 0, ImGui::GetWindowViewport()->Size.y/4+50, -1, ImGui::GetWindowViewport()->Size.x-50, 0.95f);
    Draw_Table_Dropdown("A Button position", "tableabtn", "A Button settings", "A Button", CVAR_COSMETIC("HUD.AButton"), -10, ImGui::GetWindowViewport()->Size.y/4+50, -20, ImGui::GetWindowViewport()->Size.x-50, 0.95f);
    Draw_Table_Dropdown("Start Button position", "tablestartbtn", "Start Button settings", "Start Button", CVAR_COSMETIC("HUD.StartButton"), 0, ImGui::GetWindowViewport()->Size.y/2, 0, ImGui::GetWindowViewport()->Size.x/2+70, 0.75f);
    C_Button_Dropdown("C Button Up position", "tablecubtn", "C Button Up settings", "C Button Up", CVAR_COSMETIC("HUD.CUpButton"), CVAR_COSMETIC("HUD.CUpButton.PosType"), 0.5f);
    C_Button_Dropdown("C Button Down position", "tablecdbtn", "C Button Down settings", "C Button Down", CVAR_COSMETIC("HUD.CDownButton"), CVAR_COSMETIC("HUD.CDownButton.PosType"), 0.87f);
    C_Button_Dropdown("C Button Left position", "tableclbtn", "C Button Left settings", "C Button Left", CVAR_COSMETIC("HUD.CLeftButton"), CVAR_COSMETIC("HUD.CLeftButton.PosType"), 0.87f);
    C_Button_Dropdown("C Button Right position", "tablecrbtn", "C Button Right settings", "C Button Right", CVAR_COSMETIC("HUD.CRightButton"), CVAR_COSMETIC("HUD.CRightButton.PosType"), 0.87f);
    if (CVarGetInteger(CVAR_ENHANCEMENT("DpadEquips"), 0) && ImGui::CollapsingHeader("DPad items position")) {
        if (ImGui::BeginTable("tabledpaditems", 1, FlagsTable)) {
            ImGui::TableSetupColumn("DPad items settings", FlagsCell, TablesCellsWidth);
            Table_InitHeader(false);
            DrawUseMarginsSlider("DPad items", CVAR_COSMETIC("HUD.Dpad"));
            DrawPositionsRadioBoxes(CVAR_COSMETIC("HUD.Dpad"));
            s16 Min_X_Dpad = 0;
            s16 Max_X_Dpad = ImGui::GetWindowViewport()->Size.x/2;
            if(CVarGetInteger(CVAR_COSMETIC("HUD.Dpad.PosType"),0) == 2){
                Max_X_Dpad = 290;
            } else if(CVarGetInteger(CVAR_COSMETIC("HUD.Dpad.PosType"),0) == 4){
                Min_X_Dpad = (ImGui::GetWindowViewport()->Size.x/2)*-1;
            }
            DrawPositionSlider(CVAR_COSMETIC("HUD.Dpad"), 0, ImGui::GetWindowViewport()->Size.y/2, Min_X_Dpad, Max_X_Dpad);
            DrawScaleSlider(CVAR_COSMETIC("HUD.Dpad"),1.0f);
            ImGui::NewLine();
            ImGui::EndTable();
        }
    }
    Draw_Table_Dropdown("Minimaps position", "tableminimapspos", "minimaps settings", "Minimap", CVAR_COSMETIC("HUD.Minimap"), (ImGui::GetWindowViewport()->Size.y/3)*-1, ImGui::GetWindowViewport()->Size.y/3, ImGui::GetWindowViewport()->Size.x*-1, ImGui::GetWindowViewport()->Size.x/2, 1.0f);
    Draw_Table_Dropdown("Small Keys counter position", "tablesmolekeys", "Small Keys counter settings", "Small Keys counter", CVAR_COSMETIC("HUD.SmallKey"), 0, ImGui::GetWindowViewport()->Size.y/3, -1, ImGui::GetWindowViewport()->Size.x/2, 1.0f);
    Draw_Table_Dropdown("Rupee counter position", "tablerupeecount", "Rupee counter settings", "Rupee counter", CVAR_COSMETIC("HUD.Rupees"), -2, ImGui::GetWindowViewport()->Size.y/3, -3, ImGui::GetWindowViewport()->Size.x/2, 1.0f);
    Draw_Table_Dropdown("Carrots position", "tableCarrots", "Carrots settings", "Carrots", CVAR_COSMETIC("HUD.Carrots"), 0, ImGui::GetWindowViewport()->Size.y/2, -50, ImGui::GetWindowViewport()->Size.x/2+25, 1.0f);
    Draw_Table_Dropdown("Timers position", "tabletimers", "Timers settings", "Timers", CVAR_COSMETIC("HUD.Timers"), 0, ImGui::GetWindowViewport()->Size.y/2, -50, ImGui::GetWindowViewport()->Size.x/2-50, 1.0f);
    Draw_Table_Dropdown("Archery Scores position", "tablearchery", "Archery Scores settings", "Archery scores", CVAR_COSMETIC("HUD.ArcheryScore"), 0, ImGui::GetWindowViewport()->Size.y/2, -50, ImGui::GetWindowViewport()->Size.x/2-50, 1.0f);
    Draw_Table_Dropdown("Title cards (Maps) position", "tabletcmaps", "Titlecard maps settings", "Title cards (overworld)", CVAR_COSMETIC("HUD.TitleCard.Map"), 0, ImGui::GetWindowViewport()->Size.y/2, -50, ImGui::GetWindowViewport()->Size.x/2+10, 1.0f);
    Draw_Table_Dropdown("Title cards (Bosses) position", "tabletcbosses", "Title cards (Bosses) settings", "Title cards (Bosses)", CVAR_COSMETIC("HUD.TitleCard.Boss"), 0, ImGui::GetWindowViewport()->Size.y/2, -50, ImGui::GetWindowViewport()->Size.x/2+10, 1.0f);
    Draw_Table_Dropdown("In-game Gameplay Timer position", "tablegameplaytimer", "In-game Gameplay Timer settings", "In-game Gameplay Timer", CVAR_COSMETIC("HUD.IGT"), 0, ImGui::GetWindowViewport()->Size.y / 2, -50, ImGui::GetWindowViewport()->Size.x / 2 + 10, 1.0f);
    if (ImGui::CollapsingHeader("Enemy Health Bar position")) {
        if (ImGui::BeginTable("enemyhealthbar", 1, FlagsTable)) {
            ImGui::TableSetupColumn("Enemy Health Bar settings", FlagsCell, TablesCellsWidth);
            Table_InitHeader(false);
            std::string posTypeCVar = CVAR_COSMETIC("HUD.EnemyHealthBar.PosType");
            UIWidgets::EnhancementRadioButton("Anchor to Enemy", posTypeCVar.c_str(), ENEMYHEALTH_ANCHOR_ACTOR);
            UIWidgets::Tooltip("This will use enemy on screen position");
            UIWidgets::EnhancementRadioButton("Anchor to the top", posTypeCVar.c_str(), ENEMYHEALTH_ANCHOR_TOP);
            UIWidgets::Tooltip("This will make your elements follow the top edge of your game window");
            UIWidgets::EnhancementRadioButton("Anchor to the bottom", posTypeCVar.c_str(), ENEMYHEALTH_ANCHOR_BOTTOM);
            UIWidgets::Tooltip("This will make your elements follow the bottom edge of your game window");
            DrawPositionSlider(CVAR_COSMETIC("HUD.EnemyHealthBar."), -SCREEN_HEIGHT, SCREEN_HEIGHT, -ImGui::GetWindowViewport()->Size.x / 2, ImGui::GetWindowViewport()->Size.x / 2);
            if (UIWidgets::EnhancementSliderInt("Health Bar Width: %d", "##EnemyHealthBarWidth", CVAR_COSMETIC("HUD.EnemyHealthBar.Width.Value"), 32, 128, "", 64)) {
                CVarSetInteger(CVAR_COSMETIC("HUD.EnemyHealthBar.Width.Changed"), 1);
            }
            UIWidgets::Tooltip("This will change the width of the health bar");
            ImGui::SameLine();
            if (ImGui::Button("Reset##EnemyHealthBarWidth")) {
                CVarClear(CVAR_COSMETIC("HUD.EnemyHealthBar.Width.Value"));
                CVarClear(CVAR_COSMETIC("HUD.EnemyHealthBar.Width.Changed"));
                Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
            }
            ImGui::NewLine();
            ImGui::EndTable();
        }
    }
}
void Reset_Option_Single(const char* Button_Title, const char* name) {
    ImGui::SameLine();
    if (ImGui::Button(Button_Title)) {
        CVarClear(name);
        Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
    }
}
void Reset_Option_Double(const char* Button_Title, const char* name) {
    ImGui::SameLine();
    if (ImGui::Button(Button_Title)) {
        CVarClear((std::string(name) + ".Value").c_str());
        CVarClear((std::string(name) + ".Changed").c_str());
        Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
    }
}
void DrawSillyTab() {
    ImGui::BeginDisabled(CVarGetInteger(CVAR_SETTING("DisableChanges"), 0));
    if (CVarGetInteger(CVAR_GENERAL("LetItSnow"), 0)) {
        if (UIWidgets::EnhancementCheckbox("Let It Snow", CVAR_GENERAL("LetItSnow"))) {
            Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
        }
    }
    if (UIWidgets::EnhancementSliderFloat("Link Body Scale: %.3fx", "##Link_BodyScale", CVAR_COSMETIC("Link.BodyScale.Value"), 0.001f, 0.025f, "", 0.01f, true)) {
        CVarSetInteger(CVAR_COSMETIC("Link.BodyScale.Changed"), 1);
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset##Link_BodyScale")) {
        CVarClear(CVAR_COSMETIC("Link.BodyScale.Value"));
        CVarClear(CVAR_COSMETIC("Link.BodyScale.Changed"));
        Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
        if (gPlayState != nullptr) {
            static Player* player = GET_PLAYER(gPlayState);
            player->actor.scale.x = 0.01f;
            player->actor.scale.y = 0.01f;
            player->actor.scale.z = 0.01f;
        }
    }
    if (UIWidgets::EnhancementSliderFloat("Link Head Scale: %.2fx", "##Link_HeadScale", CVAR_COSMETIC("Link.HeadScale.Value"), 0.4f, 4.0f, "", 1.0f, false)) {
        CVarSetInteger(CVAR_COSMETIC("Link.HeadScale.Changed"), 1);
    }
    Reset_Option_Double("Reset##Link_HeadScale", CVAR_COSMETIC("Link.HeadScale"));
    if (UIWidgets::EnhancementSliderFloat("Link Sword Scale: %f", "##Link_SwordScale", CVAR_COSMETIC("Link.SwordScale.Value"), 1.0f, 2.5f, "", 1.0f, false)) {
        CVarSetInteger(CVAR_COSMETIC("Link.SwordScale.Changed"), 1);
    }
    Reset_Option_Double("Reset##Link_SwordScale", CVAR_COSMETIC("Link.SwordScale"));
    UIWidgets::EnhancementSliderFloat("Bunny Hood Length: %f", "##BunnyHood_EarLength", CVAR_COSMETIC("BunnyHood.EarLength"), -300.0f, 1000.0f, "", 0.0f, false);
    Reset_Option_Single("Reset##BunnyHood_EarLength", CVAR_COSMETIC("BunnyHood.EarLength"));
    UIWidgets::EnhancementSliderFloat("Bunny Hood Spread: %f", "##BunnyHood_EarSpread", CVAR_COSMETIC("BunnyHood.EarSpread"), -300.0f, 500.0f, "", 0.0f, false);
    Reset_Option_Single("Reset##BunnyHood_EarSpread", CVAR_COSMETIC("BunnyHood.EarSpread"));
    UIWidgets::EnhancementSliderFloat("Goron Neck Length: %f", "##Goron_NeckLength", CVAR_COSMETIC("Goron.NeckLength"), 0.0f, 1000.0f, "", 0.0f, false);
    Reset_Option_Single("Reset##Goron_NeckLength", CVAR_COSMETIC("Goron.NeckLength"));
    UIWidgets::EnhancementCheckbox("Unfix Goron Spin", CVAR_COSMETIC("UnfixGoronSpin"));
    UIWidgets::EnhancementSliderFloat("Fairies Size: %f", "##Fairies_Size", CVAR_COSMETIC("Fairies.Size"), 0.25f, 5.0f, "", 1.0f, false);
    Reset_Option_Single("Reset##Fairies_Size", CVAR_COSMETIC("Fairies.Size"));
    UIWidgets::EnhancementSliderFloat("N64 Logo Spin Speed: %f", "##N64Logo_SpinSpeed", CVAR_COSMETIC("N64Logo.SpinSpeed"), 0.25f, 5.0f, "", 1.0f, false);
    Reset_Option_Single("Reset##N64Logo_SpinSpeed", CVAR_COSMETIC("N64Logo.SpinSpeed"));
    UIWidgets::EnhancementSliderFloat("Moon Size: %f", "##Moon_Size", CVAR_COSMETIC("Moon.Size"), 0.5f, 2.0f, "", 1.0f, false);
    Reset_Option_Single("Reset##Moon_Size", CVAR_COSMETIC("Moon.Size"));
    if (UIWidgets::EnhancementSliderFloat("Kak Windmill Speed: %f", "##Kak_Windmill_Speed", CVAR_COSMETIC("Kak.Windmill_Speed.Value"), 100.0f, 6000.0f, "", 100.0f, false)) {
        CVarSetInteger(CVAR_COSMETIC("Kak.Windmill_Speed.Changed"), 1);
    }
    Reset_Option_Double("Reset##Kak_Windmill_Speed", CVAR_COSMETIC("Kak.Windmill_Speed"));
    ImGui::EndDisabled();
}

// Copies the RGB values from one cosmetic option to another, multiplied by the passed in amount, this
// allows you create and use multiple shades of the same color.
void CopyMultipliedColor(CosmeticOption& cosmeticOptionSrc, CosmeticOption& cosmeticOptionTarget, float amount = 0.75f) {
    Color_RGBA8 newColor;
    newColor.r = MIN((cosmeticOptionSrc.currentColor.x * 255.0) * amount, 255);
    newColor.g = MIN((cosmeticOptionSrc.currentColor.y * 255.0) * amount, 255);
    newColor.b = MIN((cosmeticOptionSrc.currentColor.z * 255.0) * amount, 255);
    newColor.a = 255;

    cosmeticOptionTarget.currentColor.x = newColor.r / 255.0;
    cosmeticOptionTarget.currentColor.y = newColor.g / 255.0;
    cosmeticOptionTarget.currentColor.z = newColor.b / 255.0;
    cosmeticOptionTarget.currentColor.w = newColor.a / 255.0;

    CVarSetColor(cosmeticOptionTarget.cvar, newColor);
    CVarSetInteger((cosmeticOptionTarget.rainbowCvar), 0);
    CVarSetInteger((cosmeticOptionTarget.changedCvar), 1);
}

void ToggleRainbow(CosmeticOption& cosmeticOption, bool state) {
    if (state) {
        CVarSetInteger(cosmeticOption.rainbowCvar, 1);
        CVarSetInteger(cosmeticOption.changedCvar, 1);
    } else {
        CVarClear(cosmeticOption.rainbowCvar);
        CVarClear(cosmeticOption.changedCvar);
    }
}

void ApplySideEffects(CosmeticOption& cosmeticOption) {
    if (CVarGetInteger(CVAR_COSMETIC("AdvancedMode"), 0)) {
        return;
    }

    // This bit is kind of experimental, not sure how I feel about it yet, but it allows for
    // advanced cosmetic options to be changed based on a non-advanced option.
    if (cosmeticOption.label == "Bow Body") {
        CopyMultipliedColor(cosmeticOption, cosmeticOptions.at("Equipment.BowTips"), 0.5f);
        CopyMultipliedColor(cosmeticOption, cosmeticOptions.at("Equipment.BowHandle"), 1.0f);
        CopyMultipliedColor(cosmeticOption, cosmeticOption, 4.0f);
    } else if (cosmeticOption.label == "Idle Primary") {
        CopyMultipliedColor(cosmeticOption, cosmeticOptions.at("Navi.IdleSecondary"), 0.5f);
    } else if (cosmeticOption.label == "Enemy Primary") {
        CopyMultipliedColor(cosmeticOption, cosmeticOptions.at("Navi.EnemySecondary"), 0.5f);
    } else if (cosmeticOption.label == "NPC Primary") {
        CopyMultipliedColor(cosmeticOption, cosmeticOptions.at("Navi.NPCSecondary"), 1.0f);
    } else if (cosmeticOption.label == "Props Primary") {
        CopyMultipliedColor(cosmeticOption, cosmeticOptions.at("Navi.PropsSecondary"), 1.0f);
    } else if (cosmeticOption.label == "Ivan Idle Primary") {
        CopyMultipliedColor(cosmeticOption, cosmeticOptions.at("Ivan.IdleSecondary"), 0.5f);
    } else if (cosmeticOption.label == "Level 1 Secondary") {
        CopyMultipliedColor(cosmeticOption, cosmeticOptions.at("SpinAttack.Level1Primary"), 2.0f);
    } else if (cosmeticOption.label == "Level 2 Secondary") {
        CopyMultipliedColor(cosmeticOption, cosmeticOptions.at("SpinAttack.Level2Primary"), 2.0f);
    } else if (cosmeticOption.label == "Item Select Color") {
        if (CVarGetInteger(cosmeticOption.rainbowCvar, 0)) {
            ToggleRainbow(cosmeticOptions.at("Kaleido.ItemSelB"), true);
            ToggleRainbow(cosmeticOptions.at("Kaleido.ItemSelC"), true);
            ToggleRainbow(cosmeticOptions.at("Kaleido.ItemSelD"), true);
        } else {
            CopyMultipliedColor(cosmeticOption, cosmeticOptions.at("Kaleido.ItemSelB"), 2.0f);
            CopyMultipliedColor(cosmeticOption, cosmeticOptions.at("Kaleido.ItemSelC"), 2.0f);
            CopyMultipliedColor(cosmeticOption, cosmeticOptions.at("Kaleido.ItemSelD"), 1.0f);
        }
    } else if (cosmeticOption.label == "Equip Select Color") {
        if (CVarGetInteger(cosmeticOption.rainbowCvar, 0)) {
            ToggleRainbow(cosmeticOptions.at("Kaleido.EquipSelB"), true);
            ToggleRainbow(cosmeticOptions.at("Kaleido.EquipSelC"), true);
            ToggleRainbow(cosmeticOptions.at("Kaleido.EquipSelD"), true);
        } else {
            CopyMultipliedColor(cosmeticOption, cosmeticOptions.at("Kaleido.EquipSelB"), 2.0f);
            CopyMultipliedColor(cosmeticOption, cosmeticOptions.at("Kaleido.EquipSelC"), 2.0f);
            CopyMultipliedColor(cosmeticOption, cosmeticOptions.at("Kaleido.EquipSelD"), 1.0f);
        }
    } else if (cosmeticOption.label == "Map Dungeon Color") {
        if (CVarGetInteger(cosmeticOption.rainbowCvar, 0)) {
            ToggleRainbow(cosmeticOptions.at("Kaleido.MapSelDunB"), true);
            ToggleRainbow(cosmeticOptions.at("Kaleido.MapSelDunC"), true);
            ToggleRainbow(cosmeticOptions.at("Kaleido.MapSelDunD"), true);
        } else {
            CopyMultipliedColor(cosmeticOption, cosmeticOptions.at("Kaleido.MapSelDunB"), 2.0f);
            CopyMultipliedColor(cosmeticOption, cosmeticOptions.at("Kaleido.MapSelDunC"), 2.0f);
            CopyMultipliedColor(cosmeticOption, cosmeticOptions.at("Kaleido.MapSelDunD"), 1.0f);
        }
    } else if (cosmeticOption.label == "Quest Status Color") {
        if (CVarGetInteger(cosmeticOption.rainbowCvar, 0)) {
            ToggleRainbow(cosmeticOptions.at("Kaleido.QuestStatusB"), true);
            ToggleRainbow(cosmeticOptions.at("Kaleido.QuestStatusC"), true);
            ToggleRainbow(cosmeticOptions.at("Kaleido.QuestStatusD"), true);
        } else {
            CopyMultipliedColor(cosmeticOption, cosmeticOptions.at("Kaleido.QuestStatusB"), 2.0f);
            CopyMultipliedColor(cosmeticOption, cosmeticOptions.at("Kaleido.QuestStatusC"), 2.0f);
            CopyMultipliedColor(cosmeticOption, cosmeticOptions.at("Kaleido.QuestStatusD"), 1.0f);
        }
    } else if (cosmeticOption.label == "Map Color") {
        if (CVarGetInteger(cosmeticOption.rainbowCvar, 0)) {
            ToggleRainbow(cosmeticOptions.at("Kaleido.MapSelectB"), true);
            ToggleRainbow(cosmeticOptions.at("Kaleido.MapSelectC"), true);
            ToggleRainbow(cosmeticOptions.at("Kaleido.MapSelectD"), true);
        } else {
            CopyMultipliedColor(cosmeticOption, cosmeticOptions.at("Kaleido.MapSelectB"), 2.0f);
            CopyMultipliedColor(cosmeticOption, cosmeticOptions.at("Kaleido.MapSelectC"), 2.0f);
            CopyMultipliedColor(cosmeticOption, cosmeticOptions.at("Kaleido.MapSelectD"), 1.0f);
        }
    } else if (cosmeticOption.label == "Save Color") {
        if (CVarGetInteger(cosmeticOption.rainbowCvar, 0)) {
            ToggleRainbow(cosmeticOptions.at("Kaleido.SaveB"), true);
            ToggleRainbow(cosmeticOptions.at("Kaleido.SaveC"), true);
            ToggleRainbow(cosmeticOptions.at("Kaleido.SaveD"), true);
        } else {
            CopyMultipliedColor(cosmeticOption, cosmeticOptions.at("Kaleido.SaveB"), 2.0f);
            CopyMultipliedColor(cosmeticOption, cosmeticOptions.at("Kaleido.SaveC"), 2.0f);
            CopyMultipliedColor(cosmeticOption, cosmeticOptions.at("Kaleido.SaveD"), 1.0f);
        }
    }
}

void RandomizeColor(CosmeticOption& cosmeticOption) {
    Color_RGBA8 newColor;
    newColor.r = Random(0, 255);
    newColor.g = Random(0, 255);
    newColor.b = Random(0, 255);
    newColor.a = 255;
    // For alpha supported options, retain the last set alpha instead of overwriting
    if (cosmeticOption.supportsAlpha) {
        newColor.a = cosmeticOption.currentColor.w * 255;
    }

    cosmeticOption.currentColor.x = newColor.r / 255.0;
    cosmeticOption.currentColor.y = newColor.g / 255.0;
    cosmeticOption.currentColor.z = newColor.b / 255.0;
    cosmeticOption.currentColor.w = newColor.a / 255.0;

    CVarSetColor(cosmeticOption.cvar, newColor);
    CVarSetInteger((cosmeticOption.rainbowCvar), 0);
    CVarSetInteger((cosmeticOption.changedCvar), 1);
    ApplySideEffects(cosmeticOption);
}

void ResetColor(CosmeticOption& cosmeticOption) {
    Color_RGBA8 defaultColor = {cosmeticOption.defaultColor.x, cosmeticOption.defaultColor.y, cosmeticOption.defaultColor.z, cosmeticOption.defaultColor.w};
    cosmeticOption.currentColor.x = defaultColor.r / 255.0;
    cosmeticOption.currentColor.y = defaultColor.g / 255.0;
    cosmeticOption.currentColor.z = defaultColor.b / 255.0;
    cosmeticOption.currentColor.w = defaultColor.a / 255.0;

    CVarClear(cosmeticOption.changedCvar);
    CVarClear(cosmeticOption.rainbowCvar);
    CVarClear(cosmeticOption.lockedCvar);
    CVarClear(cosmeticOption.cvar);
    CVarClear((std::string(cosmeticOption.cvar) + ".R").c_str());
    CVarClear((std::string(cosmeticOption.cvar) + ".G").c_str());
    CVarClear((std::string(cosmeticOption.cvar) + ".B").c_str());
    CVarClear((std::string(cosmeticOption.cvar) + ".A").c_str());
    CVarClear((std::string(cosmeticOption.cvar) + ".Type").c_str());

    // This portion should match 1:1 the multiplied colors in `ApplySideEffect()`
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
}

void DrawCosmeticRow(CosmeticOption& cosmeticOption) {
    bool colorChanged;
    if (cosmeticOption.supportsAlpha) {
        colorChanged = ImGui::ColorEdit4(cosmeticOption.label.c_str(), (float*)&cosmeticOption.currentColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
    } else {
        colorChanged = ImGui::ColorEdit3(cosmeticOption.label.c_str(), (float*)&cosmeticOption.currentColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
    }
    if (colorChanged) {
        Color_RGBA8 color;
        color.r = cosmeticOption.currentColor.x * 255.0;
        color.g = cosmeticOption.currentColor.y * 255.0;
        color.b = cosmeticOption.currentColor.z * 255.0;
        color.a = cosmeticOption.currentColor.w * 255.0;

        CVarSetColor(cosmeticOption.cvar, color);
        CVarSetInteger((cosmeticOption.rainbowCvar), 0);
        CVarSetInteger((cosmeticOption.changedCvar), 1);
        ApplySideEffects(cosmeticOption);
        ApplyOrResetCustomGfxPatches();
        Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
    }
    ImGui::SameLine();
    ImGui::Text("%s", cosmeticOption.label.c_str());
    ImGui::SameLine((ImGui::CalcTextSize("Mirror Shield Mirror").x * 1.0f) + 60.0f);
    if (ImGui::Button(("Random##" + cosmeticOption.label).c_str())) {
        RandomizeColor(cosmeticOption);
        ApplyOrResetCustomGfxPatches();
        Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
    }
    if (cosmeticOption.supportsRainbow) {
        ImGui::SameLine();
        bool isRainbow = (bool)CVarGetInteger((cosmeticOption.rainbowCvar), 0);
        if (ImGui::Checkbox(("Rainbow##" + cosmeticOption.label).c_str(), &isRainbow)) {
            CVarSetInteger((cosmeticOption.rainbowCvar), isRainbow);
            CVarSetInteger((cosmeticOption.changedCvar), 1);
            ApplySideEffects(cosmeticOption);
            ApplyOrResetCustomGfxPatches();
            Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
        }
    }
    ImGui::SameLine();
    bool isLocked = (bool)CVarGetInteger((cosmeticOption.lockedCvar), 0);
    if (ImGui::Checkbox(("Locked##" + cosmeticOption.label).c_str(), &isLocked)) {
        CVarSetInteger((cosmeticOption.lockedCvar), isLocked);
        Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
    }
    if (CVarGetInteger((cosmeticOption.changedCvar), 0)) {
        ImGui::SameLine();
        if (ImGui::Button(("Reset##" + cosmeticOption.label).c_str())) {
            ResetColor(cosmeticOption);
            ApplyOrResetCustomGfxPatches();
            Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
        }
    }
}

void DrawCosmeticGroup(CosmeticGroup cosmeticGroup) {
    std::string label = groupLabels.at(cosmeticGroup);
    ImGui::Text("%s", label.c_str());
    ImGui::SameLine((ImGui::CalcTextSize("Mirror Shield Mirror").x * 1.0f) + 60.0f);
    if (ImGui::Button(("Random##" + label).c_str())) {
        for (auto& [id, cosmeticOption] : cosmeticOptions) {
            if (cosmeticOption.group == cosmeticGroup && (!cosmeticOption.advancedOption || CVarGetInteger(CVAR_COSMETIC("AdvancedMode"), 0)) && !CVarGetInteger(cosmeticOption.lockedCvar, 0)) {
                RandomizeColor(cosmeticOption);
            }
        }
        ApplyOrResetCustomGfxPatches();
        Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
    }
    ImGui::SameLine();
    if (ImGui::Button(("Reset##" + label).c_str())) {
        for (auto& [id, cosmeticOption] : cosmeticOptions) {
            if (cosmeticOption.group == cosmeticGroup && !CVarGetInteger(cosmeticOption.lockedCvar, 0)) {
                ResetColor(cosmeticOption);
            }
        }
        ApplyOrResetCustomGfxPatches();
        Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
    }
    for (auto& [id, cosmeticOption] : cosmeticOptions) {
        if (cosmeticOption.group == cosmeticGroup && (!cosmeticOption.advancedOption || CVarGetInteger(CVAR_COSMETIC("AdvancedMode"), 0))) {
            DrawCosmeticRow(cosmeticOption);
        }
    }
}

static const char* colorSchemes[2] = {
    "N64",
    "Gamecube",
};

void CosmeticsEditorWindow::DrawElement() {
    ImGui::Text("Color Scheme");
    ImGui::SameLine();
    UIWidgets::EnhancementCombobox(CVAR_COSMETIC("DefaultColorScheme"), colorSchemes, COLORSCHEME_N64);
    UIWidgets::EnhancementCheckbox("Advanced Mode", CVAR_COSMETIC("AdvancedMode"));
    UIWidgets::InsertHelpHoverText(
        "Some cosmetic options may not apply if you have any mods that provide custom models for the cosmetic option.\n\n"
        "For example, if you have custom Link model, then the Link's Hair color option will most likely not apply."
    );
    if (CVarGetInteger(CVAR_COSMETIC("AdvancedMode"), 0)) {
        if (ImGui::Button("Lock All Advanced", ImVec2(ImGui::GetContentRegionAvail().x / 2, 30.0f))) {
            for (auto& [id, cosmeticOption] : cosmeticOptions) {
                if (cosmeticOption.advancedOption) {
                    CVarSetInteger(cosmeticOption.lockedCvar, 1);
                }
            }
            Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
        }
        ImGui::SameLine();
        if (ImGui::Button("Unlock All Advanced", ImVec2(ImGui::GetContentRegionAvail().x, 30.0f))) {
            for (auto& [id, cosmeticOption] : cosmeticOptions) {
                if (cosmeticOption.advancedOption) {
                    CVarSetInteger(cosmeticOption.lockedCvar, 0);
                }
            }
            Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
        }
    }
    UIWidgets::EnhancementCheckbox("Sync Rainbow colors", CVAR_COSMETIC("RainbowSync"));
    UIWidgets::EnhancementSliderFloat("Rainbow Speed: %.3f", "##rainbowSpeed", CVAR_COSMETIC("RainbowSpeed"), 0.03f, 1.0f, "", 0.6f, false, true);
    UIWidgets::EnhancementCheckbox("Randomize All on New Scene", CVAR_COSMETIC("RandomizeAllOnNewScene"));
    UIWidgets::Tooltip("Enables randomizing all unlocked cosmetics when you enter a new scene.");

    if (ImGui::Button("Randomize All", ImVec2(ImGui::GetContentRegionAvail().x / 2, 30.0f))) {
        CosmeticsEditor_RandomizeAll();
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset All", ImVec2(ImGui::GetContentRegionAvail().x, 30.0f))) {
        for (auto& [id, cosmeticOption] : cosmeticOptions) {
            if (!CVarGetInteger(cosmeticOption.lockedCvar, 0)) {
                ResetColor(cosmeticOption);
            }
        }
        ApplyOrResetCustomGfxPatches();
        Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
    }

    if (ImGui::Button("Lock All", ImVec2(ImGui::GetContentRegionAvail().x / 2, 30.0f))) {
        for (auto& [id, cosmeticOption] : cosmeticOptions) {
            if (!cosmeticOption.advancedOption || CVarGetInteger(CVAR_COSMETIC("AdvancedMode"), 0)) {
                CVarSetInteger(cosmeticOption.lockedCvar, 1);
            }
        }
        Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
    }
    ImGui::SameLine();
    if (ImGui::Button("Unlock All", ImVec2(ImGui::GetContentRegionAvail().x, 30.0f))) {
        for (auto& [id, cosmeticOption] : cosmeticOptions) {
            if (!cosmeticOption.advancedOption || CVarGetInteger(CVAR_COSMETIC("AdvancedMode"), 0)) {
                CVarSetInteger(cosmeticOption.lockedCvar, 0);
            }
        }
        Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
    }

    if (ImGui::BeginTabBar("CosmeticsContextTabBar", ImGuiTabBarFlags_NoCloseWithMiddleMouseButton)) {
        if (ImGui::BeginTabItem("Link & Items")) {
            DrawCosmeticGroup(COSMETICS_GROUP_LINK);
            DrawCosmeticGroup(COSMETICS_GROUP_GLOVES);
            DrawCosmeticGroup(COSMETICS_GROUP_MIRRORSHIELD);
            DrawCosmeticGroup(COSMETICS_GROUP_EQUIPMENT);
            DrawCosmeticGroup(COSMETICS_GROUP_SWORDS);
            DrawCosmeticGroup(COSMETICS_GROUP_CONSUMABLE);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Effects")) {
            // DrawCosmeticGroup(COSMETICS_GROUP_MAGIC); // Cosmetics TODO: Implement magic effect colors
            DrawCosmeticGroup(COSMETICS_GROUP_ARROWS);
            DrawCosmeticGroup(COSMETICS_GROUP_SPIN_ATTACK);
            DrawCosmeticGroup(COSMETICS_GROUP_TRAILS);
            if (UIWidgets::EnhancementSliderInt("Trails Duration: %d", "##Trails_Duration", CVAR_COSMETIC("Trails.Duration.Value"), 2, 20, "", 4)) {
                CVarSetInteger(CVAR_COSMETIC("Trails.Duration.Changed"), 1);
            }
            ImGui::SameLine();
            if (ImGui::Button("Reset##Trails_Duration")) {
                CVarClear(CVAR_COSMETIC("Trails.Duration.Value"));
                CVarClear(CVAR_COSMETIC("Trails.Duration.Changed"));
                Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
            }
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("World & NPCs")) {
            DrawCosmeticGroup(COSMETICS_GROUP_WORLD);
            DrawCosmeticGroup(COSMETICS_GROUP_NAVI);
            DrawCosmeticGroup(COSMETICS_GROUP_IVAN);
            DrawCosmeticGroup(COSMETICS_GROUP_NPC);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Silly")) {
            DrawSillyTab();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("HUD")) {
            DrawCosmeticGroup(COSMETICS_GROUP_HUD);
            DrawCosmeticGroup(COSMETICS_GROUP_TITLE);
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("HUD Placement")) {
            Draw_Placements();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Pause Menu")) {
            DrawCosmeticGroup(COSMETICS_GROUP_KALEIDO);
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
}

void RegisterOnLoadGameHook() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnLoadGame>([](int32_t fileNum) {
        ApplyOrResetCustomGfxPatches();
    });
}

void RegisterOnGameFrameUpdateHook() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnGameFrameUpdate>([]() {
        CosmeticsUpdateTick();
    });
}

void Cosmetics_RegisterOnSceneInitHook() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneInit>([](int16_t sceneNum) {
        if (CVarGetInteger(CVAR_COSMETIC("RandomizeAllOnNewScene"), 0)) {
            CosmeticsEditor_RandomizeAll();
        }
    });
}

void CosmeticsEditorWindow::InitElement() {
    // Convert the `current color` into the format that the ImGui color picker expects
    for (auto& [id, cosmeticOption] : cosmeticOptions) {
        Color_RGBA8 defaultColor = {cosmeticOption.defaultColor.x, cosmeticOption.defaultColor.y, cosmeticOption.defaultColor.z, cosmeticOption.defaultColor.w};
        Color_RGBA8 cvarColor = CVarGetColor(cosmeticOption.cvar, defaultColor);

        cosmeticOption.currentColor.x = cvarColor.r / 255.0;
        cosmeticOption.currentColor.y = cvarColor.g / 255.0;
        cosmeticOption.currentColor.z = cvarColor.b / 255.0;
        cosmeticOption.currentColor.w = cvarColor.a / 255.0;
    }
    Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
    ApplyOrResetCustomGfxPatches();
    ApplyAuthenticGfxPatches();

    RegisterOnLoadGameHook();
    RegisterOnGameFrameUpdateHook();
    Cosmetics_RegisterOnSceneInitHook();
}

void CosmeticsEditor_RandomizeAll() {
    for (auto& [id, cosmeticOption] : cosmeticOptions) {
        if (!CVarGetInteger(cosmeticOption.lockedCvar, 0) &&
            (!cosmeticOption.advancedOption || CVarGetInteger(CVAR_COSMETIC("AdvancedMode"), 0))) {
            RandomizeColor(cosmeticOption);
        }
    }

    Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
    ApplyOrResetCustomGfxPatches();
}

void CosmeticsEditor_RandomizeGroup(CosmeticGroup group) {
    for (auto& [id, cosmeticOption] : cosmeticOptions) {
        if (!CVarGetInteger(cosmeticOption.lockedCvar, 0) &&
            (!cosmeticOption.advancedOption || CVarGetInteger(CVAR_COSMETIC("AdvancedMode"), 0)) &&
            cosmeticOption.group == group) {
            RandomizeColor(cosmeticOption);
        }
    }

    Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
    ApplyOrResetCustomGfxPatches();
}

void CosmeticsEditor_ResetAll() {
    for (auto& [id, cosmeticOption] : cosmeticOptions) {
        if (!CVarGetInteger(cosmeticOption.lockedCvar, 0)) {
            ResetColor(cosmeticOption);
        }
    }

    Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
    ApplyOrResetCustomGfxPatches();
}

void CosmeticsEditor_ResetGroup(CosmeticGroup group) {
    for (auto& [id, cosmeticOption] : cosmeticOptions) {
        if (!CVarGetInteger(cosmeticOption.lockedCvar, 0) && cosmeticOption.group == group) {
            ResetColor(cosmeticOption);
        }
    }

    Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
    ApplyOrResetCustomGfxPatches();
}
