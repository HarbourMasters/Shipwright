#include "prevent_bss_reordering.h"
#include "global.h"
#include "segment_symbols.h"
#include "z64horse.h"
#include "z64shrink_window.h"
#include "z64view.h"
#include "message_data_static.h"
#include "interface/parameter_static/parameter_static.h"
#include "z64save.h"
#include "BenPort.h"
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/BenGui/CosmeticEditor.h"
#include "assets/archives/schedule_dma_static/schedule_dma_static_yar.h"
#include "assets/archives/icon_item_static/icon_item_static_yar.h"
#include "assets/archives/icon_item_24_static/icon_item_24_static_yar.h"
#include "assets/interface/message_static/message_static.h"
#include "assets/interface/message_texture_static/message_texture_static.h"
#include <string.h>

#include "2s2h_assets.h"
#include <libultraship/bridge/consolevariablebridge.h>

const char* gBombersNotebookPhotos[] = {
    gBombersNotebookPhotoAnjuTex,
    gBombersNotebookPhotoKafeiTex,
    gBombersNotebookPhotoCuriosityShopManTex,
    gBombersNotebookPhotoBombShopLadyTex,
    gBombersNotebookPhotoRomaniTex,
    gBombersNotebookPhotoCremiaTex,
    gBombersNotebookPhotoMayorDotourTex,
    gBombersNotebookPhotoMadameAromaTex,
    gBombersNotebookPhotoTotoTex,
    gBombersNotebookPhotoGormanTex,
    gBombersNotebookPhotoPostmanTex,
    gBombersNotebookPhotoRosaSistersTex,
    gBombersNotebookPhotoToiletHandTex,
    gBombersNotebookPhotoAnjusGrandmotherTex,
    gBombersNotebookPhotoKamaroTex,
    gBombersNotebookPhotoGrogTex,
    gBombersNotebookPhotoGormanBrothersTex,
    gBombersNotebookPhotoShiroTex,
    gBombersNotebookPhotoGuruGuruTex,
    gBombersNotebookPhotoBombersTex,
    gBombersNotebookPhotoMadameAromaBrightTex,
    gBombersNotebookEntryIconExclamationPointLargeTex,
    gBombersNotebookEntryIconMaskTex,
    gBombersNotebookEntryIconRibbonTex,
};

const char* gQuestIcons[] = {
    gQuestIconGoldSkulltulaTex,   gQuestIconHeartContainerTex,  gQuestIconPieceOfHeartTex, gQuestIconPieceOfHeart2Tex,
    gQuestIconHeartContainer2Tex, gQuestIconHeartContainer3Tex, gQuestIconBossKeyTex,      gQuestIconCompassTex,
    gQuestIconDungeonMapTex,      gQuestIconGoldSkulltula2Tex,  gQuestIconSmallKeyTex,     gQuestIconSmallMagicJarTex,
    gQuestIconBigMagicJarTex,     gQuestIconLinkHumanFaceTex,
};

const char* gMessageBackgrounds[] = {
    gMessageDefaultBackgroundTex, gMessageSignBackgroundTex,     gMessageNoteStaffBackgroundTex,
    gMessageFadingBackgroundTex,  gMessageNotebookBackgroundTex,
};

// This looks very similar to gItemIcons, but it's not the same! Make sure you're using the
// right array of icon textures! This one is for messages, the other is for the z_parameter/kaleido
const char* gStaticItemIcons[] = {
    gItemIconOcarinaOfTimeTex,
    gItemIconBowTex,
    gItemIconFireArrowTex,
    gItemIconIceArrowTex,
    gItemIconLightArrowTex,
    gItemIconFairyOcarinaTex,
    gItemIconBombTex,
    gItemIconBombchuTex,
    gItemIconDekuStickTex,
    gItemIconDekuNutTex,
    gItemIconMagicBeansTex,
    gItemIconSlingshotTex,
    gItemIconPowderKegTex,
    gItemIconPictographBoxTex,
    gItemIconLensofTruthTex,
    gItemIconHookshotTex,
    gItemIconGreatFairysSwordTex,
    gItemIconLongshotTex,
    gItemIconEmptyBottleTex,
    gItemIconRedPotionTex,
    gItemIconGreenPotionTex,
    gItemIconBluePotionTex,
    gItemIconBottledFairyTex,
    gItemIconBottledDekuPrincessTex,
    gItemIconBottledFullMilkTex,
    gItemIconBottledHalfMilkTex,
    gItemIconBottledFishTex,
    gItemIconBottledBugTex,
    gItemIconBottledBlueFireTex,
    gItemIconBottledPoeTex,
    gItemIconBottledBigPoeTex,
    gItemIconSpringWaterTex,
    gItemIconHotSpringWaterTex,
    gItemIconBottledZoraEggTex,
    gItemIconBottledGoldDustTex,
    gItemIconBottledMushroomTex,
    gItemIconBottledSeahorseTex,
    gItemIconChateauRomaniTex,
    gItemIconBottledHylianLoachTex,
    gItemIconEmptyBottle2Tex,
    gItemIconMoonsTearTex,
    gItemIconLandDeedTex,
    gItemIconSwampDeedTex,
    gItemIconMountainDeedTex,
    gItemIconOceanDeedTex,
    gItemIconRoomKeyTex,
    gItemIconLetterToMamaTex,
    gItemIconLetterToKafeiTex,
    gItemIconPendantOfMemoriesTex,
    gItemIconTingleMapTex,
    gItemIconDekuMaskTex,
    gItemIconGoronMaskTex,
    gItemIconZoraMaskTex,
    gItemIconFierceDeityMaskTex,
    gItemIconMaskOfTruthTex,
    gItemIconKafeisMaskTex,
    gItemIconAllNightMaskTex,
    gItemIconBunnyHoodTex,
    gItemIconKeatonMaskTex,
    gItemIconGaroMaskTex,
    gItemIconRomaniMaskTex,
    gItemIconCircusLeaderMaskTex,
    gItemIconPostmansHatTex,
    gItemIconCouplesMaskTex,
    gItemIconGreatFairyMaskTex,
    gItemIconGibdoMaskTex,
    gItemIconDonGeroMaskTex,
    gItemIconKamaroMaskTex,
    gItemIconCaptainsHatTex,
    gItemIconStoneMaskTex,
    gItemIconBremenMaskTex,
    gItemIconBlastMaskTex,
    gItemIconMaskOfScentsTex,
    gItemIconGiantsMaskTex,
    gItemIconBowFireTex,
    gItemIconBowIceTex,
    gItemIconBowLightTex,
    gItemIconKokiriSwordTex,
    gItemIconRazorSwordTex,
    gItemIconGildedSwordTex,
    gItemIconFierceDeitySwordTex,
    gItemIconHerosShieldTex,
    gItemIconMirrorShieldTex,
    gItemIconQuiver30Tex,
    gItemIconQuiver40Tex,
    gItemIconQuiver50Tex,
    gItemIconBombBag20Tex,
    gItemIconBombBag30Tex,
    gItemIconBombBag40Tex,
    gItemIconDefaultWalletTex,
    gItemIconAdultsWalletTex,
    gItemIconGiantsWalletTex,
    gItemIconFishingRodTex,
    gItemIconOdolwasRemainsTex,
    gItemIconGohtsRemainsTex,
    gItemIconGyorgsRemainsTex,
    gItemIconTwinmoldsRemainsTex,
    gItemIconBombersNotebookTex,
    gItemIconSongNoteTex,
    gItemIconHeartPiece1Tex,
    gItemIconHeartPiece2Tex,
    gItemIconHeartPiece3Tex,
};
// #endregion

u8 D_801C6A70 = 0;
s16 sOcarinaButtonIndexBufPos = 0;
s16 sOcarinaButtonIndexBufLen = 0;
s16 sLastPlayedSong = 0xFF;
s16 sOcarinaButtonStepR = 0;
s16 sOcarinaButtonStepG = 0;
s16 sOcarinaButtonStepB = 0;
s16 sOcarinaButtonFlashTimer = 12;
s16 sOcarinaButtonFlashColorIndex = 1;
s16 D_801C6A94 = 0;

u8 gPageSwitchNextButtonStatus[][5] = {
    { BTN_ENABLED, BTN_ENABLED, BTN_DISABLED, BTN_ENABLED, BTN_ENABLED },
    { BTN_ENABLED, BTN_ENABLED, BTN_DISABLED, BTN_ENABLED, BTN_ENABLED },
    { BTN_ENABLED, BTN_DISABLED, BTN_DISABLED, BTN_ENABLED, BTN_ENABLED },
    { BTN_ENABLED, BTN_DISABLED, BTN_DISABLED, BTN_ENABLED, BTN_ENABLED },
    { BTN_ENABLED, BTN_ENABLED, BTN_DISABLED, BTN_ENABLED, BTN_ENABLED },
    { BTN_ENABLED, BTN_ENABLED, BTN_DISABLED, BTN_ENABLED, BTN_ENABLED },
};

#define DEFINE_PERSON(_enum, _photo, _description, _metEnum, metMessage, _metFlag) metMessage,
#define DEFINE_EVENT(_enum, _icon, _colorFlag, _description, completedMessage, _completedFlag) completedMessage,

u16 sBombersNotebookEventMessages[BOMBERS_NOTEBOOK_EVENT_MAX] = {
#include "tables/notebook_table.h"
};

#undef DEFINE_PERSON
#undef DEFINE_EVENT

#define DEFINE_PERSON(_enum, _photo, _description, _metEnum, _metMessage, metFlag) metFlag,
#define DEFINE_EVENT(_enum, _icon, _colorFlag, _description, _completedMessage, completedFlag) completedFlag,

u16 gBombersNotebookWeekEventFlags[BOMBERS_NOTEBOOK_EVENT_MAX] = {
#include "tables/notebook_table.h"
};

#undef DEFINE_PERSON
#undef DEFINE_EVENT

MessageTableEntry* sMessageTableNES;
MessageTableEntry* sMessageTableCredits;

s16 D_801CFC78[TEXTBOX_TYPE_MAX] = {
    0,  //  TEXTBOX_TYPE_0
    1,  //  TEXTBOX_TYPE_1
    3,  //  TEXTBOX_TYPE_2
    2,  //  TEXTBOX_TYPE_3
    14, //  TEXTBOX_TYPE_4
    14, //  TEXTBOX_TYPE_5
    0,  //  TEXTBOX_TYPE_6
    14, //  TEXTBOX_TYPE_7
    0,  //  TEXTBOX_TYPE_8
    0,  //  TEXTBOX_TYPE_9
    0,  //  TEXTBOX_TYPE_A
    14, //  TEXTBOX_TYPE_B
    14, //  TEXTBOX_TYPE_C
    4,  //  TEXTBOX_TYPE_D
    14, //  TEXTBOX_TYPE_E
    0,  //  TEXTBOX_TYPE_F
};
u8 sOcarinaButtonIndexBuf[12] = { 0 };
s16 sOcarinaButtonAlphaValues[9] = { 0 };
s16 gOcarinaSongItemMap[] = {
    OCARINA_SONG_SONATA,        OCARINA_SONG_GORON_LULLABY, OCARINA_SONG_NEW_WAVE,
    OCARINA_SONG_ELEGY,         OCARINA_SONG_OATH,          OCARINA_SONG_SARIAS,
    OCARINA_SONG_TIME,          OCARINA_SONG_HEALING,       OCARINA_SONG_EPONAS,
    OCARINA_SONG_SOARING,       OCARINA_SONG_STORMS,        OCARINA_SONG_SUNS,
    OCARINA_SONG_INVERTED_TIME, OCARINA_SONG_DOUBLE_TIME,   OCARINA_SONG_GORON_LULLABY_INTRO,
};

// bss
#include "prevent_bss_reordering.h"
s32 sCharTexSize;
s32 sCharTexScale;
s32 D_801F6B08;

s16 sOcarinaButtonAPrimR;
s16 sOcarinaButtonAPrimB;
s16 sOcarinaButtonAPrimG;
s16 sOcarinaButtonAEnvR;
s16 sOcarinaButtonAEnvB;
s16 sOcarinaButtonAEnvG;
s16 sOcarinaButtonCPrimR;
s16 sOcarinaButtonCPrimB;
s16 sOcarinaButtonCPrimG;
s16 sOcarinaButtonCEnvR;
s16 sOcarinaButtonCEnvB;
s16 sOcarinaButtonCEnvG;

void Message_ResetOcarinaButtonAlphas(void) {
    sOcarinaButtonIndexBuf[0] = OCARINA_BTN_INVALID;
    sOcarinaButtonAlphaValues[0] = sOcarinaButtonAlphaValues[1] = sOcarinaButtonAlphaValues[2] =
        sOcarinaButtonAlphaValues[3] = sOcarinaButtonAlphaValues[4] = sOcarinaButtonAlphaValues[5] =
            sOcarinaButtonAlphaValues[6] = sOcarinaButtonAlphaValues[7] = sOcarinaButtonAlphaValues[8] = 0;
}

void Message_ResetOcarinaButtonState(PlayState* play) {
    MessageContext* msgCtx = &play->msgCtx;

    msgCtx->ocarinaButtonsPosY[OCARINA_BTN_A] = 189;
    msgCtx->ocarinaButtonsPosY[OCARINA_BTN_C_DOWN] = 184;
    msgCtx->ocarinaButtonsPosY[OCARINA_BTN_C_RIGHT] = 179;
    msgCtx->ocarinaButtonsPosY[OCARINA_BTN_C_LEFT] = 174;
    msgCtx->ocarinaButtonsPosY[OCARINA_BTN_C_UP] = 169;

    Message_ResetOcarinaButtonAlphas();

    sOcarinaButtonAPrimR = 80;
    sOcarinaButtonAPrimG = 150;
    sOcarinaButtonAPrimB = 255;
    sOcarinaButtonAEnvR = 10;
    sOcarinaButtonAEnvG = 10;
    sOcarinaButtonAEnvB = 10;
    sOcarinaButtonCPrimR = 255;
    sOcarinaButtonCPrimG = 255;
    sOcarinaButtonCPrimB = 50;
    sOcarinaButtonCEnvR = 10;
    sOcarinaButtonCEnvG = 10;
    sOcarinaButtonCEnvB = 10;
}

bool Message_ShouldAdvance(PlayState* play) {
    MessageContext* msgCtx = &play->msgCtx;
    Input* controller = CONTROLLER1(&play->state);
    bool shouldAdvance;

    if ((msgCtx->textboxEndType == TEXTBOX_ENDTYPE_TWO_CHOICE) ||
        (msgCtx->textboxEndType == TEXTBOX_ENDTYPE_THREE_CHOICE)) {
        if (CHECK_BTN_ALL(controller->press.button, BTN_A)) {
            Audio_PlaySfx(NA_SE_SY_MESSAGE_PASS);
        }
        shouldAdvance = CHECK_BTN_ALL(controller->press.button, BTN_A);
    } else {
        if (CHECK_BTN_ALL(controller->press.button, BTN_A) || CHECK_BTN_ALL(controller->press.button, BTN_B) ||
            CHECK_BTN_ALL(controller->press.button, BTN_CUP)) {
            Audio_PlaySfx(NA_SE_SY_MESSAGE_PASS);
        }
        shouldAdvance =
            CHECK_BTN_ALL(controller->press.button, BTN_A) || CHECK_BTN_ALL(controller->press.button, BTN_B) ||
            // 2S2H [Enhancement] When fast text is on, we want to check if B is held instead of only if it was just
            // pressed
            (CVarGetInteger("gEnhancements.Dialogue.FastText", 0) && CHECK_BTN_ALL(controller->cur.button, BTN_B)) ||
            CHECK_BTN_ALL(controller->press.button, BTN_CUP);
    }

    return GameInteractor_Should(VB_MSG_ADVANCE, shouldAdvance);
}

bool Message_ShouldAdvanceSilent(PlayState* play) {
    MessageContext* msgCtx = &play->msgCtx;
    Input* controller = CONTROLLER1(&play->state);
    bool shouldAdvance;

    if ((msgCtx->textboxEndType == TEXTBOX_ENDTYPE_TWO_CHOICE) ||
        (msgCtx->textboxEndType == TEXTBOX_ENDTYPE_THREE_CHOICE)) {
        shouldAdvance = CHECK_BTN_ALL(controller->press.button, BTN_A);
    } else {
        shouldAdvance =
            CHECK_BTN_ALL(controller->press.button, BTN_A) || CHECK_BTN_ALL(controller->press.button, BTN_B) ||
            // 2S2H [Enhancement] When fast text is on, we want to check if B is held instead of only if it was just
            // pressed
            (CVarGetInteger("gEnhancements.Dialogue.FastText", 0) && CHECK_BTN_ALL(controller->cur.button, BTN_B)) ||
            CHECK_BTN_ALL(controller->press.button, BTN_CUP);
    }

    return GameInteractor_Should(VB_MSG_ADVANCE, shouldAdvance);
}

void Message_CloseTextbox(PlayState* play) {
    MessageContext* msgCtx = &play->msgCtx;

    if (play->msgCtx.msgLength != 0) {
        msgCtx->stateTimer = 2;
        msgCtx->msgMode = MSGMODE_TEXT_CLOSING;
        msgCtx->textboxEndType = TEXTBOX_ENDTYPE_DEFAULT;
        Audio_PlaySfx(NA_SE_NONE);
    }
}

void Message_DrawTextboxIcon(PlayState* play, Gfx** gfxP, s16 x, s16 y) {
    static Color_RGB16 sIconPrimColors[] = {
        { 0, 80, 200 },
        { 50, 130, 255 },
    };
    static Color_RGB16 sIconEnvColors[] = {
        { 0, 0, 0 },
        { 0, 130, 255 },
    };
    static s16 sIconPrimR = 0;
    static s16 sIconPrimG = 80;
    static s16 sIconPrimB = 200;
    static s16 sIconFlashTimer = 12;
    static s16 sIconFlashColorIndex = 0;
    static s16 sIconEnvR = 0;
    static s16 sIconEnvG = 0;
    static s16 sIconEnvB = 0;
    Gfx* gfx;
    MessageContext* msgCtx = &play->msgCtx;
    Font* font = &msgCtx->font;
    s16 primR;
    s16 primG;
    s16 primB;
    s16 envR;
    s16 envG;
    s16 envB;
    u8* iconTexture = msgCtx->font.iconBuf;

    gfx = *gfxP;

    if (!msgCtx->textIsCredits) {
        primR = ABS_ALT(sIconPrimR - sIconPrimColors[sIconFlashColorIndex].r) / sIconFlashTimer;
        primG = ABS_ALT(sIconPrimG - sIconPrimColors[sIconFlashColorIndex].g) / sIconFlashTimer;
        primB = ABS_ALT(sIconPrimB - sIconPrimColors[sIconFlashColorIndex].b) / sIconFlashTimer;

        if (sIconPrimR >= sIconPrimColors[sIconFlashColorIndex].r) {
            sIconPrimR -= primR;
        } else {
            sIconPrimR += primR;
        }

        if (sIconPrimG >= sIconPrimColors[sIconFlashColorIndex].g) {
            sIconPrimG -= primG;
        } else {
            sIconPrimG += primG;
        }

        if (sIconPrimB >= sIconPrimColors[sIconFlashColorIndex].b) {
            sIconPrimB -= primB;
        } else {
            sIconPrimB += primB;
        }

        envR = ABS_ALT(sIconEnvR - sIconEnvColors[sIconFlashColorIndex].r) / sIconFlashTimer;
        envG = ABS_ALT(sIconEnvG - sIconEnvColors[sIconFlashColorIndex].g) / sIconFlashTimer;
        envB = ABS_ALT(sIconEnvB - sIconEnvColors[sIconFlashColorIndex].b) / sIconFlashTimer;

        if (sIconEnvR >= sIconEnvColors[sIconFlashColorIndex].r) {
            sIconEnvR -= envR;
        } else {
            sIconEnvR += envR;
        }

        if (sIconEnvG >= sIconEnvColors[sIconFlashColorIndex].g) {
            sIconEnvG -= envG;
        } else {
            sIconEnvG += envG;
        }

        if (sIconEnvB >= sIconEnvColors[sIconFlashColorIndex].b) {
            sIconEnvB -= envB;
        } else {
            sIconEnvB += envB;
        }

        sIconFlashTimer--;
        if (sIconFlashTimer == 0) {
            sIconPrimR = sIconPrimColors[sIconFlashColorIndex].r;
            sIconPrimG = sIconPrimColors[sIconFlashColorIndex].g;
            sIconPrimB = sIconPrimColors[sIconFlashColorIndex].b;
            sIconEnvR = sIconEnvColors[sIconFlashColorIndex].r;
            sIconEnvG = sIconEnvColors[sIconFlashColorIndex].g;
            sIconEnvB = sIconEnvColors[sIconFlashColorIndex].b;
            sIconFlashTimer = 12;
            sIconFlashColorIndex ^= 1;
        }

        gDPPipeSync(gfx++);
        gDPSetCombineLERP(gfx++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0, PRIMITIVE,
                          ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
        gDPSetPrimColor(gfx++, 0, 0, sIconPrimR, sIconPrimG, sIconPrimB, 255);
        gDPSetEnvColor(gfx++, sIconEnvR, sIconEnvG, sIconEnvB, 255);

        if (!play->pauseCtx.bombersNotebookOpen) {
            gDPLoadTextureBlock_4b(gfx++, iconTexture, G_IM_FMT_I, 16, 16, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                                   G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
            gSPTextureRectangle(gfx++, x << 2, y << 2, (x + sCharTexSize) << 2, (y + sCharTexSize) << 2,
                                G_TX_RENDERTILE, 0, 0, sCharTexScale, sCharTexScale);
        }

        msgCtx->stateTimer++;
        *gfxP = gfx;
    }
}

void func_80147F18(PlayState* play, Gfx** gfxP, s16 arg2, s16 arg3) {
    static Color_RGB16 D_801CFD10[] = {
        { 0, 80, 200 },
        { 50, 130, 255 },
    };
    static Color_RGB16 D_801CFD1C[] = {
        { 0, 0, 0 },
        { 0, 130, 255 },
    };
    static s16 D_801CFD28 = 0;
    static s16 D_801CFD2C = 80;
    static s16 D_801CFD30 = 200;
    static s16 D_801CFD34 = 12;
    static s16 D_801CFD38 = 0;
    static s16 D_801CFD3C = 0;
    static s16 D_801CFD40 = 0;
    static s16 D_801CFD44 = 0;
    Gfx* gfx;
    MessageContext* msgCtx = &play->msgCtx;
    s16 primR;
    s16 primG;
    s16 primB;
    s16 envR;
    s16 envG;
    s16 envB;

    gfx = *gfxP;

    if (!msgCtx->textIsCredits) {
        primR = ABS_ALT(D_801CFD28 - D_801CFD10[D_801CFD38].r) / D_801CFD34;
        primG = ABS_ALT(D_801CFD2C - D_801CFD10[D_801CFD38].g) / D_801CFD34;
        primB = ABS_ALT(D_801CFD30 - D_801CFD10[D_801CFD38].b) / D_801CFD34;

        if (D_801CFD28 >= D_801CFD10[D_801CFD38].r) {
            D_801CFD28 -= primR;
        } else {
            D_801CFD28 += primR;
        }

        if (D_801CFD2C >= D_801CFD10[D_801CFD38].g) {
            D_801CFD2C -= primG;
        } else {
            D_801CFD2C += primG;
        }

        if (D_801CFD30 >= D_801CFD10[D_801CFD38].b) {
            D_801CFD30 -= primB;
        } else {
            D_801CFD30 += primB;
        }

        envR = ABS_ALT(D_801CFD3C - D_801CFD1C[D_801CFD38].r) / D_801CFD34;
        envG = ABS_ALT(D_801CFD40 - D_801CFD1C[D_801CFD38].g) / D_801CFD34;
        envB = ABS_ALT(D_801CFD44 - D_801CFD1C[D_801CFD38].b) / D_801CFD34;

        if (D_801CFD3C >= D_801CFD1C[D_801CFD38].r) {
            D_801CFD3C -= envR;
        } else {
            D_801CFD3C += envR;
        }

        if (D_801CFD40 >= D_801CFD1C[D_801CFD38].g) {
            D_801CFD40 -= envG;
        } else {
            D_801CFD40 += envG;
        }

        if (D_801CFD44 >= D_801CFD1C[D_801CFD38].b) {
            D_801CFD44 -= envB;
        } else {
            D_801CFD44 += envB;
        }

        D_801CFD34--;
        if (D_801CFD34 == 0) {
            D_801CFD28 = D_801CFD10[D_801CFD38].r;
            D_801CFD2C = D_801CFD10[D_801CFD38].g;
            D_801CFD30 = D_801CFD10[D_801CFD38].b;
            D_801CFD3C = D_801CFD1C[D_801CFD38].r;
            D_801CFD40 = D_801CFD1C[D_801CFD38].g;
            D_801CFD44 = D_801CFD1C[D_801CFD38].b;
            D_801CFD34 = 12;
            D_801CFD38 ^= 1;
        }

        gDPPipeSync(gfx++);
        gDPSetRenderMode(gfx++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
        gDPSetCombineMode(gfx++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
        gDPSetPrimColor(gfx++, 0, 0, D_801CFD28, D_801CFD2C, D_801CFD30, 120);
        gDPFillRectangle(gfx++, arg2 + 3, arg3, arg2 + 17, arg3 + 11);
        gDPFillRectangle(gfx++, arg2 + 6, arg3 - 2, arg2 + 14, arg3 + 13);
        gDPPipeSync(gfx++);

        msgCtx->stateTimer++;
        *gfxP = gfx++;
    }
}

void func_80148558(PlayState* play, Gfx** gfxP, s16 arg2, s16 arg3) {
    static Color_RGB16 D_801CFD48[] = {
        { 0, 80, 200 },
        { 50, 130, 255 },
    };
    static Color_RGB16 D_801CFD54[] = {
        { 0, 0, 0 },
        { 0, 130, 255 },
    };
    static s16 D_801CFD60 = 0;
    static s16 D_801CFD64 = 80;
    static s16 D_801CFD68 = 200;
    static s16 D_801CFD6C = 12;
    static s16 D_801CFD70 = 0;
    static s16 D_801CFD74 = 0;
    static s16 D_801CFD78 = 0;
    static s16 D_801CFD7C = 0;
    Gfx* gfx;
    MessageContext* msgCtx = &play->msgCtx;
    s16 primR;
    s16 primG;
    s16 primB;
    s16 envR;
    s16 envG;
    s16 envB;

    gfx = *gfxP;

    if (!msgCtx->textIsCredits) {
        primR = ABS_ALT(D_801CFD60 - D_801CFD48[D_801CFD70].r) / D_801CFD6C;
        primG = ABS_ALT(D_801CFD64 - D_801CFD48[D_801CFD70].g) / D_801CFD6C;
        primB = ABS_ALT(D_801CFD68 - D_801CFD48[D_801CFD70].b) / D_801CFD6C;

        if (D_801CFD60 >= D_801CFD48[D_801CFD70].r) {
            D_801CFD60 -= primR;
        } else {
            D_801CFD60 += primR;
        }

        if (D_801CFD64 >= D_801CFD48[D_801CFD70].g) {
            D_801CFD64 -= primG;
        } else {
            D_801CFD64 += primG;
        }

        if (D_801CFD68 >= D_801CFD48[D_801CFD70].b) {
            D_801CFD68 -= primB;
        } else {
            D_801CFD68 += primB;
        }

        envR = ABS_ALT(D_801CFD74 - D_801CFD54[D_801CFD70].r) / D_801CFD6C;
        envG = ABS_ALT(D_801CFD78 - D_801CFD54[D_801CFD70].g) / D_801CFD6C;
        envB = ABS_ALT(D_801CFD7C - D_801CFD54[D_801CFD70].b) / D_801CFD6C;

        if (D_801CFD74 >= D_801CFD54[D_801CFD70].r) {
            D_801CFD74 -= envR;
        } else {
            D_801CFD74 += envR;
        }

        if (D_801CFD78 >= D_801CFD54[D_801CFD70].g) {
            D_801CFD78 -= envG;
        } else {
            D_801CFD78 += envG;
        }

        if (D_801CFD7C >= D_801CFD54[D_801CFD70].b) {
            D_801CFD7C -= envB;
        } else {
            D_801CFD7C += envB;
        }

        D_801CFD6C--;
        if (D_801CFD6C == 0) {
            D_801CFD60 = D_801CFD48[D_801CFD70].r;
            D_801CFD64 = D_801CFD48[D_801CFD70].g;
            D_801CFD68 = D_801CFD48[D_801CFD70].b;
            D_801CFD74 = D_801CFD54[D_801CFD70].r;
            D_801CFD78 = D_801CFD54[D_801CFD70].g;
            D_801CFD7C = D_801CFD54[D_801CFD70].b;
            D_801CFD6C = 12;
            D_801CFD70 ^= 1;
        }

        gDPPipeSync(gfx++);
        gDPSetRenderMode(gfx++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
        gDPSetCombineMode(gfx++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
        gDPSetPrimColor(gfx++, 0, 0, D_801CFD60, D_801CFD64, D_801CFD68, 120);
        gDPFillRectangle(gfx++, arg2 + 3, arg3, arg2 + 29, arg3 + 11);
        gDPFillRectangle(gfx++, arg2 + 6, arg3 - 2, arg2 + 26, arg3 + 13);
        gDPPipeSync(gfx++);

        msgCtx->stateTimer++;
        *gfxP = gfx++;
    }
}

void Message_HandleChoiceSelection(PlayState* play, u8 numChoices) {
    static s16 sAnalogStickHeld = false;
    MessageContext* msgCtx = &play->msgCtx;
    Input* input = CONTROLLER1(&play->state);

    if ((input->rel.stick_y >= 30) && !sAnalogStickHeld) {
        sAnalogStickHeld = true;
        msgCtx->choiceIndex--;
        if (msgCtx->choiceIndex > 128) {
            msgCtx->choiceIndex = 0;
        } else {
            Audio_PlaySfx(NA_SE_SY_CURSOR);
        }
    } else if ((input->rel.stick_y <= -30) && !sAnalogStickHeld) {
        sAnalogStickHeld = true;
        msgCtx->choiceIndex++;
        if (msgCtx->choiceIndex > numChoices) {
            msgCtx->choiceIndex = numChoices;
        } else {
            Audio_PlaySfx(NA_SE_SY_CURSOR);
        }
    } else if (ABS_ALT(input->rel.stick_y) < 30) {
        sAnalogStickHeld = false;
    }
}

void func_80148CBC(PlayState* play, Gfx** gfxP, u8 arg2) {
    MessageContext* msgCtx = &play->msgCtx;

    msgCtx->textPosX = 48;
    if (arg2 == 1) {
        msgCtx->textPosY = msgCtx->unk11FFE[1 + msgCtx->choiceIndex];
    } else {
        msgCtx->textPosY = msgCtx->unk11FFE[msgCtx->choiceIndex];
    }
    Message_DrawTextboxIcon(play, gfxP, msgCtx->textPosX, msgCtx->textPosY);
}

void func_80148D64(PlayState* play) {
    static s16 sAnalogStickHeld = false;
    MessageContext* msgCtx = &play->msgCtx;

    if (play->msgCtx.stickAdjY <= -30) {
        msgCtx->decodedBuffer.schar[msgCtx->unk120C0 + msgCtx->unk120C2]--;
        if (msgCtx->decodedBuffer.schar[msgCtx->unk120C0 + msgCtx->unk120C2] < '0') {
            msgCtx->decodedBuffer.schar[msgCtx->unk120C0 + msgCtx->unk120C2] = '9';
        }
        Font_LoadCharNES(play, msgCtx->decodedBuffer.schar[msgCtx->unk120C0 + msgCtx->unk120C2],
                         msgCtx->unk120C4 + (msgCtx->unk120C2 << 7));
        if (GameInteractor_Should(VB_MSG_PLAY_INPUT_COUNT_SOUND, true)) {
            Audio_PlaySfx(NA_SE_SY_RUPY_COUNT);
        }
    } else if (msgCtx->stickAdjY >= 30) {
        msgCtx->decodedBuffer.schar[msgCtx->unk120C0 + msgCtx->unk120C2]++;
        if (msgCtx->decodedBuffer.schar[msgCtx->unk120C0 + msgCtx->unk120C2] > '9') {
            msgCtx->decodedBuffer.schar[msgCtx->unk120C0 + msgCtx->unk120C2] = '0';
        }
        Font_LoadCharNES(play, msgCtx->decodedBuffer.schar[msgCtx->unk120C0 + msgCtx->unk120C2],
                         msgCtx->unk120C4 + (msgCtx->unk120C2 << 7));
        if (GameInteractor_Should(VB_MSG_PLAY_INPUT_COUNT_SOUND, true)) {
            Audio_PlaySfx(NA_SE_SY_RUPY_COUNT);
        }
    } else if ((msgCtx->stickAdjX >= 30) && !sAnalogStickHeld) {
        sAnalogStickHeld = true;
        msgCtx->unk120C2++;
        if (msgCtx->unk120C2 > 2) {
            msgCtx->unk120C2 = 2;
        } else {
            Audio_PlaySfx(NA_SE_SY_CURSOR);
        }
    } else if ((msgCtx->stickAdjX <= -30) && !sAnalogStickHeld) {
        sAnalogStickHeld = true;
        msgCtx->unk120C2--;
        if (msgCtx->unk120C2 < 0) {
            msgCtx->unk120C2 = 0;
        } else {
            Audio_PlaySfx(NA_SE_SY_CURSOR);
        }
    } else {
        sAnalogStickHeld = false;
    }

    msgCtx->rupeesSelected = (msgCtx->decodedBuffer.schar[msgCtx->unk120C0] - '0') * 100;
    msgCtx->rupeesSelected += (msgCtx->decodedBuffer.schar[msgCtx->unk120C0 + 1] - '0') * 10;
    msgCtx->rupeesSelected += msgCtx->decodedBuffer.schar[msgCtx->unk120C0 + 2] - '0';
}

void func_80149048(PlayState* play) {
    MessageContext* msgCtx = &play->msgCtx;

    if (msgCtx->stickAdjY <= -30) {
        msgCtx->decodedBuffer.schar[msgCtx->unk120C0 + msgCtx->unk120C2]--;
        if (msgCtx->decodedBuffer.schar[msgCtx->unk120C0 + msgCtx->unk120C2] < '0') {
            msgCtx->decodedBuffer.schar[msgCtx->unk120C0 + msgCtx->unk120C2] = '9';
        }
        Font_LoadCharNES(play, msgCtx->decodedBuffer.schar[msgCtx->unk120C0 + msgCtx->unk120C2],
                         msgCtx->unk120C4 + (msgCtx->unk120C2 << 7));
        if (GameInteractor_Should(VB_MSG_PLAY_INPUT_COUNT_SOUND, true)) {
            Audio_PlaySfx(NA_SE_SY_RUPY_COUNT);
        }
    } else if (msgCtx->stickAdjY >= 30) {
        msgCtx->decodedBuffer.schar[msgCtx->unk120C0 + msgCtx->unk120C2]++;
        if (msgCtx->decodedBuffer.schar[msgCtx->unk120C0 + msgCtx->unk120C2] > '9') {
            msgCtx->decodedBuffer.schar[msgCtx->unk120C0 + msgCtx->unk120C2] = '0';
        }
        Font_LoadCharNES(play, msgCtx->decodedBuffer.schar[msgCtx->unk120C0 + msgCtx->unk120C2],
                         msgCtx->unk120C4 + (msgCtx->unk120C2 << 7));
        if (GameInteractor_Should(VB_MSG_PLAY_INPUT_COUNT_SOUND, true)) {
            Audio_PlaySfx(NA_SE_SY_RUPY_COUNT);
        }
    }

    msgCtx->rupeesSelected = (msgCtx->decodedBuffer.schar[msgCtx->unk120C0] - '0') * 10;
}

void func_801491DC(PlayState* play) {
    static s16 sAnalogStickHeld = false;
    MessageContext* msgCtx = &play->msgCtx;

    if (msgCtx->stickAdjY <= -30) {
        msgCtx->unk12054[msgCtx->unk120C2]--;
        if (msgCtx->unk12054[msgCtx->unk120C2] <= 0) {
            msgCtx->unk12054[msgCtx->unk120C2] = 5;
        }
        msgCtx->decodedBuffer.schar[msgCtx->unk120C0 + msgCtx->unk120C2] = msgCtx->unk12054[msgCtx->unk120C2] + '0';
        Font_LoadCharNES(play, msgCtx->decodedBuffer.schar[msgCtx->unk120C0 + msgCtx->unk120C2],
                         msgCtx->unk120C4 + (msgCtx->unk120C2 << 7));
        if (GameInteractor_Should(VB_MSG_PLAY_INPUT_COUNT_SOUND, true)) {
            Audio_PlaySfx(NA_SE_SY_RUPY_COUNT);
        }
    } else if (msgCtx->stickAdjY >= 30) {
        msgCtx->unk12054[msgCtx->unk120C2]++;
        if (msgCtx->unk12054[msgCtx->unk120C2] > 5) {
            msgCtx->unk12054[msgCtx->unk120C2] = 1;
        }
        msgCtx->decodedBuffer.schar[msgCtx->unk120C0 + msgCtx->unk120C2] = msgCtx->unk12054[msgCtx->unk120C2] + '0';
        Font_LoadCharNES(play, msgCtx->decodedBuffer.schar[msgCtx->unk120C0 + msgCtx->unk120C2],
                         msgCtx->unk120C4 + (msgCtx->unk120C2 << 7));
        if (GameInteractor_Should(VB_MSG_PLAY_INPUT_COUNT_SOUND, true)) {
            Audio_PlaySfx(NA_SE_SY_RUPY_COUNT);
        }
    } else if ((msgCtx->stickAdjX >= 30) && !sAnalogStickHeld) {
        sAnalogStickHeld = true;
        msgCtx->unk120C2++;
        if (msgCtx->unk120C2 > 4) {
            msgCtx->unk120C2 = 4;
        } else {
            Audio_PlaySfx(NA_SE_SY_CURSOR);
        }
    } else if ((msgCtx->stickAdjX <= -30) && !sAnalogStickHeld) {
        sAnalogStickHeld = true;
        msgCtx->unk120C2--;
        if (msgCtx->unk120C2 < 0) {
            msgCtx->unk120C2 = 0;
        } else {
            Audio_PlaySfx(NA_SE_SY_CURSOR);
        }
    } else {
        sAnalogStickHeld = false;
    }
}

void func_80149454(PlayState* play) {
    static s16 sAnalogStickHeld = false;
    MessageContext* msgCtx = &play->msgCtx;

    if (msgCtx->stickAdjY <= -30) {
        msgCtx->unk12054[msgCtx->unk120C2]--;
        if (msgCtx->unk12054[msgCtx->unk120C2] < 0) {
            msgCtx->unk12054[msgCtx->unk120C2] = 9;
        }
        msgCtx->decodedBuffer.schar[msgCtx->unk120C0 + msgCtx->unk120C2] = msgCtx->unk12054[msgCtx->unk120C2] + '0';
        Font_LoadCharNES(play, msgCtx->decodedBuffer.schar[msgCtx->unk120C0 + msgCtx->unk120C2],
                         msgCtx->unk120C4 + (msgCtx->unk120C2 << 7));
        if (GameInteractor_Should(VB_MSG_PLAY_INPUT_COUNT_SOUND, true)) {
            Audio_PlaySfx(NA_SE_SY_RUPY_COUNT);
        }
    } else if (msgCtx->stickAdjY >= 30) {
        msgCtx->unk12054[msgCtx->unk120C2]++;
        if (msgCtx->unk12054[msgCtx->unk120C2] > 9) {
            msgCtx->unk12054[msgCtx->unk120C2] = 0;
        }
        msgCtx->decodedBuffer.schar[msgCtx->unk120C0 + msgCtx->unk120C2] = msgCtx->unk12054[msgCtx->unk120C2] + '0';
        Font_LoadCharNES(play, msgCtx->decodedBuffer.schar[msgCtx->unk120C0 + msgCtx->unk120C2],
                         msgCtx->unk120C4 + (msgCtx->unk120C2 << 7));
        if (GameInteractor_Should(VB_MSG_PLAY_INPUT_COUNT_SOUND, true)) {
            Audio_PlaySfx(NA_SE_SY_RUPY_COUNT);
        }
    } else if ((msgCtx->stickAdjX >= 30) && !sAnalogStickHeld) {
        sAnalogStickHeld = true;
        msgCtx->unk120C2++;
        if (msgCtx->unk120C2 > 2) {
            msgCtx->unk120C2 = 2;
        } else {
            Audio_PlaySfx(NA_SE_SY_CURSOR);
        }
    } else if ((msgCtx->stickAdjX <= -30) && !sAnalogStickHeld) {
        sAnalogStickHeld = true;
        msgCtx->unk120C2--;
        if (msgCtx->unk120C2 < 0) {
            msgCtx->unk120C2 = 0;
        } else {
            Audio_PlaySfx(NA_SE_SY_CURSOR);
        }
    } else {
        sAnalogStickHeld = false;
    }
}

void func_801496C8(PlayState* play) {
    static s16 sAnalogStickHeld = false;
    MessageContext* msgCtx = &play->msgCtx;

    if (play->msgCtx.stickAdjY <= -30) {
        msgCtx->unk12054[msgCtx->unk120C2]--;
        if (msgCtx->unk12054[msgCtx->unk120C2] < 0) {
            msgCtx->unk12054[msgCtx->unk120C2] = 3;
        }
        msgCtx->decodedBuffer.wchar[msgCtx->unk120C0 + msgCtx->unk120C2] = msgCtx->unk12054[msgCtx->unk120C2] + 0x824F;
        Font_LoadChar(play, msgCtx->decodedBuffer.wchar[msgCtx->unk120C0 + msgCtx->unk120C2],
                      msgCtx->unk120C4 + (msgCtx->unk120C2 << 7));
        if (GameInteractor_Should(VB_MSG_PLAY_INPUT_COUNT_SOUND, true)) {
            Audio_PlaySfx(NA_SE_SY_RUPY_COUNT);
        }
    } else if (msgCtx->stickAdjY >= 30) {
        msgCtx->unk12054[msgCtx->unk120C2]++;
        if (msgCtx->unk12054[msgCtx->unk120C2] >= 4) {
            msgCtx->unk12054[msgCtx->unk120C2] = 0;
        }
        msgCtx->decodedBuffer.wchar[msgCtx->unk120C0 + msgCtx->unk120C2] = msgCtx->unk12054[msgCtx->unk120C2] + 0x824F;
        Font_LoadChar(play, msgCtx->decodedBuffer.wchar[msgCtx->unk120C0 + msgCtx->unk120C2],
                      msgCtx->unk120C4 + (msgCtx->unk120C2 << 7));
        if (GameInteractor_Should(VB_MSG_PLAY_INPUT_COUNT_SOUND, true)) {
            Audio_PlaySfx(NA_SE_SY_RUPY_COUNT);
        }
    } else if ((msgCtx->stickAdjX >= 30) && !sAnalogStickHeld) {
        sAnalogStickHeld = true;
        msgCtx->unk120C2++;
        if (msgCtx->unk120C2 > 5) {
            msgCtx->unk120C2 = 5;
        } else {
            Audio_PlaySfx(NA_SE_SY_CURSOR);
        }
    } else if ((msgCtx->stickAdjX <= -30) && !sAnalogStickHeld) {
        sAnalogStickHeld = true;
        msgCtx->unk120C2--;
        if (msgCtx->unk120C2 < 0) {
            msgCtx->unk120C2 = 0;
        } else {
            Audio_PlaySfx(NA_SE_SY_CURSOR);
        }
    } else {
        sAnalogStickHeld = false;
    }
}

void Message_DrawTextChar(PlayState* play, TexturePtr texture, Gfx** gfxP) {
    MessageContext* msgCtx = &play->msgCtx;
    Gfx* gfx = *gfxP;
    s16 x = msgCtx->textPosX;
    s16 y = msgCtx->textPosY;

    gDPPipeSync(gfx++);

    gDPLoadTextureBlock_4b(gfx++, texture, G_IM_FMT_I, 16, 16, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                           G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

    if ((msgCtx->textBoxType != TEXTBOX_TYPE_5) && (msgCtx->textBoxType != TEXTBOX_TYPE_D) &&
        !play->pauseCtx.bombersNotebookOpen) {
        gDPSetPrimColor(gfx++, 0, 0, 0, 0, 0, msgCtx->textColorAlpha);
        gSPTextureRectangle(gfx++, (x + 1) << 2, (y + 1) << 2, (x + sCharTexSize + 1) << 2, (y + sCharTexSize + 1) << 2,
                            G_TX_RENDERTILE, 0, 0, sCharTexScale, sCharTexScale);
        gDPPipeSync(gfx++);
    }

    gDPSetPrimColor(gfx++, 0, 0, msgCtx->textColorR, msgCtx->textColorG, msgCtx->textColorB, msgCtx->textColorAlpha);
    gSPTextureRectangle(gfx++, x << 2, y << 2, (x + sCharTexSize) << 2, (y + sCharTexSize) << 2, G_TX_RENDERTILE, 0, 0,
                        sCharTexScale, sCharTexScale);
    *gfxP = gfx++;
}

s16 sTextboxWidth = 256;
s16 sTextboxHeight = 64;
s16 sTextboxTexWidth = 1024;
s16 sTextboxTexHeight = 1024;

f32 D_801CFDA4[] = { 0.6f, 0.75f, 0.9f, 1.0f, 1.05f, 1.1f, 1.05f, 1.0f, 1.0f };
f32 D_801CFDC8[] = { 0.6f, 0.75f, 0.9f, 1.0f, 1.05f, 1.1f, 1.05f, 1.0f, 1.0f };

// resizes textboxes when opening them
void Message_GrowTextbox(PlayState* play) {
    MessageContext* msgCtx = &play->msgCtx;

    if (!play->pauseCtx.bombersNotebookOpen) {
        sTextboxWidth = D_801CFDA4[msgCtx->stateTimer] * 256.0f;
        sTextboxTexWidth = 1024.0f / D_801CFDA4[msgCtx->stateTimer];
        sTextboxHeight = D_801CFDC8[msgCtx->stateTimer] * 64.0f;
        sTextboxTexHeight = 1024.0f / D_801CFDC8[msgCtx->stateTimer];
        msgCtx->textboxY = msgCtx->textboxYTarget + ((64 - sTextboxHeight) / 2);
        msgCtx->textboxColorAlphaCurrent += msgCtx->textboxColorAlphaTarget / 8;
        msgCtx->stateTimer++;
    } else {
        msgCtx->stateTimer = 8;
        sTextboxWidth = 512;
        sTextboxTexWidth = 512;
        sTextboxHeight = 89;
        sTextboxTexHeight = 731;
        msgCtx->textboxY = msgCtx->textboxYTarget + ((90 - sTextboxHeight) / 2);
    }

    if (msgCtx->stateTimer == 8) {
        // Reached the end
        msgCtx->textboxX = msgCtx->textboxXTarget;
        msgCtx->textboxY = msgCtx->textboxYTarget;
        msgCtx->msgMode = MSGMODE_TEXT_STARTING;
        msgCtx->textboxColorAlphaCurrent = msgCtx->textboxColorAlphaTarget;
    }

    if (!play->pauseCtx.bombersNotebookOpen) {
        msgCtx->textboxX = msgCtx->textboxXTarget + ((256 - sTextboxWidth) / 2);
    } else {
        msgCtx->textboxX = msgCtx->textboxXTarget + ((512 - sTextboxWidth) / 2);
    }
}

void Message_FindMessage(PlayState* play, u16 textId) {
    MessageContext* msgCtx = &play->msgCtx;
    Font* font = &msgCtx->font;
    MessageTableEntry* msgEntry = msgCtx->messageTable;
    const char* segment = msgEntry->segment;
    const char* foundSegment;
    const char* nextSegment;

    while (msgEntry->textId != 0xFFFF) {
        if (msgEntry->textId == textId) {
            foundSegment = msgEntry->segment;
            msgEntry++;
            nextSegment = msgEntry->segment;
            font->messageStart = foundSegment - segment;
            font->messageEnd = nextSegment - foundSegment;
            return;
        }
        msgEntry++;
    }

    msgEntry = msgCtx->messageTable;
    foundSegment = msgEntry->segment;
    msgEntry++;
    nextSegment = msgEntry->segment;
    font->messageStart = foundSegment - segment;
    font->messageEnd = nextSegment - foundSegment;
}

Color_RGB8 D_801CFDEC[] = {
    { 0, 255, 0 },   { 0, 0, 255 },     { 255, 255, 255 }, { 255, 0, 0 },
    { 255, 0, 255 }, { 255, 255, 255 }, { 255, 100, 0 },   { 0, 0, 0 },
};

s16 D_801CFE04[] = {
    150, // ITEM_SONG_SONATA
    255, // ITEM_SONG_LULLABY
    100, // ITEM_SONG_NOVA
    255, // ITEM_SONG_ELEGY
    255, // ITEM_SONG_OATH
    255, // ITEM_SONG_SARIA
    255, // ITEM_SONG_TIME
    255, // ITEM_SONG_HEALING
    255, // ITEM_SONG_EPONA
    255, // ITEM_SONG_SOARING
    255, // ITEM_SONG_STORMS
    255  // ITEM_SONG_SUN

};
s16 D_801CFE1C[] = {
    255, // ITEM_SONG_SONATA
    80,  // ITEM_SONG_LULLABY
    150, // ITEM_SONG_NOVA
    160, // ITEM_SONG_ELEGY
    100, // ITEM_SONG_OATH
    240, // ITEM_SONG_SARIA
    255, // ITEM_SONG_TIME
    255, // ITEM_SONG_HEALING
    255, // ITEM_SONG_EPONA
    255, // ITEM_SONG_SOARING
    255, // ITEM_SONG_STORMS
    255  // ITEM_SONG_SUN

};
s16 D_801CFE34[] = {
    100, // ITEM_SONG_SONATA
    40,  // ITEM_SONG_LULLABY
    255, // ITEM_SONG_NOVA
    0,   // ITEM_SONG_ELEGY
    255, // ITEM_SONG_OATH
    100, // ITEM_SONG_SARIA
    255, // ITEM_SONG_TIME
    255, // ITEM_SONG_HEALING
    255, // ITEM_SONG_EPONA
    255, // ITEM_SONG_SOARING
    255, // ITEM_SONG_STORMS
    255  // ITEM_SONG_SUN
};

static TexturePtr sStrayFairyIconTextures[] = {
    gStrayFairyWoodfallIconTex,
    gStrayFairySnowheadIconTex,
    gStrayFairyGreatBayIconTex,
    gStrayFairyStoneTowerIconTex,
};

static Color_RGB8 sStrayFairyIconPrimColors[] = {
    { 255, 110, 160 },
    { 90, 255, 100 },
    { 120, 255, 255 },
    { 245, 245, 90 },
};

static Color_RGB8 sStrayFairyIconEnvColors[] = {
    { 255, 255, 255 },
    { 255, 255, 255 },
    { 255, 255, 255 },
    { 225, 170, 0 },
};

void Message_DrawItemIcon(PlayState* play, Gfx** gfxP) {
    MessageContext* msgCtx = &play->msgCtx;
    Gfx* gfx = *gfxP;
    s32 index;

    msgCtx->unk12016 = msgCtx->unk12014;

    if (msgCtx->msgMode == MSGMODE_TEXT_DISPLAYING) {
        Audio_PlaySfx(NA_SE_NONE);
    }

    gDPPipeSync(gfx++);
    gDPSetCombineMode(gfx++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
    gDPSetPrimColor(gfx++, 0, 0, 255, 255, 255, msgCtx->textColorAlpha);

    if (msgCtx->itemId == ITEM_RECOVERY_HEART) {
        gDPPipeSync(gfx++);
        gDPSetPrimColor(gfx++, 0, 0, 255, 0, 0, msgCtx->textColorAlpha);
        gDPSetEnvColor(gfx++, 0, 80, 0, 255);
        gDPLoadTextureBlock(gfx++, gHeartFullTex, G_IM_FMT_IA, G_IM_SIZ_8b, 16, 16, 0, G_TX_NOMIRROR | G_TX_WRAP,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    } else if ((msgCtx->itemId >= ITEM_RUPEE_GREEN) && (msgCtx->itemId <= ITEM_RUPEE_HUGE)) {
        index = msgCtx->itemId - ITEM_RUPEE_GREEN;
        gDPPipeSync(gfx++);
        gDPSetPrimColor(gfx++, 0, 0, D_801CFDEC[index].r, D_801CFDEC[index].g, D_801CFDEC[index].b,
                        msgCtx->textColorAlpha);
        gDPSetEnvColor(gfx++, 0, 80, 0, 255);
        gDPLoadTextureBlock(gfx++, gRupeeCounterIconTex, G_IM_FMT_IA, G_IM_SIZ_8b, 16, 16, 0, G_TX_NOMIRROR | G_TX_WRAP,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    } else if (msgCtx->itemId == ITEM_STRAY_FAIRIES) {
        msgCtx->unk12016 = 0x18;
        gDPPipeSync(gfx++);
        gDPSetPrimColor(gfx++, 0, 0, sStrayFairyIconPrimColors[((void)0, gSaveContext.dungeonSceneSharedIndex)].r,
                        sStrayFairyIconPrimColors[((void)0, gSaveContext.dungeonSceneSharedIndex)].g,
                        sStrayFairyIconPrimColors[((void)0, gSaveContext.dungeonSceneSharedIndex)].b,
                        msgCtx->textColorAlpha);
        gDPSetEnvColor(gfx++, sStrayFairyIconEnvColors[((void)0, gSaveContext.dungeonSceneSharedIndex)].r,
                       sStrayFairyIconEnvColors[((void)0, gSaveContext.dungeonSceneSharedIndex)].g,
                       sStrayFairyIconEnvColors[((void)0, gSaveContext.dungeonSceneSharedIndex)].b, 0);
        gDPLoadTextureBlock_4b(gfx++, gStrayFairyGlowingCircleIconTex, G_IM_FMT_I, 32, 24, 0, G_TX_NOMIRROR | G_TX_WRAP,
                               G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(gfx++, msgCtx->unk12010 << 2, msgCtx->unk12012 << 2,
                            (msgCtx->unk12010 + msgCtx->unk12014) << 2, (msgCtx->unk12012 + msgCtx->unk12016) << 2,
                            G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);
        gDPSetPrimColor(gfx++, 0, 0, 255, 255, 255, msgCtx->textColorAlpha);
        gDPLoadTextureBlock(gfx++, sStrayFairyIconTextures[((void)0, gSaveContext.dungeonSceneSharedIndex)],
                            G_IM_FMT_RGBA, G_IM_SIZ_32b, 32, 24, 0, G_TX_NOMIRROR | G_TX_WRAP,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    } else if ((msgCtx->itemId >= ITEM_SONG_SONATA) && (msgCtx->itemId <= ITEM_SONG_SUN)) {
        index = msgCtx->itemId - ITEM_SONG_SONATA;
        gDPSetPrimColor(gfx++, 0, 0, D_801CFE04[index], D_801CFE1C[index], D_801CFE34[index], msgCtx->textColorAlpha);
        gDPLoadTextureBlock(gfx++, msgCtx->textboxSegment[TEXTBOX_SEG_ICON], G_IM_FMT_IA, G_IM_SIZ_8b, 16, 24, 0,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                            G_TX_NOLOD);
        msgCtx->unk12016 = 0x18;
    } else if (msgCtx->itemId == ITEM_CC) {
        gDPPipeSync(gfx++);
        gDPSetPrimColor(gfx++, 0, 0, 255, 255, 0, msgCtx->textColorAlpha);
        gDPSetEnvColor(gfx++, 0, 0, 0, 255);
        gDPLoadTextureBlock(gfx++, msgCtx->textboxSegment[TEXTBOX_SEG_ICON], G_IM_FMT_IA, G_IM_SIZ_8b, 32, 32, 0,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                            G_TX_NOLOD);
    } else if (msgCtx->itemId >= ITEM_B8) {
        gDPLoadTextureBlock(gfx++, msgCtx->textboxSegment[TEXTBOX_SEG_ICON], G_IM_FMT_RGBA, G_IM_SIZ_16b, 32, 32, 0,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                            G_TX_NOLOD);
    } else if (msgCtx->itemId >= ITEM_SKULL_TOKEN) {
        gDPLoadTextureBlock(gfx++, msgCtx->textboxSegment[TEXTBOX_SEG_ICON], G_IM_FMT_RGBA, G_IM_SIZ_32b, 24, 24, 0,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                            G_TX_NOLOD);
    } else {
        msgCtx->unk12016 = msgCtx->unk12014;
        gDPLoadTextureBlock(gfx++, msgCtx->textboxSegment[TEXTBOX_SEG_ICON], G_IM_FMT_RGBA, G_IM_SIZ_32b, 32, 32, 0,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                            G_TX_NOLOD);
    }

    gSPTextureRectangle(gfx++, msgCtx->unk12010 << 2, msgCtx->unk12012 << 2, (msgCtx->unk12010 + msgCtx->unk12014) << 2,
                        (msgCtx->unk12012 + msgCtx->unk12016) << 2, G_TX_RENDERTILE, 0, 0, D_801F6B08, D_801F6B08);
    gDPPipeSync(gfx++);
    gDPSetCombineLERP(gfx++, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0);

    if (!play->pauseCtx.bombersNotebookOpen) {
        msgCtx->textPosX += 16;
    } else {
        msgCtx->textPosX += 50;
    }
    *gfxP = gfx;
}

void Message_HandleOcarina(PlayState* play) {
    MessageContext* msgCtx = &play->msgCtx;

    if (msgCtx->msgMode == MSGMODE_TEXT_DISPLAYING) {
        play->msgCtx.ocarinaMode = OCARINA_MODE_ACTIVE;
        if (msgCtx->ocarinaAction == OCARINA_ACTION_SCARECROW_LONG_RECORDING) {
            msgCtx->msgMode = MSGMODE_SCARECROW_LONG_RECORDING_START;
        } else if (msgCtx->ocarinaAction == OCARINA_ACTION_SCARECROW_LONG_DEMONSTRATION) {
            AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_DEFAULT);
            AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_DEFAULT);
            msgCtx->ocarinaStaff = AudioOcarina_GetPlaybackStaff();
            msgCtx->ocarinaStaff->pos = 0;
            sOcarinaButtonIndexBufPos = sOcarinaButtonIndexBufLen = 0;
            Message_ResetOcarinaButtonState(play);
            msgCtx->stateTimer = 3;
            msgCtx->msgMode = MSGMODE_SCARECROW_LONG_DEMONSTRATION;
            AudioOcarina_SetPlaybackSong(OCARINA_SONG_SCARECROW_LONG + 1, 1);
        } else if (msgCtx->ocarinaAction == OCARINA_ACTION_SCARECROW_SPAWN_RECORDING) {
            msgCtx->msgMode = MSGMODE_SCARECROW_SPAWN_RECORDING_START;
        } else if (msgCtx->ocarinaAction == OCARINA_ACTION_SCARECROW_SPAWN_DEMONSTRATION) {
            AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_DEFAULT);
            AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_DEFAULT);
            msgCtx->ocarinaStaff = AudioOcarina_GetPlaybackStaff();
            msgCtx->ocarinaStaff->pos = 0;
            sOcarinaButtonIndexBufPos = sOcarinaButtonIndexBufLen = 0;
            Message_ResetOcarinaButtonState(play);
            msgCtx->stateTimer = 3;
            msgCtx->msgMode = MSGMODE_SCARECROW_SPAWN_DEMONSTRATION;
            AudioOcarina_SetPlaybackSong(OCARINA_SONG_SCARECROW_SPAWN + 1, 1);
        } else if (msgCtx->ocarinaAction == OCARINA_ACTION_37) {
            msgCtx->msgMode = MSGMODE_2F;
        } else if (msgCtx->ocarinaAction == OCARINA_ACTION_3A) {
            msgCtx->msgMode = MSGMODE_32;
        } else if ((msgCtx->ocarinaAction >= OCARINA_ACTION_TIMED_PROMPT_SONATA) &&
                   (msgCtx->ocarinaAction <= OCARINA_ACTION_TIMED_PROMPT_STORMS)) {
            msgCtx->msgMode = MSGMODE_35;
        } else if (((msgCtx->ocarinaAction >= OCARINA_ACTION_DEMONSTRATE_SONATA) &&
                    (msgCtx->ocarinaAction <= OCARINA_ACTION_DEMONSTRATE_GORON_LULLABY_INTRO)) ||
                   (msgCtx->ocarinaAction == OCARINA_ACTION_3B) || (msgCtx->ocarinaAction == OCARINA_ACTION_3C) ||
                   (msgCtx->ocarinaAction == OCARINA_ACTION_DEMONSTRATE_EVAN_PART1_FIRST_HALF) ||
                   (msgCtx->ocarinaAction == OCARINA_ACTION_DEMONSTRATE_EVAN_PART2_FIRST_HALF) ||
                   (msgCtx->ocarinaAction == OCARINA_ACTION_DEMONSTRATE_EVAN_PART1_SECOND_HALF) ||
                   (msgCtx->ocarinaAction == OCARINA_ACTION_DEMONSTRATE_EVAN_PART2_SECOND_HALF)) {
            msgCtx->msgMode = MSGMODE_SONG_DEMONSTRATION_STARTING;
        } else if ((msgCtx->ocarinaAction == OCARINA_ACTION_PROMPT_EVAN_PART1_SECOND_HALF) ||
                   (msgCtx->ocarinaAction == OCARINA_ACTION_PROMPT_EVAN_PART2_SECOND_HALF) ||
                   (msgCtx->ocarinaAction == OCARINA_ACTION_PROMPT_WIND_FISH_HUMAN) ||
                   (msgCtx->ocarinaAction == OCARINA_ACTION_PROMPT_WIND_FISH_GORON) ||
                   (msgCtx->ocarinaAction == OCARINA_ACTION_PROMPT_WIND_FISH_ZORA) ||
                   (msgCtx->ocarinaAction == OCARINA_ACTION_PROMPT_WIND_FISH_DEKU)) {
            msgCtx->msgMode = MSGMODE_SONG_PROMPT_STARTING;
        } else if ((msgCtx->ocarinaAction == OCARINA_ACTION_0) || (msgCtx->ocarinaAction == OCARINA_ACTION_FREE_PLAY) ||
                   (msgCtx->ocarinaAction >= OCARINA_ACTION_CHECK_TIME)) {
            msgCtx->msgMode = MSGMODE_OCARINA_STARTING;
        } else {
            msgCtx->msgMode = MSGMODE_SONG_PROMPT_STARTING;
        }
    }
}

Color_RGB16 D_801CFE74[] = {
    { 255, 120, 0 },  { 70, 255, 80 },   { 80, 110, 255 },  { 255, 255, 30 },
    { 90, 180, 255 }, { 210, 100, 255 }, { 170, 170, 170 }, { 255, 130, 30 },
};
static Color_RGB16 sColorsNormalJPN[] = {
    { 255, 60, 60 },  { 70, 255, 80 },   { 80, 90, 255 },   { 255, 255, 50 },
    { 80, 150, 255 }, { 255, 150, 180 }, { 170, 170, 170 }, { 255, 130, 30 },
};
static Color_RGB16 sColorsButtonsJPN[] = {
    { 255, 60, 60 },  { 70, 255, 80 },   { 80, 90, 255 },   { 255, 255, 50 },
    { 80, 150, 255 }, { 255, 150, 180 }, { 180, 180, 200 }, { 255, 130, 30 },
};
Color_RGB16 D_801CFF04[] = {
    { 195, 0, 0 },    { 70, 255, 80 },   { 80, 90, 255 },   { 255, 255, 50 },
    { 80, 150, 255 }, { 255, 150, 180 }, { 170, 170, 170 }, { 255, 130, 30 },
};
static Color_RGB16 sColorsBombersNotebookJPN[] = {
    { 255, 60, 60 },  { 110, 170, 255 }, { 80, 90, 255 },   { 255, 255, 50 },
    { 80, 150, 255 }, { 255, 150, 180 }, { 170, 170, 170 }, { 255, 130, 30 },
};

static u8 sButtonColorIndicesJPN[] = { 2, 1, 3, 6, 6, 6, 3, 3, 3, 3, 1, 6 };

#define MESSAGE_ITEM_NONE 9999

void Message_DrawTextDefault(PlayState* play, Gfx** gfxP) {
    MessageContext* msgCtx = &play->msgCtx;
    u16 j;
    Font* font = &play->msgCtx.font;
    u16 i;
    u16 charTexIndex;
    Gfx* gfx = *gfxP;
    u16 character;
    s16 sp130;
    s16 prevR;
    s16 prevG;
    s16 prevB;
    u16 lookAheadCharacter;

    play->msgCtx.textPosX = play->msgCtx.unk11F1A[0] + play->msgCtx.unk11FF8;
    play->msgCtx.textPosY = play->msgCtx.unk11FFA;

    sp130 = 0;
    if (play->msgCtx.itemId != MESSAGE_ITEM_NONE) {
        Message_DrawItemIcon(play, &gfx);
    }
    msgCtx->textColorR = msgCtx->unk120C8;
    msgCtx->textColorG = msgCtx->unk120CA;
    msgCtx->textColorB = msgCtx->unk120CC;

    charTexIndex = 0;

    for (i = 0; i < msgCtx->textDrawPos; i++) {
        character = msgCtx->decodedBuffer.wchar[i];

        switch (character) {
            case 0x2000:
                if (play->pauseCtx.bombersNotebookOpen || (msgCtx->textBoxType == TEXTBOX_TYPE_D)) {
                    msgCtx->textColorR = msgCtx->textColorG = msgCtx->textColorB = 0;
                } else if (msgCtx->textBoxType == TEXTBOX_TYPE_5) {
                    msgCtx->textColorR = msgCtx->textColorG = msgCtx->textColorB = 0;
                } else {
                    msgCtx->textColorR = msgCtx->textColorG = msgCtx->textColorB = 255;
                }
                if ((i + 1) == msgCtx->textDrawPos) {
                    msgCtx->textDrawPos++;
                }
                break;

            case 0x2001:
            case 0x2002:
            case 0x2003:
            case 0x2004:
            case 0x2005:
            case 0x2006:
            case 0x2007:
            case 0x2008:
                if ((msgCtx->msgMode >= MSGMODE_NEW_CYCLE_0) && (msgCtx->msgMode <= MSGMODE_NEW_CYCLE_2) &&
                    (character == 0x2002)) {
                    msgCtx->textDrawPos = msgCtx->decodedTextLen;
                    if (msgCtx->unk120D6) {
                        msgCtx->unk120D4 += 25;
                        if (msgCtx->unk120D4 >= 100) {
                            msgCtx->unk120D6 = false;
                        }
                    } else {
                        msgCtx->unk120D4 -= 25;
                        if (msgCtx->unk120D4 <= -100) {
                            msgCtx->unk120D6 = true;
                        }
                    }

                    if (D_801CFE74[(s16)(character - 0x2001)].r + msgCtx->unk120D4 < 0) {
                        msgCtx->textColorR = 0;
                    } else {
                        msgCtx->textColorR = D_801CFE74[(s16)(character - 0x2001)].r + msgCtx->unk120D4;
                    }
                    if (D_801CFE74[(s16)(character - 0x2001)].g + msgCtx->unk120D4 >= 255) {
                        msgCtx->textColorG = D_801CFE74[(s16)(character - 0x2001)].g;
                    } else {
                        msgCtx->textColorG = D_801CFE74[(s16)(character - 0x2001)].g + msgCtx->unk120D4;
                    }
                    if (D_801CFE74[(s16)(character - 0x2001)].b + msgCtx->unk120D4 < 0) {
                        msgCtx->textColorB = 0;
                    } else {
                        msgCtx->textColorB = D_801CFE74[(s16)(character - 0x2001)].b + msgCtx->unk120D4;
                    }
                } else if (play->pauseCtx.bombersNotebookOpen) {
                    msgCtx->textColorR = sColorsBombersNotebookJPN[(s16)(character - 0x2001)].r;
                    msgCtx->textColorG = sColorsBombersNotebookJPN[(s16)(character - 0x2001)].g;
                    msgCtx->textColorB = sColorsBombersNotebookJPN[(s16)(character - 0x2001)].b;
                } else if (msgCtx->textBoxType == TEXTBOX_TYPE_1) {
                    msgCtx->textColorR = D_801CFE74[(s16)(character - 0x2001)].r;
                    msgCtx->textColorG = D_801CFE74[(s16)(character - 0x2001)].g;
                    msgCtx->textColorB = D_801CFE74[(s16)(character - 0x2001)].b;
                } else if (msgCtx->textBoxType == TEXTBOX_TYPE_D) {
                    msgCtx->textColorR = D_801CFF04[(s16)(character - 0x2001)].r;
                    msgCtx->textColorG = D_801CFF04[(s16)(character - 0x2001)].g;
                    msgCtx->textColorB = D_801CFF04[(s16)(character - 0x2001)].b;
                } else {
                    msgCtx->textColorR = sColorsNormalJPN[(s16)(character - 0x2001)].r;
                    msgCtx->textColorG = sColorsNormalJPN[(s16)(character - 0x2001)].g;
                    msgCtx->textColorB = sColorsNormalJPN[(s16)(character - 0x2001)].b;
                }

                if ((i + 1) == msgCtx->textDrawPos) {
                    msgCtx->textDrawPos++;
                }
                break;

            case 0x20: // ` `
                msgCtx->textPosX += 6;
                break;

            case 0xA:
                msgCtx->textPosY += msgCtx->unk11FFC;
                // fallthrough
            case 0xC:
                sp130++;
                msgCtx->textPosX = msgCtx->unk11F1A[sp130] + msgCtx->unk11FF8;
                if (msgCtx->choiceNum == 1) {
                    if (!play->pauseCtx.bombersNotebookOpen) {
                        msgCtx->textPosX += 32;
                    } else {
                        msgCtx->textPosX += 57;
                    }
                } else if (msgCtx->choiceNum == 2) {
                    if (!play->pauseCtx.bombersNotebookOpen) {
                        msgCtx->textPosX += 22;
                    } else {
                        msgCtx->textPosX += 57;
                    }
                }
                continue;

            case 0x9:
            case 0xB:
                if (msgCtx->msgMode == MSGMODE_TEXT_DISPLAYING) {
                    if (!msgCtx->textboxSkipped) {
                        Audio_PlaySfx(NA_SE_NONE);
                        msgCtx->msgMode = MSGMODE_TEXT_AWAIT_NEXT;
                        Font_LoadMessageBoxEndIcon(font, 0);
                    } else {
                        msgCtx->msgMode = MSGMODE_TEXT_NEXT_MSG;
                        msgCtx->textUnskippable = false;
                        msgCtx->msgBufPos++;
                    }
                    msgCtx->unk120CE = msgCtx->textColorR;
                    msgCtx->unk120D0 = msgCtx->textColorG;
                    msgCtx->unk120D2 = msgCtx->textColorB;
                }
                *gfxP = gfx;
                return;

            case 0x101: // MESSAGE_QUICKTEXT_ENABLE
                if (((i + 1) == msgCtx->textDrawPos) &&
                    ((msgCtx->msgMode == MSGMODE_TEXT_DISPLAYING) ||
                     ((msgCtx->msgMode >= MSGMODE_OCARINA_STARTING) && (msgCtx->msgMode <= MSGMODE_26)))) {

                    j = i;
                    while (true) {
                        if ((msgCtx->decodedBuffer.wchar[j] != 0x102) && (msgCtx->decodedBuffer.wchar[j] != 0x104) &&
                            (msgCtx->decodedBuffer.wchar[j] != 0x103) && (msgCtx->decodedBuffer.wchar[j] != 0x230) &&
                            (msgCtx->decodedBuffer.wchar[j] != 9) && (msgCtx->decodedBuffer.wchar[j] != 0xB)) {
                            j++;
                            continue;
                        }
                        break;
                    }

                    i = j - 1;
                    msgCtx->textDrawPos = i + 1;
                }
                break;

            case 0x102: // MESSAGE_QUICKTEXT_DISABLE
                break;

            case 0x110: // MESSAGE_BOX_BREAK_DELAYED
                if (msgCtx->msgMode == MSGMODE_TEXT_DISPLAYING) {
                    msgCtx->stateTimer = msgCtx->decodedBuffer.wchar[++i];
                    msgCtx->msgMode = MSGMODE_TEXT_DELAYED_BREAK;
                }
                *gfxP = gfx;
                return;

            case 0x111:
                if (msgCtx->msgMode == MSGMODE_TEXT_DISPLAYING) {
                    msgCtx->msgMode = MSGMODE_TEXT_DONE;
                    if (msgCtx->unk11F0C == 3) {
                        msgCtx->textboxEndType = TEXTBOX_ENDTYPE_FADE_STAGES_1;
                    } else {
                        msgCtx->textboxEndType = TEXTBOX_ENDTYPE_FADE_NORMAL;
                    }
                    msgCtx->stateTimer = msgCtx->decodedBuffer.wchar[++i];
                    Font_LoadMessageBoxEndIcon(&play->msgCtx.font, 1);
                    if (play->csCtx.state == CS_STATE_IDLE) {
                        Interface_SetAButtonDoAction(play, DO_ACTION_RETURN);
                    }
                }
                *gfxP = gfx;
                return;

            case 0x112:
                if (msgCtx->msgMode == MSGMODE_TEXT_DISPLAYING) {
                    msgCtx->msgMode = MSGMODE_TEXT_DONE;
                    msgCtx->textboxEndType = TEXTBOX_ENDTYPE_FADE_SKIPPABLE;
                    msgCtx->stateTimer = msgCtx->decodedBuffer.wchar[++i];
                    Font_LoadMessageBoxEndIcon(&play->msgCtx.font, 1);
                    if (play->csCtx.state == CS_STATE_IDLE) {
                        Interface_SetAButtonDoAction(play, DO_ACTION_RETURN);
                    }
                }
                *gfxP = gfx;
                return;

            case 0x120: // MESSAGE_SFX
                if (((i + 1) == msgCtx->textDrawPos) && (msgCtx->msgMode == MSGMODE_TEXT_DISPLAYING)) {
                    Audio_PlaySfx(msgCtx->decodedBuffer.wchar[i + 1]);
                }
                if ((i + 1) == msgCtx->textDrawPos) {
                    msgCtx->textDrawPos++;
                }
                i++;
                break;

            case 0x128:
                if (((i + 1) == msgCtx->textDrawPos) && (msgCtx->msgMode == MSGMODE_TEXT_DISPLAYING)) {
                    msgCtx->msgMode = MSGMODE_9;
                    msgCtx->textDelayTimer = msgCtx->decodedBuffer.wchar[i + 1];
                }
                i++;
                break;

            case 0x130:
            case 0x131:
            case 0x132:
            case 0x133:
            case 0x134:
            case 0x135:
            case 0x136:
                msgCtx->textDelay = character - 0x130;
                if ((i + 1) == msgCtx->textDrawPos) {
                    msgCtx->textDrawPos++;
                }
                break;

            case 0x201: // MESSAGE_BACKGROUND
                msgCtx->textPosX = 45;

                if (msgCtx->msgMode == MSGMODE_TEXT_DISPLAYING) {
                    Audio_PlaySfx(NA_SE_NONE);
                }

                gDPPipeSync(gfx++);
                gDPSetCombineMode(gfx++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
                gDPSetPrimColor(gfx++, 0, 0, 0, 0, 0, msgCtx->textColorAlpha);
                gDPLoadTextureBlock_4b(gfx++, msgCtx->textboxSegment[TEXTBOX_SEG_BG_1], G_IM_FMT_I, 96, 48, 0,
                                       G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                                       G_TX_NOLOD, G_TX_NOLOD);
                gSPTextureRectangle(gfx++, msgCtx->textPosX << 2, (msgCtx->unk12012 + 1) << 2,
                                    (msgCtx->textPosX + 96) << 2, (msgCtx->unk12012 + 49) << 2, G_TX_RENDERTILE, 0, 0,
                                    1 << 10, 1 << 10);
                gDPLoadTextureBlock_4b(gfx++, msgCtx->textboxSegment[TEXTBOX_SEG_BG_2], G_IM_FMT_I, 96, 48, 0,
                                       G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                                       G_TX_NOLOD, G_TX_NOLOD);
                gSPTextureRectangle(gfx++, (msgCtx->textPosX + 96) << 2, (msgCtx->unk12012 + 1) << 2,
                                    (msgCtx->textPosX + 193) << 2, (msgCtx->unk12012 + 49) << 2, G_TX_RENDERTILE, 0, 0,
                                    1 << 10, 1 << 10);

                gDPPipeSync(gfx++);
                gDPSetPrimColor(gfx++, 0, 0, 255, 60, 0, msgCtx->textColorAlpha);
                gDPLoadTextureBlock_4b(gfx++, msgCtx->textboxSegment[TEXTBOX_SEG_BG_1], G_IM_FMT_I, 96, 48, 0,
                                       G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                                       G_TX_NOLOD, G_TX_NOLOD);
                gSPTextureRectangle(gfx++, msgCtx->textPosX << 2, msgCtx->unk12012 << 2, (msgCtx->textPosX + 96) << 2,
                                    (msgCtx->unk12012 + 48) << 2, G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);
                gDPLoadTextureBlock_4b(gfx++, msgCtx->textboxSegment[TEXTBOX_SEG_BG_2], G_IM_FMT_I, 96, 48, 0,
                                       G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                                       G_TX_NOLOD, G_TX_NOLOD);
                gSPTextureRectangle(gfx++, (msgCtx->textPosX + 96) << 2, msgCtx->unk12012 << 2,
                                    (msgCtx->textPosX + 192) << 2, (msgCtx->unk12012 + 48) << 2, G_TX_RENDERTILE, 0, 0,
                                    1 << 10, 1 << 10);
                gDPPipeSync(gfx++);
                gDPSetCombineLERP(gfx++, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, PRIMITIVE, TEXEL0, 0,
                                  PRIMITIVE, 0);

                msgCtx->textPosX += 32;
                break;

            case 0x202:
                msgCtx->textboxEndType = TEXTBOX_ENDTYPE_TWO_CHOICE;

                if (msgCtx->msgMode == MSGMODE_TEXT_DISPLAYING) {
                    msgCtx->choiceTextId = msgCtx->currentTextId;
                    msgCtx->stateTimer = 4;
                    Font_LoadMessageBoxEndIcon(&play->msgCtx.font, 2);
                }
                break;

            case 0x203:
                msgCtx->textboxEndType = TEXTBOX_ENDTYPE_THREE_CHOICE;

                if (msgCtx->msgMode == MSGMODE_TEXT_DISPLAYING) {
                    msgCtx->choiceTextId = msgCtx->currentTextId;
                    msgCtx->stateTimer = 4;
                    Font_LoadMessageBoxEndIcon(&play->msgCtx.font, 2);
                }
                break;

            case 0x20C:
                msgCtx->textboxEndType = TEXTBOX_ENDTYPE_INPUT_BANK;

                if (msgCtx->msgMode == MSGMODE_TEXT_DISPLAYING) {
                    Font_LoadMessageBoxEndIcon(&play->msgCtx.font, 1);
                }
                break;

            case 0x220:
                msgCtx->textboxEndType = TEXTBOX_ENDTYPE_INPUT_DOGGY_RACETRACK_BET;

                if (msgCtx->msgMode == MSGMODE_TEXT_DISPLAYING) {
                    Font_LoadMessageBoxEndIcon(&play->msgCtx.font, 1);
                }
                break;

            case 0x221:
                msgCtx->textboxEndType = TEXTBOX_ENDTYPE_INPUT_BOMBER_CODE;

                if (msgCtx->msgMode == MSGMODE_TEXT_DISPLAYING) {
                    Font_LoadMessageBoxEndIcon(&play->msgCtx.font, 1);
                }
                break;

            case 0x222:
                if (msgCtx->msgMode == MSGMODE_TEXT_DISPLAYING) {
                    msgCtx->msgMode = MSGMODE_TEXT_DONE;
                    msgCtx->textboxEndType = TEXTBOX_ENDTYPE_PAUSE_MENU;
                    Font_LoadMessageBoxEndIcon(&play->msgCtx.font, 0);
                    Audio_PlaySfx(NA_SE_SY_MESSAGE_END);
                }
                break;

            case 0x225:
                msgCtx->textboxEndType = TEXTBOX_ENDTYPE_INPUT_LOTTERY_CODE;

                if (msgCtx->msgMode == MSGMODE_TEXT_DISPLAYING) {
                    Font_LoadMessageBoxEndIcon(&play->msgCtx.font, 1);
                }
                break;

            case 0x240:
            case 0x500:
                if (msgCtx->msgMode == MSGMODE_TEXT_DISPLAYING) {
                    if (msgCtx->textBoxType == TEXTBOX_TYPE_3) {
                        Message_HandleOcarina(play);
                        *gfxP = gfx;
                        return;
                    }
                    msgCtx->msgMode = MSGMODE_TEXT_DONE;
                    if (msgCtx->textboxEndType == TEXTBOX_ENDTYPE_DEFAULT) {
                        Audio_PlaySfx(NA_SE_SY_MESSAGE_END);
                        if (character == 0x500) {
                            Font_LoadMessageBoxEndIcon(font, 1);
                        } else {
                            Font_LoadMessageBoxEndIcon(font, 0);
                        }
                        if (play->csCtx.state == CS_STATE_IDLE) {
                            Interface_SetAButtonDoAction(play, DO_ACTION_RETURN);
                        }
                    } else {
                        Audio_PlaySfx(NA_SE_NONE);
                    }
                }
                *gfxP = gfx;
                return;

            case 0x104:
                if (msgCtx->msgMode == MSGMODE_TEXT_DISPLAYING) {
                    Audio_PlaySfx(NA_SE_NONE);
                    msgCtx->msgMode = MSGMODE_TEXT_DONE;
                    msgCtx->textboxEndType = TEXTBOX_ENDTYPE_PERSISTENT;
                }
                *gfxP = gfx;
                return;

            case 0x103:
                if (msgCtx->msgMode == MSGMODE_TEXT_DISPLAYING) {
                    msgCtx->msgMode = MSGMODE_TEXT_DONE;
                    msgCtx->textboxEndType = TEXTBOX_ENDTYPE_EVENT;
                    Font_LoadMessageBoxEndIcon(&play->msgCtx.font, 0);
                    Audio_PlaySfx(NA_SE_SY_MESSAGE_END);
                }
                *gfxP = gfx;
                return;

            case 0x230:
                if (msgCtx->msgMode == MSGMODE_TEXT_DISPLAYING) {
                    msgCtx->msgMode = MSGMODE_TEXT_DONE;
                    msgCtx->textboxEndType = TEXTBOX_ENDTYPE_EVENT2;
                    Font_LoadMessageBoxEndIcon(&play->msgCtx.font, 1);
                    Audio_PlaySfx(NA_SE_SY_MESSAGE_END);
                }
                *gfxP = gfx;
                return;

            default:
                switch (character) {
                    case 0x8169:
                    case 0x8175:
                        msgCtx->textPosX -= TRUNCF_BINANG(6.0f * msgCtx->textCharScale);
                        break;

                    case 0x8145:
                        msgCtx->textPosX -= TRUNCF_BINANG(3.0f * msgCtx->textCharScale);
                        break;

                    case 0x8148:
                    case 0x8149:
                        msgCtx->textPosX -= TRUNCF_BINANG(2.0f * msgCtx->textCharScale);
                        break;

                    default:
                        break;
                }

                if ((msgCtx->msgMode == MSGMODE_TEXT_DISPLAYING) && ((i + 1) == msgCtx->textDrawPos)) {
                    Audio_PlaySfx(NA_SE_NONE);
                }

                if ((character >= 0x839F) && (character < 0x83AB)) {
                    prevR = msgCtx->textColorR;
                    prevG = msgCtx->textColorG;
                    prevB = msgCtx->textColorB;
                    msgCtx->textColorR = sColorsButtonsJPN[(s16)sButtonColorIndicesJPN[character - 0x839F]].r;
                    msgCtx->textColorG = sColorsButtonsJPN[(s16)sButtonColorIndicesJPN[character - 0x839F]].g;
                    msgCtx->textColorB = sColorsButtonsJPN[(s16)sButtonColorIndicesJPN[character - 0x839F]].b;
                    Message_DrawTextChar(play, &font->charBuf[font->unk_11D88][charTexIndex], &gfx);
                    msgCtx->textColorR = prevR;
                    msgCtx->textColorG = prevG;
                    msgCtx->textColorB = prevB;
                } else {
                    Message_DrawTextChar(play, &font->charBuf[font->unk_11D88][charTexIndex], &gfx);
                }
                charTexIndex += FONT_CHAR_TEX_SIZE;
                switch (character) {
                    case 0x8144:
                        msgCtx->textPosX += TRUNCF_BINANG(8.0f * msgCtx->textCharScale);
                        break;

                    case 0x816A:
                    case 0x8176:
                        msgCtx->textPosX += TRUNCF_BINANG(10.0f * msgCtx->textCharScale);
                        break;

                    case 0x8141:
                    case 0x8142:
                    case 0x8168:
                        msgCtx->textPosX += TRUNCF_BINANG(12.0f * msgCtx->textCharScale);
                        break;

                    case 0x8194:
                        msgCtx->textPosX += TRUNCF_BINANG(14.0f * msgCtx->textCharScale);
                        break;

                    case 0x8145:
                        msgCtx->textPosX += TRUNCF_BINANG(15.0f * msgCtx->textCharScale);
                        break;

                    default:
                        if ((msgCtx->msgMode >= MSGMODE_SCENE_TITLE_CARD_FADE_IN_BACKGROUND) &&
                            (msgCtx->msgMode <= MSGMODE_SCENE_TITLE_CARD_FADE_OUT_BACKGROUND)) {
                            msgCtx->textPosX += TRUNCF_BINANG((16.0f * msgCtx->textCharScale) - 1.0f);
                        } else {
                            msgCtx->textPosX += TRUNCF_BINANG(16.0f * msgCtx->textCharScale);
                        }
                        break;
                }
                break;
        }
    }

    if (msgCtx->textDelayTimer == 0) {
        msgCtx->textDrawPos = i + 1;
        msgCtx->textDelayTimer = 0;
        if (msgCtx->msgMode == MSGMODE_9) {
            msgCtx->msgMode = MSGMODE_TEXT_DISPLAYING;
        }
    } else {
        msgCtx->textDelayTimer--;
    }
    *gfxP = gfx;
}

s16 D_801CFF70[LANGUAGE_MAX] = {
    4,    // LANGUAGE_JPN
    0x12, // LANGUAGE_ENG
    0x12, // LANGUAGE_GER
    0x12, // LANGUAGE_FRE
    0x12, // LANGUAGE_SPA
};
s16 D_801CFF7C[LANGUAGE_MAX] = {
    0,   // LANGUAGE_JPN
    0xE, // LANGUAGE_ENG
    0xE, // LANGUAGE_GER
    0xE, // LANGUAGE_FRE
    0xE, // LANGUAGE_SPA
};
s16 D_801CFF88[LANGUAGE_MAX] = {
    0,    // LANGUAGE_JPN
    0x16, // LANGUAGE_ENG
    0x16, // LANGUAGE_GER
    0x16, // LANGUAGE_FRE
    0x16, // LANGUAGE_SPA
};

s16 D_801CFF94[] = {
    /* 0x00 */ MESSAGE_ITEM_NONE,
    /* 0x01 */ ITEM_RUPEE_GREEN,
    /* 0x02 */ ITEM_RUPEE_BLUE,
    /* 0x03 */ ITEM_RUPEE_10,
    /* 0x04 */ ITEM_RUPEE_RED,
    /* 0x05 */ ITEM_RUPEE_PURPLE,
    /* 0x06 */ ITEM_RUPEE_SILVER,
    /* 0x07 */ ITEM_RUPEE_HUGE,
    /* 0x08 */ ITEM_WALLET_ADULT,
    /* 0x09 */ ITEM_WALLET_GIANT,
    /* 0x0A */ ITEM_RECOVERY_HEART,
    /* 0x0B */ ITEM_RECOVERY_HEART,
    /* 0x0C */ ITEM_HEART_PIECE,
    /* 0x0D */ ITEM_HEART_CONTAINER,
    /* 0x0E */ ITEM_MAGIC_JAR_SMALL,
    /* 0x0F */ ITEM_MAGIC_JAR_BIG,
    /* 0x10 */ ITEM_RECOVERY_HEART,
    /* 0x11 */ ITEM_STRAY_FAIRIES,
    /* 0x12 */ ITEM_RECOVERY_HEART,
    /* 0x13 */ ITEM_RECOVERY_HEART,
    /* 0x14 */ ITEM_BOMB,
    /* 0x15 */ ITEM_BOMB,
    /* 0x16 */ ITEM_BOMB,
    /* 0x17 */ ITEM_BOMB,
    /* 0x18 */ ITEM_BOMB,
    /* 0x19 */ ITEM_DEKU_STICK,
    /* 0x1A */ ITEM_BOMBCHU,
    /* 0x1B */ ITEM_BOMB_BAG_20,
    /* 0x1C */ ITEM_BOMB_BAG_30,
    /* 0x1D */ ITEM_BOMB_BAG_40,
    /* 0x1E */ ITEM_BOW,
    /* 0x1F */ ITEM_BOW,
    /* 0x20 */ ITEM_BOW,
    /* 0x21 */ ITEM_BOW,
    /* 0x22 */ ITEM_QUIVER_30,
    /* 0x23 */ ITEM_QUIVER_40,
    /* 0x24 */ ITEM_QUIVER_50,
    /* 0x25 */ ITEM_ARROW_FIRE,
    /* 0x26 */ ITEM_ARROW_ICE,
    /* 0x27 */ ITEM_ARROW_LIGHT,
    /* 0x28 */ ITEM_DEKU_NUT,
    /* 0x29 */ ITEM_DEKU_NUT,
    /* 0x2A */ ITEM_DEKU_NUT,
    /* 0x2B */ MESSAGE_ITEM_NONE,
    /* 0x2C */ MESSAGE_ITEM_NONE,
    /* 0x2D */ MESSAGE_ITEM_NONE,
    /* 0x2E */ MESSAGE_ITEM_NONE,
    /* 0x2F */ ITEM_DEKU_STICK_UPGRADE_20,
    /* 0x30 */ MESSAGE_ITEM_NONE,
    /* 0x31 */ MESSAGE_ITEM_NONE,
    /* 0x32 */ ITEM_SHIELD_HERO,
    /* 0x33 */ ITEM_SHIELD_MIRROR,
    /* 0x34 */ ITEM_POWDER_KEG,
    /* 0x35 */ ITEM_MAGIC_BEANS,
    /* 0x36 */ ITEM_PICTOGRAPH_BOX,
    /* 0x37 */ ITEM_SWORD_KOKIRI,
    /* 0x38 */ ITEM_SWORD_RAZOR,
    /* 0x39 */ ITEM_SWORD_GILDED,
    /* 0x3A */ ITEM_SWORD_DEITY,
    /* 0x3B */ ITEM_SWORD_GREAT_FAIRY,
    /* 0x3C */ ITEM_KEY_SMALL,
    /* 0x3D */ ITEM_KEY_BOSS,
    /* 0x3E */ ITEM_DUNGEON_MAP,
    /* 0x3F */ ITEM_COMPASS,
    /* 0x40 */ ITEM_POWDER_KEG,
    /* 0x41 */ ITEM_HOOKSHOT,
    /* 0x42 */ ITEM_LENS_OF_TRUTH,
    /* 0x43 */ ITEM_PICTOGRAPH_BOX,
    /* 0x44 */ ITEM_FISHING_ROD,
    /* 0x45 */ MESSAGE_ITEM_NONE,
    /* 0x46 */ MESSAGE_ITEM_NONE,
    /* 0x47 */ MESSAGE_ITEM_NONE,
    /* 0x48 */ MESSAGE_ITEM_NONE,
    /* 0x49 */ MESSAGE_ITEM_NONE,
    /* 0x4A */ MESSAGE_ITEM_NONE,
    /* 0x4B */ MESSAGE_ITEM_NONE,
    /* 0x4C */ ITEM_OCARINA_OF_TIME,
    /* 0x4D */ MESSAGE_ITEM_NONE,
    /* 0x4E */ MESSAGE_ITEM_NONE,
    /* 0x4F */ MESSAGE_ITEM_NONE,
    /* 0x50 */ ITEM_BOMBERS_NOTEBOOK,
    /* 0x51 */ MESSAGE_ITEM_NONE,
    /* 0x52 */ ITEM_SKULL_TOKEN,
    /* 0x53 */ MESSAGE_ITEM_NONE,
    /* 0x54 */ MESSAGE_ITEM_NONE,
    /* 0x55 */ ITEM_REMAINS_ODOLWA,
    /* 0x56 */ ITEM_REMAINS_GOHT,
    /* 0x57 */ ITEM_REMAINS_GYORG,
    /* 0x58 */ ITEM_REMAINS_TWINMOLD,
    /* 0x59 */ ITEM_POTION_RED,
    /* 0x5A */ ITEM_BOTTLE,
    /* 0x5B */ ITEM_POTION_RED,
    /* 0x5C */ ITEM_POTION_GREEN,
    /* 0x5D */ ITEM_POTION_BLUE,
    /* 0x5E */ ITEM_FAIRY,
    /* 0x5F */ ITEM_DEKU_PRINCESS,
    /* 0x60 */ ITEM_MILK_BOTTLE,
    /* 0x61 */ ITEM_MILK_HALF,
    /* 0x62 */ ITEM_FISH,
    /* 0x63 */ ITEM_BUG,
    /* 0x64 */ ITEM_BLUE_FIRE,
    /* 0x65 */ ITEM_POE,
    /* 0x66 */ ITEM_BIG_POE,
    /* 0x67 */ ITEM_SPRING_WATER,
    /* 0x68 */ ITEM_HOT_SPRING_WATER,
    /* 0x69 */ ITEM_ZORA_EGG,
    /* 0x6A */ ITEM_GOLD_DUST,
    /* 0x6B */ ITEM_MUSHROOM,
    /* 0x6C */ MESSAGE_ITEM_NONE,
    /* 0x6D */ MESSAGE_ITEM_NONE,
    /* 0x6E */ ITEM_SEAHORSE,
    /* 0x6F */ ITEM_CHATEAU,
    /* 0x70 */ ITEM_HYLIAN_LOACH,
    /* 0x71 */ MESSAGE_ITEM_NONE,
    /* 0x72 */ MESSAGE_ITEM_NONE,
    /* 0x73 */ MESSAGE_ITEM_NONE,
    /* 0x74 */ MESSAGE_ITEM_NONE,
    /* 0x75 */ MESSAGE_ITEM_NONE,
    /* 0x76 */ MESSAGE_ITEM_NONE,
    /* 0x77 */ MESSAGE_ITEM_NONE,
    /* 0x78 */ ITEM_MASK_DEKU,
    /* 0x79 */ ITEM_MASK_GORON,
    /* 0x7A */ ITEM_MASK_ZORA,
    /* 0x7B */ ITEM_MASK_FIERCE_DEITY,
    /* 0x7C */ ITEM_MASK_TRUTH,
    /* 0x7D */ ITEM_MASK_KAFEIS_MASK,
    /* 0x7E */ ITEM_MASK_ALL_NIGHT,
    /* 0x7F */ ITEM_MASK_BUNNY,
    /* 0x80 */ ITEM_MASK_KEATON,
    /* 0x81 */ ITEM_MASK_GARO,
    /* 0x82 */ ITEM_MASK_ROMANI,
    /* 0x83 */ ITEM_MASK_CIRCUS_LEADER,
    /* 0x84 */ ITEM_MASK_POSTMAN,
    /* 0x85 */ ITEM_MASK_COUPLE,
    /* 0x86 */ ITEM_MASK_GREAT_FAIRY,
    /* 0x87 */ ITEM_MASK_GIBDO,
    /* 0x88 */ ITEM_MASK_DON_GERO,
    /* 0x89 */ ITEM_MASK_KAMARO,
    /* 0x8A */ ITEM_MASK_CAPTAIN,
    /* 0x8B */ ITEM_MASK_STONE,
    /* 0x8C */ ITEM_MASK_BREMEN,
    /* 0x8D */ ITEM_MASK_BLAST,
    /* 0x8E */ ITEM_MASK_SCENTS,
    /* 0x8F */ ITEM_MASK_GIANT,
    /* 0x90 */ MESSAGE_ITEM_NONE,
    /* 0x91 */ ITEM_CHATEAU,
    /* 0x92 */ ITEM_MILK_BOTTLE,
    /* 0x93 */ ITEM_GOLD_DUST,
    /* 0x94 */ ITEM_HYLIAN_LOACH,
    /* 0x95 */ ITEM_SEAHORSE,
    /* 0x96 */ ITEM_MOONS_TEAR,
    /* 0x97 */ ITEM_DEED_LAND,
    /* 0x98 */ ITEM_DEED_SWAMP,
    /* 0x99 */ ITEM_DEED_MOUNTAIN,
    /* 0x9A */ ITEM_DEED_OCEAN,
    /* 0x9B */ MESSAGE_ITEM_NONE,
    /* 0x9C */ MESSAGE_ITEM_NONE,
    /* 0x9D */ MESSAGE_ITEM_NONE,
    /* 0x9E */ MESSAGE_ITEM_NONE,
    /* 0x9F */ MESSAGE_ITEM_NONE,
    /* 0xA0 */ ITEM_ROOM_KEY,
    /* 0xA1 */ ITEM_LETTER_MAMA,
    /* 0xA2 */ MESSAGE_ITEM_NONE,
    /* 0xA3 */ MESSAGE_ITEM_NONE,
    /* 0xA4 */ MESSAGE_ITEM_NONE,
    /* 0xA5 */ MESSAGE_ITEM_NONE,
    /* 0xA6 */ MESSAGE_ITEM_NONE,
    /* 0xA7 */ MESSAGE_ITEM_NONE,
    /* 0xA8 */ MESSAGE_ITEM_NONE,
    /* 0xA9 */ MESSAGE_ITEM_NONE,
    /* 0xAA */ ITEM_LETTER_TO_KAFEI,
    /* 0xAB */ ITEM_PENDANT_OF_MEMORIES,
    /* 0xAC */ MESSAGE_ITEM_NONE,
    /* 0xAD */ MESSAGE_ITEM_NONE,
    /* 0xAE */ MESSAGE_ITEM_NONE,
    /* 0xAF */ MESSAGE_ITEM_NONE,
    /* 0xB0 */ MESSAGE_ITEM_NONE,
    /* 0xB1 */ MESSAGE_ITEM_NONE,
    /* 0xB2 */ MESSAGE_ITEM_NONE,
    /* 0xB3 */ ITEM_TINGLE_MAP,
    /* 0xB4 */ ITEM_TINGLE_MAP,
    /* 0xB5 */ ITEM_TINGLE_MAP,
    /* 0xB6 */ ITEM_TINGLE_MAP,
    /* 0xB7 */ ITEM_TINGLE_MAP,
    /* 0xB8 */ ITEM_TINGLE_MAP,
    /* 0xB9 */ ITEM_TINGLE_MAP,
    /* 0xBA */ MESSAGE_ITEM_NONE,
    /* 0xBB */ MESSAGE_ITEM_NONE,
    /* 0xBC */ MESSAGE_ITEM_NONE,
    /* 0xBD */ MESSAGE_ITEM_NONE,
    /* 0xBE */ MESSAGE_ITEM_NONE,
    /* 0xBF */ MESSAGE_ITEM_NONE,
    /* 0xC0 */ MESSAGE_ITEM_NONE,
    /* 0xC1 */ MESSAGE_ITEM_NONE,
    /* 0xC2 */ MESSAGE_ITEM_NONE,
    /* 0xC3 */ MESSAGE_ITEM_NONE,
    /* 0xC4 */ MESSAGE_ITEM_NONE,
    /* 0xC5 */ MESSAGE_ITEM_NONE,
    /* 0xC6 */ MESSAGE_ITEM_NONE,
    /* 0xC7 */ MESSAGE_ITEM_NONE,
    /* 0xC8 */ ITEM_SONG_SONATA,
    /* 0xC9 */ ITEM_SONG_SONATA,
    /* 0xCA */ ITEM_SONG_SONATA,
    /* 0xCB */ ITEM_SONG_LULLABY,
    /* 0xCC */ ITEM_SONG_NOVA,
    /* 0xCD */ ITEM_SONG_ELEGY,
    /* 0xCE */ ITEM_SONG_OATH,
    /* 0xCF */ ITEM_SONG_SARIA,
    /* 0xD0 */ ITEM_SONG_TIME,
    /* 0xD1 */ ITEM_SONG_HEALING,
    /* 0xD2 */ ITEM_SONG_EPONA,
    /* 0xD3 */ ITEM_SONG_SOARING,
    /* 0xD4 */ ITEM_SONG_STORMS,
    /* 0xD5 */ ITEM_SONG_SUN,
    /* 0xD6 */ ITEM_SONG_LULLABY,
    /* 0xD7 */ ITEM_SONG_SONATA,
    /* 0xD8 */ ITEM_SONG_SONATA,
    /* 0xD9 */ ITEM_SONG_SONATA,
    /* 0xDA */ ITEM_SONG_LULLABY,
    /* 0xDB */ ITEM_SONG_NOVA,
    /* 0xDC */ ITEM_B8,
    /* 0xDD */ ITEM_B9,
    /* 0xDE */ ITEM_BA,
    /* 0xDF */ ITEM_BB,
    /* 0xE0 */ ITEM_BC,
    /* 0xE1 */ ITEM_BD,
    /* 0xE2 */ ITEM_BE,
    /* 0xE3 */ ITEM_BF,
    /* 0xE4 */ ITEM_C0,
    /* 0xE5 */ ITEM_C1,
    /* 0xE6 */ ITEM_C2,
    /* 0xE7 */ ITEM_C3,
    /* 0xE8 */ ITEM_C4,
    /* 0xE9 */ ITEM_C5,
    /* 0xEA */ ITEM_C6,
    /* 0xEB */ ITEM_C7,
    /* 0xEC */ ITEM_C8,
    /* 0xED */ ITEM_C9,
    /* 0xEE */ ITEM_CA,
    /* 0xEF */ ITEM_CB,
    /* 0xF0 */ ITEM_CC,
    /* 0xF1 */ MESSAGE_ITEM_NONE,
    /* 0xF2 */ MESSAGE_ITEM_NONE,
    /* 0xF3 */ MESSAGE_ITEM_NONE,
    /* 0xF4 */ MESSAGE_ITEM_NONE,
    /* 0xF5 */ MESSAGE_ITEM_NONE,
    /* 0xF6 */ MESSAGE_ITEM_NONE,
    /* 0xF7 */ MESSAGE_ITEM_NONE,
    /* 0xF8 */ MESSAGE_ITEM_NONE,
    /* 0xF9 */ MESSAGE_ITEM_NONE,
};

void Message_LoadItemIcon(PlayState* play, u16 itemId, s16 arg2) {
    MessageContext* msgCtx = &play->msgCtx;
    u16* new_var2 = &itemId;

    if (itemId == ITEM_RECOVERY_HEART) {
        msgCtx->unk12010 = (msgCtx->unk11FF8 - D_801CFF88[gSaveContext.options.language]);
        msgCtx->unk12012 = (arg2 + 0xA);
        msgCtx->unk12014 = 0x10;
    } else if ((itemId >= ITEM_RUPEE_GREEN) && (itemId <= ITEM_RUPEE_HUGE)) {
        msgCtx->unk12010 = (msgCtx->unk11FF8 - D_801CFF88[gSaveContext.options.language]);
        msgCtx->unk12012 = (arg2 + 0xA);
        msgCtx->unk12014 = 0x10;
    } else if (itemId == ITEM_STRAY_FAIRIES) {
        msgCtx->unk12010 = (msgCtx->unk11FF8 - D_801CFF70[gSaveContext.options.language]);
        msgCtx->unk12012 = (arg2 + 0xA);
        msgCtx->unk12014 = 0x20;
    } else if ((itemId >= ITEM_SONG_SONATA) && (itemId <= ITEM_SONG_SUN)) {
        msgCtx->unk12010 = (msgCtx->unk11FF8 - D_801CFF88[gSaveContext.options.language]);
        msgCtx->unk12012 = (arg2 + 0xA);
        msgCtx->unk12014 = 0x10;
        // #region 2S2H [Port]
        // CmpDma_LoadFile(SEGMENT_ROM_START(icon_item_static_yar), ITEM_SONG_SONATA, msgCtx->textboxSegment + 0x1000,
        //                 0x180);
        msgCtx->textboxSegment[TEXTBOX_SEG_ICON] = gStaticItemIcons[ITEM_SONG_SONATA];
    } else if (itemId == ITEM_BOMBERS_NOTEBOOK) {
        msgCtx->unk12010 = (msgCtx->unk11FF8 - D_801CFF70[gSaveContext.options.language]);
        msgCtx->unk12012 = (arg2 + 6);
        msgCtx->unk12014 = 0x20;
        // CmpDma_LoadFile(SEGMENT_ROM_START(icon_item_static_yar), ITEM_SONG_SONATA, msgCtx->textboxSegment + 0x1000,
        //                 0x1000);
        msgCtx->textboxSegment[TEXTBOX_SEG_ICON] = gStaticItemIcons[ITEM_SONG_SONATA];
    } else if (itemId <= ITEM_REMAINS_TWINMOLD) {
        msgCtx->unk12010 = (msgCtx->unk11FF8 - D_801CFF70[gSaveContext.options.language]);
        msgCtx->unk12012 = (arg2 + 6);
        msgCtx->unk12014 = 0x20;
        // CmpDma_LoadFile(SEGMENT_ROM_START(icon_item_static_yar), itemId, msgCtx->textboxSegment + 0x1000, 0x1000);
        msgCtx->textboxSegment[TEXTBOX_SEG_ICON] = gStaticItemIcons[itemId];
    } else if (itemId == ITEM_CC) {
        msgCtx->unk12010 = (msgCtx->unk11FF8 - D_801CFF70[gSaveContext.options.language]);
        msgCtx->unk12012 = (arg2 + 8);
        msgCtx->unk12014 = 0x20;
        // CmpDma_LoadFile(SEGMENT_ROM_START(schedule_dma_static_yar), ITEM_POTION_BLUE, msgCtx->textboxSegment +
        // 0x1000,
        //                 0x400);
        msgCtx->textboxSegment[TEXTBOX_SEG_ICON] = gBombersNotebookPhotos[ITEM_POTION_BLUE];
    } else if (itemId >= ITEM_B8) {
        msgCtx->unk12010 = (msgCtx->unk11FF8 - D_801CFF70[gSaveContext.options.language]);
        msgCtx->unk12012 = (arg2 + 8);
        msgCtx->unk12014 = 0x20;
        // CmpDma_LoadFile(SEGMENT_ROM_START(schedule_dma_static_yar), (itemId - ITEM_B8), msgCtx->textboxSegment +
        // 0x1000,
        //                 0x800);
        msgCtx->textboxSegment[TEXTBOX_SEG_ICON] = gBombersNotebookPhotos[itemId - ITEM_B8];
    } else if (itemId >= ITEM_SKULL_TOKEN) {
        msgCtx->unk12010 = (msgCtx->unk11FF8 - D_801CFF7C[gSaveContext.options.language]);
        msgCtx->unk12012 = (arg2 + 0xA);
        msgCtx->unk12014 = 0x18;
        // CmpDma_LoadFile(SEGMENT_ROM_START(icon_item_24_static_yar), (itemId - ITEM_SKULL_TOKEN),
        //                 msgCtx->textboxSegment + 0x1000, 0x900);
        msgCtx->textboxSegment[TEXTBOX_SEG_ICON] = gQuestIcons[itemId - ITEM_SKULL_TOKEN];
        // #endregion
    }

    if (play->pauseCtx.bombersNotebookOpen) {
        msgCtx->unk12010 = ((msgCtx->unk12010 * 1.4f) + 2.0f);
        msgCtx->unk12014 = (msgCtx->unk12014 * 1.4f);
    }

    msgCtx->choiceNum = 1;
}

void Message_LoadChar(PlayState* play, u16 codePointIndex, s32* offset, f32* arg3, s16 decodedBufPos) {
    MessageContext* msgCtx = &play->msgCtx;
    s32 temp1 = *offset;
    f32 temp2 = *arg3;

    Font_LoadChar(play, codePointIndex, temp1);
    msgCtx->decodedBuffer.wchar[decodedBufPos] = codePointIndex;
    temp1 += FONT_CHAR_TEX_SIZE;
    temp2 += (16.0f * msgCtx->textCharScale);
    *offset = temp1;
    *arg3 = temp2;
}

// TODO: SJIS support
// Message_LoadRupees JPN ?
void func_8014CCB4(PlayState* play, s16* decodedBufPos, s32* offset, f32* arg3) {
    MessageContext* msgCtx = &play->msgCtx;
    s16 t = *decodedBufPos;
    s32 k = *offset;
    f32 f = *arg3;

    Font_LoadChar(play, 0x838B, k); // 0x838B = ル in JISX0213
    k += FONT_CHAR_TEX_SIZE;
    msgCtx->decodedBuffer.wchar[t] = 0x838B;
    t++;
    Font_LoadChar(play, 0x8373, k); // 0x8373 = ピ in JISX0213
    k += FONT_CHAR_TEX_SIZE;
    msgCtx->decodedBuffer.wchar[t] = 0x8373;
    t++;
    Font_LoadChar(play, 0x815C, k); // 0x815C = ― in JISX0213
    k += FONT_CHAR_TEX_SIZE;
    msgCtx->decodedBuffer.wchar[t] = 0x815C;

    f += 16.0f * msgCtx->textCharScale * 3.0f;
    *decodedBufPos = t;
    *offset = k;
    *arg3 = f;
}

/*
 * offsetting to actual codepoints is done outside this function
 * every digit will be added 0x824F to get an actual S-JIS
 * printable character.
 */
void Message_GetTimerDigits(OSTime time, s16 digits[8]) {
    OSTime t = time;

    // 6 minutes
    digits[0] = t / SECONDS_TO_TIMER(360);
    t -= digits[0] * SECONDS_TO_TIMER(360);

    // minutes
    digits[1] = t / SECONDS_TO_TIMER(60);
    t -= digits[1] * SECONDS_TO_TIMER(60);

    digits[2] = 0x135B; // 0x135B + 0x824F = 分 (minutes) in S-JIS

    // 10 seconds
    digits[3] = t / SECONDS_TO_TIMER(10);
    t -= digits[3] * SECONDS_TO_TIMER(10);

    // seconds
    digits[4] = t / SECONDS_TO_TIMER(1);
    t -= digits[4] * SECONDS_TO_TIMER(1);

    digits[5] = 0x1313; // 0x1313 + 0x824F = 秒 (seconds) in S-JIS

    // 100 milliseconds
    digits[6] = t / SECONDS_TO_TIMER_PRECISE(0, 10);
    t -= digits[6] * SECONDS_TO_TIMER_PRECISE(0, 10);

    // 10 milliseconds
    digits[7] = t;
}

void Message_DecodeHeader(PlayState* play) {
    Font* font;
    MessageContext* msgCtx = &play->msgCtx;

    font = &msgCtx->font;
    if (msgCtx->msgBufPos == 0) {
        if (((u8)font->msgBuf.schar[msgCtx->msgBufPos + 2]) != 0xFE) {
            msgCtx->unk11F18 = 0;
            if ((msgCtx->currentTextId == 0x176F) || (msgCtx->currentTextId == 0x1770) ||
                (msgCtx->currentTextId == 0x1771)) {
                msgCtx->itemId = ITEM_OCARINA_OF_TIME;
                msgCtx->msgBufPos += 2;
            } else {
                msgCtx->msgBufPos += 2;
                if ((((u8)font->msgBuf.schar[msgCtx->msgBufPos]) < 0xC8) ||
                    (((u8)font->msgBuf.schar[msgCtx->msgBufPos]) >= 0xD8)) {
                    msgCtx->itemId = D_801CFF94[(u8)font->msgBuf.schar[msgCtx->msgBufPos]];
                } else {
                    msgCtx->itemId = 0xFE;
                }
            }
        } else {
            msgCtx->msgBufPos += 2;
            msgCtx->itemId = (u8)font->msgBuf.schar[msgCtx->msgBufPos];
        }
        msgCtx->nextTextId = font->msgBuf.schar[++msgCtx->msgBufPos] << 8;
        msgCtx->nextTextId += font->msgBuf.schar[++msgCtx->msgBufPos];

        msgCtx->unk1206C = (u8)(font->msgBuf.schar[++msgCtx->msgBufPos]) << 8;
        msgCtx->unk1206C |= font->msgBuf.schar[++msgCtx->msgBufPos];

        msgCtx->unk12070 = (u8)(font->msgBuf.schar[++msgCtx->msgBufPos]) << 8;
        msgCtx->unk12070 |= font->msgBuf.schar[++msgCtx->msgBufPos];

        msgCtx->unk12074 = (u8)(font->msgBuf.schar[++msgCtx->msgBufPos]) << 8;
        msgCtx->unk12074 |= font->msgBuf.schar[++msgCtx->msgBufPos];

        msgCtx->msgBufPos++;
        if (msgCtx->itemId != 0xFE) {
            Message_LoadItemIcon(play, msgCtx->itemId, msgCtx->textboxY + 10);
        }
    }
}

void Message_LoadTime(PlayState* play, u16 curChar, s32* offset, f32* arg3, s16* decodedBufPos) {
    MessageContext* msgCtx = &play->msgCtx;
    s16 i;
    s16 p = *decodedBufPos;
    s32 o = *offset;
    f32 f = *arg3;
    u32 dayTime;
    s16 digits[4];
    f32 timeInMinutes;

    if (curChar == 0x20F) {
        if (GameInteractor_Should(VB_TIME_UNTIL_MOON_CRASH_CALCULATION, true, &dayTime)) {
            dayTime = TIME_UNTIL_MOON_CRASH;
        }
    } else {
        dayTime = TIME_UNTIL_NEW_DAY;
    }
    timeInMinutes = TIME_TO_MINUTES_F(dayTime);

    digits[0] = 0;
    digits[1] = (timeInMinutes / 60.0f);
    while (digits[1] >= 10) {
        digits[0]++;
        digits[1] -= 10;
    }

    digits[2] = 0;
    digits[3] = (s32)timeInMinutes % 60;
    while (digits[3] >= 10) {
        digits[2]++;
        digits[3] -= 10;
    }

    for (i = 0; i < 4; i++) {
        Font_LoadChar(play, digits[i] + 0x824F, o); // 0x824F = '0' in S-JIS
        o += FONT_CHAR_TEX_SIZE;
        msgCtx->decodedBuffer.wchar[p] = digits[i] + 0x824F;
        p++;
        if (i == 1) {
            // Hours (時間)
            Font_LoadChar(play, 0x8E9E, o);
            o += FONT_CHAR_TEX_SIZE;
            msgCtx->decodedBuffer.wchar[p] = 0x8E9E;
            p++;
            Font_LoadChar(play, 0x8AD4, o);
            o += FONT_CHAR_TEX_SIZE;
            msgCtx->decodedBuffer.wchar[p] = 0x8E9E;
            p++;
        } else if (i == 3) {
            // Minutes (分)
            Font_LoadChar(play, 0x95AA, o);
            o += FONT_CHAR_TEX_SIZE;
            msgCtx->decodedBuffer.wchar[p] = 0x95AA;
        }
    }

    f += 7.0f * (16.0f * msgCtx->textCharScale);
    *decodedBufPos = p;
    *offset = o;
    *arg3 = f;
}

#define GREAT_BAY_COAST_STR \
    { 0x834F, 0x838C, 0x815B, 0x8367, 0x8378, 0x8343, 0x82CC, 0x8A43, 0x8ADD }
#define ZORA_CAPE_STR \
    { 0x835D, 0x815B, 0x8389, 0x82CC, 0x82DD, 0x82B3, 0x82AB }
#define SNOWHEAD_STR \
    { 0x8358, 0x836D, 0x815B, 0x8377, 0x8362, 0x8368 }
#define MOUNTAIN_VILLAGE_STR \
    { 0x8E52, 0x97A2 }
#define CLOCK_TOWN_STR \
    { 0x834E, 0x838D, 0x8362, 0x834E, 0x835E, 0x8345, 0x8393 }
#define MILK_ROAD_STR \
    { 0x837E, 0x838B, 0x834E, 0x838D, 0x815B, 0x8368 }
#define WOODFALL_STR \
    { 0x8345, 0x8362, 0x8368, 0x8374, 0x8348, 0x815B, 0x838B }
#define SOUTHERN_SWAMP_STR \
    { 0x8FC0, 0x926E }
#define IKANA_CANYON_STR \
    { 0x8343, 0x834A, 0x815B, 0x8369, 0x8C6B, 0x924A }
#define STONE_TOWER_STR \
    { 0x838D, 0x8362, 0x834E, 0x8372, 0x838B }
#define ENTRANCE_STR \
    { 0x93FC, 0x82E8, 0x8CFB }

// Shift JIS
u16 sOwlWarpTextJPN[OWL_WARP_MAX][9] = {
    GREAT_BAY_COAST_STR,  // OWL_WARP_GREAT_BAY_COAST
    ZORA_CAPE_STR,        // OWL_WARP_ZORA_CAPE
    SNOWHEAD_STR,         // OWL_WARP_SNOWHEAD
    MOUNTAIN_VILLAGE_STR, // OWL_WARP_MOUNTAIN_VILLAGE
    CLOCK_TOWN_STR,       // OWL_WARP_CLOCK_TOWN
    MILK_ROAD_STR,        // OWL_WARP_MILK_ROAD
    WOODFALL_STR,         // OWL_WARP_WOODFALL
    SOUTHERN_SWAMP_STR,   // OWL_WARP_SOUTHERN_SWAMP
    IKANA_CANYON_STR,     // OWL_WARP_IKANA_CANYON
    STONE_TOWER_STR,      // OWL_WARP_STONE_TOWER
    ENTRANCE_STR,         // OWL_WARP_ENTRANCE
};

//! TODO: use sizeof when we have strings
s16 sOwlWarpTextLengthJPN[OWL_WARP_MAX] = {
    9, // OWL_WARP_GREAT_BAY_COAST
    7, // OWL_WARP_ZORA_CAPE
    6, // OWL_WARP_SNOWHEAD
    2, // OWL_WARP_MOUNTAIN_VILLAGE
    7, // OWL_WARP_CLOCK_TOWN
    6, // OWL_WARP_MILK_ROAD
    7, // OWL_WARP_WOODFALL
    2, // OWL_WARP_SOUTHERN_SWAMP
    6, // OWL_WARP_IKANA_CANYON
    5, // OWL_WARP_STONE_TOWER
    3, // OWL_WARP_ENTRANCE
};

void Message_LoadOwlWarpText(PlayState* play, s32* offset, f32* arg2, s16* decodedBufPos) {
    MessageContext* msgCtx = &play->msgCtx;
    PauseContext* pauseCtx = &play->pauseCtx;
    s16 stringLimit;
    s16 curDecodedBufPos = *decodedBufPos;
    s32 temp_s2 = *offset;
    f32 sp3C = *arg2;
    s16 owlWarpId;
    s16 i;

    if (Map_CurRoomHasMapI(play) || (play->sceneId == SCENE_SECOM)) {
        owlWarpId = OWL_WARP_ENTRANCE;
    } else {
        owlWarpId = pauseCtx->cursorPoint[PAUSE_WORLD_MAP];
    }
    stringLimit = sOwlWarpTextLengthJPN[owlWarpId];

    for (i = 0; i < stringLimit; i++, curDecodedBufPos++, temp_s2 += FONT_CHAR_TEX_SIZE) {
        msgCtx->decodedBuffer.wchar[curDecodedBufPos] = sOwlWarpTextJPN[owlWarpId][i];
        Font_LoadChar(play, sOwlWarpTextJPN[owlWarpId][i], temp_s2);
    }

    curDecodedBufPos--;
    sp3C += (stringLimit - 1) * (16.0f * msgCtx->textCharScale);

    *decodedBufPos = curDecodedBufPos;
    *offset = temp_s2;
    *arg2 = sp3C;
}

u16 sTimeSpeedTextJPN[][3] = {
    { 0x82CD, 0x82E2, 0x82A2 },
    { 0x82D3, 0x82C2, 0x82A4 },
    { 0x82A8, 0x82BB, 0x82A2 },
};

u16 sMaskCodeColorCmdJPN[] = { 0x2001, 0x2003, 0x2004, 0x2002 };

// Counterpart to NES sMaskCodeTextENG
u16 D_801D0284[] = { 0x90D4, 0x90C2, 0x89A9, 0x97CE };

void Message_Decode(PlayState* play) {
    MessageContext* msgCtx = &play->msgCtx;
    Font* font = &msgCtx->font;
    Player* player = GET_PLAYER(play);
    s16 decodedBufPos;
    s16 loadChar;
    u32 timeToMoonCrash;
    s16 var_v0;
    s16 numLines;
    s16 value;
    s16 digits[5];
    s16 spD2;
    f32 timeInSeconds;
    s32 charTexIndex;
    u8* fontBuf;
    f32 spC0;
    s16 index;
    s16 playerNameLen;
    s16 spAC[8];
    f32 var_fs0;
    s16 i;
    u16 curChar;
    u8 index2 = 0;

    // BENTODO do this somewhere else
    gSaveContext.options.language = LANGUAGE_ENG;

    msgCtx->textDelayTimer = 0;
    msgCtx->textDelay = msgCtx->textDelayTimer;
    msgCtx->textFade = 0;
    spC0 = 0.0f;
    font->unk_11D88 = (font->unk_11D88 ^ 1) & 1;

    if ((gSaveContext.options.language == LANGUAGE_JPN) && !msgCtx->textIsCredits) {
        spD2 = 0;
        numLines = 0;
        decodedBufPos = 0;
        charTexIndex = 0;
        Message_DecodeHeader(play);

        while (true) {
            curChar = msgCtx->decodedBuffer.wchar[decodedBufPos] = font->msgBuf.wchar[msgCtx->msgBufPos];

            if ((curChar == 9) || (curChar == 0xB) || (curChar == 0x110) || (curChar == 0x111) || (curChar == 0x112) ||
                (curChar == 0x103) || (curChar == 0x230) || (curChar == 0x500) || (curChar == 0x240) ||
                (curChar == 0x104)) {

                msgCtx->msgMode = MSGMODE_TEXT_DISPLAYING;
                msgCtx->textDrawPos = 1;
                msgCtx->unk11FFA = msgCtx->textboxY + 6;
                msgCtx->unk11F1A[spD2] = 0;
                if (msgCtx->unk11F18 == 0) {
                    msgCtx->unk11F1A[spD2] = TRUNCF_BINANG((msgCtx->textCharScale * 16.0f * 16.0f) - spC0) / 2;
                }
                spC0 = 0.0f;
                if (curChar == 0xB) {
                    if ((msgCtx->textBoxType != TEXTBOX_TYPE_3) && (msgCtx->textBoxType != TEXTBOX_TYPE_4)) {
                        if (numLines < 2) {
                            msgCtx->unk11FFA = msgCtx->textboxY + 0x16;
                        } else if (numLines == 2) {
                            msgCtx->unk11FFA = msgCtx->textboxY + 0xE;
                        }
                    }
                } else {
                    s8 requiredScopeTemp;

                    if ((msgCtx->textBoxType != TEXTBOX_TYPE_3) && (msgCtx->textBoxType != TEXTBOX_TYPE_4)) {
                        if (numLines == 0) {
                            msgCtx->unk11FFA = msgCtx->textboxY + 0x16;
                        } else if (numLines == 1) {
                            msgCtx->unk11FFA = msgCtx->textboxY + 0xE;
                        }
                    }
                }

                if ((curChar == 0x110) || (curChar == 0x111) || (curChar == 0x112)) {
                    msgCtx->decodedBuffer.wchar[++decodedBufPos] = font->msgBuf.wchar[msgCtx->msgBufPos + 1];
                    msgCtx->msgBufPos += 2;
                }

                msgCtx->decodedTextLen = decodedBufPos;
                if (msgCtx->textboxSkipped || (msgCtx->textBoxType == TEXTBOX_TYPE_1) ||
                    (msgCtx->textBoxType == TEXTBOX_TYPE_3) || (msgCtx->textBoxType == TEXTBOX_TYPE_6) ||
                    (msgCtx->textBoxType == TEXTBOX_TYPE_8) || (msgCtx->textBoxType == TEXTBOX_TYPE_9) ||
                    (msgCtx->textBoxType == TEXTBOX_TYPE_B) || (msgCtx->unk11F0C == 3)) {
                    msgCtx->textDrawPos = msgCtx->decodedTextLen;
                }

                msgCtx->unk120C8 = msgCtx->unk120CE;
                msgCtx->unk120CA = msgCtx->unk120D0;
                msgCtx->unk120CC = msgCtx->unk120D2;
                break;
            }

            if (curChar == 0x100) {
                for (playerNameLen = ARRAY_COUNT(gSaveContext.save.saveInfo.playerData.playerName); playerNameLen > 0;
                     playerNameLen--) {
                    if (((void)0, gSaveContext.save.saveInfo.playerData.playerName[playerNameLen - 1]) != 0x3E) {
                        break;
                    }
                }

                for (i = 0; i < playerNameLen; i++) {
                    curChar = gSaveContext.save.saveInfo.playerData.playerName[i];
                    index2 = curChar & 0xFFFF;
                    fontBuf = &font->fontBuf[index2 * FONT_CHAR_TEX_SIZE];
                    msgCtx->decodedBuffer.wchar[decodedBufPos + i] = 0x100;

                    for (var_v0 = 0; var_v0 < FONT_CHAR_TEX_SIZE; var_v0 += 4) {
                        font->charBuf[font->unk_11D88][charTexIndex + var_v0 + 0] = fontBuf[var_v0 + 0];
                        font->charBuf[font->unk_11D88][charTexIndex + var_v0 + 1] = fontBuf[var_v0 + 1];
                        font->charBuf[font->unk_11D88][charTexIndex + var_v0 + 2] = fontBuf[var_v0 + 2];
                        font->charBuf[font->unk_11D88][charTexIndex + var_v0 + 3] = fontBuf[var_v0 + 3];
                    }
                    charTexIndex += FONT_CHAR_TEX_SIZE;
                }
                //! FAKE:
                fontBuf = &font->fontBuf[index2 * FONT_CHAR_TEX_SIZE];

                decodedBufPos += playerNameLen - 1;
                spC0 += playerNameLen * (16.0f * msgCtx->textCharScale);
            } else if (curChar == 0x201) {
                // #region 2S2H [Port]
                // DmaMgr_SendRequest0(msgCtx->textboxSegment + 0x1000, SEGMENT_ROM_START(message_texture_static),
                // 0x900); DmaMgr_SendRequest0(msgCtx->textboxSegment + 0x1900,
                // SEGMENT_ROM_START(message_texture_static) + 0x900,
                //                   0x900);
                msgCtx->textboxSegment[TEXTBOX_SEG_BG_1] = gMessageXLeftTex;
                msgCtx->textboxSegment[TEXTBOX_SEG_BG_2] = gMessageXRightTex;
                // #endregion
                numLines = 2;
                spD2 = 2;
                msgCtx->unk12012 = msgCtx->textboxY + 8;
                msgCtx->unk11F18 = 1;
                msgCtx->unk12010 = XREG(47);
            } else if (curChar == 0x202) {
                msgCtx->unk11F18 = 1;
                msgCtx->choiceNum = 2;
            } else if (curChar == 0x203) {
                msgCtx->unk11F18 = 1;
                msgCtx->choiceNum = 3;
                msgCtx->unk11FF8 += 0x16;
            } else if (curChar == 0x204) {
                Message_GetTimerDigits(((void)0, gSaveContext.timerCurTimes[curChar - 0x204]), spAC);

                loadChar = false;
                for (i = 0; i < ARRAY_COUNT(spAC) - 3; i++) {
                    if ((i == 1) || (spAC[i + 3] != 0)) {
                        loadChar = true;
                    }
                    if (loadChar) {
                        Message_LoadChar(play, spAC[i + 3] + 0x824F, &charTexIndex, &spC0, decodedBufPos);
                        decodedBufPos++;
                    }
                }
                decodedBufPos--;
            } else if ((curChar == 0x205) || (curChar == 0x206) || (curChar == 0x207) || (curChar == 0x208) ||
                       (curChar == 0x209)) {
                Message_GetTimerDigits(((void)0, gSaveContext.timerCurTimes[curChar - 0x204]), spAC);

                loadChar = false;
                for (i = 0; i < ARRAY_COUNT(spAC); i++) {
                    if ((i == 4) || ((i != 2) && (i != 5) && (spAC[i] != '\0'))) {
                        loadChar = true;
                    }
                    if (loadChar) {
                        Message_LoadChar(play, spAC[i] + 0x824F, &charTexIndex, &spC0, decodedBufPos);
                        decodedBufPos++;
                    }
                }
                decodedBufPos--;
            } else if (curChar == 0x20A) {
                digits[0] = 0;
                timeInSeconds = TIME_TO_MINUTES_F(CURRENT_TIME);
                digits[1] = timeInSeconds / 60.0f;
                while (digits[1] >= 10) {
                    digits[0]++;
                    digits[1] -= 10;
                }

                digits[2] = 0;
                digits[3] = (s32)timeInSeconds % 60;
                while (digits[3] >= 10) {
                    digits[2]++;
                    digits[3] -= 10;
                }

                for (i = 0; i < 4; i++) {
                    Font_LoadChar(play, digits[i] + 0x824F, charTexIndex);
                    charTexIndex += FONT_CHAR_TEX_SIZE;
                    msgCtx->decodedBuffer.wchar[decodedBufPos] = digits[i] + 0x824F;
                    decodedBufPos++;
                    if (i == 1) {
                        Font_LoadChar(play, 0x8E9E, charTexIndex);
                        charTexIndex += FONT_CHAR_TEX_SIZE;
                        msgCtx->decodedBuffer.wchar[decodedBufPos] = 0x8E9E;
                        decodedBufPos++;
                    } else if (i == 3) {
                        Font_LoadChar(play, 0x95AA, charTexIndex);
                        charTexIndex += FONT_CHAR_TEX_SIZE;
                        msgCtx->decodedBuffer.wchar[decodedBufPos] = 0x95AA;
                    }
                }
                spC0 += 6 * (16.0f * msgCtx->textCharScale);
            } else if (curChar == 0x20B) {
                digits[0] = digits[1] = digits[2] = 0;
                digits[3] = Flags_GetAllTreasure(play);

                while (digits[3] >= 1000) {
                    digits[0]++;
                    digits[3] -= 1000;
                }
                while (digits[3] >= 100) {
                    digits[1]++;
                    digits[3] -= 100;
                }
                while (digits[3] >= 10) {
                    digits[2]++;
                    digits[3] -= 10;
                }

                loadChar = false;
                for (i = 0; i < 4; i++) {
                    if ((i == 3) || (digits[i] != 0)) {
                        loadChar = true;
                    }
                    if (loadChar) {
                        Font_LoadChar(play, digits[i] + 0x824F, charTexIndex);
                        charTexIndex += FONT_CHAR_TEX_SIZE;
                        msgCtx->decodedBuffer.wchar[decodedBufPos] = digits[i] + 0x824F;
                        decodedBufPos++;
                        spC0 += 16.0f * msgCtx->textCharScale;
                    }
                }
                Message_LoadChar(play, 0x9543, &charTexIndex, &spC0, decodedBufPos);
            } else if (curChar == 0x20C) {
                decodedBufPos++;
                msgCtx->unk120BE = spD2;
                msgCtx->unk120C0 = decodedBufPos;
                msgCtx->unk120C2 = 2;
                msgCtx->rupeesSelected = 0;
                msgCtx->unk120C4 = charTexIndex;
                digits[0] = digits[1] = digits[2] = 0;

                for (i = 0; i < 3; i++) {
                    Message_LoadChar(play, digits[i] + 0x824F, &charTexIndex, &spC0, decodedBufPos);
                    decodedBufPos++;
                }
                func_8014CCB4(play, &decodedBufPos, &charTexIndex, &spC0);
            } else if (curChar == 0x20D) {
                digits[0] = digits[1] = 0;
                digits[2] = msgCtx->rupeesSelected;

                while (digits[2] >= 100) {
                    digits[0]++;
                    digits[2] -= 100;
                }

                while (digits[2] >= 10) {
                    digits[1]++;
                    digits[2] -= 10;
                }

                loadChar = false;
                for (i = 0; i < 3; i++) {
                    if ((i == 2) || (digits[i] != 0)) {
                        loadChar = true;
                    }
                    if (loadChar) {
                        Message_LoadChar(play, digits[i] + 0x824F, &charTexIndex, &spC0, decodedBufPos);
                        decodedBufPos++;
                    }
                }
                func_8014CCB4(play, &decodedBufPos, &charTexIndex, &spC0);
            } else if (curChar == 0x20E) {
                digits[0] = digits[1] = digits[2] = 0;
                digits[3] = msgCtx->rupeesTotal;

                while (digits[3] >= 1000) {
                    digits[0]++;
                    digits[3] -= 1000;
                }
                while (digits[3] >= 100) {
                    digits[1]++;
                    digits[3] -= 100;
                }
                while (digits[3] >= 10) {
                    digits[2]++;
                    digits[3] -= 10;
                }

                loadChar = false;
                for (i = 0; i < 4; i++) {
                    if ((i == 3) || (digits[i] != 0)) {
                        loadChar = true;
                    }
                    if (loadChar) {
                        Message_LoadChar(play, digits[i] + 0x824F, &charTexIndex, &spC0, decodedBufPos);
                        decodedBufPos++;
                    }
                }
                func_8014CCB4(play, &decodedBufPos, &charTexIndex, &spC0);
            } else if (curChar == 0x20F) {
                Message_LoadTime(play, curChar, &charTexIndex, &spC0, &decodedBufPos);
            } else if (curChar == 0x21C) {
                digits[0] = digits[1] = 0;
                digits[2] =
                    gSaveContext.save.saveInfo.inventory.strayFairies[(void)0, gSaveContext.dungeonSceneSharedIndex];

                while (digits[2] >= 100) {
                    digits[0]++;
                    digits[2] -= 100;
                }
                while (digits[2] >= 10) {
                    digits[1]++;
                    digits[2] -= 10;
                }

                loadChar = false;
                for (i = 0; i < 3; i++) {
                    if ((i == 2) || (digits[i] != 0)) {
                        loadChar = true;
                    }
                    if (loadChar) {
                        Message_LoadChar(play, digits[i] + 0x824F, &charTexIndex, &spC0, decodedBufPos);
                        decodedBufPos++;
                    }
                }
                Message_LoadChar(play, 0x906C, &charTexIndex, &spC0, decodedBufPos);
                decodedBufPos++;
                Message_LoadChar(play, 0x96DA, &charTexIndex, &spC0, decodedBufPos);
            } else if (curChar == 0x21D) {
                digits[0] = digits[1] = 0;
                digits[2] = Inventory_GetSkullTokenCount(play->sceneId);

                while (digits[2] >= 100) {
                    digits[0]++;
                    digits[2] -= 100;
                }
                while (digits[2] >= 10) {
                    digits[1]++;
                    digits[2] -= 10;
                }

                loadChar = false;
                for (i = 0; i < 3; i++) {
                    if ((i == 2) || (digits[i] != 0)) {
                        loadChar = true;
                    }
                    if (loadChar) {
                        Message_LoadChar(play, digits[i] + 0x824F, &charTexIndex, &spC0, decodedBufPos);
                        decodedBufPos++;
                    }
                }
                Message_LoadChar(play, 0x9543, &charTexIndex, &spC0, decodedBufPos);
            } else if (curChar == 0x21E) {
                digits[0] = 0;
                digits[1] = gSaveContext.minigameScore;

                while (digits[1] >= 10) {
                    digits[0]++;
                    digits[1] -= 10;
                }

                for (i = 0; i < 2; i++) {
                    if ((i == 1) || (digits[i] != 0)) {
                        Message_LoadChar(play, digits[i] + 0x824F, &charTexIndex, &spC0, decodedBufPos);
                        decodedBufPos++;
                    }
                }
                decodedBufPos--;
            } else if (curChar == 0x21F) {
                digits[0] = digits[1] = digits[2] = 0;
                digits[3] = gSaveContext.minigameScore;

                while (digits[3] >= 1000) {
                    digits[0]++;
                    digits[3] -= 1000;
                }
                while (digits[3] >= 100) {
                    digits[1]++;
                    digits[3] -= 100;
                }
                while (digits[3] >= 10) {
                    digits[2]++;
                    digits[3] -= 10;
                }

                loadChar = false;
                for (i = 0; i < 4; i++) {
                    if ((i == 3) || (digits[i] != 0)) {
                        loadChar = true;
                    }
                    if (loadChar) {
                        Message_LoadChar(play, digits[i] + 0x824F, &charTexIndex, &spC0, decodedBufPos);
                        decodedBufPos++;
                    }
                }
                decodedBufPos--;
            } else if (curChar == 0x220) {
                decodedBufPos++;
                msgCtx->unk120BE = spD2;
                msgCtx->unk120C0 = decodedBufPos;
                msgCtx->unk120C2 = 0;
                msgCtx->rupeesSelected = 0;
                msgCtx->unk120C4 = charTexIndex;
                digits[0] = digits[1] = digits[2] = 0;
                for (i = 0; i < 2; i++) {
                    Message_LoadChar(play, digits[i] + 0x824F, &charTexIndex, &spC0, decodedBufPos);
                    decodedBufPos++;
                }
                func_8014CCB4(play, &decodedBufPos, &charTexIndex, &spC0);
            } else if (curChar == 0x221) {
                decodedBufPos++;
                msgCtx->unk120BE = spD2;
                msgCtx->unk120C0 = decodedBufPos;
                msgCtx->unk120C2 = 0;
                msgCtx->rupeesSelected = 0;
                msgCtx->unk120C4 = charTexIndex;

                for (i = 0; i < 5; i++) {
                    msgCtx->unk12054[i] = 1;
                    Message_LoadChar(play, 0x8250, &charTexIndex, &spC0, decodedBufPos);
                    decodedBufPos++;
                }
                decodedBufPos--;
            } else if (curChar == 0x223) {
                if (((void)0, gSaveContext.save.timeSpeedOffset) == 0x12) {
                    index = 0;
                } else if (((void)0, gSaveContext.save.timeSpeedOffset) == 0) {
                    index = 1;
                } else {
                    index = 2;
                }

                for (i = 0; i < 3; i++, decodedBufPos++) {
                    msgCtx->decodedBuffer.wchar[decodedBufPos] = sTimeSpeedTextJPN[index][i];
                    Font_LoadChar(play, sTimeSpeedTextJPN[index][i], charTexIndex);
                    charTexIndex += FONT_CHAR_TEX_SIZE;
                }
                decodedBufPos--;
                spC0 += 3.0f * (16.0f * msgCtx->textCharScale);
                msgCtx->choiceIndex = index;
            } else if (curChar == 0x224) {
                Message_LoadOwlWarpText(play, &charTexIndex, &spC0, &decodedBufPos);
            } else if (curChar == 0x225) {
                decodedBufPos++;
                msgCtx->unk120BE = spD2;
                msgCtx->unk120C0 = decodedBufPos;
                msgCtx->unk120C2 = 0;
                msgCtx->rupeesSelected = 0;
                msgCtx->unk120C4 = charTexIndex;

                for (i = 0; i < 3; i++) {
                    msgCtx->unk12054[i] = 1;
                    Font_LoadChar(play, 0x8250, charTexIndex);
                    charTexIndex += FONT_CHAR_TEX_SIZE;
                    msgCtx->decodedBuffer.wchar[decodedBufPos] = 0x8250;
                    spC0 += 16.0f * msgCtx->textCharScale;
                    decodedBufPos++;
                }
                decodedBufPos--;
            } else if (curChar == 0x226) {
                for (i = 0; i < 6; i++) {
                    msgCtx->decodedBuffer.wchar[decodedBufPos] =
                        sMaskCodeColorCmdJPN[((void)0, gSaveContext.save.saveInfo.spiderHouseMaskOrder[i])];
                    decodedBufPos++;
                    Message_LoadChar(play, i + 0x8250, &charTexIndex, &spC0, decodedBufPos);
                    decodedBufPos++;
                }
                msgCtx->decodedBuffer.wchar[decodedBufPos] = 0x2000;
            } else if ((curChar >= 0x227) && (curChar < 0x22B)) {
                digits[0] = digits[1] = 0;
                digits[2] =
                    STRAY_FAIRY_SCATTERED_TOTAL - gSaveContext.save.saveInfo.inventory.strayFairies[curChar - 0x227];

                while (digits[2] >= 100) {
                    digits[0]++;
                    digits[2] -= 100;
                }
                while (digits[2] >= 10) {
                    digits[1]++;
                    digits[2] -= 10;
                }

                loadChar = false;
                for (i = 0; i < 3; i++) {
                    if ((i == 2) || (digits[i] != 0)) {
                        loadChar = true;
                    }
                    if (loadChar) {
                        Font_LoadChar(play, digits[i] + 0x824F, charTexIndex);
                        charTexIndex += FONT_CHAR_TEX_SIZE;
                        msgCtx->decodedBuffer.wchar[decodedBufPos] = digits[i] + 0x824F;
                        decodedBufPos++;
                        spC0 += 16.0f * msgCtx->textCharScale;
                    }
                }
                Message_LoadChar(play, 0x906C, &charTexIndex, &spC0, decodedBufPos);
            } else if (curChar == 0x22B) {
                digits[0] = digits[1] = digits[2] = 0;
                digits[3] = gSaveContext.minigameScore;

                while (digits[3] >= 1000) {
                    digits[0]++;
                    digits[3] -= 1000;
                }
                while (digits[3] >= 100) {
                    digits[1]++;
                    digits[3] -= 100;
                }
                while (digits[3] >= 10) {
                    digits[2]++;
                    digits[3] -= 10;
                }

                loadChar = false;
                for (i = 0; i < 4; i++) {
                    if ((i == 3) || (digits[i] != 0)) {
                        loadChar = true;
                    }
                    if (loadChar) {
                        Font_LoadChar(play, digits[i] + 0x824F, charTexIndex);
                        charTexIndex += FONT_CHAR_TEX_SIZE;
                        msgCtx->decodedBuffer.wchar[decodedBufPos] = digits[i] + 0x824F;
                        decodedBufPos++;
                        spC0 += 16.0f * msgCtx->textCharScale;
                    }
                }
                Message_LoadChar(play, 0x94AD, &charTexIndex, &spC0, decodedBufPos);
            } else if ((curChar == 0x22C) || (curChar == 0x22D)) {
                if (curChar == 0x22C) {
                    digits[0] = gSaveContext.save.saveInfo.lotteryCodes[CURRENT_DAY - 1][0];
                    digits[1] = gSaveContext.save.saveInfo.lotteryCodes[CURRENT_DAY - 1][1];
                    digits[2] = gSaveContext.save.saveInfo.lotteryCodes[CURRENT_DAY - 1][2];
                } else {
                    digits[0] = (HS_GET_LOTTERY_CODE_GUESS() & 0xF00) >> 8;
                    digits[1] = (HS_GET_LOTTERY_CODE_GUESS() & 0xF0) >> 4;
                    digits[2] = HS_GET_LOTTERY_CODE_GUESS() & 0xF;
                }
                for (i = 0; i < 3; i++) {
                    Font_LoadChar(play, digits[i] + 0x824F, charTexIndex);
                    charTexIndex += FONT_CHAR_TEX_SIZE;
                    msgCtx->decodedBuffer.wchar[decodedBufPos] = digits[i] + 0x824F;
                    decodedBufPos++;
                    spC0 += 16.0f * msgCtx->textCharScale;
                }
                decodedBufPos--;
            } else if (curChar == 0x22E) {
                digits[0] = digits[1] = digits[2] = 0;
                // #region 2S2H [Dpad]
                if (IS_HELD_DPAD(player->heldItemButton)) {
                    digits[3] = gItemPrices[DPAD_GET_CUR_FORM_BTN_ITEM(HELD_ITEM_TO_DPAD(player->heldItemButton))];
                } else {
                    digits[3] = gItemPrices[GET_CUR_FORM_BTN_ITEM(player->heldItemButton)];
                }
                // #endregion

                while (digits[3] >= 1000) {
                    digits[0]++;
                    digits[3] -= 1000;
                }
                while (digits[3] >= 100) {
                    digits[1]++;
                    digits[3] -= 100;
                }
                while (digits[3] >= 10) {
                    digits[2]++;
                    digits[3] -= 10;
                }

                loadChar = false;
                for (i = 0; i < 4; i++) {
                    if ((i == 3) || (digits[i] != 0)) {
                        loadChar = true;
                    }
                    if (loadChar) {
                        Font_LoadChar(play, digits[i] + 0x824F, charTexIndex);
                        charTexIndex += FONT_CHAR_TEX_SIZE;
                        msgCtx->decodedBuffer.wchar[decodedBufPos] = digits[i] + 0x824F;
                        decodedBufPos++;
                        spC0 += 16.0f * msgCtx->textCharScale;
                    }
                }
                func_8014CCB4(play, &decodedBufPos, &charTexIndex, &spC0);
            } else if (curChar == 0x22F) {
                for (i = 0; i < ARRAY_COUNT(gSaveContext.save.saveInfo.bomberCode); i++) {
                    digits[i] = gSaveContext.save.saveInfo.bomberCode[i];
                    Font_LoadChar(play, digits[i] + 0x824F, charTexIndex);
                    charTexIndex += FONT_CHAR_TEX_SIZE;
                    msgCtx->decodedBuffer.wchar[decodedBufPos] = digits[i] + 0x824F;
                    decodedBufPos++;
                    spC0 += 16.0f * msgCtx->textCharScale;
                }
                decodedBufPos--;
            } else if ((curChar >= 0x231) && (curChar < 0x237)) {
                msgCtx->decodedBuffer.wchar[decodedBufPos] = sMaskCodeColorCmdJPN[(
                    (void)0, gSaveContext.save.saveInfo.spiderHouseMaskOrder[(s16)(curChar - 0x231)])];
                decodedBufPos++;
                Message_LoadChar(
                    play,
                    D_801D0284[((void)0, gSaveContext.save.saveInfo.spiderHouseMaskOrder[(s16)(curChar - 0x231)])],
                    &charTexIndex, &spC0, decodedBufPos);
                decodedBufPos++;
                msgCtx->decodedBuffer.wchar[decodedBufPos] = 0x2000;
            } else if (curChar == 0x237) {
                if (GameInteractor_Should(VB_TIME_UNTIL_MOON_CRASH_CALCULATION, true, &timeToMoonCrash)) {
                    timeToMoonCrash = TIME_UNTIL_MOON_CRASH;
                }
                digits[0] = 0;
                digits[1] = TIME_TO_HOURS_F_ALT(timeToMoonCrash);

                while (digits[1] >= 10) {
                    digits[0]++;
                    digits[1] -= 10;
                }

                loadChar = false;
                for (i = 0; i < 2; i++) {
                    if ((i == 1) || (digits[i] != 0)) {
                        loadChar = true;
                    }
                    if (loadChar) {
                        Message_LoadChar(play, digits[i] + 0x824F, &charTexIndex, &spC0, decodedBufPos);
                        decodedBufPos++;
                    }
                }
                Font_LoadChar(play, 0x8E9E, charTexIndex);
                charTexIndex += FONT_CHAR_TEX_SIZE;
                msgCtx->decodedBuffer.wchar[decodedBufPos] = 0x8E9E;
                decodedBufPos++;
                Font_LoadChar(play, 0x8AD4, charTexIndex);
                charTexIndex += FONT_CHAR_TEX_SIZE;
                msgCtx->decodedBuffer.wchar[decodedBufPos] = 0x8E9E;
                spC0 += 2.0f * (16.0f * msgCtx->textCharScale);
            } else if (curChar == 0x238) {
                Message_LoadTime(play, curChar, &charTexIndex, &spC0, &decodedBufPos);
            } else if ((curChar == 0x300) || (curChar == 0x301) || (curChar == 0x302) || (curChar == 0x308)) {
                if (curChar == 0x308) {
                    value = (s32)HS_GET_HIGH_SCORE_3_LOWER();
                } else {
                    value = HIGH_SCORE(curChar - 0x300);
                }
                if (curChar == 0x302) {
                    if (LINK_AGE_IN_YEARS == YEARS_CHILD) {
                        value &= 0x7F;
                    } else {
                        value = (s16)((HIGH_SCORE(font->msgBuf.wchar[msgCtx->msgBufPos]) & 0xFF000000) >> 0x18) & 0x7F;
                    }
                }
                digits[3] = value;
                digits[0] = digits[1] = digits[2] = 0;

                while (digits[3] >= 1000) {
                    digits[0]++;
                    digits[3] -= 1000;
                }
                while (digits[3] >= 100) {
                    digits[1]++;
                    digits[3] -= 100;
                }
                while (digits[3] >= 10) {
                    digits[2]++;
                    digits[3] -= 10;
                }

                loadChar = false;
                for (i = 0; i < 4; i++) {
                    if ((i == 3) || (digits[i] != 0)) {
                        loadChar = true;
                    }
                    if (loadChar) {
                        Font_LoadChar(play, digits[i] + 0x824F, charTexIndex);
                        charTexIndex += FONT_CHAR_TEX_SIZE;
                        msgCtx->decodedBuffer.wchar[decodedBufPos] = digits[i] + 0x824F;
                        decodedBufPos++;
                        spC0 += 16.0f * msgCtx->textCharScale;
                    }
                }
                decodedBufPos--;
            } else if ((curChar == 0x307) || (curChar == 0x309) || (curChar == 0x30A) || (curChar == 0x30B) ||
                       (curChar == 0x30C)) {
                var_fs0 = 8.0f;
                if (curChar == 0x307) {
                    Message_GetTimerDigits(GET_HIGH_SCORE(HS_UNK_1), spAC);
                } else if (curChar == 0x309) {
                    Message_GetTimerDigits(GET_HIGH_SCORE(HS_HORSE_BACK_BALLOON), spAC);
                } else {
                    Message_GetTimerDigits(
                        ((void)0, gSaveContext.save.saveInfo.dekuPlaygroundHighScores[curChar - 0x30A]), spAC);
                }

                loadChar = false;
                for (i = 0; i < ARRAY_COUNT(spAC); i++) {
                    if ((i == 4) || ((i != 2) && (i != 5) && (spAC[i] != '\0'))) {
                        loadChar = true;
                    }
                    if (loadChar) {
                        Font_LoadChar(play, spAC[i] + 0x824F, charTexIndex);
                        charTexIndex += FONT_CHAR_TEX_SIZE;
                        msgCtx->decodedBuffer.wchar[decodedBufPos] = spAC[i] + 0x824F;
                        decodedBufPos++;
                    } else {
                        var_fs0 -= 1.0f;
                    }
                }
                spC0 += var_fs0 * (16.0f * msgCtx->textCharScale);
                decodedBufPos--;
            } else if ((curChar == 0x303) || (curChar == 0x304) || (curChar == 0x305)) {
                value = HIGH_SCORE(curChar - 0x303 + HS_BOAT_ARCHERY);
                digits[0] = digits[1] = digits[2] = 0;
                digits[3] = value;

                while (digits[3] >= 60) {
                    digits[1]++;
                    if (digits[1] >= 10) {
                        digits[0]++;
                        digits[1] -= 10;
                    }
                    digits[3] -= 60;
                }
                while (digits[3] >= 10) {
                    digits[2]++;
                    digits[3] -= 10;
                }

                for (i = 0; i < 4; i++) {
                    Font_LoadChar(play, digits[i] + 0x824F, charTexIndex);
                    charTexIndex += FONT_CHAR_TEX_SIZE;
                    msgCtx->decodedBuffer.wchar[decodedBufPos] = digits[i] + 0x824F;
                    decodedBufPos++;
                    if (i == 1) {
                        Font_LoadChar(play, 0x95AA, charTexIndex);
                        charTexIndex += FONT_CHAR_TEX_SIZE;
                        msgCtx->decodedBuffer.wchar[decodedBufPos] = 0x95AA;
                        decodedBufPos++;
                    } else if (i == 3) {
                        Font_LoadChar(play, 0x9562, charTexIndex);
                        charTexIndex += FONT_CHAR_TEX_SIZE;
                        msgCtx->decodedBuffer.wchar[decodedBufPos] = 0x9562;
                    }
                }
                spC0 += 4.0f * (16.0f * msgCtx->textCharScale);
            } else if (curChar == 0x306) {
                digits[0] = digits[1] = digits[2] = 0;
                digits[3] = HS_GET_TOWN_SHOOTING_GALLERY_HIGH_SCORE();

                while (digits[3] >= 1000) {
                    digits[0]++;
                    digits[3] -= 1000;
                }
                while (digits[3] >= 100) {
                    digits[1]++;
                    digits[3] -= 100;
                }
                while (digits[3] >= 10) {
                    digits[2]++;
                    digits[3] -= 10;
                }

                loadChar = false;
                for (i = 0; i < 4; i++) {
                    if ((i == 3) || (digits[i] != 0)) {
                        loadChar = true;
                    }
                    if (loadChar) {
                        Font_LoadChar(play, digits[i] + 0x824F, charTexIndex);
                        charTexIndex += FONT_CHAR_TEX_SIZE;
                        msgCtx->decodedBuffer.wchar[decodedBufPos] = digits[i] + 0x824F;
                        decodedBufPos++;
                        spC0 += 16.0f * msgCtx->textCharScale;
                    }
                }
                Message_LoadChar(play, 0x9543, &charTexIndex, &spC0, decodedBufPos);
            } else if ((curChar == 0x30D) || (curChar == 0x30E) || (curChar == 0x30F)) {
                index = curChar - 0x30D;

                for (playerNameLen = 8; playerNameLen > 0; playerNameLen--) {
                    if (gSaveContext.save.saveInfo.inventory.dekuPlaygroundPlayerName[index][playerNameLen - 1] !=
                        0x3E) {
                        break;
                    }
                }

                for (i = 0; i < playerNameLen; i++) {
                    curChar = ((void)0, gSaveContext.save.saveInfo.inventory.dekuPlaygroundPlayerName[index][i]);
                    //! FAKE: 0xFFFF
                    fontBuf = &font->fontBuf[(curChar & 0xFFFF) * FONT_CHAR_TEX_SIZE];
                    msgCtx->decodedBuffer.wchar[decodedBufPos + i] = 0x30D;

                    for (var_v0 = 0; var_v0 < FONT_CHAR_TEX_SIZE; var_v0 += 4) {
                        font->charBuf[font->unk_11D88][charTexIndex + var_v0 + 0] = fontBuf[var_v0 + 0];
                        font->charBuf[font->unk_11D88][charTexIndex + var_v0 + 1] = fontBuf[var_v0 + 1];
                        font->charBuf[font->unk_11D88][charTexIndex + var_v0 + 2] = fontBuf[var_v0 + 2];
                        font->charBuf[font->unk_11D88][charTexIndex + var_v0 + 3] = fontBuf[var_v0 + 3];
                    }
                    charTexIndex += FONT_CHAR_TEX_SIZE;
                }
                decodedBufPos += playerNameLen - 1;
                spC0 += playerNameLen * (16.0f * msgCtx->textCharScale);
            } else if (curChar == 0x310) {
                digits[0] = digits[1] = digits[2] = 0;
                digits[3] = HS_GET_BOAT_ARCHERY_HIGH_SCORE();

                while (digits[3] >= 1000) {
                    digits[0]++;
                    digits[3] -= 1000;
                }
                while (digits[3] >= 100) {
                    digits[1]++;
                    digits[3] -= 100;
                }
                while (digits[3] >= 10) {
                    digits[2]++;
                    digits[3] -= 10;
                }

                loadChar = false;
                for (i = 0; i < 4; i++) {
                    if ((i == 3) || (digits[i] != 0)) {
                        loadChar = true;
                    }
                    if (loadChar) {
                        Font_LoadChar(play, digits[i] + 0x824F, charTexIndex);
                        charTexIndex += FONT_CHAR_TEX_SIZE;
                        msgCtx->decodedBuffer.wchar[decodedBufPos] = digits[i] + 0x824F;
                        decodedBufPos++;
                        spC0 += 16.0f * msgCtx->textCharScale;
                    }
                }
                Message_LoadChar(play, 0x94AD, &charTexIndex, &spC0, decodedBufPos);
            } else if (curChar == 0x1F) {
                decodedBufPos--;
                for (i = 0; i < font->msgBuf.wchar[msgCtx->msgBufPos + 1]; i++) {
                    msgCtx->decodedBuffer.wchar[++decodedBufPos] = ' ';
                    spC0 += 6.0f;
                }
                msgCtx->msgBufPos++;
            } else if ((curChar == 0xC) || (curChar == 0xA)) {
                msgCtx->unk11F1A[spD2] = 0;
                if (msgCtx->unk11F18 == 0) {
                    msgCtx->unk11F1A[spD2] = ((msgCtx->textCharScale * 16.0f * 16.0f) - spC0) * 0.5f;
                }
                spD2++;
                spC0 = 0.0f;
                if (curChar == 0xA) {
                    numLines++;
                }
            } else if ((curChar == 0x101) || (curChar == 0x102) || (curChar == 0x104) ||
                       ((curChar >= 0x130) && (curChar < 0x137))) {
                // pass
            } else if (curChar == 0x111) {
                msgCtx->textFade = true;
                msgCtx->decodedBuffer.wchar[++decodedBufPos] = font->msgBuf.wchar[++msgCtx->msgBufPos] & 0xFF;
            } else if (curChar == 0x120) {
                msgCtx->decodedBuffer.wchar[++decodedBufPos] = font->msgBuf.wchar[++msgCtx->msgBufPos];
                if (msgCtx->textboxSkipped || (msgCtx->textBoxType == TEXTBOX_TYPE_1) ||
                    (msgCtx->textBoxType == TEXTBOX_TYPE_3) || (msgCtx->textBoxType == TEXTBOX_TYPE_6) ||
                    (msgCtx->textBoxType == TEXTBOX_TYPE_8) || (msgCtx->textBoxType == TEXTBOX_TYPE_9) ||
                    (msgCtx->textBoxType == TEXTBOX_TYPE_B) || (msgCtx->unk11F0C == 3)) {
                    Audio_PlaySfx(msgCtx->decodedBuffer.wchar[decodedBufPos]);
                }
            } else if (curChar == 0x128) {
                msgCtx->decodedBuffer.wchar[++decodedBufPos] = font->msgBuf.wchar[++msgCtx->msgBufPos];
            } else if ((curChar != 0x20) && ((curChar < 0x2000) || (curChar >= 0x2009))) {
                Font_LoadChar(play, curChar, charTexIndex);
                charTexIndex += FONT_CHAR_TEX_SIZE;
                if ((curChar == 0x8169) || (curChar == 0x8175)) {
                    spC0 += 10.0f * msgCtx->textCharScale;
                } else if (curChar == 0x8145) {
                    spC0 += (12.0f * msgCtx->textCharScale);
                } else if ((curChar == 0x8148) || (curChar == 0x8149)) {
                    spC0 += 14.0f * msgCtx->textCharScale;
                } else if (curChar == 0x8144) {
                    spC0 += 8.0f * msgCtx->textCharScale;
                } else if ((curChar == 0x816A) || (curChar == 0x8176)) {
                    spC0 += 10.0f * msgCtx->textCharScale;
                } else if ((curChar == 0x8141) || (curChar == 0x8142) || (curChar == 0x8168)) {
                    spC0 += 12.0f * msgCtx->textCharScale;
                } else if (curChar == 0x8194) {
                    spC0 += 14.0f * msgCtx->textCharScale;
                } else {
                    spC0 += 16.0f * msgCtx->textCharScale;
                }

            } else if (curChar == 0x20) {
                spC0 += 6.0f;
            }

            decodedBufPos++;
            msgCtx->msgBufPos++;
        }
    } else if (msgCtx->textIsCredits) {
        Message_DecodeCredits(play);
    } else {
        Message_DecodeNES(play);
    }
}

void func_80150A84(PlayState* play) {
    MessageContext* msgCtx = &play->msgCtx;
    s32 textBoxType = msgCtx->textBoxType;

    if (D_801CFC78[textBoxType] != 14) {
        // #region 2S2H [Port]
        // DmaMgr_SendRequest0(msgCtx->textboxSegment,
        //                    SEGMENT_ROM_START(message_static) + D_801CFC78[textBoxType] * 0x1000, 0x1000);
        msgCtx->textboxSegment[TEXTBOX_SEG_TYPE] = gMessageBackgrounds[D_801CFC78[textBoxType]];
        // #endregion

        if (!play->pauseCtx.bombersNotebookOpen) {
            if ((textBoxType == TEXTBOX_TYPE_0) || (textBoxType == TEXTBOX_TYPE_6) || (textBoxType == TEXTBOX_TYPE_A) ||
                (textBoxType == TEXTBOX_TYPE_B)) {
                msgCtx->textboxColorRed = 0;
                msgCtx->textboxColorGreen = 0;
                msgCtx->textboxColorBlue = 0;
            } else if (textBoxType == TEXTBOX_TYPE_1) {
                msgCtx->textboxColorRed = 70;
                msgCtx->textboxColorGreen = 50;
                msgCtx->textboxColorBlue = 30;
            } else if ((textBoxType == TEXTBOX_TYPE_2) || (textBoxType == TEXTBOX_TYPE_8)) {
                msgCtx->textboxColorRed = 0;
                msgCtx->textboxColorGreen = 10;
                msgCtx->textboxColorBlue = 50;
            } else if (textBoxType == TEXTBOX_TYPE_D) {
                msgCtx->textboxColorRed = 255;
                msgCtx->textboxColorGreen = 255;
                msgCtx->textboxColorBlue = 195;
            } else {
                msgCtx->textboxColorRed = 255;
                msgCtx->textboxColorGreen = 0;
                msgCtx->textboxColorBlue = 0;
            }

            if (textBoxType == TEXTBOX_TYPE_1) {
                msgCtx->textboxColorAlphaTarget = 230;
            } else if (textBoxType == TEXTBOX_TYPE_3) {
                msgCtx->textboxColorAlphaTarget = 180;
            } else if (textBoxType == TEXTBOX_TYPE_D) {
                msgCtx->textboxColorAlphaTarget = 220;
            } else {
                msgCtx->textboxColorAlphaTarget = 170;
            }
        } else {
            msgCtx->textboxColorRed = 255;
            msgCtx->textboxColorGreen = 255;
            msgCtx->textboxColorBlue = 255;
            msgCtx->textboxColorAlphaTarget = 255;
        }
        msgCtx->textboxColorAlphaCurrent = 0;
    }
}

void Message_OpenText(PlayState* play, u16 textId) {
    MessageContext* msgCtx = &play->msgCtx;
    Font* font = &msgCtx->font;
    Player* player = GET_PLAYER(play);
    f32 var_fv0;

    bool loadFromMessageTable = true;
    GameInteractor_ExecuteOnOpenText(&textId, &loadFromMessageTable);

    // BENTODO do this somewhere else
    gSaveContext.options.language = LANGUAGE_ENG;

    if (play->msgCtx.msgMode == MSGMODE_NONE) {
        gSaveContext.prevHudVisibility = gSaveContext.hudVisibility;
    }

    if (textId == 0xFF) {
        Interface_SetBButtonInterfaceDoAction(play, DO_ACTION_STOP);
        play->msgCtx.hudVisibility = gSaveContext.hudVisibility;
        Interface_SetHudVisibility(HUD_VISIBILITY_A_B_C);
        gSaveContext.save.unk_06 = 20;
    } else if ((textId == 0x579) || (textId == 0x8D8)) {
        Interface_SetHudVisibility(HUD_VISIBILITY_A_HEARTS_MAGIC_WITH_OVERWRITE);
    } else if (((textId == 0x28) || (textId == 0x29) || (textId == 0x2A)) &&
               (player->transformation == PLAYER_FORM_DEKU)) {
        //! FAKE:
        if (msgCtx) {}
        textId = 0xC9;
    } else if (textId == 0x11) {
        if (gSaveContext.save.saveInfo.inventory.strayFairies[((void)0, gSaveContext.dungeonSceneSharedIndex)] == 0xF) {
            textId = 0xF3;
        }
    } else if ((textId == 0x92) && (play->sceneId == SCENE_KOEPONARACE)) {
        textId = 0xCD;
    }

    if ((textId == 0xC) && (GET_QUEST_HEART_PIECE_COUNT != 0)) {
        textId = GET_QUEST_HEART_PIECE_COUNT;
        textId += 0xC4;
    }

    msgCtx->currentTextId = textId;

    if ((msgCtx->currentTextId == 0xC) || (msgCtx->currentTextId == 0xD) || (msgCtx->currentTextId == 0xC5) ||
        (msgCtx->currentTextId == 0xC6) || (msgCtx->currentTextId == 0xC7)) {
        Interface_SetHudVisibility(HUD_VISIBILITY_A_HEARTS_MAGIC_WITH_OVERWRITE);
    }

    msgCtx->messageHasSetSfx = false;
    D_801C6A70 = 0;
    msgCtx->textboxSkipped = false;
    msgCtx->textIsCredits = false;
    var_fv0 = 1.0f;

    if (play->pauseCtx.bombersNotebookOpen) {
        if (gSaveContext.options.language == LANGUAGE_JPN) {
            msgCtx->textCharScale = 1.4f;
            msgCtx->unk11FFC = 0x1E;
            msgCtx->unk11FF8 = 0x32;
            var_fv0 = 1.4;
        } else {
            msgCtx->textCharScale = 1.4f;
            msgCtx->unk11FFC = 0x16;
            msgCtx->unk11FF8 = 0x32;
            var_fv0 = 1.4;
        }
    } else if (textId >= 0x4E20) {
        msgCtx->textIsCredits = true;
        msgCtx->textCharScale = 0.85f;
        msgCtx->unk11FFC = 6;
        msgCtx->unk11FF8 = 0x14;
    } else if (gSaveContext.options.language == LANGUAGE_JPN) {
        msgCtx->textCharScale = 0.88f;
        msgCtx->unk11FFC = 0x12;
        msgCtx->unk11FF8 = 0x32;
    } else {
        msgCtx->textCharScale = 0.75f;
        msgCtx->unk11FFC = 0xC;
        msgCtx->unk11FF8 = 0x41;
    }

    sCharTexSize = msgCtx->textCharScale * 16.0f;
    sCharTexScale = 1024.0f / msgCtx->textCharScale;
    D_801F6B08 = 1024.0f / var_fv0;
    // BENTODO all of these
    if (!loadFromMessageTable) {
        // no-op
    } else if (msgCtx->textIsCredits) {
        Message_FindCreditsMessage(play, textId);
        MessageTableEntry* msgEntry = (MessageTableEntry*)font->messageStart;
        msgCtx->msgLength = msgEntry->msgSize;
        memcpy(&font->msgBuf, msgEntry->segment, msgEntry->msgSize);
        // DmaMgr_SendRequest0(&font->msgBuf, SEGMENT_ROM_START(staff_message_data_static) + font->messageStart,
        //                     font->messageEnd);
    } else if (gSaveContext.options.language == LANGUAGE_JPN) {
        Message_FindMessage(play, textId);
        msgCtx->msgLength = font->messageEnd;
        // DmaMgr_SendRequest0(&font->msgBuf, SEGMENT_ROM_START(message_data_static) + font->messageStart,
        //                     font->messageEnd);
    } else {
        Message_FindMessageNES(play, textId);
        MessageTableEntry* msgEntry = (MessageTableEntry*)font->messageStart;
        msgCtx->msgLength = msgEntry->msgSize;
        memcpy(&font->msgBuf, msgEntry->segment, msgEntry->msgSize);
        // msgCtx->msgLength = font->messageEnd;
        // DmaMgr_SendRequest0(&font->msgBuf, SEGMENT_ROM_START(message_data_static) + font->messageStart,
        //                     font->messageEnd);
    }

    msgCtx->choiceNum = 0;
    msgCtx->textUnskippable = false;
    msgCtx->textboxEndType = TEXTBOX_ENDTYPE_DEFAULT;
    msgCtx->textDrawPos = 0;
    msgCtx->msgBufPos = 0;
    msgCtx->decodedTextLen = 0;

    msgCtx->unk11F08 = font->msgBuf.schar[msgCtx->msgBufPos] << 8;
    msgCtx->unk11F08 |= font->msgBuf.schar[msgCtx->msgBufPos + 1];

    msgCtx->unk11F18 = (msgCtx->unk11F08 & 0xF000) >> 0xC;
    msgCtx->textBoxType = (msgCtx->unk11F08 & 0xF00) >> 8;
    msgCtx->textBoxPos = (msgCtx->unk11F08 & 0xF0) >> 4;
    msgCtx->unk11F0C = msgCtx->unk11F08 & 0xF;

    if ((msgCtx->unk11F0C == 1) || (msgCtx->unk11F0C == 3)) {
        msgCtx->textUnskippable = true;
    }
    msgCtx->itemId = 0xFE;

    if ((msgCtx->textBoxType == TEXTBOX_TYPE_5) || (msgCtx->textBoxType == TEXTBOX_TYPE_D) ||
        (play->pauseCtx.bombersNotebookOpen)) {
        msgCtx->unk120CE = msgCtx->unk120D0 = msgCtx->unk120D2 = 0;
    } else {
        msgCtx->unk120CE = msgCtx->unk120D0 = msgCtx->unk120D2 = 0xFF;
    }

    msgCtx->choiceIndex = 0;

    if (msgCtx->unk11F0C != 3) {
        msgCtx->textColorAlpha = 0xFF;
    } else {
        msgCtx->textColorAlpha = 0;
    }

    if (textId == 0x1467) {
        Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_KAFEI);
    }

    if (textId == 0x2955) {
        Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_ANJU);
        Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_KAFEI);
        Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_RECEIVED_COUPLES_MASK);
    }
}

void func_801514B0(PlayState* play, u16 arg1, u8 arg2) {
    MessageContext* msgCtx = &play->msgCtx;
    Font* font = &msgCtx->font;
    Player* player = GET_PLAYER(play);
    f32 temp = 1024.0f;
    // BENTODO do this somewhere else
    gSaveContext.options.language = LANGUAGE_ENG;

    msgCtx->ocarinaAction = 0xFFFF;

    if (msgCtx->msgMode == MSGMODE_NONE) {
        gSaveContext.prevHudVisibility = gSaveContext.hudVisibility;
    }
    msgCtx->messageHasSetSfx = false;
    D_801C6A70 = 0;
    msgCtx->textboxSkipped = false;
    msgCtx->textIsCredits = false;

    if (gSaveContext.options.language == LANGUAGE_JPN) {
        msgCtx->textCharScale = 0.88f;
        msgCtx->unk11FFC = 0x12;
        msgCtx->unk11FF8 = 0x32;
    } else {
        msgCtx->textCharScale = 0.75f;
        msgCtx->unk11FFC = 0xC;
        msgCtx->unk11FF8 = 0x41;
    }

    sCharTexSize = msgCtx->textCharScale * 16.0f;
    sCharTexScale = temp / msgCtx->textCharScale;
    D_801F6B08 = temp / 1;

    if ((arg1 == 0x1709) && (player->transformation == 3)) {
        arg1 = 0x1705;
    }

    msgCtx->currentTextId = arg1;

    if (gSaveContext.options.language == LANGUAGE_JPN) {
        Message_FindMessage(play, arg1);
        msgCtx->msgLength = font->messageEnd;
        // BENTODO
        // DmaMgr_SendRequest0(&font->msgBuf, SEGMENT_ROM_START(message_data_static) + font->messageStart,
        //                    font->messageEnd);
    } else {
        Message_FindMessageNES(play, arg1);
        MessageTableEntry* msgEntry = (MessageTableEntry*)font->messageStart;
        msgCtx->msgLength = msgEntry->msgSize;
        memcpy(&font->msgBuf, msgEntry->segment, msgEntry->msgSize);
        // msgCtx->msgLength = font->messageEnd;
        //  BENTODO
        // DmaMgr_SendRequest0(&font->msgBuf, SEGMENT_ROM_START(message_data_static) + font->messageStart,
        //                     font->messageEnd);
    }
    msgCtx->choiceNum = 0;
    msgCtx->textUnskippable = false;
    msgCtx->textboxEndType = TEXTBOX_ENDTYPE_DEFAULT;
    msgCtx->textDrawPos = 0;
    msgCtx->msgBufPos = 0;
    msgCtx->decodedTextLen = 0;
    msgCtx->unk11F08 = font->msgBuf.wchar[msgCtx->msgBufPos];
    msgCtx->unk11F18 = (msgCtx->unk11F08 & 0xF000) >> 0xC;
    msgCtx->textBoxType = TEXTBOX_TYPE_9;
    msgCtx->textBoxPos = arg2;
    msgCtx->unk11F0C = msgCtx->unk11F08 & 0xF;
    msgCtx->textUnskippable = true;
    // #region 2S2H [Port]
    // DmaMgr_SendRequest0(msgCtx->textboxSegment, SEGMENT_ROM_START(message_static) + (D_801CFC78[0] * 0x1000),
    // 0x1000);
    msgCtx->textboxSegment[TEXTBOX_SEG_TYPE] = gMessageBackgrounds[D_801CFC78[0]];
    // #endregion
    msgCtx->textboxColorRed = 0;
    msgCtx->textboxColorGreen = 0;
    msgCtx->textboxColorBlue = 0;
    msgCtx->textboxColorAlphaTarget = 0xE6;
    msgCtx->textboxColorAlphaCurrent = 0;
    msgCtx->textColorAlpha = 0xFF;
    msgCtx->talkActor = NULL;
    msgCtx->msgMode = MSGMODE_TEXT_START;
    msgCtx->stateTimer = 0;
    msgCtx->textDelayTimer = 0;
    play->msgCtx.ocarinaMode = OCARINA_MODE_NONE;
    msgCtx->unk120D2 = 0xFF;
    msgCtx->unk120CE = msgCtx->unk120D0 = msgCtx->unk120D2;
}

void Message_StartTextbox(PlayState* play, u16 textId, Actor* actor) {
    MessageContext* msgCtx = &play->msgCtx;

    msgCtx->ocarinaAction = 0xFFFF;
    Message_OpenText(play, textId);
    msgCtx->talkActor = actor;
    msgCtx->msgMode = MSGMODE_TEXT_START;
    msgCtx->stateTimer = 0;
    msgCtx->textDelayTimer = 0;
    play->msgCtx.ocarinaMode = OCARINA_MODE_NONE;
}

void Message_ContinueTextbox(PlayState* play, u16 textId) {
    MessageContext* msgCtx = &play->msgCtx;
    InterfaceContext* interfaceCtx = &play->interfaceCtx;

    msgCtx->msgLength = 0;
    Message_OpenText(play, textId);
    func_80150A84(play);
    msgCtx->msgMode = MSGMODE_TEXT_CONTINUING;
    msgCtx->stateTimer = 8;
    msgCtx->textDelayTimer = 0;

    if (!interfaceCtx->bButtonInterfaceDoActionActive) {
        if (textId != 0x1B93) {
            Interface_SetAButtonDoAction(play, DO_ACTION_NEXT);
        } else if (textId != 0xF8) {
            Interface_SetAButtonDoAction(play, DO_ACTION_DECIDE);
        }
    }
    msgCtx->textboxColorAlphaCurrent = msgCtx->textboxColorAlphaTarget;

    if (play->pauseCtx.bombersNotebookOpen) {
        msgCtx->textboxXTarget = 34;
        msgCtx->textboxYTarget = 350;
        Message_GrowTextbox(play);
        msgCtx->stateTimer = 1;
    }
}

void Message_DisplaySceneTitleCard(PlayState* play, u16 textId) {
    MessageContext* msgCtx = &play->msgCtx;

    msgCtx->msgLength = 0;
    Message_OpenText(play, textId);
    func_80150A84(play);
    Message_DecodeNES(play);
    msgCtx->msgMode = MSGMODE_SCENE_TITLE_CARD_FADE_IN_BACKGROUND;
    msgCtx->textDelayTimer = 0;
    msgCtx->textboxColorAlphaCurrent = msgCtx->textboxColorAlphaTarget = msgCtx->textColorAlpha = 0;
    msgCtx->stateTimer = 30;

    // Day/Dawn/Night.. Messages
    if ((msgCtx->currentTextId >= 0x1BB2) && (msgCtx->currentTextId <= 0x1BB6)) {
        XREG(74) = 0x6A;
        XREG(75) = 0;
        XREG(77) = 0x58;
        XREG(76) = 0x44;
    } else {
        XREG(74) = 0x42;
        XREG(75) = 0x1E;
        XREG(77) = 0x3C;
        XREG(76) = 0x1C;
        msgCtx->unk11F1A[0] = msgCtx->unk11F1A[1] = msgCtx->unk11F1A[2] = 0;
        Interface_SetHudVisibility(HUD_VISIBILITY_NONE);
    }
}

void Message_BombersNotebookQueueEvent(PlayState* play, u8 event) {
    MessageContext* msgCtx = &play->msgCtx;

    if (CHECK_QUEST_ITEM(QUEST_BOMBERS_NOTEBOOK)) {
        if (!CHECK_WEEKEVENTREG(gBombersNotebookWeekEventFlags[event])) {
            msgCtx->bombersNotebookEventQueue[msgCtx->bombersNotebookEventQueueCount] = event;
            msgCtx->bombersNotebookEventQueueCount++;
        }
    } else if (event >= BOMBERS_NOTEBOOK_PERSON_MAX) {
        // Non MET events are processed even if the player does not have the notebook yet
        if (!CHECK_WEEKEVENTREG(gBombersNotebookWeekEventFlags[event])) {
            msgCtx->bombersNotebookEventQueue[msgCtx->bombersNotebookEventQueueCount] = event;
            msgCtx->bombersNotebookEventQueueCount++;
        }
    }
}

/**
 * @return s32 true if an entry to be shown was found and triggered
 */
s32 Message_BombersNotebookProcessEventQueue(PlayState* play) {
    MessageContext* msgCtx = &play->msgCtx;

    while (true) {
        if (msgCtx->bombersNotebookEventQueueCount == 0) {
            return false;
        }
        msgCtx->bombersNotebookEventQueueCount--;

        if (!CHECK_WEEKEVENTREG(gBombersNotebookWeekEventFlags
                                    [msgCtx->bombersNotebookEventQueue[msgCtx->bombersNotebookEventQueueCount]])) {
            SET_WEEKEVENTREG(gBombersNotebookWeekEventFlags
                                 [msgCtx->bombersNotebookEventQueue[msgCtx->bombersNotebookEventQueueCount]]);

            if ((sBombersNotebookEventMessages
                     [msgCtx->bombersNotebookEventQueue[msgCtx->bombersNotebookEventQueueCount]] != 0) &&
                CHECK_QUEST_ITEM(QUEST_BOMBERS_NOTEBOOK)) {
                Message_ContinueTextbox(
                    play, sBombersNotebookEventMessages
                              [msgCtx->bombersNotebookEventQueue[msgCtx->bombersNotebookEventQueueCount]]);
                Audio_PlaySfx(NA_SE_SY_SCHEDULE_WRITE);
                return true;
            }
        }
    }
}

u16 D_801D028C[] = {
    0x1B5A, // OCARINA_ACTION_0
    0x1B5B, //
    0x1B5C, // OCARINA_ACTION_DEMONSTRATE_SONATA
    0x1B5D, // OCARINA_ACTION_DEMONSTRATE_GORON_LULLABY
    0x1B5E, // OCARINA_ACTION_DEMONSTRATE_NEW_WAVE
    0x1B5F, // OCARINA_ACTION_DEMONSTRATE_ELEGY
    0x1B60, // OCARINA_ACTION_DEMONSTRATE_OATH
    0x1B61, // OCARINA_ACTION_DEMONSTRATE_SARIAS
    0x1B62, // OCARINA_ACTION_DEMONSTRATE_TIME
    0x1B63, // OCARINA_ACTION_DEMONSTRATE_HEALING
    0x1B64, // OCARINA_ACTION_DEMONSTRATE_EPONAS
    0x1B65, // OCARINA_ACTION_DEMONSTRATE_SOARING
    0x1B66, // OCARINA_ACTION_DEMONSTRATE_STORMS
    0x1B67, // OCARINA_ACTION_DEMONSTRATE_SUNS
    0x1B68, // OCARINA_ACTION_DEMONSTRATE_INVERTED_TIME
    0x1B69, // OCARINA_ACTION_DEMONSTRATE_DOUBLE_TIME
    0x1B6A, // OCARINA_ACTION_DEMONSTRATE_GORON_LULLABY_INTRO
    0,      // OCARINA_ACTION_11
    0xDC7,  // OCARINA_ACTION_3B
    0xDC9,  // OCARINA_ACTION_3C
    0x120A, // OCARINA_ACTION_DEMONSTRATE_EVAN_PART1_FIRST_HALF
    0x120B, // OCARINA_ACTION_DEMONSTRATE_EVAN_PART2_FIRST_HALF
    0x122C, // OCARINA_ACTION_DEMONSTRATE_EVAN_PART1_SECOND_HALF
    0x122F, // OCARINA_ACTION_DEMONSTRATE_EVAN_PART2_SECOND_HALF
    0x120A, // OCARINA_ACTION_PROMPT_EVAN_PART1_SECOND_HALF
    0x120B, // OCARINA_ACTION_PROMPT_EVAN_PART2_SECOND_HALF
};

u16 sOcarinaSongFlagsMap[] = { 1, 2, 4, 8, 0x10, 0x20, 0x40, 0x80, 0x100, 0x200, 0x400, 0x800 };

void Message_DisplayOcarinaStaffImpl(PlayState* play, u16 ocarinaAction) {
    MessageContext* msgCtx = &play->msgCtx;
    s16 j;
    s16 noStop;
    s32 k;
    u32 i;

    msgCtx->ocarinaAvailableSongs = 0;

    for (i = msgCtx->ocarinaAvailableSongs; i <= (QUEST_SONG_SUN - QUEST_SONG_SONATA); i++) {
        if (CHECK_QUEST_ITEM(QUEST_SONG_SONATA + i)) {
            msgCtx->ocarinaAvailableSongs = msgCtx->ocarinaAvailableSongs | sOcarinaSongFlagsMap[i];
        }
    }

    if (CHECK_QUEST_ITEM(QUEST_SONG_TIME)) {
        msgCtx->ocarinaAvailableSongs |= 0x3000;
    }
    if (CHECK_QUEST_ITEM(QUEST_SONG_LULLABY_INTRO)) {
        msgCtx->ocarinaAvailableSongs |= 0x4000;
    }
    if (CHECK_QUEST_ITEM(QUEST_SONG_LULLABY)) {
        msgCtx->ocarinaAvailableSongs &= (u16)~0x4000;
    }
    if (gSaveContext.save.saveInfo.scarecrowSpawnSongSet) {
        msgCtx->ocarinaAvailableSongs |= 0x400000;
    }
    if (CHECK_EVENTINF(EVENTINF_31)) {
        msgCtx->ocarinaAvailableSongs |= 0x800000;
    }

    msgCtx->ocarinaStaff = AudioOcarina_GetRecordingStaff();

    if ((ocarinaAction == OCARINA_ACTION_PROMPT_EVAN_PART1_SECOND_HALF) ||
        (ocarinaAction == OCARINA_ACTION_PROMPT_EVAN_PART2_SECOND_HALF)) {
        sOcarinaButtonAlphaValues[0] = sOcarinaButtonAlphaValues[1] = sOcarinaButtonAlphaValues[2] =
            sOcarinaButtonAlphaValues[3] = 255;
    } else {
        msgCtx->ocarinaStaff->pos = 0;
        sOcarinaButtonIndexBufPos = 0;
        sOcarinaButtonIndexBufLen = 0;
        Message_ResetOcarinaButtonState(play);
    }

    sLastPlayedSong = 0xFF;
    msgCtx->lastPlayedSong = 0xFF;
    msgCtx->ocarinaButtonIndex = OCARINA_BTN_INVALID;
    noStop = false;
    msgCtx->ocarinaAction = ocarinaAction;

    if ((ocarinaAction >= OCARINA_ACTION_TIMED_PROMPT_SONATA) &&
        (ocarinaAction <= OCARINA_ACTION_TIMED_PROMPT_STORMS)) {
        Message_OpenText(play, 0x1B59);
        func_80150A84(play);
    } else if ((ocarinaAction == OCARINA_ACTION_3B) || (ocarinaAction == OCARINA_ACTION_3C)) {
        noStop = true;
        Message_OpenText(play, D_801D028C[ocarinaAction - 0x29]);
        func_80150A84(play);
    } else if ((ocarinaAction >= OCARINA_ACTION_DEMONSTRATE_EVAN_PART1_FIRST_HALF) &&
               (ocarinaAction <= OCARINA_ACTION_PROMPT_EVAN_PART2_SECOND_HALF)) {
        noStop = true;
        Message_OpenText(play, D_801D028C[ocarinaAction - 0x29]);
        func_80150A84(play);
    } else if ((ocarinaAction >= OCARINA_ACTION_PROMPT_WIND_FISH_HUMAN) &&
               (ocarinaAction <= OCARINA_ACTION_PROMPT_WIND_FISH_DEKU)) {
        noStop = true;
        Message_OpenText(play, 0x1B59);
        func_80150A84(play);
    } else if ((ocarinaAction == OCARINA_ACTION_FREE_PLAY) || (ocarinaAction >= OCARINA_ACTION_CHECK_TIME)) {
        if ((ocarinaAction >= OCARINA_ACTION_CHECK_TIME) && (ocarinaAction <= OCARINA_ACTION_CHECK_STORMS)) {
            Audio_PlaySfx(NA_SE_SY_TRE_BOX_APPEAR);
        }
        if (ocarinaAction == OCARINA_ACTION_SCARECROW_SPAWN_DEMONSTRATION) {
            Message_OpenText(play, 0x1B5B);
            func_80150A84(play);
        } else {
            Message_OpenText(play, 0x1B5A);
            func_80150A84(play);
        }
    } else {
        noStop = true;
        if (ocarinaAction >= OCARINA_ACTION_PROMPT_SONATA) {
            Message_OpenText(play, 0x1B59);
            func_80150A84(play);
        } else {
            Message_OpenText(play, D_801D028C[ocarinaAction]);
            func_80150A84(play);
        }
    }

    msgCtx->talkActor = NULL;
    msgCtx->textDelayTimer = 0;
    play->msgCtx.ocarinaMode = OCARINA_MODE_NONE;
    msgCtx->textboxXTarget = 34;
    msgCtx->textboxYTarget = 142;
    msgCtx->stateTimer = 8;
    Message_GrowTextbox(play);
    msgCtx->textboxX = 34;
    msgCtx->textboxY = 142;
    msgCtx->unk1200C = 0x200;
    msgCtx->unk1200E = 0x200;
    msgCtx->unk12008 = 0x100;
    msgCtx->unk1200A = 0x40;
    msgCtx->msgMode = MSGMODE_TEXT_CONTINUING;
    msgCtx->textboxColorAlphaCurrent = msgCtx->textboxColorAlphaTarget;
    msgCtx->textboxColorAlphaCurrent = msgCtx->textboxColorAlphaTarget;

    if (!noStop) {
        Interface_SetBButtonInterfaceDoAction(play, DO_ACTION_STOP);
        noStop = gSaveContext.hudVisibility;
        Interface_SetHudVisibility(HUD_VISIBILITY_B_ALT);
        gSaveContext.hudVisibility = noStop;
    }

    if ((ocarinaAction == OCARINA_ACTION_FREE_PLAY) || (ocarinaAction == OCARINA_ACTION_CHECK_NOTIME)) {
        msgCtx->msgMode = MSGMODE_OCARINA_STARTING;
        msgCtx->textBoxType = TEXTBOX_TYPE_E;
    } else if (ocarinaAction == OCARINA_ACTION_3A) {
        msgCtx->msgMode = MSGMODE_32;
    } else if (ocarinaAction == OCARINA_ACTION_37) {
        msgCtx->msgMode = MSGMODE_2F;
        msgCtx->textBoxType = TEXTBOX_TYPE_2;
    } else if (ocarinaAction == OCARINA_ACTION_SCARECROW_LONG_DEMONSTRATION) {
        AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_DEFAULT);
        AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_DEFAULT);
        msgCtx->ocarinaStaff = AudioOcarina_GetPlaybackStaff();
        msgCtx->ocarinaStaff->pos = 0;
        sOcarinaButtonIndexBufLen = 0;
        sOcarinaButtonIndexBufPos = sOcarinaButtonIndexBufLen;
        Message_ResetOcarinaButtonState(play);
        msgCtx->msgMode = MSGMODE_SCARECROW_LONG_DEMONSTRATION;
        AudioOcarina_SetPlaybackSong(OCARINA_SONG_SCARECROW_LONG + 1, 1);
        gSaveContext.hudVisibility = HUD_VISIBILITY_IDLE;
        Interface_SetHudVisibility(HUD_VISIBILITY_NONE);
    }

    for (j = 0, k = 0; j < 48; j++, k += FONT_CHAR_TEX_SIZE) {
        Font_LoadChar(play, 0x8140, k);
    };

    msgCtx->stateTimer = 3;
}

void Message_DisplayOcarinaStaff(PlayState* play, u16 ocarinaAction) {
    play->msgCtx.blockSunsSong = false;
    Message_DisplayOcarinaStaffImpl(play, ocarinaAction);
}

void Message_DisplayOcarinaStaffBlockSunsSong(PlayState* play, u16 ocarinaAction) {
    play->msgCtx.blockSunsSong = true;
    Message_DisplayOcarinaStaffImpl(play, ocarinaAction);
}

/**
 * @return u8 A value of the TextState enum representing the current state of the on-screen message
 */
u8 Message_GetState(MessageContext* msgCtx) {
    if (msgCtx->msgLength == 0) {
        return TEXT_STATE_NONE;
    }

    if (msgCtx->msgMode == MSGMODE_TEXT_DONE) {
        if (msgCtx->nextTextId != 0xFFFF) {
            return TEXT_STATE_NEXT;
        }

        if ((msgCtx->textboxEndType == TEXTBOX_ENDTYPE_TWO_CHOICE) ||
            (msgCtx->textboxEndType == TEXTBOX_ENDTYPE_THREE_CHOICE)) {
            return TEXT_STATE_CHOICE;
        }
        if ((msgCtx->textboxEndType == TEXTBOX_ENDTYPE_EVENT) || (msgCtx->textboxEndType == TEXTBOX_ENDTYPE_EVENT2) ||
            (msgCtx->textboxEndType == TEXTBOX_ENDTYPE_PERSISTENT)) {
            return TEXT_STATE_EVENT;
        }
        if (msgCtx->textboxEndType == TEXTBOX_ENDTYPE_PAUSE_MENU) {
            return TEXT_STATE_PAUSE_MENU;
        }
        if ((msgCtx->textboxEndType >= TEXTBOX_ENDTYPE_FADE_NORMAL) &&
            (msgCtx->textboxEndType <= TEXTBOX_ENDTYPE_FADE_STAGES_3)) {
            return TEXT_STATE_FADING;
        }
        if ((msgCtx->textboxEndType == TEXTBOX_ENDTYPE_INPUT_BANK) ||
            (msgCtx->textboxEndType == TEXTBOX_ENDTYPE_INPUT_DOGGY_RACETRACK_BET)) {
            return TEXT_STATE_INPUT_RUPEES;
        }
        if (msgCtx->textboxEndType == TEXTBOX_ENDTYPE_INPUT_BOMBER_CODE) {
            return TEXT_STATE_INPUT_BOMBER_CODE;
        }
        if (msgCtx->textboxEndType == TEXTBOX_ENDTYPE_INPUT_LOTTERY_CODE) {
            return TEXT_STATE_INPUT_LOTTERY_CODE;
        }
        if (msgCtx->textboxEndType == TEXTBOX_ENDTYPE_12) {
            return TEXT_STATE_18;
        }
        return TEXT_STATE_DONE;
    }

    if (msgCtx->msgMode == MSGMODE_TEXT_AWAIT_NEXT) {
        return TEXT_STATE_AWAITING_NEXT;
    }
    if (msgCtx->msgMode == MSGMODE_SONG_DEMONSTRATION_DONE) {
        return TEXT_STATE_SONG_DEMO_DONE;
    }
    if ((msgCtx->ocarinaMode == OCARINA_MODE_EVENT) || (msgCtx->msgMode == MSGMODE_37)) {
        return TEXT_STATE_8;
    }
    if (msgCtx->msgMode == MSGMODE_OCARINA_AWAIT_INPUT) {
        return TEXT_STATE_9;
    }
    if ((msgCtx->msgMode == MSGMODE_21) || (msgCtx->msgMode == MSGMODE_3A)) {
        return TEXT_STATE_11;
    }
    if (msgCtx->msgMode == MSGMODE_3D) {
        return TEXT_STATE_12;
    }
    if (msgCtx->msgMode == MSGMODE_40) {
        return TEXT_STATE_13;
    }
    if ((msgCtx->msgMode == MSGMODE_TEXT_CLOSING) && (msgCtx->stateTimer == 1) &&
        (msgCtx->bombersNotebookEventQueueCount == 0)) {
        return TEXT_STATE_CLOSING;
    }

    return TEXT_STATE_FADING;
}

void Message_DrawTextBox(PlayState* play, Gfx** gfxP) {
    MessageContext* msgCtx = &play->msgCtx;
    Gfx* gfx = *gfxP;

    gDPPipeSync(gfx++);

    if (((u32)msgCtx->textBoxType == TEXTBOX_TYPE_0) || (msgCtx->textBoxType == TEXTBOX_TYPE_2) ||
        (msgCtx->textBoxType == TEXTBOX_TYPE_9) || (msgCtx->textBoxType == TEXTBOX_TYPE_A)) {
        gDPSetRenderMode(gfx++, G_RM_CLD_SURF, G_RM_CLD_SURF2);
    } else if (msgCtx->textBoxType == TEXTBOX_TYPE_3) {
        gDPSetAlphaCompare(gfx++, G_AC_THRESHOLD);
        gDPSetRenderMode(gfx++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
    }

    gDPSetPrimColor(gfx++, 0, 0, msgCtx->textboxColorRed, msgCtx->textboxColorGreen, msgCtx->textboxColorBlue,
                    msgCtx->textboxColorAlphaCurrent);

    if (((u32)msgCtx->textBoxType == TEXTBOX_TYPE_0) || (msgCtx->textBoxType == TEXTBOX_TYPE_2) ||
        (msgCtx->textBoxType == TEXTBOX_TYPE_6) || (msgCtx->textBoxType == TEXTBOX_TYPE_8) ||
        (msgCtx->textBoxType == TEXTBOX_TYPE_9) || (msgCtx->textBoxType == TEXTBOX_TYPE_A)) {
        gDPLoadTextureBlock_4b(gfx++, msgCtx->textboxSegment[TEXTBOX_SEG_TYPE], G_IM_FMT_I, 128, 64, 0,
                               G_TX_MIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 7, G_TX_NOMASK, G_TX_NOLOD,
                               G_TX_NOLOD);
    } else {
        gDPPipeSync(gfx++);

        if (msgCtx->textBoxType == TEXTBOX_TYPE_3) {
            gDPSetEnvColor(gfx++, 0, 0, 0, 255);
        } else if (msgCtx->textBoxType == TEXTBOX_TYPE_D) {
            gDPSetEnvColor(gfx++, 20, 0, 10, 255);
        } else {
            gDPSetEnvColor(gfx++, 50, 20, 0, 255);
        }
        gDPLoadTextureBlock_4b(gfx++, msgCtx->textboxSegment[TEXTBOX_SEG_TYPE], G_IM_FMT_IA, 128, 64, 0,
                               G_TX_MIRROR | G_TX_WRAP, G_TX_MIRROR | G_TX_WRAP, 7, G_TX_NOMASK, G_TX_NOLOD,
                               G_TX_NOLOD);
    }

    if (msgCtx->textBoxType == TEXTBOX_TYPE_A) {
        gSPTextureRectangle(gfx++, msgCtx->textboxX << 2, (msgCtx->textboxY + 22) << 2,
                            (msgCtx->textboxX + msgCtx->unk12008) << 2, (msgCtx->textboxY + 54) << 2, G_TX_RENDERTILE,
                            0, 6, msgCtx->unk1200C << 1, 2 << 10);
    } else {
        gSPTextureRectangle(gfx++, msgCtx->textboxX << 2, (msgCtx->textboxY) << 2,
                            (msgCtx->textboxX + sTextboxWidth) << 2, (msgCtx->textboxY + sTextboxHeight) << 2,
                            G_TX_RENDERTILE, 0, 0, sTextboxTexWidth, sTextboxTexHeight);
    }

    // Draw treble clef
    if (msgCtx->textBoxType == TEXTBOX_TYPE_3) {
        gDPPipeSync(gfx++);
        gDPSetCombineLERP(gfx++, 1, 0, PRIMITIVE, 0, TEXEL0, 0, PRIMITIVE, 0, 1, 0, PRIMITIVE, 0, TEXEL0, 0, PRIMITIVE,
                          0);
        gDPSetPrimColor(gfx++, 0, 0, 255, 100, 0, 255);
        gDPLoadTextureBlock_4b(gfx++, gOcarinaTrebleClefTex, G_IM_FMT_I, 16, 32, 0, G_TX_MIRROR | G_TX_WRAP,
                               G_TX_MIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(gfx++, 78 << 2, 166 << 2, 94 << 2, 198 << 2, G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);
    }

    *gfxP = gfx++;
}

void Message_SetView(View* view) {
    SET_FULLSCREEN_VIEWPORT(view);
    View_ApplyOrthoToOverlay(view);
}

void func_80152CAC(PlayState* play) {
    MessageContext* msgCtx = &play->msgCtx;

    if (msgCtx->ocarinaAction == OCARINA_ACTION_DEMONSTRATE_SONATA) {
        AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_MONKEY);
    } else if (msgCtx->ocarinaAction == OCARINA_ACTION_DEMONSTRATE_HEALING) {
        AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_PIANO);
    } else if (msgCtx->ocarinaAction == OCARINA_ACTION_DEMONSTRATE_SARIAS) {
        AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_TATL);
    } else if (msgCtx->ocarinaAction == OCARINA_ACTION_DEMONSTRATE_EPONAS) {
        AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_FEMALE_VOICE);
    } else if (msgCtx->ocarinaAction == OCARINA_ACTION_DEMONSTRATE_STORMS) {
        AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_DEFAULT);
    } else if (msgCtx->ocarinaAction == OCARINA_ACTION_DEMONSTRATE_GORON_LULLABY) {
        AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_BABY_SINGING);
    } else if (msgCtx->ocarinaAction == OCARINA_ACTION_DEMONSTRATE_GORON_LULLABY_INTRO) {
        AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_ELDER_GORON_DRUMS);
    } else if (msgCtx->ocarinaAction == OCARINA_ACTION_DEMONSTRATE_OATH) {
        AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_TATL);
    } else if (msgCtx->ocarinaAction == OCARINA_ACTION_DEMONSTRATE_ELEGY) {
        AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_IKANA_KING);
    } else {
        AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_DEFAULT);
    }

    if ((msgCtx->ocarinaAction == OCARINA_ACTION_3B) || (msgCtx->ocarinaAction == OCARINA_ACTION_3C)) {
        AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_ELDER_GORON_DRUMS);
        AudioOcarina_SetPlaybackSong(OCARINA_SONG_GORON_LULLABY_INTRO + 1, 0x83);
        return;
    }
    if ((msgCtx->ocarinaAction == OCARINA_ACTION_DEMONSTRATE_EVAN_PART1_FIRST_HALF) ||
        (msgCtx->ocarinaAction == OCARINA_ACTION_DEMONSTRATE_EVAN_PART2_FIRST_HALF)) {
        AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_BASS_GUITAR);
        AudioOcarina_SetPlaybackSong(
            ((msgCtx->ocarinaAction - OCARINA_ACTION_DEMONSTRATE_EVAN_PART1_FIRST_HALF) + OCARINA_SONG_EVAN_PART1 + 1),
            0x84);
        return;
    }
    if ((msgCtx->ocarinaAction == OCARINA_ACTION_DEMONSTRATE_EVAN_PART1_SECOND_HALF) ||
        (msgCtx->ocarinaAction == OCARINA_ACTION_DEMONSTRATE_EVAN_PART2_SECOND_HALF)) {
        AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_ZORA_GUITAR);
        AudioOcarina_SetPlaybackSong(
            ((msgCtx->ocarinaAction - OCARINA_ACTION_DEMONSTRATE_EVAN_PART1_SECOND_HALF) + OCARINA_SONG_EVAN_PART1 + 1),
            0x44);
        return;
    }
    AudioOcarina_SetPlaybackSong((msgCtx->ocarinaAction - OCARINA_ACTION_FREE_PLAY), 2);
}

s16 sOcarinaEffectActorIds[] = {
    ACTOR_OCEFF_WIPE5, // Sonata of Awakening Effect
    ACTOR_OCEFF_WIPE5, // Sonata of Awakening Effect
    ACTOR_OCEFF_WIPE5, // Sonata of Awakening Effect
    ACTOR_OCEFF_WIPE5, // Sonata of Awakening Effect
    ACTOR_OCEFF_WIPE5, // Sonata of Awakening Effect
    ACTOR_OCEFF_WIPE5, // Sonata of Awakening Effect
    ACTOR_OCEFF_WIPE,  // Song of Time Effect
    ACTOR_OCEFF_WIPE7, // Song of Healing Effect
    ACTOR_OCEFF_WIPE2, // Epona's Song Effect
    ACTOR_OCEFF_WIPE6, // Song of Soaring Effect
    ACTOR_OCEFF_STORM, // Song of Storms Effect II [?]
    ACTOR_OCEFF_SPOT,  // Sun's Song Effect
    ACTOR_OCEFF_WIPE,  // Song of Time Effect
    ACTOR_OCEFF_WIPE,  // Song of Time Effect
    ACTOR_OCEFF_WIPE4  // Scarecrow's Song Effect
};
s32 sOcarinaEffectActorParams[] = { 0, 1, 2, 3, 4, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0 };

void Message_SpawnSongEffect(PlayState* play) {
    MessageContext* msgCtx = &play->msgCtx;
    Player* player = GET_PLAYER(play);

    //! FAKE:
    if (1) {}
    if ((msgCtx->songPlayed <= OCARINA_SONG_SCARECROW_SPAWN) &&
        (msgCtx->songPlayed != OCARINA_SONG_GORON_LULLABY_INTRO) &&
        !((msgCtx->ocarinaAction >= OCARINA_ACTION_PROMPT_WIND_FISH_HUMAN) &&
          (msgCtx->ocarinaAction <= OCARINA_ACTION_PROMPT_WIND_FISH_DEKU))) {
        msgCtx->ocarinaSongEffectActive = true;
        if (msgCtx->songPlayed != OCARINA_SONG_SCARECROW_SPAWN) {
            Actor_Spawn(&play->actorCtx, play, sOcarinaEffectActorIds[msgCtx->songPlayed], player->actor.world.pos.x,
                        player->actor.world.pos.y, player->actor.world.pos.z, 0, 0, 0,
                        sOcarinaEffectActorParams[msgCtx->songPlayed]);
        } else {
            Actor_Spawn(&play->actorCtx, play, ACTOR_OCEFF_WIPE4, player->actor.world.pos.x, player->actor.world.pos.y,
                        player->actor.world.pos.z, 0, 0, 0, 0);
        }
    }
}

Color_RGB16 sOcarinaButtonAPrimColors[] = {
    { 80, 150, 255 },
    { 100, 200, 255 },
};
Color_RGB16 sOcarinaButtonAEnvColors[] = {
    { 10, 10, 10 },
    { 50, 50, 255 },
};
Color_RGB16 sOcarinaButtonCPrimColors[] = {
    { 255, 255, 50 },
    { 255, 255, 180 },
};
Color_RGB16 sOcarinaButtonCEnvColors[] = {
    { 10, 10, 10 },
    { 110, 110, 50 },
};

void Message_FlashOcarinaButtons(void) {
    sOcarinaButtonStepR = ABS_ALT(sOcarinaButtonAPrimR - sOcarinaButtonAPrimColors[sOcarinaButtonFlashColorIndex].r) /
                          sOcarinaButtonFlashTimer;
    sOcarinaButtonStepG = ABS_ALT(sOcarinaButtonAPrimG - sOcarinaButtonAPrimColors[sOcarinaButtonFlashColorIndex].g) /
                          sOcarinaButtonFlashTimer;
    sOcarinaButtonStepB = ABS_ALT(sOcarinaButtonAPrimB - sOcarinaButtonAPrimColors[sOcarinaButtonFlashColorIndex].b) /
                          sOcarinaButtonFlashTimer;

    if (sOcarinaButtonAPrimR >= sOcarinaButtonAPrimColors[sOcarinaButtonFlashColorIndex].r) {
        sOcarinaButtonAPrimR -= sOcarinaButtonStepR;
    } else {
        sOcarinaButtonAPrimR += sOcarinaButtonStepR;
    }
    if (sOcarinaButtonAPrimG >= sOcarinaButtonAPrimColors[sOcarinaButtonFlashColorIndex].g) {
        sOcarinaButtonAPrimG -= sOcarinaButtonStepG;
    } else {
        sOcarinaButtonAPrimG += sOcarinaButtonStepG;
    }
    if (sOcarinaButtonAPrimB >= sOcarinaButtonAPrimColors[sOcarinaButtonFlashColorIndex].b) {
        sOcarinaButtonAPrimB -= sOcarinaButtonStepB;
    } else {
        sOcarinaButtonAPrimB += sOcarinaButtonStepB;
    }

    sOcarinaButtonStepR = ABS_ALT(sOcarinaButtonAEnvR - sOcarinaButtonAEnvColors[sOcarinaButtonFlashColorIndex].r) /
                          sOcarinaButtonFlashTimer;
    sOcarinaButtonStepG = ABS_ALT(sOcarinaButtonAEnvG - sOcarinaButtonAEnvColors[sOcarinaButtonFlashColorIndex].g) /
                          sOcarinaButtonFlashTimer;
    sOcarinaButtonStepB = ABS_ALT(sOcarinaButtonAEnvB - sOcarinaButtonAEnvColors[sOcarinaButtonFlashColorIndex].b) /
                          sOcarinaButtonFlashTimer;

    if (sOcarinaButtonCEnvR >= sOcarinaButtonAEnvColors[sOcarinaButtonFlashColorIndex].r) {
        sOcarinaButtonAEnvR -= sOcarinaButtonStepR;
    } else {
        sOcarinaButtonAEnvR += sOcarinaButtonStepR;
    }
    if (sOcarinaButtonCEnvG >= sOcarinaButtonAEnvColors[sOcarinaButtonFlashColorIndex].g) {
        sOcarinaButtonAEnvG -= sOcarinaButtonStepG;
    } else {
        sOcarinaButtonAEnvG += sOcarinaButtonStepG;
    }
    if (sOcarinaButtonCEnvB >= sOcarinaButtonAEnvColors[sOcarinaButtonFlashColorIndex].b) {
        sOcarinaButtonAEnvB -= sOcarinaButtonStepB;
    } else {
        sOcarinaButtonAEnvB += sOcarinaButtonStepB;
    }

    sOcarinaButtonStepR = ABS_ALT(sOcarinaButtonCPrimR - sOcarinaButtonCPrimColors[sOcarinaButtonFlashColorIndex].r) /
                          sOcarinaButtonFlashTimer;
    sOcarinaButtonStepG = ABS_ALT(sOcarinaButtonCPrimG - sOcarinaButtonCPrimColors[sOcarinaButtonFlashColorIndex].g) /
                          sOcarinaButtonFlashTimer;
    sOcarinaButtonStepB = ABS_ALT(sOcarinaButtonCPrimB - sOcarinaButtonCPrimColors[sOcarinaButtonFlashColorIndex].b) /
                          sOcarinaButtonFlashTimer;

    if (sOcarinaButtonCPrimR >= sOcarinaButtonCPrimColors[sOcarinaButtonFlashColorIndex].r) {
        sOcarinaButtonCPrimR -= sOcarinaButtonStepR;
    } else {
        sOcarinaButtonCPrimR += sOcarinaButtonStepR;
    }
    if (sOcarinaButtonCPrimG >= sOcarinaButtonCPrimColors[sOcarinaButtonFlashColorIndex].g) {
        sOcarinaButtonCPrimG -= sOcarinaButtonStepG;
    } else {
        sOcarinaButtonCPrimG += sOcarinaButtonStepG;
    }
    if (sOcarinaButtonCPrimB >= sOcarinaButtonCPrimColors[sOcarinaButtonFlashColorIndex].b) {
        sOcarinaButtonCPrimB -= sOcarinaButtonStepB;
    } else {
        sOcarinaButtonCPrimB += sOcarinaButtonStepB;
    }

    sOcarinaButtonStepR = ABS_ALT(sOcarinaButtonCEnvR - sOcarinaButtonCEnvColors[sOcarinaButtonFlashColorIndex].r) /
                          sOcarinaButtonFlashTimer;
    sOcarinaButtonStepG = ABS_ALT(sOcarinaButtonCEnvG - sOcarinaButtonCEnvColors[sOcarinaButtonFlashColorIndex].g) /
                          sOcarinaButtonFlashTimer;
    sOcarinaButtonStepB = ABS_ALT(sOcarinaButtonCEnvB - sOcarinaButtonCEnvColors[sOcarinaButtonFlashColorIndex].b) /
                          sOcarinaButtonFlashTimer;

    if (sOcarinaButtonCEnvR >= sOcarinaButtonCEnvColors[sOcarinaButtonFlashColorIndex].r) {
        sOcarinaButtonCEnvR -= sOcarinaButtonStepR;
    } else {
        sOcarinaButtonCEnvR += sOcarinaButtonStepR;
    }
    if (sOcarinaButtonCEnvG >= sOcarinaButtonCEnvColors[sOcarinaButtonFlashColorIndex].g) {
        sOcarinaButtonCEnvG -= sOcarinaButtonStepG;
    } else {
        sOcarinaButtonCEnvG += sOcarinaButtonStepG;
    }
    if (sOcarinaButtonCEnvB >= sOcarinaButtonCEnvColors[sOcarinaButtonFlashColorIndex].b) {
        sOcarinaButtonCEnvB -= sOcarinaButtonStepB;
    } else {
        sOcarinaButtonCEnvB += sOcarinaButtonStepB;
    }

    sOcarinaButtonFlashTimer--;
    if (sOcarinaButtonFlashTimer == 0) {
        sOcarinaButtonAPrimR = sOcarinaButtonAPrimColors[sOcarinaButtonFlashColorIndex].r;
        sOcarinaButtonAPrimG = sOcarinaButtonAPrimColors[sOcarinaButtonFlashColorIndex].g;
        sOcarinaButtonAPrimB = sOcarinaButtonAPrimColors[sOcarinaButtonFlashColorIndex].b;
        sOcarinaButtonAEnvR = sOcarinaButtonAEnvColors[sOcarinaButtonFlashColorIndex].r;
        sOcarinaButtonAEnvG = sOcarinaButtonAEnvColors[sOcarinaButtonFlashColorIndex].g;
        sOcarinaButtonAEnvB = sOcarinaButtonAEnvColors[sOcarinaButtonFlashColorIndex].b;
        sOcarinaButtonCPrimR = sOcarinaButtonCPrimColors[sOcarinaButtonFlashColorIndex].r;
        sOcarinaButtonCPrimG = sOcarinaButtonCPrimColors[sOcarinaButtonFlashColorIndex].g;
        sOcarinaButtonCPrimB = sOcarinaButtonCPrimColors[sOcarinaButtonFlashColorIndex].b;
        sOcarinaButtonCEnvR = sOcarinaButtonCEnvColors[sOcarinaButtonFlashColorIndex].r;
        sOcarinaButtonCEnvG = sOcarinaButtonCEnvColors[sOcarinaButtonFlashColorIndex].g;
        sOcarinaButtonCEnvB = sOcarinaButtonCEnvColors[sOcarinaButtonFlashColorIndex].b;
        sOcarinaButtonFlashTimer = 3;
        sOcarinaButtonFlashColorIndex ^= 1;
    }
}

TexturePtr sOcarinaButtonTextures[] = {
    gOcarinaATex, gOcarinaCDownTex, gOcarinaCRightTex, gOcarinaCLeftTex, gOcarinaCUpTex,
};

void Message_DrawOcarinaButtons(PlayState* play, Gfx** gfxP) {
    MessageContext* msgCtx = &play->msgCtx;
    Gfx* gfx = *gfxP;
    u16 i;
    u16 notePosX;

    if ((play->msgCtx.msgMode >= MSGMODE_OCARINA_PLAYING) && (msgCtx->msgMode <= MSGMODE_40)) {
        if ((msgCtx->ocarinaAction != OCARINA_ACTION_FREE_PLAY) &&
            (msgCtx->ocarinaAction != OCARINA_ACTION_CHECK_NOTIME)) {
            Gfx_SetupDL39_Ptr(&gfx);

            gDPSetCombineLERP(gfx++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0, PRIMITIVE,
                              ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);

            if ((msgCtx->msgMode == MSGMODE_SONG_PROMPT) || (msgCtx->msgMode == MSGMODE_36)) {
                if ((msgCtx->ocarinaAction != OCARINA_ACTION_PROMPT_EVAN_PART1_SECOND_HALF) &&
                    (msgCtx->ocarinaAction != OCARINA_ACTION_PROMPT_EVAN_PART2_SECOND_HALF)) {
                    if (msgCtx->ocarinaAction >= OCARINA_ACTION_TIMED_PROMPT_SONATA) {
                        sOcarinaButtonStepG = msgCtx->ocarinaAction - OCARINA_ACTION_TIMED_PROMPT_SONATA;
                    } else if ((msgCtx->ocarinaAction == OCARINA_ACTION_PROMPT_EVAN_PART1_SECOND_HALF) ||
                               (msgCtx->ocarinaAction == OCARINA_ACTION_PROMPT_EVAN_PART2_SECOND_HALF)) {
                        // Can never pass as the conditional is opposite to one scope higher
                        sOcarinaButtonStepG = (msgCtx->ocarinaAction - OCARINA_ACTION_PROMPT_EVAN_PART1_SECOND_HALF) +
                                              OCARINA_SONG_EVAN_PART1;
                    } else if ((msgCtx->ocarinaAction >= OCARINA_ACTION_PROMPT_WIND_FISH_HUMAN) &&
                               (msgCtx->ocarinaAction <= OCARINA_ACTION_PROMPT_WIND_FISH_DEKU)) {
                        sOcarinaButtonStepG = msgCtx->ocarinaAction - OCARINA_ACTION_PROMPT_WIND_FISH_HUMAN +
                                              OCARINA_SONG_WIND_FISH_HUMAN;
                    } else {
                        sOcarinaButtonStepG = msgCtx->ocarinaAction - OCARINA_ACTION_PROMPT_SONATA;
                    }
                    sOcarinaButtonStepR = gOcarinaSongButtons[sOcarinaButtonStepG].numButtons;

                    for (notePosX = 98, i = 0; i < sOcarinaButtonStepR; notePosX += 18, i++) {
                        gDPPipeSync(gfx++);
                        gDPSetPrimColor(gfx++, 0, 0, 150, 150, 150, 150);
                        gDPSetEnvColor(gfx++, 10, 10, 10, 0);

                        gDPLoadTextureBlock(
                            gfx++, sOcarinaButtonTextures[gOcarinaSongButtons[sOcarinaButtonStepG].buttonIndex[i]],
                            G_IM_FMT_IA, G_IM_SIZ_8b, 16, 16, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP,
                            G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

                        gSPTextureRectangle(
                            gfx++, notePosX << 2,
                            msgCtx->ocarinaButtonsPosY[gOcarinaSongButtons[sOcarinaButtonStepG].buttonIndex[i]] << 2,
                            (notePosX + 16) << 2,
                            (msgCtx->ocarinaButtonsPosY[gOcarinaSongButtons[sOcarinaButtonStepG].buttonIndex[i]] + 16)
                                << 2,
                            G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);
                    }
                }
            }

            if (msgCtx->msgMode != MSGMODE_SCARECROW_LONG_RECORDING_START) {
                for (notePosX = 98, i = 0; i < 8; notePosX += 18, i++) {
                    if (sOcarinaButtonIndexBuf[i] == OCARINA_BTN_INVALID) {
                        break;
                    }

                    if (sOcarinaButtonAlphaValues[i] != 255) {
                        sOcarinaButtonAlphaValues[i] += 50;
                        if (sOcarinaButtonAlphaValues[i] >= 255) {
                            sOcarinaButtonAlphaValues[i] = 255;
                        }
                    }

                    gDPPipeSync(gfx++);
                    if (sOcarinaButtonIndexBuf[i] == OCARINA_BTN_A) {
                        gDPSetPrimColor(gfx++, 0, 0, sOcarinaButtonAPrimR, sOcarinaButtonAPrimG, sOcarinaButtonAPrimB,
                                        sOcarinaButtonAlphaValues[i]);
                        gDPSetEnvColor(gfx++, sOcarinaButtonAEnvR, sOcarinaButtonAEnvG, sOcarinaButtonAEnvB, 0);
                    } else {
                        gDPSetPrimColor(gfx++, 0, 0, sOcarinaButtonCPrimR, sOcarinaButtonCPrimG, sOcarinaButtonCPrimB,
                                        sOcarinaButtonAlphaValues[i]);
                        gDPSetEnvColor(gfx++, sOcarinaButtonCEnvR, sOcarinaButtonCEnvG, sOcarinaButtonCEnvB, 0);
                    }

                    gDPLoadTextureBlock(gfx++, sOcarinaButtonTextures[sOcarinaButtonIndexBuf[i]], G_IM_FMT_IA,
                                        G_IM_SIZ_8b, 16, 16, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP,
                                        G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

                    gSPTextureRectangle(
                        gfx++, notePosX << 2, msgCtx->ocarinaButtonsPosY[sOcarinaButtonIndexBuf[i]] << 2,
                        (notePosX + 16) << 2, (msgCtx->ocarinaButtonsPosY[sOcarinaButtonIndexBuf[i]] + 16) << 2,
                        G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);
                }
            }
        }
    }
    *gfxP = gfx;
}

void Message_DrawText(PlayState* play, Gfx** gfxP) {
    if (!GameInteractor_Should(VB_DRAW_OCARINA_STAFF, true)) {
        return;
    }

    if ((gSaveContext.options.language == LANGUAGE_JPN) && !play->msgCtx.textIsCredits) {
        Message_DrawTextDefault(play, gfxP);
    } else if (play->msgCtx.textIsCredits) {
        Message_DrawTextCredits(play, gfxP);
    } else {
        Message_DrawTextNES(play, gfxP, 0);
    }
}

void Message_DrawSceneTitleCard(PlayState* play, Gfx** gfxP) {
    MessageContext* msgCtx = &play->msgCtx;
    Gfx* gfx;

    gfx = *gfxP;
    Gfx_SetupDL39_Ptr(&gfx);

    gDPSetCombineLERP(gfx++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0, PRIMITIVE,
                      ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
    gDPSetTextureFilter(gfx++, G_TF_BILERP);
    gDPSetAlphaDither(gfx++, G_AD_NOTPATTERN);
    gDPSetPrimColor(gfx++, 0, 0, 0, 0, 0, msgCtx->textboxColorAlphaCurrent);
    gDPSetEnvColorOverride(gfx++, 140, 40, 160, 255, COSMETIC_ID("Menus.SceneTitleCard"));
    gDPLoadTextureBlock(gfx++, gSceneTitleCardGradientTex, G_IM_FMT_I, G_IM_SIZ_8b, 64, 1, 0, G_TX_NOMIRROR | G_TX_WRAP,
                        G_TX_NOMIRROR | G_TX_WRAP, 6, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    // #region 2S2H [Cosmetic] Account for different aspect ratios than 4:3 for title card
    // gSPTextureRectangle(gfx++, 0, XREG(77) << 2, 320 << 2, (XREG(77) + XREG(76)) << 2, G_TX_RENDERTILE, 0, 0, 204,
    //                     1 << 10);
    f32 tileOffset = 204.0f / ((320.0f + (OTRGetDimensionFromRightEdge(0) * 2.0f)) / 320.0f);
    gSPWideTextureRectangle(gfx++, OTRGetRectDimensionFromLeftEdge(0) << 2, XREG(77) << 2,
                            OTRGetRectDimensionFromRightEdge(320) << 2, (XREG(77) + XREG(76)) << 2, G_TX_RENDERTILE, 8,
                            0, (u32)tileOffset - 1, 1 << 10);
    // #endregion
    gDPPipeSync(gfx++);
    gDPSetCombineLERP(gfx++, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0);
    gDPSetEnvColor(gfx++, 0, 0, 0, 255);

    if ((msgCtx->currentTextId < 0x1BB2) || (msgCtx->currentTextId >= 0x1BB7)) {
        msgCtx->unk11FF8 = XREG(75);
    }
    msgCtx->unk11FFA = XREG(74);
    Message_DrawTextNES(play, &gfx, 0);
    *gfxP = gfx++;
}

s16 sOcarinaSongFanfares[] = {
    NA_BGM_OCARINA_SONATA,            // OCARINA_SONG_SONATA
    NA_BGM_OCARINA_LULLABY,           // OCARINA_SONG_GORON_LULLABY
    NA_BGM_OCARINA_NEW_WAVE,          // OCARINA_SONG_NEW_WAVE
    NA_BGM_OCARINA_ELEGY,             // OCARINA_SONG_ELEGY
    NA_BGM_OCARINA_OATH,              // OCARINA_SONG_OATH
    NA_BGM_MAJORAS_LAIR,              // OCARINA_SONG_SARIAS
    NA_BGM_OCARINA_TIME,              // OCARINA_SONG_TIME
    NA_BGM_OCARINA_HEALING,           // OCARINA_SONG_HEALING
    NA_BGM_OCARINA_EPONA,             // OCARINA_SONG_EPONAS
    NA_BGM_OCARINA_SOARING,           // OCARINA_SONG_SOARING
    NA_BGM_OCARINA_STORM,             // OCARINA_SONG_STORMS
    NA_BGM_OCARINA_SUNS,              // OCARINA_SONG_SUNS
    NA_BGM_INVERTED_SONG_OF_TIME,     // OCARINA_SONG_INVERTED_TIME
    NA_BGM_SONG_OF_DOUBLE_TIME,       // OCARINA_SONG_DOUBLE_TIME
    NA_BGM_OCARINA_LULLABY_INTRO_PTR, // OCARINA_SONG_GORON_LULLABY_INTRO
    NA_BGM_OCARINA_LULLABY_INTRO_PTR, // OCARINA_SONG_WIND_FISH_HUMAN
    NA_BGM_OCARINA_LULLABY_INTRO_PTR, // OCARINA_SONG_WIND_FISH_GORON
};

s8 sOcarinaSongFanfareIoData[PLAYER_FORM_MAX] = {
    0x35, // PLAYER_FORM_FIERCE_DEITY
    0x5C, // PLAYER_FORM_GORON
    0x5D, // PLAYER_FORM_ZORA
    0x5E, // PLAYER_FORM_DEKU
    0x35, // PLAYER_FORM_HUMAN
};

u8 sPlayerFormOcarinaInstruments[] = {
    OCARINA_INSTRUMENT_DEFAULT,
    OCARINA_INSTRUMENT_GORON_DRUMS,
    OCARINA_INSTRUMENT_ZORA_GUITAR,
    OCARINA_INSTRUMENT_DEKU_PIPES,
};

s16 D_801D03A8[TEXTBOX_TYPE_MAX] = {
    0x3B, // TEXTBOX_TYPE_0
    0x3B, // TEXTBOX_TYPE_1
    0x3B, // TEXTBOX_TYPE_2
    0x3B, // TEXTBOX_TYPE_3
    0x22, // TEXTBOX_TYPE_4
    0x3B, // TEXTBOX_TYPE_5
    0x3B, // TEXTBOX_TYPE_6
    0x3B, // TEXTBOX_TYPE_7
    0x3B, // TEXTBOX_TYPE_8
    0x3B, // TEXTBOX_TYPE_9
    0x3B, // TEXTBOX_TYPE_A
    0x3B, // TEXTBOX_TYPE_B
    0x3B, // TEXTBOX_TYPE_C
    0x3B, // TEXTBOX_TYPE_D
    0x3B, // TEXTBOX_TYPE_E
    0x3B, // TEXTBOX_TYPE_F
};

void Message_DrawMain(PlayState* play, Gfx** gfxP) {
    s32 pad;
    MessageContext* msgCtx = &play->msgCtx;
    Gfx* gfx;
    u16 i;
    u16 buttonIndexPos;
    u8 ocarinaError;
    s32 j;
    s16 temp_v0_33;
    s16 temp;

    gfx = *gfxP;

    gSPSegment(gfx++, 0x02, play->interfaceCtx.parameterSegment);
    gSPSegment(gfx++, 0x07, msgCtx->textboxSegment[TEXTBOX_SEG_TYPE]);

    if (msgCtx->msgLength != 0) {
        if (!msgCtx->textIsCredits) {
            Message_SetView(&msgCtx->view);
            Gfx_SetupDL39_Ptr(&gfx);
            if (msgCtx->ocarinaAction != OCARINA_ACTION_37) {
                if ((msgCtx->msgMode != MSGMODE_18) && (msgCtx->msgMode != MSGMODE_39) &&
                    (msgCtx->msgMode != MSGMODE_3C) && (msgCtx->msgMode != MSGMODE_3F) &&
                    (msgCtx->msgMode != MSGMODE_3A) && (msgCtx->msgMode != MSGMODE_3D) &&
                    (msgCtx->msgMode != MSGMODE_40) &&
                    (((msgCtx->msgMode >= MSGMODE_TEXT_BOX_GROWING) && (msgCtx->msgMode <= MSGMODE_TEXT_DONE)) ||
                     ((msgCtx->msgMode >= MSGMODE_NEW_CYCLE_0) && (msgCtx->msgMode <= MSGMODE_OWL_SAVE_2))) &&
                    (D_801CFC78[msgCtx->textBoxType] != 0xE)) {
                    if (GameInteractor_Should(VB_DRAW_OCARINA_STAFF, true)) {
                        Message_DrawTextBox(play, &gfx);
                    }
                }
            }
        }
        Gfx_SetupDL39_Ptr(&gfx);
        gDPSetAlphaCompare(gfx++, G_AC_NONE);
        gDPSetCombineLERP(gfx++, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE,
                          0);

        if (YREG(0) != msgCtx->msgMode) {
            YREG(0) = msgCtx->msgMode;
            YREG(1) = msgCtx->ocarinaAction;
        }

        switch (msgCtx->msgMode) {
            case MSGMODE_TEXT_START:
            case MSGMODE_TEXT_BOX_GROWING:
            case MSGMODE_TEXT_STARTING:
            case MSGMODE_TEXT_NEXT_MSG:
                break;

            case MSGMODE_TEXT_CONTINUING:
                if (msgCtx->stateTimer == 1) {
                    for (i = 0, j = 0; i < 48; i++, j += FONT_CHAR_TEX_SIZE) {
                        Font_LoadChar(play, 0x8140, j);
                    }
                    Message_DrawText(play, &gfx);
                } else {
                    gDPPipeSync(gfx++);
                    gDPSetRenderMode(gfx++, G_RM_CLD_SURF, G_RM_CLD_SURF2);
                    gDPSetPrimColor(gfx++, 0, 0, 0, 0, 0, 0);
                    gDPSetEnvColor(gfx++, 0, 0, 0, 255);
                }
                break;

            case MSGMODE_TEXT_DISPLAYING:
            case MSGMODE_TEXT_DELAYED_BREAK:
            case MSGMODE_9:
                if ((gSaveContext.options.language == LANGUAGE_JPN) && !msgCtx->textIsCredits) {
                    if (msgCtx->textDelay != 0) {
                        msgCtx->textDrawPos += msgCtx->textDelay;
                    }
                    Message_DrawTextNES(play, &gfx, 0);
                    if (msgCtx->msgMode == MSGMODE_TEXT_DISPLAYING) {
                        Message_DrawTextNES(play, &gfx, (s32)msgCtx->textDrawPos);
                    }
                } else if (msgCtx->textIsCredits) {
                    Message_DrawTextCredits(play, &gfx);
                } else {
                    if (msgCtx->textDelay != 0) {
                        msgCtx->textDrawPos += msgCtx->textDelay;
                    }
                    Message_DrawTextNES(play, &gfx, 0);
                    if (msgCtx->msgMode == MSGMODE_TEXT_DISPLAYING) {
                        Message_DrawTextNES(play, &gfx, (s32)msgCtx->textDrawPos);
                    }
                }
                break;

            case MSGMODE_NEW_CYCLE_0:
            case MSGMODE_NEW_CYCLE_1:
            case MSGMODE_NEW_CYCLE_2:
            case MSGMODE_OWL_SAVE_0:
            case MSGMODE_OWL_SAVE_1:
            case MSGMODE_OWL_SAVE_2:
                Message_DrawTextNES(play, &gfx, 0);
                break;

            case MSGMODE_TEXT_AWAIT_INPUT:
            case MSGMODE_TEXT_AWAIT_NEXT:
                Message_DrawText(play, &gfx);
                if (GameInteractor_Should(VB_DRAW_OCARINA_STAFF, true)) {
                    Message_DrawTextboxIcon(play, &gfx, 158,
                                            (s16)(D_801D03A8[msgCtx->textBoxType] + msgCtx->textboxYTarget));
                }
                break;

            case MSGMODE_OCARINA_STARTING:
            case MSGMODE_SONG_DEMONSTRATION_STARTING:
            case MSGMODE_SONG_PROMPT_STARTING:
            case MSGMODE_32:
                AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_DEFAULT);
                msgCtx->ocarinaStaff = AudioOcarina_GetPlayingStaff();
                play->msgCtx.ocarinaMode = OCARINA_MODE_ACTIVE;
                if ((msgCtx->ocarinaAction != OCARINA_ACTION_PROMPT_EVAN_PART1_SECOND_HALF) &&
                    (msgCtx->ocarinaAction != OCARINA_ACTION_PROMPT_EVAN_PART2_SECOND_HALF)) {
                    msgCtx->ocarinaStaff->pos = 0;
                    sOcarinaButtonIndexBufPos = 0;
                    Message_ResetOcarinaButtonState(play);
                }
                sOcarinaButtonFlashColorIndex = 1;
                sOcarinaButtonFlashTimer = 3;
                if (msgCtx->msgMode == MSGMODE_OCARINA_STARTING) {
                    if ((msgCtx->ocarinaAction == OCARINA_ACTION_0) ||
                        (msgCtx->ocarinaAction == OCARINA_ACTION_FREE_PLAY) ||
                        (msgCtx->ocarinaAction == OCARINA_ACTION_SCARECROW_SPAWN_RECORDING) ||
                        (msgCtx->ocarinaAction == OCARINA_ACTION_CHECK_NOTIME) ||
                        (msgCtx->ocarinaAction >= OCARINA_ACTION_CHECK_TIME)) {
                        if ((msgCtx->ocarinaAction == OCARINA_ACTION_FREE_PLAY) ||
                            (msgCtx->ocarinaAction == OCARINA_ACTION_CHECK_NOTIME)) {
                            if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_52_10)) {
                                AudioOcarina_StartDefault(msgCtx->ocarinaAvailableSongs | 0xC0000000);
                            } else if (CHECK_EVENTINF(EVENTINF_31)) {
                                AudioOcarina_SetInstrument(sPlayerFormOcarinaInstruments[CUR_FORM]);
                                AudioOcarina_StartDefault(0x80800000);
                            } else {
                                AudioOcarina_StartAtSongStartingPos((msgCtx->ocarinaAvailableSongs + 0x80000) |
                                                                    0xC0000000);
                                AudioOcarina_SetInstrument(sPlayerFormOcarinaInstruments[CUR_FORM]);
                            }
                        } else {
                            AudioOcarina_StartDefault(msgCtx->ocarinaAvailableSongs);
                        }
                    } else {
                        AudioOcarina_StartDefault((1 << msgCtx->ocarinaAction) | 0x80000000);
                    }
                    msgCtx->msgMode = MSGMODE_OCARINA_PLAYING;
                    if (CHECK_EVENTINF(EVENTINF_24)) {
                        AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_DEKU_TRUMPET);
                    } else if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_41_20)) {
                        AudioOcarina_SetInstrument(sPlayerFormOcarinaInstruments[CUR_FORM]);
                        //! FAKE:
                        (void)CUR_FORM;
                        if (gSaveContext.save.playerForm == 4) {}
                    } else {
                        AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_AMPLIFIED_GUITAR);
                    }
                } else if (msgCtx->msgMode == MSGMODE_SONG_DEMONSTRATION_STARTING) {
                    msgCtx->stateTimer = 20;
                    msgCtx->msgMode = MSGMODE_19;
                } else if (msgCtx->msgMode == MSGMODE_32) {
                    AudioOcarina_SetInstrument(sPlayerFormOcarinaInstruments[CUR_FORM]);
                    AudioOcarina_StartDefault(0x80800000);
                    msgCtx->msgMode = MSGMODE_33;
                } else { // MSGMODE_SONG_PROMPT_STARTING
                    if (CHECK_EVENTINF(EVENTINF_24)) {
                        AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_DEKU_TRUMPET);
                    } else {
                        AudioOcarina_SetInstrument(sPlayerFormOcarinaInstruments[CUR_FORM]);
                    }

                    if ((msgCtx->ocarinaAction == OCARINA_ACTION_PROMPT_EVAN_PART1_SECOND_HALF) ||
                        (msgCtx->ocarinaAction == OCARINA_ACTION_PROMPT_EVAN_PART2_SECOND_HALF)) {
                        AudioOcarina_StartForSongCheck(
                            (1 << ((msgCtx->ocarinaAction - OCARINA_ACTION_PROMPT_EVAN_PART1_SECOND_HALF) +
                                   OCARINA_SONG_EVAN_PART1)) |
                                0x80000000,
                            4);
                        msgCtx->msgMode = MSGMODE_SONG_PROMPT;
                    } else {
                        if ((msgCtx->ocarinaAction >= OCARINA_ACTION_PROMPT_WIND_FISH_HUMAN) &&
                            (msgCtx->ocarinaAction <= OCARINA_ACTION_PROMPT_WIND_FISH_DEKU)) {
                            AudioOcarina_StartDefault(
                                (1 << ((msgCtx->ocarinaAction - OCARINA_ACTION_PROMPT_WIND_FISH_HUMAN) +
                                       OCARINA_SONG_WIND_FISH_HUMAN)) |
                                0x80000000);
                        } else {
                            AudioOcarina_StartDefault(
                                (1 << ((msgCtx->ocarinaAction - OCARINA_ACTION_PROMPT_SONATA) + OCARINA_SONG_SONATA)) |
                                0x80000000);
                        }
                        msgCtx->msgMode = MSGMODE_SONG_PROMPT;
                    }
                }

                if ((msgCtx->ocarinaAction != OCARINA_ACTION_FREE_PLAY) &&
                    (msgCtx->ocarinaAction != OCARINA_ACTION_CHECK_NOTIME)) {
                    Message_DrawText(play, &gfx);
                }
                break;

            case MSGMODE_OCARINA_PLAYING:
                msgCtx->ocarinaStaff = AudioOcarina_GetPlayingStaff();

                if ((u32)msgCtx->ocarinaStaff->pos != 0) {
                    if ((msgCtx->ocarinaStaff->pos == 1) && (sOcarinaButtonIndexBufPos == 8)) {
                        sOcarinaButtonIndexBufPos = 0;
                    }

                    if (sOcarinaButtonIndexBufPos == (msgCtx->ocarinaStaff->pos - 1)) {
                        msgCtx->ocarinaButtonIndex = sOcarinaButtonIndexBuf[msgCtx->ocarinaStaff->pos - 1] =
                            msgCtx->ocarinaStaff->buttonIndex;
                        sOcarinaButtonIndexBuf[msgCtx->ocarinaStaff->pos] = OCARINA_BTN_INVALID;
                        sOcarinaButtonIndexBufPos++;
                    }
                }

                msgCtx->songPlayed = msgCtx->ocarinaStaff->state;

                GameInteractor_Should(VB_OVERRIDE_OCARINA_STAFF_STATE, false, msgCtx->ocarinaStaff);

                bool vanillaOwnedSongCheck = (msgCtx->ocarinaStaff->state == OCARINA_SONG_SCARECROW_SPAWN) ||
                                             (msgCtx->ocarinaStaff->state == OCARINA_SONG_INVERTED_TIME) ||
                                             (msgCtx->ocarinaStaff->state == OCARINA_SONG_DOUBLE_TIME) ||
                                             (msgCtx->ocarinaStaff->state == OCARINA_SONG_GORON_LULLABY_INTRO);
                // 0xFE means the staff is up but nothing has been played
                // 0xFF means no staff is up
                //! @bug states 0xFE and 0xFF will index CHECK_QUEST_ITEM Out of bounds, causing ASAN to crash.
                // 2S2H [Port] Fix this OOB with a check.
                if (msgCtx->ocarinaStaff->state != 0xFE && msgCtx->ocarinaStaff->state != 0xFF) {
                    vanillaOwnedSongCheck =
                        vanillaOwnedSongCheck || CHECK_QUEST_ITEM(QUEST_SONG_SONATA + msgCtx->ocarinaStaff->state);
                }

                if (msgCtx->ocarinaStaff->state <= OCARINA_SONG_SCARECROW_SPAWN) {
                    if (msgCtx->ocarinaStaff->state == OCARINA_SONG_EVAN_PART1) {
                        AudioOcarina_ResetAndReadInput();
                        AudioOcarina_StartDefault(0x80100000);
                    } else if (msgCtx->ocarinaStaff->state == OCARINA_SONG_EVAN_PART2) {
                        Audio_PlaySfx(NA_SE_SY_CORRECT_CHIME);
                        AudioOcarina_SetOcarinaDisableTimer(0, 20);
                        Message_CloseTextbox(play);
                        play->msgCtx.ocarinaMode = OCARINA_MODE_PLAYED_FULL_EVAN_SONG;
                    } else if (GameInteractor_Should(VB_SONG_AVAILABLE_TO_PLAY, vanillaOwnedSongCheck,
                                                     &msgCtx->ocarinaStaff->state)) {
                        sLastPlayedSong = msgCtx->ocarinaStaff->state;
                        msgCtx->lastPlayedSong = msgCtx->ocarinaStaff->state;
                        msgCtx->songPlayed = msgCtx->ocarinaStaff->state;
                        msgCtx->msgMode = MSGMODE_E;
                        msgCtx->stateTimer = 20;

                        if (msgCtx->ocarinaAction == OCARINA_ACTION_CHECK_NOTIME) {
                            if ((msgCtx->ocarinaStaff->state <= OCARINA_SONG_SARIAS) ||
                                (msgCtx->ocarinaStaff->state == OCARINA_SONG_SCARECROW_SPAWN)) {
                                AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_OFF);
                                Audio_PlaySfx(NA_SE_SY_OCARINA_ERROR);
                                msgCtx->msgMode = MSGMODE_OCARINA_STARTING;
                            } else {
                                Message_ContinueTextbox(play, 0x1B5B);
                                msgCtx->msgMode = MSGMODE_SONG_PLAYED;
                                msgCtx->textBoxType = TEXTBOX_TYPE_3;
                                msgCtx->stateTimer = 10;
                                Audio_PlaySfx(NA_SE_SY_TRE_BOX_APPEAR);
                                Interface_SetHudVisibility(HUD_VISIBILITY_NONE);
                            }
                        } else if (msgCtx->ocarinaAction == OCARINA_ACTION_CHECK_SCARECROW_SPAWN) {
                            if (msgCtx->ocarinaStaff->state <= OCARINA_SONG_STORMS) {
                                AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_OFF);
                                Audio_PlaySfx(NA_SE_SY_OCARINA_ERROR);
                                msgCtx->stateTimer = 10;
                                msgCtx->msgMode = MSGMODE_OCARINA_FAIL;
                            } else {
                                Message_ContinueTextbox(play, 0x1B5B);
                                msgCtx->msgMode = MSGMODE_SONG_PLAYED;
                                msgCtx->textBoxType = TEXTBOX_TYPE_3;
                                msgCtx->stateTimer = 10;
                                Audio_PlaySfx(NA_SE_SY_TRE_BOX_APPEAR);
                                Interface_SetHudVisibility(HUD_VISIBILITY_NONE);
                            }
                        } else if (msgCtx->ocarinaAction == OCARINA_ACTION_FREE_PLAY) {
                            Message_ContinueTextbox(play, 0x1B5B);
                            msgCtx->msgMode = MSGMODE_SONG_PLAYED;
                            msgCtx->textBoxType = TEXTBOX_TYPE_3;
                            msgCtx->stateTimer = 10;
                            Audio_PlaySfx(NA_SE_SY_TRE_BOX_APPEAR);
                        } else {
                            Audio_PlaySfx(NA_SE_SY_TRE_BOX_APPEAR);
                        }
                        Interface_SetHudVisibility(HUD_VISIBILITY_NONE);
                    } else {
                        AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_OFF);
                        Audio_PlaySfx(NA_SE_SY_OCARINA_ERROR);
                        msgCtx->msgMode = MSGMODE_OCARINA_STARTING;
                    }
                } else if (msgCtx->ocarinaStaff->state == OCARINA_SONG_TERMINA_WALL) {
                    Message_CloseTextbox(play);
                    play->msgCtx.ocarinaMode = OCARINA_MODE_END;
                    SET_EVENTINF(EVENTINF_32);
                    Audio_PlaySfx(NA_SE_SY_CORRECT_CHIME);
                    AudioOcarina_SetOcarinaDisableTimer(0, 20);
                } else if (msgCtx->ocarinaStaff->state == 0xFF) {
                    if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_52_10)) {
                        AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_OFF);
                        Audio_PlaySfx(NA_SE_SY_OCARINA_ERROR);
                        msgCtx->stateTimer = 10;
                        msgCtx->msgMode = MSGMODE_OCARINA_FAIL;
                    } else {
                        AudioOcarina_SetSongStartingPos();
                        AudioOcarina_SetInstrument(sPlayerFormOcarinaInstruments[CUR_FORM]);
                        AudioOcarina_StartAtSongStartingPos((msgCtx->ocarinaAvailableSongs + 0x80000) | 0xC0000000);
                    }
                }
                if ((msgCtx->ocarinaAction != OCARINA_ACTION_FREE_PLAY) &&
                    (msgCtx->ocarinaAction != OCARINA_ACTION_CHECK_NOTIME)) {
                    Message_DrawText(play, &gfx);
                }
                break;

            case MSGMODE_E:
            case MSGMODE_SONG_PROMPT_SUCCESS:
            case MSGMODE_22:
            case MSGMODE_SCARECROW_SPAWN_RECORDING_DONE:
            case MSGMODE_34:
                Message_FlashOcarinaButtons();
                msgCtx->stateTimer--;
                if (msgCtx->stateTimer == 0) {
                    AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_OFF);

                    if (msgCtx->msgMode == MSGMODE_E) {
                        Message_ContinueTextbox(play, 0x1B5B);
                        msgCtx->msgMode = MSGMODE_SONG_PLAYED;
                        msgCtx->textBoxType = TEXTBOX_TYPE_3;
                        msgCtx->stateTimer = 1;
                    } else if (msgCtx->msgMode == MSGMODE_SONG_PROMPT_SUCCESS) {
                        Message_ContinueTextbox(play, 0x1B5B);
                        msgCtx->msgMode = MSGMODE_SONG_PLAYED;
                        msgCtx->textBoxType = TEXTBOX_TYPE_3;
                        msgCtx->stateTimer = 1;
                    } else if (msgCtx->msgMode == MSGMODE_22) {
                        msgCtx->msgMode = MSGMODE_23;
                        play->msgCtx.ocarinaMode = OCARINA_MODE_EVENT;
                        msgCtx->textBoxType = TEXTBOX_TYPE_0;
                    } else if (msgCtx->msgMode == MSGMODE_34) {
                        if (msgCtx->songPlayed == OCARINA_SONG_TERMINA_WALL) {
                            Message_CloseTextbox(play);
                            play->msgCtx.ocarinaMode = OCARINA_MODE_EVENT;
                            Audio_PlaySfx(NA_SE_SY_CORRECT_CHIME);
                        } else {
                            Message_CloseTextbox(play);
                            play->msgCtx.ocarinaMode = OCARINA_MODE_END;
                        }
                    } else {
                        Message_CloseTextbox(play);
                        play->msgCtx.ocarinaMode = OCARINA_MODE_EVENT;
                    }
                }
                Message_DrawText(play, &gfx);
                break;

            case MSGMODE_OCARINA_FAIL:
            case MSGMODE_SONG_PROMPT_FAIL:
                Message_DrawText(play, &gfx);
                // fallthrough
            case MSGMODE_OCARINA_FAIL_NO_TEXT:
                msgCtx->stateTimer--;
                if (msgCtx->stateTimer == 0) {
                    D_801C6A94 = 1;
                    if (msgCtx->msgMode == MSGMODE_SONG_PROMPT_FAIL) {
                        Message_ContinueTextbox(play, 0x1B89);
                        Message_Decode(play);
                        msgCtx->msgMode = MSGMODE_SONG_PROMPT_NOTES_DROP;
                    } else {
                        msgCtx->msgMode = MSGMODE_OCARINA_NOTES_DROP;
                    }
                }
                break;

            case MSGMODE_OCARINA_NOTES_DROP:
            case MSGMODE_SONG_PROMPT_NOTES_DROP:
                for (i = 0; i < 5; i++) {
                    msgCtx->ocarinaButtonsPosY[i] += D_801C6A94;
                }
                D_801C6A94 += D_801C6A94;
                if (D_801C6A94 >= 0x226) {
                    Message_ResetOcarinaButtonAlphas();
                    if (msgCtx->msgMode == MSGMODE_SONG_PROMPT_NOTES_DROP) {
                        msgCtx->msgMode = MSGMODE_OCARINA_AWAIT_INPUT;
                        msgCtx->stateTimer = 10;
                    } else {
                        msgCtx->msgMode = MSGMODE_OCARINA_STARTING;
                    }
                    AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_OFF);
                }
                break;

            case MSGMODE_SONG_PLAYED:
                msgCtx->stateTimer--;
                if (msgCtx->stateTimer == 0) {
                    AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_OFF);
                    Message_Decode(play);
                    msgCtx->msgMode = MSGMODE_SETUP_DISPLAY_SONG_PLAYED;
                    msgCtx->ocarinaStaff = AudioOcarina_GetPlayingStaff();
                    msgCtx->ocarinaStaff->pos = 0;
                    sOcarinaButtonIndexBufPos = 0;
                    Message_ResetOcarinaButtonState(play);
                    Message_SpawnSongEffect(play);
                }
                break;

            case MSGMODE_SETUP_DISPLAY_SONG_PLAYED:
                Message_DrawText(play, &gfx);
                AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_DEFAULT);

                if ((msgCtx->ocarinaAction >= OCARINA_ACTION_PROMPT_WIND_FISH_HUMAN) &&
                    (msgCtx->ocarinaAction <= OCARINA_ACTION_PROMPT_WIND_FISH_DEKU)) {
                    AudioOcarina_SetInstrument(sPlayerFormOcarinaInstruments[CUR_FORM]);
                    AudioOcarina_SetPlaybackSong(msgCtx->ocarinaAction - OCARINA_ACTION_SCARECROW_LONG_RECORDING, 1);
                } else {
                    AudioOcarina_SetInstrument(sPlayerFormOcarinaInstruments[CUR_FORM]);
                    AudioOcarina_SetPlaybackSong((u8)msgCtx->songPlayed + 1, 1);
                    if (msgCtx->songPlayed != OCARINA_SONG_SCARECROW_SPAWN) {
                        Audio_PlayFanfareWithPlayerIOPort7((u16)sOcarinaSongFanfares[msgCtx->songPlayed],
                                                           (u8)sOcarinaSongFanfareIoData[CUR_FORM]);
                        AudioSfx_MuteBanks(0x20);
                    }
                }
                play->msgCtx.ocarinaMode = OCARINA_MODE_ACTIVE;
                if (msgCtx->ocarinaAction == OCARINA_ACTION_FREE_PLAY) {
                    msgCtx->ocarinaAction = OCARINA_ACTION_FREE_PLAY_DONE;
                }
                if (msgCtx->ocarinaAction == OCARINA_ACTION_CHECK_NOTIME) {
                    msgCtx->ocarinaAction = OCARINA_ACTION_CHECK_NOTIME_DONE;
                }
                sOcarinaButtonIndexBufPos = 0;
                msgCtx->msgMode = MSGMODE_DISPLAY_SONG_PLAYED;
                break;

            case MSGMODE_DISPLAY_SONG_PLAYED_TEXT_BEGIN:
                if (msgCtx->songPlayed == OCARINA_SONG_SCARECROW_SPAWN) {
                    Message_ContinueTextbox(play, 0x1B6B);
                } else {
                    Message_ContinueTextbox(play, 0x1B72 + msgCtx->songPlayed);
                }
                Message_Decode(play);
                msgCtx->msgMode = MSGMODE_16;
                msgCtx->stateTimer = 20;
                Message_DrawText(play, &gfx);
                break;

            case MSGMODE_16:
                msgCtx->stateTimer--;
                if (msgCtx->stateTimer == 0) {
                    msgCtx->msgMode = MSGMODE_17;
                }
                Message_DrawText(play, &gfx);
                break;

            case MSGMODE_17:
                AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_OFF);
                Message_ResetOcarinaButtonState(play);
                msgCtx->msgMode = MSGMODE_18;
                msgCtx->stateTimer = 2;
                Message_DrawText(play, &gfx);
                break;

            case MSGMODE_18:
                msgCtx->stateTimer--;
                if (msgCtx->stateTimer == 0) {
                    Message_CloseTextbox(play);
                    if (msgCtx->songPlayed == OCARINA_SONG_EPONAS) {
                        gHorsePlayedEponasSong = true;
                    }

                    if (msgCtx->ocarinaAction == OCARINA_ACTION_FREE_PLAY_DONE) {
                        if (sLastPlayedSong == OCARINA_SONG_ELEGY) {
                            if (GameInteractor_Should(
                                    VB_ELEGY_CHECK_SCENE,
                                    (play->sceneId == SCENE_F40) || (play->sceneId == SCENE_F41) ||
                                        (play->sceneId == SCENE_IKANAMAE) || (play->sceneId == SCENE_CASTLE) ||
                                        (play->sceneId == SCENE_IKNINSIDE) || (play->sceneId == SCENE_IKANA) ||
                                        (play->sceneId == SCENE_INISIE_N) || (play->sceneId == SCENE_INISIE_R) ||
                                        (play->sceneId == SCENE_INISIE_BS) || (play->sceneId == SCENE_RANDOM) ||
                                        (play->sceneId == SCENE_REDEAD) || (play->sceneId == SCENE_TOUGITES))) {
                                play->msgCtx.ocarinaMode = OCARINA_MODE_EVENT;
                            } else {
                                sLastPlayedSong = 0xFF;
                                Message_StartTextbox(play, 0x1B95, NULL);
                                play->msgCtx.ocarinaMode = OCARINA_MODE_PROCESS_RESTRICTED_SONG;
                            }
                        } else {
                            play->msgCtx.ocarinaMode = OCARINA_MODE_EVENT;
                            if (msgCtx->songPlayed == OCARINA_SONG_SCARECROW_SPAWN) {
                                play->msgCtx.ocarinaMode = OCARINA_MODE_PLAYED_SCARECROW_SPAWN;
                            }
                        }
                    } else if (msgCtx->ocarinaAction >= OCARINA_ACTION_CHECK_SONATA) {
                        if ((OCARINA_ACTION_CHECK_SONATA + msgCtx->songPlayed) == msgCtx->ocarinaAction) {
                            play->msgCtx.ocarinaMode = OCARINA_MODE_EVENT;
                        } else {
                            play->msgCtx.ocarinaMode = msgCtx->songPlayed - 1;
                        }
                    } else if ((OCARINA_ACTION_PROMPT_SONATA + msgCtx->songPlayed) == msgCtx->ocarinaAction) {
                        play->msgCtx.ocarinaMode = OCARINA_MODE_EVENT;
                    } else {
                        play->msgCtx.ocarinaMode = OCARINA_MODE_END;
                    }
                }
                break;

            case MSGMODE_19:
                msgCtx->stateTimer--;
                if (msgCtx->stateTimer == 0) {
                    func_80152CAC(play);
                    sOcarinaButtonIndexBufPos = 0;
                    msgCtx->msgMode = MSGMODE_SONG_DEMONSTRATION;
                }
                Message_DrawText(play, &gfx);
                break;

            case MSGMODE_DISPLAY_SONG_PLAYED:
            case MSGMODE_SONG_DEMONSTRATION:
                msgCtx->ocarinaStaff = AudioOcarina_GetPlaybackStaff();
                if (msgCtx->ocarinaStaff->state == 0) {
                    if ((msgCtx->ocarinaAction == OCARINA_ACTION_DEMONSTRATE_EVAN_PART1_SECOND_HALF) ||
                        (msgCtx->ocarinaAction == OCARINA_ACTION_DEMONSTRATE_EVAN_PART2_SECOND_HALF)) {
                        AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_OFF);
                    }
                    if ((msgCtx->ocarinaAction >= OCARINA_ACTION_PROMPT_WIND_FISH_HUMAN) &&
                        (msgCtx->ocarinaAction <= OCARINA_ACTION_PROMPT_WIND_FISH_DEKU)) {
                        AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_OFF);
                        Message_CloseTextbox(play);
                        play->msgCtx.ocarinaMode = OCARINA_MODE_END;
                    } else if (msgCtx->msgMode == MSGMODE_DISPLAY_SONG_PLAYED) {
                        msgCtx->msgMode = MSGMODE_DISPLAY_SONG_PLAYED_TEXT_BEGIN;
                    } else {
                        msgCtx->msgMode = MSGMODE_SONG_DEMONSTRATION_DONE;
                    }
                } else {
                    if ((sOcarinaButtonIndexBufPos != 0) && (msgCtx->ocarinaStaff->pos == 1)) {
                        sOcarinaButtonIndexBufPos = 0;
                    }

                    if (((u32)msgCtx->ocarinaStaff->pos != 0) &&
                        (sOcarinaButtonIndexBufPos == (msgCtx->ocarinaStaff->pos - 1))) {
                        msgCtx->ocarinaButtonIndex = sOcarinaButtonIndexBuf[msgCtx->ocarinaStaff->pos - 1] =
                            msgCtx->ocarinaStaff->buttonIndex;
                        sOcarinaButtonIndexBuf[msgCtx->ocarinaStaff->pos] = OCARINA_BTN_INVALID;
                        sOcarinaButtonIndexBufPos++;
                    }
                }
                // fallthrough
            case MSGMODE_SONG_DEMONSTRATION_DONE:
                Message_DrawText(play, &gfx);
                break;

            case MSGMODE_SONG_PROMPT:
            case MSGMODE_33:
                msgCtx->ocarinaStaff = AudioOcarina_GetPlayingStaff();

                if (((u32)msgCtx->ocarinaStaff->pos != 0) &&
                    (sOcarinaButtonIndexBufPos == (msgCtx->ocarinaStaff->pos - 1))) {
                    msgCtx->ocarinaButtonIndex = sOcarinaButtonIndexBuf[msgCtx->ocarinaStaff->pos - 1] =
                        msgCtx->ocarinaStaff->buttonIndex;
                    sOcarinaButtonIndexBuf[msgCtx->ocarinaStaff->pos] = OCARINA_BTN_INVALID;
                    sOcarinaButtonIndexBufPos++;
                }

                if (msgCtx->ocarinaStaff->state <= OCARINA_SONG_SCARECROW_SPAWN) {
                    if ((msgCtx->ocarinaAction == OCARINA_ACTION_PROMPT_EVAN_PART1_SECOND_HALF) ||
                        (msgCtx->ocarinaAction == OCARINA_ACTION_PROMPT_EVAN_PART2_SECOND_HALF)) {
                        msgCtx->songPlayed = msgCtx->ocarinaStaff->state;
                        msgCtx->msgMode = MSGMODE_22;
                    } else {
                        msgCtx->songPlayed = msgCtx->ocarinaStaff->state;
                        msgCtx->msgMode = MSGMODE_SONG_PROMPT_SUCCESS;
                        if (msgCtx->ocarinaStaff->state == OCARINA_SONG_GORON_LULLABY_INTRO) {
                            Item_Give(play, ITEM_SONG_LULLABY_INTRO);
                        } else if (!((msgCtx->ocarinaAction >= OCARINA_ACTION_PROMPT_WIND_FISH_HUMAN) &&
                                     (msgCtx->ocarinaAction <= OCARINA_ACTION_PROMPT_WIND_FISH_DEKU)) &&
                                   (msgCtx->ocarinaStaff->state != OCARINA_SONG_NEW_WAVE)) {
                            Item_Give(play,
                                      (gOcarinaSongItemMap[msgCtx->ocarinaStaff->state] + ITEM_SONG_SONATA) & 0xFF);
                        }
                    }
                    msgCtx->stateTimer = 20;
                    Audio_PlaySfx(NA_SE_SY_TRE_BOX_APPEAR);
                } else if (msgCtx->ocarinaStaff->state == OCARINA_SONG_TERMINA_WALL) {
                    msgCtx->songPlayed = msgCtx->ocarinaStaff->state;
                    msgCtx->msgMode = MSGMODE_34;
                    Item_Give(play, (gOcarinaSongItemMap[msgCtx->ocarinaStaff->state] + ITEM_SONG_SONATA) & 0xFF);
                    msgCtx->stateTimer = 20;
                    AudioOcarina_SetOcarinaDisableTimer(0, 20);
                    Audio_PlaySfx(NA_SE_SY_TRE_BOX_APPEAR);
                } else if (msgCtx->ocarinaStaff->state == 0xFF) {
                    Audio_PlaySfx(NA_SE_SY_OCARINA_ERROR);
                    msgCtx->stateTimer = 10;
                    if (msgCtx->msgMode == MSGMODE_SONG_PROMPT) {
                        msgCtx->msgMode = MSGMODE_SONG_PROMPT_FAIL;
                    } else {
                        AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_OFF);
                        play->msgCtx.ocarinaMode = OCARINA_MODE_END;
                        Message_CloseTextbox(play);
                    }
                }
                Message_DrawText(play, &gfx);
                break;

            case MSGMODE_OCARINA_AWAIT_INPUT:
                Message_DrawText(play, &gfx);
                if ((msgCtx->ocarinaAction == OCARINA_ACTION_PROMPT_EVAN_PART1_SECOND_HALF) ||
                    (msgCtx->ocarinaAction == OCARINA_ACTION_PROMPT_EVAN_PART2_SECOND_HALF)) {
                    msgCtx->stateTimer--;
                    if (msgCtx->stateTimer == 0) {
                        msgCtx->msgMode = MSGMODE_21;
                        msgCtx->textBoxType = TEXTBOX_TYPE_0;
                    }
                }
                break;

            case MSGMODE_SCARECROW_LONG_RECORDING_START:
                AudioOcarina_SetRecordingState(OCARINA_RECORD_SCARECROW_LONG);
                AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_DEFAULT);
                msgCtx->ocarinaStaff = AudioOcarina_GetRecordingStaff();
                msgCtx->ocarinaStaff->pos = 0;
                sOcarinaButtonIndexBufPos = 0;
                sOcarinaButtonIndexBufLen = 0;
                Message_ResetOcarinaButtonState(play);
                msgCtx->msgMode = MSGMODE_SCARECROW_LONG_RECORDING_ONGOING;
                Message_DrawText(play, &gfx);
                break;

            case MSGMODE_SCARECROW_LONG_RECORDING_ONGOING:
                msgCtx->ocarinaStaff = AudioOcarina_GetRecordingStaff();

                if (((u32)msgCtx->ocarinaStaff->pos != 0) &&
                    (sOcarinaButtonIndexBufPos == (msgCtx->ocarinaStaff->pos - 1))) {

                    if (sOcarinaButtonIndexBufLen >= 8) {
                        for (buttonIndexPos = sOcarinaButtonIndexBufLen - 8, i = 0; i < 8; i++, buttonIndexPos++) {
                            sOcarinaButtonIndexBuf[buttonIndexPos] = sOcarinaButtonIndexBuf[buttonIndexPos + 1];
                        }
                        sOcarinaButtonIndexBufLen--;
                    }

                    msgCtx->ocarinaButtonIndex = sOcarinaButtonIndexBuf[sOcarinaButtonIndexBufLen] =
                        msgCtx->ocarinaStaff->buttonIndex;
                    sOcarinaButtonIndexBufLen++;
                    sOcarinaButtonIndexBuf[sOcarinaButtonIndexBufLen] = OCARINA_BTN_INVALID;
                    sOcarinaButtonIndexBufPos++;
                    if (msgCtx->ocarinaStaff) {}
                    if (msgCtx->ocarinaStaff->pos == 8) {
                        sOcarinaButtonIndexBufPos = 0;
                    }
                }
                Message_DrawText(play, &gfx);
                break;

            case MSGMODE_SCARECROW_LONG_DEMONSTRATION:
            case MSGMODE_SCARECROW_SPAWN_DEMONSTRATION:
                msgCtx->ocarinaStaff = AudioOcarina_GetPlaybackStaff();

                if (((u32)msgCtx->ocarinaStaff->pos != 0) &&
                    (sOcarinaButtonIndexBufPos == (msgCtx->ocarinaStaff->pos - 1))) {
                    if (sOcarinaButtonIndexBufLen >= 8) {
                        for (buttonIndexPos = sOcarinaButtonIndexBufLen - 8, i = 0; i < 8; i++, buttonIndexPos++) {
                            sOcarinaButtonIndexBuf[buttonIndexPos] = sOcarinaButtonIndexBuf[buttonIndexPos + 1];
                        }
                        sOcarinaButtonIndexBufLen--;
                    }
                    sOcarinaButtonIndexBuf[sOcarinaButtonIndexBufLen] = msgCtx->ocarinaStaff->buttonIndex;
                    sOcarinaButtonIndexBufLen++;
                    sOcarinaButtonIndexBuf[sOcarinaButtonIndexBufLen] = OCARINA_BTN_INVALID;
                    sOcarinaButtonIndexBufPos++;
                    if (msgCtx->ocarinaStaff->pos == 8) {
                        sOcarinaButtonIndexBufLen = sOcarinaButtonIndexBufPos = 0;
                    }
                }

                if (msgCtx->stateTimer == 0) {
                    if (msgCtx->ocarinaStaff->state == 0) {
                        AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_OFF);
                        play->msgCtx.ocarinaMode = OCARINA_MODE_11;
                        Message_CloseTextbox(play);
                    }
                } else {
                    msgCtx->stateTimer--;
                }
                break;

            case MSGMODE_SCARECROW_SPAWN_RECORDING_START:
                AudioOcarina_SetRecordingState(OCARINA_RECORD_SCARECROW_SPAWN);
                AudioOcarina_SetInstrument(sPlayerFormOcarinaInstruments[CUR_FORM]);
                msgCtx->msgMode = MSGMODE_SCARECROW_SPAWN_RECORDING_ONGOING;
                Message_DrawText(play, &gfx);
                break;

            case MSGMODE_SCARECROW_SPAWN_RECORDING_ONGOING:
                msgCtx->ocarinaStaff = AudioOcarina_GetRecordingStaff();
                if ((u32)msgCtx->ocarinaStaff->pos != 0) {
                    if (sOcarinaButtonIndexBufPos == (msgCtx->ocarinaStaff->pos - 1)) {
                        msgCtx->ocarinaButtonIndex = sOcarinaButtonIndexBuf[sOcarinaButtonIndexBufPos] =
                            msgCtx->ocarinaStaff->buttonIndex;
                        sOcarinaButtonIndexBufPos++;
                        sOcarinaButtonIndexBuf[sOcarinaButtonIndexBufPos] = OCARINA_BTN_INVALID;
                    }
                }

                if (msgCtx->ocarinaStaff->state == 0) {
                    msgCtx->stateTimer = 20;
                    gSaveContext.save.saveInfo.scarecrowSpawnSongSet = true;
                    msgCtx->msgMode = MSGMODE_SCARECROW_SPAWN_RECORDING_DONE;
                    Audio_PlaySfx(NA_SE_SY_TRE_BOX_APPEAR);
                    memcpy(gSaveContext.save.saveInfo.scarecrowSpawnSong, gScarecrowSpawnSongPtr,
                           sizeof(gSaveContext.save.saveInfo.scarecrowSpawnSong));
                    for (i = 0; i < ARRAY_COUNT(gSaveContext.save.saveInfo.scarecrowSpawnSong); i++) {
                        // osSyncPrintf("%d, ", gSaveContext.scarecrowSpawnSong[i]);
                    }
                } else if (msgCtx->ocarinaStaff->state == 0xFF) {
                    AudioOcarina_SetRecordingState(OCARINA_RECORD_OFF);
                    Audio_PlaySfx(NA_SE_SY_OCARINA_ERROR);
                    Message_CloseTextbox(play);
                    msgCtx->msgMode = MSGMODE_SCARECROW_SPAWN_RECORDING_FAILED;
                }
                Message_DrawText(play, &gfx);
                break;

            case MSGMODE_SCARECROW_SPAWN_RECORDING_FAILED:
                AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_OFF);
                Message_StartTextbox(play, 0x1647, NULL);
                play->msgCtx.ocarinaMode = OCARINA_MODE_END;
                break;

            case MSGMODE_2F:
                AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_DEFAULT);
                msgCtx->ocarinaStaff = AudioOcarina_GetPlayingStaff();
                msgCtx->ocarinaStaff->pos = 0;
                sOcarinaButtonIndexBufPos = 0;
                play->msgCtx.ocarinaMode = OCARINA_MODE_ACTIVE;
                Message_ResetOcarinaButtonState(play);
                AudioOcarina_StartDefault(msgCtx->ocarinaAvailableSongs | 0xC0000000);
                msgCtx->msgMode = MSGMODE_30;
                break;

            case MSGMODE_30:
                msgCtx->ocarinaStaff = AudioOcarina_GetPlayingStaff();
                if (((u32)msgCtx->ocarinaStaff->pos != 0) &&
                    (sOcarinaButtonIndexBufPos == (msgCtx->ocarinaStaff->pos - 1))) {
                    msgCtx->ocarinaButtonIndex = msgCtx->ocarinaStaff->buttonIndex;
                    msgCtx->ocarinaStaff->pos = 0;
                    sOcarinaButtonIndexBufPos = 0;
                    Message_ResetOcarinaButtonState(play);
                    msgCtx->msgMode = MSGMODE_31;
                }
                break;

            case MSGMODE_35:
                AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_DEFAULT);
                AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_DEFAULT);
                msgCtx->ocarinaStaff = AudioOcarina_GetPlayingStaff();
                msgCtx->ocarinaStaff->pos = 0;
                sOcarinaButtonIndexBufPos = 0;
                play->msgCtx.ocarinaMode = OCARINA_MODE_ACTIVE;
                Message_ResetOcarinaButtonState(play);
                sOcarinaButtonFlashColorIndex = 1;
                sOcarinaButtonFlashTimer = 3;
                AudioOcarina_StartWithSongNoteLengths(
                    (1 << ((msgCtx->ocarinaAction - OCARINA_ACTION_TIMED_PROMPT_SONATA) & 0xFFFF)) | 0x80000000);
                msgCtx->msgMode = MSGMODE_36;
                Message_DrawText(play, &gfx);
                break;

            case MSGMODE_36:
                msgCtx->ocarinaStaff = AudioOcarina_GetPlayingStaff();

                if ((u32)msgCtx->ocarinaStaff->pos != 0) {
                    if (sOcarinaButtonIndexBufPos == (msgCtx->ocarinaStaff->pos - 1)) {
                        sOcarinaButtonIndexBuf[msgCtx->ocarinaStaff->pos - 1] = msgCtx->ocarinaStaff->buttonIndex;
                        sOcarinaButtonIndexBuf[msgCtx->ocarinaStaff->pos] = OCARINA_BTN_INVALID;
                        sOcarinaButtonIndexBufPos++;
                    }
                }

                if (msgCtx->ocarinaStaff->state <= OCARINA_SONG_SCARECROW_SPAWN) {
                    msgCtx->songPlayed = msgCtx->ocarinaStaff->state;
                    msgCtx->msgMode = MSGMODE_37;
                    Item_Give(play, (gOcarinaSongItemMap[msgCtx->ocarinaStaff->state] + ITEM_SONG_SONATA) & 0xFF);
                    msgCtx->stateTimer = 20;
                    Audio_PlaySfx(NA_SE_SY_TRE_BOX_APPEAR);
                } else if (msgCtx->ocarinaStaff->state == 0xFF) {
                    ocarinaError = func_8019B5AC();
                    if (ocarinaError == OCARINA_ERROR_2) {
                        Audio_PlaySfx(NA_SE_SY_OCARINA_ERROR);
                        D_801C6A94 = 1;
                        msgCtx->msgMode = MSGMODE_3B;
                    } else if (ocarinaError == OCARINA_ERROR_3) {
                        Audio_PlaySfx(NA_SE_SY_OCARINA_ERROR);
                        D_801C6A94 = 1;
                        msgCtx->msgMode = MSGMODE_3E;
                    } else {
                        Audio_PlaySfx(NA_SE_SY_OCARINA_ERROR);
                        D_801C6A94 = 1;
                        msgCtx->msgMode = MSGMODE_38;
                    }
                }
                Message_DrawText(play, &gfx);
                break;

            case MSGMODE_37:
                Message_FlashOcarinaButtons();
                Message_DrawText(play, &gfx);
                break;

            case MSGMODE_38:
            case MSGMODE_3B:
            case MSGMODE_3E:
                // notes drop
                for (i = 0; i < 5; i++) {
                    msgCtx->ocarinaButtonsPosY[i] += D_801C6A94;
                }

                D_801C6A94 += D_801C6A94;
                if (D_801C6A94 >= 0x226) {
                    Message_ResetOcarinaButtonAlphas();
                    msgCtx->textBoxType = TEXTBOX_TYPE_0;
                    msgCtx->textboxColorRed = msgCtx->textboxColorGreen = msgCtx->textboxColorBlue = 0;
                    msgCtx->stateTimer = 3;
                    msgCtx->msgMode++;
                }
                break;

            case MSGMODE_39:
            case MSGMODE_3C:
            case MSGMODE_3F:
                msgCtx->stateTimer--;
                if (msgCtx->stateTimer == 0) {
                    msgCtx->msgMode++;
                }
                break;

            case MSGMODE_TEXT_DONE:
                switch (msgCtx->textboxEndType) {
                    case TEXTBOX_ENDTYPE_INPUT_BANK:
                        temp_v0_33 = msgCtx->unk120BE;
                        temp = msgCtx->unk11FFA + (msgCtx->unk11FFC * temp_v0_33);
                        func_80147F18(play, &gfx,
                                      msgCtx->unk11F1A[temp_v0_33] +
                                          (s32)(16.0f * msgCtx->textCharScale * (msgCtx->unk120C2 + 5)) - 1,
                                      temp);
                        func_80148D64(play);
                        break;

                    case TEXTBOX_ENDTYPE_INPUT_DOGGY_RACETRACK_BET:
                        temp_v0_33 = msgCtx->unk120BE;
                        temp = msgCtx->unk11FFA + (msgCtx->unk11FFC * temp_v0_33);
                        func_80148558(play, &gfx,
                                      msgCtx->unk11F1A[temp_v0_33] + (s32)(16.0f * msgCtx->textCharScale * 5.0f) - 1,
                                      temp);
                        func_80149048(play);
                        break;

                    case TEXTBOX_ENDTYPE_INPUT_BOMBER_CODE:
                        temp_v0_33 = msgCtx->unk120BE;
                        temp = msgCtx->unk11FFA + (msgCtx->unk11FFC * temp_v0_33);
                        func_80147F18(play, &gfx,
                                      msgCtx->unk11F1A[temp_v0_33] +
                                          (s32)(16.0f * msgCtx->textCharScale * (msgCtx->unk120C2 + 5)) - 1,
                                      temp);
                        func_801491DC(play);
                        break;

                    case TEXTBOX_ENDTYPE_INPUT_LOTTERY_CODE:
                        temp_v0_33 = msgCtx->unk120BE;
                        temp = msgCtx->unk11FFA + (msgCtx->unk11FFC * temp_v0_33);
                        func_80147F18(play, &gfx,
                                      msgCtx->unk11F1A[temp_v0_33] +
                                          (s32)(16.0f * msgCtx->textCharScale * (msgCtx->unk120C2 + 5)) - 1,
                                      temp);
                        func_80149454(play);
                        break;

                    case TEXTBOX_ENDTYPE_64:
                        temp_v0_33 = msgCtx->unk120BE;
                        temp = msgCtx->unk11FFA + (msgCtx->unk11FFC * temp_v0_33);
                        func_80147F18(play, &gfx,
                                      msgCtx->unk11F1A[temp_v0_33] +
                                          (s32)(16.0f * msgCtx->textCharScale * (msgCtx->unk120C2 + 4)) - 6,
                                      temp);
                        func_801496C8(play);
                        break;

                    default:
                        break;
                }

                gDPPipeSync(gfx++);
                gDPSetCombineLERP(gfx++, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, PRIMITIVE, TEXEL0, 0,
                                  PRIMITIVE, 0);
                gDPSetEnvColor(gfx++, 0, 0, 0, 255);
                Message_DrawText(play, &gfx);

                switch (msgCtx->textboxEndType) {
                    case TEXTBOX_ENDTYPE_TWO_CHOICE:
                        func_80148CBC(play, &gfx, 1);
                        break;

                    case TEXTBOX_ENDTYPE_THREE_CHOICE:
                        func_80148CBC(play, &gfx, 2);
                        break;

                    case TEXTBOX_ENDTYPE_12:
                        func_80148CBC(play, &gfx, 1);
                        break;

                    case TEXTBOX_ENDTYPE_PERSISTENT:
                    case TEXTBOX_ENDTYPE_PAUSE_MENU:
                    case TEXTBOX_ENDTYPE_FADE_NORMAL:
                    case TEXTBOX_ENDTYPE_FADE_SKIPPABLE:
                    case TEXTBOX_ENDTYPE_FADE_STAGES_1:
                    case TEXTBOX_ENDTYPE_FADE_STAGES_2:
                    case TEXTBOX_ENDTYPE_FADE_STAGES_3:
                    case TEXTBOX_ENDTYPE_INPUT_BOMBER_CODE:
                        break;

                    case TEXTBOX_ENDTYPE_EVENT:
                    case TEXTBOX_ENDTYPE_INPUT_BANK:
                    case TEXTBOX_ENDTYPE_INPUT_DOGGY_RACETRACK_BET:
                    default:
                        if (GameInteractor_Should(VB_DRAW_OCARINA_STAFF, true)) {
                            Message_DrawTextboxIcon(play, &gfx, 158,
                                                    (s16)(D_801D03A8[msgCtx->textBoxType] + msgCtx->textboxYTarget));
                        }
                        break;

                    case TEXTBOX_ENDTYPE_EVENT2:
                        if (GameInteractor_Should(VB_DRAW_OCARINA_STAFF, true)) {
                            Message_DrawTextboxIcon(play, &gfx, 158,
                                                    (s16)(D_801D03A8[msgCtx->textBoxType] + msgCtx->textboxYTarget));
                        }
                        break;
                }
                break;

            case MSGMODE_SCENE_TITLE_CARD_FADE_IN_BACKGROUND:
            case MSGMODE_SCENE_TITLE_CARD_FADE_IN_TEXT:
            case MSGMODE_SCENE_TITLE_CARD_DISPLAYING:
            case MSGMODE_SCENE_TITLE_CARD_FADE_OUT_TEXT:
            case MSGMODE_SCENE_TITLE_CARD_FADE_OUT_BACKGROUND:
                Message_DrawSceneTitleCard(play, &gfx);
                break;

            case MSGMODE_21:
            case MSGMODE_23:
            case MSGMODE_31:
            case MSGMODE_3A:
            case MSGMODE_3D:
            case MSGMODE_40:
            case MSGMODE_TEXT_CLOSING:
            case MSGMODE_PAUSED:
                break;

            case MSGMODE_24:
            case MSGMODE_25:
            case MSGMODE_26:
            default:
                msgCtx->msgMode = MSGMODE_TEXT_DISPLAYING;
                break;
        }
    }
    if (GameInteractor_Should(VB_DRAW_OCARINA_STAFF, true)) {
        Message_DrawOcarinaButtons(play, &gfx);
    }

    *gfxP = gfx;
}

void Message_Draw(PlayState* play) {
    Gfx* nextDisplayList;
    Gfx* polyOpa;
    GraphicsContext* gfxCtx = play->state.gfxCtx;

    OPEN_DISPS(gfxCtx);

    polyOpa = POLY_OPA_DISP;
    nextDisplayList = Graph_GfxPlusOne(polyOpa);
    gSPDisplayList(OVERLAY_DISP++, nextDisplayList);

    if ((play->msgCtx.currentTextId != 0x5E6) || !Play_InCsMode(play)) {
        Message_DrawMain(play, &nextDisplayList);
    }

    gSPEndDisplayList(nextDisplayList++);
    Graph_BranchDlist(polyOpa, nextDisplayList);
    POLY_OPA_DISP = nextDisplayList;

    CLOSE_DISPS(gfxCtx);
}

s16 sTextboxXPositions[TEXTBOX_TYPE_MAX] = {
    34, // TEXTBOX_TYPE_0
    34, // TEXTBOX_TYPE_1
    34, // TEXTBOX_TYPE_2
    34, // TEXTBOX_TYPE_3
    34, // TEXTBOX_TYPE_4
    34, // TEXTBOX_TYPE_5
    34, // TEXTBOX_TYPE_6
    34, // TEXTBOX_TYPE_7
    34, // TEXTBOX_TYPE_8
    34, // TEXTBOX_TYPE_9
    34, // TEXTBOX_TYPE_A
    34, // TEXTBOX_TYPE_B
    34, // TEXTBOX_TYPE_C
    34, // TEXTBOX_TYPE_D
    34, // TEXTBOX_TYPE_E
    34, // TEXTBOX_TYPE_F
};

s16 sTextboxLowerYPositions[] = {
    142, // TEXTBOX_TYPE_0
    142, // TEXTBOX_TYPE_1
    142, // TEXTBOX_TYPE_2
    142, // TEXTBOX_TYPE_3
    174, // TEXTBOX_TYPE_4
    142, // TEXTBOX_TYPE_5
    142, // TEXTBOX_TYPE_6
    142, // TEXTBOX_TYPE_7
    142, // TEXTBOX_TYPE_8
    130, // TEXTBOX_TYPE_9
    174, // TEXTBOX_TYPE_A
    0,   // TEXTBOX_TYPE_B
    142, // TEXTBOX_TYPE_C
    142, // TEXTBOX_TYPE_D
    142, // TEXTBOX_TYPE_E
    142, // TEXTBOX_TYPE_F
};

s16 sTextboxUpperYPositions[] = {
    38,  // TEXTBOX_TYPE_0
    38,  // TEXTBOX_TYPE_1
    38,  // TEXTBOX_TYPE_2
    38,  // TEXTBOX_TYPE_3
    174, // TEXTBOX_TYPE_4
    38,  // TEXTBOX_TYPE_5
    38,  // TEXTBOX_TYPE_6
    38,  // TEXTBOX_TYPE_7
    38,  // TEXTBOX_TYPE_8
    60,  // TEXTBOX_TYPE_9
    174, // TEXTBOX_TYPE_A
    0,   // TEXTBOX_TYPE_B
    38,  // TEXTBOX_TYPE_C
    38,  // TEXTBOX_TYPE_D
    38,  // TEXTBOX_TYPE_E
    38,  // TEXTBOX_TYPE_F
};

s16 sTextboxMidYPositions[] = {
    90,  // TEXTBOX_TYPE_0
    90,  // TEXTBOX_TYPE_1
    90,  // TEXTBOX_TYPE_2
    90,  // TEXTBOX_TYPE_3
    174, // TEXTBOX_TYPE_4
    90,  // TEXTBOX_TYPE_5
    90,  // TEXTBOX_TYPE_6
    90,  // TEXTBOX_TYPE_7
    90,  // TEXTBOX_TYPE_8
    90,  // TEXTBOX_TYPE_9
    174, // TEXTBOX_TYPE_A
    0,   // TEXTBOX_TYPE_B
    90,  // TEXTBOX_TYPE_C
    90,  // TEXTBOX_TYPE_D
    90,  // TEXTBOX_TYPE_E
    90,  // TEXTBOX_TYPE_F
};

s16 D_801D0448[] = { 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25 };

s16 D_801D045C[] = {
    0x1B91,
    0x1B90,
    0x1B8F,
};
s16 D_801D0464[] = {
    0x1B92,
    0x1B8E,
};

void Message_Update(PlayState* play) {
    static u8 D_801D0468 = 0;
    MessageContext* msgCtx = &play->msgCtx;
    SramContext* sramCtx = &play->sramCtx; // Optional
    PauseContext* pauseCtx = &play->pauseCtx;
    InterfaceContext* interfaceCtx = &play->interfaceCtx;
    Input* input = CONTROLLER1(&play->state);
    s16 avgScreenPosY;
    s16 screenPosX;
    u16 temp_v1_2;
    s16 playerScreenPosY;
    s16 actorScreenPosY;
    s16 sp48;
    s32 sp44;
    s32 sp40;
    u16 sp3E;
    s16 var_v1;
    u16 temp;

    msgCtx->stickAdjX = input->rel.stick_x;
    msgCtx->stickAdjY = input->rel.stick_y;

    avgScreenPosY = 0;

    // If stickAdj is held, set a delay to allow the cursor to read the next input.
    // The first delay is given a longer time than all subsequent delays.
    if (msgCtx->stickAdjX < -30) {
        if (msgCtx->stickXRepeatState == -1) {
            msgCtx->stickXRepeatTimer--;
            if (msgCtx->stickXRepeatTimer < 0) {
                // Allow the input to register and apply the delay for all subsequent repeated inputs
                msgCtx->stickXRepeatTimer = 2;
            } else {
                // Cancel the current input
                msgCtx->stickAdjX = 0;
            }
        } else {
            // Allow the input to register and apply the delay for the first repeated input
            msgCtx->stickXRepeatTimer = 10;
            msgCtx->stickXRepeatState = -1;
        }
    } else if (msgCtx->stickAdjX > 30) {
        if (msgCtx->stickXRepeatState == 1) {
            msgCtx->stickXRepeatTimer--;
            if (msgCtx->stickXRepeatTimer < 0) {
                // Allow the input to register and apply the delay for all subsequent repeated inputs
                msgCtx->stickXRepeatTimer = 2;
            } else {
                // Cancel the current input
                msgCtx->stickAdjX = 0;
            }
        } else {
            // Allow the input to register and apply the delay for the first repeated input
            msgCtx->stickXRepeatTimer = 10;
            msgCtx->stickXRepeatState = 1;
        }
    } else {
        msgCtx->stickXRepeatState = 0;
    }

    if (msgCtx->stickAdjY < -30) {
        if (msgCtx->stickYRepeatState == -1) {
            msgCtx->stickYRepeatTimer--;
            if (msgCtx->stickYRepeatTimer < 0) {
                // Allow the input to register and apply the delay for all subsequent repeated inputs
                msgCtx->stickYRepeatTimer = 2;
            } else {
                // Cancel the current input
                msgCtx->stickAdjY = 0;
            }
        } else {
            // Allow the input to register and apply the delay for the first repeated input
            msgCtx->stickYRepeatTimer = 10;
            msgCtx->stickYRepeatState = -1;
        }
    } else if (msgCtx->stickAdjY > 30) {
        if (msgCtx->stickYRepeatState == 1) {
            msgCtx->stickYRepeatTimer--;
            if (msgCtx->stickYRepeatTimer < 0) {
                // Allow the input to register and apply the delay for all subsequent repeated inputs
                msgCtx->stickYRepeatTimer = 2;
            } else {
                // Cancel the current input
                msgCtx->stickAdjY = 0;
            }
        } else {
            // Allow the input to register and apply the delay for the first repeated input
            msgCtx->stickYRepeatTimer = 10;
            msgCtx->stickYRepeatState = 1;
        }
    } else {
        msgCtx->stickYRepeatState = 0;
    }

    if (msgCtx->msgLength == 0) {
        return;
    }

    switch (msgCtx->msgMode) {
        case MSGMODE_TEXT_START:
            D_801C6A70++;

            temp = false;
            if ((D_801C6A70 >= 4) || ((msgCtx->talkActor == NULL) && (D_801C6A70 >= 2))) {
                temp = true;
            }
            if (temp) {
                if (msgCtx->talkActor != NULL) {
                    Actor_GetScreenPos(play, &GET_PLAYER(play)->actor, &screenPosX, &playerScreenPosY);
                    Actor_GetScreenPos(play, msgCtx->talkActor, &screenPosX, &actorScreenPosY);
                    if (playerScreenPosY >= actorScreenPosY) {
                        avgScreenPosY = ((playerScreenPosY - actorScreenPosY) / 2) + actorScreenPosY;
                    } else {
                        avgScreenPosY = ((actorScreenPosY - playerScreenPosY) / 2) + playerScreenPosY;
                    }
                } else {
                    msgCtx->textboxX = msgCtx->textboxXTarget;
                    msgCtx->textboxY = msgCtx->textboxYTarget;
                }

                var_v1 = msgCtx->textBoxType;

                if ((u32)msgCtx->textBoxPos == 0) {
                    if ((play->sceneId == SCENE_UNSET_04) || (play->sceneId == SCENE_UNSET_05) ||
                        (play->sceneId == SCENE_UNSET_06)) {
                        if (avgScreenPosY < 100) {
                            msgCtx->textboxYTarget = sTextboxLowerYPositions[var_v1];
                        } else {
                            msgCtx->textboxYTarget = sTextboxUpperYPositions[var_v1];
                        }
                    } else {
                        if (avgScreenPosY < 160) {
                            msgCtx->textboxYTarget = sTextboxLowerYPositions[var_v1];
                        } else {
                            msgCtx->textboxYTarget = sTextboxUpperYPositions[var_v1];
                        }
                    }
                } else if (msgCtx->textBoxPos == 1) {
                    msgCtx->textboxYTarget = sTextboxUpperYPositions[var_v1];
                } else if (msgCtx->textBoxPos == 2) {
                    msgCtx->textboxYTarget = sTextboxMidYPositions[var_v1];
                } else if (msgCtx->textBoxPos == 7) {
                    msgCtx->textboxYTarget = 158;
                } else {
                    msgCtx->textboxYTarget = sTextboxLowerYPositions[var_v1];
                }

                msgCtx->textboxXTarget = sTextboxXPositions[var_v1];

                if ((gSaveContext.options.language == LANGUAGE_JPN) && !msgCtx->textIsCredits) {
                    msgCtx->unk11FFE[0] = (s16)(msgCtx->textboxYTarget + 7);
                    msgCtx->unk11FFE[1] = (s16)(msgCtx->textboxYTarget + 25);
                    msgCtx->unk11FFE[2] = (s16)(msgCtx->textboxYTarget + 43);
                } else {
                    msgCtx->unk11FFE[0] = (s16)(msgCtx->textboxYTarget + 20);
                    msgCtx->unk11FFE[1] = (s16)(msgCtx->textboxYTarget + 32);
                    msgCtx->unk11FFE[2] = (s16)(msgCtx->textboxYTarget + 44);
                }

                if ((msgCtx->textBoxType == TEXTBOX_TYPE_4) || (msgCtx->textBoxType == TEXTBOX_TYPE_5)) {
                    msgCtx->msgMode = MSGMODE_TEXT_STARTING;
                    msgCtx->textboxX = msgCtx->textboxXTarget;
                    msgCtx->textboxY = msgCtx->textboxYTarget;
                    msgCtx->unk12008 = 0x100;
                    msgCtx->unk1200A = 0x40;
                    msgCtx->unk1200C = 0x200;
                    msgCtx->unk1200E = 0x200;
                    break;
                }

                Message_GrowTextbox(play);
                Audio_PlaySfx_IfNotInCutscene(NA_SE_NONE);
                msgCtx->stateTimer = 0;
                msgCtx->msgMode = MSGMODE_TEXT_BOX_GROWING;

                if (!pauseCtx->itemDescriptionOn) {
                    func_80150A84(play);
                }
            }
            break;

        case MSGMODE_TEXT_BOX_GROWING:
            Message_GrowTextbox(play);
            break;

        case MSGMODE_TEXT_STARTING:
            msgCtx->msgMode = MSGMODE_TEXT_NEXT_MSG;
            if (!pauseCtx->itemDescriptionOn) {
                if (msgCtx->currentTextId == 0xFF) {
                    Interface_SetAButtonDoAction(play, DO_ACTION_STOP);
                } else if (msgCtx->currentTextId != 0xF8) {
                    Interface_SetAButtonDoAction(play, DO_ACTION_NEXT);
                }
            }
            break;

        case MSGMODE_TEXT_NEXT_MSG:
            Message_Decode(play);
            if (msgCtx->textFade) {
                Interface_SetHudVisibility(HUD_VISIBILITY_NONE);
            }
            if (D_801D0468 != 0) {
                msgCtx->textDrawPos = msgCtx->decodedTextLen;
                D_801D0468 = 0;
            }
            break;

        case MSGMODE_TEXT_CONTINUING:
            msgCtx->stateTimer--;
            if (msgCtx->stateTimer == 0) {
                Message_Decode(play);
            }
            break;

        case MSGMODE_TEXT_DISPLAYING:
            if (msgCtx->textBoxType != TEXTBOX_TYPE_4) {
                if (CHECK_BTN_ALL(input->press.button, BTN_B) && !msgCtx->textUnskippable) {
                    msgCtx->textboxSkipped = true;
                    msgCtx->textDrawPos = msgCtx->decodedTextLen;
                } else if (CHECK_BTN_ALL(input->press.button, BTN_A) && !msgCtx->textUnskippable) {

                    while (true) {
                        temp_v1_2 = msgCtx->decodedBuffer.wchar[msgCtx->textDrawPos];
                        if ((temp_v1_2 == 0x10) || (temp_v1_2 == 0x12) || (temp_v1_2 == 0x1B) || (temp_v1_2 == 0x1C) ||
                            (temp_v1_2 == 0x1D) || (temp_v1_2 == 0x19) || (temp_v1_2 == 0xE0) || (temp_v1_2 == 0xBF) ||
                            (temp_v1_2 == 0x15) || (temp_v1_2 == 0x1A)) {
                            break;
                        }
                        msgCtx->textDrawPos++;
                    }
                    // 2S2H [Enhancement] When fast text is on, we want to check if B is held instead of only if it was
                    // just pressed. Has an additional check that the textbox hasnt been skipped already to prevent the
                    // message from failing to continue.
                } else if ((CVarGetInteger("gEnhancements.Dialogue.FastText", 0) &&
                            CHECK_BTN_ALL(input->cur.button, BTN_B)) &&
                           !msgCtx->textUnskippable && !msgCtx->textboxSkipped) {
                    msgCtx->textboxSkipped = true;
                    msgCtx->textDrawPos = msgCtx->decodedTextLen;
                }
            } else if (CHECK_BTN_ALL(input->press.button, BTN_A) && (msgCtx->textUnskippable == 0)) {
                while (true) {
                    temp_v1_2 = msgCtx->decodedBuffer.wchar[msgCtx->textDrawPos];
                    if ((temp_v1_2 == 0x10) || (temp_v1_2 == 0x12) || (temp_v1_2 == 0x1B) || (temp_v1_2 == 0x1C) ||
                        (temp_v1_2 == 0x1D) || (temp_v1_2 == 0x19) || (temp_v1_2 == 0xE0) || (temp_v1_2 == 0xBF) ||
                        (temp_v1_2 == 0x15) || (temp_v1_2 == 0x1A)) {
                        break;
                    }
                    msgCtx->textDrawPos++;
                }
            }
            break;

        case MSGMODE_TEXT_AWAIT_INPUT:
            if (Message_ShouldAdvance(play)) {
                msgCtx->msgMode = MSGMODE_TEXT_DISPLAYING;
                msgCtx->textDrawPos++;
            }
            break;

        case MSGMODE_TEXT_DELAYED_BREAK:
            msgCtx->stateTimer--;
            if (msgCtx->stateTimer == 0) {
                msgCtx->msgMode = MSGMODE_TEXT_NEXT_MSG;
            }
            break;

        case MSGMODE_TEXT_AWAIT_NEXT:
            if (Message_ShouldAdvance(play)) {
                msgCtx->msgMode = MSGMODE_TEXT_NEXT_MSG;
                msgCtx->msgBufPos++;
            }
            break;

        case MSGMODE_TEXT_DONE:
            if ((msgCtx->textboxEndType == TEXTBOX_ENDTYPE_FADE_NORMAL) ||
                (msgCtx->textboxEndType == TEXTBOX_ENDTYPE_FADE_SKIPPABLE)) {
                msgCtx->stateTimer--;
                if ((msgCtx->stateTimer == 0) ||
                    ((msgCtx->textboxEndType == TEXTBOX_ENDTYPE_FADE_SKIPPABLE) && Message_ShouldAdvance(play))) {
                    if (msgCtx->nextTextId != 0xFFFF) {
                        Audio_PlaySfx(NA_SE_SY_MESSAGE_PASS);
                        Message_ContinueTextbox(play, msgCtx->nextTextId);
                    } else if (msgCtx->bombersNotebookEventQueueCount != 0) {
                        if (Message_BombersNotebookProcessEventQueue(play) == 0) {
                            Message_CloseTextbox(play);
                        }
                    } else {
                        Message_CloseTextbox(play);
                    }
                }
            } else {
                if ((msgCtx->textboxEndType == TEXTBOX_ENDTYPE_PERSISTENT) ||
                    (msgCtx->textboxEndType == TEXTBOX_ENDTYPE_EVENT) ||
                    (msgCtx->textboxEndType == TEXTBOX_ENDTYPE_EVENT2) ||
                    (msgCtx->textboxEndType == TEXTBOX_ENDTYPE_PAUSE_MENU)) {
                    return;
                }

                switch (msgCtx->textboxEndType) {
                    case TEXTBOX_ENDTYPE_FADE_STAGES_1:
                        msgCtx->textColorAlpha += 20;
                        if (msgCtx->textColorAlpha >= 255) {
                            msgCtx->textColorAlpha = 255;
                            msgCtx->textboxEndType = TEXTBOX_ENDTYPE_FADE_STAGES_2;
                        }
                        break;

                    case TEXTBOX_ENDTYPE_FADE_STAGES_2:
                        msgCtx->stateTimer--;
                        if (msgCtx->stateTimer == 0) {
                            msgCtx->textboxEndType = TEXTBOX_ENDTYPE_FADE_STAGES_3;
                        }
                        break;

                    case TEXTBOX_ENDTYPE_FADE_STAGES_3:
                        msgCtx->textColorAlpha -= 20;
                        if (msgCtx->textColorAlpha <= 0) {
                            msgCtx->textColorAlpha = 0;
                            if (msgCtx->nextTextId != 0xFFFF) {
                                Audio_PlaySfx(NA_SE_SY_MESSAGE_PASS);
                                Message_ContinueTextbox(play, msgCtx->nextTextId);
                                return;
                            }
                            if (msgCtx->bombersNotebookEventQueueCount != 0) {
                                if (Message_BombersNotebookProcessEventQueue(play) == 0) {
                                    Message_CloseTextbox(play);
                                    return;
                                }
                            } else {
                                Message_CloseTextbox(play);
                                return;
                            }
                        }
                        break;

                    case TEXTBOX_ENDTYPE_TWO_CHOICE:
                        Message_HandleChoiceSelection(play, 1);
                        break;

                    case TEXTBOX_ENDTYPE_THREE_CHOICE:
                        Message_HandleChoiceSelection(play, 2);
                        break;

                    case TEXTBOX_ENDTYPE_12:
                        Message_HandleChoiceSelection(play, 1);
                        break;

                    default:
                        break;
                }

                if (GameInteractor_Should(VB_MSG_CAPTURE_MSGMODE_TEXT_DONE, false)) {
                    // no-op
                } else if ((msgCtx->textboxEndType == TEXTBOX_ENDTYPE_TWO_CHOICE) &&
                           (play->msgCtx.ocarinaMode == OCARINA_MODE_ACTIVE)) {
                    if (Message_ShouldAdvance(play)) {
                        if (msgCtx->choiceIndex == 0) {
                            play->msgCtx.ocarinaMode = OCARINA_MODE_WARP;
                        } else {
                            play->msgCtx.ocarinaMode = OCARINA_MODE_END;
                        }
                        Message_CloseTextbox(play);
                    }
                } else if ((msgCtx->textboxEndType == TEXTBOX_ENDTYPE_TWO_CHOICE) &&
                           (play->msgCtx.ocarinaMode == OCARINA_MODE_PROCESS_SOT)) {
                    if (Message_ShouldAdvance(play)) {
                        if (msgCtx->choiceIndex == 0) {
                            Audio_PlaySfx_MessageDecide();
                            msgCtx->msgMode = MSGMODE_NEW_CYCLE_0;
                            msgCtx->decodedTextLen -= 3;
                            msgCtx->unk120D6 = 0;
                            msgCtx->unk120D4 = 0;
                        } else {
                            Audio_PlaySfx_MessageCancel();
                            play->msgCtx.ocarinaMode = OCARINA_MODE_END;
                            Message_CloseTextbox(play);
                        }
                    }
                } else if ((msgCtx->textboxEndType == TEXTBOX_ENDTYPE_TWO_CHOICE) &&
                           (play->msgCtx.ocarinaMode == OCARINA_MODE_PROCESS_INVERTED_TIME)) {
                    if (Message_ShouldAdvance(play)) {
                        if (msgCtx->choiceIndex == 0) {
                            Audio_PlaySfx_MessageDecide();
                            if (gSaveContext.save.timeSpeedOffset == 0) {
                                play->msgCtx.ocarinaMode = OCARINA_MODE_APPLY_INV_SOT_SLOW;
                                gSaveContext.save.timeSpeedOffset = -2;
                            } else {
                                play->msgCtx.ocarinaMode = OCARINA_MODE_APPLY_INV_SOT_FAST;
                                gSaveContext.save.timeSpeedOffset = 0;
                            }
                            Message_CloseTextbox(play);
                        } else {
                            Audio_PlaySfx_MessageCancel();
                            play->msgCtx.ocarinaMode = OCARINA_MODE_END;
                            Message_CloseTextbox(play);
                        }
                    }
                } else if ((msgCtx->textboxEndType == TEXTBOX_ENDTYPE_TWO_CHOICE) &&
                           (play->msgCtx.ocarinaMode == OCARINA_MODE_PROCESS_DOUBLE_TIME)) {
                    if (Message_ShouldAdvance(play)) {
                        if (msgCtx->choiceIndex == 0) {
                            Audio_PlaySfx_MessageDecide();
                            if (gSaveContext.save.isNight != 0) {
                                gSaveContext.save.time = CLOCK_TIME(6, 0);
                            } else {
                                gSaveContext.save.time = CLOCK_TIME(18, 0);
                            }
                            play->msgCtx.ocarinaMode = OCARINA_MODE_APPLY_DOUBLE_SOT;
                            gSaveContext.timerStates[TIMER_ID_MOON_CRASH] = TIMER_STATE_OFF;
                        } else {
                            Audio_PlaySfx_MessageCancel();
                            play->msgCtx.ocarinaMode = OCARINA_MODE_END;
                        }
                        Message_CloseTextbox(play);
                    }
                } else if ((msgCtx->textboxEndType != TEXTBOX_ENDTYPE_TWO_CHOICE) ||
                           (pauseCtx->state != PAUSE_STATE_OWL_WARP_CONFIRM)) {
                    if ((msgCtx->textboxEndType == TEXTBOX_ENDTYPE_TWO_CHOICE) &&
                        (play->msgCtx.ocarinaMode == OCARINA_MODE_1B)) {
                        if (Message_ShouldAdvance(play)) {
                            if (msgCtx->choiceIndex == 0) {
                                Audio_PlaySfx_MessageDecide();
                                play->msgCtx.ocarinaMode = OCARINA_MODE_WARP_TO_ENTRANCE;
                            } else {
                                Audio_PlaySfx_MessageCancel();
                                play->msgCtx.ocarinaMode = OCARINA_MODE_END;
                            }
                            Message_CloseTextbox(play);
                        }
                    } else if ((msgCtx->textboxEndType == TEXTBOX_ENDTYPE_INPUT_BANK) ||
                               (msgCtx->textboxEndType == TEXTBOX_ENDTYPE_INPUT_DOGGY_RACETRACK_BET) ||
                               (msgCtx->textboxEndType == TEXTBOX_ENDTYPE_TWO_CHOICE) ||
                               (msgCtx->textboxEndType == TEXTBOX_ENDTYPE_THREE_CHOICE) ||
                               (msgCtx->textboxEndType == TEXTBOX_ENDTYPE_FADE_NORMAL) ||
                               (msgCtx->textboxEndType == TEXTBOX_ENDTYPE_FADE_SKIPPABLE) ||
                               (msgCtx->textboxEndType == TEXTBOX_ENDTYPE_FADE_STAGES_1) ||
                               (msgCtx->textboxEndType == TEXTBOX_ENDTYPE_FADE_STAGES_2) ||
                               (msgCtx->textboxEndType == TEXTBOX_ENDTYPE_FADE_STAGES_3) ||
                               (msgCtx->textboxEndType == TEXTBOX_ENDTYPE_INPUT_BOMBER_CODE)) {
                        //! FAKE: debug?
                        if (msgCtx->textboxEndType == TEXTBOX_ENDTYPE_FADE_NORMAL) {}
                    } else if (pauseCtx->itemDescriptionOn) {
                        if ((input->rel.stick_x != 0) || (input->rel.stick_y != 0) ||
                            CHECK_BTN_ALL(input->press.button, BTN_A) || CHECK_BTN_ALL(input->press.button, BTN_B) ||
                            CHECK_BTN_ALL(input->press.button, BTN_START)) {
                            Audio_PlaySfx(NA_SE_SY_DECIDE);
                            Message_CloseTextbox(play);
                        }
                    } else if (play->msgCtx.ocarinaMode == OCARINA_MODE_PROCESS_RESTRICTED_SONG) {
                        if (Message_ShouldAdvanceSilent(play)) {
                            Audio_PlaySfx(NA_SE_SY_DECIDE);
                            Message_CloseTextbox(play);
                            play->msgCtx.ocarinaMode = OCARINA_MODE_END;
                        }
                    } else if ((msgCtx->currentTextId != 0x2790) && Message_ShouldAdvanceSilent(play)) {
                        if (msgCtx->nextTextId != 0xFFFF) {
                            Audio_PlaySfx(NA_SE_SY_MESSAGE_PASS);
                            Message_ContinueTextbox(play, msgCtx->nextTextId);
                        } else if ((msgCtx->bombersNotebookEventQueueCount == 0) ||
                                   (Message_BombersNotebookProcessEventQueue(play) != 1)) {
                            if (msgCtx->currentTextId == 0x579) {
                                gSaveContext.hudVisibility = HUD_VISIBILITY_IDLE;
                            }
                            Audio_PlaySfx(NA_SE_SY_DECIDE);
                            Message_CloseTextbox(play);
                        }
                    }
                }
            }
            break;

        case MSGMODE_TEXT_CLOSING:
            msgCtx->stateTimer--;
            if (msgCtx->stateTimer != 0) {
                break;
            }

            if (sLastPlayedSong == OCARINA_SONG_SOARING) {
                if (interfaceCtx->restrictions.songOfSoaring == 0) {
                    if (Map_CurRoomHasMapI(play) || (play->sceneId == SCENE_SECOM)) {
                        Message_StartTextbox(play, 0x1B93, NULL);
                        play->msgCtx.ocarinaMode = OCARINA_MODE_1B;
                        sLastPlayedSong = 0xFF;
                    } else if (!msgCtx->ocarinaSongEffectActive) {
                        if (gSaveContext.save.saveInfo.playerData.owlActivationFlags != 0) {
                            pauseCtx->unk_2C8 = pauseCtx->pageIndex;
                            pauseCtx->unk_2CA = pauseCtx->cursorPoint[4];
                            pauseCtx->pageIndex = PAUSE_ITEM;
                            pauseCtx->state = PAUSE_STATE_OWL_WARP_0;
                            func_800F4A10(play);
                            pauseCtx->pageIndex = PAUSE_MAP;
                            sLastPlayedSong = 0xFF;
                            Message_CloseTextbox(play);
                            play->msgCtx.ocarinaMode = OCARINA_MODE_END;
                            gSaveContext.prevHudVisibility = HUD_VISIBILITY_A_B;
                            Interface_SetBButtonInterfaceDoAction(play, DO_ACTION_STOP);
                            GameState_SetFramerateDivisor(&play->state, 2);
                            if (ShrinkWindow_Letterbox_GetSizeTarget() != 0) {
                                ShrinkWindow_Letterbox_SetSizeTarget(0);
                            }
                            Audio_PlaySfx_PauseMenuOpenOrClose(1);
                            break;
                        }
                        sLastPlayedSong = 0xFF;
                        Message_StartTextbox(play, 0xFB, NULL);
                        play->msgCtx.ocarinaMode = OCARINA_MODE_PROCESS_RESTRICTED_SONG;
                    } else {
                        msgCtx->stateTimer = 1;
                    }
                } else {
                    sLastPlayedSong = 0xFF;
                    Message_StartTextbox(play, 0x1B95, NULL);
                    play->msgCtx.ocarinaMode = OCARINA_MODE_PROCESS_RESTRICTED_SONG;
                }
                break;
            }

            if ((msgCtx->currentTextId == 0xC) || (msgCtx->currentTextId == 0xD) || (msgCtx->currentTextId == 0xC5) ||
                (msgCtx->currentTextId == 0xC6) || (msgCtx->currentTextId == 0xC7) ||
                (msgCtx->currentTextId == 0x2165) || (msgCtx->currentTextId == 0x2166) ||
                (msgCtx->currentTextId == 0x2167) || (msgCtx->currentTextId == 0x2168)) {
                gSaveContext.healthAccumulator = 20 * 0x10; // Refill 20 hearts
            }

            if ((play->csCtx.state == CS_STATE_IDLE) && (gSaveContext.save.cutsceneIndex < 0xFFF0) &&
                ((play->activeCamId == CAM_ID_MAIN) ||
                 ((play->transitionTrigger == TRANS_TRIGGER_OFF) && (play->transitionMode == TRANS_MODE_OFF))) &&
                (play->msgCtx.ocarinaMode == OCARINA_MODE_END)) {
                if (((u32)gSaveContext.prevHudVisibility == HUD_VISIBILITY_IDLE) ||
                    (gSaveContext.prevHudVisibility == HUD_VISIBILITY_NONE) ||
                    (gSaveContext.prevHudVisibility == HUD_VISIBILITY_NONE_ALT)) {
                    gSaveContext.prevHudVisibility = HUD_VISIBILITY_ALL;
                }
                gSaveContext.hudVisibility = HUD_VISIBILITY_IDLE;
            }

            if ((msgCtx->currentTextId >= 0x1BB2) && (msgCtx->currentTextId <= 0x1BB6) &&
                (play->actorCtx.flags & ACTORCTX_FLAG_TELESCOPE_ON)) {
                Message_StartTextbox(play, 0x5E6, NULL);
                break;
            }

            if (msgCtx->bombersNotebookEventQueueCount != 0) {
                if (Message_BombersNotebookProcessEventQueue(play) == 0) {
                    msgCtx->stateTimer = 1;
                }
                break;
            }

            msgCtx->msgLength = 0;
            msgCtx->msgMode = MSGMODE_NONE;
            msgCtx->currentTextId = 0;
            msgCtx->stateTimer = 0;
            XREG(31) = 0;

            if (pauseCtx->itemDescriptionOn) {
                Interface_SetAButtonDoAction(play, DO_ACTION_INFO);
                pauseCtx->itemDescriptionOn = false;
            }

            if (msgCtx->textboxEndType == TEXTBOX_ENDTYPE_PERSISTENT) {
                msgCtx->textboxEndType = TEXTBOX_ENDTYPE_DEFAULT;
                play->msgCtx.ocarinaMode = OCARINA_MODE_WARP;
            } else {
                msgCtx->textboxEndType = TEXTBOX_ENDTYPE_DEFAULT;
            }

            if (EQ_MAX_QUEST_HEART_PIECE_COUNT) {
                RESET_HEART_PIECE_COUNT;
                gSaveContext.save.saveInfo.playerData.healthCapacity += 0x10;
                gSaveContext.save.saveInfo.playerData.health += 0x10;
            }

            if (msgCtx->ocarinaAction != OCARINA_ACTION_CHECK_NOTIME_DONE) {
                s16 pad;

                if (GameInteractor_Should(VB_MSG_CAPTURE_MSGMODE_TEXT_CLOSING_OCARINA_ACTION, false)) {
                    // no-op
                } else if (sLastPlayedSong == OCARINA_SONG_TIME) {
                    if (interfaceCtx->restrictions.songOfTime == 0) {
                        Message_StartTextbox(play, 0x1B8A, NULL);
                        play->msgCtx.ocarinaMode = OCARINA_MODE_PROCESS_SOT;
                    } else {
                        sLastPlayedSong = 0xFF;
                        Message_StartTextbox(play, 0x1B95, NULL);
                        play->msgCtx.ocarinaMode = OCARINA_MODE_PROCESS_RESTRICTED_SONG;
                    }
                } else if (sLastPlayedSong == OCARINA_SONG_INVERTED_TIME) {
                    if (interfaceCtx->restrictions.invSongOfTime == 0) {
                        if (R_TIME_SPEED != 0) {
                            if (gSaveContext.save.timeSpeedOffset == 0) {
                                Message_StartTextbox(play, 0x1B8C, NULL);
                            } else {
                                Message_StartTextbox(play, 0x1B8D, NULL);
                            }
                            play->msgCtx.ocarinaMode = OCARINA_MODE_PROCESS_INVERTED_TIME;
                        } else {
                            Message_StartTextbox(play, 0x1B8B, NULL);
                            play->msgCtx.ocarinaMode = OCARINA_MODE_END;
                        }
                    } else {
                        sLastPlayedSong = 0xFF;
                        Message_StartTextbox(play, 0x1B95, NULL);
                        play->msgCtx.ocarinaMode = OCARINA_MODE_PROCESS_RESTRICTED_SONG;
                    }
                } else if (sLastPlayedSong == OCARINA_SONG_DOUBLE_TIME) {
                    if (interfaceCtx->restrictions.songOfDoubleTime == 0) {
                        if (GameInteractor_Should(VB_ALLOW_SONG_DOUBLE_TIME_ON_FINAL_NIGHT,
                                                  (CURRENT_DAY != 3) || (gSaveContext.save.isNight == 0))) {
                            if (GameInteractor_Should(VB_DISPLAY_SONG_OF_DOUBLE_TIME_PROMPT, true)) {
                                if (gSaveContext.save.isNight) {
                                    Message_StartTextbox(play, D_801D0464[CURRENT_DAY - 1], NULL);
                                } else {
                                    Message_StartTextbox(play, D_801D045C[CURRENT_DAY - 1], NULL);
                                }
                                play->msgCtx.ocarinaMode = OCARINA_MODE_PROCESS_DOUBLE_TIME;
                            }
                        } else {
                            Message_StartTextbox(play, 0x1B94, NULL);
                            play->msgCtx.ocarinaMode = OCARINA_MODE_END;
                        }
                    } else {
                        sLastPlayedSong = 0xFF;
                        Message_StartTextbox(play, 0x1B95, NULL);
                        play->msgCtx.ocarinaMode = OCARINA_MODE_PROCESS_RESTRICTED_SONG;
                    }
                } else if ((msgCtx->ocarinaAction == OCARINA_ACTION_FREE_PLAY_DONE) &&
                           ((play->msgCtx.ocarinaMode == OCARINA_MODE_ACTIVE) ||
                            (play->msgCtx.ocarinaMode == OCARINA_MODE_EVENT) ||
                            (play->msgCtx.ocarinaMode == OCARINA_MODE_PLAYED_SCARECROW_SPAWN) ||
                            (play->msgCtx.ocarinaMode == OCARINA_MODE_PLAYED_FULL_EVAN_SONG))) {
                    play->msgCtx.ocarinaMode = OCARINA_MODE_END;
                    if (msgCtx->lastPlayedSong == OCARINA_SONG_SOARING) {
                        play->msgCtx.ocarinaMode = OCARINA_MODE_ACTIVE;
                    }
                }
                sLastPlayedSong = 0xFF;
            }
            break;

        case MSGMODE_OCARINA_PLAYING:
            if (CHECK_BTN_ALL(input->press.button, BTN_B)) {
                AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_OFF);
                play->msgCtx.ocarinaMode = OCARINA_MODE_END;
                Message_CloseTextbox(play);
            } else {
                msgCtx->ocarinaButtonIndex = OCARINA_BTN_INVALID;
            }
            break;

        case MSGMODE_OCARINA_AWAIT_INPUT:
            if ((msgCtx->ocarinaAction != OCARINA_ACTION_PROMPT_EVAN_PART1_SECOND_HALF) &&
                (msgCtx->ocarinaAction != OCARINA_ACTION_PROMPT_EVAN_PART2_SECOND_HALF)) {
                if (Message_ShouldAdvance(play)) {
                    Message_DisplayOcarinaStaff(play, msgCtx->ocarinaAction);
                }
            }
            break;

        case MSGMODE_SCARECROW_SPAWN_RECORDING_ONGOING:
            if (CHECK_BTN_ALL(input->press.button, BTN_B)) {
                AudioOcarina_SetRecordingState(OCARINA_RECORD_OFF);
                Audio_PlaySfx(NA_SE_SY_OCARINA_ERROR);
                Message_CloseTextbox(play);
                msgCtx->msgMode = MSGMODE_SCARECROW_SPAWN_RECORDING_FAILED;
            } else {
                msgCtx->ocarinaButtonIndex = OCARINA_BTN_INVALID;
            }
            break;

        case MSGMODE_SCENE_TITLE_CARD_FADE_IN_BACKGROUND:
            msgCtx->textboxColorAlphaCurrent += XREG(73);
            if (msgCtx->textboxColorAlphaCurrent >= 255) {
                msgCtx->textboxColorAlphaCurrent = 255;
                msgCtx->msgMode = MSGMODE_SCENE_TITLE_CARD_FADE_IN_TEXT;
            }
            break;

        case MSGMODE_SCENE_TITLE_CARD_FADE_IN_TEXT:
            msgCtx->textColorAlpha += XREG(73);
            if (msgCtx->textColorAlpha >= 255) {
                msgCtx->textColorAlpha = 255;
                msgCtx->msgMode = MSGMODE_SCENE_TITLE_CARD_DISPLAYING;
            }
            break;

        case MSGMODE_SCENE_TITLE_CARD_DISPLAYING:
            msgCtx->stateTimer--;
            if (msgCtx->stateTimer == 0) {
                msgCtx->msgMode = MSGMODE_SCENE_TITLE_CARD_FADE_OUT_TEXT;
            }
            break;

        case MSGMODE_SCENE_TITLE_CARD_FADE_OUT_TEXT:
            msgCtx->textColorAlpha -= XREG(70);
            if (msgCtx->textColorAlpha <= 0) {
                msgCtx->textColorAlpha = 0;
                msgCtx->msgMode = MSGMODE_SCENE_TITLE_CARD_FADE_OUT_BACKGROUND;
            }
            break;

        case MSGMODE_SCENE_TITLE_CARD_FADE_OUT_BACKGROUND:
            msgCtx->textboxColorAlphaCurrent -= XREG(70);
            if (msgCtx->textboxColorAlphaCurrent <= 0) {
                if ((msgCtx->currentTextId >= 0x1BB2) && (msgCtx->currentTextId <= 0x1BB6) &&
                    (play->actorCtx.flags & ACTORCTX_FLAG_TELESCOPE_ON)) {
                    Message_StartTextbox(play, 0x5E6, NULL);
                    Interface_SetHudVisibility(HUD_VISIBILITY_NONE_ALT);
                } else {
                    //! FAKE: debug?
                    if (msgCtx->currentTextId >= 0x100) {
                        if (msgCtx && msgCtx && msgCtx) {}
                    }
                    msgCtx->textboxColorAlphaCurrent = 0;
                    msgCtx->msgLength = 0;
                    msgCtx->msgMode = MSGMODE_NONE;
                    msgCtx->currentTextId = 0;
                    msgCtx->stateTimer = 0;
                }
            }
            break;

        case MSGMODE_NEW_CYCLE_0:
            play->state.unk_A3 = 1;
            sp44 = gSaveContext.save.cutsceneIndex;
            sp3E = CURRENT_TIME;
            sp40 = gSaveContext.save.day;

            Sram_SaveEndOfCycle(play);
            gSaveContext.timerStates[TIMER_ID_MOON_CRASH] = TIMER_STATE_OFF;
            func_8014546C(&play->sramCtx);

            gSaveContext.save.day = sp40;
            gSaveContext.save.time = sp3E;
            gSaveContext.save.cutsceneIndex = sp44;

            if (gSaveContext.fileNum != 0xFF) {
                Sram_SetFlashPagesDefault(&play->sramCtx,
                                          gFlashSaveStartPages[gSaveContext.fileNum * FLASH_SAVE_MAIN_MULTIPLIER],
                                          gFlashSpecialSaveNumPages[gSaveContext.fileNum * FLASH_SAVE_MAIN_MULTIPLIER]);
                Sram_StartWriteToFlashDefault(&play->sramCtx);
            }
            msgCtx->msgMode = MSGMODE_NEW_CYCLE_1;
            break;

        case MSGMODE_NEW_CYCLE_1:
            if (gSaveContext.fileNum != 0xFF) {
                play->state.unk_A3 = 1;
                if (play->sramCtx.status == 0) {
                    play->msgCtx.ocarinaMode = OCARINA_MODE_APPLY_SOT;
                    msgCtx->msgMode = MSGMODE_NEW_CYCLE_2;
                }
            } else {
                play->msgCtx.ocarinaMode = OCARINA_MODE_APPLY_SOT;
                msgCtx->msgMode = MSGMODE_NEW_CYCLE_2;
            }
            break;

        case MSGMODE_OWL_SAVE_0:
            play->state.unk_A3 = 1;
            gSaveContext.save.isOwlSave = true;
            Play_SaveCycleSceneFlags(play);
            func_8014546C(&play->sramCtx);

            if (gSaveContext.fileNum != 0xFF) {
                Sram_SetFlashPagesOwlSave(&play->sramCtx,
                                          gFlashOwlSaveStartPages[gSaveContext.fileNum * FLASH_SAVE_MAIN_MULTIPLIER],
                                          gFlashOwlSaveNumPages[gSaveContext.fileNum * FLASH_SAVE_MAIN_MULTIPLIER]);
                Sram_StartWriteToFlashOwlSave(&play->sramCtx);
            }
            msgCtx->msgMode = MSGMODE_OWL_SAVE_1;
            break;

        case MSGMODE_OWL_SAVE_1:
            if (gSaveContext.fileNum != 0xFF) {
                play->state.unk_A3 = 1;
                if (play->sramCtx.status == 0) {
                    play->msgCtx.ocarinaMode = OCARINA_MODE_APPLY_SOT;
                    msgCtx->msgMode = MSGMODE_OWL_SAVE_2;
                }
            } else {
                play->msgCtx.ocarinaMode = OCARINA_MODE_APPLY_SOT;
                msgCtx->msgMode = MSGMODE_OWL_SAVE_2;
            }

            if (msgCtx->msgMode == MSGMODE_OWL_SAVE_2) {
                gSaveContext.gameMode = GAMEMODE_OWL_SAVE;
                play->transitionTrigger = TRANS_TRIGGER_START;
                play->transitionType = TRANS_TYPE_FADE_BLACK;
                play->nextEntrance = ENTRANCE(CUTSCENE, 0);
                gSaveContext.save.cutsceneIndex = 0;
                gSaveContext.sceneLayer = 0;
            }
            break;

        case MSGMODE_9:
        case MSGMODE_PAUSED:
        case MSGMODE_NEW_CYCLE_2:
        case MSGMODE_OWL_SAVE_2:
            break;

        default:
            msgCtx->ocarinaButtonIndex = OCARINA_BTN_INVALID;
            break;
    }
}

void Message_SetTables(PlayState* play) {
    play->msgCtx.messageTableNES = sMessageTableNES;
    play->msgCtx.messageTableCredits = sMessageTableCredits;
}

void Message_Init(PlayState* play) {
    Font* font;
    MessageContext* msgCtx = &play->msgCtx;

    Message_SetTables(play);

    play->msgCtx.ocarinaMode = OCARINA_MODE_NONE;

    msgCtx->msgMode = MSGMODE_NONE;
    msgCtx->msgLength = 0;
    msgCtx->currentTextId = 0;
    msgCtx->textboxEndType = TEXTBOX_ENDTYPE_DEFAULT;
    msgCtx->choiceIndex = 0;
    msgCtx->ocarinaAction = msgCtx->textUnskippable = 0;
    msgCtx->textColorAlpha = 255;

    View_Init(&msgCtx->view, play->state.gfxCtx);

    // #region 2S2H [Port]
    msgCtx->textboxSegment = THA_AllocTailAlign16(&play->state.tha, sizeof(char*) * TEXTBOX_SEG_MAX);
    for (size_t id = 0; id < TEXTBOX_SEG_MAX; id++) {
        msgCtx->textboxSegment[id] = gEmptyTexture;
    }
    // #endregion

    font = &play->msgCtx.font;
    Font_LoadOrderedFont(&play->msgCtx.font);
    font->unk_11D88 = 0;

    msgCtx->textIsCredits = msgCtx->messageHasSetSfx = false;
    msgCtx->textboxSkipped = false;
    msgCtx->textFade = false;
    msgCtx->ocarinaAvailableSongs = 0;
    msgCtx->textboxX = 52;
    msgCtx->textboxY = 36;
    msgCtx->ocarinaSongEffectActive = false;
    msgCtx->unk120BE = 0;
    msgCtx->unk120C0 = 0;
    msgCtx->unk120C2 = 0;
    msgCtx->unk120C4 = 0;
    msgCtx->unk120C8 = 0;
    msgCtx->unk120CA = 0;
    msgCtx->unk120CC = 0;
    msgCtx->unk120CE = 0;
    msgCtx->unk120D0 = 0;
    msgCtx->unk120D2 = 0;
    msgCtx->unk120D4 = 0;
    msgCtx->unk120D6 = 0;
}
