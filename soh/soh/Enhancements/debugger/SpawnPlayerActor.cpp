#include <libultraship/bridge.h>

#include "soh/ActorDB.h"
#include "soh/Notification/Notification.h"
#include "soh/ShipInit.hpp"
#include "soh/cvar_prefixes.h"
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"

extern "C" {
#include "functions.h"
#include "macros.h"
#include "variables.h"
#include "z64.h"
}

extern "C" PlayState* gPlayState;
extern "C" ActorDBEntry* ActorDB_Retrieve(const int id);

void DebugSpawnLink_Trigger();

namespace {

#define CVAR_SPAWN_LINK_ENABLED CVAR_DEVELOPER_TOOLS("SpawnLink.Enabled")
#define CVAR_SPAWN_LINK_BTN     CVAR_DEVELOPER_TOOLS("SpawnLink.Btn")

constexpr int32_t kSpawnLinkBtnDefault = (BTN_L | BTN_R | BTN_A);

static bool sPendingEnableAfterCutscene = false;
static s16 sPendingSceneNum = -1;

static bool DebugModeEnabled() {
    return CVarGetInteger(CVAR_DEVELOPER_TOOLS("DebugEnabled"), 0) != 0;
}

static bool FeatureEnabled() {
    return CVarGetInteger(CVAR_SPAWN_LINK_ENABLED, 1) != 0;
}

static void EmitToast(const std::string& msg) {
    Notification::Emit({ .message = msg, .remainingTime = 2.0f });
}

static void ForcePlayerOutOfCsState(Player* player) {
    if (player == nullptr) {
        return;
    }

    player->stateFlags1 &= ~PLAYER_STATE1_IN_CUTSCENE;
    player->csAction = 0;
    player->unk_6AD = 0;
}

static void MovePlayerToCameraTarget(PlayState* play, Player* player) {
    if (play == nullptr || player == nullptr) {
        return;
    }

    Camera* cam = Play_GetCamera(play, SUBCAM_ACTIVE);
    if (cam == nullptr) {
        return;
    }

    Vec3f pos = cam->at;
    s16 yaw = Math_Vec3f_Yaw(&cam->eye, &cam->at);

    CollisionPoly* floorPoly = nullptr;
    s32 bgId = BGCHECK_SCENE;
    f32 floorY = BgCheck_EntityRaycastFloor3(&play->colCtx, &floorPoly, &bgId, &pos);
    if (floorY != BGCHECK_Y_MIN) {
        pos.y = floorY + 5.0f;
    }

    Math_Vec3f_Copy(&player->actor.world.pos, &pos);
    Math_Vec3f_Copy(&player->actor.prevPos, &pos);
    Math_Vec3f_Copy(&player->actor.home.pos, &pos);

    player->actor.world.rot.y = yaw;
    player->actor.shape.rot.y = yaw;
    player->yaw = yaw;

    player->actor.velocity.x = 0.0f;
    player->actor.velocity.y = 0.0f;
    player->actor.velocity.z = 0.0f;
    player->actor.speedXZ = 0.0f;
}

static Player* SpawnPlayerIfMissing(PlayState* play) {
    if (play == nullptr) {
        return nullptr;
    }

    Player* player = GET_PLAYER(play);
    if (player != nullptr) {
        return player;
    }

    Camera* cam = Play_GetCamera(play, SUBCAM_ACTIVE);
    Vec3f pos = { 0.0f, 0.0f, 0.0f };
    s16 yaw = 0;

    if (cam != nullptr) {
        pos = cam->at;
        yaw = Math_Vec3f_Yaw(&cam->eye, &cam->at);

        CollisionPoly* floorPoly = nullptr;
        s32 bgId = BGCHECK_SCENE;
        f32 floorY = BgCheck_EntityRaycastFloor3(&play->colCtx, &floorPoly, &bgId, &pos);
        if (floorY != BGCHECK_Y_MIN) {
            pos.y = floorY + 5.0f;
        }
    }

    Actor* spawned = Actor_Spawn(&play->actorCtx, play, ACTOR_PLAYER, pos.x, pos.y, pos.z, 0, yaw, 0, 0, 0);
    return (Player*)spawned;
}

static void RestorePlayerUpdateDrawIfNeeded(Player* player) {
    if (player == nullptr) {
        return;
    }

    if (player->actor.draw == nullptr || player->actor.update == nullptr) {
        ActorDBEntry* db = ActorDB_Retrieve(ACTOR_PLAYER);
        if (db != nullptr) {
            player->actor.update = db->update;
            player->actor.draw = db->draw;
        }
    }
}

static bool EnableAndControlLink(PlayState* play) {
    if (play == nullptr) {
        return false;
    }

    Player* player = SpawnPlayerIfMissing(play);
    if (player == nullptr) {
        EmitToast("Spawn Link failed");
        return false;
    }

    RestorePlayerUpdateDrawIfNeeded(player);
    ForcePlayerOutOfCsState(player);

    // Non-optional: always place Link at the camera target (avoids crashy edge cases)
    MovePlayerToCameraTarget(play, player);

    EmitToast("Link restored");
    return true;
}

static void RequestEnableAfterCutscene(PlayState* play) {
    if (play == nullptr) {
        return;
    }

    sPendingEnableAfterCutscene = true;
    sPendingSceneNum = play->sceneNum;
}

static void TryFulfillPending() {
    if (!sPendingEnableAfterCutscene || gPlayState == nullptr) {
        return;
    }

    if (gPlayState->sceneNum != sPendingSceneNum) {
        sPendingEnableAfterCutscene = false;
        return;
    }

    if (gPlayState->csCtx.state != CS_STATE_IDLE) {
        return;
    }

    sPendingEnableAfterCutscene = false;
    EnableAndControlLink(gPlayState);
}

static void OnGameFrameUpdateSpawnLinkAfterCutsceneCancel() {
    if (gPlayState == nullptr) {
        return;
    }
    if (!DebugModeEnabled() || !FeatureEnabled()) {
        return;
    }

    Input* input = &gPlayState->state.input[0];

    // Manual hotkey only (defaults to L+R+A)
    const uint16_t btnMask = (uint16_t)CVarGetInteger(CVAR_SPAWN_LINK_BTN, kSpawnLinkBtnDefault);
    if (btnMask != 0 && CHECK_BTN_ANY(input->press.button, btnMask) && CHECK_BTN_ALL(input->cur.button, btnMask)) {
        if (gPlayState->csCtx.state != CS_STATE_IDLE) {
            RequestEnableAfterCutscene(gPlayState);
            EmitToast("Link restore queued");
        } else {
            EnableAndControlLink(gPlayState);
        }
    }

    TryFulfillPending();
}

static void RegisterSpawnLinkAfterCutsceneCancel() {
    COND_HOOK(OnGameFrameUpdate, DebugModeEnabled() && FeatureEnabled(), OnGameFrameUpdateSpawnLinkAfterCutsceneCancel);
}

static RegisterShipInitFunc initFunc(RegisterSpawnLinkAfterCutsceneCancel,
                                    { CVAR_DEVELOPER_TOOLS("DebugEnabled"),
                                      CVAR_SPAWN_LINK_ENABLED,
                                      CVAR_SPAWN_LINK_BTN });

} // namespace

void DebugSpawnLink_Trigger() {
    if (gPlayState == nullptr) {
        return;
    }
    if (!DebugModeEnabled() || !FeatureEnabled()) {
        return;
    }

    if (gPlayState->csCtx.state != CS_STATE_IDLE) {
        RequestEnableAfterCutscene(gPlayState);
        EmitToast("Link restore queued");
    } else {
        EnableAndControlLink(gPlayState);
    }
}
