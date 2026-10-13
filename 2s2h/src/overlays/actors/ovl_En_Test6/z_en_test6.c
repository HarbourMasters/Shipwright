/*
 * File: z_en_test6.c
 * Overlay: ovl_En_Test6
 * Description: Song of Time effects (Return to DotFD, invert, skip forward)
 */

#include "z_en_test6.h"

#include "z64cutscene_commands.h"
#include "z64malloc.h"
#include "z64quake.h"

#include "objects/gameplay_keep/gameplay_keep.h"

#define FLAGS                                                                \
    (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED | \
     ACTOR_FLAG_UPDATE_DURING_SOARING_AND_SOT_CS | ACTOR_FLAG_UPDATE_DURING_OCARINA)

void EnTest6_Init(Actor* thisx, PlayState* play2);
void EnTest6_Destroy(Actor* thisx, PlayState* play2);
void EnTest6_Update(Actor* thisx, PlayState* play);
void EnTest6_Draw(Actor* thisx, PlayState* play);
void EnTest6_Reset(void);

struct SoTCsAmmoDrops;

typedef void (*SoTCsAmmoDropDrawFunc)(EnTest6*, PlayState*, struct SoTCsAmmoDrops*);

typedef enum SoTCsAmmoDropType {
    /* 0 */ SOTCS_AMMO_DROP_NONE,
    /* 1 */ SOTCS_AMMO_DROP_ARROWS,
    /* 2 */ SOTCS_AMMO_DROP_BOMB,
    /* 3 */ SOTCS_AMMO_DROP_DEKU_NUT,
    /* 4 */ SOTCS_AMMO_DROP_DEKU_STICK,
    /* 5 */ SOTCS_AMMO_DROP_RUPEE_GREEN,
    /* 6 */ SOTCS_AMMO_DROP_RUPEE_BLUE
} SoTCsAmmoDropType;

typedef struct SoTCsAmmoDrops {
    /* 0x00 */ SoTCsAmmoDropType type;
    /* 0x04 */ f32 scale;
    /* 0x08 */ Vec3f pos;
    /* 0x14 */ SoTCsAmmoDropDrawFunc draw;
} SoTCsAmmoDrops; // size = 0x18

void EnTest6_DrawAmmoDropDefault(EnTest6* this, PlayState* play, SoTCsAmmoDrops* ammoDrop);
void EnTest6_DrawAmmoDropRupee(EnTest6* this, PlayState* play, SoTCsAmmoDrops* ammoDrop);
void EnTest6_SetupAction(EnTest6* this, EnTest6ActionFunc actionFunc);
void EnTest6_StartCutscene(EnTest6* this, PlayState* play);
void EnTest6_SetupInvertedSoTCutscene(EnTest6* this, PlayState* play);
void EnTest6_InvertedSoTCutscene(EnTest6* this, PlayState* play);
void EnTest6_SetupDoubleSoTCutscene(EnTest6* this, PlayState* play);
void EnTest6_DoubleSoTCutscene(EnTest6* this, PlayState* play);
void EnTest6_SharedSoTCutscene(EnTest6* this, PlayState* play);

SoTCsAmmoDrops sSoTCsAmmoDrops[12];

ActorProfile En_Test6_Profile = {
    /**/ ACTOR_EN_TEST6,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(EnTest6),
    /**/ EnTest6_Init,
    /**/ EnTest6_Destroy,
    /**/ EnTest6_Update,
    /**/ EnTest6_Draw,
    /**/ EnTest6_Reset,
};

CutsceneData sDoubleSoTCsCamData[] = {
    // Header
    CS_CAM_SPLINE(13, 424, 0, 100),

    // Camera At Data
    /* 0x0 */ CS_CAM_POINT(CS_CAM_INTERP_MP_CUBIC, 100, 13, 0, 21, -19, CS_CAM_REL_0),
    /* 0x1 */ CS_CAM_POINT(CS_CAM_INTERP_MP_CUBIC, 100, 18, 0, 21, -19, CS_CAM_REL_0),
    /* 0x2 */ CS_CAM_POINT(CS_CAM_INTERP_MP_CUBIC, 100, 16, 0, 21, -19, CS_CAM_REL_0),
    /* 0x3 */ CS_CAM_POINT(CS_CAM_INTERP_MP_CUBIC, 100, 17, -26, -5, -32, CS_CAM_REL_0),
    /* 0x4 */ CS_CAM_POINT(CS_CAM_INTERP_MP_CUBIC, 100, 15, 0, 18, -32, CS_CAM_REL_0),
    /* 0x5 */ CS_CAM_POINT(CS_CAM_INTERP_MP_CUBIC, 100, 14, 1, 22, -27, CS_CAM_REL_0),
    /* 0x6 */ CS_CAM_POINT(CS_CAM_INTERP_MP_CUBIC, 100, 12, 0, 1, -5, CS_CAM_REL_0),
    /* 0x7 */ CS_CAM_POINT(CS_CAM_INTERP_MP_CUBIC, 100, 7, 16, 29, -77, CS_CAM_REL_0),
    /* 0x8 */ CS_CAM_POINT(CS_CAM_INTERP_MP_CUBIC, 100, 3, 1, 19, 111, CS_CAM_REL_0),
    /* 0x9 */ CS_CAM_POINT(CS_CAM_INTERP_MP_CUBIC, 100, 3, -59, 21, 91, CS_CAM_REL_0),
    /* 0xA */ CS_CAM_POINT(CS_CAM_INTERP_MP_CUBIC, 100, 3, -19, 59, 84, CS_CAM_REL_0),
    /* 0xB */ CS_CAM_POINT(CS_CAM_INTERP_MP_CUBIC, 100, 136, -19, 59, 84, CS_CAM_REL_0),
    /* 0xC */ CS_CAM_POINT(CS_CAM_INTERP_MP_CUBIC, 100, 108, -17, 57, 82, CS_CAM_REL_0),

    // Camera Eye Data
    /* 0x0 */ CS_CAM_POINT(CS_CAM_INTERP_MP_CUBIC, 100, 13, 0, 50, 681, CS_CAM_REL_0),
    /* 0x1 */ CS_CAM_POINT(CS_CAM_INTERP_MP_CUBIC, 100, 18, 0, 50, 681, CS_CAM_REL_0),
    /* 0x2 */ CS_CAM_POINT(CS_CAM_INTERP_MP_CUBIC, 100, 16, 0, 50, 681, CS_CAM_REL_0),
    /* 0x3 */ CS_CAM_POINT(CS_CAM_INTERP_MP_CUBIC, 100, 17, -104, 375, 345, CS_CAM_REL_0),
    /* 0x4 */ CS_CAM_POINT(CS_CAM_INTERP_MP_CUBIC, 100, 15, 0, -62, 289, CS_CAM_REL_0),
    /* 0x5 */ CS_CAM_POINT(CS_CAM_INTERP_MP_CUBIC, 100, 14, -47, 125, 205, CS_CAM_REL_0),
    /* 0x6 */ CS_CAM_POINT(CS_CAM_INTERP_MP_CUBIC, 100, 12, -58, -17, 199, CS_CAM_REL_0),
    /* 0x7 */ CS_CAM_POINT(CS_CAM_INTERP_MP_CUBIC, 100, 7, 16, 53, 211, CS_CAM_REL_0),
    /* 0x8 */ CS_CAM_POINT(CS_CAM_INTERP_MP_CUBIC, 100, 3, -31, 63, 623, CS_CAM_REL_0),
    /* 0x9 */ CS_CAM_POINT(CS_CAM_INTERP_MP_CUBIC, 100, 3, -341, 464, 542, CS_CAM_REL_0),
    /* 0xA */ CS_CAM_POINT(CS_CAM_INTERP_MP_CUBIC, 100, 3, -341, 464, 542, CS_CAM_REL_0),
    /* 0xB */ CS_CAM_POINT(CS_CAM_INTERP_MP_CUBIC, 100, 136, -341, 464, 542, CS_CAM_REL_0),
    /* 0xC */ CS_CAM_POINT(CS_CAM_INTERP_MP_CUBIC, 100, 108, -339, 462, 540, CS_CAM_REL_0),

    // Camera Roll and Fov Data
    /* 0x0 */ CS_CAM_MISC(15, 0xA, 70, 0),
    /* 0x1 */ CS_CAM_MISC(15, 0xA, 70, 0),
    /* 0x2 */ CS_CAM_MISC(15, 0xA, 70, 0),
    /* 0x3 */ CS_CAM_MISC(10, 0x2, 60, 0),
    /* 0x4 */ CS_CAM_MISC(10, -0x14, 55, 0),
    /* 0x5 */ CS_CAM_MISC(16, 0x0, 43, 0),
    /* 0x6 */ CS_CAM_MISC(12, 0x6, 47, 0),
    /* 0x7 */ CS_CAM_MISC(5, -0x5, 50, 0),
    /* 0x8 */ CS_CAM_MISC(2, -0x24, 108, 0),
    /* 0x9 */ CS_CAM_MISC(2, -0x28, 120, 0),
    /* 0xA */ CS_CAM_MISC(2, -0x3C, 120, 0),
    /* 0xB */ CS_CAM_MISC(2, -0x46, 130, 0),
    /* 0xC */ CS_CAM_MISC(2, -0x50, 140, 0),

    // Terminate
    CS_CAM_END()
};

