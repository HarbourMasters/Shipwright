#include "z_boss_ganon2.h"
#include "overlays/actors/ovl_Demo_Gj/z_demo_gj.h"
#include "overlays/actors/ovl_En_Zl3/z_en_zl3.h"
#include "objects/object_ganon/object_ganon.h"
#include "objects/object_ganon2/object_ganon2.h"
#include "objects/object_ganon_anime3/object_ganon_anime3.h"
#include "objects/object_geff/object_geff.h"
#include "soh/frame_interpolation.h"

#include <string.h>

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_4 | ACTOR_FLAG_5)

void BossGanon2_Init(Actor* thisx, PlayState* play);
void BossGanon2_Destroy(Actor* thisx, PlayState* play);
void BossGanon2_Update(Actor* thisx, PlayState* play);
void BossGanon2_Draw(Actor* thisx, PlayState* play);
void BossGanon2_Reset(void);

void func_808FD5C4(BossGanon2* this, PlayState* play);
void func_808FD5F4(BossGanon2* this, PlayState* play);
void func_808FFDB0(BossGanon2* this, PlayState* play);
void func_808FFEBC(BossGanon2* this, PlayState* play);
void func_808FFFE0(BossGanon2* this, PlayState* play);
void func_80900104(BossGanon2* this, PlayState* play);
void func_8090026C(BossGanon2* this, PlayState* play);
void func_809002CC(BossGanon2* this, PlayState* play);
void func_80900344(BossGanon2* this, PlayState* play);
void func_80900580(BossGanon2* this, PlayState* play);
void func_80900650(BossGanon2* this, PlayState* play);
void func_80900890(BossGanon2* this, PlayState* play);
void func_8090120C(BossGanon2* this, PlayState* play);
void func_80905DA8(BossGanon2* this, PlayState* play);
void func_809060E8(PlayState* play);
void BossGanon2_GenShadowTexture(void* shadowTexture, BossGanon2* this, PlayState* play);
void BossGanon2_DrawShadowTexture(void* shadowTexture, BossGanon2* this, PlayState* play);

const ActorInit Boss_Ganon2_InitVars = {
    ACTOR_BOSS_GANON2,
    ACTORCAT_BOSS,
    FLAGS,
    OBJECT_GANON2,
    sizeof(BossGanon2),
    (ActorFunc)BossGanon2_Init,
    (ActorFunc)BossGanon2_Destroy,
    (ActorFunc)BossGanon2_Update,
    (ActorFunc)BossGanon2_Draw,
    (ActorResetFunc)BossGanon2_Reset,
};

#include "z_boss_ganon2_data.c"

Vec3f D_8090EB20;

EnZl3* sBossGanon2Zelda;

Actor* D_8090EB30;

BossGanon2Effect sBossGanon2Particles[100];

s32 sBossGanon2Seed1;
s32 sBossGanon2Seed2;
s32 sBossGanon2Seed3;

Vec3f D_809105D8[4];

Vec3f D_80910608[4];

s8 D_80910638;


void BossGanon2_InitRand(s32 seedInit0, s32 seedInit1, s32 seedInit2) {
    sBossGanon2Seed1 = seedInit0;
    sBossGanon2Seed2 = seedInit1;
    sBossGanon2Seed3 = seedInit2;
}

f32 BossGanon2_RandZeroOne(void) {
    // Wichmann-Hill algorithm
    f32 randFloat;

    sBossGanon2Seed1 = (sBossGanon2Seed1 * 171) % 30269;
    sBossGanon2Seed2 = (sBossGanon2Seed2 * 172) % 30307;
    sBossGanon2Seed3 = (sBossGanon2Seed3 * 170) % 30323;

    randFloat = (sBossGanon2Seed1 / 30269.0f) + (sBossGanon2Seed2 / 30307.0f) + (sBossGanon2Seed3 / 30323.0f);
    while (randFloat >= 1.0f) {
        randFloat -= 1.0f;
    }
    return fabsf(randFloat);
}

void func_808FD080(s32 idx, ColliderJntSph* collider, Vec3f* arg2) {
    collider->elements[idx].dim.worldSphere.center.x = arg2->x;
    collider->elements[idx].dim.worldSphere.center.y = arg2->y;
    collider->elements[idx].dim.worldSphere.center.z = arg2->z;

    collider->elements[idx].dim.worldSphere.radius =
        collider->elements[idx].dim.modelSphere.radius * collider->elements[idx].dim.scale;
}

void BossGanon2_SetObjectSegment(BossGanon2* this, PlayState* play, s32 objectId, u8 setRSPSegment) {
    s32 pad;
    s32 objectIdx = Object_GetIndex(&play->objectCtx, objectId);

    gSegments[6] = PHYSICAL_TO_VIRTUAL(play->objectCtx.status[objectIdx].segment);

    if (setRSPSegment) {
        OPEN_DISPS(play->state.gfxCtx);

        gSPSegment(POLY_OPA_DISP++, 0x06, play->objectCtx.status[objectIdx].segment);
        gSPSegment(POLY_XLU_DISP++, 0x06, play->objectCtx.status[objectIdx].segment);

        CLOSE_DISPS(play->state.gfxCtx);
    }
}

void func_808FD210(PlayState* play, Vec3f* arg1) {
    BossGanon2Effect* effect = play->specialEffects;

    effect->type = 1;
    effect->position = *arg1;
    effect->unk_2E = 0;
    effect->unk_01 = 0;
    effect->velocity.x = 25.0f;
    effect->velocity.y = 15.0f;
    effect->velocity.z = 0.0f;
    effect->accel.x = 0.0f;
    effect->accel.y = -1.0f;
    effect->accel.z = 0.0f;
}

void func_808FD27C(PlayState* play, Vec3f* position, Vec3f* velocity, f32 scale) {
    BossGanon2Effect* effect = play->specialEffects;
    s16 i;

    for (i = 0; i < ARRAY_COUNT(sBossGanon2Particles); i++, effect++) {
        if (effect->type == 0) {
            effect->type = 2;
            effect->position = *position;
            effect->velocity = *velocity;
            effect->accel.x = 0.0;
            effect->accel.y = -1.0f;
            effect->accel.z = 0.0;
            effect->unk_38.z = Rand_ZeroFloat(2 * M_PI);
            effect->unk_38.y = Rand_ZeroFloat(2 * M_PI);
            effect->unk_38.x = Rand_ZeroFloat(2 * M_PI);
            effect->scale = scale;
            break;
        }
    }
}

void BossGanon2_Init(Actor* thisx, PlayState* play) {
    BossGanon2* this = (BossGanon2*)thisx;
    s32 pad;
    s16 i;

    play->specialEffects = sBossGanon2Particles;

    for (i = 0; i < ARRAY_COUNT(sBossGanon2Particles); i++) {
        sBossGanon2Particles[i].type = 0;
    }

    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    this->actor.colChkInfo.health = 30;
    Collider_InitJntSph(play, &this->unk_424);
    Collider_SetJntSph(play, &this->unk_424, &this->actor, &sJntSphInit1, this->unk_464);
    Collider_InitJntSph(play, &this->unk_444);
    Collider_SetJntSph(play, &this->unk_444, &this->actor, &sJntSphInit2, this->unk_864);
    BossGanon2_SetObjectSegment(this, play, OBJECT_GANON, false);
    SkelAnime_InitFlex(play, &this->skelAnime, &gGanondorfSkel, NULL, NULL, NULL, 0);
    func_808FD5C4(this, play);
    this->actor.naviEnemyId = 0x3E;
    this->actor.gravity = 0.0f;
}

void BossGanon2_Destroy(Actor* thisx, PlayState* play) {
    BossGanon2* this = (BossGanon2*)thisx;

    SkelAnime_Free(&this->skelAnime, play);
    Collider_DestroyJntSph(play, &this->unk_424);
    Collider_DestroyJntSph(play, &this->unk_444);
}

void func_808FD4D4(BossGanon2* this, PlayState* play, s16 arg2, s16 arg3) {
    if ((arg2 == 0) || (arg2 == 1)) {
        Actor_SpawnFloorDustRing(play, &this->actor, &this->unk_1D0, 25.0f, arg3, 8.0f, 500, 10, true);
    }

    if ((arg2 == 0) || (arg2 == 2)) {
        Actor_SpawnFloorDustRing(play, &this->actor, &this->unk_1DC, 25.0f, arg3, 8.0f, 500, 10, true);
    }

    Audio_PlayActorSound2(&this->actor, NA_SE_EN_MGANON_WALK);
    func_80033E88(&this->actor, play, 2, 0xA);
}

void func_808FD5C4(BossGanon2* this, PlayState* play) {
    this->actionFunc = func_808FD5F4;
    this->actor.flags &= ~ACTOR_FLAG_0;
    this->actor.world.pos.y = -3000.0f;
}

