#include <libultraship/bridge.h>
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"
#include "functions.h"
#include "macros.h"

extern "C" {
extern PlayState* gPlayState;
extern SaveContext gSaveContext;
}

static constexpr int32_t CVAR_DAYTIME_GS_DEFAULT = 0;
#define CVAR_DAYTIME_GS_NAME CVAR_ENHANCEMENT("NightGSAlwaysSpawn")
#define CVAR_DAYTIME_GS_VALUE CVarGetInteger(CVAR_DAYTIME_GS_NAME, CVAR_DAYTIME_GS_DEFAULT)

struct DayTimeGoldSkulltulas {
    uint16_t scene;
    uint16_t room;
    bool forChild;
    std::vector<ActorEntry> actorEntries;
};

using DayTimeGoldSkulltulasList = std::vector<DayTimeGoldSkulltulas>;

void OnSpawnNighttimeGoldSkulltula() {
    // Gold Skulltulas that are not part of the scene actor list during the day
    // Actor values copied from the night time scene actor list
    static const DayTimeGoldSkulltulasList dayTimeGoldSkulltulas = {
        // Graveyard
        { SCENE_GRAVEYARD, 1, true, { { ACTOR_EN_SW, { 156, 315, 795 }, { 16384, -32768, 0 }, -20096 } } },
        // ZF
        { SCENE_ZORAS_FOUNTAIN, 0, true, { { ACTOR_EN_SW, { -1891, 187, 1911 }, { 16384, 18022, 0 }, -19964 } } },
        // GF
        { SCENE_GERUDOS_FORTRESS, 0, false, { { ACTOR_EN_SW, { 1598, 999, -2008 }, { 16384, -16384, 0 }, -19198 } } },
        { SCENE_GERUDOS_FORTRESS, 1, false, { { ACTOR_EN_SW, { 3377, 1734, -4935 }, { 16384, 0, 0 }, -19199 } } },
        // Kak
        { SCENE_KAKARIKO_VILLAGE, 0, false, { { ACTOR_EN_SW, { -18, 540, 1800 }, { 0, -32768, 0 }, -20160 } } },
        { SCENE_KAKARIKO_VILLAGE,
          0,
          true,
          { { ACTOR_EN_SW, { -465, 377, -888 }, { 0, 28217, 0 }, -20222 },
            { ACTOR_EN_SW, { 5, 686, -171 }, { 0, -32768, 0 }, -20220 },
            { ACTOR_EN_SW, { 324, 270, 905 }, { 16384, 0, 0 }, -20216 },
            { ACTOR_EN_SW, { -602, 120, 1120 }, { 16384, 0, 0 }, -20208 } } },
        // LLR
        { SCENE_LON_LON_RANCH,
          0,
          true,
          { { ACTOR_EN_SW, { -2344, 180, 672 }, { 16384, 22938, 0 }, -29695 },
            { ACTOR_EN_SW, { 808, 48, 326 }, { 16384, 0, 0 }, -29694 },
            { ACTOR_EN_SW, { 997, 286, -2698 }, { 16384, -16384, 0 }, -29692 } } },
    };

    for (const auto& dayTimeGS : dayTimeGoldSkulltulas) {
        if (IS_DAY && dayTimeGS.forChild == LINK_IS_CHILD && dayTimeGS.scene == gPlayState->sceneNum &&
            dayTimeGS.room == gPlayState->roomCtx.curRoom.num) {
            for (const auto& actorEntry : dayTimeGS.actorEntries) {
                Actor_Spawn(&gPlayState->actorCtx, gPlayState, actorEntry.id, actorEntry.pos.x, actorEntry.pos.y,
                            actorEntry.pos.z, actorEntry.rot.x, actorEntry.rot.y, actorEntry.rot.z, actorEntry.params,
                            false);
            }
        }
    }
}

void RegisterDaytimeGoldSkultullas() {
    COND_HOOK(OnSceneSpawnActors, CVAR_DAYTIME_GS_VALUE, OnSpawnNighttimeGoldSkulltula);
}

static RegisterShipInitFunc initFunc_DaytimeGoldSkulltulas(RegisterDaytimeGoldSkultullas, { CVAR_DAYTIME_GS_NAME });
