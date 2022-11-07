/*
 * File: z_en_dha.c
 * Overlay: ovl_En_Dha
 * Description: Dead Hand's Hand
 */

#include "z_en_dha.h"
#include "overlays/actors/ovl_En_Dh/z_en_dh.h"
#include "objects/object_dh/object_dh.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_4)

void EnDha_Init(Actor* thisx, PlayState* play);
void EnDha_Destroy(Actor* thisx, PlayState* play);
void EnDha_Update(Actor* thisx, PlayState* play);
void EnDha_Draw(Actor* thisx, PlayState* play);

void EnDha_SetupWait(EnDha* this);
void EnDha_Wait(EnDha* this, PlayState* play);
void EnDha_SetupTakeDamage(EnDha* this);
void EnDha_TakeDamage(EnDha* this, PlayState* play);
void EnDha_SetupDeath(EnDha* this);
void EnDha_Die(EnDha* this, PlayState* play);
void EnDha_UpdateHealth(EnDha* this, PlayState* play);

const ActorInit En_Dha_InitVars = {
    ACTOR_EN_DHA,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_DH,
    sizeof(EnDha),
    (ActorFunc)EnDha_Init,
    (ActorFunc)EnDha_Destroy,
    (ActorFunc)EnDha_Update,
    (ActorFunc)EnDha_Draw,
    NULL,
};

static DamageTable sDamageTable = {
    /* Deku nut      */ DMG_ENTRY(0, 0x0),
    /* Deku stick    */ DMG_ENTRY(2, 0xF),
    /* Slingshot     */ DMG_ENTRY(0, 0x0),
    /* Explosive     */ DMG_ENTRY(0, 0x0),
    /* Boomerang     */ DMG_ENTRY(0, 0x0),
    /* Normal arrow  */ DMG_ENTRY(0, 0x0),
    /* Hammer swing  */ DMG_ENTRY(0, 0x0),
    /* Hookshot      */ DMG_ENTRY(0, 0x0),
    /* Kokiri sword  */ DMG_ENTRY(2, 0xF),
    /* Master sword  */ DMG_ENTRY(2, 0xF),
    /* Giant's Knife */ DMG_ENTRY(4, 0xF),
    /* Fire arrow    */ DMG_ENTRY(0, 0x0),
    /* Ice arrow     */ DMG_ENTRY(0, 0x0),
    /* Light arrow   */ DMG_ENTRY(0, 0x0),
    /* Unk arrow 1   */ DMG_ENTRY(0, 0x0),
    /* Unk arrow 2   */ DMG_ENTRY(0, 0x0),
    /* Unk arrow 3   */ DMG_ENTRY(0, 0x0),
    /* Fire magic    */ DMG_ENTRY(0, 0x0),
    /* Ice magic     */ DMG_ENTRY(0, 0x0),
    /* Light magic   */ DMG_ENTRY(0, 0x0),
    /* Shield        */ DMG_ENTRY(0, 0x0),
    /* Mirror Ray    */ DMG_ENTRY(0, 0x0),
    /* Kokiri spin   */ DMG_ENTRY(2, 0xF),
    /* Giant spin    */ DMG_ENTRY(4, 0xF),
    /* Master spin   */ DMG_ENTRY(2, 0xF),
    /* Kokiri jump   */ DMG_ENTRY(4, 0xF),
    /* Giant jump    */ DMG_ENTRY(8, 0xF),
    /* Master jump   */ DMG_ENTRY(4, 0xF),
    /* Unknown 1     */ DMG_ENTRY(0, 0x0),
    /* Unblockable   */ DMG_ENTRY(0, 0x0),
    /* Hammer jump   */ DMG_ENTRY(4, 0xF),
    /* Unknown 2     */ DMG_ENTRY(0, 0x0),
};

