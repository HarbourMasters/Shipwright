#include "ShipUtils.h"
#include "assets/2s2h_assets.h"
#include <string>
#include <bit>
#include <random>
#include <vector>
#include <cassert>
#include <libultraship/bridge/consolevariablebridge.h>
#include <ship/Context.h>
#include <ship/window/Window.h>
// Image Icons
#include "assets/interface/parameter_static/parameter_static.h"
#include "assets/archives/icon_item_24_static/icon_item_24_static_yar.h"
#include "assets/archives/icon_item_static/icon_item_static_yar.h"
#include "assets/interface/icon_item_dungeon_static/icon_item_dungeon_static.h"
#include "assets/interface/icon_item_field_static/icon_item_field_static.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"
#include "assets/overlays/ovl_En_Syateki_Okuta/ovl_En_Syateki_Okuta.h"

#include <fast/Fast3dGui.h>

extern "C" {
#include "z64.h"
#include "functions.h"
#include "macros.h"

extern float OTRGetAspectRatio();

extern f32 sNESFontWidths[160];
extern const char* fontTbl[156];
extern TexturePtr gItemIcons[131];
extern TexturePtr gQuestIcons[14];
extern TexturePtr gBombersNotebookPhotos[24];
}

// 2S2H Added columns to scene table: entranceSceneId, betterMapSelectIndex, humanName
#define DEFINE_SCENE(_name, enumValue, _textId, _drawConfig, _restrictionFlags, _persistentCycleFlags, \
                     _entranceSceneId, _betterMapSelectIndex, humanName)                               \
    { enumValue, humanName },
#define DEFINE_SCENE_UNSET(_enumValue)

std::unordered_map<s16, const char*> sceneNames = {
#include "tables/scene_table.h"
};

#undef DEFINE_SCENE
#undef DEFINE_SCENE_UNSET

std::array<const char*, ACTORCAT_MAX> actorCategoryNames = { "Switch", "Background", "Player", "Explosive",
                                                             "NPC",    "Enemy",      "Prop",   "Item/Action",
                                                             "Misc.",  "Boss",       "Door",   "Chest" };

#define DEFINE_ACTOR(name, _enumValue, _allocType, _debugName, _humanName) { _enumValue, _humanName },
#define DEFINE_ACTOR_INTERNAL(_name, _enumValue, _allocType, _debugName, _humanName) { _enumValue, _humanName },
#define DEFINE_ACTOR_UNSET(_enumValue) { _enumValue, "Unset" },

std::unordered_map<s16, const char*> actorDescriptions = {
#include "tables/actor_table.h"
};

#undef DEFINE_ACTOR
#undef DEFINE_ACTOR_INTERNAL
#undef DEFINE_ACTOR_UNSET

#define DEFINE_ACTOR(name, _enumValue, _allocType, _debugName, _humanName) { _enumValue, _debugName },
#define DEFINE_ACTOR_INTERNAL(_name, _enumValue, _allocType, _debugName, _humanName) { _enumValue, _debugName },
#define DEFINE_ACTOR_UNSET(_enumValue) { _enumValue, "Unset" },

std::unordered_map<s16, const char*> actorDebugNames = {
#include "tables/actor_table.h"
};

#undef DEFINE_ACTOR
#undef DEFINE_ACTOR_INTERNAL
#undef DEFINE_ACTOR_UNSET

/*
 * This is identical to `sOwlWarpEntrances` in decomp code, which is a static variable defined in multiple places and
 * cannot be externed. We redundantly define it here and extern it, for use in port enhancements.
 */
extern u16 sOwlWarpEntrancesForMods[OWL_WARP_MAX - 1] = {
    ENTRANCE(GREAT_BAY_COAST, 11),         // OWL_WARP_GREAT_BAY_COAST
    ENTRANCE(ZORA_CAPE, 6),                // OWL_WARP_ZORA_CAPE
    ENTRANCE(SNOWHEAD, 3),                 // OWL_WARP_SNOWHEAD
    ENTRANCE(MOUNTAIN_VILLAGE_WINTER, 8),  // OWL_WARP_MOUNTAIN_VILLAGE
    ENTRANCE(SOUTH_CLOCK_TOWN, 9),         // OWL_WARP_CLOCK_TOWN
    ENTRANCE(MILK_ROAD, 4),                // OWL_WARP_MILK_ROAD
    ENTRANCE(WOODFALL, 4),                 // OWL_WARP_WOODFALL
    ENTRANCE(SOUTHERN_SWAMP_POISONED, 10), // OWL_WARP_SOUTHERN_SWAMP
    ENTRANCE(IKANA_CANYON, 4),             // OWL_WARP_IKANA_CANYON
    ENTRANCE(STONE_TOWER, 3),              // OWL_WARP_STONE_TOWER
};

