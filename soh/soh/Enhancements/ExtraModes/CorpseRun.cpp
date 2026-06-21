#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/Enhancements/randomizer/draw.h"
#include "soh/ShipInit.hpp"
#include "z64item.h"

extern "C" {
#include "functions.h"
#include "macros.h"
#include "variables.h"
extern PlayState* gPlayState;
}

static constexpr int32_t CVAR_CORPSE_RUN_DEFAULT = 0;
#define CVAR_CORPSE_RUN_NAME CVAR_ENHANCEMENT("CorpseRun")
#define CVAR_CORPSE_RUN_VALUE CVarGetInteger(CVAR_CORPSE_RUN_NAME, CVAR_CORPSE_RUN_DEFAULT)

struct CorpseRunRemnant {
    bool dropped = false;
    bool active = false;

    int16_t sceneNum = -1;
    int8_t roomNum = -1;

    Vec3f pos = {};
    int16_t yaw = 0;

    int32_t rupees = 0;
    int32_t bombs = 0;
    int32_t arrows = 0;
    int32_t sticks = 0;
    int32_t nuts = 0;
    int32_t magic = 0;

    int32_t seeds = 0;
    int32_t bombchus = 0;
};

static CorpseRunRemnant sCorpseRunRemnant;

static bool IsInRemnantRoom() {
    return sCorpseRunRemnant.dropped 
        && gPlayState != nullptr 
        && gPlayState->sceneNum == sCorpseRunRemnant.sceneNum 
        && gPlayState->roomCtx.curRoom.num == sCorpseRunRemnant.roomNum;
}

static void ActivateRemnantOnPlayerInit(Player* player, PlayState* playState, int32_t respawnFlag) {
    if (!sCorpseRunRemnant.dropped) {
        return;
    }

    sCorpseRunRemnant.active = true;
}

static void DropRemnantOnDeath() {
    // Safety in case death hook may fire more than once during the same death flow.
    // If we already dropped a remnant, but it has not been activated by Player_Init yet, this is probably the same death, so ignore it.
    if (sCorpseRunRemnant.dropped && !sCorpseRunRemnant.active) {
        return;
    }
    
    if (gPlayState == nullptr) {
        return;
    }

    Player* player = GET_PLAYER(gPlayState);
    if (player == nullptr) {
        return;
    }

    // If there is already an active remnant, v1 can simply overwrite it.
    // Later, this is where second-death behavior lives.
    sCorpseRunRemnant = {};

    sCorpseRunRemnant.dropped = true;
    sCorpseRunRemnant.active = false;
    sCorpseRunRemnant.sceneNum = gPlayState->sceneNum;
    sCorpseRunRemnant.roomNum = gPlayState->roomCtx.curRoom.num;
    sCorpseRunRemnant.pos = player->actor.world.pos;
    sCorpseRunRemnant.yaw = player->actor.shape.rot.y;

    // v1 - consumables only.
    sCorpseRunRemnant.rupees = gSaveContext.rupees;
    sCorpseRunRemnant.bombs = AMMO(ITEM_BOMB);
    sCorpseRunRemnant.arrows = AMMO(ITEM_BOW);
    sCorpseRunRemnant.sticks = AMMO(ITEM_STICK);
    sCorpseRunRemnant.nuts = AMMO(ITEM_NUT);
    sCorpseRunRemnant.seeds = AMMO(ITEM_SLINGSHOT);
    sCorpseRunRemnant.bombchus = AMMO(ITEM_BOMBCHU);

    // Then remove them from the player.
    gSaveContext.rupees = 0;
    AMMO(ITEM_BOMB) = 0;
    AMMO(ITEM_BOW) = 0;
    AMMO(ITEM_STICK) = 0;
    AMMO(ITEM_NUT) = 0;
    AMMO(ITEM_SLINGSHOT) = 0;
    AMMO(ITEM_BOMBCHU) = 0;
}

static void RecoverRemnant() {
    gSaveContext.rupees += sCorpseRunRemnant.rupees;
    AMMO(ITEM_BOMB) += sCorpseRunRemnant.bombs;
    AMMO(ITEM_BOW) += sCorpseRunRemnant.arrows;
    AMMO(ITEM_STICK) += sCorpseRunRemnant.sticks;
    AMMO(ITEM_NUT) += sCorpseRunRemnant.nuts;
    AMMO(ITEM_SLINGSHOT) += sCorpseRunRemnant.seeds;
    AMMO(ITEM_BOMBCHU) += sCorpseRunRemnant.bombchus;

    // TODO(jperos): clamp ammo/rupees to capacity

    sCorpseRunRemnant = {};
}

static void UpdateRemnant() {
    if (!IsInRemnantRoom() || !sCorpseRunRemnant.active) {
        return;
    }

    Player* player = GET_PLAYER(gPlayState);
    if (player == nullptr) {
        return;
    }

    const float dx = player->actor.world.pos.x - sCorpseRunRemnant.pos.x;
    const float dy = player->actor.world.pos.y - sCorpseRunRemnant.pos.y;
    const float dz = player->actor.world.pos.z - sCorpseRunRemnant.pos.z;
    const float distSq = dx * dx + dy * dy + dz * dz;

    if (distSq < SQ(80.0f) && sCorpseRunRemnant.active) {
        RecoverRemnant();
    }
}

static GetItemEntry sCorpseRunMysteryItem = GET_ITEM_MYSTERY;

static void DrawRemnant() {
    if (!IsInRemnantRoom()) {
        return;
    }

    Matrix_Push();

    // Cheap bob/spin so it reads as collectible-ish.
    const s16 bobAngle = static_cast<s16>(gPlayState->gameplayFrames * 0x400);
    const s16 spinAngle = static_cast<s16>(gPlayState->gameplayFrames * 0x200);

    const f32 bob = Math_SinS(bobAngle) * 8.0f;

    Matrix_Translate(
        sCorpseRunRemnant.pos.x,
        sCorpseRunRemnant.pos.y + 40.0f + bob,
        sCorpseRunRemnant.pos.z,
        MTXMODE_NEW
    );

    Matrix_RotateY(static_cast<f32>(BINANG_TO_RAD(spinAngle)), MTXMODE_APPLY);
    Matrix_Scale(0.35f, 0.35f, 0.35f, MTXMODE_APPLY);

    Randomizer_DrawMysteryItem(gPlayState, &sCorpseRunMysteryItem);

    Matrix_Pop();
}

static void RegisterCorpseRun() {
    COND_HOOK(OnPlayerInit, CVAR_CORPSE_RUN_VALUE, ActivateRemnantOnPlayerInit);
    COND_HOOK(OnPlayerDeath, CVAR_CORPSE_RUN_VALUE, DropRemnantOnDeath);
    COND_HOOK(OnGameFrameUpdate, CVAR_CORPSE_RUN_VALUE, UpdateRemnant);
    COND_HOOK(OnPlayDrawEnd, CVAR_CORPSE_RUN_VALUE, DrawRemnant);
}

static RegisterShipInitFunc initFunc(RegisterCorpseRun, { CVAR_CORPSE_RUN_NAME });
