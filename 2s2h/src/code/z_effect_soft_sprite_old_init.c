#include "z64effect_ss.h"

#include "global.h"

#include "overlays/effects/ovl_Effect_En_Ice_Block/z_eff_en_ice_block.h"
#include "overlays/effects/ovl_Effect_Ss_Blast/z_eff_ss_blast.h"
#include "overlays/effects/ovl_Effect_Ss_Bomb2/z_eff_ss_bomb2.h"
#include "overlays/effects/ovl_Effect_Ss_Bubble/z_eff_ss_bubble.h"
#include "overlays/effects/ovl_Effect_Ss_Dead_Db/z_eff_ss_dead_db.h"
#include "overlays/effects/ovl_Effect_Ss_Dead_Dd/z_eff_ss_dead_dd.h"
#include "overlays/effects/ovl_Effect_Ss_Dead_Ds/z_eff_ss_dead_ds.h"
#include "overlays/effects/ovl_Effect_Ss_Dt_Bubble/z_eff_ss_dt_bubble.h"
#include "overlays/effects/ovl_Effect_Ss_Dust/z_eff_ss_dust.h"
#include "overlays/effects/ovl_Effect_Ss_D_Fire/z_eff_ss_d_fire.h"
#include "overlays/effects/ovl_Effect_Ss_En_Fire/z_eff_ss_en_fire.h"
#include "overlays/effects/ovl_Effect_Ss_En_Ice/z_eff_ss_en_ice.h"
#include "overlays/effects/ovl_Effect_Ss_Extra/z_eff_ss_extra.h"
#include "overlays/effects/ovl_Effect_Ss_Fhg_Flash/z_eff_ss_fhg_flash.h"
#include "overlays/effects/ovl_Effect_Ss_Fire_Tail/z_eff_ss_fire_tail.h"
#include "overlays/effects/ovl_Effect_Ss_G_Fire/z_eff_ss_g_fire.h"
#include "overlays/effects/ovl_Effect_Ss_G_Ripple/z_eff_ss_g_ripple.h"
#include "overlays/effects/ovl_Effect_Ss_G_Spk/z_eff_ss_g_spk.h"
#include "overlays/effects/ovl_Effect_Ss_G_Splash/z_eff_ss_g_splash.h"
#include "overlays/effects/ovl_Effect_Ss_Hahen/z_eff_ss_hahen.h"
#include "overlays/effects/ovl_Effect_Ss_Hitmark/z_eff_ss_hitmark.h"
#include "overlays/effects/ovl_Effect_Ss_Ice_Piece/z_eff_ss_ice_piece.h"
#include "overlays/effects/ovl_Effect_Ss_Ice_Smoke/z_eff_ss_ice_smoke.h"
#include "overlays/effects/ovl_Effect_Ss_Kakera/z_eff_ss_kakera.h"
#include "overlays/effects/ovl_Effect_Ss_Kirakira/z_eff_ss_kirakira.h"
#include "overlays/effects/ovl_Effect_Ss_K_Fire/z_eff_ss_k_fire.h"
#include "overlays/effects/ovl_Effect_Ss_Lightning/z_eff_ss_lightning.h"
#include "overlays/effects/ovl_Effect_Ss_Sbn/z_eff_ss_sbn.h"
#include "overlays/effects/ovl_Effect_Ss_Sibuki/z_eff_ss_sibuki.h"
#include "overlays/effects/ovl_Effect_Ss_Solder_Srch_Ball/z_eff_ss_solder_srch_ball.h"
#include "overlays/effects/ovl_Effect_Ss_Stick/z_eff_ss_stick.h"
#include "overlays/effects/ovl_Effect_Ss_Stone1/z_eff_ss_stone1.h"

void EffectSs_DrawGEffect(PlayState* play, EffectSs* this, TexturePtr texture) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    f32 scale;
    MtxF mfTrans;
    MtxF mfScale;
    MtxF mfResult;
    MtxF mfTrans11DA0;
    s32 pad1;
    Mtx* mtx;
    void* objectPtr = play->objectCtx.slots[this->rgObjectSlot].segment;

    OPEN_DISPS(gfxCtx);

    scale = this->rgScale * 0.0025f;
    SkinMatrix_SetTranslate(&mfTrans, this->pos.x, this->pos.y, this->pos.z);
    SkinMatrix_SetScale(&mfScale, scale, scale, scale);
    SkinMatrix_MtxFMtxFMult(&mfTrans, &play->billboardMtxF, &mfTrans11DA0);
    SkinMatrix_MtxFMtxFMult(&mfTrans11DA0, &mfScale, &mfResult);
    gSegments[0x06] = OS_K0_TO_PHYSICAL(objectPtr);
    gSPSegment(POLY_XLU_DISP++, 0x06, objectPtr);

    mtx = SkinMatrix_MtxFToNewMtx(gfxCtx, &mfResult);

    if (mtx != NULL) {
        gSPMatrix(POLY_XLU_DISP++, mtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPSegment(POLY_XLU_DISP++, 0x08, Lib_SegmentedToVirtual(texture));
        Gfx_SetupDL61_Xlu(gfxCtx);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, this->rgPrimColorR, this->rgPrimColorG, this->rgPrimColorB,
                        this->rgPrimColorA);
        gDPSetEnvColor(POLY_XLU_DISP++, this->rgEnvColorR, this->rgEnvColorG, this->rgEnvColorB, this->rgEnvColorA);
        gSPDisplayList(POLY_XLU_DISP++, this->gfx);
    }

    CLOSE_DISPS(gfxCtx);
}

// EffectSsDust Spawn Functions

void EffectSsDust_Spawn(PlayState* play, u16 drawFlags, Vec3f* pos, Vec3f* velocity, Vec3f* accel,
                        Color_RGBA8* primColor, Color_RGBA8* envColor, s16 scale, s16 scaleStep, s16 life,
                        u8 updateMode) {
    EffectSsDustInitParams initParams;

    Math_Vec3f_Copy(&initParams.pos, pos);
    Math_Vec3f_Copy(&initParams.velocity, velocity);
    Math_Vec3f_Copy(&initParams.accel, accel);
    initParams.primColor = *primColor;
    initParams.envColor = *envColor;
    initParams.drawFlags = drawFlags;
    initParams.scale = scale;
    initParams.scaleStep = scaleStep;
    initParams.life = life;
    initParams.updateMode = updateMode;

    EffectSs_Spawn(play, EFFECT_SS_DUST, 128, &initParams);
}

