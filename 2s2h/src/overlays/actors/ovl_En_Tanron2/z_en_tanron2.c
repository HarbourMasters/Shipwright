/*
 * File: z_en_tanron2.c
 * Overlay: ovl_En_Tanron2
 * Description: Wart's Bubbles
 */

#include "prevent_bss_reordering.h"
#include "z_en_tanron2.h"
#include "overlays/actors/ovl_Boss_04/z_boss_04.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/object_boss04/object_boss04.h"
#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"

#define FLAGS                                                                                 \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_DRAW_CULLING_DISABLED)

void EnTanron2_Init(Actor* thisx, PlayState* play);
void EnTanron2_Destroy(Actor* thisx, PlayState* play);
void EnTanron2_Update(Actor* thisx, PlayState* play);
void EnTanron2_Draw(Actor* thisx, PlayState* play2);

void func_80BB69C0(EnTanron2* this);
void func_80BB69FC(EnTanron2* this, PlayState* play);
void func_80BB6BD8(EnTanron2* this, PlayState* play);
void func_80BB6F64(EnTanron2* this);
void func_80BB6F78(EnTanron2* this, PlayState* play);
void func_80BB7408(EnTanron2* this, PlayState* play);
void func_80BB7B90(Actor* thisx, PlayState* play);

Boss04* D_80BB8450;
f32 D_80BB8454;
EnTanron2* D_80BB8458[82];

ActorProfile En_Tanron2_Profile = {
    /**/ ACTOR_EN_TANRON2,
    /**/ ACTORCAT_BOSS,
    /**/ FLAGS,
    /**/ OBJECT_BOSS04,
    /**/ sizeof(EnTanron2),
    /**/ EnTanron2_Init,
    /**/ EnTanron2_Destroy,
    /**/ EnTanron2_Update,
    /**/ EnTanron2_Draw,
};

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(1, 0xF),
    /* Deku Stick     */ DMG_ENTRY(1, 0xF),
    /* Horse trample  */ DMG_ENTRY(0, 0x0),
    /* Explosives     */ DMG_ENTRY(2, 0xF),
    /* Zora boomerang */ DMG_ENTRY(1, 0xF),
    /* Normal arrow   */ DMG_ENTRY(2, 0xF),
    /* UNK_DMG_0x06   */ DMG_ENTRY(0, 0x0),
    /* Hookshot       */ DMG_ENTRY(0, 0xF),
    /* Goron punch    */ DMG_ENTRY(2, 0xF),
    /* Sword          */ DMG_ENTRY(2, 0xF),
    /* Goron pound    */ DMG_ENTRY(2, 0xF),
    /* Fire arrow     */ DMG_ENTRY(2, 0x2),
    /* Ice arrow      */ DMG_ENTRY(2, 0x3),
    /* Light arrow    */ DMG_ENTRY(2, 0x4),
    /* Goron spikes   */ DMG_ENTRY(2, 0xF),
    /* Deku spin      */ DMG_ENTRY(1, 0xF),
    /* Deku bubble    */ DMG_ENTRY(1, 0xF),
    /* Deku launch    */ DMG_ENTRY(1, 0xF),
    /* UNK_DMG_0x12   */ DMG_ENTRY(1, 0xF),
    /* Zora barrier   */ DMG_ENTRY(0, 0x0),
    /* Normal shield  */ DMG_ENTRY(0, 0xF),
    /* Light ray      */ DMG_ENTRY(0, 0xF),
    /* Thrown object  */ DMG_ENTRY(2, 0xF),
    /* Zora punch     */ DMG_ENTRY(2, 0xF),
    /* Spin attack    */ DMG_ENTRY(2, 0xF),
    /* Sword beam     */ DMG_ENTRY(0, 0x0),
    /* Normal Roll    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(2, 0x2),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, 0x0),
    /* Unblockable    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, 0x0),
    /* Powder Keg     */ DMG_ENTRY(1, 0xF),
};

