/*
 * File: z_obj_um.c
 * Overlay: ovl_Obj_Um
 * Description: Cremia's Cart and milk run minigame
 */

#include "z_obj_um.h"
#include "z64horse.h"
#include "overlays/actors/ovl_En_In/z_en_in.h"
#include "overlays/effects/ovl_Effect_Ss_Hitmark/z_eff_ss_hitmark.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define FLAGS                                                                                  \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_DRAW_CULLING_DISABLED)

/**
 * weekEventReg flags checked by this actor:
 * - WEEKEVENTREG_DEFENDED_AGAINST_ALIENS: Aliens defeated
 *     If false: The actor doesn't spawn
 * - WEEKEVENTREG_31_40
 *     If true: Cremia doesn't explain again she'll deliever milk to town
 * - WEEKEVENTREG_31_80
 *     If true: Triggers cutscene on Romani's Ranch
 * - WEEKEVENTREG_34_80
 *     If true: Doesn't spawn on Romani's Ranch
 * - WEEKEVENTREG_ESCORTED_CREMIA
 *     If true: Doesn't spawn on Romani's Ranch or Milk Road
 * - WEEKEVENTREG_52_02
 *     If true: Doesn't spawn on Romani's Ranch or Milk Road
 * - WEEKEVENTREG_59_02
 *     If true: Doesn't spawn again on Milk Road
 *
 * weekEventReg flags set by this actor:
 * - WEEKEVENTREG_31_40: Cremia offered a ride
 *     Cremia offered a ride accross the Milk Road to Player
 * - WEEKEVENTREG_31_80: Player is in Milk Run
 *     Player accepts the ride and is with Cremia during the Milk Run
 * - WEEKEVENTREG_34_80: Cremia does Milk Run alone
 *     Player didn't interact or didn't accept the ride
 * - WEEKEVENTREG_ESCORTED_CREMIA: Won Milk Run minigame
 *     At least one pot is safe. Turns off the "Lose Milk Run minigame"
 * - WEEKEVENTREG_52_02: Lose Milk Run minigame
 *     Every pot was broken by bandits. Turns off the "Win Milk Run minigame"
 * - WEEKEVENTREG_59_02: ?
 *     Passed through Milk Road after winning the Milk Run
 *
 * weekEventReg flags unset by this actor:
 * - WEEKEVENTREG_31_80
 *     Turned off when the Milk Run finishes
 * - WEEKEVENTREG_ESCORTED_CREMIA
 *     Turned off if Player lose the Milk Run
 * - WEEKEVENTREG_52_02
 *     Turned off if Player wins the Milk Run
 */

void ObjUm_Init(Actor* thisx, PlayState* play);
void ObjUm_Destroy(Actor* thisx, PlayState* play);
void ObjUm_Update(Actor* thisx, PlayState* play);
void ObjUm_Draw(Actor* thisx, PlayState* play);

void ObjUm_DefaultAnim(ObjUm* this, PlayState* play);
void ObjUm_ChangeAnim(ObjUm* this, PlayState* play, ObjUmAnimation animIndex);
void ObjUm_SetupAction(ObjUm* this, ObjUmActionFunc actionFunc);

ActorProfile Obj_Um_Profile = {
    /**/ ACTOR_OBJ_UM,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_UM,
    /**/ sizeof(ObjUm),
    /**/ ObjUm_Init,
    /**/ ObjUm_Destroy,
    /**/ ObjUm_Update,
    /**/ ObjUm_Draw,
};

static TexturePtr sEyeTextures[] = {
    gUmCremiaEyeOpenTex,  gUmCremiaEyeHalfTex,  gUmCremiaEyeClosedTex,
    gUmCremiaEyeHappyTex, gUmCremiaEyeAngryTex, gUmCremiaEyeSadTex,
};

static TexturePtr sMouthTextures[] = {
    gUmMouthNormalTex,
    gUmMouthHappyTex,
    gUmMouthHangingOpenTex,
    gUmMouthFrownTex,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_HIT3,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK1,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000020, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_NONE,
    },
    { 40, 64, 0, { 0, 0, 0 } },
};

// actionfuncs
void func_80B7A144(ObjUm* this, PlayState* play);
void ObjUm_RanchWait(ObjUm* this, PlayState* play);
void ObjUm_PreMilkRunStartCs(ObjUm* this, PlayState* play);
void ObjUm_StartCs(ObjUm* this, PlayState* play);
void ObjUm_PostMilkRunStartCs(ObjUm* this, PlayState* play);
void ObjUm_TerminaFieldIdle(ObjUm* this, PlayState* play);
void ObjUm_RanchWaitPathFinished(ObjUm* this, PlayState* play);
void func_80B7A0E0(ObjUm* this, PlayState* play);
void func_80B7A070(ObjUm* this, PlayState* play);
void ObjUm_RanchStartCs(ObjUm* this, PlayState* play);
void func_80B7A2AC(ObjUm* this, PlayState* play);
void func_80B7A240(ObjUm* this, PlayState* play);
void func_80B7A394(ObjUm* this, PlayState* play);
void ObjUm_RunMinigame(ObjUm* this, PlayState* play);
void func_80B7A614(ObjUm* this, PlayState* play);
void func_80B7AB78(ObjUm* this, PlayState* play);
void func_80B7ABE4(ObjUm* this, PlayState* play);
void ObjUm_PostMilkRunWaitPathFinished(ObjUm* this, PlayState* play);

// Maybe it is updating the bandits positions?
void func_80B77770(ObjUm* this, PlayState* play) {
    s16 rotY = this->dyna.actor.shape.rot.y;
    Vec3f sp108;
    Vec3f spFC;
    Vec3f spF0;
    Vec3f spE4;
    Vec3f spD8;
    Vec3f spCC;
    Vec3f spC0;
    Vec3f spB4;
    Vec3f spA8;
    Vec3f sp9C;
    Vec3f sp90;
    Vec3f sp84;
    Vec3f sp78;
    Vec3f sp6C;
    Vec3f sp60 = this->dyna.actor.world.pos;

    // Loop unroll?
    this->unk_360[15] = sp60;
    sp6C = sp60;
    this->unk_360[14] = sp6C;
    sp78 = sp6C;
    this->unk_360[13] = sp78;
    sp84 = sp78;
    this->unk_360[12] = sp84;
    sp90 = sp84;
    this->unk_360[11] = sp90;
    sp9C = sp90;
    this->unk_360[10] = sp9C;
    spA8 = sp9C;
    this->unk_360[9] = spA8;
    spB4 = spA8;
    this->unk_360[8] = spB4;
    spC0 = spB4;
    this->unk_360[7] = spC0;
    spCC = spC0;
    this->unk_360[6] = spCC;
    spD8 = spCC;
    this->unk_360[5] = spD8;
    spE4 = spD8;
    this->unk_360[4] = spE4;
    spF0 = spE4;
    this->unk_360[3] = spF0;
    spFC = spF0;
    this->unk_360[2] = spFC;
    sp108 = spFC;
    this->unk_360[1] = sp108;
    this->unk_360[0] = sp108;

    this->unk_360[0].x += 110.0f * Math_SinS(rotY + 0x4000);
    this->unk_360[0].z += 110.0f * Math_CosS(rotY + 0x4000);
    this->unk_360[1].x += 110.0f * Math_SinS(rotY - 0x4000);
    this->unk_360[1].z += 110.0f * Math_CosS(rotY - 0x4000);
    Math_Vec3f_Copy(&this->unk_360[2], &this->unk_360[0]);
    this->unk_360[2].x -= 200.0f * Math_SinS(rotY);
    this->unk_360[2].z -= 200.0f * Math_CosS(rotY);
    Math_Vec3f_Copy(&this->unk_360[4], &this->unk_360[1]);
    this->unk_360[4].x -= 200.0f * Math_SinS(rotY);
    this->unk_360[4].z -= 200.0f * Math_CosS(rotY);
    this->unk_360[3].x -= 220.0f * Math_SinS(rotY);
    this->unk_360[3].z -= 220.0f * Math_CosS(rotY);
    Math_Vec3f_Copy(&this->unk_360[5], &this->unk_360[0]);
    this->unk_360[5].x -= 500.0f * Math_SinS(rotY);
    this->unk_360[5].z -= 500.0f * Math_CosS(rotY);
    Math_Vec3f_Copy(&this->unk_360[7], &this->unk_360[1]);
    this->unk_360[7].x -= 500.0f * Math_SinS(rotY);
    this->unk_360[7].z -= 500.0f * Math_CosS(rotY);
    this->unk_360[6].x -= 500.0f * Math_SinS(rotY);
    this->unk_360[6].z -= 500.0f * Math_CosS(rotY);
    Math_Vec3f_Copy(&this->unk_360[8], &this->unk_360[0]);
    this->unk_360[8].x -= 700.0f * Math_SinS(rotY);
    this->unk_360[8].z -= 700.0f * Math_CosS(rotY);
    Math_Vec3f_Copy(&this->unk_360[10], &this->unk_360[1]);
    this->unk_360[10].x -= 700.0f * Math_SinS(rotY);
    this->unk_360[10].z -= 700.0f * Math_CosS(rotY);
    this->unk_360[9].x -= 700.0f * Math_SinS(rotY);
    this->unk_360[9].z -= 700.0f * Math_CosS(rotY);
    this->unk_360[11] = this->unk_360[3];
    this->unk_360[11].x += 40.0f * Math_SinS(rotY + 0x4000);
    this->unk_360[11].z += 40.0f * Math_CosS(rotY + 0x4000);
    this->unk_360[12] = this->unk_360[3];
    this->unk_360[12].x += 40.0f * Math_SinS(rotY - 0x4000);
    this->unk_360[12].z += 40.0f * Math_CosS(rotY - 0x4000);
    this->unk_360[13] = this->unk_360[2];
    this->unk_360[14] = this->unk_360[4];
}

