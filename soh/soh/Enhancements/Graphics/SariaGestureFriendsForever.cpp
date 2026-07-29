#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "src/overlays/actors/ovl_En_Sa/z_en_sa.h"
extern "C" PlayState* gPlayState;

void EnSa_ChangeAnim(EnSa* enSa, s32 index);
}

static constexpr int32_t CVAR_SARIA_GESTURE_DEFAULT = 0;
#define CVAR_SARIA_GESTURE_NAME CVAR_ENHANCEMENT("SariaGestureFriendsForever")
#define CVAR_SARIA_GESTURE_VALUE CVarGetInteger(CVAR_SARIA_GESTURE_NAME, CVAR_SARIA_GESTURE_DEFAULT)

// Resets Saria back to her usual swaying animation; otherwise, she stands frozen
static void EnSa_ResetAnimation(EnSa* enSa) {
    static bool sAnimationStarted = false;

    if (enSa->unk_20B == 7 && enSa->unk_20A == 2 && !sAnimationStarted) {
        sAnimationStarted = true;
    }

    if (sAnimationStarted && Animation_OnFrame(&enSa->skelAnime, enSa->skelAnime.endFrame)) {
        EnSa_ChangeAnim(enSa, 4);
        sAnimationStarted = false;
    }
}

static void RegisterSariaGestureFriendsForever() {
    COND_VB_SHOULD(VB_SARIA_GESTURE, CVAR_SARIA_GESTURE_VALUE, {
        bool isInHouse = gPlayState->sceneNum == SCENE_SARIAS_HOUSE;
        *should = *should || isInHouse;

        if (isInHouse) {
            EnSa* enSa = va_arg(args, EnSa*);
            EnSa_ResetAnimation(enSa);
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterSariaGestureFriendsForever, { CVAR_SARIA_GESTURE_NAME });
