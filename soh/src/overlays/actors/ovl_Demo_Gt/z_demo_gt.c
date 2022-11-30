#include "z_demo_gt.h"
#include "objects/object_gt/object_gt.h"
#include "objects/object_geff/object_geff.h"
#include "vt.h"
#include "overlays/effects/ovl_Effect_Ss_Kakera/z_eff_ss_kakera.h"

#define FLAGS (ACTOR_FLAG_4 | ACTOR_FLAG_5)

void DemoGt_Init(Actor* thisx, PlayState* play);
void DemoGt_Destroy(Actor* thisx, PlayState* play);
void DemoGt_Update(Actor* thisx, PlayState* play);
void DemoGt_Draw(Actor* thisx, PlayState* play);

void DemoGt_Destroy(Actor* thisx, PlayState* play) {
    DemoGt* this = (DemoGt*)thisx;

    if ((this->dyna.actor.params == 1) || (this->dyna.actor.params == 2)) {
        DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
    }
}

void DemoGt_PlayEarthquakeSfx() {
    func_800788CC(NA_SE_EV_EARTHQUAKE - SFX_FLAG);
}

void DemoGt_PlayExplosion1Sfx(PlayState* play, Vec3f* pos) {
    SoundSource_PlaySfxAtFixedWorldPos(play, pos, 60, NA_SE_IT_BOMB_EXPLOSION);
}

void DemoGt_PlayExplosion2Sfx(PlayState* play, Vec3f* pos) {
    SoundSource_PlaySfxAtFixedWorldPos(play, pos, 60, NA_SE_EV_GRAVE_EXPLOSION);
}

void DemoGt_Rumble(PlayState* play) {
    func_800AA000(0.0f, 0x32, 0xA, 5);
}

void DemoGt_SpawnDust(PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, f32 scale, s16 scaleStep,
                      s16 life) {
    static Color_RGBA8 brownPrim = { 100, 80, 100, 0 };
    static Color_RGBA8 redEnv = { 255, 110, 96, 0 };

    func_8002843C(play, pos, velocity, accel, &brownPrim, &redEnv, ((Rand_ZeroOne() * (scale * 0.2f)) + scale),
                  scaleStep, life);
}

void func_8097D7D8(PlayState* play, Vec3f* pos, Vec3f* velOffset, f32 scale, s32 arg4, s32 arg5, s16 life) {
    s32 pad;

    if (!FrameAdvance_IsEnabled(play)) {
        s32 frames = play->gameplayFrames;

        if (ABS(frames % arg4) == arg5) {
            s32 pad[2];
            Vec3f velocity = { 0.0f, 6.0f, 0.0f };
            Vec3f accel = { 0.0f, 0.0f, 0.0f };

            velocity.x *= scale;
            velocity.y *= scale;
            velocity.z *= scale;

            velocity.x += velOffset->x;
            velocity.y += velOffset->y;
            velocity.z += velOffset->z;

            accel.x *= scale;
            accel.y *= scale;
            accel.z *= scale;

            DemoGt_SpawnDust(play, pos, &velocity, &accel, (300.0f * scale), (15.0f * scale), life);
        }
    }
}

Actor* DemoGt_SpawnCloudRing(PlayState* play, Vec3f* pos, s16 params) {
    return Actor_Spawn(&play->actorCtx, play, ACTOR_BG_SPOT16_DOUGHNUT, pos->x, pos->y, pos->z, 0, 0, 0,
                       params);
}

void DemoGt_SpawnExplosionWithSound(PlayState* play, Vec3f* pos, f32 scale) {
    s32 pad;
    Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    Vec3f accel = { 0.0f, 0.0f, 0.0f };

    EffectSsBomb2_SpawnLayered(play, pos, &velocity, &accel, (100.0f * scale), (15.0f * scale));
    DemoGt_PlayExplosion1Sfx(play, pos);
}

void DemoGt_SpawnExplosionNoSound(PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, f32 scale) {
    EffectSsBomb2_SpawnLayered(play, pos, velocity, accel, (100.0f * scale), (25.0f * scale));
}

void func_8097DAC8(DemoGt* this, PlayState* play, Vec3f* spawnerPos) {
    Vec3f pos;
    Vec3f velocity;
    f32 temp_f0;
    s16 angle;
    s16 phi_s0;
    s32 i;

    angle = 0;

    for (i = 0; i < 12; i++) {

        pos.x = Math_SinS(angle) * 46.0f;
        pos.y = (Rand_ZeroOne() * 75.0f) + 2.0f;
        pos.z = Math_CosS(angle) * 46.0f;

        velocity.x = (pos.x * 0.1f) + 20.0f;
        velocity.y = Rand_ZeroOne() * 16.0f;
        velocity.z = pos.z * 0.1f;

        pos.x += spawnerPos->x;
        pos.y += spawnerPos->y;
        pos.z += spawnerPos->z;

        temp_f0 = Rand_ZeroOne();

        if (temp_f0 < 0.1f) {
            phi_s0 = 96;
        } else if (temp_f0 < 0.7f) {
            phi_s0 = 64;
        } else {
            phi_s0 = 32;
        }

        EffectSsKakera_Spawn(play, &pos, &velocity, spawnerPos, -247, phi_s0, 3, 0, 0,
                             (s32)(Rand_ZeroOne() * 10.0f + 30.0f), 2, 300, (s32)(Rand_ZeroOne() * 0.0f) + 30,
                             KAKERA_COLOR_NONE, OBJECT_GEFF, gGanonRubbleDL);
        angle += 0x1555;
    }
}

void func_8097DD28(DemoGt* this, PlayState* play, Vec3f* spawnerPos) {
    Vec3f pos;
    Vec3f velocity;
    f32 temp_f0;
    s16 angle;
    s16 phi_s0;
    s32 i;

    angle = 0;

    for (i = 0; i < 8; i++) {

        pos.x = Math_SinS(angle) * 30.0f;
        pos.y = (Rand_ZeroOne() * 75.0f) + 2.0f;
        pos.z = Math_CosS(angle) * 30.0f;

        velocity.x = 0.0f;
        velocity.y = Rand_ZeroOne() * -4.0f;
        velocity.z = pos.z * 0.1f;

        pos.x += spawnerPos->x;
        pos.y += spawnerPos->y;
        pos.z += spawnerPos->z;

        temp_f0 = Rand_ZeroOne();

        if (temp_f0 < 0.1f) {
            phi_s0 = 96;
        } else if (temp_f0 < 0.7f) {
            phi_s0 = 64;
        } else {
            phi_s0 = 32;
        }

        EffectSsKakera_Spawn(play, &pos, &velocity, spawnerPos, -247, phi_s0, 3, 0, 0,
                             (s32)((Rand_ZeroOne() * 10.0f) + 30.0f), 2, 300, (s32)(Rand_ZeroOne() * 0.0f) + 0x1E,
                             KAKERA_COLOR_NONE, OBJECT_GEFF, gGanonRubbleDL);

        angle += 0x2000;
    }
}

void func_8097DF70(DemoGt* this, PlayState* play, Vec3f* spawnerPos) {
    Vec3f pos;
    Vec3f velocity;
    f32 temp_f0;
    s16 angle;
    s16 phi_s0;
    s32 i;

    angle = 0;

    for (i = 0; i < 12; i++) {

        pos.x = Math_SinS(angle) * 16.0f;
        pos.y = (Rand_ZeroOne() * 5.0f) + 2.0f;
        pos.z = Math_CosS(angle) * 16.0f;

        velocity.x = pos.x * 0.6f;
        velocity.y = (Rand_ZeroOne() * 36.0f) + 6.0f;
        velocity.z = pos.z * 0.6f;

        pos.x += spawnerPos->x;
        pos.y += spawnerPos->y;
        pos.z += spawnerPos->z;

        temp_f0 = Rand_ZeroOne();

        if (temp_f0 < 0.1f) {
            phi_s0 = 97;
        } else if (temp_f0 < 0.7f) {
            phi_s0 = 65;
        } else {
            phi_s0 = 33;
        }

        EffectSsKakera_Spawn(play, &pos, &velocity, spawnerPos, -200, phi_s0, 10, 10, 0,
                             Rand_ZeroOne() * 30.0f + 30.0f, 2, 300, (s32)(Rand_ZeroOne() * 30.0f) + 30,
                             KAKERA_COLOR_NONE, OBJECT_GEFF, gGanonRubbleDL);
        angle += 0x1555;
    }
}