void func_800B0DE0(PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, Color_RGBA8* primColor,
                   Color_RGBA8* envColor, s16 scale, s16 scaleStep) {
    EffectSsDust_Spawn(play, DUST_DRAWFLAG0, pos, velocity, accel, primColor, envColor, scale, scaleStep, 10,
                       DUST_UPDATE_NORMAL);
}

void func_800B0E48(PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, Color_RGBA8* primColor,
                   Color_RGBA8* envColor, s16 scale, s16 scaleStep) {
    EffectSsDust_Spawn(play, DUST_DRAWFLAG1, pos, velocity, accel, primColor, envColor, scale, scaleStep, 10,
                       DUST_UPDATE_NORMAL);
}

void func_800B0EB0(PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, Color_RGBA8* primColor,
                   Color_RGBA8* envColor, s16 scale, s16 scaleStep, s16 life) {
    EffectSsDust_Spawn(play, DUST_DRAWFLAG0, pos, velocity, accel, primColor, envColor, scale, scaleStep, life,
                       DUST_UPDATE_NORMAL);
}

void func_800B0F18(PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, Color_RGBA8* primColor,
                   Color_RGBA8* envColor, s16 scale, s16 scaleStep, s16 life) {
    EffectSsDust_Spawn(play, DUST_DRAWFLAG1, pos, velocity, accel, primColor, envColor, scale, scaleStep, life,
                       DUST_UPDATE_NORMAL);
}

void func_800B0F80(PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, Color_RGBA8* primColor,
                   Color_RGBA8* envColor, s16 scale, s16 scaleStep, s16 life) {
    EffectSsDust_Spawn(play, DUST_DRAWFLAG2, pos, velocity, accel, primColor, envColor, scale, scaleStep, life,
                       DUST_UPDATE_NORMAL);
}

void func_800B0FE8(PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, Color_RGBA8* primColor,
                   Color_RGBA8* envColor, s16 scale, s16 scaleStep) {
    EffectSsDust_Spawn(play, DUST_DRAWFLAG0, pos, velocity, accel, primColor, envColor, scale, scaleStep, 10,
                       DUST_UPDATE_FIRE);
}

void func_800B1054(PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, Color_RGBA8* primColor,
                   Color_RGBA8* envColor, s16 scale, s16 scaleStep) {
    EffectSsDust_Spawn(play, DUST_DRAWFLAG1, pos, velocity, accel, primColor, envColor, scale, scaleStep, 10,
                       DUST_UPDATE_FIRE);
}

static Color_RGBA8 sDustBrownPrim = { 170, 130, 90, 255 };
static Color_RGBA8 sDustBrownEnv = { 100, 60, 20, 255 };

void func_800B10C0(PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel) {
    EffectSsDust_Spawn(play, DUST_DRAWFLAG_RAND_COLOR_OFFSET | DUST_DRAWFLAG0, pos, velocity, accel, &sDustBrownPrim,
                       &sDustBrownEnv, 100, 5, 10, DUST_UPDATE_NORMAL);
}

void func_800B1130(PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel) {
    EffectSsDust_Spawn(play, DUST_DRAWFLAG_RAND_COLOR_OFFSET | DUST_DRAWFLAG1, pos, velocity, accel, &sDustBrownPrim,
                       &sDustBrownEnv, 100, 5, 10, DUST_UPDATE_NORMAL);
}

void func_800B11A0(PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, s16 scale, s16 scaleStep) {
    EffectSsDust_Spawn(play, DUST_DRAWFLAG_RAND_COLOR_OFFSET | DUST_DRAWFLAG0, pos, velocity, accel, &sDustBrownPrim,
                       &sDustBrownEnv, scale, scaleStep, 10, DUST_UPDATE_NORMAL);
}

void func_800B1210(PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, s16 scale, s16 scaleStep) {
    EffectSsDust_Spawn(play, DUST_DRAWFLAG_RAND_COLOR_OFFSET | DUST_DRAWFLAG1, pos, velocity, accel, &sDustBrownPrim,
                       &sDustBrownEnv, scale, scaleStep, 10, DUST_UPDATE_NORMAL);
}

void func_800B1280(PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, s16 scale, s16 scaleStep, s16 life) {
    EffectSsDust_Spawn(play, DUST_DRAWFLAG_RAND_COLOR_OFFSET | DUST_DRAWFLAG0, pos, velocity, accel, &sDustBrownPrim,
                       &sDustBrownEnv, scale, scaleStep, life, DUST_UPDATE_NORMAL);
}
void func_800B12F0(PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, s16 scale, s16 scaleStep, s16 life) {
    EffectSsDust_Spawn(play, DUST_DRAWFLAG_RAND_COLOR_OFFSET | DUST_DRAWFLAG1, pos, velocity, accel, &sDustBrownPrim,
                       &sDustBrownEnv, scale, scaleStep, life, DUST_UPDATE_NORMAL);
}

void func_800B1360(PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, Color_RGBA8* primColor,
                   Color_RGBA8* envColor) {
    func_800B0DE0(play, pos, velocity, accel, primColor, envColor, 100, 5);
}

void func_800B139C(PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, Color_RGBA8* primColor,
                   Color_RGBA8* envColor) {
    func_800B0E48(play, pos, velocity, accel, primColor, envColor, 100, 5);
}

void func_800B13D8(Vec3f* srcPos, f32 randScale, Vec3f* newPos, Vec3f* velocity, Vec3f* accel) {
    s16 randAngle;
    f32 rand = Rand_ZeroOne() * randScale;

    randAngle = Rand_ZeroOne() * 0x10000;

    *newPos = *srcPos;

    newPos->x += Math_SinS(randAngle) * rand;
    newPos->z += Math_CosS(randAngle) * rand;

    velocity->y = 1.0f;
    velocity->x = Math_SinS(randAngle);
    velocity->z = Math_CosS(randAngle);

    accel->x = 0.0f;
    accel->y = 0.0f;
    accel->z = 0.0f;
}

