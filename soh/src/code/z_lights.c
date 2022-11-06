#include "global.h"

#include <string.h>

#include "objects/gameplay_keep/gameplay_keep.h"

#include "soh/frame_interpolation.h"

#define LIGHTS_BUFFER_SIZE 32
//#define LIGHTS_BUFFER_SIZE 1024 // Kill me

typedef struct {
    /* 0x000 */ s32 numOccupied;
    /* 0x004 */ s32 searchIndex;
    /* 0x008 */ LightNode buf[LIGHTS_BUFFER_SIZE];
} LightsBuffer; // size = 0x188

LightsBuffer sLightsBuffer;

void Lights_PointSetInfo(LightInfo* info, s16 x, s16 y, s16 z, u8 r, u8 g, u8 b, s16 radius, s32 type) {
    info->type = type;
    info->params.point.x = x;
    info->params.point.y = y;
    info->params.point.z = z;
    Lights_PointSetColorAndRadius(info, r, g, b, radius);
}

void Lights_PointNoGlowSetInfo(LightInfo* info, s16 x, s16 y, s16 z, u8 r, u8 g, u8 b, s16 radius) {
    Lights_PointSetInfo(info, x, y, z, r, g, b, radius, LIGHT_POINT_NOGLOW);
}

void Lights_PointGlowSetInfo(LightInfo* info, s16 x, s16 y, s16 z, u8 r, u8 g, u8 b, s16 radius) {
    Lights_PointSetInfo(info, x, y, z, r, g, b, radius, LIGHT_POINT_GLOW);
}

void Lights_PointSetColorAndRadius(LightInfo* info, u8 r, u8 g, u8 b, s16 radius) {
    info->params.point.color[0] = r;
    info->params.point.color[1] = g;
    info->params.point.color[2] = b;
    info->params.point.radius = radius;
}

void Lights_DirectionalSetInfo(LightInfo* info, s8 x, s8 y, s8 z, u8 r, u8 g, u8 b) {
    info->type = LIGHT_DIRECTIONAL;
    info->params.dir.x = x;
    info->params.dir.y = y;
    info->params.dir.z = z;
    info->params.dir.color[0] = r;
    info->params.dir.color[1] = g;
    info->params.dir.color[2] = b;
}

// unused
void Lights_Reset(Lights* lights, u8 ambentR, u8 ambentG, u8 ambentB) {
    lights->l.a.l.col[0] = lights->l.a.l.colc[0] = ambentR;
    lights->l.a.l.col[1] = lights->l.a.l.colc[1] = ambentG;
    lights->l.a.l.col[2] = lights->l.a.l.colc[2] = ambentB;
    lights->numLights = 0;
}

/*
 * Draws every light in the provided Lights group
 */
void Lights_Draw(Lights* lights, GraphicsContext* gfxCtx) {
    Light* light;
    s32 i;

#if 1

    OPEN_DISPS(gfxCtx);

    gSPNumLights(POLY_OPA_DISP++, lights->numLights);
    gSPNumLights(POLY_XLU_DISP++, lights->numLights);

    i = 0;
    light = &lights->l.l[0];

    while (i < lights->numLights) {
        i++;
        gSPLight(POLY_OPA_DISP++, light, i);
        gSPLight(POLY_XLU_DISP++, light, i);
        light++;
    }

    i++; // abmient light is total number of lights + 1
    gSPLight(POLY_OPA_DISP++, &lights->l.a, i);
    gSPLight(POLY_XLU_DISP++, &lights->l.a, i);

    CLOSE_DISPS(gfxCtx);
#endif
}

Light* Lights_FindSlot(Lights* lights) {
    if (lights->numLights >= 7) {
        return NULL;
    } else {
        return &lights->l.l[lights->numLights++];
    }
}

