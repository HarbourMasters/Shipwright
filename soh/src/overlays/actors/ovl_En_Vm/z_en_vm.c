/*
 * File: z_en_vm.c
 * Overlay: ovl_En_Vm
 * Description: Beamos
 */

#include "z_en_vm.h"
#include "objects/object_vm/object_vm.h"
#include "overlays/actors/ovl_En_Bom/z_en_bom.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_4)

void EnVm_Init(Actor* thisx, PlayState* play);
void EnVm_Destroy(Actor* thisx, PlayState* play);
void EnVm_Update(Actor* thisx, PlayState* play);
void EnVm_Draw(Actor* thisx, PlayState* play);

void EnVm_SetupWait(EnVm* this);
void EnVm_Wait(EnVm* this, PlayState* play);
void EnVm_SetupAttack(EnVm* this);
void EnVm_Attack(EnVm* this, PlayState* play);
void EnVm_Stun(EnVm* this, PlayState* play);
void EnVm_Die(EnVm* this, PlayState* play);

const ActorInit En_Vm_InitVars = {
    ACTOR_EN_VM,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_VM,
    sizeof(EnVm),
    (ActorFunc)EnVm_Init,
    (ActorFunc)EnVm_Destroy,
    (ActorFunc)EnVm_Update,
    (ActorFunc)EnVm_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_METAL,
        AT_NONE,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_ON,
    },
    { 25, 70, 0, { 0, 0, 0 } },
};