s32 ObjUm_InitBandits(ObjUm* this, PlayState* play) {
    Path* path = &play->setupPathList[this->pathIndex];
    s16 pad;
    Vec3s* spawnPoints;
    EnHorse* bandit1;
    EnHorse* bandit2;

    spawnPoints = Lib_SegmentedToVirtual(path->points);
    SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, NA_BGM_CHASE | SEQ_FLAG_ASYNC);

    bandit1 = (EnHorse*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_HORSE, spawnPoints[0].x, spawnPoints[0].y,
                                    spawnPoints[0].z, 0, this->dyna.actor.shape.rot.y, 0,
                                    ENHORSE_PARAMS(ENHORSE_PARAM_BANDIT, ENHORSE_19));
    this->bandit1 = bandit1;

    bandit1->unk_540 = bandit1->actor.world.pos;

    bandit1->unk_54C = 0xF;
    bandit1->unk_550 = 10;

    bandit1->unk_554 = this->pathIndex;
    bandit1->unk_568 = 0.0f;
    bandit1->unk_56C = 0.0f;
    bandit1->unk_558 = 0;
    bandit1->unk_55C = 40;
    bandit1->unk_560 = 40;

    bandit1->banditPosition = gZeroVec3f;
    bandit1->unk_57C = gZeroVec3f;

    bandit1->unk_588 = this->dyna.actor.shape.rot.y;
    bandit1->curRaceWaypoint = 1;

    bandit2 = (EnHorse*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_HORSE, spawnPoints[1].x, spawnPoints[1].y,
                                    spawnPoints[1].z, 0, this->dyna.actor.shape.rot.y, 0,
                                    ENHORSE_PARAMS(ENHORSE_PARAM_BANDIT, ENHORSE_20));
    this->bandit2 = bandit2;

    bandit2->unk_540 = bandit2->actor.world.pos;

    bandit2->unk_54C = 0xF;
    bandit2->unk_550 = 8;

    bandit2->unk_554 = this->pathIndex;
    bandit2->unk_568 = 0.0f;
    bandit2->unk_56C = 0.0f;
    bandit2->unk_55C = 40;
    bandit2->unk_560 = 40;

    bandit2->unk_57C = gZeroVec3f;

    bandit2->unk_588 = this->dyna.actor.shape.rot.y;
    bandit2->curRaceWaypoint = 2;

    this->potsLife[0] = 4;
    this->potsLife[1] = 4;
    this->potsLife[2] = 4;

    return 0;
}

typedef struct {
    /* 0x00 */ s32 unk_00;
    /* 0x04 */ s32 unk_04;
    /* 0x08 */ s32 unk_08;
    /* 0x0C */ f32 unk_0C;
    /* 0x10 */ s32 unk_10;
} struct_80B7C254; // size = 0x14

struct_80B7C254 D_80B7C164[] = {
    { 2, 0, 0, 1.0f, 40 },   { 4, 1, 0, 1.0f, 40 },  { 3, 2, 0, 1.0f, 40 },   { 3, 4, 0, 1.0f, 40 },
    { 5, 3, 1, -1.0f, 30 },  { 7, 3, 1, 1.0f, 30 },  { 0, 13, 1, -1.0f, 60 }, { 1, 14, 1, 1.0f, 60 },
    { 13, 10, 0, 1.0f, 40 }, { 14, 8, 0, 1.0f, 40 }, { 8, 5, 0, 1.0f, 30 },   { 10, 7, 0, 1.0f, 30 },
};

// BanditAttack?
s32 func_80B781DC(ObjUm* this, EnHorse* bandit1, EnHorse* bandit2, PlayState* play) {
    s32 temp_v0;
    s32 phi_s3 = -1;
    s32 phi_s4 = 0;
    s32 phi_s2 = 0;
    f32 phi_f20 = 0.0f;
    s32 i;

    for (i = 0; i < ARRAY_COUNT(D_80B7C164); i++) {
        if (bandit1->unk_550 == D_80B7C164[i].unk_00) {
            if (bandit2->unk_550 != D_80B7C164[i].unk_04) {
                if (D_80B7C164[i].unk_00 != 3) {
                    if ((D_80B7C164[i].unk_04 != 3) || (Player_GetMask(play) != PLAYER_MASK_CIRCUS_LEADER)) {
                        phi_s3 = D_80B7C164[i].unk_04;
                        phi_s4 = D_80B7C164[i].unk_08;
                        phi_f20 = D_80B7C164[i].unk_0C;
                        phi_s2 = D_80B7C164[i].unk_10;
                    }
                } else if (((bandit1->unk_54C != 5) || (D_80B7C164[i].unk_04 != 2)) &&
                           ((bandit1->unk_54C != 7) || (D_80B7C164[i].unk_04 != 4))) {
                    phi_s3 = D_80B7C164[i].unk_04;
                    phi_s4 = D_80B7C164[i].unk_08;
                    phi_f20 = D_80B7C164[i].unk_0C;
                    if ((2.0f * Rand_ZeroOne()) < 1.0f) {
                        phi_f20 *= -1.0f;
                    }
                    phi_s2 = D_80B7C164[i].unk_10;

                    //! FAKE:
                    if (1) {}
                }
            }
        }
    }

    if (phi_s3 == -1) {
        return 0;
    }

    bandit1->unk_540 = bandit1->actor.world.pos;
    bandit1->unk_54C = bandit1->unk_550;
    bandit1->unk_550 = phi_s3;
    bandit1->unk_55C = phi_s2;
    bandit1->unk_560 = phi_s2;
    bandit1->unk_564 = phi_s4;
    bandit1->unk_568 = phi_f20;

    if (phi_s3 == 3) {
        f32 rand = Rand_ZeroOne();

        bandit1->unk_558 = (s32)(rand * 3.0f);
    } else {
        bandit1->unk_558 = 0;
    }

    return 0;
}

// ObjUm_Bandit_UpdatePosition?
s32 func_80B783E0(ObjUm* this, PlayState* play, s32 banditIndex, EnHorse* bandit) {
    Path* path = &play->setupPathList[this->pathIndex];
    s32 sp68;
    Vec3s* sp64;
    f32 phi_f12;
    s32 phi_v1_2;
    Vec3f sp50;
    f32 sp4C;
    f32 sp48;
    f32 sp44;
    s32 temp_a1;
    f32 sp3C;
    f32 phi_f14;

    sp68 = path->count;
    sp64 = Lib_SegmentedToVirtual(path->points);

    if (sp68 == 0) {
        return 0;
    }

    if (Math3D_Vec3f_DistXYZ(&bandit->actor.world.pos, &this->dyna.actor.world.pos) < 800.0f) {
        if (banditIndex == 0) {
            this->flags |= OBJ_UM_FLAG_0200;
        } else {
            this->flags |= OBJ_UM_FLAG_0400;
        }
        bandit->unk_540 = bandit->actor.world.pos;
        bandit->unk_55C = 50;
        bandit->unk_560 = 50;
        bandit->unk_564 = 1;
    }

    Math_Vec3s_ToVec3f(&sp50, &sp64[bandit->curRaceWaypoint]);

    if (bandit->curRaceWaypoint == 0) {
        phi_f12 = sp64[1].x - sp64[0].x;
        phi_f14 = sp64[1].z - sp64[0].z;
    } else if ((bandit->curRaceWaypoint + 1) == path->count) {
        phi_f12 = sp64[path->count - 1].x - sp64[path->count - 2].x;
        phi_f14 = sp64[path->count - 1].z - sp64[path->count - 2].z;
    } else {
        phi_f12 = sp64[bandit->curRaceWaypoint + 1].x - sp64[bandit->curRaceWaypoint - 1].x;
        phi_f14 = sp64[bandit->curRaceWaypoint + 1].z - sp64[bandit->curRaceWaypoint - 1].z;
    }

    temp_a1 = Math_Atan2S(phi_f12, phi_f14);

    Math3D_RotateXZPlane(&sp50, temp_a1, &sp4C, &sp48, &sp44);
    if (((bandit->actor.world.pos.x * sp4C) + (sp48 * bandit->actor.world.pos.z) + sp44) > 0.0f) {
        bandit->curRaceWaypoint++;
        if (bandit->curRaceWaypoint >= sp68) {
            bandit->curRaceWaypoint = 0;
        }
        Math_Vec3s_ToVec3f(&sp50, &sp64[bandit->curRaceWaypoint]);
    }

    bandit->actor.world.rot.y = Math_Vec3f_Yaw(&bandit->actor.world.pos, &sp50);
    bandit->actor.speed = 45.0f;

    sp3C = Math_CosS(bandit->actor.world.rot.x) * bandit->actor.speed;
    bandit->actor.velocity.x = Math_SinS(bandit->actor.world.rot.y) * sp3C;
    bandit->actor.velocity.y = Math_SinS(bandit->actor.world.rot.x) * bandit->actor.speed;
    bandit->actor.velocity.z = Math_CosS(bandit->actor.world.rot.y) * sp3C;

    bandit->banditPosition.x =
        bandit->actor.world.pos.x + (bandit->actor.velocity.x * 0.5f) + bandit->actor.colChkInfo.displacement.x;
    bandit->banditPosition.y =
        bandit->actor.world.pos.y + (bandit->actor.velocity.y * 0.5f) + bandit->actor.colChkInfo.displacement.y;
    bandit->banditPosition.z =
        bandit->actor.world.pos.z + (bandit->actor.velocity.z * 0.5f) + bandit->actor.colChkInfo.displacement.z;

    phi_v1_2 = BINANG_SUB(bandit->actor.world.rot.y, bandit->actor.shape.rot.y);

    if (phi_v1_2 > 0x190) {
        phi_v1_2 = 0x190;
    } else if (phi_v1_2 < -0x190) {
        phi_v1_2 = -0x190;
    }

    bandit->actor.shape.rot.y += phi_v1_2;
    return 0;
}

