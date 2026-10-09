/*
 * File: z_demo_tre_lgt.c
 * Overlay: ovl_Demo_Tre_Lgt
 * Description: Light Radiating From Treasure Chest
 */

#include "z_demo_tre_lgt.h"
#include "overlays/actors/ovl_En_Box/z_en_box.h"
#include "objects/object_box/object_box.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void DemoTreLgt_Init(Actor* thisx, PlayState* play);
void DemoTreLgt_Destroy(Actor* thisx, PlayState* play);
void DemoTreLgt_Update(Actor* thisx, PlayState* play);
void DemoTreLgt_Draw(Actor* thisx, PlayState* play);

void DemoTreLgt_SetupWait(DemoTreLgt* this);

void DemoTreLgt_Wait(DemoTreLgt* this, PlayState* play);
void DemoTreLgt_SetupAnimate(DemoTreLgt* this, PlayState* play, f32 frame);
void DemoTreLgt_Animate(DemoTreLgt* this, PlayState* play);

typedef struct {
    /* 0x0 */ f32 unk0;
    /* 0x4 */ f32 unk4;
    /* 0x8 */ f32 unk8;
    /* 0xC */ f32 unkC;
} DemoTreLgtInfo; // size = 0x10

static DemoTreLgtInfo D_808E1490[2] = {
    { 1.0f, 136.0f, 190.0f, 40.0f },
    { 1.0f, 136.0f, 220.0f, 50.0f },
};

ActorProfile Demo_Tre_Lgt_Profile = {
    /**/ ACTOR_DEMO_TRE_LGT,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ OBJECT_BOX,
    /**/ sizeof(DemoTreLgt),
    /**/ DemoTreLgt_Init,
    /**/ DemoTreLgt_Destroy,
    /**/ DemoTreLgt_Update,
    /**/ DemoTreLgt_Draw,
};

static CurveAnimationHeader* sBoxLightAnimations[] = {
    &gBoxLightAdultCurveAnim,
    &gBoxLightChildCurveAnim,
};

typedef enum {
    /* 0 */ DEMO_TRE_LGT_ACTION_WAIT, // wait until animation is needed
    /* 1 */ DEMO_TRE_LGT_ACTION_ANIMATE
} DemoTreLgtAction;

static DemoTreLgtActionFunc sActionFuncs[] = {
    DemoTreLgt_Wait,
    DemoTreLgt_Animate,
};

void DemoTreLgt_Init(Actor* thisx, PlayState* play) {
    DemoTreLgt* this = (DemoTreLgt*)thisx;

    SkelCurve_Init(play, &this->skelCurve, &gBoxLightCurveSkel, sBoxLightAnimations[0]);
    this->colorAlpha1 = 255;
    this->colorAlpha2 = 255;
    this->status = 0;
    //! @bug Zora Link should also use animationType 0
    if (GET_PLAYER_FORM == PLAYER_FORM_FIERCE_DEITY) {
        this->animationType = 0;
    } else {
        this->animationType = 1;
    }
    DemoTreLgt_SetupWait(this);
}

void DemoTreLgt_Destroy(Actor* thisx, PlayState* play) {
    DemoTreLgt* this = (DemoTreLgt*)thisx;

    SkelCurve_Destroy(play, &this->skelCurve);
}

void DemoTreLgt_SetupWait(DemoTreLgt* this) {
    this->action = DEMO_TRE_LGT_ACTION_WAIT;
}

void DemoTreLgt_Wait(DemoTreLgt* this, PlayState* play) {
    EnBox* chest = (EnBox*)this->actor.parent;

    if (chest != NULL) {
        if (Animation_OnFrame(&chest->skelAnime, 10.0f)) {
            DemoTreLgt_SetupAnimate(this, play, chest->skelAnime.curFrame);
        }
    }
}

void DemoTreLgt_SetupAnimate(DemoTreLgt* this, PlayState* play, f32 frame) {
    this->action = DEMO_TRE_LGT_ACTION_ANIMATE;
    SkelCurve_SetAnim(&this->skelCurve, sBoxLightAnimations[this->animationType], 1.0f,
                      D_808E1490[this->animationType].unk4 + D_808E1490[this->animationType].unk8, frame, 1.0f);
    SkelCurve_Update(play, &this->skelCurve);
}

void DemoTreLgt_Animate(DemoTreLgt* this, PlayState* play) {
    f32 curFrame = this->skelCurve.curFrame;

    if (curFrame < D_808E1490[this->animationType].unk4) {
        this->colorAlpha1 = 255;
    } else if (curFrame <= (D_808E1490[this->animationType].unk4 + D_808E1490[this->animationType].unk8)) {
        this->colorAlpha1 =
            ((D_808E1490[this->animationType].unk4 - curFrame) / D_808E1490[this->animationType].unk8) * 255 + 255;
    } else {
        this->colorAlpha1 = 0;
    }

    if (curFrame < D_808E1490[this->animationType].unkC) {
        this->colorAlpha2 = 255;
    } else if (curFrame < (D_808E1490[this->animationType].unkC + 10.0f)) {
        this->colorAlpha2 = ((D_808E1490[this->animationType].unkC - curFrame) / 10.0f) * 255 + 255;
    } else {
        this->colorAlpha2 = 0;
    }
    if (curFrame > 30.0f) {
        if (!(this->status & 1)) {
            this->status |= 1;
            Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_EV_TRE_BOX_FLASH);
        }
    }
    if (SkelCurve_Update(play, &this->skelCurve)) {
        Actor_Kill(&this->actor);
    }
}

void DemoTreLgt_Update(Actor* thisx, PlayState* play) {
    DemoTreLgt* this = (DemoTreLgt*)thisx;

    sActionFuncs[this->action](this, play);
}

s32 DemoTreLgt_OverrideLimbDraw(PlayState* play, SkelCurve* skelCuve, s32 limbIndex, Actor* thisx) {
    s32 pad;
    DemoTreLgt* this = (DemoTreLgt*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    gSPSegment(POLY_XLU_DISP++, 0x08,
               Gfx_TwoTexScrollEx(play->state.gfxCtx, 0, (play->state.frames * 2) % 256, 0, 64, 32, 1,
                                  (play->state.frames * -2) % 256, 0, 64, 32, 2, 0, -2, 0));

    if (limbIndex == OBJECT_BOX_LIGHT_LIMB_01) {
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, 255, 255, 180, this->colorAlpha1);
    } else if ((limbIndex == OBJECT_BOX_LIGHT_LIMB_13) || (limbIndex == OBJECT_BOX_LIGHT_LIMB_07) ||
               (limbIndex == OBJECT_BOX_LIGHT_LIMB_04) || (limbIndex == OBJECT_BOX_LIGHT_LIMB_10)) {
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, 255, 255, 180, this->colorAlpha2);
    }

    CLOSE_DISPS(play->state.gfxCtx);

    return true;
}

void DemoTreLgt_Draw(Actor* thisx, PlayState* play) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    DemoTreLgt* this = (DemoTreLgt*)thisx;

    OPEN_DISPS(gfxCtx);

    if (this->action == DEMO_TRE_LGT_ACTION_ANIMATE) {
        Gfx_SetupDL25_Xlu(gfxCtx);
        gDPSetEnvColor(POLY_XLU_DISP++, 200, 255, 0, 0);
        SkelCurve_Draw(&this->actor, play, &this->skelCurve, DemoTreLgt_OverrideLimbDraw, NULL, 1, &this->actor);
    }

    CLOSE_DISPS(gfxCtx);
}
