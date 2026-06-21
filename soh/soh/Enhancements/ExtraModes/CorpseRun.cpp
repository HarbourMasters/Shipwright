#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/Enhancements/randomizer/draw.h"
#include "soh/SaveManager.h"
#include "soh/ShipInit.hpp"
#include "z64item.h"

extern "C" {
#include "functions.h"
#include "macros.h"
#include "variables.h"
extern PlayState* gPlayState;
}

static constexpr int32_t INVALID_SAVE_SECTION_ID = -1;
static constexpr int32_t CORPSE_RUN_VERSION = 1;
static int32_t sCorpseRunSaveSectionId = INVALID_SAVE_SECTION_ID;

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

static CorpseRunRemnant sRemnant;

static void InitCorpseRunSave(bool isDebug) {
    sRemnant = {};
}

static void LoadCorpseRunSave() {
    sRemnant = {};

    SaveManager::Instance->LoadData("dropped", sRemnant.dropped, false);

    if (!sRemnant.dropped) {
        return;
    }

    // Loaded remnants should always be recoverable
    sRemnant.active = true;

    SaveManager::Instance->LoadData("sceneNum", sRemnant.sceneNum, (int16_t)-1);
    SaveManager::Instance->LoadData("roomNum", sRemnant.roomNum, (int8_t)-1);

    SaveManager::Instance->LoadStruct("pos", []() {
        SaveManager::Instance->LoadData("x", sRemnant.pos.x, 0.0f);
        SaveManager::Instance->LoadData("y", sRemnant.pos.y, 0.0f);
        SaveManager::Instance->LoadData("z", sRemnant.pos.z, 0.0f);
    });

    SaveManager::Instance->LoadData("yaw", sRemnant.yaw, (int16_t)0);

    SaveManager::Instance->LoadStruct("contents", []() {
        SaveManager::Instance->LoadData("rupees",   sRemnant.rupees,   0);
        SaveManager::Instance->LoadData("bombs",    sRemnant.bombs,    0);
        SaveManager::Instance->LoadData("arrows",   sRemnant.arrows,   0);
        SaveManager::Instance->LoadData("sticks",   sRemnant.sticks,   0);
        SaveManager::Instance->LoadData("nuts",     sRemnant.nuts,     0);
        SaveManager::Instance->LoadData("seeds",    sRemnant.seeds,    0);
        SaveManager::Instance->LoadData("bombchus", sRemnant.bombchus, 0);
    });
}

static void SaveCorpseRunSave(SaveContext* saveContext, int sectionID, bool fullSave) {
    SaveManager::Instance->SaveData("dropped", sRemnant.dropped);

    if (!sRemnant.dropped) {
        return;
    }

    SaveManager::Instance->SaveData("sceneNum", sRemnant.sceneNum);
    SaveManager::Instance->SaveData("roomNum", sRemnant.roomNum);

    SaveManager::Instance->SaveStruct("pos", []() {
        SaveManager::Instance->SaveData("x", sRemnant.pos.x);
        SaveManager::Instance->SaveData("y", sRemnant.pos.y);
        SaveManager::Instance->SaveData("z", sRemnant.pos.z);
    });

    SaveManager::Instance->SaveData("yaw", sRemnant.yaw);

    SaveManager::Instance->SaveStruct("contents", []() {
        SaveManager::Instance->SaveData("rupees",   sRemnant.rupees  );
        SaveManager::Instance->SaveData("bombs",    sRemnant.bombs   );
        SaveManager::Instance->SaveData("arrows",   sRemnant.arrows  );
        SaveManager::Instance->SaveData("sticks",   sRemnant.sticks  );
        SaveManager::Instance->SaveData("nuts",     sRemnant.nuts    );
        SaveManager::Instance->SaveData("seeds",    sRemnant.seeds   );
        SaveManager::Instance->SaveData("bombchus", sRemnant.bombchus);
    });
}

static void RegisterCorpseRunSave() {
    if (sCorpseRunSaveSectionId != INVALID_SAVE_SECTION_ID) {
        return;
    }
    SaveManager::Instance->AddInitFunction(InitCorpseRunSave);
    SaveManager::Instance->AddLoadFunction("corpseRun", CORPSE_RUN_VERSION, LoadCorpseRunSave);
    sCorpseRunSaveSectionId = SaveManager::Instance->AddSaveFunction("corpseRun", CORPSE_RUN_VERSION, SaveCorpseRunSave, true, SECTION_PARENT_NONE);
}

static bool HasRemnantContents(const CorpseRunRemnant& remnant) {
    return remnant.rupees   > 0
        || remnant.bombs    > 0
        || remnant.arrows   > 0
        || remnant.sticks   > 0
        || remnant.nuts     > 0
        || remnant.seeds    > 0
        || remnant.bombchus > 0;
}

