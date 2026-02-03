/**
 * fx_helper.c - Visual effects for custom items
 */

#include "fx_helper.h"
#include "functions.h"
#include "variables.h"
#include "objects/gameplay_keep/gameplay_keep.h"

static Color_RGBA8 sDust = { 139, 90, 43, 255 };
static Color_RGBA8 sSpark = { 255, 255, 200, 255 };
static Color_RGBA8 sFire = { 255, 100, 0, 255 };
static Color_RGBA8 sIce = { 100, 200, 255, 255 };
static Color_RGBA8 sShock = { 255, 255, 100, 255 };

void FX_SpawnParticles(PlayState* play, Vec3f* pos, FX_Type type, u8 count) {
    switch (type) {
        case FX_DUST:
            FX_SpawnDust(play, pos, (FX_Color*)&sDust, count);
            break;
        case FX_FIRE: {
            Vec3f vel = { 0, 3.0f, 0 }, accel = { 0, -0.3f, 0 };
            for (u8 i = 0; i < count; i++) {
                Vec3f p = *pos;
                p.x += Rand_CenteredFloat(15.0f);
                p.y += Rand_CenteredFloat(10.0f);
                p.z += Rand_CenteredFloat(15.0f);
                func_8002836C(play, &p, &vel, &accel, &sFire, &sFire, 150, 30, 12);
            }
        } break;
        case FX_ICE: {
            Vec3f vel = { 0, 2.0f, 0 }, accel = { 0, -0.2f, 0 };
            for (u8 i = 0; i < count; i++) {
                Vec3f p = *pos;
                p.x += Rand_CenteredFloat(12.0f);
                p.y += Rand_CenteredFloat(8.0f);
                p.z += Rand_CenteredFloat(12.0f);
                func_8002836C(play, &p, &vel, &accel, &sIce, &sIce, 120, 25, 10);
            }
        } break;
        case FX_SHOCK: {
            Vec3f vel = { 0, 2.5f, 0 }, accel = { 0, 0.1f, 0 };
            for (u8 i = 0; i < count; i++) {
                Vec3f p = *pos;
                p.x += Rand_CenteredFloat(10.0f);
                p.y += Rand_CenteredFloat(10.0f);
                p.z += Rand_CenteredFloat(10.0f);
                func_8002836C(play, &p, &vel, &accel, &sShock, &sShock, 100, 20, 8);
            }
        } break;
        default:
            break;
    }
}

void FX_SpawnDust(PlayState* play, Vec3f* pos, FX_Color* color, u8 count) {
    Vec3f vel, accel = { 0, -1.0f, 0 };
    Color_RGBA8 col = { color->r, color->g, color->b, color->a };
    for (u8 i = 0; i < count; i++) {
        Vec3f p = *pos;
        p.x += Rand_CenteredFloat(20.0f);
        p.y += 3.0f;
        p.z += Rand_CenteredFloat(20.0f);
        vel.x = Rand_CenteredFloat(6.0f);
        vel.y = 6.0f + Rand_ZeroOne() * 4.0f;
        vel.z = Rand_CenteredFloat(6.0f);
        func_8002836C(play, &p, &vel, &accel, &col, &col, 250, 30, 10);
    }
}

void FX_SpawnSparkles(Player* player, PlayState* play) {
    Vec3f sparklePos = player->actor.world.pos;
    Vec3f zero = { 0.0f, 0.0f, 0.0f };
    Color_RGBA8 prim = { 255, 255, 200, 255 };
    Color_RGBA8 env = { 200, 200, 100, 0 };

    for (s32 i = 0; i < 5; i++) {
        s16 angle = (s16)(Rand_ZeroOne() * 0xFFFF);
        f32 dist = 5 + Rand_ZeroOne() * 5;
        sparklePos.x = player->actor.world.pos.x + Math_SinS(angle) * dist;
        sparklePos.y = player->actor.world.pos.y + 5.0f;
        sparklePos.z = player->actor.world.pos.z + Math_CosS(angle) * dist;
        EffectSsKiraKira_SpawnSmall(play, &sparklePos, &zero, &zero, &prim, &env);
    }
}

void FX_SpawnExplosion(PlayState* play, Vec3f* pos, f32 scale) {
    Vec3f vel = { 0, 8.0f * scale, 0 }, accel = { 0, -0.3f, 0 };
    EffectSsBomb2_SpawnLayered(play, pos, &vel, &accel, (s16)(120 * scale), (s16)(30 * scale));
}

