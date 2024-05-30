/*
 * File: z_en_mag.c
 * Overlay: ovl_En_Mag
 * Description: Title Screen Manager & Logo
 */

#include "z_en_mag.h"
#include "objects/object_mag/object_mag.h"
#include <GameVersions.h>

#define FLAGS (ACTOR_FLAG_UPDATE_WHILE_CULLED | ACTOR_FLAG_DRAW_WHILE_CULLED)

void EnMag_Init(Actor* thisx, PlayState* play);
void EnMag_InitMq(Actor* thisx, PlayState* play);
void EnMag_InitVanilla(Actor* thisx, PlayState* play);

void EnMag_Destroy(Actor* thisx, PlayState* play);
void EnMag_UpdateMq(Actor* thisx, PlayState* play);
void EnMag_UpdateVanilla(Actor* thisx, PlayState* play);

void EnMag_Draw(Actor* thisx, PlayState* play);
void EnMag_DrawInnerVanilla(Actor* thisx, PlayState* play, Gfx** gfxp);
void EnMag_DrawInnerMq(Actor* thisx, PlayState* play, Gfx** gfxp);
void EnMag_DrawInnerVanillaNtsc(Actor* thisx, PlayState* play, Gfx** gfxp);
void EnMag_DrawInnerMqNtsc(Actor* thisx, PlayState* play, Gfx** gfxp);

typedef void (*EnMagDrawInnerFunc)(struct Actor*, PlayState*, Gfx**);

static EnMagDrawInnerFunc drawInnerFunc;

const ActorInit En_Mag_InitVars = {
    ACTOR_EN_MAG,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_MAG,
    sizeof(EnMag),
    (ActorFunc)EnMag_Init,
    (ActorFunc)EnMag_Destroy,
    (ActorFunc)NULL,
    (ActorFunc)EnMag_Draw,
    NULL,
};

const char* noControllerMsg[] = {
    "NO CONTROLLER",
    "CONTROLLER FEHLT",
    "MANETTE DEBRANCHEE",
};

const char* pressStartMsg[] = {
    "PRESS START",
    "DRUCKE START",
    "APPUYEZ SUR START",
};

void EnMag_Init(Actor* thisx, PlayState* play) {
    if (ResourceMgr_IsGameMasterQuest()) {
        EnMag_InitMq(thisx, play);
        if (ResourceMgr_GetGameRegion(0) == GAME_REGION_PAL) {
            drawInnerFunc = EnMag_DrawInnerMq;
        } else {
            drawInnerFunc = EnMag_DrawInnerMqNtsc;
        }
        thisx->update = EnMag_UpdateMq;
    } else {
        EnMag_InitVanilla(thisx, play);
        thisx->update = EnMag_UpdateVanilla;
        if (ResourceMgr_GetGameRegion(0) == GAME_REGION_PAL) {
            drawInnerFunc = EnMag_DrawInnerVanilla;
        } else {
            drawInnerFunc = EnMag_DrawInnerVanillaNtsc;
        }
    }
}

static s16 sDelayTimer = 0;
void EnMag_InitMq(Actor* thisx, PlayState* play) {
    EnMag* this = (EnMag*)thisx;

    YREG(1) = 63;
    YREG(3) = 80;
    YREG(4) = 255;
    YREG(5) = 30;
    YREG(6) = 30;
    YREG(7) = 119;
    YREG(8) = 7;
    YREG(9) = 5;
    YREG(10) = 3;

    VREG(4) = 1;
    VREG(5) = 6;
    VREG(6) = 2;

    this->copyrightAlphaStep = 6;
    this->fadeOutAlphaStep = 10;

    VREG(19) = 99;
    VREG(21) = 9;
    VREG(23) = 10;
    VREG(24) = 8;

    this->effectScroll = 0;
    this->unk_E30C = 0;

    this->effectPrimColor[0] = 0.0f;
    this->effectPrimColor[1] = 100.0f;
    this->effectPrimColor[2] = 170.0f;
    this->effectEnvColor[0] = 0.0f;
    this->effectEnvColor[1] = 100.0f;
    this->effectEnvColor[2] = 0.0f;

    this->effectFadeInTimer = 40;

    this->effectFadeInState = this->effectPrimLodFrac = this->globalState = this->effectAlpha = this->mainAlpha =
        this->subAlpha = this->copyrightAlpha = 0.0f;

    if (gSaveContext.unk_13E7 != 0) {
        this->mainAlpha = 210;
        this->subAlpha = 255;
        this->copyrightAlpha = 255;

        this->effectPrimLodFrac = 128.0f;
        this->effectAlpha = 255.0f;

        this->effectPrimColor[0] = 170;
        this->effectPrimColor[1] = 255.0f;
        this->effectPrimColor[2] = 255.0f;
        this->effectEnvColor[0] = 200.0f;
        this->effectEnvColor[1] = 255.0f;
        this->effectEnvColor[2] = 0;

        gSaveContext.unk_13E7 = 0;
        this->globalState = MAG_STATE_DISPLAY;
        sDelayTimer = 20;
        gSaveContext.transFadeDuration = 1;
        gSaveContext.transWipeSpeed = 255;
    }

    if (ResourceMgr_GetGameRegion(0) == GAME_REGION_PAL) {
        Font_LoadOrderedFont(&this->font);
    } else { // GAME_REGION_NTSC
        Font_LoadOrderedFontNTSC(&this->font);
    }

    this->unk_E316 = 0;
    this->unk_E318 = 0;
    this->unk_E31C = 0;
    this->unk_E320 = 0;
}

void EnMag_InitVanilla(Actor* thisx, PlayState* play) {
    EnMag* this = (EnMag*)thisx;

    YREG(1) = 63;
    YREG(3) = 80;
    YREG(4) = 255;
    YREG(5) = 30;
    YREG(6) = 30;
    YREG(7) = 119;
    YREG(8) = 7;
    YREG(9) = 5;
    YREG(10) = 3;

    VREG(4) = 1;
    VREG(5) = 6;
    VREG(6) = 2;

    this->copyrightAlphaStep = 6;
    this->fadeOutAlphaStep = 10;

    VREG(19) = 99;
    VREG(21) = 9;
    VREG(23) = 10;
    VREG(24) = 8;

    this->effectScroll = 0;
    this->unk_E30C = 0;

    this->effectPrimColor[0] = 0.0f;
    this->effectPrimColor[1] = 100.0f;
    this->effectPrimColor[2] = 170.0f;
    this->effectEnvColor[0] = 0.0f;
    this->effectEnvColor[1] = 100.0f;
    this->effectEnvColor[2] = 0.0f;

    this->effectFadeInTimer = 40;

    this->effectFadeInState = this->effectPrimLodFrac = this->globalState = this->effectAlpha = this->mainAlpha =
        this->subAlpha = this->copyrightAlpha = 0.0f;

    if (gSaveContext.unk_13E7 != 0) {
        this->mainAlpha = 210;
        this->subAlpha = 255;
        this->copyrightAlpha = 255;

        this->effectPrimLodFrac = 128.0f;
        this->effectAlpha = 255.0f;

        this->effectPrimColor[0] = 255.0f;
        this->effectPrimColor[1] = 255.0f;
        this->effectPrimColor[2] = 170.0f;
        this->effectEnvColor[0] = 255.0f;
        this->effectEnvColor[1] = 100.0f;
        this->effectEnvColor[2] = 0.0f;

        gSaveContext.unk_13E7 = 0;
        this->globalState = MAG_STATE_DISPLAY;
        sDelayTimer = 20;
        gSaveContext.transFadeDuration = 1;
        gSaveContext.transWipeSpeed = 255;
    }

    if (ResourceMgr_GetGameRegion(0) == GAME_REGION_PAL) {
        Font_LoadOrderedFont(&this->font);
    } else { // GAME_REGION_NTSC
        Font_LoadOrderedFontNTSC(&this->font);
    }

    this->unk_E316 = 0;
    this->unk_E318 = 0;
    this->unk_E31C = 0;
    this->unk_E320 = 0;
}