// These textures are not in existing lists that we iterate over.
std::array<const char*, 33> miscellaneousTextures = {
    gArcheryScoreIconTex,
    gBarrelTrackerIcon,
    gChestTrackerIcon,
    gCrateTrackerIcon,
    gDungeonStrayFairyGreatBayIconTex,
    gDungeonStrayFairySnowheadIconTex,
    gDungeonStrayFairyStoneTowerIconTex,
    gDungeonStrayFairyWoodfallIconTex,
    gPotTrackerIcon,
    gQuestIconGoldSkulltulaTex,
    gMagicArrowEquipEffectTex,
    gRupeeCounterIconTex,
    gStrayFairyGreatBayIconTex,
    gStrayFairySnowheadIconTex,
    gStrayFairyStoneTowerIconTex,
    gStrayFairyWoodfallIconTex,
    gTimerClockIconTex,
    gTriforcePieceTex,
    gFlippersTex,
    gWorldMapOwlFaceTex,
    gameplay_keep_Tex_053140,
    gDungeonMapSkullTex,
    gPauseUnusedCursorTex,
    gWorldMapOwlFaceTex,
    gItemIconTingleMapTex,
    gThreeDayClockSunHourTex,
    gThreeDayClockMoonHourTex,
    gOcarinaATex,
    gOcarinaCDownTex,
    gOcarinaCLeftTex,
    gOcarinaCRightTex,
    gOcarinaCUpTex,
    gShootingGalleryOctorokCrossTex,
};

std::array<const char*, 11> digitList = { gCounterDigit0Tex, gCounterDigit1Tex, gCounterDigit2Tex, gCounterDigit3Tex,
                                          gCounterDigit4Tex, gCounterDigit5Tex, gCounterDigit6Tex, gCounterDigit7Tex,
                                          gCounterDigit8Tex, gCounterDigit9Tex, gCounterColonTex };

std::map<uint32_t, ImVec4> itemColorMap = {
    { ITEM_SONG_SONATA, ImVec4(0.588f, 1.0f, 0.392f, 1.0f) },
    { ITEM_SONG_LULLABY, ImVec4(1.0f, 0.313f, 0.156f, 1.0f) },
    { ITEM_SONG_NOVA, ImVec4(0.392f, 0.588f, 1.0f, 1.0f) },
    { ITEM_SONG_ELEGY, ImVec4(1.0f, 0.627f, 0.0f, 1.0f) },
    { ITEM_SONG_OATH, ImVec4(1.0f, 0.392f, 1.0f, 1.0f) },
    { ITEM_SONG_LULLABY_INTRO, ImVec4(1.0f, 0.313f, 0.156f, 1.0f) },
};

ImVec4 Ship_GetItemColorTint(uint32_t itemId) {
    auto findColor = itemColorMap.find(itemId);
    if (findColor != itemColorMap.end()) {
        return findColor->second;
    } else {
        return ImVec4(1, 1, 1, 1);
    }
}

