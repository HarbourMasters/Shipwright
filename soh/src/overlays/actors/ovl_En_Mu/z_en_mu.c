/*
 * File: z_en_mu.c
 * Overlay: ovl_En_Mu
 * Description: Haggling townspeople
 */

#include "z_en_mu.h"
#include "objects/object_mu/object_mu.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3)

void EnMu_Init(Actor* thisx, GlobalContext* globalCtx);
void EnMu_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnMu_Update(Actor* thisx, GlobalContext* globalCtx);
void EnMu_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnMu_Pose(EnMu* this, GlobalContext* globalCtx);
s16 EnMu_CheckDialogState(GlobalContext* globalCtx, Actor* thisx);

static ColliderCylinderInit D_80AB0BD0 = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_NONE,
        OCELEM_ON,
    },
    { 100, 70, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 D_80AB0BFC = { 0, 0, 0, 0, MASS_IMMOVABLE };

const ActorInit En_Mu_InitVars = {
    ACTOR_EN_MU,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_MU,
    sizeof(EnMu),
    (ActorFunc)EnMu_Init,
    (ActorFunc)EnMu_Destroy,
    (ActorFunc)EnMu_Update,
    (ActorFunc)EnMu_Draw,
    NULL,
};

void EnMu_SetupAction(EnMu* this, EnMuActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void EnMu_Interact(EnMu* this, GlobalContext* globalCtx) {
    u8 textIdOffset[] = { 0x42, 0x43, 0x3F, 0x41, 0x3E };
    u8 bitmask[] = { 0x01, 0x02, 0x04, 0x08, 0x10 };
    u8 textFlags;
    s32 randomIndex;
    s32 i;

    textFlags = gSaveContext.eventInf[2] & 0x1F;
    gSaveContext.eventInf[2] &= ~0x1F;
    randomIndex = (globalCtx->state.frames + (s32)(Rand_ZeroOne() * 5.0f)) % 5;

    for (i = 0; i < 5; i++) {

        if (!(textFlags & bitmask[randomIndex])) {
            break;
        }

        randomIndex++;
        if (randomIndex >= 5) {
            randomIndex = 0;
        }
    }

    if (i == 5) {
        if (this->defFaceReaction == (textIdOffset[randomIndex] | 0x7000)) {
            randomIndex++;
            if (randomIndex >= 5) {
                randomIndex = 0;
            }
        }
        textFlags = 0;
    }

    textFlags |= bitmask[randomIndex];
    this->defFaceReaction = textIdOffset[randomIndex] | 0x7000;
    textFlags &= 0xFF;
    gSaveContext.eventInf[2] |= textFlags;
}

u16 EnMu_GetFaceReaction(GlobalContext* globalCtx, Actor* thisx) {
    EnMu* this = (EnMu*)thisx;
    u16 faceReaction = Text_GetFaceReaction(globalCtx, this->actor.params + 0x3A);

    if (faceReaction != 0) {
        return faceReaction;
    }
    return this->defFaceReaction;
}

s16 EnMu_CheckDialogState(GlobalContext* globalCtx, Actor* thisx) {
    EnMu* this = (EnMu*)thisx;

    switch (Message_GetState(&globalCtx->msgCtx)) {
        case TEXT_STATE_NONE:
        case TEXT_STATE_DONE_HAS_NEXT:
        case TEXT_STATE_DONE_FADING:
        case TEXT_STATE_CHOICE:
        case TEXT_STATE_EVENT:
        case TEXT_STATE_DONE:
        case TEXT_STATE_SONG_DEMO_DONE:
        case TEXT_STATE_8:
        case TEXT_STATE_9:
            return 1;
        case TEXT_STATE_CLOSING:
            EnMu_Interact(this, globalCtx);
            return 0;
        default:
            return 1;
    }
}

void EnMu_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnMu* this = (EnMu*)thisx;
    s32 pad;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 160.0f);
    SkelAnime_InitFlex(globalCtx, &this->skelAnime, &object_mu_Skel_004F70, &object_mu_Anim_0003F4, NULL, NULL, 0);
    Collider_InitCylinder(globalCtx, &this->collider);
    Collider_SetCylinder(globalCtx, &this->collider, &this->actor, &D_80AB0BD0);
    CollisionCheck_SetInfo2(&this->actor.colChkInfo, NULL, &D_80AB0BFC);
    this->actor.targetMode = 6;
    Actor_SetScale(&this->actor, 0.01f);
    EnMu_Interact(this, globalCtx);
    EnMu_SetupAction(this, EnMu_Pose);
}