void func_808FD5F4(BossGanon2* this, PlayState* play) {
    s16 pad;
    u8 sp8D;
    Player* player;
    s32 objectIdx;
    s32 zero = 0;
    s32 pad2;

    sp8D = false;
    player = GET_PLAYER(play);
    this->unk_398++;

    switch (this->unk_39C) {
        case 0:
            objectIdx = Object_GetIndex(&play->objectCtx, OBJECT_GANON_ANIME3);
            if (Object_IsLoaded(&play->objectCtx, objectIdx)) {
                func_80064520(play, &play->csCtx);
                func_8002DF54(play, &this->actor, 8);
                this->unk_39E = Play_CreateSubCamera(play);
                Play_ChangeCameraStatus(play, MAIN_CAM, CAM_STAT_WAIT);
                Play_ChangeCameraStatus(play, this->unk_39E, CAM_STAT_ACTIVE);
                this->unk_39C = 1;
                sBossGanon2Zelda = (EnZl3*)Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EN_ZL3, 970.0f,
                                                    1086.0f, -200.0f, 0, 0, 0, 1);
                sBossGanon2Zelda->unk_3C8 = 0;
                sBossGanon2Zelda->actor.world.pos.x = 970.0f;
                sBossGanon2Zelda->actor.world.pos.y = 1086.0f;
                sBossGanon2Zelda->actor.world.pos.z = -214.0f;
                sBossGanon2Zelda->actor.shape.rot.y = -0x7000;
                this->unk_3BC.x = 0.0f;
                this->unk_3BC.y = 1.0f;
                this->unk_3BC.z = 0.0f;
                this->unk_398 = 0;
                this->unk_3A4.x = 0.0f;
                this->unk_3A4.y = 1400.0f;
                this->unk_3A4.z = 1600.0f;
                player->actor.world.pos.x = 970.0f;
                player->actor.world.pos.y = 1086.0f;
                player->actor.world.pos.z = -186.0f;
                player->actor.shape.rot.y = -0x5000;
                Animation_MorphToLoop(&this->skelAnime, &gGanondorfBurstOutAnim, 0.0f);
                play->envCtx.unk_D8 = 0.0f;
                // fake, tricks the compiler into allocating more stack
                if (zero) {
                    this->unk_3A4.x *= 2.0;
                }
            } else {
                break;
            }
        case 1:
            if (this->unk_398 < 70) {
                play->envCtx.unk_D8 = 0.0f;
            }
            this->unk_339 = 3;
            Math_ApproachF(&this->unk_3A4.x, 1500.0f, 0.1f, this->unk_410.x * 1500.0f);
            Math_ApproachF(&this->unk_3A4.z, -160.0f, 0.1f, this->unk_410.x * 1760.0f);
            Math_ApproachF(&this->unk_410.x, 0.0075f, 1.0f, 0.0001f);
            this->unk_3B0.x = -200.0f;
            this->unk_3B0.y = 1086.0f;
            this->unk_3B0.z = -200.0f;
            if (this->unk_398 == 150) {
                Message_StartTextbox(play, 0x70D3, NULL);
            }
            if (this->unk_398 > 250 && Message_GetState(&play->msgCtx) == TEXT_STATE_NONE) {
                this->unk_39C = 2;
                this->unk_398 = 0;
                this->unk_410.x = 0.0f;
                play->envCtx.unk_D8 = 1.0f;
            } else {
                break;
            }
        case 2:
            this->unk_339 = 4;
            player->actor.world.pos.x = 970.0f;
            player->actor.world.pos.y = 1086.0f;
            player->actor.world.pos.z = -166.0f;
            sBossGanon2Zelda->actor.world.pos.x = 974.0f;
            sBossGanon2Zelda->actor.world.pos.y = 1086.0f;
            sBossGanon2Zelda->actor.world.pos.z = -186.0f;
            player->actor.shape.rot.y = -0x5000;
            sBossGanon2Zelda->actor.shape.rot.y = -0x5000;
            if (this->unk_398 == 60) {
                Message_StartTextbox(play, 0x70D4, NULL);
            }
            if (this->unk_398 == 40) {
                sBossGanon2Zelda->unk_3C8 = 1;
                func_8002DF54(play, &this->actor, 0x4E);
            }
            if (this->unk_398 == 85) {
                sBossGanon2Zelda->unk_3C8 = 2;
                func_8002DF54(play, &this->actor, 0x4F);
            }
            this->unk_3A4.x = 930.0f;
            this->unk_3A4.y = 1129.0f;
            this->unk_3A4.z = -181.0f;
            this->unk_3B0.x = player->actor.world.pos.x;
            this->unk_3B0.z = (player->actor.world.pos.z - 15.0f) + 5.0f;
            if (this->unk_398 > 104) {
                Math_ApproachF(&this->unk_3B0.y, player->actor.world.pos.y + 47.0f + 7.0f + 15.0f, 0.1f,
                               this->unk_410.x);
                Math_ApproachF(&this->unk_410.x, 2.0f, 1.0f, 0.1f);
            } else {
                this->unk_3B0.y = player->actor.world.pos.y + 47.0f + 7.0f;
            }
            if ((this->unk_398 > 170) && (Message_GetState(&play->msgCtx) == TEXT_STATE_NONE)) {
                this->unk_39C = 3;
                this->unk_398 = 0;
                this->unk_410.x = 0.0f;
            }
            break;
        case 3:
            Math_ApproachF(&this->unk_3B0.y, player->actor.world.pos.y + 47.0f + 7.0f, 0.1f, 2.0f);
            this->unk_339 = 4;
            if (this->unk_398 == 10) {
                func_80078914(&D_80906D6C, NA_SE_EV_STONE_BOUND);
                Audio_QueueSeqCmd(SEQ_PLAYER_BGM_MAIN << 24 | NA_BGM_STOP);
            }
            if (this->unk_398 == 20) {
                sBossGanon2Zelda->unk_3C8 = 3;
                func_8002DF54(play, &this->actor, 0x50);
            }
            if (this->unk_398 == 55) {
                this->unk_39C = 4;
                this->unk_398 = 0;
                this->unk_410.x = 0.0f;
                sBossGanon2Zelda->unk_3C8 = 4;
                func_8002DF54(play, &this->actor, 0x50);
            }
            break;
        case 4:
            this->unk_339 = 4;
            Math_ApproachF(&this->unk_3A4.x, -360.0f, 0.1f, this->unk_410.x * 1290.0f);
            Math_ApproachF(&this->unk_3A4.z, -20.0f, 0.1f, this->unk_410.x * 170.0f);
            Math_ApproachF(&this->unk_410.x, 0.04f, 1.0f, 0.0005f);
            if (this->unk_398 == 100) {
                Camera* camera = Play_GetCamera(play, MAIN_CAM);

                camera->eye = this->unk_3A4;
                camera->eyeNext = this->unk_3A4;
                camera->at = this->unk_3B0;
                func_800C08AC(play, this->unk_39E, 0);
                this->unk_39E = 0;
                func_80064534(play, &play->csCtx);
                func_8002DF54(play, &this->actor, 7);
                this->unk_39C = 5;
                this->unk_398 = 0;
            }
            break;
        case 5:
            this->unk_339 = 4;
            if (this->actor.xzDistToPlayer < 500.0f) {
                Message_CloseTextbox(play);
                this->unk_39C = 10;
                this->unk_398 = 0;
                func_80064520(play, &play->csCtx);
                this->unk_39E = Play_CreateSubCamera(play);
                Play_ChangeCameraStatus(play, MAIN_CAM, CAM_STAT_WAIT);
                Play_ChangeCameraStatus(play, this->unk_39E, CAM_STAT_ACTIVE);
            } else {
                break;
            }
        case 10:
            player->actor.world.pos.x = 490.0f;
            player->actor.world.pos.y = 1086.0f;
            player->actor.world.pos.z = -166.0f;
            sBossGanon2Zelda->actor.world.pos.x = 724.0f;
            sBossGanon2Zelda->actor.world.pos.y = 1086.0f;
            sBossGanon2Zelda->actor.world.pos.z = -186.0f;
            player->actor.shape.rot.y = -0x4000;
            sBossGanon2Zelda->actor.shape.rot.y = -0x5000;
            this->unk_3A4.x = 410.0f;
            this->unk_3A4.y = 1096.0f;
            this->unk_3A4.z = -110.0f;
            this->unk_3B0.x = player->actor.world.pos.x + 10.0f;
            this->unk_3B0.y = (player->actor.world.pos.y + 200.0f) - 160.0f;
            this->unk_3B0.z = player->actor.world.pos.z;
            if (this->unk_398 >= 20) {
                func_80078884(NA_SE_EN_GOMA_LAST - SFX_FLAG);
                Math_ApproachF(&this->unk_324, 255.0f, 1.0f, 10.0f);
                this->unk_339 = 5;
                if (this->unk_398 == 20) {
                    this->unk_33C = 0.0f;
                    play->envCtx.unk_D8 = 0.0f;
                }
            } else {
                this->unk_339 = 4;
            }
            if (this->unk_398 == 30) {
                sBossGanon2Zelda->unk_3C8 = 5;
                func_8002DF54(play, &this->actor, 0x51);
            }
            if (this->unk_398 == 50) {
                this->unk_398 = 0;
                this->unk_39C = 11;
            }
            break;
        case 11:
            this->unk_339 = 5;
            func_80078884(NA_SE_EN_GOMA_LAST - SFX_FLAG);
            player->actor.world.pos.x = 490.0f;
            player->actor.world.pos.y = 1086.0f;
            player->actor.world.pos.z = -166.0f;
            sBossGanon2Zelda->actor.world.pos.x = 724.0f;
            sBossGanon2Zelda->actor.world.pos.y = 1086.0f;
            sBossGanon2Zelda->actor.world.pos.z = -186.0f;
            player->actor.shape.rot.y = -0x4000;
            sBossGanon2Zelda->actor.shape.rot.y = -0x5000;
            this->unk_3A4.x = 450.0f;
            this->unk_3A4.y = 1121.0f;
            this->unk_3A4.z = -158.0f;
            this->unk_3B0.x = (player->actor.world.pos.x - 20.0f) + 2.0f;
            this->unk_3B0.y = ((player->actor.world.pos.y + 200.0f) - 151.0f) - 2.0f;
            this->unk_3B0.z = player->actor.world.pos.z + 2.0f;
            if (this->unk_398 == 10) {
                func_80078914(&D_80906D6C, NA_SE_EV_STONE_BOUND);
            }
            if (this->unk_398 == 20) {
                func_80078884(NA_SE_EV_STONE_BOUND);
            }
            if (this->unk_398 == 30) {
                func_8002DF54(play, &this->actor, 0x52);
            }
            if (this->unk_398 == 50) {
                this->unk_398 = 0;
                this->unk_39C = 12;
                Animation_MorphToPlayOnce(&this->skelAnime, &gGanondorfBurstOutAnim, 0.0f);
                this->unk_194 = Animation_GetLastFrame(&gGanondorfBurstOutAnim);
                this->actor.world.pos.x = this->actor.world.pos.z = -200.0f;
                this->actor.world.pos.y = 1009.0f;
                this->actor.shape.yOffset = 7000.0f;
                this->actor.world.rot.y = 0x5000;
                this->unk_3A4.x = -60.0f;
                this->unk_3A4.y = 1106.0f;
                this->unk_3A4.z = -200.0f;
                this->unk_3B0.x = this->unk_3B0.z = -200.0f;
                this->unk_3B0.y = this->actor.world.pos.y + 70.0f;
                play->envCtx.unk_D8 = 0.0f;
                play->envCtx.unk_BE = play->envCtx.unk_BD = 0;
                this->unk_339 = 0;
            } else {
                break;
            }
        case 12:
        case 13:
            SkelAnime_Update(&this->skelAnime);
            if (this->unk_398 == 30) {
                D_80906D78 = 1;
                this->unk_314 = 1;
                func_800A9F6C(0.0f, 0xC8, 0x14, 0x14);
            }
            if (this->unk_398 == 30) {
                func_80078884(NA_SE_EV_GRAVE_EXPLOSION);
            }
            if (this->unk_398 >= 30) {
                Math_ApproachF(&this->actor.world.pos.y, 1289.0f, 0.1f, 10.0f);
                this->unk_3B0.y = this->actor.world.pos.y + 70.0f;
            }
            if (Animation_OnFrame(&this->skelAnime, this->unk_194)) {
                Animation_MorphToLoop(&this->skelAnime, &gGanondorfFloatingHeavyBreathingAnim, 0.0f);
                this->unk_39C = 14;
                this->unk_398 = 0;
                this->actor.world.pos.x = -200.0f;
                this->actor.world.pos.y = this->actor.world.pos.y - 30.0f;
                this->actor.world.pos.z = -200.0f;
                Audio_QueueSeqCmd(SEQ_PLAYER_BGM_MAIN << 24 | NA_BGM_OPENING_GANON);
            } else {
                break;
            }
        case 14:
            SkelAnime_Update(&this->skelAnime);
            Math_ApproachF(&this->actor.world.pos.y, 1289.0f, 0.05f, 1.0f);
            player->actor.world.pos.x = 250.0f;
            player->actor.world.pos.y = 1086.0f;
            player->actor.world.pos.z = -266.0f;
            player->actor.shape.rot.y = -0x4000;
            sBossGanon2Zelda->actor.world.pos.x = 724.0f;
            sBossGanon2Zelda->actor.world.pos.y = 1086.0f;
            sBossGanon2Zelda->actor.world.pos.z = -186.0f;
            this->unk_3A4.x = this->actor.world.pos.x + -10.0f;
            this->unk_3A4.y = this->actor.world.pos.y + 80.0f;
            this->unk_3A4.z = this->actor.world.pos.z + 50.0f;
            this->unk_3B0.x = player->actor.world.pos.x;
            this->unk_3B0.y = player->actor.world.pos.y;
            this->unk_3B0.z = player->actor.world.pos.z - 200.0f;
            if (this->unk_398 == 20) {
                func_8002DF54(play, &this->actor, 0x1E);
            }
            if (this->unk_398 == 60) {
                this->unk_3A4.x = (this->actor.world.pos.x + 200.0f) - 154.0f;
                this->unk_3A4.y = this->actor.world.pos.y + 60.0f;
                this->unk_3A4.z = this->actor.world.pos.z - 15.0f;
                this->unk_39C = 15;
                this->unk_398 = 0;
                this->unk_3B0.y = this->actor.world.pos.y + 77.0f + 100.0f;
                this->unk_314 = 2;
                this->unk_3B0.z = this->actor.world.pos.z + 5.0f;
                this->unk_3B0.x = this->actor.world.pos.x;
            }
            if ((play->gameplayFrames % 32) == 0) {
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_GANON_BREATH);
            }
            break;
        case 15:
            if (((play->gameplayFrames % 32) == 0) && (this->unk_398 < 100)) {
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_GANON_BREATH);
            }
            SkelAnime_Update(&this->skelAnime);
            Math_ApproachF(&this->unk_3B0.y, this->actor.world.pos.y + 77.0f, 0.05f, 5.0f);
            if (this->unk_398 >= 50) {
                if (this->unk_398 == 50) {
                    Animation_MorphToPlayOnce(&this->skelAnime, &gGanondorfShowTriforceStartAnim, 0.0f);
                    this->unk_194 = Animation_GetLastFrame(&gGanondorfShowTriforceStartAnim);
                    this->unk_314 = 3;
                }
                if (Animation_OnFrame(&this->skelAnime, this->unk_194)) {
                    Animation_MorphToLoop(&this->skelAnime, &gGanondorfShowTriforceLoopAnim, 0.0f);
                    this->unk_194 = 1000.0f;
                }
            }
            if (this->unk_398 > 70) {
                Math_ApproachF(&this->unk_1B4, 255.0f, 1.0f, 10.0f);
            }
            if (this->unk_398 == 140) {
                this->unk_39C = 16;
                this->unk_398 = 0;
                Animation_MorphToPlayOnce(&this->skelAnime, &gGanondorfTransformStartAnim, 0.0f);
                this->unk_194 = Animation_GetLastFrame(&gGanondorfTransformStartAnim);
                this->unk_339 = 55;
                play->envCtx.unk_D8 = 1.0f;
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_GANON_CASBREAK);
            } else {
                break;
            }
        case 16:
            if (this->unk_398 < 25) {
                this->unk_339 = 55;
            } else {
                this->unk_339 = 6;
                if (this->unk_194 > 100.0f) {
                    Math_ApproachF(&this->unk_30C, 15.0f, 1.0f, 2.0f);
                } else {
                    Math_ApproachF(&this->unk_30C, 7.0f, 1.0f, 0.2f);
                }
            }
            this->unk_1B4 = 0.0f;
            SkelAnime_Update(&this->skelAnime);
            if (Animation_OnFrame(&this->skelAnime, this->unk_194)) {
                Animation_MorphToPlayOnce(&this->skelAnime, &gGanondorfTransformEndAnim, 0.0f);
                this->unk_194 = 1000.0f;
            }
            Math_ApproachF(&this->unk_3A4.x, (this->actor.world.pos.x + 200.0f) - 90.0f, 0.1f, 6.3999996f);
            Math_ApproachF(&this->unk_3A4.y, ((this->actor.world.pos.y + 60.0f) - 60.0f) - 70.0f, 0.1f, 13.0f);
            Math_ApproachF(&this->unk_3B0.y, this->actor.world.pos.y + 40.0f, 0.1f, 3.6999998f);
            if (this->unk_398 == 30) {
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_GANON_BIGMASIC);
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_GANON_THROW_BIG);
            }
            if (this->unk_398 <= 50) {
                sp8D = true;
            }
            if (this->unk_398 >= 60) {
                Camera* camera = Play_GetCamera(play, MAIN_CAM);

                camera->eye = this->unk_3A4;
                camera->eyeNext = this->unk_3A4;
                camera->at = this->unk_3B0;
                this->unk_39C = 17;
                this->unk_398 = 0;
                this->unk_337 = 2;
                BossGanon2_SetObjectSegment(this, play, OBJECT_GANON2, false);
                SkelAnime_Free(&this->skelAnime, play);
                SkelAnime_InitFlex(play, &this->skelAnime, &gGanonSkel, NULL, NULL, NULL, 0);
                BossGanon2_SetObjectSegment(this, play, OBJECT_GANON_ANIME3, false);
                func_8002DF54(play, &this->actor, 0x54);
                this->unk_314 = 3;

                // At this point, the actor has Ganon's skeleton but is still playing an animation for Ganondorf. This
                // causes issues when trying to access the limb posotions as Ganon has more limbs than Ganondorf. When
                // animating, data from past the end of the animation data is accessed. This is a hack solution so
                // that we are at least playing an animation meant for Ganon. There is no visible change since Ganon is
                // off-screen. There is actually 1 frame where he is visible, and in the vanilla game he is an
                // explosion of limbs since half of them are in random positions from the junk data accessed.
                Animation_PlayOnce(&this->skelAnime, &gGanonUncurlAndFlailAnim);
            }
            // fake, tricks the compiler into using stack the way we need it to
            if (zero) {
                Math_ApproachF(&this->unk_3B0.y, 0.0f, 0.0f, 0.0f);
            }
            break;
        case 17:
            this->unk_339 = 6;
            SkelAnime_Update(&this->skelAnime);
            this->unk_3A4.x = player->actor.world.pos.x - 40.0f;
            this->unk_3A4.y = player->actor.world.pos.y + 40.0f;
            this->unk_3A4.z = player->actor.world.pos.z + 20.0f;
            this->unk_3B0.x = player->actor.world.pos.x;
            this->unk_3B0.y = (player->actor.world.pos.y + 10.0f + 60.0f) - 30.0f;
            this->unk_3B0.z = player->actor.world.pos.z;
            if (this->unk_398 == 25) {
                this->unk_39C = 18;
                this->unk_398 = 0;
                Animation_MorphToPlayOnce(&this->skelAnime, &gGanonUncurlAndFlailAnim, 0.0f);
                this->skelAnime.playSpeed = 0.0f;
                this->unk_3A4.x = ((this->actor.world.pos.x + 500.0f) - 350.0f) - 50.0f;
                this->unk_3A4.y = this->actor.world.pos.y;
                this->unk_3A4.z = this->actor.world.pos.z;
                this->unk_3B0.x = this->actor.world.pos.x + 50.0f;
                this->unk_3B0.y = this->actor.world.pos.y + 60.0f;
                this->unk_3B0.z = this->actor.world.pos.z;
                this->actor.world.rot.y = 0x4000;
            }
            break;
        case 18:
            this->unk_339 = 6;
            if (this->unk_398 == 30) {
                Audio_QueueSeqCmd(SEQ_PLAYER_BGM_MAIN << 24 | NA_BGM_GANON_BOSS);
            }
            Math_ApproachF(&this->unk_30C, 7.0f, 1.0f, 0.1f);
            Math_ApproachF(&this->unk_3A4.x, (this->actor.world.pos.x + 500.0f) - 350.0f, 0.1f, 1.0f);
            Math_ApproachF(&this->unk_3B0.x, this->actor.world.pos.x, 0.1f, 1.0f);
            Math_ApproachF(&this->unk_228, 1.0f, 0.1f, 0.02f);
            if (this->unk_398 == 65) {
                this->unk_39C = 19;
                this->unk_398 = 0;
            }
            break;
        case 19:
            this->unk_394 += 0.5f;
            this->unk_339 = 6;
            this->actor.world.pos.y += this->actor.velocity.y;
            this->actor.velocity.y -= 1.0f;
            if (this->unk_398 == 10) {
                this->unk_39C = 20;
                this->unk_398 = 0;
                this->actor.world.pos.x += 250;
                this->actor.world.pos.y = 1886.0f;
                this->unk_394 = 0.0f;
                func_8002DF54(play, &this->actor, 0x53);
                this->unk_30C = 5.0f;
                this->unk_228 = 1.0f;
            }
            break;
        case 20:
            this->unk_339 = 6;
            SkelAnime_Update(&this->skelAnime);
            this->actor.world.pos.y += this->actor.velocity.y;
            this->actor.velocity.y -= 1.0f;
            player->actor.world.pos.x = 250.0f;
            player->actor.world.pos.y = 1086.0f;
            player->actor.world.pos.z = -266.0f;
            player->actor.shape.rot.y = -0x4000;
            this->unk_3A4.x = (player->actor.world.pos.x - 40.0f) - 200.0f;
            this->unk_3A4.y = (player->actor.world.pos.y + 40.0f) - 30.0f;
            this->unk_3A4.z = (player->actor.world.pos.z - 20.0f) + 100.0f;
            this->unk_3B0.x = player->actor.world.pos.x;
            this->unk_3B0.y = ((player->actor.world.pos.y + 10.0f + 60.0f) - 20.0f) + 30.0f;
            this->unk_3B0.z = player->actor.world.pos.z;
            this->unk_3BC.x = 0.8f;
            if (this->actor.world.pos.y <= 1099.0f) {
                this->actor.world.pos.y = 1099.0f;
                this->unk_39C = 21;
                this->unk_398 = 0;
                this->unk_420 = 10.0f;
                this->actor.velocity.y = 0.0f;
                Animation_MorphToPlayOnce(&this->skelAnime, &gGanonUncurlAndFlailAnim, 0.0f);
                func_808FD4D4(this, play, 0, 3);
                func_800A9F6C(0.0f, 0xC8, 0x14, 0x14);
            }
            break;
        case 21:
            this->unk_339 = 6;
            SkelAnime_Update(&this->skelAnime);
            this->unk_41C = Math_CosS(play->gameplayFrames * 0x8000) * this->unk_420;
            Math_ApproachZeroF(&this->unk_420, 1.0f, 0.75f);
            if (this->unk_398 == 30) {
                this->unk_39C = 22;
                this->unk_30C = 10.0f;
            } else {
                break;
            }
        case 22:
            if (this->unk_398 < 60) {
                this->unk_339 = 7;
            }
            this->unk_3BC.x = 0.0f;
            this->actor.world.pos.y = 1099.0f;
            SkelAnime_Update(&this->skelAnime);
            Math_ApproachZeroF(&this->unk_30C, 1.0f, 0.1f);
            if (this->unk_398 > 50) {
                Math_ApproachF(&this->unk_224, 1.0f, 1.0f, 0.025f);
            }
            if (this->unk_398 == 60) {
                this->unk_336 = 2;
            }
            if (this->unk_398 == 80) {
                BossGanon2_SetObjectSegment(this, play, OBJECT_GANON2, false);
                TitleCard_InitBossName(play, &play->actorCtx.titleCtx,
                                       SEGMENTED_TO_VIRTUAL(gGanonTitleCardTex), 160, 180, 128, 40, true);
                                       //It has translation but they are all the same. they all say "GANON" only
            }
            this->unk_3A4.x = ((this->actor.world.pos.x + 500.0f) - 350.0f) + 100.0f;
            this->unk_3A4.y = this->actor.world.pos.y;
            this->unk_3A4.z = this->actor.world.pos.z;
            this->unk_3B0.x = this->actor.world.pos.x;
            this->unk_3B0.z = this->actor.world.pos.z;
            this->unk_3B0.y = (this->unk_1B8.y + 60.0f) - 40.0f;
            if (this->unk_398 > 166 && this->unk_398 < 173) {
                this->unk_312 = 2;
            }
            if (this->unk_398 > 186 && this->unk_398 < 196) {
                this->unk_312 = 1;
            }
            if (this->unk_398 > 202 && this->unk_398 < 210) {
                this->unk_312 = 2;
            }
            if ((this->unk_398 == 166) || (this->unk_398 == 185) || (this->unk_398 == 200)) {
                func_80078884(NA_SE_EN_MGANON_SWORD);
                func_80078884(NA_SE_EN_MGANON_ROAR);
            }
            if (this->unk_398 == 215) {
                this->unk_39C = 23;
                this->unk_224 = 0.0f;
                func_8002DF54(play, &this->actor, 0x55);
            }
            break;
        case 23:
            SkelAnime_Update(&this->skelAnime);
            if (this->unk_398 > 222 && this->unk_398 < 232) {
                this->unk_312 = 2;
            }
            if (this->unk_398 == 222) {
                func_80078884(NA_SE_EN_MGANON_SWORD);
                func_80078884(NA_SE_EN_MGANON_ROAR);
            }
            this->unk_3A4.x = (player->actor.world.pos.x - 40.0f) + 6.0f;
            this->unk_3A4.y = player->actor.world.pos.y + 40.0f;
            this->unk_3A4.z = (player->actor.world.pos.z + 20.0f) - 7.0f;
            this->unk_3B0.x = player->actor.world.pos.x;
            this->unk_3B0.y = ((player->actor.world.pos.y + 10.0f + 60.0f) - 20.0f) - 2.0f;
            this->unk_3B0.z = player->actor.world.pos.z;
            if (this->unk_398 == 228) {
                func_80078884(NA_SE_IT_SHIELD_REFLECT_SW);
                func_8002DF54(play, &this->actor, 0x56);
                func_800A9F6C(0.0f, 0xFF, 0xA, 0x32);
            }
            if (this->unk_398 >= 229) {
                play->envCtx.fillScreen = true;
                play->envCtx.screenFillColor[0] = play->envCtx.screenFillColor[1] =
                    play->envCtx.screenFillColor[2] = 255;
                play->envCtx.screenFillColor[3] = 100;
                if (this->unk_398 == 234) {
                    Vec3f sp68;

                    play->envCtx.fillScreen = false;
                    this->unk_39C = 24;
                    this->unk_398 = 0;
                    sp68 = player->actor.world.pos;
                    sp68.y += 60.0f;
                    func_808FD210(play, &sp68);
                    play->envCtx.unk_D8 = 0.0f;
                    play->envCtx.unk_BE = 0;
                    this->unk_339 = 0;
                }
            }
            break;
        case 24:
            SkelAnime_Update(&this->skelAnime);
            if (1) {
                BossGanon2Effect* effect = play->specialEffects;

                this->unk_3B0 = effect->position;
                this->unk_3A4.x = effect->position.x + 70.0f;
                this->unk_3A4.y = effect->position.y - 30.0f;
                this->unk_3A4.z = effect->position.z + 70.0f;
            }
            if ((this->unk_398 & 3) == 0) {
                func_80078884(NA_SE_IT_SWORD_SWING);
            }
            if (this->unk_398 == 25) {
                func_8002DF54(play, &this->actor, 0x57);
                this->unk_39C = 25;
                this->unk_398 = 0;
            }
            break;
        case 25:
            SkelAnime_Update(&this->skelAnime);
            this->unk_3A4.x = (player->actor.world.pos.x - 40.0f) + 80.0f;
            this->unk_3A4.y = player->actor.world.pos.y + 40.0f + 10.0f;
            this->unk_3A4.z = player->actor.world.pos.z + 20.0f + 10.0f;
            this->unk_3B0.x = player->actor.world.pos.x - 20.0f;
            this->unk_3B0.y = ((player->actor.world.pos.y + 10.0f + 60.0f) - 20.0f) - 3.0f;
            this->unk_3B0.z = (player->actor.world.pos.z - 40.0f) - 10.0f;
            if (this->unk_398 == 10) {
                BossGanon2Effect* effect = play->specialEffects;

                effect->unk_2E = 1;
                effect->position.x = sBossGanon2Zelda->actor.world.pos.x + 50.0f + 10.0f;
                effect->position.y = sBossGanon2Zelda->actor.world.pos.y + 350.0f;
                effect->position.z = sBossGanon2Zelda->actor.world.pos.z - 25.0f;
                effect->velocity.x = 0.0f;
                effect->velocity.z = 0.0f;
                effect->velocity.y = -30.0f;
                this->unk_39C = 26;
                this->unk_398 = 0;
            } else {
                break;
            }
        case 26:
            this->unk_3A4.x = sBossGanon2Zelda->actor.world.pos.x + 100.0f + 30.0f;
            this->unk_3A4.y = sBossGanon2Zelda->actor.world.pos.y + 10.0f;
            this->unk_3A4.z = sBossGanon2Zelda->actor.world.pos.z + 5.0f;
            this->unk_3B0.x = sBossGanon2Zelda->actor.world.pos.x;
            this->unk_3B0.y = sBossGanon2Zelda->actor.world.pos.y + 30.0f;
            this->unk_3B0.z = sBossGanon2Zelda->actor.world.pos.z - 20.0f;
            this->unk_3BC.z = -0.5f;
            if (this->unk_398 == 13) {
                sBossGanon2Zelda->unk_3C8 = 6;
            }
            if (this->unk_398 == 50) {
                this->unk_39C = 27;
                this->unk_398 = 0;
            }
            break;
        case 27:
            this->unk_3BC.z = 0.0f;
            if (this->unk_398 == 4) {
                func_8002DF54(play, &this->actor, 0x58);
            }
            this->unk_3A4.x = player->actor.world.pos.x - 20.0f;
            this->unk_3A4.y = player->actor.world.pos.y + 50.0f;
            this->unk_3A4.z = player->actor.world.pos.z;
            this->unk_3B0.x = player->actor.world.pos.x;
            this->unk_3B0.y = player->actor.world.pos.y + 50.0f;
            this->unk_3B0.z = player->actor.world.pos.z;
            if (this->unk_398 == 26) {
                D_8090EB30 = play->actorCtx.actorLists[ACTORCAT_ITEMACTION].head;
                while (D_8090EB30 != NULL) {
                    if (D_8090EB30->id == ACTOR_EN_ELF) {
                        this->unk_3A4.x = D_8090EB30->world.pos.x - 30.0f;
                        this->unk_3A4.y = D_8090EB30->world.pos.y;
                        this->unk_3A4.z = D_8090EB30->world.pos.z;
                        this->unk_3B0.x = D_8090EB30->world.pos.x;
                        this->unk_3B0.y = D_8090EB30->world.pos.y;
                        this->unk_3B0.z = D_8090EB30->world.pos.z;
                        break;
                    }
                    D_8090EB30 = D_8090EB30->next;
                }
                this->unk_39C = 28;
                this->unk_398 = 0;
            }
            break;
        case 28:
            if (this->unk_398 == 5) {
                Message_StartTextbox(play, 0x70D6, NULL);
            }
            if (D_8090EB30 != NULL) {
                this->unk_3A4.x = D_8090EB30->world.pos.x - 20.0f;
                this->unk_3A4.y = D_8090EB30->world.pos.y;
                this->unk_3A4.z = D_8090EB30->world.pos.z;
                Math_ApproachF(&this->unk_3B0.x, D_8090EB30->world.pos.x, 0.2f, 50.0f);
                Math_ApproachF(&this->unk_3B0.y, D_8090EB30->world.pos.y, 0.2f, 50.0f);
                Math_ApproachF(&this->unk_3B0.z, D_8090EB30->world.pos.z, 0.2f, 50.0f);
                if ((this->unk_398 > 40) && (Message_GetState(&play->msgCtx) == TEXT_STATE_NONE)) {
                    this->unk_39C = 29;
                    this->unk_398 = 0;
                    Animation_MorphToPlayOnce(&this->skelAnime, &gGanonRoarAnim, 0.0f);
                    this->unk_194 = Animation_GetLastFrame(&gGanonRoarAnim);
                    this->actor.shape.yOffset = 0.0f;
                    this->actor.world.pos.y = 1086.0f;
                    this->actor.gravity = -1.0f;
                    this->unk_335 = 1;
                    this->unk_224 = 1.0f;
                }
            }
            break;
        case 29:
            SkelAnime_Update(&this->skelAnime);
            this->unk_3A4.x = (((this->actor.world.pos.x + 500.0f) - 350.0f) + 100.0f) - 60.0f;
            this->unk_3B0.x = this->actor.world.pos.x;
            this->unk_3B0.z = this->actor.world.pos.z;
            this->unk_3A4.y = this->actor.world.pos.y;
            this->unk_3A4.z = this->actor.world.pos.z + 10.0f;
            this->unk_3B0.y = (this->unk_1B8.y + 60.0f) - 40.0f;
            player->actor.shape.rot.y = -0x4000;
            player->actor.world.pos.x = 140.0f;
            player->actor.world.pos.z = -196.0f;
            if (this->unk_398 == 50) {
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_MGANON_ROAR);
            }
            if (Animation_OnFrame(&this->skelAnime, this->unk_194)) {
                Camera* camera = Play_GetCamera(play, MAIN_CAM);

                camera->eye = this->unk_3A4;
                camera->eyeNext = this->unk_3A4;
                camera->at = this->unk_3B0;
                func_800C08AC(play, this->unk_39E, 0);
                this->unk_39E = 0;
                func_80064534(play, &play->csCtx);
                func_8002DF54(play, &this->actor, 7);
                this->unk_39C = 0;
                this->unk_337 = 1;
                func_808FFDB0(this, play);
                this->unk_1A2[1] = 50;
                this->actor.flags |= ACTOR_FLAG_0;
                sBossGanon2Zelda->unk_3C8 = 7;
            }
            break;
    }

    if ((this->unk_30C > 4.0f) && !sp8D) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_GANON_BODY_SPARK - SFX_FLAG);
    }

    if (this->unk_39E != 0) {
        // fake, tricks the compiler into putting some pointers on the stack
        if (zero) {
            osSyncPrintf(NULL, 0, 0);
        }
        this->unk_3B0.y += this->unk_41C;
        Play_CameraSetAtEyeUp(play, this->unk_39E, &this->unk_3B0, &this->unk_3A4, &this->unk_3BC);
    }
}