void func_8097E1D4(PlayState* play, Vec3f* arg1, s16 arg2) {
    Vec3f pos;
    Vec3f velocity;
    f32 temp_f0;
    s16 angle;
    s16 phi_s0;
    s32 i;

    angle = 0;

    for (i = 0; i < 1; i++) {

        pos.x = Math_SinS(angle) * 46.0f;
        pos.y = (Rand_ZeroOne() * 75.0f) - 28.0f;
        pos.z = Math_CosS(angle) * 46.0f;

        velocity.x = Math_SinS(arg2) * 3.0f;
        velocity.y = (Rand_ZeroOne() * -4.0f) + 10.0f;
        velocity.z = Math_CosS(arg2) * 3.0f;

        pos.x += arg1->x;
        pos.y += arg1->y;
        pos.z += arg1->z;

        temp_f0 = Rand_ZeroOne();

        if (temp_f0 < 0.1f) {
            phi_s0 = 97;
        } else if (temp_f0 < 0.7f) {
            phi_s0 = 65;
        } else {
            phi_s0 = 33;
        }

        EffectSsKakera_Spawn(play, &pos, &velocity, arg1, -247, phi_s0, 3, 0, 0,
                             (s32)((Rand_ZeroOne() * 10.0f) + 30.0f), 2, 300, (s32)(Rand_ZeroOne() * 0.0f) + 30,
                             KAKERA_COLOR_NONE, OBJECT_GEFF, gGanonRubbleDL);

        angle += 0x10000;
    }
}

void func_8097E454(PlayState* play, Vec3f* spawnerPos, Vec3f* velocity, Vec3f* accel, f32 arg4, f32 scale,
                   s32 arg6, s32 arg7, s16 life) {
    s32 pad2[3];
    s16 increment;
    s32 frames;
    s32 i;
    s16 phi_s0;
    s16 dustScaleStep = 15.0f * scale;
    f32 dustScale = 300.0f * scale;
    Vec3f pos;

    if ((!FrameAdvance_IsEnabled(play)) && (arg7 > 0) && (arg6 > 0)) {
        frames = (ABS((s32)play->gameplayFrames) % arg7);
        phi_s0 = 0x10000 * frames / arg6;
        increment = 0x10000 / arg6;

        for (i = frames; i < arg6; i += arg7) {

            pos.x = (Math_SinS(phi_s0) * arg4) + spawnerPos->x;
            pos.y = spawnerPos->y;
            pos.z = (Math_CosS(phi_s0) * arg4) + spawnerPos->z;

            DemoGt_SpawnDust(play, &pos, velocity, accel, dustScale, dustScaleStep, life);

            if (Rand_ZeroOne() <= 0.05f) {
                func_8097E1D4(play, &pos, phi_s0);
            }

            phi_s0 += increment;
        }
    }
}

u8 func_8097E69C(PlayState* play) {
    if (play->csCtx.state == CS_STATE_IDLE) {
        return true;
    } else {
        return false;
    }
}

CsCmdActorAction* DemoGt_GetNpcAction(PlayState* play, u32 actionIdx) {
    s32 pad[2];
    CsCmdActorAction* ret = NULL;

    if (!func_8097E69C(play)) {
        ret = play->csCtx.npcActions[actionIdx];
    }

    return ret;
}

u8 func_8097E704(PlayState* play, u16 arg1, s32 arg2) {
    CsCmdActorAction* action = DemoGt_GetNpcAction(play, arg2);

    if ((action != NULL) && (action->action == arg1)) {
        return true;
    } else {
        return false;
    }
}

void func_8097E744(DemoGt* this, PlayState* play, u32 actionIdx) {
    CsCmdActorAction* npcAction = DemoGt_GetNpcAction(play, actionIdx);
    Vec3f* pos = &this->dyna.actor.world.pos;
    f32 startX;
    f32 startY;
    f32 startZ;
    f32 endX;
    f32 endY;
    f32 endZ;
    f32 someFloat;

    if (npcAction != NULL) {
        someFloat =
            Environment_LerpWeightAccelDecel(npcAction->endFrame, npcAction->startFrame, play->csCtx.frames, 8, 0);
        startX = npcAction->startPos.x;
        startY = npcAction->startPos.y;
        startZ = npcAction->startPos.z;
        endX = npcAction->endPos.x;
        endY = npcAction->endPos.y;
        endZ = npcAction->endPos.z;

        pos->x = ((endX - startX) * someFloat) + startX;
        pos->y = ((endY - startY) * someFloat) + startY;
        pos->z = ((endZ - startZ) * someFloat) + startZ;
    }
}

void func_8097E824(DemoGt* this, s32 arg1) {
    s16 phi_a1;
    s16 phi_a2;
    s16 phi_a3;
    Vec3f* pos = &this->dyna.actor.world.pos;
    Vec3s* unk16C = &this->unk_16C;
    s32 pad;
    f32 tempf3;
    f32 tempf2;
    f32 tempf1;
    f32 phi_f2;
    f32 phi_f12;
    f32 phi_f14;

    if (arg1 == 1) {
        phi_a1 = kREG(19) + 0x8000;
        phi_a2 = kREG(20) + 0x8000;
        phi_a3 = kREG(21) + 0x8000;
        phi_f14 = kREG(16) * 0.1f;
        phi_f12 = (kREG(17) * 0.1f) + 0.5f;
        phi_f2 = kREG(18) * 0.1f;
    } else if (arg1 == 2) {
        phi_a1 = kREG(25) + 0x8000;
        phi_a2 = kREG(26) + 0x8000;
        phi_a3 = kREG(27) + 0x8000;
        phi_f14 = kREG(22) * 0.1f;
        phi_f12 = (kREG(23) * 0.1f) + 0.5f;
        phi_f2 = kREG(24) * 0.1f;
    } else if (arg1 == 3) {
        phi_a1 = kREG(31) + 0x8000;
        phi_a2 = kREG(32) + 0x8000;
        phi_a3 = kREG(33) + 0x8000;
        phi_f14 = kREG(28) * 0.1f;
        phi_f12 = (kREG(29) * 0.1f) + 0.5f;
        phi_f2 = kREG(30) * 0.1f;
    } else if (arg1 == 4) {
        phi_a1 = kREG(37) + 0x8000;
        phi_a2 = kREG(38) + 0x8000;
        phi_a3 = kREG(39) + 0x8000;
        phi_f14 = kREG(34) * 0.1f;
        phi_f12 = (kREG(35) * 0.1f) + 0.5f;
        phi_f2 = kREG(36) * 0.1f;
    } else if (arg1 == 5) {
        phi_a1 = kREG(43) + 0x8000;
        phi_a2 = kREG(44) + 0x8000;
        phi_a3 = kREG(45) + 0x8000;
        phi_f14 = kREG(40) * 0.1f;
        phi_f12 = (kREG(41) * 0.1f) + 0.5f;
        phi_f2 = kREG(42) * 0.1f;
    } else if (arg1 == 6) {
        phi_a1 = kREG(49) + 0x8000;
        phi_a2 = kREG(50) + 0x8000;
        phi_a3 = kREG(51) + 0x8000;
        phi_f14 = kREG(46) * 0.1f;
        phi_f12 = (kREG(47) * 0.1f) + 0.5f;
        phi_f2 = kREG(48) * 0.1f;
    } else if (arg1 == 7) {
        phi_a1 = kREG(85) + 0x8000;
        phi_a2 = kREG(86) + 0x8000;
        phi_a3 = kREG(87) + 0x8000;
        phi_f14 = kREG(82) * 0.1f;
        phi_f12 = (kREG(83) * 0.1f) + 0.5f;
        phi_f2 = kREG(84) * 0.1f;
    } else {
        phi_a1 = kREG(91) + 0x8000;
        phi_a2 = kREG(92) + 0x8000;
        phi_a3 = kREG(93) + 0x8000;
        phi_f14 = kREG(88) * 0.1f;
        phi_f12 = (kREG(89) * 0.1f) + 0.5f;
        phi_f2 = kREG(90) * 0.1f;
    }

    unk16C->x += phi_a1;
    unk16C->y += phi_a2;
    unk16C->z += phi_a3;

    tempf1 = Math_CosS(unk16C->x) * phi_f14;
    tempf2 = Math_CosS(unk16C->y) * phi_f12;
    tempf3 = Math_CosS(unk16C->z) * phi_f2;

    pos->x += tempf1;
    pos->y += tempf2;
    pos->z += tempf3;
}

