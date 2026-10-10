#include "ActorBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>

#include "2s2h/CustomItem/CustomItem.h"
#include "2s2h/Rando/Rando.h"
#include "2s2h/ShipInit.hpp"
#include "2s2h/ObjectExtension/ActorListIndex.h"
#include "2s2h/ObjectExtension/ObjectExtension.h"
#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"
#include "assets/2s2h_assets.h"
#include <spdlog/spdlog.h>

extern "C" {
#include "variables.h"
#include "overlays/actors/ovl_Obj_Mure/z_obj_mure.h"

void ObjMure_CulledState(ObjMure*, PlayState*);
void ObjMure_ActiveState(ObjMure*, PlayState*);
void EnButte_Draw(Actor* thisx, PlayState* play);
void EnButte_Update(Actor* thisx, PlayState* play);
}

typedef enum {
    BUTTERFLY_CATCH_IDLE,
    BUTTERFLY_CATCH_APPROACHING,
} ButterflyCatchPhase;

struct ButterflyCatchState {
    ButterflyCatchPhase phase = BUTTERFLY_CATCH_IDLE;
    // Our own tracked facing, independent of shape.rot.y. Vanilla's own turning (func_8091C6B4, run every
    // frame via EnButte_Update) keeps nudging shape.rot.y towards its own ambient-flight heading, so smoothing
    // from the live shape.rot.y value fights that every frame and never settles. Smoothing our own value and
    // writing it into shape.rot.y afterwards avoids that tug-of-war entirely.
    s16 facingYaw = 0;
};
static ObjectExtension::Register<ButterflyCatchState> ButterflyCatchStateRegister;

struct ObjMureLastActionFunc {
    void* value = nullptr;
};
static ObjectExtension::Register<ObjMureLastActionFunc> ObjMureLastActionFuncRegister;

template <typename T> T& GetOrCreateExtension(const void* object) {
    T* existing = ObjectExtension::GetInstance().Get<T>(object);
    if (existing != nullptr) {
        return *existing;
    }
    ObjectExtension::GetInstance().Set<T>(object, T{});
    return *ObjectExtension::GetInstance().Get<T>(object);
}

// clang-format off
std::map<std::tuple<s16, u8, u8>, std::tuple<RandoCheckId, u8>> butterflyMap = {
    { { SCENE_00KEIKOKU, 0, 171 }, { RC_TERMINA_FIELD_BUTTERFLY_01, 2 } },
    { { SCENE_30GYOSON, 0, 64 }, { RC_GREAT_BAY_COAST_BUTTERFLY_01, 2 } },
    { { SCENE_30GYOSON, 0, 133 }, { RC_GREAT_BAY_COAST_BUTTERFLY_01, 2 } }, // Post-Great Bay Temple clear (4 spawn here, only first 2 are checks)
    { { SCENE_10YUKIYAMANOMURA2, 0, 32 }, { RC_MOUNTAIN_VILLAGE_SPRING_BUTTERFLY_01, 4 } },
    { { SCENE_10YUKIYAMANOMURA2, 0, 38 }, { RC_MOUNTAIN_VILLAGE_SPRING_BUTTERFLY_01, 4 } }, // First-time entrance cutscene layer
    { { SCENE_10YUKIYAMANOMURA2, 0, 33 }, { RC_MOUNTAIN_VILLAGE_SPRING_BUTTERFLY_05, 5 } },
    { { SCENE_10YUKIYAMANOMURA2, 0, 39 }, { RC_MOUNTAIN_VILLAGE_SPRING_BUTTERFLY_05, 5 } }, // First-time entrance cutscene layer
    { { SCENE_KAKUSIANA, 0, 2 }, { RC_TERMINA_FIELD_GOSSIP_STONE_GROTTO_3_BUTTERFLY_01, 2 } },
    { { SCENE_KAKUSIANA, 10, 6 }, { RC_GREAT_BAY_COAST_COW_GROTTO_BUTTERFLY_01, 3 } }, // Adjusted for Termina Field cow grotto below
    { { SCENE_KAKUSIANA, 12, 4 }, { RC_DEKU_PALACE_BEAN_SALESMAN_GROTTO_BUTTERFLY_01, 4 } },
};
// clang-format on

void DrawButterfly(Actor* actor, PlayState* play) {
    EnButte_Draw(actor, play);

    OPEN_DISPS(gPlayState->state.gfxCtx);
    RandoCheckId randoCheckId = Rando::ActorBehavior::GetObjectRandoCheckId(actor);
    Matrix_Scale(8.0f, 8.0f, 8.0f, MTXMODE_APPLY);
    Rando::DrawItem(Rando::ConvertItem(RANDO_SAVE_CHECKS[randoCheckId].randoItemId, randoCheckId), randoCheckId, actor);
    CLOSE_DISPS(gPlayState->state.gfxCtx);
}