void func_800B14D4(PlayState* play, f32 randScale, Vec3f* srcPos) {
    s32 i;
    Vec3f pos;
    Vec3f velocity;
    Vec3f accel;

    for (i = 0; i < 20; i++) {
        func_800B13D8(srcPos, randScale, &pos, &velocity, &accel);
        func_800B1280(play, &pos, &velocity, &accel, 100, 30, 7);
    }
}

void func_800B1598(PlayState* play, f32 randScale, Vec3f* srcPos) {
    s32 i;
    Vec3f pos;
    Vec3f velocity;
    Vec3f accel;

    for (i = 0; i < 20; i++) {
        func_800B13D8(srcPos, randScale, &pos, &velocity, &accel);
        func_800B12F0(play, &pos, &velocity, &accel, 100, 30, 7);
    }
}

void EffectSsKirakira_SpawnSmallYellow(PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel) {
    Color_RGBA8 primColor = { 255, 255, 200, 255 };
    Color_RGBA8 envColor = { 255, 200, 0, 0 };

    EffectSsKirakira_SpawnDispersed(play, pos, velocity, accel, &primColor, &envColor, 1000, 16);
}

void EffectSsKirakira_SpawnSmall(PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, Color_RGBA8* primColor,
                                 Color_RGBA8* envColor) {
    EffectSsKirakira_SpawnDispersed(play, pos, velocity, accel, primColor, envColor, 1000, 16);
}

void EffectSsKirakira_SpawnDispersed(PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, Color_RGBA8* primColor,
                                     Color_RGBA8* envColor, s16 scale, s32 life) {
    EffectSsKirakiraInitParams initParams;

    Math_Vec3f_Copy(&initParams.pos, pos);
    Math_Vec3f_Copy(&initParams.velocity, velocity);
    initParams.velocity.y = ((Rand_ZeroOne() * initParams.velocity.y) + initParams.velocity.y) / 2.0f;
    Math_Vec3f_Copy(&initParams.accel, accel);
    initParams.accel.y = ((Rand_ZeroOne() * initParams.accel.y) + initParams.accel.y) / 2.0f;
    initParams.life = life;
    initParams.updateMode = 0;
    initParams.rotSpeed = 0x1518;
    initParams.yaw = Rand_ZeroOne() * 0x4000;
    initParams.scale = scale;
    initParams.primColor = *primColor;
    initParams.envColor = *envColor;
    initParams.alphaStep = (-(255.0f / initParams.life)) + (-(255.0f / initParams.life));

    EffectSs_Spawn(play, EFFECT_SS_KIRAKIRA, 128, &initParams);
}

void EffectSsKirakira_SpawnFocused(PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, Color_RGBA8* primColor,
                                   Color_RGBA8* envColor, s16 scale, s32 life) {
    EffectSsKirakiraInitParams initParams;

    Math_Vec3f_Copy(&initParams.pos, pos);
    Math_Vec3f_Copy(&initParams.velocity, velocity);
    Math_Vec3f_Copy(&initParams.accel, accel);
    initParams.life = life;
    initParams.updateMode = 1;
    initParams.rotSpeed = 0x1518;
    initParams.yaw = Rand_ZeroOne() * 0x4000;
    initParams.scale = scale;
    Color_RGBA8_Copy(&initParams.primColor, primColor);
    Color_RGBA8_Copy(&initParams.envColor, envColor);
    initParams.alphaStep = (-(255.0f / initParams.life)) + (-(255.0f / initParams.life));

    EffectSs_Spawn(play, EFFECT_SS_KIRAKIRA, 128, &initParams);
}

void EffectSsBomb2_SpawnFade(PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel) {
    EffectSsBomb2InitParams initParams;

    Math_Vec3f_Copy(&initParams.pos, pos);
    Math_Vec3f_Copy(&initParams.velocity, velocity);
    Math_Vec3f_Copy(&initParams.accel, accel);
    initParams.scale = 100;
    initParams.scaleStep = 0;
    initParams.drawMode = 0;

    EffectSs_Spawn(play, EFFECT_SS_BOMB2, 10, &initParams);
}

void EffectSsBomb2_SpawnLayered(PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, s16 scale, s16 scaleStep) {
    EffectSsBomb2InitParams initParams;

    Math_Vec3f_Copy(&initParams.pos, pos);
    Math_Vec3f_Copy(&initParams.velocity, velocity);
    Math_Vec3f_Copy(&initParams.accel, accel);
    initParams.scale = scale;
    initParams.scaleStep = scaleStep;
    initParams.drawMode = 1;

    EffectSs_Spawn(play, EFFECT_SS_BOMB2, 10, &initParams);
}

// EffectSsBlast Spawn Functions

void EffectSsBlast_Spawn(PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, Color_RGBA8* primColor,
                         Color_RGBA8* envColor, s16 scale, s16 scaleStep, s16 scaleStepDecay, s16 life) {
    EffectSsBlastInitParams initParams;

    Math_Vec3f_Copy(&initParams.pos, pos);
    Math_Vec3f_Copy(&initParams.velocity, velocity);
    Math_Vec3f_Copy(&initParams.accel, accel);
    Color_RGBA8_Copy(&initParams.primColor, primColor);
    Color_RGBA8_Copy(&initParams.envColor, envColor);
    initParams.scale = scale;
    initParams.scaleStep = scaleStep;
    initParams.scaleStepDecay = scaleStepDecay;
    initParams.life = life;

    EffectSs_Spawn(play, EFFECT_SS_BLAST, 128, &initParams);
}

void EffectSsBlast_SpawnWhiteCustomScale(PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, s16 scale,
                                         s16 scaleStep, s16 life) {
    static Color_RGBA8 sPrimColor = { 255, 255, 255, 255 };
    static Color_RGBA8 sEnvColor = { 200, 200, 200, 0 };

    EffectSsBlast_Spawn(play, pos, velocity, accel, &sPrimColor, &sEnvColor, scale, scaleStep, 35, life);
}

void EffectSsBlast_SpawnShockwave(PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, Color_RGBA8* primColor,
                                  Color_RGBA8* envColor, s16 life) {
    EffectSsBlast_Spawn(play, pos, velocity, accel, primColor, envColor, 100, 375, 35, life);
}

