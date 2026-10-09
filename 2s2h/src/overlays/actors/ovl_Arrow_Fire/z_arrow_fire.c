/*
 * File: z_arrow_fire.c
 * Overlay: ovl_Arrow_Fire
 * Description: Fire Arrow
 */

#include "z_arrow_fire.h"
#include "overlays/actors/ovl_En_Arrow/z_en_arrow.h"

#include "2s2h/BenGui/CosmeticEditor.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_UPDATE_DURING_OCARINA)

void ArrowFire_Init(Actor* thisx, PlayState* play);
void ArrowFire_Destroy(Actor* thisx, PlayState* play);
void ArrowFire_Update(Actor* thisx, PlayState* play);
void ArrowFire_Draw(Actor* thisx, PlayState* play);

void FireArrow_ChargeAndWait(ArrowFire* this, PlayState* play);
void FireArrow_Fly(ArrowFire* this, PlayState* play);

#include "overlays/ovl_Arrow_Fire/ovl_Arrow_Fire.h"

ActorProfile Arrow_Fire_Profile = {
    /**/ ACTOR_ARROW_FIRE,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(ArrowFire),
    /**/ ArrowFire_Init,
    /**/ ArrowFire_Destroy,
    /**/ ArrowFire_Update,
    /**/ ArrowFire_Draw,
};

static ColliderQuadInit sQuadInit = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_PLAYER,
        AC_NONE,
        OC1_NONE,
        OC2_TYPE_PLAYER,
        COLSHAPE_QUAD,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x08000000, 0x00, 0x02 },
        { 0x00000000, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_NONE,
        OCELEM_NONE,
    },
    { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(cullingVolumeDistance, 2000, ICHAIN_STOP),
};

static s32 sBssPad;

void ArrowFire_SetupAction(ArrowFire* this, ArrowFireActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void ArrowFire_Init(Actor* thisx, PlayState* play) {
    ArrowFire* this = (ArrowFire*)thisx;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    this->radius = 0;
    this->height = 1.0f;
    ArrowFire_SetupAction(this, FireArrow_ChargeAndWait);
    Actor_SetScale(&this->actor, 0.01f);
    this->alpha = 160;
    this->timer = 0;
    this->screenFillIntensity = 0.0f;
    Collider_InitAndSetQuad(play, &this->collider1, &this->actor, &sQuadInit);
    Collider_InitAndSetQuad(play, &this->collider2, &this->actor, &sQuadInit);
}

void ArrowFire_Destroy(Actor* thisx, PlayState* play) {
    ArrowFire* this = (ArrowFire*)thisx;

    Magic_Reset(play);
    Collider_DestroyQuad(play, &this->collider1);
    Collider_DestroyQuad(play, &this->collider2);
}

void FireArrow_ChargeAndWait(ArrowFire* this, PlayState* play) {
    EnArrow* arrow = (EnArrow*)this->actor.parent;

    if ((arrow == NULL) || (arrow->actor.update == NULL)) {
        Actor_Kill(&this->actor);
        return;
    }
    if (this->radius < 10) {
        this->radius++;
    }

    this->actor.world.pos = arrow->actor.world.pos;
    this->actor.shape.rot = arrow->actor.shape.rot;

    Actor_PlaySfx_Flagged(&this->actor, NA_SE_PL_ARROW_CHARGE_FIRE - SFX_FLAG);

    // if arrow has no parent, player has fired the arrow
    if (arrow->actor.parent == NULL) {
        this->firedPos = this->actor.world.pos;
        this->radius = 10;
        ArrowFire_SetupAction(this, FireArrow_Fly);
        this->alpha = 255;
    }
}

void FireArrow_Lerp(Vec3f* firedPos, Vec3f* pos, f32 scale) {
    VEC3F_LERPIMPDST(firedPos, firedPos, pos, scale);
}

void FireArrow_Hit(ArrowFire* this, PlayState* play) {
    f32 offset;
    f32 scale;
    u16 timer;

    if (this->actor.projectedW < 50.0f) {
        scale = 10.0f;
    } else if (this->actor.projectedW > 950.0f) {
        scale = 310.0f;
    } else {
        scale = this->actor.projectedW;
        scale = ((scale - 50.0f) * (1.0f / 3.0f)) + 10.0f;
    }

    timer = this->timer;
    if (timer != 0) {
        this->timer--;
        if (this->timer >= 8) {
            offset = (this->timer - 8) * (1.0f / 24.0f);
            offset = SQ(offset);
            this->radius = (((1.0f - offset) * scale) + 10.0f);
            this->height = F32_LERPIMP(this->height, 2.0f, 0.1f);
            if (this->timer < 16) {
                this->alpha = (this->timer * 35) - 280;
            }
        }
    }
    if (this->timer >= 9) {
        if (this->screenFillIntensity < 1.0f) {
            this->screenFillIntensity += 0.25f;
        }
    } else {
        if (this->screenFillIntensity > 0.0f) {
            this->screenFillIntensity -= 0.125f;
        }
    }
    if (this->timer < 8) {
        this->alpha = 0;
    }
    if (this->timer == 0) {
        this->timer = 255;
        Actor_Kill(&this->actor);
        return;
    }
    if (this->timer >= 13) {
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider1.base);
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider2.base);
    }
}