void func_8097ED64(DemoGt* this, PlayState* play, s32 actionIdx) {
    func_8097E744(this, play, actionIdx);
    func_8097E824(this, actionIdx);
}

u8 func_8097ED94() {
    if (kREG(2) != 0) {
        return true;
    } else if (gSaveContext.sceneSetupIndex < 4) {
        return false;
    } else {
        return true;
    }
}

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void func_8097EDD8(DemoGt* this, PlayState* play, CollisionHeader* collision) {
    s32 pad[3];
    CollisionHeader* colHeader;

    if (collision != NULL) {
        Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
        DynaPolyActor_Init(&this->dyna, DPM_UNK);
        colHeader = NULL;
        CollisionHeader_GetVirtual(collision, &colHeader);
        this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
    }
}

u8 func_8097EE44(DemoGt* this, PlayState* play, s32 updateMode, s32 drawConfig, CollisionHeader* colHeader) {

    if (func_8097ED94()) {
        this->updateMode = updateMode;
        this->drawConfig = drawConfig;
        func_8097EDD8(this, play, colHeader);
        return true;
    } else {
        Actor_Kill(&this->dyna.actor);
        return false;
    }
}

void func_8097EEA8_Init0(DemoGt* this, PlayState* play) {
    this->dyna.actor.scale.x *= 10.0f;
    this->dyna.actor.scale.y *= 10.0f;
    this->dyna.actor.scale.z *= 10.0f;

    func_8097EE44(this, play, 0, 1, NULL);
}

void func_8097EF00(DemoGt* this, PlayState* play) {
    u16 frames = play->csCtx.frames;

    if (frames == 527) {
        func_800F3F3C(13);
    }
}

void func_8097EF34(DemoGt* this, PlayState* play) {
}

void func_8097EF40(DemoGt* this, PlayState* play) {
    u16 frames = play->csCtx.frames;
    s32 pad1[3];
    Vec3f dustPos;
    Vec3f velocity = { 0.0f, -16.0f, 0.0f };
    Vec3f accel = { 0.0f, 1.2f, 0.0f };
    Vec3f* pos = &this->dyna.actor.world.pos;
    s32 pad;

    if ((kREG(1) == 20) || (frames == 220)) {
        dustPos.x = pos->x + 256.0f;
        dustPos.y = pos->y + 679.0f;
        dustPos.z = pos->z + 82.0f;

        DemoGt_SpawnDust(play, &dustPos, &velocity, &accel, 1700.0f, 15, 30);

        dustPos.x = pos->x + 256.0f;
        dustPos.y = pos->y + 679.0f;
        dustPos.z = pos->z - 60.0f;

        DemoGt_SpawnDust(play, &dustPos, &velocity, &accel, 1700.0f, 15, 30);
    }
}

void func_8097F0AC(DemoGt* this, PlayState* play) {
    s32 pad[3];
    Vec3f sp38;
    s16 pad1[3];
    Vec3f sp24;
    u16 frames = play->csCtx.frames;
    s32 pad2;

    if ((frames == 140) || (kREG(1) == 19)) {
        sp38.x = this->dyna.actor.world.pos.x + 260.0f;
        sp38.y = this->dyna.actor.world.pos.y + 340.0f;
        sp38.z = this->dyna.actor.world.pos.z + 45.0f;
        DemoGt_SpawnExplosionWithSound(play, &sp38, 2.0f);
    }

    if (frames == 176) {
        sp24.x = this->dyna.actor.world.pos.x + 260.0f;
        sp24.y = this->dyna.actor.world.pos.y + 840.0f;
        sp24.z = this->dyna.actor.world.pos.z + 45.0f;
        DemoGt_SpawnExplosionWithSound(play, &sp24, 2.0f);
    }
}

void func_8097F19C(DemoGt* this, PlayState* play) {
    func_8097EF34(this, play);
    func_8097EF40(this, play);
    func_8097F0AC(this, play);
}

void func_8097F1D8(DemoGt* this) {
    f32 temp_v0 = this->unk_172;

    this->unk_174 = (temp_v0 * ((kREG(64) * 0.001f) + 0.048f)) + (kREG(72) + 10.0f);
    this->unk_172 += this->unk_174;

    if (this->unk_172 > (s16)(kREG(73) + 0x250)) {
        this->unk_172 = kREG(73) + 0x250;
    }
}

void func_8097F280(DemoGt* this, PlayState* play) {
    s32* unk178 = this->unk_178;
    s32* unk188 = this->unk_188;
    s32* unk198 = this->unk_198;
    f32 temp_f0;

    if (play->csCtx.frames < 160) {
        unk178[0] = 100;
        unk178[1] = 255;
        unk178[2] = 200;

        unk188[0] = 255;
        unk188[1] = 120;
        unk188[2] = 100;

        unk198[0]++;
        unk198[1]--;
    } else if (play->csCtx.frames < 170) {
        temp_f0 = Environment_LerpWeightAccelDecel(170, 160, play->csCtx.frames, 0, 0);

        unk178[0] = (temp_f0 * -63.0f) + 163.0f;
        unk178[1] = (temp_f0 * -155.0f) + 255.0f;
        unk178[2] = temp_f0 * -100.0f + 200.0f;

        unk188[0] = (temp_f0 * -155.0f) + 255.0f;
        unk188[1] = (temp_f0 * -20.0f) + 120.0f;
        unk188[2] = 100;
    } else {
        unk178[0] = 100;
        unk178[1] = 100;
        unk178[2] = 100;

        unk188[0] = 100;
        unk188[1] = 100;
        unk188[2] = 100;
    }
}

void func_8097F3EC(DemoGt* this, PlayState* play) {
    if (func_8097E704(play, 2, 1)) {
        this->updateMode = 8;
    }
}

void DemoGt_Update0(DemoGt* this, PlayState* play) {
    func_8097F280(this, play);
    func_8097E824(this, 1);
    func_8097F19C(this, play);
    func_8097F3EC(this, play);
    DemoGt_PlayEarthquakeSfx();
    DemoGt_Rumble(play);
    func_8097EF00(this, play);
}

void DemoGt_Update8(DemoGt* this, PlayState* play) {
    func_8097F280(this, play);
    func_8097F1D8(this);
    func_8097ED64(this, play, 1);
    func_8097F19C(this, play);
    DemoGt_PlayEarthquakeSfx();
    DemoGt_Rumble(play);
    func_8097EF00(this, play);
}

void DemoGt_Draw1(DemoGt* this, PlayState* play) {
    s32 pad;
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    u32 gameplayFrames = play->gameplayFrames;
    s16 pad2[2];
    s16 spC6;
    f32 spC0;
    f32 spBC;
    s16 spBA;
    s16 spB8;
    Mtx* spB4;
    Vec3f spA8;
    Vec3f sp9C;
    f32 sp98;
    s32* unk198;
    s32* unk188;
    s32* unk178;

    spC6 = this->unk_172;
    spC0 = fabsf(spC6 * (M_PI / 0x8000));
    spBC = kREG(71);
    spB8 = (s16)((s32)kREG(70)) + 0x4000;
    spBA = kREG(70);
    spB4 = Graph_Alloc(gfxCtx, sizeof(Mtx));
    sp98 = 1.0f - Math_CosS(spC6);

    OPEN_DISPS(gfxCtx);

    spA8.x = Math_CosS(spB8);
    spA8.y = 0.0f;
    spA8.z = Math_SinS(spB8);
    sp9C.x = Math_CosS(spBA) * spBC * sp98;
    sp9C.y = Math_SinS(spC6) * spBC;
    sp9C.z = Math_SinS(spBA) * spBC * sp98;

    Matrix_Push();

    Matrix_RotateAxis(spC0, &spA8, MTXMODE_APPLY);
    Matrix_Translate(sp9C.x, sp9C.y, sp9C.z, MTXMODE_APPLY);
    MATRIX_TOMTX(spB4);
    unk198 = this->unk_198;
    unk188 = this->unk_188;
    unk178 = this->unk_178;

    Matrix_Pop();

    Gfx_SetupDL_25Opa(gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08,
               Gfx_TwoTexScrollEnvColor(gfxCtx, 0, 0, unk198[0], 0x20, 0x40, 1, 0, unk198[1], 0x20, 0x40, unk178[0],
                                        unk178[1], unk178[2], 0x80));
    gSPSegment(POLY_OPA_DISP++, 0x0A,
               Gfx_TwoTexScrollEnvColor(gfxCtx, 0, 0, unk198[0], 0x20, 0x40, 1, 0, unk198[1], 0x20, 0x40, unk188[0],
                                        unk188[1], unk188[2], 0x80));
    gSPMatrix(POLY_OPA_DISP++, spB4, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, gTowerCollapseCsExteriorStructureDL);
    Gfx_SetupDL_25Xlu(gfxCtx);
    gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);
    gSPSegment(
        POLY_XLU_DISP++, 0x09,
        Gfx_TwoTexScroll(gfxCtx, 0, 0, gameplayFrames * 0x14, 0x10, 0x200, 1, 0, gameplayFrames * 0x1E, 0x10, 0x200));
    gSPMatrix(POLY_XLU_DISP++, spB4, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, gTowerCollapseCsFlameSmokeDL);

    CLOSE_DISPS(gfxCtx);
}

