#include "z_kaleido_scope.h"
#include "soh/SceneDB.h"
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

static const Vtx sMarkBossVtx[] = {
    VTX(-4, 4, 0, 0, 0, 255, 255, 255, 255),
    VTX(-4, -4, 0, 0, 256, 255, 255, 255, 255),
    VTX(4, 4, 0, 256, 0, 255, 255, 255, 255),
    VTX(4, -4, 0, 256, 256, 255, 255, 255, 255),
};

static const Vtx sMarkChestVtx[] = {
    VTX(-4, 4, 0, 0, 0, 255, 255, 255, 255),
    VTX(-4, -4, 0, 0, 256, 255, 255, 255, 255),
    VTX(4, 4, 0, 256, 0, 255, 255, 255, 255),
    VTX(4, -4, 0, 256, 256, 255, 255, 255, 255),
};

void PauseMapMark_Init(PlayState* play) {
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
    f32 scale;
    SceneDBEntry* entry = SceneDB_Retrieve(play->sceneNum);
    SceneDBFloor* floor = &entry->dungeonData.floors[play->pauseCtx.dungeonMapSlot - 3];

    if (SceneRB_IsBoss(play->sceneNum)) {
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

    OPEN_DISPS(play->state.gfxCtx);

    gDPPipeSync(POLY_KAL_DISP++);
    gDPSetPrimColor(POLY_KAL_DISP++, 0, 0, 255, 255, 255, 255);
    gDPSetEnvColor(POLY_KAL_DISP++, 0, 0, 0, 255);

    Matrix_Push();

    if ((play->pauseCtx.state == 4) || (play->pauseCtx.state >= 0x12)) {
        Matrix_Translate(-36.0f, 101.0f, 0.0f, MTXMODE_APPLY);
    } else {
        Matrix_Translate(-36.0f, 21.0f, 0.0f, MTXMODE_APPLY);
    }

    // TODO check G_IM_SIZ (is replacing it with the bit size equivalent to MARK)
    gDPLoadTextureBlock(POLY_KAL_DISP++, gMapChestIconTex, G_IM_FMT_RGBA, G_IM_SIZ_16b,
        8, 8, 0, G_TX_NOMIRROR | G_TX_WRAP,
        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    for (s32 i = 0; i < floor->numChestMarks; i++) {
        s32 display;

        if (Flags_GetTreasure(play, floor->chestMarks[i].chestFlag)) {
            display = false;
        } else {
            display = SceneRB_IsDungeon(play->sceneNum);
        }

        if (display) {
            // Compute the offset to mirror icons over the map center (48) as an axis line
            s16 mirrorOffset = CVarGetInteger("gMirroredWorld", 0) ? mirrorOffset = (48 - floor->chestMarks[i].x) * 2 + 1 : 0;

            Matrix_Push();
            Matrix_Translate(GREG(92) + floor->chestMarks[i].x + mirrorOffset, GREG(93) + floor->chestMarks[i].y, 0.0f, MTXMODE_APPLY);
            Matrix_Scale(1.0f, 1.0f, 1.0f, MTXMODE_APPLY);
            gSPMatrix(POLY_KAL_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            Matrix_Pop();

            gSPVertex(POLY_KAL_DISP++, sMarkChestVtx, 4, 0);
            gSP1Quadrangle(POLY_KAL_DISP++, 1, 3, 2, 0, 0);
        }
    }

    Matrix_Pop();

    Matrix_Push();

    if ((play->pauseCtx.state == 4) || (play->pauseCtx.state >= 0x12)) {
        Matrix_Translate(-36.0f, 101.0f, 0.0f, MTXMODE_APPLY);
    } else {
        Matrix_Translate(-36.0f, 21.0f, 0.0f, MTXMODE_APPLY);
    }

    gDPLoadTextureBlock(POLY_KAL_DISP++, gMapBossIconTex, G_IM_FMT_IA, G_IM_SIZ_8b,
        8, 8, 0, G_TX_NOMIRROR | G_TX_WRAP,
        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    for (s32 i = 0; i < floor->numBossMarks; i++) {
        // Compute the offset to mirror icons over the map center (48) as an axis line
        s16 mirrorOffset = CVarGetInteger("gMirroredWorld", 0) ? mirrorOffset = (48 - floor->bossMarks[i].x) * 2 + 1 : 0;

        Matrix_Push();
        Matrix_Translate(GREG(92) + floor->bossMarks[i].x + mirrorOffset, GREG(93) + floor->bossMarks[i].y, 0.0f, MTXMODE_APPLY);
        Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
        gSPMatrix(POLY_KAL_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
            G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        Matrix_Pop();

        gSPVertex(POLY_KAL_DISP++, sMarkBossVtx, 4, 0);
        gSP1Quadrangle(POLY_KAL_DISP++, 1, 3, 2, 0, 0);
    }

    Matrix_Pop();
    /*
    while (false) {
        if (mapMarkData->markType == PAUSE_MAP_MARK_NONE) {
            break;
        }

        if ((mapMarkData->markType == PAUSE_MAP_MARK_BOSS) && SceneRB_IsBoss(play->sceneNum)) {
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

        markPoint = &mapMarkData->points[0];
        for (s32 i = 0; i < mapMarkData->count; i++) {
            s32 display;

            if (mapMarkData->markType == PAUSE_MAP_MARK_CHEST) {
                if (Flags_GetTreasure(play, markPoint->chestFlag)) {
                    display = false;
                } else {
                    display = SceneRB_IsDungeon(play->sceneNum);
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

                // Compute the offset to mirror icons over the map center (48) as an axis line
                s16 mirrorOffset = CVarGetInteger(CVAR_ENHANCEMENT("MirroredWorld"), 0) ? mirrorOffset = (48 - markPoint->x) * 2 + 1 : 0;

                Matrix_Push();
                Matrix_Translate(GREG(92) + markPoint->x + mirrorOffset, GREG(93) + markPoint->y, 0.0f, MTXMODE_APPLY);
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
    */

    CLOSE_DISPS(play->state.gfxCtx);
}

void PauseMapMark_Draw(PlayState* play) {
    PauseMapMark_Init(play);

    if (SceneRB_IsDungeon(play->sceneNum) || (CVarGetInteger("TODO.PulsateBossIcon", 0) != 0 && SceneRB_IsBoss(play->sceneNum))) {
        PauseMapMark_DrawForDungeon(play);
    }

    PauseMapMark_Clear(play);
}