void EnMag_Destroy(Actor* thisx, PlayState* play) {
}

void EnMag_UpdateMq(Actor* thisx, PlayState* play) {
    s32 pad[2];
    EnMag* this = (EnMag*)thisx;

    if (gSaveContext.fileNum != 0xFEDC) {
        if (this->globalState < MAG_STATE_DISPLAY) {
            if (CHECK_BTN_ALL(play->state.input[0].press.button, BTN_START) ||
                CHECK_BTN_ALL(play->state.input[0].press.button, BTN_A) ||
                CHECK_BTN_ALL(play->state.input[0].press.button, BTN_B)) {

                Audio_PlaySoundGeneral(NA_SE_SY_PIECE_OF_HEART, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);

                this->mainAlpha = 210;
                this->subAlpha = 255;
                this->copyrightAlpha = 255;

                this->effectPrimLodFrac = 128.0f;
                this->effectAlpha = 255.0f;

                this->effectPrimColor[0] = 170;
                this->effectPrimColor[1] = 255.0f;
                this->effectPrimColor[2] = 255.0f;
                this->effectEnvColor[0] = 200.0f;
                this->effectEnvColor[1] = 255.0f;
                this->effectEnvColor[2] = 0;

                this->globalState = MAG_STATE_DISPLAY;
                sDelayTimer = 20;
                gSaveContext.transFadeDuration = 1;
                gSaveContext.transWipeSpeed = 255;
            }
        } else if (this->globalState >= MAG_STATE_DISPLAY) {
            if (sDelayTimer == 0) {
                if (CHECK_BTN_ALL(play->state.input[0].press.button, BTN_START) ||
                    CHECK_BTN_ALL(play->state.input[0].press.button, BTN_A) ||
                    CHECK_BTN_ALL(play->state.input[0].press.button, BTN_B)) {

                    if (play->transitionTrigger != TRANS_TRIGGER_START) {
                        Audio_SetCutsceneFlag(0);

                        Audio_PlaySoundGeneral(NA_SE_SY_PIECE_OF_HEART, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                               &D_801333E8);

                        gSaveContext.gameMode = 2;
                        play->transitionTrigger = TRANS_TRIGGER_START;
                        play->transitionType = TRANS_TYPE_FADE_BLACK;
                    }

                    this->copyrightAlphaStep = 15;
                    this->fadeOutAlphaStep = 25;
                    this->globalState = MAG_STATE_FADE_OUT;
                }
            } else {
                sDelayTimer--;
            }
        }
    }

    if (this->globalState == MAG_STATE_FADE_IN) {
        if (this->effectFadeInState == 0) {
            this->effectAlpha += 6.375f;
            this->effectPrimLodFrac += 0.8f;

            this->effectPrimColor[0] += 6.375f;
            this->effectPrimColor[1] += 3.875f;
            this->effectPrimColor[2] += 2.125f;
            this->effectEnvColor[0] += 6.375f;
            this->effectEnvColor[1] += 3.875f;

            this->effectFadeInTimer--;

            if (this->effectFadeInTimer == 0) {
                this->effectPrimLodFrac = 32.0f;
                this->effectAlpha = 255.0f;

                this->effectPrimColor[0] = 255.0f;
                this->effectPrimColor[1] = 255.0f;
                this->effectPrimColor[2] = 255.0f;
                this->effectEnvColor[0] = 255.0f;
                this->effectEnvColor[1] = 255.0f;

                this->effectFadeInTimer = 40;
                this->effectFadeInState = 1;
            }
        } else if (this->effectFadeInState == 1) {
            this->effectPrimColor[0] += -2.125f;
            this->effectEnvColor[0] += -1.375f;

            this->effectPrimLodFrac += 2.4f;

            this->effectFadeInTimer--;

            if (this->effectFadeInTimer == 0) {
                this->effectPrimLodFrac = 128.0f;

                this->effectPrimColor[0] = 170.0f;
                this->effectEnvColor[0] = 200.0f;

                this->effectFadeInTimer = 32;
                this->effectFadeInState = 2;
            }
        }

        if (this->effectAlpha > 160) {
            this->mainAlpha += VREG(5);
            if (this->mainAlpha >= 210.0f) {
                this->mainAlpha = 210.0f;
            }

            if (this->mainAlpha >= 210) {
                this->subAlpha += VREG(6);
                if (this->subAlpha >= 255.0f) {
                    this->subAlpha = 255.0f;
                }

                if (this->subAlpha >= 200) {
                    this->copyrightAlpha += this->copyrightAlphaStep;
                    if (this->copyrightAlpha >= 255.0f) {
                        this->copyrightAlpha = 255.0f;
                        this->globalState = MAG_STATE_DISPLAY;
                        sDelayTimer = 20;
                    }
                }
            }
        }
    } else if (this->globalState == MAG_STATE_FADE_OUT) {
        this->effectAlpha -= this->fadeOutAlphaStep;
        if (this->effectAlpha < 0.0f) {
            this->effectAlpha = 0.0f;
        }

        this->mainAlpha -= this->fadeOutAlphaStep;
        if (this->mainAlpha < 0.0f) {
            this->mainAlpha = 0.0f;
        }

        this->subAlpha -= this->fadeOutAlphaStep;
        if (this->subAlpha < 0.0f) {
            this->subAlpha = 0.0f;
        }

        this->copyrightAlpha -= this->copyrightAlphaStep;
        if (this->copyrightAlpha < 0.0f) {
            this->copyrightAlpha = 0.0f;
            this->globalState = MAG_STATE_POST_DISPLAY;
        }
    }

    if (this->globalState == MAG_STATE_INITIAL) {
        if (Flags_GetEnv(play, 3)) {
            this->effectFadeInTimer = 40;
            this->globalState = MAG_STATE_FADE_IN;
        }
    } else if (this->globalState == MAG_STATE_DISPLAY) {
        if (Flags_GetEnv(play, 4)) {
            this->globalState = MAG_STATE_FADE_OUT;
        }
    }
}