void func_808FF898(BossGanon2* this, PlayState* play) {
    if ((this->unk_312 != 0) && (this->unk_39E == 0)) {
        Actor* actor = play->actorCtx.actorLists[ACTORCAT_PROP].head;
        while (actor != NULL) {
            if (actor->id == ACTOR_DEMO_GJ) {
                DemoGj* gj = (DemoGj*)actor;

                if (((actor->params & 0xFF) == 0x10) || ((actor->params & 0xFF) == 0x11) ||
                    ((actor->params & 0xFF) == 0x16)) {
                    if (SQ(this->unk_218.x - gj->dyna.actor.world.pos.x) +
                            SQ(this->unk_218.z - gj->dyna.actor.world.pos.z) <
                        SQ(100.0f)) {
                        s32 pad;
                        Vec3f sp28;

                        Matrix_RotateY(((this->actor.shape.rot.y / (f32)0x8000) * M_PI) + 0.5f, MTXMODE_NEW);
                        sp28.x = 0.0f;
                        sp28.y = 0.0f;
                        sp28.z = 1.0f;
                        Matrix_MultVec3f(&sp28, &gj->unk_26C);
                        gj->killFlag = true;
                        func_800A9F6C(0.0f, 0x96, 0x14, 0x32);
                        this->unk_392 = 6;
                        return;
                    }
                }
            }

            actor = actor->next;
        }

        if (this->unk_392 == 4) {
            func_80078884(NA_SE_EV_GRAVE_EXPLOSION);
        }

        if (this->unk_392 == 3) {
            func_80078884(NA_SE_EN_MGANON_SWDIMP);
        }
    }
}

s32 func_808FFA24(BossGanon2* this, PlayState* play) {
    Actor* actor = play->actorCtx.actorLists[ACTORCAT_PROP].head;

    while (actor != NULL) {
        if (actor->id == ACTOR_DEMO_GJ) {
            DemoGj* gj = (DemoGj*)actor;

            if (((actor->params & 0xFF) == 0x10) || ((actor->params & 0xFF) == 0x11) ||
                ((actor->params & 0xFF) == 0x16)) {
                if (SQ(this->actor.world.pos.x - gj->dyna.actor.world.pos.x) +
                        SQ(this->actor.world.pos.z - gj->dyna.actor.world.pos.z) <
                    SQ(200.0f)) {
                    return true;
                }
            }
        }

        actor = actor->next;
    }

    return false;
}

void func_808FFAC8(BossGanon2* this, PlayState* play, u8 arg2) {
    s16 temp_v1;
    s16 phi_a1;

    if (this->unk_313 || (arg2 != 0)) {
        phi_a1 = this->actor.shape.rot.y - this->actor.yawTowardsPlayer;

        if (phi_a1 > 0x3000) {
            phi_a1 = 0x3000;
        } else if (phi_a1 < -0x3000) {
            phi_a1 = -0x3000;
        }
    } else if (this->unk_19C & 0x20) {
        phi_a1 = 0x3000;
    } else {
        phi_a1 = -0x3000;
    }

    Math_ApproachS(&this->unk_31A, phi_a1, 5, 0x7D0);

    temp_v1 = Math_Atan2S(this->actor.xzDistToPlayer, 150.0f) - 0xBB8;
    temp_v1 = CLAMP_MAX(temp_v1, 0x1B58);
    temp_v1 = CLAMP_MIN(temp_v1, -0x1B58);

    Math_ApproachS(&this->unk_31C, temp_v1, 5, 0x7D0);
}

void func_808FFBBC(BossGanon2* this, PlayState* play, u8 arg2) {
    if (arg2 != 0 || this->unk_313) {
        f32 phi_f0;
        f32 phi_f2;

        Math_ApproachS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 5, this->unk_320);

        if (this->unk_334 != 0) {
            phi_f0 = 5000.0f;
            phi_f2 = 200.0f;
        } else {
            phi_f0 = 3000.0f;
            phi_f2 = 30.0f;
        }

        Math_ApproachF(&this->unk_320, phi_f0, 1.0f, phi_f2);
    } else {
        this->unk_320 = 0.0f;
    }
}

void func_808FFC84(BossGanon2* this) {
    if (ABS((s16)(this->actor.yawTowardsPlayer - this->actor.shape.rot.y)) < 0x2800) {
        this->unk_313 = true;
        this->actor.focus.pos = this->unk_1B8;
    } else {
        this->unk_313 = false;
        this->actor.focus.pos = this->unk_1C4;
    }
}

void func_808FFCFC(BossGanon2* this, PlayState* play) {
    if (this->actor.xzDistToPlayer < 150.0f &&
        ABS((s16)(this->actor.yawTowardsPlayer - this->actor.shape.rot.y)) < 0x2800) {
        this->unk_311 = false;
        func_80900580(this, play);
        Audio_StopSfxById(NA_SE_EN_MGANON_UNARI);
    } else if ((this->actor.bgCheckFlags & 8) && func_808FFA24(this, play)) {
        this->unk_311 = false;
        func_80900580(this, play);
        Audio_StopSfxById(NA_SE_EN_MGANON_UNARI);
    }
}

void func_808FFDB0(BossGanon2* this, PlayState* play) {
    s32 sp28;
    s32 objectIdx = Object_GetIndex(&play->objectCtx, OBJECT_GANON2);

    if (Object_IsLoaded(&play->objectCtx, objectIdx)) {
        gSegments[6] = PHYSICAL_TO_VIRTUAL(play->objectCtx.status[objectIdx].segment);
        Animation_MorphToLoop(&this->skelAnime, &gGanonGuardIdleAnim, -10.0f);
        this->actionFunc = func_808FFEBC;

        if (this->unk_334 != 0) {
            this->unk_1A2[0] = Rand_ZeroFloat(30.0f);
        } else {
            this->unk_1A2[0] = 40;
        }

        this->unk_336 = 1;
        this->actor.flags |= ACTOR_FLAG_0;
        this->unk_228 = 1.0f;
        this->unk_224 = 1.0f;
    } else {
        this->actionFunc = func_808FFDB0;
    }
}

void func_808FFEBC(BossGanon2* this, PlayState* play) {
    if (this->unk_390 == 0) {
        this->unk_390 = (s16)Rand_ZeroFloat(50.0f) + 30;
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_MGANON_UNARI);
    }

    SkelAnime_Update(&this->skelAnime);
    Math_ApproachZeroF(&this->actor.speedXZ, 0.5f, 1.0f);

    if (this->unk_1A2[0] == 0) {
        func_809002CC(this, play);
    } else if (this->unk_1A2[1] == 0) {
        func_808FFCFC(this, play);
    }

    func_808FFAC8(this, play, 0);
    func_808FFBBC(this, play, 0);
}

void func_808FFF90(BossGanon2* this, PlayState* play) {
    Animation_MorphToLoop(&this->skelAnime, &gGanonGuardIdleAnim, -10.0f);
    this->actionFunc = func_808FFFE0;
    this->unk_1A2[0] = 40;
}

void func_808FFFE0(BossGanon2* this, PlayState* play) {
    s16 target;

    SkelAnime_Update(&this->skelAnime);
    Math_ApproachZeroF(&this->actor.speedXZ, 0.5f, 1.0f);

    if (this->unk_1A2[0] == 0) {
        func_809002CC(this, play);
    }

    if (this->unk_1A2[0] < 30 && this->unk_1A2[0] >= 10) {
        target = Math_SinS(this->unk_1A2[0] * 0x3000) * (f32)0x2000;
    } else {
        target = 0;
    }

    Math_ApproachS(&this->unk_31A, target, 2, 0x4000);
}

