#include "ActorBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>

extern "C" {
#include "functions.h"
#include "variables.h"

#include "overlays/actors/ovl_En_Si/z_en_si.h"
}

void EnSi_DrawCustom(Actor* thisx, PlayState* play) {
    EnSi* enSi = (EnSi*)thisx;

    auto randoStaticCheck = Rando::StaticData::GetCheckFromFlag(FLAG_CYCL_SCENE_CHEST,
                                                                ENSI_GET_CHEST_FLAG(&enSi->actor), gPlayState->sceneId);
    if (randoStaticCheck.randoCheckId == RC_UNKNOWN) {
        return;
    }

    auto randoSaveCheck = RANDO_SAVE_CHECKS[randoStaticCheck.randoCheckId];

    Rando::DrawItem(Rando::ConvertItem(randoSaveCheck.randoItemId, randoStaticCheck.randoCheckId),
                    randoStaticCheck.randoCheckId, thisx);
}

void Rando::ActorBehavior::InitEnSiBehavior() {
    COND_ID_HOOK(OnActorInit, ACTOR_EN_SI, IS_RANDO, [](Actor* actor) {
        EnSi* enSi = (EnSi*)actor;

        auto randoStaticCheck = Rando::StaticData::GetCheckFromFlag(
            FLAG_CYCL_SCENE_CHEST, ENSI_GET_CHEST_FLAG(&enSi->actor), gPlayState->sceneId);
        if (randoStaticCheck.randoCheckId == RC_UNKNOWN) {
            return;
        }

        auto randoSaveCheck = RANDO_SAVE_CHECKS[randoStaticCheck.randoCheckId];

        if (!randoSaveCheck.shuffled) {
            return;
        }

        actor->draw = EnSi_DrawCustom;
    });

    COND_VB_SHOULD(VB_GIVE_ITEM_FROM_SI, IS_RANDO, {
        EnSi* enSi = va_arg(args, EnSi*);

        auto randoStaticCheck = Rando::StaticData::GetCheckFromFlag(
            FLAG_CYCL_SCENE_CHEST, ENSI_GET_CHEST_FLAG(&enSi->actor), gPlayState->sceneId);
        if (randoStaticCheck.randoCheckId == RC_UNKNOWN) {
            return;
        }

        auto randoSaveCheck = RANDO_SAVE_CHECKS[randoStaticCheck.randoCheckId];

        if (!randoSaveCheck.shuffled) {
            return;
        }

        *should = false;
    });
}
