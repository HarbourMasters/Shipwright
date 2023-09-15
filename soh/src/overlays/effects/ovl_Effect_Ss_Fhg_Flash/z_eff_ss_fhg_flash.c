/*
 * File: z_eff_ss_fhg_flash.c
 * Overlay: ovl_Effect_Ss_Fhg_Flash
 * Description: Shock and Light Ball Effect
 */

#include "z_eff_ss_fhg_flash.h"
#include "overlays/actors/ovl_Boss_Ganondrof/z_boss_ganondrof.h"
#include "objects/object_fhg/object_fhg.h"

#define rAlpha regs[0]
#define rObjBankIdx regs[2]
#define rXZRot regs[3]
#define rParam regs[4]
#define rScale regs[8]

u32 EffectSsFhgFlash_Init(PlayState* play, u32 index, EffectSs* this, void* initParamsx);
void EffectSsFhgFlash_DrawLightBall(PlayState* play, u32 index, EffectSs* this);
void EffectSsFhgFlash_UpdateLightBall(PlayState* play, u32 index, EffectSs* this);
void EffectSsFhgFlash_DrawShock(PlayState* play, u32 index, EffectSs* this);
void EffectSsFhgFlash_UpdateShock(PlayState* play, u32 index, EffectSs* this);

EffectSsInit Effect_Ss_Fhg_Flash_InitVars = {
    EFFECT_SS_FHG_FLASH,
    EffectSsFhgFlash_Init,
};

static UNK_TYPE D_809A5178[258];
static Gfx D_809A5100[15];

u32 EffectSsFhgFlash_Init(PlayState* play, u32 index, EffectSs* this, void* initParamsx) {
    EffectSsFhgFlashInitParams* initParams = (EffectSsFhgFlashInitParams*)initParamsx;
    s32 pad;
    s32 objBankIdx;
    Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };
    Vec3f sp34 = { 0.0f, -1000.0f, 0.0f };
    void* oldSeg6;

    if (initParams->type == FHGFLASH_LIGHTBALL) {
        objBankIdx = Object_GetIndex(&play->objectCtx, OBJECT_FHG);

        if ((objBankIdx > -1) && Object_IsLoaded(&play->objectCtx, objBankIdx)) {
            oldSeg6 = gSegments[6];
            gSegments[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.status[objBankIdx].segment);
            this->rObjBankIdx = objBankIdx;
            this->pos = initParams->pos;
            this->velocity = initParams->velocity;
            this->accel = initParams->accel;
            this->rParam = initParams->param;
            this->life = 100;
            this->rScale = initParams->scale;
            this->rAlpha = 255;
            this->draw = EffectSsFhgFlash_DrawLightBall;
            this->update = EffectSsFhgFlash_UpdateLightBall;
            this->gfx = SEGMENTED_TO_VIRTUAL(gPhantomEnergyBallDL);
            gSegments[6] = oldSeg6;
        } else {
            osSyncPrintf("Effect_Ss_Fhg_Flash_ct():pffd->modeエラー\n");
            return 0;
        }
    } else {
        this->actor = initParams->actor;
        this->velocity = this->accel = zeroVec;
        this->life = (s16)(Rand_ZeroOne() * 10.0f) + 111;
        this->rScale = (s16)Rand_ZeroFloat(initParams->scale) + initParams->scale;
        this->rAlpha = 255;
        this->draw = EffectSsFhgFlash_DrawShock;
        this->update = EffectSsFhgFlash_UpdateShock;
        this->rParam = initParams->param;

        if (initParams->param != FHGFLASH_SHOCK_NO_ACTOR) {
            this->pos = sp34;
            this->gfx = SEGMENTED_TO_VIRTUAL(D_809A5100);
        } else {
            this->pos = initParams->pos;
            this->gfx = SEGMENTED_TO_VIRTUAL(D_809A5100);
        }
    }
    return 1;
}

static Color_RGB8 sColors[] = {
    { 165, 255, 61 }, { 0, 255, 255 }, { 255, 40, 0 }, { 255, 255, 0 }, { 0, 0, 255 },
    { 255, 0, 255 },  { 255, 150, 0 }, { 0, 0, 0 },    { 0, 0, 0 },
};

