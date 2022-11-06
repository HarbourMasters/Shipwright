#include "z_kaleido_scope.h"
#include "textures/parameter_static/parameter_static.h"

typedef struct {
    /* 0x00 */ void* texture;
    /* 0x04 */ u32 imageFormat;
    /* 0x08 */ u32 imageSize;
    /* 0x0C */ u32 textureWidth;
    /* 0x10 */ u32 textureHeight;
    /* 0x14 */ u32 rectWidth;
    /* 0x18 */ u32 rectHeight;
    /* 0x1C */ u32 dsdx;
    /* 0x20 */ u32 dtdy;
} PauseMapMarkInfo; // size = 0x24

static PauseMapMarkInfo sMapMarkInfoTable[] = {
    { gMapChestIconTex, G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 8, 32, 32, 1 << 10, 1 << 10 },
    { gMapBossIconTex, G_IM_FMT_IA, G_IM_SIZ_8b, 8, 8, 32, 32, 1 << 10, 1 << 10 },
};

static const u32 sBaseImageSizes[] = { 0, 1, 2, 3 };
static const u32 sLoadBlockImageSizes[] = { 2, 2, 2, 3 };
static const u32 sIncrImageSizes[] = { 3, 1, 0, 0 };
static const u32 sShiftImageSizes[] = { 2, 1, 0, 0 };
static const u32 sBytesImageSizes[] = { 0, 1, 2, 4 };
static const u32 sLineBytesImageSizes[] = { 0, 1, 2, 2 };

#define G_IM_SIZ_MARK sBaseImageSizes[markInfo->imageSize]
#define G_IM_SIZ_MARK_LOAD_BLOCK sLoadBlockImageSizes[markInfo->imageSize]
#define G_IM_SIZ_MARK_INCR sIncrImageSizes[markInfo->imageSize]
#define G_IM_SIZ_MARK_SHIFT sShiftImageSizes[markInfo->imageSize]
#define G_IM_SIZ_MARK_BYTES sBytesImageSizes[markInfo->imageSize]
#define G_IM_SIZ_MARK_LINE_BYTES sLineBytesImageSizes[markInfo->imageSize]

extern PauseMapMarksData gPauseMapMarkDataTable[];
extern PauseMapMarksData gPauseMapMarkDataTableMasterQuest[];

void PauseMapMark_Init(PlayState* play) {
    gBossMarkState = 0;
    gBossMarkScale = 1.0f;
    if(ResourceMgr_IsGameMasterQuest()) {
        gLoadedPauseMarkDataTable = gPauseMapMarkDataTableMasterQuest;
    } else {
        gLoadedPauseMarkDataTable = gPauseMapMarkDataTable;
    }
}

void PauseMapMark_Clear(PlayState* play) {
    gLoadedPauseMarkDataTable = NULL;
}

void PauseMapMark_DrawForDungeon(PlayState* play) {
    PauseMapMarkData* mapMarkData;
    PauseMapMarkPoint* markPoint;
    PauseMapMarkInfo* markInfo;
    f32 scale;
    s32 i = 0;

    mapMarkData = &gLoadedPauseMarkDataTable[R_MAP_TEX_INDEX >> 1][i];

    OPEN_DISPS(play->state.gfxCtx);

    while (true) {
        if (mapMarkData->markType == PAUSE_MAP_MARK_NONE) {
            break;
        }

        if ((mapMarkData->markType == PAUSE_MAP_MARK_BOSS) && (play->sceneNum >= SCENE_YDAN_BOSS) &&
            (play->sceneNum <= SCENE_GANON_FINAL)) {
            if (gBossMarkState == 0) {
                Math_ApproachF(&gBossMarkScale, 1.5f, 1.0f, 0.041f);
                if (gBossMarkScale == 1.5f) {
                    gBossMarkState = 1;
                }
            } else {
                Math_ApproachF(&gBossMarkScale, 1.0f, 1.0f, 0.041f);
                if (gBossMarkScale == 1.0f) {
                    gBossMarkState = 0;
                }
            }
            scale = gBossMarkScale;
        } else {
            scale = 1.0f;
        }

        Matrix_Push();

        if ((play->pauseCtx.state == 4) || (play->pauseCtx.state >= 0x12)) {
            Matrix_Translate(-36.0f, 101.0f, 0.0f, MTXMODE_APPLY);
        } else {
            Matrix_Translate(-36.0f, 21.0f, 0.0f, MTXMODE_APPLY);
        }

        gDPPipeSync(POLY_KAL_DISP++);
        gDPSetPrimColor(POLY_KAL_DISP++, 0, 0, 255, 255, 255, 255);
        gDPSetEnvColor(POLY_KAL_DISP++, 0, 0, 0, 255);

        markPoint = &mapMarkData->points[0];
        for (i = 0; i < mapMarkData->count; i++) {
            s32 display;

            if (mapMarkData->markType == PAUSE_MAP_MARK_CHEST) {
                if (Flags_GetTreasure(play, markPoint->chestFlag)) {
                    display = false;
                } else {
                    switch (play->sceneNum) {
                        case SCENE_YDAN_BOSS:
                        case SCENE_DDAN_BOSS:
                        case SCENE_BDAN_BOSS:
                        case SCENE_MORIBOSSROOM:
                        case SCENE_FIRE_BS:
                        case SCENE_MIZUSIN_BS:
                        case SCENE_JYASINBOSS:
                        case SCENE_HAKADAN_BS:
                            display = false;
                            break;
                        default:
                            display = true;
                            break;
                    }
                }
            } else {
                display = true;
            }

            if (display) {
                markInfo = &sMapMarkInfoTable[mapMarkData->markType];

                gDPPipeSync(POLY_KAL_DISP++);
                gDPLoadTextureBlock(POLY_KAL_DISP++, markInfo->texture, markInfo->imageFormat, G_IM_SIZ_MARK,
                                    markInfo->textureWidth, markInfo->textureHeight, 0, G_TX_NOMIRROR | G_TX_WRAP,
                                    G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

                Matrix_Push();
                Matrix_Translate(GREG(92) + markPoint->x, GREG(93) + markPoint->y, 0.0f, MTXMODE_APPLY);
                Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
                gSPMatrix(POLY_KAL_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                          G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                Matrix_Pop();

                gSPVertex(POLY_KAL_DISP++, mapMarkData->vtx, mapMarkData->vtxCount, 0);
                gSP1Quadrangle(POLY_KAL_DISP++, 1, 3, 2, 0, 0);
            }

            markPoint++;
        }

        mapMarkData++;
        Matrix_Pop();
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void PauseMapMark_Draw(PlayState* play) {
    PauseMapMark_Init(play);

    switch (play->sceneNum) {
        case SCENE_YDAN:
        case SCENE_DDAN:
        case SCENE_BDAN:
        case SCENE_BMORI1:
        case SCENE_HIDAN:
        case SCENE_MIZUSIN:
        case SCENE_JYASINZOU:
        case SCENE_HAKADAN:
        case SCENE_HAKADANCH:
        case SCENE_ICE_DOUKUTO:
            PauseMapMark_DrawForDungeon(play);
            break;
    }

    PauseMapMark_Clear(play);
}