TexturePtr sSoTCsAmmoDropTextures[] = {
    NULL,              // SOTCS_AMMO_DROP_NONE
    gDropArrows2Tex,   // SOTCS_AMMO_DROP_ARROWS
    gDropBombTex,      // SOTCS_AMMO_DROP_BOMB
    gDropDekuNutTex,   // SOTCS_AMMO_DROP_DEKU_NUT
    gDropDekuStickTex, // SOTCS_AMMO_DROP_DEKU_STICK
    gRupeeGreenTex,    // SOTCS_AMMO_DROP_RUPEE_GREEN
    gRupeeBlueTex,     // SOTCS_AMMO_DROP_RUPEE_BLUE
};

typedef enum SoTCsDrawType {
    /*  0 */ SOTCS_DRAW_DOUBLE_SOT,
    /*  1 */ SOTCS_DRAW_RESET_CYCLE_SOT,
    /*  2 */ SOTCS_DRAW_INVERTED_SOT,
    /* 99 */ SOTCS_DRAW_TYPE_NONE = 99
} SoTCsDrawType;

#define SOTCS_AMMO_FLAG_RUPEE (1 << 0)
#define SOTCS_AMMO_FLAG_BOMB (1 << 1)
#define SOTCS_AMMO_FLAG_NUT (1 << 2)
#define SOTCS_AMMO_FLAG_ARROW (1 << 4)

void EnTest6_SetupCutscene(EnTest6* this, PlayState* play) {
    s32 i;
    Player* player = GET_PLAYER(play);
    s32 ammoFlags;
    f32 yOffset;

    this->actor.home.pos = player->actor.world.pos;
    this->actor.home.rot = player->actor.shape.rot;

    switch (SOTCS_GET_OCARINA_MODE(&this->actor)) {
        case OCARINA_MODE_APPLY_INV_SOT_FAST:
        case OCARINA_MODE_APPLY_INV_SOT_SLOW:
            EnTest6_SetupInvertedSoTCutscene(this, play);
            CutsceneManager_Queue(play->playerCsIds[PLAYER_CS_ID_SONG_WARP]);
            break;

        case OCARINA_MODE_APPLY_DOUBLE_SOT:
            EnTest6_SetupDoubleSoTCutscene(this, play);
            CutsceneManager_Queue(play->playerCsIds[PLAYER_CS_ID_SONG_WARP]);
            break;

        default: // Setup "return to first day cutscene"
            ammoFlags = 0;
            yOffset = -900.0f;

            if (CHECK_EVENTINF(EVENTINF_THREEDAYRESET_LOST_RUPEES)) {
                for (i = 0; i < 6; i++) {
                    sSoTCsAmmoDrops[(s32)(Rand_ZeroOne() * ARRAY_COUNT(sSoTCsAmmoDrops))].type =
                        SOTCS_AMMO_DROP_RUPEE_GREEN;
                }
                sSoTCsAmmoDrops[(s32)(Rand_ZeroOne() * ARRAY_COUNT(sSoTCsAmmoDrops))].type =
                    SOTCS_AMMO_DROP_RUPEE_GREEN;
                sSoTCsAmmoDrops[(s32)(Rand_ZeroOne() * ARRAY_COUNT(sSoTCsAmmoDrops))].type = SOTCS_AMMO_DROP_RUPEE_BLUE;
                ammoFlags |= SOTCS_AMMO_FLAG_RUPEE;
            }

            if (CHECK_EVENTINF(EVENTINF_THREEDAYRESET_LOST_ARROW_AMMO)) {
                sSoTCsAmmoDrops[(s32)(Rand_ZeroOne() * ARRAY_COUNT(sSoTCsAmmoDrops))].type = SOTCS_AMMO_DROP_ARROWS;
                sSoTCsAmmoDrops[(s32)(Rand_ZeroOne() * ARRAY_COUNT(sSoTCsAmmoDrops))].type = SOTCS_AMMO_DROP_ARROWS;
                if (!(ammoFlags & SOTCS_AMMO_FLAG_RUPEE)) {
                    sSoTCsAmmoDrops[(s32)(Rand_ZeroOne() * ARRAY_COUNT(sSoTCsAmmoDrops))].type = SOTCS_AMMO_DROP_ARROWS;
                    sSoTCsAmmoDrops[(s32)(Rand_ZeroOne() * ARRAY_COUNT(sSoTCsAmmoDrops))].type = SOTCS_AMMO_DROP_ARROWS;
                }
                ammoFlags |= SOTCS_AMMO_FLAG_ARROW;
            }

            if (CHECK_EVENTINF(EVENTINF_THREEDAYRESET_LOST_BOMB_AMMO)) {
                sSoTCsAmmoDrops[(s32)(Rand_ZeroOne() * ARRAY_COUNT(sSoTCsAmmoDrops))].type = SOTCS_AMMO_DROP_BOMB;
                sSoTCsAmmoDrops[(s32)(Rand_ZeroOne() * ARRAY_COUNT(sSoTCsAmmoDrops))].type = SOTCS_AMMO_DROP_BOMB;
                if (!(ammoFlags & SOTCS_AMMO_FLAG_RUPEE)) {
                    sSoTCsAmmoDrops[(s32)(Rand_ZeroOne() * ARRAY_COUNT(sSoTCsAmmoDrops))].type = SOTCS_AMMO_DROP_BOMB;
                    sSoTCsAmmoDrops[(s32)(Rand_ZeroOne() * ARRAY_COUNT(sSoTCsAmmoDrops))].type = SOTCS_AMMO_DROP_BOMB;
                }
                ammoFlags |= SOTCS_AMMO_FLAG_BOMB;
            }

            if (CHECK_EVENTINF(EVENTINF_THREEDAYRESET_LOST_NUT_AMMO)) {
                sSoTCsAmmoDrops[(s32)(Rand_ZeroOne() * ARRAY_COUNT(sSoTCsAmmoDrops))].type = SOTCS_AMMO_DROP_DEKU_NUT;
                if (!(ammoFlags & (SOTCS_AMMO_FLAG_ARROW | SOTCS_AMMO_FLAG_BOMB | SOTCS_AMMO_FLAG_RUPEE))) {
                    sSoTCsAmmoDrops[(s32)(Rand_ZeroOne() * ARRAY_COUNT(sSoTCsAmmoDrops))].type =
                        SOTCS_AMMO_DROP_DEKU_NUT;
                }
                ammoFlags |= SOTCS_AMMO_FLAG_NUT;
            }

            if (CHECK_EVENTINF(EVENTINF_THREEDAYRESET_LOST_STICK_AMMO)) {
                sSoTCsAmmoDrops[(s32)(Rand_ZeroOne() * ARRAY_COUNT(sSoTCsAmmoDrops))].type = SOTCS_AMMO_DROP_DEKU_STICK;
                if (!(ammoFlags & (SOTCS_AMMO_FLAG_ARROW | SOTCS_AMMO_FLAG_BOMB | SOTCS_AMMO_FLAG_RUPEE))) {
                    sSoTCsAmmoDrops[(s32)(Rand_ZeroOne() * ARRAY_COUNT(sSoTCsAmmoDrops))].type =
                        SOTCS_AMMO_DROP_DEKU_STICK;
                }
            }

            for (i = 0; i < ARRAY_COUNT(sSoTCsAmmoDrops); i++) {
                sSoTCsAmmoDrops[i].pos.x = ((2.0f * Rand_ZeroOne()) - 1.0f) * 80.0f;
                sSoTCsAmmoDrops[i].pos.z = ((2.0f * Rand_ZeroOne()) - 1.0f) * 80.0f;
                sSoTCsAmmoDrops[i].pos.y = (((2.0f * Rand_ZeroOne()) - 1.0f) * 40.0f) + yOffset;
                sSoTCsAmmoDrops[i].scale = -10.0f;
                if (sSoTCsAmmoDrops[i].type <= SOTCS_AMMO_DROP_DEKU_STICK) {
                    sSoTCsAmmoDrops[i].draw = EnTest6_DrawAmmoDropDefault;
                } else {
                    sSoTCsAmmoDrops[i].draw = EnTest6_DrawAmmoDropRupee;
                }
                yOffset += 50.0f;
            }
            break;
    }
}

void EnTest6_EnableMotionBlur(s16 alpha) {
    Play_EnableMotionBlur(alpha);
}

void EnTest6_DisableMotionBlur(void) {
    Play_DisableMotionBlur();
}

void EnTest6_EnableWhiteFillScreen(PlayState* play, f32 alphaRatio) {
    play->envCtx.fillScreen = true;
    play->envCtx.screenFillColor[0] = 250;
    play->envCtx.screenFillColor[1] = 250;
    play->envCtx.screenFillColor[2] = 250;
    play->envCtx.screenFillColor[3] = 255.0f * alphaRatio;
}

