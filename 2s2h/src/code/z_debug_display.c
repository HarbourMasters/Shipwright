#include "z64debug_display.h"
#include "global.h"

DebugDispObject* sDebugObjectListHead;

typedef struct {
    /* 0x0 */ s16 drawType;  // indicates which draw function to use when displaying the object
    /* 0x4 */ void* drawArg; // segment address (display list or texture) passed to the draw funciton when called
} DebugDispObjectInfo;       // size = 0x8

typedef void (*DebugDispObjectDrawFunc)(DebugDispObject*, void*, PlayState*);

void DebugDisplay_DrawSpriteI8(DebugDispObject* dispObj, void* texture, PlayState* play);
void DebugDisplay_DrawPolygon(DebugDispObject* dispObj, void* dList, PlayState* play);
Gfx* DebugDisplay_PathDisplayList(GraphicsContext* gfxCtx, Path* path);

DebugDispObject* DebugDisplay_Init(void) {
    sDebugObjectListHead = NULL;
    return sDebugObjectListHead;
}

DebugDispObject* DebugDisplay_AddObject(f32 posX, f32 posY, f32 posZ, s16 rotX, s16 rotY, s16 rotZ, f32 scaleX,
                                        f32 scaleY, f32 scaleZ, u8 red, u8 green, u8 blue, u8 alpha, s16 type,
                                        GraphicsContext* gfxCtx) {
    DebugDispObject* oldHead = sDebugObjectListHead;

    sDebugObjectListHead = GRAPH_ALLOC(gfxCtx, sizeof(DebugDispObject));
    sDebugObjectListHead->pos.x = posX;
    sDebugObjectListHead->pos.y = posY;
    sDebugObjectListHead->pos.z = posZ;
    sDebugObjectListHead->rot.x = rotX;
    sDebugObjectListHead->rot.y = rotY;
    sDebugObjectListHead->rot.z = rotZ;
    sDebugObjectListHead->scale.x = scaleX;
    sDebugObjectListHead->scale.y = scaleY;
    sDebugObjectListHead->scale.z = scaleZ;
    sDebugObjectListHead->color.r = red;
    sDebugObjectListHead->color.g = green;
    sDebugObjectListHead->color.b = blue;
    sDebugObjectListHead->color.a = alpha;
    sDebugObjectListHead->type = type;
    sDebugObjectListHead->next = oldHead;
    return sDebugObjectListHead;
}

#include "code/debug_display/debug_display.h"

DebugDispObjectDrawFunc sDebugObjectDrawFuncTable[] = { DebugDisplay_DrawSpriteI8, DebugDisplay_DrawPolygon };

DebugDispObjectInfo sDebugObjectInfoTable[] = {
    { 0, sDebugDisplayCircleTex }, { 0, sDebugDisplayCrossTex }, { 0, sDebugDisplayBallTex },
    { 0, sDebugDisplayCursorTex }, { 1, sDebugDisplay1DL },      { 1, sDebugDisplay3DL },
    { 1, sDebugDisplay2DL },
};

void DebugDisplay_DrawObjects(PlayState* play) {
    DebugDispObject* dispObj = sDebugObjectListHead;
    DebugDispObjectInfo* objInfo;

    while (dispObj != NULL) {
        objInfo = &sDebugObjectInfoTable[dispObj->type];
        sDebugObjectDrawFuncTable[objInfo->drawType](dispObj, objInfo->drawArg, play);
        dispObj = dispObj->next;
    }
}

void DebugDisplay_DrawSpriteI8(DebugDispObject* dispObj, void* texture, PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL47_Xlu(play->state.gfxCtx);

    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, dispObj->color.r, dispObj->color.g, dispObj->color.b, dispObj->color.a);
    Matrix_Translate(dispObj->pos.x, dispObj->pos.y, dispObj->pos.z, MTXMODE_NEW);
    Matrix_Scale(dispObj->scale.x, dispObj->scale.y, dispObj->scale.z, MTXMODE_APPLY);
    Matrix_Mult(&play->billboardMtxF, MTXMODE_APPLY);
    Matrix_RotateZYX(dispObj->rot.x, dispObj->rot.y, dispObj->rot.z, MTXMODE_APPLY);

    gDPLoadTextureBlock(POLY_XLU_DISP++, texture, G_IM_FMT_I, G_IM_SIZ_8b, 16, 16, 0, G_TX_NOMIRROR | G_TX_WRAP,
                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);

    gSPDisplayList(POLY_XLU_DISP++, sDebugDisplaySpriteDL);

    CLOSE_DISPS(play->state.gfxCtx);
}

Lights1 sDebugDisplayLight1 = gdSPDefLights1(128, 128, 128, 255, 255, 255, 73, 73, 73);

void DebugDisplay_DrawPolygon(DebugDispObject* dispObj, void* dList, PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL4_Xlu(play->state.gfxCtx);

    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, dispObj->color.r, dispObj->color.g, dispObj->color.b, dispObj->color.a);

    gSPSetLights1(POLY_XLU_DISP++, sDebugDisplayLight1);

    Matrix_SetTranslateRotateYXZ(dispObj->pos.x, dispObj->pos.y, dispObj->pos.z, &dispObj->rot);
    Matrix_Scale(dispObj->scale.x, dispObj->scale.y, dispObj->scale.z, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);

    gSPDisplayList(POLY_XLU_DISP++, dList);
    CLOSE_DISPS(play->state.gfxCtx);
}

