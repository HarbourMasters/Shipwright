#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "overlays/actors/ovl_En_Minifrog/z_en_minifrog.h"

void EnMinifrog_Update(Actor* thisx, PlayState* play);
void EnMinifrog_Draw(Actor* thisx, PlayState* play);
void EnMinifrog_SetupNextFrogInit(EnMinifrog* enMinifrog, PlayState* play);
void EnMinifrog_UpdateMissingFrog(Actor* thisx, PlayState* play);
}

#define CVAR_NAME "gEnhancements.DifficultyOptions.FrogChoirCount"
#define CVAR CVarGetInteger(CVAR_NAME, 5)

u8 SavedFrogs() {
    // Start at 1 for the Yellow Frog
    u8 saved = 1;
    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_32_40)) {
        saved++;
    }
    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_32_80)) {
        saved++;
    }
    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_33_01)) {
        saved++;
    }
    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_33_02)) {
        saved++;
    }
    return saved;
}

void RegisterFrogChoirCount() {
    COND_ID_HOOK(OnActorUpdate, ACTOR_EN_MINIFROG, CVAR, [](Actor* actor) {
        EnMinifrog* enMinifrog = (EnMinifrog*)actor;

        if (gPlayState->sceneId != SCENE_10YUKIYAMANOMURA2 || enMinifrog->frogIndex == 0) {
            return;
        }

        if (SavedFrogs() >= CVAR) {
            if (enMinifrog->actor.draw == NULL) {
                enMinifrog->actionFunc = EnMinifrog_SetupNextFrogInit;
                enMinifrog->actor.draw = EnMinifrog_Draw;
                enMinifrog->actor.update = EnMinifrog_Update;
            }
        } else if (enMinifrog->actor.draw == EnMinifrog_Draw) {
            if (enMinifrog->frogIndex == 1 && !CHECK_WEEKEVENTREG(WEEKEVENTREG_32_40)) {
                enMinifrog->actor.draw = NULL;
                enMinifrog->actor.update = EnMinifrog_UpdateMissingFrog;
            }
            if (enMinifrog->frogIndex == 2 && !CHECK_WEEKEVENTREG(WEEKEVENTREG_32_80)) {
                enMinifrog->actor.draw = NULL;
                enMinifrog->actor.update = EnMinifrog_UpdateMissingFrog;
            }
            if (enMinifrog->frogIndex == 3 && !CHECK_WEEKEVENTREG(WEEKEVENTREG_33_01)) {
                enMinifrog->actor.draw = NULL;
                enMinifrog->actor.update = EnMinifrog_UpdateMissingFrog;
            }
            if (enMinifrog->frogIndex == 4 && !CHECK_WEEKEVENTREG(WEEKEVENTREG_33_02)) {
                enMinifrog->actor.draw = NULL;
                enMinifrog->actor.update = EnMinifrog_UpdateMissingFrog;
            }
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterFrogChoirCount, { CVAR_NAME });