void EffectSsBlast_SpawnWhiteShockwave(PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel) {
    static Color_RGBA8 sPrimColor = { 255, 255, 255, 255 };
    static Color_RGBA8 sEnvColor = { 200, 200, 200, 0 };

    EffectSsBlast_SpawnShockwave(play, pos, velocity, accel, &sPrimColor, &sEnvColor, 10);
}

// EffectSsGSpk Spawn Functions

void EffectSsGSpk_SpawnAccel(PlayState* play, Actor* actor, Vec3f* pos, Vec3f* velocity, Vec3f* accel,
                             Color_RGBA8* primColor, Color_RGBA8* envColor, s16 scale, s16 scaleStep) {
    EffectSsGSpkInitParams initParams;

    Math_Vec3f_Copy(&initParams.pos, pos);
    Math_Vec3f_Copy(&initParams.velocity, velocity);
    Math_Vec3f_Copy(&initParams.accel, accel);
    Color_RGBA8_Copy(&initParams.primColor, primColor);
    Color_RGBA8_Copy(&initParams.envColor, envColor);
    initParams.actor = actor;
    initParams.scale = scale;
    initParams.scaleStep = scaleStep;
    initParams.updateMode = GSPK_UPDATEMODE_NORMAL;

    EffectSs_Spawn(play, EFFECT_SS_G_SPK, 128, &initParams);
}

// unused
void EffectSsGSpk_SpawnNoAccel(PlayState* play, Actor* actor, Vec3f* pos, Vec3f* velocity, Vec3f* accel,
                               Color_RGBA8* primColor, Color_RGBA8* envColor, s16 scale, s16 scaleStep) {
    EffectSsGSpkInitParams initParams;

    Math_Vec3f_Copy(&initParams.pos, pos);
    Math_Vec3f_Copy(&initParams.velocity, velocity);
    Math_Vec3f_Copy(&initParams.accel, accel);
    Color_RGBA8_Copy(&initParams.primColor, primColor);
    Color_RGBA8_Copy(&initParams.envColor, envColor);
    initParams.actor = actor;
    initParams.scale = scale;
    initParams.scaleStep = scaleStep;
    initParams.updateMode = GSPK_UPDATEMODE_NO_ACCEL;

    EffectSs_Spawn(play, EFFECT_SS_G_SPK, 128, &initParams);
}

void EffectSsGSpk_SpawnFuse(PlayState* play, Actor* actor, Vec3f* pos, Vec3f* velocity, Vec3f* accel) {
    Color_RGBA8 primColor = { 255, 255, 150, 255 };
    Color_RGBA8 envColor = { 255, 0, 0, 0 };

    EffectSsGSpk_SpawnSmall(play, actor, pos, velocity, accel, &primColor, &envColor);
}

// unused
void EffectSsGSpk_SpawnRandColor(PlayState* play, Actor* actor, Vec3f* pos, Vec3f* velocity, Vec3f* accel, s16 scale,
                                 s16 scaleStep) {
    Color_RGBA8 primColor = { 255, 255, 150, 255 };
    Color_RGBA8 envColor = { 255, 0, 0, 0 };
    s32 randOffset = (Rand_ZeroOne() * 20.0f) - 10.0f;

    primColor.r += randOffset;
    primColor.g += randOffset;
    primColor.b += randOffset;
    primColor.a += randOffset;
    envColor.r += randOffset;
    envColor.g += randOffset;
    envColor.b += randOffset;
    envColor.a += randOffset;

    EffectSsGSpk_SpawnAccel(play, actor, pos, velocity, accel, &primColor, &envColor, scale, scaleStep);
}

void EffectSsGSpk_SpawnSmall(PlayState* play, Actor* actor, Vec3f* pos, Vec3f* velocity, Vec3f* accel,
                             Color_RGBA8* primColor, Color_RGBA8* envColor) {
    EffectSsGSpk_SpawnAccel(play, actor, pos, velocity, accel, primColor, envColor, 100, 5);
}

// EffectSsDFire Spawn Functions
void EffectSsDFire_Spawn(PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, s16 scale, s16 scaleStep,
                         s16 alpha, s16 alphaStep, s16 fadeDelay, s32 life) {
    EffectSsDFireInitParams initParams;

    Math_Vec3f_Copy(&initParams.pos, pos);
    Math_Vec3f_Copy(&initParams.velocity, velocity);
    Math_Vec3f_Copy(&initParams.accel, accel);
    initParams.scale = scale;
    initParams.scaleStep = scaleStep;
    initParams.alpha = alpha;
    initParams.alphaStep = alphaStep;
    initParams.fadeDelay = fadeDelay;
    initParams.life = life;

    EffectSs_Spawn(play, EFFECT_SS_D_FIRE, 128, &initParams);
}

// EffectSsBubble Spawn Functions

void EffectSsBubble_Spawn(PlayState* play, Vec3f* pos, f32 yPosOffset, f32 yPosRandScale, f32 xzPosRandScale,
                          f32 scale) {
    EffectSsBubbleInitParams initParams;

    Math_Vec3f_Copy(&initParams.pos, pos);
    initParams.yPosOffset = yPosOffset;
    initParams.yPosRandScale = yPosRandScale;
    initParams.xzPosRandScale = xzPosRandScale;
    initParams.scale = scale;

    EffectSs_Spawn(play, EFFECT_SS_BUBBLE, 128, &initParams);
}

// EffectSsGRipple Spawn Functions

void EffectSsGRipple_Spawn(PlayState* play, Vec3f* pos, s16 radius, s16 radiusMax, s16 life) {
    EffectSsGRippleInitParams initParams;

    Math_Vec3f_Copy(&initParams.pos, pos);
    initParams.radius = radius;
    initParams.radiusMax = radiusMax;
    initParams.life = life;

    EffectSs_Spawn(play, EFFECT_SS_G_RIPPLE, 128, &initParams);
}

// EffectSsGSplash Spawn Functions

