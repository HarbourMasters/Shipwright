#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/OTRGlobals.h"

extern "C" {
#include "overlays/actors/ovl_En_Ru1/z_en_ru1.h"
#include "assets/objects/object_ru1/object_ru1.h"

Actor* func_80AEB124(PlayState* play);
}

void SkipChildRutoInteractions_Register() {
    // Skips the Child Ruto introduction cutscene, where she drops down into the hole in Jabu-Jabu's Belly
    REGISTER_VB_SHOULD(VB_PLAY_CHILD_RUTO_INTRO, {
        EnRu1* enRu1 = va_arg(args, EnRu1*);

        if (!CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipMiscInteractions"), IS_RANDO)) {
            return;
        }

        Flags_SetInfTable(INFTABLE_RUTO_IN_JJ_MEET_RUTO);
        Flags_SetInfTable(INFTABLE_RUTO_IN_JJ_TALK_FIRST_TIME);
        Flags_SetInfTable(INFTABLE_143);
        enRu1->drawConfig = 1;
        enRu1->actor.world.pos.x = 127.0f;
        enRu1->actor.world.pos.y = -340.0f;
        enRu1->actor.world.pos.z = -3041.0f;
        enRu1->actor.shape.rot.y = enRu1->actor.world.rot.y = -5098;

        if (*should) {
            Animation_Change(&enRu1->skelAnime, (AnimationHeader*)&gRutoChildTurnAroundAnim, 1.0f, 0,
                        Animation_GetLastFrame((void*)&gRutoChildTurnAroundAnim), ANIMMODE_ONCE, -8.0f);
            enRu1->action = 10;
        }

        *should = false;
    });

    // Skips a short dialogue sequence where Ruto tells you to throw her to the Sapphire
    REGISTER_VB_SHOULD(VB_RUTO_WANT_TO_BE_TOSSED_TO_SAPPHIRE, {
        if (!CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipMiscInteractions"), IS_RANDO)) {
            return;
        }

        if (*should) {
            Flags_SetInfTable(INFTABLE_RUTO_IN_JJ_WANTS_TO_BE_TOSSED_TO_SAPPHIRE);
            *should = false;
        }
    });

    // Prevents Ruto from running to the Sapphire when she wants to be tossed to it, instead she just stands up and waits for link to get closer
    REGISTER_VB_SHOULD(VB_RUTO_RUN_TO_SAPPHIRE, {
        EnRu1* enRu1 = va_arg(args, EnRu1*);
        DynaPolyActor* dynaPolyActor = va_arg(args, DynaPolyActor*);

        if (!CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipMiscInteractions"), IS_RANDO)) {
            return;
        }

        if (*should) {
            enRu1->unk_28C = (BgBdanObjects*)dynaPolyActor;
            Flags_SetInfTable(INFTABLE_145);
            Flags_SetSwitch(gPlayState, 0x02);
            Flags_SetSwitch(gPlayState, 0x1F);
            enRu1->action = 42;
            Animation_Change(&enRu1->skelAnime, (AnimationHeader*)&gRutoChildWait2Anim, 1.0f, 0, 
                        Animation_GetLastFrame((void*)&gRutoChildWait2Anim), ANIMMODE_LOOP, -8.0f);
            // If we aren't skipping one point cutscenes and BgBdan objects has set the camera setting
            // to CAM_SET_NORMAL1 (2), don't reset the camera setting to 1. This prevents the One Point
            // Cutscene of Ruto getting lifted up from getting queued up twice.
            if (CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.OnePoint"), IS_RANDO) || enRu1->unk_28C->cameraSetting != 2) {
                enRu1->unk_28C->cameraSetting = 1;
            }
            Actor* sapphire = func_80AEB124(gPlayState);
            if (sapphire != NULL) {
                Actor_Kill(sapphire);
            }
            enRu1->actor.room = gPlayState->roomCtx.curRoom.num;
            *should = false;
        }
    });

    // This overrides the behavior that causes Ruto to get upset at you before sitting back down again when INFTABLE_RUTO_IN_JJ_TALK_FIRST_TIME is set
    GameInteractor::Instance->RegisterGameHookForID<GameInteractor::OnActorInit>(ACTOR_EN_RU1, [](void* actorRef) {
        EnRu1* enRu1 = static_cast<EnRu1*>(actorRef);
        if (!CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipMiscInteractions"), IS_RANDO)) {
            return;
        }

        if (enRu1->action == 22) {
            enRu1->action = 27;
            enRu1->drawConfig = 1;
            enRu1->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY;
            Animation_Change(&enRu1->skelAnime, (AnimationHeader*)&gRutoChildSittingAnim, 1.0f, 0.0f,
                         Animation_GetLastFrame((void*)&gRutoChildSittingAnim), ANIMMODE_LOOP, 0.0f);
        }
    });
}
