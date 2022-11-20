#include "global.h"
#include "vt.h"
#include "textures/parameter_static/parameter_static.h"
#include "textures/do_action_static/do_action_static.h"
#include "textures/icon_item_static/icon_item_static.h"
#include "soh/Enhancements/randomizer/adult_trade_shuffle.h"
#include "soh/Enhancements/randomizer/randomizer_entrance.h"

#ifdef _MSC_VER
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#endif

#include "soh/Enhancements/debugconsole.h"


static uint16_t _doActionTexWidth, _doActionTexHeight = -1;
static uint16_t DO_ACTION_TEX_WIDTH() {
    return 48;

    // TODO: Figure out why Ship::Texture is not returning a valid width
    if (_doActionTexWidth == -1)
        _doActionTexWidth = ResourceMgr_LoadTexWidthByName(gCheckDoActionENGTex);
    return _doActionTexWidth;
}
static uint16_t DO_ACTION_TEX_HEIGHT() {
    return 16;

    // TODO: Figure out why Ship::Texture is not returning a valid height
    if (_doActionTexHeight == -1)
        _doActionTexHeight = ResourceMgr_LoadTexHeightByName(gCheckDoActionENGTex);
    return _doActionTexHeight;
}

static uint32_t _doActionTexSize = -1;
static uint32_t DO_ACTION_TEX_SIZE() {
    if (_doActionTexSize == -1)
        _doActionTexSize = ResourceMgr_LoadTexSizeByName(gCheckDoActionENGTex);
    return _doActionTexSize;
}

// The button statuses include the A button when most things are only the equip item buttons
// So, when indexing into it with a item button index, we need to adjust
#define BUTTON_STATUS_INDEX(button) ((button) >= 4) ? ((button) + 1) : (button)

s16 Top_HUD_Margin = 0;
s16 Left_HUD_Margin = 0;
s16 Right_HUD_Margin = 0;
s16 Bottom_HUD_Margin = 0;

typedef struct {
    /* 0x00 */ u8 scene;
    /* 0x01 */ u8 flags1;
    /* 0x02 */ u8 flags2;
    /* 0x03 */ u8 flags3;
} RestrictionFlags;

static RestrictionFlags sRestrictionFlags[] = {
    { SCENE_SPOT00, 0x00, 0x00, 0x10 },
    { SCENE_SPOT01, 0x00, 0x00, 0x10 },
    { SCENE_SPOT02, 0x00, 0x00, 0x10 },
    { SCENE_SPOT03, 0x00, 0x00, 0x10 },
    { SCENE_SPOT04, 0x00, 0x00, 0x10 },
    { SCENE_SPOT05, 0x00, 0x00, 0x10 },
    { SCENE_SPOT06, 0x00, 0x00, 0x10 },
    { SCENE_SPOT07, 0x00, 0x00, 0x10 },
    { SCENE_SPOT08, 0x00, 0x00, 0x10 },
    { SCENE_SPOT09, 0x00, 0x00, 0x10 },
    { SCENE_SPOT10, 0x00, 0x00, 0x10 },
    { SCENE_SPOT11, 0x00, 0x00, 0x10 },
    { SCENE_SPOT12, 0x00, 0x00, 0x10 },
    { SCENE_SPOT13, 0x00, 0x00, 0x10 },
    { SCENE_SPOT15, 0x00, 0x00, 0x10 },
    { SCENE_GANON_TOU, 0x00, 0x00, 0x10 },
    { SCENE_SPOT16, 0x00, 0x00, 0x10 },
    { SCENE_SPOT17, 0x00, 0x00, 0x10 },
    { SCENE_SPOT18, 0x00, 0x00, 0x10 },
    { SCENE_SPOT20, 0x00, 0x00, 0x10 },
    { SCENE_TOKINOMA, 0x00, 0x10, 0x15 },
    { SCENE_KENJYANOMA, 0xA2, 0xAA, 0xAA },
    { SCENE_SYATEKIJYOU, 0x11, 0x55, 0x55 },
    { SCENE_HAIRAL_NIWA, 0x11, 0x55, 0x55 },
    { SCENE_HAIRAL_NIWA_N, 0x11, 0x55, 0x55 },
    { SCENE_HAKAANA, 0x00, 0x00, 0xD0 },
    { SCENE_HAKAANA2, 0x00, 0x00, 0xD0 },
    { SCENE_HAKAANA_OUKE, 0x00, 0x00, 0xD0 },
    { SCENE_DAIYOUSEI_IZUMI, 0x00, 0x00, 0x10 },
    { SCENE_YOUSEI_IZUMI_TATE, 0x00, 0x00, 0xD0 },
    { SCENE_YOUSEI_IZUMI_YOKO, 0x00, 0x00, 0x10 },
    { SCENE_GANON_FINAL, 0x00, 0x05, 0x50 },
    { SCENE_NAKANIWA, 0x00, 0x05, 0x54 },
    { SCENE_TURIBORI, 0x11, 0x55, 0x55 },
    { SCENE_BOWLING, 0x11, 0x55, 0x55 },
    { SCENE_SOUKO, 0x00, 0x10, 0x15 },
    { SCENE_MIHARIGOYA, 0x00, 0x10, 0x14 },
    { SCENE_MAHOUYA, 0x10, 0x15, 0x55 },
    { SCENE_TAKARAYA, 0x10, 0x15, 0x55 },
    { SCENE_KINSUTA, 0x00, 0x10, 0x15 },
    { SCENE_ENTRA, 0x00, 0x10, 0x15 },
    { SCENE_ENTRA_N, 0x00, 0x10, 0x15 },
    { SCENE_ENRUI, 0x00, 0x10, 0xD5 },
    { SCENE_MARKET_DAY, 0x00, 0x10, 0x15 },
    { SCENE_MARKET_NIGHT, 0x00, 0x10, 0x15 },
    { SCENE_MARKET_RUINS, 0x00, 0x10, 0xD5 },
    { SCENE_MARKET_ALLEY, 0x00, 0x10, 0x15 },
    { SCENE_MARKET_ALLEY_N, 0x00, 0x10, 0x15 },
    { SCENE_SHRINE, 0x00, 0x10, 0x15 },
    { SCENE_SHRINE_N, 0x00, 0x10, 0x15 },
    { SCENE_SHRINE_R, 0x00, 0x10, 0xD5 },
    { SCENE_LINK_HOME, 0x10, 0x10, 0x15 },
    { SCENE_KAKARIKO, 0x10, 0x10, 0x15 },
    { SCENE_KAKARIKO3, 0x10, 0x10, 0x15 },
    { SCENE_KOKIRI_HOME, 0x10, 0x10, 0x15 },
    { SCENE_KOKIRI_HOME3, 0x10, 0x10, 0x15 },
    { SCENE_KOKIRI_HOME4, 0x10, 0x10, 0x15 },
    { SCENE_KOKIRI_HOME5, 0x10, 0x10, 0x15 },
    { SCENE_MALON_STABLE, 0x10, 0x10, 0x15 },
    { SCENE_HUT, 0x10, 0x10, 0x15 },
    { SCENE_IMPA, 0x10, 0x10, 0x15 },
    { SCENE_LABO, 0x10, 0x10, 0x15 },
    { SCENE_HYLIA_LABO, 0x00, 0x10, 0x15 },
    { SCENE_TENT, 0x10, 0x10, 0x15 },
    { SCENE_SHOP1, 0x10, 0x10, 0x15 },
    { SCENE_KOKIRI_SHOP, 0x10, 0x10, 0x15 },
    { SCENE_GOLON, 0x10, 0x10, 0x15 },
    { SCENE_ZOORA, 0x10, 0x10, 0x15 },
    { SCENE_DRAG, 0x10, 0x10, 0x15 },
    { SCENE_ALLEY_SHOP, 0x10, 0x10, 0x15 },
    { SCENE_NIGHT_SHOP, 0x10, 0x10, 0x15 },
    { SCENE_FACE_SHOP, 0x10, 0x10, 0x15 },
    { SCENE_MEN, 0x00, 0x03, 0x10 },
    { SCENE_YDAN, 0x00, 0x00, 0x00 },
    { SCENE_YDAN_BOSS, 0x00, 0x45, 0x50 },
    { SCENE_DDAN, 0x00, 0x00, 0x00 },
    { SCENE_DDAN_BOSS, 0x00, 0x45, 0x50 },
    { SCENE_BDAN, 0x00, 0x00, 0x00 },
    { SCENE_BDAN_BOSS, 0x00, 0x45, 0x50 },
    { SCENE_BMORI1, 0x00, 0x00, 0x00 },
    { SCENE_MORIBOSSROOM, 0x00, 0x45, 0x50 },
    { SCENE_HAKADANCH, 0x00, 0x00, 0x00 },
    { SCENE_HAKADAN, 0x00, 0x00, 0x00 },
    { SCENE_HAKADAN_BS, 0x00, 0x45, 0x50 },
    { SCENE_HIDAN, 0x00, 0x00, 0x00 },
    { SCENE_FIRE_BS, 0x00, 0x45, 0x50 },
    { SCENE_MIZUSIN, 0x00, 0x00, 0x00 },
    { SCENE_MIZUSIN_BS, 0x00, 0x45, 0x50 },
    { SCENE_JYASINZOU, 0x00, 0x00, 0x00 },
    { SCENE_JYASINBOSS, 0x00, 0x45, 0x50 },
    { SCENE_GANON, 0x00, 0x00, 0x00 },
    { SCENE_GANON_BOSS, 0x00, 0x45, 0x50 },
    { SCENE_ICE_DOUKUTO, 0x00, 0x00, 0xC0 },
    { SCENE_HAKASITARELAY, 0x00, 0x03, 0x14 },
    { SCENE_GANONTIKA, 0x00, 0x03, 0x10 },
    { SCENE_GANON_DEMO, 0x00, 0x45, 0x50 },
    { SCENE_GANON_SONOGO, 0x00, 0x05, 0x50 },
    { SCENE_GANONTIKA_SONOGO, 0x00, 0x05, 0x50 },
    { SCENE_GERUDOWAY, 0x00, 0x00, 0x10 },
    { SCENE_KAKUSIANA, 0x00, 0x00, 0xD0 },
    { 0xFF, 0x00, 0x00, 0x00 },
};

static s16 sHBAScoreTier = 0;
static u16 sHBAScoreDigits[] = { 0, 0, 0, 0 };

static u16 sCUpInvisible = 0;
static u16 sCUpTimer = 0;

s16 gSpoilingItems[] = { ITEM_ODD_MUSHROOM, ITEM_FROG, ITEM_EYEDROPS };
s16 gSpoilingItemReverts[] = { ITEM_COJIRO, ITEM_PRESCRIPTION, ITEM_PRESCRIPTION };

static Color_RGB8 sMagicBorder = {255,255,255};
static Color_RGB8 sMagicBorder_ori = {255,255,255};

static s16 sExtraItemBases[] = {
    ITEM_STICK, ITEM_STICK, ITEM_NUT,   ITEM_NUT,     ITEM_BOMB,    ITEM_BOMB,  ITEM_BOMB,  ITEM_BOMB, ITEM_BOW,
    ITEM_BOW,   ITEM_BOW,   ITEM_SEEDS, ITEM_BOMBCHU, ITEM_BOMBCHU, ITEM_STICK, ITEM_STICK, ITEM_NUT,  ITEM_NUT,
};

static s16 D_80125A58 = 0;
static s16 D_80125A5C = 0;

static Gfx sSetupDL_80125A60[] = {
    gsDPPipeSync(),
    gsSPClearGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BOTH | G_FOG | G_LIGHTING | G_TEXTURE_GEN |
                          G_TEXTURE_GEN_LINEAR | G_SHADING_SMOOTH | G_LOD),
    gsDPSetOtherMode(G_AD_DISABLE | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                         G_TD_CLAMP | G_TP_NONE | G_CYC_1CYCLE | G_PM_1PRIMITIVE,
                     G_AC_NONE | G_ZS_PIXEL | G_RM_CLD_SURF | G_RM_CLD_SURF2),
    gsDPSetCombineMode(G_CC_PRIMITIVE, G_CC_PRIMITIVE),
    gsSPEndDisplayList(),
};

static const char* actionsTbl[] =
{
    gAttackDoActionENGTex,
    gCheckDoActionENGTex,
    gEnterDoActionENGTex,
    gReturnDoActionENGTex,
    gOpenDoActionENGTex,
    gJumpDoActionENGTex,
    gDecideDoActionENGTex,
    gDiveDoActionENGTex,
    gFasterDoActionENGTex,
    gThrowDoActionENGTex,
    gUnusedNaviDoActionENGTex,
    gClimbDoActionENGTex,
    gDropDoActionENGTex,
    gDownDoActionENGTex,
    gSaveDoActionENGTex,
    gSpeakDoActionENGTex,
    gNextDoActionENGTex,
    gGrabDoActionENGTex,
    gStopDoActionENGTex,
    gPutAwayDoActionENGTex,
    gReelDoActionENGTex,
    gNum1DoActionENGTex,
    gNum2DoActionENGTex,
    gNum3DoActionENGTex,
    gNum4DoActionENGTex,
    gNum5DoActionENGTex,
    gNum6DoActionENGTex,
    gNum7DoActionENGTex,
    gNum8DoActionENGTex,
};

// original name: "alpha_change"
void Interface_ChangeAlpha(u16 alphaType) {
    if (alphaType != gSaveContext.unk_13EA) {
        osSyncPrintf("ＡＬＰＨＡーＴＹＰＥ＝%d  LAST_TIME_TYPE=%d\n", alphaType, gSaveContext.unk_13EE);
        gSaveContext.unk_13EA = gSaveContext.unk_13E8 = alphaType;
        gSaveContext.unk_13EC = 1;
    }
}

void func_80082644(PlayState* play, s16 alpha) {
    InterfaceContext* interfaceCtx = &play->interfaceCtx;

    if (gSaveContext.buttonStatus[0] == BTN_DISABLED) {
        if (interfaceCtx->bAlpha != 70) {
            interfaceCtx->bAlpha = 70;
        }
    } else {
        if (interfaceCtx->bAlpha != 255) {
            interfaceCtx->bAlpha = alpha;
        }
    }

    if (gSaveContext.buttonStatus[1] == BTN_DISABLED) {
        if (interfaceCtx->cLeftAlpha != 70) {
            interfaceCtx->cLeftAlpha = 70;
        }
    } else {
        if (interfaceCtx->cLeftAlpha != 255) {
            interfaceCtx->cLeftAlpha = alpha;
        }
    }

    if (gSaveContext.buttonStatus[2] == BTN_DISABLED) {
        if (interfaceCtx->cDownAlpha != 70) {
            interfaceCtx->cDownAlpha = 70;
        }
    } else {
        if (interfaceCtx->cDownAlpha != 255) {
            interfaceCtx->cDownAlpha = alpha;
        }
    }

    if (gSaveContext.buttonStatus[3] == BTN_DISABLED) {
        if (interfaceCtx->cRightAlpha != 70) {
            interfaceCtx->cRightAlpha = 70;
        }
    } else {
        if (interfaceCtx->cRightAlpha != 255) {
            interfaceCtx->cRightAlpha = alpha;
        }
    }

    if (gSaveContext.buttonStatus[4] == BTN_DISABLED) {
        if (interfaceCtx->aAlpha != 70) {
            interfaceCtx->aAlpha = 70;
        }
    } else {
        if (interfaceCtx->aAlpha != 255) {
            interfaceCtx->aAlpha = alpha;
        }
    }

    if (gSaveContext.buttonStatus[5] == BTN_DISABLED) {
        if (interfaceCtx->dpadUpAlpha != 70) {
            interfaceCtx->dpadUpAlpha = 70;
        }
    } else {
        if (interfaceCtx->dpadUpAlpha != 255) {
            interfaceCtx->dpadUpAlpha = alpha;
        }
    }

    if (gSaveContext.buttonStatus[6] == BTN_DISABLED) {
        if (interfaceCtx->dpadDownAlpha != 70) {
            interfaceCtx->dpadDownAlpha = 70;
        }
    } else {
        if (interfaceCtx->dpadDownAlpha != 255) {
            interfaceCtx->dpadDownAlpha = alpha;
        }
    }

    if (gSaveContext.buttonStatus[7] == BTN_DISABLED) {
        if (interfaceCtx->dpadLeftAlpha != 70) {
            interfaceCtx->dpadLeftAlpha = 70;
        }
    } else {
        if (interfaceCtx->dpadLeftAlpha != 255) {
            interfaceCtx->dpadLeftAlpha = alpha;
        }
    }

    if (gSaveContext.buttonStatus[8] == BTN_DISABLED) {
        if (interfaceCtx->dpadRightAlpha != 70) {
            interfaceCtx->dpadRightAlpha = 70;
        }
    } else {
        if (interfaceCtx->dpadRightAlpha != 255) {
            interfaceCtx->dpadRightAlpha = alpha;
        }
    }
}

void func_8008277C(PlayState* play, s16 maxAlpha, s16 alpha) {
    InterfaceContext* interfaceCtx = &play->interfaceCtx;

    if (gSaveContext.unk_13E7 != 0) {
        func_80082644(play, alpha);
        return;
    }

    if ((interfaceCtx->bAlpha != 0) && (interfaceCtx->bAlpha > maxAlpha)) {
        interfaceCtx->bAlpha = maxAlpha;
    }

    if ((interfaceCtx->aAlpha != 0) && (interfaceCtx->aAlpha > maxAlpha)) {
        interfaceCtx->aAlpha = maxAlpha;
    }

    if ((interfaceCtx->cLeftAlpha != 0) && (interfaceCtx->cLeftAlpha > maxAlpha)) {
        interfaceCtx->cLeftAlpha = maxAlpha;
    }

    if ((interfaceCtx->cDownAlpha != 0) && (interfaceCtx->cDownAlpha > maxAlpha)) {
        interfaceCtx->cDownAlpha = maxAlpha;
    }

    if ((interfaceCtx->cRightAlpha != 0) && (interfaceCtx->cRightAlpha > maxAlpha)) {
        interfaceCtx->cRightAlpha = maxAlpha;
    }

    if ((interfaceCtx->dpadUpAlpha != 0) && (interfaceCtx->dpadUpAlpha > maxAlpha)) {
        interfaceCtx->dpadUpAlpha = maxAlpha;
    }

    if ((interfaceCtx->dpadDownAlpha != 0) && (interfaceCtx->dpadDownAlpha > maxAlpha)) {
        interfaceCtx->dpadDownAlpha = maxAlpha;
    }

    if ((interfaceCtx->dpadLeftAlpha != 0) && (interfaceCtx->dpadLeftAlpha > maxAlpha)) {
        interfaceCtx->dpadLeftAlpha = maxAlpha;
    }

    if ((interfaceCtx->dpadRightAlpha != 0) && (interfaceCtx->dpadRightAlpha > maxAlpha)) {
        interfaceCtx->dpadRightAlpha = maxAlpha;
    }
}

void func_80082850(PlayState* play, s16 maxAlpha) {
    InterfaceContext* interfaceCtx = &play->interfaceCtx;
    s16 alpha = 255 - maxAlpha;

    switch (gSaveContext.unk_13E8) {
        case 1:
        case 2:
        case 8:
            osSyncPrintf("a_alpha=%d, c_alpha=%d   →   ", interfaceCtx->aAlpha, interfaceCtx->cLeftAlpha);

            if (gSaveContext.unk_13E8 == 8) {
                if (interfaceCtx->bAlpha != 255) {
                    interfaceCtx->bAlpha = alpha;
                }
            } else {
                if ((interfaceCtx->bAlpha != 0) && (interfaceCtx->bAlpha > maxAlpha)) {
                    interfaceCtx->bAlpha = maxAlpha;
                }
            }

            if ((interfaceCtx->aAlpha != 0) && (interfaceCtx->aAlpha > maxAlpha)) {
                interfaceCtx->aAlpha = maxAlpha;
            }

            if ((interfaceCtx->cLeftAlpha != 0) && (interfaceCtx->cLeftAlpha > maxAlpha)) {
                interfaceCtx->cLeftAlpha = maxAlpha;
            }

            if ((interfaceCtx->cDownAlpha != 0) && (interfaceCtx->cDownAlpha > maxAlpha)) {
                interfaceCtx->cDownAlpha = maxAlpha;
            }

            if ((interfaceCtx->cRightAlpha != 0) && (interfaceCtx->cRightAlpha > maxAlpha)) {
                interfaceCtx->cRightAlpha = maxAlpha;
            }

            if ((interfaceCtx->dpadUpAlpha != 0) && (interfaceCtx->dpadUpAlpha > maxAlpha)) {
                interfaceCtx->dpadUpAlpha = maxAlpha;
            }

            if ((interfaceCtx->dpadDownAlpha != 0) && (interfaceCtx->dpadDownAlpha > maxAlpha)) {
                interfaceCtx->dpadDownAlpha = maxAlpha;
            }

            if ((interfaceCtx->dpadLeftAlpha != 0) && (interfaceCtx->dpadLeftAlpha > maxAlpha)) {
                interfaceCtx->dpadLeftAlpha = maxAlpha;
            }

            if ((interfaceCtx->dpadRightAlpha != 0) && (interfaceCtx->dpadRightAlpha > maxAlpha)) {
                interfaceCtx->dpadRightAlpha = maxAlpha;
            }

            if ((interfaceCtx->healthAlpha != 0) && (interfaceCtx->healthAlpha > maxAlpha)) {
                interfaceCtx->healthAlpha = maxAlpha;
            }

            if ((interfaceCtx->magicAlpha != 0) && (interfaceCtx->magicAlpha > maxAlpha)) {
                interfaceCtx->magicAlpha = maxAlpha;
            }

            if ((interfaceCtx->minimapAlpha != 0) && (interfaceCtx->minimapAlpha > maxAlpha)) {
                interfaceCtx->minimapAlpha = maxAlpha;
            }

            osSyncPrintf("a_alpha=%d, c_alpha=%d\n", interfaceCtx->aAlpha, interfaceCtx->cLeftAlpha);

            break;
        case 3:
            if ((interfaceCtx->aAlpha != 0) && (interfaceCtx->aAlpha > maxAlpha)) {
                interfaceCtx->aAlpha = maxAlpha;
            }

            func_8008277C(play, maxAlpha, alpha);

            if ((interfaceCtx->magicAlpha != 0) && (interfaceCtx->magicAlpha > maxAlpha)) {
                interfaceCtx->magicAlpha = maxAlpha;
            }

            if ((interfaceCtx->minimapAlpha != 0) && (interfaceCtx->minimapAlpha > maxAlpha)) {
                interfaceCtx->minimapAlpha = maxAlpha;
            }

            if (interfaceCtx->healthAlpha != 255) {
                interfaceCtx->healthAlpha = alpha;
            }

            break;
        case 4:
            if ((interfaceCtx->bAlpha != 0) && (interfaceCtx->bAlpha > maxAlpha)) {
                interfaceCtx->bAlpha = maxAlpha;
            }

            if ((interfaceCtx->aAlpha != 0) && (interfaceCtx->aAlpha > maxAlpha)) {
                interfaceCtx->aAlpha = maxAlpha;
            }

            if ((interfaceCtx->cLeftAlpha != 0) && (interfaceCtx->cLeftAlpha > maxAlpha)) {
                interfaceCtx->cLeftAlpha = maxAlpha;
            }

            if ((interfaceCtx->cDownAlpha != 0) && (interfaceCtx->cDownAlpha > maxAlpha)) {
                interfaceCtx->cDownAlpha = maxAlpha;
            }

            if ((interfaceCtx->cRightAlpha != 0) && (interfaceCtx->cRightAlpha > maxAlpha)) {
                interfaceCtx->cRightAlpha = maxAlpha;
            }

            if ((interfaceCtx->dpadUpAlpha != 0) && (interfaceCtx->dpadUpAlpha > maxAlpha)) {
                interfaceCtx->dpadUpAlpha = maxAlpha;
            }

            if ((interfaceCtx->dpadDownAlpha != 0) && (interfaceCtx->dpadDownAlpha > maxAlpha)) {
                interfaceCtx->dpadDownAlpha = maxAlpha;
            }

            if ((interfaceCtx->dpadLeftAlpha != 0) && (interfaceCtx->dpadLeftAlpha > maxAlpha)) {
                interfaceCtx->dpadLeftAlpha = maxAlpha;
            }

            if ((interfaceCtx->dpadRightAlpha != 0) && (interfaceCtx->dpadRightAlpha > maxAlpha)) {
                interfaceCtx->dpadRightAlpha = maxAlpha;
            }

            if ((interfaceCtx->healthAlpha != 0) && (interfaceCtx->healthAlpha > maxAlpha)) {
                interfaceCtx->healthAlpha = maxAlpha;
            }

            if ((interfaceCtx->magicAlpha != 0) && (interfaceCtx->magicAlpha > maxAlpha)) {
                interfaceCtx->magicAlpha = maxAlpha;
            }

            if ((interfaceCtx->minimapAlpha != 0) && (interfaceCtx->minimapAlpha > maxAlpha)) {
                interfaceCtx->minimapAlpha = maxAlpha;
            }

            if (interfaceCtx->aAlpha != 255) {
                interfaceCtx->aAlpha = alpha;
            }

            break;
        case 5:
            func_8008277C(play, maxAlpha, alpha);

            if ((interfaceCtx->minimapAlpha != 0) && (interfaceCtx->minimapAlpha > maxAlpha)) {
                interfaceCtx->minimapAlpha = maxAlpha;
            }

            if (interfaceCtx->aAlpha != 255) {
                interfaceCtx->aAlpha = alpha;
            }

            if (interfaceCtx->healthAlpha != 255) {
                interfaceCtx->healthAlpha = alpha;
            }

            if (interfaceCtx->magicAlpha != 255) {
                interfaceCtx->magicAlpha = alpha;
            }

            break;
        case 6:
            func_8008277C(play, maxAlpha, alpha);

            if (interfaceCtx->aAlpha != 255) {
                interfaceCtx->aAlpha = alpha;
            }

            if (interfaceCtx->healthAlpha != 255) {
                interfaceCtx->healthAlpha = alpha;
            }

            if (interfaceCtx->magicAlpha != 255) {
                interfaceCtx->magicAlpha = alpha;
            }

            switch (play->sceneNum) {
                case SCENE_SPOT00:
                case SCENE_SPOT01:
                case SCENE_SPOT02:
                case SCENE_SPOT03:
                case SCENE_SPOT04:
                case SCENE_SPOT05:
                case SCENE_SPOT06:
                case SCENE_SPOT07:
                case SCENE_SPOT08:
                case SCENE_SPOT09:
                case SCENE_SPOT10:
                case SCENE_SPOT11:
                case SCENE_SPOT12:
                case SCENE_SPOT13:
                case SCENE_SPOT15:
                case SCENE_SPOT16:
                case SCENE_SPOT17:
                case SCENE_SPOT18:
                case SCENE_SPOT20:
                case SCENE_GANON_TOU:
                    if (interfaceCtx->minimapAlpha < 170) {
                        interfaceCtx->minimapAlpha = alpha;
                    } else {
                        interfaceCtx->minimapAlpha = 170;
                    }
                    break;
                default:
                    if (interfaceCtx->minimapAlpha != 255) {
                        interfaceCtx->minimapAlpha = alpha;
                    }
                    break;
            }
            break;
        case 7:
            if ((interfaceCtx->minimapAlpha != 0) && (interfaceCtx->minimapAlpha > maxAlpha)) {
                interfaceCtx->minimapAlpha = maxAlpha;
            }

            func_80082644(play, alpha);

            if (interfaceCtx->healthAlpha != 255) {
                interfaceCtx->healthAlpha = alpha;
            }

            if (interfaceCtx->magicAlpha != 255) {
                interfaceCtx->magicAlpha = alpha;
            }

            break;
        case 9:
            if ((interfaceCtx->bAlpha != 0) && (interfaceCtx->bAlpha > maxAlpha)) {
                interfaceCtx->bAlpha = maxAlpha;
            }

            if ((interfaceCtx->aAlpha != 0) && (interfaceCtx->aAlpha > maxAlpha)) {
                interfaceCtx->aAlpha = maxAlpha;
            }

            if ((interfaceCtx->cLeftAlpha != 0) && (interfaceCtx->cLeftAlpha > maxAlpha)) {
                interfaceCtx->cLeftAlpha = maxAlpha;
            }

            if ((interfaceCtx->cDownAlpha != 0) && (interfaceCtx->cDownAlpha > maxAlpha)) {
                interfaceCtx->cDownAlpha = maxAlpha;
            }

            if ((interfaceCtx->cRightAlpha != 0) && (interfaceCtx->cRightAlpha > maxAlpha)) {
                interfaceCtx->cRightAlpha = maxAlpha;
            }

            if ((interfaceCtx->dpadUpAlpha != 0) && (interfaceCtx->dpadUpAlpha > maxAlpha)) {
                interfaceCtx->dpadUpAlpha = maxAlpha;
            }

            if ((interfaceCtx->dpadDownAlpha != 0) && (interfaceCtx->dpadDownAlpha > maxAlpha)) {
                interfaceCtx->dpadDownAlpha = maxAlpha;
            }

            if ((interfaceCtx->dpadLeftAlpha != 0) && (interfaceCtx->dpadLeftAlpha > maxAlpha)) {
                interfaceCtx->dpadLeftAlpha = maxAlpha;
            }

            if ((interfaceCtx->dpadRightAlpha != 0) && (interfaceCtx->dpadRightAlpha > maxAlpha)) {
                interfaceCtx->dpadRightAlpha = maxAlpha;
            }

            if ((interfaceCtx->minimapAlpha != 0) && (interfaceCtx->minimapAlpha > maxAlpha)) {
                interfaceCtx->minimapAlpha = maxAlpha;
            }

            if (interfaceCtx->healthAlpha != 255) {
                interfaceCtx->healthAlpha = alpha;
            }

            if (interfaceCtx->magicAlpha != 255) {
                interfaceCtx->magicAlpha = alpha;
            }

            break;
        case 10:
            if ((interfaceCtx->aAlpha != 0) && (interfaceCtx->aAlpha > maxAlpha)) {
                interfaceCtx->aAlpha = maxAlpha;
            }

            if ((interfaceCtx->cLeftAlpha != 0) && (interfaceCtx->cLeftAlpha > maxAlpha)) {
                interfaceCtx->cLeftAlpha = maxAlpha;
            }

            if ((interfaceCtx->cDownAlpha != 0) && (interfaceCtx->cDownAlpha > maxAlpha)) {
                interfaceCtx->cDownAlpha = maxAlpha;
            }

            if ((interfaceCtx->cRightAlpha != 0) && (interfaceCtx->cRightAlpha > maxAlpha)) {
                interfaceCtx->cRightAlpha = maxAlpha;
            }

            if ((interfaceCtx->dpadUpAlpha != 0) && (interfaceCtx->dpadUpAlpha > maxAlpha)) {
                interfaceCtx->dpadUpAlpha = maxAlpha;
            }

            if ((interfaceCtx->dpadDownAlpha != 0) && (interfaceCtx->dpadDownAlpha > maxAlpha)) {
                interfaceCtx->dpadDownAlpha = maxAlpha;
            }

            if ((interfaceCtx->dpadLeftAlpha != 0) && (interfaceCtx->dpadLeftAlpha > maxAlpha)) {
                interfaceCtx->dpadLeftAlpha = maxAlpha;
            }

            if ((interfaceCtx->dpadRightAlpha != 0) && (interfaceCtx->dpadRightAlpha > maxAlpha)) {
                interfaceCtx->dpadRightAlpha = maxAlpha;
            }

            if ((interfaceCtx->healthAlpha != 0) && (interfaceCtx->healthAlpha > maxAlpha)) {
                interfaceCtx->healthAlpha = maxAlpha;
            }

            if ((interfaceCtx->magicAlpha != 0) && (interfaceCtx->magicAlpha > maxAlpha)) {
                interfaceCtx->magicAlpha = maxAlpha;
            }

            if ((interfaceCtx->minimapAlpha != 0) && (interfaceCtx->minimapAlpha > maxAlpha)) {
                interfaceCtx->minimapAlpha = maxAlpha;
            }

            if (interfaceCtx->bAlpha != 255) {
                interfaceCtx->bAlpha = alpha;
            }

            break;
        case 11:
            if ((interfaceCtx->bAlpha != 0) && (interfaceCtx->bAlpha > maxAlpha)) {
                interfaceCtx->bAlpha = maxAlpha;
            }

            if ((interfaceCtx->aAlpha != 0) && (interfaceCtx->aAlpha > maxAlpha)) {
                interfaceCtx->aAlpha = maxAlpha;
            }

            if ((interfaceCtx->cLeftAlpha != 0) && (interfaceCtx->cLeftAlpha > maxAlpha)) {
                interfaceCtx->cLeftAlpha = maxAlpha;
            }

            if ((interfaceCtx->cDownAlpha != 0) && (interfaceCtx->cDownAlpha > maxAlpha)) {
                interfaceCtx->cDownAlpha = maxAlpha;
            }

            if ((interfaceCtx->cRightAlpha != 0) && (interfaceCtx->cRightAlpha > maxAlpha)) {
                interfaceCtx->cRightAlpha = maxAlpha;
            }

            if ((interfaceCtx->dpadUpAlpha != 0) && (interfaceCtx->dpadUpAlpha > maxAlpha)) {
                interfaceCtx->dpadUpAlpha = maxAlpha;
            }

            if ((interfaceCtx->dpadDownAlpha != 0) && (interfaceCtx->dpadDownAlpha > maxAlpha)) {
                interfaceCtx->dpadDownAlpha = maxAlpha;
            }

            if ((interfaceCtx->dpadLeftAlpha != 0) && (interfaceCtx->dpadLeftAlpha > maxAlpha)) {
                interfaceCtx->dpadLeftAlpha = maxAlpha;
            }

            if ((interfaceCtx->dpadRightAlpha != 0) && (interfaceCtx->dpadRightAlpha > maxAlpha)) {
                interfaceCtx->dpadRightAlpha = maxAlpha;
            }

            if ((interfaceCtx->minimapAlpha != 0) && (interfaceCtx->minimapAlpha > maxAlpha)) {
                interfaceCtx->minimapAlpha = maxAlpha;
            }

            if ((interfaceCtx->magicAlpha != 0) && (interfaceCtx->magicAlpha > maxAlpha)) {
                interfaceCtx->magicAlpha = maxAlpha;
            }

            if (interfaceCtx->healthAlpha != 255) {
                interfaceCtx->healthAlpha = alpha;
            }

            break;
        case 12:
            if (interfaceCtx->aAlpha != 255) {
                interfaceCtx->aAlpha = alpha;
            }

            if (interfaceCtx->bAlpha != 255) {
                interfaceCtx->bAlpha = alpha;
            }

            if (interfaceCtx->minimapAlpha != 255) {
                interfaceCtx->minimapAlpha = alpha;
            }

            if ((interfaceCtx->cLeftAlpha != 0) && (interfaceCtx->cLeftAlpha > maxAlpha)) {
                interfaceCtx->cLeftAlpha = maxAlpha;
            }

            if ((interfaceCtx->cDownAlpha != 0) && (interfaceCtx->cDownAlpha > maxAlpha)) {
                interfaceCtx->cDownAlpha = maxAlpha;
            }

            if ((interfaceCtx->cRightAlpha != 0) && (interfaceCtx->cRightAlpha > maxAlpha)) {
                interfaceCtx->cRightAlpha = maxAlpha;
            }

            if ((interfaceCtx->dpadUpAlpha != 0) && (interfaceCtx->dpadUpAlpha > maxAlpha)) {
                interfaceCtx->dpadUpAlpha = maxAlpha;
            }

            if ((interfaceCtx->dpadDownAlpha != 0) && (interfaceCtx->dpadDownAlpha > maxAlpha)) {
                interfaceCtx->dpadDownAlpha = maxAlpha;
            }

            if ((interfaceCtx->dpadLeftAlpha != 0) && (interfaceCtx->dpadLeftAlpha > maxAlpha)) {
                interfaceCtx->dpadLeftAlpha = maxAlpha;
            }

            if ((interfaceCtx->dpadRightAlpha != 0) && (interfaceCtx->dpadRightAlpha > maxAlpha)) {
                interfaceCtx->dpadRightAlpha = maxAlpha;
            }

            if ((interfaceCtx->magicAlpha != 0) && (interfaceCtx->magicAlpha > maxAlpha)) {
                interfaceCtx->magicAlpha = maxAlpha;
            }

            if ((interfaceCtx->healthAlpha != 0) && (interfaceCtx->healthAlpha > maxAlpha)) {
                interfaceCtx->healthAlpha = maxAlpha;
            }

            break;
        case 13:
            func_8008277C(play, maxAlpha, alpha);

            if ((interfaceCtx->minimapAlpha != 0) && (interfaceCtx->minimapAlpha > maxAlpha)) {
                interfaceCtx->minimapAlpha = maxAlpha;
            }

            if ((interfaceCtx->aAlpha != 0) && (interfaceCtx->aAlpha > maxAlpha)) {
                interfaceCtx->aAlpha = maxAlpha;
            }

            if (interfaceCtx->healthAlpha != 255) {
                interfaceCtx->healthAlpha = alpha;
            }

            if (interfaceCtx->magicAlpha != 255) {
                interfaceCtx->magicAlpha = alpha;
            }

            break;
    }

    if ((play->roomCtx.curRoom.behaviorType1 == ROOM_BEHAVIOR_TYPE1_1) && (interfaceCtx->minimapAlpha >= 255)) {
        interfaceCtx->minimapAlpha = 255;
    }
}

void func_80083108(PlayState* play) {
    MessageContext* msgCtx = &play->msgCtx;
    Player* player = GET_PLAYER(play);
    InterfaceContext* interfaceCtx = &play->interfaceCtx;
    s16 i;
    s16 sp28 = 0;

    if ((gSaveContext.cutsceneIndex < 0xFFF0) ||
        ((play->sceneNum == SCENE_SPOT20) && (gSaveContext.cutsceneIndex == 0xFFF0))) {
        gSaveContext.unk_13E7 = 0;

        if ((player->stateFlags1 & 0x00800000) || (play->shootingGalleryStatus > 1) ||
            ((play->sceneNum == SCENE_BOWLING) && Flags_GetSwitch(play, 0x38))) {
            if (gSaveContext.equips.buttonItems[0] != ITEM_NONE) {
                gSaveContext.unk_13E7 = 1;

                if (gSaveContext.buttonStatus[0] == BTN_DISABLED) {
                    gSaveContext.buttonStatus[0] = gSaveContext.buttonStatus[1] = gSaveContext.buttonStatus[2] =
                        gSaveContext.buttonStatus[3] = BTN_ENABLED;
                    gSaveContext.buttonStatus[5] = gSaveContext.buttonStatus[6] = gSaveContext.buttonStatus[7] =
                        gSaveContext.buttonStatus[8] = BTN_ENABLED;
                }

                if ((gSaveContext.equips.buttonItems[0] != ITEM_SLINGSHOT) &&
                    (gSaveContext.equips.buttonItems[0] != ITEM_BOW) &&
                    (gSaveContext.equips.buttonItems[0] != ITEM_BOMBCHU) &&
                    (gSaveContext.equips.buttonItems[0] != ITEM_NONE)) {
                    gSaveContext.buttonStatus[0] = gSaveContext.equips.buttonItems[0];

                    if ((play->sceneNum == SCENE_BOWLING) && Flags_GetSwitch(play, 0x38)) {
                        gSaveContext.equips.buttonItems[0] = ITEM_BOMBCHU;
                        Interface_LoadItemIcon1(play, 0);
                    } else {
                        gSaveContext.equips.buttonItems[0] = ITEM_BOW;
                        if (play->shootingGalleryStatus > 1) {
                            if (LINK_AGE_IN_YEARS == YEARS_CHILD) {
                                gSaveContext.equips.buttonItems[0] = ITEM_SLINGSHOT;
                            }

                            Interface_LoadItemIcon1(play, 0);
                        } else {
                            if (gSaveContext.inventory.items[SLOT_BOW] == ITEM_NONE) {
                                gSaveContext.equips.buttonItems[0] = ITEM_NONE;
                            } else {
                                Interface_LoadItemIcon1(play, 0);
                            }
                        }
                    }

                    gSaveContext.buttonStatus[1] = gSaveContext.buttonStatus[2] = gSaveContext.buttonStatus[3] =
                        BTN_DISABLED;
                    gSaveContext.buttonStatus[5] = gSaveContext.buttonStatus[6] = gSaveContext.buttonStatus[7] =
                        gSaveContext.buttonStatus[8] = BTN_DISABLED;
                    Interface_ChangeAlpha(6);
                }

                if (play->transitionMode != 0) {
                    Interface_ChangeAlpha(1);
                } else if (gSaveContext.minigameState == 1) {
                    Interface_ChangeAlpha(8);
                } else if (play->shootingGalleryStatus > 1) {
                    Interface_ChangeAlpha(8);
                } else if ((play->sceneNum == SCENE_BOWLING) && Flags_GetSwitch(play, 0x38)) {
                    Interface_ChangeAlpha(8);
                } else if (player->stateFlags1 & 0x00800000) {
                    Interface_ChangeAlpha(12);
                }
            } else {
                if (player->stateFlags1 & 0x00800000) {
                    Interface_ChangeAlpha(12);
                }
            }
        } else if (play->sceneNum == SCENE_KENJYANOMA) {
            Interface_ChangeAlpha(1);
        } else if (play->sceneNum == SCENE_TURIBORI) {
            gSaveContext.unk_13E7 = 2;
            if (play->interfaceCtx.unk_260 != 0) {
                if (gSaveContext.equips.buttonItems[0] != ITEM_FISHING_POLE) {
                    gSaveContext.buttonStatus[0] = gSaveContext.equips.buttonItems[0];
                    gSaveContext.equips.buttonItems[0] = ITEM_FISHING_POLE;
                    gSaveContext.unk_13EA = 0;
                    Interface_LoadItemIcon1(play, 0);
                    Interface_ChangeAlpha(12);
                }

                if (gSaveContext.unk_13EA != 12) {
                    Interface_ChangeAlpha(12);
                }
            } else if (gSaveContext.equips.buttonItems[0] == ITEM_FISHING_POLE) {
                gSaveContext.equips.buttonItems[0] = gSaveContext.buttonStatus[0];
                gSaveContext.unk_13EA = 0;

                if (gSaveContext.equips.buttonItems[0] != ITEM_NONE) {
                    Interface_LoadItemIcon1(play, 0);
                }

                gSaveContext.buttonStatus[0] = gSaveContext.buttonStatus[1] = gSaveContext.buttonStatus[2] =
                    gSaveContext.buttonStatus[3] = BTN_DISABLED;
                gSaveContext.buttonStatus[5] = gSaveContext.buttonStatus[6] = gSaveContext.buttonStatus[7] =
                    gSaveContext.buttonStatus[8] = BTN_DISABLED;
                Interface_ChangeAlpha(50);
            } else {
                if (gSaveContext.buttonStatus[0] == BTN_ENABLED) {
                    gSaveContext.unk_13EA = 0;
                }

                gSaveContext.buttonStatus[0] = gSaveContext.buttonStatus[1] = gSaveContext.buttonStatus[2] =
                    gSaveContext.buttonStatus[3] = BTN_DISABLED;
                gSaveContext.buttonStatus[5] = gSaveContext.buttonStatus[6] = gSaveContext.buttonStatus[7] =
                    gSaveContext.buttonStatus[8] = BTN_DISABLED;
                Interface_ChangeAlpha(50);
            }
        } else if (msgCtx->msgMode == MSGMODE_NONE) {
            if (chaosEffectPacifistMode) {
                gSaveContext.buttonStatus[0] = gSaveContext.buttonStatus[1] = gSaveContext.buttonStatus[2] =
                gSaveContext.buttonStatus[3] = gSaveContext.buttonStatus[5] = gSaveContext.buttonStatus[6] =
                gSaveContext.buttonStatus[7] = gSaveContext.buttonStatus[8] = BTN_DISABLED;
            } else if ((func_8008F2F8(play) >= 2) && (func_8008F2F8(play) < 5)) {
                if (gSaveContext.buttonStatus[0] != BTN_DISABLED) {
                    sp28 = 1;
                }

                gSaveContext.buttonStatus[0] = BTN_DISABLED;

                for (i = 1; i < ARRAY_COUNT(gSaveContext.equips.buttonItems); i++) {
                    if ((gSaveContext.equips.buttonItems[i] >= ITEM_SHIELD_DEKU) &&
                        (gSaveContext.equips.buttonItems[i] <= ITEM_BOOTS_HOVER)) {
                        // Equipment on c-buttons is always enabled
                        if (gSaveContext.buttonStatus[BUTTON_STATUS_INDEX(i)] == BTN_DISABLED) {
                            sp28 = 1;
                        }

                        gSaveContext.buttonStatus[BUTTON_STATUS_INDEX(i)] = BTN_ENABLED;
                    } else if (func_8008F2F8(play) == 2) {
                        if ((gSaveContext.equips.buttonItems[i] != ITEM_HOOKSHOT) &&
                            (gSaveContext.equips.buttonItems[i] != ITEM_LONGSHOT)) {
                            if (gSaveContext.buttonStatus[BUTTON_STATUS_INDEX(i)] == BTN_ENABLED) {
                                sp28 = 1;
                            }

                            gSaveContext.buttonStatus[BUTTON_STATUS_INDEX(i)] = BTN_DISABLED;
                        } else {
                            if (gSaveContext.buttonStatus[BUTTON_STATUS_INDEX(i)] == BTN_DISABLED) {
                                sp28 = 1;
                            }

                            gSaveContext.buttonStatus[BUTTON_STATUS_INDEX(i)] = BTN_ENABLED;
                        }
                    } else {
                        if (gSaveContext.buttonStatus[BUTTON_STATUS_INDEX(i)] == BTN_ENABLED) {
                            sp28 = 1;
                        }

                        gSaveContext.buttonStatus[BUTTON_STATUS_INDEX(i)] = BTN_DISABLED;
                    }
                }

                if (sp28) {
                    gSaveContext.unk_13EA = 0;
                }

                Interface_ChangeAlpha(50);
            } else if ((player->stateFlags1 & 0x00200000) || (player->stateFlags2 & 0x00040000)) {
                if (gSaveContext.buttonStatus[0] != BTN_DISABLED) {
                    gSaveContext.buttonStatus[0] = BTN_DISABLED;
                    gSaveContext.buttonStatus[1] = BTN_DISABLED;
                    gSaveContext.buttonStatus[2] = BTN_DISABLED;
                    gSaveContext.buttonStatus[3] = BTN_DISABLED;
                    gSaveContext.buttonStatus[5] = BTN_DISABLED;
                    gSaveContext.buttonStatus[6] = BTN_DISABLED;
                    gSaveContext.buttonStatus[7] = BTN_DISABLED;
                    gSaveContext.buttonStatus[8] = BTN_DISABLED;
                    gSaveContext.unk_13EA = 0;
                    Interface_ChangeAlpha(50);
                }
            } else if ((gSaveContext.eventInf[0] & 0xF) == 1) {
                if (player->stateFlags1 & 0x00800000) {
                    if ((gSaveContext.equips.buttonItems[0] != ITEM_NONE) &&
                        (gSaveContext.equips.buttonItems[0] != ITEM_BOW)) {
                        if (gSaveContext.inventory.items[SLOT_BOW] == ITEM_NONE) {
                            gSaveContext.equips.buttonItems[0] = ITEM_NONE;
                        } else {
                            gSaveContext.equips.buttonItems[0] = ITEM_BOW;
                            sp28 = 1;
                        }
                    }
                } else {
                    sp28 = 1;

                    if ((gSaveContext.equips.buttonItems[0] == ITEM_NONE) ||
                        (gSaveContext.equips.buttonItems[0] == ITEM_BOW)) {

                        if ((gSaveContext.equips.buttonItems[0] != ITEM_SWORD_KOKIRI) &&
                            (gSaveContext.equips.buttonItems[0] != ITEM_SWORD_MASTER) &&
                            (gSaveContext.equips.buttonItems[0] != ITEM_SWORD_BGS) &&
                            (gSaveContext.equips.buttonItems[0] != ITEM_SWORD_KNIFE)) {
                            gSaveContext.equips.buttonItems[0] = gSaveContext.buttonStatus[0];
                        } else {
                            gSaveContext.buttonStatus[0] = gSaveContext.equips.buttonItems[0];
                        }
                    }
                }

                if (sp28) {
                    Interface_LoadItemIcon1(play, 0);
                    sp28 = 0;
                }

                for (i = 1; i < ARRAY_COUNT(gSaveContext.equips.buttonItems); i++) {
                    if ((gSaveContext.equips.buttonItems[i] != ITEM_OCARINA_FAIRY) &&
                        (gSaveContext.equips.buttonItems[i] != ITEM_OCARINA_TIME)) {
                        if (gSaveContext.buttonStatus[BUTTON_STATUS_INDEX(i)] == BTN_ENABLED) {
                            sp28 = 1;
                        }

                        gSaveContext.buttonStatus[BUTTON_STATUS_INDEX(i)] = BTN_DISABLED;
                    } else {
                        if (gSaveContext.buttonStatus[BUTTON_STATUS_INDEX(i)] == BTN_DISABLED) {
                            sp28 = 1;
                        }

                        gSaveContext.buttonStatus[BUTTON_STATUS_INDEX(i)] = BTN_ENABLED;
                    }
                }

                if (sp28) {
                    gSaveContext.unk_13EA = 0;
                }

                Interface_ChangeAlpha(50);
            } else {
                if (interfaceCtx->restrictions.bButton == 0) {
                    if ((gSaveContext.equips.buttonItems[0] == ITEM_SLINGSHOT) ||
                        (gSaveContext.equips.buttonItems[0] == ITEM_BOW) ||
                        (gSaveContext.equips.buttonItems[0] == ITEM_BOMBCHU) ||
                        (gSaveContext.equips.buttonItems[0] == ITEM_NONE)) {
                        if ((gSaveContext.equips.buttonItems[0] != ITEM_NONE) || (gSaveContext.infTable[29] == 0)) {
                            gSaveContext.equips.buttonItems[0] = gSaveContext.buttonStatus[0];
                            sp28 = 1;

                            if (gSaveContext.equips.buttonItems[0] != ITEM_NONE) {
                                Interface_LoadItemIcon1(play, 0);
                            }
                        }
                    } else if ((gSaveContext.buttonStatus[0] & 0xFF) == BTN_DISABLED) {
                        sp28 = 1;

                        if (((gSaveContext.buttonStatus[0] & 0xFF) == BTN_DISABLED) ||
                            ((gSaveContext.buttonStatus[0] & 0xFF) == BTN_ENABLED)) {
                            gSaveContext.buttonStatus[0] = BTN_ENABLED;
                        } else {
                            gSaveContext.equips.buttonItems[0] = gSaveContext.buttonStatus[0] & 0xFF;
                        }
                    }
                } else if (interfaceCtx->restrictions.bButton == 1) {
                    if ((gSaveContext.equips.buttonItems[0] == ITEM_SLINGSHOT) ||
                        (gSaveContext.equips.buttonItems[0] == ITEM_BOW) ||
                        (gSaveContext.equips.buttonItems[0] == ITEM_BOMBCHU) ||
                        (gSaveContext.equips.buttonItems[0] == ITEM_NONE)) {
                        if ((gSaveContext.equips.buttonItems[0] != ITEM_NONE) || (gSaveContext.infTable[29] == 0)) {
                            gSaveContext.equips.buttonItems[0] = gSaveContext.buttonStatus[0];
                            sp28 = 1;

                            if (gSaveContext.equips.buttonItems[0] != ITEM_NONE) {
                                Interface_LoadItemIcon1(play, 0);
                            }
                        }
                    } else {
                        if (gSaveContext.buttonStatus[0] == BTN_ENABLED) {
                            sp28 = 1;
                        }

                        gSaveContext.buttonStatus[0] = BTN_DISABLED;
                    }
                }

                for (i = 1; i < ARRAY_COUNT(gSaveContext.equips.buttonItems); i++) {
                    if ((gSaveContext.equips.buttonItems[i] >= ITEM_SHIELD_DEKU) &&
                        (gSaveContext.equips.buttonItems[i] <= ITEM_BOOTS_HOVER)) {
                        // Equipment on c-buttons is always enabled
                        if (gSaveContext.buttonStatus[BUTTON_STATUS_INDEX(i)] == BTN_DISABLED) {
                            sp28 = 1;
                        }

                        gSaveContext.buttonStatus[BUTTON_STATUS_INDEX(i)] = BTN_ENABLED;
                    }
                }

                if (interfaceCtx->restrictions.bottles != 0) {
                    for (i = 1; i < ARRAY_COUNT(gSaveContext.equips.buttonItems); i++) {
                        if ((gSaveContext.equips.buttonItems[i] >= ITEM_BOTTLE) &&
                            (gSaveContext.equips.buttonItems[i] <= ITEM_POE)) {
                            if (gSaveContext.buttonStatus[BUTTON_STATUS_INDEX(i)] == BTN_ENABLED) {
                                sp28 = 1;
                            }

                            gSaveContext.buttonStatus[BUTTON_STATUS_INDEX(i)] = BTN_DISABLED;
                        }
                    }
                } else if (interfaceCtx->restrictions.bottles == 0) {
                    for (i = 1; i < ARRAY_COUNT(gSaveContext.equips.buttonItems); i++) {
                        if ((gSaveContext.equips.buttonItems[i] >= ITEM_BOTTLE) &&
                            (gSaveContext.equips.buttonItems[i] <= ITEM_POE)) {
                            if (gSaveContext.buttonStatus[BUTTON_STATUS_INDEX(i)] == BTN_DISABLED) {
                                sp28 = 1;
                            }

                            gSaveContext.buttonStatus[BUTTON_STATUS_INDEX(i)] = BTN_ENABLED;
                        }
                    }
                }

                if (interfaceCtx->restrictions.tradeItems != 0) {
                    for (i = 1; i < ARRAY_COUNT(gSaveContext.equips.buttonItems); i++) {
                        if ((CVar_GetS32("gMMBunnyHood", 0) != 0)
                            && (gSaveContext.equips.buttonItems[i] >= ITEM_MASK_KEATON)
                            && (gSaveContext.equips.buttonItems[i] <= ITEM_MASK_TRUTH)) {
                            gSaveContext.buttonStatus[BUTTON_STATUS_INDEX(i)] = BTN_ENABLED;
                        } else if ((gSaveContext.equips.buttonItems[i] >= ITEM_WEIRD_EGG) &&
                            (gSaveContext.equips.buttonItems[i] <= ITEM_CLAIM_CHECK)) {
                            if (gSaveContext.buttonStatus[BUTTON_STATUS_INDEX(i)] == BTN_ENABLED) {
                                sp28 = 1;
                            }

                            gSaveContext.buttonStatus[BUTTON_STATUS_INDEX(i)] = BTN_DISABLED;
                        }
                    }
                } else if (interfaceCtx->restrictions.tradeItems == 0) {
                    for (i = 1; i < ARRAY_COUNT(gSaveContext.equips.buttonItems); i++) {
                        if ((gSaveContext.equips.buttonItems[i] >= ITEM_WEIRD_EGG) &&
                            (gSaveContext.equips.buttonItems[i] <= ITEM_CLAIM_CHECK)) {
                            if (gSaveContext.buttonStatus[BUTTON_STATUS_INDEX(i)] == BTN_DISABLED) {
                                sp28 = 1;
                            }

                            gSaveContext.buttonStatus[BUTTON_STATUS_INDEX(i)] = BTN_ENABLED;
                        }
                    }
                }

                if (interfaceCtx->restrictions.hookshot != 0) {
                    for (i = 1; i < ARRAY_COUNT(gSaveContext.equips.buttonItems); i++) {
                        if ((gSaveContext.equips.buttonItems[i] == ITEM_HOOKSHOT) ||
                            (gSaveContext.equips.buttonItems[i] == ITEM_LONGSHOT)) {
                            if (gSaveContext.buttonStatus[BUTTON_STATUS_INDEX(i)] == BTN_ENABLED) {
                                sp28 = 1;
                            }

                            gSaveContext.buttonStatus[BUTTON_STATUS_INDEX(i)] = BTN_DISABLED;
                        }
                    }
                } else if (interfaceCtx->restrictions.hookshot == 0) {
                    for (i = 1; i < ARRAY_COUNT(gSaveContext.equips.buttonItems); i++) {
                        if ((gSaveContext.equips.buttonItems[i] == ITEM_HOOKSHOT) ||
                            (gSaveContext.equips.buttonItems[i] == ITEM_LONGSHOT)) {
                            if (gSaveContext.buttonStatus[BUTTON_STATUS_INDEX(i)] == BTN_DISABLED) {
                                sp28 = 1;
                            }

                            gSaveContext.buttonStatus[BUTTON_STATUS_INDEX(i)] = BTN_ENABLED;
                        }
                    }
                }

                if (interfaceCtx->restrictions.ocarina != 0) {
                    for (i = 1; i < ARRAY_COUNT(gSaveContext.equips.buttonItems); i++) {
                        if ((gSaveContext.equips.buttonItems[i] == ITEM_OCARINA_FAIRY) ||
                            (gSaveContext.equips.buttonItems[i] == ITEM_OCARINA_TIME)) {
                            if (gSaveContext.buttonStatus[BUTTON_STATUS_INDEX(i)] == BTN_ENABLED) {
                                sp28 = 1;
                            }

                            gSaveContext.buttonStatus[BUTTON_STATUS_INDEX(i)] = BTN_DISABLED;
                        }
                    }
                } else if (interfaceCtx->restrictions.ocarina == 0) {
                    for (i = 1; i < ARRAY_COUNT(gSaveContext.equips.buttonItems); i++) {
                        if ((gSaveContext.equips.buttonItems[i] == ITEM_OCARINA_FAIRY) ||
                            (gSaveContext.equips.buttonItems[i] == ITEM_OCARINA_TIME)) {
                            if (gSaveContext.buttonStatus[BUTTON_STATUS_INDEX(i)] == BTN_DISABLED) {
                                sp28 = 1;
                            }

                            gSaveContext.buttonStatus[BUTTON_STATUS_INDEX(i)] = BTN_ENABLED;
                        }
                    }
                }

                if (interfaceCtx->restrictions.farores != 0) {
                    for (i = 1; i < ARRAY_COUNT(gSaveContext.equips.buttonItems); i++) {
                        if (gSaveContext.equips.buttonItems[i] == ITEM_FARORES_WIND) {
                            if (gSaveContext.buttonStatus[BUTTON_STATUS_INDEX(i)] == BTN_ENABLED) {
                                sp28 = 1;
                            }

                            gSaveContext.buttonStatus[BUTTON_STATUS_INDEX(i)] = BTN_DISABLED;
                            osSyncPrintf("***(i=%d)***  ", i);
                        }
                    }
                } else if (interfaceCtx->restrictions.farores == 0) {
                    for (i = 1; i < ARRAY_COUNT(gSaveContext.equips.buttonItems); i++) {
                        if (gSaveContext.equips.buttonItems[i] == ITEM_FARORES_WIND) {
                            if (gSaveContext.buttonStatus[BUTTON_STATUS_INDEX(i)] == BTN_DISABLED) {
                                sp28 = 1;
                            }

                            gSaveContext.buttonStatus[BUTTON_STATUS_INDEX(i)] = BTN_ENABLED;
                        }
                    }
                }

                if (interfaceCtx->restrictions.dinsNayrus != 0) {
                    for (i = 1; i < ARRAY_COUNT(gSaveContext.equips.buttonItems); i++) {
                        if ((gSaveContext.equips.buttonItems[i] == ITEM_DINS_FIRE) ||
                            (gSaveContext.equips.buttonItems[i] == ITEM_NAYRUS_LOVE)) {
                            if (gSaveContext.buttonStatus[BUTTON_STATUS_INDEX(i)] == BTN_ENABLED) {
                                sp28 = 1;
                            }

                            gSaveContext.buttonStatus[BUTTON_STATUS_INDEX(i)] = BTN_DISABLED;
                        }
                    }
                } else if (interfaceCtx->restrictions.dinsNayrus == 0) {
                    for (i = 1; i < ARRAY_COUNT(gSaveContext.equips.buttonItems); i++) {
                        if ((gSaveContext.equips.buttonItems[i] == ITEM_DINS_FIRE) ||
                            (gSaveContext.equips.buttonItems[i] == ITEM_NAYRUS_LOVE)) {
                            if (gSaveContext.buttonStatus[BUTTON_STATUS_INDEX(i)] == BTN_DISABLED) {
                                sp28 = 1;
                            }

                            gSaveContext.buttonStatus[BUTTON_STATUS_INDEX(i)] = BTN_ENABLED;
                        }
                    }
                }

                if (interfaceCtx->restrictions.all != 0) {
                    for (i = 1; i < ARRAY_COUNT(gSaveContext.equips.buttonItems); i++) {
                        if ((gSaveContext.equips.buttonItems[i] != ITEM_OCARINA_FAIRY) &&
                            (gSaveContext.equips.buttonItems[i] != ITEM_OCARINA_TIME) &&
                            !((gSaveContext.equips.buttonItems[i] >= ITEM_BOTTLE) &&
                              (gSaveContext.equips.buttonItems[i] <= ITEM_POE)) &&
                            !((gSaveContext.equips.buttonItems[i] >= ITEM_SHIELD_DEKU) &&  // Never disable equipment
                              (gSaveContext.equips.buttonItems[i] <= ITEM_BOOTS_HOVER)) && // (tunics/boots) on C-buttons
                            !((gSaveContext.equips.buttonItems[i] >= ITEM_WEIRD_EGG) &&
                              (gSaveContext.equips.buttonItems[i] <= ITEM_CLAIM_CHECK))) {
                            if ((play->sceneNum != SCENE_TAKARAYA) ||
                                (gSaveContext.equips.buttonItems[i] != ITEM_LENS)) {
                                if (gSaveContext.buttonStatus[BUTTON_STATUS_INDEX(i)] == BTN_ENABLED) {
                                    sp28 = 1;
                                }

                                gSaveContext.buttonStatus[BUTTON_STATUS_INDEX(i)] = BTN_DISABLED;
                            } else {
                                if (gSaveContext.buttonStatus[BUTTON_STATUS_INDEX(i)] == BTN_DISABLED) {
                                    sp28 = 1;
                                }

                                gSaveContext.buttonStatus[BUTTON_STATUS_INDEX(i)] = BTN_ENABLED;
                            }
                        }
                    }
                } else if (interfaceCtx->restrictions.all == 0) {
                    for (i = 1; i < ARRAY_COUNT(gSaveContext.equips.buttonItems); i++) {
                        if ((gSaveContext.equips.buttonItems[i] != ITEM_DINS_FIRE) &&
                            (gSaveContext.equips.buttonItems[i] != ITEM_HOOKSHOT) &&
                            (gSaveContext.equips.buttonItems[i] != ITEM_LONGSHOT) &&
                            (gSaveContext.equips.buttonItems[i] != ITEM_FARORES_WIND) &&
                            (gSaveContext.equips.buttonItems[i] != ITEM_NAYRUS_LOVE) &&
                            (gSaveContext.equips.buttonItems[i] != ITEM_OCARINA_FAIRY) &&
                            (gSaveContext.equips.buttonItems[i] != ITEM_OCARINA_TIME) &&
                            !((gSaveContext.equips.buttonItems[i] >= ITEM_BOTTLE) &&
                              (gSaveContext.equips.buttonItems[i] <= ITEM_POE)) &&
                            !((gSaveContext.equips.buttonItems[i] >= ITEM_WEIRD_EGG) &&
                              (gSaveContext.equips.buttonItems[i] <= ITEM_CLAIM_CHECK))) {
                            if (gSaveContext.buttonStatus[BUTTON_STATUS_INDEX(i)] == BTN_DISABLED) {
                                sp28 = 1;
                            }

                            gSaveContext.buttonStatus[BUTTON_STATUS_INDEX(i)] = BTN_ENABLED;
                        }
                    }
                }
            }
        }
    }

    if (sp28) {
        gSaveContext.unk_13EA = 0;
        if ((play->sceneLoadFlag == 0) && (play->transitionMode == 0)) {
            Interface_ChangeAlpha(50);
            osSyncPrintf("????????  alpha_change( 50 );  ?????\n");
        } else {
            osSyncPrintf("game_play->fade_direction || game_play->fbdemo_wipe_modem");
        }
    }
}

void Interface_SetSceneRestrictions(PlayState* play) {
    InterfaceContext* interfaceCtx = &play->interfaceCtx;
    s16 i;
    u8 currentScene;

    // clang-format off
    interfaceCtx->restrictions.hGauge = interfaceCtx->restrictions.bButton =
    interfaceCtx->restrictions.aButton = interfaceCtx->restrictions.bottles =
    interfaceCtx->restrictions.tradeItems = interfaceCtx->restrictions.hookshot =
    interfaceCtx->restrictions.ocarina = interfaceCtx->restrictions.warpSongs =
    interfaceCtx->restrictions.sunsSong = interfaceCtx->restrictions.farores =
    interfaceCtx->restrictions.dinsNayrus = interfaceCtx->restrictions.all = 0;
    // clang-format on

    i = 0;

    // "Data settings related to button display scene_data_ID=%d\n"
    osSyncPrintf("ボタン表示関係データ設定 scene_data_ID=%d\n", play->sceneNum);

    do {
        currentScene = (u8)play->sceneNum;
        if (sRestrictionFlags[i].scene == currentScene) {
            interfaceCtx->restrictions.hGauge = (sRestrictionFlags[i].flags1 & 0xC0) >> 6;
            interfaceCtx->restrictions.bButton = (sRestrictionFlags[i].flags1 & 0x30) >> 4;
            interfaceCtx->restrictions.aButton = (sRestrictionFlags[i].flags1 & 0x0C) >> 2;
            interfaceCtx->restrictions.bottles = (sRestrictionFlags[i].flags1 & 0x03) >> 0;
            interfaceCtx->restrictions.tradeItems = (sRestrictionFlags[i].flags2 & 0xC0) >> 6;
            interfaceCtx->restrictions.hookshot = (sRestrictionFlags[i].flags2 & 0x30) >> 4;
            interfaceCtx->restrictions.ocarina = (sRestrictionFlags[i].flags2 & 0x0C) >> 2;
            interfaceCtx->restrictions.warpSongs = (sRestrictionFlags[i].flags2 & 0x03) >> 0;
            interfaceCtx->restrictions.sunsSong = (sRestrictionFlags[i].flags3 & 0xC0) >> 6;
            interfaceCtx->restrictions.farores = (sRestrictionFlags[i].flags3 & 0x30) >> 4;
            interfaceCtx->restrictions.dinsNayrus = (sRestrictionFlags[i].flags3 & 0x0C) >> 2;
            interfaceCtx->restrictions.all = (sRestrictionFlags[i].flags3 & 0x03) >> 0;

            osSyncPrintf(VT_FGCOL(YELLOW));
            osSyncPrintf("parameter->button_status = %x,%x,%x\n", sRestrictionFlags[i].flags1,
                         sRestrictionFlags[i].flags2, sRestrictionFlags[i].flags3);
            osSyncPrintf("h_gage=%d, b_button=%d, a_button=%d, c_bottle=%d\n", interfaceCtx->restrictions.hGauge,
                         interfaceCtx->restrictions.bButton, interfaceCtx->restrictions.aButton,
                         interfaceCtx->restrictions.bottles);
            osSyncPrintf("c_warasibe=%d, c_hook=%d, c_ocarina=%d, c_warp=%d\n", interfaceCtx->restrictions.tradeItems,
                         interfaceCtx->restrictions.hookshot, interfaceCtx->restrictions.ocarina,
                         interfaceCtx->restrictions.warpSongs);
            osSyncPrintf("c_sunmoon=%d, m_wind=%d, m_magic=%d, another=%d\n", interfaceCtx->restrictions.sunsSong,
                         interfaceCtx->restrictions.farores, interfaceCtx->restrictions.dinsNayrus,
                         interfaceCtx->restrictions.all);
            osSyncPrintf(VT_RST);
            return;
        }
        i++;
    } while (sRestrictionFlags[i].scene != 0xFF);
}

Gfx* Gfx_TextureIA8(Gfx* displayListHead, void* texture, s16 textureWidth, s16 textureHeight, s16 rectLeft, s16 rectTop,
                    s16 rectWidth, s16 rectHeight, u16 dsdx, u16 dtdy) {
    gDPLoadTextureBlock(displayListHead++, texture, G_IM_FMT_IA, G_IM_SIZ_8b, textureWidth, textureHeight, 0,
                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                        G_TX_NOLOD);

    gSPWideTextureRectangle(displayListHead++, rectLeft << 2, rectTop << 2, (rectLeft + rectWidth) << 2,
                        (rectTop + rectHeight) << 2, G_TX_RENDERTILE, 0, 0, dsdx, dtdy);

    return displayListHead;
}

Gfx* Gfx_TextureI8(Gfx* displayListHead, void* texture, s16 textureWidth, s16 textureHeight, s16 rectLeft, s16 rectTop,
                   s16 rectWidth, s16 rectHeight, u16 dsdx, u16 dtdy) {
    gDPLoadTextureBlock(displayListHead++, texture, G_IM_FMT_I, G_IM_SIZ_8b, textureWidth, textureHeight, 0,
                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                        G_TX_NOLOD);

    gSPWideTextureRectangle(displayListHead++, rectLeft << 2, rectTop << 2, (rectLeft + rectWidth) << 2,
                        (rectTop + rectHeight) << 2, G_TX_RENDERTILE, 0, 0, dsdx, dtdy);

    return displayListHead;
}

void Inventory_SwapAgeEquipment(void) {
    s16 i;
    u16 temp;

    if (LINK_AGE_IN_YEARS == YEARS_CHILD) {
        // When becoming adult, remove swordless flag since we'll get master sword
        // Only in rando to keep swordless link bugs in vanilla
        if (gSaveContext.n64ddFlag) {
            gSaveContext.infTable[29] &= ~1;
        }

        for (i = 0; i < ARRAY_COUNT(gSaveContext.equips.buttonItems); i++) {
            if (i != 0) {
                gSaveContext.childEquips.buttonItems[i] = gSaveContext.equips.buttonItems[i];
            } else {
                gSaveContext.childEquips.buttonItems[i] = ITEM_SWORD_KOKIRI;
            }

            if (i != 0) {
                gSaveContext.childEquips.cButtonSlots[i - 1] = gSaveContext.equips.cButtonSlots[i - 1];
            }
        }

        gSaveContext.childEquips.equipment = gSaveContext.equips.equipment;

        if (gSaveContext.adultEquips.buttonItems[0] == ITEM_NONE) {
            gSaveContext.equips.buttonItems[0] = ITEM_SWORD_MASTER;

            if (gSaveContext.inventory.items[SLOT_NUT] != ITEM_NONE) {
                gSaveContext.equips.buttonItems[1] = ITEM_NUT;
                gSaveContext.equips.cButtonSlots[0] = SLOT_NUT;
            } else {
                gSaveContext.equips.buttonItems[1] = gSaveContext.equips.cButtonSlots[0] = ITEM_NONE;
            }

            gSaveContext.equips.buttonItems[2] = ITEM_BOMB;
            gSaveContext.equips.buttonItems[3] = gSaveContext.inventory.items[SLOT_OCARINA];
            gSaveContext.equips.cButtonSlots[1] = SLOT_BOMB;
            gSaveContext.equips.cButtonSlots[2] = SLOT_OCARINA;
            gSaveContext.equips.equipment = 0x1122;

            // Set the dpad to nothing
            gSaveContext.equips.buttonItems[4] = ITEM_NONE;
            gSaveContext.equips.buttonItems[5] = ITEM_NONE;
            gSaveContext.equips.buttonItems[6] = ITEM_NONE;
            gSaveContext.equips.buttonItems[7] = ITEM_NONE;
            gSaveContext.equips.cButtonSlots[3] = SLOT_NONE;
            gSaveContext.equips.cButtonSlots[4] = SLOT_NONE;
            gSaveContext.equips.cButtonSlots[5] = SLOT_NONE;
            gSaveContext.equips.cButtonSlots[6] = SLOT_NONE;
        } else {
            for (i = 0; i < ARRAY_COUNT(gSaveContext.equips.buttonItems); i++) {
                gSaveContext.equips.buttonItems[i] = gSaveContext.adultEquips.buttonItems[i];

                if (i != 0) {
                    gSaveContext.equips.cButtonSlots[i - 1] = gSaveContext.adultEquips.cButtonSlots[i - 1];
                }

                if (((gSaveContext.equips.buttonItems[i] >= ITEM_BOTTLE) &&
                     (gSaveContext.equips.buttonItems[i] <= ITEM_POE)) ||
                    ((gSaveContext.equips.buttonItems[i] >= ITEM_WEIRD_EGG) &&
                     (gSaveContext.equips.buttonItems[i] <= ITEM_CLAIM_CHECK))) {
                    osSyncPrintf("Register_Item_Pt(%d)=%d\n", i, gSaveContext.equips.cButtonSlots[i - 1]);
                    gSaveContext.equips.buttonItems[i] =
                        gSaveContext.inventory.items[gSaveContext.equips.cButtonSlots[i - 1]];
                }
            }

            gSaveContext.equips.equipment = gSaveContext.adultEquips.equipment;
        }
    } else {
        // When becoming child, set swordless flag if player doesn't have kokiri sword
        // Only in rando to keep swordless link bugs in vanilla
        if (gSaveContext.n64ddFlag && (1 << 0 & gSaveContext.inventory.equipment) == 0) {
            gSaveContext.infTable[29] |= 1;
        }

        for (i = 0; i < ARRAY_COUNT(gSaveContext.equips.buttonItems); i++) {
            gSaveContext.adultEquips.buttonItems[i] = gSaveContext.equips.buttonItems[i];

            if (i != 0) {
                gSaveContext.adultEquips.cButtonSlots[i - 1] = gSaveContext.equips.cButtonSlots[i - 1];
            }
        }

        gSaveContext.adultEquips.equipment = gSaveContext.equips.equipment;

        if (gSaveContext.childEquips.buttonItems[0] != ITEM_NONE) {
            for (i = 0; i < ARRAY_COUNT(gSaveContext.equips.buttonItems); i++) {
                gSaveContext.equips.buttonItems[i] = gSaveContext.childEquips.buttonItems[i];

                if (i != 0) {
                    gSaveContext.equips.cButtonSlots[i - 1] = gSaveContext.childEquips.cButtonSlots[i - 1];
                }

                if (((gSaveContext.equips.buttonItems[i] >= ITEM_BOTTLE) &&
                     (gSaveContext.equips.buttonItems[i] <= ITEM_POE)) ||
                    ((gSaveContext.equips.buttonItems[i] >= ITEM_WEIRD_EGG) &&
                     (gSaveContext.equips.buttonItems[i] <= ITEM_CLAIM_CHECK))) {
                    osSyncPrintf("Register_Item_Pt(%d)=%d\n", i, gSaveContext.equips.cButtonSlots[i - 1]);
                    gSaveContext.equips.buttonItems[i] =
                        gSaveContext.inventory.items[gSaveContext.equips.cButtonSlots[i - 1]];
                }
            }

            gSaveContext.equips.equipment = gSaveContext.childEquips.equipment;
            gSaveContext.equips.equipment &= 0xFFF0;
            gSaveContext.equips.equipment |= 0x0001;
        } else if (gSaveContext.n64ddFlag && Randomizer_GetSettingValue(RSK_STARTING_AGE)) {
            /*If in rando and starting age is adult, childEquips is not initialized and buttonItems[0]
            will be ITEM_NONE. When changing age from adult -> child, reset equips to "default"
            (only kokiri tunic/boots equipped, no sword, no C-button items, no D-Pad items).
            When becoming child, set swordless flag if player doesn't have kokiri sword
            Only in rando to keep swordless link bugs in vanilla*/
            if (1 << 0 & gSaveContext.inventory.equipment == 0) {
                gSaveContext.infTable[29] |= 1;
            }

            //zero out items
            for (i = 0; i < ARRAY_COUNT(gSaveContext.equips.buttonItems); i++) {
                gSaveContext.equips.buttonItems[i] = ITEM_NONE;
                if (i != 0) {
                    gSaveContext.equips.cButtonSlots[i-1] = ITEM_NONE;
                }
            }
            gSaveContext.equips.equipment = 0x1111;
        }
    }

    temp = gEquipMasks[EQUIP_SHIELD] & gSaveContext.equips.equipment;
    if (temp != 0) {
        temp >>= gEquipShifts[EQUIP_SHIELD];
        if (!(gBitFlags[temp + 3] & gSaveContext.inventory.equipment)) {
            gSaveContext.equips.equipment &= gEquipNegMasks[EQUIP_SHIELD];
        }
    }
}

void Interface_InitHorsebackArchery(PlayState* play) {
    InterfaceContext* interfaceCtx = &play->interfaceCtx;

    gSaveContext.minigameState = 1;
    interfaceCtx->unk_23C = interfaceCtx->unk_240 = interfaceCtx->unk_242 = 0;
    gSaveContext.minigameScore = sHBAScoreTier = 0;
    interfaceCtx->hbaAmmo = 20;
}

void func_800849EC(PlayState* play) {
    gSaveContext.inventory.equipment |= gBitFlags[2] << gEquipShifts[EQUIP_SWORD];
    gSaveContext.inventory.equipment ^= 8 << gEquipShifts[EQUIP_SWORD];

    if (gBitFlags[3] & gSaveContext.inventory.equipment) {
        gSaveContext.equips.buttonItems[0] = ITEM_SWORD_KNIFE;
    } else {
        gSaveContext.equips.buttonItems[0] = ITEM_SWORD_BGS;
    }

    Interface_LoadItemIcon1(play, 0);
}

void Interface_LoadItemIcon1(PlayState* play, u16 button) {
    InterfaceContext* interfaceCtx = &play->interfaceCtx;

    osCreateMesgQueue(&interfaceCtx->loadQueue, &interfaceCtx->loadMsg, OS_MESG_BLOCK);
    DmaMgr_SendRequest2(&interfaceCtx->dmaRequest_160, interfaceCtx->iconItemSegment + button * 0x1000,
                        (uintptr_t)_icon_item_staticSegmentRomStart + (gSaveContext.equips.buttonItems[button] * 0x1000),
                        0x1000, 0, &interfaceCtx->loadQueue, OS_MESG_PTR(NULL), __FILE__, __LINE__);
    osRecvMesg(&interfaceCtx->loadQueue, NULL, OS_MESG_BLOCK);
}

void Interface_LoadItemIcon2(PlayState* play, u16 button) {
    InterfaceContext* interfaceCtx = &play->interfaceCtx;

    osCreateMesgQueue(&interfaceCtx->loadQueue, &interfaceCtx->loadMsg, OS_MESG_BLOCK);
    DmaMgr_SendRequest2(&interfaceCtx->dmaRequest_180, interfaceCtx->iconItemSegment + button * 0x1000,
                        (uintptr_t)_icon_item_staticSegmentRomStart + (gSaveContext.equips.buttonItems[button] * 0x1000),
                        0x1000, 0, &interfaceCtx->loadQueue, OS_MESG_PTR(NULL), __FILE__, __LINE__);
    osRecvMesg(&interfaceCtx->loadQueue, NULL, OS_MESG_BLOCK);
}

void func_80084BF4(PlayState* play, u16 flag) {
    if (flag) {
        if ((gSaveContext.equips.buttonItems[0] == ITEM_SLINGSHOT) ||
            (gSaveContext.equips.buttonItems[0] == ITEM_BOW) || (gSaveContext.equips.buttonItems[0] == ITEM_BOMBCHU) ||
            (gSaveContext.equips.buttonItems[0] == ITEM_FISHING_POLE) ||
            (gSaveContext.buttonStatus[0] == BTN_DISABLED)) {
            if ((gSaveContext.equips.buttonItems[0] == ITEM_SLINGSHOT) ||
                (gSaveContext.equips.buttonItems[0] == ITEM_BOW) ||
                (gSaveContext.equips.buttonItems[0] == ITEM_BOMBCHU) ||
                (gSaveContext.equips.buttonItems[0] == ITEM_FISHING_POLE)) {
                gSaveContext.equips.buttonItems[0] = gSaveContext.buttonStatus[0];
                Interface_LoadItemIcon1(play, 0);
            }
        } else if (gSaveContext.equips.buttonItems[0] == ITEM_NONE) {
            if ((gSaveContext.equips.buttonItems[0] != ITEM_NONE) || (gSaveContext.infTable[29] == 0)) {
                gSaveContext.equips.buttonItems[0] = gSaveContext.buttonStatus[0];
                Interface_LoadItemIcon1(play, 0);
            }
        }

        gSaveContext.buttonStatus[0] = gSaveContext.buttonStatus[1] = gSaveContext.buttonStatus[2] =
            gSaveContext.buttonStatus[3] = BTN_ENABLED;
        gSaveContext.buttonStatus[5] = gSaveContext.buttonStatus[6] = gSaveContext.buttonStatus[7] =
            gSaveContext.buttonStatus[8] = BTN_ENABLED;
        Interface_ChangeAlpha(7);
    } else {
        gSaveContext.buttonStatus[0] = gSaveContext.buttonStatus[1] = gSaveContext.buttonStatus[2] =
            gSaveContext.buttonStatus[3] = BTN_ENABLED;
        gSaveContext.buttonStatus[5] = gSaveContext.buttonStatus[6] = gSaveContext.buttonStatus[7] =
            gSaveContext.buttonStatus[8] = BTN_ENABLED;
        func_80083108(play);
    }
}

// Gameplay stat tracking: Update time the item was acquired
// (special cases for some duplicate items)
void GameplayStats_SetTimestamp(u8 item) {

    if (gSaveContext.sohStats.timestamp[item] != 0) {
        return;
    }

    u32 time = GAMEPLAYSTAT_TOTAL_TIME;

    // Have items in Link's pocket shown as being obtained at 0.1 seconds
    if (time == 0) {
        time = 1;
    }

    // Count any bottled item as a bottle
    if (item >= ITEM_BOTTLE && item <= ITEM_POE) {
        if (gSaveContext.sohStats.timestamp[ITEM_BOTTLE] == 0) {
            gSaveContext.sohStats.timestamp[ITEM_BOTTLE] = time;
        }
        return;
    }
    // Count any bombchu pack as bombchus
    if (item == ITEM_BOMBCHU || (item >= ITEM_BOMBCHUS_5 && item <= ITEM_BOMBCHUS_20)) {
        if (gSaveContext.sohStats.timestamp[ITEM_BOMBCHU] == 0) {
            gSaveContext.sohStats.timestamp[ITEM_BOMBCHU] = time;
        }
        return;
    }

    gSaveContext.sohStats.timestamp[item] = time;
}

// Gameplay stat tracking: Update time the item was acquired
// (special cases for rando items)
void Randomizer_GameplayStats_SetTimestamp(uint16_t item) {

    u32 time = GAMEPLAYSTAT_TOTAL_TIME;

    // Have items in Link's pocket shown as being obtained at 0.1 seconds
    if (time == 0) {
        time = 1;
    }

    // Count any bottled item as a bottle
    if (item >= RG_EMPTY_BOTTLE && item <= RG_BOTTLE_WITH_BIG_POE) {
        if (gSaveContext.sohStats.timestamp[ITEM_BOTTLE] == 0) {
            gSaveContext.sohStats.timestamp[ITEM_BOTTLE] = time;
        }
        return;
    }
    // Count any bombchu pack as bombchus
    if (item >= RG_BOMBCHU_5 && item <= RG_BOMBCHU_DROP) {
        if (gSaveContext.sohStats.timestamp[ITEM_BOMBCHU] = 0) {
            gSaveContext.sohStats.timestamp[ITEM_BOMBCHU] = time;
        }
        return;
    }
    if (item == RG_MAGIC_SINGLE) {
        gSaveContext.sohStats.timestamp[ITEM_SINGLE_MAGIC] = time;
    }
    if (item == RG_DOUBLE_DEFENSE) {
        gSaveContext.sohStats.timestamp[ITEM_DOUBLE_DEFENSE] = time;
    }
}

/**
 * @brief Adds the given item to Link's inventory.
 * 
 * NOTE: This function has been edited to be safe to use with a NULL play.
 * If you need to add to this function, be sure you check if the play is not
 * NULL before doing any operations requiring it.
 * 
 * @param play 
 * @param item 
 * @return u8 
 */
u8 Item_Give(PlayState* play, u8 item) {
    static s16 sAmmoRefillCounts[] = { 5, 10, 20, 30, 5, 10, 30, 0, 5, 20, 1, 5, 20, 50, 200, 10 };
    s16 i;
    s16 slot;
    s16 temp;

    // Gameplay stats: Update the time the item was obtained
    GameplayStats_SetTimestamp(item);

    slot = SLOT(item);
    if (item >= ITEM_STICKS_5) {
        slot = SLOT(sExtraItemBases[item - ITEM_STICKS_5]);
    }

    osSyncPrintf(VT_FGCOL(YELLOW));
    osSyncPrintf("item_get_setting=%d  pt=%d  z=%x\n", item, slot, gSaveContext.inventory.items[slot]);
    osSyncPrintf(VT_RST);

    if ((item >= ITEM_MEDALLION_FOREST) && (item <= ITEM_MEDALLION_LIGHT)) {
        gSaveContext.inventory.questItems |= gBitFlags[item - ITEM_MEDALLION_FOREST + QUEST_MEDALLION_FOREST];

        osSyncPrintf(VT_FGCOL(YELLOW));
        osSyncPrintf("封印 = %x\n", gSaveContext.inventory.questItems); // "Seals = %x"
        osSyncPrintf(VT_RST);

        if (item == ITEM_MEDALLION_WATER) {
            func_8006D0AC(play);
        }

        PerformAutosave(play, item);
        return ITEM_NONE;
    } else if ((item >= ITEM_SONG_MINUET) && (item <= ITEM_SONG_STORMS)) {
        gSaveContext.inventory.questItems |= gBitFlags[item - ITEM_SONG_MINUET + QUEST_SONG_MINUET];

        osSyncPrintf(VT_FGCOL(YELLOW));
        osSyncPrintf("楽譜 = %x\n", gSaveContext.inventory.questItems); // "Musical scores = %x"
        // "Musical scores = %x (%x) (%x)"
        osSyncPrintf("楽譜 = %x (%x) (%x)\n", gSaveContext.inventory.questItems,
                     gBitFlags[item - ITEM_SONG_MINUET + QUEST_SONG_MINUET], gBitFlags[item - ITEM_SONG_MINUET]);
        osSyncPrintf(VT_RST);

        PerformAutosave(play, item);
        return ITEM_NONE;
    } else if ((item >= ITEM_KOKIRI_EMERALD) && (item <= ITEM_ZORA_SAPPHIRE)) {
        gSaveContext.inventory.questItems |= gBitFlags[item - ITEM_KOKIRI_EMERALD + QUEST_KOKIRI_EMERALD];

        osSyncPrintf(VT_FGCOL(YELLOW));
        osSyncPrintf("精霊石 = %x\n", gSaveContext.inventory.questItems); // "Spiritual Stones = %x"
        osSyncPrintf(VT_RST);

        PerformAutosave(play, item);
        return ITEM_NONE;
    } else if ((item == ITEM_STONE_OF_AGONY) || (item == ITEM_GERUDO_CARD)) {
        gSaveContext.inventory.questItems |= gBitFlags[item - ITEM_STONE_OF_AGONY + QUEST_STONE_OF_AGONY];

        osSyncPrintf(VT_FGCOL(YELLOW));
        osSyncPrintf("アイテム = %x\n", gSaveContext.inventory.questItems); // "Items = %x"
        osSyncPrintf(VT_RST);

        PerformAutosave(play, item);
        return ITEM_NONE;
    } else if (item == ITEM_SKULL_TOKEN) {
        gSaveContext.inventory.questItems |= gBitFlags[item - ITEM_SKULL_TOKEN + QUEST_SKULL_TOKEN];
        gSaveContext.inventory.gsTokens++;

        osSyncPrintf(VT_FGCOL(YELLOW));
        // "N Coins = %x(%d)"
        osSyncPrintf("Ｎコイン = %x(%d)\n", gSaveContext.inventory.questItems, gSaveContext.inventory.gsTokens);
        osSyncPrintf(VT_RST);

        PerformAutosave(play, item);
        return ITEM_NONE;
    } else if ((item >= ITEM_SWORD_KOKIRI) && (item <= ITEM_SWORD_BGS)) {
        gSaveContext.inventory.equipment |= gBitFlags[item - ITEM_SWORD_KOKIRI] << gEquipShifts[EQUIP_SWORD];

        // Both Giant's Knife and Biggoron Sword have the same Item ID, so this part handles both of them
        if (item == ITEM_SWORD_BGS) {
            gSaveContext.swordHealth = 8;

            // In rando, when buying Giant's Knife, also check
            // for 0xE in case we don't have Kokiri Sword
            if (ALL_EQUIP_VALUE(EQUIP_SWORD) == 0xF || (gSaveContext.n64ddFlag && ALL_EQUIP_VALUE(EQUIP_SWORD) == 0xE)) {

                gSaveContext.inventory.equipment ^= 8 << gEquipShifts[EQUIP_SWORD]; 

                if (gSaveContext.equips.buttonItems[0] == ITEM_SWORD_KNIFE) {
                    gSaveContext.equips.buttonItems[0] = ITEM_SWORD_BGS;
                    if (play != NULL) {
                        Interface_LoadItemIcon1(play, 0);
                    }
                }
            }
            
        } else if (item == ITEM_SWORD_MASTER) {
            gSaveContext.equips.buttonItems[0] = ITEM_SWORD_MASTER;
            gSaveContext.equips.equipment &= 0xFFF0;
            gSaveContext.equips.equipment |= 0x0002;
            if (play != NULL) {
                Interface_LoadItemIcon1(play, 0);
            }
        }

        PerformAutosave(play, item);
        return ITEM_NONE;
    } else if ((item >= ITEM_SHIELD_DEKU) && (item <= ITEM_SHIELD_MIRROR)) {
        gSaveContext.inventory.equipment |= (gBitFlags[item - ITEM_SHIELD_DEKU] << gEquipShifts[EQUIP_SHIELD]);
        PerformAutosave(play, item);
        return ITEM_NONE;
    } else if ((item >= ITEM_TUNIC_KOKIRI) && (item <= ITEM_TUNIC_ZORA)) {
        gSaveContext.inventory.equipment |= (gBitFlags[item - ITEM_TUNIC_KOKIRI] << gEquipShifts[EQUIP_TUNIC]);
        PerformAutosave(play, item);
        return ITEM_NONE;
    } else if ((item >= ITEM_BOOTS_KOKIRI) && (item <= ITEM_BOOTS_HOVER)) {
        gSaveContext.inventory.equipment |= (gBitFlags[item - ITEM_BOOTS_KOKIRI] << gEquipShifts[EQUIP_BOOTS]);
        PerformAutosave(play, item);
        return ITEM_NONE;
    } else if ((item == ITEM_KEY_BOSS) || (item == ITEM_COMPASS) || (item == ITEM_DUNGEON_MAP)) {
        // Boss Key, Compass, and Dungeon Map exceptions for rando.
        // Rando should never be able to get here for Link's Pocket unless something goes wrong,
        // but we check for a play here so the game won't crash if we do somehow get here.
        if (gSaveContext.n64ddFlag && play != NULL) {
            if (play->sceneNum == 13) { // ganon's castle -> ganon's tower
                gSaveContext.inventory.dungeonItems[10] |= 1;
            } else if (play->sceneNum == 92) { // Desert Colossus -> Spirit Temple.
                gSaveContext.inventory.dungeonItems[6] |= gBitFlags[item - ITEM_KEY_BOSS];
            } else {
                gSaveContext.inventory.dungeonItems[gSaveContext.mapIndex] |= gBitFlags[item - ITEM_KEY_BOSS];
            }
        } else {
            gSaveContext.inventory.dungeonItems[gSaveContext.mapIndex] |= gBitFlags[item - ITEM_KEY_BOSS];
        }
        PerformAutosave(play, item);
        return ITEM_NONE;
    } else if (item == ITEM_KEY_SMALL) {
        // Small key exceptions for rando with keysanity off.
        // Rando should never be able to get here for Link's Pocket unless something goes wrong,
        // but we check for a play here so the game won't crash if we do somehow get here.
        if (gSaveContext.n64ddFlag && play != NULL) {
            if (play->sceneNum == 10) { // ganon's tower -> ganon's castle
                gSaveContext.sohStats.dungeonKeys[13]++;
                if (gSaveContext.inventory.dungeonKeys[13] < 0) {
                    gSaveContext.inventory.dungeonKeys[13] = 1;
                    PerformAutosave(play, item);
                    return ITEM_NONE;
                } else {
                    gSaveContext.inventory.dungeonKeys[13]++;
                    PerformAutosave(play, item);
                    return ITEM_NONE;
                }
            }

            if (play->sceneNum == 92) { // Desert Colossus -> Spirit Temple.
                gSaveContext.sohStats.dungeonKeys[6]++;
                if (gSaveContext.inventory.dungeonKeys[6] < 0) {
                    gSaveContext.inventory.dungeonKeys[6] = 1;
                    PerformAutosave(play, item);
                    return ITEM_NONE;
                } else {
                    gSaveContext.inventory.dungeonKeys[6]++;
                    PerformAutosave(play, item);
                    return ITEM_NONE;
                }
            }
        }
        gSaveContext.sohStats.dungeonKeys[gSaveContext.mapIndex]++;
        if (gSaveContext.inventory.dungeonKeys[gSaveContext.mapIndex] < 0) {
            gSaveContext.inventory.dungeonKeys[gSaveContext.mapIndex] = 1;
            PerformAutosave(play, item);
            return ITEM_NONE;
        } else {
            gSaveContext.inventory.dungeonKeys[gSaveContext.mapIndex]++;
            PerformAutosave(play, item);
            return ITEM_NONE;
        }
    } else if ((item == ITEM_QUIVER_30) || (item == ITEM_BOW)) {
        if (CUR_UPG_VALUE(UPG_QUIVER) == 0) {
            Inventory_ChangeUpgrade(UPG_QUIVER, 1);
            INV_CONTENT(ITEM_BOW) = ITEM_BOW;
            AMMO(ITEM_BOW) = CAPACITY(UPG_QUIVER, 1);
            PerformAutosave(play, item);
            return ITEM_NONE;
        } else {
            AMMO(ITEM_BOW)++;
            if (AMMO(ITEM_BOW) > CUR_CAPACITY(UPG_QUIVER)) {
                AMMO(ITEM_BOW) = CUR_CAPACITY(UPG_QUIVER);
            }
        }
    } else if (item == ITEM_QUIVER_40) {
        Inventory_ChangeUpgrade(UPG_QUIVER, 2);
        AMMO(ITEM_BOW) = CAPACITY(UPG_QUIVER, 2);
        PerformAutosave(play, item);
        return ITEM_NONE;
    } else if (item == ITEM_QUIVER_50) {
        Inventory_ChangeUpgrade(UPG_QUIVER, 3);
        AMMO(ITEM_BOW) = CAPACITY(UPG_QUIVER, 3);
        PerformAutosave(play, item);
        return ITEM_NONE;
    } else if (item == ITEM_BULLET_BAG_40) {
        Inventory_ChangeUpgrade(UPG_BULLET_BAG, 2);
        AMMO(ITEM_SLINGSHOT) = CAPACITY(UPG_BULLET_BAG, 2);
        PerformAutosave(play, item);
        return ITEM_NONE;
    } else if (item == ITEM_BULLET_BAG_50) {
        Inventory_ChangeUpgrade(UPG_BULLET_BAG, 3);
        AMMO(ITEM_SLINGSHOT) = CAPACITY(UPG_BULLET_BAG, 3);
        PerformAutosave(play, item);
        return ITEM_NONE;
    } else if (item == ITEM_BOMB_BAG_20) {
        if (CUR_UPG_VALUE(UPG_BOMB_BAG) == 0) {
            Inventory_ChangeUpgrade(UPG_BOMB_BAG, 1);
            INV_CONTENT(ITEM_BOMB) = ITEM_BOMB;
            AMMO(ITEM_BOMB) = CAPACITY(UPG_BOMB_BAG, 1);
            PerformAutosave(play, item);
            return ITEM_NONE;
        } else {
            AMMO(ITEM_BOMB)++;
            if (AMMO(ITEM_BOMB) > CUR_CAPACITY(UPG_BOMB_BAG)) {
                AMMO(ITEM_BOMB) = CUR_CAPACITY(UPG_BOMB_BAG);
            }
        }
    } else if (item == ITEM_BOMB_BAG_30) {
        Inventory_ChangeUpgrade(UPG_BOMB_BAG, 2);
        AMMO(ITEM_BOMB) = CAPACITY(UPG_BOMB_BAG, 2);
        PerformAutosave(play, item);
        return ITEM_NONE;
    } else if (item == ITEM_BOMB_BAG_40) {
        Inventory_ChangeUpgrade(UPG_BOMB_BAG, 3);
        AMMO(ITEM_BOMB) = CAPACITY(UPG_BOMB_BAG, 3);
        PerformAutosave(play, item);
        return ITEM_NONE;
    } else if (item == ITEM_BRACELET) {
        Inventory_ChangeUpgrade(UPG_STRENGTH, 1);
        PerformAutosave(play, item);
        return ITEM_NONE;
    } else if (item == ITEM_GAUNTLETS_SILVER) {
        Inventory_ChangeUpgrade(UPG_STRENGTH, 2);
        PerformAutosave(play, item);
        return ITEM_NONE;
    } else if (item == ITEM_GAUNTLETS_GOLD) {
        Inventory_ChangeUpgrade(UPG_STRENGTH, 3);
        PerformAutosave(play, item);
        return ITEM_NONE;
    } else if (item == ITEM_SCALE_SILVER) {
        Inventory_ChangeUpgrade(UPG_SCALE, 1);
        PerformAutosave(play, item);
        return ITEM_NONE;
    } else if (item == ITEM_SCALE_GOLDEN) {
        Inventory_ChangeUpgrade(UPG_SCALE, 2);
        PerformAutosave(play, item);
        return ITEM_NONE;
    } else if (item == ITEM_WALLET_ADULT) {
        Inventory_ChangeUpgrade(UPG_WALLET, 1);
        if (gSaveContext.n64ddFlag && Randomizer_GetSettingValue(RSK_FULL_WALLETS)) {
            Rupees_ChangeBy(200);
        }
        PerformAutosave(play, item);
        return ITEM_NONE;
    } else if (item == ITEM_WALLET_GIANT) {
        Inventory_ChangeUpgrade(UPG_WALLET, 2);
        if (gSaveContext.n64ddFlag && Randomizer_GetSettingValue(RSK_FULL_WALLETS)) {
            Rupees_ChangeBy(500);
        }
        PerformAutosave(play, item);
        return ITEM_NONE;
    } else if (item == ITEM_STICK_UPGRADE_20) {
        if (gSaveContext.inventory.items[slot] == ITEM_NONE) {
            INV_CONTENT(ITEM_STICK) = ITEM_STICK;
        }
        Inventory_ChangeUpgrade(UPG_STICKS, 2);
        AMMO(ITEM_STICK) = CAPACITY(UPG_STICKS, 2);
        PerformAutosave(play, item);
        return ITEM_NONE;
    } else if (item == ITEM_STICK_UPGRADE_30) {
        if (gSaveContext.inventory.items[slot] == ITEM_NONE) {
            INV_CONTENT(ITEM_STICK) = ITEM_STICK;
        }
        Inventory_ChangeUpgrade(UPG_STICKS, 3);
        AMMO(ITEM_STICK) = CAPACITY(UPG_STICKS, 3);
        PerformAutosave(play, item);
        return ITEM_NONE;
    } else if (item == ITEM_NUT_UPGRADE_30) {
        if (gSaveContext.inventory.items[slot] == ITEM_NONE) {
            INV_CONTENT(ITEM_NUT) = ITEM_NUT;
        }
        Inventory_ChangeUpgrade(UPG_NUTS, 2);
        AMMO(ITEM_NUT) = CAPACITY(UPG_NUTS, 2);
        PerformAutosave(play, item);
        return ITEM_NONE;
    } else if (item == ITEM_NUT_UPGRADE_40) {
        if (gSaveContext.inventory.items[slot] == ITEM_NONE) {
            INV_CONTENT(ITEM_NUT) = ITEM_NUT;
        }
        Inventory_ChangeUpgrade(UPG_NUTS, 3);
        AMMO(ITEM_NUT) = CAPACITY(UPG_NUTS, 3);
        PerformAutosave(play, item);
        return ITEM_NONE;
    } else if (item == ITEM_LONGSHOT) {
        INV_CONTENT(item) = item;
        // always update "equips" as this is what is currently on the c-buttons
        for (i = 1; i < ARRAY_COUNT(gSaveContext.equips.buttonItems); i++) {
            if (gSaveContext.equips.buttonItems[i] == ITEM_HOOKSHOT) {
                gSaveContext.equips.buttonItems[i] = ITEM_LONGSHOT;
                if (play != NULL) {
                    Interface_LoadItemIcon1(play, i);
                }
            }
        }
        // update the adult/child equips when rando'd (accounting for equp swapped hookshot as child)
        if (gSaveContext.n64ddFlag && LINK_IS_CHILD) {
            for (i = 1; i < ARRAY_COUNT(gSaveContext.adultEquips.buttonItems); i++) {
                if (gSaveContext.adultEquips.buttonItems[i] == ITEM_HOOKSHOT) {
                    gSaveContext.adultEquips.buttonItems[i] = ITEM_LONGSHOT;
                    if (play != NULL) {
                        Interface_LoadItemIcon1(play, i);
                    }
                }
            }
        }
        if (gSaveContext.n64ddFlag && LINK_IS_ADULT) {
            for (i = 1; i < ARRAY_COUNT(gSaveContext.childEquips.buttonItems); i++) {
                if (gSaveContext.childEquips.buttonItems[i] == ITEM_HOOKSHOT) {
                    gSaveContext.childEquips.buttonItems[i] = ITEM_LONGSHOT;
                    if (play != NULL) {
                        Interface_LoadItemIcon1(play, i);
                    }
                }
            }
        }
        PerformAutosave(play, item);
        return ITEM_NONE;
    } else if (item == ITEM_STICK) {
        if (gSaveContext.inventory.items[slot] == ITEM_NONE) {
            Inventory_ChangeUpgrade(UPG_STICKS, 1);
            AMMO(ITEM_STICK) = 1;
        } else {
            AMMO(ITEM_STICK)++;
            if (AMMO(ITEM_STICK) > CUR_CAPACITY(UPG_STICKS)) {
                AMMO(ITEM_STICK) = CUR_CAPACITY(UPG_STICKS);
            }
        }
    } else if ((item == ITEM_STICKS_5) || (item == ITEM_STICKS_10)) {
        if (gSaveContext.inventory.items[slot] == ITEM_NONE) {
            Inventory_ChangeUpgrade(UPG_STICKS, 1);
            AMMO(ITEM_STICK) = sAmmoRefillCounts[item - ITEM_STICKS_5];
        } else {
            AMMO(ITEM_STICK) += sAmmoRefillCounts[item - ITEM_STICKS_5];
            if (AMMO(ITEM_STICK) > CUR_CAPACITY(UPG_STICKS)) {
                AMMO(ITEM_STICK) = CUR_CAPACITY(UPG_STICKS);
            }
        }
        item = ITEM_STICK;
    } else if (item == ITEM_NUT) {
        if (gSaveContext.inventory.items[slot] == ITEM_NONE) {
            Inventory_ChangeUpgrade(UPG_NUTS, 1);
            AMMO(ITEM_NUT) = ITEM_NUT;
        } else {
            AMMO(ITEM_NUT)++;
            if (AMMO(ITEM_NUT) > CUR_CAPACITY(UPG_NUTS)) {
                AMMO(ITEM_NUT) = CUR_CAPACITY(UPG_NUTS);
            }
        }
    } else if ((item == ITEM_NUTS_5) || (item == ITEM_NUTS_10)) {
        if (gSaveContext.inventory.items[slot] == ITEM_NONE) {
            Inventory_ChangeUpgrade(UPG_NUTS, 1);
            AMMO(ITEM_NUT) += sAmmoRefillCounts[item - ITEM_NUTS_5];
            // "Deku Nuts %d(%d)=%d BS_count=%d"
            osSyncPrintf("デクの実 %d(%d)=%d  BS_count=%d\n", item, ITEM_NUTS_5, item - ITEM_NUTS_5,
                         sAmmoRefillCounts[item - ITEM_NUTS_5]);
        } else {
            AMMO(ITEM_NUT) += sAmmoRefillCounts[item - ITEM_NUTS_5];
            if (AMMO(ITEM_NUT) > CUR_CAPACITY(UPG_NUTS)) {
                AMMO(ITEM_NUT) = CUR_CAPACITY(UPG_NUTS);
            }
        }
        item = ITEM_NUT;
    } else if (item == ITEM_BOMB) {
        // "Bomb  Bomb  Bomb  Bomb Bomb   Bomb Bomb"
        osSyncPrintf(" 爆弾  爆弾  爆弾  爆弾 爆弾   爆弾 爆弾 \n");
        if ((AMMO(ITEM_BOMB) += 1) > CUR_CAPACITY(UPG_BOMB_BAG)) {
            AMMO(ITEM_BOMB) = CUR_CAPACITY(UPG_BOMB_BAG);
        }
        PerformAutosave(play, item);
        return ITEM_NONE;
    } else if ((item >= ITEM_BOMBS_5) && (item <= ITEM_BOMBS_30)) {
        if ((AMMO(ITEM_BOMB) += sAmmoRefillCounts[item - ITEM_BOMBS_5]) > CUR_CAPACITY(UPG_BOMB_BAG)) {
            AMMO(ITEM_BOMB) = CUR_CAPACITY(UPG_BOMB_BAG);
        }
        PerformAutosave(play, item);
        return ITEM_NONE;
    } else if (item == ITEM_BOMBCHU) {
        if (gSaveContext.inventory.items[slot] == ITEM_NONE) {
            INV_CONTENT(ITEM_BOMBCHU) = ITEM_BOMBCHU;
            AMMO(ITEM_BOMBCHU) = 10;
            PerformAutosave(play, item);
            return ITEM_NONE;
        } else {
            AMMO(ITEM_BOMBCHU) += 10;
            if (AMMO(ITEM_BOMBCHU) > 50) {
                AMMO(ITEM_BOMBCHU) = 50;
            }
            PerformAutosave(play, item);
            return ITEM_NONE;
        }
    } else if ((item == ITEM_BOMBCHUS_5) || (item == ITEM_BOMBCHUS_20)) {
        if (gSaveContext.inventory.items[slot] == ITEM_NONE) {
            INV_CONTENT(ITEM_BOMBCHU) = ITEM_BOMBCHU;
            AMMO(ITEM_BOMBCHU) += sAmmoRefillCounts[item - ITEM_BOMBCHUS_5 + 8];
            PerformAutosave(play, item);
            return ITEM_NONE;
        } else {
            AMMO(ITEM_BOMBCHU) += sAmmoRefillCounts[item - ITEM_BOMBCHUS_5 + 8];
            if (AMMO(ITEM_BOMBCHU) > 50) {
                AMMO(ITEM_BOMBCHU) = 50;
            }
            PerformAutosave(play, item);
            return ITEM_NONE;
        }
    } else if ((item >= ITEM_ARROWS_SMALL) && (item <= ITEM_ARROWS_LARGE)) {
        AMMO(ITEM_BOW) += sAmmoRefillCounts[item - ITEM_ARROWS_SMALL + 4];

        if ((AMMO(ITEM_BOW) >= CUR_CAPACITY(UPG_QUIVER)) || (AMMO(ITEM_BOW) < 0)) {
            AMMO(ITEM_BOW) = CUR_CAPACITY(UPG_QUIVER);
        }

        osSyncPrintf("%d本  Item_MaxGet=%d\n", AMMO(ITEM_BOW), CUR_CAPACITY(UPG_QUIVER));

        PerformAutosave(play, item);
        return ITEM_BOW;
    } else if (item == ITEM_SLINGSHOT) {
        Inventory_ChangeUpgrade(UPG_BULLET_BAG, 1);
        INV_CONTENT(ITEM_SLINGSHOT) = ITEM_SLINGSHOT;
        AMMO(ITEM_SLINGSHOT) = 30;
        PerformAutosave(play, item);
        return ITEM_NONE;
    } else if (item == ITEM_SEEDS) {
        AMMO(ITEM_SLINGSHOT) += 5;

        if (AMMO(ITEM_SLINGSHOT) >= CUR_CAPACITY(UPG_BULLET_BAG)) {
            AMMO(ITEM_SLINGSHOT) = CUR_CAPACITY(UPG_BULLET_BAG);
        }

        if (!(gSaveContext.itemGetInf[1] & 8)) {
            gSaveContext.itemGetInf[1] |= 8;
            PerformAutosave(play, item);
            return ITEM_NONE;
        }

        PerformAutosave(play, item);
        return ITEM_SEEDS;
    } else if (item == ITEM_SEEDS_30) {
        AMMO(ITEM_SLINGSHOT) += 30;

        if (AMMO(ITEM_SLINGSHOT) >= CUR_CAPACITY(UPG_BULLET_BAG)) {
            AMMO(ITEM_SLINGSHOT) = CUR_CAPACITY(UPG_BULLET_BAG);
        }

        if (!(gSaveContext.itemGetInf[1] & 8)) {
            gSaveContext.itemGetInf[1] |= 8;
            PerformAutosave(play, item);
            return ITEM_NONE;
        }

        PerformAutosave(play, item);
        return ITEM_SEEDS;
    } else if (item == ITEM_OCARINA_FAIRY) {
        INV_CONTENT(ITEM_OCARINA_FAIRY) = ITEM_OCARINA_FAIRY;
        PerformAutosave(play, item);
        return ITEM_NONE;
    } else if (item == ITEM_OCARINA_TIME) {
        INV_CONTENT(ITEM_OCARINA_TIME) = ITEM_OCARINA_TIME;
        // always update "equips" as this is what is currently on the c-buttons
        for (i = 1; i < ARRAY_COUNT(gSaveContext.equips.buttonItems); i++) {
            if (gSaveContext.equips.buttonItems[i] == ITEM_OCARINA_FAIRY) {
                gSaveContext.equips.buttonItems[i] = ITEM_OCARINA_TIME;
                Interface_LoadItemIcon1(play, i);
            }
        }

        // update the adult/child equips when rando'd
        if (gSaveContext.n64ddFlag && LINK_IS_CHILD) {
            for (i = 1; i < ARRAY_COUNT(gSaveContext.adultEquips.buttonItems); i++) {
                if (gSaveContext.adultEquips.buttonItems[i] == ITEM_OCARINA_FAIRY) {
                    gSaveContext.adultEquips.buttonItems[i] = ITEM_OCARINA_TIME;
                    if (play != NULL) {
                        Interface_LoadItemIcon1(play, i);
                    }
                }
            }
        }
        if (gSaveContext.n64ddFlag && LINK_IS_ADULT) {
            for (i = 1; i < ARRAY_COUNT(gSaveContext.childEquips.buttonItems); i++) {
                if (gSaveContext.childEquips.buttonItems[i] == ITEM_OCARINA_FAIRY) {
                    gSaveContext.childEquips.buttonItems[i] = ITEM_OCARINA_TIME;
                    if (play != NULL) {
                        Interface_LoadItemIcon1(play, i);
                    }
                }
            }
        }
        PerformAutosave(play, item);
        return ITEM_NONE;
    } else if (item == ITEM_BEAN) {
        if (gSaveContext.inventory.items[slot] == ITEM_NONE) {
            INV_CONTENT(item) = item;
            AMMO(ITEM_BEAN) = 1;
            BEANS_BOUGHT = 1;
        } else {
            AMMO(ITEM_BEAN)++;
            BEANS_BOUGHT++;
        }
        PerformAutosave(play, item);
        return ITEM_NONE;
    } else if ((item == ITEM_HEART_PIECE_2) || (item == ITEM_HEART_PIECE)) {
        gSaveContext.inventory.questItems += 1 << (QUEST_HEART_PIECE + 4);
        gSaveContext.sohStats.heartPieces++;
        PerformAutosave(play, item);
        return ITEM_NONE;
    } else if (item == ITEM_HEART_CONTAINER) {
        gSaveContext.healthCapacity += 0x10;
        gSaveContext.health += 0x10;
        gSaveContext.sohStats.heartContainers++;
        PerformAutosave(play, item);
        return ITEM_NONE;
    } else if (item == ITEM_HEART) {
        osSyncPrintf("回復ハート回復ハート回復ハート\n"); // "Recovery Heart"
        if (play != NULL) {
            Health_ChangeBy(play, 0x10);
        }
        PerformAutosave(play, item);
        return item;
    } else if (item == ITEM_MAGIC_SMALL) {
        if (gSaveContext.unk_13F0 != 10) {
            if (play != NULL) {
                Magic_Fill(play);
            }
        }

        if (play != NULL) {
            func_80087708(play, 12, 5);
        }

        if (!(gSaveContext.infTable[25] & 0x100)) {
            gSaveContext.infTable[25] |= 0x100;
            PerformAutosave(play, item);
            return ITEM_NONE;
        }

        PerformAutosave(play, item);
        return item;
    } else if (item == ITEM_MAGIC_LARGE) {
        if (gSaveContext.unk_13F0 != 10) {
            if (play != NULL) {
                Magic_Fill(play);
            }
        }
        if (play != NULL) {
            func_80087708(play, 24, 5);
        }

        if (!(gSaveContext.infTable[25] & 0x100)) {
            gSaveContext.infTable[25] |= 0x100;
            PerformAutosave(play, item);
            return ITEM_NONE;
        }

        PerformAutosave(play, item);
        return item;
    } else if ((item >= ITEM_RUPEE_GREEN) && (item <= ITEM_INVALID_8)) {
        Rupees_ChangeBy(sAmmoRefillCounts[item - ITEM_RUPEE_GREEN + 10]);
        PerformAutosave(play, item);
        return ITEM_NONE;
    } else if (item == ITEM_BOTTLE) {
        temp = SLOT(item);

        for (i = 0; i < 4; i++) {
            if (gSaveContext.inventory.items[temp + i] == ITEM_NONE) {
                gSaveContext.inventory.items[temp + i] = item;
                PerformAutosave(play, item);
                return ITEM_NONE;
            }
        }
    } else if (((item >= ITEM_POTION_RED) && (item <= ITEM_POE)) || (item == ITEM_MILK)) {
        temp = SLOT(item);

        if ((item != ITEM_MILK_BOTTLE) && (item != ITEM_LETTER_RUTO)) {
            if (item == ITEM_MILK) {
                item = ITEM_MILK_BOTTLE;
                temp = SLOT(item);
            }

            for (i = 0; i < 4; i++) {
                if (gSaveContext.inventory.items[temp + i] == ITEM_BOTTLE) {
                    // "Item_Pt(1)=%d Item_Pt(2)=%d Item_Pt(3)=%d   Empty Bottle=%d   Content=%d"
                    osSyncPrintf("Item_Pt(1)=%d Item_Pt(2)=%d Item_Pt(3)=%d   空瓶=%d   中味=%d\n",
                                 gSaveContext.equips.cButtonSlots[0], gSaveContext.equips.cButtonSlots[1],
                                 gSaveContext.equips.cButtonSlots[2], temp + i, item);

                    for (int buttonIndex = 1; buttonIndex < ARRAY_COUNT(gSaveContext.equips.buttonItems); buttonIndex++) {
                        if ((temp + i) == gSaveContext.equips.cButtonSlots[buttonIndex - 1]) {
                            gSaveContext.equips.buttonItems[buttonIndex] = item;
                            if (play != NULL) {
                                Interface_LoadItemIcon2(play, buttonIndex);
                            }
                            gSaveContext.buttonStatus[BUTTON_STATUS_INDEX(buttonIndex)] = BTN_ENABLED;
                            break;
                        }
                    }

                    gSaveContext.inventory.items[temp + i] = item;
                    PerformAutosave(play, item);
                    return ITEM_NONE;
                }
            }
        } else {
            for (i = 0; i < 4; i++) {
                if (gSaveContext.inventory.items[temp + i] == ITEM_NONE) {
                    gSaveContext.inventory.items[temp + i] = item;
                    PerformAutosave(play, item);
                    return ITEM_NONE;
                }
            }
        }
    } else if ((item >= ITEM_WEIRD_EGG) && (item <= ITEM_CLAIM_CHECK)) {
        if ((item == ITEM_SAW) && CVar_GetS32("gDekuNutUpgradeFix", 0) == 0) {
            gSaveContext.itemGetInf[1] |= 0x8000;
        }

        if (item >= ITEM_POCKET_EGG) {
            gSaveContext.adultTradeItems |= ADULT_TRADE_FLAG(item);
        }

        temp = INV_CONTENT(item);
        INV_CONTENT(item) = item;

        if (temp != ITEM_NONE) {
            for (i = 1; i < ARRAY_COUNT(gSaveContext.equips.buttonItems); i++) {
                if (temp == gSaveContext.equips.buttonItems[i]) {
                    if (item != ITEM_SOLD_OUT) {
                        gSaveContext.equips.buttonItems[i] = item;
                        if (play != NULL) {
                            Interface_LoadItemIcon1(play, i);
                        }
                    } else {
                        gSaveContext.equips.buttonItems[i] = ITEM_NONE;
                    }
                    PerformAutosave(play, item);
                    return ITEM_NONE;
                }
            }
        }

        PerformAutosave(play, item);
        return ITEM_NONE;
    }

    temp = gSaveContext.inventory.items[slot];
    osSyncPrintf("Item_Register(%d)=%d  %d\n", slot, item, temp);
    INV_CONTENT(item) = item;

    PerformAutosave(play, item);
    return temp;
}

u16 Randomizer_Item_Give(PlayState* play, GetItemEntry giEntry) {
    uint16_t item = giEntry.itemId;
    uint16_t temp;
    uint16_t i;
    uint16_t slot;

    // Gameplay stats: Update the time the item was obtained
    Randomizer_GameplayStats_SetTimestamp(item);

    slot = SLOT(item);
    if (item == RG_MAGIC_SINGLE) {
        gSaveContext.magicAcquired = true;
        gSaveContext.unk_13F6 = 0x30;
        Magic_Fill(play);
        return RG_NONE;
    } else if (item == RG_MAGIC_DOUBLE) {
        if (!gSaveContext.magicAcquired) {
            gSaveContext.magicAcquired = true;
        }
        gSaveContext.doubleMagic = true;
        gSaveContext.unk_13F6 = 0x60;
        gSaveContext.magicLevel = 0;
        Magic_Fill(play);
        return RG_NONE;
    }

    if (item == RG_MAGIC_BEAN_PACK) {
        if (INV_CONTENT(ITEM_BEAN) == ITEM_NONE) {
            INV_CONTENT(ITEM_BEAN) = ITEM_BEAN;
            AMMO(ITEM_BEAN) = 10;
        }
        return RG_NONE;
    }

    if (item == RG_DOUBLE_DEFENSE) {
        gSaveContext.doubleDefense = true;
        gSaveContext.inventory.defenseHearts = 20;
        gSaveContext.healthAccumulator = 0x140;
        return RG_NONE;
    }

    if (item >= RG_BOTTLE_WITH_RED_POTION && item <= RG_BOTTLE_WITH_BIG_POE) {
        temp = SLOT(ITEM_BOTTLE);
        for (i = 0; i < 4; i++) {
            if (gSaveContext.inventory.items[temp + i] == ITEM_NONE) {
                switch (item) {
                    case RG_BOTTLE_WITH_RED_POTION:
                        item = ITEM_POTION_RED;
                        break;
                    case RG_BOTTLE_WITH_GREEN_POTION:
                        item = ITEM_POTION_GREEN;
                        break;
                    case RG_BOTTLE_WITH_BLUE_POTION:
                        item = ITEM_POTION_BLUE;
                        break;
                    case RG_BOTTLE_WITH_FAIRY:
                        item = ITEM_FAIRY;
                        break;
                    case RG_BOTTLE_WITH_FISH:
                        item = ITEM_FISH;
                        break;
                    case RG_BOTTLE_WITH_BLUE_FIRE:
                        item = ITEM_BLUE_FIRE;
                        break;
                    case RG_BOTTLE_WITH_BUGS:
                        item = ITEM_BUG;
                        break;
                    case RG_BOTTLE_WITH_POE:
                        item = ITEM_POE;
                        break;
                    case RG_BOTTLE_WITH_BIG_POE:
                        item = ITEM_BIG_POE;
                        break;
                }

                gSaveContext.inventory.items[temp + i] = item;
                return ITEM_NONE;
            }
        }
    } else if ((item >= RG_FOREST_TEMPLE_SMALL_KEY && item <= RG_GANONS_CASTLE_SMALL_KEY) ||
                (item >= RG_FOREST_TEMPLE_KEY_RING && item <= RG_GANONS_CASTLE_KEY_RING) ||
                (item >= RG_FOREST_TEMPLE_BOSS_KEY && item <= RG_GANONS_CASTLE_BOSS_KEY) ||
                (item >= RG_DEKU_TREE_MAP && item <= RG_ICE_CAVERN_MAP) ||
                (item >= RG_DEKU_TREE_COMPASS && item <= RG_ICE_CAVERN_COMPASS)) {
        int mapIndex = gSaveContext.mapIndex;
        int numOfKeysOnKeyring = 0;
        switch (item) {
            case RG_DEKU_TREE_MAP:
            case RG_DEKU_TREE_COMPASS:
                mapIndex = SCENE_YDAN;
                break;
            case RG_DODONGOS_CAVERN_MAP:
            case RG_DODONGOS_CAVERN_COMPASS:
                mapIndex = SCENE_DDAN;
                break;
            case RG_JABU_JABUS_BELLY_MAP:
            case RG_JABU_JABUS_BELLY_COMPASS:
                mapIndex = SCENE_BDAN;
                break;
            case RG_FOREST_TEMPLE_MAP:
            case RG_FOREST_TEMPLE_COMPASS:
            case RG_FOREST_TEMPLE_SMALL_KEY:
            case RG_FOREST_TEMPLE_KEY_RING:
            case RG_FOREST_TEMPLE_BOSS_KEY:
                mapIndex = SCENE_BMORI1;
                numOfKeysOnKeyring = FOREST_TEMPLE_SMALL_KEY_MAX;
                break;
            case RG_FIRE_TEMPLE_MAP:
            case RG_FIRE_TEMPLE_COMPASS:
            case RG_FIRE_TEMPLE_SMALL_KEY:
            case RG_FIRE_TEMPLE_KEY_RING:
            case RG_FIRE_TEMPLE_BOSS_KEY:
                mapIndex = SCENE_HIDAN;
                numOfKeysOnKeyring = FIRE_TEMPLE_SMALL_KEY_MAX;
                break;
            case RG_WATER_TEMPLE_MAP:
            case RG_WATER_TEMPLE_COMPASS:
            case RG_WATER_TEMPLE_SMALL_KEY:
            case RG_WATER_TEMPLE_KEY_RING:
            case RG_WATER_TEMPLE_BOSS_KEY:
                mapIndex = SCENE_MIZUSIN;
                numOfKeysOnKeyring = WATER_TEMPLE_SMALL_KEY_MAX;
                break;
            case RG_SPIRIT_TEMPLE_MAP:
            case RG_SPIRIT_TEMPLE_COMPASS:
            case RG_SPIRIT_TEMPLE_SMALL_KEY:
            case RG_SPIRIT_TEMPLE_KEY_RING:
            case RG_SPIRIT_TEMPLE_BOSS_KEY:
                mapIndex = SCENE_JYASINZOU;
                numOfKeysOnKeyring = SPIRIT_TEMPLE_SMALL_KEY_MAX;
                break;
            case RG_SHADOW_TEMPLE_MAP:
            case RG_SHADOW_TEMPLE_COMPASS:
            case RG_SHADOW_TEMPLE_SMALL_KEY:
            case RG_SHADOW_TEMPLE_KEY_RING:
            case RG_SHADOW_TEMPLE_BOSS_KEY:
                mapIndex = SCENE_HAKADAN;
                numOfKeysOnKeyring = SHADOW_TEMPLE_SMALL_KEY_MAX;
                break;
            case RG_BOTTOM_OF_THE_WELL_MAP:
            case RG_BOTTOM_OF_THE_WELL_COMPASS:
            case RG_BOTTOM_OF_THE_WELL_SMALL_KEY:
            case RG_BOTTOM_OF_THE_WELL_KEY_RING:
                mapIndex = SCENE_HAKADANCH;
                numOfKeysOnKeyring = BOTTOM_OF_THE_WELL_SMALL_KEY_MAX;
                break;
            case RG_ICE_CAVERN_MAP:
            case RG_ICE_CAVERN_COMPASS:
                mapIndex = SCENE_ICE_DOUKUTO;
                break;
            case RG_GANONS_CASTLE_BOSS_KEY:
                mapIndex = SCENE_GANON;
                break;
            case RG_GERUDO_TRAINING_GROUNDS_SMALL_KEY:
            case RG_GERUDO_TRAINING_GROUNDS_KEY_RING:
                mapIndex = SCENE_MEN;
                numOfKeysOnKeyring = GERUDO_TRAINING_GROUNDS_SMALL_KEY_MAX;
                break;
            case RG_GERUDO_FORTRESS_SMALL_KEY:
            case RG_GERUDO_FORTRESS_KEY_RING:
                mapIndex = SCENE_GERUDOWAY;
                numOfKeysOnKeyring = GERUDO_FORTRESS_SMALL_KEY_MAX;
                break;
            case RG_GANONS_CASTLE_SMALL_KEY:
            case RG_GANONS_CASTLE_KEY_RING:
                mapIndex = SCENE_GANONTIKA;
                numOfKeysOnKeyring = GANONS_CASTLE_SMALL_KEY_MAX;
                break;
        }

        if ((item >= RG_FOREST_TEMPLE_SMALL_KEY) && (item <= RG_GANONS_CASTLE_SMALL_KEY)) {
            gSaveContext.sohStats.dungeonKeys[mapIndex]++;
            if (gSaveContext.inventory.dungeonKeys[mapIndex] < 0) {
                gSaveContext.inventory.dungeonKeys[mapIndex] = 1;
                return RG_NONE;
            } else {
                gSaveContext.inventory.dungeonKeys[mapIndex]++;
                return RG_NONE;
            }
        } else if ((item >= RG_FOREST_TEMPLE_KEY_RING) && (item <= RG_GANONS_CASTLE_KEY_RING)) {
            gSaveContext.sohStats.dungeonKeys[mapIndex] = numOfKeysOnKeyring;
            gSaveContext.inventory.dungeonKeys[mapIndex] = numOfKeysOnKeyring;
            return RG_NONE;
        } else {
            int bitmask;
            if ((item >= RG_DEKU_TREE_MAP) && (item <= RG_ICE_CAVERN_MAP)) {
                bitmask = gBitFlags[2];
            } else if ((item >= RG_DEKU_TREE_COMPASS) && (item <= RG_ICE_CAVERN_COMPASS)) {
                bitmask = gBitFlags[1];
            } else {
                bitmask = gBitFlags[0];
            }

            gSaveContext.inventory.dungeonItems[mapIndex] |= bitmask;
            return RG_NONE;
        }
    }

    if (item == RG_TYCOON_WALLET) {
        Inventory_ChangeUpgrade(UPG_WALLET, 3);
        if (gSaveContext.n64ddFlag && Randomizer_GetSettingValue(RSK_FULL_WALLETS)) {
            Rupees_ChangeBy(999);
        }
        return RG_NONE;
    }

    temp = gSaveContext.inventory.items[slot];
    osSyncPrintf("Item_Register(%d)=%d  %d\n", slot, item, temp);
    INV_CONTENT(item) = item;

    return temp;
}

u8 Item_CheckObtainability(u8 item) {
    s16 i;
    s16 slot = SLOT(item);
    s32 temp;

    if (item >= ITEM_STICKS_5) {
        slot = SLOT(sExtraItemBases[item - ITEM_STICKS_5]);
    }

    osSyncPrintf(VT_FGCOL(GREEN));
    osSyncPrintf("item_get_non_setting=%d  pt=%d  z=%x\n", item, slot, gSaveContext.inventory.items[slot]);
    osSyncPrintf(VT_RST);

    if (gSaveContext.n64ddFlag) {
        if (item == ITEM_SINGLE_MAGIC || item == ITEM_DOUBLE_MAGIC || item == ITEM_DOUBLE_DEFENSE) {
            return ITEM_NONE;
        }
    }
    
    if ((item >= ITEM_SONG_MINUET) && (item <= ITEM_SONG_STORMS)) {
        return ITEM_NONE;
    } else if ((item >= ITEM_MEDALLION_FOREST) && (item <= ITEM_MEDALLION_LIGHT)) {
        return ITEM_NONE;
    } else if ((item >= ITEM_KOKIRI_EMERALD) && (item <= ITEM_SKULL_TOKEN)) {
        return ITEM_NONE;
    } else if ((item >= ITEM_SWORD_KOKIRI) && (item <= ITEM_SWORD_BGS)) {
        if (item == ITEM_SWORD_BGS) {
            return ITEM_NONE;
        } else if ((gBitFlags[item - ITEM_SWORD_KOKIRI] << gEquipShifts[EQUIP_SWORD]) &
                   gSaveContext.inventory.equipment) {
            return gSaveContext.n64ddFlag ? ITEM_NONE : item;
        } else {
            return ITEM_NONE;
        }
    } else if ((item >= ITEM_SHIELD_DEKU) && (item <= ITEM_SHIELD_MIRROR)) {
        if ((gBitFlags[item - ITEM_SHIELD_DEKU] << gEquipShifts[EQUIP_SHIELD]) & gSaveContext.inventory.equipment) {
            return gSaveContext.n64ddFlag ? ITEM_NONE : item;
        } else {
            return ITEM_NONE;
        }
    } else if ((item >= ITEM_TUNIC_KOKIRI) && (item <= ITEM_TUNIC_ZORA)) {
        if ((gBitFlags[item - ITEM_TUNIC_KOKIRI] << gEquipShifts[EQUIP_TUNIC]) & gSaveContext.inventory.equipment) {
            return gSaveContext.n64ddFlag ? ITEM_NONE : item;
        } else {
            return ITEM_NONE;
        }
    } else if ((item >= ITEM_BOOTS_KOKIRI) && (item <= ITEM_BOOTS_HOVER)) {
        if ((gBitFlags[item - ITEM_BOOTS_KOKIRI] << gEquipShifts[EQUIP_BOOTS]) & gSaveContext.inventory.equipment) {
            return gSaveContext.n64ddFlag ? ITEM_NONE : item;
        } else {
            return ITEM_NONE;
        }
    } else if ((item == ITEM_KEY_BOSS) || (item == ITEM_COMPASS) || (item == ITEM_DUNGEON_MAP)) {
        return ITEM_NONE;
    } else if (item == ITEM_KEY_SMALL) {
        return ITEM_NONE;
    } else if ((item >= ITEM_SLINGSHOT) && (item <= ITEM_BOMBCHU)) {
        return ITEM_NONE;
    } else if ((item == ITEM_BOMBCHUS_5) || (item == ITEM_BOMBCHUS_20)) {
        return ITEM_NONE;
    } else if ((item == ITEM_QUIVER_30) || (item == ITEM_BOW)) {
        if (CUR_UPG_VALUE(UPG_QUIVER) == 0) {
            return ITEM_NONE;
        } else {
            return 0;
        }
    } else if ((item == ITEM_QUIVER_40) || (item == ITEM_QUIVER_50)) {
        return ITEM_NONE;
    } else if ((item == ITEM_BULLET_BAG_40) || (item == ITEM_BULLET_BAG_50)) {
        return ITEM_NONE;
    } else if ((item == ITEM_BOMB_BAG_20) || (item == ITEM_BOMB)) {
        if (CUR_UPG_VALUE(UPG_BOMB_BAG) == 0) {
            return ITEM_NONE;
        } else {
            return 0;
        }
    } else if ((item >= ITEM_STICK_UPGRADE_20) && (item <= ITEM_NUT_UPGRADE_40)) {
        return ITEM_NONE;
    } else if ((item >= ITEM_BOMB_BAG_30) && (item <= ITEM_WALLET_GIANT)) {
        return ITEM_NONE;
    } else if (item == ITEM_LONGSHOT) {
        return ITEM_NONE;
    } else if ((item == ITEM_SEEDS) || (item == ITEM_SEEDS_30)) {
        if (!(gSaveContext.itemGetInf[1] & 0x8)) {
            return ITEM_NONE;
        } else {
            return ITEM_SEEDS;
        }
    } else if (item == ITEM_BEAN) {
        return ITEM_NONE;
    } else if ((item == ITEM_HEART_PIECE_2) || (item == ITEM_HEART_PIECE)) {
        return ITEM_NONE;
    } else if (item == ITEM_HEART_CONTAINER) {
        return ITEM_NONE;
    } else if (item == ITEM_HEART) {
        return ITEM_HEART;
    } else if ((item == ITEM_MAGIC_SMALL) || (item == ITEM_MAGIC_LARGE)) {
        // "Magic Pot Get_Inf_Table( 25, 0x0100)=%d"
        osSyncPrintf("魔法の壷 Get_Inf_Table( 25, 0x0100)=%d\n", gSaveContext.infTable[25] & 0x100);
        if (!(gSaveContext.infTable[25] & 0x100)) {
            return ITEM_NONE;
        } else {
            return item;
        }
    } else if ((item >= ITEM_RUPEE_GREEN) && (item <= ITEM_INVALID_8)) {
        return ITEM_NONE;
    } else if (item == ITEM_BOTTLE) {
        return ITEM_NONE;
    } else if (((item >= ITEM_POTION_RED) && (item <= ITEM_POE)) || (item == ITEM_MILK)) {
        temp = SLOT(item);

        if ((item != ITEM_MILK_BOTTLE) && (item != ITEM_LETTER_RUTO)) {
            if (item == ITEM_MILK) {
                item = ITEM_MILK_BOTTLE;
                temp = SLOT(item);
            }

            for (i = 0; i < 4; i++) {
                if (gSaveContext.inventory.items[temp + i] == ITEM_BOTTLE) {
                    return ITEM_NONE;
                }
            }
        } else {
            for (i = 0; i < 4; i++) {
                if (gSaveContext.inventory.items[temp + i] == ITEM_NONE) {
                    return ITEM_NONE;
                }
            }
        }
    } else if ((item >= ITEM_WEIRD_EGG) && (item <= ITEM_CLAIM_CHECK)) {
        return ITEM_NONE;
    }

    return gSaveContext.inventory.items[slot];
}

void PerformAutosave(PlayState* play, u8 item) {
    if (CVar_GetS32("gAutosave", 0)) {
        if (CVar_GetS32("gAutosaveAllItems", 0)) {
            Play_PerformSave(play);
        } else if (CVar_GetS32("gAutosaveMajorItems", 1)) {
            switch (item) {
                case ITEM_STICK:
                case ITEM_NUT:
                case ITEM_BOMB:
                case ITEM_BOW:
                case ITEM_SEEDS:
                case ITEM_FISHING_POLE:
                case ITEM_MAGIC_SMALL:
                case ITEM_MAGIC_LARGE:
                case ITEM_INVALID_4:
                case ITEM_INVALID_5:
                case ITEM_INVALID_6:
                case ITEM_INVALID_7:
                case ITEM_HEART:
                case ITEM_RUPEE_GREEN:
                case ITEM_RUPEE_BLUE:
                case ITEM_RUPEE_RED:
                case ITEM_RUPEE_PURPLE:
                case ITEM_RUPEE_GOLD:
                case ITEM_INVALID_8:
                case ITEM_STICKS_5:
                case ITEM_STICKS_10:
                case ITEM_NUTS_5:
                case ITEM_NUTS_10:
                case ITEM_BOMBS_5:
                case ITEM_BOMBS_10:
                case ITEM_BOMBS_20:
                case ITEM_BOMBS_30:
                case ITEM_ARROWS_SMALL:
                case ITEM_ARROWS_MEDIUM:
                case ITEM_ARROWS_LARGE:
                case ITEM_SEEDS_30:
                    break;
                case ITEM_SWORD_MASTER:
                    if (play->sceneNum == SCENE_GANON_DEMO) {
                        break;
                    }
                default:
                    Play_PerformSave(play);
                    break;
            }
        }
    }
}

void Inventory_DeleteItem(u16 item, u16 invSlot) {
    s16 i;

    if (item == ITEM_BEAN) {
        BEANS_BOUGHT = 0;
    }

    gSaveContext.inventory.items[invSlot] = ITEM_NONE;

    osSyncPrintf("\nItem_Register(%d)\n", invSlot, gSaveContext.inventory.items[invSlot]);

    for (i = 1; i < ARRAY_COUNT(gSaveContext.equips.buttonItems); i++) {
        if (gSaveContext.equips.buttonItems[i] == item) {
            gSaveContext.equips.buttonItems[i] = ITEM_NONE;
            gSaveContext.equips.cButtonSlots[i - 1] = SLOT_NONE;
        }
    }
}

s32 Inventory_ReplaceItem(PlayState* play, u16 oldItem, u16 newItem) {
    s16 i;

    for (i = 0; i < ARRAY_COUNT(gSaveContext.inventory.items); i++) {
        if (gSaveContext.inventory.items[i] == oldItem) {
            gSaveContext.inventory.items[i] = newItem;
            osSyncPrintf("アイテム消去(%d)\n", i); // "Item Purge (%d)"
            for (i = 1; i < ARRAY_COUNT(gSaveContext.equips.buttonItems); i++) {
                if (gSaveContext.equips.buttonItems[i] == oldItem) {
                    gSaveContext.equips.buttonItems[i] = newItem;
                    Interface_LoadItemIcon1(play, i);
                    break;
                }
            }
            return 1;
        }
    }

    return 0;
}

s32 Inventory_HasEmptyBottle(void) {
    u8* items = gSaveContext.inventory.items;

    if (items[SLOT_BOTTLE_1] == ITEM_BOTTLE) {
        return 1;
    } else if (items[SLOT_BOTTLE_2] == ITEM_BOTTLE) {
        return 1;
    } else if (items[SLOT_BOTTLE_3] == ITEM_BOTTLE) {
        return 1;
    } else if (items[SLOT_BOTTLE_4] == ITEM_BOTTLE) {
        return 1;
    } else {
        return 0;
    }
}

bool Inventory_HasEmptyBottleSlot(void) {
    u8* items = gSaveContext.inventory.items;

    return (
        items[SLOT_BOTTLE_1] == ITEM_NONE ||
        items[SLOT_BOTTLE_2] == ITEM_NONE ||
        items[SLOT_BOTTLE_3] == ITEM_NONE ||
        items[SLOT_BOTTLE_4] == ITEM_NONE
    );
}

s32 Inventory_HasSpecificBottle(u8 bottleItem) {
    u8* items = gSaveContext.inventory.items;

    if (items[SLOT_BOTTLE_1] == bottleItem) {
        return 1;
    } else if (items[SLOT_BOTTLE_2] == bottleItem) {
        return 1;
    } else if (items[SLOT_BOTTLE_3] == bottleItem) {
        return 1;
    } else if (items[SLOT_BOTTLE_4] == bottleItem) {
        return 1;
    } else {
        return 0;
    }
}

void Inventory_UpdateBottleItem(PlayState* play, u8 item, u8 button) {
    osSyncPrintf("item_no=%x,  c_no=%x,  Pt=%x  Item_Register=%x\n", item, button,
                 gSaveContext.equips.cButtonSlots[button - 1],
                 gSaveContext.inventory.items[gSaveContext.equips.cButtonSlots[button - 1]]);

    // Special case to only empty half of a Lon Lon Milk Bottle
    if ((gSaveContext.inventory.items[gSaveContext.equips.cButtonSlots[button - 1]] == ITEM_MILK_BOTTLE) &&
        (item == ITEM_BOTTLE)) {
        item = ITEM_MILK_HALF;
    }

    gSaveContext.inventory.items[gSaveContext.equips.cButtonSlots[button - 1]] = item;
    gSaveContext.equips.buttonItems[button] = item;

    Interface_LoadItemIcon1(play, button);

    play->pauseCtx.cursorItem[PAUSE_ITEM] = item;
    gSaveContext.buttonStatus[BUTTON_STATUS_INDEX(button)] = BTN_ENABLED;
}

s32 Inventory_ConsumeFairy(PlayState* play) {
    s32 bottleSlot = SLOT(ITEM_FAIRY);
    s16 i;
    s16 j;

    for (i = 0; i < 4; i++) {
        if (gSaveContext.inventory.items[bottleSlot + i] == ITEM_FAIRY) {
            for (j = 1; j < ARRAY_COUNT(gSaveContext.equips.buttonItems); j++) {
                if (gSaveContext.equips.buttonItems[j] == ITEM_FAIRY) {
                    gSaveContext.equips.buttonItems[j] = ITEM_BOTTLE;
                    Interface_LoadItemIcon1(play, j);
                    i = 0;
                    bottleSlot = gSaveContext.equips.cButtonSlots[j - 1];
                    break;
                }
            }
            osSyncPrintf("妖精使用＝%d\n", bottleSlot); // "Fairy Usage＝%d"
            gSaveContext.inventory.items[bottleSlot + i] = ITEM_BOTTLE;
            return 1;
        }
    }

    return 0;
}

bool Inventory_HatchPocketCucco(PlayState* play) {
    if (!gSaveContext.n64ddFlag) {
        return Inventory_ReplaceItem(play, ITEM_POCKET_EGG, ITEM_POCKET_CUCCO);
    }

    if (!PLAYER_HAS_SHUFFLED_ADULT_TRADE_ITEM(ITEM_POCKET_EGG)) { 
         return 0;
    }

    gSaveContext.adultTradeItems &= ~ADULT_TRADE_FLAG(ITEM_POCKET_EGG);
    gSaveContext.adultTradeItems |= ADULT_TRADE_FLAG(ITEM_POCKET_CUCCO);
    Inventory_ReplaceItem(play, ITEM_POCKET_EGG, ITEM_POCKET_CUCCO);
    return 1;
}

void func_80086D5C(s32* buf, u16 size) {
    u16 i;

    //buf = ResourceMgr_LoadTexByName(buf);

    for (i = 0; i < size; i++) {
        buf[i] = 0;
    }
}

void Interface_LoadActionLabel(InterfaceContext* interfaceCtx, u16 action, s16 loadOffset) {
    static void* sDoActionTextures[] = { gAttackDoActionENGTex, gCheckDoActionENGTex };
    if (action >= DO_ACTION_MAX) {
        action = DO_ACTION_NONE;
    }

    char* doAction = actionsTbl[action];

    char newName[512];
    if (gSaveContext.language != LANGUAGE_ENG) {
        size_t length = strlen(doAction);
        strcpy(newName, doAction);
        if (gSaveContext.language == LANGUAGE_FRA) {
            newName[length - 6] = 'F';
            newName[length - 5] = 'R';
            newName[length - 4] = 'A';
        } else if (gSaveContext.language == LANGUAGE_GER) {
            newName[length - 6] = 'G';
            newName[length - 5] = 'E';
            newName[length - 4] = 'R';
        }
        doAction = newName;
    }

    /*
    if (gSaveContext.language != LANGUAGE_ENG) {
        action += DO_ACTION_MAX;
    }

    if (gSaveContext.language == LANGUAGE_FRA) {
        action += DO_ACTION_MAX;
    }*/


    if (action != DO_ACTION_NONE) {
        //osCreateMesgQueue(&interfaceCtx->loadQueue, &interfaceCtx->loadMsg, OS_MESG_BLOCK);
        memcpy(interfaceCtx->doActionSegment + (loadOffset * DO_ACTION_TEX_SIZE()), ResourceMgr_LoadTexByName(doAction),
               DO_ACTION_TEX_SIZE());
        //DmaMgr_SendRequest2(&interfaceCtx->dmaRequest_160,
                            //interfaceCtx->doActionSegment + (loadOffset * DO_ACTION_TEX_SIZE),
                            //(uintptr_t)_do_action_staticSegmentRomStart + (action * DO_ACTION_TEX_SIZE), DO_ACTION_TEX_SIZE,
                            //0, &interfaceCtx->loadQueue, NULL, __FILE__, __LINE__);
        //osRecvMesg(&interfaceCtx->loadQueue, NULL, OS_MESG_BLOCK);
    } else {
        gSegments[7] = VIRTUAL_TO_PHYSICAL(interfaceCtx->doActionSegment);
        //func_80086D5C(SEGMENTED_TO_VIRTUAL(sDoActionTextures[loadOffset]), DO_ACTION_TEX_SIZE / 4);
        func_80086D5C(interfaceCtx->doActionSegment + (loadOffset * DO_ACTION_TEX_SIZE()), DO_ACTION_TEX_SIZE() / 4);
    }
}

void Interface_SetDoAction(PlayState* play, u16 action) {
    InterfaceContext* interfaceCtx = &play->interfaceCtx;
    PauseContext* pauseCtx = &play->pauseCtx;

    if (interfaceCtx->unk_1F0 != action) {
        interfaceCtx->unk_1F0 = action;
        interfaceCtx->unk_1EC = 1;
        interfaceCtx->unk_1F4 = 0.0f;
        Interface_LoadActionLabel(interfaceCtx, action, 1);
        if (pauseCtx->state != 0) {
            interfaceCtx->unk_1EC = 3;
        }
    }
}

void Interface_SetNaviCall(PlayState* play, u16 naviCallState) {
    InterfaceContext* interfaceCtx = &play->interfaceCtx;

    if (((naviCallState == 0x1D) || (naviCallState == 0x1E)) && !interfaceCtx->naviCalling &&
        (play->csCtx.state == CS_STATE_IDLE)) {
        if (!CVar_GetS32("gDisableNaviCallAudio", 0)) {
            // clang-format off
            if (naviCallState == 0x1E) { Audio_PlaySoundGeneral(NA_SE_VO_NAVY_CALL, &D_801333D4, 4,
                                                                &D_801333E0, &D_801333E0, &D_801333E8); }
            // clang-format on

            if (naviCallState == 0x1D) {
                func_800F4524(&D_801333D4, NA_SE_VO_NA_HELLO_2, 32);
            }
        }

        interfaceCtx->naviCalling = 1;
        sCUpInvisible = 0;
        sCUpTimer = 10;
    } else if ((naviCallState == 0x1F) && interfaceCtx->naviCalling) {
        interfaceCtx->naviCalling = 0;
    }
}

void Interface_LoadActionLabelB(PlayState* play, u16 action) {
    InterfaceContext* interfaceCtx = &play->interfaceCtx;

    char* doAction = actionsTbl[action];
    char newName[512];

    if (gSaveContext.language != LANGUAGE_ENG) {
        size_t length = strlen(doAction);
        strcpy(newName, doAction);
        if (gSaveContext.language == LANGUAGE_FRA) {
            newName[length - 6] = 'F';
            newName[length - 5] = 'R';
            newName[length - 4] = 'A';
        } else if (gSaveContext.language == LANGUAGE_GER) {
            newName[length - 6] = 'G';
            newName[length - 5] = 'E';
            newName[length - 4] = 'R';
        }
        doAction = newName;
    }

    /*if (gSaveContext.language != LANGUAGE_ENG) {
        action += DO_ACTION_MAX;
    }

    if (gSaveContext.language == LANGUAGE_FRA) {
        action += DO_ACTION_MAX;
    }*/

    interfaceCtx->unk_1FC = action;



    // OTRTODO
    osCreateMesgQueue(&interfaceCtx->loadQueue, &interfaceCtx->loadMsg, OS_MESG_BLOCK);
    memcpy(interfaceCtx->doActionSegment + DO_ACTION_TEX_SIZE(), ResourceMgr_LoadTexByName(doAction), DO_ACTION_TEX_SIZE());
    //DmaMgr_SendRequest2(&interfaceCtx->dmaRequest_160, interfaceCtx->doActionSegment + DO_ACTION_TEX_SIZE,
                        //(uintptr_t)_do_action_staticSegmentRomStart + (action * DO_ACTION_TEX_SIZE), DO_ACTION_TEX_SIZE, 0,
                        //&interfaceCtx->loadQueue, NULL, __FILE__, __LINE__);
    osRecvMesg(&interfaceCtx->loadQueue, NULL, OS_MESG_BLOCK);

    interfaceCtx->unk_1FA = 1;
}

s32 Health_ChangeBy(PlayState* play, s16 healthChange) {
    u16 heartCount;
    u16 healthLevel;

    // "＊＊＊＊＊ Fluctuation=%d (now=%d, max=%d) ＊＊＊"
    osSyncPrintf("＊＊＊＊＊  増減=%d (now=%d, max=%d)  ＊＊＊", healthChange, gSaveContext.health,
                 gSaveContext.healthCapacity);

    if (healthChange < 0) {
        gSaveContext.sohStats.count[COUNT_DAMAGE_TAKEN] += -healthChange;
    }

    // If one-hit ko mode is on, any damage kills you and you cannot gain health.
    if (chaosEffectOneHitKO) {
        if (healthChange < 0) {
            gSaveContext.health = 0;
        }
        
        return 0;
    }

    // clang-format off
    if (healthChange > 0) { Audio_PlaySoundGeneral(NA_SE_SY_HP_RECOVER, &D_801333D4, 4,
                                                   &D_801333E0, &D_801333E0, &D_801333E8);
    } else if ((gSaveContext.doubleDefense != 0) && (healthChange < 0)) {
        healthChange >>= 1;
        osSyncPrintf("ハート減少半分！！＝%d\n", healthChange); // "Heart decrease halved!!＝%d"
    }
    // clang-format on

    if (chaosEffectDefenseModifier != 0 && healthChange < 0) {
        if (chaosEffectDefenseModifier > 0) {
            healthChange /= chaosEffectDefenseModifier;
        } else {
            healthChange *= abs(chaosEffectDefenseModifier);
        }
    }

    gSaveContext.health += healthChange;

    if (gSaveContext.health > gSaveContext.healthCapacity) {
        gSaveContext.health = gSaveContext.healthCapacity;
    }

    heartCount = gSaveContext.health % 0x10;

    healthLevel = heartCount;
    if (heartCount != 0) {
        if (heartCount > 10) {
            healthLevel = 3;
        } else if (heartCount > 5) {
            healthLevel = 2;
        } else {
            healthLevel = 1;
        }
    }

    // "Life=%d ＊＊＊  %d ＊＊＊＊＊＊"
    osSyncPrintf("  ライフ=%d  ＊＊＊  %d  ＊＊＊＊＊＊\n", gSaveContext.health, healthLevel);

    if (gSaveContext.health <= 0) {
        gSaveContext.health = 0;
        return 0;
    } else {
        return 1;
    }
}

void Health_GiveHearts(s16 hearts) {
    gSaveContext.healthCapacity += hearts * 0x10;
}

void Health_RemoveHearts(s16 hearts) {
    gSaveContext.healthCapacity -= hearts * 0x10;
}

void Rupees_ChangeBy(s16 rupeeChange) {
    gSaveContext.rupeeAccumulator += rupeeChange;

    if (rupeeChange > 0) {
        gSaveContext.sohStats.count[COUNT_RUPEES_COLLECTED] += rupeeChange;
    }
    if (rupeeChange < 0) {
        gSaveContext.sohStats.count[COUNT_RUPEES_SPENT] += -rupeeChange;
    }
}

void GameplayStats_UpdateAmmoUsed(s16 item, s16 ammoUsed) {

    switch (item) { 
        case ITEM_STICK:
            gSaveContext.sohStats.count[COUNT_AMMO_USED_STICK] += ammoUsed;
            break;
        case ITEM_NUT:
            gSaveContext.sohStats.count[COUNT_AMMO_USED_NUT] += ammoUsed;
            break;
        case ITEM_BOMB:
            gSaveContext.sohStats.count[COUNT_AMMO_USED_BOMB] += ammoUsed;
            break;
        case ITEM_BOW:
            gSaveContext.sohStats.count[COUNT_AMMO_USED_ARROW] += ammoUsed;
            break;
        case ITEM_SLINGSHOT:
            gSaveContext.sohStats.count[COUNT_AMMO_USED_SEED] += ammoUsed;
            break;
        case ITEM_BOMBCHU:
            gSaveContext.sohStats.count[COUNT_AMMO_USED_BOMBCHU] += ammoUsed;
            break;
        case ITEM_BEAN:
            gSaveContext.sohStats.count[COUNT_AMMO_USED_BEAN] += ammoUsed;
            break;
        default:
            break;
    }
    return;
}

void Inventory_ChangeAmmo(s16 item, s16 ammoChange) {
    // "Item = (%d)    Amount = (%d + %d)"
    osSyncPrintf("アイテム = (%d)    数 = (%d + %d)  ", item, AMMO(item), ammoChange);

    if (item == ITEM_STICK) {
        AMMO(ITEM_STICK) += ammoChange;

        if (AMMO(ITEM_STICK) >= CUR_CAPACITY(UPG_STICKS)) {
            AMMO(ITEM_STICK) = CUR_CAPACITY(UPG_STICKS);
        } else if (AMMO(ITEM_STICK) < 0) {
            AMMO(ITEM_STICK) = 0;
        }
    } else if (item == ITEM_NUT) {
        AMMO(ITEM_NUT) += ammoChange;

        if (AMMO(ITEM_NUT) >= CUR_CAPACITY(UPG_NUTS)) {
            AMMO(ITEM_NUT) = CUR_CAPACITY(UPG_NUTS);
        } else if (AMMO(ITEM_NUT) < 0) {
            AMMO(ITEM_NUT) = 0;
        }
    } else if (item == ITEM_BOMBCHU) {
        AMMO(ITEM_BOMBCHU) += ammoChange;

        if (AMMO(ITEM_BOMBCHU) >= 50) {
            AMMO(ITEM_BOMBCHU) = 50;
        } else if (AMMO(ITEM_BOMBCHU) < 0) {
            AMMO(ITEM_BOMBCHU) = 0;
        }
    } else if (item == ITEM_BOW) {
        AMMO(ITEM_BOW) += ammoChange;

        if (AMMO(ITEM_BOW) >= CUR_CAPACITY(UPG_QUIVER)) {
            AMMO(ITEM_BOW) = CUR_CAPACITY(UPG_QUIVER);
        } else if (AMMO(ITEM_BOW) < 0) {
            AMMO(ITEM_BOW) = 0;
        }
    } else if ((item == ITEM_SLINGSHOT) || (item == ITEM_SEEDS)) {
        AMMO(ITEM_SLINGSHOT) += ammoChange;

        if (AMMO(ITEM_SLINGSHOT) >= CUR_CAPACITY(UPG_BULLET_BAG)) {
            AMMO(ITEM_SLINGSHOT) = CUR_CAPACITY(UPG_BULLET_BAG);
        } else if (AMMO(ITEM_SLINGSHOT) < 0) {
            AMMO(ITEM_SLINGSHOT) = 0;
        }
    } else if (item == ITEM_BOMB) {
        AMMO(ITEM_BOMB) += ammoChange;

        if (AMMO(ITEM_BOMB) >= CUR_CAPACITY(UPG_BOMB_BAG)) {
            AMMO(ITEM_BOMB) = CUR_CAPACITY(UPG_BOMB_BAG);
        } else if (AMMO(ITEM_BOMB) < 0) {
            AMMO(ITEM_BOMB) = 0;
        }
    } else if (item == ITEM_BEAN) {
        AMMO(ITEM_BEAN) += ammoChange;
    }

    osSyncPrintf("合計 = (%d)\n", AMMO(item)); // "Total = (%d)"

    if (ammoChange < 0) {
        GameplayStats_UpdateAmmoUsed(item, -ammoChange);
    }
}

void Magic_Fill(PlayState* play) {
    if (gSaveContext.magicAcquired) {
        gSaveContext.unk_13F2 = gSaveContext.unk_13F0;
        gSaveContext.unk_13F6 = (gSaveContext.doubleMagic + 1) * 0x30;
        gSaveContext.unk_13F0 = 9;
    }
}

void func_800876C8(PlayState* play) {
    if ((gSaveContext.unk_13F0 != 8) && (gSaveContext.unk_13F0 != 9)) {
        if (gSaveContext.unk_13F0 == 10) {
            gSaveContext.unk_13F2 = gSaveContext.unk_13F0;
        }
        gSaveContext.unk_13F0 = 5;
    }
}

s32 func_80087708(PlayState* play, s16 arg1, s16 arg2) {
    if (!gSaveContext.magicAcquired) {
        return 0;
    }

    if ((arg2 != 5) && (gSaveContext.magic - arg1) < 0) {
        if (gSaveContext.unk_13F4 != 0) {
            Audio_PlaySoundGeneral(NA_SE_SY_ERROR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        }
        return 0;
    }

    switch (arg2) {
        case 0:
        case 2:
            if ((gSaveContext.unk_13F0 == 0) || (gSaveContext.unk_13F0 == 7)) {
                if (gSaveContext.unk_13F0 == 7) {
                    play->actorCtx.lensActive = false;
                }
                gSaveContext.unk_13F8 = gSaveContext.magic - arg1;
                gSaveContext.unk_13F0 = 1;
                return 1;
            } else {
                Audio_PlaySoundGeneral(NA_SE_SY_ERROR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                return 0;
            }
        case 1:
            if ((gSaveContext.unk_13F0 == 0) || (gSaveContext.unk_13F0 == 7)) {
                if (gSaveContext.unk_13F0 == 7) {
                    play->actorCtx.lensActive = false;
                }
                gSaveContext.unk_13F8 = gSaveContext.magic - arg1;
                gSaveContext.unk_13F0 = 6;
                return 1;
            } else {
                Audio_PlaySoundGeneral(NA_SE_SY_ERROR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                return 0;
            }
        case 3:
            if (gSaveContext.unk_13F0 == 0) {
                if (gSaveContext.magic != 0) {
                    play->interfaceCtx.unk_230 = 80;
                    gSaveContext.unk_13F0 = 7;
                    return 1;
                } else {
                    return 0;
                }
            } else {
                if (gSaveContext.unk_13F0 == 7) {
                    return 1;
                } else {
                    return 0;
                }
            }
        case 4:
            if ((gSaveContext.unk_13F0 == 0) || (gSaveContext.unk_13F0 == 7)) {
                if (gSaveContext.unk_13F0 == 7) {
                    play->actorCtx.lensActive = false;
                }
                gSaveContext.unk_13F8 = gSaveContext.magic - arg1;
                gSaveContext.unk_13F0 = 4;
                return 1;
            } else {
                Audio_PlaySoundGeneral(NA_SE_SY_ERROR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                return 0;
            }
        case 5:
            if (gSaveContext.unk_13F4 >= gSaveContext.magic) {
                gSaveContext.unk_13F8 = gSaveContext.magic + arg1;

                if (gSaveContext.unk_13F8 >= gSaveContext.unk_13F4) {
                    gSaveContext.unk_13F8 = gSaveContext.unk_13F4;
                }

                gSaveContext.unk_13F0 = 10;
                return 1;
            }
            break;
    }

    return 0;
}

void Interface_UpdateMagicBar(PlayState* play) {
    static s16 sMagicBorderColors[][3] = {
        { 255, 255, 255 },
        { 150, 150, 150 },
        { 255, 255, 150 },
        { 255, 255, 50 },
    };
    Color_RGB8 MagicBorder_0 = { 255, 255, 255 };
    Color_RGB8 MagicBorder_1 = { 150, 150, 150 };
    Color_RGB8 MagicBorder_2 = { 255, 255, 150 };
    Color_RGB8 MagicBorder_3 = { 255, 255, 50 };

    if (CVar_GetS32("gHudColors", 1) == 2) { //This will make custom color based on users selected colors.
        sMagicBorderColors[0][0] = CVar_GetRGB("gCCMagicBorderPrim", MagicBorder_0).r;
        sMagicBorderColors[0][1] = CVar_GetRGB("gCCMagicBorderPrim", MagicBorder_0).g;
        sMagicBorderColors[0][2] = CVar_GetRGB("gCCMagicBorderPrim", MagicBorder_0).b;

        sMagicBorderColors[1][0] = CVar_GetRGB("gCCMagicBorderPrim", MagicBorder_1).r/2;
        sMagicBorderColors[1][1] = CVar_GetRGB("gCCMagicBorderPrim", MagicBorder_1).g/2;
        sMagicBorderColors[1][2] = CVar_GetRGB("gCCMagicBorderPrim", MagicBorder_1).b/2;

        sMagicBorderColors[2][0] = CVar_GetRGB("gCCMagicBorderPrim", MagicBorder_2).r/2.5;
        sMagicBorderColors[2][1] = CVar_GetRGB("gCCMagicBorderPrim", MagicBorder_2).g/2.5;
        sMagicBorderColors[2][2] = CVar_GetRGB("gCCMagicBorderPrim", MagicBorder_2).b/2.5;

        sMagicBorderColors[3][0] = CVar_GetRGB("gCCMagicBorderPrim", MagicBorder_3).r/3;
        sMagicBorderColors[3][1] = CVar_GetRGB("gCCMagicBorderPrim", MagicBorder_3).g/3;
        sMagicBorderColors[3][2] = CVar_GetRGB("gCCMagicBorderPrim", MagicBorder_3).b/3;
    }

    static s16 sMagicBorderIndexes[] = { 0, 1, 1, 0 };
    static s16 sMagicBorderRatio = 2;
    static s16 sMagicBorderStep = 1;
    MessageContext* msgCtx = &play->msgCtx;
    InterfaceContext* interfaceCtx = &play->interfaceCtx;
    s16 borderChangeR;
    s16 borderChangeG;
    s16 borderChangeB;
    s16 temp;

    switch (gSaveContext.unk_13F0) {
        case 8:
            temp = gSaveContext.magicLevel * 0x30;
            if (gSaveContext.unk_13F4 != temp) {
                if (gSaveContext.unk_13F4 < temp) {
                    gSaveContext.unk_13F4 += 8;
                    if (gSaveContext.unk_13F4 > temp) {
                        gSaveContext.unk_13F4 = temp;
                    }
                } else {
                    gSaveContext.unk_13F4 -= 8;
                    if (gSaveContext.unk_13F4 <= temp) {
                        gSaveContext.unk_13F4 = temp;
                    }
                }
            } else {
                gSaveContext.unk_13F0 = 9;
            }
            break;

        case 9:
            gSaveContext.magic += 4;

            if (gSaveContext.gameMode == 0 && gSaveContext.sceneSetupIndex < 4) {
                Audio_PlaySoundGeneral(NA_SE_SY_GAUGE_UP - SFX_FLAG, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                       &D_801333E8);
            }

            // "Storage  MAGIC_NOW=%d (%d)"
            osSyncPrintf("蓄電  MAGIC_NOW=%d (%d)\n", gSaveContext.magic, gSaveContext.unk_13F6);
            if (gSaveContext.magic >= gSaveContext.unk_13F6) {
                gSaveContext.magic = gSaveContext.unk_13F6;
                gSaveContext.unk_13F0 = gSaveContext.unk_13F2;
                gSaveContext.unk_13F2 = 0;
            }
            break;

        case 1:
            sMagicBorderRatio = 2;
            gSaveContext.unk_13F0 = 2;
            break;

        case 2:
            gSaveContext.magic -= 2;
            if (gSaveContext.magic <= 0) {
                gSaveContext.magic = 0;
                gSaveContext.unk_13F0 = 3;
                if (CVar_GetS32("gHudColors", 1) == 2) {
                    sMagicBorder = CVar_GetRGB("gCCMagicBorderNormPrim", sMagicBorder_ori);
                } else {
                    sMagicBorder = sMagicBorder_ori;
                }
            } else if (gSaveContext.magic == gSaveContext.unk_13F8) {
                gSaveContext.unk_13F0 = 3;
                if (CVar_GetS32("gHudColors", 1) == 2) {
                    sMagicBorder = CVar_GetRGB("gCCMagicBorderNormPrim", sMagicBorder_ori);
                } else {
                    sMagicBorder = sMagicBorder_ori;
                }
            }
        case 3:
        case 4:
        case 6:
            temp = sMagicBorderIndexes[sMagicBorderStep];
            borderChangeR = ABS(sMagicBorder.r - sMagicBorderColors[temp][0]) / sMagicBorderRatio;
            borderChangeG = ABS(sMagicBorder.g - sMagicBorderColors[temp][1]) / sMagicBorderRatio;
            borderChangeB = ABS(sMagicBorder.b - sMagicBorderColors[temp][2]) / sMagicBorderRatio;

            if (sMagicBorder.r >= sMagicBorderColors[temp][0]) {
                sMagicBorder.r -= borderChangeR;
            } else {
                sMagicBorder.r += borderChangeR;
            }

            if (sMagicBorder.g >= sMagicBorderColors[temp][1]) {
                sMagicBorder.g -= borderChangeG;
            } else {
                sMagicBorder.g += borderChangeG;
            }

            if (sMagicBorder.b >= sMagicBorderColors[temp][2]) {
                sMagicBorder.b -= borderChangeB;
            } else {
                sMagicBorder.b += borderChangeB;
            }

            sMagicBorderRatio--;
            if (sMagicBorderRatio == 0) {
                sMagicBorder.r = sMagicBorderColors[temp][0];
                sMagicBorder.g = sMagicBorderColors[temp][1];
                sMagicBorder.b = sMagicBorderColors[temp][2];
                sMagicBorderRatio = YREG(40 + sMagicBorderStep);
                sMagicBorderStep++;
                if (sMagicBorderStep >= 4) {
                    sMagicBorderStep = 0;
                }
            }
            break;

        case 5:
            if (CVar_GetS32("gHudColors", 1) == 2) {
                sMagicBorder = CVar_GetRGB("gCCMagicBorderNormPrim", sMagicBorder_ori);
            } else {
                sMagicBorder = sMagicBorder_ori;
            }
            gSaveContext.unk_13F0 = 0;
            break;

        case 7:
            if ((play->pauseCtx.state == 0) && (play->pauseCtx.debugState == 0) &&
                (msgCtx->msgMode == MSGMODE_NONE) && (play->gameOverCtx.state == GAMEOVER_INACTIVE) &&
                (play->sceneLoadFlag == 0) && (play->transitionMode == 0) && !Play_InCsMode(play)) {
                bool hasLens = false;
                for (int buttonIndex = 1; buttonIndex < (CVar_GetS32("gDpadEquips", 0) != 0) ? ARRAY_COUNT(gSaveContext.equips.buttonItems) : 4; buttonIndex++) {
                    if (gSaveContext.equips.buttonItems[buttonIndex] == ITEM_LENS) {
                        hasLens = true;
                        break;
                    }
                }
                if ((gSaveContext.magic == 0) || ((func_8008F2F8(play) >= 2) && (func_8008F2F8(play) < 5)) ||
                    !hasLens ||
                    !play->actorCtx.lensActive) {
                    play->actorCtx.lensActive = false;
                    Audio_PlaySoundGeneral(NA_SE_SY_GLASSMODE_OFF, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                           &D_801333E8);
                    gSaveContext.unk_13F0 = 0;
                    if (CVar_GetS32("gHudColors", 1) == 2) {
                        sMagicBorder = CVar_GetRGB("gCCMagicBorderNormPrim", sMagicBorder_ori);
                    } else {
                        sMagicBorder = sMagicBorder_ori;
                    }
                    break;
                }

                interfaceCtx->unk_230--;
                if (interfaceCtx->unk_230 == 0) {
                    gSaveContext.magic--;
                    interfaceCtx->unk_230 = 80;
                }
            }

            temp = sMagicBorderIndexes[sMagicBorderStep];
            borderChangeR = ABS(sMagicBorder.r - sMagicBorderColors[temp][0]) / sMagicBorderRatio;
            borderChangeG = ABS(sMagicBorder.g - sMagicBorderColors[temp][1]) / sMagicBorderRatio;
            borderChangeB = ABS(sMagicBorder.b - sMagicBorderColors[temp][2]) / sMagicBorderRatio;

            if (sMagicBorder.r >= sMagicBorderColors[temp][0]) {
                sMagicBorder.r -= borderChangeR;
            } else {
                sMagicBorder.r += borderChangeR;
            }

            if (sMagicBorder.g >= sMagicBorderColors[temp][1]) {
                sMagicBorder.g -= borderChangeG;
            } else {
                sMagicBorder.g += borderChangeG;
            }

            if (sMagicBorder.b >= sMagicBorderColors[temp][2]) {
                sMagicBorder.b -= borderChangeB;
            } else {
                sMagicBorder.b += borderChangeB;
            }

            sMagicBorderRatio--;
            if (sMagicBorderRatio == 0) {
                sMagicBorder.r = sMagicBorderColors[temp][0];
                sMagicBorder.g = sMagicBorderColors[temp][1];
                sMagicBorder.b = sMagicBorderColors[temp][2];
                sMagicBorderRatio = YREG(40 + sMagicBorderStep);
                sMagicBorderStep++;
                if (sMagicBorderStep >= 4) {
                    sMagicBorderStep = 0;
                }
            }
            break;

        case 10:
            gSaveContext.magic += 4;
            Audio_PlaySoundGeneral(NA_SE_SY_GAUGE_UP - SFX_FLAG, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            if (gSaveContext.magic >= gSaveContext.unk_13F8) {
                gSaveContext.magic = gSaveContext.unk_13F8;
                gSaveContext.unk_13F0 = gSaveContext.unk_13F2;
                gSaveContext.unk_13F2 = 0;
            }
            break;

        default:
            gSaveContext.unk_13F0 = 0;
            break;
    }
}

void Interface_DrawLineupTick(PlayState* play) {
    InterfaceContext* interfaceCtx = &play->interfaceCtx;

    OPEN_DISPS(play->state.gfxCtx);

    func_80094520(play->state.gfxCtx);

    gDPSetEnvColor(OVERLAY_DISP++, 255, 255, 255, 255);
    gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, 255);

    s16 width = 32;
    s16 height = 32;
    s16 x = -8 + (SCREEN_WIDTH / 2);
    s16 y = CVar_GetS32("gOpenMenuBar", 0) ? -4 : -6;

    OVERLAY_DISP = Gfx_TextureIA8(OVERLAY_DISP, gEmptyCDownArrowTex, width, height, x, y, width, height, 2 << 10, 2 << 10);

    gDPPipeSync(OVERLAY_DISP++);

    CLOSE_DISPS(play->state.gfxCtx);
}

void Interface_DrawMagicBar(PlayState* play) {
    InterfaceContext* interfaceCtx = &play->interfaceCtx;
    s16 magicDrop = R_MAGIC_BAR_LARGE_Y-R_MAGIC_BAR_SMALL_Y+2;
    s16 magicBarY;
    Color_RGB8 magicbar_yellow = {250,250,0}; //Magic bar being used
    Color_RGB8 magicbar_green = {R_MAGIC_FILL_COLOR(0),R_MAGIC_FILL_COLOR(1),R_MAGIC_FILL_COLOR(2)}; //Magic bar fill

    OPEN_DISPS(play->state.gfxCtx);

    if (gSaveContext.magicLevel != 0) {
        s16 X_Margins;
        s16 Y_Margins;
        if (CVar_GetS32("gMagicBarUseMargins", 0) != 0) {
            X_Margins = Left_HUD_Margin;
            Y_Margins = (Top_HUD_Margin*-1);
        } else {
            X_Margins = 0;
            Y_Margins = 0;
        }
        const s16 magicBarY_original_l = R_MAGIC_BAR_LARGE_Y + Y_Margins;
        const s16 magicBarY_original_s = R_MAGIC_BAR_SMALL_Y + Y_Margins;
        const s16 PosX_Start_original = OTRGetRectDimensionFromLeftEdge(R_MAGIC_BAR_X + X_Margins);
        const s16 PosX_MidEnd_original = OTRGetRectDimensionFromLeftEdge(R_MAGIC_BAR_X + X_Margins+8);
        const s16 rMagicBarX_original = OTRGetRectDimensionFromLeftEdge(R_MAGIC_BAR_X + X_Margins);
        const s16 rMagicFillX_original = OTRGetRectDimensionFromLeftEdge(R_MAGIC_FILL_X + X_Margins);
        s16 PosX_Start;
        s16 magicBarY;
        s16 rMagicBarX;
        s16 PosX_MidEnd;
        s16 rMagicFillX;
        s32 lineLength = CVar_GetS32("gHeartsLineLength", 10);
        if (CVar_GetS32("gMagicBarPosType", 0) != 0) {
            magicBarY = CVar_GetS32("gMagicBarPosY", 0)+Y_Margins;
            if (CVar_GetS32("gMagicBarPosType", 0) == 1) {//Anchor Left
                if (CVar_GetS32("gMagicBarUseMargins", 0) != 0) {X_Margins = Left_HUD_Margin;};
                PosX_Start = OTRGetDimensionFromLeftEdge(CVar_GetS32("gMagicBarPosX", 0)+X_Margins);
                rMagicBarX = OTRGetDimensionFromLeftEdge(CVar_GetS32("gMagicBarPosX", 0)+X_Margins);
                PosX_MidEnd = OTRGetDimensionFromLeftEdge(CVar_GetS32("gMagicBarPosX", 0)+X_Margins+8);
                rMagicFillX = OTRGetDimensionFromLeftEdge(CVar_GetS32("gMagicBarPosX", 0)+X_Margins+8);
            } else if (CVar_GetS32("gMagicBarPosType", 0) == 2) {//Anchor Right
                if (CVar_GetS32("gMagicBarUseMargins", 0) != 0) {X_Margins = Right_HUD_Margin;};
                PosX_Start = OTRGetDimensionFromRightEdge(CVar_GetS32("gMagicBarPosX", 0)+X_Margins);
                rMagicBarX = OTRGetDimensionFromRightEdge(CVar_GetS32("gMagicBarPosX", 0)+X_Margins);
                PosX_MidEnd = OTRGetDimensionFromRightEdge(CVar_GetS32("gMagicBarPosX", 0)+X_Margins+8);
                rMagicFillX = OTRGetDimensionFromRightEdge(CVar_GetS32("gMagicBarPosX", 0)+X_Margins+8);
            } else if (CVar_GetS32("gMagicBarPosType", 0) == 3) {//Anchor None
                PosX_Start = CVar_GetS32("gMagicBarPosX", 0)+X_Margins;
                rMagicBarX = CVar_GetS32("gMagicBarPosX", 0)+X_Margins;
                PosX_MidEnd = CVar_GetS32("gMagicBarPosX", 0)+X_Margins+8;
                rMagicFillX = CVar_GetS32("gMagicBarPosX", 0)+X_Margins+8;
            } else if (CVar_GetS32("gMagicBarPosType", 0) == 4) {//hidden
                PosX_Start = -9999;
                rMagicBarX = -9999;
                PosX_MidEnd = -9999;
                rMagicFillX = -9999;
            } else if (CVar_GetS32("gMagicBarPosType", 0) == 5) {//Anchor To life meter
                magicBarY = R_MAGIC_BAR_SMALL_Y-2 +
                            magicDrop*(lineLength == 0 ? 0 : (gSaveContext.healthCapacity-1)/(0x10*lineLength)) +
                            CVar_GetS32("gMagicBarPosY", 0) + getHealthMeterYOffset();
                s16 xPushover = CVar_GetS32("gMagicBarPosX", 0) + getHealthMeterXOffset() + R_MAGIC_BAR_X-1;
                PosX_Start = xPushover;
                rMagicBarX = xPushover;
                PosX_MidEnd = xPushover+8;
                rMagicFillX = CVar_GetS32("gMagicBarPosX", 0) + getHealthMeterXOffset() + R_MAGIC_FILL_X-1;
            }
        } else {
            if ((gSaveContext.healthCapacity-1)/0x10 >= lineLength && lineLength != 0) {
                magicBarY = magicBarY_original_l +
                            magicDrop*(lineLength == 0 ? 0 : ((gSaveContext.healthCapacity-1)/(0x10*lineLength) - 1));
            } else {
                magicBarY = magicBarY_original_s;
            }
            PosX_Start = PosX_Start_original;
            rMagicBarX = rMagicBarX_original;
            PosX_MidEnd = PosX_MidEnd_original;
            rMagicFillX = rMagicFillX_original;
        }

        func_80094520(play->state.gfxCtx);

        if (CVar_GetS32("gHudColors", 1) == 2) {//Original game add color there so to prevent miss match we make it all white :)
            gDPSetEnvColor(OVERLAY_DISP++, 255, 255, 255, 255);
        } else {
            gDPSetEnvColor(OVERLAY_DISP++, 100, 50, 50, 255);
        }
        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, sMagicBorder.r, sMagicBorder.g, sMagicBorder.b, interfaceCtx->magicAlpha);

        OVERLAY_DISP = Gfx_TextureIA8(OVERLAY_DISP, gMagicMeterEndTex, 8, 16, PosX_Start, magicBarY, 8, 16, 1 << 10, 1 << 10);

        OVERLAY_DISP = Gfx_TextureIA8(OVERLAY_DISP, gMagicMeterMidTex, 24, 16, PosX_MidEnd, magicBarY, gSaveContext.unk_13F4, 16, 1 << 10, 1 << 10);

        gDPLoadTextureBlock(OVERLAY_DISP++, gMagicMeterEndTex, G_IM_FMT_IA, G_IM_SIZ_8b, 8, 16, 0,
                            G_TX_MIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 3, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

        gSPWideTextureRectangle(OVERLAY_DISP++, ((rMagicBarX + gSaveContext.unk_13F4) + 8) << 2, magicBarY << 2,
                            ((rMagicBarX + gSaveContext.unk_13F4) + 16) << 2, (magicBarY + 16) << 2, G_TX_RENDERTILE,
                            256, 0, 1 << 10, 1 << 10);

        gDPPipeSync(OVERLAY_DISP++);
        gDPSetCombineLERP(OVERLAY_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, 0, 0, 0, PRIMITIVE, PRIMITIVE,
                          ENVIRONMENT, TEXEL0, ENVIRONMENT, 0, 0, 0, PRIMITIVE);
        gDPSetEnvColor(OVERLAY_DISP++, 0, 0, 0, 255);

        if (gSaveContext.unk_13F0 == 4) {
            // Yellow part of the bar indicating the amount of magic to be subtracted
            if (CVar_GetS32("gHudColors", 1) == 2) {
                gDPSetPrimColor(OVERLAY_DISP++, 0, 0, CVar_GetRGB("gCCMagicUsePrim", magicbar_yellow).r, CVar_GetRGB("gCCMagicUsePrim", magicbar_yellow).g, CVar_GetRGB("gCCMagicUsePrim", magicbar_yellow).b, interfaceCtx->magicAlpha);
            } else {
                gDPSetPrimColor(OVERLAY_DISP++, 0, 0, magicbar_yellow.r, magicbar_yellow.g, magicbar_yellow.b, interfaceCtx->magicAlpha);
            }

            gDPLoadMultiBlock_4b(OVERLAY_DISP++, gMagicMeterFillTex, 0, G_TX_RENDERTILE, G_IM_FMT_I, 16, 16, 0,
                                 G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                                 G_TX_NOLOD, G_TX_NOLOD);

            gSPWideTextureRectangle(OVERLAY_DISP++, rMagicFillX << 2, (magicBarY + 3) << 2,
                                (rMagicFillX + gSaveContext.magic) << 2, (magicBarY + 10) << 2, G_TX_RENDERTILE, 0,
                                0, 1 << 10, 1 << 10);

            // Fill the rest of the bar with the normal magic color
            gDPPipeSync(OVERLAY_DISP++);
            if (CVar_GetS32("gHudColors", 1) == 2) {
                gDPSetPrimColor(OVERLAY_DISP++, 0, 0, CVar_GetRGB("gCCMagicPrim", magicbar_green).r, CVar_GetRGB("gCCMagicPrim", magicbar_green).g, CVar_GetRGB("gCCMagicPrim", magicbar_green).b, interfaceCtx->magicAlpha);
            } else {
                gDPSetPrimColor(OVERLAY_DISP++, 0, 0, magicbar_green.r, magicbar_green.g, magicbar_green.b, interfaceCtx->magicAlpha);
            }

            gSPWideTextureRectangle(OVERLAY_DISP++, rMagicFillX << 2, (magicBarY + 3) << 2,
                                (rMagicFillX + gSaveContext.unk_13F8) << 2, (magicBarY + 10) << 2, G_TX_RENDERTILE,
                                0, 0, 1 << 10, 1 << 10);
        } else {
            // Fill the whole bar with the normal magic color
            if (CVar_GetS32("gHudColors", 1) == 2) {
                gDPSetPrimColor(OVERLAY_DISP++, 0, 0, CVar_GetRGB("gCCMagicPrim", magicbar_green).r, CVar_GetRGB("gCCMagicPrim", magicbar_green).g, CVar_GetRGB("gCCMagicPrim", magicbar_green).b, interfaceCtx->magicAlpha);
            } else {
                gDPSetPrimColor(OVERLAY_DISP++, 0, 0, magicbar_green.r, magicbar_green.g, magicbar_green.b, interfaceCtx->magicAlpha);
            }

            gDPLoadMultiBlock_4b(OVERLAY_DISP++, gMagicMeterFillTex, 0, G_TX_RENDERTILE, G_IM_FMT_I, 16, 16, 0,
                                 G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                                 G_TX_NOLOD, G_TX_NOLOD);

            gSPWideTextureRectangle(OVERLAY_DISP++, rMagicFillX << 2, (magicBarY + 3) << 2,
                                (rMagicFillX + gSaveContext.magic) << 2, (magicBarY + 10) << 2, G_TX_RENDERTILE, 0,
                                0, 1 << 10, 1 << 10);
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void func_80088AA0(s16 arg0) {
    gSaveContext.timerX[1] = 140;
    gSaveContext.timerY[1] = 80;
    D_80125A5C = 0;
    gSaveContext.timer2Value = arg0;

    if (arg0 != 0) {
        gSaveContext.timer2State = 1;
    } else {
        gSaveContext.timer2State = 7;
    }
}

void func_80088AF0(PlayState* play) {
    if (gSaveContext.timer2State != 0) {
        if (gSaveContext.eventInf[1] & 1) {
            gSaveContext.timer2Value = 239;
        } else {
            gSaveContext.timer2Value = 1;
        }
    }
}

void func_80088B34(s16 arg0) {
    gSaveContext.timerX[0] = 140;
    gSaveContext.timerY[0] = 80;
    D_80125A5C = 0;
    gSaveContext.timer1Value = arg0;

    if (arg0 != 0) {
        gSaveContext.timer1State = 5;
    } else {
        gSaveContext.timer1State = 11;
    }
}

void Interface_DrawActionLabel(GraphicsContext* gfxCtx, void* texture) {
    OPEN_DISPS(gfxCtx);

    gDPLoadTextureBlock_4b(OVERLAY_DISP++, texture, G_IM_FMT_IA, DO_ACTION_TEX_WIDTH(), DO_ACTION_TEX_HEIGHT(), 0,
                           G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                           G_TX_NOLOD);

    gSP1Quadrangle(OVERLAY_DISP++, 0, 2, 3, 1, 0);

    CLOSE_DISPS(gfxCtx);
}

void Interface_DrawItemButtons(PlayState* play) {
    static void* cUpLabelTextures[] = { gNaviCUpENGTex, gNaviCUpENGTex, gNaviCUpENGTex };
    static s16 startButtonLeftPos[] = { 132, 130, 130 };
    InterfaceContext* interfaceCtx = &play->interfaceCtx;
    Player* player = GET_PLAYER(play);
    PauseContext* pauseCtx = &play->pauseCtx;
    s16 temp; // Used as both an alpha value and a button index
    s16 dxdy;
    s16 width;
    s16 height;
    Color_RGB8 A_button_ori = {R_A_BTN_COLOR(0), R_A_BTN_COLOR(1), R_A_BTN_COLOR(2)};
    Color_RGB8 A_button = CVar_GetRGB("gCCABtnPrim", A_button_ori);
    Color_RGB8 B_button_ori = {R_B_BTN_COLOR(0), R_B_BTN_COLOR(1), R_B_BTN_COLOR(2)};
    Color_RGB8 B_button = CVar_GetRGB("gCCBBtnPrim", B_button_ori);
    Color_RGB8 Start_button_ori = {120, 120, 120};
    Color_RGB8 Start_button = CVar_GetRGB("gCCStartBtnPrim", Start_button_ori);
    Color_RGB8 C_button_ori = {R_C_BTN_COLOR(0), R_C_BTN_COLOR(1), R_C_BTN_COLOR(2)};
    Color_RGB8 C_button_uni = CVar_GetRGB("gCCCBtnPrim", C_button_ori);
    Color_RGB8 C_button_U = CVar_GetRGB("gCCCUBtnPrim", C_button_ori);
    Color_RGB8 C_button_D = CVar_GetRGB("gCCCDBtnPrim", C_button_ori);
    Color_RGB8 C_button_L = CVar_GetRGB("gCCCLBtnPrim", C_button_ori);
    Color_RGB8 C_button_R = CVar_GetRGB("gCCCRBtnPrim", C_button_ori);

    //B Button
    s16 X_Margins_BtnB;
    s16 Y_Margins_BtnB;
    s16 BBtn_Size = 32;
    int BBtnScaled = BBtn_Size * 0.95f;
    if (CVar_GetS32("gBBtnPosType", 0) != 0) {
        BBtnScaled = BBtn_Size * CVar_GetFloat("gBBtnScale", 0.95f);
    }
    int BBtn_factor = (1 << 10) * BBtn_Size / BBtnScaled;
    if (CVar_GetS32("gBBtnUseMargins", 0) != 0) {
        if (CVar_GetS32("gBBtnPosType", 0) == 0) {X_Margins_BtnB = Right_HUD_Margin;};
        Y_Margins_BtnB = (Top_HUD_Margin*-1);
    } else {
        X_Margins_BtnB = 0;
        Y_Margins_BtnB = 0;
    }
    s16 PosX_BtnB_ori = OTRGetRectDimensionFromRightEdge(R_ITEM_BTN_X(0)+X_Margins_BtnB);
    s16 PosY_BtnB_ori = R_ITEM_BTN_Y(0)+Y_Margins_BtnB;
    s16 PosX_BtnB;
    s16 PosY_BtnB;
    if (CVar_GetS32("gBBtnPosType", 0) != 0) {
        PosY_BtnB = CVar_GetS32("gBBtnPosY", 0)+Y_Margins_BtnB;
        if (CVar_GetS32("gBBtnPosType", 0) == 1) {//Anchor Left
            if (CVar_GetS32("gBBtnUseMargins", 0) != 0) {X_Margins_BtnB = Left_HUD_Margin;};
            PosX_BtnB = OTRGetDimensionFromLeftEdge(CVar_GetS32("gBBtnPosX", 0)+X_Margins_BtnB);
        } else if (CVar_GetS32("gBBtnPosType", 0) == 2) {//Anchor Right
            if (CVar_GetS32("gBBtnUseMargins", 0) != 0) {X_Margins_BtnB = Right_HUD_Margin;};
            PosX_BtnB = OTRGetDimensionFromRightEdge(CVar_GetS32("gBBtnPosX", 0)+X_Margins_BtnB);
        } else if (CVar_GetS32("gBBtnPosType", 0) == 3) {//Anchor None
            PosX_BtnB = CVar_GetS32("gBBtnPosX", 0);
        } else if (CVar_GetS32("gBBtnPosType", 0) == 4) {//Hidden
           PosX_BtnB = -9999;
        }
    } else {
        PosY_BtnB = PosY_BtnB_ori;
        PosX_BtnB = PosX_BtnB_ori;
    }
    //Start Button
    s16 X_Margins_StartBtn;
    s16 Y_Margins_StartBtn;
    if (CVar_GetS32("gStartBtnUseMargins", 0) != 0) {
        if (CVar_GetS32("gStartBtnPosType", 0) == 0) {
            X_Margins_StartBtn = Right_HUD_Margin;
        };
        Y_Margins_StartBtn = Top_HUD_Margin*-1;
    } else {
        X_Margins_StartBtn = 0;
        Y_Margins_StartBtn = 0;
    }
    s16 StartBtn_Icon_H = 32;
    s16 StartBtn_Icon_W = 32;
    float Start_BTN_Scale = 0.75f;
    if (CVar_GetS32("gStartBtnPosType", 0) != 0) {
        Start_BTN_Scale = CVar_GetFloat("gStartBtnScale", 0.75f);
    }
    int StartBTN_H_Scaled = StartBtn_Icon_H * Start_BTN_Scale;
    int StartBTN_W_Scaled = StartBtn_Icon_W * Start_BTN_Scale;
    int StartBTN_W_factor = (1 << 10) * StartBtn_Icon_W / StartBTN_W_Scaled;
    int StartBTN_H_factor = (1 << 10) * StartBtn_Icon_H / StartBTN_H_Scaled;
    const s16 PosX_StartBtn_ori = OTRGetRectDimensionFromRightEdge(startButtonLeftPos[gSaveContext.language]+X_Margins_StartBtn);
    const s16 PosY_StartBtn_ori = 16+Y_Margins_StartBtn;
    s16 StartBTN_Label_W = DO_ACTION_TEX_WIDTH();
    s16 StartBTN_Label_H = DO_ACTION_TEX_HEIGHT();
    s16 PosX_StartBtn;
    s16 PosY_StartBtn;
    if (CVar_GetS32("gStartBtnPosType", 0) != 0) {
        PosY_StartBtn = CVar_GetS32("gStartBtnPosY", 0)-(Start_BTN_Scale*13)+Y_Margins_StartBtn;
        if (CVar_GetS32("gStartBtnPosType", 0) == 1) {//Anchor Left
            if (CVar_GetS32("gStartBtnUseMargins", 0) != 0) {X_Margins_StartBtn = Left_HUD_Margin;};
            PosX_StartBtn = OTRGetDimensionFromLeftEdge(CVar_GetS32("gStartBtnPosX", 0)-(Start_BTN_Scale*13)+X_Margins_StartBtn);
        } else if (CVar_GetS32("gStartBtnPosType", 0) == 2) {//Anchor Right
            if (CVar_GetS32("gStartBtnUseMargins", 0) != 0) {X_Margins_StartBtn = Right_HUD_Margin;};
            PosX_StartBtn = OTRGetDimensionFromRightEdge(CVar_GetS32("gStartBtnPosX", 0)-(Start_BTN_Scale*13)+X_Margins_StartBtn);
        } else if (CVar_GetS32("gStartBtnPosType", 0) == 3) {//Anchor None
            PosX_StartBtn = CVar_GetS32("gStartBtnPosX", 0);
        } else if (CVar_GetS32("gStartBtnPosType", 0) == 4) {//Hidden
           PosX_StartBtn = -9999;
        }
    } else {
        StartBTN_H_Scaled = StartBtn_Icon_H * 0.75f;
        StartBTN_W_Scaled = StartBtn_Icon_W * 0.75f;
        StartBTN_W_factor = (1 << 10) * StartBtn_Icon_W / StartBTN_W_Scaled;
        StartBTN_H_factor = (1 << 10) * StartBtn_Icon_H / StartBTN_H_Scaled;
        PosY_StartBtn = PosY_StartBtn_ori;
        PosX_StartBtn = PosX_StartBtn_ori;
    }
    //C Buttons position
    s16 X_Margins_CL;
    s16 X_Margins_CR;
    s16 X_Margins_CU;
    s16 X_Margins_CD;
    s16 Y_Margins_CL;
    s16 Y_Margins_CR;
    s16 Y_Margins_CU;
    s16 Y_Margins_CD;
    if (CVar_GetS32("gCBtnLUseMargins", 0) != 0) {
        if (CVar_GetS32("gCBtnLPosType", 0) == 0) {X_Margins_CL = Right_HUD_Margin;};
        Y_Margins_CL = (Top_HUD_Margin*-1);
    } else {
        X_Margins_CL = 0;
        Y_Margins_CL = 0;
    }
    if (CVar_GetS32("gCBtnRUseMargins", 0) != 0) {
        if (CVar_GetS32("gCBtnRPosType", 0) == 0) {X_Margins_CR = Right_HUD_Margin;};
        Y_Margins_CR = (Top_HUD_Margin*-1);
    } else {
        X_Margins_CR = 0;
        Y_Margins_CR = 0;
    }
    if (CVar_GetS32("gCBtnUUseMargins", 0) != 0) {
        if (CVar_GetS32("gCBtnUPosType", 0) == 0) {X_Margins_CU = Right_HUD_Margin;};
        Y_Margins_CU = (Top_HUD_Margin*-1);
    } else {
        X_Margins_CU = 0;
        Y_Margins_CU = 0;
    }
    if (CVar_GetS32("gCBtnDUseMargins", 0) != 0) {
        if (CVar_GetS32("gCBtnDPosType", 0) == 0) {X_Margins_CD = Right_HUD_Margin;};
        Y_Margins_CD = (Top_HUD_Margin*-1);
    } else {
        X_Margins_CD = 0;
        Y_Margins_CD = 0;
    }
    const s16 C_Left_BTN_Pos_ori[]  = { C_LEFT_BUTTON_X+X_Margins_CL, C_LEFT_BUTTON_Y+Y_Margins_CL }; //(X,Y)
    const s16 C_Right_BTN_Pos_ori[] = { C_RIGHT_BUTTON_X+X_Margins_CR, C_RIGHT_BUTTON_Y+Y_Margins_CR };
    const s16 C_Up_BTN_Pos_ori[]    = { C_UP_BUTTON_X+X_Margins_CU, C_UP_BUTTON_Y+Y_Margins_CU };
    const s16 C_Down_BTN_Pos_ori[]  = { C_DOWN_BUTTON_X+X_Margins_CD, C_DOWN_BUTTON_Y+Y_Margins_CD };
    s16 LabelX_Navi=7 + !!CVar_GetS32("gNaviTextFix", 0);
    s16 LabelY_Navi=4;
    s16 C_Left_BTN_Pos[2]; //(X,Y)
    s16 C_Right_BTN_Pos[2];
    s16 C_Up_BTN_Pos[2];
    s16 C_Down_BTN_Pos[2];
    //C button Left
    s16 C_Left_BTN_Size = 32;
    float CLeftScale = CVar_GetFloat("gCBtnLScale", 0.87f);
    int CLeftScaled = C_Left_BTN_Size * 0.87f;
    if (CVar_GetS32("gCBtnLPosType", 0) != 0) {
        CLeftScaled = C_Left_BTN_Size * CLeftScale;
    }
    int CLeft_factor = (1 << 10) * C_Left_BTN_Size / CLeftScaled;
    if (CVar_GetS32("gCBtnLPosType", 0) != 0) {
        C_Left_BTN_Pos[1] = CVar_GetS32("gCBtnLPosY", 0)+Y_Margins_CL;
        if (CVar_GetS32("gCBtnLPosType", 0) == 1) {//Anchor Left
            if (CVar_GetS32("gCBtnLUseMargins", 0) != 0) {X_Margins_CL = Left_HUD_Margin;};
            C_Left_BTN_Pos[0] = OTRGetDimensionFromLeftEdge(CVar_GetS32("gCBtnLPosX", 0)+X_Margins_CL);
        } else if (CVar_GetS32("gCBtnLPosType", 0) == 2) {//Anchor Right
            if (CVar_GetS32("gCBtnLUseMargins", 0) != 0) {X_Margins_CL = Right_HUD_Margin;};
            C_Left_BTN_Pos[0] = OTRGetDimensionFromRightEdge(CVar_GetS32("gCBtnLPosX", 0)+X_Margins_CL);
        } else if (CVar_GetS32("gCBtnLPosType", 0) == 3) {//Anchor None
            C_Left_BTN_Pos[0] = CVar_GetS32("gCBtnLPosX", 0);
        } else if (CVar_GetS32("gCBtnLPosType", 0) == 4) {//Hidden
            C_Left_BTN_Pos[0] = -9999;
        }
    } else {
        C_Left_BTN_Pos[1] = C_Left_BTN_Pos_ori[1];
        C_Left_BTN_Pos[0] = OTRGetRectDimensionFromRightEdge(C_Left_BTN_Pos_ori[0]);
    }
    //C button Right
    s16 C_Right_BTN_Size = 32;
    float CRightScale = CVar_GetFloat("gCBtnRScale", 0.87f);
    int CRightScaled = C_Right_BTN_Size * 0.87f;
    if (CVar_GetS32("gCBtnRPosType", 0) != 0) {
        CRightScaled = C_Right_BTN_Size * CRightScale;
    }
    int CRight_factor = (1 << 10) * C_Right_BTN_Size / CRightScaled;
    if (CVar_GetS32("gCBtnRPosType", 0) != 0) {
        C_Right_BTN_Pos[1] = CVar_GetS32("gCBtnRPosY", 0)+Y_Margins_CR;
        if (CVar_GetS32("gCBtnRPosType", 0) == 1) {//Anchor Left
            if (CVar_GetS32("gCBtnRUseMargins", 0) != 0) {X_Margins_CR = Left_HUD_Margin;};
            C_Right_BTN_Pos[0] = OTRGetDimensionFromLeftEdge(CVar_GetS32("gCBtnRPosX", 0)+X_Margins_CR);
        } else if (CVar_GetS32("gCBtnRPosType", 0) == 2) {//Anchor Right
            if (CVar_GetS32("gCBtnRUseMargins", 0) != 0) {X_Margins_CR = Right_HUD_Margin;};
            C_Right_BTN_Pos[0] = OTRGetDimensionFromRightEdge(CVar_GetS32("gCBtnRPosX", 0)+X_Margins_CR);
        } else if (CVar_GetS32("gCBtnRPosType", 0) == 3) {//Anchor None
            C_Right_BTN_Pos[0] = CVar_GetS32("gCBtnRPosX", 0);
        } else if (CVar_GetS32("gCBtnRPosType", 0) == 4) {//Hidden
            C_Right_BTN_Pos[0] = -9999;
        }
    } else {
        C_Right_BTN_Pos[1] = C_Right_BTN_Pos_ori[1];
        C_Right_BTN_Pos[0] = OTRGetRectDimensionFromRightEdge(C_Right_BTN_Pos_ori[0]);
    }
    //C Button Up
    s16 C_Up_BTN_Size = 32;
    int CUpScaled = C_Up_BTN_Size * 0.5f;
    float CUpScale = CVar_GetFloat("gCBtnUScale", 0.5f);
    if (CVar_GetS32("gCBtnUPosType", 0) != 0) {
        CUpScaled = C_Up_BTN_Size * CUpScale;
    }
    int CUp_factor = (1 << 10) * C_Up_BTN_Size / CUpScaled;
    if (CVar_GetS32("gCBtnUPosType", 0) != 0) {
        C_Up_BTN_Pos[1] = CVar_GetS32("gCBtnUPosY", 0)+Y_Margins_CU;
        if (CVar_GetS32("gCBtnUPosType", 0) == 1) {//Anchor Left
            if (CVar_GetS32("gCBtnUUseMargins", 0) != 0) {X_Margins_CU = Left_HUD_Margin;};
            C_Up_BTN_Pos[0] = OTRGetDimensionFromLeftEdge(CVar_GetS32("gCBtnUPosX", 0)+X_Margins_CU);
        } else if (CVar_GetS32("gCBtnUPosType", 0) == 2) {//Anchor Right
            if (CVar_GetS32("gCBtnUUseMargins", 0) != 0) {X_Margins_CU = Right_HUD_Margin;};
            C_Up_BTN_Pos[0] = OTRGetDimensionFromRightEdge(CVar_GetS32("gCBtnUPosX", 0)+X_Margins_CU);
        } else if (CVar_GetS32("gCBtnUPosType", 0) == 3) {//Anchor None
            C_Up_BTN_Pos[0] = CVar_GetS32("gCBtnUPosX", 0);
        } else if (CVar_GetS32("gCBtnUPosType", 0) == 4) {//Hidden
            C_Up_BTN_Pos[0] = -9999;
        }
    } else {
        C_Up_BTN_Pos[1] = C_Up_BTN_Pos_ori[1];
        C_Up_BTN_Pos[0] = OTRGetRectDimensionFromRightEdge(C_Up_BTN_Pos_ori[0]);
    }
    //C Button down
    s16 C_Down_BTN_Size = 32;
    float CDownScale = CVar_GetFloat("gCBtnDScale", 0.87f);
    if (CVar_GetS32("gCBtnDPosType", 0) == 0) {
        CDownScale = 0.87f;
    }
    int CDownScaled = C_Down_BTN_Size * CDownScale;
    int CDown_factor = (1 << 10) * C_Down_BTN_Size / CDownScaled;
    int PositionAdjustment = CDownScaled/2;
    if (CVar_GetS32("gCBtnDPosType", 0) != 0) {
        C_Down_BTN_Pos[1] = CVar_GetS32("gCBtnDPosY", 0)+Y_Margins_CD;
        if (CVar_GetS32("gCBtnDPosType", 0) == 1) {//Anchor Left
            if (CVar_GetS32("gCBtnDUseMargins", 0) != 0) {X_Margins_CD = Left_HUD_Margin;};
            C_Down_BTN_Pos[0] = OTRGetDimensionFromLeftEdge(CVar_GetS32("gCBtnDPosX", 0)+X_Margins_CD);
        } else if (CVar_GetS32("gCBtnDPosType", 0) == 2) {//Anchor Right
            if (CVar_GetS32("gCBtnDUseMargins", 0) != 0) {X_Margins_CD = Right_HUD_Margin;};
            C_Down_BTN_Pos[0] = OTRGetDimensionFromRightEdge(CVar_GetS32("gCBtnDPosX", 0)+X_Margins_CD);
        } else if (CVar_GetS32("gCBtnDPosType", 0) == 3) {//Anchor None
            C_Down_BTN_Pos[0] = CVar_GetS32("gCBtnDPosX", 0);
        } else if (CVar_GetS32("gCBtnDPosType", 0) == 4) {//Hidden
            C_Down_BTN_Pos[0] = -9999;
        }
    } else {
        C_Down_BTN_Pos[1] = C_Down_BTN_Pos_ori[1];
        C_Down_BTN_Pos[0] = OTRGetRectDimensionFromRightEdge(C_Down_BTN_Pos_ori[0]);
    }

    OPEN_DISPS(play->state.gfxCtx);

    // B Button Color & Texture
    // Also loads the Item Button Texture reused by other buttons afterwards
    gDPPipeSync(OVERLAY_DISP++);
    gDPSetCombineMode(OVERLAY_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
    if (CVar_GetS32("gHudColors", 1) == 0) {
        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 0, 150, 0, interfaceCtx->bAlpha);
    } else if (CVar_GetS32("gHudColors", 1) == 1) {
        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, B_button_ori.r,B_button_ori.g,B_button_ori.b, interfaceCtx->bAlpha);
    } else if (CVar_GetS32("gHudColors", 1) == 2) {
        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, B_button.r,B_button.g,B_button.b, interfaceCtx->bAlpha);
    }
    gDPSetEnvColor(OVERLAY_DISP++, 0, 0, 0, 255);

    OVERLAY_DISP = Gfx_TextureIA8(OVERLAY_DISP, gButtonBackgroundTex, BBtn_Size, BBtn_Size, PosX_BtnB, PosY_BtnB, BBtnScaled, BBtnScaled, BBtn_factor, BBtn_factor);

    // C-Left Button Color & Texture
    gDPPipeSync(OVERLAY_DISP++);
    if (CVar_GetS32("gHudColors", 1) == 0) {
        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, C_button_ori.r, C_button_ori.g, C_button_ori.b, interfaceCtx->cLeftAlpha);
    } else if (CVar_GetS32("gHudColors", 1) == 1) {
        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, C_button_ori.r, C_button_ori.g, C_button_ori.b, interfaceCtx->cLeftAlpha);
    } else if (CVar_GetS32("gHudColors", 1) == 2 && !CVar_GetS32("gCCparated",0)) {
        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, C_button_uni.r, C_button_uni.g, C_button_uni.b, interfaceCtx->cLeftAlpha);
    } else if (CVar_GetS32("gHudColors", 1) == 2 && CVar_GetS32("gCCparated",0)) {
        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, C_button_L.r, C_button_L.g, C_button_L.b, interfaceCtx->cLeftAlpha);
    }
    gSPWideTextureRectangle(OVERLAY_DISP++, C_Left_BTN_Pos[0] << 2, C_Left_BTN_Pos[1] << 2,
                        (C_Left_BTN_Pos[0] + R_ITEM_BTN_WIDTH(1)) << 2,
                        (C_Left_BTN_Pos[1] + R_ITEM_BTN_WIDTH(1)) << 2,
                        G_TX_RENDERTILE, 0, 0, R_ITEM_BTN_DD(1) << 1, R_ITEM_BTN_DD(1) << 1);

    // C-Down Button Color & Texture
    if (CVar_GetS32("gHudColors", 1) == 0) {
        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, C_button_ori.r, C_button_ori.g, C_button_ori.b, interfaceCtx->cDownAlpha);
    } else if (CVar_GetS32("gHudColors", 1) == 1) {
        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, C_button_ori.r, C_button_ori.g, C_button_ori.b, interfaceCtx->cDownAlpha);
    } else if (CVar_GetS32("gHudColors", 1) == 2 && !CVar_GetS32("gCCparated",0)) {
        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, C_button_uni.r, C_button_uni.g, C_button_uni.b, interfaceCtx->cDownAlpha);
    } else if (CVar_GetS32("gHudColors", 1) == 2 && CVar_GetS32("gCCparated",0)) {
        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, C_button_D.r, C_button_D.g, C_button_D.b, interfaceCtx->cDownAlpha);
    }
    gSPWideTextureRectangle(OVERLAY_DISP++,  C_Down_BTN_Pos[0] << 2, C_Down_BTN_Pos[1] << 2,
                        (C_Down_BTN_Pos[0] + R_ITEM_BTN_WIDTH(2)) << 2,
                        (C_Down_BTN_Pos[1] + R_ITEM_BTN_WIDTH(2)) << 2,
                        G_TX_RENDERTILE, 0, 0, R_ITEM_BTN_DD(2) << 1, R_ITEM_BTN_DD(2) << 1);

    // C-Right Button Color & Texture
    if (CVar_GetS32("gHudColors", 1) == 0) {
        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, C_button_ori.r, C_button_ori.g, C_button_ori.b, interfaceCtx->cRightAlpha);
    } else if (CVar_GetS32("gHudColors", 1) == 1) {
        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, C_button_ori.r, C_button_ori.g, C_button_ori.b, interfaceCtx->cRightAlpha);
    } else if (CVar_GetS32("gHudColors", 1) == 2 && !CVar_GetS32("gCCparated",0)) {
        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, C_button_uni.r, C_button_uni.g, C_button_uni.b, interfaceCtx->cRightAlpha);
    } else if (CVar_GetS32("gHudColors", 1) == 2 && CVar_GetS32("gCCparated",0)) {
        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, C_button_R.r, C_button_R.g, C_button_R.b, interfaceCtx->cRightAlpha);
    }
    gSPWideTextureRectangle(OVERLAY_DISP++, C_Right_BTN_Pos[0] << 2, C_Right_BTN_Pos[1] << 2,
                        (C_Right_BTN_Pos[0] + R_ITEM_BTN_WIDTH(3)) << 2,
                        (C_Right_BTN_Pos[1] + R_ITEM_BTN_WIDTH(3)) << 2,
                        G_TX_RENDERTILE, 0, 0, R_ITEM_BTN_DD(3) << 1, R_ITEM_BTN_DD(3) << 1);

    if ((pauseCtx->state < 8) || (pauseCtx->state >= 18)) {
        if ((play->pauseCtx.state != 0) || (play->pauseCtx.debugState != 0)) {
            // Start Button Texture, Color & Label
            gDPPipeSync(OVERLAY_DISP++);

            if (CVar_GetS32("gHudColors", 1) == 0) {
                gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 200, 0, 0, interfaceCtx->startAlpha);
            } else if (CVar_GetS32("gHudColors", 1) == 1) {
                gDPSetPrimColor(OVERLAY_DISP++, 0, 0, Start_button_ori.r, Start_button_ori.g, Start_button_ori.b, interfaceCtx->startAlpha);
            } else if (CVar_GetS32("gHudColors", 1) == 2) {
                gDPSetPrimColor(OVERLAY_DISP++, 0, 0, Start_button.r, Start_button.g, Start_button.b, interfaceCtx->startAlpha);
            }
            gSPWideTextureRectangle(OVERLAY_DISP++, PosX_StartBtn << 2, PosY_StartBtn << 2,
                                (PosX_StartBtn + StartBTN_W_Scaled) << 2, (PosY_StartBtn + StartBTN_H_Scaled) << 2,
                                G_TX_RENDERTILE, 0, 0, StartBTN_W_factor, StartBTN_H_factor);
            gDPPipeSync(OVERLAY_DISP++);
            gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, interfaceCtx->startAlpha);
            gDPSetEnvColor(OVERLAY_DISP++, 0, 0, 0, 0);
            gDPSetCombineLERP(OVERLAY_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0,
                              PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);

            //There is probably a more elegant way to do it.
            char* doAction = actionsTbl[3];
            char newName[512];
            if (gSaveContext.language != LANGUAGE_ENG) {
                size_t length = strlen(doAction);
                strcpy(newName, doAction);
                if (gSaveContext.language == LANGUAGE_FRA) {
                    newName[length - 6] = 'F';
                    newName[length - 5] = 'R';
                    newName[length - 4] = 'A';
                } else if (gSaveContext.language == LANGUAGE_GER) {
                    newName[length - 6] = 'G';
                    newName[length - 5] = 'E';
                    newName[length - 4] = 'R';
                }
                doAction = newName;
            }

            memcpy(interfaceCtx->doActionSegment + DO_ACTION_TEX_SIZE() * 2, ResourceMgr_LoadTexByName(doAction), DO_ACTION_TEX_SIZE());

            gDPLoadTextureBlock_4b(OVERLAY_DISP++, interfaceCtx->doActionSegment + DO_ACTION_TEX_SIZE() * 2, G_IM_FMT_IA,
                                   DO_ACTION_TEX_WIDTH(), DO_ACTION_TEX_HEIGHT(), 0, G_TX_NOMIRROR | G_TX_WRAP,
                                   G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

            gDPPipeSync(OVERLAY_DISP++);
            gSPSetGeometryMode(OVERLAY_DISP++, G_CULL_BACK);
            gDPSetCombineLERP(OVERLAY_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0,
                            PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
            gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, interfaceCtx->startAlpha);
            gDPSetEnvColor(OVERLAY_DISP++, 0, 0, 0, 0);
            Matrix_Translate(PosX_StartBtn-160+((Start_BTN_Scale+Start_BTN_Scale/3)*11.5f), (PosY_StartBtn-120+((Start_BTN_Scale+Start_BTN_Scale/3)*11.5f)) * -1, 1.0f, MTXMODE_NEW);
            Matrix_Scale(Start_BTN_Scale+(Start_BTN_Scale/3), Start_BTN_Scale+(Start_BTN_Scale/3), Start_BTN_Scale+(Start_BTN_Scale/3), MTXMODE_APPLY);
            gSPMatrix(OVERLAY_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                    G_MTX_MODELVIEW | G_MTX_LOAD);
            gSPVertex(OVERLAY_DISP++, &interfaceCtx->actionVtx[4], 4, 0);
            Interface_DrawActionLabel(play->state.gfxCtx, interfaceCtx->doActionSegment + DO_ACTION_TEX_SIZE() * 2);
            gDPPipeSync(OVERLAY_DISP++);
        }
    }

    if (interfaceCtx->naviCalling && (play->pauseCtx.state == 0) && (play->pauseCtx.debugState == 0) &&
        (play->csCtx.state == CS_STATE_IDLE)) {
        if (!sCUpInvisible) {
            // C-Up Button Texture, Color & Label (Navi Text)
            gDPPipeSync(OVERLAY_DISP++);

            if ((gSaveContext.unk_13EA == 1) || (gSaveContext.unk_13EA == 2) || (gSaveContext.unk_13EA == 5)) {
                temp = 0;
            } else if ((player->stateFlags1 & 0x00200000) || (func_8008F2F8(play) == 4) ||
                       (player->stateFlags2 & 0x00040000)) {
                temp = 70;
            } else {
                temp = interfaceCtx->healthAlpha;
            }

            if (CVar_GetS32("gHudColors", 1) == 0) {
                gDPSetPrimColor(OVERLAY_DISP++, 0, 0, C_button_ori.r, C_button_ori.g, C_button_ori.b, temp);
            } else if (CVar_GetS32("gHudColors", 1) == 1) {
                gDPSetPrimColor(OVERLAY_DISP++, 0, 0, C_button_ori.r, C_button_ori.g, C_button_ori.b, temp);
            } else if (CVar_GetS32("gHudColors", 1) == 2 && !CVar_GetS32("gCCparated",0)) {
                gDPSetPrimColor(OVERLAY_DISP++, 0, 0, C_button_uni.r, C_button_uni.g, C_button_uni.b, temp);
            } else if (CVar_GetS32("gHudColors", 1) == 2 && CVar_GetS32("gCCparated",0)) {
                gDPSetPrimColor(OVERLAY_DISP++, 0, 0, C_button_U.r, C_button_U.g, C_button_U.b, temp);
            }
            gDPSetCombineMode(OVERLAY_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
            gSPWideTextureRectangle(OVERLAY_DISP++, C_Up_BTN_Pos[0] << 2, C_Up_BTN_Pos[1] << 2, (C_Up_BTN_Pos[0] + 16) << 2,
                                (C_Up_BTN_Pos[1] + 16) << 2, G_TX_RENDERTILE, 0, 0, 2 << 10, 2 << 10);
            gDPPipeSync(OVERLAY_DISP++);
            gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, temp);
            gDPSetEnvColor(OVERLAY_DISP++, 0, 0, 0, 0);
            gDPSetCombineLERP(OVERLAY_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0,
                              PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);

            gDPLoadTextureBlock_4b(OVERLAY_DISP++, cUpLabelTextures[gSaveContext.language], G_IM_FMT_IA, 32, 8, 0,
                                   G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                                   G_TX_NOLOD, G_TX_NOLOD);

            gSPWideTextureRectangle(OVERLAY_DISP++, C_Up_BTN_Pos[0]-LabelX_Navi << 2, C_Up_BTN_Pos[1]+LabelY_Navi << 2, 
                        (C_Up_BTN_Pos[0]-LabelX_Navi + 32) << 2, (C_Up_BTN_Pos[1]+LabelY_Navi + 8) << 2, G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);

        }

        sCUpTimer--;
        if (sCUpTimer == 0) {
            sCUpInvisible ^= 1;
            sCUpTimer = 10;
        }
    }

    gDPPipeSync(OVERLAY_DISP++);

    // Empty C Button Arrows
    for (temp = 1; temp < 4; temp++) {
        if (gSaveContext.equips.buttonItems[temp] > 0xF0) {
            s16 X_Margins_CL;
            s16 X_Margins_CR;
            s16 X_Margins_CD;
            s16 Y_Margins_CL;
            s16 Y_Margins_CR;
            s16 Y_Margins_CD;
            if (CVar_GetS32("gCBtnLUseMargins", 0) != 0) {
                if (CVar_GetS32("gCBtnLPosType", 0) == 0) {X_Margins_CL = Right_HUD_Margin;};
                Y_Margins_CL = (Top_HUD_Margin*-1);
            } else {
                X_Margins_CL = 0;
                Y_Margins_CL = 0;
            }
            if (CVar_GetS32("gCBtnRUseMargins", 0) != 0) {
                if (CVar_GetS32("gCBtnRPosType", 0) == 0) {X_Margins_CR = Right_HUD_Margin;};
                Y_Margins_CR = (Top_HUD_Margin*-1);
            } else {
                X_Margins_CR = 0;
                Y_Margins_CR = 0;
            }
            if (CVar_GetS32("gCBtnDUseMargins", 0) != 0) {
                if (CVar_GetS32("gCBtnDPosType", 0) == 0) {X_Margins_CD = Right_HUD_Margin;};
                Y_Margins_CD = (Top_HUD_Margin*-1);
            } else {
                X_Margins_CD = 0;
                Y_Margins_CD = 0;
            }
            const s16 ItemIconWidthFactor[3][2] = {
                { CLeftScaled, CLeft_factor },
                { CDownScaled, CDown_factor },
                { CRightScaled, CRight_factor },
            };
            const s16 ItemIconPos_ori[3][2] = {
                { R_ITEM_ICON_X(1)+X_Margins_CL, R_ITEM_ICON_Y(1)+Y_Margins_CL },
                { R_ITEM_ICON_X(2)+X_Margins_CD, R_ITEM_ICON_Y(2)+Y_Margins_CD },
                { R_ITEM_ICON_X(3)+X_Margins_CR, R_ITEM_ICON_Y(3)+Y_Margins_CR },
            };
            s16 ItemIconPos[3][2]; //(X,Y)
            //C button Left
            if (CVar_GetS32("gCBtnLPosType", 0) != 0) {
                ItemIconPos[0][1] = CVar_GetS32("gCBtnLPosY", 0)+Y_Margins_CL;
                if (CVar_GetS32("gCBtnLPosType", 0) == 1) {//Anchor Left
                    if (CVar_GetS32("gCBtnLUseMargins", 0) != 0) {X_Margins_CL = Left_HUD_Margin;};
                    ItemIconPos[0][0] = OTRGetDimensionFromLeftEdge(CVar_GetS32("gCBtnLPosX", 0)+X_Margins_CL);
                } else if (CVar_GetS32("gCBtnLPosType", 0) == 2) {//Anchor Right
                    if (CVar_GetS32("gCBtnLUseMargins", 0) != 0) {X_Margins_CL = Right_HUD_Margin;};
                    ItemIconPos[0][0] = OTRGetDimensionFromRightEdge(CVar_GetS32("gCBtnLPosX", 0)+X_Margins_CL);
                } else if (CVar_GetS32("gCBtnLPosType", 0) == 3) {//Anchor None
                    ItemIconPos[0][0] = CVar_GetS32("gCBtnLPosX", 0);
                } else if (CVar_GetS32("gCBtnLPosType", 0) == 4) {//Hidden
                    ItemIconPos[0][0] = -9999;
                }
            } else {
                ItemIconPos[0][0] = OTRGetRectDimensionFromRightEdge(ItemIconPos_ori[0][0]);
                ItemIconPos[0][1] = ItemIconPos_ori[0][1];
            }
            //C Button down
            if (CVar_GetS32("gCBtnDPosType", 0) != 0) {
                ItemIconPos[1][1] = CVar_GetS32("gCBtnDPosY", 0)+Y_Margins_CD;
                if (CVar_GetS32("gCBtnDPosType", 0) == 1) {//Anchor Left
                    if (CVar_GetS32("gCBtnDUseMargins", 0) != 0) {X_Margins_CD = Left_HUD_Margin;};
                    ItemIconPos[1][0] = OTRGetDimensionFromLeftEdge(CVar_GetS32("gCBtnDPosX", 0)+X_Margins_CD);
                } else if (CVar_GetS32("gCBtnDPosType", 0) == 2) {//Anchor Right
                    if (CVar_GetS32("gCBtnDUseMargins", 0) != 0) {X_Margins_CD = Right_HUD_Margin;};
                    ItemIconPos[1][0] = OTRGetDimensionFromRightEdge(CVar_GetS32("gCBtnDPosX", 0)+X_Margins_CD);
                } else if (CVar_GetS32("gCBtnDPosType", 0) == 3) {//Anchor None
                    ItemIconPos[1][0] = CVar_GetS32("gCBtnDPosX", 0);
                } else if (CVar_GetS32("gCBtnDPosType", 0) == 4) {//Hidden
                    ItemIconPos[1][0] = -9999;
                }
            } else {
                ItemIconPos[1][0] = OTRGetRectDimensionFromRightEdge(ItemIconPos_ori[1][0]);
                ItemIconPos[1][1] = ItemIconPos_ori[1][1];
            }
            //C button Right
            if (CVar_GetS32("gCBtnRPosType", 0) != 0) {
                ItemIconPos[2][1] = CVar_GetS32("gCBtnRPosY", 0)+Y_Margins_CR;
                if (CVar_GetS32("gCBtnRPosType", 0) == 1) {//Anchor Left
                    if (CVar_GetS32("gCBtnRUseMargins", 0) != 0) {X_Margins_CR = Left_HUD_Margin;};
                    ItemIconPos[2][0] = OTRGetDimensionFromLeftEdge(CVar_GetS32("gCBtnRPosX", 0)+X_Margins_CR);
                } else if (CVar_GetS32("gCBtnRPosType", 0) == 2) {//Anchor Right
                    if (CVar_GetS32("gCBtnRUseMargins", 0) != 0) {X_Margins_CR = Right_HUD_Margin;};
                    ItemIconPos[2][0] = OTRGetDimensionFromRightEdge(CVar_GetS32("gCBtnRPosX", 0)+X_Margins_CR);
                } else if (CVar_GetS32("gCBtnRPosType", 0) == 3) {//Anchor None
                    ItemIconPos[2][0] = CVar_GetS32("gCBtnRPosX", 0);
                } else if (CVar_GetS32("gCBtnRPosType", 0) == 4) {//Hidden
                    ItemIconPos[2][0] = -9999;
                }
            } else {
                ItemIconPos[2][0] = OTRGetRectDimensionFromRightEdge(ItemIconPos_ori[2][0]);
                ItemIconPos[2][1] = ItemIconPos_ori[2][1];
            }

            if (temp == 1) {
                gDPSetPrimColor(OVERLAY_DISP++, 0, 0, C_button_ori.r, C_button_ori.g, C_button_ori.b, interfaceCtx->cLeftAlpha);
            } else if (temp == 2) {
                gDPSetPrimColor(OVERLAY_DISP++, 0, 0, C_button_ori.r, C_button_ori.g, C_button_ori.b, interfaceCtx->cDownAlpha);
            } else {
                gDPSetPrimColor(OVERLAY_DISP++, 0, 0, C_button_ori.r, C_button_ori.g, C_button_ori.b, interfaceCtx->cRightAlpha);
            }

            if (CVar_GetS32("gHudColors", 1) == 2 && !CVar_GetS32("gCCparated", 0)) {
                gDPSetPrimColor(OVERLAY_DISP++, 0, 0, C_button_uni.r, C_button_uni.g, C_button_uni.b, interfaceCtx->cRightAlpha);
            } else if (temp == 1 && CVar_GetS32("gHudColors", 1) == 2 && CVar_GetS32("gCCparated", 0)) {
                gDPSetPrimColor(OVERLAY_DISP++, 0, 0, C_button_L.r, C_button_L.g, C_button_L.b, interfaceCtx->cLeftAlpha);
            } else if (temp == 2 && CVar_GetS32("gHudColors", 1) == 2 && CVar_GetS32("gCCparated", 0)) {
                gDPSetPrimColor(OVERLAY_DISP++, 0, 0, C_button_D.r, C_button_D.g, C_button_D.b, interfaceCtx->cDownAlpha);
            } else if (CVar_GetS32("gHudColors", 1) == 2 && CVar_GetS32("gCCparated", 0)) {
                gDPSetPrimColor(OVERLAY_DISP++, 0, 0, C_button_R.r, C_button_R.g, C_button_R.b, interfaceCtx->cRightAlpha);
            }
            
            OVERLAY_DISP = Gfx_TextureIA8(OVERLAY_DISP, ((u8*)gButtonBackgroundTex), 32, 32, 
                                          ItemIconPos[temp-1][0], ItemIconPos[temp-1][1], ItemIconWidthFactor[temp-1][0],
                                          ItemIconWidthFactor[temp-1][0], ItemIconWidthFactor[temp-1][1], ItemIconWidthFactor[temp-1][1]);

            const char* cButtonIcons[] = { gButtonBackgroundTex, gEquippedItemOutlineTex, gEmptyCLeftArrowTex,
                                           gEmptyCDownArrowTex, gEmptyCRightArrowTex
            };

            OVERLAY_DISP = Gfx_TextureIA8(OVERLAY_DISP, cButtonIcons[(temp + 1)], 32, 32,
                                          ItemIconPos[temp-1][0], ItemIconPos[temp-1][1], ItemIconWidthFactor[temp-1][0],
                                          ItemIconWidthFactor[temp-1][0], ItemIconWidthFactor[temp-1][1], ItemIconWidthFactor[temp-1][1]);
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

int16_t gItemIconWidth[] = { 30, 24, 24, 24, 16, 16, 16, 16 };
int16_t gItemIconDD[] = { 550, 680, 680, 680, 1024, 1024, 1024, 1024 };

void Interface_DrawItemIconTexture(PlayState* play, void* texture, s16 button) {
    OPEN_DISPS(play->state.gfxCtx);
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    InterfaceContext* interfaceCtx = &play->interfaceCtx;
    s16 X_Margins_CL;
    s16 X_Margins_CR;
    s16 X_Margins_CD;
    s16 Y_Margins_CL;
    s16 Y_Margins_CR;
    s16 Y_Margins_CD;
    s16 X_Margins_BtnB;
    s16 Y_Margins_BtnB;
    s16 X_Margins_DPad_Items;
    s16 Y_Margins_DPad_Items;
    if (CVar_GetS32("gBBtnUseMargins", 0) != 0) {
        if (CVar_GetS32("gBBtnPosType", 0) == 0) {X_Margins_BtnB = Right_HUD_Margin;};
        Y_Margins_BtnB = (Top_HUD_Margin*-1);
    } else {
        X_Margins_BtnB = 0;
        Y_Margins_BtnB = 0;
    }
    if (CVar_GetS32("gCBtnLUseMargins", 0) != 0) {
        if (CVar_GetS32("gCBtnLPosType", 0) == 0) {X_Margins_CL = Right_HUD_Margin;};
        Y_Margins_CL = (Top_HUD_Margin*-1);
    } else {
        X_Margins_CL = 0;
        Y_Margins_CL = 0;
    }
    if (CVar_GetS32("gCBtnRUseMargins", 0) != 0) {
        if (CVar_GetS32("gCBtnRPosType", 0) == 0) {X_Margins_CR = Right_HUD_Margin;};
        Y_Margins_CR = (Top_HUD_Margin*-1);
    } else {
        X_Margins_CR = 0;
        Y_Margins_CR = 0;
    }
    if (CVar_GetS32("gCBtnDUseMargins", 0) != 0) {
        if (CVar_GetS32("gCBtnDPosType", 0) == 0) {X_Margins_CD = Right_HUD_Margin;};
        Y_Margins_CD = (Top_HUD_Margin*-1);
    } else {
        X_Margins_CD = 0;
        Y_Margins_CD = 0;
    }
    if (CVar_GetS32("gDPadUseMargins", 0) != 0) {
        if (CVar_GetS32("gDPadPosType", 0) == 0) {X_Margins_DPad_Items = Right_HUD_Margin;};
        Y_Margins_DPad_Items = (Top_HUD_Margin*-1);
    } else {
        X_Margins_DPad_Items = 0;
        Y_Margins_DPad_Items = 0;
    }
    const s16 ItemIconPos_ori[8][2] = {
        { B_BUTTON_X+X_Margins_BtnB, B_BUTTON_Y+Y_Margins_BtnB },
        { C_LEFT_BUTTON_X+X_Margins_CL, C_LEFT_BUTTON_Y+Y_Margins_CL },
        { C_DOWN_BUTTON_X+X_Margins_CD, C_DOWN_BUTTON_Y+Y_Margins_CD },
        { C_RIGHT_BUTTON_X+X_Margins_CR, C_RIGHT_BUTTON_Y+Y_Margins_CR },
        { DPAD_UP_X+X_Margins_DPad_Items, DPAD_UP_Y+Y_Margins_DPad_Items },
        { DPAD_DOWN_X+X_Margins_DPad_Items, DPAD_DOWN_Y+Y_Margins_DPad_Items }, 
        { DPAD_LEFT_X+X_Margins_DPad_Items, DPAD_LEFT_Y+Y_Margins_DPad_Items }, 
        { DPAD_RIGHT_X+X_Margins_DPad_Items, DPAD_RIGHT_Y+Y_Margins_DPad_Items }
    };
    u16 ItemsSlotsAlpha[8] = {
        interfaceCtx->bAlpha,
        interfaceCtx->cLeftAlpha,
        interfaceCtx->cRightAlpha,
        interfaceCtx->cDownAlpha,
        interfaceCtx->dpadUpAlpha,
        interfaceCtx->dpadDownAlpha,
        interfaceCtx->dpadLeftAlpha,
        interfaceCtx->dpadRightAlpha
    };
    s16 DPad_ItemsOffset[4][2] = {
        { 7,-8},//Up
        { 7,24},//Down
        {-9, 8},//Left
        {23, 8},//Right
    }; //(X,Y) Used with custom position to place it properly.
    s16 ItemIconPos[8][2]; //(X,Y)
    //DPadItems
    if (CVar_GetS32("gDPadPosType", 0) != 0) {
        ItemIconPos[4][1] = CVar_GetS32("gDPadPosY", 0)+Y_Margins_DPad_Items+DPad_ItemsOffset[0][1];//Up
        ItemIconPos[5][1] = CVar_GetS32("gDPadPosY", 0)+Y_Margins_DPad_Items+DPad_ItemsOffset[1][1];//Down
        ItemIconPos[6][1] = CVar_GetS32("gDPadPosY", 0)+Y_Margins_DPad_Items+DPad_ItemsOffset[2][1];//Left
        ItemIconPos[7][1] = CVar_GetS32("gDPadPosY", 0)+Y_Margins_DPad_Items+DPad_ItemsOffset[3][1];//Right
        if (CVar_GetS32("gDPadPosType", 0) == 1) {//Anchor Left
            if (CVar_GetS32("gDPadUseMargins", 0) != 0) {X_Margins_DPad_Items = Left_HUD_Margin;};
            ItemIconPos[4][0] = OTRGetDimensionFromLeftEdge(CVar_GetS32("gDPadPosX", 0)+X_Margins_DPad_Items+DPad_ItemsOffset[0][0]);
            ItemIconPos[5][0] = OTRGetDimensionFromLeftEdge(CVar_GetS32("gDPadPosX", 0)+X_Margins_DPad_Items+DPad_ItemsOffset[1][0]);
            ItemIconPos[6][0] = OTRGetDimensionFromLeftEdge(CVar_GetS32("gDPadPosX", 0)+X_Margins_DPad_Items+DPad_ItemsOffset[2][0]);
            ItemIconPos[7][0] = OTRGetDimensionFromLeftEdge(CVar_GetS32("gDPadPosX", 0)+X_Margins_DPad_Items+DPad_ItemsOffset[3][0]);
        } else if (CVar_GetS32("gDPadPosType", 0) == 2) {//Anchor Right
            if (CVar_GetS32("gDPadUseMargins", 0) != 0) {X_Margins_DPad_Items = Right_HUD_Margin;};
            ItemIconPos[4][0] = OTRGetDimensionFromRightEdge(CVar_GetS32("gDPadPosX", 0)+X_Margins_DPad_Items+DPad_ItemsOffset[0][0]);
            ItemIconPos[5][0] = OTRGetDimensionFromRightEdge(CVar_GetS32("gDPadPosX", 0)+X_Margins_DPad_Items+DPad_ItemsOffset[1][0]);
            ItemIconPos[6][0] = OTRGetDimensionFromRightEdge(CVar_GetS32("gDPadPosX", 0)+X_Margins_DPad_Items+DPad_ItemsOffset[2][0]);
            ItemIconPos[7][0] = OTRGetDimensionFromRightEdge(CVar_GetS32("gDPadPosX", 0)+X_Margins_DPad_Items+DPad_ItemsOffset[3][0]);
        } else if (CVar_GetS32("gDPadPosType", 0) == 3) {//Anchor None
            ItemIconPos[4][0] = CVar_GetS32("gDPadPosX", 0)+DPad_ItemsOffset[0][0];
            ItemIconPos[5][0] = CVar_GetS32("gDPadPosX", 0)+DPad_ItemsOffset[1][0];
            ItemIconPos[6][0] = CVar_GetS32("gDPadPosX", 0)+DPad_ItemsOffset[2][0];
            ItemIconPos[7][0] = CVar_GetS32("gDPadPosX", 0)+DPad_ItemsOffset[3][0];
        } else if (CVar_GetS32("gDPadPosType", 0) == 4) {//Hidden
            ItemIconPos[4][0] = -9999;
            ItemIconPos[5][0] = -9999;
            ItemIconPos[6][0] = -9999;
            ItemIconPos[7][0] = -9999;
        }
    } else {
        ItemIconPos[4][0] = OTRGetDimensionFromRightEdge(ItemIconPos_ori[4][0]);
        ItemIconPos[5][0] = OTRGetDimensionFromRightEdge(ItemIconPos_ori[5][0]);
        ItemIconPos[6][0] = OTRGetDimensionFromRightEdge(ItemIconPos_ori[6][0]);
        ItemIconPos[7][0] = OTRGetDimensionFromRightEdge(ItemIconPos_ori[7][0]);
        ItemIconPos[4][1] = ItemIconPos_ori[4][1];
        ItemIconPos[5][1] = ItemIconPos_ori[5][1];
        ItemIconPos[6][1] = ItemIconPos_ori[6][1];
        ItemIconPos[7][1] = ItemIconPos_ori[7][1];
    }
    //B Button
    if (CVar_GetS32("gBBtnPosType", 0) != 0) {
        ItemIconPos[0][1] = CVar_GetS32("gBBtnPosY", 0)+Y_Margins_BtnB;
        if (CVar_GetS32("gBBtnPosType", 0) == 1) {//Anchor Left
            if (CVar_GetS32("gBBtnUseMargins", 0) != 0) {X_Margins_BtnB = Left_HUD_Margin;};
            ItemIconPos[0][0] = OTRGetDimensionFromLeftEdge(CVar_GetS32("gBBtnPosX", 0)+X_Margins_BtnB);
        } else if (CVar_GetS32("gBBtnPosType", 0) == 2) {//Anchor Right
            if (CVar_GetS32("gBBtnUseMargins", 0) != 0) {X_Margins_BtnB = Right_HUD_Margin;};
            ItemIconPos[0][0] = OTRGetDimensionFromRightEdge(CVar_GetS32("gBBtnPosX", 0)+X_Margins_BtnB);
        } else if (CVar_GetS32("gBBtnPosType", 0) == 3) {//Anchor None
            ItemIconPos[0][0] = CVar_GetS32("gBBtnPosX", 0);
        } else if (CVar_GetS32("gBBtnPosType", 0) == 4) {//Hidden
           ItemIconPos[0][0] = -9999;
        }
    } else {
        ItemIconPos[0][0] = OTRGetRectDimensionFromRightEdge(ItemIconPos_ori[0][0]);
        ItemIconPos[0][1] = ItemIconPos_ori[0][1];
    }
    //C button Left
    if (CVar_GetS32("gCBtnLPosType", 0) != 0) {
        ItemIconPos[1][1] = CVar_GetS32("gCBtnLPosY", 0)+Y_Margins_CL;
        if (CVar_GetS32("gCBtnLPosType", 0) == 1) {//Anchor Left
            if (CVar_GetS32("gCBtnLUseMargins", 0) != 0) {X_Margins_CL = Left_HUD_Margin;};
            ItemIconPos[1][0] = OTRGetDimensionFromLeftEdge(CVar_GetS32("gCBtnLPosX", 0)+X_Margins_CL);
        } else if (CVar_GetS32("gCBtnLPosType", 0) == 2) {//Anchor Right
            if (CVar_GetS32("gCBtnLUseMargins", 0) != 0) {X_Margins_CL = Right_HUD_Margin;};
            ItemIconPos[1][0] = OTRGetDimensionFromRightEdge(CVar_GetS32("gCBtnLPosX", 0)+X_Margins_CL);
        } else if (CVar_GetS32("gCBtnLPosType", 0) == 3) {//Anchor None
            ItemIconPos[1][0] = CVar_GetS32("gCBtnLPosX", 0);
        } else if (CVar_GetS32("gCBtnLPosType", 0) == 4) {//Hidden
            ItemIconPos[1][0] = -9999;
        }
    } else {
        ItemIconPos[1][0] = OTRGetRectDimensionFromRightEdge(ItemIconPos_ori[1][0]);
        ItemIconPos[1][1] = ItemIconPos_ori[1][1];
    }
    //C Button down
    if (CVar_GetS32("gCBtnDPosType", 0) != 0) {
        ItemIconPos[2][1] = CVar_GetS32("gCBtnDPosY", 0)+Y_Margins_CD;
        if (CVar_GetS32("gCBtnDPosType", 0) == 1) {//Anchor Left
            if (CVar_GetS32("gCBtnDUseMargins", 0) != 0) {X_Margins_CD = Left_HUD_Margin;};
            ItemIconPos[2][0] = OTRGetDimensionFromLeftEdge(CVar_GetS32("gCBtnDPosX", 0)+X_Margins_CD);
        } else if (CVar_GetS32("gCBtnDPosType", 0) == 2) {//Anchor Right
            if (CVar_GetS32("gCBtnDUseMargins", 0) != 0) {X_Margins_CD = Right_HUD_Margin;};
            ItemIconPos[2][0] = OTRGetDimensionFromRightEdge(CVar_GetS32("gCBtnDPosX", 0)+X_Margins_CD);
        } else if (CVar_GetS32("gCBtnDPosType", 0) == 3) {//Anchor None
            ItemIconPos[2][0] = CVar_GetS32("gCBtnDPosX", 0);
        } else if (CVar_GetS32("gCBtnDPosType", 0) == 4) {//Hidden
            ItemIconPos[2][0] = -9999;
        }
    } else {
        ItemIconPos[2][0] = OTRGetRectDimensionFromRightEdge(ItemIconPos_ori[2][0]);
        ItemIconPos[2][1] = ItemIconPos_ori[2][1];
    }
    //C button Right
    if (CVar_GetS32("gCBtnRPosType", 0) != 0) {
        ItemIconPos[3][1] = CVar_GetS32("gCBtnRPosY", 0)+Y_Margins_CR;
        if (CVar_GetS32("gCBtnRPosType", 0) == 1) {//Anchor Left
            if (CVar_GetS32("gCBtnRUseMargins", 0) != 0) {X_Margins_CR = Left_HUD_Margin;};
            ItemIconPos[3][0] = OTRGetDimensionFromLeftEdge(CVar_GetS32("gCBtnRPosX", 0)+X_Margins_CR);
        } else if (CVar_GetS32("gCBtnRPosType", 0) == 2) {//Anchor Right
            if (CVar_GetS32("gCBtnRUseMargins", 0) != 0) {X_Margins_CR = Right_HUD_Margin;};
            ItemIconPos[3][0] = OTRGetDimensionFromRightEdge(CVar_GetS32("gCBtnRPosX", 0)+X_Margins_CR);
        } else if (CVar_GetS32("gCBtnRPosType", 0) == 3) {//Anchor None
            ItemIconPos[3][0] = CVar_GetS32("gCBtnRPosX", 0);
        } else if (CVar_GetS32("gCBtnRPosType", 0) == 4) {//Hidden
            ItemIconPos[3][0] = -9999;
        }
    } else {
        ItemIconPos[3][0] = OTRGetRectDimensionFromRightEdge(ItemIconPos_ori[3][0]);
        ItemIconPos[3][1] = ItemIconPos_ori[3][1];
    }

    gDPLoadTextureBlock(OVERLAY_DISP++, texture, G_IM_FMT_RGBA, G_IM_SIZ_32b, 32, 32, 0, G_TX_NOMIRROR | G_TX_WRAP,
                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

    gSPWideTextureRectangle(OVERLAY_DISP++, ItemIconPos[button][0] << 2, ItemIconPos[button][1] << 2,
                        (ItemIconPos[button][0] + gItemIconWidth[button]) << 2,
                        (ItemIconPos[button][1] + gItemIconWidth[button]) << 2, G_TX_RENDERTILE, 0, 0,
                        gItemIconDD[button] << 1, gItemIconDD[button] << 1);

    CLOSE_DISPS(play->state.gfxCtx);
}

const char* _gAmmoDigit0Tex[] =
{
    gAmmoDigit0Tex, gAmmoDigit1Tex, gAmmoDigit2Tex, gAmmoDigit3Tex, gAmmoDigit4Tex,
    gAmmoDigit5Tex, gAmmoDigit6Tex, gAmmoDigit7Tex, gAmmoDigit8Tex, gAmmoDigit9Tex,
    gUnusedAmmoDigitHalfTex };

static int16_t gItemAmmoX[] = { B_BUTTON_X + 2, C_LEFT_BUTTON_X + 1, C_DOWN_BUTTON_X + 1, C_RIGHT_BUTTON_X + 1,
                                DPAD_UP_X,      DPAD_DOWN_X,         DPAD_LEFT_X,         DPAD_RIGHT_X };
static int16_t gItemAmmoY[] = { B_BUTTON_Y + 18, C_LEFT_BUTTON_Y + 17, C_DOWN_BUTTON_Y + 17, C_RIGHT_BUTTON_Y + 17,
                                DPAD_UP_Y + 11,  DPAD_DOWN_Y + 11,     DPAD_LEFT_Y + 11,     DPAD_RIGHT_Y + 11 };

void Interface_DrawAmmoCount(PlayState* play, s16 button, s16 alpha) {
    s16 i;
    s16 ammo;
    s16 X_Margins_CL;
    s16 X_Margins_CR;
    s16 X_Margins_CD;
    s16 Y_Margins_CL;
    s16 Y_Margins_CR;
    s16 Y_Margins_CD;
    s16 X_Margins_BtnB;
    s16 Y_Margins_BtnB;
    s16 X_Margins_DPad_Items;
    s16 Y_Margins_DPad_Items;
    if (CVar_GetS32("gBBtnUseMargins", 0) != 0) {
        if (CVar_GetS32("gBBtnPosType", 0) == 0) {X_Margins_BtnB = Right_HUD_Margin;};
        Y_Margins_BtnB = (Top_HUD_Margin*-1);
    } else {
        X_Margins_BtnB = 0;
        Y_Margins_BtnB = 0;
    }
    if (CVar_GetS32("gCBtnLUseMargins", 0) != 0) {
        if (CVar_GetS32("gCBtnLPosType", 0) == 0) {X_Margins_CL = Right_HUD_Margin;};
        Y_Margins_CL = (Top_HUD_Margin*-1);
    } else {
        X_Margins_CL = 0;
        Y_Margins_CL = 0;
    }
    if (CVar_GetS32("gCBtnRUseMargins", 0) != 0) {
        if (CVar_GetS32("gCBtnRPosType", 0) == 0) {X_Margins_CR = Right_HUD_Margin;};
        Y_Margins_CR = (Top_HUD_Margin*-1);
    } else {
        X_Margins_CR = 0;
        Y_Margins_CR = 0;
    }
    if (CVar_GetS32("gCBtnDUseMargins", 0) != 0) {
        if (CVar_GetS32("gCBtnDPosType", 0) == 0) {X_Margins_CD = Right_HUD_Margin;};
        Y_Margins_CD = (Top_HUD_Margin*-1);
    } else {
        X_Margins_CD = 0;
        Y_Margins_CD = 0;
    }
    if (CVar_GetS32("gDPadUseMargins", 0) != 0) {
        if (CVar_GetS32("gDPadPosType", 0) == 0) {X_Margins_DPad_Items = Right_HUD_Margin;};
        Y_Margins_DPad_Items = (Top_HUD_Margin*-1);
    } else {
        X_Margins_DPad_Items = 0;
        Y_Margins_DPad_Items = 0;
    }
    const s16 ItemIconPos_ori[8][2] = {
        { R_ITEM_AMMO_X(0)+X_Margins_BtnB, R_ITEM_AMMO_Y(0)+Y_Margins_BtnB }, //Bow on Epona?
        { R_ITEM_AMMO_X(1)+X_Margins_CL, R_ITEM_AMMO_Y(1)+Y_Margins_CL },
        { R_ITEM_AMMO_X(2)+X_Margins_CD, R_ITEM_AMMO_Y(2)+Y_Margins_CD },
        { R_ITEM_AMMO_X(3)+X_Margins_CR, R_ITEM_AMMO_Y(3)+Y_Margins_CR },
        { DPAD_UP_X+X_Margins_DPad_Items, DPAD_UP_Y + 11 + Y_Margins_DPad_Items },
        { DPAD_DOWN_X+X_Margins_DPad_Items, DPAD_DOWN_Y + 11 + Y_Margins_DPad_Items },
        { DPAD_LEFT_X+X_Margins_DPad_Items, DPAD_LEFT_Y + 11 + Y_Margins_DPad_Items },
        { DPAD_RIGHT_X+X_Margins_DPad_Items, DPAD_RIGHT_Y + 11 + Y_Margins_DPad_Items }
    };
    s16 ItemIconPos[8][2]; //(X,Y)
    s16 DPad_ItemsOffset[4][2] = {
        { 7, 3},//Up
        { 7,35},//Down
        {-9,19},//Left
        {23,19},//Right
    }; //(X,Y) Used with custom position to place it properly.
    //DPadItems
    if (CVar_GetS32("gDPadPosType", 0) != 0) {
        ItemIconPos[4][1] = CVar_GetS32("gDPadPosY", 0)+Y_Margins_DPad_Items+DPad_ItemsOffset[0][1];//Up
        ItemIconPos[5][1] = CVar_GetS32("gDPadPosY", 0)+Y_Margins_DPad_Items+DPad_ItemsOffset[1][1];//Down
        ItemIconPos[6][1] = CVar_GetS32("gDPadPosY", 0)+Y_Margins_DPad_Items+DPad_ItemsOffset[2][1];//Left
        ItemIconPos[7][1] = CVar_GetS32("gDPadPosY", 0)+Y_Margins_DPad_Items+DPad_ItemsOffset[3][1];//Right
        if (CVar_GetS32("gDPadPosType", 0) == 1) {//Anchor Left
            if (CVar_GetS32("gDPadUseMargins", 0) != 0) {X_Margins_DPad_Items = Left_HUD_Margin;};
            ItemIconPos[4][0] = OTRGetDimensionFromLeftEdge(CVar_GetS32("gDPadPosX", 0)+X_Margins_DPad_Items+DPad_ItemsOffset[0][0]);
            ItemIconPos[5][0] = OTRGetDimensionFromLeftEdge(CVar_GetS32("gDPadPosX", 0)+X_Margins_DPad_Items+DPad_ItemsOffset[1][0]);
            ItemIconPos[6][0] = OTRGetDimensionFromLeftEdge(CVar_GetS32("gDPadPosX", 0)+X_Margins_DPad_Items+DPad_ItemsOffset[2][0]);
            ItemIconPos[7][0] = OTRGetDimensionFromLeftEdge(CVar_GetS32("gDPadPosX", 0)+X_Margins_DPad_Items+DPad_ItemsOffset[3][0]);
        } else if (CVar_GetS32("gDPadPosType", 0) == 2) {//Anchor Right
            if (CVar_GetS32("gDPadUseMargins", 0) != 0) {X_Margins_DPad_Items = Right_HUD_Margin;};
            ItemIconPos[4][0] = OTRGetDimensionFromRightEdge(CVar_GetS32("gDPadPosX", 0)+X_Margins_DPad_Items+DPad_ItemsOffset[0][0]);
            ItemIconPos[5][0] = OTRGetDimensionFromRightEdge(CVar_GetS32("gDPadPosX", 0)+X_Margins_DPad_Items+DPad_ItemsOffset[1][0]);
            ItemIconPos[6][0] = OTRGetDimensionFromRightEdge(CVar_GetS32("gDPadPosX", 0)+X_Margins_DPad_Items+DPad_ItemsOffset[2][0]);
            ItemIconPos[7][0] = OTRGetDimensionFromRightEdge(CVar_GetS32("gDPadPosX", 0)+X_Margins_DPad_Items+DPad_ItemsOffset[3][0]);
        } else if (CVar_GetS32("gDPadPosType", 0) == 3) {//Anchor None
            ItemIconPos[4][0] = CVar_GetS32("gDPadPosX", 0)+DPad_ItemsOffset[0][0];
            ItemIconPos[5][0] = CVar_GetS32("gDPadPosX", 0)+DPad_ItemsOffset[1][0];
            ItemIconPos[6][0] = CVar_GetS32("gDPadPosX", 0)+DPad_ItemsOffset[2][0];
            ItemIconPos[7][0] = CVar_GetS32("gDPadPosX", 0)+DPad_ItemsOffset[3][0];
        } else if (CVar_GetS32("gDPadPosType", 0) == 4) {//Hidden
            ItemIconPos[4][0] = -9999;
            ItemIconPos[5][0] = -9999;
            ItemIconPos[6][0] = -9999;
            ItemIconPos[7][0] = -9999;
        }
    } else {
        ItemIconPos[4][0] = OTRGetDimensionFromRightEdge(ItemIconPos_ori[4][0]);
        ItemIconPos[5][0] = OTRGetDimensionFromRightEdge(ItemIconPos_ori[5][0]);
        ItemIconPos[6][0] = OTRGetDimensionFromRightEdge(ItemIconPos_ori[6][0]);
        ItemIconPos[7][0] = OTRGetDimensionFromRightEdge(ItemIconPos_ori[7][0]);
        ItemIconPos[4][1] = ItemIconPos_ori[4][1];
        ItemIconPos[5][1] = ItemIconPos_ori[5][1];
        ItemIconPos[6][1] = ItemIconPos_ori[6][1];
        ItemIconPos[7][1] = ItemIconPos_ori[7][1];
    }
    //B Button
    s16 PosX_adjust = 1;
    s16 PosY_adjust = 17;
    if (CVar_GetS32("gBBtnPosType", 0) != 0) {
        ItemIconPos[0][1] = CVar_GetS32("gBBtnPosY", 0)+Y_Margins_BtnB+PosY_adjust;
        if (CVar_GetS32("gBBtnPosType", 0) == 1) {//Anchor Left
            if (CVar_GetS32("gBBtnUseMargins", 0) != 0) {X_Margins_BtnB = Left_HUD_Margin;};
            ItemIconPos[0][0] = OTRGetDimensionFromLeftEdge(CVar_GetS32("gBBtnPosX", 0)+X_Margins_BtnB+PosX_adjust);
        } else if (CVar_GetS32("gBBtnPosType", 0) == 2) {//Anchor Right
            if (CVar_GetS32("gBBtnUseMargins", 0) != 0) {X_Margins_BtnB = Right_HUD_Margin;};
            ItemIconPos[0][0] = OTRGetDimensionFromRightEdge(CVar_GetS32("gBBtnPosX", 0)+X_Margins_BtnB+PosX_adjust);
        } else if (CVar_GetS32("gBBtnPosType", 0) == 3) {//Anchor None
            ItemIconPos[0][0] = CVar_GetS32("gBBtnPosX", 0)+PosX_adjust;
        } else if (CVar_GetS32("gBBtnPosType", 0) == 4) {//Hidden
           ItemIconPos[0][0] = -9999;
        }
    } else {
        ItemIconPos[0][0] = OTRGetRectDimensionFromRightEdge(ItemIconPos_ori[0][0]);
        ItemIconPos[0][1] = ItemIconPos_ori[0][1];
    }
    //C button Left
    if (CVar_GetS32("gCBtnLPosType", 0) != 0) {
        ItemIconPos[1][1] = CVar_GetS32("gCBtnLPosY", 0)+Y_Margins_CL+PosY_adjust;
        if (CVar_GetS32("gCBtnLPosType", 0) == 1) {//Anchor Left
            if (CVar_GetS32("gCBtnLUseMargins", 0) != 0) {X_Margins_CL = Left_HUD_Margin;};
            ItemIconPos[1][0] = OTRGetDimensionFromLeftEdge(CVar_GetS32("gCBtnLPosX", 0)+X_Margins_CL+PosX_adjust);
        } else if (CVar_GetS32("gCBtnLPosType", 0) == 2) {//Anchor Right
            if (CVar_GetS32("gCBtnLUseMargins", 0) != 0) {X_Margins_CL = Right_HUD_Margin;};
            ItemIconPos[1][0] = OTRGetDimensionFromRightEdge(CVar_GetS32("gCBtnLPosX", 0)+X_Margins_CL+PosX_adjust);
        } else if (CVar_GetS32("gCBtnLPosType", 0) == 3) {//Anchor None
            ItemIconPos[1][0] = CVar_GetS32("gCBtnLPosX", 0)+PosX_adjust;
        } else if (CVar_GetS32("gCBtnLPosType", 0) == 4) {//Hidden
            ItemIconPos[1][0] = -9999;
        }
    } else {
        ItemIconPos[1][0] = OTRGetRectDimensionFromRightEdge(ItemIconPos_ori[1][0]);
        ItemIconPos[1][1] = ItemIconPos_ori[1][1];
    }
    //C Button down
    if (CVar_GetS32("gCBtnDPosType", 0) != 0) {
        ItemIconPos[2][1] = CVar_GetS32("gCBtnDPosY", 0)+Y_Margins_CD+PosY_adjust;
        if (CVar_GetS32("gCBtnDPosType", 0) == 1) {//Anchor Left
            if (CVar_GetS32("gCBtnDUseMargins", 0) != 0) {X_Margins_CD = Left_HUD_Margin;};
            ItemIconPos[2][0] = OTRGetDimensionFromLeftEdge(CVar_GetS32("gCBtnDPosX", 0)+X_Margins_CD+PosX_adjust);
        } else if (CVar_GetS32("gCBtnDPosType", 0) == 2) {//Anchor Right
            if (CVar_GetS32("gCBtnDUseMargins", 0) != 0) {X_Margins_CD = Right_HUD_Margin;};
            ItemIconPos[2][0] = OTRGetDimensionFromRightEdge(CVar_GetS32("gCBtnDPosX", 0)+X_Margins_CD+PosX_adjust);
        } else if (CVar_GetS32("gCBtnDPosType", 0) == 3) {//Anchor None
            ItemIconPos[2][0] = CVar_GetS32("gCBtnDPosX", 0)+PosX_adjust;
        } else if (CVar_GetS32("gCBtnDPosType", 0) == 4) {//Hidden
            ItemIconPos[2][0] = -9999;
        }
    } else {
        ItemIconPos[2][0] = OTRGetRectDimensionFromRightEdge(ItemIconPos_ori[2][0]);
        ItemIconPos[2][1] = ItemIconPos_ori[2][1];
    }
    //C button Right
    if (CVar_GetS32("gCBtnRPosType", 0) != 0) {
        ItemIconPos[3][1] = CVar_GetS32("gCBtnRPosY", 0)+Y_Margins_CR+PosY_adjust;
        if (CVar_GetS32("gCBtnRPosType", 0) == 1) {//Anchor Left
            if (CVar_GetS32("gCBtnRUseMargins", 0) != 0) {X_Margins_CR = Left_HUD_Margin;};
            ItemIconPos[3][0] = OTRGetDimensionFromLeftEdge(CVar_GetS32("gCBtnRPosX", 0)+X_Margins_CR+PosX_adjust);
        } else if (CVar_GetS32("gCBtnRPosType", 0) == 2) {//Anchor Right
            if (CVar_GetS32("gCBtnRUseMargins", 0) != 0) {X_Margins_CR = Right_HUD_Margin;};
            ItemIconPos[3][0] = OTRGetDimensionFromRightEdge(CVar_GetS32("gCBtnRPosX", 0)+X_Margins_CR+PosX_adjust);
        } else if (CVar_GetS32("gCBtnRPosType", 0) == 3) {//Anchor None
            ItemIconPos[3][0] = CVar_GetS32("gCBtnRPosX", 0)+PosX_adjust;
        } else if (CVar_GetS32("gCBtnRPosType", 0) == 4) {//Hidden
            ItemIconPos[3][0] = -9999;
        }
    } else {
        ItemIconPos[3][0] = OTRGetRectDimensionFromRightEdge(ItemIconPos_ori[3][0]);
        ItemIconPos[3][1] = ItemIconPos_ori[3][1];
    }

    OPEN_DISPS(play->state.gfxCtx);

    i = gSaveContext.equips.buttonItems[button];

    if ((i == ITEM_STICK) || (i == ITEM_NUT) || (i == ITEM_BOMB) || (i == ITEM_BOW) ||
        ((i >= ITEM_BOW_ARROW_FIRE) && (i <= ITEM_BOW_ARROW_LIGHT)) || (i == ITEM_SLINGSHOT) || (i == ITEM_BOMBCHU) ||
        (i == ITEM_BEAN)) {

        if ((i >= ITEM_BOW_ARROW_FIRE) && (i <= ITEM_BOW_ARROW_LIGHT)) {
            i = ITEM_BOW;
        }

        ammo = AMMO(i);

        gDPPipeSync(OVERLAY_DISP++);

        if ((button == 0) && (gSaveContext.minigameState == 1)) {
            ammo = play->interfaceCtx.hbaAmmo;
        } else if ((button == 0) && (play->shootingGalleryStatus > 1)) {
            ammo = play->shootingGalleryStatus - 1;
        } else if ((button == 0) && (play->sceneNum == SCENE_BOWLING) && Flags_GetSwitch(play, 0x38)) {
            ammo = play->bombchuBowlingStatus;
            if (ammo < 0) {
                ammo = 0;
            }
        } else if (((i == ITEM_BOW) && (AMMO(i) == CUR_CAPACITY(UPG_QUIVER))) ||
                   ((i == ITEM_BOMB) && (AMMO(i) == CUR_CAPACITY(UPG_BOMB_BAG))) ||
                   ((i == ITEM_SLINGSHOT) && (AMMO(i) == CUR_CAPACITY(UPG_BULLET_BAG))) ||
                   ((i == ITEM_STICK) && (AMMO(i) == CUR_CAPACITY(UPG_STICKS))) ||
                   ((i == ITEM_NUT) && (AMMO(i) == CUR_CAPACITY(UPG_NUTS))) || ((i == ITEM_BOMBCHU) && (ammo == 50)) ||
                   ((i == ITEM_BEAN) && (ammo == 15))) {
            gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 120, 255, 0, alpha);
        }

        if (ammo == 0) {
            gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 100, 100, 100, alpha);
        }

        for (i = 0; ammo >= 10; i++) {
            ammo -= 10;
        }

        if (i != 0) {
            OVERLAY_DISP = Gfx_TextureIA8(OVERLAY_DISP, (u8*)_gAmmoDigit0Tex[i], 8, 8, 
                                      ItemIconPos[button][0], ItemIconPos[button][1], 8, 8, 1 << 10, 1 << 10);
        }

            OVERLAY_DISP = Gfx_TextureIA8(OVERLAY_DISP, (u8*)_gAmmoDigit0Tex[ammo], 8, 8, 
                                      ItemIconPos[button][0] + 6, ItemIconPos[button][1], 8, 8, 1 << 10, 1 << 10);

    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void Interface_DrawActionButton(PlayState* play, f32 x, f32 y) {
    InterfaceContext* interfaceCtx = &play->interfaceCtx;

    OPEN_DISPS(play->state.gfxCtx);

    Matrix_Translate(-137.0f + x, 97.0f - y, XREG(18) / 10.0f, MTXMODE_NEW);
    Matrix_Scale(1.0f, 1.0f, 1.0f, MTXMODE_APPLY);
    Matrix_RotateX(interfaceCtx->unk_1F4 / 10000.0f, MTXMODE_APPLY);

    gSPMatrix(OVERLAY_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_MODELVIEW | G_MTX_LOAD);
    gSPVertex(OVERLAY_DISP++, &interfaceCtx->actionVtx[0], 4, 0);

    gDPLoadTextureBlock(OVERLAY_DISP++, gButtonBackgroundTex, G_IM_FMT_IA, G_IM_SIZ_8b, 32, 32, 0,
                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                        G_TX_NOLOD);

    gSP1Quadrangle(OVERLAY_DISP++, 0, 2, 3, 1, 0);

    CLOSE_DISPS(play->state.gfxCtx);
}

void Interface_InitVertices(PlayState* play) {
    InterfaceContext* interfaceCtx = &play->interfaceCtx;
    s16 i;

    interfaceCtx->actionVtx = Graph_Alloc(play->state.gfxCtx, 8 * sizeof(Vtx));

    // clang-format off
    interfaceCtx->actionVtx[0].v.ob[0] =
    interfaceCtx->actionVtx[2].v.ob[0] = -15;
    interfaceCtx->actionVtx[1].v.ob[0] =
    interfaceCtx->actionVtx[3].v.ob[0] = interfaceCtx->actionVtx[0].v.ob[0] + 29;

    interfaceCtx->actionVtx[0].v.ob[1] =
    interfaceCtx->actionVtx[1].v.ob[1] = 15;
    interfaceCtx->actionVtx[2].v.ob[1] =
    interfaceCtx->actionVtx[3].v.ob[1] = interfaceCtx->actionVtx[0].v.ob[1] - 29;

    interfaceCtx->actionVtx[4].v.ob[0] =
    interfaceCtx->actionVtx[6].v.ob[0] = -((XREG(21) + 1) / 2);
    interfaceCtx->actionVtx[5].v.ob[0] =
    interfaceCtx->actionVtx[7].v.ob[0] = interfaceCtx->actionVtx[4].v.ob[0] + (XREG(21) + 1);

    interfaceCtx->actionVtx[4].v.ob[1] =
    interfaceCtx->actionVtx[5].v.ob[1] = XREG(28) / 2;
    interfaceCtx->actionVtx[6].v.ob[1] =
    interfaceCtx->actionVtx[7].v.ob[1] = interfaceCtx->actionVtx[4].v.ob[1] - XREG(28);

    for (i = 0; i < 8; i += 4) {
        interfaceCtx->actionVtx[i].v.ob[2] = interfaceCtx->actionVtx[i+1].v.ob[2] =
        interfaceCtx->actionVtx[i+2].v.ob[2] = interfaceCtx->actionVtx[i+3].v.ob[2] = 0;

        interfaceCtx->actionVtx[i].v.flag = interfaceCtx->actionVtx[i+1].v.flag =
        interfaceCtx->actionVtx[i+2].v.flag = interfaceCtx->actionVtx[i+3].v.flag = 0;

        interfaceCtx->actionVtx[i].v.tc[0] = interfaceCtx->actionVtx[i].v.tc[1] =
        interfaceCtx->actionVtx[i+1].v.tc[1] = interfaceCtx->actionVtx[i+2].v.tc[0] = -16;
        interfaceCtx->actionVtx[i+1].v.tc[0] = interfaceCtx->actionVtx[i+2].v.tc[1] =
        interfaceCtx->actionVtx[i+3].v.tc[0] = interfaceCtx->actionVtx[i+3].v.tc[1] = 1024 - 16;

        interfaceCtx->actionVtx[i].v.cn[0] = interfaceCtx->actionVtx[i+1].v.cn[0] =
        interfaceCtx->actionVtx[i+2].v.cn[0] = interfaceCtx->actionVtx[i+3].v.cn[0] =
        interfaceCtx->actionVtx[i].v.cn[1] = interfaceCtx->actionVtx[i+1].v.cn[1] =
        interfaceCtx->actionVtx[i+2].v.cn[1] = interfaceCtx->actionVtx[i+3].v.cn[1] =
        interfaceCtx->actionVtx[i].v.cn[2] = interfaceCtx->actionVtx[i+1].v.cn[2] =
        interfaceCtx->actionVtx[i+2].v.cn[2] = interfaceCtx->actionVtx[i+3].v.cn[2] = 255;

        interfaceCtx->actionVtx[i].v.cn[3] = interfaceCtx->actionVtx[i+1].v.cn[3] =
        interfaceCtx->actionVtx[i+2].v.cn[3] = interfaceCtx->actionVtx[i+3].v.cn[3] = 255;
    }

    interfaceCtx->actionVtx[5].v.tc[0] = interfaceCtx->actionVtx[7].v.tc[0] = 1536;
    interfaceCtx->actionVtx[6].v.tc[1] = interfaceCtx->actionVtx[7].v.tc[1] = 512;

    interfaceCtx->beatingHeartVtx = Graph_Alloc(play->state.gfxCtx, 4 * sizeof(Vtx));

    interfaceCtx->beatingHeartVtx[0].v.ob[0] = interfaceCtx->beatingHeartVtx[2].v.ob[0] = -8;
    interfaceCtx->beatingHeartVtx[1].v.ob[0] = interfaceCtx->beatingHeartVtx[3].v.ob[0] = 8;
    interfaceCtx->beatingHeartVtx[0].v.ob[1] = interfaceCtx->beatingHeartVtx[1].v.ob[1] = 8;
    interfaceCtx->beatingHeartVtx[2].v.ob[1] = interfaceCtx->beatingHeartVtx[3].v.ob[1] = -8;

    interfaceCtx->beatingHeartVtx[0].v.ob[2] = interfaceCtx->beatingHeartVtx[1].v.ob[2] =
    interfaceCtx->beatingHeartVtx[2].v.ob[2] = interfaceCtx->beatingHeartVtx[3].v.ob[2] = 0;

    interfaceCtx->beatingHeartVtx[0].v.flag = interfaceCtx->beatingHeartVtx[1].v.flag =
    interfaceCtx->beatingHeartVtx[2].v.flag = interfaceCtx->beatingHeartVtx[3].v.flag = 0;

    interfaceCtx->beatingHeartVtx[0].v.tc[0] = interfaceCtx->beatingHeartVtx[0].v.tc[1] =
    interfaceCtx->beatingHeartVtx[1].v.tc[1] = interfaceCtx->beatingHeartVtx[2].v.tc[0] = 0;
    interfaceCtx->beatingHeartVtx[1].v.tc[0] = interfaceCtx->beatingHeartVtx[2].v.tc[1] =
    interfaceCtx->beatingHeartVtx[3].v.tc[0] = interfaceCtx->beatingHeartVtx[3].v.tc[1] = 512;

    interfaceCtx->beatingHeartVtx[0].v.cn[0] = interfaceCtx->beatingHeartVtx[1].v.cn[0] =
    interfaceCtx->beatingHeartVtx[2].v.cn[0] = interfaceCtx->beatingHeartVtx[3].v.cn[0] =
    interfaceCtx->beatingHeartVtx[0].v.cn[1] = interfaceCtx->beatingHeartVtx[1].v.cn[1] =
    interfaceCtx->beatingHeartVtx[2].v.cn[1] = interfaceCtx->beatingHeartVtx[3].v.cn[1] =
    interfaceCtx->beatingHeartVtx[0].v.cn[2] = interfaceCtx->beatingHeartVtx[1].v.cn[2] =
    interfaceCtx->beatingHeartVtx[2].v.cn[2] = interfaceCtx->beatingHeartVtx[3].v.cn[2] =
    interfaceCtx->beatingHeartVtx[0].v.cn[3] = interfaceCtx->beatingHeartVtx[1].v.cn[3] =
    interfaceCtx->beatingHeartVtx[2].v.cn[3] = interfaceCtx->beatingHeartVtx[3].v.cn[3] = 255;
    // clang-format on
}

/*
void func_8008A8B8(PlayState* play, s32 topY, s32 bottomY, s32 leftX, s32 rightX) {
    InterfaceContext* interfaceCtx = &play->interfaceCtx;
    Vec3f eye;
    Vec3f lookAt;
    Vec3f up;

    eye.x = eye.y = eye.z = 0.0f;
    lookAt.x = lookAt.y = 0.0f;
    lookAt.z = -1.0f;
    up.x = up.z = 0.0f;
    up.y = 1.0f;

    func_800AA358(&interfaceCtx->view, &eye, &lookAt, &up);

    interfaceCtx->viewport.topY = topY;
    interfaceCtx->viewport.bottomY = bottomY;
    interfaceCtx->viewport.leftX = leftX;
    interfaceCtx->viewport.rightX = rightX;
    View_SetViewport(&interfaceCtx->view, &interfaceCtx->viewport);

    func_800AA460(&interfaceCtx->view, 60.0f, 10.0f, 60.0f);
    func_800AB560(&interfaceCtx->view);
}
*/

void func_8008A994(InterfaceContext* interfaceCtx) {
    SET_FULLSCREEN_VIEWPORT(&interfaceCtx->view);
    func_800AB2C4(&interfaceCtx->view);
}

const char* digitTextures[] =
{
    gCounterDigit0Tex, gCounterDigit1Tex, gCounterDigit2Tex, gCounterDigit3Tex,
    gCounterDigit4Tex, gCounterDigit5Tex, gCounterDigit6Tex, gCounterDigit7Tex, gCounterDigit8Tex,
    gCounterDigit9Tex, gCounterColonTex, gCounterDigit1Tex, gCounterDigit2Tex, gCounterDigit3Tex,
    gCounterDigit4Tex, gCounterDigit5Tex, gCounterDigit6Tex, gCounterDigit7Tex, gCounterDigit8Tex
};

void Interface_Draw(PlayState* play) {
    static s16 magicArrowEffectsR[] = { 255, 100, 255 };
    static s16 magicArrowEffectsG[] = { 0, 100, 255 };
    static s16 magicArrowEffectsB[] = { 0, 255, 100 };
    static s16 timerDigitLeftPos[] = { 16, 25, 34, 42, 51 };
    static s16 digitWidth[] = { 9, 9, 8, 9, 9 };
    // unused, most likely colors
    static s16 D_80125B1C[][3] = {
        { 0, 150, 0 }, { 100, 255, 0 }, { 255, 255, 255 }, { 0, 0, 0 }, { 255, 255, 255 },
    };
    static s16 rupeeDigitsFirst[] = { 1, 0, 0, 0 };
    static s16 rupeeDigitsCount[] = { 2, 3, 3, 3 };

    // courtesy of https://github.com/TestRunnerSRL/OoT-Randomizer/blob/Dev/ASM/c/hud_colors.c
    static s16 rupeeWalletColors[4][3] = {
        { 0xC8, 0xFF, 0x64 }, // Base Wallet (Green)
        { 0x82, 0x82, 0xFF }, // Adult's Wallet (Blue)
        { 0xFF, 0x64, 0x64 }, // Giant's Wallet (Red)
        { 0xFF, 0x5A, 0xFF }, // Tycoon's Wallet (Purple). Only used in rando shopsanity.
    };
    Color_RGB8 rColor_ori = { 200, 255, 100 };
    Color_RGB8 rColor;
    Color_RGB8 keyColor_ori = { 200, 230, 255 };
    Color_RGB8 keyColor;
    Color_RGB8 DPad_colors_ori = {255, 255, 255};
    Color_RGB8 DPad_colors = CVar_GetRGB("gCCDpadPrim", DPad_colors_ori);
    static s16 spoilingItemEntrances[] = { 0x01AD, 0x0153, 0x0153 };
    static f32 D_80125B54[] = { -40.0f, -35.0f }; // unused
    static s16 D_80125B5C[] = { 91, 91 };         // unused
    static s16 D_8015FFE0;
    static s16 D_8015FFE2;
    static s16 D_8015FFE4;
    static s16 D_8015FFE6;
    static s16 timerDigits[5];
    InterfaceContext* interfaceCtx = &play->interfaceCtx;
    PauseContext* pauseCtx = &play->pauseCtx;
    MessageContext* msgCtx = &play->msgCtx;
    Player* player = GET_PLAYER(play);
    s16 svar1;
    s16 svar2;
    s16 svar3;
    s16 svar4;
    s16 svar5;
    s16 svar6;
    bool fullUi = !CVar_GetS32("gMinimalUI", 0) || !R_MINIMAP_DISABLED || play->pauseCtx.state != 0;

    if (chaosEffectNoUI) {
        return;
    }

    OPEN_DISPS(play->state.gfxCtx);

    // Invalidate Do Action textures as they may have changed
    gSPInvalidateTexCache(OVERLAY_DISP++, interfaceCtx->doActionSegment);
    gSPInvalidateTexCache(OVERLAY_DISP++, interfaceCtx->doActionSegment + DO_ACTION_TEX_SIZE());

    gSPSegment(OVERLAY_DISP++, 0x02, interfaceCtx->parameterSegment);
    gSPSegment(OVERLAY_DISP++, 0x07, interfaceCtx->doActionSegment);
    gSPSegment(OVERLAY_DISP++, 0x08, interfaceCtx->iconItemSegment);
    gSPSegment(OVERLAY_DISP++, 0x0B, interfaceCtx->mapSegment);

    if (pauseCtx->debugState == 0) {
        Interface_InitVertices(play);
        func_8008A994(interfaceCtx);
        if (fullUi || gSaveContext.health != gSaveContext.healthCapacity) {
            HealthMeter_Draw(play);
        }

        func_80094520(play->state.gfxCtx);

        if (fullUi) {
            // Rupee Icon
            if (CVar_GetS32("gHudColors", 1) == 2) {
                //Custom Color is on but check if Dynamic Wallet is on.
                if (CVar_GetS32("gDynamicWalletIcon", 0) != 0) {
                    //if on let's use Dynamic Colors
                    rColor.r = rupeeWalletColors[CUR_UPG_VALUE(UPG_WALLET)][0];
                    rColor.g = rupeeWalletColors[CUR_UPG_VALUE(UPG_WALLET)][1];
                    rColor.b = rupeeWalletColors[CUR_UPG_VALUE(UPG_WALLET)][2];
                } else {
                    //else use our custom color
                    rColor = CVar_GetRGB("gCCRupeePrim", rColor_ori);
                }
            } else {
                //Custom colors is off so check if Dynamic Wallet is on.
                if (CVar_GetS32("gDynamicWalletIcon", 0) != 0) {
                    rColor.r = rupeeWalletColors[CUR_UPG_VALUE(UPG_WALLET)][0];
                    rColor.g = rupeeWalletColors[CUR_UPG_VALUE(UPG_WALLET)][1];
                    rColor.b = rupeeWalletColors[CUR_UPG_VALUE(UPG_WALLET)][2];
                } else {
                    rColor.r = rupeeWalletColors[0][0];
                    rColor.g = rupeeWalletColors[0][1];
                    rColor.b = rupeeWalletColors[0][2];
                }
            }
            if (CVar_GetS32("gHudColors", 1) == 2 && CVar_GetS32("gDynamicWalletIcon", 0) != 1) {
                //We reset this here so it match user color only if both HUD is on and Dynamic is off.
                gDPSetEnvColor(OVERLAY_DISP++, 0, 0, 0, 255); 
            }
            //Rupee icon & counter
            s16 X_Margins_RC;
            s16 Y_Margins_RC;
            if (CVar_GetS32("gRCUseMargins", 0) != 0) {
                if (CVar_GetS32("gRCPosType", 0) == 0) {X_Margins_RC = Left_HUD_Margin;};
                Y_Margins_RC = Bottom_HUD_Margin;
            } else {
                X_Margins_RC = 0;
                Y_Margins_RC = 0;
            }
            s16 PosX_RC_ori = OTRGetRectDimensionFromLeftEdge(26+X_Margins_RC);
            s16 PosY_RC_ori = 206+Y_Margins_RC;
            s16 PosX_RC;
            s16 PosY_RC;
            if (CVar_GetS32("gRCPosType", 0) != 0) {
                PosY_RC = CVar_GetS32("gRCPosY", 0)+Y_Margins_RC;
                if (CVar_GetS32("gRCPosType", 0) == 1) {//Anchor Left
                    if (CVar_GetS32("gRCUseMargins", 0) != 0) {X_Margins_RC = Left_HUD_Margin;};
                    PosX_RC = OTRGetDimensionFromLeftEdge(CVar_GetS32("gRCPosX", 0)+X_Margins_RC);
                } else if (CVar_GetS32("gRCPosType", 0) == 2) {//Anchor Right
                    if (CVar_GetS32("gRCUseMargins", 0) != 0) {X_Margins_RC = Right_HUD_Margin;};
                    PosX_RC = OTRGetDimensionFromRightEdge(CVar_GetS32("gRCPosX", 0)+X_Margins_RC);
                } else if (CVar_GetS32("gRCPosType", 0) == 3) {//Anchor None
                    PosX_RC = CVar_GetS32("gRCPosX", 0);
                } else if (CVar_GetS32("gRCPosType", 0) == 4) {//Hidden
                PosX_RC = -9999;
                }
            } else {
                PosY_RC = PosY_RC_ori;
                PosX_RC = PosX_RC_ori;
            }
            gDPSetPrimColor(OVERLAY_DISP++, 0, 0, rColor.r, rColor.g, rColor.b, interfaceCtx->magicAlpha);
            OVERLAY_DISP = Gfx_TextureIA8(OVERLAY_DISP, gRupeeCounterIconTex, 16, 16, PosX_RC, PosY_RC, 16, 16, 1 << 10, 1 << 10);

            switch (play->sceneNum) {
                case SCENE_BMORI1:
                case SCENE_HIDAN:
                case SCENE_MIZUSIN:
                case SCENE_JYASINZOU:
                case SCENE_HAKADAN:
                case SCENE_HAKADANCH:
                case SCENE_ICE_DOUKUTO:
                case SCENE_GANON:
                case SCENE_MEN:
                case SCENE_GERUDOWAY:
                case SCENE_GANONTIKA:
                case SCENE_GANON_SONOGO:
                case SCENE_GANONTIKA_SONOGO:
                case SCENE_TAKARAYA:
                    if (gSaveContext.inventory.dungeonKeys[gSaveContext.mapIndex] >= 0) {
                        s16 X_Margins_SKC;
                        s16 Y_Margins_SKC;
                        if (CVar_GetS32("gSKCUseMargins", 0) != 0) {
                            if (CVar_GetS32("gSKCPosType", 0) == 0) {X_Margins_SKC = Left_HUD_Margin;};
                            Y_Margins_SKC = Bottom_HUD_Margin;
                        } else {
                            X_Margins_SKC = 0;
                            Y_Margins_SKC = 0;
                        }
                        s16 PosX_SKC_ori = OTRGetRectDimensionFromLeftEdge(26+X_Margins_SKC);
                        s16 PosY_SKC_ori = 190+Y_Margins_SKC;
                        s16 PosX_SKC;
                        s16 PosY_SKC;
                        if (CVar_GetS32("gSKCPosType", 0) != 0) {
                            PosY_SKC = CVar_GetS32("gSKCPosY", 0)+Y_Margins_SKC;
                            if (CVar_GetS32("gSKCPosType", 0) == 1) {//Anchor Left
                                if (CVar_GetS32("gSKCUseMargins", 0) != 0) {X_Margins_SKC = Left_HUD_Margin;};
                                PosX_SKC = OTRGetDimensionFromLeftEdge(CVar_GetS32("gSKCPosX", 0)+X_Margins_SKC);
                            } else if (CVar_GetS32("gSKCPosType", 0) == 2) {//Anchor Right
                                if (CVar_GetS32("gSKCUseMargins", 0) != 0) {X_Margins_SKC = Right_HUD_Margin;};
                                PosX_SKC = OTRGetDimensionFromRightEdge(CVar_GetS32("gSKCPosX", 0)+X_Margins_SKC);
                            } else if (CVar_GetS32("gSKCPosType", 0) == 3) {//Anchor None
                                PosX_SKC = CVar_GetS32("gSKCPosX", 0);
                            } else if (CVar_GetS32("gSKCPosType", 0) == 4) {//Hidden
                            PosX_SKC = -9999;
                            }
                        } else {
                            PosY_SKC = PosY_SKC_ori;
                            PosX_SKC = PosX_SKC_ori;
                        }
                        // Small Key Icon
                        gDPPipeSync(OVERLAY_DISP++);

                        if (CVar_GetS32("gHudColors", 1) == 2) {
                            gDPSetPrimColor(OVERLAY_DISP++, 0, 0, CVar_GetRGB("gCCKeysPrim", keyColor_ori).r,CVar_GetRGB("gCCKeysPrim", keyColor_ori).g,CVar_GetRGB("gCCKeysPrim", keyColor_ori).b, interfaceCtx->magicAlpha);
                            gDPSetEnvColor(OVERLAY_DISP++, 0, 0, 0, 255); //We reset this here so it match user color :)
                        } else {
                            gDPSetPrimColor(OVERLAY_DISP++, 0, 0, keyColor_ori.r, keyColor_ori.g, keyColor_ori.b, interfaceCtx->magicAlpha);
                            gDPSetEnvColor(OVERLAY_DISP++, 0, 0, 20, 255);
                        }
                        OVERLAY_DISP = Gfx_TextureIA8(OVERLAY_DISP, gSmallKeyCounterIconTex, 16, 16, PosX_SKC, PosY_SKC, 16, 16,
                                                      1 << 10, 1 << 10);

                        // Small Key Counter
                        gDPPipeSync(OVERLAY_DISP++);
                        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, interfaceCtx->magicAlpha);
                        gDPSetCombineLERP(OVERLAY_DISP++, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, PRIMITIVE,
                                          TEXEL0, 0, PRIMITIVE, 0);

                        interfaceCtx->counterDigits[2] = 0;
                        interfaceCtx->counterDigits[3] = gSaveContext.inventory.dungeonKeys[gSaveContext.mapIndex];

                        while (interfaceCtx->counterDigits[3] >= 10) {
                            interfaceCtx->counterDigits[2]++;
                            interfaceCtx->counterDigits[3] -= 10;
                        }

                        if (interfaceCtx->counterDigits[2] != 0) {
                            OVERLAY_DISP = Gfx_TextureI8(OVERLAY_DISP, ((u8*)((u8*)digitTextures[interfaceCtx->counterDigits[2]])), 8, 16, PosX_SKC+8, PosY_SKC, 8, 16, 1 << 10, 1 << 10);
                        }

                        OVERLAY_DISP = Gfx_TextureI8(OVERLAY_DISP, ((u8*)digitTextures[interfaceCtx->counterDigits[3]]), 8, 16, PosX_SKC+16, PosY_SKC, 8, 16, 1 << 10, 1 << 10);
                    }
                    break;
                default:
                    break;
            }

            // Rupee Counter
            gDPPipeSync(OVERLAY_DISP++);

            if (gSaveContext.rupees == CUR_CAPACITY(UPG_WALLET)) {
                gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 120, 255, 0, interfaceCtx->magicAlpha);
            } else if (gSaveContext.rupees != 0) {
                gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, interfaceCtx->magicAlpha);
            } else {
                gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 100, 100, 100, interfaceCtx->magicAlpha);
            }

            gDPSetCombineLERP(OVERLAY_DISP++, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, PRIMITIVE, TEXEL0, 0,
                              PRIMITIVE, 0);

            interfaceCtx->counterDigits[0] = interfaceCtx->counterDigits[1] = 0;
            interfaceCtx->counterDigits[2] = gSaveContext.rupees;

            if ((interfaceCtx->counterDigits[2] > 9999) || (interfaceCtx->counterDigits[2] < 0)) {
                interfaceCtx->counterDigits[2] &= 0xDDD;
            }

            while (interfaceCtx->counterDigits[2] >= 100) {
                interfaceCtx->counterDigits[0]++;
                interfaceCtx->counterDigits[2] -= 100;
            }

            while (interfaceCtx->counterDigits[2] >= 10) {
                interfaceCtx->counterDigits[1]++;
                interfaceCtx->counterDigits[2] -= 10;
            }

            svar2 = rupeeDigitsFirst[CUR_UPG_VALUE(UPG_WALLET)];
            svar5 = rupeeDigitsCount[CUR_UPG_VALUE(UPG_WALLET)];

            for (svar1 = 0, svar3 = 16; svar1 < svar5; svar1++, svar2++, svar3 += 8) {
                OVERLAY_DISP =
                    Gfx_TextureI8(OVERLAY_DISP, ((u8*)digitTextures[interfaceCtx->counterDigits[svar2]]), 8, 16,
                        PosX_RC+svar3, PosY_RC, 8, 16, 1 << 10, 1 << 10);
            }
        }
        else {
            // Make sure item counts have black backgrounds
            gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 0, 0, 0, interfaceCtx->magicAlpha);
            gDPSetEnvColor(OVERLAY_DISP++, 0, 0, 0, 0);
        }

        if (CVar_GetS32("gDrawLineupTick", 0)) {
            Interface_DrawLineupTick(play);
        }

        if (fullUi || gSaveContext.unk_13F0 > 0) {
            Interface_DrawMagicBar(play);
        }

        Minimap_Draw(play);

        if ((R_PAUSE_MENU_MODE != 2) && (R_PAUSE_MENU_MODE != 3)) {
            func_8002C124(&play->actorCtx.targetCtx, play); // Draw Z-Target
        }

        func_80094520(play->state.gfxCtx);

        if (fullUi) {
            Interface_DrawItemButtons(play);
        }

        gDPPipeSync(OVERLAY_DISP++);
        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, interfaceCtx->bAlpha);
        gDPSetCombineMode(OVERLAY_DISP++, G_CC_MODULATERGBA_PRIM, G_CC_MODULATERGBA_PRIM);

        if (!(interfaceCtx->unk_1FA)) {
            // B Button Icon & Ammo Count
            if (gSaveContext.equips.buttonItems[0] != ITEM_NONE)
            {
                if (fullUi) {
                    Interface_DrawItemIconTexture(play, gItemIcons[gSaveContext.equips.buttonItems[0]], 0);
                }

                if ((player->stateFlags1 & 0x00800000) || (play->shootingGalleryStatus > 1) ||
                    ((play->sceneNum == SCENE_BOWLING) && Flags_GetSwitch(play, 0x38))) {

                    if (!fullUi) {
                        Interface_DrawItemIconTexture(play, gItemIcons[gSaveContext.equips.buttonItems[0]], 0);
                    }

                    gDPPipeSync(OVERLAY_DISP++);
                    gDPSetCombineLERP(OVERLAY_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE,
                                      0, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
                    Interface_DrawAmmoCount(play, 0, interfaceCtx->bAlpha);
                }
            }
        } else {
            // B Button Do Action Label
            s16 PosX_adjust;
            s16 PosY_adjust;
            if (gSaveContext.language == 2) {
                PosX_adjust = -12;
                PosY_adjust = 5;
            } else if (gSaveContext.language == 1) { //ger
                PosY_adjust = 6;
                PosX_adjust = -9;
            } else {
                PosY_adjust = 6;
                PosX_adjust = -10;
            }
            
            s16 BbtnPosX;
            s16 BbtnPosY;
            s16 X_Margins_BtnB_label;
            s16 Y_Margins_BtnB_label;
            if (CVar_GetS32("gBBtnUseMargins", 0) != 0) {
                if (CVar_GetS32("gBBtnPosType", 0) == 0) {X_Margins_BtnB_label = Right_HUD_Margin;};
                Y_Margins_BtnB_label = (Top_HUD_Margin*-1);
            } else {
                X_Margins_BtnB_label = 0;
                Y_Margins_BtnB_label = 0;
            }
            if (CVar_GetS32("gBBtnPosType", 0) != 0) {
                BbtnPosY = CVar_GetS32("gBBtnPosY", 0)+Y_Margins_BtnB_label+PosY_adjust;
                if (CVar_GetS32("gBBtnPosType", 0) == 1) {//Anchor Left
                    if (CVar_GetS32("gBBtnUseMargins", 0) != 0) {X_Margins_BtnB_label = Left_HUD_Margin;};
                    BbtnPosX = OTRGetDimensionFromLeftEdge(CVar_GetS32("gBBtnPosX", 0)+X_Margins_BtnB_label+PosX_adjust);
                } else if (CVar_GetS32("gBBtnPosType", 0) == 2) {//Anchor Right
                    if (CVar_GetS32("gBBtnUseMargins", 0) != 0) {X_Margins_BtnB_label = Right_HUD_Margin;};
                    BbtnPosX = OTRGetDimensionFromRightEdge(CVar_GetS32("gBBtnPosX", 0)+X_Margins_BtnB_label+PosX_adjust);
                } else if (CVar_GetS32("gBBtnPosType", 0) == 3) {//Anchor None
                    BbtnPosX = CVar_GetS32("gBBtnPosX", 0)+PosX_adjust;
                } else if (CVar_GetS32("gBBtnPosType", 0) == 4) {//Hidden
                    BbtnPosX = -9999;
                }
            } else {
                BbtnPosX = OTRGetRectDimensionFromRightEdge(R_B_LABEL_X(gSaveContext.language)+X_Margins_BtnB_label);
                BbtnPosY = R_B_LABEL_Y(gSaveContext.language)+Y_Margins_BtnB_label;
            }
            gDPPipeSync(OVERLAY_DISP++);
            gDPSetCombineLERP(OVERLAY_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0,
                              PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
            gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, interfaceCtx->bAlpha);

            gDPLoadTextureBlock_4b(OVERLAY_DISP++, interfaceCtx->doActionSegment + DO_ACTION_TEX_SIZE(), G_IM_FMT_IA,
                                   DO_ACTION_TEX_WIDTH(), DO_ACTION_TEX_HEIGHT(), 0, G_TX_NOMIRROR | G_TX_WRAP,
                                   G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

            R_B_LABEL_DD = (1 << 10) / (WREG(37 + gSaveContext.language) / 100.0f);
            gSPWideTextureRectangle(OVERLAY_DISP++, BbtnPosX << 2, BbtnPosY << 2,
                                (BbtnPosX + DO_ACTION_TEX_WIDTH()) << 2,
                                (BbtnPosY + DO_ACTION_TEX_HEIGHT()) << 2, G_TX_RENDERTILE, 0, 0,
                                R_B_LABEL_DD, R_B_LABEL_DD);
        }

        gDPPipeSync(OVERLAY_DISP++);

        // C-Left Button Icon & Ammo Count
        if (gSaveContext.equips.buttonItems[1] < 0xF0) {
            gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, interfaceCtx->cLeftAlpha);
            gDPSetCombineMode(OVERLAY_DISP++, G_CC_MODULATERGBA_PRIM, G_CC_MODULATERGBA_PRIM);
            Interface_DrawItemIconTexture(play, gItemIcons[gSaveContext.equips.buttonItems[1]], 1);
            gDPPipeSync(OVERLAY_DISP++);
            gDPSetCombineLERP(OVERLAY_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0,
                              PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
            Interface_DrawAmmoCount(play, 1, interfaceCtx->cLeftAlpha);
        }

        gDPPipeSync(OVERLAY_DISP++);

        // C-Down Button Icon & Ammo Count
        if (gSaveContext.equips.buttonItems[2] < 0xF0) {
            gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, interfaceCtx->cDownAlpha);
            gDPSetCombineMode(OVERLAY_DISP++, G_CC_MODULATERGBA_PRIM, G_CC_MODULATERGBA_PRIM);
            Interface_DrawItemIconTexture(play, gItemIcons[gSaveContext.equips.buttonItems[2]], 2);
            gDPPipeSync(OVERLAY_DISP++);
            gDPSetCombineLERP(OVERLAY_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0,
                              PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
            Interface_DrawAmmoCount(play, 2, interfaceCtx->cDownAlpha);
        }

        gDPPipeSync(OVERLAY_DISP++);

        // C-Right Button Icon & Ammo Count
        if (gSaveContext.equips.buttonItems[3] < 0xF0) {
            gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, interfaceCtx->cRightAlpha);
            gDPSetCombineMode(OVERLAY_DISP++, G_CC_MODULATERGBA_PRIM, G_CC_MODULATERGBA_PRIM);
            Interface_DrawItemIconTexture(play, gItemIcons[gSaveContext.equips.buttonItems[3]], 3);
            gDPPipeSync(OVERLAY_DISP++);
            gDPSetCombineLERP(OVERLAY_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0,
                              PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
            Interface_DrawAmmoCount(play, 3, interfaceCtx->cRightAlpha);
        }

        if (CVar_GetS32("gDpadEquips", 0) != 0) {
            // DPad is only greyed-out when all 4 DPad directions are too
            uint16_t dpadAlpha =
                MAX(MAX(MAX(interfaceCtx->dpadUpAlpha, interfaceCtx->dpadDownAlpha), interfaceCtx->dpadLeftAlpha),
                    interfaceCtx->dpadRightAlpha);

            // Draw DPad
            s16 DpadPosX;
            s16 DpadPosY;
            s16 X_Margins_Dpad;
            s16 Y_Margins_Dpad;
            if (CVar_GetS32("gDPadUseMargins", 0) != 0) {
                if (CVar_GetS32("gDPadPosType", 0) == 0) {X_Margins_Dpad = Right_HUD_Margin;};
                Y_Margins_Dpad = (Top_HUD_Margin*-1);
            } else {
                Y_Margins_Dpad = 0;
                X_Margins_Dpad = 0;
            }
            if (CVar_GetS32("gDPadPosType", 0) != 0) {
                DpadPosY = CVar_GetS32("gDPadPosY", 0)+Y_Margins_Dpad;
                if (CVar_GetS32("gDPadPosType", 0) == 1) {//Anchor Left
                    if (CVar_GetS32("gDPadUseMargins", 0) != 0) {X_Margins_Dpad = Left_HUD_Margin;};
                    DpadPosX = OTRGetDimensionFromLeftEdge(CVar_GetS32("gDPadPosX", 0)+X_Margins_Dpad);
                } else if (CVar_GetS32("gDPadPosType", 0) == 2) {//Anchor Right
                    if (CVar_GetS32("gDPadUseMargins", 0) != 0) {X_Margins_Dpad = Right_HUD_Margin;};
                    DpadPosX = OTRGetDimensionFromRightEdge(CVar_GetS32("gDPadPosX", 0)+X_Margins_Dpad);
                } else if (CVar_GetS32("gDPadPosType", 0) == 3) {//Anchor None
                    DpadPosX = CVar_GetS32("gDPadPosX", 0);
                } else if (CVar_GetS32("gDPadPosType", 0) == 4) {//Hidden
                    DpadPosX = -9999;
                }
            } else {
                DpadPosX = OTRGetRectDimensionFromRightEdge(DPAD_X+X_Margins_Dpad);
                DpadPosY = DPAD_Y+Y_Margins_Dpad;
            }

            gDPSetCombineMode(OVERLAY_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);

            if (CVar_GetS32("gHudColors", 1) == 2) {
                gDPSetPrimColor(OVERLAY_DISP++, 0, 0, DPad_colors.r,DPad_colors.g,DPad_colors.b, dpadAlpha);
            } else {
                gDPSetPrimColor(OVERLAY_DISP++, 0, 0, DPad_colors_ori.r,DPad_colors_ori.g,DPad_colors_ori.b, dpadAlpha);
            }
            if (fullUi) {
                gDPLoadTextureBlock(OVERLAY_DISP++, ResourceMgr_LoadFileRaw("assets/textures/parameter_static/gDPad"),
                                    G_IM_FMT_IA, G_IM_SIZ_16b, 32, 32, 0, G_TX_NOMIRROR | G_TX_WRAP,
                                    G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
                gSPWideTextureRectangle(OVERLAY_DISP++, DpadPosX << 2, DpadPosY << 2,
                                        (DpadPosX + 32) << 2, (DpadPosY + 32) << 2,
                                        G_TX_RENDERTILE, 0, 0, (1 << 10), (1 << 10));
            }

            // DPad-Up Button Icon & Ammo Count
            if (gSaveContext.equips.buttonItems[4] < 0xF0) {
                gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, interfaceCtx->dpadUpAlpha);
                gDPSetCombineMode(OVERLAY_DISP++, G_CC_MODULATERGBA_PRIM, G_CC_MODULATERGBA_PRIM);
                Interface_DrawItemIconTexture(play, gItemIcons[gSaveContext.equips.buttonItems[4]], 4);
                gDPPipeSync(OVERLAY_DISP++);
                gDPSetCombineLERP(OVERLAY_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0,
                                  PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
                Interface_DrawAmmoCount(play, 4, interfaceCtx->dpadUpAlpha);
            }

            // DPad-Down Button Icon & Ammo Count
            if (gSaveContext.equips.buttonItems[5] < 0xF0) {
                gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, interfaceCtx->dpadDownAlpha);
                gDPSetCombineMode(OVERLAY_DISP++, G_CC_MODULATERGBA_PRIM, G_CC_MODULATERGBA_PRIM);
                Interface_DrawItemIconTexture(play, gItemIcons[gSaveContext.equips.buttonItems[5]], 5);
                gDPPipeSync(OVERLAY_DISP++);
                gDPSetCombineLERP(OVERLAY_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0,
                                  PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
                Interface_DrawAmmoCount(play, 5, interfaceCtx->dpadDownAlpha);
            }

            // DPad-Left Button Icon & Ammo Count
            if (gSaveContext.equips.buttonItems[6] < 0xF0) {
                gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, interfaceCtx->dpadLeftAlpha);
                gDPSetCombineMode(OVERLAY_DISP++, G_CC_MODULATERGBA_PRIM, G_CC_MODULATERGBA_PRIM);
                Interface_DrawItemIconTexture(play, gItemIcons[gSaveContext.equips.buttonItems[6]], 6);
                gDPPipeSync(OVERLAY_DISP++);
                gDPSetCombineLERP(OVERLAY_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0,
                                  PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
                Interface_DrawAmmoCount(play, 6, interfaceCtx->dpadLeftAlpha);
            }

            // DPad-Right Button Icon & Ammo Count
            if (gSaveContext.equips.buttonItems[7] < 0xF0) {
                gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, interfaceCtx->dpadRightAlpha);
                gDPSetCombineMode(OVERLAY_DISP++, G_CC_MODULATERGBA_PRIM, G_CC_MODULATERGBA_PRIM);
                Interface_DrawItemIconTexture(play, gItemIcons[gSaveContext.equips.buttonItems[7]], 7);
                gDPPipeSync(OVERLAY_DISP++);
                gDPSetCombineLERP(OVERLAY_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0,
                                  PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
                Interface_DrawAmmoCount(play, 7, interfaceCtx->dpadRightAlpha);
            }
        }

        // A Button
        func_80094A14(play->state.gfxCtx);
        const Color_RGB8 A_Button_Colors = {R_A_BTN_COLOR(0), R_A_BTN_COLOR(1), R_A_BTN_COLOR(2)};
        s16 X_Margins_BtnA;
        s16 Y_Margins_BtnA;
        if (CVar_GetS32("gABtnUseMargins", 0) != 0) {
            X_Margins_BtnA = Right_HUD_Margin;
            Y_Margins_BtnA = (Top_HUD_Margin*-1);
        } else {
            X_Margins_BtnA = 0;
            Y_Margins_BtnA = 0;
        }
        s16 PosX_BtnA_ori = OTRGetDimensionFromRightEdge(R_A_BTN_X+X_Margins_BtnA);
        s16 PosY_BtnA_ori = R_A_BTN_Y+Y_Margins_BtnA;
        const f32 rAIconX_ori = OTRGetDimensionFromRightEdge(R_A_ICON_X+X_Margins_BtnA);
        const f32 rAIconY_ori = 98.0f - (R_A_ICON_Y+Y_Margins_BtnA);
        s16 PosX_BtnA;
        s16 PosY_BtnA;
        s16 rAIconX;
        s16 rAIconY;
        if (CVar_GetS32("gABtnPosType", 0) != 0) {
            PosY_BtnA = CVar_GetS32("gABtnPosY", 0)+Y_Margins_BtnA;
            rAIconY = 98.0f - PosY_BtnA;
            if (CVar_GetS32("gABtnPosType", 0) == 1) {//Anchor Left
                if (CVar_GetS32("gABtnUseMargins", 0) != 0) {X_Margins_BtnA = Left_HUD_Margin;};
                PosX_BtnA = OTRGetDimensionFromLeftEdge(CVar_GetS32("gABtnPosX", 0)+X_Margins_BtnA);
                rAIconX = OTRGetDimensionFromLeftEdge(CVar_GetS32("gABtnPosX", 0)+X_Margins_BtnA);
            } else if (CVar_GetS32("gABtnPosType", 0) == 2) {//Anchor Right
                if (CVar_GetS32("gABtnUseMargins", 0) != 0) {X_Margins_BtnA = Right_HUD_Margin;};
                PosX_BtnA = OTRGetDimensionFromRightEdge(CVar_GetS32("gABtnPosX", 0)+X_Margins_BtnA);
                rAIconX= OTRGetDimensionFromRightEdge(CVar_GetS32("gABtnPosX", 0)+X_Margins_BtnA);
            } else if (CVar_GetS32("gABtnPosType", 0) == 3) {//Anchor None
                PosX_BtnA = CVar_GetS32("gABtnPosX", 0);
                rAIconX = CVar_GetS32("gABtnPosX", 0);
            } else if (CVar_GetS32("gABtnPosType", 0) == 4) {//Hidden
                PosX_BtnA = -9999;
                rAIconX = -9999;
            }
        } else {
            PosY_BtnA = PosY_BtnA_ori;
            PosX_BtnA = PosX_BtnA_ori;
            rAIconY = rAIconY_ori;
            rAIconX = rAIconX_ori;
        }
        gSPClearGeometryMode(OVERLAY_DISP++, G_CULL_BOTH);
        gDPSetCombineMode(OVERLAY_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
        if (CVar_GetS32("gHudColors", 1) == 0) {
            gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 90, 90, 255, interfaceCtx->aAlpha);
        } else if (CVar_GetS32("gHudColors", 1) == 1) {
            gDPSetPrimColor(OVERLAY_DISP++, 0, 0, A_Button_Colors.r, A_Button_Colors.g, A_Button_Colors.b, interfaceCtx->aAlpha);
        } else if (CVar_GetS32("gHudColors", 1) == 2) {
            gDPSetPrimColor(OVERLAY_DISP++, 0, 0, CVar_GetRGB("gCCABtnPrim", A_Button_Colors).r,CVar_GetRGB("gCCABtnPrim", A_Button_Colors).g,CVar_GetRGB("gCCABtnPrim", A_Button_Colors).b, interfaceCtx->aAlpha);
        }
        if (fullUi) {
            Interface_DrawActionButton(play, PosX_BtnA, PosY_BtnA);
        }
        gDPPipeSync(OVERLAY_DISP++);
        gSPSetGeometryMode(OVERLAY_DISP++, G_CULL_BACK);
        gDPSetCombineLERP(OVERLAY_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0,
                          PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, interfaceCtx->aAlpha);
        gDPSetEnvColor(OVERLAY_DISP++, 0, 0, 0, 0);
        Matrix_Translate(-138.0f + rAIconX, rAIconY, WREG(46 + gSaveContext.language) / 10.0f, MTXMODE_NEW);
        Matrix_Scale(1.0f, 1.0f, 1.0f, MTXMODE_APPLY);
        Matrix_RotateX(interfaceCtx->unk_1F4 / 10000.0f, MTXMODE_APPLY);
        gSPMatrix(OVERLAY_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_MODELVIEW | G_MTX_LOAD);
        gSPVertex(OVERLAY_DISP++, &interfaceCtx->actionVtx[4], 4, 0);

        if ((interfaceCtx->unk_1EC < 2) || (interfaceCtx->unk_1EC == 3)) {
            Interface_DrawActionLabel(play->state.gfxCtx, interfaceCtx->doActionSegment);
        } else {
            Interface_DrawActionLabel(play->state.gfxCtx, interfaceCtx->doActionSegment + DO_ACTION_TEX_SIZE());
        }

        gDPPipeSync(OVERLAY_DISP++);

        func_8008A994(interfaceCtx);
        svar3 = 16;

        if ((pauseCtx->state == 6) && (pauseCtx->unk_1E4 == 3)) {
            // Inventory Equip Effects
            gSPSegment(OVERLAY_DISP++, 0x08, pauseCtx->iconItemSegment);
            func_80094A14(play->state.gfxCtx);
            gDPSetCombineMode(OVERLAY_DISP++, G_CC_MODULATERGBA_PRIM, G_CC_MODULATERGBA_PRIM);
            gSPMatrix(OVERLAY_DISP++, &gMtxClear, G_MTX_MODELVIEW | G_MTX_LOAD);

            pauseCtx->cursorVtx[svar3].v.ob[0] = pauseCtx->cursorVtx[18].v.ob[0] = svar2 = pauseCtx->equipAnimX / 10;
            pauseCtx->cursorVtx[17].v.ob[0] = pauseCtx->cursorVtx[19].v.ob[0] = svar2 =
                pauseCtx->cursorVtx[svar3].v.ob[0] + WREG(90) / 10;
            pauseCtx->cursorVtx[svar3].v.ob[1] = pauseCtx->cursorVtx[17].v.ob[1] = svar2 = pauseCtx->equipAnimY / 10;
            pauseCtx->cursorVtx[18].v.ob[1] = pauseCtx->cursorVtx[19].v.ob[1] = svar2 =
                pauseCtx->cursorVtx[svar3].v.ob[1] - WREG(90) / 10;

            if (pauseCtx->equipTargetItem < 0xBF) {
                // Normal Equip (icon goes from the inventory slot to the C button when equipping it)
                gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, pauseCtx->equipAnimAlpha);
                gSPVertex(OVERLAY_DISP++, &pauseCtx->cursorVtx[16], 4, 0);

                gDPLoadTextureBlock(OVERLAY_DISP++, gItemIcons[pauseCtx->equipTargetItem], G_IM_FMT_RGBA, G_IM_SIZ_32b,
                                    32, 32, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK,
                                    G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
            } else {
                // Magic Arrow Equip Effect
                svar1 = pauseCtx->equipTargetItem - 0xBF;
                gDPSetPrimColor(OVERLAY_DISP++, 0, 0, magicArrowEffectsR[svar1], magicArrowEffectsG[svar1],
                                magicArrowEffectsB[svar1], pauseCtx->equipAnimAlpha);

                if ((pauseCtx->equipAnimAlpha > 0) && (pauseCtx->equipAnimAlpha < 255)) {
                    svar1 = (pauseCtx->equipAnimAlpha / 8) / 2;
                    pauseCtx->cursorVtx[16].v.ob[0] = pauseCtx->cursorVtx[18].v.ob[0] = svar2 =
                        pauseCtx->cursorVtx[16].v.ob[0] - svar1;
                    pauseCtx->cursorVtx[17].v.ob[0] = pauseCtx->cursorVtx[19].v.ob[0] = svar2 =
                        pauseCtx->cursorVtx[16].v.ob[0] + svar1 * 2 + 32;
                    pauseCtx->cursorVtx[16].v.ob[1] = pauseCtx->cursorVtx[17].v.ob[1] = svar2 =
                        pauseCtx->cursorVtx[16].v.ob[1] + svar1;
                    pauseCtx->cursorVtx[18].v.ob[1] = pauseCtx->cursorVtx[19].v.ob[1] = svar2 =
                        pauseCtx->cursorVtx[16].v.ob[1] - svar1 * 2 - 32;
                }

                gSPVertex(OVERLAY_DISP++, &pauseCtx->cursorVtx[16], 4, 0);
                gDPLoadTextureBlock(OVERLAY_DISP++, gMagicArrowEquipEffectTex, G_IM_FMT_IA, G_IM_SIZ_8b, 32, 32, 0,
                                    G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                                    G_TX_NOLOD, G_TX_NOLOD);
            }

            gSP1Quadrangle(OVERLAY_DISP++, 0, 2, 3, 1, 0);
        }

        func_80094520(play->state.gfxCtx);

        if ((play->pauseCtx.state == 0) && (play->pauseCtx.debugState == 0)) {
            if (gSaveContext.minigameState != 1) {
                // Carrots rendering if the action corresponds to riding a horse
                if (interfaceCtx->unk_1EE == 8 && !CVar_GetS32("gInfiniteEpona", 0)) {
                    // Load Carrot Icon
                    gDPLoadTextureBlock(OVERLAY_DISP++, gCarrotIconTex, G_IM_FMT_RGBA, G_IM_SIZ_32b, 16, 16, 0,
                                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                                        G_TX_NOLOD, G_TX_NOLOD);

                    // Draw 6 carrots
                    s16 CarrotsPosX = ZREG(14);
                    s16 CarrotsPosY = ZREG(15);
                    s16 CarrotsMargins_X = 0;
                    if (CVar_GetS32("gCarrotsPosType", 0) != 0) {
                        CarrotsPosY = CVar_GetS32("gCarrotsPosY", 0);
                        if (CVar_GetS32("gCarrotsPosType", 0) == 1) {//Anchor Left
                            if (CVar_GetS32("gCarrotsUseMargins", 0) != 0) {CarrotsMargins_X = Left_HUD_Margin;};
                            CarrotsPosX = OTRGetDimensionFromLeftEdge(CVar_GetS32("gCarrotsPosX", 0)+CarrotsMargins_X);            
                        } else if (CVar_GetS32("gCarrotsPosType", 0) == 2) {//Anchor Right
                            if (CVar_GetS32("gCarrotsUseMargins", 0) != 0) {CarrotsMargins_X = Right_HUD_Margin;};
                            CarrotsPosX = OTRGetDimensionFromRightEdge(CVar_GetS32("gCarrotsPosX", 0)+CarrotsMargins_X);
                        } else if (CVar_GetS32("gCarrotsPosType", 0) == 3) {//Anchor None
                            CarrotsPosX = CVar_GetS32("gCarrotsPosX", 0);
                        } else if (CVar_GetS32("gCarrotsPosType", 0) == 4) {//Hidden
                            CarrotsPosX = -9999;
                        }
                    }
                    for (svar1 = 1, svar5 = CarrotsPosX; svar1 < 7; svar1++, svar5 += 16) {
                        // Carrot Color (based on availability)
                        if ((interfaceCtx->numHorseBoosts == 0) || (interfaceCtx->numHorseBoosts < svar1)) {
                            gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 0, 150, 255, interfaceCtx->aAlpha);
                        } else {
                            gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, interfaceCtx->aAlpha);
                        }

                        gSPWideTextureRectangle(OVERLAY_DISP++, svar5 << 2, CarrotsPosY << 2, (svar5 + 16) << 2,
                                            (CarrotsPosY + 16) << 2, G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);
                    }
                }
            } else {
                // Score for the Horseback Archery
                s32 X_Margins_Archery;
                if (CVar_GetS32("gASUseMargins", 0) != 0) {
                    if (CVar_GetS32("gASPosType", 0) == 0) {X_Margins_Archery = Right_HUD_Margin;};
                } else {
                    X_Margins_Archery = 0;
                }
                s16 ArcheryPos_Y = ZREG(15);
                s16 ArcheryPos_X = OTRGetRectDimensionFromRightEdge(WREG(32)+X_Margins_Archery);

                if (CVar_GetS32("gASPosType", 0) != 0) {
                    ArcheryPos_Y = CVar_GetS32("gASPosY", 0);
                    if (CVar_GetS32("gASPosType", 0) == 1) {//Anchor Left
                        if (CVar_GetS32("gASUseMargins", 0) != 0) {X_Margins_Archery = Left_HUD_Margin;};
                        ArcheryPos_X = OTRGetRectDimensionFromLeftEdge(CVar_GetS32("gASPosX", 0)+X_Margins_Archery);
                    } else if (CVar_GetS32("gASPosType", 0) == 2) {//Anchor Right
                        if (CVar_GetS32("gASUseMargins", 0) != 0) {X_Margins_Archery = Right_HUD_Margin;};
                        ArcheryPos_X = OTRGetRectDimensionFromRightEdge(CVar_GetS32("gASPosX", 0)+X_Margins_Archery);
                    } else if (CVar_GetS32("gASPosType", 0) == 3) {//Anchor None
                        ArcheryPos_X = CVar_GetS32("gASPosX", 0)+204+X_Margins_Archery;
                    } else if (CVar_GetS32("gASPosType", 0) == 4) {//Hidden
                        ArcheryPos_X = -9999;
                    }
                }

                gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, interfaceCtx->bAlpha);

                // Target Icon
                gDPLoadTextureBlock(OVERLAY_DISP++, gArcheryScoreIconTex, G_IM_FMT_RGBA, G_IM_SIZ_16b, 24, 16, 0,
                                    G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                                    G_TX_NOLOD, G_TX_NOLOD);

                gSPWideTextureRectangle(OVERLAY_DISP++, (ArcheryPos_X + 28) << 2, ArcheryPos_Y << 2, (ArcheryPos_X + 52) << 2,
                                    (ArcheryPos_Y + 16) << 2, G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);

                // Score Counter
                gDPPipeSync(OVERLAY_DISP++);
                gDPSetCombineLERP(OVERLAY_DISP++, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, PRIMITIVE,
                                  TEXEL0, 0, PRIMITIVE, 0);

                ArcheryPos_X = ArcheryPos_X + 6 * 9;

                for (svar1 = svar2 = 0; svar1 < 4; svar1++) {
                    if (sHBAScoreDigits[svar1] != 0 || (svar2 != 0) || (svar1 >= 3)) {
                        OVERLAY_DISP = Gfx_TextureI8(
                            OVERLAY_DISP, digitTextures[sHBAScoreDigits[svar1]], 8, 16, ArcheryPos_X,
                            (ArcheryPos_Y - 2), digitWidth[0], VREG(42), VREG(43) << 1, VREG(43) << 1);
                        ArcheryPos_X += 9;
                        svar2++;
                    }
                }

                gDPPipeSync(OVERLAY_DISP++);
                gDPSetCombineMode(OVERLAY_DISP++, G_CC_MODULATERGBA_PRIM, G_CC_MODULATERGBA_PRIM);
            }
        }

        if ((gSaveContext.timer2State == 5) && (Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT)) {
            // Trade quest timer reached 0
            D_8015FFE6 = 40;
            gSaveContext.cutsceneIndex = 0;
            play->sceneLoadFlag = 0x14;
            play->fadeTransition = 3;
            gSaveContext.timer2State = 0;

            if ((gSaveContext.equips.buttonItems[0] != ITEM_SWORD_KOKIRI) &&
                (gSaveContext.equips.buttonItems[0] != ITEM_SWORD_MASTER) &&
                (gSaveContext.equips.buttonItems[0] != ITEM_SWORD_BGS) &&
                (gSaveContext.equips.buttonItems[0] != ITEM_SWORD_KNIFE)) {
                if (gSaveContext.buttonStatus[0] != BTN_ENABLED) {
                    gSaveContext.equips.buttonItems[0] = gSaveContext.buttonStatus[0];
                } else {
                    gSaveContext.equips.buttonItems[0] = ITEM_NONE;
                }
            }

            // Revert any spoiling trade quest items
            for (svar1 = 0; svar1 < ARRAY_COUNT(gSpoilingItems); svar1++) {
                if (INV_CONTENT(ITEM_TRADE_ADULT) == gSpoilingItems[svar1]) {
                    gSaveContext.eventInf[0] &= 0x7F80;
                    osSyncPrintf("EVENT_INF=%x\n", gSaveContext.eventInf[0]);
                    play->nextEntranceIndex = spoilingItemEntrances[svar1];
                    INV_CONTENT(gSpoilingItemReverts[svar1]) = gSpoilingItemReverts[svar1];

                    for (svar2 = 1; svar2 < ARRAY_COUNT(gSaveContext.equips.buttonItems); svar2++) {
                        if (gSaveContext.equips.buttonItems[svar2] == gSpoilingItems[svar1]) {
                            gSaveContext.equips.buttonItems[svar2] = gSpoilingItemReverts[svar1];
                            Interface_LoadItemIcon1(play, svar2);
                        }
                    }
                }
            }
        }

        if ((play->pauseCtx.state == 0) && (play->pauseCtx.debugState == 0) &&
            (play->gameOverCtx.state == GAMEOVER_INACTIVE) && (msgCtx->msgMode == MSGMODE_NONE) &&
            !(player->stateFlags2 & 0x01000000) && (play->sceneLoadFlag == 0) &&
            (play->transitionMode == 0) && !Play_InCsMode(play) && (gSaveContext.minigameState != 1) &&
            (play->shootingGalleryStatus <= 1) &&
            !((play->sceneNum == SCENE_BOWLING) && Flags_GetSwitch(play, 0x38))) {
            svar6 = 0;
            switch (gSaveContext.timer1State) {
                case 1:
                    D_8015FFE2 = 20;
                    D_8015FFE0 = 20;
                    gSaveContext.timer1Value = gSaveContext.health >> 1;
                    gSaveContext.timer1State = 2;
                    break;
                case 2:
                    D_8015FFE2--;
                    if (D_8015FFE2 == 0) {
                        D_8015FFE2 = 20;
                        gSaveContext.timer1State = 3;
                    }
                    break;
                case 5:
                case 11:
                    D_8015FFE2 = 20;
                    D_8015FFE0 = 20;
                    if (gSaveContext.timer1State == 5) {
                        gSaveContext.timer1State = 6;
                    } else {
                        gSaveContext.timer1State = 12;
                    }
                    break;
                case 6:
                case 12:
                    D_8015FFE2--;
                    if (D_8015FFE2 == 0) {
                        D_8015FFE2 = 20;
                        if (gSaveContext.timer1State == 6) {
                            gSaveContext.timer1State = 7;
                        } else {
                            gSaveContext.timer1State = 13;
                        }
                    }
                    break;
                case 3:
                case 7:
                    svar1 = (gSaveContext.timerX[0] - 26) / D_8015FFE2;
                    gSaveContext.timerX[0] -= svar1;

                    if (gSaveContext.healthCapacity > 0xA0) {
                        svar1 = (gSaveContext.timerY[0] - 54) / D_8015FFE2;
                    } else {
                        svar1 = (gSaveContext.timerY[0] - 46) / D_8015FFE2;
                    }
                    gSaveContext.timerY[0] -= svar1;

                    D_8015FFE2--;
                    if (D_8015FFE2 == 0) {
                        D_8015FFE2 = 20;
                        gSaveContext.timerX[0] = 26;

                        if (gSaveContext.healthCapacity > 0xA0) {
                            gSaveContext.timerY[0] = 54;
                        } else {
                            gSaveContext.timerY[0] = 46;
                        }

                        if (gSaveContext.timer1State == 3) {
                            gSaveContext.timer1State = 4;
                        } else {
                            gSaveContext.timer1State = 8;
                        }
                    }
                case 4:
                case 8:
                    if ((gSaveContext.timer1State == 4) || (gSaveContext.timer1State == 8)) {
                        if (gSaveContext.healthCapacity > 0xA0) {
                            gSaveContext.timerY[0] = 54;
                        } else {
                            gSaveContext.timerY[0] = 46;
                        }
                    }

                    if ((gSaveContext.timer1State >= 3) && (msgCtx->msgLength == 0)) {
                        D_8015FFE0--;
                        if (D_8015FFE0 == 0) {
                            if (gSaveContext.timer1Value != 0) {
                                gSaveContext.timer1Value--;
                            }

                            D_8015FFE0 = 20;

                            if (gSaveContext.timer1Value == 0) {
                                gSaveContext.timer1State = 10;
                                if (D_80125A5C != 0) {
                                    gSaveContext.health = 0;
                                    play->damagePlayer(play, -(gSaveContext.health + 2));
                                }
                                D_80125A5C = 0;
                            } else if (gSaveContext.timer1Value > 60) {
                                if (timerDigits[4] == 1) {
                                    Audio_PlaySoundGeneral(NA_SE_SY_MESSAGE_WOMAN, &D_801333D4, 4, &D_801333E0,
                                                           &D_801333E0, &D_801333E8);
                                }
                            } else if (gSaveContext.timer1Value >= 11) {
                                if (timerDigits[4] & 1) {
                                    Audio_PlaySoundGeneral(NA_SE_SY_WARNING_COUNT_N, &D_801333D4, 4, &D_801333E0,
                                                           &D_801333E0, &D_801333E8);
                                }
                            } else {
                                Audio_PlaySoundGeneral(NA_SE_SY_WARNING_COUNT_E, &D_801333D4, 4, &D_801333E0,
                                                       &D_801333E0, &D_801333E8);
                            }
                        }
                    }
                    break;
                case 13:
                    svar1 = (gSaveContext.timerX[0] - 26) / D_8015FFE2;
                    gSaveContext.timerX[0] -= svar1;

                    if (gSaveContext.healthCapacity > 0xA0) {
                        svar1 = (gSaveContext.timerY[0] - 54) / D_8015FFE2;
                    } else {
                        svar1 = (gSaveContext.timerY[0] - 46) / D_8015FFE2;
                    }
                    gSaveContext.timerY[0] -= svar1;

                    D_8015FFE2--;
                    if (D_8015FFE2 == 0) {
                        D_8015FFE2 = 20;
                        gSaveContext.timerX[0] = 26;
                        if (gSaveContext.healthCapacity > 0xA0) {
                            gSaveContext.timerY[0] = 54;
                        } else {
                            gSaveContext.timerY[0] = 46;
                        }

                        gSaveContext.timer1State = 14;
                    }
                case 14:
                    if (gSaveContext.timer1State == 14) {
                        if (gSaveContext.healthCapacity > 0xA0) {
                            gSaveContext.timerY[0] = 54;
                        } else {
                            gSaveContext.timerY[0] = 46;
                        }
                    }

                    if (gSaveContext.timer1State >= 3) {
                        D_8015FFE0--;
                        if (D_8015FFE0 == 0) {
                            gSaveContext.timer1Value++;
                            D_8015FFE0 = 20;

                            if (gSaveContext.timer1Value == 3599) {
                                D_8015FFE2 = 40;
                                gSaveContext.timer1State = 15;
                            } else {
                                Audio_PlaySoundGeneral(NA_SE_SY_WARNING_COUNT_N, &D_801333D4, 4, &D_801333E0,
                                                       &D_801333E0, &D_801333E8);
                            }
                        }
                    }
                    break;
                case 10:
                    if (gSaveContext.timer2State != 0) {
                        D_8015FFE6 = 20;
                        D_8015FFE4 = 20;
                        gSaveContext.timerX[1] = 140;
                        gSaveContext.timerY[1] = 80;

                        if (gSaveContext.timer2State < 7) {
                            gSaveContext.timer2State = 2;
                        } else {
                            gSaveContext.timer2State = 8;
                        }

                        gSaveContext.timer1State = 0;
                    } else {
                        gSaveContext.timer1State = 0;
                    }
                case 15:
                    break;
                default:
                    svar6 = 1;
                    switch (gSaveContext.timer2State) {
                        case 1:
                        case 7:
                            D_8015FFE6 = 20;
                            D_8015FFE4 = 20;
                            gSaveContext.timerX[1] = 140;
                            gSaveContext.timerY[1] = 80;
                            if (gSaveContext.timer2State == 1) {
                                gSaveContext.timer2State = 2;
                            } else {
                                gSaveContext.timer2State = 8;
                            }
                            break;
                        case 2:
                        case 8:
                            D_8015FFE6--;
                            if (D_8015FFE6 == 0) {
                                D_8015FFE6 = 20;
                                if (gSaveContext.timer2State == 2) {
                                    gSaveContext.timer2State = 3;
                                } else {
                                    gSaveContext.timer2State = 9;
                                }
                            }
                            break;
                        case 3:
                        case 9:
                            osSyncPrintf("event_xp[1]=%d,  event_yp[1]=%d  TOTAL_EVENT_TM=%d\n",
                                         svar5 = gSaveContext.timerX[1], svar2 = gSaveContext.timerY[1],
                                         gSaveContext.timer2Value);
                            svar1 = (gSaveContext.timerX[1] - 26) / D_8015FFE6;
                            gSaveContext.timerX[1] -= svar1;
                            if (gSaveContext.healthCapacity > 0xA0) {
                                svar1 = (gSaveContext.timerY[1] - 54) / D_8015FFE6;
                            } else {
                                svar1 = (gSaveContext.timerY[1] - 46) / D_8015FFE6;
                            }
                            gSaveContext.timerY[1] -= svar1;

                            D_8015FFE6--;
                            if (D_8015FFE6 == 0) {
                                D_8015FFE6 = 20;
                                gSaveContext.timerX[1] = 26;

                                if (gSaveContext.healthCapacity > 0xA0) {
                                    gSaveContext.timerY[1] = 54;
                                } else {
                                    gSaveContext.timerY[1] = 46;
                                }

                                if (gSaveContext.timer2State == 3) {
                                    gSaveContext.timer2State = 4;
                                } else {
                                    gSaveContext.timer2State = 10;
                                }
                            }
                        case 4:
                        case 10:
                            if ((gSaveContext.timer2State == 4) || (gSaveContext.timer2State == 10)) {
                                if (gSaveContext.healthCapacity > 0xA0) {
                                    gSaveContext.timerY[1] = 54;
                                } else {
                                    gSaveContext.timerY[1] = 46;
                                }
                            }

                            if (gSaveContext.timer2State >= 3) {
                                D_8015FFE4--;
                                if (D_8015FFE4 == 0) {
                                    D_8015FFE4 = 20;
                                    if (gSaveContext.timer2State == 4) {
                                        gSaveContext.timer2Value--;
                                        osSyncPrintf("TOTAL_EVENT_TM=%d\n", gSaveContext.timer2Value);

                                        if (gSaveContext.timer2Value <= 0) {
                                            if (!Flags_GetSwitch(play, 0x37) ||
                                                ((play->sceneNum != SCENE_GANON_DEMO) &&
                                                 (play->sceneNum != SCENE_GANON_FINAL) &&
                                                 (play->sceneNum != SCENE_GANON_SONOGO) &&
                                                 (play->sceneNum != SCENE_GANONTIKA_SONOGO))) {
                                                D_8015FFE6 = 40;
                                                gSaveContext.timer2State = 5;
                                                gSaveContext.cutsceneIndex = 0;
                                                Message_StartTextbox(play, 0x71B0, NULL);
                                                func_8002DF54(play, NULL, 8);
                                            } else {
                                                D_8015FFE6 = 40;
                                                gSaveContext.timer2State = 6;
                                            }
                                        } else if (gSaveContext.timer2Value > 60) {
                                            if (timerDigits[4] == 1) {
                                                Audio_PlaySoundGeneral(NA_SE_SY_MESSAGE_WOMAN, &D_801333D4, 4,
                                                                       &D_801333E0, &D_801333E0, &D_801333E8);
                                            }
                                        } else if (gSaveContext.timer2Value > 10) {
                                            if ((timerDigits[4] & 1)) {
                                                Audio_PlaySoundGeneral(NA_SE_SY_WARNING_COUNT_N, &D_801333D4, 4,
                                                                       &D_801333E0, &D_801333E0, &D_801333E8);
                                            }
                                        } else {
                                            Audio_PlaySoundGeneral(NA_SE_SY_WARNING_COUNT_E, &D_801333D4, 4,
                                                                   &D_801333E0, &D_801333E0, &D_801333E8);
                                        }
                                    } else {
                                        gSaveContext.timer2Value++;
                                        if (gSaveContext.eventInf[1] & 1) {
                                            if (gSaveContext.timer2Value == 240) {
                                                Message_StartTextbox(play, 0x6083, NULL);
                                                gSaveContext.eventInf[1] &= ~1;
                                                gSaveContext.timer2State = 0;
                                            }
                                        }
                                    }

                                    if ((gSaveContext.timer2Value % 60) == 0) {
                                        Audio_PlaySoundGeneral(NA_SE_SY_WARNING_COUNT_N, &D_801333D4, 4, &D_801333E0,
                                                               &D_801333E0, &D_801333E8);
                                    }
                                }
                            }
                            break;
                        case 6:
                            D_8015FFE6--;
                            if (D_8015FFE6 == 0) {
                                gSaveContext.timer2State = 0;
                            }
                            break;
                    }
                    break;
            }

            if (((gSaveContext.timer1State != 0) && (gSaveContext.timer1State != 10)) ||
                (gSaveContext.timer2State != 0)) {
                timerDigits[0] = timerDigits[1] = svar2 = timerDigits[3] = 0;
                timerDigits[2] = 10; // digit 10 is used as ':' (colon)

                if (gSaveContext.timer1State != 0) {
                    timerDigits[4] = gSaveContext.timer1Value;
                } else {
                    timerDigits[4] = gSaveContext.timer2Value;
                }

                while (timerDigits[4] >= 60) {
                    timerDigits[1]++;
                    if (timerDigits[1] >= 10) {
                        timerDigits[0]++;
                        timerDigits[1] -= 10;
                    }
                    timerDigits[4] -= 60;
                }

                while (timerDigits[4] >= 10) {
                    timerDigits[3]++;
                    timerDigits[4] -= 10;
                }

                // Clock Icon
                gDPPipeSync(OVERLAY_DISP++);
                gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, 255);
                gDPSetEnvColor(OVERLAY_DISP++, 0, 0, 0, 0);
                s32 X_Margins_Timer;
                if (CVar_GetS32("gTimersUseMargins", 0) != 0) {
                    if (CVar_GetS32("gTimersPosType", 0) == 0) {X_Margins_Timer = Left_HUD_Margin;};
                } else {
                    X_Margins_Timer = 0;
                }
                svar5 = OTRGetRectDimensionFromLeftEdge(gSaveContext.timerX[svar6]+X_Margins_Timer);
                svar2 = gSaveContext.timerY[svar6];
                if (CVar_GetS32("gTimersPosType", 0) != 0) {
                    svar2 = (CVar_GetS32("gTimersPosY", 0));
                    if (CVar_GetS32("gTimersPosType", 0) == 1) {//Anchor Left
                        if (CVar_GetS32("gTimersUseMargins", 0) != 0) {X_Margins_Timer = Left_HUD_Margin;};
                        svar5 = OTRGetRectDimensionFromLeftEdge(CVar_GetS32("gTimersPosX", 0)+X_Margins_Timer);
                    } else if (CVar_GetS32("gTimersPosType", 0) == 2) {//Anchor Right
                        if (CVar_GetS32("gTimersUseMargins", 0) != 0) {X_Margins_Timer = Right_HUD_Margin;};
                        svar5 = OTRGetRectDimensionFromRightEdge(CVar_GetS32("gTimersPosX", 0)+X_Margins_Timer);
                    } else if (CVar_GetS32("gTimersPosType", 0) == 3) {//Anchor None
                        svar5 = CVar_GetS32("gTimersPosX", 0)+204+X_Margins_Timer;
                    } else if (CVar_GetS32("gTimersPosType", 0) == 4) {//Hidden
                        svar5 = -9999;
                    }
                }

                OVERLAY_DISP =
                    Gfx_TextureIA8(OVERLAY_DISP, gClockIconTex, 16, 16, svar5, svar2 + 2, 16, 16, 1 << 10, 1 << 10);

                // Timer Counter
                gDPPipeSync(OVERLAY_DISP++);
                gDPSetCombineLERP(OVERLAY_DISP++, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, PRIMITIVE,
                                  TEXEL0, 0, PRIMITIVE, 0);

                if (gSaveContext.timer1State != 0) {
                    if ((gSaveContext.timer1Value < 10) && (gSaveContext.timer1State < 11)) {
                        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 50, 0, 255);
                    } else {
                        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, 255);
                    }
                } else {
                    if ((gSaveContext.timer2Value < 10) && (gSaveContext.timer2State < 6)) {
                        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 50, 0, 255);
                    } else {
                        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 0, 255);
                    }
                }

                for (svar1 = 0; svar1 < 5; svar1++) {
                    // clang-format off
                    //svar5 = svar5 + 8;
                    //svar5 = OTRGetRectDimensionFromLeftEdge(gSaveContext.timerX[svar6]); 
                    OVERLAY_DISP = Gfx_TextureI8(OVERLAY_DISP, digitTextures[timerDigits[svar1]], 8, 16,
                                      svar5 + timerDigitLeftPos[svar1],
                                      svar2, digitWidth[svar1], VREG(42), VREG(43) << 1,
                                      VREG(43) << 1);
                    // clang-format on
                }
            }
        }
    }

    if (pauseCtx->debugState == 3) {
        FlagSet_Update(play);
    }

    if (interfaceCtx->unk_244 != 0) {
        gDPPipeSync(OVERLAY_DISP++);
        gSPDisplayList(OVERLAY_DISP++, sSetupDL_80125A60);
        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 0, 0, 0, interfaceCtx->unk_244);
        gDPFillRectangle(OVERLAY_DISP++, 0, 0, gScreenWidth - 1, gScreenHeight - 1);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void Interface_Update(PlayState* play) {
    static u8 D_80125B60 = 0;
    static s16 sPrevTimeIncrement = 0;
    MessageContext* msgCtx = &play->msgCtx;
    InterfaceContext* interfaceCtx = &play->interfaceCtx;
    Player* player = GET_PLAYER(play);
    s16 alpha;
    s16 alpha1;
    u16 action;
    Input* debugInput = &play->state.input[2];

    Top_HUD_Margin = CVar_GetS32("gHUDMargin_T", 0);
    Left_HUD_Margin = CVar_GetS32("gHUDMargin_L", 0);
    Right_HUD_Margin = CVar_GetS32("gHUDMargin_R", 0);
    Bottom_HUD_Margin = CVar_GetS32("gHUDMargin_B", 0);

    if (CHECK_BTN_ALL(debugInput->press.button, BTN_DLEFT)) {
        gSaveContext.language = LANGUAGE_ENG;
        osSyncPrintf("J_N=%x J_N=%x\n", gSaveContext.language, &gSaveContext.language);
    } else if (CHECK_BTN_ALL(debugInput->press.button, BTN_DUP)) {
        gSaveContext.language = LANGUAGE_GER;
        osSyncPrintf("J_N=%x J_N=%x\n", gSaveContext.language, &gSaveContext.language);
    } else if (CHECK_BTN_ALL(debugInput->press.button, BTN_DRIGHT)) {
        gSaveContext.language = LANGUAGE_FRA;
        osSyncPrintf("J_N=%x J_N=%x\n", gSaveContext.language, &gSaveContext.language);
    }

    if ((play->pauseCtx.state == 0) && (play->pauseCtx.debugState == 0)) {
        if ((gSaveContext.minigameState == 1) || (gSaveContext.sceneSetupIndex < 4) ||
            ((play->sceneNum == SCENE_SPOT20) && (gSaveContext.sceneSetupIndex == 4))) {
            if ((msgCtx->msgMode == MSGMODE_NONE) ||
                ((msgCtx->msgMode != MSGMODE_NONE) && (play->sceneNum == SCENE_BOWLING))) {
                if (play->gameOverCtx.state == GAMEOVER_INACTIVE) {
                    func_80083108(play);
                }
            }
        }
    }

    switch (gSaveContext.unk_13E8) {
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
            alpha = 255 - (gSaveContext.unk_13EC << 5);
            if (alpha < 0) {
                alpha = 0;
            }

            func_80082850(play, alpha);
            gSaveContext.unk_13EC++;

            if (alpha == 0) {
                gSaveContext.unk_13E8 = 0;
            }
            break;
        case 50:
            alpha = 255 - (gSaveContext.unk_13EC << 5);
            if (alpha < 0) {
                alpha = 0;
            }

            alpha1 = 0xFF - alpha;
            if (alpha1 >= 0xFF) {
                alpha1 = 0xFF;
            }

            osSyncPrintf("case 50 : alpha=%d  alpha1=%d\n", alpha, alpha1);
            func_80082644(play, alpha1);

            if (interfaceCtx->healthAlpha != 255) {
                interfaceCtx->healthAlpha = alpha1;
            }

            if (interfaceCtx->magicAlpha != 255) {
                interfaceCtx->magicAlpha = alpha1;
            }

            switch (play->sceneNum) {
                case SCENE_SPOT00:
                case SCENE_SPOT01:
                case SCENE_SPOT02:
                case SCENE_SPOT03:
                case SCENE_SPOT04:
                case SCENE_SPOT05:
                case SCENE_SPOT06:
                case SCENE_SPOT07:
                case SCENE_SPOT08:
                case SCENE_SPOT09:
                case SCENE_SPOT10:
                case SCENE_SPOT11:
                case SCENE_SPOT12:
                case SCENE_SPOT13:
                case SCENE_SPOT15:
                case SCENE_SPOT16:
                case SCENE_SPOT17:
                case SCENE_SPOT18:
                case SCENE_SPOT20:
                case SCENE_GANON_TOU:
                    if (interfaceCtx->minimapAlpha < 170) {
                        interfaceCtx->minimapAlpha = alpha1;
                    } else {
                        interfaceCtx->minimapAlpha = 170;
                    }
                    break;
                default:
                    if (interfaceCtx->minimapAlpha != 255) {
                        interfaceCtx->minimapAlpha = alpha1;
                    }
                    break;
            }

            gSaveContext.unk_13EC++;
            if (alpha1 == 0xFF) {
                gSaveContext.unk_13E8 = 0;
            }

            break;
        case 52:
            gSaveContext.unk_13E8 = 1;
            func_80082850(play, 0);
            gSaveContext.unk_13E8 = 0;
        default:
            break;
    }

    Map_Update(play);

    if (gSaveContext.healthAccumulator != 0) {
        gSaveContext.healthAccumulator -= 4;
        gSaveContext.health += 4;

        if ((gSaveContext.health & 0xF) < 4) {
            Audio_PlaySoundGeneral(NA_SE_SY_HP_RECOVER, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        }

        osSyncPrintf("now_life=%d  max_life=%d\n", gSaveContext.health, gSaveContext.healthCapacity);

        if (gSaveContext.health >= gSaveContext.healthCapacity) {
            gSaveContext.health = gSaveContext.healthCapacity;
            osSyncPrintf("S_Private.now_life=%d  S_Private.max_life=%d\n", gSaveContext.health,
                         gSaveContext.healthCapacity);
            gSaveContext.healthAccumulator = 0;
        }
    }

    HealthMeter_HandleCriticalAlarm(play);
    D_80125A58 = func_8008F2F8(play);

    if (D_80125A58 == 1) {
        if (CUR_EQUIP_VALUE(EQUIP_TUNIC) == 2 || CVar_GetS32("gSuperTunic", 0) != 0) {
            D_80125A58 = 0;
        }
    } else if ((func_8008F2F8(play) >= 2) && (func_8008F2F8(play) < 5)) {
        if (CUR_EQUIP_VALUE(EQUIP_TUNIC) == 3 || CVar_GetS32("gSuperTunic", 0) != 0) {
            D_80125A58 = 0;
        }
    }

    HealthMeter_Update(play);

    if ((gSaveContext.timer1State >= 3) && (play->pauseCtx.state == 0) && (play->pauseCtx.debugState == 0) &&
        (msgCtx->msgMode == MSGMODE_NONE) && !(player->stateFlags2 & 0x01000000) && (play->sceneLoadFlag == 0) &&
        (play->transitionMode == 0) && !Play_InCsMode(play)) {}

    if (gSaveContext.rupeeAccumulator != 0) {
        if (gSaveContext.rupeeAccumulator > 0) {
            if (gSaveContext.rupees < CUR_CAPACITY(UPG_WALLET)) {
                gSaveContext.rupeeAccumulator--;
                gSaveContext.rupees++;
                Audio_PlaySoundGeneral(NA_SE_SY_RUPY_COUNT, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            } else {
                // "Rupee Amount MAX = %d"
                osSyncPrintf("ルピー数ＭＡＸ = %d\n", CUR_CAPACITY(UPG_WALLET));
                gSaveContext.rupees = CUR_CAPACITY(UPG_WALLET);
                gSaveContext.rupeeAccumulator = 0;
            }
        } else if (gSaveContext.rupees != 0) {
            if (gSaveContext.rupeeAccumulator <= -50) {
                gSaveContext.rupeeAccumulator += 10;
                gSaveContext.rupees -= 10;

                if (gSaveContext.rupees < 0) {
                    gSaveContext.rupees = 0;
                }

                Audio_PlaySoundGeneral(NA_SE_SY_RUPY_COUNT, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            } else {
                gSaveContext.rupeeAccumulator++;
                gSaveContext.rupees--;
                Audio_PlaySoundGeneral(NA_SE_SY_RUPY_COUNT, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            }
        } else {
            gSaveContext.rupeeAccumulator = 0;
        }
    }

    switch (interfaceCtx->unk_1EC) {
        case 1:
            interfaceCtx->unk_1F4 += 31400.0f / WREG(5);
            if (interfaceCtx->unk_1F4 >= 15700.0f) {
                interfaceCtx->unk_1F4 = -15700.0f;
                interfaceCtx->unk_1EC = 2;
            }
            break;
        case 2:
            interfaceCtx->unk_1F4 += 31400.0f / WREG(5);
            if (interfaceCtx->unk_1F4 >= 0.0f) {
                interfaceCtx->unk_1F4 = 0.0f;
                interfaceCtx->unk_1EC = 0;
                interfaceCtx->unk_1EE = interfaceCtx->unk_1F0;
                action = interfaceCtx->unk_1EE;
                if ((action == DO_ACTION_MAX) || (action == DO_ACTION_MAX + 1)) {
                    action = DO_ACTION_NONE;
                }
                Interface_LoadActionLabel(interfaceCtx, action, 0);
            }
            break;
        case 3:
            interfaceCtx->unk_1F4 += 31400.0f / WREG(5);
            if (interfaceCtx->unk_1F4 >= 15700.0f) {
                interfaceCtx->unk_1F4 = -15700.0f;
                interfaceCtx->unk_1EC = 2;
            }
            break;
        case 4:
            interfaceCtx->unk_1F4 += 31400.0f / WREG(5);
            if (interfaceCtx->unk_1F4 >= 0.0f) {
                interfaceCtx->unk_1F4 = 0.0f;
                interfaceCtx->unk_1EC = 0;
                interfaceCtx->unk_1EE = interfaceCtx->unk_1F0;
                action = interfaceCtx->unk_1EE;
                if ((action == DO_ACTION_MAX) || (action == DO_ACTION_MAX + 1)) {
                    action = DO_ACTION_NONE;
                }
                Interface_LoadActionLabel(interfaceCtx, action, 0);
            }
            break;
    }

    WREG(7) = interfaceCtx->unk_1F4;

    if ((play->pauseCtx.state == 0) && (play->pauseCtx.debugState == 0) &&
        (msgCtx->msgMode == MSGMODE_NONE) && (play->sceneLoadFlag == 0) &&
        (play->gameOverCtx.state == GAMEOVER_INACTIVE) && (play->transitionMode == 0) &&
        ((play->csCtx.state == CS_STATE_IDLE) || !Player_InCsMode(play))) {
        if ((gSaveContext.magicAcquired != 0) && (gSaveContext.magicLevel == 0)) {
            gSaveContext.magicLevel = gSaveContext.doubleMagic + 1;
            gSaveContext.unk_13F0 = 8;
            osSyncPrintf(VT_FGCOL(YELLOW));
            osSyncPrintf("魔法スター─────ト！！！！！！！！！\n"); // "Magic Start!!!!!!!!!"
            osSyncPrintf("MAGIC_MAX=%d\n", gSaveContext.magicLevel);
            osSyncPrintf("MAGIC_NOW=%d\n", gSaveContext.magic);
            osSyncPrintf("Z_MAGIC_NOW_NOW=%d\n", gSaveContext.unk_13F6);
            osSyncPrintf("Z_MAGIC_NOW_MAX=%d\n", gSaveContext.unk_13F4);
            osSyncPrintf(VT_RST);
        }

        Interface_UpdateMagicBar(play);
    }

    if (gSaveContext.timer1State == 0) {
        if (((D_80125A58 == 1) || (D_80125A58 == 2) || (D_80125A58 == 4)) && ((gSaveContext.health >> 1) != 0)) {
            gSaveContext.timer1State = 1;
            gSaveContext.timerX[0] = 140;
            gSaveContext.timerY[0] = 80;
            D_80125A5C = 1;
        }
    } else {
        if (((D_80125A58 == 0) || (D_80125A58 == 3)) && (gSaveContext.timer1State < 5)) {
            gSaveContext.timer1State = 0;
        }
    }

    if (gSaveContext.minigameState == 1) {
        gSaveContext.minigameScore += interfaceCtx->unk_23C;
        interfaceCtx->unk_23C = 0;

        if (sHBAScoreTier == 0) {
            if (gSaveContext.minigameScore >= 1000) {
                sHBAScoreTier++;
            }
        } else if (sHBAScoreTier == 1) {
            if (gSaveContext.minigameScore >= 1500) {
                sHBAScoreTier++;
            }
        }

        sHBAScoreDigits[0] = sHBAScoreDigits[1] = 0;
        sHBAScoreDigits[2] = 0;
        sHBAScoreDigits[3] = gSaveContext.minigameScore;

        while (sHBAScoreDigits[3] >= 1000) {
            sHBAScoreDigits[0]++;
            sHBAScoreDigits[3] -= 1000;
        }

        while (sHBAScoreDigits[3] >= 100) {
            sHBAScoreDigits[1]++;
            sHBAScoreDigits[3] -= 100;
        }

        while (sHBAScoreDigits[3] >= 10) {
            sHBAScoreDigits[2]++;
            sHBAScoreDigits[3] -= 10;
        }
    }

    if (gSaveContext.sunsSongState != SUNSSONG_INACTIVE) {
        // exit out of ocarina mode after suns song finishes playing
        if ((msgCtx->ocarinaAction != OCARINA_ACTION_CHECK_NOWARP_DONE) &&
            (gSaveContext.sunsSongState == SUNSSONG_START)) {
            play->msgCtx.ocarinaMode = OCARINA_MODE_04;
        }

        // handle suns song in areas where time moves
        if (play->envCtx.timeIncrement != 0) {
            if (gSaveContext.sunsSongState != SUNSSONG_SPEED_TIME) {
                D_80125B60 = 0;
                if ((gSaveContext.dayTime >= 0x4555) && (gSaveContext.dayTime <= 0xC001)) {
                    D_80125B60 = 1;
                }

                gSaveContext.sunsSongState = SUNSSONG_SPEED_TIME;
                sPrevTimeIncrement = gTimeIncrement;
                gTimeIncrement = 400;
            } else if (D_80125B60 == 0) {
                if ((gSaveContext.dayTime >= 0x4555) && (gSaveContext.dayTime <= 0xC001)) {
                    gSaveContext.sunsSongState = SUNSSONG_INACTIVE;
                    gTimeIncrement = sPrevTimeIncrement;
                    play->msgCtx.ocarinaMode = OCARINA_MODE_04;
                }
            } else if (gSaveContext.dayTime > 0xC001) {
                gSaveContext.sunsSongState = SUNSSONG_INACTIVE;
                gTimeIncrement = sPrevTimeIncrement;
                play->msgCtx.ocarinaMode = OCARINA_MODE_04;
            }
        } else if ((play->roomCtx.curRoom.behaviorType1 != ROOM_BEHAVIOR_TYPE1_1) &&
                   (interfaceCtx->restrictions.sunsSong != 3)) {
            if ((gSaveContext.dayTime >= 0x4555) && (gSaveContext.dayTime < 0xC001)) {
                gSaveContext.nextDayTime = 0;
                play->fadeTransition = 4;
                gSaveContext.nextTransition = 2;
                play->unk_11DE9 = 1;
            } else {
                gSaveContext.nextDayTime = 0x8001;
                play->fadeTransition = 5;
                gSaveContext.nextTransition = 3;
                play->unk_11DE9 = 1;
            }

            if (play->sceneNum == SCENE_SPOT13) {
                play->fadeTransition = 14;
                gSaveContext.nextTransition = 14;
            }

            gSaveContext.respawnFlag = -2;
            play->nextEntranceIndex = gSaveContext.entranceIndex;

            // In ER, handle sun song respawn from last entrance from grottos
            if (gSaveContext.n64ddFlag && Randomizer_GetSettingValue(RSK_SHUFFLE_ENTRANCES)) {
                Grotto_ForceGrottoReturn();
            }

            play->sceneLoadFlag = 0x14;
            gSaveContext.sunsSongState = SUNSSONG_INACTIVE;
            func_800F6964(30);
            gSaveContext.seqId = (u8)NA_BGM_DISABLED;
            gSaveContext.natureAmbienceId = NATURE_ID_DISABLED;
        } else {
            gSaveContext.sunsSongState = SUNSSONG_SPECIAL;
        }
    }
}
