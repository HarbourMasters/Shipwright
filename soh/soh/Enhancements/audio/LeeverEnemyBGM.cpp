#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "macros.h"
}

static constexpr int32_t CVAR_ENEMYBGMDISABLE_DEFAULT = 0;
#define CVAR_ENEMYBGMDISABLE_NAME CVAR_AUDIO("EnemyBGMDisable")
#define CVAR_ENEMYBGMDISABLE_VALUE CVarGetInteger(CVAR_ENEMYBGMDISABLE_NAME, CVAR_ENEMYBGMDISABLE_DEFAULT)

static constexpr int32_t CVAR_LEEVERENEMYBGM_DEFAULT = 0;
#define CVAR_LEEVERENEMYBGM_NAME CVAR_AUDIO("LeeverEnemyBGM")
#define CVAR_LEEVERENEMYBGM_VALUE CVarGetInteger(CVAR_LEEVERENEMYBGM_NAME, CVAR_LEEVERENEMYBGM_DEFAULT)

static void RegisterLeeverEnemyBGM() {
    COND_VB_SHOULD(VB_DETECT_BGM_ENEMY, !CVAR_ENEMYBGMDISABLE_VALUE && CVAR_LEEVERENEMYBGM_VALUE, {
        Actor* actor = va_arg(args, Actor*);
        f32* sbgmEnemyDistSq = va_arg(args, f32*);
        int32_t actorCategory = va_arg(args, int32_t);

        if ((actor->id == ACTOR_EN_REEBA) &&
            CHECK_FLAG_ALL(actor->flags, ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE) &&
            (actor->xyzDistToPlayerSq < SQ(500.0f)) && (actor->xyzDistToPlayerSq < *sbgmEnemyDistSq)) {
            *should = true;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterLeeverEnemyBGM, { CVAR_ENEMYBGMDISABLE_NAME, CVAR_LEEVERENEMYBGM_NAME });