void EffectSsGSplash_Spawn(PlayState* play, Vec3f* pos, Color_RGBA8* primColor, Color_RGBA8* envColor, s16 type,
                           s16 scale) {
    EffectSsGSplashInitParams initParams;

    Math_Vec3f_Copy(&initParams.pos, pos);
    initParams.type = type;
    initParams.scale = scale;

    if (primColor != NULL) {
        initParams.primColor = *primColor;
        initParams.envColor = *envColor;
        initParams.customColor = 1;
    } else {
        initParams.customColor = 0;
    }

    EffectSs_Spawn(play, EFFECT_SS_G_SPLASH, 128, &initParams);
}

// EffectSsGFire Spawn Functions

void EffectSsGFire_Spawn(PlayState* play, Vec3f* pos) {
    EffectSsGFireInitParams initParams;

    Math_Vec3f_Copy(&initParams.pos, pos);

    EffectSs_Spawn(play, EFFECT_SS_G_FIRE, 128, &initParams);
}

// EffectSsLightning Spawn Functions

void EffectSsLightning_Spawn(PlayState* play, Vec3f* pos, Color_RGBA8* primColor, Color_RGBA8* envColor, s16 scale,
                             s16 yaw, s16 life, s16 numBolts) {
    EffectSsLightningInitParams initParams;

    Math_Vec3f_Copy(&initParams.pos, pos);
    Color_RGBA8_Copy(&initParams.primColor, primColor);
    Color_RGBA8_Copy(&initParams.envColor, envColor);
    initParams.scale = scale;
    initParams.yaw = yaw;
    initParams.life = life;
    initParams.numBolts = numBolts;

    EffectSs_Spawn(play, EFFECT_SS_LIGHTNING, 128, &initParams);
}

// EffectSsDtBubble Spawn Functions

void EffectSsDtBubble_SpawnColorProfile(PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, s16 scale, s16 life,
                                        s16 colorProfile, s16 randXZ) {
    EffectSsDtBubbleInitParams initParams;

    Math_Vec3f_Copy(&initParams.pos, pos);
    Math_Vec3f_Copy(&initParams.velocity, velocity);
    Math_Vec3f_Copy(&initParams.accel, accel);
    initParams.customColor = false;
    initParams.colorProfile = colorProfile;
    initParams.scale = scale;
    initParams.life = life;
    initParams.randXZ = randXZ;

    EffectSs_Spawn(play, EFFECT_SS_DT_BUBBLE, 128, &initParams);
}

void EffectSsDtBubble_SpawnCustomColor(PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel,
                                       Color_RGBA8* primColor, Color_RGBA8* envColor, s16 scale, s16 life, s16 randXZ) {
    EffectSsDtBubbleInitParams initParams;

    Math_Vec3f_Copy(&initParams.pos, pos);
    Math_Vec3f_Copy(&initParams.velocity, velocity);
    Math_Vec3f_Copy(&initParams.accel, accel);
    Color_RGBA8_Copy(&initParams.primColor, primColor);
    Color_RGBA8_Copy(&initParams.envColor, envColor);
    initParams.scale = scale;
    initParams.life = life;
    initParams.randXZ = randXZ;
    initParams.customColor = true;

    EffectSs_Spawn(play, EFFECT_SS_DT_BUBBLE, 128, &initParams);
}

// EffectSsHahen Spawn Functions

/**
 * Spawn a single fragment
 *
 *  * Notes:
 *     - if a display list is not provided, an unknown default will be used as default
 *     - the unused arg does not do anything, any value can be passed here
 *     - due to how life is implemented it is capped at 200. Any value over 200 is accepted, but the fragment will
 *       only live for 200 frames
 */
void EffectSsHahen_Spawn(PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, s16 flags, s16 scale, s16 objectId,
                         s16 life, Gfx* dList) {
    EffectSsHahenInitParams initParams;

    Math_Vec3f_Copy(&initParams.pos, pos);
    Math_Vec3f_Copy(&initParams.velocity, velocity);
    Math_Vec3f_Copy(&initParams.accel, accel);
    initParams.dList = dList;
    initParams.flags = flags;
    initParams.scale = scale;
    initParams.objectId = objectId;
    initParams.life = life;

    EffectSs_Spawn(play, EFFECT_SS_HAHEN, 128, &initParams);
}

/**
 * Spawn a burst of fragments, with the amount of fragments specifed by count and burst speed set by <arg2>
 *
 * Notes:
 *     - if a display list is not provided, D_0400C0D0 (wilted deku fragment) will be used as default
 *     - the unused arg does not do anything, any value can be passed here
 *     - due to how life is implemented it is capped at 200. Any value over 200 is accepted, but the fragment will
 *       only live for 200 frames
 */
void EffectSsHahen_SpawnBurst(PlayState* play, Vec3f* pos, f32 burstScale, s16 flags, s16 scale, s16 randScaleRange,
                              s16 count, s16 objectId, s16 life, Gfx* dList) {
    s32 i;
    Vec3f velocity;
    Vec3f accel;

    accel.y = -0.07f * burstScale;
    accel.x = accel.z = 0.0f;

    for (i = 0; i < count; i++) {
        velocity.x = (Rand_ZeroOne() - 0.5f) * burstScale;
        velocity.z = (Rand_ZeroOne() - 0.5f) * burstScale;
        velocity.y = ((Rand_ZeroOne() * 0.5f) + 0.5f) * burstScale;

        EffectSsHahen_Spawn(play, pos, &velocity, &accel, flags, Rand_S16Offset(scale, randScaleRange), objectId, life,
                            dList);
    }
}

void func_800B2364(PlayState* play, Vec3f* pos, Gfx* dList) {
    Vec3f accel = { 0.0f, -2.0f, 0.0f };

    EffectSsHahen_Spawn(play, pos, &gZeroVec3f, &accel, HAHEN_SMALL, 5, GAMEPLAY_KEEP, 10, dList);
}

// EffectSsStick Spawn Functions

/**
 * As child, spawn a broken stick fragment
 * As adult, spawn a broken sword fragment
 */
void EffectSsStick_Spawn(PlayState* play, Vec3f* pos, s16 yaw) {
    EffectSsStickInitParams initParams;

    initParams.pos = *pos;
    initParams.yaw = yaw;

    EffectSs_Spawn(play, EFFECT_SS_STICK, 128, &initParams);
}

// EffectSsSibuki Spawn Functions

