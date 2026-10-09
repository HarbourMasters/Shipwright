#include "ActorBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>
#include "CustomItem/CustomItem.h"

extern "C" {
#include "variables.h"
#include "overlays/actors/ovl_En_Gamelupy/z_en_gamelupy.h"
}

#define IS_AT(xx, zz) (actor->home.pos.x == xx && actor->home.pos.z == zz)

RandoCheckId IdentifyGameLupy(Actor* actor) {
    RandoCheckId randoCheckId = RC_UNKNOWN;

    switch (CURRENT_DAY) {
        case 1: {
            if (IS_AT(-100.0f, 150.0f)) {
                randoCheckId = RC_DEKU_PLAYGROUND_DAY_1_RUPEE_01;
            } else if (IS_AT(100.0f, -50.0f)) {
                randoCheckId = RC_DEKU_PLAYGROUND_DAY_1_RUPEE_02;
            } else if (IS_AT(-200.0f, -250.0f)) {
                randoCheckId = RC_DEKU_PLAYGROUND_DAY_1_RUPEE_03;
            } else if (IS_AT(200.0f, 350.0f)) {
                randoCheckId = RC_DEKU_PLAYGROUND_DAY_1_RUPEE_04;
            } else if (IS_AT(-500.0f, 350.0f)) {
                randoCheckId = RC_DEKU_PLAYGROUND_DAY_1_RUPEE_05;
            } else if (IS_AT(500.0f, -250.0f)) {
                randoCheckId = RC_DEKU_PLAYGROUND_DAY_1_RUPEE_06;
            }
        } break;
        case 2: {
            if (IS_AT(-100.0f, -50.0f)) {
                randoCheckId = RC_DEKU_PLAYGROUND_DAY_2_RUPEE_01;
            } else if (IS_AT(300.0f, -250.0f)) {
                randoCheckId = RC_DEKU_PLAYGROUND_DAY_2_RUPEE_02;
            } else if (IS_AT(500.0f, 550.0f)) {
                randoCheckId = RC_DEKU_PLAYGROUND_DAY_2_RUPEE_03;
            } else if (IS_AT(100.0f, 150.0f)) {
                randoCheckId = RC_DEKU_PLAYGROUND_DAY_2_RUPEE_04;
            } else if (IS_AT(-300.0f, 350.0f)) {
                randoCheckId = RC_DEKU_PLAYGROUND_DAY_2_RUPEE_05;
            } else if (IS_AT(-500.0f, -450.0f)) {
                randoCheckId = RC_DEKU_PLAYGROUND_DAY_2_RUPEE_06;
            }
        } break;
        case 3: {
            if (IS_AT(-100.0f, -50.0f)) {
                randoCheckId = RC_DEKU_PLAYGROUND_DAY_3_RUPEE_01;
            } else if (IS_AT(100.0f, 150.0f)) {
                randoCheckId = RC_DEKU_PLAYGROUND_DAY_3_RUPEE_02;
            } else if (IS_AT(-300.0f, 250.0f)) {
                randoCheckId = RC_DEKU_PLAYGROUND_DAY_3_RUPEE_03;
            } else if (IS_AT(300.0f, -150.0f)) {
                randoCheckId = RC_DEKU_PLAYGROUND_DAY_3_RUPEE_04;
            } else if (IS_AT(-500.0f, -450.0f)) {
                randoCheckId = RC_DEKU_PLAYGROUND_DAY_3_RUPEE_05;
            } else if (IS_AT(500.0f, 550.0f)) {
                randoCheckId = RC_DEKU_PLAYGROUND_DAY_3_RUPEE_06;
            }
        } break;
    }

    return randoCheckId;
}

void Gamelupy_RandoDrawFunc(Actor* actor, PlayState* play) {
    auto randoSaveCheck = RANDO_SAVE_CHECKS[(RandoCheckId)actor->home.rot.x];

    Matrix_Scale(20.0f, 20.0f, 20.0f, MTXMODE_APPLY);
    Rando::DrawItem(Rando::ConvertItem(randoSaveCheck.randoItemId, (RandoCheckId)actor->home.rot.x),
                    (RandoCheckId)actor->home.rot.x, actor);
}

void Rando::ActorBehavior::InitEnGamelupyBehavior() {
    COND_ID_HOOK(OnActorInit, ACTOR_EN_GAMELUPY, IS_RANDO, [](Actor* actor) {
        if (gPlayState->sceneId != SCENE_DEKUTES) {
            return;
        }

        RandoCheckId randoCheckId = IdentifyGameLupy(actor);
        if (randoCheckId == RC_UNKNOWN) {
            return;
        }

        auto& randoSaveCheck = RANDO_SAVE_CHECKS[randoCheckId];
        if (randoSaveCheck.cycleObtained || !randoSaveCheck.shuffled) {
            return;
        }

        actor->home.rot.x = randoCheckId;
        actor->draw = Gamelupy_RandoDrawFunc;
    });

    COND_VB_SHOULD(VB_COLLECT_PLAYGROUND_RUPEE, IS_RANDO, {
        if (!*should) {
            return;
        }

        EnGamelupy* gameLupy = va_arg(args, EnGamelupy*);
        if (gameLupy->actor.home.rot.x == RC_UNKNOWN) {
            return;
        }

        auto& randoSaveCheck = RANDO_SAVE_CHECKS[(RandoCheckId)gameLupy->actor.home.rot.x];
        randoSaveCheck.eligible = true;
        *should = false;
        *gameLupy->minigameScore += ENGAMELUPY_POINTS;
        Actor_Kill(&gameLupy->actor);
    });
}