void Lights_BindPoint(Lights* lights, LightParams* params, Vec3f* vec) {
    f32 xDiff;
    f32 yDiff;
    f32 zDiff;
    f32 posDiff;
    f32 scale;
    Light* light;

    if (vec != NULL) {
        xDiff = params->point.x - vec->x;
        yDiff = params->point.y - vec->y;
        zDiff = params->point.z - vec->z;
        scale = params->point.radius;
        posDiff = SQ(xDiff) + SQ(yDiff) + SQ(zDiff);

        if (posDiff < SQ(scale)) {
            light = Lights_FindSlot(lights);

            if (light != NULL) {
                posDiff = sqrtf(posDiff);
                scale = posDiff / scale;
                scale = 1 - SQ(scale);

                light->l.col[0] = light->l.colc[0] = params->point.color[0] * scale;
                light->l.col[1] = light->l.colc[1] = params->point.color[1] * scale;
                light->l.col[2] = light->l.colc[2] = params->point.color[2] * scale;

                scale = (posDiff < 1.0f) ? 120.0f : 120.0f / posDiff;

                light->l.dir[0] = xDiff * scale;
                light->l.dir[1] = yDiff * scale;
                light->l.dir[2] = zDiff * scale;
            }
        }
    }
}

void Lights_BindDirectional(Lights* lights, LightParams* params, Vec3f* vec) {
    Light* light = Lights_FindSlot(lights);

    if (light != NULL) {
        light->l.col[0] = light->l.colc[0] = params->dir.color[0];
        light->l.col[1] = light->l.colc[1] = params->dir.color[1];
        light->l.col[2] = light->l.colc[2] = params->dir.color[2];
        light->l.dir[0] = params->dir.x;
        light->l.dir[1] = params->dir.y;
        light->l.dir[2] = params->dir.z;
    }
}

/**
 * For every light in a provided list, try to find a free slot in the provided Lights group and bind
 * a light to it. Then apply color and positional/directional info for each light
 * based on the parameters supplied by the node.
 *
 * Note: Lights in a given list can only be binded to however many free slots are
 * available in the Lights group. This is at most 7 slots for a new group, but could be less.
 */
void Lights_BindAll(Lights* lights, LightNode* listHead, Vec3f* vec) {
    LightsBindFunc bindFuncs[] = { Lights_BindPoint, Lights_BindDirectional, Lights_BindPoint };
    LightInfo* info;

    while (listHead != NULL) {
        info = listHead->info;
        // OTRTODO: we do not know the root cause of the info->type value being invalid
        // but this prevents it from crashing the game on the game over screen
        if (info->type < 3) {
            bindFuncs[info->type](lights, &info->params, vec);
        }
        listHead = listHead->next;
    }
}

LightNode* Lights_FindBufSlot() {
    LightNode* node;

    if (sLightsBuffer.numOccupied >= LIGHTS_BUFFER_SIZE) {
        return NULL;
    }

    node = &sLightsBuffer.buf[sLightsBuffer.searchIndex];

    while (node->info != NULL) {
        sLightsBuffer.searchIndex++;

        if (sLightsBuffer.searchIndex < LIGHTS_BUFFER_SIZE) {
            node++;
        } else {
            sLightsBuffer.searchIndex = 0;
            node = &sLightsBuffer.buf[0];
        }
    }

    sLightsBuffer.numOccupied++;

    return node;
}

// return type must not be void to match
s32 Lights_FreeNode(LightNode* light) {
    if (light != NULL) {
        sLightsBuffer.numOccupied--;
        light->info = NULL;
        sLightsBuffer.searchIndex = (light - sLightsBuffer.buf) / sizeof(LightNode);
    }
}

void LightContext_Init(PlayState* play, LightContext* lightCtx) {
    LightContext_InitList(play, lightCtx);
    LightContext_SetAmbientColor(lightCtx, 80, 80, 80);
    LightContext_SetFog(lightCtx, 0, 0, 0, 996, 12800);
    memset(&sLightsBuffer, 0, sizeof(sLightsBuffer));
}