void FX_SpawnShockwave(PlayState* play, Vec3f* pos) {
    Vec3f zero = { 0, 0, 0 };
    EffectSsBlast_SpawnWhiteShockwave(play, pos, &zero, &zero);
}

void FX_SpawnShockwaveSmall(PlayState* play, Vec3f* pos, s16 scale, s16 scaleStep) {
    Vec3f zero = { 0, 0, 0 };
    EffectSsBlast_SpawnWhiteCustomScale(play, pos, &zero, &zero, scale, scaleStep, 8);
}

void FX_SpawnTrail(PlayState* play, Vec3f* pos, FX_Type type) {
    Vec3f zero = { 0, 0, 0 };
    if (type == FX_FIRE)
        EffectSsDFire_Spawn(play, pos, &zero, &zero, 15, 1, 255, 80, 20);
    else if (type == FX_ICE)
        EffectSsIcePiece_Spawn(play, pos, 1.2f, &zero, &zero, 8);
    else if (type == FX_SHOCK) {
        Color_RGBA8 env = { 255, 255, 255, 255 };
        EffectSsKiraKira_SpawnSmall(play, pos, &zero, &zero, &sShock, &env);
    }
}

void FX_SpawnRadialDust(PlayState* play, Vec3f* center, f32 minRadius, f32 maxRadius, u8 count, FX_Color* color) {
    Color_RGBA8 col = { color->r, color->g, color->b, color->a };
    Color_RGBA8 env = { 0, 0, 0, 255 };
    u8 cnt = (count > 8) ? 8 : count;

    for (u8 i = 0; i < cnt; i++) {
        s16 angle = (s16)((f32)i / cnt * 0xFFFF);
        f32 radius = minRadius + Rand_ZeroOne() * (maxRadius - minRadius);

        Vec3f pos, vel, accel;
        pos.x = center->x + Math_SinS(angle) * radius;
        pos.y = center->y + 2.0f;
        pos.z = center->z + Math_CosS(angle) * radius;

        vel.x = Math_SinS(angle) * 3.0f;
        vel.y = 1.0f + Rand_ZeroOne() * 2.0f;
        vel.z = Math_CosS(angle) * 3.0f;

        accel.x = -vel.x * 0.1f;
        accel.y = -0.1f;
        accel.z = -vel.z * 0.1f;

        func_8002829C(play, &pos, &vel, &accel, &col, &env, 300, 10);
    }
}

void FX_SpawnRadialExplosion(PlayState* play, Vec3f* center, f32 radius, u8 count, f32 scale) {
    Vec3f zero = { 0, 0, 0 };
    u8 cnt = (count > 12) ? 12 : count;

    for (u8 i = 0; i < cnt; i++) {
        s16 angle = (s16)((f32)i / cnt * 0xFFFF);
        Vec3f pos;
        pos.x = center->x + Math_SinS(angle) * radius;
        pos.y = center->y;
        pos.z = center->z + Math_CosS(angle) * radius;
        EffectSsBomb2_SpawnLayered(play, &pos, &zero, &zero, (s16)(80 * scale), (s16)(20 * scale));
    }
}

void FX_SpawnLightning(PlayState* play, Vec3f* pos, u8 red, u8 scale) {
    Color_RGBA8 prim = { 255, 255, 255, 255 };
    Color_RGBA8 env = { red > 0 ? 180 : 100, red > 0 ? 0 : 100, red > 0 ? 0 : 255, 255 };
    Vec3f zero = { 0, 0, 0 };

    EffectSsLightning_Spawn(play, pos, &prim, &env, scale, (s16)(Rand_ZeroOne() * 0xFFFF), 6, 2);
    EffectSsBomb2_SpawnLayered(play, pos, &zero, &zero, 15, 5);
}

void FX_SpawnLightningRing(PlayState* play, Vec3f* center, f32 minRadius, f32 maxRadius, u8 count, u8 red, u8 scale) {
    u8 cnt = (count > 6) ? 6 : count;

    for (u8 i = 0; i < cnt; i++) {
        s16 angle = (s16)(Rand_ZeroOne() * 0xFFFF);
        f32 radius = minRadius + Rand_ZeroOne() * (maxRadius - minRadius);

        Vec3f pos;
        pos.x = center->x + Math_SinS(angle) * radius;
        pos.y = center->y;
        pos.z = center->z + Math_CosS(angle) * radius;

        FX_SpawnLightning(play, &pos, red, scale);
    }
}