// ObjUm_Bandit_GetCloserAndAttack
s32 func_80B78764(ObjUm* this, PlayState* play, EnHorse* bandit1, EnHorse* bandit2) {
    s32 pad;
    Vec3f sp30;
    s16 phi_v1_5;

    bandit1->unk_55C--;
    if (bandit1->unk_55C <= 0) {
        bandit1->unk_55C = 0;

        if ((bandit1->unk_550 == 3) && !(this->flags & OBJ_UM_FLAG_MINIGAME_FINISHED)) {
            s32 potIndex = -1;

            if (this->potsLife[0] != 1) {
                potIndex = 0;
            }

            if (this->potsLife[1] != 1) {
                if ((potIndex == -1) || ((potIndex == 0) && (Rand_ZeroOne() < 0.3f))) {
                    potIndex = 1;
                }
            }

            if (this->potsLife[2] != 1) {
                if ((potIndex == -1) || ((potIndex != -1) && (Rand_ZeroOne() < 0.3f))) {
                    potIndex = 2;
                }
            }

            if (this->potsLife[potIndex] != 1) {
                this->wasPotHit[potIndex] = true;
                if (this->potsLife[potIndex] == 2) {
                    Audio_PlaySfx_AtPos(&this->potPos[potIndex], NA_SE_EV_MILK_POT_BROKEN);
                } else {
                    Audio_PlaySfx_AtPos(&this->potPos[potIndex], NA_SE_EV_MILK_POT_DAMAGE);
                }

                this->potsLife[potIndex]--;
                if (this->potsLife[potIndex] < 1) {
                    this->potsLife[potIndex] = 1;
                }
            }
        }

        func_80B781DC(this, bandit1, bandit2, play);
    }

    Math3D_LineSplitRatio(&bandit1->unk_540, &this->unk_360[bandit1->unk_550],
                          1.0f - ((f32)bandit1->unk_55C / bandit1->unk_560), &sp30);
    bandit1->banditPosition = sp30;
    bandit1->unk_588 = this->dyna.actor.shape.rot.y;

    if ((bandit1->unk_550 == 10) || ((bandit1->unk_550 == 8))) {
        phi_v1_5 = bandit1->unk_588;
    } else if (Math3D_Vec3f_DistXYZ(&bandit1->actor.prevPos, &bandit1->actor.world.pos) < 10.0f) {
        phi_v1_5 = bandit1->unk_588;
    } else {
        phi_v1_5 = Math_Vec3f_Yaw(&bandit1->actor.prevPos, &bandit1->actor.world.pos);
    }

    phi_v1_5 -= bandit1->actor.shape.rot.y;
    if (phi_v1_5 > 0x190) {
        bandit1->actor.shape.rot.y += 0x190;
    } else if (phi_v1_5 < -0x190) {
        bandit1->actor.shape.rot.y -= 0x190;
    } else {
        bandit1->actor.shape.rot.y += phi_v1_5;
    }

    return 0;
}

// ObjUm_UpdateBanditsDamage?
s32 func_80B78A54(ObjUm* this, PlayState* play, s32 arg2, EnHorse* arg3, EnHorse* arg4) {
    if (this->banditsCollisions[arg2].base.acFlags & AC_HIT) {
        if (arg3->unk_550 == 3) {
            s16 sp36 =
                Math_Vec3f_Yaw(&this->dyna.actor.world.pos, &arg3->actor.world.pos) - this->dyna.actor.shape.rot.y;

            this->banditsCollisions[arg2].base.acFlags &= ~AC_HIT;
            Audio_PlaySfx_AtPos(&arg3->actor.projectedPos, NA_SE_EN_CUTBODY);
            arg3->unk_54C = 0xF;

            if (Math_SinS(sp36) > 0.0f) {
                arg3->unk_550 = arg4->unk_550 != 10 ? 10 : 8;
                arg3->unk_568 = -1.0f;
            } else {
                arg3->unk_550 = arg4->unk_550 != 8 ? 8 : 10;
                arg3->unk_568 = 1.0f;
            }

            arg3->unk_540 = arg3->actor.world.pos;
            arg3->unk_55C = 40;
            arg3->unk_560 = 40;
            arg3->unk_564 = 1;
            if (arg3->rider != NULL) {
                arg3->rider->actor.colorFilterTimer = 20;
                Actor_SetColorFilter(&arg3->rider->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 40);
            }
        } else {
            if (arg3->rider != NULL) {
                arg3->rider->actor.colorFilterTimer = 20;
                Actor_SetColorFilter(&arg3->rider->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 40);
            }
            Audio_PlaySfx_AtPos(&arg3->actor.projectedPos, NA_SE_EN_CUTBODY);
        }
    }

    return 0;
}

// ObjUm_UpdateBanditLogic?
s32 func_80B78C18(ObjUm* this, PlayState* play) {
    EnHorse* bandit1 = this->bandit1;
    EnHorse* bandit2 = this->bandit2;

    if (!(this->flags & OBJ_UM_FLAG_0200)) {
        func_80B783E0(this, play, 0, bandit1);
    } else {
        func_80B78764(this, play, bandit1, bandit2);
    }

    if (!(this->flags & OBJ_UM_FLAG_0400)) {
        func_80B783E0(this, play, 1, bandit2);
    } else {
        func_80B78764(this, play, bandit2, bandit1);
    }

    func_80B78A54(this, play, 0, bandit1, bandit2);
    func_80B78A54(this, play, 1, bandit2, bandit1);
    return 0;
}

s32 ObjUm_UpdateBanditsCollisions(ObjUm* this, PlayState* play) {
    s32 pad[3];

    this->banditsCollisions[0].dim.pos.x = this->bandit1->actor.world.pos.x;
    this->banditsCollisions[0].dim.pos.y = this->bandit1->actor.world.pos.y + 70.0f;
    this->banditsCollisions[0].dim.pos.z = this->bandit1->actor.world.pos.z;

    this->banditsCollisions[1].dim.pos.x = this->bandit2->actor.world.pos.x;
    this->banditsCollisions[1].dim.pos.y = this->bandit2->actor.world.pos.y + 70.0f;
    this->banditsCollisions[1].dim.pos.z = this->bandit2->actor.world.pos.z;

    CollisionCheck_SetAC(play, &play->colChkCtx, &this->banditsCollisions[0].base);
    CollisionCheck_SetAC(play, &play->colChkCtx, &this->banditsCollisions[1].base);
    return 0;
}

// ObjUm_UpdateBandits?
s32 func_80B78DF0(ObjUm* this, PlayState* play) {
    func_80B78C18(this, play);
    ObjUm_UpdateBanditsCollisions(this, play);
    return 0;
}

void ObjUm_SetupAction(ObjUm* this, ObjUmActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void ObjUm_SetPlayerPosition(ObjUm* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (this->flags & OBJ_UM_FLAG_DRAWN_FLOOR) {
        player->actor.world.pos = this->unk_308;
        player->actor.prevPos = this->unk_308;
    }
}

void ObjUm_RotatePlayer(ObjUm* this, PlayState* play, s16 angle) {
    Player* player = GET_PLAYER(play);

    player->actor.shape.rot.y = player->actor.world.rot.y = player->yaw = this->dyna.actor.shape.rot.y + angle;
}

void func_80B78EBC(ObjUm* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    player->actor.focus.rot.x = 0;
    player->actor.focus.rot.z = 0;
    player->actor.focus.rot.y = player->actor.shape.rot.y;

    player->headLimbRot.x = 0;
    player->headLimbRot.y = 0;
    player->headLimbRot.z = 0;

    player->upperLimbRot.x = 0;
    player->upperLimbRot.y = 0;
    player->upperLimbRot.z = 0;

    player->yaw = player->actor.focus.rot.y;
}

void ObjUm_RotatePlayerView(ObjUm* this, PlayState* play, s16 angle) {
    Player* player = GET_PLAYER(play);

    player->actor.focus.rot.y += angle;
}

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(cullingVolumeScale, 1200, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 300, ICHAIN_STOP),
};

void ObjUm_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    ObjUm* this = (ObjUm*)thisx;
    s32 sp54 = true;
    s32 i;

    DynaPolyActor_Init(&this->dyna, 0);
    this->unk_350 = 0;

    this->unk_2C4 = this->dyna.actor.world.pos;
    this->unk_2DC = this->dyna.actor.world.pos;
    this->unk_308 = this->dyna.actor.world.pos;

    for (i = 0; i < MILK_POTS_COUNT; i++) {
        this->potsLife[i] = 5;
        this->wasPotHit[i] = false;
        this->potPos[i] = gZeroVec3f;
    }

    this->donkey = NULL;
    this->unk_354 = 0;
    this->unk_420 = 0;
    this->cartBedPos = this->dyna.actor.world.pos;

    this->unk_4CC = 0;
    this->mouthTexIndex = 0;
    this->flags = OBJ_UM_FLAG_NONE;
    this->dyna.actor.gravity = -3.5f;

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    ActorShape_Init(&this->dyna.actor.shape, 0.0f, NULL, 50.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &gUmSkel, NULL, this->jointTable, this->morphTable, UM_LIMB_MAX);
    Animation_PlayLoop(&this->skelAnime, &gUmTrotAnim);

    this->wheelRot = 0;
    ObjUm_DefaultAnim(this, play);

    this->type = OBJ_UM_GET_TYPE(thisx);
    this->initialPathIndex = OBJ_UM_GET_PATH_INDEX(thisx);

    // if (!AliensDefeated)
    if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_DEFENDED_AGAINST_ALIENS)) {
        Actor_Kill(&this->dyna.actor);
        return;
    }

    if (this->type == OBJ_UM_TYPE_TERMINA_FIELD) {
        ObjUm_SetupAction(this, ObjUm_TerminaFieldIdle);
    } else if (this->type == OBJ_UM_TYPE_RANCH) {
        this->pathIndex = this->initialPathIndex;
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_31_80)) {
            // In cutscene

            sp54 = false;
            this->flags |= OBJ_UM_FLAG_0100;
            ObjUm_SetupAction(this, func_80B7A144);
            Environment_StopTime();
        } else {
            // Waiting for player

            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_34_80) || (CURRENT_TIME >= CLOCK_TIME(19, 0)) ||
                (CURRENT_TIME <= CLOCK_TIME(6, 0)) || CHECK_WEEKEVENTREG(WEEKEVENTREG_ESCORTED_CREMIA) ||
                CHECK_WEEKEVENTREG(WEEKEVENTREG_52_02)) {
                Actor_Kill(&this->dyna.actor);
                return;
            }

            this->dyna.actor.attentionRangeType = ATTENTION_RANGE_6;
            this->unk_2B4 = 0;
            ObjUm_SetupAction(this, ObjUm_RanchWait);
        }
    } else if (this->type == OBJ_UM_TYPE_PRE_MILK_RUN) {
        if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_31_80) || CHECK_WEEKEVENTREG(WEEKEVENTREG_ESCORTED_CREMIA)) {
            Actor_Kill(&this->dyna.actor);
            return;
        }

        if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_52_02)) {
            this->pathIndex = this->initialPathIndex;
            sp54 = false;
            Environment_StopTime();
            ObjUm_SetupAction(this, ObjUm_PreMilkRunStartCs);
            this->unk_354 = 0;
            ObjUm_RotatePlayer(this, play, 0);
        }
    } else if (this->type == OBJ_UM_TYPE_MILK_RUN_MINIGAME) {
        if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_31_80)) {
            Actor_Kill(&this->dyna.actor);
            return;
        }

        this->pathIndex = this->initialPathIndex;
        sp54 = false;
        Environment_StopTime();
        ObjUm_SetupAction(this, ObjUm_StartCs);
        this->unk_354 = 0;
        ObjUm_RotatePlayer(this, play, 0);
    } else if (this->type == OBJ_UM_TYPE_POST_MILK_RUN) {
        if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_ESCORTED_CREMIA) || CHECK_WEEKEVENTREG(WEEKEVENTREG_59_02)) {
            Actor_Kill(&this->dyna.actor);
            return;
        }

        this->pathIndex = this->initialPathIndex;
        sp54 = false;
        Environment_StopTime();
        ObjUm_SetupAction(this, ObjUm_PostMilkRunStartCs);
        this->unk_354 = 0;
        ObjUm_RotatePlayer(this, play, 0);
        Audio_PlayFanfare(NA_BGM_CLEAR_EVENT);
    } else {
        this->type = OBJ_UM_TYPE_TERMINA_FIELD;
        ObjUm_SetupAction(this, ObjUm_TerminaFieldIdle);
    }

    this->unk_2F8 = this->unk_2FE = gZeroVec3s;

    if (sp54) {
        DynaPolyActor_Init(&this->dyna, 0);
        DynaPolyActor_LoadMesh(play, &this->dyna, &object_um_Colheader_007E20);
    } else {
        DynaPolyActor_Init(&this->dyna, DYNA_TRANSFORM_POS | DYNA_TRANSFORM_ROT_Y);
        DynaPolyActor_LoadMesh(play, &this->dyna, &object_um_Colheader_007F50);
    }

    if (this->dyna.bgId == BGCHECK_SCENE) {
        Actor_Kill(&this->dyna.actor);
        return;
    }

    DynaPoly_DisableCeilingCollision(play, &play->colCtx.dyna, this->dyna.bgId);

    this->donkey =
        (EnHorse*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_HORSE, this->dyna.actor.world.pos.x,
                              this->dyna.actor.world.pos.y, this->dyna.actor.world.pos.z, 0,
                              this->dyna.actor.shape.rot.y, 0, ENHORSE_PARAMS(ENHORSE_PARAM_DONKEY, ENHORSE_18));

    if (this->donkey == NULL) {
        Actor_Kill(&this->dyna.actor);
        return;
    }

    Collider_InitAndSetCylinder(play, &this->banditsCollisions[0], &this->dyna.actor, &sCylinderInit);
    Collider_InitAndSetCylinder(play, &this->banditsCollisions[1], &this->dyna.actor, &sCylinderInit);
}