void EnMu_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnMu* this = (EnMu*)thisx;

    SkelAnime_Free(&this->skelAnime, globalCtx);
}

void EnMu_Pose(EnMu* this, GlobalContext* globalCtx) {
    func_80034F54(globalCtx, this->unk_20A, this->unk_22A, 16);
}

void EnMu_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnMu* this = (EnMu*)thisx;
    s32 pad;
    f32 talkDist;
    Vec3s pos;

    pos.x = this->actor.world.pos.x;
    pos.y = this->actor.world.pos.y;
    pos.z = this->actor.world.pos.z;

    this->collider.dim.pos = pos;

    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &this->collider.base);
    SkelAnime_Update(&this->skelAnime);
    Actor_UpdateBgCheckInfo(globalCtx, &this->actor, 0.0f, 0.0f, 0.0f, 4);
    this->actionFunc(this, globalCtx);
    talkDist = this->collider.dim.radius + 30.0f;
    func_800343CC(globalCtx, &this->actor, &this->npcInfo.unk_00, talkDist, EnMu_GetFaceReaction,
                  EnMu_CheckDialogState);

    this->actor.focus.pos = this->actor.world.pos;
    this->actor.focus.pos.y += 60.0f;
}

s32 EnMu_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnMu* this = (EnMu*)thisx;

    if ((limbIndex == 5) || (limbIndex == 6) || (limbIndex == 7) || (limbIndex == 11) || (limbIndex == 12) ||
        (limbIndex == 13) || (limbIndex == 14)) {
        rot->y += Math_SinS(this->unk_20A[limbIndex]) * 200.0f;
        rot->z += Math_CosS(this->unk_22A[limbIndex]) * 200.0f;
    }
    return false;
}

void EnMu_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
}

Gfx* EnMu_DisplayListSetColor(GraphicsContext* gfxCtx, u8 r, u8 g, u8 b, u8 a) {
    Gfx* dlist;

    dlist = Graph_Alloc(gfxCtx, 2 * sizeof(Gfx));
    gDPSetEnvColor(dlist, r, g, b, a);
    gSPEndDisplayList(dlist + 1);
    return dlist;
}

void EnMu_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnMu* this = (EnMu*)thisx;
    Color_RGBA8 colors[2][5] = {
        { { 100, 130, 235, 0 }, { 160, 250, 60, 0 }, { 90, 60, 20, 0 }, { 30, 240, 200, 0 }, { 140, 70, 20, 0 } },
        { { 140, 70, 20, 0 }, { 30, 240, 200, 0 }, { 90, 60, 20, 0 }, { 160, 250, 60, 0 }, { 100, 130, 235, 0 } }
    };
    u8 segmentId[] = { 0x08, 0x09, 0x0A, 0x0B, 0x0C };
    s32 i;

    OPEN_DISPS(globalCtx->state.gfxCtx);
    Matrix_Translate(-1200.0f, 0.0f, -1400.0f, MTXMODE_APPLY);
    for (i = 0; i < 5; i++) {
        gSPSegment(POLY_OPA_DISP++, segmentId[i],
                   EnMu_DisplayListSetColor(globalCtx->state.gfxCtx, colors[this->actor.params][i].r,
                                            colors[this->actor.params][i].g, colors[this->actor.params][i].b,
                                            colors[this->actor.params][i].a));
    }
    SkelAnime_DrawFlexOpa(globalCtx, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnMu_OverrideLimbDraw, EnMu_PostLimbDraw, this);
    CLOSE_DISPS(globalCtx->state.gfxCtx);
}