static ColliderJntSphElementInit sJntSphElementsInit[] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON,
            OCELEM_NONE,
        },
        { 1, { { 0, 0, 0 }, 12 }, 100 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON,
            OCELEM_ON,
        },
        { 2, { { 3200, 0, 0 }, 10 }, 100 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON,
            OCELEM_ON,
        },
        { 3, { { 1200, 0, 0 }, 10 }, 100 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON,
            OCELEM_NONE,
        },
        { 4, { { 2700, 0, 0 }, 10 }, 100 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON,
            OCELEM_ON,
        },
        { 5, { { 1200, 0, 0 }, 10 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COLTYPE_HIT6,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER | OC1_TYPE_1,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    5,
    sJntSphElementsInit,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_S8(naviEnemyId, 0x2E, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 2000, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 10, ICHAIN_STOP),
};

void EnDha_SetupAction(EnDha* this, EnDhaActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void EnDha_Init(Actor* thisx, PlayState* play) {
    EnDha* this = (EnDha*)thisx;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    this->actor.colChkInfo.damageTable = &sDamageTable;
    SkelAnime_InitFlex(play, &this->skelAnime, &object_dh_Skel_000BD8, &object_dh_Anim_0015B0, this->jointTable,
                       this->morphTable, 4);
    ActorShape_Init(&this->actor.shape, 0, ActorShadow_DrawFeet, 90.0f);
    this->actor.focus.pos = this->actor.world.pos;
    this->actor.focus.pos.y += 50.0f;
    this->actor.colChkInfo.mass = MASS_HEAVY;
    this->actor.colChkInfo.health = 8;
    this->limbAngleX[0] = -0x4000;
    Collider_InitJntSph(play, &this->collider);
    Collider_SetJntSph(play, &this->collider, &this->actor, &sJntSphInit, this->colliderItem);
    this->actor.flags &= ~ACTOR_FLAG_0;

    EnDha_SetupWait(this);
}

void EnDha_Destroy(Actor* thisx, PlayState* play) {
    s32 pad;
    EnDha* this = (EnDha*)thisx;

    Collider_DestroyJntSph(play, &this->collider);
}

void EnDha_SetupWait(EnDha* this) {
    Animation_PlayLoop(&this->skelAnime, &object_dh_Anim_0015B0);
    this->unk_1C0 = 0;
    this->actionTimer = ((Rand_ZeroOne() * 10.0f) + 5.0f);
    this->actor.speedXZ = 0.0f;
    this->actor.world.rot.y = this->actor.shape.rot.y;
    this->actor.home.rot.z = 1;
    EnDha_SetupAction(this, EnDha_Wait);
}

void EnDha_Wait(EnDha* this, PlayState* play) {
    Vec3f zeroVec = { 0.0f, 0.0f, 0.0f }; // unused
    Vec3f armPosMultiplier1 = { 0.0f, 0.0f, 55.0f };
    Vec3f armPosMultiplier2 = { 0.0f, 0.0f, -54.0f };
    Player* player = GET_PLAYER(play);
    s32 pad;
    s32 pad2;
    Vec3f playerPos = player->actor.world.pos;
    Vec3s angle;
    s16 yaw;

    playerPos.x += Math_SinS(player->actor.shape.rot.y) * -5.0f;
    playerPos.z += Math_CosS(player->actor.shape.rot.y) * -5.0f;

    if (!LINK_IS_ADULT) {
        playerPos.y += 38.0f;
    } else {
        playerPos.y += 56.0f;
    }

    if (this->actor.xzDistToPlayer <= 100.0f) {
        this->handAngle.y = this->handAngle.x = this->limbAngleY = 0;

        if (Math_Vec3f_DistXYZ(&playerPos, &this->handPos[0]) <= 12.0f) {
            if (this->unk_1CC == 0) {
                if (play->grabPlayer(play, player)) {
                    this->timer = 0;
                    this->unk_1CC++;

                    if (this->actor.parent != NULL) {
                        this->actor.parent->params = ENDH_START_ATTACK_GRAB;
                    }

                    Audio_PlayActorSound2(&this->actor, NA_SE_EN_DEADHAND_GRIP);
                }
            } else {
                this->timer += 0x1194;
                this->limbAngleY = Math_SinS(this->timer) * 1820.0f;

                if (!(player->stateFlags2 & 0x80)) {
                    this->unk_1CC = 0;
                    EnDha_SetupTakeDamage(this);
                    return;
                }

                if (this->timer < -0x6E6B) {
                    Audio_PlayActorSound2(&this->actor, NA_SE_EN_DEADHAND_GRIP);
                }
            }

            func_80035844(&this->handPos[1], &playerPos, &this->handAngle, 0);
            this->handAngle.y -= this->actor.shape.rot.y + this->limbAngleY;
            this->handAngle.x -= this->actor.shape.rot.x + this->limbAngleX[0] + this->limbAngleX[1];
        } else {
            if ((player->stateFlags2 & 0x80) && (&this->actor == player->actor.parent)) {
                player->stateFlags2 &= ~0x80;
                player->actor.parent = NULL;
                player->unk_850 = 200;
            }

            if (this->actor.home.rot.z != 0) {
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_DEADHAND_HAND_AT);
                this->actor.home.rot.z = 0;
            }
        }

        this->actor.shape.rot.y = Math_Vec3f_Yaw(&this->actor.world.pos, &playerPos);

        Math_SmoothStepToF(&this->handPos[0].x, playerPos.x, 1.0f, 16.0f, 0.0f);
        Math_SmoothStepToF(&this->handPos[0].y, playerPos.y, 1.0f, 16.0f, 0.0f);
        Math_SmoothStepToF(&this->handPos[0].z, playerPos.z, 1.0f, 16.0f, 0.0f);

        func_80035844(&this->armPos, &this->handPos[0], &angle, 0);
        Matrix_Translate(this->handPos[0].x, this->handPos[0].y, this->handPos[0].z, MTXMODE_NEW);
        Matrix_RotateZYX(angle.x, angle.y, 0, MTXMODE_APPLY);
        Matrix_MultVec3f(&armPosMultiplier2, &this->armPos);
        Matrix_Translate(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z, MTXMODE_NEW);
        func_80035844(&this->actor.world.pos, &this->armPos, &angle, 0);
        Matrix_RotateZYX(angle.x, angle.y, 0, MTXMODE_APPLY);
        Matrix_MultVec3f(&armPosMultiplier1, &this->armPos);
        this->limbAngleX[0] = Math_Vec3f_Pitch(&this->actor.world.pos, &this->armPos);
        yaw = Math_Vec3f_Yaw(&this->actor.world.pos, &this->armPos) - this->actor.shape.rot.y;

        if (ABS(yaw) >= 0x4000) {
            this->limbAngleX[0] = -0x8000 - this->limbAngleX[0];
        }

        this->limbAngleX[1] = (Math_Vec3f_Pitch(&this->armPos, &this->handPos[0]) - this->limbAngleX[0]);

        if (this->limbAngleX[1] < 0) {
            this->limbAngleX[0] += this->limbAngleX[1] * 2;
            this->limbAngleX[1] *= -2;
        }
    } else {
        if ((player->stateFlags2 & 0x80) && (&this->actor == player->actor.parent)) {
            player->stateFlags2 &= ~0x80;
            player->actor.parent = NULL;
            player->unk_850 = 200;
        }

        this->actor.home.rot.z = 1;
        Math_SmoothStepToS(&this->limbAngleX[1], 0, 1, 0x3E8, 0);
        Math_SmoothStepToS(&this->limbAngleX[0], -0x4000, 1, 0x3E8, 0);
        SkelAnime_Update(&this->skelAnime);
    }
}

void EnDha_SetupTakeDamage(EnDha* this) {
    this->actionTimer = 15;
    EnDha_SetupAction(this, EnDha_TakeDamage);
}

void EnDha_TakeDamage(EnDha* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if ((player->stateFlags2 & 0x80) && (&this->actor == player->actor.parent)) {
        player->stateFlags2 &= ~0x80;
        player->actor.parent = NULL;
        player->unk_850 = 200;
    }

    Math_SmoothStepToS(&this->limbAngleX[1], 0, 1, 2000, 0);
    Math_SmoothStepToS(&this->limbAngleY, 0, 1, 600, 0);
    Math_SmoothStepToS(&this->limbAngleX[0], -0x4000, 1, 2000, 0);
    SkelAnime_Update(&this->skelAnime);
    this->actionTimer--;

    if (this->actionTimer == 0) {
        EnDha_SetupWait(this);
    }
}

void EnDha_SetupDeath(EnDha* this) {
    this->unk_1C0 = 8;
    this->actionTimer = 300;

    if (this->actor.parent != NULL) {
        if (this->actor.parent->params != ENDH_DEATH) {
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_DEADHAND_HAND_DEAD);
        }
        if (this->actor.parent->params <= ENDH_WAIT_UNDERGROUND) {
            this->actor.parent->params--;
        }
    }

    EnDha_SetupAction(this, EnDha_Die);
}