void func_809000A0(BossGanon2* this, PlayState* play) {
    Animation_MorphToLoop(&this->skelAnime, &gGanonStunStartAnim, -2.0f);
    this->unk_194 = Animation_GetLastFrame(&gGanonStunStartAnim);
    this->unk_1AC = 0;
    this->actionFunc = func_80900104;
}

void func_80900104(BossGanon2* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    Math_ApproachZeroF(&this->actor.speedXZ, 0.5f, 1.0f);

    switch (this->unk_1AC) {
        case 0:
            if (Animation_OnFrame(&this->skelAnime, this->unk_194)) {
                this->unk_1AC = 1;
                Animation_MorphToLoop(&this->skelAnime, &gGanonStunLoopAnim, 0.0f);
                this->unk_1A2[0] = 80;
            }
            break;
        case 1:
            if (this->unk_1A2[0] == 0) {
                this->unk_1AC = 2;
                Animation_MorphToLoop(&this->skelAnime, &gGanonStunEndAnim, -5.0f);
                this->unk_194 = Animation_GetLastFrame(&gGanonStunEndAnim);
            }
            break;
        case 2:
            if (Animation_OnFrame(&this->skelAnime, this->unk_194)) {
                func_809002CC(this, play);
            }
            break;
    }
}

void func_80900210(BossGanon2* this, PlayState* play) {
    Animation_MorphToPlayOnce(&this->skelAnime, &gGanonDamageAnim, -3.0f);
    this->unk_194 = Animation_GetLastFrame(&gGanonDamageAnim);
    this->actionFunc = func_8090026C;
}

void func_8090026C(BossGanon2* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    Math_ApproachZeroF(&this->actor.speedXZ, 0.5f, 2.0f);

    if (Animation_OnFrame(&this->skelAnime, this->unk_194)) {
        func_809002CC(this, play);
    }
}

void func_809002CC(BossGanon2* this, PlayState* play) {
    Animation_MorphToLoop(&this->skelAnime, &gGanonGuardWalkAnim, -10.0f);
    this->actionFunc = func_80900344;
    this->unk_338 = 0;
    this->unk_1A2[0] = 100;
    this->unk_390 = (s16)Rand_ZeroFloat(50.0f) + 50;
}

void func_80900344(BossGanon2* this, PlayState* play) {
    f32 phi_f0;

    if (this->unk_390 == 0) {
        this->unk_390 = (s16)Rand_ZeroFloat(50.0f) + 30;
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_MGANON_UNARI);
    }

    Math_ApproachF(&this->unk_324, 255.0f, 1.0f, 10.0f);

    if (this->unk_338 != 0) {
        if (Animation_OnFrame(&this->skelAnime, 13.0f)) {
            func_808FD4D4(this, play, 1, 3);
        } else if (Animation_OnFrame(&this->skelAnime, 28.0f)) {
            func_808FD4D4(this, play, 2, 3);
        }
        if (this->actor.xzDistToPlayer < 200.0f) {
            this->unk_338 = 0;
            Animation_MorphToLoop(&this->skelAnime, &gGanonGuardWalkAnim, -10.0f);
        } else {
            this->skelAnime.playSpeed = ((this->actor.xzDistToPlayer - 300.0f) * 0.005f) + 1.0f;
            if (this->skelAnime.playSpeed > 2.0f) {
                this->skelAnime.playSpeed = 2.0f;
            }
            if (this->unk_334 != 0) {
                this->skelAnime.playSpeed *= 1.5f;
            }
        }
        phi_f0 = this->skelAnime.playSpeed * 3.0f;
    } else {
        phi_f0 = 2.0f;
        if (this->actor.xzDistToPlayer >= 200.0f) {
            this->unk_338 = 1;
            Animation_MorphToLoop(&this->skelAnime, &gGanonWalkAnim, -10.0f);
        }
    }

    SkelAnime_Update(&this->skelAnime);
    Math_ApproachF(&this->actor.speedXZ, phi_f0, 0.5f, 1.0f);

    if (this->unk_1A2[0] == 0) {
        func_808FFDB0(this, play);
    } else {
        func_808FFCFC(this, play);
    }

    func_808FFAC8(this, play, 1);
    func_808FFBBC(this, play, 1);
}

void func_80900580(BossGanon2* this, PlayState* play) {
    if (this->unk_311 == 0) {
        Animation_MorphToPlayOnce(&this->skelAnime, &gGanonLeftSwordSwingAnim, -5.0f);
        this->unk_194 = Animation_GetLastFrame(&gGanonLeftSwordSwingAnim);
        this->unk_198 = (this->unk_194 - 15.0f) - 5.0f;
    } else {
        Animation_MorphToPlayOnce(&this->skelAnime, &gGanonRightSwordSwingAnim, -5.0f);
        this->unk_194 = Animation_GetLastFrame(&gGanonRightSwordSwingAnim);
        this->unk_198 = (this->unk_194 - 15.0f) - 5.0f;
    }

    this->actionFunc = func_80900650;
}

void func_80900650(BossGanon2* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);

    if (Animation_OnFrame(&this->skelAnime, this->unk_198)) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_MGANON_SWORD);
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_MGANON_ROAR);
    }

    if (this->unk_311 == 0) {
        if (((this->unk_198 - 4.0f) < this->skelAnime.curFrame) &&
            (this->skelAnime.curFrame < (this->unk_198 + 6.0f))) {
            this->unk_312 = 1;
        }
    } else if ((((this->unk_198 - 4.0f) + 4.0f) < this->skelAnime.curFrame) &&
               (this->skelAnime.curFrame < (this->unk_198 + 6.0f))) {
        this->unk_312 = 2;
    }

    Math_ApproachZeroF(&this->actor.speedXZ, 0.5f, 1.0f);

    if (Animation_OnFrame(&this->skelAnime, this->unk_194)) {
        this->unk_311 = 1 - this->unk_311;

        if ((this->unk_311 == 1) && (this->actor.xzDistToPlayer < 250.0f) && this->unk_313) {
            func_80900580(this, play);
        } else {
            func_808FFDB0(this, play);
        }
    }

    func_808FFAC8(this, play, 0);

    if ((this->unk_334 == 0) && (this->unk_311 == 0)) {
        this->unk_320 = 0.0f;
    } else {
        func_808FFBBC(this, play, 0);
    }
}

void func_80900818(BossGanon2* this, PlayState* play) {
    Animation_MorphToPlayOnce(&this->skelAnime, &gGanonDownedStartAnim, -5.0f);
    this->unk_194 = Animation_GetLastFrame(&gGanonDownedStartAnim);
    this->actionFunc = func_80900890;
    this->unk_1AC = 0;
    this->unk_39C = 0;
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_MGANON_DEAD1);
    this->unk_336 = 0;
}

void func_80900890(BossGanon2* this, PlayState* play) {
    Vec3f sp5C;
    Vec3f sp50;
    Camera* sp4C;
    Player* player;
    Camera* temp_v0;
    Camera* temp_v0_2;
    s32 pad;
    f32 temp_f12;
    f32 temp_f2;

    sp4C = Play_GetCamera(play, MAIN_CAM);
    player = GET_PLAYER(play);
    SkelAnime_Update(&this->skelAnime);
    this->unk_398++;
    this->unk_339 = 20;

    switch (this->unk_39C) {
        case 0:
            func_80064520(play, &play->csCtx);
            this->unk_39E = Play_CreateSubCamera(play);
            Play_ChangeCameraStatus(play, MAIN_CAM, CAM_STAT_WAIT);
            Play_ChangeCameraStatus(play, this->unk_39E, CAM_STAT_ACTIVE);
            func_8002DF54(play, &this->actor, 8);
            this->unk_39C = 1;
            this->unk_3A4 = sp4C->eye;
            this->unk_3B0 = sp4C->at;
            this->unk_1A2[0] = 300;
            this->unk_1A2[1] = 100;
            play->envCtx.unk_D8 = 0.0f;
        case 1:
            if (this->unk_1A2[1] == 50) {
                func_80078884(NA_SE_EN_MGANON_WALK);
            }
            Matrix_RotateY(((this->actor.shape.rot.y / (f32)0x8000) * M_PI) + 0.3f, MTXMODE_NEW);
            sp5C.x = 0.0f;
            sp5C.y = 0.0f;
            sp5C.z = 250.0f;
            Matrix_MultVec3f(&sp5C, &sp50);
            Math_ApproachF(&this->unk_3A4.x, this->actor.world.pos.x + sp50.x, 0.2f, 100.0f);
            Math_ApproachF(&this->unk_3A4.y, 1136.0f, 0.2f, 100.0f);
            Math_ApproachF(&this->unk_3A4.z, this->actor.world.pos.z + sp50.z, 0.2f, 100.0f);
            Math_ApproachF(&this->unk_3B0.x, this->unk_1B8.x, 0.2f, 100.0f);
            Math_ApproachF(&this->unk_3B0.y, this->unk_1B8.y, 0.2f, 100.0f);
            Math_ApproachF(&this->unk_3B0.z, this->unk_1B8.z, 0.2f, 100.0f);
            if (this->unk_1A2[1] == 0) {
                this->unk_39C = 2;
                this->unk_1A2[1] = 90;
            }
            break;
        case 2:
            this->unk_1A2[0] = 300;
            this->unk_3A4.x = sBossGanon2Zelda->actor.world.pos.x - 100.0f;
            this->unk_3A4.y = sBossGanon2Zelda->actor.world.pos.y + 30.0f;
            this->unk_3A4.z = (sBossGanon2Zelda->actor.world.pos.z + 30.0f) - 60.0f;
            this->unk_3B0.x = sBossGanon2Zelda->actor.world.pos.x;
            this->unk_3B0.y = sBossGanon2Zelda->actor.world.pos.y + 30.0f;
            this->unk_3B0.z = sBossGanon2Zelda->actor.world.pos.z - 10.0f;
            Math_ApproachZeroF(&this->unk_324, 1.0f, 5.0f);
            Math_ApproachF(&play->envCtx.unk_D8, 1.0f, 1.0f, 1.0f / 51);
            if (this->unk_1A2[1] == 80) {
                Message_StartTextbox(play, 0x70D7, NULL);
            }
            if ((this->unk_1A2[1] < 30) && (Message_GetState(&play->msgCtx) == TEXT_STATE_NONE)) {
                temp_v0 = Play_GetCamera(play, MAIN_CAM);
                temp_v0->eye = this->unk_3A4;
                temp_v0->eyeNext = this->unk_3A4;
                temp_v0->at = this->unk_3B0;
                func_800C08AC(play, this->unk_39E, 0);
                this->unk_39E = 0;
                func_80064534(play, &play->csCtx);
                func_8002DF54(play, &this->actor, 7);
                this->unk_39C = 3;
            }
            break;
        case 10:
            func_80064520(play, &play->csCtx);
            this->unk_39E = Play_CreateSubCamera(play);
            Play_ChangeCameraStatus(play, MAIN_CAM, CAM_STAT_WAIT);
            Play_ChangeCameraStatus(play, this->unk_39E, CAM_STAT_ACTIVE);
            this->unk_39C = 11;
            this->unk_334 = 1;
            func_8002DF54(play, &this->actor, 0x60);
            this->unk_398 = 0;
        case 11:
            player->actor.world.pos.x = sBossGanon2Zelda->actor.world.pos.x + 50.0f + 10.0f;
            player->actor.world.pos.z = sBossGanon2Zelda->actor.world.pos.z - 25.0f;
            player->actor.shape.rot.y = -0x8000;
            this->unk_3A4.x = (player->actor.world.pos.x + 100.0f) - 80.0f;
            this->unk_3A4.y = (player->actor.world.pos.y + 60.0f) - 40.0f;
            this->unk_3A4.z = player->actor.world.pos.z - 110.0f;
            this->unk_3B0.x = player->actor.world.pos.x;
            this->unk_3B0.y = (player->actor.world.pos.y + 60.0f) - 25.0f;
            this->unk_3B0.z = player->actor.world.pos.z;
            if (this->unk_398 == 80) {
                temp_v0_2 = Play_GetCamera(play, MAIN_CAM);
                temp_v0_2->eye = this->unk_3A4;
                temp_v0_2->eyeNext = this->unk_3A4;
                temp_v0_2->at = this->unk_3B0;
                this->unk_39C = 3;
                func_800C08AC(play, this->unk_39E, 0);
                this->unk_39E = 0;
                func_80064534(play, &play->csCtx);
                func_8002DF54(play, &this->actor, 7);
            }
            break;
    }

    if (this->unk_39E != 0) {
        Play_CameraSetAtEye(play, this->unk_39E, &this->unk_3B0, &this->unk_3A4);
    }

    switch (this->unk_1AC) {
        case 0:
            if (Animation_OnFrame(&this->skelAnime, this->unk_194)) {
                Animation_MorphToLoop(&this->skelAnime, &gGanonDownedLoopAnim, 0.0f);
                this->unk_1AC = 1;
            }
            break;
        case 1:
            if ((play->gameplayFrames % 32) == 0) {
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_MGANON_BREATH);
            }

            if ((this->unk_1A2[0] == 0) || (this->unk_334 != 0)) {
                temp_f2 = -200.0f - player->actor.world.pos.x;
                temp_f12 = -200.0f - player->actor.world.pos.z;

                if (sqrtf(SQ(temp_f2) + SQ(temp_f12)) <= 784.0f) {
                    Animation_MorphToPlayOnce(&this->skelAnime, &gGanonGetUpAnim, 0.0f);
                    this->unk_194 = Animation_GetLastFrame(&gGanonGetUpAnim);
                    this->unk_1AC = 2;
                    this->unk_1A2[0] = 40;
                    Audio_PlayActorSound2(&this->actor, NA_SE_EN_MGANON_ROAR);
                }
            }
            break;
        case 2:
            Math_ApproachF(&this->unk_324, 255.0f, 1.0f, 10.0f);
            Math_ApproachZeroF(&play->envCtx.unk_D8, 1.0f, 2.0f / 51.0f);
            if (Animation_OnFrame(&this->skelAnime, this->unk_194)) {
                func_808FFDB0(this, play);
                if (this->unk_334 == 0) {
                    this->actor.colChkInfo.health = 25;
                }
                this->unk_336 = 1;
            }
            break;
    }

    Math_ApproachZeroF(&this->actor.speedXZ, 0.5f, 1.0f);
}

void func_80901020(BossGanon2* this, PlayState* play) {
    Animation_MorphToPlayOnce(&this->skelAnime, &gGanonDownedStartAnim, -5.0f);
    this->unk_194 = Animation_GetLastFrame(&gGanonDownedStartAnim);
    this->actionFunc = func_8090120C;
    this->unk_1AC = 0;
    this->unk_39C = 0;
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_MGANON_DEAD1);
    this->unk_314 = 4;
}

void func_8090109C(BossGanon2* this, PlayState* play) {
    u8 i;

    static Color_RGBA8 sPrimColor = { 0, 120, 0, 255 };
    static Color_RGBA8 sEnvColor = { 0, 120, 0, 255 };

    if(CVar_GetS32("gRedGanonBlood", 0)) {
        sPrimColor.r = 120;
        sPrimColor.g = 0;

        sEnvColor.r = 120;
        sEnvColor.g = 0;
    }

    for (i = 0; i < 70; i++) {
        Vec3f velocity;
        Vec3f accel;
        Vec3f pos;

        velocity.x = Rand_CenteredFloat(50.0f);
        velocity.y = Rand_CenteredFloat(10.0f) + 5.0f;
        velocity.z = Rand_CenteredFloat(50.0f);
        accel.x = 0.0f;
        accel.y = -1.0f;
        accel.z = 0.0f;
        pos.x = this->unk_1B8.x;
        pos.y = this->unk_1B8.y;
        pos.z = this->unk_1B8.z;
        func_8002836C(play, &pos, &velocity, &accel, &sPrimColor, &sEnvColor, (s16)Rand_ZeroFloat(50.0f) + 50, 0,
                      17);
    }
}

