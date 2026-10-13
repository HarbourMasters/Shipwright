/*
 * File: z_en_mag.c
 * Overlay: ovl_En_Mag
 * Description: Title Logo
 */

#include "z_en_mag.h"
#include "objects/object_mag/object_mag.h"
#include "BenPort.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void EnMag_Init(Actor* thisx, PlayState* play);
void EnMag_Destroy(Actor* thisx, PlayState* play);
void EnMag_Update(Actor* thisx, PlayState* play);
void EnMag_Draw(Actor* thisx, PlayState* play);

/**
 * Steps `var` towards `target` linearly, so that it will arrive in `timeRemaining` seconds. Can be used from either
 * direction. `timeRemaining` must be decremented separately for this to work properly, and obviously timeRemaining = 0
 * must be handled separately.
 *
 * @param var           Variable to step.
 * @param target        Target to step towards.
 * @param timeRemaining Number of times this function should be run for `var` to reach `target`
 * @param stepVar       Variable to use for the step (required to match).
 *
 * The progression is not quite linear because of truncation in the division, but the variable will always reach
 * `target` at the appropriate time since the last step is always the full difference.
 */
#define TIMED_STEP_TO(var, target, timeRemaining, stepVar) \
    {                                                      \
        stepVar = ABS_ALT(var - target) / timeRemaining;   \
        if (var >= target) {                               \
            var -= stepVar;                                \
        } else {                                           \
            var += stepVar;                                \
        }                                                  \
    }                                                      \
    (void)0

/**
 * Similar to `TIMED_STEP_TO`, but will always increase `var`. If var > target, this will eventually increase `var` by
 * an amount that is at most ( timeRemaining + 1 ) * | var - target |, but which depends on the amount lost to
 * truncation from the divisions.
 */
#define TIMED_STEP_UP_TO(var, target, timeRemaining, stepVar) \
    {                                                         \
        stepVar = ABS_ALT(var - target) / timeRemaining;      \
        var += stepVar;                                       \
    }                                                         \
    (void)0

/**
 * Similar to `TIMED_STEP_TO`, but will always increase `var`. If var < target, this will eventually dncrease `var` by
 * an amount that is at most ( timeRemaining + 1 ) * | var - target |, but which depends on the amount lost to
 * truncation from the divisions.
 */
#define TIMED_STEP_DOWN_TO(var, target, timeRemaining, stepVar) \
    {                                                           \
        stepVar = ABS_ALT(var - target) / timeRemaining;        \
        var -= stepVar;                                         \
    }                                                           \
    (void)0

typedef enum {
    /*  0 */ MAG_STATE_INITIAL,
    /*  1 */ MAG_STATE_FADE_IN_MASK_EFFECTS,
    /*  2 */ MAG_STATE_FADE_IN_MASK,
    /*  3 */ MAG_STATE_FADE_IN_MAIN_TITLE,
    /*  5 */ MAG_STATE_FADE_IN_SUBTITLE = 5,
    /*  6 */ MAG_STATE_FADE_IN_COPYRIGHT, // And PRESS START
    /* 10 */ MAG_STATE_CALLED = 10,       // by a button press
    /* 13 */ MAG_STATE_DISPLAY = 13,      // Buttons will trigger File Select state
    /* 20 */ MAG_STATE_FADE_OUT = 20,
    /* 21 */ MAG_STATE_POST_DISPLAY // Go to next scene
} EnMagState;

static s16 sInputDelayTimer = 0;

static s16 sZeldaEffectColorTimer = 30;
static s16 sZeldaEffectColorTargetIndex = 0;

static s16 sTextAlphaTargetIndex = 0;
static s16 sTextAlphaTimer = 20;

ActorProfile En_Mag_Profile = {
    /**/ ACTOR_EN_MAG,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_MAG,
    /**/ sizeof(EnMag),
    /**/ EnMag_Init,
    /**/ EnMag_Destroy,
    /**/ EnMag_Update,
    /**/ EnMag_Draw,
};

void EnMag_Init(Actor* thisx, PlayState* play) {
    EnMag* this = (EnMag*)thisx;
    u16 i;

    this->unk11F54 = 6;
    this->unk11F56 = 10;

    for (i = 0; i < 6; i++) {
        this->effectScrollSs[i] = 0;
        this->effectScrollTs[i] = 0;
    }

    this->appearEffectPrimColor[0] = 255;
    this->appearEffectPrimColor[1] = 155;
    this->appearEffectPrimColor[2] = 255;
    this->appearEffectEnvColor[0] = 0;
    this->appearEffectEnvColor[1] = 255;
    this->appearEffectEnvColor[2] = 155;
    this->appearEffectPrimLodFrac = 20;
    this->appearEffectAlpha = 0;

    this->displayEffectPrimColor[0] = 255;
    this->displayEffectPrimColor[1] = 155;
    this->displayEffectPrimColor[2] = 255;
    this->displayEffectEnvColor[0] = 0;
    this->displayEffectEnvColor[1] = 255;
    this->displayEffectEnvColor[2] = 155;
    this->displayEffectPrimLodFrac = 55;
    this->displayEffectAlpha = 0;

    this->majorasMaskAlpha = 0;
    this->majorasMaskEnvColor[0] = this->majorasMaskEnvColor[1] = this->majorasMaskEnvColor[2] = 255;

    this->mainTitleAlpha = this->subtitleAlpha = this->unk11F32 = this->copyrightAlpha = 0;

    this->unk11F02 = 30;
    this->unk11F00 = this->state = MAG_STATE_INITIAL;

    if (gSaveContext.hudVisibilityForceButtonAlphasByStatus) {
        this->mainTitleAlpha = 210;
        this->unk11F32 = 255;
        this->copyrightAlpha = 255;

        this->appearEffectPrimLodFrac = 100;
        this->appearEffectAlpha = 255;
        this->appearEffectPrimColor[0] = 255;
        this->appearEffectPrimColor[1] = 255;
        this->appearEffectPrimColor[2] = 255;
        this->appearEffectEnvColor[0] = 0;
        this->appearEffectEnvColor[1] = 255;
        this->appearEffectEnvColor[2] = 155;

        this->displayEffectPrimLodFrac = 100;
        this->displayEffectAlpha = 255;
        this->displayEffectPrimColor[0] = 255;
        this->displayEffectPrimColor[1] = 255;
        this->displayEffectPrimColor[2] = 255;
        this->displayEffectEnvColor[0] = 0;
        this->displayEffectEnvColor[1] = 255;
        this->displayEffectEnvColor[2] = 155;

        gSaveContext.hudVisibilityForceButtonAlphasByStatus = false;
        this->state = MAG_STATE_FADE_IN_MASK;
        sInputDelayTimer = 20;
        gSaveContext.transFadeDuration = 1;
        gSaveContext.transWipeSpeed = 255;
    }

    Font_LoadOrderedFont(&this->font);

    this->unk11F58 = 0;
    this->unk11F5A = 0;
    this->unk11F5C = 0;
    this->unk11F60 = 0;

    this->majorasMaskEffectsFadeInTimer = 25;
    this->majorasMaskFadeInTimer = 25;
    this->mainTitleAlphaFadeInTimer = 20;
    this->effectAlphaFadeInTimer = 40;
    this->subtitleFadeInTimer = 10;
    this->copyrightFadeInTimer = 10;
    this->fadeOutTimer = 15;

    sZeldaEffectColorTimer = 30;
    sZeldaEffectColorTargetIndex = 0;
}