void func_8097F904_Init1(DemoGt* this, PlayState* play) {
    this->dyna.actor.scale.x *= 10.0f;
    this->dyna.actor.scale.y *= 10.0f;
    this->dyna.actor.scale.z *= 10.0f;
    func_8097EE44(this, play, 1, 2, &gTowerCollapseCsCollapsedStructureInnerCol);
}

void func_8097F960(DemoGt* this, PlayState* play) {
}

void func_8097F96C(DemoGt* this, PlayState* play) {
    static Actor* cloudRing = NULL;
    s32 pad[4];
    Vec3f pos;
    Actor* actor;
    u16 frames = play->csCtx.frames;

    if (((frames > 1059) && (frames < 1062)) || kREG(1) == 17) {
        pos.x = this->dyna.actor.world.pos.x;
        pos.y = this->dyna.actor.world.pos.y + 612.0f;
        pos.z = this->dyna.actor.world.pos.z;

        if (cloudRing == NULL) {
            cloudRing = DemoGt_SpawnCloudRing(play, &pos, 2);
        } else {
            actor = cloudRing;
            actor->world.pos.x = pos.x;
            actor->world.pos.y = pos.y;
            actor->world.pos.z = pos.z;
        }
    }
}

void func_8097FA1C(DemoGt* this, PlayState* play) {
    s32 pad[3];
    Vec3f dustPos;
    u16 frames = play->csCtx.frames;
    Vec3f* pos = &this->dyna.actor.world.pos;
    Vec3f velOffset = { -12.0f, -17.0, 5.0 };
    s32 pad1[3];

    if (((frames > 502) && !(frames >= 581)) || (kREG(1) == 5)) {
        dustPos.x = pos->x + 300.0f;
        dustPos.y = pos->y + 360.0f;
        dustPos.z = pos->z - 377.0f;
        func_8097D7D8(play, &dustPos, &velOffset, 6.0f, 6, 1, 35);
    }
}

void func_8097FAFC(DemoGt* this, PlayState* play) {
    static Vec3f velocity = { 0.0f, 1.0f, 0.0f };
    static Vec3f accel = { 0.0f, 0.0f, 0.0f };
    static f32 arg4 = 280.0f;
    static f32 scale = 8.0f;
    static s32 arg6 = 11;
    static s32 arg7 = 1;
    static s16 life = 3;
    s32 pad[2];
    u16 frames = play->csCtx.frames;
    Vec3f pos;
    f32 new_var = -200.0;

    if (((frames > 582) && (frames < 683)) || (kREG(1) == 6)) {
        pos = this->dyna.actor.world.pos;
        pos.y += 680.0f;

        if (frames == 682) {
            velocity.y += new_var;
        } else if (frames == 681) {
            accel.y += new_var;
        }

        func_8097E454(play, &pos, &velocity, &accel, arg4, scale, arg6, arg7, life);
    }
}

void func_8097FC1C(DemoGt* this, PlayState* play) {
    s32 pad[3];
    Vec3f dustPos;
    u16 frames = play->csCtx.frames;
    Vec3f* pos = &this->dyna.actor.world.pos;
    Vec3f velOffset = { 5.0f, -16.0f, -16.0f };
    s32 pad1[3];

    if (frames > 682 || kREG(1) == 7) {
        dustPos.x = pos->x + 260.0f;
        dustPos.y = pos->y + 360.0f;
        dustPos.z = pos->z + 260.0f;
        func_8097D7D8(play, &dustPos, &velOffset, 6.0f, 6, 0, 35);
    }
}

void func_8097FCE4(DemoGt* this, PlayState* play) {
    s32 pad[3];
    Vec3f vec;
    u16 frames = play->csCtx.frames;

    if (frames == 0x1F7 || kREG(1) == 4) {
        vec.x = this->dyna.actor.world.pos.x + 300.0f;
        vec.y = this->dyna.actor.world.pos.y + 560.0f;
        vec.z = this->dyna.actor.world.pos.z - 377.0f;
        DemoGt_SpawnExplosionWithSound(play, &vec, 2.0f);
    }
}

void func_8097FD70(DemoGt* this, PlayState* play) {
    func_8097F960(this, play);
    func_8097F96C(this, play);
    func_8097FA1C(this, play);
    func_8097FAFC(this, play);
    func_8097FC1C(this, play);
    func_8097FCE4(this, play);
}

void func_8097FDDC(DemoGt* this, PlayState* play) {
    s32* unk178 = this->unk_178;
    s32* unk198 = this->unk_198;

    if (play->csCtx.frames < 610) {
        unk178[0] = 163;
        unk178[1] = 193;
        unk178[2] = 193;
        unk198[0]++;
        unk198[1]--;
    } else if (play->csCtx.frames < 620) {
        f32 temp_f0 = Environment_LerpWeightAccelDecel(620, 610, play->csCtx.frames, 0, 0);

        unk178[0] = (temp_f0 * (-13.0f)) + 163.0f;
        unk178[1] = (temp_f0 * (-43.0f)) + 193.0f;
        unk178[2] = (temp_f0 * (-43.0f)) + 193.0f;
    } else {
        unk178[0] = 150;
        unk178[1] = 150;
        unk178[2] = 150;
    }
}

void func_8097FED8(DemoGt* this, PlayState* play) {
    if (func_8097E704(play, 2, 2)) {
        this->updateMode = 9;
    }
}

void DemoGt_Update1(DemoGt* this, PlayState* play) {
    func_8097FDDC(this, play);
    func_8097E824(this, 2);
    func_8097FD70(this, play);
    func_8097FED8(this, play);
}

void DemoGt_Update9(DemoGt* this, PlayState* play) {
    func_8097FDDC(this, play);
    func_8097ED64(this, play, 2);
    func_8097FD70(this, play);
}

void DemoGt_Draw2(DemoGt* this, PlayState* play) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    s32* unk198;
    s32* unk178;
    s32 pad;

    OPEN_DISPS(gfxCtx);

    Gfx_SetupDL_25Opa(gfxCtx);
    unk198 = this->unk_198;
    unk178 = this->unk_178;
    gSPSegment(POLY_OPA_DISP++, 0x08,
               Gfx_TwoTexScrollEnvColor(gfxCtx, 0, 0, unk198[0], 0x20, 0x40, 1, 0, unk198[1], 0x20, 0x40, unk178[0],
                                        unk178[1], unk178[2], 128));
    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(gfxCtx),
              G_MTX_PUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, gTowerCollapseCsCollapsedStructureInnerDL);
    gSPPopMatrix(POLY_OPA_DISP++, G_MTX_MODELVIEW);

    CLOSE_DISPS(gfxCtx);
}

void func_80980110_Init2(DemoGt* this, PlayState* play) {
    this->dyna.actor.scale.x *= 10.0f;
    this->dyna.actor.scale.y *= 10.0f;
    this->dyna.actor.scale.z *= 10.0f;
    func_8097EE44(this, play, 2, 3, &gTowerCollapseCsCollapsedStructureOuterCol);
}