static bool IsInRemnantRoom() {
    return sRemnant.dropped 
        && gPlayState != nullptr 
        && gPlayState->sceneNum == sRemnant.sceneNum 
        && gPlayState->roomCtx.curRoom.num == sRemnant.roomNum;
}

static void ActivateRemnantOnPlayerInit(Player* player, PlayState* playState, int32_t respawnFlag) {
    if (!sRemnant.dropped) {
        return;
    }

    sRemnant.active = true;
}

static void DropRemnantOnDeath() {
    // Safety in case death hook may fire more than once during the same death flow.
    // If we already dropped a remnant, but it has not been activated by Player_Init yet, this is probably the same death, so ignore it.
    if (sRemnant.dropped && !sRemnant.active) {
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
    sRemnant = {};

    // TODO(jperos): Should we drop a remnant if the player has no inventory? We do still want to kill the previous

    sRemnant.dropped = true;
    sRemnant.active = false;
    sRemnant.sceneNum = gPlayState->sceneNum;
    sRemnant.roomNum = gPlayState->roomCtx.curRoom.num;
    sRemnant.pos = player->actor.world.pos;
    sRemnant.yaw = player->actor.shape.rot.y;

    // v1 - consumables only.
    sRemnant.rupees   = gSaveContext.rupees;
    sRemnant.bombs    = AMMO(ITEM_BOMB);
    sRemnant.arrows   = AMMO(ITEM_BOW);
    sRemnant.sticks   = AMMO(ITEM_STICK);
    sRemnant.nuts     = AMMO(ITEM_NUT);
    sRemnant.seeds    = AMMO(ITEM_SLINGSHOT);
    sRemnant.bombchus = AMMO(ITEM_BOMBCHU);

    // Then remove them from the player.
    gSaveContext.rupees  = 0;
    AMMO(ITEM_BOMB)      = 0;
    AMMO(ITEM_BOW)       = 0;
    AMMO(ITEM_STICK)     = 0;
    AMMO(ITEM_NUT)       = 0;
    AMMO(ITEM_SLINGSHOT) = 0;
    AMMO(ITEM_BOMBCHU)   = 0;

    SaveManager::Instance->SaveFile(gSaveContext.fileNum);
}

static void RecoverRemnant() {
    gSaveContext.rupees  += sRemnant.rupees;
    AMMO(ITEM_BOMB)      += sRemnant.bombs;
    AMMO(ITEM_BOW)       += sRemnant.arrows;
    AMMO(ITEM_STICK)     += sRemnant.sticks;
    AMMO(ITEM_NUT)       += sRemnant.nuts;
    AMMO(ITEM_SLINGSHOT) += sRemnant.seeds;
    AMMO(ITEM_BOMBCHU)   += sRemnant.bombchus;

    // TODO(jperos): clamp ammo/rupees to capacity

    sRemnant = {};

    SaveManager::Instance->SaveFile(gSaveContext.fileNum);
}

static void UpdateRemnant() {
    if (!IsInRemnantRoom() || !sRemnant.active) {
        return;
    }

    Player* player = GET_PLAYER(gPlayState);
    if (player == nullptr) {
        return;
    }

    const float dx = player->actor.world.pos.x - sRemnant.pos.x;
    const float dy = player->actor.world.pos.y - sRemnant.pos.y;
    const float dz = player->actor.world.pos.z - sRemnant.pos.z;
    const float distSq = dx * dx + dy * dy + dz * dz;

    if (distSq < SQ(80.0f) && sRemnant.active) {
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
        sRemnant.pos.x,
        sRemnant.pos.y + 40.0f + bob,
        sRemnant.pos.z,
        MTXMODE_NEW
    );

    Matrix_RotateY(static_cast<f32>(BINANG_TO_RAD(spinAngle)), MTXMODE_APPLY);
    Matrix_Scale(0.35f, 0.35f, 0.35f, MTXMODE_APPLY);

    Randomizer_DrawMysteryItem(gPlayState, &sCorpseRunMysteryItem);

    Matrix_Pop();
}

static void RegisterCorpseRun() {
    RegisterCorpseRunSave();

    COND_HOOK(OnPlayerInit,      CVAR_CORPSE_RUN_VALUE, ActivateRemnantOnPlayerInit);
    COND_HOOK(OnPlayerDeath,     CVAR_CORPSE_RUN_VALUE, DropRemnantOnDeath);
    COND_HOOK(OnGameFrameUpdate, CVAR_CORPSE_RUN_VALUE, UpdateRemnant);
    COND_HOOK(OnPlayDrawEnd,     CVAR_CORPSE_RUN_VALUE, DrawRemnant);
}

static RegisterShipInitFunc initFunc(RegisterCorpseRun, { CVAR_CORPSE_RUN_NAME });