static ColliderCylinderInit sCylinderInit1 = {
    {
        COL_MATERIAL_HIT3,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK3,
        { 0xF7CFFFFF, 0x00, 0x04 },
        { 0xFFFFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON | ACELEM_HOOKABLE,
        OCELEM_ON,
    },
    { 30, 50, -25, { 0, 0, 0 } },
};

static ColliderCylinderInit sCylinderInit2 = {
    {
        COL_MATERIAL_HIT3,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK3,
        { 0xF7CFFFFF, 0x00, 0x04 },
        { 0xF7FFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON | ACELEM_HOOKABLE,
        OCELEM_ON,
    },
    { 22, 42, -21, { 0, 0, 0 } },
};

void EnTanron2_Init(Actor* thisx, PlayState* play) {
    EnTanron2* this = (EnTanron2*)thisx;

    D_80BB8450 = (Boss04*)this->actor.parent;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;

    if (this->actor.params == 100) {
        this->actor.update = func_80BB7B90;
        Actor_ChangeCategory(play, &play->actorCtx, &this->actor, ACTORCAT_ENEMY);
        return;
    }

    this->actor.flags |= ACTOR_FLAG_HOOKSHOT_PULLS_ACTOR;
    Actor_SetScale(&this->actor, 1.0f);

    this->actor.draw = NULL;
    this->actor.colChkInfo.health = 1;
    this->actor.colChkInfo.damageTable = &sDamageTable;
    this->actor.attentionRangeType = ATTENTION_RANGE_5;

    Collider_InitAndSetCylinder(play, &this->collider1, &this->actor, &sCylinderInit1);
    Collider_InitAndSetCylinder(play, &this->collider2, &this->actor, &sCylinderInit2);

    if ((KREG(64) != 0) || CHECK_EVENTINF(EVENTINF_INTRO_CS_WATCHED_WART)) {
        func_80BB69C0(this);
    } else {
        func_80BB6F64(this);
    }

    this->unk_14A = Rand_ZeroFloat(0x10000);
    this->unk_14C = Rand_ZeroFloat(800.0f) + 1400.0f;
    if (Rand_ZeroOne() < 0.5f) {
        this->unk_14C = -this->unk_14C;
    }

    Actor_UpdateBgCheckInfo(play, &this->actor, 35.0f, 60.0f, 60.0f, UPDBGCHECKINFO_FLAG_4);
    this->actor.floorHeight += 20.0f;
    this->unk_148 = Rand_ZeroFloat(32.0f);
}

void EnTanron2_Destroy(Actor* thisx, PlayState* play) {
}

void func_80BB69C0(EnTanron2* this) {
    this->actionFunc = func_80BB69FC;
    this->unk_160 = 0.0f;
    this->unk_15C = 0.0f;
    this->collider1.dim.radius = 25;
    this->collider1.dim.height = 46;
    this->collider1.dim.yShift = -23;
    this->unk_158 = 0;
}

void func_80BB69FC(EnTanron2* this, PlayState* play) {
    f32 sp34;
    f32 sp30;
    f32 sp2C;
    Vec3f* temp_s1 = &D_80BB8450->unk_2E4[this->actor.params];

    if (this->unk_15C == 1.0f) {
        Math_Vec3f_Copy(&this->actor.world.pos, temp_s1);
    } else {
        sp34 = Math_SmoothStepToF(&this->actor.world.pos.x, temp_s1->x, this->unk_15C, this->unk_160, 0.0f);
        sp30 = Math_SmoothStepToF(&this->actor.world.pos.y, temp_s1->y, this->unk_15C, this->unk_160, 0.0f);
        sp2C = Math_SmoothStepToF(&this->actor.world.pos.z, temp_s1->z, this->unk_15C, this->unk_160, 0.0f);

        if ((this->unk_158 == 0) && ((sp34 + sp30 + sp2C) < 2.0f)) {
            this->unk_158 = 1;
            Actor_PlaySfx(&this->actor, NA_SE_EN_B_SLIME_EAT);
        }

        Math_ApproachF(&this->unk_15C, 1.0f, 1.0f, 0.02f);
        Math_ApproachF(&this->unk_160, 20.0f, 1.0f, 1.0f);
    }

    Math_ApproachF(&this->actor.scale.x, 1.0f, 0.1f, 0.2f);
    Math_ApproachF(&this->actor.scale.y, 1.0f, 0.1f, 0.2f);
}

void func_80BB6B80(EnTanron2* this) {
    this->actionFunc = func_80BB6BD8;
    this->actor.speed = 0.0f;
    this->actor.velocity.z = 0.0f;
    this->actor.velocity.y = 0.0f;
    this->actor.velocity.x = 0.0f;
    this->unk_158 = 0;
    this->unk_159 = 1;
    this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    this->collider1.dim.radius = 30;
    this->collider1.dim.height = 50;
    this->collider1.dim.yShift = -25;
}

void func_80BB6BD8(EnTanron2* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 sp32;
    f32 sp2C;
    f32 sp28;

    if (this->unk_14E == 0) {
        this->actor.world.pos.x += this->actor.velocity.x;
        this->actor.world.pos.y += this->actor.velocity.y;
        this->actor.world.pos.z += this->actor.velocity.z;
        this->actor.velocity.y -= 2.0f;

        if (this->actor.world.pos.y <= this->actor.floorHeight) {
            this->actor.world.pos.y = this->actor.floorHeight;

            Actor_PlaySfx(&this->actor, NA_SE_EN_IKURA_JUMP2);

            sp2C = D_80BB8450->unk_6BC.x - this->actor.world.pos.x;
            sp28 = D_80BB8450->unk_6BC.z - this->actor.world.pos.z;

            switch (this->unk_158) {
                case 0:
                    if (Rand_ZeroOne() > 0.2f) {
                        sp32 = Rand_ZeroFloat(0x10000);
                    } else {
                        sp32 = Math_Atan2S(sp2C, sp28);
                    }
                    this->actor.speed = Rand_ZeroFloat(5.0f) + 5.0f;
                    break;

                case 1:
                    sp32 = Math_Atan2S(sp2C, sp28);
                    this->actor.speed += 2.0f;
                    if (this->actor.speed > 10.0f) {
                        this->actor.speed = 10.0f;
                    }
                    break;

                case 2:
                    sp32 = Math_Atan2S(player->actor.world.pos.x - this->actor.world.pos.x,
                                       player->actor.world.pos.z - this->actor.world.pos.z) +
                           TRUNCF_BINANG(Rand_ZeroFloat(20000.0f));
                    this->actor.speed = Rand_ZeroFloat(7.0f) + 7.0f;
                    if ((this->unk_152 == 0) && (D_80BB8450->unk_1F6 == 0)) {
                        this->unk_158 = 1;
                    }
                    break;
            }

            //! @bug: sp32 may be used uninitialized
            Matrix_RotateYS(sp32, MTXMODE_NEW);
            Matrix_MultVecZ(this->actor.speed, &this->actor.velocity);
            this->actor.velocity.y = Rand_ZeroFloat(5.0f) + 12.0f;
            this->unk_14E = 5;
        }

        if (this->unk_150 != 0) {
            Math_ApproachF(&this->actor.scale.x, 0.75f, 0.5f, 1.0f);
            Math_ApproachF(&this->actor.scale.y, 1.2f, 0.5f, 1.0f);
        } else {
            Math_ApproachF(&this->actor.scale.x, 1.0f, 0.1f, 0.2f);
            Math_ApproachF(&this->actor.scale.y, 1.0f, 0.1f, 0.2f);
        }
    } else {
        Math_ApproachF(&this->actor.scale.x, 1.2f, 0.75f, 1.5f);
        Math_ApproachF(&this->actor.scale.y, 0.75f, 0.75f, 1.5f);
        if (this->unk_14E == 1) {
            sp2C = D_80BB8450->unk_6BC.x - this->actor.world.pos.x;
            sp28 = D_80BB8450->unk_6BC.z - this->actor.world.pos.z;

            if ((this->unk_158 != 0) && ((SQ(sp2C) + SQ(sp28)) < 14400.0f)) {
                func_80BB69C0(this);
            } else {
                this->unk_150 = 10;
                Actor_PlaySfx(&this->actor, NA_SE_EN_IKURA_JUMP1);
            }
        }
    }
}

void func_80BB6F64(EnTanron2* this) {
    this->actionFunc = func_80BB6F78;
}

void func_80BB6F78(EnTanron2* this, PlayState* play) {
    switch (this->unk_158) {
        case 0:
            if (D_80BB8450->unk_708 == 13) {
                this->unk_158 = 10;
                this->unk_14E = Rand_ZeroFloat(50.0f);
            }
            break;

        case 10:
            if (this->unk_14E == 0) {
                this->unk_150 = 10;
                this->unk_158 = 1;
            }
            break;

        case 1:
            if (this->unk_14E == 0) {
                this->actor.world.pos.y += this->actor.velocity.y;
                this->actor.velocity.y -= 2.0f;

                if (this->actor.world.pos.y <= this->actor.floorHeight) {
                    this->actor.world.pos.y = this->actor.floorHeight;
                    Actor_PlaySfx(&this->actor, NA_SE_EN_IKURA_JUMP2);
                    if (D_80BB8450->unk_6F8 > 0.1f) {
                        Actor_PlaySfx(&this->actor, NA_SE_EV_OUT_OF_WATER_L);
                    }
                    this->actor.velocity.y = Rand_ZeroFloat(5.0f) + 12.0f;
                    this->unk_14E = 5;
                }

                if (this->unk_150 != 0) {
                    Math_ApproachF(&this->actor.scale.x, 0.75f, 0.5f, 1.0f);
                    Math_ApproachF(&this->actor.scale.y, 1.2f, 0.5f, 1.0f);
                } else {
                    Math_ApproachF(&this->actor.scale.x, 1.0f, 0.1f, 0.2f);
                    Math_ApproachF(&this->actor.scale.y, 1.0f, 0.1f, 0.2f);
                }
            } else {
                Math_ApproachF(&this->actor.scale.x, 1.2f, 0.75f, 1.5f);
                Math_ApproachF(&this->actor.scale.y, 0.75f, 0.75f, 1.5f);

                if (this->unk_14E == 1) {
                    if (D_80BB8450->unk_708 == 3) {
                        func_80BB6B80(this);
                        this->unk_158 = 1;
                        this->unk_159 = 0;
                    } else {
                        this->unk_150 = 0xA;
                        Actor_PlaySfx(&this->actor, NA_SE_EN_IKURA_JUMP1);
                    }
                }
            }
            break;
    }
}

void func_80BB71C8(EnTanron2* this, PlayState* play) {
    static Color_RGBA8 sPrimColor = { 255, 255, 255, 255 };
    static Color_RGBA8 sEnvColor = { 255, 100, 100, 255 };
    s32 i;
    Vec3f velocity;
    Vec3f accel;
    Vec3f pos;

    for (i = 0; i < 15; i++) {
        Matrix_RotateYF(Rand_ZeroFloat(2 * M_PIf), MTXMODE_NEW);
        Matrix_RotateXFApply(Rand_ZeroFloat(2 * M_PIf));
        Matrix_MultVecZ(Rand_ZeroFloat(10.0f) + 5.0f, &velocity);
        pos.x = this->actor.world.pos.x + velocity.x;
        pos.y = this->actor.world.pos.y + velocity.y;
        pos.z = this->actor.world.pos.z + velocity.z;

        accel.x = velocity.x * -0.03f;
        accel.y = velocity.y * -0.03f;
        accel.z = velocity.z * -0.03f;

        EffectSsDtBubble_SpawnCustomColor(play, &pos, &velocity, &accel, &sPrimColor, &sEnvColor,
                                          Rand_ZeroFloat(100.0f) + 200.0f, Rand_ZeroFloat(5.0f) + 15.0f, false);
    }
}

void func_80BB7398(EnTanron2* this, PlayState* play) {
    this->actionFunc = func_80BB7408;
    if ((s8)this->actor.colChkInfo.health <= 0) {
        Enemy_StartFinishingBlow(play, &this->actor);
        SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 30, NA_SE_EN_IKURA_DEAD);
    } else {
        Actor_PlaySfx(&this->actor, NA_SE_EN_IKURA_DAMAGE);
    }
}

void func_80BB7408(EnTanron2* this, PlayState* play) {
    this->actor.world.pos.x += this->actor.velocity.x;
    this->actor.world.pos.y += this->actor.velocity.y;
    this->actor.world.pos.z += this->actor.velocity.z;
    this->actor.velocity.y -= 2.0f;

    if (this->actor.world.pos.y <= this->actor.floorHeight) {
        this->actor.world.pos.y = this->actor.floorHeight;
        if ((s8)this->actor.colChkInfo.health <= 0) {
            Actor_Kill(&this->actor);
            func_80BB71C8(this, play);
            D_80BB8450->unk_6F6--;
            D_80BB8450->unk_2E2 += 4;
            D_80BB8450->unk_2E0 += 4;
        } else {
            this->actor.velocity.x *= 0.5f;
            this->actor.velocity.z *= 0.5f;
            Math_ApproachF(&this->actor.scale.x, 1.2f, 0.75f, 1.5f);
            Math_ApproachF(&this->actor.scale.y, 0.75f, 0.75f, 1.5f);
            if ((this->actor.velocity.x < 0.1f) && (this->actor.velocity.z < 0.1f)) {
                func_80BB6B80(this);
                this->unk_158 = 1;
            }
        }
    }
}

void func_80BB7578(EnTanron2* this, PlayState* play) {
    ColliderElement* acHitElem;
    s32 pad;
    Player* player = GET_PLAYER(play);
    s32 pad2[2];
    u8 damage;

    if (this->unk_154 == 0) {
        if (this->collider1.base.acFlags & AC_HIT) {
            this->collider1.base.acFlags &= ~AC_HIT;
            acHitElem = this->collider1.elem.acHitElem;
            if (acHitElem->atDmgInfo.dmgFlags & 0x80) {
                func_80BB6B80(this);
                this->unk_158 = 1;
                Actor_PlaySfx(&this->actor, NA_SE_EN_IKURA_DAMAGE);
                if ((player->focusActor != NULL) && (&this->actor != player->focusActor)) {
                    player->focusActor = &this->actor;
                    play->actorCtx.attention.tatlHoverActor = &this->actor;
                    play->actorCtx.attention.reticleActor = &this->actor;
                }
            } else {
                this->unk_154 = 15;
                if (this->actionFunc != func_80BB69FC) {
                    Matrix_RotateYS(this->actor.yawTowardsPlayer, MTXMODE_NEW);
                    if ((acHitElem->atDmgInfo.dmgFlags & 0x300000) != 0) {
                        this->unk_154 = 10;
                        Matrix_MultVecZ(-10.0f, &this->actor.velocity);
                    } else {
                        this->unk_156 = 15;
                        Matrix_MultVecZ(-20.0f, &this->actor.velocity);
                        damage = this->actor.colChkInfo.damage;
                        this->actor.colChkInfo.health -= damage;
                        func_80BB7398(this, play);
                    }
                } else {
                    Actor_PlaySfx(&this->actor, NA_SE_EN_IKURA_DAMAGE);
                    goto block_18;
                }
            }
        }
    }

    if ((this->actionFunc == func_80BB69FC) &&
        ((D_80BB8450->unk_1F6 == 1) || ((D_80BB8450->unk_1F7 != 0) && (Rand_ZeroOne() < 0.1f)))) {
        if (D_80BB8450->unk_1F7 != 0) {
            D_80BB8450->unk_1F7--;
        }

    block_18:
        func_80BB6B80(this);
        this->unk_158 = 2;
        Matrix_RotateYS(Math_Atan2S(this->actor.world.pos.x - D_80BB8450->unk_6BC.x,
                                    this->actor.world.pos.z - D_80BB8450->unk_6BC.z),
                        MTXMODE_NEW);
        Matrix_MultVecZ(10.0f, &this->actor.velocity);
        this->unk_152 = Rand_ZeroFloat(100.0f) + 200.0f;
    } else if (D_80BB8450->unk_1F6 == 10) {
        Actor_Kill(&this->actor);
        func_80BB71C8(this, play);
    }
}

void EnTanron2_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnTanron2* this = (EnTanron2*)thisx;
    s32 pad2[2];
    Input* input;

    this->unk_148++;
    Actor_SetFocus(&this->actor, 0.0f);

    if (this->unk_14E != 0) {
        this->unk_14E--;
    }

    if (this->unk_150 != 0) {
        this->unk_150--;
    }

    if (this->unk_152 != 0) {
        this->unk_152--;
    }

    if (this->unk_154 != 0) {
        this->unk_154--;
    }

    if (this->unk_156 != 0) {
        this->unk_156--;
    }

    this->actionFunc(this, play);

    if (this->actionFunc != func_80BB69FC) {
        if ((this->actor.world.pos.x < (D_80BB8450->unk_6D8 + 20.0f)) ||
            ((D_80BB8450->unk_6DC - 20.0f) < this->actor.world.pos.x)) {
            this->actor.world.pos.x = this->actor.prevPos.x;
            this->actor.velocity.x *= -1.0f;
        }

        if ((this->actor.world.pos.z < (D_80BB8450->unk_6E0 + 20.0f)) ||
            ((D_80BB8450->unk_6E4 - 20.0f) < this->actor.world.pos.z)) {
            this->actor.world.pos.z = this->actor.prevPos.z;
            this->actor.velocity.z *= -1.0f;
        }
    }

    this->unk_14A += this->unk_14C;
    func_80BB7578(this, play);

    if (this->unk_159 != 0) {
        if (this->actor.xzDistToPlayer < 100.0f) {
            if (this->unk_15A == 0) {
                Collider_UpdateCylinder(&this->actor, &this->collider2);
                CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider2.base);
            } else {
                this->unk_15A--;
            }
        }

        Collider_UpdateCylinder(&this->actor, &this->collider1);
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider1.base);
    }

    if (!(this->unk_148 & 0x1F)) {
        if ((this->actionFunc != func_80BB69FC) && (this->actor.xyzDistToPlayerSq < SQ(200.0f))) {
            this->unk_15B = 1;
        } else {
            this->unk_15B = 0;
        }

        if (this->actionFunc == func_80BB69FC) {
            s16 atan = Math_Atan2S(this->actor.world.pos.x - D_80BB8450->unk_6BC.x,
                                   this->actor.world.pos.z - D_80BB8450->unk_6BC.z);

            if (ABS_ALT(BINANG_SUB(D_80BB8450->actor.yawTowardsPlayer, atan)) > 0x3000) {
                this->unk_159 = 0;
                this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
            } else {
                this->unk_159 = 1;
                this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
            }
        }
    }

    if (CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_HOOKSHOT_ATTACHED) && (this->actor.xzDistToPlayer < 80.0f)) {
        this->actor.flags &= ~ACTOR_FLAG_HOOKSHOT_ATTACHED;
        this->unk_15A = 25;
        this->unk_159 = 1;
    }

    input = CONTROLLER3(&play->state);
    if (CHECK_BTN_ALL(input->press.button, BTN_L)) {
        this->unk_158 = 1;
    }

    if (CHECK_BTN_ALL(input->press.button, BTN_R)) {
        func_80BB6B80(this);
    }
}