void EffectSsFhgFlash_DrawLightBall(PlayState* play, u32 index, EffectSs* this) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    s32 pad;
    f32 scale;
    void* object;

    scale = this->rScale / 100.0f;
    object = play->objectCtx.status[this->rObjBankIdx].segment;

    OPEN_DISPS(gfxCtx);

    Matrix_Translate(this->pos.x, this->pos.y, this->pos.z, MTXMODE_NEW);
    Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
    gSegments[6] = VIRTUAL_TO_PHYSICAL(object);
    gSPSegment(POLY_XLU_DISP++, 0x06, object);
    Gfx_SetupDL_25Xlu(play->state.gfxCtx);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, this->rAlpha);
    gDPSetEnvColor(POLY_XLU_DISP++, sColors[this->rParam].r, sColors[this->rParam].g, sColors[this->rParam].b, 0);
    gDPPipeSync(POLY_XLU_DISP++);
    Matrix_ReplaceRotation(&play->billboardMtxF);
    Matrix_RotateZ((this->rXZRot / 32768.0f) * 3.1416f, MTXMODE_APPLY);
    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, this->gfx);

    CLOSE_DISPS(gfxCtx);
}

void EffectSsFhgFlash_DrawShock(PlayState* play, u32 index, EffectSs* this) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    s32 pad;
    f32 scale;

    scale = this->rScale / 100.0f;

    OPEN_DISPS(gfxCtx);

    Matrix_Translate(this->pos.x, this->pos.y, this->pos.z, MTXMODE_NEW);
    Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);

    if (this->rParam != FHGFLASH_SHOCK_NO_ACTOR) {
        Gfx_SetupDL_44Xlu(play->state.gfxCtx);
        Matrix_RotateX((this->rXZRot / 32768.0f) * 1.1416f, MTXMODE_APPLY);
        gDPSetRenderMode(POLY_XLU_DISP++, G_RM_PASS, G_RM_AA_ZB_XLU_DECAL2);
    } else {
        Gfx_SetupDL_25Xlu(play->state.gfxCtx);
        Matrix_ReplaceRotation(&play->billboardMtxF);
        gDPSetRenderMode(POLY_XLU_DISP++, G_RM_PASS, G_RM_AA_ZB_XLU_SURF2);
    }

    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, this->rAlpha);
    gDPSetEnvColor(POLY_XLU_DISP++, 0, 255, 155, 0);
    Matrix_RotateZ((this->rXZRot / 32768.0f) * 3.1416f, MTXMODE_APPLY);
    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, this->gfx);

    CLOSE_DISPS(gfxCtx);
}

void EffectSsFhgFlash_UpdateLightBall(PlayState* play, u32 index, EffectSs* this) {
    s16 rand = (Rand_ZeroOne() * 20000.0f);

    this->rXZRot = (this->rXZRot + rand) + 0x4000;

    if (this->rScale > 0) {
        this->rScale -= 10;

        if (this->rScale <= 0) {
            this->rScale = 0;
            this->life = 0;
        }
    }

    if (this->rAlpha > 0) {
        this->rAlpha -= 10;

        if (this->rAlpha <= 0) {
            this->rAlpha = 0;
        }
    }
}

void EffectSsFhgFlash_UpdateShock(PlayState* play, u32 index, EffectSs* this) {
    s16 randBodypart;
    Player* player;
    BossGanondrof* phantomGanon;
    s16 rand;

    rand = (Rand_ZeroOne() * 20000.0f);
    this->rXZRot = (this->rXZRot + rand) + 0x4000;

    if (this->rParam == FHGFLASH_SHOCK_PLAYER) {
        player = GET_PLAYER(play);
        randBodypart = Rand_ZeroFloat(17.9f);
        this->pos.x = player->bodyPartsPos[randBodypart].x + Rand_CenteredFloat(10.0f);
        this->pos.y = player->bodyPartsPos[randBodypart].y + Rand_CenteredFloat(15.0f);
        this->pos.z = player->bodyPartsPos[randBodypart].z + Rand_CenteredFloat(10.0f);
    } else if (this->rParam == FHGFLASH_SHOCK_PG) {
        phantomGanon = (BossGanondrof*)this->actor;
        randBodypart = Rand_ZeroFloat(23.9f);
        this->pos.x = phantomGanon->bodyPartsPos[randBodypart].x + Rand_CenteredFloat(15.0f);
        this->pos.y = phantomGanon->bodyPartsPos[randBodypart].y + Rand_CenteredFloat(20.0f);
        this->pos.z = phantomGanon->bodyPartsPos[randBodypart].z + Rand_CenteredFloat(15.0f);
    }

    if (this->life < 100) {
        this->rAlpha -= 50;

        if (this->rAlpha < 0) {
            this->rAlpha = 0;
            this->life = 0;
        }
    }
}

