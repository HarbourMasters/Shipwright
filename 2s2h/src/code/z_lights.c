#include "z64light.h"

#include "sys_cfb.h"
#include "z64skin_matrix.h"
#include "z64.h"
#include "functions.h"

#include "objects/gameplay_keep/gameplay_keep.h"
#include <string.h>
#include "BenPort.h"
#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"

LightsBuffer sLightsBuffer;

void Lights_PointSetInfo(LightInfo* info, s16 x, s16 y, s16 z, u8 r, u8 g, u8 b, s16 radius, LightType type) {
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

void Lights_PointSetPosition(LightInfo* info, s16 x, s16 y, s16 z) {
    info->params.point.x = x;
    info->params.point.y = y;
    info->params.point.z = z;
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

void Lights_Reset(Lights* lights, u8 r, u8 g, u8 b) {
    lights->l.a.l.colc[0] = r;
    lights->l.a.l.col[0] = r;
    lights->l.a.l.colc[1] = g;
    lights->l.a.l.col[1] = g;
    lights->l.a.l.colc[2] = b;
    lights->l.a.l.col[2] = b;
    lights->numLights = 0;
}

/*
 * Draws every light in the provided Lights group
 */
void Lights_Draw(Lights* lights, GraphicsContext* gfxCtx) {
    Light* l;
    s32 i;

    OPEN_DISPS(gfxCtx);

    gSPNumLights(POLY_OPA_DISP++, lights->numLights);
    gSPNumLights(POLY_XLU_DISP++, lights->numLights);

    l = lights->l.l;

    for (i = 0; i < lights->numLights;) {
        gSPLight(POLY_OPA_DISP++, l, ++i);
        gSPLight(POLY_XLU_DISP++, l++, i);
    }

    gSPLight(POLY_OPA_DISP++, &lights->l.a, ++i);
    gSPLight(POLY_XLU_DISP++, &lights->l.a, i);

    CLOSE_DISPS(gfxCtx);
}

Light* Lights_FindSlot(Lights* lights) {
    if (lights->numLights >= ARRAY_COUNT(lights->l.l)) {
        return NULL;
    }
    return &lights->l.l[lights->numLights++];
}

void Lights_BindPointWithReference(Lights* lights, LightParams* params, Vec3f* pos) {
    f32 xDiff;
    f32 yDiff;
    f32 zDiff;
    f32 posDiff;
    f32 scale;
    Light* light;

    if ((pos != NULL) && (params->point.radius >= 1)) {
        xDiff = params->point.x - pos->x;
        yDiff = params->point.y - pos->y;
        zDiff = params->point.z - pos->z;
        scale = params->point.radius;
        posDiff = SQ(xDiff) + SQ(yDiff) + SQ(zDiff);

        if (SQ(scale) > posDiff) {
            light = Lights_FindSlot(lights);

            if (light != NULL) {
                posDiff = sqrtf(posDiff);
                scale = posDiff / scale;
                scale = 1 - SQ(scale);

                light->l.col[0] = params->point.color[0] * scale;
                light->l.colc[0] = light->l.col[0];
                light->l.col[1] = params->point.color[1] * scale;
                light->l.colc[1] = light->l.col[1];
                light->l.col[2] = params->point.color[2] * scale;
                light->l.colc[2] = light->l.col[2];

                scale = (posDiff < 1) ? 120 : 120 / posDiff;

                light->l.dir[0] = xDiff * scale;
                light->l.dir[1] = yDiff * scale;
                light->l.dir[2] = zDiff * scale;
            }
        }
    }
}

void Lights_BindPoint(Lights* lights, LightParams* params, PlayState* play) {
    Light* light;
    f32 radiusF = params->point.radius;
    Vec3f posF;
    Vec3f adjustedPos;
    u32 pad;

    if (radiusF > 0) {
        posF.x = params->point.x;
        posF.y = params->point.y;
        posF.z = params->point.z;
        SkinMatrix_Vec3fMtxFMultXYZ(&play->viewProjectionMtxF, &posF, &adjustedPos);
        if ((adjustedPos.z > -radiusF) && (600 + radiusF > adjustedPos.z) && (400 > fabsf(adjustedPos.x) - radiusF) &&
            (400 > fabsf(adjustedPos.y) - radiusF)) {
            light = Lights_FindSlot(lights);
            if (light != NULL) {
                radiusF = 4500000.0f / (radiusF * radiusF);
                if (radiusF > 255) {
                    radiusF = 255;
                } else if (20 > radiusF) {
                    radiusF = 20;
                }

                light->p.col[0] = params->point.color[0];
                light->p.colc[0] = light->p.col[0];
                light->p.col[1] = params->point.color[1];
                light->p.colc[1] = light->p.col[1];
                light->p.col[2] = params->point.color[2];
                light->p.colc[2] = light->p.col[2];
                light->p.pos[0] = params->point.x;
                light->p.pos[1] = params->point.y;
                light->p.pos[2] = params->point.z;
                light->p.unk3 = 0x8;
                light->p.unk7 = 0xFF;
                light->p.unkE = (s32)radiusF;
            }
        }
    }
}

void Lights_BindDirectional(Lights* lights, LightParams* params, void* unused) {
    Light* light = Lights_FindSlot(lights);

    if (light != NULL) {
        light->l.col[0] = params->dir.color[0];
        light->l.colc[0] = light->l.col[0];
        light->l.col[1] = params->dir.color[1];
        light->l.colc[1] = light->l.col[1];
        light->l.col[2] = params->dir.color[2];
        light->l.colc[2] = light->l.col[2];
        light->l.dir[0] = params->dir.x;
        light->l.dir[1] = params->dir.y;
        light->l.dir[2] = params->dir.z;
        light->l.pad1 = 0; // TODO the fact that pad1 is set here means that it now does something in MM's microcode
    }
}

typedef void (*LightsBindFuncLegacy)(Lights* lights, LightParams* params, Vec3f* vec);
typedef void (*LightsBindFunc)(Lights* lights, LightParams* params, struct PlayState* play);

/**
 * For every light in a provided list, try to find a free slot in the provided Lights group and bind
 * a light to it. Then apply color and positional/directional info for each light
 * based on the parameters supplied by the node.
 *
 * Note: Lights in a given list can only be binded to however many free slots are
 * available in the Lights group. This is at most 7 slots for a new group, but could be less.
 *
 * Note: In F3DZEX2 versions that predate MM, microcode point lights didn't exist so `PointLight_t` could not be used.
 * Instead, fake point lights by using a directional light that constantly changes to face a reference position.
 * `sBindFuncs` maps to the new microcode point lights, and `sBindFuncsLegacy` maps to the old fake point lights.
 */
void Lights_BindAll(Lights* lights, LightNode* listHead, Vec3f* refPos, PlayState* play) {
    static LightsBindFunc sBindFuncs[] = {
        Lights_BindPoint,
        (LightsBindFunc)Lights_BindDirectional,
        Lights_BindPoint,
    };
    static LightsBindFuncLegacy sBindFuncsLegacy[] = {
        Lights_BindPointWithReference,
        (LightsBindFuncLegacy)Lights_BindDirectional,
        Lights_BindPointWithReference,
    };

    if (listHead != NULL) {
        if ((refPos == NULL) && (lights->enablePosLights == 1)) {
            do {
                sBindFuncs[listHead->info->type](lights, &listHead->info->params, play);
                listHead = listHead->next;
            } while (listHead != NULL);
        } else {
            do {
                sBindFuncsLegacy[listHead->info->type](lights, &listHead->info->params, refPos);
                listHead = listHead->next;
            } while (listHead != NULL);
        }
    }
}

LightNode* Lights_FindBufSlot(void) {
    LightNode* ret;

    if (sLightsBuffer.numOccupied >= LIGHTS_BUFFER_SIZE) {
        return NULL;
    }

    ret = &sLightsBuffer.lights[sLightsBuffer.searchIndex];
    while (ret->info != NULL) {
        sLightsBuffer.searchIndex++;
        if (sLightsBuffer.searchIndex < LIGHTS_BUFFER_SIZE) {
            ret++;
        } else {
            sLightsBuffer.searchIndex = 0;
            ret = &sLightsBuffer.lights[0];
        }
    }

    sLightsBuffer.numOccupied++;
    return ret;
}

void Lights_FreeNode(LightNode* light) {
    if (light == NULL) {
        return;
    }

    sLightsBuffer.numOccupied--;
    light->info = NULL;

    //! @bug Due to pointer arithmetic, the division is unnecessary
    sLightsBuffer.searchIndex = (light - sLightsBuffer.lights) / (s32)sizeof(LightNode);
}

void LightContext_Init(PlayState* play, LightContext* lightCtx) {
    LightContext_InitList(play, lightCtx);
    LightContext_SetAmbientColor(lightCtx, 80, 80, 80);
    LightContext_SetFog(lightCtx, 0, 0, 0, 996, 12800);
    memset(&sLightsBuffer, 0, sizeof(LightsBuffer));
}

void LightContext_SetAmbientColor(LightContext* lightCtx, u8 r, u8 g, u8 b) {
    lightCtx->ambientColor[0] = r;
    lightCtx->ambientColor[1] = g;
    lightCtx->ambientColor[2] = b;
}

void LightContext_SetFog(LightContext* lightCtx, u8 r, u8 g, u8 b, s16 near, s16 far) {
    lightCtx->fogColor[0] = r;
    lightCtx->fogColor[1] = g;
    lightCtx->fogColor[2] = b;
    lightCtx->fogNear = near;
    lightCtx->zFar = far;
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
    LightNode* light;

    light = Lights_FindBufSlot();
    if (light != NULL) {
        light->info = info;
        light->prev = NULL;
        light->next = lightCtx->listHead;

        if (lightCtx->listHead != NULL) {
            lightCtx->listHead->prev = light;
        }

        lightCtx->listHead = light;
    }

    return light;
}

void LightContext_RemoveLight(PlayState* play, LightContext* lightCtx, LightNode* light) {
    if (light != NULL) {
        if (light->prev != NULL) {
            light->prev->next = light->next;
        } else {
            lightCtx->listHead = light->next;
        }

        if (light->next != NULL) {
            light->next->prev = light->prev;
        }

        Lights_FreeNode(light);
    }
}

Lights* Lights_NewAndDraw(GraphicsContext* gfxCtx, u8 ambientR, u8 ambientG, u8 ambientB, u8 numLights, u8 r, u8 g,
                          u8 b, s8 x, s8 y, s8 z) {
    Lights* lights;
    s32 i;

    lights = GRAPH_ALLOC(gfxCtx, sizeof(Lights));

    lights->l.a.l.col[0] = lights->l.a.l.colc[0] = ambientR;
    lights->l.a.l.col[1] = lights->l.a.l.colc[1] = ambientG;
    lights->l.a.l.col[2] = lights->l.a.l.colc[2] = ambientB;
    lights->enablePosLights = false;
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

    lights = GRAPH_ALLOC(gfxCtx, sizeof(Lights));

    lights->l.a.l.col[0] = lights->l.a.l.colc[0] = ambientR;
    lights->l.a.l.col[1] = lights->l.a.l.colc[1] = ambientG;
    lights->l.a.l.col[2] = lights->l.a.l.colc[2] = ambientB;
    lights->enablePosLights = false;
    lights->numLights = 0;

    return lights;
}

// 2S2H [Port] Similar to Lights_GlowCheck, but only submits the coordinates to the depth prepare
// for batching and then will be read after when Lights_GlowCheck is called
void Lights_GlowCheckPrepare(PlayState* play) {
    LightNode* light = play->lightCtx.listHead;

    while (light != NULL) {
        LightPoint* params = &light->info->params.point;

        if (light->info->type == LIGHT_POINT_GLOW) {
            Vec3f worldPos;
            Vec3f projectedPos;
            f32 invW;

            worldPos.x = params->x;
            worldPos.y = params->y;
            worldPos.z = params->z;
            Actor_GetProjectedPos(play, &worldPos, &projectedPos, &invW);

            if ((projectedPos.z > 1) && (fabsf(projectedPos.x * invW) < 1) && (fabsf(projectedPos.y * invW) < 1)) {
                s32 screenPosX = PROJECTED_TO_SCREEN_X(projectedPos, invW);
                s32 screenPosY = PROJECTED_TO_SCREEN_Y(projectedPos, invW);
                OTRGetPixelDepthPrepare(screenPosX, screenPosY);
            }
        }

        light = light->next;
    }
}

void Lights_GlowCheck(PlayState* play) {
    LightNode* light = play->lightCtx.listHead;

    while (light != NULL) {
        LightPoint* params = &light->info->params.point;

        if (light->info->type == LIGHT_POINT_GLOW) {
            Vec3f worldPos;
            Vec3f projectedPos;
            f32 invW;

            worldPos.x = params->x;
            worldPos.y = params->y;
            worldPos.z = params->z;
            Actor_GetProjectedPos(play, &worldPos, &projectedPos, &invW);

            params->drawGlow = false;

            if ((projectedPos.z > 1) && (fabsf(projectedPos.x * invW) < 1) && (fabsf(projectedPos.y * invW) < 1)) {
                s32 screenPosX = PROJECTED_TO_SCREEN_X(projectedPos, invW);
                s32 screenPosY = PROJECTED_TO_SCREEN_Y(projectedPos, invW);
                s32 wZ = (s32)(projectedPos.z * invW * ((G_MAXZ / 2) * 32)) + ((G_MAXZ / 2) * 32);
                s32 zBuf = SysCfb_GetZBufferInt(screenPosX, screenPosY);

                if (wZ < zBuf) {
                    params->drawGlow = true;
                }
            }
        }

        light = light->next;
    }
}

void Lights_DrawGlow(PlayState* play) {
    Gfx* dl;
    LightPoint* params;
    LightNode* light = play->lightCtx.listHead;

    if (light != NULL) {
        OPEN_DISPS(play->state.gfxCtx);

        dl = Gfx_SetupDL65_NoCD(POLY_XLU_DISP);

        gDPSetDither(dl++, G_CD_NOISE);

        gDPSetCombineLERP(dl++, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE,
                          0);

        gSPDisplayList(dl++, gameplay_keep_DL_029CB0);

        do {
            if (light->info->type == LIGHT_POINT_GLOW) {
                params = &light->info->params.point;
                if (params->drawGlow) {
                    f32 scale = SQ((f32)params->radius) * 2e-6f;

                    FrameInterpolation_RecordOpenChild(light, 0);
                    gDPSetPrimColor(dl++, 0, 0, params->color[0], params->color[1], params->color[2], 50);

                    Matrix_Translate(params->x, params->y, params->z, MTXMODE_NEW);
                    Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);

                    MATRIX_FINALIZE_AND_LOAD(dl++, play->state.gfxCtx);

                    gSPDisplayList(dl++, gameplay_keep_DL_029CF0);
                    FrameInterpolation_RecordCloseChild();
                }
            }

            light = light->next;
        } while (light != NULL);

        POLY_XLU_DISP = dl;

        CLOSE_DISPS(play->state.gfxCtx);
    }
}