void func_8098016C(DemoGt* this, PlayState* play) {
}

void func_80980178(DemoGt* this, PlayState* play) {
}

void func_80980184(DemoGt* this, PlayState* play) {
    static Actor* cloudRing = NULL;
    s32 pad[4];
    Vec3f pos;
    Actor* actor;

    if ((play->csCtx.frames > 1027) && (play->csCtx.frames < 1031)) {
        pos.x = this->dyna.actor.world.pos.x;
        pos.y = this->dyna.actor.world.pos.y + 247.0f;
        pos.z = this->dyna.actor.world.pos.z;

        if (cloudRing == NULL) {
            cloudRing = DemoGt_SpawnCloudRing(play, &pos, 3);
        } else {
            actor = cloudRing;
            actor->world.pos.x = pos.x;
            actor->world.pos.y = pos.y;
            actor->world.pos.z = pos.z;
        }
    }
}

void func_80980218(DemoGt* this, PlayState* play) {
    static Actor* cloudRing = NULL;
    s32 pad[4];
    Vec3f pos;
    Actor* actor;

    if ((play->csCtx.frames > 997) && (play->csCtx.frames < 1001)) {
        pos.x = this->dyna.actor.home.pos.x;
        pos.y = this->dyna.actor.home.pos.y + 38.0f;
        pos.z = this->dyna.actor.home.pos.z;

        if (cloudRing == NULL) {
            cloudRing = DemoGt_SpawnCloudRing(play, &pos, 4);
        } else {
            actor = cloudRing;
            actor->world.pos.x = pos.x;
            actor->world.pos.y = pos.y;
            actor->world.pos.z = pos.z;
        }
    }
}

void func_809802AC(DemoGt* this, PlayState* play) {
    s32 pad[3];
    Vec3f dustPos;
    u16 frames = play->csCtx.frames;
    Vec3f* pos = &this->dyna.actor.world.pos;
    Vec3f velOffset = { 0.0f, 0.0f, -10.0f };
    s32 pad1[3];

    if (frames > 109 && frames < 140) {
        dustPos.x = pos->x - 100.0f;
        dustPos.y = pos->y + 1260.0f;
        dustPos.z = pos->z - 323.0f;
        func_8097D7D8(play, &dustPos, &velOffset, 4.0f, 3, 0, 20);
    }
}

void func_8098036C(DemoGt* this, PlayState* play) {
    s32 pad[3];
    Vec3f dustPos;
    u16 frames = play->csCtx.frames;
    Vec3f* world = &this->dyna.actor.world.pos;
    Vec3f velOffset = { 5.0f, -3.0f, 0.0f };
    s32 pad1[3];

    if ((frames > 284) && (frames < 421)) {
        dustPos.x = world->x + 760.0f;
        dustPos.y = world->y - 40.0f;
        dustPos.z = world->z - 240.0f;
        func_8097D7D8(play, &dustPos, &velOffset, 6.0f, 6, 1, 35);
    }
}

void func_80980430(DemoGt* this, PlayState* play) {
    s32 pad[3];
    Vec3f dustPos;
    s32 frames = play->csCtx.frames;
    Vec3f* pos = &this->dyna.actor.world.pos;
    Vec3f velOffset = { 5.0f, -3.0f, 0.0f };
    s32 pad1[3];

    if (frames > 709 || kREG(1) == 8) {
        dustPos.x = pos->x + 760.0f;
        dustPos.y = pos->y - 40.0f;
        dustPos.z = pos->z - 240.0f;
        func_8097D7D8(play, &dustPos, &velOffset, 6.0f, 6, 1, 35);
    }
}

void func_80980504(DemoGt* this, PlayState* play) {
    s32 pad[3];
    Vec3f dustPos;
    u16 frames = play->csCtx.frames;
    Vec3f* pos = &this->dyna.actor.world.pos;
    Vec3f velOffset = { 5.0f, -16.0f, -16.0f };
    s32 pad1[3];

    if ((frames > 704) || kREG(1) == 9) {
        dustPos.x = pos->x + 830.0f;
        dustPos.y = pos->y + 60.0f;
        dustPos.z = pos->z + 390.0f;
        func_8097D7D8(play, &dustPos, &velOffset, 6.0f, 6, 2, 35);
    }
}

void func_809805D8(DemoGt* this, PlayState* play) {
    s32 pad[3];
    Vec3f dustPos;
    u16 frames = play->csCtx.frames;
    Vec3f* homePos = &this->dyna.actor.home.pos;
    Vec3f velOffset = { 15.0f, -26.0, 0.0f };
    s32 pad1[3];

    if (((frames > 739) && (frames < 781)) || kREG(1) == 11) {
        dustPos.x = homePos->x + 550.0f;
        dustPos.y = homePos->y - 110.0f;
        dustPos.z = homePos->z + 50.0f;
        func_8097D7D8(play, &dustPos, &velOffset, 6.0f, 6, 4, 35);
    }
}

void func_809806B8(DemoGt* this, PlayState* playState) {
    s32 pad[3];
    Vec3f dustPos;
    u16 frames = playState->csCtx.frames;
    Vec3f* pos = &this->dyna.actor.world.pos;
    Vec3f velOffset = { 5.0f, -16.0f, -16.0f };
    s32 pad1[3];

    if ((frames > 964) || (kREG(1) == 12)) {
        dustPos.x = pos->x + 460.0f;
        dustPos.y = pos->y + 60.0f;
        dustPos.z = pos->z + 760.0f;
        func_8097D7D8(playState, &dustPos, &velOffset, 6.0f, 6, 3, 35);
    }
}

void func_8098078C(DemoGt* this, PlayState* playState) {
    s32 pad[3];
    Vec3f dustPos;
    u16 frames = playState->csCtx.frames;
    Vec3f* pos = &this->dyna.actor.world.pos;
    Vec3f velOffset = { 5.0f, -16.0f, -16.0f };
    s32 pad1[3];

    if ((frames > 939) || (kREG(1) == 14)) {
        dustPos.x = pos->x + 360.0f;
        dustPos.y = pos->y + 70.0f;
        dustPos.z = pos->z - 640.0f;
        func_8097D7D8(playState, &dustPos, &velOffset, 6.0, 6, 0, 35);
    }
}

void func_8098085C(DemoGt* this, PlayState* play) {
    s32 pad[3];
    Vec3f sp28;
    u16 frames = play->csCtx.frames;
    Vec3f* pos = &this->dyna.actor.world.pos;

    if ((frames == 58) || (kREG(1) == 1)) {
        sp28.x = pos->x + 900.0f;
        sp28.y = pos->y - 50.0f;
        sp28.z = pos->z + 93.0f;
        DemoGt_SpawnExplosionWithSound(play, &sp28, 2.0f);
    } else if (frames == 80) {
        sp28.x = pos->x + 810.0f;
        sp28.y = pos->y + 200.0f;
        sp28.z = pos->z - 37.0f;
        DemoGt_SpawnExplosionWithSound(play, &sp28, 0.9f);
    } else if (frames == 90) {
        sp28.x = pos->x - 220.0f;
        sp28.y = pos->y + 1350.0f;
        sp28.z = pos->z - 287.0f;
        DemoGt_SpawnExplosionWithSound(play, &sp28, 2.0f);
    }
}

void func_809809C0(DemoGt* this, PlayState* play2) {
    PlayState* play = play2;
    DemoGt* this2 = this;
    s32 gameplayFrames = play->gameplayFrames;
    u16 frames = play->csCtx.frames;
    Vec3f sp54;
    s16 pad[3];

    if (((frames > 469) && (frames < 481)) || (kREG(1) == 3)) {
        Vec3f sp40 = { 20.0f, 6.0f, 0.0f };
        Vec3f sp34 = { 0.0f, 0.0f, 0.0f };
        s16 pad2[3];

        sp54.x = this2->dyna.actor.world.pos.x + 790.0f;
        sp54.y = this2->dyna.actor.world.pos.y + 60.0f;
        sp54.z = this2->dyna.actor.world.pos.z + 23.0f;

        if (ABS(gameplayFrames % 12) == 0) {
            DemoGt_SpawnExplosionNoSound(play, &sp54, &sp40, &sp34, 2.0f);
        }
    }
}

