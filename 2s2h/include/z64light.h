#ifndef Z64LIGHT_H
#define Z64LIGHT_H

#include "ultra64.h"
#include "color.h"
#include "z64math.h"

struct GraphicsContext;
struct PlayState;

typedef struct AdjLightSettings {
    /* 0x00 */ s16 ambientColor[3];
    /* 0x06 */ s16 light1Color[3];
    /* 0x0C */ s16 light2Color[3];
    /* 0x12 */ s16 fogColor[3];
    /* 0x18 */ s16 fogNear;
    /* 0x1A */ s16 zFar;
} AdjLightSettings; // size = 0x1C

typedef struct LightPoint {
    /* 0x0 */ s16 x;
    /* 0x2 */ s16 y;
    /* 0x4 */ s16 z;
    /* 0x6 */ u8 color[3];
    /* 0x9 */ u8 drawGlow;
    /* 0xA */ s16 radius;
} LightPoint; // size = 0xC

typedef struct LightDirectional {
    /* 0x0 */ s8 x;
    /* 0x1 */ s8 y;
    /* 0x2 */ s8 z;
    /* 0x3 */ u8 color[3];
} LightDirectional; // size = 0x6

typedef union LightParams {
    LightPoint point;
    LightDirectional dir;
} LightParams; // size = 0xC

typedef struct LightInfo {
    /* 0x0 */ u8 type; // LightType enum
    /* 0x2 */ LightParams params;
} LightInfo; // size = 0xE

typedef struct Lights {
    /* 0x00 */ u8 enablePosLights;
    /* 0x01 */ u8 numLights;
    /* 0x08 */ Lightsn l;
} Lights; // size = 0x80

typedef struct LightNode {
    /* 0x0 */ LightInfo* info;
    /* 0x4 */ struct LightNode* prev;
    /* 0x8 */ struct LightNode* next;
} LightNode; // size = 0xC

#define LIGHTS_BUFFER_SIZE 32

typedef struct LightsBuffer {
    /* 0x000 */ s32 numOccupied;
    /* 0x004 */ s32 searchIndex;
    /* 0x008 */ LightNode lights[LIGHTS_BUFFER_SIZE];
} LightsBuffer; // size = 0x188

typedef struct LightContext {
    /* 0x0 */ LightNode* listHead;
    /* 0x4 */ u8 ambientColor[3];
    /* 0x7 */ u8 fogColor[3];
    /* 0xA */ s16 fogNear; // how close until fog starts taking effect. range 0 - 996
    /* 0xC */ s16 zFar; // draw distance. range 0 - 12800
} LightContext; // size = 0x10

typedef enum LightType {
    /* 0 */ LIGHT_POINT_NOGLOW,
    /* 1 */ LIGHT_DIRECTIONAL,
    /* 2 */ LIGHT_POINT_GLOW
} LightType;

void Lights_PointNoGlowSetInfo(LightInfo* info, s16 x, s16 y, s16 z, u8 r, u8 g, u8 b, s16 radius);
void Lights_PointGlowSetInfo(LightInfo* info, s16 x, s16 y, s16 z, u8 r, u8 g, u8 b, s16 radius);
void Lights_PointSetColorAndRadius(LightInfo* info, u8 r, u8 g, u8 b, s16 radius);
void Lights_PointSetPosition(LightInfo* info, s16 x, s16 y, s16 z);
void Lights_DirectionalSetInfo(LightInfo* info, s8 x, s8 y, s8 z, u8 r, u8 g, u8 b);
void Lights_Reset(Lights* lights, u8 r, u8 g, u8 b);
void Lights_Draw(Lights* lights, struct GraphicsContext* gfxCtx);
void Lights_BindAll(Lights* lights, LightNode* listHead, Vec3f* refPos, struct PlayState* play);
void LightContext_Init(struct PlayState* play, LightContext* lightCtx);
void LightContext_SetAmbientColor(LightContext* lightCtx, u8 r, u8 g, u8 b);
void LightContext_SetFog(LightContext* lightCtx, u8 r, u8 g, u8 b, s16 near, s16 far);
Lights* LightContext_NewLights(LightContext* lightCtx, struct GraphicsContext* gfxCtx);
void LightContext_InitList(struct PlayState* play, LightContext* lightCtx);
void LightContext_DestroyList(struct PlayState* play, LightContext* lightCtx);
LightNode* LightContext_InsertLight(struct PlayState* play, LightContext* lightCtx, LightInfo* info);
void LightContext_RemoveLight(struct PlayState* play, LightContext* lightCtx, LightNode* light);
Lights* Lights_NewAndDraw(struct GraphicsContext* gfxCtx, u8 ambientR, u8 ambientG, u8 ambientB, u8 numLights, u8 r, u8 g, u8 b, s8 x, s8 y, s8 z);
Lights* Lights_New(struct GraphicsContext* gfxCtx, u8 ambientR, u8 ambientG, u8 ambientB);
void Lights_GlowCheck(struct PlayState* play);
void Lights_DrawGlow(struct PlayState* play);

#endif