void func_80BB7B90(Actor* thisx, PlayState* play) {
    EnTanron2* this = (EnTanron2*)thisx;

    D_80BB8454 = (Math_SinS(play->gameplayFrames * 0x3000) * 0.1f) + 1.0f;
    if (D_80BB8450->unk_1F6 == 11) {
        Actor_Kill(&this->actor);
    }
}

void EnTanron2_Draw(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    s32 i;
    s32 j;
    s32 found;
    Actor* tanron2;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    gSPDisplayList(POLY_XLU_DISP++, gWartBubbleMaterialDL);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, 150);

    tanron2 = play->actorCtx.actorLists[ACTORCAT_BOSS].first;

    for (i = 0; i < ARRAY_COUNT(D_80BB8458); i++) {
        D_80BB8458[i] = NULL;
    }

    found = 0;
    while (tanron2 != NULL) {
        if (tanron2->params < 100) {
            D_80BB8458[found] = (EnTanron2*)tanron2;
            found++;
        }
        tanron2 = tanron2->next;
    }

    for (j = 0; j < found - 1; j++) {
        for (i = 0; i < found - 1; i++) {
            if (D_80BB8458[i + 1] != NULL) {
                if (D_80BB8458[i]->actor.projectedPos.z < D_80BB8458[i + 1]->actor.projectedPos.z) {
                    SWAP(EnTanron2*, D_80BB8458[i], D_80BB8458[i + 1]);
                }
            }
        }
    }

    for (i = 0; i < ARRAY_COUNT(D_80BB8458); i++) {
        if (D_80BB8458[i] != NULL) {
            FrameInterpolation_RecordOpenChild(D_80BB8458[i], D_80BB8458[i]->actor.params);
            Matrix_Translate(D_80BB8458[i]->actor.world.pos.x, D_80BB8458[i]->actor.world.pos.y,
                             D_80BB8458[i]->actor.world.pos.z, MTXMODE_NEW);
            Matrix_ReplaceRotation(&play->billboardMtxF);
            Matrix_Scale(D_80BB8458[i]->actor.scale.x, D_80BB8458[i]->actor.scale.y, 0.0f, MTXMODE_APPLY);
            Matrix_RotateZS(D_80BB8458[i]->unk_14A, MTXMODE_APPLY);
            Matrix_Scale(0.13f, 0.14299999f, 0.13f, MTXMODE_APPLY);
            Matrix_RotateZS(-D_80BB8458[i]->unk_14A, MTXMODE_APPLY);

            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
            gSPDisplayList(POLY_XLU_DISP++, gWartBubbleModelDL);
            FrameInterpolation_RecordCloseChild();
        }
    }

    Gfx_SetupDL44_Xlu(play->state.gfxCtx);

    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 0, 0, 0, 150);
    gSPDisplayList(POLY_XLU_DISP++, gWartShadowMaterialDL);

    tanron2 = play->actorCtx.actorLists[ACTORCAT_BOSS].first;
    while (tanron2 != NULL) {
        if ((tanron2->params < 100) && (((EnTanron2*)tanron2)->unk_15B != 0)) {
            FrameInterpolation_RecordOpenChild(tanron2, tanron2->params);
            Matrix_Translate(tanron2->world.pos.x, D_80BB8450->actor.floorHeight, tanron2->world.pos.z, MTXMODE_NEW);
            Matrix_Scale(0.6f, 0.0f, 0.6f, MTXMODE_APPLY);

            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
            gSPDisplayList(POLY_XLU_DISP++, gWartShadowModelDL);
            FrameInterpolation_RecordCloseChild();
        }
        tanron2 = tanron2->next;
    }

    Gfx_SetupDL60_XluNoCD(play->state.gfxCtx);

    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, 255);
    gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 255, 255);
    gDPSetAlphaDither(POLY_XLU_DISP++, G_AD_NOISE);
    gDPSetColorDither(POLY_XLU_DISP++, G_CD_NOISE);

    tanron2 = play->actorCtx.actorLists[ACTORCAT_BOSS].first;
    while (tanron2 != NULL) {
        if ((tanron2->params < 100) && (((EnTanron2*)tanron2)->unk_15B != 0) &&
            (tanron2->world.pos.y <= tanron2->floorHeight)) {
            FrameInterpolation_RecordOpenChild(tanron2, tanron2->params);
            Matrix_Translate(tanron2->world.pos.x, D_80BB8450->actor.floorHeight + 2.0f, tanron2->world.pos.z,
                             MTXMODE_NEW);
            Matrix_Scale(D_80BB8454, 0.0f, D_80BB8454, MTXMODE_APPLY);

            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
            gSPDisplayList(POLY_XLU_DISP++, gEffWaterRippleDL);
            FrameInterpolation_RecordCloseChild();
        }
        tanron2 = tanron2->next;
    }

    CLOSE_DISPS(play->state.gfxCtx);
}