void FX_SpawnSuction(PlayState* play, Vec3f* origin, s16 yaw, s16 pitch) {
    static Color_RGBA8 sGustCol = { 200, 200, 200, 180 };

    Vec3f pos, vel, accel = { 0, 0, 0 };
    f32 dist = 60.0f + Rand_ZeroOne() * 60.0f;
    s16 yawSpread = (s16)Rand_CenteredFloat(0x1500);
    s16 pitchSpread = (s16)Rand_CenteredFloat(0x800);
    f32 horizontalDist = dist * Math_CosS(pitch + pitchSpread);

    pos.x = origin->x + Math_SinS(yaw + yawSpread) * horizontalDist;
    pos.y = origin->y - Math_SinS(pitch + pitchSpread) * dist;
    pos.z = origin->z + Math_CosS(yaw + yawSpread) * horizontalDist;

    f32 speed = 10.0f;
    vel.x = (origin->x - pos.x) / dist * speed;
    vel.y = (origin->y - pos.y) / dist * speed;
    vel.z = (origin->z - pos.z) / dist * speed;

    func_8002836C(play, &pos, &vel, &accel, &sGustCol, &sGustCol, 60, 15, 8);
}

void FX_SpawnWindBlow(PlayState* play, Vec3f* origin, s16 yaw, f32 range) {
    static Color_RGBA8 sWindCol = { 220, 220, 220, 160 };

    Vec3f pos, vel, accel = { 0, 0, 0 };
    f32 startDist = 10.0f + Rand_ZeroOne() * 20.0f;
    s16 yawSpread = (s16)Rand_CenteredFloat(0x2000);

    pos.x = origin->x + Math_SinS(yaw + yawSpread) * startDist;
    pos.y = origin->y + Rand_CenteredFloat(15.0f);
    pos.z = origin->z + Math_CosS(yaw + yawSpread) * startDist;

    f32 speed = 15.0f + Rand_ZeroOne() * 10.0f;
    vel.x = Math_SinS(yaw + yawSpread) * speed;
    vel.y = Rand_CenteredFloat(2.0f);
    vel.z = Math_CosS(yaw + yawSpread) * speed;

    accel.x = vel.x * -0.05f;
    accel.y = -0.1f;
    accel.z = vel.z * -0.05f;

    func_8002836C(play, &pos, &vel, &accel, &sWindCol, &sWindCol, 80, 20, 10);
}

void FX_SpawnProjectileTrail(PlayState* play, Vec3f* pos, FX_Type type) {
    Vec3f zero = { 0, 0, 0 };
    Vec3f randPos = *pos;

    if (type == FX_FIRE) {
        EffectSsDFire_Spawn(play, pos, &zero, &zero, 15, 1, 255, 80, 20);
        randPos.x += Rand_CenteredFloat(10.0f);
        randPos.y += Rand_CenteredFloat(10.0f);
        randPos.z += Rand_CenteredFloat(10.0f);
        func_8002836C(play, &randPos, &zero, &zero, &sFire, &sFire, 150, 30, 12);
    } else if (type == FX_ICE) {
        EffectSsIcePiece_Spawn(play, pos, 1.2f, &zero, &zero, 8);
        randPos.x += Rand_CenteredFloat(8.0f);
        randPos.y += Rand_CenteredFloat(8.0f);
        randPos.z += Rand_CenteredFloat(8.0f);
        func_8002836C(play, &randPos, &zero, &zero, &sIce, &sIce, 120, 25, 10);
    } else if (type == FX_SHOCK) {
        Color_RGBA8 env = { 255, 255, 255, 255 };
        for (int i = 0; i < 3; i++) {
            Vec3f shockPos = *pos;
            shockPos.x += Rand_CenteredFloat(12.0f);
            shockPos.y += Rand_CenteredFloat(12.0f);
            shockPos.z += Rand_CenteredFloat(12.0f);
            EffectSsKiraKira_SpawnSmall(play, &shockPos, &zero, &zero, &sShock, &env);
        }
        func_8002836C(play, pos, &zero, &zero, &sShock, &sShock, 100, 20, 8);
    } else {
        static Color_RGBA8 sDustTrail = { 200, 200, 200, 200 };
        func_8002836C(play, pos, &zero, &zero, &sDustTrail, &sDustTrail, 250, 45, 18);
    }
}