void EffectSsSibuki_Spawn(PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, s16 moveDelay, s16 direction,
                          s16 scale) {
    EffectSsSibukiInitParams initParams;

    Math_Vec3f_Copy(&initParams.pos, pos);
    Math_Vec3f_Copy(&initParams.velocity, velocity);
    Math_Vec3f_Copy(&initParams.accel, accel);
    initParams.moveDelay = moveDelay;
    initParams.direction = direction;
    initParams.scale = scale;

    EffectSs_Spawn(play, EFFECT_SS_SIBUKI, 128, &initParams);
}

void EffectSsSibuki_SpawnBurst(PlayState* play, Vec3f* pos) {
    s16 i;
    Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };
    s16 randDirection = Rand_ZeroOne() * 1.99f;

    for (i = 0; i < KREG(19) + 30; i++) {
        EffectSsSibuki_Spawn(play, pos, &zeroVec, &zeroVec, i / (KREG(27) + 6), randDirection, KREG(18) + 40);
    }
}

// EffectSsStone1 Spawn Functions

void EffectSsStone1_Spawn(PlayState* play, Vec3f* pos, s32 reg0) {
    EffectSsStone1InitParams initParams;

    initParams.pos = *pos;
    initParams.reg0 = reg0;

    EffectSs_Spawn(play, EFFECT_SS_STONE1, 128, &initParams);
}

// EffectSsHitmark Spawn Functions

void EffectSsHitmark_Spawn(PlayState* play, s32 type, s16 scale, Vec3f* pos) {
    EffectSsHitmarkInitParams initParams;

    initParams.type = type;
    initParams.scale = scale;
    Math_Vec3f_Copy(&initParams.pos, pos);

    EffectSs_Spawn(play, EFFECT_SS_HITMARK, 128, &initParams);
}

void EffectSsHitmark_SpawnFixedScale(PlayState* play, s32 type, Vec3f* pos) {
    EffectSsHitmark_Spawn(play, type, 300, pos);
}

void EffectSsHitmark_SpawnCustomScale(PlayState* play, s32 type, s16 scale, Vec3f* pos) {
    EffectSsHitmark_Spawn(play, type, scale, pos);
}

// EffectSsFhgFlash Spawn Functions

/**
 * Spawn a shock effect
 *
 * param determines where the ligntning should go
 * 0: dont attach to any actor. spawns at the position specified by pos
 * 1: spawn at one of Player's body parts, chosen at random
 * 2 - 6: spawn at one of Goht's body parts
 */
void EffectSsFhgFlash_SpawnShock(PlayState* play, Actor* actor, Vec3f* pos, s16 scale, u8 params) {
    EffectSsFhgFlashInitParams initParams;

    initParams.actor = actor;
    Math_Vec3f_Copy(&initParams.pos, pos);
    initParams.scale = scale;
    initParams.params = params;
    initParams.type = FHGFLASH_SHOCK;

    EffectSs_Spawn(play, EFFECT_SS_FHG_FLASH, 128, &initParams);
}

// EffectSsKFire Spawn Functions

void EffectSsKFire_Spawn(PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, s16 scaleMax, u8 type) {
    EffectSsKFireInitParams initParams;

    Math_Vec3f_Copy(&initParams.pos, pos);
    Math_Vec3f_Copy(&initParams.velocity, velocity);
    Math_Vec3f_Copy(&initParams.accel, accel);
    initParams.scaleMax = scaleMax;
    initParams.type = type;

    EffectSs_Spawn(play, EFFECT_SS_K_FIRE, 128, &initParams);
}

// EffectSsSolderSrchBall Spawn Functions

void EffectSsSolderSrchBall_Spawn(PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, s16 scale,
                                  s16* playerDetected, s16 flags) {
    EffectSsSolderSrchBallInitParams initParams;

    Math_Vec3f_Copy(&initParams.pos, pos);
    Math_Vec3f_Copy(&initParams.velocity, velocity);
    Math_Vec3f_Copy(&initParams.accel, accel);
    initParams.scale = scale;
    initParams.playerDetected = playerDetected;
    initParams.flags = flags;

    EffectSs_Spawn(play, EFFECT_SS_SOLDER_SRCH_BALL, 128, &initParams);
}

// EffectSsKakera Spawn Functions

void EffectSsKakera_Spawn(PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* arg3, s16 gravity, s16 arg5, s16 arg6,
                          s16 arg7, s16 arg8, s16 scale, s16 arg10, s16 arg11, s32 life, s16 colorIdx, s16 objectId,
                          Gfx* dList) {
    EffectSsKakeraInitParams initParams;

    Math_Vec3f_Copy(&initParams.pos, pos);
    Math_Vec3f_Copy(&initParams.velocity, velocity);
    Math_Vec3f_Copy(&initParams.unk_18, arg3);
    initParams.gravity = gravity;
    initParams.unk_26 = arg5;
    initParams.unk_28 = arg6;
    initParams.unk_2A = arg7;
    initParams.unk_2C = arg8;
    initParams.scale = scale;
    initParams.unk_30 = arg10;
    initParams.unk_32 = arg11;
    initParams.life = life;
    initParams.colorIdx = colorIdx;
    initParams.objectId = objectId;
    initParams.dList = dList;

    EffectSs_Spawn(play, EFFECT_SS_KAKERA, 101, &initParams);
}

// EffectSsIcePiece Spawn Functions

void EffectSsIcePiece_Spawn(PlayState* play, Vec3f* pos, f32 scale, Vec3f* velocity, Vec3f* accel, s32 life) {
    EffectSsIcePieceInitParams initParams;

    Math_Vec3f_Copy(&initParams.pos, pos);
    Math_Vec3f_Copy(&initParams.velocity, velocity);
    Math_Vec3f_Copy(&initParams.accel, accel);
    initParams.scale = scale;
    initParams.life = life;
    EffectSs_Spawn(play, EFFECT_SS_ICE_PIECE, 128, &initParams);
}