void func_80980AD4(DemoGt* this, PlayState* play) {
    s32 pad[4];
    Vec3f pos;
    u16 frames = play->csCtx.frames;

    if ((frames == 477) || (kREG(2) == 1)) {
        pos.x = this->dyna.actor.world.pos.x + 790.0f;
        pos.y = this->dyna.actor.world.pos.y + 60.0f;
        pos.z = this->dyna.actor.world.pos.z + 23.0f;

        func_8097DAC8(this, play, &pos);
        DemoGt_PlayExplosion2Sfx(play, &pos);
    }
}

void func_80980B68(DemoGt* this, PlayState* play) {
    s32 pad[4];
    Vec3f pos;
    u16 frames = play->csCtx.frames;

    if ((frames == 317) || (kREG(3) == 1)) {
        pos.x = this->dyna.actor.world.pos.x + 980.0f;
        pos.y = this->dyna.actor.world.pos.y + 410.0f;
        pos.z = this->dyna.actor.world.pos.z - 177.0f;
        func_8097DD28(this, play, &pos);
        DemoGt_PlayExplosion2Sfx(play, &pos);
    }
}

void func_80980BFC(DemoGt* this, PlayState* play) {
    s32 pad[4];
    Vec3f pos;
    u16 frames = play->csCtx.frames;

    if ((frames == 740) || (kREG(4) == 1)) {
        pos.x = this->dyna.actor.world.pos.x + 790.0f;
        pos.y = this->dyna.actor.world.pos.y + 60.0f;
        pos.z = this->dyna.actor.world.pos.z + 23.0f;

        func_8097DF70(this, play, &pos);
        DemoGt_PlayExplosion2Sfx(play, &pos);
    }
}

void func_80980C90(DemoGt* this, PlayState* play) {
    func_8098016C(this, play);
    func_80980178(this, play);
    func_80980184(this, play);
    func_80980218(this, play);
    func_809802AC(this, play);
    func_8098036C(this, play);
    func_80980430(this, play);
    func_80980504(this, play);
    func_809805D8(this, play);
    func_809806B8(this, play);
    func_8098078C(this, play);
    func_8098085C(this, play);
    func_809809C0(this, play);
    func_80980AD4(this, play);
    func_80980B68(this, play);
    func_80980BFC(this, play);
}

void func_80980D74(DemoGt* this, PlayState* play) {
    if (func_8097E704(play, 2, 3)) {
        this->updateMode = 10;
    }
}

void DemoGt_Update2(DemoGt* this, PlayState* play) {
    func_8097E824(this, 3);
    func_80980C90(this, play);
    func_80980D74(this, play);
}

void DemoGt_Update10(DemoGt* this, PlayState* play) {
    func_8097ED64(this, play, 3);
    func_80980C90(this, play);
}

void DemoGt_Draw3(DemoGt* this, PlayState* play) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;

    OPEN_DISPS(gfxCtx);

    Gfx_SetupDL_25Opa(gfxCtx);
    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(gfxCtx),
              G_MTX_PUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, gTowerCollapseCsCollapsedStructureOuterDL);
    gSPPopMatrix(POLY_OPA_DISP++, G_MTX_MODELVIEW);

    CLOSE_DISPS(gfxCtx);
}

void func_80980F00_Init5(DemoGt* this, PlayState* play) {
    this->dyna.actor.scale.x *= 10.0f;
    this->dyna.actor.scale.y *= 10.0f;
    this->dyna.actor.scale.z *= 10.0f;

    func_8097EE44(this, play, 3, 4, NULL);
}

void func_80980F58(DemoGt* this, PlayState* play) {
    u16 frames = play->csCtx.frames;

    if (frames == 244) {
        func_80078914(&this->dyna.actor.projectedPos, NA_SE_EV_TOWER_PARTS_BROKEN - SFX_FLAG);
    }
}

void func_80980F8C(DemoGt* this, PlayState* play) {
    s32 pad[3];
    Vec3f sp58;
    Vec3f dustPos;
    u16 frames = play->csCtx.frames;

    if ((frames > 259) && (frames < 289)) {
        Vec3f velOfset = { 0.0f, -17.0f, 0.0f };
        s32 pad[3];

        sp58.x = 640.0f;
        sp58.y = 2100.0f;
        sp58.z = -170.0f;

        Matrix_MultVec3f(&sp58, &dustPos);
        func_8097D7D8(play, &dustPos, &velOfset, 3.0f, 5, 0, 30);
    }
}

void func_8098103C(DemoGt* this, PlayState* play) {
    if (func_8097E704(play, 2, 4)) {
        this->updateMode = 11;
    } else if (func_8097E704(play, 3, 4)) {
        this->updateMode = 16;
    }
}

void DemoGt_Update3(DemoGt* this, PlayState* play) {
    func_8097E824(this, 4);
    func_80980F58(this, play);
    func_8098103C(this, play);
}

void DemoGt_Update11(DemoGt* this, PlayState* play) {
    func_8097ED64(this, play, 4);
    func_80980F58(this, play);
}

void DemoGt_Update16(DemoGt* this, PlayState* play) {
    f32 temp = this->unk_172;

    this->unk_174 = (temp * ((kREG(64) * 0.001f) + 0.048f)) + (kREG(65) + 98.0f);
    this->unk_172 += this->unk_174;

    if (this->unk_172 > 0x4000) {
        this->unk_172 = 0x4000;
    }
}

void DemoGt_Draw4(DemoGt* this, PlayState* play2) {
    GraphicsContext* gfxCtx;
    PlayState* play = play2;
    u16 frames = play->csCtx.frames;
    s32 pad;
    s16 sp76;
    f32 sp70;
    f32 sp6C;
    s16 sp6A;
    s16 sp68;
    s16 pad2;
    Mtx* sp60;
    Vec3f sp54;
    Vec3f sp48;
    f32 sp44;

    if (frames < 301) {

        sp76 = this->unk_172;
        sp70 = fabsf(sp76 * (M_PI / 0x8000));
        sp6C = kREG(61);
        sp68 = (s16)((s32)kREG(58)) + 0x4000;
        sp6A = kREG(58);
        gfxCtx = play->state.gfxCtx;
        sp60 = Graph_Alloc(gfxCtx, sizeof(Mtx));
        sp44 = 1.0f - Math_CosS(sp76);

        OPEN_DISPS(gfxCtx);

        sp54.x = Math_CosS(sp68);
        sp54.y = 0.0f;
        sp54.z = Math_SinS(sp68);

        sp48.x = (Math_CosS(sp6A) * sp6C) * sp44;
        sp48.y = Math_SinS(sp76) * sp6C;
        sp48.z = (Math_SinS(sp6A) * sp6C) * sp44;

        Matrix_Push();

        Matrix_RotateAxis(sp70, &sp54, MTXMODE_APPLY);
        Matrix_Translate(sp48.x, sp48.y, sp48.z, MTXMODE_APPLY);
        MATRIX_TOMTX(sp60);

        if (!FrameAdvance_IsEnabled(play)) {
            func_80980F8C(this, play);
        }

        Matrix_Pop();

        Gfx_SetupDL_25Opa(gfxCtx);
        gSPMatrix(POLY_OPA_DISP++, sp60, (G_MTX_PUSH | G_MTX_LOAD) | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_OPA_DISP++, gTowerCollapseCsStandalonePillarDL);
        gSPPopMatrix(POLY_OPA_DISP++, G_MTX_MODELVIEW);

        CLOSE_DISPS(gfxCtx);
    }
}

void func_809813CC_Init6(DemoGt* this, PlayState* play) {
    this->dyna.actor.scale.x *= 10.0f;
    this->dyna.actor.scale.y *= 10.0f;
    this->dyna.actor.scale.z *= 10.0f;

    func_8097EE44(this, play, 4, 5, NULL);
}

void func_80981424(DemoGt* this, PlayState* play) {
    u16 frames = play->csCtx.frames;

    if (frames == 789) {
        func_80078914(&this->dyna.actor.projectedPos, NA_SE_EV_TOWER_PARTS_BROKEN - SFX_FLAG);
    }
}