void ObjUm_Destroy(Actor* thisx, PlayState* play) {
    ObjUm* this = (ObjUm*)thisx;
    s32 i;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);

    for (i = 0; i < ARRAY_COUNT(this->potPos); i++) {
        AudioSfx_StopByPos(&this->potPos[i]);
    }

    Collider_DestroyCylinder(play, &this->banditsCollisions[0]);
    Collider_DestroyCylinder(play, &this->banditsCollisions[1]);
}

// ObjUm_KillMyDonkeyAndMyself, ObjUm_TerminateMe, ObjUmn't, ObjUm_Asinucide
void func_80B79524(ObjUm* this) {
    Actor_Kill(&this->dyna.actor);
    if (this->donkey != NULL) {
        Actor_Kill(&this->donkey->actor);
    }
}

void func_80B79560(PlayState* play, ObjUm* this, s32 arg2, u16 textId) {
    // "Thanks, I rely on you"
    if (textId == 0x33BF) {
        ObjUm_SetupAction(this, ObjUm_StartCs);
    }
}

s32 func_80B795A0(PlayState* play, ObjUm* this, s32 arg2) {
    s32 pad[2];
    s32 phi_v1 = true;
    u16 textId = this->dyna.actor.textId;
    Player* player;

    switch (textId) {
        // "I'll go to town"
        case 0x33B4:
        // "Want a ride?"
        case 0x33CF:
            SET_WEEKEVENTREG(WEEKEVENTREG_31_40);
            if (play->msgCtx.choiceIndex == 0) {
                player = GET_PLAYER(play);
                Audio_PlaySfx_MessageDecide();
                SET_WEEKEVENTREG(WEEKEVENTREG_31_80);
                play->nextEntrance = ENTRANCE(ROMANI_RANCH, 11);
                if (player->stateFlags1 & PLAYER_STATE1_800000) {
                    D_801BDAA0 = true;
                }
                play->transitionType = TRANS_TYPE_64;
                gSaveContext.nextTransitionType = TRANS_TYPE_FADE_WHITE;
                play->transitionTrigger = TRANS_TRIGGER_START;
                phi_v1 = true;
            } else {
                Actor_ContinueText(play, &this->dyna.actor, 0x33B5);
                Audio_PlaySfx_MessageCancel();
                Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_CREMIA);
                phi_v1 = false;
            }
            break;

        // "I'll go as fast as I can!"
        case 0x33BB:
            Actor_ContinueText(play, &this->dyna.actor, 0x33BC);
            phi_v1 = false;
            break;

        // "Chase pursuers with your arrows."
        case 0x33BC:
            Actor_ContinueText(play, &this->dyna.actor, 0x33BD);
            phi_v1 = false;
            break;

        // "Understand?"
        case 0x33BD:
            if (play->msgCtx.choiceIndex == 0) {
                Actor_ContinueText(play, &this->dyna.actor, 0x33BE);
                Audio_PlaySfx_MessageCancel();
            } else {
                Actor_ContinueText(play, &this->dyna.actor, 0x33BF);
                Audio_PlaySfx_MessageDecide();
            }
            phi_v1 = false;
            break;

        // "I'll tell you again!"
        case 0x33BE:
            Actor_ContinueText(play, &this->dyna.actor, 0x33BC);
            phi_v1 = false;
            break;

        default:
            break;
    }

    return phi_v1;
}

s32 func_80B79734(PlayState* play, ObjUm* this, s32 arg2) {
    MessageContext* msgCtx = &play->msgCtx;
    s32 ret = false;

    switch (Message_GetState(msgCtx)) {
        case TEXT_STATE_CLOSING:
            func_80B79560(play, this, arg2, this->dyna.actor.textId);
            return true;

        case TEXT_STATE_CHOICE:
        case TEXT_STATE_EVENT:
            if (Message_ShouldAdvance(play) && func_80B795A0(play, this, arg2)) {
                msgCtx->msgMode = MSGMODE_TEXT_CLOSING;
                ret = true;
            }
            break;

        default:
            break;
    }
    return ret;
}

u16 ObjUm_RanchGetDialogue(PlayState* play, ObjUm* this, s32 arg2) {
    u16 textId = 0;

    if (GET_PLAYER_FORM == PLAYER_FORM_HUMAN) {
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_31_40)) {
            // "Want a ride?"
            textId = 0x33CF;
        } else {
            // "I'll deliver milk"
            textId = 0x33B4;
        }
    } else {
        textId = 0x33B7;
    }

    if (textId == 0) {
        textId = 1;
    }
    return textId;
}

s32 func_80B7984C(PlayState* play, ObjUm* this, s32 arg2, s32* arg3) {
    s16 temp_v0_2;
    s16 phi_v1;

    if (*arg3 == 4) {
        return 0;
    }
    if (*arg3 == 2) {
        Message_StartTextbox(play, this->dyna.actor.textId, &this->dyna.actor);
        *arg3 = 1;
        return 0;
    }
    if (*arg3 == 3) {
        Message_ContinueTextbox(play, this->dyna.actor.textId);
        *arg3 = 1;
        return 0;
    }

    if (Actor_TalkOfferAccepted(&this->dyna.actor, &play->state)) {
        *arg3 = 1;
        return 1;
    }

    if (*arg3 == 1) {
        if (func_80B79734(play, this, arg2)) {
            *arg3 = 0;
        }
        return 0;
    }

    phi_v1 = this->dyna.actor.yawTowardsPlayer - this->dyna.actor.shape.rot.y;
    temp_v0_2 = ABS_ALT(phi_v1);
    if (temp_v0_2 >= 0x4E20) {
        return 0;
    }

    if ((this->dyna.actor.xyzDistToPlayerSq > SQ(100.0f)) && !this->dyna.actor.isLockedOn) {
        return 0;
    }

    if (this->dyna.actor.xyzDistToPlayerSq <= SQ(50.0f)) {
        if (Actor_OfferTalk(&this->dyna.actor, play, 50.0f)) {
            this->dyna.actor.textId = ObjUm_RanchGetDialogue(play, this, arg2);
        }
    } else if (Actor_OfferTalkNearColChkInfoCylinder(&this->dyna.actor, play)) {
        this->dyna.actor.textId = ObjUm_RanchGetDialogue(play, this, arg2);
    }

    return 0;
}

s32 func_80B79A24(s32 arg0) {
    switch (arg0) {
        case 1:
        case 2:
        case 3:
            return true;

        default:
            return false;
    }
}

void ObjUm_RanchWait(ObjUm* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    this->dyna.actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    SkelAnime_Update(&this->skelAnime);
    ObjUm_ChangeAnim(this, play, OBJ_UM_ANIM_IDLE);
    this->flags |= OBJ_UM_FLAG_WAITING;
    if ((CURRENT_TIME > CLOCK_TIME(18, 0)) && (CURRENT_TIME <= CLOCK_TIME(19, 0))) {
        if (!(player->stateFlags1 & PLAYER_STATE1_800000)) {
            func_80B7984C(play, this, 0, &this->unk_2B4);
        }
    } else if (!func_80B79A24(this->unk_2B4) && (CURRENT_TIME > CLOCK_TIME(19, 0))) {
        SET_WEEKEVENTREG(WEEKEVENTREG_34_80);
        ObjUm_SetupAction(this, ObjUm_RanchWaitPathFinished);
    }

    switch (play->msgCtx.currentTextId) {
        default:
            this->unk_4CC = 0;
            this->mouthTexIndex = 0;
            break;

        // "I'm worried about my sister"
        case 0x33B7:
        // "I'll deliver milk"
        case 0x33B4:
            this->unk_4CC = 0;
            this->mouthTexIndex = 1;
            break;

        // "I'll leave at 7"
        case 0x33B5:
            this->unk_4CC = 3;
            this->mouthTexIndex = 1;
            break;
    }
}