void func_8090120C(BossGanon2* this, PlayState* play) {
    Player* player;
    f32 temp_f14;
    f32 temp_f12;
    Camera* temp_v0_2;
    s16 temp_a0_2;
    f32 phi_f0;
    s32 phi_a1;

    player = GET_PLAYER(play);
    this->unk_398++;
    SkelAnime_Update(&this->skelAnime);

    this->unk_3BC.x = 0.0f;
    this->unk_3BC.y = 1.0f;
    this->unk_3BC.z = 0.0f;

    switch (this->unk_39C) {
        case 0:
            func_80064520(play, &play->csCtx);
            this->unk_39E = Play_CreateSubCamera(play);
            Play_ChangeCameraStatus(play, MAIN_CAM, CAM_STAT_WAIT);
            Play_ChangeCameraStatus(play, this->unk_39E, CAM_STAT_ACTIVE);
            func_8002DF54(play, &this->actor, 8);
            this->unk_39C = 1;
            this->unk_398 = 0;
            sBossGanon2Zelda->unk_3C8 = 9;
            this->unk_31C = 0;
            this->unk_1A2[2] = 0;
            this->unk_336 = 0;
            this->unk_324 = 0.0f;
            this->actor.speedXZ = 0.0f;
            this->unk_31A = this->unk_31C;
            play->envCtx.unk_D8 = 0.0f;
        case 1:
            if (this->unk_398 < 90) {
                this->unk_339 = 20;
                Math_ApproachF(&play->envCtx.unk_D8, 1.0f, 1.0f, 0.1f);
            } else if (this->unk_398 >= 90) {
                this->unk_339 = 21;
                Math_ApproachZeroF(&play->envCtx.unk_D8, 1.0f, 0.08f);
            }
            if (this->unk_398 == 50) {
                func_80078884(NA_SE_EN_MGANON_WALK);
            }
            if (this->unk_398 > 90) {
                Math_ApproachF(&this->unk_380, 0.25f, 1.0f, 0.0125f);
                this->unk_37C = 200.0f;
                func_80078884(NA_SE_EV_TIMETRIP_LIGHT - SFX_FLAG);
            }
            if (this->unk_398 >= 110) {
                if (this->unk_398 == 110) {
                    Audio_PlayActorSound2(&this->actor, NA_SE_EN_GANON_HIT_THUNDER);
                    Audio_PlayActorSound2(&this->actor, NA_SE_EN_MGANON_DAMAGE);
                }
                Math_ApproachF(&this->unk_30C, 10.0f, 0.2f, 5.0f);
                this->skelAnime.playSpeed = 3.0f;
            }
            if (this->unk_398 == 120) {
                func_8002DF54(play, &this->actor, 0x63);
            }
            this->actor.world.rot.y = 0x4000;
            this->actor.world.pos.x = this->actor.world.pos.z = 0.0f;
            player->actor.shape.rot.y = -0x4000;
            player->actor.world.pos.x = 200.0f;
            player->actor.world.pos.z = 30.0f;
            sBossGanon2Zelda->actor.world.pos.x = 340.0f;
            sBossGanon2Zelda->actor.world.pos.z = -250.0f;
            sBossGanon2Zelda->actor.world.rot.y = sBossGanon2Zelda->actor.shape.rot.y = -0x2000;
            this->unk_3A4.x = 250;
            this->unk_3A4.y = 1150.0f;
            this->unk_3A4.z = 0.0f;
            this->unk_3B0.x = this->unk_1B8.x;
            this->unk_3B0.y = this->unk_1B8.y;
            this->unk_3B0.z = this->unk_1B8.z;
            if (this->unk_398 > 135) {
                this->unk_39C = 2;
                this->unk_398 = 0;
            }
            break;
        case 2:
            this->unk_339 = 22;
            Math_ApproachF(&play->envCtx.unk_D8, 1.0f, 1.0f, 0.1f);
            func_80078884(NA_SE_EV_TIMETRIP_LIGHT - SFX_FLAG);
            this->unk_3A4.x = 250;
            this->unk_3A4.y = 1150.0f;
            this->unk_3A4.z = 0.0f;
            Math_ApproachF(&this->unk_3B0.x, sBossGanon2Zelda->actor.world.pos.x, 0.2f, 20.0f);
            Math_ApproachF(&this->unk_3B0.y, sBossGanon2Zelda->actor.world.pos.y + 50.0f, 0.2f, 10.0f);
            Math_ApproachF(&this->unk_3B0.z, sBossGanon2Zelda->actor.world.pos.z, 0.2f, 20.0f);
            if (this->unk_398 == 50) {
                this->unk_39C = 3;
                this->unk_398 = 0;
            }
            break;
        case 3:
            this->unk_339 = 22;
            func_80078884(NA_SE_EV_TIMETRIP_LIGHT - SFX_FLAG);
            this->unk_3A4.x = 330.0f;
            this->unk_3A4.y = 1120.0f;
            this->unk_3A4.z = -150.0f;
            this->unk_3B0.x = sBossGanon2Zelda->actor.world.pos.x;
            this->unk_3B0.y = sBossGanon2Zelda->actor.world.pos.y + 40.0f;
            this->unk_3B0.z = sBossGanon2Zelda->actor.world.pos.z;
            if (this->unk_398 == 10) {
                Message_StartTextbox(play, 0x70D8, NULL);
            }
            if ((this->unk_398 > 80) && (Message_GetState(&play->msgCtx) == TEXT_STATE_NONE)) {
                this->unk_39C = 4;
                this->unk_398 = 0;
            }
            break;
        case 4:
            if (this->unk_398 > 10) {
                Math_ApproachZeroF(&this->unk_37C, 1.0f, 10.0f);
                if (this->unk_398 == 30) {
                    sBossGanon2Zelda->unk_3C8 = 10;
                }
                this->unk_339 = 23;
                Math_ApproachZeroF(&play->envCtx.unk_D8, 1.0f, 0.05f);
            } else {
                this->unk_339 = 22;
            }
            if (this->unk_398 == 100) {
                this->unk_39C = 5;
                this->unk_398 = 40;
                this->skelAnime.playSpeed = 1.0f;
                func_8002DF54(play, &this->actor, 0x64);
            }
            break;
        case 5:
            this->unk_339 = 23;
            if ((this->unk_398 >= 60) && (this->unk_398 <= 90)) {
                if (this->unk_398 == 62) {
                    func_80078884(NA_SE_EV_TRIFORCE_FLASH);
                }
                Math_ApproachF(&this->unk_38C, 200.0f, 1.0f, 8.0f);
            } else {
                Math_ApproachZeroF(&this->unk_38C, 1.0f, 8.0f);
            }
            if (this->unk_398 == 70) {
                func_8002DF54(play, &this->actor, 0x65);
            }
            if (this->unk_398 == 150) {
                func_8002DF54(play, &this->actor, 0x66);
            }
            this->unk_30C = 10.0f;
            player->actor.world.pos.x = 250.0f;
            player->actor.world.pos.z = 30.0f;
            this->unk_3A4.x = player->actor.world.pos.x - 50.0f;
            this->unk_3A4.y = player->actor.world.pos.y + 50.0f;
            this->unk_3A4.z = player->actor.world.pos.z + 40.0f;
            this->unk_3B0.x = player->actor.world.pos.x;
            this->unk_3B0.y = player->actor.world.pos.y + 40.0f;
            this->unk_3B0.z = player->actor.world.pos.z;
            if (this->unk_398 == 166) {
                temp_v0_2 = Play_GetCamera(play, MAIN_CAM);
                temp_v0_2->eye = this->unk_3A4;
                temp_v0_2->eyeNext = this->unk_3A4;
                temp_v0_2->at = this->unk_3B0;
                func_800C08AC(play, this->unk_39E, 0);
                this->unk_39E = 0;
                func_80064534(play, &play->csCtx);
                func_8002DF54(play, &this->actor, 7);
                this->unk_39C = 6;
            }
            break;
        case 6:
            this->unk_339 = 23;
            temp_f14 = this->unk_1B8.x - player->actor.world.pos.x;
            temp_f12 = this->unk_1B8.z - player->actor.world.pos.z;
            temp_a0_2 = Math_Atan2S(temp_f12, temp_f14) - player->actor.shape.rot.y;
            if ((ABS(temp_a0_2) < 0x2000) && (sqrtf(SQ(temp_f14) + SQ(temp_f12)) < 70.0f) &&
                (player->swordState != 0) && (player->heldItemActionParam == PLAYER_AP_SWORD_MASTER)) {
                func_80064520(play, &play->csCtx);
                this->unk_39E = Play_CreateSubCamera(play);
                Play_ChangeCameraStatus(play, MAIN_CAM, CAM_STAT_WAIT);
                Play_ChangeCameraStatus(play, this->unk_39E, CAM_STAT_ACTIVE);
                this->unk_39C = 7;
                this->unk_398 = 0;
                Animation_MorphToPlayOnce(&this->skelAnime, &gGanonFinalBlowAnim, 0.0f);
                this->unk_194 = Animation_GetLastFrame(&gGanonFinalBlowAnim);
                play->startPlayerCutscene(play, &this->actor, 0x61);
            } else {
                break;
            }
        case 7:
            this->unk_339 = 23;
            Math_ApproachZeroF(&play->envCtx.unk_D8, 1.0f, 0.2f);
            player->actor.world.pos.x = 250.0f;
            player->actor.shape.rot.y = -0x4000;
            player->actor.world.pos.z = 30.0f;
            if ((this->unk_398 == 20) || (this->unk_398 == 30) || (this->unk_398 == 65) || (this->unk_398 == 40)) {
                func_80078884(NA_SE_VO_LI_SWORD_N);
                func_80078884(NA_SE_IT_SWORD_SWING_HARD);
            }
            if ((this->unk_398 == 22) || (this->unk_398 == 35) || (this->unk_398 == 72) || (this->unk_398 == 45)) {
                func_80078884(NA_SE_EN_MGANON_DAMAGE);
                func_80078884(NA_SE_IT_SHIELD_BOUND);
                play->envCtx.unk_D8 = 1.0f;
            }
            if ((this->unk_398 == 22) || (this->unk_398 == 35) || (this->unk_398 == 72) || (this->unk_398 == 45)) {
                func_8090109C(this, play);
            }
            if ((this->unk_398 >= 34) && (this->unk_398 < 40)) {
                this->unk_3A4.x = 269.0f;
                this->unk_3A4.y = 1112.0f;
                this->unk_3A4.z = -28.0f;
                this->unk_3B0.x = 234.0f;
                this->unk_3B0.y = 1117.0f;
                this->unk_3B0.z = -11.0f;
            } else {
                if (this->unk_398 < 30) {
                    phi_a1 = 0;
                } else if (this->unk_398 < 43) {
                    phi_a1 = 1;
                } else {
                    this->unk_3BC.z = -0.8f;
                    player->actor.world.pos.x = 200.0f;
                    player->actor.world.pos.z = 10.0f;
                    phi_a1 = 2;
                }
                this->unk_3A4.x = D_8090702C[phi_a1].x + (player->actor.world.pos.x - 50.0f);
                this->unk_3A4.y = D_8090702C[phi_a1].y + (player->actor.world.pos.y + 50.0f);
                this->unk_3A4.z = D_8090702C[phi_a1].z + (player->actor.world.pos.z + 40.0f);
                this->unk_3B0.x = D_80907050[phi_a1].x + player->actor.world.pos.x;
                this->unk_3B0.y = D_80907050[phi_a1].y + (player->actor.world.pos.y + 40.0f);
                this->unk_3B0.z = D_80907050[phi_a1].z + player->actor.world.pos.z;
            }
            if (this->unk_398 > 80) {
                Audio_QueueSeqCmd(0x1 << 28 | SEQ_PLAYER_BGM_MAIN << 24 | 0x100FF);
                this->unk_39C = 75;
                this->unk_398 = 0;
                this->unk_3A4.x = 112.0f;
                this->unk_3A4.y = 1146.0f;
                this->unk_3A4.z = 202.0f;
                this->unk_3B0.x = 110.0f;
                this->unk_3B0.y = 1144.0f;
                this->unk_3B0.z = 177.0f;
                player->actor.world.pos.x = 200.0f;
                this->unk_3BC.z = 0.0f;
            }
            break;
        case 75:
            this->unk_339 = 23;
            if (this->unk_398 == 55) {
                Animation_MorphToPlayOnce(&this->skelAnime, &gGanonDeadStartAnim, 0.0f);
                this->unk_194 = Animation_GetLastFrame(&gGanonDeadStartAnim);
                func_8002DF54(play, &this->actor, 0x62);
                this->unk_39C = 8;
                this->unk_398 = 1000;
            }
            break;
        case 8:
            if (this->unk_398 == 1025) {
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_MGANON_STAND);
            }
            if (this->unk_398 >= 1000) {
                if (this->unk_398 < 1040) {
                    this->unk_339 = 23;
                    Math_ApproachZeroF(&play->envCtx.unk_D8, 1.0f, 0.2f);
                }
            }
            if (this->unk_398 == 1040) {
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_MGANON_DEAD2);
                this->unk_336 = 2;
                this->unk_339 = 0;
                play->envCtx.unk_BE = 0;
                play->envCtx.unk_D8 = 0.0f;
            }
            if (Animation_OnFrame(&this->skelAnime, this->unk_194)) {
                Animation_MorphToPlayOnce(&this->skelAnime, &gGanonDeadLoopAnim, 0.0f);
                this->unk_398 = 0;
                this->unk_194 = 1000.0f;
            }
            this->unk_3A4.x = 250;
            this->unk_3A4.y = 1150.0f;
            this->unk_3A4.z = 0.0f;
            this->unk_3B0.x = this->unk_1B8.x;
            this->unk_3B0.y = this->unk_1B8.y;
            this->unk_3B0.z = this->unk_1B8.z;
            if ((this->unk_398 < 1000) && ((this->unk_398 % 16) == 0)) {
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_MGANON_SWORD);
            }
            if (this->unk_398 == 40) {
                this->unk_39C = 9;
                this->unk_398 = 0;
                sBossGanon2Zelda->unk_3C8 = 11;
                Message_StartTextbox(play, 0x70D9, NULL);
                this->unk_336 = 0;
                play->envCtx.unk_D8 = 0.0f;
            }
            break;
        case 9:
            this->unk_339 = 24;
            this->unk_3A4.x = 330.0f;
            this->unk_3A4.y = 1120.0f;
            this->unk_3A4.z = -150.0f;
            this->unk_3B0.x = sBossGanon2Zelda->actor.world.pos.x;
            this->unk_3B0.y = sBossGanon2Zelda->actor.world.pos.y + 40.0f;
            this->unk_3B0.z = sBossGanon2Zelda->actor.world.pos.z;
            if (this->unk_398 > 60) {
                this->unk_39C = 10;
                this->unk_398 = 0;
                this->unk_410.x = 0.0f;
            }
            break;
        case 10:
            this->unk_339 = 24;
            Math_ApproachF(&this->unk_3A4.x, 290.0f, 0.05f, this->unk_410.x);
            Math_ApproachF(&this->unk_3A4.y, 1130.0f, 0.05f, this->unk_410.x * 0.25f);
            Math_ApproachF(&this->unk_3A4.z, -260.0f, 0.05f, this->unk_410.x * 1.25f);
            if ((this->unk_398 >= 40) && (this->unk_398 <= 110)) {
                Math_ApproachF(&play->envCtx.unk_D8, 1.0f, 1.0f, 0.02f);
                Math_ApproachF(&this->unk_384, 10.0f, 0.1f, 0.2f);
                Audio_PlayActorSound2(&sBossGanon2Zelda->actor, NA_SE_EV_GOD_LIGHTBALL_2 - SFX_FLAG);
            } else {
                Math_ApproachZeroF(&this->unk_384, 1.0f, 0.2f);
            }
            if (this->unk_398 > 130) {
                Math_ApproachF(&this->unk_3B0.y, (sBossGanon2Zelda->actor.world.pos.y + 40.0f + 10.0f) - 20.0f, 0.1f,
                               this->unk_410.x);
            } else {
                Math_ApproachF(&this->unk_3B0.y, sBossGanon2Zelda->actor.world.pos.y + 40.0f + 10.0f, 0.05f,
                               this->unk_410.x * 0.25f);
            }
            Math_ApproachF(&this->unk_410.x, 1.0f, 1.0f, 0.01f);
            if (this->unk_398 == 10) {
                sBossGanon2Zelda->unk_3C8 = 12;
            }
            if (this->unk_398 == 110) {
                sBossGanon2Zelda->unk_3C8 = 13;
            }
            if (this->unk_398 == 140) {
                Audio_PlayActorSound2(&sBossGanon2Zelda->actor, NA_SE_EV_HUMAN_BOUND);
            }
            if (this->unk_398 < 160) {
                break;
            }
        case 20:
            play->nextEntranceIndex = 0x6B;
            gSaveContext.nextCutsceneIndex = 0xFFF2;
            play->sceneLoadFlag = 0x14;
            play->fadeTransition = 3;
            play->linkAgeOnLoad = 1;
            break;
    }

    if (this->unk_39E != 0) {
        Play_CameraSetAtEyeUp(play, this->unk_39E, &this->unk_3B0, &this->unk_3A4, &this->unk_3BC);
    }

    switch (this->unk_1AC) {
        case 0:
            if (Animation_OnFrame(&this->skelAnime, this->unk_194)) {
                Animation_MorphToLoop(&this->skelAnime, &gGanonDownedLoopAnim, 0.0f);
                this->unk_1AC = 1;
            }
            break;
        case 1:
            if ((this->unk_39C < 7) && ((play->gameplayFrames % 32) == 0)) {
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_MGANON_BREATH);
            }
            break;
    }
}