void LightContext_SetAmbientColor(LightContext* lightCtx, u8 r, u8 g, u8 b) {
    lightCtx->ambientColor[0] = r;
    lightCtx->ambientColor[1] = g;
    lightCtx->ambientColor[2] = b;
}

void LightContext_SetFog(LightContext* lightCtx, u8 r, u8 g, u8 b, s16 fogNear, s16 fogFar) {
    lightCtx->fogColor[0] = r;
    lightCtx->fogColor[1] = g;
    lightCtx->fogColor[2] = b;
    lightCtx->fogNear = fogNear;
    lightCtx->fogFar = fogFar;
}

/**
 * Allocate a new Lights group and initilize the ambient color with that provided by LightContext
 */
Lights* LightContext_NewLights(LightContext* lightCtx, GraphicsContext* gfxCtx) {
    return Lights_New(gfxCtx, lightCtx->ambientColor[0], lightCtx->ambientColor[1], lightCtx->ambientColor[2]);
}

void LightContext_InitList(PlayState* play, LightContext* lightCtx) {
    lightCtx->listHead = NULL;
}

void LightContext_DestroyList(PlayState* play, LightContext* lightCtx) {
    while (lightCtx->listHead != NULL) {
        LightContext_RemoveLight(play, lightCtx, lightCtx->listHead);
        lightCtx->listHead = lightCtx->listHead->next;
    }
}

/**
 * Insert a new light into the list pointed to by LightContext
 *
 * Note: Due to the limited number of slots in a Lights group, inserting too many lights in the
 * list may result in older entries not being bound to a Light when calling Lights_BindAll
 */
LightNode* LightContext_InsertLight(PlayState* play, LightContext* lightCtx, LightInfo* info) {
    LightNode* node;

    node = Lights_FindBufSlot();

    if (node != NULL) {
        node->info = info;
        node->prev = NULL;
        node->next = lightCtx->listHead;

        if (lightCtx->listHead != NULL) {
            lightCtx->listHead->prev = node;
        }

        lightCtx->listHead = node;
    }

    return node;
}

void LightContext_RemoveLight(PlayState* play, LightContext* lightCtx, LightNode* node) {
    if (node != NULL) {
        if (node->prev != NULL) {
            node->prev->next = node->next;
        } else {
            lightCtx->listHead = node->next;
        }

        if (node->next != NULL) {
            node->next->prev = node->prev;
        }

        Lights_FreeNode(node);
    }
}

// unused
Lights* Lights_NewAndDraw(GraphicsContext* gfxCtx, u8 ambientR, u8 ambientG, u8 ambientB, u8 numLights, u8 r, u8 g,
                          u8 b, s8 x, s8 y, s8 z) {
    Lights* lights;
    s32 i;

    lights = Graph_Alloc(gfxCtx, sizeof(Lights));

    lights->l.a.l.col[0] = lights->l.a.l.colc[0] = ambientR;
    lights->l.a.l.col[1] = lights->l.a.l.colc[1] = ambientG;
    lights->l.a.l.col[2] = lights->l.a.l.colc[2] = ambientB;
    lights->numLights = numLights;

    for (i = 0; i < numLights; i++) {
        lights->l.l[i].l.col[0] = lights->l.l[i].l.colc[0] = r;
        lights->l.l[i].l.col[1] = lights->l.l[i].l.colc[1] = g;
        lights->l.l[i].l.col[2] = lights->l.l[i].l.colc[2] = b;
        lights->l.l[i].l.dir[0] = x;
        lights->l.l[i].l.dir[1] = y;
        lights->l.l[i].l.dir[2] = z;
    }

    Lights_Draw(lights, gfxCtx);

    return lights;
}