void EnMag_UpdateVanilla(Actor* thisx, PlayState* play) {
    s32 pad[2];
    EnMag* this = (EnMag*)thisx;

    if (gSaveContext.fileNum != 0xFEDC) {
        if (this->globalState < MAG_STATE_DISPLAY) {
            if (CHECK_BTN_ALL(play->state.input[0].press.button, BTN_START) ||
                CHECK_BTN_ALL(play->state.input[0].press.button, BTN_A) ||
                CHECK_BTN_ALL(play->state.input[0].press.button, BTN_B)) {

                Audio_PlaySoundGeneral(NA_SE_SY_PIECE_OF_HEART, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);

                this->mainAlpha = 210;
                this->subAlpha = 255;
                this->copyrightAlpha = 255;

                this->effectPrimLodFrac = 128.0f;
                this->effectAlpha = 255.0f;

                this->effectPrimColor[0] = 255.0f;
                this->effectPrimColor[1] = 255.0f;
                this->effectPrimColor[2] = 170.0f;
                this->effectEnvColor[0] = 255.0f;
                this->effectEnvColor[1] = 100.0f;
                this->effectEnvColor[2] = 0.0f;

                this->globalState = MAG_STATE_DISPLAY;
                sDelayTimer = 20;
                gSaveContext.transFadeDuration = 1;
                gSaveContext.transWipeSpeed = 255;
            }
        } else if (this->globalState >= MAG_STATE_DISPLAY) {
            if (sDelayTimer == 0) {
                if (CHECK_BTN_ALL(play->state.input[0].press.button, BTN_START) ||
                    CHECK_BTN_ALL(play->state.input[0].press.button, BTN_A) ||
                    CHECK_BTN_ALL(play->state.input[0].press.button, BTN_B)) {

                    if (play->transitionTrigger != TRANS_TRIGGER_START) {
                        Audio_SetCutsceneFlag(0);

                        Audio_PlaySoundGeneral(NA_SE_SY_PIECE_OF_HEART, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                               &D_801333E8);

                        gSaveContext.gameMode = 2;
                        play->transitionTrigger = TRANS_TRIGGER_START;
                        play->transitionType = TRANS_TYPE_FADE_BLACK;
                    }

                    this->copyrightAlphaStep = 15;
                    this->fadeOutAlphaStep = 25;
                    this->globalState = MAG_STATE_FADE_OUT;
                }
            } else {
                sDelayTimer--;
            }
        }
    }

    if (this->globalState == MAG_STATE_FADE_IN) {
        if (this->effectFadeInState == 0) {
            this->effectAlpha += 6.375f;
            this->effectPrimLodFrac += 0.8f;

            this->effectPrimColor[0] += 6.375f;
            this->effectPrimColor[1] += 3.875f;
            this->effectPrimColor[2] += 2.125f;
            this->effectEnvColor[0] += 6.375f;
            this->effectEnvColor[1] += 3.875f;

            this->effectFadeInTimer--;

            if (this->effectFadeInTimer == 0) {
                this->effectPrimLodFrac = 32.0f;
                this->effectAlpha = 255.0f;

                this->effectPrimColor[0] = 255.0f;
                this->effectPrimColor[1] = 255.0f;
                this->effectPrimColor[2] = 255.0f;
                this->effectEnvColor[0] = 255.0f;
                this->effectEnvColor[1] = 255.0f;

                this->effectFadeInTimer = 40;
                this->effectFadeInState = 1;
            }
        } else if (this->effectFadeInState == 1) {
            this->effectPrimColor[2] += -2.125f;
            this->effectEnvColor[1] += -3.875f;

            this->effectPrimLodFrac += 2.4f;

            this->effectFadeInTimer--;

            if (this->effectFadeInTimer == 0) {
                this->effectPrimLodFrac = 128.0f;

                this->effectPrimColor[2] = 170.0f;
                this->effectEnvColor[1] = 100.0f;

                this->effectFadeInTimer = 32;
                this->effectFadeInState = 2;
            }
        }

        if (this->effectAlpha > 160) {
            this->mainAlpha += VREG(5);
            if (this->mainAlpha >= 210.0f) {
                this->mainAlpha = 210.0f;
            }

            if (this->mainAlpha >= 210) {
                this->subAlpha += VREG(6);
                if (this->subAlpha >= 255.0f) {
                    this->subAlpha = 255.0f;
                }

                if (this->subAlpha >= 200) {
                    this->copyrightAlpha += this->copyrightAlphaStep;
                    if (this->copyrightAlpha >= 255.0f) {
                        this->copyrightAlpha = 255.0f;
                        this->globalState = MAG_STATE_DISPLAY;
                        sDelayTimer = 20;
                    }
                }
            }
        }
    } else if (this->globalState == MAG_STATE_FADE_OUT) {
        this->effectAlpha -= this->fadeOutAlphaStep;
        if (this->effectAlpha < 0.0f) {
            this->effectAlpha = 0.0f;
        }

        this->mainAlpha -= this->fadeOutAlphaStep;
        if (this->mainAlpha < 0.0f) {
            this->mainAlpha = 0.0f;
        }

        this->subAlpha -= this->fadeOutAlphaStep;
        if (this->subAlpha < 0.0f) {
            this->subAlpha = 0.0f;
        }

        this->copyrightAlpha -= this->copyrightAlphaStep;
        if (this->copyrightAlpha < 0.0f) {
            this->copyrightAlpha = 0.0f;
            this->globalState = MAG_STATE_POST_DISPLAY;
        }
    }

    if (this->globalState == MAG_STATE_INITIAL) {
        if (Flags_GetEnv(play, 3)) {
            this->effectFadeInTimer = 40;
            this->globalState = MAG_STATE_FADE_IN;
        }
    } else if (this->globalState == MAG_STATE_DISPLAY) {
        if (Flags_GetEnv(play, 4)) {
            this->globalState = MAG_STATE_FADE_OUT;
        }
    }
}

