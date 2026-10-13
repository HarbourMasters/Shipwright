#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"
#include "2s2h/ShipInit.hpp"

#include <string>

extern "C" {
uint8_t ResourceMgr_FileExists(const char* resName);
Gfx* ResourceMgr_LoadGfxByName(const char* path);
bool ResourceMgr_IsAltAssetsEnabled();
}

static const char* sFierceDeitySwordInSheathDLPath = "__OTR__objects/object_link_boy/gLinkFierceDeitySwordInSheathDL";
static const char* sFierceDeitySwordSheathDLPath = "__OTR__objects/object_link_boy/gLinkFierceDeitySwordSheathDL";
static Gfx* sFierceDeitySwordInSheathDLs[2] = { nullptr, nullptr };
static Gfx* sFierceDeitySwordSheathDLs[2] = { nullptr, nullptr };

static s32 sFierceDeitySheathAltState = -1;

static bool ResourceMgr_DListExistsForCurrentAltState(const char* basePath) {
    std::string resourcePath = basePath;
    if (resourcePath.starts_with("__OTR__")) {
        resourcePath = resourcePath.substr(7);
    }

    if (ResourceMgr_IsAltAssetsEnabled()) {
        const std::string altPath = "__OTR__alt/" + resourcePath;
        if (ResourceMgr_FileExists(altPath.c_str())) {
            return true;
        }
    }

    return ResourceMgr_FileExists(basePath);
}

static bool CustomFDSheath_LoadAssets() {
    const s32 altState = ResourceMgr_IsAltAssetsEnabled() ? 1 : 0;
    if (sFierceDeitySheathAltState != altState) {
        sFierceDeitySheathAltState = altState;
        sFierceDeitySwordInSheathDLs[0] = sFierceDeitySwordInSheathDLs[1] = nullptr;
        sFierceDeitySwordSheathDLs[0] = sFierceDeitySwordSheathDLs[1] = nullptr;

        if (ResourceMgr_DListExistsForCurrentAltState(sFierceDeitySwordInSheathDLPath) &&
            ResourceMgr_DListExistsForCurrentAltState(sFierceDeitySwordSheathDLPath)) {
            sFierceDeitySwordInSheathDLs[0] = sFierceDeitySwordInSheathDLs[1] =
                ResourceMgr_LoadGfxByName(sFierceDeitySwordInSheathDLPath);
            sFierceDeitySwordSheathDLs[0] = sFierceDeitySwordSheathDLs[1] =
                ResourceMgr_LoadGfxByName(sFierceDeitySwordSheathDLPath);
        }
    }

    return (sFierceDeitySwordInSheathDLs[0] != nullptr) && (sFierceDeitySwordSheathDLs[0] != nullptr);
}

static Gfx** CustomFDSheath_GetDLists(Player* player) {
    if (!CustomFDSheath_LoadAssets()) {
        return nullptr;
    }

    if (player->modelGroup == PLAYER_MODELGROUP_TWO_HAND_SWORD) {
        return sFierceDeitySwordSheathDLs;
    }

    return sFierceDeitySwordInSheathDLs;
}

void RegisterCustomFDSheath() {
    GameInteractor::Instance->RegisterGameHookForID<GameInteractor::OnActorUpdate>(ACTOR_PLAYER, [](Actor* actor) {
        Player* player = (Player*)actor;
        Gfx** sheathDLists;

        if (player->transformation != PLAYER_FORM_FIERCE_DEITY) {
            return;
        }

        sheathDLists = CustomFDSheath_GetDLists(player);
        if (sheathDLists == nullptr) {
            return;
        }

        player->sheathDLists = sheathDLists;
    });
}

static RegisterShipInitFunc initFunc(RegisterCustomFDSheath, {});