void FX_SpawnRodFireball(PlayState* play, Vec3f* pos, s16 yaw, RodColor* color) {
    Player* player = GET_PLAYER(play);
    Color_RGBA8 prim = { color->primR, color->primG, color->primB, color->primA };
    Color_RGBA8 env = { color->envR, color->envG, color->envB, color->envA };
    Vec3f vel, accel = { 0, 0, 0 };

    f32 speed = 8.0f;
    vel.x = Math_SinS(yaw) * speed;
    vel.y = 0.0f;
    vel.z = Math_CosS(yaw) * speed;

    for (s32 i = 0; i < 5; i++) {
        Vec3f firePos = *pos;
        firePos.x += Rand_CenteredFloat(10.0f);
        firePos.y += Rand_CenteredFloat(10.0f);
        firePos.z += Rand_CenteredFloat(10.0f);

        Vec3f fireVel = vel;
        fireVel.x += Rand_CenteredFloat(2.0f);
        fireVel.y += Rand_CenteredFloat(2.0f);
        fireVel.z += Rand_CenteredFloat(2.0f);

        EffectSsDFire_Spawn(play, &firePos, &fireVel, &accel,
                            (s16)(100 + Rand_ZeroOne() * 50), 20,
                            (s16)(255 - i * 30), (s16)(i + 2), 12);
    }

    for (s32 i = 0; i < 3; i++) {
        Vec3f sparkPos = *pos;
        sparkPos.x += Rand_CenteredFloat(8.0f);
        sparkPos.y += Rand_CenteredFloat(8.0f);
        sparkPos.z += Rand_CenteredFloat(8.0f);
        EffectSsGSpk_SpawnAccel(play, &player->actor, &sparkPos, &vel, &accel, &prim, &env, 100, 12);
    }
}

void FX_SpawnRodFireSmoke(PlayState* play, Vec3f* pos, RodColor* color) {
    Player* player = GET_PLAYER(play);
    Color_RGBA8 prim = { color->primR, color->primG, color->primB, color->primA };
    Color_RGBA8 env = { color->envR, color->envG, color->envB, 0 };
    Vec3f zero = { 0, 0, 0 };

    EffectSsBomb2_SpawnLayered(play, pos, &zero, &zero, 100, 25);

    for (s32 i = 0; i < 8; i++) {
        s16 angle = (s16)(i * 0x2000);
        f32 dist = 20.0f + Rand_ZeroOne() * 30.0f;

        Vec3f smokePos, vel, accel = { 0, -0.1f, 0 };
        smokePos.x = pos->x + Math_SinS(angle) * dist;
        smokePos.y = pos->y + 5.0f;
        smokePos.z = pos->z + Math_CosS(angle) * dist;

        vel.x = Math_SinS(angle) * 5.0f;
        vel.y = 3.0f + Rand_ZeroOne() * 4.0f;
        vel.z = Math_CosS(angle) * 5.0f;

        EffectSsGSpk_SpawnAccel(play, &player->actor, &smokePos, &vel, &accel, &prim, &env, 120, 15);
    }
}

void FX_SpawnRodEnergyBall(PlayState* play, Vec3f* pos, s16 yaw, s16 pitch, RodColor* color) {
    Player* player = GET_PLAYER(play);
    Color_RGBA8 prim = { color->primR, color->primG, color->primB, color->primA };
    Color_RGBA8 env = { color->envR, color->envG, color->envB, 0 };
    Vec3f vel, accel = { 0, 0, 0 };

    f32 speed = 8.0f;
    f32 cosP = Math_CosS(pitch);
    vel.x = Math_SinS(yaw) * cosP * speed;
    vel.y = -Math_SinS(pitch) * speed;
    vel.z = Math_CosS(yaw) * cosP * speed;

    for (s32 i = 0; i < 5; i++) {
        Vec3f firePos = *pos;
        firePos.x += Rand_CenteredFloat(10.0f);
        firePos.y += Rand_CenteredFloat(10.0f);
        firePos.z += Rand_CenteredFloat(10.0f);

        Vec3f fireVel = vel;
        fireVel.x += Rand_CenteredFloat(2.0f);
        fireVel.y += Rand_CenteredFloat(2.0f);
        fireVel.z += Rand_CenteredFloat(2.0f);

        EffectSsDFire_Spawn(play, &firePos, &fireVel, &accel,
                            (s16)(100 + Rand_ZeroOne() * 50), 20,
                            (s16)(255 - i * 30), (s16)(i + 2), 12);
    }

    for (s32 i = 0; i < 3; i++) {
        Vec3f sparkPos = *pos;
        sparkPos.x += Rand_CenteredFloat(8.0f);
        sparkPos.y += Rand_CenteredFloat(8.0f);
        sparkPos.z += Rand_CenteredFloat(8.0f);
        EffectSsGSpk_SpawnAccel(play, &player->actor, &sparkPos, &vel, &accel, &prim, &env, 100, 12);
    }
}

