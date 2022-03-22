#include "global.h"
#include "objects/gameplay_keep/gameplay_keep.h"

typedef struct {
    /* 0x00 */ s16 drawType;  // indicates which draw function to use when displaying the object
    /* 0x04 */ void* drawArg; // segment address (display list or texture) passed to the draw function when called
} DebugDispObjectInfo;        // size = 0x8

typedef void (*DebugDispObject_DrawFunc)(DebugDispObject*, void*, GlobalContext*);

void DebugDisplay_DrawSpriteI8(DebugDispObject* dispObj, void* texture, GlobalContext* globalCtx);
void DebugDisplay_DrawPolygon(DebugDispObject* dispObj, void* dlist, GlobalContext* globalCtx);

static DebugDispObject_DrawFunc sDebugObjectDrawFuncTable[] = {
    DebugDisplay_DrawSpriteI8,
    DebugDisplay_DrawPolygon,
};

static DebugDispObjectInfo sDebugObjectInfoTable[] = {
    { 0, gDebugCircleTex }, { 0, gDebugCrossTex }, { 0, gDebugBallTex },
    { 0, gDebugCursorTex }, { 1, gDebugArrowDL },  { 1, gDebugCameraDL },
};

static Lights1 sDebugObjectLights = gdSPDefLights1(0x80, 0x80, 0x80, 0xFF, 0xFF, 0xFF, 0x49, 0x49, 0x49);

static DebugDispObject* sDebugObjectListHead;

void DebugDisplay_Init(void) {
    sDebugObjectListHead = NULL;
}

DebugDispObject* DebugDisplay_AddObject(f32 posX, f32 posY, f32 posZ, s16 rotX, s16 rotY, s16 rotZ, f32 scaleX,
                                        f32 scaleY, f32 scaleZ, u8 red, u8 green, u8 blue, u8 alpha, s16 type,
                                        GraphicsContext* gfxCtx) {
    DebugDispObject* prevHead = sDebugObjectListHead;

    sDebugObjectListHead = Graph_Alloc(gfxCtx, sizeof(DebugDispObject));

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
    sDebugObjectListHead->next = prevHead;

    return sDebugObjectListHead;
}

void DebugDisplay_DrawObjects(GlobalContext* globalCtx) {
    DebugDispObject* dispObj = sDebugObjectListHead;
    DebugDispObjectInfo* objInfo;

    while (dispObj != NULL) {
        objInfo = &sDebugObjectInfoTable[dispObj->type];
        sDebugObjectDrawFuncTable[objInfo->drawType](dispObj, objInfo->drawArg, globalCtx);
        dispObj = dispObj->next;
    }
}

void DebugDisplay_DrawSpriteI8(DebugDispObject* dispObj, void* texture, GlobalContext* globalCtx) {
    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_debug_display.c", 169);

    func_80094678(globalCtx->state.gfxCtx);

    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, dispObj->color.r, dispObj->color.g, dispObj->color.b, dispObj->color.a);

    Matrix_Translate(dispObj->pos.x, dispObj->pos.y, dispObj->pos.z, MTXMODE_NEW);
    Matrix_Scale(dispObj->scale.x, dispObj->scale.y, dispObj->scale.z, MTXMODE_APPLY);
    Matrix_Mult(&globalCtx->billboardMtxF, MTXMODE_APPLY);
    Matrix_RotateZYX(dispObj->rot.x, dispObj->rot.y, dispObj->rot.z, MTXMODE_APPLY);

    gDPLoadTextureBlock(POLY_XLU_DISP++, texture, G_IM_FMT_I, G_IM_SIZ_8b, 16, 16, 0, G_TX_NOMIRROR | G_TX_WRAP,
                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_debug_display.c", 189),
              G_MTX_MODELVIEW | G_MTX_LOAD);
    gSPDisplayList(POLY_XLU_DISP++, gDebugSpriteDL);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_debug_display.c", 192);
}

void DebugDisplay_DrawPolygon(DebugDispObject* dispObj, void* dlist, GlobalContext* globalCtx) {
    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_debug_display.c", 211);

    func_8009435C(globalCtx->state.gfxCtx);

    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, dispObj->color.r, dispObj->color.g, dispObj->color.b, dispObj->color.a);

    gSPSetLights1(POLY_XLU_DISP++, sDebugObjectLights);

    Matrix_SetTranslateRotateYXZ(dispObj->pos.x, dispObj->pos.y, dispObj->pos.z, &dispObj->rot);
    Matrix_Scale(dispObj->scale.x, dispObj->scale.y, dispObj->scale.z, MTXMODE_APPLY);

    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_debug_display.c", 228),
              G_MTX_MODELVIEW | G_MTX_LOAD);
    gSPDisplayList(POLY_XLU_DISP++, dlist);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_debug_display.c", 231);
}
