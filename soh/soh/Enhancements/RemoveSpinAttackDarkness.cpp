#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "src/overlays/actors/ovl_En_M_Thunder/z_en_m_thunder.h"
}

void Custom_EnMThunder_Update(Actor* thisx, PlayState* play) {
    EnMThunder* enMThunder = (EnMThunder*)thisx;
    f32 blueRadius;
    s32 redGreen;

    // If thunder effect doesn't exist (aka player doesn't have magic),
    // don't do anything.
    if (enMThunder->actionFunc == nullptr) {
        return;
    }

    enMThunder->actionFunc(enMThunder, play);
    // don't call this part, it's what makes the spin attack darkness happen
    // func_80A9F314(play, this->unk_1BC);
    blueRadius = enMThunder->unk_1AC;
    redGreen = (u32)(blueRadius * 255.0f) & 0xFF;
    Lights_PointNoGlowSetInfo(&enMThunder->lightInfo, enMThunder->actor.world.pos.x, enMThunder->actor.world.pos.y,
                              enMThunder->actor.world.pos.z, redGreen, redGreen, (u32)(blueRadius * 100.0f),
                              (s32)(blueRadius * 800.0f));
}

void OnEnMThunderInitReplaceUpdateWithCustom(void* thunder) {
    EnMThunder* enMThunder = (EnMThunder*)thunder;
    enMThunder->actor.update = Custom_EnMThunder_Update;
}

#define CVAR_REMOVESPINATTACKDARKNESS_NAME CVAR_ENHANCEMENT("RemoveSpinAttackDarkness")
#define CVAR_REMOVESPINATTACKDARKNESS_DEFAULT 0
#define CVAR_REMOVESPINATTACKDARKNESS_VALUE \
    CVarGetInteger(CVAR_REMOVESPINATTACKDARKNESS_NAME, CVAR_REMOVESPINATTACKDARKNESS_DEFAULT)

void RegisterCustomEnMThunderUpdate() {
    COND_ID_HOOK(OnActorInit, ACTOR_EN_M_THUNDER,
                 CVAR_REMOVESPINATTACKDARKNESS_VALUE != CVAR_REMOVESPINATTACKDARKNESS_DEFAULT,
                 OnEnMThunderInitReplaceUpdateWithCustom);
}

static RegisterShipInitFunc initFunc(RegisterCustomEnMThunderUpdate, { CVAR_REMOVESPINATTACKDARKNESS_NAME });