typedef enum ObjUmPathState {
    /* 0 */ OBJUM_PATH_STATE_0,
    /* 1 */ OBJUM_PATH_STATE_1,
    /* 2 */ OBJUM_PATH_STATE_FINISH,
    /* 3 */ OBJUM_PATH_STATE_3,
    /* 4 */ OBJUM_PATH_STATE_4
} ObjUmPathState;

ObjUmPathState ObjUm_UpdatePath(ObjUm* this, PlayState* play) {
    Path* path = &play->setupPathList[this->pathIndex];
    s32 pathCount;
    Vec3s* pathPoints;
    f32 xDiff;
    f32 zDiff;
    Vec3f sp50;
    f32 sp4C;
    f32 sp48;
    f32 sp44;
    s32 angle;
    ObjUmPathState sp3C;
    s16 yawDiff;

    pathCount = path->count;
    pathPoints = Lib_SegmentedToVirtual(path->points);
    sp3C = OBJUM_PATH_STATE_0;

    if (pathCount == 0) {
        return 0;
    }

    Math_Vec3s_ToVec3f(&sp50, &pathPoints[this->pointIndex]);

    if (this->pointIndex == 0) {
        xDiff = pathPoints[1].x - pathPoints[0].x;
        zDiff = pathPoints[1].z - pathPoints[0].z;
    } else if ((this->pointIndex + 1) == path->count) {
        xDiff = pathPoints[path->count - 1].x - pathPoints[path->count - 2].x;
        zDiff = pathPoints[path->count - 1].z - pathPoints[path->count - 2].z;
    } else {
        xDiff = pathPoints[this->pointIndex + 1].x - pathPoints[this->pointIndex - 1].x;
        zDiff = pathPoints[this->pointIndex + 1].z - pathPoints[this->pointIndex - 1].z;
    }

    angle = Math_Atan2S(xDiff, zDiff);

    Math3D_RotateXZPlane(&sp50, angle, &sp4C, &sp48, &sp44);
    if (((this->dyna.actor.world.pos.x * sp4C) + (sp48 * this->dyna.actor.world.pos.z) + sp44) > 0.0f) {
        this->pointIndex++;

        if (this->pointIndex >= (pathCount - 7)) {
            sp3C = OBJUM_PATH_STATE_3;
        }
        if (this->pointIndex >= (pathCount - 3)) {
            sp3C = OBJUM_PATH_STATE_1;
        }
        if (this->pointIndex >= (pathCount - 2)) {
            sp3C = OBJUM_PATH_STATE_4;
        }
        if (this->pointIndex >= pathCount) {
            this->pointIndex = 0;
            sp3C = OBJUM_PATH_STATE_FINISH;
        }

        Math_Vec3s_ToVec3f(&sp50, &pathPoints[this->pointIndex]);
    }

    if (this->donkey != NULL) {
        this->dyna.actor.world.rot.y = Math_Vec3f_Yaw(&this->dyna.actor.world.pos, &sp50);
        Horse_RotateToPoint(&this->donkey->actor, &sp50, 0x190);

        yawDiff = this->donkey->actor.shape.rot.y - this->dyna.actor.shape.rot.y;
        if (fabsf(yawDiff) < 2730.0f) {
            if (fabsf(yawDiff) < 100.0f) {
                this->dyna.actor.shape.rot.y = this->donkey->actor.shape.rot.y;
            } else if (yawDiff > 0) {
                this->dyna.actor.shape.rot.y += 0x64;
                yawDiff = 0x64;
            } else if (yawDiff < 0) {
                this->dyna.actor.shape.rot.y -= 0x64;
                yawDiff = -0x64;
            }
        } else if (yawDiff > 0) {
            this->dyna.actor.shape.rot.y += 0x190;
            yawDiff = 0x190;
        } else if (yawDiff < 0) {
            this->dyna.actor.shape.rot.y -= 0x190;
            yawDiff = -0x190;
        }

        if (this->flags & (OBJ_UM_FLAG_0010 | OBJ_UM_FLAG_0004)) {
            ObjUm_RotatePlayerView(this, play, yawDiff);
        }
    }

    if (this->animIndex == OBJ_UM_ANIM_TROT) {
        this->dyna.actor.speed = 4.0f;
    } else if (this->animIndex == OBJ_UM_ANIM_GALLOP) {
        this->dyna.actor.speed = 8.0f;
    }

    return sp3C;
}

void ObjUm_RanchWaitPathFinished(ObjUm* this, PlayState* play) {
    this->wheelRot += 0x3E8;
    this->flags &= ~OBJ_UM_FLAG_WAITING;
    ObjUm_ChangeAnim(this, play, OBJ_UM_ANIM_TROT);

    switch (ObjUm_UpdatePath(this, play)) {
        case OBJUM_PATH_STATE_1:
        case OBJUM_PATH_STATE_FINISH:
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_31_80)) {
                CutsceneManager_Stop(this->dyna.actor.csId);
                play->nextEntrance = ENTRANCE(MILK_ROAD, 5);
                play->transitionType = TRANS_TYPE_64;
                gSaveContext.nextTransitionType = TRANS_TYPE_FADE_WHITE;
                play->transitionTrigger = TRANS_TRIGGER_START;
            } else {
                func_80B79524(this);
            }
            break;

        default:
            Actor_MoveWithGravity(&this->dyna.actor);
            break;
    }
}

void ObjUm_RanchStartCs(ObjUm* this, PlayState* play) {
    ObjUm_ChangeAnim(this, play, OBJ_UM_ANIM_IDLE);

    if (CutsceneManager_IsNext(this->dyna.actor.csId)) {
        CutsceneManager_StartWithPlayerCs(this->dyna.actor.csId, &this->dyna.actor);
        this->lastTime = CURRENT_TIME;
        ObjUm_SetupAction(this, func_80B7A0E0);
    } else {
        CutsceneManager_Queue(this->dyna.actor.csId);
    }
}

void func_80B7A070(ObjUm* this, PlayState* play) {
    ObjUm_RanchWaitPathFinished(this, play);
    ObjUm_RotatePlayer(this, play, 0);

    switch (play->msgCtx.currentTextId) {
        case 0x33B6:
            this->unk_4CC = 1;
            this->mouthTexIndex = 1;
            break;

        default:
            this->unk_4CC = 0;
            this->mouthTexIndex = 0;
            break;
    }
}

void func_80B7A0E0(ObjUm* this, PlayState* play) {
    ObjUm_ChangeAnim(this, play, OBJ_UM_ANIM_IDLE);
    if (CURRENT_TIME != this->lastTime) {
        ObjUm_ChangeAnim(this, play, OBJ_UM_ANIM_TROT);
        ObjUm_SetupAction(this, func_80B7A070);
    }
}

void func_80B7A144(ObjUm* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    ObjUm_SetPlayerPosition(this, play);
    this->flags |= OBJ_UM_FLAG_0100;
    this->flags |= OBJ_UM_FLAG_0004;
    player->stateFlags1 |= PLAYER_STATE1_20;
    ObjUm_ChangeAnim(this, play, OBJ_UM_ANIM_IDLE);
    ObjUm_SetupAction(this, ObjUm_RanchStartCs);
}

void ObjUm_PreMilkRunDialogueHandler(ObjUm* this, PlayState* play) {
    ObjUm_SetPlayerPosition(this, play);
    this->flags |= OBJ_UM_FLAG_0004;

    switch (play->msgCtx.currentTextId) {
        case 0x33B8:
            this->unk_4CC = 0;
            this->mouthTexIndex = 3;
            break;

        case 0x33B9:
            this->unk_4CC = 2;
            this->mouthTexIndex = 0;
            break;

        default:
            this->flags &= ~OBJ_UM_FLAG_LOOK_BACK;
            this->unk_4CC = 0;
            this->mouthTexIndex = 0;
            break;
    }
}

void func_80B7A240(ObjUm* this, PlayState* play) {
    ObjUm_ChangeAnim(this, play, OBJ_UM_ANIM_IDLE);
    if (CURRENT_TIME != this->lastTime) {
        ObjUm_SetupAction(this, func_80B7A2AC);
    }

    this->lastTime = CURRENT_TIME;
    ObjUm_PreMilkRunDialogueHandler(this, play);
}

void func_80B7A2AC(ObjUm* this, PlayState* play) {
    this->wheelRot += 0x3E8;
    ObjUm_ChangeAnim(this, play, OBJ_UM_ANIM_TROT);

    switch (ObjUm_UpdatePath(this, play)) {
        case OBJUM_PATH_STATE_1:
        case OBJUM_PATH_STATE_FINISH:
            play->nextEntrance = ENTRANCE(GORMAN_TRACK, 4);
            play->transitionType = TRANS_TYPE_64;
            gSaveContext.nextTransitionType = TRANS_TYPE_FADE_WHITE;
            play->transitionTrigger = TRANS_TRIGGER_START;
            break;

        default:
            if (CURRENT_TIME == this->lastTime) {
                ObjUm_SetupAction(this, func_80B7A240);
            }

            this->lastTime = CURRENT_TIME;
            Actor_MoveWithGravity(&this->dyna.actor);
            ObjUm_PreMilkRunDialogueHandler(this, play);
            break;
    }
}

void func_80B7A394(ObjUm* this, PlayState* play) {
    ObjUm_SetPlayerPosition(this, play);
    this->flags |= OBJ_UM_FLAG_0004;
    if (CURRENT_TIME != this->lastTime) {
        ObjUm_ChangeAnim(this, play, OBJ_UM_ANIM_TROT);
        ObjUm_SetupAction(this, func_80B7A2AC);
    }
}

void ObjUm_PreMilkRunStartCs(ObjUm* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    ObjUm_SetPlayerPosition(this, play);
    this->flags |= OBJ_UM_FLAG_0004;
    player->stateFlags1 |= PLAYER_STATE1_20;
    if (CutsceneManager_IsNext(this->dyna.actor.csId)) {
        CutsceneManager_StartWithPlayerCs(this->dyna.actor.csId, &this->dyna.actor);
        this->lastTime = CURRENT_TIME;
        ObjUm_SetupAction(this, func_80B7A394);
    } else {
        CutsceneManager_Queue(this->dyna.actor.csId);
    }
}

