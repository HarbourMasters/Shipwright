#ifndef Z64SKYBOX_H
#define Z64SKYBOX_H

#include "PR/ultratypes.h"
#include "PR/gbi.h"
#include "color.h"

#include "z64dma.h"
#include "z64math.h"
#include "z64view.h"

struct GraphicsContext;
struct GameState;
struct PlayState;

typedef enum SkyboxId {
    /* 0 */ SKYBOX_NONE,
    /* 1 */ SKYBOX_NORMAL_SKY,
    /* 2 */ SKYBOX_2,
    /* 3 */ SKYBOX_3,
    /* 5 */ SKYBOX_CUTSCENE_MAP = 5
} SkyboxId;

typedef struct SkyboxContext {
    /* 0x000 */ View view;
    /* 0x168 */ void* staticSegments[2][6]; // 2S2H [Port] Changed to hold enough pointers to OTR resources
    /* 0x178 */ void* palette;
    /* 0x17C */ Gfx (*dListBuf)[150];
    /* 0x180 */ Gfx* gfx;
    /* 0x184 */ Vtx* roomVtx;
    /* 0x188 */ DmaRequest skybox1DmaRequest;
    /* 0x1A8 */ DmaRequest skybox2DmaRequest;
    /* 0x1C8 */ DmaRequest paletteDmaRequest;
    /* 0x1E8 */ OSMesgQueue loadQueue;
    /* 0x200 */ OSMesg loadMsg[1];
    /* 0x204 */ s16 shouldDraw;
    /* 0x208 */ Vec3f rot;
    /* 0x214 */ Vec3f eye;
    /* 0x220 */ s16 angle;
    /* 0x222 */ Color_RGB8 prim;
    /* 0x225 */ Color_RGB8 env;
} SkyboxContext; // size = 0x228

void Skybox_Reload(struct PlayState* play, SkyboxContext* skyboxCtx, s16 skyboxId);
void Skybox_Init(struct GameState* gameState, SkyboxContext* skyboxCtx, s16 skyboxId);
Mtx* Skybox_UpdateMatrix(SkyboxContext* skyboxCtx, f32 x, f32 y, f32 z);
void Skybox_SetColors(SkyboxContext* skyboxCtx, u8 primR, u8 primG, u8 primB, u8 envR, u8 envG, u8 envB);
void Skybox_Draw(SkyboxContext* skyboxCtx, struct GraphicsContext* gfxCtx, s16 skyboxId, s16 blend, f32 x, f32 y,
                 f32 z);
void Skybox_Update(SkyboxContext* skyboxCtx);
void Skybox_Calculate128(SkyboxContext* skyboxCtx, s32 nFaces);

// #region 2S2H [Port]
typedef enum SkyboxTexturesIndex {
    /* 0 */ SKYBOX_TEXTURES_FINE,
    /* 1 */ SKYBOX_TEXTURES_CLOUD,
} SkyboxTexturesIndex;

extern TexturePtr sSkyboxTextures[2][6];
// #endregion

#endif
