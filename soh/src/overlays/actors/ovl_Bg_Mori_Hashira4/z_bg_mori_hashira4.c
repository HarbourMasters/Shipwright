/*
 * File: z_bg_mori_hashira4
 * Overlay: ovl_Bg_Mori_Hashira4
 * Description: Forest Temple gates and rotating pillars
 */

#include "z_bg_mori_hashira4.h"
#include "objects/object_mori_objects/object_mori_objects.h"

#define FLAGS ACTOR_FLAG_4

void BgMoriHashira4_Init(Actor* thisx, PlayState* play);
void BgMoriHashira4_Destroy(Actor* thisx, PlayState* play);
void BgMoriHashira4_Update(Actor* thisx, PlayState* play);
void BgMoriHashira4_Draw(Actor* thisx, PlayState* play);

void BgMoriHashira4_SetupWaitForMoriTex(BgMoriHashira4* this);
void BgMoriHashira4_WaitForMoriTex(BgMoriHashira4* this, PlayState* play);
void BgMoriHashira4_SetupPillarsRotate(BgMoriHashira4* this);
void BgMoriHashira4_PillarsRotate(BgMoriHashira4* this, PlayState* play);
void BgMoriHashira4_GateWait(BgMoriHashira4* this, PlayState* play);
void BgMoriHashira4_GateOpen(BgMoriHashira4* this, PlayState* play);

const ActorInit Bg_Mori_Hashira4_InitVars = {
    ACTOR_BG_MORI_HASHIRA4,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_MORI_OBJECTS,
    sizeof(BgMoriHashira4),
    (ActorFunc)BgMoriHashira4_Init,
    (ActorFunc)BgMoriHashira4_Destroy,
    (ActorFunc)BgMoriHashira4_Update,
    NULL,
    NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(uncullZoneForward, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 700, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 1000, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 1000, ICHAIN_STOP),
};

static Gfx* sDisplayLists[] = { gMoriHashiraPlatformsDL, gMoriHashiraGateDL };

static s16 sUnkTimer; // seems to be unused

void BgMoriHashira4_SetupAction(BgMoriHashira4* this, BgMoriHashira4ActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void BgMoriHashira4_InitDynaPoly(BgMoriHashira4* this, PlayState* play, CollisionHeader* collision,
                                 s32 moveFlag) {
    s32 pad;
    CollisionHeader* colHeader;
    s32 pad2;

    colHeader = NULL;
    DynaPolyActor_Init(&this->dyna, moveFlag);
    CollisionHeader_GetVirtual(collision, &colHeader);
    this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);

    if (this->dyna.bgId == BG_ACTOR_MAX) {
        // "Warning : move BG login failed"
        osSyncPrintf("Warning : move BG 登録失敗(%s %d)(name %d)(arg_data 0x%04x)\n", __FILE__, __LINE__,
                     this->dyna.actor.id, this->dyna.actor.params);
    }
}

void BgMoriHashira4_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    BgMoriHashira4* this = (BgMoriHashira4*)thisx;

    this->switchFlag = (this->dyna.actor.params >> 8) & 0x3F;
    this->dyna.actor.params &= 0xFF;

    if (this->dyna.actor.params == 0) {
        BgMoriHashira4_InitDynaPoly(this, play, &gMoriHashira1Col, DPM_UNK3);
    } else {
        BgMoriHashira4_InitDynaPoly(this, play, &gMoriHashira2Col, DPM_UNK);
    }
    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    this->moriTexObjIndex = Object_GetIndex(&play->objectCtx, OBJECT_MORI_TEX);
    if (this->moriTexObjIndex < 0) {
        Actor_Kill(&this->dyna.actor);
        // "Bank danger!"
        osSyncPrintf("Error : バンク危険！(arg_data 0x%04x)(%s %d)\n", this->dyna.actor.params,
                     __FILE__, __LINE__);
        return;
    }
    if ((this->dyna.actor.params != 0) && Flags_GetSwitch(play, this->switchFlag)) {
        Actor_Kill(&this->dyna.actor);
        return;
    }
    Actor_SetFocus(&this->dyna.actor, 50.0f);
    BgMoriHashira4_SetupWaitForMoriTex(this);
    // "(4 pillars of the Forest Temple) Bank danger"
    osSyncPrintf("(森の神殿 ４本柱)(arg_data 0x%04x)\n", this->dyna.actor.params);
    sUnkTimer = 0;
}

void BgMoriHashira4_Destroy(Actor* thisx, PlayState* play) {
    s32 pad;
    BgMoriHashira4* this = (BgMoriHashira4*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void BgMoriHashira4_SetupWaitForMoriTex(BgMoriHashira4* this) {
    BgMoriHashira4_SetupAction(this, BgMoriHashira4_WaitForMoriTex);
}

void BgMoriHashira4_WaitForMoriTex(BgMoriHashira4* this, PlayState* play) {
    if (Object_IsLoaded(&play->objectCtx, this->moriTexObjIndex)) {
        this->gateTimer = 0;
        if (this->dyna.actor.params == 0) {
            BgMoriHashira4_SetupPillarsRotate(this);
        } else {
            BgMoriHashira4_SetupAction(this, BgMoriHashira4_GateWait);
        }
        this->dyna.actor.draw = BgMoriHashira4_Draw;
    }
}

void BgMoriHashira4_SetupPillarsRotate(BgMoriHashira4* this) {
    BgMoriHashira4_SetupAction(this, BgMoriHashira4_PillarsRotate);
}

void BgMoriHashira4_PillarsRotate(BgMoriHashira4* this, PlayState* play) {
    this->dyna.actor.shape.rot.y = this->dyna.actor.world.rot.y += 0x96;
    Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EV_ROLL_STAND_2 - SFX_FLAG);
}

void BgMoriHashira4_GateWait(BgMoriHashira4* this, PlayState* play) {
    if (Flags_GetSwitch(play, this->switchFlag) || (this->gateTimer != 0)) {
        this->gateTimer++;
        if (this->gateTimer > 30) {
            Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EV_METALDOOR_OPEN);
            BgMoriHashira4_SetupAction(this, BgMoriHashira4_GateOpen);
            OnePointCutscene_Init(play, 6010, 20, &this->dyna.actor, MAIN_CAM);
            sUnkTimer++;
        }
    }
}

void BgMoriHashira4_GateOpen(BgMoriHashira4* this, PlayState* play) {
    if (Math_StepToF(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y + 120.0f, 10.0f)) {
        Actor_Kill(&this->dyna.actor);
    }
}

void BgMoriHashira4_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    BgMoriHashira4* this = (BgMoriHashira4*)thisx;

    if (this->actionFunc != NULL) {
        this->actionFunc(this, play);
    }
}

void BgMoriHashira4_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    BgMoriHashira4* this = (BgMoriHashira4*)thisx;

    OPEN_DISPS(play->state.gfxCtx);
    Gfx_SetupDL_25Opa(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, play->objectCtx.status[this->moriTexObjIndex].segment);

    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    gSPDisplayList(POLY_OPA_DISP++, sDisplayLists[this->dyna.actor.params]);
    CLOSE_DISPS(play->state.gfxCtx);
}
