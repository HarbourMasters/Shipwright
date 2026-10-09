#include "ActorBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>
#include "CustomItem/CustomItem.h"

extern "C" {
#include "variables.h"
#include "src/overlays/actors/ovl_En_Fish2/z_en_fish2.h"
void Player_StartTalking(PlayState* play, Actor* actor);
}

void Rando::ActorBehavior::InitEnFish2Behavior() {
    COND_VB_SHOULD(VB_FISH2_SPAWN_HEART_PIECE, IS_RANDO, {
        EnFish2* refActor = va_arg(args, EnFish2*);
        if (refActor->actor.id != ACTOR_EN_FISH2) {
            return;
        }

        RandoCheckId randoCheckId =
            Rando::StaticData::Checks[RC_GREAT_BAY_COAST_MARINE_LAB_FISH_PIECE_OF_HEART].randoCheckId;

        CustomItem::Spawn(
            221.624f, 100.0f, -182.550f, 0, CustomItem::KILL_ON_TOUCH, randoCheckId,
            [](Actor* actor, PlayState* play) {
                RandoSaveCheck& randoSaveCheck = RANDO_SAVE_CHECKS[CUSTOM_ITEM_PARAM];
                randoSaveCheck.eligible = true;
            },
            [](Actor* actor, PlayState* play) {
                auto& randoSaveCheck = RANDO_SAVE_CHECKS[CUSTOM_ITEM_PARAM];
                RandoItemId randoItemId = Rando::ConvertItem(randoSaveCheck.randoItemId);
                Matrix_Scale(30.0f, 30.0f, 30.0f, MTXMODE_APPLY);
                Rando::DrawItem(Rando::ConvertItem(randoSaveCheck.randoItemId, (RandoCheckId)CUSTOM_ITEM_PARAM),
                                (RandoCheckId)CUSTOM_ITEM_PARAM, actor);
            });

        CLEAR_WEEKEVENTREG(WEEKEVENTREG_81_10);
        CLEAR_WEEKEVENTREG(WEEKEVENTREG_81_20);
        CLEAR_WEEKEVENTREG(WEEKEVENTREG_81_40);
        CLEAR_WEEKEVENTREG(WEEKEVENTREG_81_80);
        CLEAR_WEEKEVENTREG(WEEKEVENTREG_82_01);
        CLEAR_WEEKEVENTREG(WEEKEVENTREG_82_02);

        *should = false;
    });
}