void FX_SpawnRodSwingParticles(PlayState* play, Vec3f* tipPos, RodColor* color) {
    Player* player = GET_PLAYER(play);
    Color_RGBA8 prim = { color->primR, color->primG, color->primB, color->primA };
    Color_RGBA8 env = { color->envR, color->envG, color->envB, 0 };
    Vec3f vel = { 0, 0.5f, 0 };
    Vec3f accel = { 0, 0, 0 };

    EffectSsGSpk_SpawnAccel(play, &player->actor, tipPos, &vel, &accel, &prim, &env, 100, 10);
}

void FX_SpawnRodSpinFire(PlayState* play, Vec3f* center, RodColor* color) {
    Player* player = GET_PLAYER(play);
    Color_RGBA8 prim = { color->primR, color->primG, color->primB, color->primA };
    Color_RGBA8 env = { color->envR, color->envG, color->envB, 0 };

    for (s32 i = 0; i < 8; i++) {
        s16 angle = (s16)(i * 0x2000 + Rand_ZeroOne() * 0x1000);
        f32 dist = 30.0f + Rand_ZeroOne() * 20.0f;

        Vec3f pos, vel, accel = { 0, 0.2f, 0 };
        pos.x = center->x + Math_SinS(angle) * dist;
        pos.y = center->y + 10.0f;
        pos.z = center->z + Math_CosS(angle) * dist;

        vel.x = Math_SinS(angle) * 8.0f;
        vel.y = 2.0f;
        vel.z = Math_CosS(angle) * 8.0f;

        EffectSsGSpk_SpawnAccel(play, &player->actor, &pos, &vel, &accel, &prim, &env, 80, 12);
    }
}

void FX_DrawRodFireball(PlayState* play, Vec3f* pos, f32 scale, RodColor* color, u32 frame) {
    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Xlu(play->state.gfxCtx);

    gSPSegment(POLY_XLU_DISP++, 0x08,
               Gfx_TwoTexScroll(play->state.gfxCtx, 0, 0, 0, 0x20, 0x40, 1, 0,
                                (frame * -20) % 0x200, 0x20, 0x80));

    gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, color->primR, color->primG, color->primB, color->primA);
    gDPSetEnvColor(POLY_XLU_DISP++, color->envR, color->envG, color->envB, 0);

    Matrix_Translate(pos->x, pos->y, pos->z, MTXMODE_NEW);
    Matrix_ReplaceRotation(&play->billboardMtxF);
    Matrix_Scale(scale, scale, 1.0f, MTXMODE_APPLY);

    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, gEffFire1DL);

    CLOSE_DISPS(play->state.gfxCtx);
}

