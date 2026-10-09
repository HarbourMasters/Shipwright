#include "ActorBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>
#include "CustomItem/CustomItem.h"

extern "C" {
#include "variables.h"
#include "overlays/actors/ovl_En_Ruppecrow/z_en_ruppecrow.h"
}

void Rando::ActorBehavior::InitEnRuppecrowBehavior() {
    COND_VB_SHOULD(VB_GUAY_DROP_RUPEE, IS_RANDO, {
        EnRuppecrow* refActor = va_arg(args, EnRuppecrow*);
        uint32_t rupeeIndex = refActor->rupeeIndex;

        RandoCheckId randoCheckId = (RandoCheckId)(RC_TERMINA_FIELD_GUAY_RUPEE_DROP_01 + rupeeIndex);

        if (RANDO_SAVE_CHECKS[randoCheckId].cycleObtained || !RANDO_SAVE_CHECKS[randoCheckId].shuffled) {
            return;
        }

        EnItem00* rupee = CustomItem::Spawn(
            refActor->actor.world.pos.x, refActor->actor.world.pos.y, refActor->actor.world.pos.z, 0,
            CustomItem::KILL_ON_TOUCH | CustomItem::TOSS_ON_SPAWN, randoCheckId,
            [](Actor* actor, PlayState* play) { RANDO_SAVE_CHECKS[CUSTOM_ITEM_PARAM].eligible = true; },
            [](Actor* actor, PlayState* play) {
                auto& randoSaveCheck = RANDO_SAVE_CHECKS[CUSTOM_ITEM_PARAM];
                Matrix_Scale(30.0f, 30.0f, 30.0f, MTXMODE_APPLY);
                Rando::DrawItem(Rando::ConvertItem(randoSaveCheck.randoItemId, (RandoCheckId)CUSTOM_ITEM_PARAM),
                                (RandoCheckId)CUSTOM_ITEM_PARAM, actor);
            });

        // Apply rupee drop heavy gravity
        rupee->actor.gravity = -5.0f;
        rupee->actor.velocity.y = 0.0f;

        Actor_PlaySfx(&refActor->actor, NA_SE_EV_RUPY_FALL);

        *should = false;
    });
}