void EnMag_Destroy(Actor* thisx, PlayState* play) {
}

void EnMag_UpdateDisplayEffectColors(Actor* thisx) {
    static s16 sDisplayEffectPrimRedTargets[] = { 155, 255 };
    static s16 sDisplayEffectPrimGreenTargets[] = { 255, 155 };
    static s16 sDisplayEffectPrimBlueTargets[] = { 55, 255 };
    static s16 sDisplayEffectEnvRedTargets[] = { 255, 0 };
    static s16 sDisplayEffectEnvBlueTargets[] = { 255, 155 };
    EnMag* this = (EnMag*)thisx;
    s16 colorStep;

    TIMED_STEP_TO(this->displayEffectPrimColor[0], sDisplayEffectPrimRedTargets[sZeldaEffectColorTargetIndex],
                  sZeldaEffectColorTimer, colorStep);
    TIMED_STEP_TO(this->displayEffectPrimColor[1], sDisplayEffectPrimGreenTargets[sZeldaEffectColorTargetIndex],
                  sZeldaEffectColorTimer, colorStep);
    TIMED_STEP_TO(this->displayEffectPrimColor[2], sDisplayEffectPrimBlueTargets[sZeldaEffectColorTargetIndex],
                  sZeldaEffectColorTimer, colorStep);
    TIMED_STEP_TO(this->displayEffectEnvColor[0], sDisplayEffectEnvRedTargets[sZeldaEffectColorTargetIndex],
                  sZeldaEffectColorTimer, colorStep);
    // Skips 1, i.e. green.
    TIMED_STEP_TO(this->displayEffectEnvColor[2], sDisplayEffectEnvBlueTargets[sZeldaEffectColorTargetIndex],
                  sZeldaEffectColorTimer, colorStep);

    sZeldaEffectColorTimer--;
    if (sZeldaEffectColorTimer == 0) {
        this->displayEffectPrimColor[0] = sDisplayEffectPrimRedTargets[sZeldaEffectColorTargetIndex];
        this->displayEffectPrimColor[1] = sDisplayEffectPrimGreenTargets[sZeldaEffectColorTargetIndex];
        this->displayEffectPrimColor[2] = sDisplayEffectPrimBlueTargets[sZeldaEffectColorTargetIndex];
        this->displayEffectEnvColor[0] = sDisplayEffectEnvRedTargets[sZeldaEffectColorTargetIndex];
        // Skips 1, i.e. green.
        this->displayEffectEnvColor[2] = sDisplayEffectEnvBlueTargets[sZeldaEffectColorTargetIndex];
        sZeldaEffectColorTimer = 30;
        sZeldaEffectColorTargetIndex ^= 1;
    }
}

/**
 * Controls the actions performed using a switch and the `state` struct variable rather than action functions. Most of
 * these are to do with fading various parts in and out.
 */