// Runs the actual vanilla update (flight AI, wing animation, gravity, collider sync) every frame so a butterfly
// looks and moves exactly like normal until it's lured in, then eases its position toward the stick tip on top.
// The moment it gets close enough it instantly grants the check and goes back to idle, where vanilla's own
// home-seeking behavior (func_8091C794) carries it back home on its own.
void EnButte_UpdateRandoCheck(Actor* thisx, PlayState* play) {
    EnButte_Update(thisx, play);

    RandoCheckId randoCheckId = Rando::ActorBehavior::GetObjectRandoCheckId(thisx);
    RandoSaveCheck& randoSaveCheck = RANDO_SAVE_CHECKS[randoCheckId];
    if (randoSaveCheck.cycleObtained) {
        thisx->draw = EnButte_Draw;
        thisx->update = EnButte_Update;
        return;
    }

    ButterflyCatchState& state = GetOrCreateExtension<ButterflyCatchState>(thisx);
    Player* player = GET_PLAYER(play);
    bool holdingStick = player->heldItemAction == PLAYER_IA_DEKU_STICK;

    if (state.phase == BUTTERFLY_CATCH_IDLE) {
        if (randoSaveCheck.eligible || !holdingStick || thisx->xzDistToPlayer > 240.0f ||
            fabsf(thisx->playerHeightRel) > 400.0f) {
            return;
        }
        state.phase = BUTTERFLY_CATCH_APPROACHING;
        state.facingYaw = thisx->shape.rot.y;
    } else if (!holdingStick) {
        state.phase = BUTTERFLY_CATCH_IDLE;
        return;
    }

    Vec3f target = player->meleeWeaponInfo[0].tip;

    f32 dist = Math_Vec3f_DistXYZ(&thisx->world.pos, &target);
    f32 step = CLAMP(dist * 0.06f, 1.0f, 2.2f);
    Math_Vec3f_StepTo(&thisx->world.pos, &target, step);

    s16 yawTowardsTarget = BINANG_ROT180(Math_Vec3f_Yaw(&thisx->world.pos, &target));
    Math_SmoothStepToS(&state.facingYaw, yawTowardsTarget, 6, 0x7D0, 1);
    thisx->shape.rot.y = state.facingYaw;

    if (dist <= 10.0f) {
        randoSaveCheck.eligible = true;
        state.phase = BUTTERFLY_CATCH_IDLE;
    }
}

void Rando::ActorBehavior::InitEnButteBehavior() {
    bool shouldRegister = IS_RANDO && RANDO_SAVE_OPTIONS[RO_SHUFFLE_BUTTERFLIES] != RO_GENERIC_OFF;

    COND_ID_HOOK(OnActorUpdate, ACTOR_OBJ_MURE, shouldRegister, [](Actor* actor) {
        ObjMure* objMure = (ObjMure*)actor;
        ObjMureLastActionFunc& lastActionFuncState = GetOrCreateExtension<ObjMureLastActionFunc>(actor);

        if (lastActionFuncState.value == (void*)ObjMure_CulledState &&
            (void*)objMure->actionFunc == (void*)ObjMure_ActiveState) {
            auto it =
                butterflyMap.find({ gPlayState->sceneId, gPlayState->roomCtx.curRoom.num, GetActorListIndex(actor) });
            if (it != butterflyMap.end()) {
                RandoCheckId startingRandoCheckId = std::get<0>(it->second);
                u8 butterflyCount = std::get<1>(it->second);
                // Adjust for Termina Field cow grotto
                if (gPlayState->sceneId == SCENE_KAKUSIANA && gPlayState->roomCtx.curRoom.num == 10 &&
                    gSaveContext.respawn[RESPAWN_MODE_UNK_3].data == 31) {
                    startingRandoCheckId =
                        static_cast<RandoCheckId>(startingRandoCheckId + (RC_TERMINA_FIELD_COW_GROTTO_BUTTERFLY_01 -
                                                                          RC_GREAT_BAY_COAST_COW_GROTTO_BUTTERFLY_01));
                }
                for (u8 i = 0; i < butterflyCount; i++) {
                    RandoCheckId randoCheckId = static_cast<RandoCheckId>(static_cast<int>(startingRandoCheckId) + i);
                    if (!RANDO_SAVE_CHECKS[randoCheckId].shuffled || RANDO_SAVE_CHECKS[randoCheckId].cycleObtained ||
                        objMure->children[i] == nullptr) {
                        continue;
                    }
                    Actor* butterfly = objMure->children[i];
                    SetObjectRandoCheckId(butterfly, randoCheckId);
                    butterfly->draw = DrawButterfly;
                    butterfly->update = EnButte_UpdateRandoCheck;
                    // Clear the "golden butterfly" params bit so vanilla's own stick-attraction never triggers
                    butterfly->params &= ~1;
                }
            }
        }
        lastActionFuncState.value = (void*)objMure->actionFunc;
    });
}