std::unordered_map<uint32_t, ImVec4> randoItemColorMap = {
    { RI_OCARINA_BUTTON_A, ImVec4(0.085f, 0.494f, 0.796f, 1) },
    { RI_OCARINA_BUTTON_C_DOWN, ImVec4(0.84f, 0.768f, 0.089f, 1) },
    { RI_OCARINA_BUTTON_C_LEFT, ImVec4(0.84f, 0.768f, 0.089f, 1) },
    { RI_OCARINA_BUTTON_C_RIGHT, ImVec4(0.84f, 0.768f, 0.089f, 1) },
    { RI_OCARINA_BUTTON_C_UP, ImVec4(0.84f, 0.768f, 0.089f, 1) },
    { RI_CLOCK_TOWN_STRAY_FAIRY, ImVec4(1.0f, 0.9f, 0.5f, 0.4f) },
    { RI_WOODFALL_SMALL_KEY, ImVec4(0.9f, 0.33f, 0.56f, 0.4f) },
    { RI_SNOWHEAD_SMALL_KEY, ImVec4(0.1f, 0.54f, 0.16f, 0.4f) },
    { RI_GREAT_BAY_SMALL_KEY, ImVec4(0.61f, 0.04f, 0.86f, 0.4f) },
    { RI_STONE_TOWER_SMALL_KEY, ImVec4(0.58f, 0.65f, 0.15f, 0.4f) },
    { RI_WOODFALL_BOSS_KEY, ImVec4(0.9f, 0.33f, 0.56f, 0.4f) },
    { RI_SNOWHEAD_BOSS_KEY, ImVec4(0.1f, 0.54f, 0.16f, 0.4f) },
    { RI_GREAT_BAY_BOSS_KEY, ImVec4(0.61f, 0.04f, 0.86f, 0.4f) },
    { RI_STONE_TOWER_BOSS_KEY, ImVec4(0.58f, 0.65f, 0.15f, 0.4f) },
    { RI_TIME_DAY_1, ImVec4(1.0f, 0.9f, 0.3f, 1.0f) },
    { RI_TIME_DAY_2, ImVec4(1.0f, 0.9f, 0.3f, 1.0f) },
    { RI_TIME_DAY_3, ImVec4(1.0f, 0.9f, 0.3f, 1.0f) },
    { RI_TIME_NIGHT_1, ImVec4(0.5f, 0.7f, 1.0f, 1.0f) },
    { RI_TIME_NIGHT_2, ImVec4(0.5f, 0.7f, 1.0f, 1.0f) },
    { RI_TIME_NIGHT_3, ImVec4(0.5f, 0.7f, 1.0f, 1.0f) },
    { RI_SONG_SONATA, ImVec4(0.588f, 1.0f, 0.392f, 1.0f) },
    { RI_SONG_LULLABY, ImVec4(1.0f, 0.313f, 0.156f, 1.0f) },
    { RI_SONG_NOVA, ImVec4(0.392f, 0.588f, 1.0f, 1.0f) },
    { RI_SONG_ELEGY, ImVec4(1.0f, 0.627f, 0.0f, 1.0f) },
    { RI_SONG_OATH, ImVec4(1.0f, 0.392f, 1.0f, 1.0f) },
    { RI_SONG_LULLABY_INTRO, ImVec4(1.0f, 0.313f, 0.156f, 1.0f) },
};

ImVec4 Ship_GetRandoItemColorTint(uint32_t randoItemId) {
    auto findColor = randoItemColorMap.find(randoItemId);
    if (findColor != randoItemColorMap.end()) {
        return findColor->second;
    } else {
        return ImVec4(1, 1, 1, 1);
    }
}

extern "C" const char* Ship_GetSceneName(s16 sceneId) {
    if (sceneNames.contains(sceneId)) {
        return sceneNames[sceneId];
    }

    return "Unknown";
}

std::string Ship_FormatTimeDisplay(uint32_t value) {
    uint32_t sec = value / 10;
    uint32_t hh = sec / 3600;
    uint32_t mm = (sec - hh * 3600) / 60;
    uint32_t ss = sec - hh * 3600 - mm * 60;
    uint32_t ds = value % 10;
    return fmt::format("{}:{:0>2}:{:0>2}.{}", hh, mm, ss, ds);
}

std::string Ship_RemoveSpecialCharacters(const std::string& str) {
    std::string result;
    for (char ch : str) {
        // Only keep alphanumeric characters (letters and digits)
        if (std::isalnum(static_cast<unsigned char>(ch))) {
            result += ch;
        }
    }
    return result;
}

constexpr f32 fourByThree = 4.0f / 3.0f;

// Gets the additional ratio of the screen compared to the original 4:3 ratio, clamping to 1 if smaller
extern "C" f32 Ship_GetExtendedAspectRatioMultiplier() {
    f32 currentRatio = OTRGetAspectRatio();
    return MAX(currentRatio / fourByThree, 1.0f);
}

// Enables Extended Culling options on specific actors by applying an inverse ratio of the draw distance slider
// to the projected Z value of the actor. This tricks distance checks without having to replace hardcoded values.
// Requires that Ship_ExtendedCullingActorRestoreProjectedPos is called within the same function scope.
extern "C" void Ship_ExtendedCullingActorAdjustProjectedZ(Actor* actor) {
    s32 multiplier = CVarGetInteger("gEnhancements.Graphics.IncreaseActorDrawDistance", 1);
    multiplier = MAX(multiplier, 1);
    if (multiplier > 1) {
        actor->projectedPos.z /= multiplier;
    }
}

// Enables Extended Culling options on specific actors by applying an inverse ratio of the widescreen aspect ratio
// to the projected X value of the actor. This tricks distance checks without having to replace hardcoded values.
// Requires that Ship_ExtendedCullingActorRestoreProjectedPos is called within the same function scope.
extern "C" void Ship_ExtendedCullingActorAdjustProjectedX(Actor* actor) {
    if (CVarGetInteger("gEnhancements.Graphics.ActorCullingAccountsForWidescreen", 0)) {
        f32 ratioAdjusted = Ship_GetExtendedAspectRatioMultiplier();
        actor->projectedPos.x /= ratioAdjusted;
    }
}