void EnMag_Update(Actor* thisx, PlayState* play) {
    static s16 sAppearEffectPrimGreenTargets[] = { 255, 155 };
    static s16 sAppearEffectEnvRedTargets[] = { 255, 0 };
    static s16 sAppearEffectEnvBlueTargets[] = { 0, 155 };
    s16 step;
    s32 pad[2];
    EnMag* this = (EnMag*)thisx;

    if (gSaveContext.fileNum != 0xFEDC) {
        if (this->state == MAG_STATE_INITIAL) {
            if (CHECK_BTN_ALL(CONTROLLER1(&play->state)->press.button, BTN_START) ||
                CHECK_BTN_ALL(CONTROLLER1(&play->state)->press.button, BTN_A) ||
                CHECK_BTN_ALL(CONTROLLER1(&play->state)->press.button, BTN_B)) {

                if (!CutsceneFlags_Get(play, 4)) {
                    Audio_PlaySfx(NA_SE_SY_PIECE_OF_HEART);
                    this->state = MAG_STATE_CALLED;
                    this->unk11F00 = 0;
                    this->unk11F02 = 30;
                    sInputDelayTimer = 20;
                    gSaveContext.transFadeDuration = 1;
                    gSaveContext.transWipeSpeed = 255;
                }
            }
        } else {
            switch (this->state) {
                case MAG_STATE_FADE_IN_MASK_EFFECTS:
                    TIMED_STEP_TO(this->appearEffectPrimColor[1], sAppearEffectPrimGreenTargets[0],
                                  this->majorasMaskEffectsFadeInTimer, step);
                    TIMED_STEP_TO(this->appearEffectEnvColor[0], sAppearEffectEnvRedTargets[0],
                                  this->majorasMaskEffectsFadeInTimer, step);
                    TIMED_STEP_TO(this->appearEffectEnvColor[2], sAppearEffectEnvBlueTargets[0],
                                  this->majorasMaskEffectsFadeInTimer, step);
                    TIMED_STEP_UP_TO(this->appearEffectAlpha, 255, this->majorasMaskEffectsFadeInTimer, step);
                    TIMED_STEP_UP_TO(this->appearEffectPrimLodFrac, 32, this->majorasMaskEffectsFadeInTimer, step);

                    this->majorasMaskEffectsFadeInTimer--;
                    if (this->majorasMaskEffectsFadeInTimer == 0) {
                        this->appearEffectPrimColor[1] = sAppearEffectPrimGreenTargets[0];
                        this->appearEffectEnvColor[0] = sAppearEffectEnvRedTargets[0];
                        this->appearEffectEnvColor[2] = sAppearEffectEnvBlueTargets[0];
                        this->appearEffectAlpha = 255;
                        this->state = MAG_STATE_FADE_IN_MASK;
                        this->delayTimer = 5;
                    }
                    break;

                case MAG_STATE_FADE_IN_MASK:
                    if (this->delayTimer == 0) {
                        TIMED_STEP_TO(this->appearEffectPrimColor[1], sAppearEffectPrimGreenTargets[1],
                                      this->majorasMaskFadeInTimer, step);
                        TIMED_STEP_TO(this->appearEffectEnvColor[0], sAppearEffectEnvRedTargets[1],
                                      this->majorasMaskFadeInTimer, step);
                        TIMED_STEP_TO(this->appearEffectEnvColor[2], sAppearEffectEnvBlueTargets[1],
                                      this->majorasMaskFadeInTimer, step);
                        TIMED_STEP_UP_TO(this->appearEffectPrimLodFrac, 128, this->majorasMaskFadeInTimer, step);
                        TIMED_STEP_UP_TO(this->majorasMaskAlpha, 255, this->majorasMaskFadeInTimer, step);

                        this->majorasMaskFadeInTimer--;
                        if (this->majorasMaskFadeInTimer == 0) {
                            this->appearEffectPrimColor[1] = sAppearEffectPrimGreenTargets[1];
                            this->appearEffectEnvColor[0] = sAppearEffectEnvRedTargets[1];
                            this->appearEffectEnvColor[2] = sAppearEffectEnvBlueTargets[1];
                            this->appearEffectPrimLodFrac = 128;
                            this->majorasMaskAlpha = 255;
                            this->state = MAG_STATE_FADE_IN_MAIN_TITLE;
                            this->delayTimer = 5;
                        }
                    } else {
                        this->delayTimer--;
                    }
                    break;

                case MAG_STATE_FADE_IN_MAIN_TITLE:
                    if (this->delayTimer == 0) {
                        TIMED_STEP_UP_TO(this->mainTitleAlpha, 255, this->mainTitleAlphaFadeInTimer, step);

                        this->mainTitleAlphaFadeInTimer--;
                        if (this->mainTitleAlphaFadeInTimer == 0) {
                            this->mainTitleAlphaFadeInTimer = 1;
                            this->mainTitleAlpha = 255;
                        }

                        TIMED_STEP_DOWN_TO(this->appearEffectAlpha, 60, this->effectAlphaFadeInTimer, step);
                        TIMED_STEP_UP_TO(this->displayEffectAlpha, 255, this->effectAlphaFadeInTimer, step);
                        TIMED_STEP_UP_TO(this->displayEffectPrimLodFrac, 128, this->effectAlphaFadeInTimer, step);

                        this->effectAlphaFadeInTimer--;

                        if (this->effectAlphaFadeInTimer == 0) {
                            this->appearEffectAlpha = 60;
                            this->displayEffectAlpha = 255;
                            this->displayEffectPrimLodFrac = 128;
                            this->state = MAG_STATE_FADE_IN_SUBTITLE;
                            this->delayTimer = 20;
                        }
                    } else {
                        this->delayTimer--;
                    }
                    break;

                case MAG_STATE_FADE_IN_SUBTITLE:
                    EnMag_UpdateDisplayEffectColors(&this->actor);

                    if (this->delayTimer == 0) {
                        TIMED_STEP_UP_TO(this->subtitleAlpha, 255, this->subtitleFadeInTimer, step);

                        this->subtitleFadeInTimer--;
                        if (this->subtitleFadeInTimer == 0) {
                            this->displayEffectAlpha = 255;
                            this->state = MAG_STATE_FADE_IN_COPYRIGHT;
                            this->delayTimer = 20;
                        }
                    } else {
                        this->delayTimer--;
                    }
                    break;

                case MAG_STATE_FADE_IN_COPYRIGHT:
                    EnMag_UpdateDisplayEffectColors(&this->actor);

                    if (this->delayTimer == 0) {
                        TIMED_STEP_UP_TO(this->copyrightAlpha, 255, this->copyrightFadeInTimer, step);

                        this->copyrightFadeInTimer--;
                        if (this->copyrightFadeInTimer == 0) {
                            this->copyrightAlpha = 255;
                            this->state = MAG_STATE_DISPLAY;
                        }
                    } else {
                        this->delayTimer--;
                    }
                    break;

                case MAG_STATE_CALLED:
                    this->appearEffectPrimColor[1] = sAppearEffectPrimGreenTargets[0];
                    this->appearEffectEnvColor[0] = sAppearEffectEnvRedTargets[0];
                    this->appearEffectEnvColor[2] = sAppearEffectEnvBlueTargets[0];
                    this->appearEffectAlpha = 60;
                    this->appearEffectPrimLodFrac = 128;
                    this->majorasMaskAlpha = 255;
                    this->mainTitleAlpha = 255;
                    this->displayEffectAlpha = 255;
                    this->displayEffectPrimLodFrac = 128;
                    this->subtitleAlpha = 255;
                    this->copyrightAlpha = 255;
                    this->state = MAG_STATE_DISPLAY;
                    break;

                case MAG_STATE_DISPLAY:
                    EnMag_UpdateDisplayEffectColors(&this->actor);

                    if (sInputDelayTimer == 0) {
                        if (CHECK_BTN_ALL(CONTROLLER1(&play->state)->press.button, BTN_START) ||
                            CHECK_BTN_ALL(CONTROLLER1(&play->state)->press.button, BTN_A) ||
                            CHECK_BTN_ALL(CONTROLLER1(&play->state)->press.button, BTN_B)) {
                            if (play->transitionTrigger != TRANS_TRIGGER_START) {
                                Audio_SetCutsceneFlag(false);
                                gOpeningEntranceIndex++;
                                if (gOpeningEntranceIndex >= 2) {
                                    gOpeningEntranceIndex = 0;
                                }
                                Audio_PlaySfx(NA_SE_SY_PIECE_OF_HEART);
                                gSaveContext.gameMode = GAMEMODE_FILE_SELECT;
                                play->transitionTrigger = TRANS_TRIGGER_START;
                                play->transitionType = TRANS_TYPE_FADE_BLACK;
                                play->nextEntrance = ENTRANCE(CUTSCENE, 0);
                                gSaveContext.save.cutsceneIndex = 0;
                                gSaveContext.sceneLayer = 0;
                            }
                            this->unk11F54 = 15;
                            this->unk11F56 = 25;
                            this->state = MAG_STATE_FADE_OUT;
                        }
                    } else {
                        sInputDelayTimer--;
                    }
                    break;

                case MAG_STATE_FADE_OUT:
                    TIMED_STEP_DOWN_TO(this->appearEffectAlpha, 0, this->fadeOutTimer, step);
                    TIMED_STEP_DOWN_TO(this->majorasMaskAlpha, 0, this->fadeOutTimer, step);
                    TIMED_STEP_DOWN_TO(this->displayEffectAlpha, 0, this->fadeOutTimer, step);
                    TIMED_STEP_DOWN_TO(this->mainTitleAlpha, 0, this->fadeOutTimer, step);
                    TIMED_STEP_DOWN_TO(this->subtitleAlpha, 0, this->fadeOutTimer, step);
                    TIMED_STEP_DOWN_TO(this->copyrightAlpha, 0, this->fadeOutTimer, step);

                    this->fadeOutTimer--;
                    if (this->fadeOutTimer == 0) {
                        this->appearEffectAlpha = 0;
                        this->majorasMaskAlpha = 0;
                        this->mainTitleAlpha = 0;
                        this->subtitleAlpha = 0;
                        this->displayEffectAlpha = 0;
                        this->copyrightAlpha = 0;
                        this->state = MAG_STATE_POST_DISPLAY;
                    }
                    break;

                default:
                    break;
            }

            // Appear fully immediately if called during fade-in states.
            if ((this->state > MAG_STATE_INITIAL) && (this->state < MAG_STATE_CALLED)) {
                if (CHECK_BTN_ALL(CONTROLLER1(&play->state)->press.button, BTN_START) ||
                    CHECK_BTN_ALL(CONTROLLER1(&play->state)->press.button, BTN_A) ||
                    CHECK_BTN_ALL(CONTROLLER1(&play->state)->press.button, BTN_B)) {
                    Audio_PlaySfx(NA_SE_SY_PIECE_OF_HEART);
                    this->state = MAG_STATE_CALLED;
                }
            }
        }
    }

    if (this->state == MAG_STATE_INITIAL) {
        if (CutsceneFlags_Get(play, 3)) {
            this->unk11F02 = 40;
            this->state = MAG_STATE_FADE_IN_MASK_EFFECTS;
        }
    } else if (this->state < MAG_STATE_FADE_OUT) {
        if (CutsceneFlags_Get(play, 4)) {
            this->state = MAG_STATE_FADE_OUT;
        }
    }
}