void func_80902348(BossGanon2* this, PlayState* play) {
    Player* player;
    f32 temp_f2;
    f32 temp_f12;
    s16 i;
    s16 j;
    s16 phi_v0_2;

    if (this->unk_316 == 0) {
        for (i = 0; i < ARRAY_COUNT(this->unk_864); i++) {
            if (this->unk_444.elements[i].info.bumperFlags & 2) {
                this->unk_444.elements[i].info.bumperFlags &= ~2;
            } else if (this->unk_444.elements[i].info.toucherFlags & 2) {
                this->unk_444.elements[i].info.toucherFlags &= ~2;

                if (this->unk_312 == 1) {
                    phi_v0_2 = 0x1800;
                } else {
                    phi_v0_2 = 0;
                }

                func_8002F6D4(play, &this->actor, 15.0f, this->actor.yawTowardsPlayer + phi_v0_2, 2.0f, 0);
                sBossGanon2Zelda->unk_3C8 = 8;
                this->unk_316 = 10;
                break;
            }
        }
    }

    if (this->unk_324 > 0.0f) {
        player = GET_PLAYER(play);
        temp_f2 = -200.0f - player->actor.world.pos.x;
        temp_f12 = -200.0f - player->actor.world.pos.z;

        if (sqrtf(SQ(temp_f2) + SQ(temp_f12)) > 784.0f) {
            for (j = 0; j < ARRAY_COUNT(player->flameTimers); j++) {
                player->flameTimers[j] = Rand_S16Offset(0, 200);
            }

            player->isBurning = true;
            func_8002F6D4(play, &this->actor, 10.0f, Math_Atan2S(temp_f12, temp_f2), 0.0f, 0x10);
            sBossGanon2Zelda->unk_3C8 = 8;
        }
    }
}

void func_80902524(BossGanon2* this, PlayState* play) {
    s8 temp_v0_4;
    ColliderInfo* acHitInfo;
    s16 i;
    u8 phi_v1_2;

    osSyncPrintf("this->no_hit_time %d\n", this->unk_316);
    if (this->unk_316 != 0 || ((this->unk_334 == 0) && (this->actionFunc == func_80900890))) {
        for (i = 0; i < ARRAY_COUNT(this->unk_464); i++) {
            this->unk_424.elements[i].info.bumperFlags &= ~2;
        }
    }

    osSyncPrintf("this->look_on %d\n", this->unk_313);
    if (this->unk_313) {
        if (this->actionFunc != func_808FFFE0) {
            if (this->unk_424.elements[0].info.bumperFlags & 2) {
                this->unk_424.elements[0].info.bumperFlags &= ~2;
                acHitInfo = this->unk_424.elements[0].info.acHitInfo;
                if ((acHitInfo->toucher.dmgFlags & 0x2000) && (this->actionFunc != func_80900890)) {
                    func_809000A0(this, play);
                    Audio_PlayActorSound2(&this->actor, NA_SE_EN_FANTOM_HIT_THUNDER);
                    Audio_PlayActorSound2(&this->actor, NA_SE_EN_MGANON_DAMAGE);
                    Audio_StopSfxById(NA_SE_EN_MGANON_UNARI);
                } else if ((this->actionFunc == func_80900890) && (acHitInfo->toucher.dmgFlags & 0x9000200)) {
                    this->unk_316 = 60;
                    this->unk_342 = 5;
                    Audio_PlayActorSound2(&this->actor, NA_SE_EN_MGANON_DAMAGE);
                    Audio_StopSfxById(NA_SE_EN_MGANON_UNARI);
                    this->actor.colChkInfo.health -= 2;
                    temp_v0_4 = this->actor.colChkInfo.health;
                    if (temp_v0_4 < 0x15 && this->unk_334 == 0) {
                        func_80900818(this, play);
                    } else {
                        if (temp_v0_4 <= 0) {
                            func_80901020(this, play);
                        } else {
                            func_80900210(this, play);
                        }
                    }
                } else if (this->actionFunc != func_80900890) {
                    func_808FFF90(this, play);
                    Audio_PlayActorSound2(&this->actor, NA_SE_IT_HOOKSHOT_REFLECT);
                }
            }
        }
    } else {
        if (this->unk_424.elements[15].info.bumperFlags & 2) {
            this->unk_424.elements[15].info.bumperFlags &= ~2;
            acHitInfo = this->unk_424.elements[15].info.acHitInfo;
            this->unk_316 = 60;
            this->unk_344 = 0x32;
            this->unk_342 = 5;
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_MGANON_DAMAGE);
            Audio_StopSfxById(NA_SE_EN_MGANON_UNARI);
            phi_v1_2 = 1;
            if (acHitInfo->toucher.dmgFlags & 0x9000200) {
                if (acHitInfo->toucher.dmgFlags & 0x8000000) {
                    phi_v1_2 = 4;
                } else {
                    phi_v1_2 = 2;
                }
            }
            this->actor.colChkInfo.health -= phi_v1_2;
            temp_v0_4 = this->actor.colChkInfo.health;
            if ((temp_v0_4 < 0x15) && (this->unk_334 == 0)) {
                func_80900818(this, play);
            } else if ((temp_v0_4 <= 0) && (phi_v1_2 >= 2)) {
                func_80901020(this, play);
            } else {
                if (temp_v0_4 <= 0) {
                    this->actor.colChkInfo.health = 1;
                }
                func_80900210(this, play);
            }
        }
    }
}

void BossGanon2_Update(Actor* thisx, PlayState* play) {
    BossGanon2* this = (BossGanon2*)thisx;
    s32 pad;
    s16 i;
    f32 phi_f2;
    u16 i2;
    Vec3f sp58;
    Vec3f sp4C;
    f32 angle;
    f32 sp44;

    if ((this->unk_337 == 0) || (this->unk_337 == 2)) {
        BossGanon2_SetObjectSegment(this, play, OBJECT_GANON_ANIME3, false);
    } else {
        BossGanon2_SetObjectSegment(this, play, OBJECT_GANON2, false);
        Math_ApproachZeroF(&this->unk_30C, 1.0f, 0.5f);
    }
    func_808FFC84(this);
    this->unk_312 = 0;
    this->unk_19C++;
    Actor_SetScale(&this->actor, 0.01f);
    this->actionFunc(this, play);
    for (i = 0; i < ARRAY_COUNT(this->unk_1A2); i++) {
        if (this->unk_1A2[i] != 0) {
            this->unk_1A2[i]--;
        }
    }
    if (this->unk_316 != 0) {
        this->unk_316--;
    }
    if (this->unk_342 != 0) {
        this->unk_342--;
    }
    if (this->unk_390 != 0) {
        this->unk_390--;
    }
    if (this->unk_392 != 0) {
        this->unk_392--;
    }
    Actor_MoveForward(&this->actor);
    this->actor.shape.rot = this->actor.world.rot;
    if (this->unk_335 != 0) {
        Actor_UpdateBgCheckInfo(play, &this->actor, 60.0f, 60.0f, 100.0f, 5);
        if (this->actor.bgCheckFlags & 1) {
            if (this->actor.velocity.y < -5.0f) {
                func_80033E88(&this->actor, play, 5, 20);
                func_80078884(NA_SE_IT_BOMB_EXPLOSION);
            }
            this->actor.velocity.y = 0.0f;
        }
    }
    if (((this->unk_19C & 0x1F) == 0) && (Rand_ZeroOne() < 0.3f)) {
        this->unk_318 = 4;
    }
    this->unk_310 = D_80907074[this->unk_318];
    if (this->unk_318 != 0) {
        this->unk_318--;
    }
    this->unk_1B0 = (Math_SinS(this->unk_19C * 0x2AAA) * 64.0f) + 191.0f;
    if (this->unk_344 != 0) {
        this->unk_344--;
        Math_ApproachF(&this->unk_360.x, 5000.0f, 0.5f, 3000.0f);
        Math_ApproachF(&this->unk_370.x, 5500.0f, 0.5f, 3000.0f);
        Math_ApproachF(&this->unk_360.z, 8000.0f, 0.1f, 4000.0f);
        Math_ApproachF(&this->unk_370.z, 8000.0f, 0.1f, 4000.0f);
        Math_ApproachS(&this->unk_346, 0xFA0, 0xA, 0x7D0);
    } else {
        this->unk_360.y = 14000.0f;
        Math_ApproachF(&this->unk_360.x, 2000.0f, 0.1f, 100.0f);
        this->unk_370.y = 12000.0f;
        Math_ApproachF(&this->unk_370.x, 1500.0f, 0.1f, 100.0f);
        if ((this->actionFunc == func_808FFEBC) || (this->actionFunc == func_808FFFE0) ||
            (this->actionFunc == func_80900104)) {
            Math_ApproachF(&this->unk_360.z, 1000.0f, 0.1f, 100.0f);
            Math_ApproachF(&this->unk_370.z, 1000.0f, 0.1f, 100.0f);
            Math_ApproachS(&this->unk_346, -0xFA0, 0xA, 0x64);
        } else {
            Math_ApproachF(&this->unk_360.z, 5000.0f, 0.1f, 200.0f);
            Math_ApproachF(&this->unk_370.z, 5000.0f, 0.1f, 200.0f);
            Math_ApproachS(&this->unk_346, 0, 0xA, 0x64);
        }
    }
    if (this->unk_39C != 75) {
        this->unk_35C += this->unk_360.x;
        this->unk_36C += this->unk_370.x;
    }
    if (this->unk_337 == 2) {
        this->unk_370.z = 0.0f;
        this->unk_360.z = 0.0f;
    }

    for (i = 0; i < ARRAY_COUNT(this->unk_348); i++) {
        if (i == 0) {
            phi_f2 = 0.2f;
        } else if (i == 1) {
            phi_f2 = 0.5f;
        } else {
            phi_f2 = 1.0f;
        }

        this->unk_348[i] = Math_SinS(((s16)this->unk_35C + (i * (s16)this->unk_360.y))) * phi_f2 * this->unk_360.z;
        this->unk_352[i] = Math_SinS(((s16)this->unk_36C + (i * (s16)this->unk_370.y))) * phi_f2 * this->unk_370.z;
    }

    func_808FF898(this, play);
    func_80902348(this, play);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->unk_424.base);
    if (this->actionFunc != func_8090120C) {
        func_80902524(this, play);
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->unk_424.base);
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->unk_444.base);
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->unk_444.base);
        if (this->unk_39E == 0) {
            CollisionCheck_SetAT(play, &play->colChkCtx, &this->unk_444.base);
        }
    }
    if ((this->unk_332 == 0) && (this->unk_336 != 0)) {
        if (this->unk_336 == 2) {
            this->unk_332 = (s16)Rand_ZeroFloat(30.0f) + 8;
        } else {
            this->unk_332 = (s16)Rand_ZeroFloat(60.0f) + 0xA;
        }
        this->unk_339 = 0;
        play->envCtx.unk_BE = 0;
        play->envCtx.unk_BD = (s8)Rand_ZeroFloat(1.9f) + 1;
        play->envCtx.unk_D8 = 1.0f;
        D_8090EB20.y = 0.0f;
        D_8090EB20.x = D_8090EB20.y;
        D_8090EB20.z = D_8090EB20.x;
        if (Rand_ZeroOne() < 0.5f) {
            D_8090EB20.z = Rand_ZeroFloat(1000.0f);
        }
        func_80078914(&D_8090EB20, NA_SE_EV_LIGHTNING);
        this->unk_328 = 0xFF;
        this->unk_330 = 5;
        this->unk_32C = 0.0f;
        this->unk_340 = (s16)Rand_ZeroFloat(10000.0f);
    } else if (this->unk_332 != 0) {
        this->unk_332--;
    }
    if ((play->envCtx.unk_D8 > 0.0f) && (this->unk_336 != 0)) {
        play->envCtx.customSkyboxFilter = 1;
        play->envCtx.skyboxFilterColor[0] = 255;
        play->envCtx.skyboxFilterColor[1] = 255;
        play->envCtx.skyboxFilterColor[2] = 255;
        play->envCtx.skyboxFilterColor[3] = (s16)(play->envCtx.unk_D8 * 200.0f);
    } else {
        play->envCtx.customSkyboxFilter = 0;
    }
    play->envCtx.unk_BF = 0;
    play->envCtx.unk_DC = 2;

    switch (this->unk_339) {
        case 0:
            Math_ApproachZeroF(&play->envCtx.unk_D8, 1.0f, 0.1f);
            break;
        case 3:
            play->envCtx.unk_BE = 3;
            play->envCtx.unk_BD = 4;
            Math_ApproachF(&play->envCtx.unk_D8, 1.0f, 1.0f, 0.0125f);
            break;
        case 4:
            play->envCtx.unk_BE = 5;
            play->envCtx.unk_BD = 6;
            Math_ApproachF(&play->envCtx.unk_D8, 1.0f, 1.0f, 0.0125f);
            break;
        case 5:
            play->envCtx.unk_BE = 6;
            play->envCtx.unk_BD = 7;
            Math_ApproachF(&this->unk_33C, 0.69f, 1.0f, 0.05f);
            play->envCtx.unk_D8 =
                (Math_SinS(play->gameplayFrames * 0x5000) * 0.15f) + (0.15f + this->unk_33C);
            break;
        case 55:
            play->envCtx.unk_BE = 2;
            play->envCtx.unk_BD = 0;
            Math_ApproachZeroF(&play->envCtx.unk_D8, 1.0f, 0.05f);
            break;
        case 6:
            play->envCtx.unk_BE = 2;
            play->envCtx.unk_BD = 8;
            Math_ApproachF(&this->unk_33C, 0.69f, 1.0f, 0.05f);
            play->envCtx.unk_D8 =
                (Math_SinS(play->gameplayFrames * 0x7000) * 0.15f) + (0.15f + this->unk_33C);
            break;
        case 7:
            play->envCtx.unk_BE = 0;
            play->envCtx.unk_BD = 8;
            Math_ApproachZeroF(&play->envCtx.unk_D8, 1.0f, 0.02f);
            break;
        case 20:
            play->envCtx.unk_BE = 0;
            play->envCtx.unk_BD = 9;
            break;
        case 21:
            play->envCtx.unk_BE = 0xA;
            play->envCtx.unk_BD = 9;
            break;
        case 22:
            play->envCtx.unk_BE = 0xA;
            play->envCtx.unk_BD = 0xB;
            break;
        case 23:
            play->envCtx.unk_BE = 9;
            play->envCtx.unk_BD = 0xB;
            break;
        case 24:
            play->envCtx.unk_BE = 0;
            play->envCtx.unk_BD = 0xC;
            break;
        case -1:
            break;
    }

    if (this->unk_339 >= 0) {
        this->unk_339 = 0;
    }
    if (D_80906D78 != 0) {
        D_80906D78 = 0;

        for (i2 = 0; i2 < ARRAY_COUNT(sBossGanon2Particles); i2++) {
            angle = Rand_ZeroFloat(2 * M_PI);
            sp44 = Rand_ZeroFloat(40.0f) + 10.0f;
            sp58 = this->actor.world.pos;
            sp58.y = 1200.0f;
            sp4C.x = cosf(angle) * sp44;
            sp4C.z = sinf(angle) * sp44;
            sp4C.y = Rand_ZeroFloat(15.0f) + 15.0f;
            sp58.x += sp4C.x * 10.0f * 0.1f;
            sp58.z += sp4C.z * 10.0f * 0.1f;
            func_808FD27C(play, &sp58, &sp4C, Rand_ZeroFloat(0.3f) + 0.2f);
        }
    }
    this->unk_388 += 0.15f;
    func_80905DA8(this, play);
}