void EnDha_Die(EnDha* this, PlayState* play) {
    s16 angle;
    Vec3f vec;
    Player* player = GET_PLAYER(play);

    if ((player->stateFlags2 & 0x80) && (&this->actor == player->actor.parent)) {
        player->stateFlags2 &= ~0x80;
        player->actor.parent = NULL;
        player->unk_850 = 200;
    }

    Math_SmoothStepToS(&this->limbAngleX[1], 0, 1, 0x7D0, 0);
    angle = Math_SmoothStepToS(&this->limbAngleX[0], -0x4000, 1, 0x7D0, 0);
    SkelAnime_Update(&this->skelAnime);

    if (angle == 0) {
        vec = this->actor.world.pos;

        if (this->actionTimer != 0) {
            if (-12000.0f < this->actor.shape.yOffset) {
                this->actor.shape.yOffset -= 1000.0f;
                func_80033480(play, &vec, 7.0f, 1, 0x5A, 0x14, 1);
            } else {
                this->actionTimer--;

                if ((this->actor.parent != NULL) && (this->actor.parent->params == ENDH_DEATH)) {
                    Actor_Kill(&this->actor);
                }
            }
        } else {
            this->actor.shape.yOffset += 500.0f;
            func_80033480(play, &vec, 7.0f, 1, 0x5A, 0x14, 1);

            if (this->actor.shape.yOffset == 0.0f) {
                EnDha_SetupWait(this);
            }
        }
    }
}