void ObjUm_RunMinigame(ObjUm* this, PlayState* play) {
    ObjUm_SetPlayerPosition(this, play);
    ObjUm_RotatePlayer(this, play, 0x7FFF);
    this->wheelRot += 0x7D0;
    this->flags |= OBJ_UM_FLAG_0010;
    ObjUm_ChangeAnim(this, play, OBJ_UM_ANIM_GALLOP);

    switch (ObjUm_UpdatePath(this, play)) {
        case OBJUM_PATH_STATE_1:
        case OBJUM_PATH_STATE_FINISH:
            gSaveContext.seqId = NA_BGM_DISABLED;
            CLEAR_WEEKEVENTREG(WEEKEVENTREG_31_80);
            gSaveContext.ambienceId = AMBIENCE_ID_DISABLED;

            if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_ESCORTED_CREMIA) && !CHECK_WEEKEVENTREG(WEEKEVENTREG_52_02)) {
                if (!this->areAllPotsBroken) {
                    play->nextEntrance = ENTRANCE(MILK_ROAD, 6);
                    play->transitionType = TRANS_TYPE_64;
                    gSaveContext.nextTransitionType = TRANS_TYPE_FADE_WHITE;
                    play->transitionTrigger = TRANS_TRIGGER_START;
                    SET_WEEKEVENTREG(WEEKEVENTREG_ESCORTED_CREMIA);
                    CLEAR_WEEKEVENTREG(WEEKEVENTREG_52_02);
                } else {
                    play->nextEntrance = ENTRANCE(ROMANI_RANCH, 8);
                    play->transitionType = TRANS_TYPE_64;
                    gSaveContext.nextTransitionType = TRANS_TYPE_FADE_WHITE;
                    play->transitionTrigger = TRANS_TRIGGER_START;
                    SET_WEEKEVENTREG(WEEKEVENTREG_52_02);
                    CLEAR_WEEKEVENTREG(WEEKEVENTREG_ESCORTED_CREMIA);
                }
            }
            break;

        default:
            Actor_MoveWithGravity(&this->dyna.actor);
            func_80B78DF0(this, play);
            break;
    }
}

void func_80B7A614(ObjUm* this, PlayState* play) {
    s32 pad;

    ObjUm_SetPlayerPosition(this, play);
    ObjUm_RotatePlayer(this, play, 0x7FFF);
    this->wheelRot += 0x7D0;
    this->flags |= OBJ_UM_FLAG_0010;
    this->flags |= OBJ_UM_FLAG_PLAYING_MINIGAME;
    ObjUm_ChangeAnim(this, play, OBJ_UM_ANIM_GALLOP);

    if ((ObjUm_UpdatePath(this, play) == OBJUM_PATH_STATE_3) && (this->unk_4DC == 0)) {
        this->unk_4DC = 1;
    } else if (this->unk_4DC > 0) {
        if (this->unk_4DC == 1) {
            s32 i;

            this->areAllPotsBroken = true;

            for (i = 0; i < ARRAY_COUNT(this->potsLife); i++) {
                if (this->potsLife[i] != 1) {
                    this->areAllPotsBroken = false;
                    break;
                }
            }

            this->flags |= OBJ_UM_FLAG_MINIGAME_FINISHED;
        }

        this->unk_4DC++;
    }

    if (this->flags & OBJ_UM_FLAG_MINIGAME_FINISHED) {
        s32 csId = CutsceneManager_GetAdditionalCsId(this->dyna.actor.csId);

        if (this->areAllPotsBroken) {
            csId = CutsceneManager_GetAdditionalCsId(csId);
        }
        if (CutsceneManager_IsNext(csId)) {
            CutsceneManager_StartWithPlayerCs(csId, &this->dyna.actor);
            ObjUm_SetupAction(this, ObjUm_RunMinigame);
            this->flags &= ~OBJ_UM_FLAG_PLAYING_MINIGAME;
        } else {
            CutsceneManager_Queue(csId);
        }
    }

    Actor_MoveWithGravity(&this->dyna.actor);
    func_80B78DF0(this, play);
}

void func_80B7A7AC(ObjUm* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    this->unk_4DC = 0;
    this->areAllPotsBroken = false;
    player->stateFlags1 &= ~PLAYER_STATE1_20;
    ObjUm_SetPlayerPosition(this, play);
    ObjUm_RotatePlayer(this, play, 0x7FFF);
    func_80B78EBC(this, play);
    this->flags |= OBJ_UM_FLAG_0010;
    func_80B78DF0(this, play);
    this->flags |= 4;
    ObjUm_ChangeAnim(this, play, OBJ_UM_ANIM_GALLOP);
    ObjUm_SetupAction(this, func_80B7A614);
}

void func_80B7A860(ObjUm* this, PlayState* play) {
    ObjUm_SetPlayerPosition(this, play);
    ObjUm_RotatePlayer(this, play, 0);
    this->flags |= OBJ_UM_FLAG_0004;

    if (play->csCtx.curFrame == 449) {
        ObjUm_InitBandits(this, play);
    } else if (play->csCtx.curFrame >= 450) {
        func_80B78DF0(this, play);
    }

    if (play->csCtx.state == CS_STATE_IDLE) {
        CutsceneManager_Stop(this->dyna.actor.csId);
        ObjUm_SetupAction(this, func_80B7A7AC);
    }

    switch (play->msgCtx.currentTextId) {
        case 0x33BA:
            this->unk_4CC = 2;
            this->mouthTexIndex = 3;
            break;

        // "We'll go through here as fast as we can"
        case 0x33BB:
            if ((fabsf(this->skelAnime.curFrame) < 0.008f) && !(this->flags & OBJ_UM_FLAG_1000)) {
                this->flags |= OBJ_UM_FLAG_1000;
                this->unk_4CC = 4;
                this->mouthTexIndex = 0;
            } else if (fabsf(this->skelAnime.morphWeight) < 0.008f) {
                this->unk_4CC = 2;
                this->mouthTexIndex = 2;
            } else {
                this->unk_4CC = 5;
                this->mouthTexIndex = 0;
            }

            this->flags |= OBJ_UM_FLAG_LOOK_BACK;
            break;

        // "Chase pursuers with your arrows"
        case 0x33BC:
            if ((fabsf(this->skelAnime.curFrame) < 0.008f) && !(this->flags & OBJ_UM_FLAG_1000)) {
                this->flags |= OBJ_UM_FLAG_1000;
                this->unk_4CC = 4;
                this->mouthTexIndex = 0;
            } else if (fabsf(this->skelAnime.morphWeight) < 0.008f) {
                this->unk_4CC = 2;
                this->mouthTexIndex = 2;
            } else {
                this->unk_4CC = 5;
                this->mouthTexIndex = 0;
            }
            this->flags |= OBJ_UM_FLAG_LOOK_BACK;
            break;

        // "Understand?"
        case 0x33BD:
            if ((fabsf(this->skelAnime.curFrame) < 0.008f) && !(this->flags & OBJ_UM_FLAG_1000)) {
                this->flags |= OBJ_UM_FLAG_1000;
                this->unk_4CC = 4;
                this->mouthTexIndex = 0;
            } else if (fabsf(this->skelAnime.morphWeight) < 0.008f) {
                this->unk_4CC = 2;
            } else {
                this->unk_4CC = 5;
            }
            this->mouthTexIndex = 0;
            this->unk_4D8 = 0;
            this->flags |= OBJ_UM_FLAG_LOOK_BACK;
            break;

        case 0x33BE:
            this->flags |= OBJ_UM_FLAG_LOOK_BACK;
            this->unk_4CC = 2;
            this->mouthTexIndex = 3;
            break;

        case 0x33BF:
            this->unk_4D8++;
            if ((fabsf(this->skelAnime.morphWeight) < 0.008f) && (this->unk_4D8 >= 6)) {
                this->unk_4CC = 0;
                this->mouthTexIndex = 0;
            } else {
                this->unk_4CC = 2;
                this->mouthTexIndex = 2;
            }
            this->flags &= ~OBJ_UM_FLAG_LOOK_BACK;
            break;

        default:
            this->unk_4CC = 0;
            this->mouthTexIndex = 0;
            break;
    }
}

void func_80B7AB78(ObjUm* this, PlayState* play) {
    ObjUm_ChangeAnim(this, play, OBJ_UM_ANIM_IDLE);
    if (CURRENT_TIME != this->lastTime) {
        ObjUm_SetupAction(this, func_80B7ABE4);
    }

    this->lastTime = CURRENT_TIME;
    func_80B7A860(this, play);
}

void func_80B7ABE4(ObjUm* this, PlayState* play) {
    this->wheelRot += 0x3E8;

    ObjUm_ChangeAnim(this, play, OBJ_UM_ANIM_TROT);
    switch (ObjUm_UpdatePath(this, play)) {
        case OBJUM_PATH_STATE_FINISH:
            func_80B79524(this);
            break;

        default:
            if (CURRENT_TIME == this->lastTime) {
                ObjUm_SetupAction(this, func_80B7AB78);
            }

            this->lastTime = CURRENT_TIME;
            Actor_MoveWithGravity(&this->dyna.actor);
            func_80B7A860(this, play);
            break;
    }
}

void ObjUm_StartCs(ObjUm* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    player->stateFlags1 |= PLAYER_STATE1_20;
    ObjUm_SetPlayerPosition(this, play);
    ObjUm_RotatePlayer(this, play, 0);
    this->flags |= OBJ_UM_FLAG_0004;

    if (CutsceneManager_IsNext(this->dyna.actor.csId)) {
        CutsceneManager_StartWithPlayerCs(this->dyna.actor.csId, &this->dyna.actor);
        this->lastTime = CURRENT_TIME;
        ObjUm_SetupAction(this, func_80B7ABE4);
    } else {
        CutsceneManager_Queue(this->dyna.actor.csId);
    }
}