static ColliderQuadInit sQuadInit1 = {
    {
        COLTYPE_METAL,
        AT_ON | AT_TYPE_ENEMY,
        AC_NONE,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_QUAD,
    },
    {
        ELEMTYPE_UNK0,
        { 0xFFCFFFFF, 0x00, 0x10 },
        { 0x00000000, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL | TOUCH_UNK7,
        BUMP_NONE,
        OCELEM_NONE,
    },
    { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
};

static ColliderQuadInit sQuadInit2 = {
    {
        COLTYPE_METAL,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_QUAD,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_NONE,
    },
    { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
};

static Vec3f D_80B2EAEC = { 0.0f, 0.0f, 0.0f };

static Vec3f D_80B2EAF8 = { 0.0f, 0.0f, 0.0f };

static Vec3f D_80B2EB04 = { 500.0f, 0.0f, 0.0f };

static Vec3f D_80B2EB10 = { -500.0f, 0.0f, 0.0f };

static Vec3f D_80B2EB1C = { 0.0f, 0.0f, 0.0f };

static Vec3f D_80B2EB28 = { 0.0f, 0.0f, 1600.0f };

static Vec3f D_80B2EB34 = { 1000.0f, 700.0f, 2000.0f };

static Vec3f D_80B2EB40 = { 1000.0f, -700.0f, 2000.0f };

static Vec3f D_80B2EB4C = { -1000.0f, 700.0f, 1500.0f };

static Vec3f D_80B2EB58 = { -1000.0f, -700.0f, 1500.0f };

static Vec3f D_80B2EB64 = { 500.0f, 0.0f, 0.0f };

static Vec3f D_80B2EB70 = { -500.0f, 0.0f, 0.0f };

static Vec3f D_80B2EB7C = { 0.4f, 0.4f, 0.4f };

static void* D_80B2EB88[] = {
    gEffEnemyDeathFlame1Tex, gEffEnemyDeathFlame2Tex,  gEffEnemyDeathFlame3Tex, gEffEnemyDeathFlame4Tex,
    gEffEnemyDeathFlame5Tex, gEffEnemyDeathFlame6Tex,  gEffEnemyDeathFlame7Tex, gEffEnemyDeathFlame8Tex,
    gEffEnemyDeathFlame9Tex, gEffEnemyDeathFlame10Tex,
};

void EnVm_SetupAction(EnVm* this, EnVmActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void EnVm_Init(Actor* thisx, PlayState* play) {
    EnVm* this = (EnVm*)thisx;

    SkelAnime_Init(play, &this->skelAnime, &gBeamosSkel, &gBeamosAnim, this->jointTable, this->morphTable, 11);
    ActorShape_Init(&thisx->shape, 0.0f, NULL, 0.0f);
    Collider_InitCylinder(play, &this->colliderCylinder);
    Collider_SetCylinder(play, &this->colliderCylinder, thisx, &sCylinderInit);
    Collider_InitQuad(play, &this->colliderQuad1);
    Collider_SetQuad(play, &this->colliderQuad1, thisx, &sQuadInit1);
    Collider_InitQuad(play, &this->colliderQuad2);
    Collider_SetQuad(play, &this->colliderQuad2, thisx, &sQuadInit2);
    this->beamSightRange = (thisx->params >> 8) * 40.0f;
    thisx->params &= 0xFF;
    thisx->naviEnemyId = 0x39;

    if (thisx->params == BEAMOS_LARGE) {
        thisx->colChkInfo.health = 2;
        Actor_SetScale(thisx, 0.014f);
    } else {
        thisx->colChkInfo.health = 1;
        Actor_SetScale(thisx, 0.01f);
    }

    EnVm_SetupWait(this);
}

void EnVm_Destroy(Actor* thisx, PlayState* play) {
    EnVm* this = (EnVm*)thisx;

    Collider_DestroyCylinder(play, &this->colliderCylinder);
}

void EnVm_SetupWait(EnVm* this) {
    f32 frameCount = Animation_GetLastFrame(&gBeamosAnim);

    Animation_Change(&this->skelAnime, &gBeamosAnim, 1.0f, frameCount, frameCount, ANIMMODE_ONCE, 0.0f);
    this->unk_25E = this->unk_260 = 0;
    this->unk_21C = 0;
    this->timer = 10;
    EnVm_SetupAction(this, EnVm_Wait);
}

void EnVm_Wait(EnVm* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 dist;
    s16 headRot;
    s16 pad;
    s16 pitch;

    switch (this->unk_25E) {
        case 0:
            Math_SmoothStepToS(&this->beamRot.x, 0, 10, 1500, 0);
            headRot = this->actor.yawTowardsPlayer - this->headRotY - this->actor.shape.rot.y;
            pitch = Math_Vec3f_Pitch(&this->beamPos1, &player->actor.world.pos);

            if (pitch > 0x1B91) {
                pitch = 0x1B91;
            }

            dist = this->beamSightRange - this->actor.xzDistToPlayer;

            if (this->actor.xzDistToPlayer <= this->beamSightRange && ABS(headRot) <= 0x2710 && pitch >= 0xE38 &&
                this->actor.yDistToPlayer <= 80.0f && this->actor.yDistToPlayer >= -160.0f) {
                Math_SmoothStepToS(&this->beamRot.x, pitch, 10, 0xFA0, 0);
                if (Math_SmoothStepToS(&this->headRotY, this->actor.yawTowardsPlayer - this->actor.shape.rot.y, 1,
                                       (ABS((s16)(dist * 180.0f)) / 3) + 0xFA0, 0) <= 5460) {
                    this->timer--;
                    if (this->timer == 0) {
                        this->unk_25E++;
                        this->skelAnime.curFrame = 0.0f;
                        this->skelAnime.startFrame = 0.0f;
                        this->skelAnime.playSpeed = 2.0f;
                        Audio_PlayActorSound2(&this->actor, NA_SE_EN_BIMOS_AIM);
                    }
                }
            } else {
                this->headRotY -= 0x1F4;
            }

            SkelAnime_Update(&this->skelAnime);
            return;
        case 1:
            break;
        default:
            return;
    }

    Math_SmoothStepToS(&this->headRotY, this->actor.yawTowardsPlayer - this->actor.shape.rot.y, 1, 0x1F40, 0);

    if (SkelAnime_Update(&this->skelAnime)) {
        this->unk_260++;
        this->skelAnime.curFrame = 0.0f;
    }

    if (this->unk_260 == 2) {
        this->beamRot.y = this->actor.yawTowardsPlayer;
        this->beamRot.x = Math_Vec3f_Pitch(&this->beamPos1, &player->actor.world.pos);

        if (this->beamRot.x > 0x1B91) {
            this->beamRot.x = 0x1B91;
        }

        if (this->beamRot.x < 0xAAA) {
            this->skelAnime.startFrame = this->skelAnime.curFrame = this->skelAnime.endFrame;
            this->unk_25E = this->unk_260 = 0;
            this->timer = 10;
            this->skelAnime.playSpeed = 1.0f;
        } else {
            this->skelAnime.curFrame = 6.0f;
            EffectSsDeadDd_Spawn(play, &this->beamPos2, &D_80B2EAEC, &D_80B2EAEC, 150, -25, 0, 0, 255, 0, 255, 255,
                                 255, 16, 20);
            EnVm_SetupAttack(this);
        }
    }
}

void EnVm_SetupAttack(EnVm* this) {
    Animation_Change(&this->skelAnime, &gBeamosAnim, 3.0f, 3.0f, 7.0f, ANIMMODE_ONCE, 0.0f);
    this->timer = 305;
    this->beamScale.x = 0.6f;
    this->beamSpeed = 40.0f;
    this->unk_21C = 1;
    this->colliderQuad1.base.atFlags &= ~AT_HIT;
    EnVm_SetupAction(this, EnVm_Attack);
}

void EnVm_Attack(EnVm* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 pitch = Math_Vec3f_Pitch(&this->beamPos1, &player->actor.world.pos);
    f32 dist;
    Vec3f playerPos;

    if (pitch > 0x1B91) {
        pitch = 0x1B91;
    }

    if (this->colliderQuad1.base.atFlags & AT_HIT) {
        this->colliderQuad1.base.atFlags &= ~AT_HIT;
        this->timer = 0;

        if (this->beamScale.x > 0.1f) {
            this->beamScale.x = 0.1f;
        }
    }

    if (this->beamRot.x < 0xAAA || this->timer == 0) {
        Math_SmoothStepToF(&this->beamScale.x, 0.0f, 1.0f, 0.03f, 0.0f);
        this->unk_260 = 0;

        if (this->beamScale.x == 0.0f) {
            this->beamScale.y = this->beamScale.z = 0.0f;
            EnVm_SetupWait(this);
        }
    } else {
        if (--this->timer > 300) {
            return;
        }

        Math_SmoothStepToS(&this->headRotY, -this->actor.shape.rot.y + this->actor.yawTowardsPlayer, 10, 0xDAC, 0);
        Math_SmoothStepToS(&this->beamRot.y, this->actor.yawTowardsPlayer, 10, 0xDAC, 0);
        Math_SmoothStepToS(&this->beamRot.x, pitch, 10, 0xDAC, 0);
        playerPos = player->actor.world.pos;

        if (player->actor.floorHeight > BGCHECK_Y_MIN) {
            playerPos.y = player->actor.floorHeight;
        }

        dist = Math_Vec3f_DistXYZ(&this->beamPos1, &playerPos);
        Math_SmoothStepToF(&this->beamScale.z, dist, 1.0f, this->beamSpeed, 0.0f);
        Math_SmoothStepToF(&this->beamScale.x, 0.1f, 1.0f, 0.12f, 0.0f);
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_BIMOS_LAZER - SFX_FLAG);

        if (this->unk_260 > 2) {
            CollisionCheck_SetAT(play, &play->colChkCtx, &this->colliderQuad1.base);
        }

        this->unk_260 = 3;
    }

    if (SkelAnime_Update(&this->skelAnime)) {
        this->skelAnime.curFrame = this->skelAnime.startFrame;
    }
}

void EnVm_SetupStun(EnVm* this) {
    Animation_Change(&this->skelAnime, &gBeamosAnim, -1.0f, Animation_GetLastFrame(&gBeamosAnim), 0.0f, ANIMMODE_ONCE,
                     0.0f);
    this->unk_260 = 0;
    this->timer = 180;
    this->unk_25E = this->unk_260;
    this->unk_21C = 2;
    this->beamScale.z = 0.0f;
    this->beamScale.y = 0.0f;
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_GOMA_JR_FREEZE);
    EnVm_SetupAction(this, EnVm_Stun);
}

void EnVm_Stun(EnVm* this, PlayState* play) {
    if (this->timer == 0) {
        if (SkelAnime_Update(&this->skelAnime)) {
            this->unk_25E++;
            if (this->unk_25E == 3) {
                EnVm_SetupWait(this);
            } else if (this->unk_25E == 1) {
                Animation_Change(&this->skelAnime, &gBeamosAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gBeamosAnim),
                                 ANIMMODE_ONCE, 0.0f);
            } else {
                this->timer = 10;
                this->skelAnime.curFrame = 0.0f;
                this->skelAnime.playSpeed = 2.0f;
            }
        }
    } else {
        Math_SmoothStepToS(&this->beamRot.x, 0, 10, 0x5DC, 0);
        this->timer--;
        SkelAnime_Update(&this->skelAnime);
    }
}

void EnVm_SetupDie(EnVm* this) {
    Animation_Change(&this->skelAnime, &gBeamosAnim, -1.0f, Animation_GetLastFrame(&gBeamosAnim), 0.0f, ANIMMODE_ONCE,
                     0.0f);
    this->timer = 33;
    this->unk_25E = this->unk_260 = 0;
    this->unk_21C = 3;
    this->beamScale.z = 0.0f;
    this->beamScale.y = 0.0f;
    this->actor.shape.yOffset = -5000.0f;
    this->actor.world.pos.y += 5000.0f * this->actor.scale.y;
    this->actor.velocity.y = 8.0f;
    this->actor.gravity = -0.5f;
    this->actor.speedXZ = Rand_ZeroOne() + 1.0f;
    this->actor.world.rot.y = Rand_CenteredFloat(65535.0f);
    EnVm_SetupAction(this, EnVm_Die);
    gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_BEAMOS]++;
}

