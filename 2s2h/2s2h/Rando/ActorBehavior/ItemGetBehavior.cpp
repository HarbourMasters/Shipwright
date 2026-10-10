#include "ActorBehavior.h"

extern "C" {
#include "overlays/actors/ovl_En_Dno/z_en_dno.h"
#include "overlays/actors/ovl_Obj_Bean/z_obj_bean.h"
void func_809388A8(ObjBean* objBean, PlayState* play);
void func_80A730A0(EnDno* enDno, PlayState* play);
}

#define PLAYER_IN_GET_ITEM_STATE(player) ((player->stateFlags1 & PLAYER_STATE1_400) || player->actor.freezeTimer)

void Rando::ActorBehavior::InitItemGetBehavior() {
    COND_ID_HOOK(ShouldActorUpdate, ACTOR_OBJ_BEAN, IS_RANDO, [](Actor* actor, bool* should) {
        ObjBean* objBean = (ObjBean*)actor;
        // Bean is in moving state, and player is in Get Item state
        Player* player = GET_PLAYER(gPlayState);
        if (objBean->actionFunc == func_809388A8 && PLAYER_IN_GET_ITEM_STATE(player)) {
            *should = false;
        }
    });

    // Deku Butler
    COND_ID_HOOK(ShouldActorUpdate, ACTOR_EN_DNO, IS_RANDO, [](Actor* actor, bool* should) {
        EnDno* enDno = (EnDno*)actor;
        // Deku Butler is in race state, and player is in Get Item state
        Player* player = GET_PLAYER(gPlayState);
        if (enDno->actionFunc == func_80A730A0 && PLAYER_IN_GET_ITEM_STATE(player)) {
            *should = false;
        }
    });

    // Deku Butler race moving platform
    COND_ID_HOOK(ShouldActorUpdate, ACTOR_OBJ_DANPEILIFT, IS_RANDO, [](Actor* actor, bool* should) {
        Player* player = GET_PLAYER(gPlayState);
        if (PLAYER_IN_GET_ITEM_STATE(player)) {
            *should = false;
        }
    });

    // Deku Butler race door
    COND_ID_HOOK(ShouldActorUpdate, ACTOR_BG_CRACE_MOVEBG, IS_RANDO, [](Actor* actor, bool* should) {
        Player* player = GET_PLAYER(gPlayState);
        if (PLAYER_IN_GET_ITEM_STATE(player)) {
            *should = false;
        }
    });

    // Deku Scrub Playground platform
    COND_ID_HOOK(ShouldActorUpdate, ACTOR_OBJ_LUPYGAMELIFT, IS_RANDO, [](Actor* actor, bool* should) {
        Player* player = GET_PLAYER(gPlayState);
        if (PLAYER_IN_GET_ITEM_STATE(player)) {
            *should = false;
        }
    });

    // Deku Palace moving platform
    COND_ID_HOOK(ShouldActorUpdate, ACTOR_OBJ_RAILLIFT, IS_RANDO, [](Actor* actor, bool* should) {
        Player* player = GET_PLAYER(gPlayState);
        if (PLAYER_IN_GET_ITEM_STATE(player)) {
            *should = false;
        }
    });
}