void ObjUm_PostMilkRunWaitPathFinished(ObjUm* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    player->stateFlags1 |= PLAYER_STATE1_20;
    ObjUm_SetPlayerPosition(this, play);
    ObjUm_RotatePlayer(this, play, 0);
    this->flags |= OBJ_UM_FLAG_0004;
    this->wheelRot += 0x3E8;
    ObjUm_ChangeAnim(this, play, OBJ_UM_ANIM_TROT);

    if ((ObjUm_UpdatePath(this, play) == OBJUM_PATH_STATE_4) && !CHECK_WEEKEVENTREG(WEEKEVENTREG_59_02)) {
        CutsceneManager_Stop(this->dyna.actor.csId);
        Audio_SetCutsceneFlag(false);
        SET_WEEKEVENTREG(WEEKEVENTREG_59_02);
        gSaveContext.nextCutsceneIndex = 0xFFF3;
        play->nextEntrance = ENTRANCE(TERMINA_FIELD, 0);
        play->transitionType = TRANS_TYPE_64;
        gSaveContext.nextTransitionType = TRANS_TYPE_FADE_WHITE;
        play->transitionTrigger = TRANS_TRIGGER_START;
        gSaveContext.save.time += CLOCK_TIME(1, 0) + 2;
    }
    Actor_MoveWithGravity(&this->dyna.actor);
}

void ObjUm_PostMilkRunStartCs(ObjUm* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    player->stateFlags1 |= PLAYER_STATE1_20;
    ObjUm_SetPlayerPosition(this, play);
    ObjUm_RotatePlayer(this, play, 0);
    this->flags |= OBJ_UM_FLAG_0004;
    ObjUm_ChangeAnim(this, play, OBJ_UM_ANIM_IDLE);

    if (CutsceneManager_IsNext(this->dyna.actor.csId)) {
        CutsceneManager_StartWithPlayerCs(this->dyna.actor.csId, &this->dyna.actor);
        ObjUm_SetupAction(this, ObjUm_PostMilkRunWaitPathFinished);
    } else {
        CutsceneManager_Queue(this->dyna.actor.csId);
    }
}

void ObjUm_TerminaFieldIdle(ObjUm* this, PlayState* play) {
    ObjUm_ChangeAnim(this, play, OBJ_UM_ANIM_IDLE);
    SkelAnime_Update(&this->skelAnime);
}

void func_80B7AF30(ObjUm* this, PlayState* play) {
    if (!(this->flags & OBJ_UM_FLAG_0001)) {
        this->dyna.actor.shape.rot.x = 0;
        this->dyna.actor.shape.rot.z = 0;
    } else {
        CollisionPoly* sp44;
        s32 pad[2];
        Vec3f sp30;

        this->flags &= ~OBJ_UM_FLAG_0001;

        if (1) {}

        this->unk_2D0 = this->unk_2C4;
        this->unk_2E8 = this->unk_2DC;

        this->unk_2D0.y = BgCheck_EntityRaycastFloor1(&play->colCtx, &sp44, &this->unk_2C4);
        if (this->unk_2D0.y == BGCHECK_Y_MIN) {
            this->unk_2D0.y = this->dyna.actor.floorHeight;
        }

        if (1) {}

        this->unk_2E8.y = BgCheck_EntityRaycastFloor1(&play->colCtx, &sp44, &this->unk_2DC);
        if (this->unk_2E8.y == BGCHECK_Y_MIN) {
            this->unk_2E8.y = this->dyna.actor.floorHeight;
        }

        sp30.x = (this->unk_2D0.x + this->unk_2E8.x) * 0.5f;
        sp30.y = (this->unk_2D0.y + this->unk_2E8.y) * 0.5f;
        sp30.z = (this->unk_2D0.z + this->unk_2E8.z) * 0.5f;

        this->dyna.actor.shape.rot.x =
            (s16)Math_Atan2S(sp30.y - this->dyna.actor.world.pos.y, sqrtf(SQ(sp30.x - this->dyna.actor.world.pos.x) +
                                                                          SQ(sp30.z - this->dyna.actor.world.pos.z)));
        this->dyna.actor.shape.rot.z = (s16)-Math_Atan2S(
            sp30.y - this->unk_2D0.y, sqrtf(SQ(sp30.x - this->unk_2D0.x) + SQ(sp30.z - this->unk_2D0.z)));
        if (this->flags & OBJ_UM_FLAG_MOVING) {
            this->dyna.actor.shape.rot.x += BINANG_SUB((Rand_ZeroOne() * 100.0f), 50.0f);
            this->dyna.actor.shape.rot.z += BINANG_SUB((Rand_ZeroOne() * 100.0f), 50.0f);
        }
    }
}

void ObjUm_DefaultAnim(ObjUm* this, PlayState* play) {
    Animation_PlayOnce(&this->skelAnime, &gUmTrotAnim);
    this->animIndex = OBJ_UM_ANIM_TROT;
}

typedef struct {
    /* 0x0 */ AnimationHeader* anim;
    /* 0x4 */ s32 doesMove; // `true` if the animation is intended to be used while the actor is moving
} struct_80B7C25C;          // size = 0x8

struct_80B7C25C sUmAnims[] = {
    { &gUmTrotAnim, true },      // OBJ_UM_ANIM_TROT
    { &gUmGallopAnim, true },    // OBJ_UM_ANIM_GALLOP
    { &gUmIdleAnim, false },     // OBJ_UM_ANIM_IDLE
    { NULL, false },             // OBJ_UM_ANIM_3
    { &gUmLookBackAnim, false }, // OBJ_UM_ANIM_LOOK_BACK
};

void ObjUm_ChangeAnim(ObjUm* this, PlayState* play, ObjUmAnimation animIndex) {
    s32 changeAnim;
    s32 temp;
    s32 indexTemp = animIndex;
    f32 animPlaybackSpeed = 0.0f;

    if (sUmAnims[animIndex].doesMove) {
        this->flags |= OBJ_UM_FLAG_MOVING;
    } else {
        this->flags &= ~OBJ_UM_FLAG_MOVING;
    }

    if (animIndex == OBJ_UM_ANIM_TROT) {
        animPlaybackSpeed = this->dyna.actor.speed * 0.25f;
    } else if (animIndex == OBJ_UM_ANIM_GALLOP) {
        animPlaybackSpeed = this->dyna.actor.speed * 0.2f;
    } else if (animIndex == OBJ_UM_ANIM_IDLE) {
        animPlaybackSpeed = 1.0f;
    }
    this->skelAnime.playSpeed = animPlaybackSpeed;

    if (this->flags & OBJ_UM_FLAG_LOOK_BACK) {
        this->skelAnime.playSpeed = 1.0f;
        animIndex = OBJ_UM_ANIM_MINUS_1;
    }

    changeAnim = (this->animIndex != animIndex);
    if (SkelAnime_Update(&this->skelAnime) || changeAnim) {
        this->animIndex = animIndex;

        if (animIndex != OBJ_UM_ANIM_MINUS_1) {
            if (this->donkey != NULL) {
                this->donkey->unk_538 = animIndex;
            }

            if (changeAnim) {
                Animation_MorphToPlayOnce(&this->skelAnime, sUmAnims[animIndex].anim, -3.0f);
            } else {
                Animation_PlayOnce(&this->skelAnime, sUmAnims[animIndex].anim);
            }
        } else {
            EnHorse* donkey = this->donkey;

            if (donkey != NULL) {
                donkey->unk_538 = indexTemp;
            }

            if (changeAnim) {
                temp = 3 - animIndex; // OBJ_UM_ANIM_LOOK_BACK
                Animation_MorphToPlayOnce(&this->skelAnime, sUmAnims[temp].anim, -10.0f);
            } else {
                temp = 3 - animIndex; // OBJ_UM_ANIM_LOOK_BACK
                Animation_PlayOnce(&this->skelAnime, sUmAnims[temp].anim);
            }
        }
    }

    if ((this->wheelRot / 0x199A) != this->unk_420) {
        this->unk_420 = this->wheelRot / 0x199A;
        //! FAKE
        if (sUmAnims[0].doesMove) {}
        Actor_PlaySfx(&this->dyna.actor, NA_SE_EV_CART_WHEEL);
    }
}

void ObjUm_Update(Actor* thisx, PlayState* play) {
    ObjUm* this = (ObjUm*)thisx;

    this->actionFunc(this, play);
    this->unk_350++;
    Actor_UpdateBgCheckInfo(play, &this->dyna.actor, 0.0f, 0.0f, 0.0f,
                            UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_8 | UPDBGCHECKINFO_FLAG_10);

    if (this->donkey != NULL) {
        this->donkey->actor.world.pos.x = this->dyna.actor.world.pos.x;
        this->donkey->actor.world.pos.y = this->dyna.actor.floorHeight;
        this->donkey->actor.world.pos.z = this->dyna.actor.world.pos.z;
    }

    if (this->flags & OBJ_UM_FLAG_0010) {
        Player_SetBButtonAmmo(play, 99);
        this->flags &= ~OBJ_UM_FLAG_0010;
    } else if (this->flags & OBJ_UM_FLAG_0004) {
        Player_SetBButtonAmmo(play, -3);
        this->flags &= ~OBJ_UM_FLAG_0004;
    }

    if (this->flags & OBJ_UM_FLAG_0100) {
        this->flags &= ~OBJ_UM_FLAG_0100;
        ObjUm_RotatePlayer(this, play, 0);
        func_80B78EBC(this, play);
    }

    func_80B7AF30(this, play);
    switch (this->unk_4CC) {
        case 0:
            switch (this->eyeTexIndex) {
                case 0:
                    if (Rand_ZeroOne() < 0.025f) {
                        this->eyeTexIndex = 1;
                    }
                    break;

                case 1:
                    if (Rand_ZeroOne() < 0.6f) {
                        this->eyeTexIndex = 2;
                    }
                    break;

                case 2:
                    if (Rand_ZeroOne() < 0.6f) {
                        this->eyeTexIndex = 0;
                    }
                    break;

                default:
                    this->eyeTexIndex = 0;
                    break;
            }
            break;

        case 1:
            this->eyeTexIndex = 3;
            break;

        case 2:
            this->eyeTexIndex = 4;
            break;

        case 3:
            this->eyeTexIndex = 5;
            break;

        case 4:
            this->eyeTexIndex = 2;
            break;

        case 5:
            this->eyeTexIndex = 0;
            break;

        default:
            this->unk_4CC = 0;
            this->eyeTexIndex = 0;
            break;
    }
}