void EffectSsIcePiece_SpawnBurst(PlayState* play, Vec3f* refPos, f32 scale) {
    static Vec3f sAccel = { 0.0f, 0.0f, 0.0f };
    static Vec3f sVecScales[] = {
        { 0.0f, 70.0f, 0.0f },
        { 0.0f, 45.0f, 20.0f },
        { 17.320474f, 45.0f, 9.999695f },
        { 17.320474f, 45.0f, -9.999695f },
        { 0.0f, 45.0f, -20.0f },
        { -17.320474f, 45.0f, -9.999695f },
        { -17.320474f, 45.0f, 9.999695f },
        { 0.0f, 20.0f, 20.0f },
        { 17.320474f, 20.0f, -9.999695f },
        { -17.320474f, 20.0f, -9.999695f },
    };
    s32 i;
    Vec3f velocity;
    Vec3f pos;
    f32 velocityScale;

    sAccel.y = -0.2f;

    for (i = 0; i < ARRAY_COUNT(sVecScales); i++) {
        pos = *refPos;
        velocityScale = Rand_ZeroFloat(1.0f) + 0.5f;
        velocity.x = (sVecScales[i].x * 0.18f) * velocityScale;
        velocity.y = (sVecScales[i].y * 0.18f) * velocityScale;
        velocity.z = (sVecScales[i].z * 0.18f) * velocityScale;
        pos.x += sVecScales[i].x;
        pos.y += sVecScales[i].y;
        pos.z += sVecScales[i].z;

        EffectSsIcePiece_Spawn(play, &pos, (Rand_ZeroFloat(1.0f) + 0.5f) * ((scale * 1.3f) * 100.0f), &velocity,
                               &sAccel, 25);
    }
}

// EffectSsEnIce Spawn Functions

void EffectSsEnIce_SpawnFlying(PlayState* play, Actor* actor, Vec3f* pos, Color_RGBA8* prim, Color_RGBA8* env,
                               f32 scale) {
    EffectSsEnIceInitParams initParams;

    initParams.actor = actor;
    Math_Vec3f_Copy(&initParams.pos, pos);
    Color_RGBA8_Copy(&initParams.primColor, prim);
    Color_RGBA8_Copy(&initParams.envColor, env);
    initParams.type = ENICE_TYPE_FLYING;
    initParams.scale = scale;

    if (actor != NULL) {
        Actor_PlaySfx(actor, NA_SE_PL_FREEZE_S);
    }

    EffectSs_Spawn(play, EFFECT_SS_EN_ICE, 80, &initParams);
}

void func_800B2B44(PlayState* play, Actor* actor, Vec3f* pos, f32 scale) {
    static Color_RGBA8 sPrimColor = { 150, 150, 150, 250 };
    static Color_RGBA8 sEnvColor = { 235, 245, 255, 255 };

    EffectSsEnIce_SpawnFlying(play, actor, pos, &sPrimColor, &sEnvColor, scale);
}

void func_800B2B7C(PlayState* play, Actor* actor, Vec3s* arg2, f32 scale) {
    Vec3f dest;

    Math_Vec3s_ToVec3f(&dest, arg2);
    func_800B2B44(play, actor, &dest, scale);
}

void EffectSsEnIce_Spawn(PlayState* play, Vec3f* pos, f32 scale, Vec3f* velocity, Vec3f* accel, Color_RGBA8* primColor,
                         Color_RGBA8* envColor, s32 life) {
    EffectSsEnIceInitParams initParams;

    Math_Vec3f_Copy(&initParams.pos, pos);
    Math_Vec3f_Copy(&initParams.velocity, velocity);
    Math_Vec3f_Copy(&initParams.accel, accel);
    Color_RGBA8_Copy(&initParams.primColor, primColor);
    Color_RGBA8_Copy(&initParams.envColor, envColor);
    initParams.scale = scale;
    initParams.life = life;
    initParams.type = ENICE_TYPE_NORMAL;

    EffectSs_Spawn(play, EFFECT_SS_EN_ICE, 128, &initParams);
}

// EffectSsFireTail Spawn Functions

void EffectSsFireTail_Spawn(PlayState* play, Actor* actor, Vec3f* pos, f32 scale, Vec3f* arg4, s16 arg5,
                            Color_RGBA8* primColor, Color_RGBA8* envColor, s16 type, s16 bodyPart, s32 life) {
    EffectSsFireTailInitParams initParams;

    Math_Vec3f_Copy(&initParams.pos, pos);
    Math_Vec3f_Copy(&initParams.unk_14, arg4);
    Color_RGBA8_Copy(&initParams.primColor, primColor);
    Color_RGBA8_Copy(&initParams.envColor, envColor);
    initParams.unk_20 = arg5;
    initParams.actor = actor;
    initParams.scale = scale;
    initParams.type = type;
    initParams.bodyPart = bodyPart;
    initParams.life = life;

    EffectSs_Spawn(play, EFFECT_SS_FIRE_TAIL, 128, &initParams);
}

void EffectSsFireTail_SpawnFlame(PlayState* play, Actor* actor, Vec3f* pos, f32 arg3, s16 bodyPart,
                                 f32 colorIntensity) {
    static Color_RGBA8 sPrimColor = { 255, 255, 0, 255 };
    static Color_RGBA8 sEnvColor = { 255, 0, 0, 255 };

    sPrimColor.g = (s32)(255.0f * colorIntensity);
    sPrimColor.b = 0;

    sEnvColor.g = 0;
    sEnvColor.b = 0;
    sPrimColor.r = sEnvColor.r = (s32)(255.0f * colorIntensity);

    EffectSsFireTail_Spawn(play, actor, pos, arg3, &actor->velocity, 15, &sPrimColor, &sEnvColor,
                           (colorIntensity == 1.0f) ? 0 : 1, bodyPart, 1);
}

void EffectSsFireTail_SpawnFlameOnPlayer(PlayState* play, f32 scale, s16 bodyPart, f32 colorIntensity) {
    Player* player = GET_PLAYER(play);

    EffectSsFireTail_SpawnFlame(play, &player->actor, &player->bodyPartsPos[bodyPart], scale, bodyPart, colorIntensity);
}

// EffectSsEnFire Spawn Functions

void EffectSsEnFire_SpawnVec3f(PlayState* play, Actor* actor, Vec3f* pos, s16 scale, s16 params, s16 flags,
                               s16 bodyPart) {
    EffectSsEnFireInitParams initParams;

    Math_Vec3f_Copy(&initParams.pos, pos);
    initParams.actor = actor;
    initParams.scale = scale;
    initParams.params = params;
    initParams.flags = flags;
    initParams.bodyPart = bodyPart;

    if (actor != NULL) {
        Actor_PlaySfx(actor, NA_SE_EV_FLAME_IGNITION);
    }

    EffectSs_Spawn(play, EFFECT_SS_EN_FIRE, 128, &initParams);
}