void EnMag_DrawTextureI8(Gfx** gfxp, const void* texture, s16 texWidth, s16 texHeight, s16 rectLeft, s16 rectTop,
                         s16 rectWidth, s16 rectHeight, u16 dsdx, u16 dtdy) {
    Gfx* gfx = *gfxp;

    gDPLoadTextureBlock(gfx++, texture, G_IM_FMT_I, G_IM_SIZ_8b, texWidth, texHeight, 0, G_TX_NOMIRROR | G_TX_WRAP,
                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

    gSPTextureRectangle(gfx++, rectLeft << 2, rectTop << 2, (rectLeft + rectWidth) << 2, (rectTop + rectHeight) << 2,
                        G_TX_RENDERTILE, 0, 0, dsdx, dtdy);

    *gfxp = gfx;
}

void EnMag_DrawEffectTextures(Gfx** gfxp, const void* maskTex, void* effectTex, s16 maskWidth, s16 maskHeight,
                              s16 effectWidth, s16 effectHeight, s16 rectLeft, s16 rectTop, s16 rectWidth,
                              s16 rectHeight, u16 dsdx, u16 dtdy, u16 shifts, u16 shiftt, u16 flag, EnMag* this) {
    Gfx* gfx = *gfxp;

    gDPLoadMultiBlock_4b(gfx++, maskTex, 0x0000, 0, G_IM_FMT_I, maskWidth, maskHeight, 0, G_TX_NOMIRROR | G_TX_WRAP,
                         G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

    if (!flag) {
        gDPLoadMultiBlock(gfx++, effectTex, 0x0100, 1, G_IM_FMT_I, G_IM_SIZ_8b, effectWidth, effectHeight, 0,
                          G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, 5, shifts, shiftt);

        gDPSetTileSize(gfx++, 1, 0, this->effectScroll & 0x7F, 0x7C, (this->effectScroll & 0x7F) + 0x7C);
    }

    gSPTextureRectangle(gfx++, rectLeft << 2, rectTop << 2, (rectLeft + rectWidth) << 2, (rectTop + rectHeight) << 2,
                        G_TX_RENDERTILE, 0, 0, dsdx, dtdy);

    *gfxp = gfx;
}

void EnMag_DrawImageRGBA32(Gfx** gfxp, s16 centerX, s16 centerY, const char* source, u32 width, u32 height) {
    Gfx* gfx = *gfxp;
    u8* curTexture;
    s32 textureCount;
    u32 rectLeft;
    u32 rectTop;
    u32 textureHeight;
    s32 remainingSize;
    s32 textureSize;
    s32 pad;
    s32 i;

    Gfx_SetupDL_56Ptr(&gfx);

    rectLeft = centerX - (width / 2);
    rectTop = centerY - (height / 2);

    gDPSetTileCustom(gfx++, G_IM_FMT_RGBA, G_IM_SIZ_32b, width, height, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                     G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

    gDPSetTextureImage(gfx++, G_IM_FMT_RGBA, G_IM_SIZ_32b, width, source);

    gDPLoadSync(gfx++);
    gDPLoadTile(gfx++, G_TX_LOADTILE, 0, 0, (width - 1) << 2, (height - 1) << 2);

    gSPTextureRectangle(gfx++, rectLeft << 2, rectTop << 2, (rectLeft + (s32)width) << 2, (rectTop + height) << 2,
                        G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);

    *gfxp = gfx;
}

void EnMag_DrawCharTexture(Gfx** gfxp, u8* texture, s32 rectLeft, s32 rectTop) {
    Gfx* gfx = *gfxp;

    YREG(0) = 1024.0f / (YREG(1) / 100.0f);
    YREG(2) = 16.0f * (YREG(1) / 100.0f);

    gDPLoadTextureBlock_4b(gfx++, texture, G_IM_FMT_I, 16, 16, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                           G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

    gSPTextureRectangle(gfx++, rectLeft << 2, rectTop << 2, (rectLeft + YREG(2)) << 2, (rectTop + YREG(2)) << 2,
                        G_TX_RENDERTILE, 0, 0, YREG(0), YREG(0));

    *gfxp = gfx;
}

s16 GetCharArraySize(const char* str) {
    s16 length = 0;
    char c = str[length];

    while (c != 0) {
        length++;
        c = str[length];
    }

    return length;
}

static void EnMag_SetCopyValues(const char** copy_tex, u16* copy_width, u16* copy_xl, u16* copy_xh) {
    u32 platform = ResourceMgr_GetGamePlatform(0);
    switch (platform) {
        case GAME_PLATFORM_N64:
            *copy_tex = gTitleCopyright1998Tex;
            *copy_width = 128;
            *copy_xl = 376;
            *copy_xh = 888;
            break;
        default:
            *copy_tex = gTitleCopyright19982003Tex;
            *copy_width = 160;
            *copy_xl = 312;
            *copy_xh = 952;
            break;
    }
}

void EnMag_DrawInnerMq(Actor* thisx, PlayState* play, Gfx** gfxp) {
    static s16 textAlpha = 0;
    static s16 textFadeDirection = 0;
    static s16 textFadeTimer = 0;
    static void* effectMaskTextures[] = {
        gTitleEffectMask00Tex, gTitleEffectMask01Tex, gTitleEffectMask02Tex,
        gTitleEffectMask10Tex, gTitleEffectMask11Tex, gTitleEffectMask12Tex,
        gTitleEffectMask20Tex, gTitleEffectMask21Tex, gTitleEffectMask22Tex,
    };
    EnMag* this = (EnMag*)thisx;
    Font* font = &this->font;
    s32 pad;
    Gfx* gfx = *gfxp;
    u16 i, j, k;
    u16 rectLeft;
    u16 rectTop;
    u16 length;
    int lang = LANGUAGE_ENG;
    if (CVarGetInteger(CVAR_SETTING("TitleScreenTranslation"), 0)) {
        lang = gSaveContext.language;
    }

    const char* copy_tex = NULL;
    u16 copy_width;
    u16 copy_xl;
    u16 copy_xh;
    EnMag_SetCopyValues(&copy_tex, &copy_width, &copy_xl, &copy_xh);

    gSPSegment(gfx++, 0x06, play->objectCtx.status[this->actor.objBankIndex].segment);

    Gfx_SetupDL_39Ptr(&gfx);

    this->effectScroll -= 2;

    gDPSetCycleType(gfx++, G_CYC_2CYCLE);
    gDPSetAlphaCompare(gfx++, G_AC_THRESHOLD);
    gDPSetRenderMode(gfx++, G_RM_PASS, G_RM_CLD_SURF2);
    gDPSetCombineLERP(gfx++, TEXEL1, PRIMITIVE, PRIM_LOD_FRAC, TEXEL0, TEXEL1, 1, PRIM_LOD_FRAC, TEXEL0, PRIMITIVE,
                      ENVIRONMENT, COMBINED, ENVIRONMENT, COMBINED, 0, PRIMITIVE, 0);

    gDPSetPrimColor(gfx++, 0, (s16)this->effectPrimLodFrac, (s16)this->effectPrimColor[0],
                    (s16)this->effectPrimColor[1], (s16)this->effectPrimColor[2], (s16)this->effectAlpha);
    gDPSetEnvColor(gfx++, (s16)this->effectEnvColor[0], (s16)this->effectEnvColor[1], (s16)this->effectEnvColor[2],
                   255);

    if ((s16)this->effectPrimLodFrac != 0) {
        for (k = 0, i = 0, rectTop = 0; i < 3; i++, rectTop += 64) {
            for (j = 0, rectLeft = 56; j < 3; j++, k++, rectLeft += 64) {
                EnMag_DrawEffectTextures(&gfx, effectMaskTextures[k], gTitleFlameEffectTex, 64, 64, 32, 32, rectLeft,
                                         rectTop, 64, 64, 1024, 1024, 1, 1, k, this);
            }
        }
    }

    gDPSetPrimColor(gfx++, 0, 0, 255, 255, 255, (s16)this->mainAlpha);

    if ((s16)this->mainAlpha != 0) {
        EnMag_DrawImageRGBA32(&gfx, 152, 100, (u8*)gTitleZeldaShieldLogoMQTex, 160, 160);
    }

    Gfx_SetupDL_39Ptr(&gfx);

    gDPPipeSync(gfx++);
    gDPSetAlphaCompare(gfx++, G_AC_NONE);
    gDPSetCombineLERP(gfx++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0, PRIMITIVE,
                      ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);

    if ((s16)this->mainAlpha < 100) {
        gDPSetRenderMode(gfx++, G_RM_CLD_SURF, G_RM_CLD_SURF2);
    } else {
        gDPSetRenderMode(gfx++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
    }

    gDPSetPrimColor(gfx++, 0, 0, 0, 0, 0, (s16)this->mainAlpha);
    gDPSetEnvColor(gfx++, 0, 0, 100, 255);

    if ((s16)this->mainAlpha != 0) {
        EnMag_DrawTextureI8(&gfx, gTitleTheLegendOfTextTex, 72, 8, 146, 73, 72, 8, 1024, 1024);
        EnMag_DrawTextureI8(&gfx, gTitleOcarinaOfTimeTMTextTex, 96, 8, 144, 127, 96, 8, 1024, 1024);

        gDPPipeSync(gfx++);
        gDPSetPrimColor(gfx++, 0, 0, 100, 150, 255, (s16)this->mainAlpha);
        gDPSetEnvColor(gfx++, 20, 80, 160, 255);

        EnMag_DrawTextureI8(&gfx, gTitleTheLegendOfTextTex, 72, 8, 145, 72, 72, 8, 1024, 1024);
        EnMag_DrawTextureI8(&gfx, gTitleOcarinaOfTimeTMTextTex, 96, 8, 143, 126, 96, 8, 1024, 1024);

        gDPPipeSync(gfx++);
        gDPSetPrimColor(gfx++, 0, 0, 255, 255, 255, (s16)this->subAlpha);
        EnMag_DrawImageRGBA32(&gfx, 174, 145, gTitleMasterQuestSubtitleTex, 128, 32);
    }

    Gfx_SetupDL_39Ptr(&gfx);

    gDPSetAlphaCompare(gfx++, G_AC_NONE);
    gDPSetCombineMode(gfx++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);

    gDPSetPrimColor(gfx++, 0, 0, (s16)this->copyrightAlpha, (s16)this->copyrightAlpha, (s16)this->copyrightAlpha,
                    (s16)this->copyrightAlpha);

    if ((s16)this->copyrightAlpha != 0) {
        gDPLoadTextureBlock(gfx++, copy_tex, G_IM_FMT_IA, G_IM_SIZ_8b, copy_width, 16, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                            G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

        gSPTextureRectangle(gfx++, copy_xl, 792, copy_xh, 856, G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);
    }

    if (gSaveContext.fileNum == 0xFEDC) {
        // Draw "NO CONTROLLER" Text
        textAlpha = textFadeTimer * 10;
        if (textAlpha >= 255) {
            textAlpha = 255;
        }

        // Text Shadow
        gDPPipeSync(gfx++);
        gDPSetCombineLERP(gfx++, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE,
                          0);
        gDPSetPrimColor(gfx++, 0, 0, 0, 0, 0, textAlpha);

        length = GetCharArraySize(noControllerMsg[lang]);
        rectLeft = VREG(19) + 1 + ((length - 13) * -3);
        for (i = 0; i < length; i++) {
            EnMag_DrawCharTexture(&gfx, font->fontBuf + (noControllerMsg[lang][i] - '\x37') * FONT_CHAR_TEX_SIZE,
                                  rectLeft, YREG(10) + 172);
            if (noControllerMsg[lang][i] == ' ') {
                rectLeft += VREG(23);
            } else {
                rectLeft += VREG(21);
            }
        }

        // Actual Text
        gDPPipeSync(gfx++);
        gDPSetPrimColor(gfx++, 0, 0, 100, 255, 255, textAlpha);

        rectLeft = VREG(19) + ((length - 13) * -3);
        for (i = 0; i < length; i++) {
            EnMag_DrawCharTexture(&gfx, font->fontBuf + (noControllerMsg[lang][i] - '\x37') * FONT_CHAR_TEX_SIZE,
                                  rectLeft, YREG(10) + 171);
            if (noControllerMsg[lang][i] == ' ') {
                rectLeft += VREG(23);
            } else {
                rectLeft += VREG(21);
            }
        }
    } else if (this->copyrightAlpha >= 200.0f) {
        // Draw "PRESS START" Text
        textAlpha = textFadeTimer * 10;
        if (textAlpha >= 255) {
            textAlpha = 255;
        }

        // Text Shadow
        gDPPipeSync(gfx++);
        gDPSetCombineLERP(gfx++, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE,
                          0);
        gDPSetPrimColor(gfx++, 0, 0, 0, 0, 0, textAlpha);

        length = GetCharArraySize(pressStartMsg[lang]);
        rectLeft = YREG(7) + 1 + ((length - 11) * -3);
        for (i = 0; i < length; i++) {
            EnMag_DrawCharTexture(&gfx, font->fontBuf + (pressStartMsg[lang][i] - '\x37') * FONT_CHAR_TEX_SIZE,
                                  rectLeft, YREG(10) + 172);
            if (pressStartMsg[lang][i] == ' ') {
                rectLeft += YREG(9);
            } else {
                rectLeft += YREG(8);
            }
        }

        // Actual Text
        gDPPipeSync(gfx++);
        gDPSetPrimColor(gfx++, 0, 0, YREG(4), YREG(5), YREG(6), textAlpha);

        rectLeft = YREG(7) + ((length - 11) * -3);
        for (i = 0; i < length; i++) {
            EnMag_DrawCharTexture(&gfx, font->fontBuf + (pressStartMsg[lang][i] - '\x37') * FONT_CHAR_TEX_SIZE,
                                  rectLeft, YREG(10) + 171);
            if (pressStartMsg[lang][i] == ' ') {
                rectLeft += YREG(9);
            } else {
                rectLeft += YREG(8);
            }
        }
    }

    if (textFadeDirection != 0) {
        if (--textFadeTimer == 0) {
            textFadeDirection = 0;
        }
    } else {
        if (++textFadeTimer >= 26) {
            textFadeDirection = 1;
        }
    }

    *gfxp = gfx;
}

void EnMag_DrawInnerVanilla(Actor* thisx, PlayState* play, Gfx** gfxp) {
    static s16 textAlpha = 0;
    static s16 textFadeDirection = 0;
    static s16 textFadeTimer = 0;
    static const void* effectMaskTextures[] = {
        gTitleEffectMask00Tex, gTitleEffectMask01Tex, gTitleEffectMask02Tex,
        gTitleEffectMask10Tex, gTitleEffectMask11Tex, gTitleEffectMask12Tex,
        gTitleEffectMask20Tex, gTitleEffectMask21Tex, gTitleEffectMask22Tex,
    };
    EnMag* this = (EnMag*)thisx;
    Font* font = &this->font;
    s32 pad;
    Gfx* gfx = *gfxp;
    u16 i, j, k;
    u16 rectLeft;
    u16 rectTop;
    u16 length;
    int lang = LANGUAGE_ENG;
    if (CVarGetInteger(CVAR_SETTING("TitleScreenTranslation"), 0)) {
        lang = gSaveContext.language;
    }

    const char* copy_tex = NULL;
    u16 copy_width;
    u16 copy_xl;
    u16 copy_xh;
    EnMag_SetCopyValues(&copy_tex, &copy_width, &copy_xl, &copy_xh);

    gSPSegment(gfx++, 0x06, play->objectCtx.status[this->actor.objBankIndex].segment);

    Gfx_SetupDL_39Ptr(&gfx);

    this->effectScroll -= 2;

    gDPSetCycleType(gfx++, G_CYC_2CYCLE);
    gDPSetAlphaCompare(gfx++, G_AC_THRESHOLD);
    gDPSetRenderMode(gfx++, G_RM_PASS, G_RM_CLD_SURF2);
    gDPSetCombineLERP(gfx++, TEXEL1, PRIMITIVE, PRIM_LOD_FRAC, TEXEL0, TEXEL1, 1, PRIM_LOD_FRAC, TEXEL0, PRIMITIVE,
                      ENVIRONMENT, COMBINED, ENVIRONMENT, COMBINED, 0, PRIMITIVE, 0);

    gDPSetPrimColor(gfx++, 0, (s16)this->effectPrimLodFrac, (s16)this->effectPrimColor[0],
                    (s16)this->effectPrimColor[1], (s16)this->effectPrimColor[2], (s16)this->effectAlpha);
    gDPSetEnvColor(gfx++, (s16)this->effectEnvColor[0], (s16)this->effectEnvColor[1], (s16)this->effectEnvColor[2],
                   255);

    if ((s16)this->effectPrimLodFrac != 0) {
        for (k = 0, i = 0, rectTop = 0; i < 3; i++, rectTop += 64) {
            for (j = 0, rectLeft = 64; j < 3; j++, k++, rectLeft += 64) {
                EnMag_DrawEffectTextures(&gfx, effectMaskTextures[k], gTitleFlameEffectTex, 64, 64, 32, 32, rectLeft,
                                         rectTop, 64, 64, 1024, 1024, 1, 1, k, this);
            }
        }
    }

    gDPSetPrimColor(gfx++, 0, 0, 255, 255, 255, (s16)this->mainAlpha);

    if ((s16)this->mainAlpha != 0) {
        EnMag_DrawImageRGBA32(&gfx, 160, 100, (u8*)gTitleZeldaShieldLogoTex, 160, 160);
    }

    Gfx_SetupDL_39Ptr(&gfx);

    gDPPipeSync(gfx++);
    gDPSetAlphaCompare(gfx++, G_AC_NONE);
    gDPSetCombineLERP(gfx++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0, PRIMITIVE,
                      ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);

    if ((s16)this->mainAlpha < 100) {
        gDPSetRenderMode(gfx++, G_RM_CLD_SURF, G_RM_CLD_SURF2);
    } else {
        gDPSetRenderMode(gfx++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
    }

    gDPSetPrimColor(gfx++, 0, 0, 0, 0, 0, (s16)this->mainAlpha);
    gDPSetEnvColor(gfx++, 100, 0, 100, 255);

    if ((s16)this->mainAlpha != 0) {
        EnMag_DrawTextureI8(&gfx, gTitleTheLegendOfTextTex, 72, 8, 154, 73, 72, 8, 1024, 1024);
        EnMag_DrawTextureI8(&gfx, gTitleOcarinaOfTimeTMTextTex, 96, 8, 152, 127, 96, 8, 1024, 1024);

        gDPPipeSync(gfx++);
        gDPSetPrimColor(gfx++, 0, 0, 200, 200, 150, (s16)this->mainAlpha);
        gDPSetEnvColor(gfx++, 100, 100, 50, 255);

        EnMag_DrawTextureI8(&gfx, gTitleTheLegendOfTextTex, 72, 8, 153, 72, 72, 8, 1024, 1024);
        EnMag_DrawTextureI8(&gfx, gTitleOcarinaOfTimeTMTextTex, 96, 8, 151, 126, 96, 8, 1024, 1024);
    }

    Gfx_SetupDL_39Ptr(&gfx);

    gDPSetAlphaCompare(gfx++, G_AC_NONE);
    gDPSetCombineMode(gfx++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);

    gDPSetPrimColor(gfx++, 0, 0, (s16)this->copyrightAlpha, (s16)this->copyrightAlpha, (s16)this->copyrightAlpha,
                    (s16)this->copyrightAlpha);

    if ((s16)this->copyrightAlpha != 0) {
        gDPLoadTextureBlock(gfx++, copy_tex, G_IM_FMT_IA, G_IM_SIZ_8b, copy_width, 16, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                            G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

        gSPTextureRectangle(gfx++, copy_xl, 792, copy_xh, 856, G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);
    }

    if (gSaveContext.fileNum == 0xFEDC) {
        // Draw "NO CONTROLLER" Text
        textAlpha = textFadeTimer * 10;
        if (textAlpha >= 255) {
            textAlpha = 255;
        }

        // Text Shadow
        gDPPipeSync(gfx++);
        gDPSetCombineLERP(gfx++, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE,
                          0);
        gDPSetPrimColor(gfx++, 0, 0, 0, 0, 0, textAlpha);

        length = GetCharArraySize(noControllerMsg[lang]);
        rectLeft = VREG(19) + 1 + ((length - 13) * -3);
        for (i = 0; i < length; i++) {
            EnMag_DrawCharTexture(&gfx, font->fontBuf + (noControllerMsg[lang][i] - '\x37') * FONT_CHAR_TEX_SIZE,
                                  rectLeft, YREG(10) + 172);
            if (noControllerMsg[lang][i] == ' ') {
                rectLeft += VREG(23);
            } else {
                rectLeft += VREG(21);
            }
        }

        // Actual Text
        gDPPipeSync(gfx++);
        gDPSetPrimColor(gfx++, 0, 0, 100, 255, 255, textAlpha);

        rectLeft = VREG(19) + ((length - 13) * -3);
        for (i = 0; i < length; i++) {
            EnMag_DrawCharTexture(&gfx, font->fontBuf + (noControllerMsg[lang][i] - '\x37') * FONT_CHAR_TEX_SIZE,
                                  rectLeft, YREG(10) + 171);
            if (noControllerMsg[lang][i] == ' ') {
                rectLeft += VREG(23);
            } else {
                rectLeft += VREG(21);
            }
        }
    } else if (this->copyrightAlpha >= 200.0f) {
        // Draw "PRESS START" Text
        textAlpha = textFadeTimer * 10;
        if (textAlpha >= 255) {
            textAlpha = 255;
        }

        // Text Shadow
        gDPPipeSync(gfx++);
        gDPSetCombineLERP(gfx++, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE,
                          0);
        gDPSetPrimColor(gfx++, 0, 0, 0, 0, 0, textAlpha);

        length = GetCharArraySize(pressStartMsg[lang]);
        rectLeft = YREG(7) + 1 + ((length - 11) * -3);
        for (i = 0; i < length; i++) {
            EnMag_DrawCharTexture(&gfx, font->fontBuf + (pressStartMsg[lang][i] - '\x37') * FONT_CHAR_TEX_SIZE,
                                  rectLeft, YREG(10) + 172);
            if (pressStartMsg[lang][i] == ' ') {
                rectLeft += YREG(9);
            } else {
                rectLeft += YREG(8);
            }
        }

        // Actual Text
        gDPPipeSync(gfx++);
        gDPSetPrimColor(gfx++, 0, 0, YREG(4), YREG(5), YREG(6), textAlpha);

        rectLeft = YREG(7) + ((length - 11) * -3);
        for (i = 0; i < length; i++) {
            EnMag_DrawCharTexture(&gfx, font->fontBuf + (pressStartMsg[lang][i] - '\x37') * FONT_CHAR_TEX_SIZE,
                                  rectLeft, YREG(10) + 171);
            if (pressStartMsg[lang][i] == ' ') {
                rectLeft += YREG(9);
            } else {
                rectLeft += YREG(8);
            }
        }
    }

    if (textFadeDirection != 0) {
        if (--textFadeTimer == 0) {
            textFadeDirection = 0;
        }
    } else {
        if (++textFadeTimer >= 26) {
            textFadeDirection = 1;
        }
    }

    *gfxp = gfx;
}

void EnMag_DrawInnerMqNtsc(Actor* thisx, PlayState* play, Gfx** gfxp) {
    // TODO: Implement MQ NTSC
    EnMag_DrawInnerMq(thisx, play, gfxp);
}

void EnMag_DrawInnerVanillaNtsc(Actor* thisx, PlayState* play, Gfx** gfxp) {
    static s16 textAlpha = 0;
    static s16 textFadeDirection = 0;
    static s16 textFadeTimer = 0;
    static u8 noControllerFontIndices[12] = {
        0xB8, 0xB9, 0xAD, 0xB9, 0xB8, 0xBE, 0xBC, 0xB9, 0xB6, 0xB6, 0xAF, 0xBC
    };
    
    static u8 pressStartFontIndices[10] = {
        0xBA, 0xBC, 0xAF, 0xBD, 0xBD, 0xBD, 0xBE, 0xAB, 0xBC, 0xBE,
    };
    static void* effectMaskTextures[] = {
        gTitleEffectMask00Tex, gTitleEffectMask01Tex, gTitleEffectMask02Tex,
        gTitleEffectMask10Tex, gTitleEffectMask11Tex, gTitleEffectMask12Tex,
        gTitleEffectMask20Tex, gTitleEffectMask21Tex, gTitleEffectMask22Tex,
    };
    EnMag* this = (EnMag*)thisx;
    Font* font = &this->font;
    // s32 pad;
    s32 textLocalAlpha;
    Gfx* gfx = *gfxp;
    u16 i, j, k;
    u16 rectLeft;
    u16 rectTop;

    const char* copy_tex = NULL;
    u16 copy_width;
    u16 copy_xl;
    u16 copy_xh;
    EnMag_SetCopyValues(&copy_tex, &copy_width, &copy_xl, &copy_xh);

    gSPSegment(gfx++, 0x06, play->objectCtx.status[this->actor.objBankIndex].segment);

    Gfx_SetupDL_39Ptr(&gfx);

    this->effectScroll -= 2;

    gDPSetCycleType(gfx++, G_CYC_2CYCLE);
    gDPSetAlphaCompare(gfx++, G_AC_THRESHOLD);
    gDPSetRenderMode(gfx++, G_RM_PASS, G_RM_CLD_SURF2);
    gDPSetCombineLERP(gfx++, TEXEL1, PRIMITIVE, PRIM_LOD_FRAC, TEXEL0, TEXEL1, 1, PRIM_LOD_FRAC, TEXEL0, PRIMITIVE,
                      ENVIRONMENT, COMBINED, ENVIRONMENT, COMBINED, 0, PRIMITIVE, 0);

    gDPSetPrimColor(gfx++, 0, (s16)this->effectPrimLodFrac, (s16)this->effectPrimColor[0],
                    (s16)this->effectPrimColor[1], (s16)this->effectPrimColor[2], (s16)this->effectAlpha);
    gDPSetEnvColor(gfx++, (s16)this->effectEnvColor[0], (s16)this->effectEnvColor[1], (s16)this->effectEnvColor[2],
                   255);

    if ((s16)this->effectPrimLodFrac != 0) {
        for (k = 0, i = 0, rectTop = 0; i < 3; i++, rectTop += 64) {
            for (j = 0, rectLeft = 64; j < 3; j++, k++, rectLeft += 64) {
                EnMag_DrawEffectTextures(&gfx, effectMaskTextures[k], gTitleFlameEffectTex, 64, 64, 32, 32, rectLeft,
                                         rectTop, 64, 64, 1024, 1024, 1, 1, k, this);
            }
        }
    }

    gDPSetPrimColor(gfx++, 0, 0, 255, 255, 255, (s16)this->mainAlpha);

    if ((s16)this->mainAlpha != 0) {
        EnMag_DrawImageRGBA32(&gfx, 160, 100, (u8*)gTitleZeldaShieldLogoTex, 160, 160);
        // #region [SoH] Function below would display `disk` if the n64dd was used
        // EnMag_DrawTitleDisk(&gfx, 160, 100, (u32)this->mainAlpha);
    }

    Gfx_SetupDL_39Ptr(&gfx);

    gDPPipeSync(gfx++);
    gDPSetAlphaCompare(gfx++, G_AC_NONE);
    gDPSetCombineLERP(gfx++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0, PRIMITIVE,
                      ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);

    if ((s16)this->mainAlpha < 100) {
        gDPSetRenderMode(gfx++, G_RM_CLD_SURF, G_RM_CLD_SURF2);
    } else {
        gDPSetRenderMode(gfx++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
    }

    gDPSetPrimColor(gfx++, 0, 0, 0, 0, 0, (s16)this->mainAlpha);

    gDPSetEnvColor(gfx++, 100, 0, 100, 255);

    if ((s16)this->mainAlpha != 0) {
        EnMag_DrawTextureI8(&gfx, gTitleTheLegendOfTextTex, 72, 8, 154, 73, 72, 8, 1024, 1024);
        EnMag_DrawTextureI8(&gfx, gTitleOcarinaOfTimeTMTextTex, 96, 8, 152, 127, 96, 8, 1024, 1024);

        gDPPipeSync(gfx++);

        gDPSetPrimColor(gfx++, 0, 0, 200, 200, 150, (s16)this->mainAlpha);
        gDPSetEnvColor(gfx++, 100, 100, 50, 255);

        EnMag_DrawTextureI8(&gfx, gTitleTheLegendOfTextTex, 72, 8, 153, 72, 72, 8, 1024, 1024);
        EnMag_DrawTextureI8(&gfx, gTitleOcarinaOfTimeTMTextTex, 96, 8, 151, 126, 96, 8, 1024, 1024);
    }

    if (gSaveContext.language == LANGUAGE_JPN) {
        this->unk_E30C++;
        gDPPipeSync(gfx++);
        gDPSetCycleType(gfx++, G_CYC_2CYCLE);
        if ((s16)this->subAlpha < 100) {
            gDPSetRenderMode(gfx++, G_RM_PASS, G_RM_CLD_SURF2);
        } else {
            gDPSetRenderMode(gfx++, G_RM_PASS, G_RM_XLU_SURF2);
        }
        gDPSetCombineLERP(gfx++, TEXEL1, PRIMITIVE, PRIM_LOD_FRAC, TEXEL0, 0, 0, 0, TEXEL0, PRIMITIVE, ENVIRONMENT, COMBINED, ENVIRONMENT, COMBINED, 0, PRIMITIVE, 0);
        gDPSetPrimColor(gfx++, 0, 0x80, 255, 255, 170, (s16)this->subAlpha);
        gDPSetEnvColor(gfx++, 255, 150, 0, 255);
        if ((s16)this->subAlpha != 0) {
            gDPLoadTextureBlock(gfx++, gTitleTitleJPNTex, G_IM_FMT_I, G_IM_SIZ_8b, 128, 16, 0,
                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOLOD);
            gDPLoadMultiBlock(gfx++, gTitleFlameEffectTex, 0x100, 1, G_IM_FMT_I, G_IM_SIZ_8b, 32, 32, 0,
                          G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 5, 5, 2, 1);
            gDPSetTileSize(gfx++, 1, this->unk_E30C & 0x7F, this->effectScroll & 0x7F, (this->unk_E30C & 0x7F) + ((32 - 1) << 2), (this->effectScroll & 0x7F) + ((32 - 1) << 2));
            gSPTextureRectangle(gfx++, 424, 576, 424 + 512, 576 + 64, G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);
        }
    }

    Gfx_SetupDL_39Ptr(&gfx);

    gDPSetAlphaCompare(gfx++, G_AC_NONE);
    gDPSetCombineMode(gfx++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);

    gDPSetPrimColor(gfx++, 0, 0, (s16)this->copyrightAlpha, (s16)this->copyrightAlpha, (s16)this->copyrightAlpha,
                    (s16)this->copyrightAlpha);

    if ((s16)this->copyrightAlpha != 0) {
        gDPLoadTextureBlock(gfx++, copy_tex, G_IM_FMT_IA, G_IM_SIZ_8b, copy_width, 16, 0,
                            G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK,
                            G_TX_NOLOD, G_TX_NOLOD);

        gSPTextureRectangle(gfx++, copy_xl, 198 << 2, copy_xh, 214 << 2, G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);
    }

    if (gSaveContext.fileNum == 0xFEDC) {
        // Draw "NO CONTROLLER" Text
        textAlpha = textFadeTimer * 10;
        if (textAlpha >= 255) {
            textAlpha = 255;
        }

        // Text Shadow
        gDPPipeSync(gfx++);
        gDPSetCombineLERP(gfx++, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE,
                          0);
        gDPSetPrimColor(gfx++, 0, 0, 0, 0, 0, textAlpha);

        rectLeft = VREG(19) + 1;
        for (i = 0; i < ARRAY_COUNT(noControllerFontIndices); i++) {
            EnMag_DrawCharTexture(&gfx, font->fontBuf + noControllerFontIndices[i] * FONT_CHAR_TEX_SIZE, rectLeft,
                                  YREG(10) + 172);
            rectLeft += VREG(21);
            if (i == 1) {
                rectLeft += VREG(23);
            }
        }

        // Actual Text
        gDPPipeSync(gfx++);
        gDPSetPrimColor(gfx++, 0, 0, 100, 255, 255, textAlpha);

        rectLeft = VREG(19);
        for (i = 0; i < ARRAY_COUNT(noControllerFontIndices); i++) {
            EnMag_DrawCharTexture(&gfx, font->fontBuf + noControllerFontIndices[i] * FONT_CHAR_TEX_SIZE, rectLeft,
                                  YREG(10) + 171);
            rectLeft += VREG(21);
            if (i == 1) {
                rectLeft += VREG(23);
            }
        }
    } else if (this->copyrightAlpha >= 200.0f) {
        // Draw "PRESS START" Text
        textAlpha = textFadeTimer * 10;
        if (textAlpha >= 255) {
            textAlpha = 255;
        }

        // Text Shadow
        gDPPipeSync(gfx++);
        gDPSetCombineLERP(gfx++, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE,
                          0);
        gDPSetPrimColor(gfx++, 0, 0, 0, 0, 0, textAlpha);

        rectLeft = YREG(7) + 1;
        for (i = 0; i < ARRAY_COUNT(pressStartFontIndices); i++) {
            EnMag_DrawCharTexture(&gfx, font->fontBuf + pressStartFontIndices[i] * FONT_CHAR_TEX_SIZE, rectLeft,
                                  YREG(10) + 172);
            rectLeft += YREG(8);
            if (i == 4) {
                rectLeft += YREG(9);
            }
        }

        // Actual Text
        gDPPipeSync(gfx++);
        gDPSetPrimColor(gfx++, 0, 0, YREG(4), YREG(5), YREG(6), textAlpha);

        rectLeft = YREG(7);
        for (i = 0; i < ARRAY_COUNT(pressStartFontIndices); i++) {
            EnMag_DrawCharTexture(&gfx, font->fontBuf + pressStartFontIndices[i] * FONT_CHAR_TEX_SIZE, rectLeft,
                                  YREG(10) + 171);
            rectLeft += YREG(8);
            if (i == 4) {
                rectLeft += YREG(9);
            }
        }
    }

    if (textFadeDirection != 0) {
        if (--textFadeTimer == 0) {
            textFadeDirection = 0;
        }
    } else {
        if (++textFadeTimer >= 26) {
            textFadeDirection = 1;
        }
    }

    *gfxp = gfx;
}

void EnMag_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    Gfx* gfx;
    Gfx* gfxRef;

    OPEN_DISPS(play->state.gfxCtx);

    gfxRef = POLY_OPA_DISP;
    gfx = Graph_GfxPlusOne(gfxRef);
    gSPDisplayList(OVERLAY_DISP++, gfx);

    drawInnerFunc(thisx, play, &gfx);

    gSPEndDisplayList(gfx++);
    Graph_BranchDlist(gfxRef, gfx);
    POLY_OPA_DISP = gfx;

    CLOSE_DISPS(play->state.gfxCtx);
}