s32 ObjUm_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    ObjUm* this = (ObjUm*)thisx;
    Player* player = GET_PLAYER(play);
    s32 pad;
    s16 temp_v0_3;
    Vec3f sp5C = { 4223.0f, -979.0f, 4098.0f };
    Vec3f sp50 = { 4223.0f, -980.0f, -4083.0f };

    if ((limbIndex >= UM_LIMB_CREMIA_ROOT) && (this->type == OBJ_UM_TYPE_TERMINA_FIELD)) {
        *dList = NULL;
        return false;
    }

    if (limbIndex == UM_LIMB_CREMIA_HEAD) {
        OPEN_DISPS(play->state.gfxCtx);

        gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(sEyeTextures[this->eyeTexIndex]));
        gSPSegment(POLY_OPA_DISP++, 0x09, Lib_SegmentedToVirtual(sMouthTextures[this->mouthTexIndex]));

        CLOSE_DISPS(play->state.gfxCtx);
    }

    if (limbIndex == UM_LIMB_WAGON_RIGHT_WHEEL) {
        if (this->flags & OBJ_UM_FLAG_MOVING) {
            rot->x = -this->wheelRot;
        }
        Matrix_MultVec3f(&sp5C, &this->unk_2C4);
    } else if (limbIndex == UM_LIMB_WAGON_LEFT_WHEEL) {
        if (this->flags & OBJ_UM_FLAG_MOVING) {
            rot->x = this->wheelRot;
        }
        Matrix_MultVec3f(&sp50, &this->unk_2DC);
    } else if ((limbIndex == UM_LIMB_CREMIA_HEAD) && (this->flags & OBJ_UM_FLAG_WAITING)) {
        if (SubS_AngleDiffLessEqual(this->dyna.actor.shape.rot.y, 0x4E20, this->dyna.actor.yawTowardsPlayer) &&
            (this->dyna.actor.xzDistToPlayer < 500.0f)) {
            s16 sp3E;
            Vec3f sp30 = player->actor.world.pos;

            sp30.y = player->bodyPartsPos[PLAYER_BODYPART_HEAD].y + 3.0f;
            sp3E = Math_Vec3f_Yaw(&this->dyna.actor.focus.pos, &sp30) - this->dyna.actor.shape.rot.y;
            temp_v0_3 = Math_Atan2S(
                this->dyna.actor.focus.pos.y - sp30.y,
                Math3D_Dist1D(sp30.x - this->dyna.actor.focus.pos.x, sp30.z - this->dyna.actor.focus.pos.z));
            this->unk_2FE.x = rot->x + sp3E;
            this->unk_2FE.y = rot->y;
            this->unk_2FE.z = rot->z + temp_v0_3;
            Math_SmoothStepToS(&this->unk_2F8.x, this->unk_2FE.x, 4, 0x3E8, 1);
            Math_SmoothStepToS(&this->unk_2F8.z, this->unk_2FE.z, 4, 0x3E8, 1);
            this->unk_2F8.y = rot->y;
            rot->x = this->unk_2F8.x;
            rot->z = this->unk_2F8.z;
        } else {
            this->unk_2FE.x = rot->x;
            this->unk_2FE.y = rot->y;
            this->unk_2FE.z = rot->z;
            Math_SmoothStepToS(&this->unk_2F8.x, this->unk_2FE.x, 4, 0x3E8, 1);
            Math_SmoothStepToS(&this->unk_2F8.z, this->unk_2FE.z, 4, 0x3E8, 1);
            this->unk_2F8.y = rot->y;
            rot->x = this->unk_2F8.x;
            rot->z = this->unk_2F8.z;
        }
    } else if ((limbIndex == UM_LIMB_WAGON_CART_COVER) && (this->flags & OBJ_UM_FLAG_PLAYING_MINIGAME)) {
        *dList = NULL;
    }

    return false;
}

void ObjUm_SpawnFragments(PlayState* play, Vec3f* potPos) {
    Vec3f sp8C = { 0.0f, -1.0f, 0.0f };
    Gfx* potFragments[] = { object_um_DL_000040, object_um_DL_000910, object_um_DL_0011E0 };
    s32 i;
    Vec3f sp70;

    EffectSsHitmark_SpawnFixedScale(play, EFFECT_HITMARK_WHITE, potPos);

    for (i = 0; i < 20; i++) {
        sp70.x = (Rand_ZeroOne() * 20.0f) - 10.0f;
        sp70.y = -((Rand_ZeroOne() * 20.0f) - 10.0f);
        sp70.z = (Rand_ZeroOne() * 20.0f) - 10.0f;
        EffectSsHahen_Spawn(play, potPos, &sp70, &sp8C, 1, 100, OBJECT_UM, 10,
                            potFragments[(s32)(Rand_ZeroOne() * ARRAY_COUNT(potFragments))]);
    }
}

void ObjUm_PostLimbDraw(PlayState* play2, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    PlayState* play = play2;
    ObjUm* this = (ObjUm*)thisx;
    GraphicsContext* gfxCtx = play2->state.gfxCtx;
    Gfx* spFC[] = {
        NULL, gUmBrokenMinigamePotDL, gUmMinigamePotDL, gUmMinigamePotDL, gUmMinigamePotDL, object_um_DL_0067C0
    };
    Gfx* spE4[] = { NULL, NULL, object_um_DL_004B60, object_um_DL_0043E0, NULL, NULL };
    f32 spCC[] = { 0.0f, 1070.0f, 1070.0f, 1070.0f, 1070.0f, 2100.0f };
    Vec3f spC0 = gZeroVec3f;
    Vec3f calcPotPos;
    f32 spB0;

    spC0.y += 1700.0f;

    if (limbIndex == UM_LIMB_WAGON_CART_BED) {
        Vec3f spA4 = { 2000.0f, 1070.0f, 0.0f };

        Matrix_MultVec3f(&spA4, &this->unk_308);
        this->flags |= OBJ_UM_FLAG_DRAWN_FLOOR;
    }

    if (limbIndex == UM_LIMB_WAGON_CART_BED) {
        Vec3f sp98 = { 2500.0f, 200.0f, 0.0f };

        Matrix_MultVec3f(&sp98, &this->cartBedPos);
    }

    if (limbIndex == UM_LIMB_WAGON_CART_BED) {
        Mtx* mtx;
        Vec3f sp88;
        Vec3s sp80;
        s32 i;
        f32 sp70[] = { 2000.0f, 0.0f, -2000.0f };

        sp80.x = 0;
        sp80.z = 0;
        sp88.x = 6800.0f;

        OPEN_DISPS(gfxCtx);

        for (i = 0; i < ARRAY_COUNT(this->potsLife); i++) {
            sp88.z = sp70[i];
            sp88.y = spCC[this->potsLife[i]];

            if (this->potsLife[i] == 5) {
                sp80.y = 0x4000;
            } else {
                sp80.y = -0x4000;
            }

            Matrix_Push();
            Matrix_TranslateRotateZYX(&sp88, &sp80);
            mtx = Matrix_Finalize(gfxCtx);
            Matrix_MultVec3f(&spC0, &calcPotPos);
            SkinMatrix_Vec3fMtxFMultXYZW(&play->viewProjectionMtxF, &calcPotPos, &this->potPos[i], &spB0);

            if (this->wasPotHit[i]) {
                this->wasPotHit[i] = false;
                if (this->potsLife[i] == 1) {
                    ObjUm_SpawnFragments(play, &calcPotPos);
                } else {
                    EffectSsHitmark_SpawnFixedScale(play, EFFECT_HITMARK_WHITE, &calcPotPos);
                }
            }
            Matrix_Pop();

            if (mtx == NULL) {
                //! @bug skips CLOSE_DISPS
                // 2S2H [Port] We need our close disps helpers called to prevent interpolation crashes
                CLOSE_DISPS_PORT_HELPERS(gfxCtx);
                return;
            }

            gSPMatrix(POLY_OPA_DISP++, mtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

            if (spFC[this->potsLife[i]] != NULL) {
                gSPDisplayList(POLY_OPA_DISP++, spFC[this->potsLife[i]]);

                if (spE4[this->potsLife[i]] != NULL) {
                    gSPDisplayList(POLY_OPA_DISP++, spE4[this->potsLife[i]]);
                }
            }
        }

        CLOSE_DISPS(gfxCtx);
    }

    if (limbIndex == UM_LIMB_CREMIA_HEAD) {
        Matrix_MultZero(&this->dyna.actor.focus.pos);
    }
}

void func_80B7BEA4(Vec3f* cartBedPos, s16 arg1, Vec3f* arg2, u8 alpha, PlayState* play) {
    f32 temp;
    Vec3f sp108;
    CollisionPoly* sp104;
    Mtx* sp100;
    MtxF spC0;
    MtxF sp80;
    MtxF sp40;

    sp108.x = cartBedPos->x;
    sp108.y = cartBedPos->y + 1.0f;
    sp108.z = cartBedPos->z;

    temp = BgCheck_EntityRaycastFloor2(play, &play->colCtx, &sp104, &sp108);
    if (sp104 != NULL) {
        func_800C0094(sp104, cartBedPos->x, temp, cartBedPos->z, &spC0);
    } else {
        SkinMatrix_SetTranslate(&spC0, cartBedPos->x, cartBedPos->y, cartBedPos->z);
    }

    SkinMatrix_MulYRotation(&spC0, arg1);
    SkinMatrix_SetScale(&sp80, arg2->x, 1.0f, arg2->z);
    SkinMatrix_MtxFMtxFMult(&spC0, &sp80, &sp40);

    sp100 = SkinMatrix_MtxFToNewMtx(play->state.gfxCtx, &sp40);
    if (sp100 != NULL) {
        OPEN_DISPS(play->state.gfxCtx);

        gSPMatrix(POLY_OPA_DISP++, &gIdentityMtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        POLY_OPA_DISP = Gfx_SetupDL(POLY_OPA_DISP, SETUPDL_44);
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 0, 0, 0, alpha);
        gSPMatrix(POLY_OPA_DISP++, sp100, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_OPA_DISP++, gSquareShadowDL);

        CLOSE_DISPS(play->state.gfxCtx);
    }
}

void ObjUm_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    ObjUm* this = (ObjUm*)thisx;
    Vec3f sp34;

    this->flags |= OBJ_UM_FLAG_0001;
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          ObjUm_OverrideLimbDraw, ObjUm_PostLimbDraw, &this->dyna.actor);
    sp34.x = 0.45f;
    sp34.y = 0.0f;
    sp34.z = 0.7f;
    func_80B7BEA4(&this->cartBedPos, this->dyna.actor.shape.rot.y, &sp34, 180, play);
    func_80B77770(this, play);
}