/**
 * Visualise a path, see DebugDisplay_PathDisplayList for details.
 */

void DebugDisplay_DrawPath(PlayState* play, Path* path) {
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL38_Xlu(play->state.gfxCtx);
    gSPMatrix(POLY_XLU_DISP++, &gIdentityMtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, DebugDisplay_PathDisplayList(play->state.gfxCtx, path));

    CLOSE_DISPS(play->state.gfxCtx);
}

#define R_DRAW_PATH_SEGMENT 0  // bREG(82)
#define R_DRAW_PATH_SCALE 1.0f // (1.0f + 0.1f * bREG(83))
#define R_DRAW_PATH_RED 0      // bREG(84)
#define R_DRAW_PATH_GREEN 0    // bREG(85)
#define R_DRAW_PATH_BLUE 0     // bREG(86)
#define R_DRAW_PATH_ALPHA 0    // bREG(87)

/**
 * Offsets of the points of the triaxial shape used for visualising paths
 */
Vec3s sDebugPathVtxOffsets[][2] = {
    { { 20, 0, 0 }, { -20, 0, 0 } },
    { { 0, 20, 0 }, { 0, -20, 0 } },
    { { 0, 0, 20 }, { 0, 0, -20 } },
};

/**
 * Constructs a displaylist to show a Path, by extruding the three-axis shape defined by `sPathDebugVtx` along it.
 */
Gfx* DebugDisplay_PathDisplayList(GraphicsContext* gfxCtx, Path* path) {
    Gfx* gfxHead;
    Gfx* gfx;
    Vtx* curVtx;
    Vtx* curBaseVtx;  // First Vtx for the current point
    Vtx* prevBaseVtx; // First Vtx for the previous point
    Vec3s* curPoint;
    s32 segment; // of path ending at the current point, 1-indexed

    if (path != NULL) {
        // (count - 1) segments, 1 gSPVertex and 3 gSP2Triangles for each, plus a gSPEndDisplayList
        gfx = GRAPH_ALLOC(gfxCtx, ((path->count - 1) * 4 + 1) * sizeof(Gfx));
        curVtx = GRAPH_ALLOC(gfxCtx, path->count * 6 * sizeof(Vtx));

        gfxHead = gfx;
        curBaseVtx = curVtx;
        curPoint = Lib_SegmentedToVirtual(path->points);

        for (segment = 0; segment < path->count; segment++, curPoint++) {
            s32 i;
            s32 j;

            prevBaseVtx = curBaseVtx;
            curBaseVtx = curVtx;

            // Add a vertex for each point in the triaxial shape
            for (i = 0; i < ARRAY_COUNT(sDebugPathVtxOffsets); i++) {
                for (j = 0; j < ARRAY_COUNT(sDebugPathVtxOffsets[0]); j++) {
                    curVtx->v.ob[0] = curPoint->x + (s32)(sDebugPathVtxOffsets[i][j].x * R_DRAW_PATH_SCALE);
                    curVtx->v.ob[1] = curPoint->y + (s32)(sDebugPathVtxOffsets[i][j].y * R_DRAW_PATH_SCALE);
                    curVtx->v.ob[2] = curPoint->z + (s32)(sDebugPathVtxOffsets[i][j].z * R_DRAW_PATH_SCALE);
                    curVtx->v.flag = 0;
                    curVtx->v.tc[0] = 0;
                    curVtx->v.tc[1] = 0;
                    curVtx->v.cn[0] = 128 + R_DRAW_PATH_RED;
                    curVtx->v.cn[1] = 128 + R_DRAW_PATH_GREEN;
                    curVtx->v.cn[2] = 128 + R_DRAW_PATH_BLUE;
                    curVtx->v.cn[3] = 128 + R_DRAW_PATH_ALPHA;
                    curVtx++;
                }
            }

            // Draw the path segment ending at point `R_DRAW_PATH_SEGMENT`, or the whole path if `R_DRAW_PATH_SEGMENT`
            // is 0
            if (R_DRAW_PATH_SEGMENT == 0) {
                if (segment > 0) {
                    gSPVertex(gfx++, prevBaseVtx, 12, 0);
                    gSP2Triangles(gfx++, 0, 7, 1, 0, 0, 6, 7, 0);
                    gSP2Triangles(gfx++, 2, 3, 8, 0, 3, 9, 8, 0);
                    gSP2Triangles(gfx++, 4, 11, 10, 0, 4, 5, 11, 0);
                }
            } else {
                if ((segment > 0) && (segment == R_DRAW_PATH_SEGMENT)) {
                    gSPVertex(gfx++, prevBaseVtx, 12, 0);
                    gSP2Triangles(gfx++, 0, 7, 1, 0, 0, 6, 7, 0);
                    gSP2Triangles(gfx++, 2, 3, 8, 0, 3, 9, 8, 0);
                    gSP2Triangles(gfx++, 4, 11, 10, 0, 4, 5, 11, 0);
                }
            }
        }

        gSPEndDisplayList(gfx++);
    } else {
        // No path, trivial displaylist
        gfx = gfxHead = GRAPH_ALLOC(gfxCtx, sizeof(Gfx));
        gSPEndDisplayList(gfx++);
    }
    return gfxHead;
}