void EnTest6_DisableWhiteFillScreen(PlayState* play) {
    play->envCtx.fillScreen = false;
}

void EnTest6_DrawAmmoDropDefault(EnTest6* this, PlayState* play, SoTCsAmmoDrops* ammoDrop) {
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx);

    if (ammoDrop->type != SOTCS_AMMO_DROP_NONE) {
        Matrix_Translate(ammoDrop->pos.x * ammoDrop->scale, ammoDrop->pos.y, ammoDrop->pos.z * ammoDrop->scale,
                         MTXMODE_NEW);
        Matrix_Scale(ammoDrop->scale * 0.02f, ammoDrop->scale * 0.02f, ammoDrop->scale * 0.02f, MTXMODE_APPLY);
        POLY_OPA_DISP = Play_SetFog(play, POLY_OPA_DISP);
        POLY_OPA_DISP = Gfx_SetupDL66(POLY_OPA_DISP);

        gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(sSoTCsAmmoDropTextures[ammoDrop->type]));
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_OPA_DISP++, gItemDropDL);
    }

    Matrix_Translate(ammoDrop->pos.x * ammoDrop->scale, ammoDrop->pos.y, ammoDrop->pos.z * ammoDrop->scale,
                     MTXMODE_NEW);
    Matrix_Scale(2.0f * ammoDrop->scale, 2.0f * ammoDrop->scale, 2.0f * ammoDrop->scale, MTXMODE_APPLY);
    Matrix_Mult(&play->billboardMtxF, MTXMODE_APPLY);
    Matrix_RotateZS(play->state.frames * 512, MTXMODE_APPLY);
    Matrix_Translate(0.0f, 0.0f, 2.0f, MTXMODE_APPLY);

    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 210, 210, 230, 128);
    gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 255, 0);
    gSPClearGeometryMode(POLY_XLU_DISP++, G_FOG | G_LIGHTING);
    gSPDisplayList(POLY_XLU_DISP++, gEffSparklesDL);
    gSPSetGeometryMode(POLY_XLU_DISP++, G_FOG | G_LIGHTING);

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnTest6_DrawAmmoDropRupee(EnTest6* this, PlayState* play, SoTCsAmmoDrops* ammoDrop) {
    s32 pad;
    Gfx* gfxHead = GRAPH_ALLOC(play->state.gfxCtx, 2 * sizeof(Gfx));
    Gfx* gfx = gfxHead;
    Hilite* hilite;
    Vec3f ammoDropPos;

    ammoDropPos.x = ammoDrop->pos.x * ammoDrop->scale;
    ammoDropPos.y = ammoDrop->pos.y;
    ammoDropPos.z = ammoDrop->pos.z * ammoDrop->scale;

    hilite = func_800BCBF4(&ammoDropPos, play);

    OPEN_DISPS(play->state.gfxCtx);

    if (gfxHead != NULL) {
        Gfx_SetupDL25_Opa(play->state.gfxCtx);

        gDPSetHilite1Tile(gfx++, 1, hilite, 0x10, 0x10);
        gSPEndDisplayList(gfx++);

        gSPSegment(POLY_OPA_DISP++, 0x07, gfxHead);

        Matrix_Translate(ammoDropPos.x, ammoDropPos.y, ammoDropPos.z, MTXMODE_NEW);
        Matrix_Scale(ammoDrop->scale * 0.018f, ammoDrop->scale * 0.018f, ammoDrop->scale * 0.018f, MTXMODE_APPLY);
        Matrix_Mult(&play->billboardMtxF, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
        gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(sSoTCsAmmoDropTextures[ammoDrop->type]));
        gSPDisplayList(POLY_OPA_DISP++, gRupeeDL);
    }

    Matrix_Translate(ammoDrop->pos.x * ammoDrop->scale, ammoDrop->pos.y, ammoDrop->pos.z * ammoDrop->scale,
                     MTXMODE_NEW);
    Matrix_Scale(ammoDrop->scale * 2.5f, ammoDrop->scale * 2.5f, ammoDrop->scale * 2.5f, MTXMODE_APPLY);
    Matrix_Mult(&play->billboardMtxF, MTXMODE_APPLY);
    Matrix_RotateZS(play->state.frames * 256, MTXMODE_APPLY);
    Matrix_Translate(0.0f, 0.0f, 4.0f, MTXMODE_APPLY);

    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 220, 220, 230, 192);
    gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 0);
    gSPClearGeometryMode(POLY_XLU_DISP++, G_FOG | G_LIGHTING);
    gSPDisplayList(POLY_XLU_DISP++, gEffSparklesDL);
    gSPSetGeometryMode(POLY_XLU_DISP++, G_FOG | G_LIGHTING);

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnTest6_SetupAction(EnTest6* this, EnTest6ActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void EnTest6_Init(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnTest6* this = (EnTest6*)thisx;
    s32 i;

    if (((SOTCS_GET_OCARINA_MODE(&this->actor) == OCARINA_MODE_APPLY_INV_SOT_FAST) ||
         (SOTCS_GET_OCARINA_MODE(&this->actor) == OCARINA_MODE_APPLY_INV_SOT_SLOW) ||
         (SOTCS_GET_OCARINA_MODE(&this->actor) == OCARINA_MODE_APPLY_DOUBLE_SOT)) &&
        (play->playerCsIds[PLAYER_CS_ID_SONG_WARP] == CS_ID_NONE)) {
        Actor_Kill(&this->actor);
        return;
    }

    for (i = 0; i < ARRAY_COUNT(this->lights); i++) {
        Lights_PointNoGlowSetInfo(&this->lights[i].info, this->actor.world.pos.x, (s32)this->actor.world.pos.y - 20,
                                  this->actor.world.pos.z, 255, 255, 180, -1);
        this->lights[i].node = LightContext_InsertLight(play, &play->lightCtx, &this->lights[i].info);
    }

    this->screenFillAlpha = 0;
    this->cueId = SOTCS_CUEID_NONE;
    this->invSoTClockYaw = 0;
    this->drawType = SOTCS_DRAW_TYPE_NONE;

    EnTest6_SetupCutscene(this, play);
    EnTest6_SetupAction(this, EnTest6_StartCutscene);
}

void EnTest6_Destroy(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnTest6* this = (EnTest6*)thisx;
    s32 i;

    play->envCtx.adjLightSettings.ambientColor[0] = 0;
    play->envCtx.adjLightSettings.ambientColor[1] = 0;
    play->envCtx.adjLightSettings.ambientColor[2] = 0;

    play->envCtx.adjLightSettings.light1Color[0] = 0;
    play->envCtx.adjLightSettings.light1Color[1] = 0;
    play->envCtx.adjLightSettings.light1Color[2] = 0;

    play->envCtx.adjLightSettings.light2Color[0] = 0;
    play->envCtx.adjLightSettings.light2Color[1] = 0;
    play->envCtx.adjLightSettings.light2Color[2] = 0;

    play->envCtx.adjLightSettings.fogColor[0] = 0;
    play->envCtx.adjLightSettings.fogColor[1] = 0;
    play->envCtx.adjLightSettings.fogColor[2] = 0;

    play->envCtx.adjLightSettings.fogNear = 0;
    play->envCtx.adjLightSettings.zFar = 0;

    play->envCtx.fillScreen = false;

    for (i = 0; i < ARRAY_COUNT(this->lights); i++) {
        LightContext_RemoveLight(play, &play->lightCtx, this->lights[i].node);
    }
}

void EnTest6_StartCutscene(EnTest6* this, PlayState* play) {
    switch (SOTCS_GET_OCARINA_MODE(&this->actor)) {
        case OCARINA_MODE_APPLY_INV_SOT_FAST:
        case OCARINA_MODE_APPLY_INV_SOT_SLOW:
            if (!CutsceneManager_IsNext(play->playerCsIds[PLAYER_CS_ID_SONG_WARP])) {
                CutsceneManager_Queue(play->playerCsIds[PLAYER_CS_ID_SONG_WARP]);
            } else {
                CutsceneManager_Start(play->playerCsIds[PLAYER_CS_ID_SONG_WARP], NULL);
                this->subCamId = CutsceneManager_GetCurrentSubCamId(play->playerCsIds[PLAYER_CS_ID_SONG_WARP]);
                EnTest6_SetupAction(this, EnTest6_InvertedSoTCutscene);
            }
            break;

        case OCARINA_MODE_APPLY_DOUBLE_SOT:
            if (!CutsceneManager_IsNext(play->playerCsIds[PLAYER_CS_ID_SONG_WARP])) {
                CutsceneManager_Queue(play->playerCsIds[PLAYER_CS_ID_SONG_WARP]);
            } else {
                CutsceneManager_Start(play->playerCsIds[PLAYER_CS_ID_SONG_WARP], NULL);
                this->subCamId = CutsceneManager_GetCurrentSubCamId(play->playerCsIds[PLAYER_CS_ID_SONG_WARP]);
                EnTest6_SetupAction(this, EnTest6_DoubleSoTCutscene);
            }
            break;

        default:
            gSaveContext.save.eventDayCount = 0;
            gSaveContext.save.day = 0;
            gSaveContext.save.time = CLOCK_TIME(6, 0) - 1;
            EnTest6_SetupAction(this, EnTest6_SharedSoTCutscene);
            break;
    }
}

void EnTest6_SetupInvertedSoTCutscene(EnTest6* this, PlayState* play) {
    this->cueId = SOTCS_CUEID_INV_INIT;
    this->timer = 100;
    this->screenFillAlpha = 0;

    if (SOTCS_GET_OCARINA_MODE(&this->actor) == OCARINA_MODE_APPLY_INV_SOT_SLOW) {
        Audio_PlaySfx(NA_SE_SY_TIME_CONTROL_SLOW);
    } else if (SOTCS_GET_OCARINA_MODE(&this->actor) == OCARINA_MODE_APPLY_INV_SOT_FAST) {
        Audio_PlaySfx(NA_SE_SY_TIME_CONTROL_NORMAL);
    }
}

void EnTest6_StopInvertedSoTCutscene(EnTest6* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    player->actor.freezeTimer = 0;
    play->soaringCsOrSoTCsPlaying = false;
    CutsceneManager_Stop(play->playerCsIds[PLAYER_CS_ID_SONG_WARP]);
    Player_SetCsActionWithHaltedActors(play, NULL, PLAYER_CSACTION_END);
    EnTest6_DisableMotionBlur();
    Distortion_RemoveRequest(DISTORTION_TYPE_SONG_OF_TIME);
    Actor_Kill(&this->actor);
}

Color_RGB8 sInvSoTCsFogColor = { 230, 230, 220 };
Color_RGB8 sInvSoTCsAmbientColor = { 120, 120, 100 };
Color_RGB8 sInvSoTCsDiffuseColor = { 0, 0, 0 };
s16 sInvSoTCsFogNear = 500;
s16 sInvSoTCsZFar = 1500;

void EnTest6_InvertedSoTCutscene(EnTest6* this, PlayState* play) {
    Input* input = CONTROLLER1(&play->state);
    s16 clockYaw;
    Player* player = GET_PLAYER(play);
    Camera* mainCam;
    Vec3f subCamAt;
    Vec3f subCamEye;
    Vec3f eyeNext;
    s32 i;
    f32 temp;
    Camera* subCam;

    subCam = Play_GetCamera(play, this->subCamId);
    mainCam = Play_GetCamera(play, CAM_ID_MAIN);

    // Update cutscene effects
    switch (this->cueId) {
        case SOTCS_CUEID_INV_INIT:
            this->drawType = SOTCS_DRAW_INVERTED_SOT;
            this->invSoTEnvLerp = 0.0f;
            this->speed = 0.1f;
            this->alpha = 0;
            this->invSoTClockYaw = 0;
            this->cueId = SOTCS_CUEID_INV_SETUP_CLOCKS;
            break;

        case SOTCS_CUEID_INV_SETUP_CLOCKS:
            this->invSoTEnvLerp += this->speed;
            Environment_LerpAmbientColor(play, &sInvSoTCsAmbientColor, this->invSoTEnvLerp);
            Environment_LerpDiffuseColor(play, &sInvSoTCsDiffuseColor, this->invSoTEnvLerp);
            Environment_LerpFogColor(play, &sInvSoTCsFogColor, this->invSoTEnvLerp);
            Environment_LerpFog(play, sInvSoTCsFogNear, sInvSoTCsZFar, this->invSoTEnvLerp);

            if (this->timer == 90) {
                this->alpha = 0;
                if (SOTCS_GET_OCARINA_MODE(&this->actor) == OCARINA_MODE_APPLY_INV_SOT_FAST) {
                    this->clockAngle = 0x200;
                    this->clockDist = 0.0f;
                    temp = -100.0f;
                } else {
                    this->clockAngle = 0x570;
                    this->clockDist = 110.0f;
                    temp = 100.0f;
                }
                this->speed = 1.0f;

                for (i = 0; i < SOTCS_INV_NUM_CLOCKS; i++) {
                    this->invSoTClockPos[i].x = player->actor.world.pos.x;
                    this->invSoTClockPos[i].y = player->actor.world.pos.y;
                    this->invSoTClockPos[i].z = player->actor.world.pos.z;
                }

                this->invSoTParticles = ZeldaArena_Malloc(sizeof(Vec3f) * SOTCS_NUM_PARTICLES);
                if (this->invSoTParticles != NULL) {
                    for (i = 0; i < ARRAY_COUNT(this->invSoTParticles[0]); i++) {
                        (*this->invSoTParticles)[i].x = (((2.0f * Rand_ZeroOne()) - 1.0f) * 40.0f) + subCam->eye.x +
                                                        ((subCam->at.x - subCam->eye.x) * 0.2f);
                        (*this->invSoTParticles)[i].y = (((2.0f * Rand_ZeroOne()) - 1.0f) * 120.0f) + subCam->eye.y +
                                                        ((subCam->at.y - subCam->eye.y) * 0.2f) + temp;
                        (*this->invSoTParticles)[i].z = (((2.0f * Rand_ZeroOne()) - 1.0f) * 40.0f) + subCam->eye.z +
                                                        ((subCam->at.z - subCam->eye.z) * 0.2f);
                    }
                }

                EnTest6_EnableMotionBlur(120);
                Distortion_Request(DISTORTION_TYPE_SONG_OF_TIME);
                Distortion_SetDuration(80);

                play->soaringCsOrSoTCsPlaying = true;
                this->cueId = SOTCS_CUEID_INV_CLOCKS;
            }
            break;

        case SOTCS_CUEID_INV_CLOCKS:
            if (this->timer > 80) {
                this->alpha += 25;
            }

            if (this->timer < 20) {
                this->alpha -= 25;
            }

            Environment_LerpAmbientColor(play, &sInvSoTCsAmbientColor, this->invSoTEnvLerp);
            Environment_LerpDiffuseColor(play, &sInvSoTCsDiffuseColor, this->invSoTEnvLerp);
            Environment_LerpFogColor(play, &sInvSoTCsFogColor, this->invSoTEnvLerp);
            Environment_LerpFog(play, sInvSoTCsFogNear + this->alpha, sInvSoTCsZFar + this->alpha, this->invSoTEnvLerp);

            this->invSoTClockYaw -= this->clockAngle;
            clockYaw = this->invSoTClockYaw;

            if (SOTCS_GET_OCARINA_MODE(&this->actor) == OCARINA_MODE_APPLY_INV_SOT_FAST) {
                this->clockAngle += 8;
                this->clockDist += this->speed;
            } else {
                this->clockAngle -= 8;
                this->clockDist -= this->speed;
            }

            for (i = 0; i < SOTCS_INV_NUM_CLOCKS; i++) {
                clockYaw += 0x10000 / SOTCS_INV_NUM_CLOCKS;
                this->invSoTClockPos[i].x = player->actor.world.pos.x + (Math_SinS(clockYaw) * this->clockDist);
                this->invSoTClockPos[i].y = player->actor.world.pos.y;
                this->invSoTClockPos[i].z = player->actor.world.pos.z + (Math_CosS(clockYaw) * this->clockDist);
            }

            if (this->invSoTParticles != NULL) {
                for (i = 0; i < ARRAY_COUNT(this->invSoTParticles[0]); i++) {
                    // Wiggle in the x-direction
                    (*this->invSoTParticles)[i].x += 2.0f * ((2.0f * Rand_ZeroOne()) - 1.0f);

                    // Fall or rise depending on slow-down or speed-up
                    if (SOTCS_GET_OCARINA_MODE(&this->actor) == OCARINA_MODE_APPLY_INV_SOT_FAST) {
                        // Rise up
                        (*this->invSoTParticles)[i].y += 1.0f;
                    } else {
                        // Fall down
                        (*this->invSoTParticles)[i].y -= 1.0f;
                    }
                    // Wiggle in the z-direction
                    (*this->invSoTParticles)[i].z += 2.0f * ((2.0f * Rand_ZeroOne()) - 1.0f);
                }
            }

            if (this->timer == 10) {
                this->speed = 0.1f;
                EnTest6_DisableMotionBlur();
                Distortion_RemoveRequest(DISTORTION_TYPE_SONG_OF_TIME);
                play->soaringCsOrSoTCsPlaying = false;
                if (this->invSoTParticles != NULL) {
                    ZeldaArena_Free(this->invSoTParticles);
                }
                this->cueId = SOTCS_CUEID_INV_END;
            }
            break;

        case SOTCS_CUEID_INV_END:
            this->invSoTEnvLerp -= this->speed;
            Environment_LerpAmbientColor(play, &sInvSoTCsAmbientColor, this->invSoTEnvLerp);
            Environment_LerpDiffuseColor(play, &sInvSoTCsDiffuseColor, this->invSoTEnvLerp);
            Environment_LerpFogColor(play, &sInvSoTCsFogColor, this->invSoTEnvLerp);
            Environment_LerpFog(play, sInvSoTCsFogNear, sInvSoTCsZFar, this->invSoTEnvLerp);
            break;

        default:
            break;
    }

    // Update Player Cutscene Animation
    if (this->screenFillAlpha != 0) {
        Player_SetCsActionWithHaltedActors(play, NULL, PLAYER_CSACTION_WAIT);
    } else {
        if (this->timer == 90) {
            // Look side-to-side but downwards, with chin down
            // gPlayerAnim_al_elf_tobidasi
            Player_SetCsActionWithHaltedActors(play, NULL, PLAYER_CSACTION_66);
        }

        if (this->timer == 70) {
            // close eyes and sway body in circles
            // gPlayerAnim_alink_yurayura
            Player_SetCsActionWithHaltedActors(play, NULL, PLAYER_CSACTION_82);
        }

        if (this->timer == 30) {
            // Look side-to-side but upwards, with chin up
            // gPlayerAnim_alink_kyoro
            Player_SetCsActionWithHaltedActors(play, NULL, PLAYER_CSACTION_81);
        }

        if (this->timer == 5) {
            // Give a big nod of approval
            // gPlayerAnim_al_yes
            Player_SetCsActionWithHaltedActors(play, NULL, PLAYER_CSACTION_74);
        }
    }

    // Update camera
    if (this->timer > 80) {
        subCam->fov += (90.0f - subCam->fov) / (this->timer - 80);
    } else if (this->timer > 60) {
        temp = 1.0f / (this->timer - 60);

        subCamAt.x = subCam->at.x + ((player->actor.world.pos.x - subCam->at.x) * temp);
        subCamAt.y = subCam->at.y + (((player->actor.focus.pos.y - subCam->at.y) - 20.0f) * temp);
        subCamAt.z = subCam->at.z + ((player->actor.world.pos.z - subCam->at.z) * temp);

        eyeNext.x = subCamAt.x + (Math_SinS(player->actor.world.rot.y) * 80.0f);
        eyeNext.y = subCamAt.y + 20.0f;
        eyeNext.z = subCamAt.z + (Math_CosS(player->actor.world.rot.y) * 80.0f);
        temp *= 0.75f;

        VEC3F_LERPIMPDST(&subCamEye, &subCam->eye, &eyeNext, temp);

        Play_SetCameraAtEye(play, this->subCamId, &subCamAt, &subCamEye);
    } else if ((this->timer < 11) && (this->timer > 0)) {
        subCam->fov += (mainCam->fov - subCam->fov) / this->timer;
    }

    // Update white screen
    if (this->screenFillAlpha != 0) {
        //! FAKE:
        if (1) {}

        EnTest6_EnableWhiteFillScreen(play, this->screenFillAlpha * 0.05f);
        subCam->fov += (mainCam->fov - subCam->fov) * 0.05f;
        this->screenFillAlpha++;
        if (this->screenFillAlpha >= 20) {
            this->timer = 1;
        }
    } else if ((this->timer <= 60) && (this->timer > 40) &&
               (CHECK_BTN_ALL(input->press.button, BTN_A) || CHECK_BTN_ALL(input->press.button, BTN_B))) {
        this->screenFillAlpha = 1;

        if (SOTCS_GET_OCARINA_MODE(&this->actor) == OCARINA_MODE_APPLY_INV_SOT_SLOW) {
            AudioSfx_StopById(NA_SE_SY_TIME_CONTROL_SLOW);
        } else if (SOTCS_GET_OCARINA_MODE(&this->actor) == OCARINA_MODE_APPLY_INV_SOT_FAST) {
            AudioSfx_StopById(NA_SE_SY_TIME_CONTROL_NORMAL);
        }
    }

    if (DECR(this->timer) == 0) {
        EnTest6_StopInvertedSoTCutscene(this, play);
        play->msgCtx.ocarinaMode = OCARINA_MODE_END;
    }
}

void EnTest6_SetupDoubleSoTCutscene(EnTest6* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    this->timer = 120;
    this->screenFillAlpha = 0;
    this->doubleSoTEnvLerp = 0.0f;
    this->actor.home.pos = player->actor.world.pos;
    this->actor.home.rot = player->actor.shape.rot;
}

void EnTest6_StopDoubleSoTCutscene(EnTest6* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    player->actor.freezeTimer = 0;
    play->soaringCsOrSoTCsPlaying = false;
    CutsceneManager_Stop(play->playerCsIds[PLAYER_CS_ID_SONG_WARP]);
    Player_SetCsActionWithHaltedActors(play, NULL, PLAYER_CSACTION_END);
    EnTest6_DisableMotionBlur();
    Distortion_RemoveRequest(DISTORTION_TYPE_SONG_OF_TIME);
    Actor_Kill(&this->actor);
}

static Vec3f sSubCamUp = { 0.0f, 1.0f, 0.0f };

Color_RGB8 sDoubleSoTCsFogColor = { 225, 230, 225 };
Color_RGB8 sDoubleSoTCsAmbientColor = { 120, 120, 100 };
Color_RGB8 sDoubleSoTCsDiffuseColor = { 0, 0, 0 };
s16 sDoubleSoTCsFogNear = 940;
s16 sDoubleSoTCsZFar = 2000;

void EnTest6_DoubleSoTCutscene(EnTest6* this, PlayState* play) {
    Input* input = CONTROLLER1(&play->state);
    Player* player = GET_PLAYER(play);
    Camera* subCam;
    s32 pad;
    s16 subCamId;
    s16 pad2;

    if (this->timer > 115) {
        this->doubleSoTEnvLerp += 0.2f;
        EnTest6_EnableWhiteFillScreen(play, this->doubleSoTEnvLerp);
    } else if (this->timer > 90) {
        this->doubleSoTEnvLerp -= 0.05f;
        EnTest6_EnableWhiteFillScreen(play, this->doubleSoTEnvLerp);
    } else if (this->timer == 90) {
        this->doubleSoTEnvLerp = 0.0f;
        EnTest6_DisableWhiteFillScreen(play);
    }

    if (this->timer == 1) {
        this->doubleSoTEnvLerp = 0.0f;
        EnTest6_DisableWhiteFillScreen(play);
    } else if (this->timer < 17) {
        this->doubleSoTEnvLerp -= 0.06666666f;
        EnTest6_EnableWhiteFillScreen(play, this->doubleSoTEnvLerp);
    } else if (this->timer < 22) {
        this->doubleSoTEnvLerp += 0.2f;
        EnTest6_EnableWhiteFillScreen(play, this->doubleSoTEnvLerp);
    }

    if (this->timer == 115) {
        Environment_LerpAmbientColor(play, &sDoubleSoTCsAmbientColor, 1.0f);
        Environment_LerpDiffuseColor(play, &sDoubleSoTCsDiffuseColor, 1.0f);
        Environment_LerpFogColor(play, &sDoubleSoTCsFogColor, 1.0f);
        Environment_LerpFog(play, sDoubleSoTCsFogNear, sDoubleSoTCsZFar, 1.0f);
        play->soaringCsOrSoTCsPlaying = true;
    }

    if (this->timer == 15) {
        Environment_LerpAmbientColor(play, &sDoubleSoTCsAmbientColor, 0.0f);
        Environment_LerpDiffuseColor(play, &sDoubleSoTCsDiffuseColor, 0.0f);
        Environment_LerpFogColor(play, &sDoubleSoTCsFogColor, 0.0f);
        Environment_LerpFog(play, sDoubleSoTCsFogNear, sDoubleSoTCsZFar, 0.0f);
        play->soaringCsOrSoTCsPlaying = false;
    }

    if (this->screenFillAlpha >= 20) {
        Environment_LerpAmbientColor(play, &sDoubleSoTCsAmbientColor, this->doubleSoTEnvLerp);
        Environment_LerpDiffuseColor(play, &sDoubleSoTCsDiffuseColor, this->doubleSoTEnvLerp);
        Environment_LerpFogColor(play, &sDoubleSoTCsFogColor, this->doubleSoTEnvLerp);
        Environment_LerpFog(play, sDoubleSoTCsFogNear, sDoubleSoTCsZFar, this->doubleSoTEnvLerp);
        play->soaringCsOrSoTCsPlaying = false;
    }

    Actor_PlaySfx_Flagged2(&player->actor, NA_SE_PL_FLYING_AIR - SFX_FLAG);

    switch (this->timer) {
        case 119:
            EnTest6_EnableMotionBlur(50);
            break;

        case 115:
            EnTest6_EnableMotionBlur(20);
            Distortion_Request(DISTORTION_TYPE_SONG_OF_TIME);
            Distortion_SetDuration(90);
            this->cueId = SOTCS_CUEID_DOUBLE_INIT;
            break;

        case 110:
            Audio_PlayFanfare(NA_BGM_SONG_OF_DOUBLE_TIME);
            break;

        case 38:
        case 114:
            this->cueId = SOTCS_CUEID_DOUBLE_WAIT;
            break;

        case 76:
            this->cueId = SOTCS_CUEID_DOUBLE_CLOCKS_INWARD;
            break;

        case 61:
            EnTest6_EnableMotionBlur(150);
            this->cueId = SOTCS_CUEID_DOUBLE_CLOCKS_SPIN;
            break;

        case 51:
            EnTest6_EnableMotionBlur(180);
            this->cueId = SOTCS_CUEID_DOUBLE_CLOCKS_OUTWARD;
            break;

        case 14:
        case 15:
            EnTest6_EnableMotionBlur(50);
            Distortion_RemoveRequest(DISTORTION_TYPE_SONG_OF_TIME);
            this->cueId = SOTCS_CUEID_NONE;
            break;

        case 1:
            EnTest6_DisableMotionBlur();
            if (CHECK_EVENTINF(EVENTINF_HAS_DAYTIME_TRANSITION_CS)) {
                this->cueId = SOTCS_CUEID_DOUBLE_END;
            }
            break;

        default:
            break;
    }

    EnTest6_SharedSoTCutscene(this, play);

    if (this->timer == 115) {
        subCamId = CutsceneManager_GetCurrentSubCamId(play->playerCsIds[PLAYER_CS_ID_SONG_WARP]);
        subCam = Play_GetCamera(play, subCamId);

        this->subCamAt = subCam->at;
        this->subCamEye = subCam->eye;
        this->subCamFov = subCam->fov;
        CutsceneCamera_Init(subCam, &this->csCamInfo);
    }

    if ((this->timer <= 115) && (this->timer >= 16)) {
        CutsceneCamera_UpdateSplines((u8*)sDoubleSoTCsCamData, &this->csCamInfo);
    } else if (this->timer < 16) {
        subCamId = CutsceneManager_GetCurrentSubCamId(play->playerCsIds[PLAYER_CS_ID_SONG_WARP]);

        Play_SetCameraAtEyeUp(play, subCamId, &this->subCamAt, &this->subCamEye, &sSubCamUp);
        Play_SetCameraFov(play, subCamId, this->subCamFov);
        Play_SetCameraRoll(play, subCamId, 0);
    }

    switch (this->timer) {
        case 116:
            player->actor.freezeTimer = 2;
            player->actor.shape.rot.x = 0;
            player->actor.shape.rot.y = 0;
            player->actor.world.pos.x = 0.0f;
            player->actor.world.pos.y = 0.0f;
            player->actor.world.pos.z = 0.0f;
            player->actor.home.pos.x = 0.0f;
            player->actor.home.pos.y = 0.0f;
            player->actor.home.pos.z = 0.0f;
            break;

        case 98:
            Player_SetCsActionWithHaltedActors(play, NULL, PLAYER_CSACTION_64);
            break;

        case 68:
            Player_SetCsActionWithHaltedActors(play, NULL, PLAYER_CSACTION_65);
            break;

        case 52:
            Player_SetCsActionWithHaltedActors(play, NULL, PLAYER_CSACTION_88);
            break;

        case 43:
            Player_SetCsActionWithHaltedActors(play, NULL, PLAYER_CSACTION_114);
            break;

        case 38:
            Player_SetCsActionWithHaltedActors(play, NULL, PLAYER_CSACTION_WAIT);
            break;

        case 14:
            player->actor.freezeTimer = 5;
            player->actor.world.pos = player->actor.home.pos = this->actor.home.pos;
            player->actor.shape.rot = this->actor.home.rot;
            player->actor.focus.rot.y = player->actor.shape.rot.y;
            player->yaw = player->actor.shape.rot.y;
            player->unk_ABC = 0.0f;
            player->unk_AC0 = 0.0f;
            player->actor.shape.yOffset = 0.0f;
            break;

        default:
            break;
    }

    if ((this->screenFillAlpha > 0) && (this->screenFillAlpha < 20)) {
        EnTest6_EnableWhiteFillScreen(play, this->screenFillAlpha * 0.05f);
        this->screenFillAlpha++;
        if (this->screenFillAlpha >= 20) {
            this->timer = 15;
            this->doubleSoTEnvLerp = 0.9333333f;
        }
    } else if ((this->timer < 96) && (this->timer > 50) &&
               (CHECK_BTN_ALL(input->press.button, BTN_A) || CHECK_BTN_ALL(input->press.button, BTN_B))) {
        this->screenFillAlpha = 1;
        this->timer = 39;
        SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_FANFARE, 20);
    }

    if (DECR(this->timer) == 0) {
        EnTest6_StopDoubleSoTCutscene(this, play);
    }
}

