/*
 * File: z_bg_sst_floor.c
 * Overlay: ovl_Bg_Sst_Floor
 * Description: Bongo Bongo's drum
 */

#include "z_bg_sst_floor.h"
#include "objects/object_sst/object_sst.h"

#define FLAGS (ACTOR_FLAG_4 | ACTOR_FLAG_5)

void BgSstFloor_Init(BgSstFloor* this, GlobalContext* globalCtx);
void BgSstFloor_Destroy(BgSstFloor* this, GlobalContext* globalCtx);
void BgSstFloor_Update(BgSstFloor* this, GlobalContext* globalCtx);
void BgSstFloor_Draw(BgSstFloor* this, GlobalContext* globalCtx);

static s32 sUnkValues[] = { 0, 0, 0 }; // Unused, probably a zero vector

const ActorInit Bg_Sst_Floor_InitVars = {
    ACTOR_BG_SST_FLOOR,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_SST,
    sizeof(BgSstFloor),
    (ActorFunc)BgSstFloor_Init,
    (ActorFunc)BgSstFloor_Destroy,
    (ActorFunc)BgSstFloor_Update,
    (ActorFunc)BgSstFloor_Draw,
    NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale.x, 100, ICHAIN_STOP),
};

void BgSstFloor_Init(BgSstFloor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    BgSstFloor* this = (BgSstFloor*)thisx;
    CollisionHeader* colHeader = NULL;

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    DynaPolyActor_Init(&this->dyna, DPM_PLAYER);
    CollisionHeader_GetVirtual(&gBongoDrumCol, &colHeader);
    this->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, &this->dyna.actor, colHeader);
}

void BgSstFloor_Destroy(BgSstFloor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    BgSstFloor* this = (BgSstFloor*)thisx;

    DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, this->dyna.bgId);
}

void BgSstFloor_Update(BgSstFloor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    BgSstFloor* this = (BgSstFloor*)thisx;
    Player* player = GET_PLAYER(globalCtx);
    CollisionHeader* colHeader = SEGMENTED_TO_VIRTUAL(&gBongoDrumCol);

    colHeader = ResourceMgr_LoadColByName(colHeader);

    colHeader->vtxList = SEGMENTED_TO_VIRTUAL(colHeader->vtxList);

    if (func_80043590(&this->dyna) && (this->dyna.actor.yDistToPlayer < 1000.0f)) {
        Camera_ChangeSetting(globalCtx->cameraPtrs[MAIN_CAM], CAM_SET_BOSS_BONGO);
    } else {
        Camera_ChangeSetting(globalCtx->cameraPtrs[MAIN_CAM], CAM_SET_DUNGEON0);
    }

    if (func_8004356C(&this->dyna) && (player->fallDistance > 1000.0f)) {
        this->dyna.actor.params = 1;
        Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EN_SHADEST_TAIKO_HIGH);
    }

    if (this->dyna.actor.params == BONGOFLOOR_HIT) {
        Actor* item00 = globalCtx->actorCtx.actorLists[ACTORCAT_MISC].head;
        f32 distFromRim;
        f32 xzDist;

        this->drumAmp = 80;
        this->dyna.actor.params = BONGOFLOOR_REST;
        this->drumPhase = 28;

        if (func_8004356C(&this->dyna) && !(player->stateFlags1 & 0x6000)) {
            distFromRim = 600.0f - this->dyna.actor.xzDistToPlayer;
            if (distFromRim > 0.0f) {
                if (distFromRim > 350.0f) {
                    distFromRim = 350.0f;
                }
                player->actor.bgCheckFlags &= ~1;
                player->actor.velocity.y = 9.0f * distFromRim * (1.0f / 350.0f);
            }
        }

        while (item00 != NULL) {
            if ((item00->id == ACTOR_EN_ITEM00) && (item00->world.pos.y == 0.0f)) {
                xzDist = Actor_WorldDistXZToActor(&this->dyna.actor, item00);
                distFromRim = 600.0f - xzDist;
                if (xzDist < 600.0f) {
                    if (distFromRim > 350.0f) {
                        distFromRim = 350.0f;
                    }
                    item00->bgCheckFlags &= ~3;
                    item00->velocity.y = 9.0f * distFromRim * (1.0f / 350.0f);
                }
            }
            item00 = item00->next;
        }
    }
    this->drumHeight = sinf(this->drumPhase * (M_PI / 2)) * (-this->drumAmp);
    Math_StepToS(&this->drumAmp, 0, 5);

    colHeader->vtxList[1].y = colHeader->vtxList[0].y = colHeader->vtxList[2].y = colHeader->vtxList[3].y =
        colHeader->vtxList[4].y = colHeader->vtxList[7].y = colHeader->vtxList[9].y = colHeader->vtxList[11].y =
            colHeader->vtxList[13].y = this->dyna.actor.home.pos.y + this->drumHeight;

    if (this->drumPhase != 0) {
        this->drumPhase--;
    }
    func_8003EE6C(globalCtx, &globalCtx->colCtx.dyna);
}

void BgSstFloor_Draw(BgSstFloor* thisx, GlobalContext* globalCtx) {
    BgSstFloor* this = (BgSstFloor*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx);
    func_80093D18(globalCtx->state.gfxCtx);
    Matrix_Scale(1.0f, this->drumHeight * -0.0025f, 1.0f, MTXMODE_APPLY);

    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(globalCtx->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    gSPDisplayList(POLY_OPA_DISP++, gBongoDrumDL);

    CLOSE_DISPS(globalCtx->state.gfxCtx);
}