void func_80981458(DemoGt* this, PlayState* play) {
    s32 pad[3];
    Vec3f sp58;
    Vec3f dustPos;
    u16 frames = play->csCtx.frames;

    if (((frames > 855) && (frames < 891)) || (kREG(1) == 13)) {
        Vec3f velOffset = { 0.0f, -30.0f, 0.0f };
        s32 pad1[3];

        sp58.x = 0.0f;
        sp58.y = 1170.0f;
        sp58.z = -1100.0f;

        Matrix_MultVec3f(&sp58, &dustPos);
        func_8097D7D8(play, &dustPos, &velOffset, 7.1f, 5, 1, 30);
    }
}

void func_80981524(DemoGt* this, PlayState* play) {
    if (func_8097E704(play, 2, 5)) {
        this->updateMode = 12;
    } else if (func_8097E704(play, 3, 5)) {
        this->updateMode = 17;
    }
}

void DemoGt_Update4(DemoGt* this, PlayState* play) {
    func_8097E824(this, 5);
    func_80981424(this, play);
    func_80981524(this, play);
}

void DemoGt_Update12(DemoGt* this, PlayState* play) {
    func_8097ED64(this, play, 5);
    func_80981424(this, play);
}

void DemoGt_Update17(DemoGt* this, PlayState* play) {
    f32 temp = this->unk_172;

    this->unk_174 = temp * ((kREG(66) * 0.001f) + 0.048f) + (kREG(67) + 50.0f);
    this->unk_172 += this->unk_174;

    if (this->unk_172 > 0x4000) {
        this->unk_172 = 0x4000;
    }
}

void DemoGt_Draw5(DemoGt* this, PlayState* play) {
    GraphicsContext* gfxCtx;
    s32 pad;
    s16 sp76;
    f32 sp70;
    f32 sp6C;
    s16 sp6A;
    s16 sp68;
    s16 pad1;
    Mtx* sp60;
    Vec3f sp54;
    Vec3f sp48;
    f32 sp44;

    sp76 = this->unk_172;
    sp70 = fabsf(sp76 * (M_PI / 0x8000));
    sp6C = kREG(62);
    sp6A = kREG(59) - 0x4000;
    sp68 = (s16)(kREG(59) - 0x4000) + 0x4000;
    gfxCtx = play->state.gfxCtx;
    sp60 = Graph_Alloc(gfxCtx, sizeof(Mtx));
    sp44 = 1 - Math_CosS(sp76);

    OPEN_DISPS(gfxCtx);

    sp54.x = Math_CosS(sp68);
    sp54.y = 0.0f;
    sp54.z = Math_SinS(sp68);

    sp48.x = Math_CosS(sp6A) * sp6C * sp44;
    sp48.y = Math_SinS(sp76) * sp6C;
    sp48.z = Math_SinS(sp6A) * sp6C * sp44;

    Matrix_Push();

    Matrix_RotateAxis(sp70, &sp54, MTXMODE_APPLY);
    Matrix_Translate(sp48.x, sp48.y, sp48.z, MTXMODE_APPLY);
    MATRIX_TOMTX(sp60);

    if (!FrameAdvance_IsEnabled(play)) {
        func_80981458(this, play);
    }

    Matrix_Pop();

    Gfx_SetupDL_25Opa(gfxCtx);
    gSPMatrix(POLY_OPA_DISP++, sp60, G_MTX_PUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, gTowerCollapseCsStandalonePillarDL);
    gSPPopMatrix(POLY_OPA_DISP++, G_MTX_MODELVIEW);

    CLOSE_DISPS(gfxCtx);
}

void func_809818A4_Init7(DemoGt* this, PlayState* play) {
    this->dyna.actor.scale.x *= 10.0f;
    this->dyna.actor.scale.y *= 10.0f;
    this->dyna.actor.scale.z *= 10.0f;

    func_8097EE44(this, play, 5, 6, NULL);
}

void func_809818FC(DemoGt* this, PlayState* play) {
    u16 frames = play->csCtx.frames;

    if (frames == 845) {
        func_80078914(&this->dyna.actor.projectedPos, NA_SE_EV_TOWER_PARTS_BROKEN - SFX_FLAG);
    }
}
void func_80981930(DemoGt* this, PlayState* play) {
    if (func_8097E704(play, 2, 6)) {
        this->updateMode = 13;
    } else if (func_8097E704(play, 3, 6)) {
        this->updateMode = 18;
    }
}
void DemoGt_Update5(DemoGt* this, PlayState* play) {
    func_8097E824(this, 6);
    func_809818FC(this, play);
    func_80981930(this, play);
}

void DemoGt_Update13(DemoGt* this, PlayState* play) {
    func_8097ED64(this, play, 6);
    func_809818FC(this, play);
}

void DemoGt_Update18(DemoGt* this, PlayState* play) {
    f32 temp = this->unk_172;

    this->unk_174 = (temp * ((kREG(68) * 0.001f) + 0.005f)) + (kREG(69) + 50.0f);
    this->unk_172 += this->unk_174;

    if (this->unk_172 > 0x4000) {
        this->unk_172 = 0x4000;
    }
}

void DemoGt_Draw6(DemoGt* this, PlayState* play) {
    DemoGt* this2 = this;
    s16 sp78 = this2->unk_172;
    f32 sp74;
    f32 sp70;
    s16 sp6E;
    s16 sp6C;
    GraphicsContext* gfxCtx;
    Mtx* sp64;
    Vec3f sp58;
    Vec3f sp4C;
    f32 sp48;

    sp74 = fabsf(sp78 * (M_PI / 0x8000));
    sp70 = kREG(63);
    sp6E = kREG(60) + 0x4000;
    sp6C = kREG(60) + 0x4000;
    sp6C += 0x4000;
    gfxCtx = play->state.gfxCtx;
    sp64 = Graph_Alloc(gfxCtx, sizeof(Mtx));
    sp48 = 1.0f - Math_CosS(sp78);

    OPEN_DISPS(gfxCtx);

    sp58.x = Math_CosS(sp6C);
    sp58.y = 0.0f;
    sp58.z = Math_SinS(sp6C);

    sp4C.x = Math_CosS(sp6E) * sp70 * sp48;
    sp4C.y = Math_SinS(sp78) * sp70;
    sp4C.z = Math_SinS(sp6E) * sp70 * sp48;

    Matrix_Push();

    Matrix_RotateAxis(sp74, &sp58, MTXMODE_APPLY);
    Matrix_Translate(sp4C.x, sp4C.y, sp4C.z, MTXMODE_APPLY);
    MATRIX_TOMTX(sp64);

    Matrix_Pop();

    Gfx_SetupDL_25Opa(gfxCtx);
    gSPMatrix(POLY_OPA_DISP++, sp64, G_MTX_PUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, gTowerCollapseCsStandalonePillarDL);
    gSPPopMatrix(POLY_OPA_DISP++, G_MTX_MODELVIEW);

    CLOSE_DISPS(gfxCtx);
}

void func_80981C94_Init23(DemoGt* this, PlayState* play) {
    this->dyna.actor.scale.x *= 10.0f;
    this->dyna.actor.scale.y *= 10.0f;
    this->dyna.actor.scale.z *= 10.0f;
    func_8097EE44(this, play, 6, 7, NULL);
}

void func_80981CEC(DemoGt* this, PlayState* play) {
    u16 frames = play->csCtx.frames;

    if (frames == 183) {
        func_80078914(&this->dyna.actor.projectedPos, NA_SE_EV_TOWER_PARTS_BROKEN - SFX_FLAG);
    }
}

void func_80981D20(DemoGt* this) {
    f32 temp = this->unk_172;

    this->unk_174 = temp * ((kREG(64) * 0.001f) + 0.048f) + (kREG(76) + 100.0f);
    this->unk_172 += this->unk_174;

    if (this->unk_172 > (s16)(kREG(80) + 0x4000)) {
        this->unk_172 = kREG(80) + 0x4000;
    }
}

void func_80981DC8(DemoGt* this, PlayState* play) {
    if (func_8097E704(play, 2, 7)) {
        this->updateMode = 0xE;
    }
}

void DemoGt_Update6(DemoGt* this, PlayState* play) {
    func_8097E824(this, 7);
    func_80981CEC(this, play);
    func_80981DC8(this, play);
}