void EnVm_Die(EnVm* this, PlayState* play) {
    EnBom* bomb;

    this->beamRot.x += 0x5DC;
    this->headRotY += 0x9C4;
    Actor_MoveForward(&this->actor);

    if (--this->timer == 0) {
        bomb = (EnBom*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_BOM, this->actor.world.pos.x,
                                   this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0x6FF, BOMB_BODY);

        if (bomb != NULL) {
            bomb->timer = 0;
        }

        Item_DropCollectibleRandom(play, &this->actor, &this->actor.world.pos, 0xA0);
        Actor_Kill(&this->actor);
    }
}

void EnVm_CheckHealth(EnVm* this, PlayState* play) {
    EnBom* bomb;

    if (Actor_GetCollidedExplosive(play, &this->colliderCylinder.base) != NULL) {
        this->actor.colChkInfo.health--;
        osSyncPrintf("hp down %d\n", this->actor.colChkInfo.health);
    } else {
        if (!(this->colliderQuad2.base.acFlags & AC_HIT) || this->unk_21C == 2) {
            return;
        }
        this->colliderQuad2.base.acFlags &= ~AC_HIT;
    }

    if (this->actor.colChkInfo.health != 0) {
        Actor_SetColorFilter(&this->actor, 0x4000, 0xFF, 0, 8);
        EnVm_SetupStun(this);
    } else {
        bomb = (EnBom*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_BOM, this->actor.world.pos.x,
                                   this->actor.world.pos.y + 20.0f, this->actor.world.pos.z, 0, 0, 0x601, BOMB_BODY);

        if (bomb != NULL) {
            bomb->timer = 0;
        }

        EnVm_SetupDie(this);
    }
}