void EffectSsEnFire_SpawnVec3s(PlayState* play, Actor* actor, Vec3s* pos, s16 scale, s16 params, s16 flags,
                               s16 bodyPart) {
    EffectSsEnFireInitParams initParams;

    initParams.pos.x = pos->x;
    initParams.pos.y = pos->y;
    initParams.pos.z = pos->z;
    initParams.actor = actor;
    initParams.scale = scale;
    initParams.params = params;
    initParams.flags = flags | ENFIRE_FLAGS_BODYPART_POS_VEC3S;
    initParams.bodyPart = bodyPart;

    if (actor != NULL) {
        Actor_PlaySfx(actor, NA_SE_EV_FLAME_IGNITION);
    }

    EffectSs_Spawn(play, EFFECT_SS_EN_FIRE, 128, &initParams);
}

// EffectSsExtra Spawn Functions

void EffectSsExtra_Spawn(PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, s16 scale, s16 scoreIndex) {
    EffectSsExtraInitParams initParams;

    Math_Vec3f_Copy(&initParams.pos, pos);
    Math_Vec3f_Copy(&initParams.velocity, velocity);
    Math_Vec3f_Copy(&initParams.accel, accel);
    initParams.scale = scale;
    initParams.scoreIndex = scoreIndex;

    EffectSs_Spawn(play, EFFECT_SS_EXTRA, 100, &initParams);
}

// EffectSsDeadDb Spawn Functions

void EffectSsDeadDb_Spawn(PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, Color_RGBA8* prim,
                          Color_RGBA8* env, s16 scale, s16 scaleStep, s32 life) {
    EffectSsDeadDbInitParams initParams;

    Math_Vec3f_Copy(&initParams.pos, pos);
    Math_Vec3f_Copy(&initParams.velocity, velocity);
    Math_Vec3f_Copy(&initParams.accel, accel);
    initParams.scale = scale;
    initParams.scaleStep = scaleStep;
    initParams.primColor.r = prim->r;
    initParams.primColor.g = prim->g;
    initParams.primColor.b = prim->b;
    initParams.primColor.a = prim->a;
    initParams.envColor.r = env->r;
    initParams.envColor.g = env->g;
    initParams.envColor.b = env->b;
    initParams.life = life;

    EffectSs_Spawn(play, EFFECT_SS_DEAD_DB, 120, &initParams);
}

void func_800B3030(PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, s16 scale, s16 scaleStep,
                   s32 colorIndex) {
    static Color_RGBA8 sPrimColor = { 255, 255, 255, 255 };
    static Color_RGBA8 sEnvColors[] = {
        { 255, 0, 0, 255 },
        { 0, 255, 0, 255 },
        { 0, 0, 255, 255 },
        { 150, 150, 150, 255 },
    };

    EffectSsDeadDb_Spawn(play, pos, velocity, accel, &sPrimColor, &sEnvColors[colorIndex], scale, scaleStep, 9);
}

// EffectSsDeadDd Spawn Functions

void EffectSsDeadDd_Spawn(PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, Color_RGBA8* prim,
                          Color_RGBA8* env, s16 scale, s16 scaleStep, s16 alphaStep, s32 life) {
    EffectSsDeadDdInitParams initParams;

    Math_Vec3f_Copy(&initParams.pos, pos);
    Math_Vec3f_Copy(&initParams.velocity, velocity);
    Math_Vec3f_Copy(&initParams.accel, accel);
    initParams.scale = scale;
    initParams.scaleStep = scaleStep;
    initParams.primColor.r = prim->r;
    initParams.primColor.g = prim->g;
    initParams.primColor.b = prim->b;
    initParams.primColor.a = prim->a;
    initParams.envColor.r = env->r;
    initParams.envColor.g = env->g;
    initParams.envColor.b = env->b;
    initParams.type = DEADDD_TYPE_0;
    initParams.alphaStep = alphaStep;
    initParams.life = life;

    EffectSs_Spawn(play, EFFECT_SS_DEAD_DD, 120, &initParams);
}

// EffectSsDeadDs Spawn Functions

void EffectSsDeadDs_Spawn(PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, s16 scale, s16 scaleStep,
                          s16 alpha, s32 life) {
    EffectSsDeadDsInitParams initParams;

    Math_Vec3f_Copy(&initParams.pos, pos);
    Math_Vec3f_Copy(&initParams.velocity, velocity);
    Math_Vec3f_Copy(&initParams.accel, accel);
    initParams.scale = scale;
    initParams.scaleStep = scaleStep;
    initParams.alpha = alpha;
    initParams.life = life;
    EffectSs_Spawn(play, EFFECT_SS_DEAD_DS, 100, &initParams);
}

void func_800B31BC(PlayState* play, Vec3f* pos, s16 scale, s16 scaleStep, s16 alpha, s32 life) {
    EffectSsDeadDs_Spawn(play, pos, &gZeroVec3f, &gZeroVec3f, scale, scaleStep, alpha, life);
}

// EffectSsIceSmoke Spawn Functions

void EffectSsIceSmoke_Spawn(PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, s16 scale) {
    EffectSsIceSmokeInitParams initParams;

    Math_Vec3f_Copy(&initParams.pos, pos);
    Math_Vec3f_Copy(&initParams.velocity, velocity);
    Math_Vec3f_Copy(&initParams.accel, accel);
    initParams.scale = scale;

    EffectSs_Spawn(play, EFFECT_SS_ICE_SMOKE, 128, &initParams);
}

// EffectSsIceBlock Spawn Functions

void EffectSsIceBlock_Spawn(PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, s16 scale) {
    EffectEnIceBlockInitParams initParams;

    Math_Vec3f_Copy(&initParams.pos, pos);
    Math_Vec3f_Copy(&initParams.velocity, velocity);
    Math_Vec3f_Copy(&initParams.accel, accel);
    initParams.scale = scale;

    EffectSs_Spawn(play, EFFECT_EN_ICE_BLOCK, 128, &initParams);
}