void EnTest6_Update(Actor* thisx, PlayState* play) {
    EnTest6* this = (EnTest6*)thisx;

    this->actionFunc(this, play);
}

/**
 * Processes two different cutscenes:
 * return to "Dawn of the First Day" Cs, and Song of Double Time Cs
 */
void EnTest6_SharedSoTCutscene(EnTest6* this, PlayState* play) {
    s32 pad[2];
    Player* player = GET_PLAYER(play);
    f32 yDiff;
    s32 i;
    s32 cueChannel;

    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_SOTCS)) {
        cueChannel = Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_SOTCS);
        this->cueId = play->csCtx.actorCues[cueChannel]->id;

        switch (this->cueId) {
            case SOTCS_CUEID_DOUBLE_WAIT:
                break;

            case SOTCS_CUEID_DOUBLE_INIT:
                this->drawType = SOTCS_DRAW_DOUBLE_SOT;
                this->counter = 0;
                this->clockAngle = 0;
                player->actor.shape.shadowDraw = NULL;

                if (play->csCtx.actorCues[cueChannel]->startPos.x != 0) {
                    this->clockSpeed = (u32)play->csCtx.actorCues[cueChannel]->startPos.x;
                } else {
                    this->clockSpeed = 150.0f;
                }

                if (play->csCtx.actorCues[cueChannel]->startPos.y != 0) {
                    this->clockColorGray = play->csCtx.actorCues[cueChannel]->startPos.y;
                } else {
                    this->clockColorGray = 38;
                }

                if (play->csCtx.actorCues[cueChannel]->startPos.z != 0) {
                    this->clockDist = (u32)play->csCtx.actorCues[cueChannel]->startPos.z;
                } else {
                    this->clockDist = 480.0f;
                }
                break;

            case SOTCS_CUEID_DOUBLE_CLOCKS_INWARD:
                if (play->csCtx.actorCues[cueChannel]->startPos.x != 0) {
                    this->clockSpeed += (u32)play->csCtx.actorCues[cueChannel]->startPos.x;
                }

                if (play->csCtx.actorCues[cueChannel]->startPos.y != 0) {
                    this->clockColorGray += (s16)play->csCtx.actorCues[cueChannel]->startPos.y;

                } else {
                    this->clockColorGray += 6;
                }

                if (play->csCtx.actorCues[cueChannel]->startPos.z != 0) {
                    this->clockRadialSpeed = (u32)play->csCtx.actorCues[cueChannel]->startPos.z;
                } else {
                    this->clockRadialSpeed = -32.0f;
                }
                this->clockDist += this->clockRadialSpeed;
                break;

            case SOTCS_CUEID_DOUBLE_CLOCKS_SPIN:
                if (play->csCtx.actorCues[cueChannel]->startPos.x != 0) {
                    this->clockSpeed += (u32)play->csCtx.actorCues[cueChannel]->startPos.x;
                }

                if (play->csCtx.actorCues[cueChannel]->startPos.y != 0) {
                    this->clockColorGray += (s16)play->csCtx.actorCues[cueChannel]->startPos.y;
                } else {
                    this->clockColorGray -= 4;
                }
                break;

            case SOTCS_CUEID_DOUBLE_CLOCKS_OUTWARD:
                if (play->csCtx.actorCues[cueChannel]->startPos.x != 0) {
                    this->clockSpeed += (u32)play->csCtx.actorCues[cueChannel]->startPos.x;
                }

                if (play->csCtx.actorCues[cueChannel]->startPos.y != 0) {
                    this->clockColorGray += (s16)play->csCtx.actorCues[cueChannel]->startPos.y;
                } else {
                    this->clockColorGray -= 8;
                }

                if (play->csCtx.actorCues[cueChannel]->startPos.z != 0) {
                    this->clockRadialSpeed += (u32)play->csCtx.actorCues[cueChannel]->startPos.z;
                } else {
                    this->clockRadialSpeed += 20.0f;
                }

                this->clockDist += this->clockRadialSpeed;
                if (this->clockDist > 3500.0f) {
                    this->clockDist = 3500.0f;
                    this->cueId = SOTCS_CUEID_NONE;
                }
                break;

            case SOTCS_CUEID_RESET_INIT:
                this->drawType = SOTCS_DRAW_RESET_CYCLE_SOT;
                this->counter = 0;
                this->clockAngle = 0;
                player->actor.shape.shadowDraw = NULL;

                if (play->csCtx.actorCues[cueChannel]->startPos.x != 0) {
                    this->clockSpeed = (u32)play->csCtx.actorCues[cueChannel]->startPos.x;
                } else {
                    this->clockSpeed = 100.0f;
                }

                if (play->csCtx.actorCues[cueChannel]->startPos.y != 0) {
                    this->speed = (u32)play->csCtx.actorCues[cueChannel]->startPos.y;
                } else {
                    this->speed = 20.0f;
                }

                if (play->csCtx.actorCues[cueChannel]->startPos.z != 0) {
                    this->clockDist = (u32)play->csCtx.actorCues[cueChannel]->startPos.z;
                } else {
                    this->clockDist = 300.0f;
                }
                this->clockAccel = 0.0f;
                break;

            case SOTCS_CUEID_RESET_CLOCKS_SLOW_DOWN:
                if (play->csCtx.actorCues[cueChannel]->startPos.x != 0) {
                    this->clockAccel = (u32)play->csCtx.actorCues[cueChannel]->startPos.x;
                } else {
                    this->clockAccel = -5.0f;
                }
                this->clockSpeed += this->clockAccel;
                break;

            case SOTCS_CUEID_RESET_CLOCKS_SPEED_UP:
                if (play->csCtx.actorCues[cueChannel]->startPos.x != 0) {
                    this->clockAccel += (u32)play->csCtx.actorCues[cueChannel]->startPos.x;
                } else {
                    this->clockAccel += 2.0f;
                }

                this->clockSpeed += this->clockAccel;
                if (this->clockSpeed > 10000.0f) {
                    this->clockSpeed = 10000.0f;
                    this->cueId = SOTCS_CUEID_NONE;
                }
                break;

            case SOTCS_CUEID_NONE:
            default:
                this->drawType = SOTCS_DRAW_TYPE_NONE;
                return;

            case SOTCS_CUEID_DOUBLE_END:
                Play_SetRespawnData(play, RESPAWN_MODE_RETURN, ((void)0, gSaveContext.save.entrance), player->unk_3CE,
                                    PLAYER_PARAMS(0xFF, PLAYER_START_MODE_B), &player->unk_3C0, player->unk_3CC);
                this->drawType = SOTCS_DRAW_TYPE_NONE;
                play->transitionTrigger = TRANS_TRIGGER_START;
                play->nextEntrance = gSaveContext.respawn[RESPAWN_MODE_RETURN].entrance;
                play->transitionType = TRANS_TYPE_FADE_BLACK;
                if ((CURRENT_TIME > CLOCK_TIME(18, 0)) || (CURRENT_TIME < CLOCK_TIME(6, 0))) {
                    gSaveContext.respawnFlag = -0x63;
                    SET_EVENTINF(EVENTINF_TRIGGER_DAYTELOP);
                } else {
                    gSaveContext.respawnFlag = 2;
                }
                play->msgCtx.ocarinaMode = OCARINA_MODE_END;
                return;
        }
    } else {
        switch (this->cueId) {
            case SOTCS_CUEID_DOUBLE_INIT:
                this->drawType = SOTCS_DRAW_DOUBLE_SOT;
                this->counter = 0;
                this->clockAngle = 0;
                player->actor.shape.shadowDraw = NULL;
                this->clockColorGray = 38;
                this->clockSpeed = 150.0f;
                this->clockDist = 480.0f;

            case SOTCS_CUEID_DOUBLE_WAIT:
                break;

            case SOTCS_CUEID_DOUBLE_CLOCKS_INWARD:
                this->clockRadialSpeed = -32.0f;
                this->clockColorGray += 6;
                this->clockDist += -32.0f;
                break;

            case SOTCS_CUEID_DOUBLE_CLOCKS_SPIN:
                this->clockColorGray -= 4;
                break;

            case SOTCS_CUEID_DOUBLE_CLOCKS_OUTWARD:
                this->clockColorGray -= 8;
                this->clockRadialSpeed += 20.0f;
                this->clockDist += this->clockRadialSpeed;
                if (this->clockDist > 3500.0f) {
                    this->clockDist = 3500.0f;
                    this->cueId = SOTCS_CUEID_NONE;
                }
                break;

            case SOTCS_CUEID_RESET_INIT:
                this->drawType = SOTCS_DRAW_RESET_CYCLE_SOT;
                this->counter = 0;
                this->clockAngle = 0;
                player->actor.shape.shadowDraw = NULL;
                this->clockSpeed = 100.0f;
                this->speed = 20.0f;
                this->clockDist = 300.0f;
                this->clockAccel = 0.0f;
                break;

            case SOTCS_CUEID_RESET_CLOCKS_SLOW_DOWN:
                this->clockAccel = -5.0f;
                this->clockSpeed += -5.0f;
                break;

            case SOTCS_CUEID_RESET_CLOCKS_SPEED_UP:
                this->clockAccel += 2.0f;
                this->clockSpeed += this->clockAccel;
                if (this->clockSpeed > 10000.0f) {
                    this->clockSpeed = 10000.0f;
                    this->cueId = SOTCS_CUEID_NONE;
                }
                break;

            case SOTCS_CUEID_NONE:
            default:
                this->drawType = SOTCS_DRAW_TYPE_NONE;
                return;

            case SOTCS_CUEID_DOUBLE_END:
                if (CURRENT_TIME > CLOCK_TIME(12, 0)) {
                    Play_SetRespawnData(play, RESPAWN_MODE_RETURN, ((void)0, gSaveContext.save.entrance),
                                        player->unk_3CE, PLAYER_PARAMS(0xFF, PLAYER_START_MODE_B), &player->unk_3C0,
                                        player->unk_3CC);
                    this->drawType = SOTCS_DRAW_TYPE_NONE;
                    play->transitionTrigger = TRANS_TRIGGER_START;
                    play->nextEntrance = gSaveContext.respawn[RESPAWN_MODE_RETURN].entrance;
                    play->transitionType = TRANS_TYPE_FADE_BLACK;
                    gSaveContext.respawnFlag = 2;
                    play->msgCtx.ocarinaMode = OCARINA_MODE_END;
                }
                return;
        }
    }

    if (this->drawType == SOTCS_DRAW_RESET_CYCLE_SOT) {
        for (i = 0; i < ARRAY_COUNT(sSoTCsAmmoDrops); i++) {
            sSoTCsAmmoDrops[i].pos.x += 2.0f * ((2.0f * Rand_ZeroOne()) - 1.0f);
            sSoTCsAmmoDrops[i].pos.z += 2.0f * ((2.0f * Rand_ZeroOne()) - 1.0f);
            sSoTCsAmmoDrops[i].pos.y += 3.0f;

            if (player->actor.world.pos.y < sSoTCsAmmoDrops[i].pos.y) {
                yDiff = sSoTCsAmmoDrops[i].pos.y - player->actor.world.pos.y;
                if (yDiff > 550.0f) {
                    yDiff = 1.0f;
                } else {
                    yDiff /= 550.0f;
                }
                sSoTCsAmmoDrops[i].scale = SQ(yDiff);
            } else {
                sSoTCsAmmoDrops[i].scale = -10.0f;
            }
        }
    }
    this->counter++;
}