Lights* Lights_New(GraphicsContext* gfxCtx, u8 ambientR, u8 ambientG, u8 ambientB) {
    Lights* lights;

    lights = Graph_Alloc(gfxCtx, sizeof(Lights));

    lights->l.a.l.col[0] = lights->l.a.l.colc[0] = ambientR;
    lights->l.a.l.col[1] = lights->l.a.l.colc[1] = ambientG;
    lights->l.a.l.col[2] = lights->l.a.l.colc[2] = ambientB;
    lights->numLights = 0;

    return lights;
}

void Lights_GlowCheckPrepare(PlayState* play) {
    LightNode* node;
    LightPoint* params;
    Vec3f pos;
    Vec3f multDest;
    f32 wDest;
    f32 wX;
    f32 wY;

    node = play->lightCtx.listHead;

    while (node != NULL) {
        params = &node->info->params.point;

        if (node->info->type == LIGHT_POINT_GLOW) {
            f32 x, y;
            u32 shrink;
            uint32_t height;

            pos.x = params->x;
            pos.y = params->y;
            pos.z = params->z;
            func_8002BE04(play, &pos, &multDest, &wDest);
            wX = multDest.x * wDest;
            wY = multDest.y * wDest;

            x = wX * 160 + 160;
            y = wY * 120 + 120;
            shrink = ShrinkWindow_GetCurrentVal();

            if ((multDest.z > 1.0f) && y >= shrink && y <= SCREEN_HEIGHT - shrink) {
                OTRGetPixelDepthPrepare(x, y);
            }
        }
        node = node->next;
    }
}

void Lights_GlowCheck(PlayState* play) {
    LightNode* node;
    LightPoint* params;
    Vec3f pos;
    Vec3f multDest;
    f32 wDest;
    f32 wX;
    f32 wY;
    s32 wZ;
    s32 zBuf;

    node = play->lightCtx.listHead;

    while (node != NULL) {
        params = &node->info->params.point;

        if (node->info->type == LIGHT_POINT_GLOW) {
            f32 x, y;
            u32 shrink;
            uint32_t height;

            pos.x = params->x;
            pos.y = params->y;
            pos.z = params->z;
            func_8002BE04(play, &pos, &multDest, &wDest);
            params->drawGlow = false;
            wX = multDest.x * wDest;
            wY = multDest.y * wDest;

            x = wX * 160 + 160;
            y = wY * 120 + 120;
            shrink = ShrinkWindow_GetCurrentVal();

            if ((multDest.z > 1.0f) && y >= shrink && y <= SCREEN_HEIGHT - shrink) {
                wZ = (s32)((multDest.z * wDest) * 16352.0f) + 16352;
                zBuf = OTRGetPixelDepth(x, y) * 4;
        
                if (wZ < (zBuf >> 3)) {
                    params->drawGlow = true;
                }
            }
        }
        node = node->next;
    }
}

void Lights_DrawGlow(PlayState* play) {
    s32 pad;
    LightNode* node;

    node = play->lightCtx.listHead;

    OPEN_DISPS(play->state.gfxCtx);

    POLY_XLU_DISP = func_800947AC(POLY_XLU_DISP++);
    gDPSetAlphaDither(POLY_XLU_DISP++, G_AD_NOISE);
    gDPSetColorDither(POLY_XLU_DISP++, G_CD_MAGICSQ);
    gSPDisplayList(POLY_XLU_DISP++, gGlowCircleTextureLoadDL);

    while (node != NULL) {
        LightInfo* info;
        LightPoint* params;
        f32 scale;
        s32 pad[4];

        info = node->info;
        params = &info->params.point;

        if ((info->type == LIGHT_POINT_GLOW) && (params->drawGlow)) {
            scale = SQ(params->radius) * 0.0000026f;

            FrameInterpolation_RecordOpenChild(node, 0);
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, params->color[0], params->color[1], params->color[2], 50);
            Matrix_Translate(params->x, params->y, params->z, MTXMODE_NEW);
            Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, gGlowCircleDL);
            FrameInterpolation_RecordCloseChild();
        }

        node = node->next;
    }

    CLOSE_DISPS(play->state.gfxCtx);
}
