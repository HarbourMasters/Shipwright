#include <libultraship/bridge/consolevariablebridge.h>

#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/randomizer/SeedContext.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "overlays/actors/ovl_En_Ru1/z_en_ru1.h"
#include "assets/objects/object_ru1/object_ru1.h"

Actor* func_80AEB124(PlayState* play);
}

static constexpr const char* CVAR_SKIP_NAME = CVAR_ENHANCEMENT("TimeSavers.SkipMiscInteractions");
#define CVAR_SKIP_VALUE CVarGetInteger(CVAR_SKIP_NAME, IS_RANDO)

static void Ru1PreInit(void* actorRef, bool* should) {
    // Skips the Child Ruto introduction cutscene, where she drops down into the hole in Jabu-Jabu's Belly
    Flags_SetInfTable(INFTABLE_RUTO_IN_JJ_MEET_RUTO);
    Flags_SetInfTable(INFTABLE_RUTO_IN_JJ_TALK_FIRST_TIME);
    Flags_SetInfTable(INFTABLE_143);

    // Sets up Ruto in holes room as if carried there, thus skipping the basement portion of carrying her
    Flags_SetInfTable(INFTABLE_147);

    // Skips a short dialogue sequence where Ruto tells you to throw her to the Sapphire
    Flags_SetInfTable(INFTABLE_RUTO_IN_JJ_WANTS_TO_BE_TOSSED_TO_SAPPHIRE);
}

static void Ru1Init(void* actorRef) {
    if (IS_RANDO && RAND_GET_OPTION(RSK_SHUFFLE_SPEAK) && !Flags_GetRandomizerInf(RAND_INF_CAN_SPEAK_ZORA)) {
        return;
    }

    EnRu1* enRu1 = static_cast<EnRu1*>(actorRef);

    if (enRu1->action == 22) {
        enRu1->action = 27;
        enRu1->drawConfig = 1;
        enRu1->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
        Animation_Change(&enRu1->skelAnime, (AnimationHeader*)&gRutoChildSittingAnim, 1.0f, 0.0f,
                         Animation_GetLastFrame((void*)&gRutoChildSittingAnim), ANIMMODE_LOOP, 0.0f);
    }
}

static void RegisterSkipChildRutoInteractions() {
    // Skips several interactions with Child Ruto (see `Ru1PreInit` for details)
    COND_ID_HOOK(ShouldActorInit, ACTOR_EN_RU1, CVAR_SKIP_VALUE, Ru1PreInit);

    // Prevents Ruto from running to the Sapphire when she wants to be tossed to it, instead she just stands up and
    // waits for link to get closer
    COND_VB_SHOULD(VB_RUTO_RUN_TO_SAPPHIRE, CVAR_SKIP_VALUE, {
        EnRu1* enRu1 = va_arg(args, EnRu1*);
        DynaPolyActor* dynaPolyActor = va_arg(args, DynaPolyActor*);

        if (*should) {
            enRu1->unk_28C = (BgBdanObjects*)dynaPolyActor;
            Flags_SetInfTable(INFTABLE_145);
            Flags_SetSwitch(gPlayState, 0x02);
            Flags_SetSwitch(gPlayState, 0x1F);
            enRu1->action = 42;
            Animation_Change(&enRu1->skelAnime, (AnimationHeader*)&gRutoChildWait2Anim, 1.0f, 0,
                             Animation_GetLastFrame((void*)&gRutoChildWait2Anim), ANIMMODE_LOOP, -8.0f);
            // If BgBdan objects has set the camera setting to CAM_SET_NORMAL1 (2), don't reset the
            // camera setting to 1. This prevents the One Point Cutscene of Ruto getting lifted up
            // from getting queued up twice.
            if (enRu1->unk_28C->cameraSetting != 2) {
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

    // This overrides the behavior that causes Ruto to get upset at you before sitting back down again when
    // INFTABLE_RUTO_IN_JJ_TALK_FIRST_TIME is set
    COND_ID_HOOK(OnActorInit, ACTOR_EN_RU1, CVAR_SKIP_VALUE, Ru1Init);
}

static RegisterShipInitFunc initFunc(RegisterSkipChildRutoInteractions, { CVAR_SKIP_NAME, "IS_RANDO" });