/**
 * Draws clocks in a double spiral above and below player
 */
void EnTest6_DrawThreeDayResetSoTCutscene(EnTest6* this, PlayState* play) {
    s16 clock1Yaw;
    s16 clock2Yaw;
    s16 angle;
    s32 i;
    Vec3f clockPos;

    OPEN_DISPS(play->state.gfxCtx);

    this->gfx = POLY_OPA_DISP;
    clockPos.y = 0.0f;

    clock1Yaw = this->clockAngle;
    clock2Yaw = clock1Yaw + 0x4E20 + (s32)(0x2EE0 * Math_SinS(play->state.frames));
    // The `& 0x3C` ensures the angle only updates once every 4 frames
    angle = (play->state.frames & 0x3C) * 1024;
    angle *= this->clockSpeed / 200.0f;
    this->clockAngle += TRUNCF_BINANG(this->clockSpeed);
    this->clockRingRotZ = TRUNCF_BINANG((this->clockSpeed / 200.0f) * 256.0f);

    // Draw 2 clocks per loop
    for (i = 0; i < (SOTCS_RESET_NUM_CLOCKS / 2); i++) {
        // Clock 1
        clock1Yaw += 0x1000;
        clockPos.x = Math_CosS(clock1Yaw) * this->clockDist;
        clockPos.z = Math_SinS(clock1Yaw) * this->clockDist;
        Matrix_Translate(clockPos.x, clockPos.y, clockPos.z, MTXMODE_NEW);
        Matrix_RotateXS(0x4000, MTXMODE_APPLY);
        Matrix_Scale(0.8f, 0.8f, 0.8f, MTXMODE_APPLY);
        Matrix_RotateZS(angle, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD(this->gfx++, play->state.gfxCtx);
        gDPSetPrimColor(this->gfx++, 0, 0xFF, 28, 28, 28, 255);
        gDPSetEnvColor(this->gfx++, 255, 255, 255, 255);
        gDPSetRenderMode(this->gfx++, G_RM_FOG_SHADE_A, G_RM_AA_ZB_OPA_SURF2);
        gSPDisplayList(this->gfx++, gSongOfTimeClockDL);

        // Clock 2
        clock2Yaw += 0x1000;
        clockPos.x = Math_CosS(clock2Yaw) * this->clockDist;
        clockPos.z = Math_SinS(clock2Yaw) * this->clockDist;
        Matrix_Translate(clockPos.x, clockPos.y, clockPos.z, MTXMODE_NEW);
        Matrix_RotateXS(0x4000, MTXMODE_APPLY);
        Matrix_Scale(0.8f, 0.8f, 0.8f, MTXMODE_APPLY);
        Matrix_RotateZS(-angle, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD(this->gfx++, play->state.gfxCtx);
        gDPSetPrimColor(this->gfx++, 0, 0xFF, 28, 28, 28, 255);
        gDPSetEnvColor(this->gfx++, 255, 255, 255, 255);
        gDPSetRenderMode(this->gfx++, G_RM_FOG_SHADE_A, G_RM_AA_ZB_OPA_SURF2);
        gSPDisplayList(this->gfx++, gSongOfTimeClockDL);

        clockPos.y -= this->speed;
        angle += this->clockRingRotZ;
    }

    POLY_OPA_DISP = this->gfx;

    for (i = 0; i < ARRAY_COUNT(sSoTCsAmmoDrops); i++) {
        if (sSoTCsAmmoDrops[i].scale > 0.0f) {
            sSoTCsAmmoDrops[i].draw(this, play, &sSoTCsAmmoDrops[i]);
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

/**
 * Draws clocks in a ring that spins around player
 */
void EnTest6_DrawDoubleSoTCutscene(EnTest6* this, PlayState* play) {
    s32 pad1[2];
    Vec3f clockPos;
    s16 clockRotX;
    s16 clockNormalAngle;
    s32 i;
    Player* player = GET_PLAYER(play);
    s32 pad2;

    OPEN_DISPS(play->state.gfxCtx);

    this->gfx = POLY_OPA_DISP;
    this->clockAngle += TRUNCF_BINANG(this->clockSpeed);
    this->clockRingRotZ = this->clockAngle * 2;

    // The `& 0x3C` ensures the clock only turns once every 4 frames.
    // Each turn rotates the clock 22.5 degrees (0x10000 / 64 * 4)
    // clockNormalAngle = (play->state.frames & 0x3C) * ((0x10000 / 64 * 4) / 4);
    clockNormalAngle = (play->state.frames & 0x3C) * (DEG_TO_BINANG(22.5f) / 4);
    clockRotX = this->clockAngle + 0x4000;

    // All cases have the exact same code
    switch (player->transformation) {
        case PLAYER_FORM_DEKU:
            clockPos.x = player->actor.world.pos.y + 40.0f;
            break;

        case PLAYER_FORM_GORON:
            clockPos.x = player->actor.world.pos.y + 40.0f;
            break;

        case PLAYER_FORM_ZORA:
            clockPos.x = player->actor.world.pos.y + 40.0f;
            break;

        case PLAYER_FORM_FIERCE_DEITY:
            clockPos.x = player->actor.world.pos.y + 40.0f;
            break;

        default:
            clockPos.x = player->actor.world.pos.y + 40.0f;
            break;
    }

    // Draw clocks
    for (i = 0; i < SOTCS_DOUBLE_NUM_CLOCKS; i++) {
        clockPos.y = Math_CosS(clockRotX) * this->clockDist;
        clockPos.z = Math_SinS(clockRotX) * this->clockDist;
        // Rotate the entire clock ring
        Matrix_RotateZS(this->clockRingRotZ, MTXMODE_NEW);

        Matrix_Translate(clockPos.x, clockPos.y, clockPos.z, MTXMODE_APPLY);
        Matrix_Scale(0.85f, 0.85f, 0.85f, MTXMODE_APPLY);
        // Orient the clock so the plane it's drawn on also passes through player
        Matrix_RotateXS(clockRotX, MTXMODE_APPLY);
        // Rotate around the normal of the plane, so the clock plane does not change,
        // the drawn shape is spun internally
        Matrix_RotateZS(clockNormalAngle, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD(this->gfx++, play->state.gfxCtx);
        gDPSetPrimColor(this->gfx++, 0, 0xFF, this->clockColorGray, this->clockColorGray, this->clockColorGray, 255);
        gDPSetEnvColor(this->gfx++, 235, 238, 235, 255);
        gDPSetRenderMode(this->gfx++, G_RM_FOG_SHADE_A, G_RM_AA_ZB_OPA_SURF2);
        gSPDisplayList(this->gfx++, gSongOfTimeClockDL);

        clockRotX += 0x10000 / SOTCS_DOUBLE_NUM_CLOCKS;
    }

    Lights_PointSetPosition(&this->lights[0].info, player->actor.world.pos.x, player->actor.world.pos.y - 10.0f,
                            player->actor.world.pos.z);
    Lights_PointSetColorAndRadius(&this->lights[0].info, 100, 250, 100, 200);

    POLY_OPA_DISP = this->gfx;

    CLOSE_DISPS(play->state.gfxCtx);
}

/**
 * Draws clocks flat on the ground.
 * Also draws black particles that raise or fall to indicate if time is speeding up or slowing down
 */
void EnTest6_DrawInvertedSoTCutscene(EnTest6* this, PlayState* play2) {
    PlayState* play = play2;
    Player* player = GET_PLAYER(play);
    f32 flashScale;
    s32 i;

    OPEN_DISPS(play->state.gfxCtx);

    switch (this->cueId) {
        case SOTCS_CUEID_INV_SETUP_CLOCKS:
        case SOTCS_CUEID_INV_UNUSED:
            Lights_PointSetPosition(&this->lights[0].info, player->actor.world.pos.x, player->actor.world.pos.y - 10.0f,
                                    player->actor.world.pos.z);
            Lights_PointSetColorAndRadius(&this->lights[0].info, 245, 245, 200, this->alpha);
            break;

        case SOTCS_CUEID_INV_CLOCKS:
            this->gfx = POLY_XLU_DISP;

            // Draw clocks
            for (i = 0; i < ARRAY_COUNT(this->invSoTClockPos); i++) {
                Matrix_Translate(this->invSoTClockPos[i].x, this->invSoTClockPos[i].y, this->invSoTClockPos[i].z,
                                 MTXMODE_NEW);
                Matrix_Scale(0.3f, 0.3f, 0.3f, MTXMODE_APPLY);
                Matrix_RotateXS(-0x4000, MTXMODE_APPLY);
                Matrix_RotateZS(this->invSoTClockYaw, MTXMODE_APPLY);

                MATRIX_FINALIZE_AND_LOAD(this->gfx++, play->state.gfxCtx);
                gDPSetPrimColor(this->gfx++, 0, 0xFF, 28, 28, 28, 255);
                gDPSetEnvColor(this->gfx++, 245, 245, 200, this->alpha);
                gDPSetRenderMode(this->gfx++, G_RM_FOG_SHADE_A, G_RM_AA_ZB_XLU_SURF2);
                gSPDisplayList(this->gfx++, gSongOfTimeClockDL);

                POLY_XLU_DISP = this->gfx;
            }

            // Draw black particles
            if (this->invSoTParticles != NULL) {
                for (i = 0; i < SOTCS_NUM_PARTICLES; i++) {
                    flashScale = Rand_ZeroOne() * 0.0025f;
                    Matrix_Translate((*this->invSoTParticles)[i].x, (*this->invSoTParticles)[i].y,
                                     (*this->invSoTParticles)[i].z, MTXMODE_NEW);
                    Matrix_Scale(flashScale, flashScale, flashScale, MTXMODE_APPLY);

                    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, 128, 128, 128, this->alpha >> 1);
                    gDPSetEnvColor(POLY_XLU_DISP++, 230, 230, 180, this->alpha);

                    Gfx_SetupDL25_Xlu(play->state.gfxCtx);
                    Matrix_Mult(&play->billboardMtxF, MTXMODE_APPLY);
                    Matrix_RotateZS(this->invSoTClockYaw + (i << 2), MTXMODE_APPLY);

                    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
                    gSPDisplayList(POLY_XLU_DISP++, gEffFlash1DL);
                }
            }

            Lights_PointSetPosition(&this->lights[0].info, player->actor.world.pos.x, player->actor.world.pos.y - 10.0f,
                                    player->actor.world.pos.z);
            Lights_PointSetColorAndRadius(&this->lights[0].info, 250, 250, 100, this->alpha);
            break;

        case SOTCS_CUEID_INV_INIT:
        case SOTCS_CUEID_INV_END:
            break;
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnTest6_Draw(Actor* thisx, PlayState* play) {
    EnTest6* this = (EnTest6*)thisx;

    if (this->cueId != SOTCS_CUEID_NONE) {
        switch (this->drawType) {
            case SOTCS_DRAW_RESET_CYCLE_SOT:
                EnTest6_DrawThreeDayResetSoTCutscene(this, play);
                break;

            case SOTCS_DRAW_DOUBLE_SOT:
                EnTest6_DrawDoubleSoTCutscene(this, play);
                break;

            case SOTCS_DRAW_INVERTED_SOT:
                EnTest6_DrawInvertedSoTCutscene(this, play);
                break;

            default:
                break;
        }
    }
}

void EnTest6_Reset(void) {
    for (int i = 0; i < ARRAY_COUNT(sSoTCsAmmoDrops); i++) {
        sSoTCsAmmoDrops[i].type = SOTCS_AMMO_DROP_NONE;
    }
}