static Vtx D_809A50C0[4] = {
    VTX(-10, -10, 0, 0, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(10, -10, 0, 1024, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(10, 10, 0, 1024, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-10, 10, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),
};

static Gfx D_809A5100[15] = {
    gsDPPipeSync(),
    gsDPSetTextureLUT(G_TT_NONE),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPLoadTextureBlock(D_809A5178, G_IM_FMT_I, G_IM_SIZ_8b, 32, 32, 0, G_TX_MIRROR | G_TX_WRAP,
                         G_TX_MIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD),
    gsDPSetCombineLERP(PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, COMBINED, 0, 0, 0,
                       COMBINED),
    gsSPClearGeometryMode(G_CULL_BACK | G_FOG | G_LIGHTING | G_TEXTURE_GEN | G_TEXTURE_GEN_LINEAR),
    gsSPVertex(D_809A50C0, 4, 0),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsSPEndDisplayList(),
};

static UNK_TYPE D_809A5178[258] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x005B3000, 0x00000000, 0x00000000,
    0x00000000, 0x000B0000, 0x07000000, 0x00000000, 0x00000000, 0x005BB64B, 0x3A000000, 0x00000000, 0x00000000,
    0x005B0000, 0x00000000, 0x00000000, 0x00000000, 0x001E00B6, 0xFF5B0000, 0x00000000, 0x00000000, 0x00251F0C,
    0x07000000, 0x0A000000, 0x00000000, 0x00000000, 0xB6FF0000, 0x00000000, 0x00000000, 0x00255B00, 0x071F1E14,
    0x0A000000, 0x00000000, 0x00000000, 0x00457350, 0x00000000, 0x00000000, 0x00295B8C, 0x5B5B0000, 0x00000000,
    0x00000000, 0x00000000, 0x00455C39, 0x0F000000, 0x00000000, 0x0000A1FF, 0x5B000000, 0x00000000, 0x00000000,
    0x00000000, 0x005B5B00, 0x00000000, 0x00000000, 0x005B311C, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x5BB60000, 0x00000000, 0x00000000, 0x5BB63100, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xFFB60000,
    0x00000000, 0x00000046, 0xEA310000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x0072B646, 0x00000000,
    0x0000B6A1, 0x81000000, 0x00000000, 0x00000000, 0x00000000, 0x000C0000, 0x00172E19, 0xBDAB5D41, 0x366BEAEA,
    0x81000000, 0x00000000, 0x00000000, 0x00000000, 0x06000000, 0x00000074, 0xFFFF0500, 0x0A2342B6, 0xFF000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x005B0000, 0x03030100, 0x00FF0700, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00020503, 0x00000000, 0x00466200, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00076200, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00002962, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00003EA1, 0x62000000, 0x00000000, 0x00000000, 0x0000002E, 0xB6350000,
    0x00000000, 0x00000000, 0x000000EA, 0x46000000, 0x00000000, 0x00000000, 0x00002EFF, 0xFF5A0000, 0x00000000,
    0x00000000, 0x000000FF, 0xFF001700, 0x00000000, 0x00000000, 0x01015BFF, 0xA1A10000, 0x00000000, 0x00000000,
    0x00030046, 0x97732100, 0x00000000, 0x00000000, 0x040CB65E, 0x4A5B4600, 0x00000000, 0x00000003, 0x0C0E8C46,
    0x1C035C00, 0x00000000, 0x00000017, 0x134F5B00, 0x00000046, 0x46000000, 0x00000000, 0x34D20000, 0x0401005B,
    0x00000000, 0x00000000, 0x5BB60000, 0x00000000, 0x30FCB600, 0x0000A1E7, 0x00000001, 0x0100141E, 0x5B000000,
    0x00000000, 0x00000000, 0x00000000, 0x0000A1FF, 0x5B46FF00, 0x00000100, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x000000FC, 0xFFA13100, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0xB6000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
};
