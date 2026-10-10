/*
 * File: z_bg_tobira01.c
 * Overlay: ovl_Bg_Tobira01
 * Description: Gate to Goron Shrine
 */

#include "z_bg_tobira01.h"
#include "objects/object_spot11_obj/object_spot11_obj.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void BgTobira01_Init(Actor* thisx, PlayState* play);
void BgTobira01_Destroy(Actor* thisx, PlayState* play);
void BgTobira01_Update(Actor* thisx, PlayState* play);
void BgTobira01_Draw(Actor* thisx, PlayState* play);

ActorProfile Bg_Tobira01_Profile = {
    /**/ ACTOR_BG_TOBIRA01,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_SPOT11_OBJ,
    /**/ sizeof(BgTobira01),
    /**/ BgTobira01_Init,
    /**/ BgTobira01_Destroy,
    /**/ BgTobira01_Update,
    /**/ BgTobira01_Draw,
};

void BgTobira01_Action(BgTobira01* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 csId = this->dyna.actor.csId;
    s16 prevPosYTick;

    if (this->cutsceneRequested) {
        if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
            CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
        } else if (CutsceneManager_IsNext(csId)) {
            CutsceneManager_StartWithPlayerCs(csId, &this->dyna.actor);
            SET_WEEKEVENTREG(WEEKEVENTREG_GATEKEEPER_OPENED_GORON_SHRINE);
            this->cutsceneRequested = false;
        } else {
            CutsceneManager_Queue(csId);
        }
    } else if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_GATEKEEPER_OPENED_GORON_SHRINE) && (this->posYTick == 0) &&
               (play->actorCtx.playerImpact.timer != 0) &&
               (play->actorCtx.playerImpact.type == PLAYER_IMPACT_GORON_GROUND_POUND) &&
               (SurfaceType_GetSceneExitIndex(&play->colCtx, player->actor.floorPoly, player->actor.floorBgId) == 6)) {
        this->cutsceneRequested = true;
        this->unk16C = 0;
    }

    prevPosYTick = this->posYTick;
    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_GATEKEEPER_OPENED_GORON_SHRINE)) {
        this->posYTick++;
    } else {
        this->posYTick--;
    }

    this->posYTick = CLAMP(this->posYTick, 0, 60);
    if (prevPosYTick != this->posYTick) {
        Actor_PlaySfx(&this->dyna.actor, NA_SE_EV_STONEDOOR_OPEN_S - SFX_FLAG);
        this->dyna.actor.world.pos.y = this->posY = (this->posYTick * (5.0f / 3.0f)) + this->dyna.actor.home.pos.y;
        this->timer = 180;
    }

    if (!(player->stateFlags1 & PLAYER_STATE1_TALKING) &&
        CHECK_WEEKEVENTREG(WEEKEVENTREG_GATEKEEPER_OPENED_GORON_SHRINE) && (DECR(this->timer) == 0)) {
        CLEAR_WEEKEVENTREG(WEEKEVENTREG_GATEKEEPER_OPENED_GORON_SHRINE);
    }
}

void BgTobira01_Init(Actor* thisx, PlayState* play) {
    BgTobira01* this = (BgTobira01*)thisx;

    DynaPolyActor_Init(&this->dyna, DYNA_TRANSFORM_POS);
    DynaPolyActor_LoadMesh(play, &this->dyna, &gGoronDoorCol);
    CLEAR_WEEKEVENTREG(WEEKEVENTREG_GATEKEEPER_OPENED_GORON_SHRINE);
    Actor_SetScale(&this->dyna.actor, 1.0f);
    this->timer = gSaveContext.save.isNight;
    this->posYTick = 0;
    this->actionFunc = BgTobira01_Action;
}

void BgTobira01_Destroy(Actor* thisx, PlayState* play) {
    BgTobira01* this = (BgTobira01*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void BgTobira01_Update(Actor* thisx, PlayState* play) {
    BgTobira01* this = (BgTobira01*)thisx;

    this->actionFunc(this, play);
}

void BgTobira01_Draw(Actor* thisx, PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, gGoronDoorDL);

    CLOSE_DISPS(play->state.gfxCtx);
}
