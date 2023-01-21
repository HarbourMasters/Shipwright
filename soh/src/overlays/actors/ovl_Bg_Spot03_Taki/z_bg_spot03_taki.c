/*
 * File: z_bg_spot03_taki.c
 * Overlay: ovl_Bg_Spot03_Taki
 * Description: Zora's River Waterfall
 */

#include "z_bg_spot03_taki.h"
#include "objects/object_spot03_object/object_spot03_object.h"

#define FLAGS (ACTOR_FLAG_4 | ACTOR_FLAG_5)

void BgSpot03Taki_Init(Actor* thisx, PlayState* play);
void BgSpot03Taki_Destroy(Actor* thisx, PlayState* play);
void BgSpot03Taki_Update(Actor* thisx, PlayState* play);
void BgSpot03Taki_Draw(Actor* thisx, PlayState* play);

void func_808ADEF0(BgSpot03Taki* this, PlayState* play);

const ActorInit Bg_Spot03_Taki_InitVars = {
    ACTOR_BG_SPOT03_TAKI,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_SPOT03_OBJECT,
    sizeof(BgSpot03Taki),
    (ActorFunc)BgSpot03Taki_Init,
    (ActorFunc)BgSpot03Taki_Destroy,
    (ActorFunc)BgSpot03Taki_Update,
    (ActorFunc)BgSpot03Taki_Draw,
    NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void BgSpot03Taki_ApplyOpeningAlpha(BgSpot03Taki* this, s32 bufferIndex) {
    s32 i;
    Vtx* vtx = (bufferIndex == 0) ? SEGMENTED_TO_VIRTUAL(object_spot03_object_Vtx_000800)
                                  : SEGMENTED_TO_VIRTUAL(object_spot03_object_Vtx_000990);
    
    vtx = ResourceMgr_LoadVtxByName(vtx);

    for (i = 0; i < 5; i++) {
        vtx[i + 10].v.cn[3] = this->openingAlpha;
    }
}

void BgSpot03Taki_Init(Actor* thisx, PlayState* play) {
    BgSpot03Taki* this = (BgSpot03Taki*)thisx;
    s16 pad;
    CollisionHeader* colHeader = NULL;

    this->switchFlag = (this->dyna.actor.params & 0x3F);
    DynaPolyActor_Init(&this->dyna, DPM_UNK);
    CollisionHeader_GetVirtual(&object_spot03_object_Col_000C98, &colHeader);
    this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    this->bufferIndex = 0;
    this->openingAlpha = 255.0f;
    BgSpot03Taki_ApplyOpeningAlpha(this, 0);
    BgSpot03Taki_ApplyOpeningAlpha(this, 1);
    this->actionFunc = func_808ADEF0;
}

void BgSpot03Taki_Destroy(Actor* thisx, PlayState* play) {
    BgSpot03Taki* this = (BgSpot03Taki*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void func_808ADEF0(BgSpot03Taki* this, PlayState* play) {
    if (this->state == WATERFALL_CLOSED) {
        if (Flags_GetSwitch(play, this->switchFlag)) {
            this->state = WATERFALL_OPENING_ANIMATED;
            this->timer = 40;
            OnePointCutscene_Init(play, 4100, -99, NULL, MAIN_CAM);
        }
    } else if (this->state == WATERFALL_OPENING_IDLE) {
        this->timer--;
        if (this->timer < 0) {
            this->state = WATERFALL_OPENING_ANIMATED;
        }
    } else if (this->state == WATERFALL_OPENING_ANIMATED) {
        if (this->openingAlpha > 0) {
            this->openingAlpha -= 5;
            if (this->openingAlpha <= 0.0f) {
                func_8003EBF8(play, &play->colCtx.dyna, this->dyna.bgId);
                this->timer = 400;
                this->state = WATERFALL_OPENED;
                this->openingAlpha = 0;
            }
        }
    } else if (this->state == WATERFALL_OPENED) {
        this->timer--;
        if (this->timer < 0) {
            this->state = WATERFALL_CLOSING;
        }
    } else if (this->state == WATERFALL_CLOSING) {
        if (this->openingAlpha < 255.0f) {
            this->openingAlpha += 5.0f;
            if (this->openingAlpha >= 255.0f) {
                func_8003EC50(play, &play->colCtx.dyna, this->dyna.bgId);
                this->state = WATERFALL_CLOSED;
                this->openingAlpha = 255.0f;
                Flags_UnsetSwitch(play, this->switchFlag);
            }
        }
    }

    BgSpot03Taki_ApplyOpeningAlpha(this, this->bufferIndex);
}

void BgSpot03Taki_Update(Actor* thisx, PlayState* play) {
    BgSpot03Taki* this = (BgSpot03Taki*)thisx;

    this->actionFunc(this, play);
}

void BgSpot03Taki_Draw(Actor* thisx, PlayState* play) {
    BgSpot03Taki* this = (BgSpot03Taki*)thisx;
    s32 pad;
    u32 gameplayFrames;

    OPEN_DISPS(play->state.gfxCtx);

    gameplayFrames = play->gameplayFrames;

    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    Gfx_SetupDL_25Xlu(play->state.gfxCtx);

    gSPSegment(
        POLY_XLU_DISP++, 0x08,
        Gfx_TwoTexScroll(play->state.gfxCtx, 0, 0, gameplayFrames * 5, 64, 64, 1, 0, gameplayFrames * 5, 64, 64));

    gSPDisplayList(POLY_XLU_DISP++, object_spot03_object_DL_000B20);

    if (this->bufferIndex == 0) {
        gSPVertex(POLY_XLU_DISP++, object_spot03_object_Vtx_000800, 25, 0);
    } else {
        gSPVertex(POLY_XLU_DISP++, object_spot03_object_Vtx_000990, 25, 0);
    }

    gSPDisplayList(POLY_XLU_DISP++, object_spot03_object_DL_000BC0);

    gSPSegment(POLY_XLU_DISP++, 0x08,
               Gfx_TwoTexScroll(play->state.gfxCtx, 0, gameplayFrames * 1, gameplayFrames * 3, 64, 64, 1,
                                -gameplayFrames, gameplayFrames * 3, 64, 64));

    gSPDisplayList(POLY_XLU_DISP++, object_spot03_object_DL_001580);

    CLOSE_DISPS(play->state.gfxCtx);

    this->bufferIndex = this->bufferIndex == 0;

    if (this->state >= WATERFALL_OPENING_IDLE && this->state <= WATERFALL_OPENED) {
        Audio_PlaySoundWaterfall(&this->dyna.actor.projectedPos, 0.5f);
    } else {
        Audio_PlaySoundWaterfall(&this->dyna.actor.projectedPos, 1.0f);
    }
}