void DemoGt_Update14(DemoGt* this, PlayState* play) {
    func_80981D20(this);
    func_8097ED64(this, play, 7);
    func_80981CEC(this, play);
}

void DemoGt_Draw7(DemoGt* this, PlayState* play) {
    DemoGt* this2 = this;
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    s16 sp6E;
    f32 sp68;
    f32 sp64;
    s16 sp62;
    s16 sp60;
    Mtx* sp5C;
    Vec3f sp50;
    Vec3f sp44;
    f32 sp40;

    sp6E = this2->unk_172;
    sp68 = fabsf(sp6E * (M_PI / 0x8000));
    sp64 = kREG(75);
    sp62 = kREG(74) + 0x7FEC;
    sp60 = kREG(74) + 0x7FEC;
    sp60 = sp60 + 0x4000;
    sp5C = Graph_Alloc(gfxCtx, sizeof(Mtx));
    sp40 = 1.0f - Math_CosS(sp6E);

    OPEN_DISPS(gfxCtx);

    sp50.x = Math_CosS(sp60);
    sp50.y = 0.0f;
    sp50.z = Math_SinS(sp60);

    sp44.x = (Math_CosS(sp62) * sp64) * sp40;
    sp44.y = Math_SinS(sp6E) * sp64;
    sp44.z = (Math_SinS(sp62) * sp64) * sp40;

    Matrix_Push();

    Matrix_RotateAxis(sp68, &sp50, MTXMODE_APPLY);
    Matrix_Translate(sp44.x, sp44.y, sp44.z, MTXMODE_APPLY);
    MATRIX_TOMTX(sp5C);

    Matrix_Pop();

    Gfx_SetupDL_25Opa(gfxCtx);
    gSPMatrix(POLY_OPA_DISP++, sp5C, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, gTowerCollapseCsWalkwayDL);

    CLOSE_DISPS(gfxCtx);
}

void func_80982054_Init24(DemoGt* this, PlayState* play) {
    this->dyna.actor.scale.x *= 10.0f;
    this->dyna.actor.scale.y *= 10.0f;
    this->dyna.actor.scale.z *= 10.0f;
    func_8097EE44(this, play, 7, 8, NULL);
}

void func_809820AC(DemoGt* this, PlayState* play) {
    u16 frames = play->csCtx.frames;

    if (frames == 154) {
        func_80078914(&this->dyna.actor.projectedPos, NA_SE_EV_TOWER_PARTS_BROKEN - SFX_FLAG);
    }
}

void func_809820E0(DemoGt* this) {
    f32 temp = this->unk_172;

    this->unk_174 = (temp * ((kREG(64) * 0.001f) + 0.048f)) + (kREG(79) + 100.0f);
    this->unk_172 += this->unk_174;

    if (this->unk_172 > (s16)(kREG(81) + 0x4000)) {
        this->unk_172 = kREG(81) + 0x4000;
    }
}

void func_80982188(DemoGt* this, PlayState* play) {
    if (func_8097E704(play, 2, 9) != 0) {
        this->updateMode = 15;
    }
}

void DemoGt_Update7(DemoGt* this, PlayState* play) {
    func_8097E824(this, 9);
    func_809820AC(this, play);
    func_80982188(this, play);
}

void DemoGt_Update15(DemoGt* this, PlayState* play) {
    func_809820E0(this);
    func_8097ED64(this, play, 9);
    func_809820AC(this, play);
}

void DemoGt_Draw8(DemoGt* this, PlayState* play) {
    DemoGt* this2 = this;
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    s16 sp6E;
    f32 sp68;
    f32 sp64;
    s16 sp62;
    s16 sp60;
    Mtx* sp5C;
    Vec3f sp50;
    Vec3f sp44;
    f32 sp40;

    sp6E = this2->unk_172;
    sp68 = fabsf(sp6E * (M_PI / 0x8000));
    sp64 = kREG(78);
    sp62 = kREG(77) + 0xBE80;
    sp60 = kREG(77) + 0xBE80;
    sp60 += 0x4000;
    sp5C = Graph_Alloc(gfxCtx, sizeof(Mtx));
    sp40 = 1.0f - Math_CosS(sp6E);

    OPEN_DISPS(gfxCtx);

    sp50.x = Math_CosS(sp60);
    sp50.y = 0.0f;
    sp50.z = Math_SinS(sp60);

    sp44.x = Math_CosS(sp62) * sp64 * sp40;
    sp44.y = Math_SinS(sp6E) * sp64;
    sp44.z = Math_SinS(sp62) * sp64 * sp40;

    Matrix_Push();

    Matrix_RotateAxis(sp68, &sp50, MTXMODE_APPLY);
    Matrix_Translate(sp44.x, sp44.y, sp44.z, MTXMODE_APPLY);
    MATRIX_TOMTX(sp5C);

    Matrix_Pop();

    Gfx_SetupDL_25Opa(gfxCtx);
    gSPMatrix(POLY_OPA_DISP++, sp5C, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, gTowerCollapseCsAlternativeWalkwayDL);

    CLOSE_DISPS(gfxCtx);
}

static DemoGtUpdateFunc sUpdateFuncs[] = {
    DemoGt_Update0,  DemoGt_Update1,  DemoGt_Update2,  DemoGt_Update3,  DemoGt_Update4,
    DemoGt_Update5,  DemoGt_Update6,  DemoGt_Update7,  DemoGt_Update8,  DemoGt_Update9,
    DemoGt_Update10, DemoGt_Update11, DemoGt_Update12, DemoGt_Update13, DemoGt_Update14,
    DemoGt_Update15, DemoGt_Update16, DemoGt_Update17, DemoGt_Update18,
};

void DemoGt_Update(Actor* thisx, PlayState* play) {
    DemoGt* this = (DemoGt*)thisx;
    DemoGtUpdateFunc updateFunc;

    if ((this->updateMode < 0) || (this->updateMode >= 19) || (updateFunc = sUpdateFuncs[this->updateMode]) == NULL) {
        // "The main mode is strange!"
        osSyncPrintf(VT_FGCOL(RED) "メインモードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }

    updateFunc(this, play);
}

void DemoGt_Init(Actor* thisx, PlayState* play) {
    DemoGt* this = (DemoGt*)thisx;

    switch (this->dyna.actor.params) {
        case 0:
            func_8097EEA8_Init0(this, play);
            break;
        case 1:
            func_8097F904_Init1(this, play);
            break;
        case 2:
            func_80980110_Init2(this, play);
            break;
        case 5:
            func_80980F00_Init5(this, play);
            break;
        case 6:
            func_809813CC_Init6(this, play);
            break;
        case 7:
            func_809818A4_Init7(this, play);
            break;
        case 23:
            func_80981C94_Init23(this, play);
            break;
        case 24:
            func_80982054_Init24(this, play);
            break;
        default:
            // "Demo_Gt_Actor_ct There is no such argument !"
            osSyncPrintf("Demo_Gt_Actor_ct そんな引数は無い!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
            Actor_Kill(&this->dyna.actor);
    }
}

void DemoGt_Draw0(DemoGt* this, PlayState* play) {
}

static DemoGtDrawFunc sDrawFuncs[] = {
    DemoGt_Draw0, DemoGt_Draw1, DemoGt_Draw2, DemoGt_Draw3, DemoGt_Draw4,
    DemoGt_Draw5, DemoGt_Draw6, DemoGt_Draw7, DemoGt_Draw8,
};

void DemoGt_Draw(Actor* thisx, PlayState* play) {
    DemoGt* this = (DemoGt*)thisx;
    DemoGtDrawFunc drawFunc;

    if ((this->drawConfig < 0) || (this->drawConfig >= 9) || (drawFunc = sDrawFuncs[this->drawConfig]) == NULL) {
        // "The drawing mode is strange !!!!!!!!!!!!!!!!!!!!!!!!!"
        osSyncPrintf(VT_FGCOL(RED) "描画モードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }

    drawFunc(this, play);
}

const ActorInit Demo_Gt_InitVars = {
    ACTOR_DEMO_GT,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_GT,
    sizeof(DemoGt),
    (ActorFunc)DemoGt_Init,
    (ActorFunc)DemoGt_Destroy,
    (ActorFunc)DemoGt_Update,
    (ActorFunc)DemoGt_Draw,
    NULL,
};