void EnVm_Update(Actor* thisx, PlayState* play) {
    EnVm* this = (EnVm*)thisx;
    CollisionCheckContext* colChkCtx = &play->colChkCtx;

    if (this->actor.colChkInfo.health != 0) {
        EnVm_CheckHealth(this, play);
    }

    if (this->unk_260 == 4) {
        EffectSsDeadDs_SpawnStationary(play, &this->beamPos3, 20, -1, 255, 20);
        func_80033480(play, &this->beamPos3, 6.0f, 1, 120, 20, 1);
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_BIMOS_LAZER_GND - SFX_FLAG);
    }

    this->actionFunc(this, play);
    this->beamTexScroll += 0xC;

    if (this->actor.colChkInfo.health != 0 && this->unk_21C != 2) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_BIMOS_ROLL_HEAD - SFX_FLAG);
    }

    Collider_UpdateCylinder(&this->actor, &this->colliderCylinder);
    CollisionCheck_SetOC(play, colChkCtx, &this->colliderCylinder.base);

    if (this->actor.colorFilterTimer == 0 && this->actor.colChkInfo.health != 0) {
        CollisionCheck_SetAC(play, colChkCtx, &this->colliderCylinder.base);
    }

    CollisionCheck_SetAC(play, colChkCtx, &this->colliderQuad2.base);
    this->actor.focus.pos = this->actor.world.pos;
    this->actor.focus.pos.y += (6500.0f + this->actor.shape.yOffset) * this->actor.scale.y;
}

s32 EnVm_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnVm* this = (EnVm*)thisx;

    if (limbIndex == 2) {
        rot->x += this->beamRot.x;
        rot->y += this->headRotY;
    } else if (limbIndex == 10) {
        if (this->unk_21C == 3) {
            *dList = NULL;
        }
    }

    return false;
}

