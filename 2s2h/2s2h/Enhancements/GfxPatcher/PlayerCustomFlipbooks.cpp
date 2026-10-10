#include "PlayerCustomFlipbooks.h"
#include "2s2h/BenPort.h"

#include <array>
#include <string>

extern "C" {
#include "z64player.h"
extern TexturePtr sPlayerEyesTextures[PLAYER_FORM_MAX][PLAYER_EYES_MAX];
extern TexturePtr sPlayerMouthTextures[PLAYER_FORM_MAX][PLAYER_MOUTH_MAX];
uint8_t ResourceMgr_FileExists(const char* resName);
bool ResourceMgr_IsAltAssetsEnabled();
}

static const char* sFDEyesTextures[PLAYER_EYES_MAX] = {
    "__OTR__objects/object_link_boy/gLinkFierceDeityEyesOpenTex",
    "__OTR__objects/object_link_boy/gLinkFierceDeityEyesHalfTex",
    "__OTR__objects/object_link_boy/gLinkFierceDeityEyesClosedTex",
    "__OTR__objects/object_link_boy/gLinkFierceDeityEyesRightTex",
    "__OTR__objects/object_link_boy/gLinkFierceDeityEyesLeftTex",
    "__OTR__objects/object_link_boy/gLinkFierceDeityEyesUpTex",
    "__OTR__objects/object_link_boy/gLinkFierceDeityEyesDownTex",
    "__OTR__objects/object_link_boy/gLinkFierceDeityEyesWincingTex",
};

static const char* sFDMouthTextures[PLAYER_MOUTH_MAX] = {
    "__OTR__objects/object_link_boy/gLinkFierceDeityMouthClosedTex",
    "__OTR__objects/object_link_boy/gLinkFierceDeityMouthHalfTex",
    "__OTR__objects/object_link_boy/gLinkFierceDeityMouthOpenTex",
    "__OTR__objects/object_link_boy/gLinkFierceDeityMouthSmileTex",
};

static const char* sDekuEyesTextures[PLAYER_EYES_MAX] = {
    "__OTR__objects/object_link_nuts/gLinkDekuEyesOpenTex",   "__OTR__objects/object_link_nuts/gLinkDekuEyesHalfTex",
    "__OTR__objects/object_link_nuts/gLinkDekuEyesClosedTex", "__OTR__objects/object_link_nuts/gLinkDekuEyesRightTex",
    "__OTR__objects/object_link_nuts/gLinkDekuEyesLeftTex",   "__OTR__objects/object_link_nuts/gLinkDekuEyesUpTex",
    "__OTR__objects/object_link_nuts/gLinkDekuEyesDownTex",   "__OTR__objects/object_link_nuts/gLinkDekuEyesWincingTex",
};

static const char* sDekuMouthTextures[PLAYER_MOUTH_MAX] = {
    "__OTR__objects/object_link_nuts/gLinkDekuMouthClosedTex",
    "__OTR__objects/object_link_nuts/gLinkDekuMouthHalfTex",
    "__OTR__objects/object_link_nuts/gLinkDekuMouthOpenTex",
    "__OTR__objects/object_link_nuts/gLinkDekuMouthSmileTex",
};

static const char* sGoronMouthTextures[PLAYER_MOUTH_MAX] = {
    "__OTR__objects/object_link_goron/gLinkGoronMouthClosedTex",
    "__OTR__objects/object_link_goron/gLinkGoronMouthHalfTex",
    "__OTR__objects/object_link_goron/gLinkGoronMouthOpenTex",
    "__OTR__objects/object_link_goron/gLinkGoronMouthSmileTex",
};

static std::array<std::string, PLAYER_EYES_MAX> sResolvedFDEyesTextures;
static std::array<std::string, PLAYER_MOUTH_MAX> sResolvedFDMouthTextures;
static std::array<std::string, PLAYER_EYES_MAX> sResolvedDekuEyesTextures;
static std::array<std::string, PLAYER_MOUTH_MAX> sResolvedDekuMouthTextures;
static std::array<std::string, PLAYER_MOUTH_MAX> sResolvedGoronMouthTextures;

static s32 sFacePatchAltState = -1;

static bool ResolveTexturePath(const char* basePath, std::string& resolvedPath) {
    std::string resourcePath = basePath;
    if (resourcePath.starts_with("__OTR__")) {
        resourcePath = resourcePath.substr(7);
    }

    if (ResourceMgr_IsAltAssetsEnabled()) {
        const std::string altPath = "__OTR__alt/" + resourcePath;
        if (ResourceMgr_FileExists(altPath.c_str())) {
            resolvedPath = altPath;
            return true;
        }
    }

    if (ResourceMgr_FileExists(basePath)) {
        resolvedPath = basePath;
        return true;
    }

    return false;
}

template <size_t Count>
static bool ResolveTextureSet(const char* const (&sourcePaths)[Count], std::array<std::string, Count>& resolvedPaths) {
    for (size_t i = 0; i < Count; i++) {
        if (!ResolveTexturePath(sourcePaths[i], resolvedPaths[i])) {
            return false;
        }
    }

    return true;
}

static void ApplyEyesTextureSet(s32 form, const std::array<std::string, PLAYER_EYES_MAX>& resolvedPaths) {
    for (s32 i = 0; i < PLAYER_EYES_MAX; i++) {
        sPlayerEyesTextures[form][i] = (TexturePtr)resolvedPaths[i].c_str();
    }
}

static void ApplyMouthTextureSet(s32 form, const std::array<std::string, PLAYER_MOUTH_MAX>& resolvedPaths) {
    for (s32 i = 0; i < PLAYER_MOUTH_MAX; i++) {
        sPlayerMouthTextures[form][i] = (TexturePtr)resolvedPaths[i].c_str();
    }
}

void PlayerCustomFlipbooks_Patch(void) {
    const s32 altState = ResourceMgr_IsAltAssetsEnabled() ? 1 : 0;
    if (sFacePatchAltState == altState) {
        return;
    }

    sFacePatchAltState = altState;

    if (ResolveTextureSet(sFDEyesTextures, sResolvedFDEyesTextures)) {
        ApplyEyesTextureSet(PLAYER_FORM_FIERCE_DEITY, sResolvedFDEyesTextures);
    }

    if (ResolveTextureSet(sFDMouthTextures, sResolvedFDMouthTextures)) {
        ApplyMouthTextureSet(PLAYER_FORM_FIERCE_DEITY, sResolvedFDMouthTextures);
    }

    if (ResolveTextureSet(sDekuEyesTextures, sResolvedDekuEyesTextures)) {
        ApplyEyesTextureSet(PLAYER_FORM_DEKU, sResolvedDekuEyesTextures);
    }

    if (ResolveTextureSet(sDekuMouthTextures, sResolvedDekuMouthTextures)) {
        ApplyMouthTextureSet(PLAYER_FORM_DEKU, sResolvedDekuMouthTextures);
    }

    if (ResolveTextureSet(sGoronMouthTextures, sResolvedGoronMouthTextures)) {
        ApplyMouthTextureSet(PLAYER_FORM_GORON, sResolvedGoronMouthTextures);
    }
}