void FX_DrawRodFireSmoke(PlayState* play, Vec3f* pos, f32 scale, RodColor* color, u32 frame) {
    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Xlu(play->state.gfxCtx);

    gSPSegment(POLY_XLU_DISP++, 0x08,
               Gfx_TwoTexScroll(play->state.gfxCtx, 0, 0, 0, 0x20, 0x40, 1, 0,
                                (frame * -15) % 0x200, 0x20, 0x80));

    for (s32 i = 0; i < 5; i++) {
        Vec3f smokePos = *pos;
        s16 angle = (s16)(i * 0x3333);
        f32 dist = 15.0f * scale;
        smokePos.x += Math_SinS(angle) * dist;
        smokePos.y += (f32)i * 8.0f * scale;
        smokePos.z += Math_CosS(angle) * dist;

        u8 alpha = (u8)(255 - i * 40);

        gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, color->primR, color->primG, color->primB, alpha);
        gDPSetEnvColor(POLY_XLU_DISP++, color->envR, color->envG, color->envB, 0);

        Matrix_Translate(smokePos.x, smokePos.y, smokePos.z, MTXMODE_NEW);
        Matrix_ReplaceRotation(&play->billboardMtxF);

        f32 spriteScale = scale * (1.2f - (f32)i * 0.15f);
        Matrix_Scale(spriteScale, spriteScale, 1.0f, MTXMODE_APPLY);

        gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, gEffFire1DL);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void FX_DrawRodEnergyBall(PlayState* play, Vec3f* pos, f32 scale, RodColor* color, u32 frame) {
    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Xlu(play->state.gfxCtx);

    u8 pulse = (u8)(200 + (s32)(55.0f * Math_SinS(frame * 0x1000)));

    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, color->primR, color->primG, color->primB, pulse);
    gDPSetEnvColor(POLY_XLU_DISP++, color->envR, color->envG, color->envB, 180);

    Matrix_Translate(pos->x, pos->y, pos->z, MTXMODE_NEW);
    Matrix_ReplaceRotation(&play->billboardMtxF);

    f32 pulseScale = scale * (1.0f + 0.15f * Math_SinS(frame * 0x1000));
    Matrix_Scale(pulseScale, pulseScale, pulseScale, MTXMODE_APPLY);

    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, gEffBubbleDL);

    CLOSE_DISPS(play->state.gfxCtx);
}

s32 FX_InitSwordTrail(PlayState* play, RodColor* color) {
    EffectBlureInit1 blureInit;
    s32 blureIdx = -1;

    blureInit.p1StartColor[0] = color->primR;
    blureInit.p1StartColor[1] = color->primG;
    blureInit.p1StartColor[2] = color->primB;
    blureInit.p1StartColor[3] = color->primA;

    blureInit.p2StartColor[0] = color->envR;
    blureInit.p2StartColor[1] = color->envG;
    blureInit.p2StartColor[2] = color->envB;
    blureInit.p2StartColor[3] = color->envA;

    blureInit.p1EndColor[0] = color->primR;
    blureInit.p1EndColor[1] = color->primG;
    blureInit.p1EndColor[2] = color->primB;
    blureInit.p1EndColor[3] = 0;

    blureInit.p2EndColor[0] = color->envR;
    blureInit.p2EndColor[1] = color->envG;
    blureInit.p2EndColor[2] = color->envB;
    blureInit.p2EndColor[3] = 0;

    blureInit.elemDuration = 8;
    blureInit.unkFlag = 0;
    blureInit.calcMode = 2;

    Effect_Add(play, &blureIdx, EFFECT_BLURE1, 0, 0, &blureInit);
    return blureIdx;
}

void FX_AddSwordTrailVertex(s32 blureIdx, Vec3f* base, Vec3f* tip) {
    if (blureIdx >= 0) {
        EffectBlure* blure = Effect_GetByIndex(blureIdx);
        if (blure != NULL) {
            EffectBlure_AddVertex(blure, tip, base);
        }
    }
}

void FX_KillSwordTrail(PlayState* play, s32 blureIdx) {
    if (blureIdx >= 0) {
        Effect_Delete(play, blureIdx);
    }
}

void FX_DrawChargeAura(PlayState* play, Player* player, f32 chargeLevel, RodColor* color) {
    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Xlu(play->state.gfxCtx);

    u8 primR, primG, primB, envR, envG, envB;
    u8 alpha;

    if (chargeLevel < 0.85f) {
        f32 t = chargeLevel / 0.85f;
        primR = color->primR;
        primG = color->primG;
        primB = color->primB;
        envR = color->envR;
        envG = color->envG;
        envB = color->envB;
        alpha = (u8)(100 + t * 100);
    } else {
        primR = color->primR;
        primG = color->primG;
        primB = color->primB;
        envR = color->envR;
        envG = color->envG;
        envB = color->envB;
        alpha = 220;
    }

    f32 pulse = 1.0f + 0.1f * Math_SinS((s16)(play->gameplayFrames * 0x800));
    f32 baseScale = 0.02f + chargeLevel * 0.04f;
    f32 scaleXZ = baseScale * pulse;
    f32 scaleY = 0.025f + chargeLevel * 0.015f;

    Vec3f pos;
    pos.x = player->actor.world.pos.x;
    pos.y = player->actor.world.pos.y + 5.0f;
    pos.z = player->actor.world.pos.z;

    u32 scroll = play->gameplayFrames;
    gSPSegment(POLY_XLU_DISP++, 0x08,
               Gfx_TwoTexScroll(play->state.gfxCtx, 0, scroll & 0x7F, 0, 0x20, 0x40, 1, 0,
                                (scroll * -15) & 0xFF, 0x20, 0x40));

    gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, primR, primG, primB, alpha);
    gDPSetEnvColor(POLY_XLU_DISP++, envR, envG, envB, 0);

    Matrix_Translate(pos.x, pos.y, pos.z, MTXMODE_NEW);
    Matrix_RotateY(player->actor.shape.rot.y * (M_PI / 0x8000), MTXMODE_APPLY);
    Matrix_Scale(scaleXZ, scaleY, scaleXZ, MTXMODE_APPLY);

    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, gEffFireCircleDL);

    CLOSE_DISPS(play->state.gfxCtx);
}