void EnVm_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    EnVm* this = (EnVm*)thisx;
    Vec3f sp80 = D_80B2EAF8;
    Vec3f sp74 = D_80B2EB04;
    Vec3f sp68 = D_80B2EB10;
    s32 pad;
    Vec3f posResult;
    CollisionPoly* poly;
    s32 bgId;
    f32 dist;

    if (limbIndex == 2) {
        Matrix_MultVec3f(&D_80B2EB1C, &this->beamPos1);
        Matrix_MultVec3f(&D_80B2EB28, &this->beamPos2);

        if (this->unk_260 >= 3) {
            poly = NULL;
            sp80.z = (this->beamScale.z + 500.0f) * (this->actor.scale.y * 10000.0f);
            Matrix_MultVec3f(&sp80, &this->beamPos3);

            if (BgCheck_EntityLineTest1(&play->colCtx, &this->beamPos1, &this->beamPos3, &posResult, &poly, true,
                                        true, false, true, &bgId) == true) {
                this->beamScale.z = Math_Vec3f_DistXYZ(&this->beamPos1, &posResult) - 5.0f;
                this->unk_260 = 4;
                this->beamPos3 = posResult;
            }
            if (this->beamScale.z != 0.0f) {
                dist = 100.0f;
                if (this->actor.scale.y > 0.01f) {
                    dist = 70.0f;
                }
                sp74.z = sp68.z = Math_Vec3f_DistXYZ(&this->beamPos1, &this->beamPos3) * dist;
                Matrix_MultVec3f(&D_80B2EB64, &this->colliderQuad1.dim.quad[3]);
                Matrix_MultVec3f(&D_80B2EB70, &this->colliderQuad1.dim.quad[2]);
                Matrix_MultVec3f(&sp74, &this->colliderQuad1.dim.quad[1]);
                Matrix_MultVec3f(&sp68, &this->colliderQuad1.dim.quad[0]);
                Collider_SetQuadVertices(&this->colliderQuad1, &this->colliderQuad1.dim.quad[0],
                                         &this->colliderQuad1.dim.quad[1], &this->colliderQuad1.dim.quad[2],
                                         &this->colliderQuad1.dim.quad[3]);
            }
        }
        Matrix_MultVec3f(&D_80B2EB34, &this->colliderQuad2.dim.quad[1]);
        Matrix_MultVec3f(&D_80B2EB40, &this->colliderQuad2.dim.quad[0]);
        Matrix_MultVec3f(&D_80B2EB4C, &this->colliderQuad2.dim.quad[3]);
        Matrix_MultVec3f(&D_80B2EB58, &this->colliderQuad2.dim.quad[2]);
        Collider_SetQuadVertices(&this->colliderQuad2, &this->colliderQuad2.dim.quad[0],
                                 &this->colliderQuad2.dim.quad[1], &this->colliderQuad2.dim.quad[2],
                                 &this->colliderQuad2.dim.quad[3]);
    }
}

void EnVm_Draw(Actor* thisx, PlayState* play2) {
    EnVm* this = (EnVm*)thisx;
    PlayState* play = play2;
    Vec3f actorPos;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Opa(play->state.gfxCtx);
    Gfx_SetupDL_25Xlu(play->state.gfxCtx);
    SkelAnime_DrawOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, EnVm_OverrideLimbDraw,
                      EnVm_PostLimbDraw, this);
    actorPos = this->actor.world.pos;
    func_80033C30(&actorPos, &D_80B2EB7C, 255, play);

    if (this->unk_260 >= 3) {
        Matrix_Translate(this->beamPos3.x, this->beamPos3.y + 10.0f, this->beamPos3.z, MTXMODE_NEW);
        Matrix_Scale(0.8f, 0.8f, 0.8f, MTXMODE_APPLY);
        gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, 168);
        Gfx_SetupDL_60NoCDXlu(play->state.gfxCtx);
        gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 255, 0);
        gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(D_80B2EB88[play->gameplayFrames % 8]));
        gSPDisplayList(POLY_XLU_DISP++, gEffEnemyDeathFlameDL);
        Matrix_RotateY(32767.0f, MTXMODE_APPLY);
        gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(D_80B2EB88[(play->gameplayFrames + 4) % 8]));
        gSPDisplayList(POLY_XLU_DISP++, gEffEnemyDeathFlameDL);
    }
    gSPSegment(POLY_OPA_DISP++, 0x08, func_80094E78(play->state.gfxCtx, 0, this->beamTexScroll));
    Matrix_Translate(this->beamPos1.x, this->beamPos1.y, this->beamPos1.z, MTXMODE_NEW);
    Matrix_RotateZYX(this->beamRot.x, this->beamRot.y, this->beamRot.z, MTXMODE_APPLY);
    Matrix_Scale(this->beamScale.x * 0.1f, this->beamScale.x * 0.1f, this->beamScale.z * 0.0015f, MTXMODE_APPLY);
    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, gBeamosLaserDL);

    CLOSE_DISPS(play->state.gfxCtx);
}