void FireArrow_Fly(ArrowFire* this, PlayState* play) {
    EnArrow* arrow = (EnArrow*)this->actor.parent;
    s32 pad;
    s32 pad2;

    if ((arrow == NULL) || (arrow->actor.update == NULL)) {
        Actor_Kill(&this->actor);
        return;
    }

    this->actor.world.pos = arrow->actor.world.pos;
    this->actor.shape.rot = arrow->actor.shape.rot;

    this->height = Math_Vec3f_DistXYZ(&this->firedPos, &this->actor.world.pos) * (1.0f / 24.0f);

    if (this->height < 1.0f) {
        this->height = 1.0f;
    }

    FireArrow_Lerp(&this->firedPos, &this->actor.world.pos, 0.05f);
    if (arrow->unk_261 & 1) {
        Actor_PlaySfx(&this->actor, NA_SE_IT_EXPLOSION_FRAME);
        ArrowFire_SetupAction(this, FireArrow_Hit);
        this->timer = 32;
        this->alpha = 255;
        return;
    }
    if (arrow->unk_260 < 34) {
        if (this->alpha < 35) {
            Actor_Kill(&this->actor);
            return;
        }
        this->alpha -= 25;
    }
}

void ArrowFire_Update(Actor* thisx, PlayState* play) {
    ArrowFire* this = (ArrowFire*)thisx;

    if ((play->msgCtx.msgMode == MSGMODE_E) || (play->msgCtx.msgMode == MSGMODE_SONG_PLAYED)) {
        Actor_Kill(&this->actor);
        return;
    }
    this->actionFunc(this, play);
}

void FireArrow_SetQuadVerticies(ArrowFire* this) {
    static Vec3f D_80922284 = { 100.0f, 700.0f, 0.0f };
    static Vec3f D_80922290 = { 400.0f, 27.0f, 0.0f };
    static Vec3f D_8092229C = { -100.0f, 700.0f, 0.0f };
    static Vec3f D_809222A8 = { -400, 27.0f, 0.0f };
    static Vec3f D_809222B4 = { 0.0f, 700.0f, 100.0f };
    static Vec3f D_809222C0 = { 0.0f, 27.0f, 400.0f };
    static Vec3f D_809222CC = { 0.0f, 700.0f, -100.0f };
    static Vec3f D_809222D8 = { 0.0f, 27.0f, -400.0f };
    Vec3f sp44;
    Vec3f sp38;
    Vec3f sp2C;
    Vec3f sp20;

    Matrix_MultVec3f(&D_80922284, &sp44);
    Matrix_MultVec3f(&D_80922290, &sp38);
    Matrix_MultVec3f(&D_8092229C, &sp2C);
    Matrix_MultVec3f(&D_809222A8, &sp20);
    Collider_SetQuadVertices(&this->collider1, &sp44, &sp38, &sp2C, &sp20);
    Matrix_MultVec3f(&D_809222B4, &sp44);
    Matrix_MultVec3f(&D_809222C0, &sp38);
    Matrix_MultVec3f(&D_809222CC, &sp2C);
    Matrix_MultVec3f(&D_809222D8, &sp20);
    Collider_SetQuadVertices(&this->collider2, &sp44, &sp38, &sp2C, &sp20);
}