void FX_DrawSpinFireCylinder(PlayState* play, Player* player, f32 radius, u8 isBigSpin, RodColor* color) {
    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Xlu(play->state.gfxCtx);

    u8 primR = color->primR;
    u8 primG = color->primG;
    u8 primB = color->primB;
    u8 envR = color->envR;
    u8 envG = color->envG;
    u8 envB = color->envB;
    u8 alpha = isBigSpin ? 220 : 200;

    f32 pulse = 1.0f + 0.05f * Math_SinS((s16)(play->gameplayFrames * 0x1000));
    f32 scaleXZ = (radius / 1000.0f) * pulse;
    f32 scaleY = 0.08f;

    Vec3f pos;
    pos.x = player->actor.world.pos.x;
    pos.y = player->actor.world.pos.y + 5.0f;
    pos.z = player->actor.world.pos.z;

    u32 scroll = play->gameplayFrames;
    gSPSegment(POLY_XLU_DISP++, 0x08,
               Gfx_TwoTexScroll(play->state.gfxCtx, 0, scroll & 0x7F, 0, 0x20, 0x40, 1, 0,
                                (scroll * -20) & 0xFF, 0x20, 0x40));

    gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, primR, primG, primB, alpha);
    gDPSetEnvColor(POLY_XLU_DISP++, envR, envG, envB, 0);

    Matrix_Translate(pos.x, pos.y, pos.z, MTXMODE_NEW);
    Matrix_Scale(scaleXZ, scaleY, scaleXZ, MTXMODE_APPLY);

    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, gEffFireCircleDL);

    CLOSE_DISPS(play->state.gfxCtx);
}

void FX_SpawnFireShockwave(PlayState* play, Vec3f* center, RodColor* color, f32 radius) {
    Vec3f vel = { 0.0f, 0.0f, 0.0f };
    Vec3f accel = { 0.0f, 0.0f, 0.0f };
    Color_RGBA8 prim = { color->primR, color->primG, color->primB, color->primA };
    Color_RGBA8 env = { color->envR, color->envG, color->envB, 0 };

    s16 scale = (s16)(radius * 1.0f);
    s16 scaleStep = (s16)(radius * 3.75f);

    EffectSsBlast_Spawn(play, center, &vel, &accel, &prim, &env, scale, scaleStep, 35, 12);
}

void FX_SpawnFireBurstInRadius(PlayState* play, Vec3f* center, f32 radius, RodColor* color, u8 count) {
    Player* player = GET_PLAYER(play);
    Color_RGBA8 prim = { color->primR, color->primG, color->primB, color->primA };
    Color_RGBA8 env = { color->envR, color->envG, color->envB, 0 };
    Vec3f vel, accel, pos;

    accel.x = 0.0f;
    accel.y = 0.3f;
    accel.z = 0.0f;

    for (u8 i = 0; i < count; i++) {
        s16 angle = Rand_S16Offset(0, 0xFFFF);
        f32 dist = Rand_ZeroOne() * radius;

        pos.x = center->x + Math_SinS(angle) * dist;
        pos.y = center->y + Rand_ZeroOne() * 20.0f;
        pos.z = center->z + Math_CosS(angle) * dist;

        vel.x = Math_SinS(angle) * 3.0f;
        vel.y = 2.0f + Rand_ZeroOne() * 4.0f;
        vel.z = Math_CosS(angle) * 3.0f;

        EffectSsGSpk_SpawnAccel(play, &player->actor, &pos, &vel, &accel, &prim, &env, 100, 20);
    }
}
