/*
 * File: z_eff_ss_sbn.c
 * Overlay: ovl_Effect_Ss_Sbn
 * Description: Popped Deku Bubble
 *
 * Note: The gfx field is used to store the colPoly, not anything gfx related
 * This is not a problem since gfx is already void*
 */

#include "z_eff_ss_sbn.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define rScrollStep regs[0]
#define rScroll regs[1]
#define rTexIndex regs[2]
#define rAlpha regs[3]
#define rReg4 regs[4]
#define rReg4Step regs[5]
#define rRotAngle regs[6]
#define rScale regs[7]
#define rReg8 regs[8]

#define PARAMS ((EffectSsSbnInitParams*)initParamsx)

u32 EffectSsSbn_Init(PlayState* play, u32 index, EffectSs* this, void* initParamsx);
void EffectSsSbn_Update(PlayState* play, u32 index, EffectSs* this);
void EffectSsSbn_DrawSliding(PlayState* play, u32 index, EffectSs* this);
void EffectSsSbn_Draw(PlayState* play, u32 index, EffectSs* this);

EffectSsProfile Effect_Ss_Sbn_Profile = {
    EFFECT_SS_SBN,
    EffectSsSbn_Init,
};

static TexturePtr sSlidingTextures[] = {
    gEffPoppedDekuBubbleSliding1Tex,  gEffPoppedDekuBubbleSliding2Tex,  gEffPoppedDekuBubbleSliding3Tex,
    gEffPoppedDekuBubbleSliding4Tex,  gEffPoppedDekuBubbleSliding5Tex,  gEffPoppedDekuBubbleSliding6Tex,
    gEffPoppedDekuBubbleSliding7Tex,  gEffPoppedDekuBubbleSliding8Tex,  gEffPoppedDekuBubbleSliding9Tex,
    gEffPoppedDekuBubbleSliding10Tex, gEffPoppedDekuBubbleSliding11Tex, gEffPoppedDekuBubbleSliding12Tex,
};

static TexturePtr sTextures[] = {
    gEffPoppedDekuBubble1Tex, gEffPoppedDekuBubble2Tex, gEffPoppedDekuBubble3Tex,
    gEffPoppedDekuBubble4Tex, gEffPoppedDekuBubble5Tex,
};

u32 EffectSsSbn_Init(PlayState* play, u32 index, EffectSs* this, void* initParamsx) {
    EffectSsSbnInitParams* initParams = PARAMS;
    CollisionPoly* colPoly;
    MtxF mtx;
    Vec3f normal;
    Vec3f colPolyVec;
    Vec3f bubbleVec;
    f32 angle;
    f32 opposite;

    Math_Vec3f_Copy(&this->pos, &initParams->pos);
    Math_Vec3f_Copy(&this->velocity, &gZeroVec3f);
    Math_Vec3f_Copy(&this->accel, &gZeroVec3f);
    this->flags = 0;

    this->gfx = initParams->colPoly;

    this->rScroll = 0;
    this->rScrollStep = 7;
    this->rTexIndex = 0;
    this->rAlpha = 200;
    this->rReg4 = 250;
    this->rReg4Step = 30;

    this->rScale = Rand_CenteredFloat(100.0f) + (initParams->scale * 120.0f);
    if (this->rScale < 600) {
        this->rScale = 600;
    } else if (this->rScale > 1500) {
        this->rScale = 1500;
    }

    this->rReg8 = 1000;
    this->rRotAngle = 0;
    this->life = 93;
    this->draw = EffectSsSbn_DrawSliding;
    this->update = EffectSsSbn_Update;

    if (this->gfx != NULL) {
        colPoly = this->gfx;
        func_800C0094(colPoly, this->pos.x, this->pos.y, this->pos.z, &mtx);
        normal.x = COLPOLY_GET_NORMAL(colPoly->normal.x);
        normal.y = COLPOLY_GET_NORMAL(colPoly->normal.y);
        normal.z = COLPOLY_GET_NORMAL(colPoly->normal.z);
        if ((normal.y > 0.95f) || (normal.y < -0.95f)) {
            this->draw = EffectSsSbn_Draw;
            return 1;
        }
        if (normal.y > 0.05f) {
            colPolyVec.x = normal.x * 10.0f;
            colPolyVec.y = (-(SQ(normal.x) + SQ(normal.z)) * 10.0f) / normal.y;
            colPolyVec.z = normal.z * 10.0f;
        } else if (normal.y < -0.05f) {
            colPolyVec.x = -normal.x * 10.0f;
            colPolyVec.y = (-(SQ(normal.x) + SQ(normal.z)) * 10.0f) / -normal.y;
            colPolyVec.z = -normal.z * 10.0f;
        } else {
            colPolyVec.x = 0.0f;
            colPolyVec.y = -10.0f;
            colPolyVec.z = 0.0f;
        }
        bubbleVec.x = -mtx.mf[2][0] * 10.0f;
        bubbleVec.y = -mtx.mf[2][1] * 10.0f;
        bubbleVec.z = -mtx.mf[2][2] * 10.0f;
        Math3D_CosOut(&colPolyVec, &bubbleVec, &angle);

        opposite = (SQ(angle) >= 1.0f) ? 0.0f : sqrtf(1.0f - SQ(angle));
        if (((mtx.mf[0][0] * colPolyVec.x) + (mtx.mf[0][1] * colPolyVec.y) + (mtx.mf[0][2] * colPolyVec.z)) < 0.0f) {
            this->rRotAngle = Math_Atan2S_XY(angle, opposite);
        } else {
            this->rRotAngle = -Math_Atan2S_XY(angle, opposite);
        }
    }

    return 1;
}