void ArrowFire_Draw(Actor* thisx, PlayState* play) {
    EnArrow* arrow;
    ArrowFire* this = (ArrowFire*)thisx;
    u32 frames = play->state.frames;
    s32 pad;

    arrow = (EnArrow*)this->actor.parent;

    if ((arrow != NULL) && (arrow->actor.update != NULL) && (this->timer < 255)) {
        Actor* transform = (arrow->unk_261 & 2) ? &this->actor : &arrow->actor;

        OPEN_DISPS(play->state.gfxCtx);

        Matrix_Translate(transform->world.pos.x, transform->world.pos.y, transform->world.pos.z, MTXMODE_NEW);
        Matrix_RotateYS(transform->shape.rot.y, MTXMODE_APPLY);
        Matrix_RotateXS(transform->shape.rot.x, MTXMODE_APPLY);
        Matrix_RotateZS(transform->shape.rot.z, MTXMODE_APPLY);

        Matrix_Scale(0.01f, 0.01f, 0.01f, MTXMODE_APPLY);
        if (this->screenFillIntensity > 0.0f) {
            POLY_XLU_DISP = Gfx_SetupDL57(POLY_XLU_DISP);

            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, (s32)(this->screenFillIntensity * 40.0f) & 0xFF, 0, 0,
                            (s32)(150.0f * this->screenFillIntensity) & 0xFF);

            gDPSetAlphaDither(POLY_XLU_DISP++, G_AD_DISABLE);
            gDPSetColorDither(POLY_XLU_DISP++, G_CD_DISABLE);

            gSPDisplayList(POLY_XLU_DISP++, D_0E000000_TO_SEGMENTED(fillRect));
        }
        Gfx_SetupDL25_Xlu(play->state.gfxCtx);

        gDPSetPrimColorOverride(POLY_XLU_DISP++, 0x80, 0x80, 255, 200, 0, this->alpha,
                                COSMETIC_ID("Effects.FireArrowPrim"));
        gDPSetEnvColorOverride(POLY_XLU_DISP++, 255, 0, 0, 128, COSMETIC_ID("Effects.FireArrowSec"));

        Matrix_RotateZYX(0x4000, 0, 0, MTXMODE_APPLY);

        if (this->timer != 0) {
            Matrix_Translate(0.0f, 0.0f, 0.0f, MTXMODE_APPLY);
        } else {
            Matrix_Translate(0.0f, 1500.0f, 0.0f, MTXMODE_APPLY);
        }

        Matrix_Scale(this->radius * 0.2f, this->height * 4.0f, this->radius * 0.2f, MTXMODE_APPLY);
        Matrix_Translate(0.0f, -700.0f, 0.0f, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);

        FireArrow_SetQuadVerticies(this);
        gSPDisplayList(POLY_XLU_DISP++, gFireArrowMaterialDL);
        gSPDisplayList(POLY_XLU_DISP++,
                       Gfx_TwoTexScrollEx(play->state.gfxCtx, 0, 255 - ((frames * 2) % 256), 0, 64, 32, 1,
                                          255 - (frames % 256), 511 - ((frames * 10) % 512), 64, 64, -2, 0, -1, -10));
        gSPDisplayList(POLY_XLU_DISP++, gFireArrowModelDL);

        CLOSE_DISPS(play->state.gfxCtx);
    }
}