/**
 * Draws an i8 texture.
 *
 * @param[in,out] gfxP      Pointer to current displaylist.
 * @param[in]     texture   Texture to draw.
 * @param[in]     texWidth  Width of the texture.
 * @param[in]     texHeight Height of the texture.
 * @param[in]     rectLeft  X coordinate of the top-left of the draw position.
 * @param[in]     rectTop   Y coordinate of the top-left of the draw position.
 */
void EnMag_DrawTextureI8(Gfx** gfxP, TexturePtr texture, s16 texWidth, s16 texHeight, s16 rectLeft, s16 rectTop) {
    Gfx* gfx = *gfxP;

    gDPLoadTextureBlock(gfx++, texture, G_IM_FMT_I, G_IM_SIZ_8b, texWidth, texHeight, 0, G_TX_NOMIRROR | G_TX_WRAP,
                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

    gSPTextureRectangle(gfx++, rectLeft << 2, rectTop << 2, (rectLeft + texWidth) << 2, (rectTop + texHeight) << 2,
                        G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);

    *gfxP = gfx;
}

/**
 * Draws an ia8 texture.
 *
 * @param[in,out] gfxP      Pointer to current displaylist.
 * @param[in]     texture   Texture to draw.
 * @param[in]     texWidth  Width of the texture.
 * @param[in]     texHeight Height of the texture.
 * @param[in]     rectLeft  X coordinate of the top-left of the draw position.
 * @param[in]     rectTop   Y coordinate of the top-left of the draw position.
 */
void EnMag_DrawTextureIA8(Gfx** gfxP, TexturePtr texture, s16 texWidth, s16 texHeight, s16 rectLeft, s16 rectTop) {
    Gfx* gfx = *gfxP;

    gDPLoadTextureBlock(gfx++, texture, G_IM_FMT_IA, G_IM_SIZ_8b, texWidth, texHeight, 0, G_TX_NOMIRROR | G_TX_WRAP,
                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

    gSPTextureRectangle(gfx++, rectLeft << 2, rectTop << 2, (rectLeft + texWidth) << 2, (rectTop + texHeight) << 2,
                        G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);

    *gfxP = gfx;
}

/**
 * Draws an i8 effect texture, masking it with an i4 mask, with shifting
 *
 * @param[in,out] gfxP         Pointer to current displaylist.
 * @param[in]     maskTex      Texture with which to mask, i4.
 * @param[in]     effectTex    Effect texture to draw, i8.
 * @param[in]     maskWidth    Width of masking texture.
 * @param[in]     maskHeight   Height of masking texture.
 * @param[in]     effectWidth  Width of effect texture.
 * @param[in]     effectHeight Height of effect texture.
 * @param[in]     rectLeft     X coordinate of the top-left of the draw position.
 * @param[in]     rectTop      Y coordinate of the top-left of the draw position.
 * @param[in]     shifts       Shift to apply to effect texture's S coordinate to control LOD.
 * @param[in]     shiftt       Shift to apply to effect texture's T coordinate to control LOD.
 * @param[in]     index        Index into the scrolling arrays to use for gDPSetTileSize.
 * @param[in]     this         Pointer to EnMag instance.
 */
void EnMag_DrawEffectTextures(Gfx** gfxP, TexturePtr maskTex, TexturePtr effectTex, s16 maskWidth, s16 maskHeight,
                              s16 effectWidth, s16 effectHeight, s16 rectLeft, s16 rectTop, u16 shifts, u16 shiftt,
                              u16 index, EnMag* this) {
    Gfx* gfx = *gfxP;

    gDPLoadMultiBlock_4b(gfx++, maskTex, 0x0000, G_TX_RENDERTILE, G_IM_FMT_I, maskWidth, maskHeight, 0,
                         G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                         G_TX_NOLOD);

    gDPLoadMultiBlock(gfx++, effectTex, 0x0100, 1, G_IM_FMT_I, G_IM_SIZ_8b, effectWidth, effectHeight, 0,
                      G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 5, 5, shifts, shiftt);

    gDPSetTileSize(gfx++, 1, this->effectScrollSs[index] & 0x7F, this->effectScrollTs[index] & 0x7F,
                   (this->effectScrollSs[index] & 0x7F) + 15, (this->effectScrollTs[index] & 0x7F) + 15);

    gSPTextureRectangle(gfx++, rectLeft << 2, rectTop << 2, (rectLeft + maskWidth) << 2, (rectTop + maskHeight) << 2,
                        G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);

    *gfxP = gfx;
}

// 2S2H [HD Textures] The original function used pointer math and tiling to draw the full texture, this breaks support
// for HD textures, additionally `source` is now an OTR path, not a pointer to the raw texture. This is our own version
// that just hands off the OTR path and let the GBI handle the rest.
void EnMag_DrawImageRGBA32HDSupport(Gfx** gfxp, s16 centerX, s16 centerY, TexturePtr source, u32 width, u32 height) {
    Gfx* gfx = *gfxp;
    u32 rectLeft = centerX - (width / 2);
    u32 rectTop = centerY - (height / 2);

    Gfx_SetupDL56_Ptr(&gfx);

    gDPSetTileCustom(gfx++, G_IM_FMT_RGBA, G_IM_SIZ_32b, width, height, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                     G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

    gDPSetTextureImage(gfx++, G_IM_FMT_RGBA, G_IM_SIZ_32b, width, source);

    gDPLoadSync(gfx++);
    gDPLoadTile(gfx++, G_TX_LOADTILE, 0, 0, (width - 1) << 2, (height - 1) << 2);

    gSPTextureRectangle(gfx++, rectLeft << 2, rectTop << 2, (rectLeft + (s32)width) << 2, (rectTop + height) << 2,
                        G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);

    *gfxp = gfx;
}

/**
 * Draws an rgba32 texture. Because these are so large, this will draw the texture in horizontal stripes, each narrow
 * enough that that part of the texture will fit into TMEM's 4kB.
 *
 * @param[in,out] gfxP    Pointer to current displaylist.
 * @param[in]     centerX X coordinate of the center of the draw position.
 * @param[in]     centerY Y coordinate of the center of the draw position.
 * @param[in]     source  Texture to draw.
 * @param[in]     width   Width of the texture.
 * @param[in]     height  Height of the texture.
 */
void EnMag_DrawImageRGBA32(Gfx** gfxP, s16 centerX, s16 centerY, TexturePtr source, u32 width, u32 height) {
    // 2S2H [HD Textures]
    return EnMag_DrawImageRGBA32HDSupport(gfxP, centerX, centerY, source, width, height);

    Gfx* gfx = *gfxP;
    uintptr_t curTexture;
    s32 textureCount;
    u32 rectLeft;
    u32 rectTop;
    u32 textureHeight;
    s32 remainingSize;
    s32 textureSize;
    s32 pad;
    s32 i;

    Gfx_SetupDL56_Ptr(&gfx);

    curTexture = (uintptr_t)source;
    rectLeft = centerX - (width / 2);
    rectTop = centerY - (height / 2);
    textureHeight = TMEM_SIZE / (width << 2);
    remainingSize = (width * height) << 2;
    textureSize = (width * textureHeight) << 2;
    textureCount = remainingSize / textureSize;
    if ((remainingSize % textureSize) != 0) {
        textureCount++;
    }

    gDPSetTileCustom(gfx++, G_IM_FMT_RGBA, G_IM_SIZ_32b, width, textureHeight, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                     G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

    remainingSize -= textureSize;

    for (i = 0; i < textureCount; i++) {
        gDPSetTextureImage(gfx++, G_IM_FMT_RGBA, G_IM_SIZ_32b, width, curTexture);

        gDPLoadSync(gfx++);
        gDPLoadTile(gfx++, G_TX_LOADTILE, 0, 0, (width - 1) << 2, (textureHeight - 1) << 2);

        gSPTextureRectangle(gfx++, rectLeft << 2, rectTop << 2, (rectLeft + (s32)width) << 2,
                            (rectTop + textureHeight) << 2, G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);

        curTexture += textureSize;
        rectTop += textureHeight;

        if ((remainingSize - textureSize) < 0) {
            if (remainingSize > 0) {
                textureHeight = remainingSize / (s32)(width << 2);
                remainingSize -= textureSize;

                gDPSetTileCustom(gfx++, G_IM_FMT_RGBA, G_IM_SIZ_32b, width, textureHeight, 0,
                                 G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK,
                                 G_TX_NOLOD, G_TX_NOLOD);
            }
        } else {
            remainingSize -= textureSize;
        }
    }

    *gfxP = gfx;
}

/**
 * Draws one character, expected to be a 16 by 16 i4 texture. It will draw shrunk to 10 by 10.
 *
 * @param[in,out] gfxP     Pointer to current displaylist.
 * @param[in]     texture  Texture to draw.
 * @param[in]     rectLeft X coordinate of the top-left of the draw position.
 * @param[in]     rectTop  Y coordinate of the top-left of the draw position.
 */
void EnMag_DrawCharTexture(Gfx** gfxP, TexturePtr texture, s32 rectLeft, s32 rectTop) {
    Gfx* gfx = *gfxP;

    gDPLoadTextureBlock_4b(gfx++, texture, G_IM_FMT_I, 16, 16, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                           G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

    // The dsdx and dtdy are roughly 1.587 * 0x400, to fit the texture into 10 by 10 pixels. It is not the more
    // obvious 1.6 * 0x400 = 1656.
    gSPTextureRectangle(gfx++, rectLeft << 2, rectTop << 2, (rectLeft + 10) << 2, (rectTop + 10) << 2, G_TX_RENDERTILE,
                        0, 0, 1625, 1625);

    *gfxP = gfx;
}

#define EFFECT_MASK_TEX_WIDTH 64
#define EFFECT_MASK_TEX_HEIGHT 64
#define EFFECT_TEX_WIDTH 32
#define EFFECT_TEX_HEIGHT 32
#define EFFECT_TEX_LEFT 38
#define EFFECT_TEX_TOP 57

#define MAJORAS_MASK_TEX_WIDTH 128
#define MAJORAS_MASK_TEX_HEIGHT 112
#define MAJORAS_MASK_TEX_CENTER_X 124
#define MAJORAS_MASK_TEX_CENTER_Y 103

#define ZELDA_TEX_WIDTH 144
#define ZELDA_TEX_HEIGHT 64
#define ZELDA_TEX_CENTER_X 177
#define ZELDA_TEX_CENTER_Y 105

#define SUBTITLE_TEX_WIDTH 104
#define SUBTITLE_TEX_HEIGHT 16
#define SUBTITLE_TEX_LEFT 151
#define SUBTITLE_TEX_TOP 124

#define THE_LEGEND_OF_TEX_WIDTH 72
#define THE_LEGEND_OF_TEX_HEIGHT 8
#define THE_LEGEND_OF_TEX_LEFT 158
#define THE_LEGEND_OF_TEX_TOP 71

#define COPYRIGHT_TEX_WIDTH 128
#define COPYRIGHT_TEX_HEIGHT 16
#define COPYRIGHT_TEX_LEFT 94
#define COPYRIGHT_TEX_TOP 198

#define COPYRIGHT_TEX_WIDTH_GC 200
#define COPYRIGHT_TEX_LEFT_GC 58

#define NO_CONTROLLER_FIRST_TEX_WIDTH 256
#define NO_CONTROLLER_FIRST_TEX_HEIGHT 9
#define NO_CONTROLLER_FIRST_TEX_LEFT 35
#define NO_CONTROLLER_FIRST_TEX_TOP 175

#define NO_CONTROLLER_SECOND_TEX_WIDTH 144
#define NO_CONTROLLER_SECOND_TEX_HEIGHT 9
#define NO_CONTROLLER_SECOND_TEX_LEFT 91
#define NO_CONTROLLER_SECOND_TEX_TOP 188

// Top-left of the text itself, not the shadow
#define PRESS_START_LEFT 119
#define PRESS_START_TOP 174
#define PRESS_START_CHAR_SPACING 7 // Amount of rightward shift before printing next char
#define PRESS_START_SPACE 5        // Extra space between the words

/**
 * Loads title, PRESS START text, etc. graphics to gfxP, which is made to live on
 * POLY_OPA_DISP, but is used by OVERLAY_DISP.
 */
void EnMag_DrawInner(Actor* thisx, PlayState* play, Gfx** gfxP) {
    static u8 sPressStartFontIndices[] = {
        0x19, 0x1B, 0x0E, 0x1C, 0x1C, 0x1C, 0x1D, 0x0A, 0x1B, 0x1D,
    }; // Indices into this->font.fontBuf
    static TexturePtr sAppearEffectMaskTextures[] = {
        gTitleScreenAppearEffectMask00Tex, gTitleScreenAppearEffectMask01Tex, gTitleScreenAppearEffectMask02Tex,
        gTitleScreenAppearEffectMask10Tex, gTitleScreenAppearEffectMask11Tex, gTitleScreenAppearEffectMask12Tex,
    }; // Visible when mask appearing
    static TexturePtr sDisplayEffectMaskTextures[] = {
        gTitleScreenDisplayEffectMask00Tex, gTitleScreenDisplayEffectMask01Tex, gTitleScreenDisplayEffectMask02Tex,
        gTitleScreenDisplayEffectMask10Tex, gTitleScreenDisplayEffectMask11Tex, gTitleScreenDisplayEffectMask12Tex,
    }; // Visible when full game logo displayed
    static TexturePtr sEffectTextures[] = {
        gTitleScreenFlame0Tex, gTitleScreenFlame1Tex, gTitleScreenFlame1Tex,
        gTitleScreenFlame2Tex, gTitleScreenFlame3Tex, gTitleScreenFlame3Tex,
    };
    static s16 sEffectScrollVelocitySs[] = { -1, 1, 1, -1, 1, 1 };
    static s16 sEffectScrollVelocityTs[] = { -2, -2, -2, 2, 2, 2 };
    static s16 sTextAlpha = 0; // For drawing both the No Controller message and "PRESS START"
    static s16 sTextAlphaTargets[] = { 255, 0 };
    s32 pad;
    EnMag* this = (EnMag*)thisx;
    Font* font = &this->font;
    Gfx* gfx = *gfxP;
    u16 i;
    u16 j;
    u16 k;
    s32 rectLeft;
    s32 rectTop;
    s16 step;

    // Set segment 6 to the object, since this will be read by OVERLAY_DISP where it is not set by default.
    gSPSegment(gfx++, 0x06, play->objectCtx.slots[this->actor.objectSlot].segment);

    Gfx_SetupDL39_Ptr(&gfx);

    // Mask appearing effects
    gDPPipeSync(gfx++);
    gDPSetCycleType(gfx++, G_CYC_2CYCLE);
    gDPSetAlphaCompare(gfx++, G_AC_THRESHOLD);
    gDPSetRenderMode(gfx++, G_RM_PASS, G_RM_CLD_SURF2);
    gDPSetCombineLERP(gfx++, TEXEL1, PRIMITIVE, PRIM_LOD_FRAC, TEXEL0, TEXEL1, 1, PRIM_LOD_FRAC, TEXEL0, PRIMITIVE,
                      ENVIRONMENT, COMBINED, ENVIRONMENT, COMBINED, 0, PRIMITIVE, 0);

    gDPSetPrimColor(gfx++, 0, this->appearEffectPrimLodFrac, this->appearEffectPrimColor[0],
                    this->appearEffectPrimColor[1], this->appearEffectPrimColor[2], this->appearEffectAlpha);
    gDPSetEnvColor(gfx++, this->appearEffectEnvColor[0], this->appearEffectEnvColor[1], this->appearEffectEnvColor[2],
                   255);

    if (this->appearEffectPrimLodFrac != 0) {
        for (k = 0, i = 0, rectTop = EFFECT_TEX_LEFT; i < 2; i++, rectTop += EFFECT_MASK_TEX_HEIGHT) {
            for (j = 0, rectLeft = EFFECT_TEX_TOP; j < 3; j++, k++, rectLeft += EFFECT_MASK_TEX_WIDTH) {
                this->effectScrollSs[k] += sEffectScrollVelocitySs[k];
                this->effectScrollTs[k] += sEffectScrollVelocityTs[k];
                EnMag_DrawEffectTextures(&gfx, sAppearEffectMaskTextures[k], sEffectTextures[k], EFFECT_MASK_TEX_WIDTH,
                                         EFFECT_MASK_TEX_HEIGHT, EFFECT_TEX_WIDTH, EFFECT_TEX_HEIGHT, rectLeft, rectTop,
                                         1, 1, k, this);
            }
        }
    }

    Gfx_SetupDL39_Ptr(&gfx);

    if (this->majorasMaskAlpha != 0) {
        gDPSetCombineLERP(gfx++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0, PRIMITIVE,
                          ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);

        gDPSetPrimColor(gfx++, 0, 0, 255, 255, 255, this->majorasMaskAlpha);
        gDPSetEnvColor(gfx++, this->majorasMaskEnvColor[0], this->majorasMaskEnvColor[1], this->majorasMaskEnvColor[2],
                       255);

        EnMag_DrawImageRGBA32(&gfx, MAJORAS_MASK_TEX_CENTER_X, MAJORAS_MASK_TEX_CENTER_Y, gTitleScreenMajorasMaskTex,
                              MAJORAS_MASK_TEX_WIDTH, MAJORAS_MASK_TEX_HEIGHT);
    }

    // Flame glow effects on full game logo when displayed
    gDPPipeSync(gfx++);
    gDPSetCycleType(gfx++, G_CYC_2CYCLE);
    gDPSetAlphaCompare(gfx++, G_AC_THRESHOLD);
    gDPSetRenderMode(gfx++, G_RM_PASS, G_RM_CLD_SURF2);
    gDPSetCombineLERP(gfx++, TEXEL1, PRIMITIVE, PRIM_LOD_FRAC, TEXEL0, TEXEL1, 1, PRIM_LOD_FRAC, TEXEL0, PRIMITIVE,
                      ENVIRONMENT, COMBINED, ENVIRONMENT, COMBINED, 0, PRIMITIVE, 0);

    gDPSetPrimColor(gfx++, 0, this->displayEffectPrimLodFrac, this->displayEffectPrimColor[0],
                    this->displayEffectPrimColor[1], this->displayEffectPrimColor[2], this->displayEffectAlpha);
    gDPSetEnvColor(gfx++, this->displayEffectEnvColor[0], this->displayEffectEnvColor[1],
                   this->displayEffectEnvColor[2], 255);

    if (this->displayEffectPrimLodFrac != 0) {
        for (k = 0, i = 0, rectTop = EFFECT_TEX_LEFT; i < 2; i++, rectTop += EFFECT_MASK_TEX_HEIGHT) {
            for (j = 0, rectLeft = EFFECT_TEX_TOP; j < 3; j++, k++, rectLeft += EFFECT_MASK_TEX_WIDTH) {
                EnMag_DrawEffectTextures(&gfx, sDisplayEffectMaskTextures[k], sEffectTextures[k], EFFECT_MASK_TEX_WIDTH,
                                         EFFECT_MASK_TEX_HEIGHT, EFFECT_TEX_WIDTH, EFFECT_TEX_HEIGHT, rectLeft, rectTop,
                                         1, 1, k, this);
            }
        }
    }

    if (this->subtitleAlpha != 0) {
        Gfx_SetupDL39_Ptr(&gfx);

        gDPSetAlphaCompare(gfx++, G_AC_NONE);
        gDPSetCombineMode(gfx++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);

        if (this->mainTitleAlpha < 100) {
            gDPSetRenderMode(gfx++, G_RM_CLD_SURF, G_RM_CLD_SURF2);
        } else {
            gDPSetRenderMode(gfx++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
        }

        gDPSetPrimColor(gfx++, 0, 0, 0, 0, 0, this->subtitleAlpha);
        gDPSetEnvColor(gfx++, 100, 0, 100, 255);

        EnMag_DrawTextureI8(&gfx, gTitleScreenMajorasMaskSubtitleMaskTex, SUBTITLE_TEX_WIDTH, SUBTITLE_TEX_HEIGHT,
                            SUBTITLE_TEX_LEFT, SUBTITLE_TEX_TOP);
    }

    Gfx_SetupDL39_Ptr(&gfx);

    gDPSetAlphaCompare(gfx++, G_AC_NONE);
    gDPSetCombineMode(gfx++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);

    if (this->mainTitleAlpha < 100) {
        gDPSetRenderMode(gfx++, G_RM_CLD_SURF, G_RM_CLD_SURF2);
    } else {
        gDPSetRenderMode(gfx++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
    }

    gDPSetPrimColor(gfx++, 0, 120, 208, 102, 222, this->subtitleAlpha);

    EnMag_DrawTextureI8(&gfx, gTitleScreenMajorasMaskSubtitleTex, SUBTITLE_TEX_WIDTH, SUBTITLE_TEX_HEIGHT,
                        SUBTITLE_TEX_LEFT, SUBTITLE_TEX_TOP);

    Gfx_SetupDL39_Ptr(&gfx);

    gDPSetAlphaCompare(gfx++, G_AC_NONE);
    gDPSetCombineMode(gfx++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);

    if (this->mainTitleAlpha != 0) {
        gDPSetPrimColor(gfx++, 0, 0, 255, 255, 255, this->mainTitleAlpha);

        EnMag_DrawImageRGBA32(&gfx, ZELDA_TEX_CENTER_X, ZELDA_TEX_CENTER_Y, gTitleScreenZeldaLogoTex, ZELDA_TEX_WIDTH,
                              ZELDA_TEX_HEIGHT);

        gDPPipeSync(gfx++);

        if (this->mainTitleAlpha < 100) {
            gDPSetRenderMode(gfx++, G_RM_CLD_SURF, G_RM_CLD_SURF2);
        } else {
            gDPSetRenderMode(gfx++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
        }

        gDPSetPrimColor(gfx++, 0, 0, 208, 102, 222, this->mainTitleAlpha);

        EnMag_DrawTextureI8(&gfx, gTitleScreenTheLegendOfTextTex, THE_LEGEND_OF_TEX_WIDTH, THE_LEGEND_OF_TEX_HEIGHT,
                            THE_LEGEND_OF_TEX_LEFT, THE_LEGEND_OF_TEX_TOP);
    }

    Gfx_SetupDL39_Ptr(&gfx);

    if (this->copyrightAlpha != 0) {
        gDPSetAlphaCompare(gfx++, G_AC_NONE);
        gDPSetCombineMode(gfx++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
        gDPSetPrimColor(gfx++, 0, 0, this->copyrightAlpha, this->copyrightAlpha, this->copyrightAlpha,
                        this->copyrightAlpha);

        uint32_t gameVersion = ResourceMgr_GetGameVersion(0);

        if (gameVersion == MM_NTSC_US_GC) {
            EnMag_DrawTextureIA8(&gfx, gTitleScreenCopyright20002003NintendoTex, COPYRIGHT_TEX_WIDTH_GC,
                                 COPYRIGHT_TEX_HEIGHT, COPYRIGHT_TEX_LEFT_GC, COPYRIGHT_TEX_TOP);
        } else { // Default: MM_NTSC_US_10
            EnMag_DrawTextureIA8(&gfx, gTitleScreenCopyright2000NintendoTex, COPYRIGHT_TEX_WIDTH, COPYRIGHT_TEX_HEIGHT,
                                 COPYRIGHT_TEX_LEFT, COPYRIGHT_TEX_TOP);
        }
    }

    if (gSaveContext.fileNum == 0xFEDC) {
        // Draw No controller message

        TIMED_STEP_TO(sTextAlpha, sTextAlphaTargets[sTextAlphaTargetIndex], sTextAlphaTimer, step);

        gDPPipeSync(gfx++);
        gDPSetAlphaCompare(gfx++, G_AC_THRESHOLD);

        gDPSetCombineLERP(gfx++, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE,
                          0);
        gDPSetPrimColor(gfx++, 0, 0, 0, 0, 0, sTextAlpha);
        gDPLoadTextureBlock_4b(gfx++, gTitleScreenControllerNotConnectedTextTex, G_IM_FMT_I,
                               NO_CONTROLLER_FIRST_TEX_WIDTH, NO_CONTROLLER_FIRST_TEX_HEIGHT, 0,
                               G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                               G_TX_NOLOD, G_TX_NOLOD);
        // Texture shadow
        gSPTextureRectangle(gfx++, (NO_CONTROLLER_FIRST_TEX_LEFT + 1) << 2, (NO_CONTROLLER_FIRST_TEX_TOP + 1) << 2,
                            (NO_CONTROLLER_FIRST_TEX_LEFT + 1 + NO_CONTROLLER_FIRST_TEX_WIDTH) << 2,
                            (NO_CONTROLLER_FIRST_TEX_TOP + 1 + NO_CONTROLLER_FIRST_TEX_HEIGHT) << 2, G_TX_RENDERTILE, 0,
                            0, 1 << 10, 1 << 10);
        // Actual texture
        gDPSetPrimColor(gfx++, 0, 0, 205, 255, 255, sTextAlpha);
        gSPTextureRectangle(gfx++, NO_CONTROLLER_FIRST_TEX_LEFT << 2, NO_CONTROLLER_FIRST_TEX_TOP << 2,
                            (NO_CONTROLLER_FIRST_TEX_LEFT + NO_CONTROLLER_FIRST_TEX_WIDTH) << 2,
                            (NO_CONTROLLER_FIRST_TEX_TOP + NO_CONTROLLER_FIRST_TEX_HEIGHT) << 2, G_TX_RENDERTILE, 0, 0,
                            1 << 10, 1 << 10);

        gDPSetPrimColor(gfx++, 0, 0, 0, 0, 0, sTextAlpha);
        gDPLoadTextureBlock_4b(gfx++, gTitleScreenInsertControllerTextTex, G_IM_FMT_I, NO_CONTROLLER_SECOND_TEX_WIDTH,
                               NO_CONTROLLER_SECOND_TEX_HEIGHT, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP,
                               G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
        // Texture shadow
        gSPTextureRectangle(gfx++, (NO_CONTROLLER_SECOND_TEX_LEFT + 1) << 2, (NO_CONTROLLER_SECOND_TEX_TOP + 1) << 2,
                            (NO_CONTROLLER_SECOND_TEX_LEFT + 1 + NO_CONTROLLER_SECOND_TEX_WIDTH) << 2,
                            (NO_CONTROLLER_SECOND_TEX_TOP + 1 + NO_CONTROLLER_SECOND_TEX_HEIGHT) << 2, G_TX_RENDERTILE,
                            0, 0, 1 << 10, 1 << 10);
        // Actual texture
        gDPSetPrimColor(gfx++, 0, 0, 205, 255, 255, sTextAlpha);
        gSPTextureRectangle(gfx++, NO_CONTROLLER_SECOND_TEX_LEFT << 2, NO_CONTROLLER_SECOND_TEX_TOP << 2,
                            (NO_CONTROLLER_SECOND_TEX_LEFT + NO_CONTROLLER_SECOND_TEX_WIDTH) << 2,
                            (NO_CONTROLLER_SECOND_TEX_TOP + NO_CONTROLLER_SECOND_TEX_HEIGHT) << 2, G_TX_RENDERTILE, 0,
                            0, 1 << 10, 1 << 10);

        sTextAlphaTimer--;
        if (sTextAlphaTimer == 0) {
            sTextAlpha = sTextAlphaTargets[sTextAlphaTargetIndex];
            if (gSaveContext.fileNum == 0xFEDC) {
                sTextAlphaTimer = 40;
            } else {
                sTextAlphaTimer = 20;
            }
            sTextAlphaTargetIndex ^= 1;
        }
    } else if (this->copyrightAlpha != 0) {
        // Draw "PRESS START" characters

        TIMED_STEP_TO(sTextAlpha, sTextAlphaTargets[sTextAlphaTargetIndex], sTextAlphaTimer, step);

        // Text shadow
        gDPPipeSync(gfx++);
        gDPSetCombineLERP(gfx++, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE,
                          0);
        gDPSetPrimColor(gfx++, 0, 0, 0, 0, 0, sTextAlpha);

        rectLeft = PRESS_START_LEFT + 1;
        for (i = 0; i < ARRAY_COUNT(sPressStartFontIndices); i++) {
            EnMag_DrawCharTexture(&gfx, font->fontBuf + sPressStartFontIndices[i] * FONT_CHAR_TEX_SIZE, rectLeft,
                                  PRESS_START_TOP + 1);

            rectLeft += PRESS_START_CHAR_SPACING;
            if (i == 4) {
                rectLeft += PRESS_START_SPACE;
            }
        }

        // Actual text
        gDPPipeSync(gfx++);
        gDPSetPrimColor(gfx++, 0, 0, 255, 30, 30, sTextAlpha);

        rectLeft = PRESS_START_LEFT;
        for (i = 0; i < ARRAY_COUNT(sPressStartFontIndices); i++) {
            EnMag_DrawCharTexture(&gfx, font->fontBuf + sPressStartFontIndices[i] * FONT_CHAR_TEX_SIZE, rectLeft,
                                  PRESS_START_TOP);
            rectLeft += PRESS_START_CHAR_SPACING;
            if (i == 4) {
                rectLeft += PRESS_START_SPACE;
            }
        }

        sTextAlphaTimer--;
        if (sTextAlphaTimer == 0) {
            sTextAlpha = sTextAlphaTargets[sTextAlphaTargetIndex];
            if (gSaveContext.fileNum == 0xFEDC) {
                sTextAlphaTimer = 40;
            } else {
                sTextAlphaTimer = 20;
            }
            sTextAlphaTargetIndex ^= 1;
        }
    }

    *gfxP = gfx;
}

/**
 * Jumps drawing to POLY_OPA_DISP to take advantage of the extra space available, but jmups back and actually draws
 * using OVERLAY_DISP.
 */
void EnMag_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    Gfx* gfx;
    Gfx* gfxRef;

    OPEN_DISPS(play->state.gfxCtx);

    gfxRef = POLY_OPA_DISP;
    gfx = Graph_GfxPlusOne(gfxRef);
    gSPDisplayList(OVERLAY_DISP++, gfx);

    EnMag_DrawInner(thisx, play, &gfx);

    gSPEndDisplayList(gfx++);
    Graph_BranchDlist(gfxRef, gfx);
    POLY_OPA_DISP = gfx;

    CLOSE_DISPS(play->state.gfxCtx);
}