// Restores the projectedPos values on the actor after modifications from the Extended Culling hacks
extern "C" void Ship_ExtendedCullingActorRestoreProjectedPos(PlayState* play, Actor* actor) {
    f32 invW = 0.0f;
    Actor_GetProjectedPos(play, &actor->world.pos, &actor->projectedPos, &invW);
}

extern "C" bool Ship_IsCStringEmpty(const char* str) {
    return str == NULL || str[0] == '\0';
}

// Build vertex coordinates for a quad command
// In order of top left, top right, bottom left, then bottom right
// Supports flipping the texture horizontally
extern "C" void Ship_CreateQuadVertexGroup(Vtx* vtxList, s32 xStart, s32 yStart, s32 width, s32 height, u8 flippedH) {
    vtxList[0].v.ob[0] = xStart;
    vtxList[0].v.ob[1] = yStart;
    vtxList[0].v.tc[0] = (flippedH ? width : 0) << 5;
    vtxList[0].v.tc[1] = 0 << 5;

    vtxList[1].v.ob[0] = xStart + width;
    vtxList[1].v.ob[1] = yStart;
    vtxList[1].v.tc[0] = (flippedH ? width * 2 : width) << 5;
    vtxList[1].v.tc[1] = 0 << 5;

    vtxList[2].v.ob[0] = xStart;
    vtxList[2].v.ob[1] = yStart + height;
    vtxList[2].v.tc[0] = (flippedH ? width : 0) << 5;
    vtxList[2].v.tc[1] = height << 5;

    vtxList[3].v.ob[0] = xStart + width;
    vtxList[3].v.ob[1] = yStart + height;
    vtxList[3].v.tc[0] = (flippedH ? width * 2 : width) << 5;
    vtxList[3].v.tc[1] = height << 5;
}

extern "C" f32 Ship_GetCharFontWidthNES(u8 character) {
    u8 adjustedChar = character - ' ';

    if (adjustedChar >= ARRAY_COUNTU(sNESFontWidths)) {
        return 0.0f;
    }

    return sNESFontWidths[adjustedChar];
}

extern "C" TexturePtr Ship_GetCharFontTextureNES(u8 character) {
    u8 adjustedChar = character - ' ';

    if (adjustedChar >= ARRAY_COUNTU(sNESFontWidths)) {
        return (TexturePtr)gEmptyTexture;
    }

    return (TexturePtr)fontTbl[adjustedChar];
}

static bool seeded = false;
static uint64_t state = 0;
const uint64_t multiplier = 6364136223846793005ULL;
const uint64_t increment = 11634580027462260723ULL;

extern "C" void Ship_Random_Seed(u64 seed) {
    seeded = true;
    state = seed;
}

uint32_t next32() {
    if (!seeded) {
        uint64_t seed = static_cast<uint64_t>(std::random_device{}());
        Ship_Random_Seed(seed);
    }
    state = state * multiplier + increment;
    uint32_t xorshifted = static_cast<uint32_t>(((state >> 18) ^ state) >> 27);
    uint32_t rot = static_cast<int>(state >> 59);
    return std::rotr(xorshifted, rot);
}

extern "C" s32 Ship_Random(s32 min, s32 max) {
    if (min == max) {
        return min;
    }
    assert(max > min);
    uint32_t n = max - min;
    uint32_t cutoff = UINT32_MAX - UINT32_MAX % static_cast<uint32_t>(n);
    for (;;) {
        uint32_t r = next32();
        if (r <= cutoff) {
            return min + r % n;
        }
    }
}

extern uint32_t Ship_Hash(std::string str) {
    // FNV-1a
    const size_t len = str.size();
    uint32_t hval = 0x811c9dc5;
    for (size_t pos = 0; pos < len; pos++) {
        hval ^= (uint32_t)str[pos];
        hval *= 0x01000193;
    }
    return hval;
}