void func_809034E4(Vec3f* arg0, Vec3f* arg1) {
    Vtx* vtx;
    Vec3f sp2D0;
    s16 temp_s1;
    s16 temp_a1;
    s16 sp2CA;
    s16 sp2C8;
    s16 i;
    u8 phi_s2;
    u8 temp_s4;
    u8 temp_s4_2;
    f32 temp_f12;
    Vec3f temp_f20;
    Vec3f temp_f2;
    Vec3f temp_f22;
    f32 sp294;
    f32 phi_f30;
    f32 temp_f28;
    f32 temp_f26;
    s32 pad[3];
    Vec3f sp18C[20];
    Vec3f sp9C[20];

    for (i = 0; i < 20; i++) {
        sp18C[i] = *arg0;
        sp9C[i] = *arg1;
    }

    temp_s4 = 0;

    D_809105D8[3] = D_809105D8[2];
    D_809105D8[2] = D_809105D8[1];
    D_809105D8[1] = D_809105D8[0];
    D_809105D8[0] = *arg0;

    sp2D0 = D_809105D8[0];

    temp_f20.x = D_809105D8[1].x - sp2D0.x;
    temp_f20.y = D_809105D8[1].y - sp2D0.y;
    temp_f20.z = D_809105D8[1].z - sp2D0.z;

    sp2CA = Math_Atan2S(temp_f20.z, temp_f20.x);
    sp2C8 = Math_Atan2S(sqrtf(SQXZ(temp_f20)), temp_f20.y);

    temp_f2.x = D_809105D8[2].x - D_809105D8[1].x;
    temp_f2.y = D_809105D8[2].y - D_809105D8[1].y;
    temp_f2.z = D_809105D8[2].z - D_809105D8[1].z;

    temp_f22.x = D_809105D8[3].x - D_809105D8[2].x;
    temp_f22.y = D_809105D8[3].y - D_809105D8[2].y;
    temp_f22.z = D_809105D8[3].z - D_809105D8[2].z;

    temp_f12 = sqrtf(SQXYZ(temp_f20)) + sqrtf(SQXYZ(temp_f2)) + sqrtf(SQXYZ(temp_f22));
    if (temp_f12 <= 1.0f) {
        temp_f12 = 1.0f;
    }

    temp_f28 = temp_f12 * 0.083f;
    phi_f30 = sqrtf(SQXYZ(temp_f20)) / 2.0f;
    sp294 = sqrtf(SQXYZ(temp_f2)) / 2.0f;

    phi_s2 = 1;

    while (true) {
        temp_f20.x = D_809105D8[phi_s2].x - sp2D0.x;
        temp_f20.y = D_809105D8[phi_s2].y - sp2D0.y;
        temp_f20.z = D_809105D8[phi_s2].z - sp2D0.z;

        temp_s1 = Math_Atan2S(temp_f20.z, temp_f20.x);
        temp_a1 = Math_Atan2S(sqrtf(SQXZ(temp_f20)), temp_f20.y);

        Math_ApproachS(&sp2C8, temp_a1, 1, 0x1000);
        Math_ApproachS(&sp2CA, temp_s1, 1, 0x1000);

        temp_f26 = temp_f28 * Math_CosS(sp2C8);

        sp18C[temp_s4] = sp2D0;

        sp2D0.x += temp_f26 * Math_SinS(sp2CA);
        sp2D0.y += temp_f28 * Math_SinS(sp2C8);
        sp2D0.z += temp_f26 * Math_CosS(sp2CA);

        temp_f20.x = D_809105D8[phi_s2].x - sp2D0.x;
        temp_f20.y = D_809105D8[phi_s2].y - sp2D0.y;
        temp_f20.z = D_809105D8[phi_s2].z - sp2D0.z;

        if (phi_s2 < 3) {
            if (sqrtf(SQXYZ(temp_f20)) <= phi_f30) {
                phi_f30 = sp294;
                phi_s2++;
            }
        } else {
            if (sqrtf(SQXYZ(temp_f20)) <= (temp_f28 + 1.0f)) {
                phi_s2++;
            }
        }

        temp_s4++;

        if ((temp_s4 >= 20) || (phi_s2 >= 4)) {
            break;
        }
    }

    temp_s4_2 = 0;

    D_80910608[3] = D_80910608[2];
    D_80910608[2] = D_80910608[1];
    D_80910608[1] = D_80910608[0];
    D_80910608[0] = *arg1;

    sp2D0 = D_80910608[0];

    temp_f20.x = D_80910608[1].x - sp2D0.x;
    temp_f20.y = D_80910608[1].y - sp2D0.y;
    temp_f20.z = D_80910608[1].z - sp2D0.z;

    sp2CA = Math_Atan2S(temp_f20.z, temp_f20.x);
    sp2C8 = Math_Atan2S(sqrtf(SQXZ(temp_f20)), temp_f20.y);

    temp_f2.x = D_80910608[2].x - D_80910608[1].x;
    temp_f2.y = D_80910608[2].y - D_80910608[1].y;
    temp_f2.z = D_80910608[2].z - D_80910608[1].z;

    temp_f22.x = D_80910608[3].x - D_80910608[2].x;
    temp_f22.y = D_80910608[3].y - D_80910608[2].y;
    temp_f22.z = D_80910608[3].z - D_80910608[2].z;

    temp_f12 = sqrtf(SQXYZ(temp_f20)) + sqrtf(SQXYZ(temp_f2)) + sqrtf(SQXYZ(temp_f22));
    if (temp_f12 <= 1.0f) {
        temp_f12 = 1.0f;
    }

    temp_f28 = temp_f12 * 0.083f;
    phi_f30 = sqrtf(SQXYZ(temp_f20)) / 2.0f;
    sp294 = sqrtf(SQXYZ(temp_f2)) / 2.0f;

    phi_s2 = 1;

    while (true) {
        temp_f20.x = D_80910608[phi_s2].x - sp2D0.x;
        temp_f20.y = D_80910608[phi_s2].y - sp2D0.y;
        temp_f20.z = D_80910608[phi_s2].z - sp2D0.z;

        temp_s1 = Math_Atan2S(temp_f20.z, temp_f20.x);
        temp_a1 = Math_Atan2S(sqrtf(SQXZ(temp_f20)), temp_f20.y);

        Math_ApproachS(&sp2C8, temp_a1, 1, 0x1000);
        Math_ApproachS(&sp2CA, temp_s1, 1, 0x1000);

        temp_f26 = temp_f28 * Math_CosS(sp2C8);

        sp9C[temp_s4_2] = sp2D0;

        sp2D0.x += temp_f26 * Math_SinS(sp2CA);
        sp2D0.y += temp_f28 * Math_SinS(sp2C8);
        sp2D0.z += temp_f26 * Math_CosS(sp2CA);

        temp_f20.x = D_80910608[phi_s2].x - sp2D0.x;
        temp_f20.y = D_80910608[phi_s2].y - sp2D0.y;
        temp_f20.z = D_80910608[phi_s2].z - sp2D0.z;

        if (phi_s2 < 3) {
            if (sqrtf(SQXYZ(temp_f20)) <= phi_f30) {
                phi_f30 = sp294;
                phi_s2++;
            }
        } else {
            if (sqrtf(SQXYZ(temp_f20)) <= (temp_f28 + 1.0f)) {
                phi_s2++;
            }
        }

        temp_s4_2++;

        if ((temp_s4_2 >= 20) || (phi_s2 >= 4)) {
            break;
        }
    }

    vtx = ResourceMgr_LoadVtxByName(SEGMENTED_TO_VIRTUAL(ovl_Boss_Ganon2_Vtx_00BA20));
    for (i = 0; i < 11; i++) {
        if ((temp_s4 - i) > 0) {
            vtx[D_80907084[i]].n.ob[0] = sp18C[temp_s4 - i - 1].x;
            vtx[D_80907084[i]].n.ob[1] = sp18C[temp_s4 - i - 1].y;
            vtx[D_80907084[i]].n.ob[2] = sp18C[temp_s4 - i - 1].z;
        }
        if ((temp_s4_2 - i) > 0) {
            vtx[D_80907090[i]].n.ob[0] = sp9C[temp_s4_2 - i - 1].x;
            vtx[D_80907090[i]].n.ob[1] = sp9C[temp_s4_2 - i - 1].y;
            vtx[D_80907090[i]].n.ob[2] = sp9C[temp_s4_2 - i - 1].z;
        }
    }
}

void func_80903F38(BossGanon2* this, PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx);

    if (this->unk_312 != 0) {
        func_809034E4(&this->unk_200, &this->unk_20C);
        D_80907080 = 0xFF;
    }

    if (D_80910638 >= 4) {
        gSPSegment(
            POLY_XLU_DISP++, 0x08,
            Gfx_TwoTexScroll(play->state.gfxCtx, 0, 0, 0, 32, 32, 1, play->gameplayFrames * 18, 0, 32, 32));
        gDPPipeSync(POLY_XLU_DISP++);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, D_80907080);
        Matrix_Translate(0.0f, 0.0f, 0.0f, MTXMODE_NEW);
        gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, ovl_Boss_Ganon2_DL_00BB80);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void func_80904108(BossGanon2* this, PlayState* play) {
    s32 pad;

    if (this->unk_324 > 0.0f) {
        OPEN_DISPS(play->state.gfxCtx);

        Matrix_Push();
        gDPPipeSync(POLY_XLU_DISP++);
        gSPSegment(POLY_XLU_DISP++, 0x08,
                   Gfx_TwoTexScroll(play->state.gfxCtx, 0, (s32)play->gameplayFrames, 0, 32, 64, 1,
                                    -play->gameplayFrames * 2, -play->gameplayFrames * 8, 32, 32));
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 200, 0, (s8)this->unk_324);
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 0, 0, 128);
        Matrix_Translate(-200.0f, 1086.0f, -200.0f, MTXMODE_NEW);
        Matrix_Scale(0.098000005f, 0.1f, 0.098000005f, MTXMODE_APPLY);
        gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(ovl_Boss_Ganon2_DL_00E1C0));
        Matrix_Pop();

        CLOSE_DISPS(play->state.gfxCtx);
    }
}

void func_80904340(BossGanon2* this, PlayState* play) {
    s16 i;
    f32 rand;
    f32 angle;
    f32 sin;
    f32 cos;

    OPEN_DISPS(play->state.gfxCtx);
    Matrix_Push();

    if ((this->unk_330 != 0) || (this->unk_328 != 0)) {
        if (this->unk_330 != 0) {
            this->unk_330--;
        } else {
            this->unk_328 -= 70;

            if (this->unk_328 < 0) {
                this->unk_328 = 0;
            }
        }

        Math_ApproachF(&this->unk_32C, 0.13f, 1.0f, 0.065f);
        gDPPipeSync(POLY_XLU_DISP++);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, this->unk_328);
        BossGanon2_InitRand(this->unk_340 + 1, 0x71AC - this->unk_340, 0x263A);
        rand = BossGanon2_RandZeroOne();

        for (i = 0; i < 5; i++) {
            FrameInterpolation_RecordOpenChild("Ganon 80904340", i);
            angle = (i * (2 * M_PI / 5)) + (rand * M_PI);
            sin = 5000.0f * sinf(angle);
            cos = 5000.0f * cosf(angle);
            Matrix_Translate(-200.0f + sin, 4786.0f, -200.0f + cos, MTXMODE_NEW);
            Matrix_Scale(this->unk_32C, this->unk_32C, this->unk_32C, MTXMODE_APPLY);
            Matrix_RotateY(angle, MTXMODE_APPLY);
            Matrix_RotateZ((BossGanon2_RandZeroOne() - 0.5f) * 100.0f * 0.01f, MTXMODE_APPLY);

            if (BossGanon2_RandZeroOne() < 0.5f) {
                Matrix_RotateY(M_PI, MTXMODE_APPLY);
            }

            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(ovl_Boss_Ganon2_DL_00D798));
            FrameInterpolation_RecordCloseChild();
        }
    }

    Matrix_Pop();
    CLOSE_DISPS(play->state.gfxCtx);
}

void func_8090464C(BossGanon2* this, PlayState* play) {
    s32 pad;

    if (this->unk_1B4 > 0.0f) {
        OPEN_DISPS(play->state.gfxCtx);

        Matrix_Push();
        gDPPipeSync(POLY_XLU_DISP++);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 170, (s16)this->unk_1B4);
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 200, 0, 128);
        Matrix_Translate(this->unk_1B8.x, this->unk_1B8.y, this->unk_1B8.z, MTXMODE_NEW);
        Matrix_ReplaceRotation(&play->billboardMtxF);
        Matrix_RotateZ(-0.2f, MTXMODE_APPLY);
        Matrix_Scale(0.6f, 0.6f, 1.0f, MTXMODE_APPLY);
        gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(ovl_Boss_Ganon2_DL_00CCD8));
        Matrix_Pop();

        CLOSE_DISPS(play->state.gfxCtx);
    }
}

s32 BossGanon2_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                void* thisx) {
    s32 pad;
    BossGanon2* this = (BossGanon2*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    if (limbIndex == 15) {
        rot->y += this->unk_31A;
        rot->z += this->unk_31C;
    }

    if (limbIndex >= 42) {
        rot->x += this->unk_2F4[limbIndex] + this->unk_346;
        rot->y += this->unk_2FE[limbIndex];

        if (this->unk_342 & 1) {
            gDPSetEnvColor(POLY_OPA_DISP++, 255, 0, 0, 255);
        } else {
            gDPSetEnvColor(POLY_OPA_DISP++, (s16)this->unk_1B0, (s16)this->unk_1B0, (s16)(*this).unk_1B0, 255);
        }
    }

    if ((limbIndex == 7) || (limbIndex == 13) || (limbIndex == 33) || (limbIndex == 34)) {
        *dList = NULL;
    }

    CLOSE_DISPS(play->state.gfxCtx);
    return 0;
}

void BossGanon2_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    s8 pad;
    s8 temp_v0;
    BossGanon2* this = (BossGanon2*)thisx;
    Vec3f sp4C;

    OPEN_DISPS(play->state.gfxCtx);

    D_80907120.z = 17000.0f;
    D_8090712C.z = 3000.0f;

    if (D_809070CC[limbIndex] >= 0) {
        Matrix_MultVec3f(&D_80906D60, &this->unk_234[D_809070CC[limbIndex]]);
    }

    if (limbIndex == 15) {
        Matrix_MultVec3f(&D_80906D60, &this->unk_1B8);
    } else if (limbIndex == 3) {
        Matrix_MultVec3f(&D_80907108, &this->unk_1F4);
    } else if (limbIndex == 9) {
        Matrix_MultVec3f(&D_80907114, &this->unk_1E8);
    } else if (limbIndex == 38) {
        Matrix_MultVec3f(&D_80906D60, &this->unk_1DC);
    } else if (limbIndex == 41) {
        Matrix_MultVec3f(&D_80906D60, &this->unk_1D0);
    } else if (limbIndex == 45) {
        Matrix_MultVec3f(&D_80907138, &this->unk_1C4);
    }

    temp_v0 = D_8090709C[limbIndex];
    if (temp_v0 >= 0) {
        Matrix_MultVec3f(&D_80906D60, &sp4C);
        func_808FD080(temp_v0, &this->unk_424, &sp4C);
    }

    if ((limbIndex == 7) || (limbIndex == 13)) {
        Matrix_Push();
        Matrix_Scale(this->unk_224, this->unk_224, this->unk_224, MTXMODE_APPLY);
        gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_OPA_DISP++, *dList);
        Matrix_Pop();
    } else if ((limbIndex == 33) || (limbIndex == 34)) {
        Matrix_Push();
        Matrix_Scale(this->unk_228, this->unk_228, this->unk_228, MTXMODE_APPLY);
        gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_OPA_DISP++, *dList);
        Matrix_Pop();
    }

    if (*dList != NULL) {
        if ((limbIndex == 7) && (this->unk_312 == 1)) {
            Matrix_MultVec3f(&D_809070FC, &this->unk_218);
            func_808FD080(0, &this->unk_444, &this->unk_218);
            Matrix_MultVec3f(&D_80907120, &this->unk_200);
            Matrix_MultVec3f(&D_8090712C, &this->unk_20C);
        } else if ((limbIndex == 13) && (this->unk_312 == 2)) {
            Matrix_MultVec3f(&D_809070FC, &this->unk_218);
            func_808FD080(1, &this->unk_444, &this->unk_218);
            Matrix_MultVec3f(&D_80907120, &this->unk_200);
            Matrix_MultVec3f(&D_8090712C, &this->unk_20C);
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void func_80904D88(BossGanon2* this, PlayState* play) {
    s32 pad;
    s16 i;

    OPEN_DISPS(play->state.gfxCtx);

    if (this->unk_30C > 0.0f) {
        func_80093D84(play->state.gfxCtx);
        if (this->unk_380 > 0.0f) {
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 170, 255);
            gDPSetEnvColor(POLY_XLU_DISP++, 255, 200, 0, 0);
        } else {
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, 255);
            gDPSetEnvColor(POLY_XLU_DISP++, 100, 255, 255, 0);
        }
        gSPDisplayList(POLY_XLU_DISP++, ovl_Boss_Ganon2_DL_00B308);

        for (i = 0; i < 15; i++) {
            FrameInterpolation_RecordOpenChild("Ganon 80904D88", i);

            Matrix_Translate(this->unk_234[i].x, this->unk_234[i].y, this->unk_234[i].z, MTXMODE_NEW);
            Matrix_ReplaceRotation(&play->billboardMtxF);
            Matrix_Scale(this->unk_30C, this->unk_30C, this->unk_30C, MTXMODE_APPLY);
            Matrix_RotateZ(Rand_CenteredFloat(M_PI), MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, ovl_Boss_Ganon2_DL_00B378);
            
            FrameInterpolation_RecordCloseChild();
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void func_80904FC8(BossGanon2* this, PlayState* play) {
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx);

    if (this->unk_384 > 0.0f) {
        func_80093D84(play->state.gfxCtx);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, 200);
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 200, 0, 0);
        gSPDisplayList(POLY_XLU_DISP++, ovl_Boss_Ganon2_DL_00B308);
        Matrix_Translate(sBossGanon2Zelda->actor.world.pos.x, sBossGanon2Zelda->actor.world.pos.y + 80.0f, sBossGanon2Zelda->actor.world.pos.z,
                         MTXMODE_NEW);
        Matrix_ReplaceRotation(&play->billboardMtxF);
        Matrix_Scale(this->unk_384, this->unk_384, this->unk_384, MTXMODE_APPLY);
        Matrix_RotateZ(this->unk_388, MTXMODE_APPLY);
        gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(ovl_Boss_Ganon2_DL_00B378));
        Matrix_RotateZ(this->unk_388 * -2.0f, MTXMODE_APPLY);
        gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(ovl_Boss_Ganon2_DL_00B378));
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void func_8090523C(BossGanon2* this, PlayState* play) {
    Player* player;
    f32 phi_f20;

    OPEN_DISPS(play->state.gfxCtx);

    if (this->unk_38C > 0.0f) {
        s8 i;

        player = GET_PLAYER(play);
        func_80093D84(play->state.gfxCtx);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, (s16)this->unk_38C);
        gDPSetEnvColor(POLY_XLU_DISP++, 0, 255, 255, 0);
        gSPDisplayList(POLY_XLU_DISP++, ovl_Boss_Ganon2_DL_00B308);

        for (i = 0; i < 11; i++) {
            FrameInterpolation_RecordOpenChild("Ganon 8090523C", i);

            Matrix_Mult(&player->mf_9E0, MTXMODE_NEW);
            Matrix_Translate((i * 250.0f) + 900.0f, 350.0f, 0.0f, MTXMODE_APPLY);

            if (i < 7) {
                phi_f20 = 1.0f;
            } else {
                phi_f20 = 1.0f - ((i - 7) * 0.2333333f); // 7 / 30
            }

            Matrix_ReplaceRotation(&play->billboardMtxF);
            Matrix_Scale(200.0f * phi_f20, 200.0f * phi_f20, 1.0f, MTXMODE_APPLY);
            Matrix_RotateZ(Rand_ZeroFloat(2.0f * M_PI), MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(ovl_Boss_Ganon2_DL_00B378));
            
            FrameInterpolation_RecordCloseChild();
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void BossGanon2_PostLimbDraw2(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    s8 temp_v1 = D_80907144[limbIndex];
    BossGanon2* this = (BossGanon2*)thisx;

    if (temp_v1 >= 0) {
        Matrix_MultVec3f(&D_80906D60, &this->unk_234[temp_v1]);
    }
    if (limbIndex == 11) {
        OPEN_DISPS(play->state.gfxCtx);

        gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gGanondorfEyesDL));

        CLOSE_DISPS(play->state.gfxCtx);
    } else if (limbIndex == 10) {
        Matrix_MultVec3f(&D_80907164, &this->unk_1B8);
    }
}