void EffectSsSbn_DrawSliding(PlayState* play, u32 index, EffectSs* this) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    MtxF mtx;
    f32 scale = this->rScale * 0.001f;
    s32 pad;

    func_800C0094(this->gfx, this->pos.x, this->pos.y, this->pos.z, &mtx);
    SkinMatrix_MulYRotation(&mtx, this->rRotAngle);
    Matrix_Mult(&mtx, MTXMODE_NEW);
    Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
    Matrix_Scale(0.05f, 0.05f, 0.05f, MTXMODE_APPLY);

    OPEN_DISPS(gfxCtx);

    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx);
    Gfx_SetupDL25_Xlu(gfxCtx);
    gDPSetRenderMode(POLY_XLU_DISP++, G_RM_FOG_SHADE_A, G_RM_ZB_XLU_DECAL2);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, 255, 255, 255, this->rAlpha);

    if (this->rTexIndex < ARRAY_COUNT(sSlidingTextures)) {
        {
            TexturePtr tex = sSlidingTextures[this->rTexIndex];

            gSPSegment(POLY_XLU_DISP++, 0x08, Lib_SegmentedToVirtual(tex));
        }
        this->rTexIndex++;
        this->rScroll += this->rScrollStep;
    } else {
        gSPSegment(POLY_XLU_DISP++, 0x08, Lib_SegmentedToVirtual(gEffPoppedDekuBubbleSliding12Tex));
        if ((this->rScrollStep >= 2) && ((play->state.frames % 2) == 0)) {
            this->rScrollStep--;
        }
        this->rScroll += this->rScrollStep;
    }
    gSPSegment(POLY_XLU_DISP++, 0x09,
               Gfx_TwoTexScrollEx(play->state.gfxCtx, 0, 0, 0, 0x20, 0x40, 1, 0, this->rScroll, 0x20, 0x20, 0, 0, 0,
                                  this->rScrollStep));
    gSPDisplayList(POLY_XLU_DISP++, gEffPoppedDekuBubbleSlidingDL);

    CLOSE_DISPS(gfxCtx);
}

void EffectSsSbn_Draw(PlayState* play, u32 index, EffectSs* this) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    MtxF mtx;
    f32 scale = this->rScale * 0.001f;

    func_800C0094(this->gfx, this->pos.x, this->pos.y, this->pos.z, &mtx);
    SkinMatrix_MulYRotation(&mtx, this->rRotAngle);
    Matrix_Mult(&mtx, MTXMODE_NEW);
    Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
    Matrix_Scale(0.05f, 0.05f, 0.05f, MTXMODE_APPLY);

    OPEN_DISPS(gfxCtx);

    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx);
    Gfx_SetupDL25_Xlu(gfxCtx);
    gDPSetRenderMode(POLY_XLU_DISP++, G_RM_FOG_SHADE_A, G_RM_ZB_XLU_DECAL2);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, 255, 255, 255, this->rAlpha);

    if (this->rTexIndex < ARRAY_COUNT(sTextures)) {
        {
            TexturePtr tex = sTextures[this->rTexIndex];

            gSPSegment(POLY_XLU_DISP++, 0x08, Lib_SegmentedToVirtual(tex));
        }
        this->rTexIndex++;
    } else {
        gSPSegment(POLY_XLU_DISP++, 0x08, Lib_SegmentedToVirtual(gEffPoppedDekuBubble5Tex));
    }
    gSPDisplayList(POLY_XLU_DISP++, gEffPoppedDekuBubbleDL);

    CLOSE_DISPS(gfxCtx);
}

void EffectSsSbn_Update(PlayState* play, u32 index, EffectSs* this) {
    if (this->life < 10) {
        this->rAlpha -= 20;
    } else if (this->life >= 45) {
        if (this->life < 85) {
            this->rReg8 += 12;
            this->rReg4 -= this->rReg4Step;
        } else {
            this->rReg8 = 1000;
        }
    }
}