void EnDha_UpdateHealth(EnDha* this, PlayState* play) {
    if (!((this->unk_1C0 >= 8) || !(this->collider.base.acFlags & AC_HIT))) {
        this->collider.base.acFlags &= ~AC_HIT;

        if (this->actor.colChkInfo.damageEffect == 0 || this->actor.colChkInfo.damageEffect == 6) {
            return;
        } else {
            Actor_SetColorFilter(&this->actor, 0x4000, 0xFF, 0, 8);
            if (Actor_ApplyDamage(&this->actor) == 0) {
                EnDha_SetupDeath(this);
                this->actor.colChkInfo.health = 8;
                Item_DropCollectibleRandom(play, &this->actor, &this->actor.world.pos, 0xE0);
            } else {
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_DEADHAND_DAMAGE);
                this->unk_1C0 = 9;
                EnDha_SetupTakeDamage(this);
            }
        }
    }

    if ((this->actor.parent != NULL) && (this->actor.parent->params == ENDH_DEATH)) {
        EnDha_SetupDeath(this);
    }
}

void EnDha_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnDha* this = (EnDha*)thisx;

    if (this->actor.parent == NULL) {
        this->actor.parent = Actor_FindNearby(play, &this->actor, ACTOR_EN_DH, ACTORCAT_ENEMY, 10000.0f);
    }

    EnDha_UpdateHealth(this, play);
    this->actionFunc(this, play);
    CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
}

s32 EnDha_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnDha* this = (EnDha*)thisx;

    if (limbIndex == 1) {
        rot->y = -(s16)(this->limbAngleX[0] + 0x4000);
        rot->z += this->limbAngleY;
    } else if (limbIndex == 2) {
        rot->z = this->limbAngleX[1];
        rot->y -= this->limbAngleY;
    } else if (limbIndex == 3) {
        rot->y = -this->handAngle.y;
        rot->z = -this->handAngle.x;
    }

    return false;
}

void EnDha_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    Vec3f handVec = { 1100.0f, 0.0f, 0.0f };
    Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };
    EnDha* this = (EnDha*)thisx;

    switch (limbIndex) {
        case 1:
            Collider_UpdateSpheres(2, &this->collider);
            Collider_UpdateSpheres(3, &this->collider);
            break;
        case 2:
            Collider_UpdateSpheres(4, &this->collider);
            Collider_UpdateSpheres(5, &this->collider);
            Matrix_MultVec3f(&zeroVec, &this->armPos);
            break;
        case 3:
            Collider_UpdateSpheres(1, &this->collider);
            Matrix_MultVec3f(&handVec, &this->handPos[0]);
            Matrix_MultVec3f(&zeroVec, &this->handPos[1]);
            break;
    }
}

void EnDha_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    EnDha* this = (EnDha*)thisx;

    func_80093D18(play->state.gfxCtx);
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnDha_OverrideLimbDraw, EnDha_PostLimbDraw, this);
}