void func_80905674(BossGanon2* this, PlayState* play) {
    s32 pad;

    if (this->unk_380 > 0.0f) {
        OPEN_DISPS(play->state.gfxCtx);

        Matrix_Push();
        gDPPipeSync(POLY_XLU_DISP++);
        gSPSegment(POLY_XLU_DISP++, 0x08,
                   Gfx_TwoTexScroll(play->state.gfxCtx, 0, this->unk_19C * -8, 0, 32, 64, 1, this->unk_19C * -4,
                                    this->unk_19C * -8, 32, 32));
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 170, (s16)this->unk_37C);
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 200, 0, 128);
        Matrix_Translate(sBossGanon2Zelda->actor.world.pos.x + 100.0f, sBossGanon2Zelda->actor.world.pos.y + 35.0f + 7.0f,
                         sBossGanon2Zelda->actor.world.pos.z - 100.0f, MTXMODE_NEW);
        Matrix_RotateY(-M_PI / 4.0f, MTXMODE_APPLY);
        Matrix_Scale(0.040000003f, 0.040000003f, this->unk_380, MTXMODE_APPLY);
        Matrix_RotateX(M_PI / 2.0f, MTXMODE_APPLY);
        gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(ovl_Boss_Ganon2_DL_00EC40));
        Matrix_Pop();

        CLOSE_DISPS(play->state.gfxCtx);
    }
}

void BossGanon2_Draw(Actor* thisx, PlayState* play) {
    void* shadowTexture = Graph_Alloc(play->state.gfxCtx, 4096);
    BossGanon2* this = (BossGanon2*)thisx;
    s16 i;

    OPEN_DISPS(play->state.gfxCtx);

    func_80093D18(play->state.gfxCtx);
    func_80093D84(play->state.gfxCtx);

    switch (this->unk_337) {
        case 0:
            BossGanon2_SetObjectSegment(this, play, OBJECT_GANON, true);
            gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(gGanondorfEmptyEyeTex));
            gSPSegment(POLY_XLU_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(gGanondorfEmptyEyeTex));
            SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                  this->skelAnime.dListCount, NULL, BossGanon2_PostLimbDraw2, this);
            break;
        case 1:
        case 2:
            BossGanon2_SetObjectSegment(this, play, OBJECT_GANON2, true);
            gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(sEyeTextures[this->unk_310]));
            func_808FD080(0, &this->unk_444, &D_8090717C);
            func_808FD080(1, &this->unk_444, &D_8090717C);
            this->unk_218 = D_8090717C;
            if (this->unk_342 & 1) {
                POLY_OPA_DISP = Gfx_SetFog(POLY_OPA_DISP, 0xFF, 0, 0, 0xFF, 0x384, 0x44B);
            }
            Matrix_Translate(0.0f, -4000.0f, 4000.0f, MTXMODE_APPLY);
            Matrix_RotateX(this->unk_394, MTXMODE_APPLY);
            Matrix_Translate(0.0f, 4000.0f, -4000.0f, MTXMODE_APPLY);
            gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                  this->skelAnime.dListCount, BossGanon2_OverrideLimbDraw, BossGanon2_PostLimbDraw,
                                  this);
            POLY_OPA_DISP = Play_SetFog(play, POLY_OPA_DISP);
            BossGanon2_GenShadowTexture(shadowTexture, this, play);
            BossGanon2_DrawShadowTexture(shadowTexture, this, play);
            break;
    }

    BossGanon2_SetObjectSegment(this, play, OBJECT_GANON2, true);
    func_80904340(this, play);
    func_80904108(this, play);
    func_80904D88(this, play);
    func_8090464C(this, play);
    func_80905674(this, play);
    func_80904FC8(this, play);
    func_8090523C(this, play);

    if ((this->unk_312 != 0) || (D_80907080 != 0)) {
        func_80903F38(this, play);
        if (this->unk_312 == 0) {
            s32 pad;

            D_80907080 -= 40;
            if (D_80907080 <= 0) {
                D_80907080 = 0;
            }
        }

        D_80910638++;
    } else {
        for (i = 0; i < 3; i++) {
            D_809105D8[i] = this->unk_200;
            D_80910608[i] = this->unk_20C;
        }

        D_80910638 = 0;
    }

    CLOSE_DISPS(play->state.gfxCtx);

    func_809060E8(play);
}

void func_80905DA8(BossGanon2* this, PlayState* play) {
    s32 pad[5];
    Player* player = GET_PLAYER(play);
    BossGanon2Effect* effect = play->specialEffects;
    Vec3f sp78;
    s16 i;

    for (i = 0; i < ARRAY_COUNT(sBossGanon2Particles); i++, effect++) {
        if (effect->type != 0) {
            effect->position.x += effect->velocity.x;
            effect->position.y += effect->velocity.y;
            effect->position.z += effect->velocity.z;
            effect->unk_01++;
            effect->velocity.x += effect->accel.x;
            effect->velocity.y += effect->accel.y;
            effect->velocity.z += effect->accel.z;
            if (effect->type == 1) {
                if (effect->unk_2E == 0) {
                    effect->unk_38.z += 1.0f;
                    effect->unk_38.y = (2.0f * M_PI) / 5.0f;
                } else {
                    effect->unk_38.z = M_PI / 2.0f;
                    effect->unk_38.y = 0.0f;
                    if (effect->position.y <= 1098.0f) {
                        effect->position.y = 1098.0f;
                        if (effect->velocity.y < -10.0f) {
                            sp78 = effect->position;
                            sp78.y = 1086.0f;
                            func_80078884(NA_SE_IT_SHIELD_REFLECT_SW);
                            CollisionCheck_SpawnShieldParticlesMetal(play, &sp78);
                        }
                        effect->velocity.y = 0.0f;
                    }
                    if ((SQ(player->actor.world.pos.x - effect->position.x) +
                         SQ(player->actor.world.pos.z - effect->position.z)) < SQ(25.0f)) {
                        effect->type = 0;
                        this->unk_39C = 10;
                    }
                }
            } else if (effect->type == 2) {
                effect->unk_38.x += 0.1f;
                effect->unk_38.y += 0.4f;
                if ((sqrtf(SQ(-200.0f - effect->position.x) + SQ(-200.0f - effect->position.z)) < 1000.0f)) {
                    if (effect->position.y < 1186.0f) {
                        if (effect->unk_2E == 0) {
                            effect->unk_2E++;
                            effect->position.y = 1186.0f;
                            effect->velocity.x *= 0.75f;
                            effect->velocity.z *= 0.75f;
                            effect->velocity.y *= -0.2f;
                        } else {
                            effect->type = 0;
                        }
                    }
                } else if ((effect->position.y < 0.0f)) {
                    effect->type = 0;
                }
            }
        }
    }
}

void func_809060E8(PlayState* play) {
    s16 alpha;
    u8 usingObjectGEff = false;
    BossGanon2Effect* effect;
    s16 i;
    BossGanon2Effect* effects;

    effects = effect = play->specialEffects;

    OPEN_DISPS(play->state.gfxCtx);

    func_80093D18(play->state.gfxCtx);

    for (i = 0; i < 1; i++) {
        if (effect->type == 1) {
            FrameInterpolation_RecordOpenChild("Ganon 809060E8 0", i);

            Vec3f spA0;
            f32 temp_f0;
            f32 angle;

            func_80093D84(play->state.gfxCtx);
            spA0.x = play->envCtx.dirLight1.params.dir.x;
            spA0.y = play->envCtx.dirLight1.params.dir.y;
            spA0.z = play->envCtx.dirLight1.params.dir.z;
            func_8002EABC(&effect->position, &play->view.eye, &spA0, play->state.gfxCtx);
            Matrix_Translate(effect->position.x, effect->position.y, effect->position.z, MTXMODE_NEW);
            Matrix_Scale(0.03f, 0.03f, 0.03f, MTXMODE_APPLY);
            Matrix_RotateY(effect->unk_38.z, MTXMODE_APPLY);
            Matrix_RotateX(effect->unk_38.y, MTXMODE_APPLY);
            gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPSegment(POLY_OPA_DISP++, 0x08,
                       Gfx_TexScroll(play->state.gfxCtx, 0, 0 - (play->gameplayFrames & 0x7F), 32, 32));
            gSPDisplayList(POLY_OPA_DISP++, ovl_Boss_Ganon2_DL_0103A8);
            if ((play->envCtx.unk_BD == 1) || (play->envCtx.unk_BD == 2)) {
                alpha = (s16)(play->envCtx.unk_D8 * 150.0f) + 50;
                angle = M_PI / 5.0f;
            } else {
                alpha = 100;
                angle = M_PI / 2.0f;
            }
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 0, 0, 0, alpha);
            temp_f0 = effect->position.y - 1098.0f;
            Matrix_Translate(effect->position.x + temp_f0, 1086.0f, (effect->position.z - 1.0f) + temp_f0, MTXMODE_NEW);
            Matrix_RotateY(angle, MTXMODE_APPLY);
            Matrix_Scale(1.0f, 0.0f, 1.0f, MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, ovl_Boss_Ganon2_DL_00F188);
            
            FrameInterpolation_RecordCloseChild();
        }
    }

    effect = effects;

    for (i = 0; i < ARRAY_COUNT(sBossGanon2Particles); i++, effect++) {
        if (effect->type == 2) {
            FrameInterpolation_RecordOpenChild("Ganon 809060E8 1", i);

            if (!usingObjectGEff) {
                BossGanon2_SetObjectSegment(NULL, play, OBJECT_GEFF, true);
                usingObjectGEff++;
            }
            Matrix_Translate(effect->position.x, effect->position.y, effect->position.z, MTXMODE_NEW);
            Matrix_Scale(effect->scale, effect->scale, effect->scale, MTXMODE_APPLY);
            Matrix_RotateY(effect->unk_38.z, MTXMODE_APPLY);
            Matrix_RotateX(effect->unk_38.y, MTXMODE_APPLY);
            Matrix_RotateZ(effect->unk_38.x, MTXMODE_APPLY);
            gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_OPA_DISP++, gGanonRubbleDL);
            
            FrameInterpolation_RecordCloseChild();
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void func_80906538(BossGanon2* this, u8* shadowTexture, f32 arg2) {
    s16 temp_t0;
    s16 temp_v0;
    s16 temp_a3;
    s16 phi_v1;
    s16 phi_a1;
    s16 i;
    f32 lerpx;
    s16 j;
    f32 lerpy;
    f32 lerpz;
    Vec3f sp70;
    Vec3f sp64;

    for (i = 0; i < 15; i++) {
        if ((arg2 == 0.0f) || ((j = D_809071CC[i]) >= 0)) {
            if (arg2 > 0.0f) {
                lerpx = this->unk_234[i].x + (this->unk_234[j].x - this->unk_234[i].x) * arg2;
                lerpy = this->unk_234[i].y + (this->unk_234[j].y - this->unk_234[i].y) * arg2;
                lerpz = this->unk_234[i].z + (this->unk_234[j].z - this->unk_234[i].z) * arg2;

                sp70.x = lerpx - this->actor.world.pos.x;
                sp70.y = lerpy - this->actor.world.pos.y + 76.0f + 30.0f + 30.0f + 100.0f;
                sp70.z = lerpz - this->actor.world.pos.z;
            } else {
                sp70.x = this->unk_234[i].x - this->actor.world.pos.x;
                sp70.y = this->unk_234[i].y - this->actor.world.pos.y + 76.0f + 30.0f + 30.0f + 100.0f;
                sp70.z = this->unk_234[i].z - this->actor.world.pos.z;
            }

            Matrix_MultVec3f(&sp70, &sp64);
            sp64.x *= 0.2f;
            sp64.y *= 0.2f;
            temp_a3 = sp64.x + 32.0f;
            temp_t0 = (s16)sp64.y * 64;

            if (D_809071EC[i] == 2) {
                for (j = 0, phi_a1 = -0x180; j < 12; j++, phi_a1 += 0x40) {
                    for (phi_v1 = -D_809071B4[j]; phi_v1 < D_809071B4[j]; phi_v1++) {
                        temp_v0 = temp_a3 + phi_v1 + temp_t0 + phi_a1;
                        if ((temp_v0 >= 0) && (temp_v0 < 0x1000)) {
                            shadowTexture[temp_v0] = 0xFF;
                        }
                    }
                }
            } else if (D_809071EC[i] == 1) {
                for (j = 0, phi_a1 = -0x100; j < 8; j++, phi_a1 += 0x40) {
                    for (phi_v1 = -D_809071A4[j]; phi_v1 < D_809071A4[j]; phi_v1++) {
                        temp_v0 = temp_a3 + phi_v1 + temp_t0 + phi_a1;
                        if ((temp_v0 >= 0) && (temp_v0 < 0x1000)) {
                            shadowTexture[temp_v0] = 0xFF;
                        }
                    }
                }
            } else if (D_809071EC[i] == 0) {
                for (j = 0, phi_a1 = -0xC0; j < 7; j++, phi_a1 += 0x40) {
                    for (phi_v1 = -D_80907194[j]; phi_v1 < D_80907194[j] - 1; phi_v1++) {
                        temp_v0 = temp_a3 + phi_v1 + temp_t0 + phi_a1;
                        if ((temp_v0 >= 0) && (temp_v0 < 0x1000)) {
                            shadowTexture[temp_v0] = 0xFF;
                        }
                    }
                }
            } else {
                for (j = 0, phi_a1 = -0x80; j < 6; j++, phi_a1 += 0x40) {
                    for (phi_v1 = -D_80907188[j]; phi_v1 < D_80907188[j] - 1; phi_v1++) {
                        temp_v0 = temp_a3 + phi_v1 + temp_t0 + phi_a1;
                        if ((temp_v0 >= 0) && (temp_v0 < 0x1000)) {
                            shadowTexture[temp_v0] = 0xFF;
                        }
                    }
                }
            }
        }
    }
}

void BossGanon2_GenShadowTexture(void* shadowTexture, BossGanon2* this, PlayState* play) {
    s16 i;
    u32* p = shadowTexture;

    for (i = 0; i < 1024; i++, p++) {
        *p = 0;
    }

    Matrix_RotateX(1.0f, MTXMODE_NEW);

    for (i = 0; i < 6; i++) {
        func_80906538(this, shadowTexture, i / 5.0f);
    }
}

void BossGanon2_DrawShadowTexture(void* shadowTexture, BossGanon2* this, PlayState* play) {
    s32 pad;
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    s16 alpha;

    OPEN_DISPS(gfxCtx);

    func_80093D18(play->state.gfxCtx);

    if ((play->envCtx.unk_BD == 1) || (play->envCtx.unk_BD == 2)) {
        alpha = (s16)(play->envCtx.unk_D8 * 180.0f) + 30;
    } else {
        alpha = 120;
    }

    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 0, 0, 0, alpha);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);
    Matrix_Translate(this->actor.world.pos.x, this->actor.floorHeight, this->actor.world.pos.z - 20.0f, MTXMODE_NEW);
    Matrix_Scale(1.65f, 1.0f, 1.65f, MTXMODE_APPLY);
    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, ovl_Boss_Ganon2_DL_00B3D0);
    gDPLoadTextureBlock(POLY_OPA_DISP++, shadowTexture, G_IM_FMT_I, G_IM_SIZ_8b, 64, 64, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, 6, 6, G_TX_NOLOD, G_TX_NOLOD);
    gSPDisplayList(POLY_OPA_DISP++, ovl_Boss_Ganon2_DL_00B3F0);

    CLOSE_DISPS(gfxCtx);
}

void BossGanon2_Reset(void) {
    D_8090EB20.x = 0;
    D_8090EB20.y = 0;
    D_8090EB20.z = 0;
    D_80910638 = 0;
    sBossGanon2Zelda = NULL;
    D_8090EB30 = NULL;
    sBossGanon2Seed1 = 0;
    sBossGanon2Seed2 = 0;
    sBossGanon2Seed3 = 0;
    memset(D_809105D8, 0, sizeof(D_809105D8));
    memset(D_80910608, 0, sizeof(D_80910608));
    memset(sBossGanon2Particles, 0, sizeof(sBossGanon2Particles));
}