void LoadGuiTextures() {
    for (const TexturePtr entry : gItemIcons) {
        auto path = static_cast<const char*>(entry);
        std::dynamic_pointer_cast<Fast::Fast3dGui>(Ship::Context::GetRawInstance()->GetWindow()->GetGui())
            ->LoadGuiTexture(path, path, "", ImVec4(1, 1, 1, 1));
    }
    for (const TexturePtr entry : gQuestIcons) {
        auto path = static_cast<const char*>(entry);
        std::dynamic_pointer_cast<Fast::Fast3dGui>(Ship::Context::GetRawInstance()->GetWindow()->GetGui())
            ->LoadGuiTexture(path, path, "", ImVec4(1, 1, 1, 1));
    }
    for (const TexturePtr entry : gBombersNotebookPhotos) {
        auto path = static_cast<const char*>(entry);
        std::dynamic_pointer_cast<Fast::Fast3dGui>(Ship::Context::GetRawInstance()->GetWindow()->GetGui())
            ->LoadGuiTexture(path, path, "", ImVec4(1, 1, 1, 1));
    }
    for (const auto entry : miscellaneousTextures) {
        std::dynamic_pointer_cast<Fast::Fast3dGui>(Ship::Context::GetRawInstance()->GetWindow()->GetGui())
            ->LoadGuiTexture(entry, entry, "", ImVec4(1, 1, 1, 1));
    }
    for (const auto entry : digitList) {
        std::dynamic_pointer_cast<Fast::Fast3dGui>(Ship::Context::GetRawInstance()->GetWindow()->GetGui())
            ->LoadGuiTexture(entry, entry, "", ImVec4(1, 1, 1, 1));
    }
}

std::string convertEnumToReadableName(const std::string& input, const std::string& prefix) {
    std::string result;
    std::string content = input;

    // Step 1: Remove prefix if present
    if (!prefix.empty() && content.rfind(prefix, 0) == 0) {
        content = content.substr(prefix.size());
    }

    // Step 2: Split the string by '_'
    std::vector<std::string> words;
    std::string word;
    std::istringstream stream(content);
    while (std::getline(stream, word, '_')) {
        words.push_back(word);
    }

    // Step 3: Capitalize the first letter of each word
    for (auto& w : words) {
        std::transform(w.begin(), w.end(), w.begin(), [](unsigned char c) { return std::tolower(c); });
        if (!w.empty()) {
            if (w == "hp") {
                w = "HP";
            } else {
                w[0] = std::toupper(w[0]);
            }
        }
    }

    // Step 4: Join the words with spaces
    for (size_t i = 0; i < words.size(); ++i) {
        result += words[i];
        if (i < words.size() - 1) {
            result += " ";
        }
    }

    return result;
}

std::string GetActorDescription(u16 actorNum) {
    return actorDescriptions.contains(actorNum) ? actorDescriptions[actorNum] : "???";
}

std::string GetActorDebugName(u16 actorNum) {
    return actorDebugNames.contains(actorNum) ? actorDebugNames[actorNum] : "???";
}

std::string GetActorCategoryName(u8 category) {
    if (category < actorCategoryNames.size()) {
        return actorCategoryNames[category];
    }
    return "Unknown";
}

extern "C" {
Vtx sCycleExtraItemVtx[8] = {
    // Left Item
    VTX(-48, 16, 0, 0 << 5, 0 << 5, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-16, 16, 0, 32 << 5, 0 << 5, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-48, -16, 0, 0 << 5, 32 << 5, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-16, -16, 0, 32 << 5, 32 << 5, 0xFF, 0xFF, 0xFF, 0xFF),
    // Right Item
    VTX(16, 16, 0, 0 << 5, 0 << 5, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(48, 16, 0, 32 << 5, 0 << 5, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(16, -16, 0, 0 << 5, 32 << 5, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(48, -16, 0, 32 << 5, 32 << 5, 0xFF, 0xFF, 0xFF, 0xFF),
};

// Vertices for A button indicator (coordinates 1.5x larger than texture size)
Vtx sCycleAButtonVtx[4] = {
    VTX(-18, 12, 0, 0 << 5, 0 << 5, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(18, 12, 0, 24 << 5, 0 << 5, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-18, -12, 0, 0 << 5, 16 << 5, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(18, -12, 0, 24 << 5, 16 << 5, 0xFF, 0xFF, 0xFF, 0xFF),
};

void Ship_DrawKaleidoCycleAButtonPrompt(PlayState* play, u8 alpha) {
    Color_RGB8 aButtonColor = { 0, 100, 255 };

    OPEN_DISPS(play->state.gfxCtx);
    gSPVertex(POLY_OPA_DISP++, (uintptr_t)sCycleAButtonVtx, 4, 0);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, aButtonColor.r, aButtonColor.g, aButtonColor.b, alpha);
    gDPLoadTextureBlock(POLY_OPA_DISP++, gABtnSymbolTex, G_IM_FMT_IA, G_IM_SIZ_8b, 24, 16, 0,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, 4, 4, G_TX_NOLOD, G_TX_NOLOD);
    gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);
    CLOSE_DISPS(play->state.gfxCtx);
}
}
