/*
 * File: z_en_fz.c
 * Overlay: ovl_En_Fz
 * Description: Freezard
 */

#include "z_en_fz.h"
#include "overlays/actors/ovl_En_Wiz/z_en_wiz.h"
#include "objects/object_fz/object_fz.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"
#include "2s2h/GameInteractor/GameInteractor.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void EnFz_Init(Actor* thisx, PlayState* play);
void EnFz_Destroy(Actor* thisx, PlayState* play);
void EnFz_Update(Actor* thisx, PlayState* play);
void EnFz_Draw(Actor* thisx, PlayState* play);

void func_80932784(EnFz* this, PlayState* play);
void func_80932AE8(EnFz* this);
void func_80932AF4(EnFz* this);
void func_80932BD4(EnFz* this);
void func_809330D4(EnFz* this);
void func_80933104(EnFz* this, PlayState* play);
void func_80933184(EnFz* this);
void func_809331F8(EnFz* this, PlayState* play);
void func_80933248(EnFz* this);
void func_80933274(EnFz* this, PlayState* play);
void func_80933324(EnFz* this);
void func_80933368(EnFz* this, PlayState* play);
void func_809333A4(EnFz* this);
void func_809333D8(EnFz* this, PlayState* play);
void func_80933414(EnFz* this);
void func_80933444(EnFz* this, PlayState* play);
void func_80933480(EnFz* this, PlayState* play);
void func_809334B8(EnFz* this, PlayState* play);
void func_809336C0(EnFz* this, PlayState* play);
void func_80933760(EnFz* this, PlayState* play);
void func_80933790(EnFz* this);
void func_809337D4(EnFz* this, PlayState* play);
void func_8093389C(EnFz* this);
void func_809338E0(EnFz* this, PlayState* play);
void func_80933AF4(EnFz* this);
void func_80933B38(EnFz* this, PlayState* play);
void func_80934018(EnFz* this, Vec3f* a, Vec3f* b, Vec3f* c, f32 arg4);
void func_809340BC(EnFz* this, Vec3f* a, Vec3f* b, Vec3f* c, f32 arg4, f32 arg5, s16 arg6, u8 arg7);
void func_80934178(EnFz* this, PlayState* play);
void func_80934464(EnFz* this, PlayState* play);

ActorProfile En_Fz_Profile = {
    /**/ ACTOR_EN_FZ,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_FZ,
    /**/ sizeof(EnFz),
    /**/ EnFz_Init,
    /**/ EnFz_Destroy,
    /**/ EnFz_Update,
    /**/ EnFz_Draw,
};

static s16 D_809346F0[] = { 0, 0x2000, 0x4000, 0 };

static ColliderCylinderInitType1 sCylinderInit1 = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xF7CFFFFF, 0x00, 0x00 },
        { 0xF7CFEFDD, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON | ACELEM_HOOKABLE,
        OCELEM_ON,
    },
    { 30, 80, 0, { 0, 0, 0 } },
};

static ColliderCylinderInitType1 sCylinderInit2 = {
    {
        COL_MATERIAL_METAL,
        AT_NONE,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_NONE,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xF7CFFFFF, 0x00, 0x00 },
        { 0x00001022, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_NONE,
    },
    { 35, 80, 0, { 0, 0, 0 } },
};

static ColliderCylinderInitType1 sCylinderInit3 = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_NONE,
        OC1_NONE,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x20000000, 0x02, 0x04 },
        { 0x00000000, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_NONE,
        OCELEM_NONE,
    },
    { 20, 30, -15, { 0, 0, 0 } },
};

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, 0x0),
    /* Deku Stick     */ DMG_ENTRY(0, 0xD),
    /* Horse trample  */ DMG_ENTRY(0, 0x0),
    /* Explosives     */ DMG_ENTRY(2, 0xF),
    /* Zora boomerang */ DMG_ENTRY(0, 0x0),
    /* Normal arrow   */ DMG_ENTRY(0, 0xD),
    /* UNK_DMG_0x06   */ DMG_ENTRY(2, 0xF),
    /* Hookshot       */ DMG_ENTRY(3, 0xF),
    /* Goron punch    */ DMG_ENTRY(2, 0xF),
    /* Sword          */ DMG_ENTRY(1, 0xF),
    /* Goron pound    */ DMG_ENTRY(3, 0xF),
    /* Fire arrow     */ DMG_ENTRY(2, 0x2),
    /* Ice arrow      */ DMG_ENTRY(0, 0x0),
    /* Light arrow    */ DMG_ENTRY(2, 0x4),
    /* Goron spikes   */ DMG_ENTRY(1, 0xF),
    /* Deku spin      */ DMG_ENTRY(0, 0x0),
    /* Deku bubble    */ DMG_ENTRY(0, 0x0),
    /* Deku launch    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, 0x0),
    /* Zora barrier   */ DMG_ENTRY(0, 0x0),
    /* Normal shield  */ DMG_ENTRY(0, 0x0),
    /* Light ray      */ DMG_ENTRY(0, 0xE),
    /* Thrown object  */ DMG_ENTRY(1, 0xF),
    /* Zora punch     */ DMG_ENTRY(1, 0xF),
    /* Spin attack    */ DMG_ENTRY(1, 0xF),
    /* Sword beam     */ DMG_ENTRY(0, 0x0),
    /* Normal Roll    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, 0x0),
    /* Unblockable    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, 0x0),
    /* Powder Keg     */ DMG_ENTRY(1, 0xF),
};

static InitChainEntry sInitChain[] = {
    ICHAIN_S8(hintId, TATL_HINT_ID_FREEZARD, ICHAIN_CONTINUE),
    ICHAIN_U8(attentionRangeType, ATTENTION_RANGE_2, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 1400, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 30, ICHAIN_STOP),
};

void EnFz_Init(Actor* thisx, PlayState* play) {
    EnFz* this = (EnFz*)thisx;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    this->actor.colChkInfo.damageTable = &sDamageTable;
    this->actor.colChkInfo.health = 3;

    Collider_InitCylinder(play, &this->collider1);
    Collider_SetCylinderType1(play, &this->collider1, &this->actor, &sCylinderInit1);
    Collider_InitCylinder(play, &this->collider2);
    Collider_SetCylinderType1(play, &this->collider2, &this->actor, &sCylinderInit2);
    Collider_InitCylinder(play, &this->collider3);
    Collider_SetCylinderType1(play, &this->collider3, &this->actor, &sCylinderInit3);

    Actor_SetScale(&this->actor, 0.008f);
    this->actor.gravity = 0.0f;
    this->actor.velocity.y = 0.0f;
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->unk_BC8 = 0;
    this->unk_BCF = 0;
    this->unk_BCC = 1;
    this->unk_BCD = 0;
    this->unk_BCE = 0;
    this->unk_BD7 = 1;
    this->unk_BD8 = 0;
    this->actor.speed = 0.0f;
    this->actor.cullingVolumeScale = 400.0f;
    this->unk_BAC = this->actor.world.pos.y;
    this->unk_BB4 = this->actor.world.pos.y;
    this->unk_BA8 = this->actor.world.pos.x;
    this->unk_BB0 = this->actor.world.pos.z;
    this->actor.velocity.y = this->actor.gravity;
    this->unk_BB8 = 135.0f;

    if (ENFZ_GET_8000(&this->actor)) {
        this->unk_BC0 = 0;
        this->actor.scale.y = 0.0f;
        func_80933184(this);
    } else {
        this->unk_BC0 = 255;
        if (this->actor.shape.rot.z == 0) {
            this->unk_BC6 = (s32)Rand_ZeroFloat(64.0f) + 192;
        } else {
            if (this->actor.shape.rot.z < 0) {
                this->actor.shape.rot.z = 1;
            } else if (this->actor.shape.rot.z > 0x10) {
                this->actor.shape.rot.z = 0x10;
            }
            this->actor.shape.rot.z += -1;
            this->unk_BC6 = this->actor.shape.rot.z * 0x10;
        }

        this->actor.shape.rot.z = 0;
        if (ENFZ_GET_4000(&this->actor)) {
            this->unk_BC0 = 0;
            this->actor.scale.y = 0.0f;
            func_80933184(this);
        } else if (ENFZ_GET_F(&this->actor) == ENFZ_F_3) {
            func_80933AF4(this);
        } else {
            func_8093389C(this);
        }
    }

    this->drawDmgEffTimer = 0;
    this->drawDmgEffScale = 0.0f;
    this->drawDmgEffAlpha = 0.0f;
    func_80932784(this, play);
}

void EnFz_Destroy(Actor* thisx, PlayState* play) {
    EnFz* this = (EnFz*)thisx;

    Collider_DestroyCylinder(play, &this->collider1);
    Collider_DestroyCylinder(play, &this->collider2);
    Collider_DestroyCylinder(play, &this->collider3);

    if ((this->actor.parent != NULL) && (this->unk_BC4 == 0) && (this->actor.parent->id == ACTOR_EN_WIZ) &&
        (this->actor.parent->update != NULL) && (((EnWiz*)this->actor.parent)->freezard != NULL)) {
        EnWiz* wiz = (EnWiz*)this->actor.parent;

        wiz->freezard = NULL;
    }
}

void func_80932784(EnFz* this, PlayState* play) {
    Vec3f sp5C;
    Vec3f sp50;
    Vec3f sp44;
    s32 bgId;
    CollisionPoly* sp3C;

    sp5C.x = this->actor.world.pos.x;
    sp5C.y = this->actor.world.pos.y + 20.0f;
    sp5C.z = this->actor.world.pos.z;

    Matrix_Translate(sp5C.x, sp5C.y, sp5C.z, MTXMODE_NEW);
    Matrix_RotateZYX(this->actor.shape.rot.x, this->actor.shape.rot.y, this->actor.shape.rot.z, MTXMODE_APPLY);

    sp44.x = sp44.y = 0.0f;
    sp44.z = 440.0f;

    Matrix_MultVec3f(&sp44, &this->unk_22C);
    if (BgCheck_EntityLineTest1(&play->colCtx, &sp5C, &this->unk_22C, &sp50, &sp3C, true, false, false, true, &bgId)) {
        Math_Vec3f_Copy(&this->unk_22C, &sp50);
    }

    sp5C.x = this->actor.world.pos.x - this->unk_22C.x;
    sp5C.z = this->actor.world.pos.z - this->unk_22C.z;
    this->unk_238 = SQ(sp5C.x) + SQ(sp5C.z);
}

s32 func_809328A4(EnFz* this, Vec3f* arg1) {
    f32 temp_f0 = this->actor.world.pos.x - arg1->x;
    f32 temp_f2 = this->actor.world.pos.z - arg1->z;

    if (this->unk_238 <= (SQ(temp_f0) + SQ(temp_f2))) {
        return true;
    }
    return false;
}

void func_809328F4(EnFz* this, PlayState* play, Vec3f* arg2, s32 arg3, f32 arg4) {
    s32 i;
    Vec3f spA8;
    Vec3f sp9C;
    Vec3f sp90;
    Color_RGBA8 sp8C;
    Color_RGBA8 sp88;
    f32 temp_f24;
    s32 temp_s1;

    sp90.x = sp90.z = 0.0f;
    sp90.y = -1.0f;
    sp8C.r = 155;
    sp8C.g = 255;
    sp8C.b = 255;
    sp8C.a = 255;
    sp88.r = 200;
    sp88.g = 200;
    sp88.b = 200;

    for (i = 0; i < arg3; i++) {
        temp_f24 = Rand_CenteredFloat(0.3f) + 0.6f;
        temp_s1 = (s32)Rand_CenteredFloat(5.0f) + 12;
        spA8.x = Rand_CenteredFloat(arg4) + arg2->x;
        spA8.y = Rand_ZeroFloat(arg4) + arg2->y;
        spA8.z = Rand_CenteredFloat(arg4) + arg2->z;
        sp9C.x = Rand_CenteredFloat(10.0f);
        sp9C.y = Rand_ZeroFloat(10.0f) + 2.0f;
        sp9C.z = Rand_CenteredFloat(10.0f);
        EffectSsEnIce_Spawn(play, &spA8, temp_f24, &sp9C, &sp90, &sp8C, &sp88, temp_s1);
    }

    CollisionCheck_SpawnShieldParticles(play, arg2);
}

void func_80932AE8(EnFz* this) {
}

void func_80932AF4(EnFz* this) {
    Vec3f sp44;
    Vec3f sp38;
    Vec3f sp2C;

    if (!(this->unk_BC6 & 0xF)) {
        sp44.x = Rand_CenteredFloat(40.0f) + this->actor.world.pos.x;
        sp44.y = Rand_CenteredFloat(40.0f) + this->actor.world.pos.y + 30.0f;
        sp44.z = Rand_CenteredFloat(40.0f) + this->actor.world.pos.z;
        sp2C.x = sp2C.z = 0.0f;
        sp2C.y = 0.1f;
        sp38.x = sp38.y = sp38.z = 0.0f;
        func_80934018(this, &sp44, &sp38, &sp2C, Rand_ZeroFloat(7.5f) + 15.0f);
    }
}

void func_80932BD4(EnFz* this) {
    Vec3f sp44;
    Vec3f sp38;
    Vec3f sp2C;

    if (!(this->unk_BC6 & 3)) {
        sp44.x = Rand_CenteredFloat(40.0f) + this->actor.world.pos.x;
        sp44.y = this->unk_BB4;
        sp44.z = Rand_CenteredFloat(40.0f) + this->actor.world.pos.z;
        sp2C.x = sp2C.z = 0.0f;
        sp2C.y = 0.1f;
        sp38.x = sp38.y = sp38.z = 0.0f;
        func_80934018(this, &sp44, &sp38, &sp2C, Rand_ZeroFloat(7.5f) + 15.0f);
    }
}

void func_80932C98(EnFz* this, PlayState* play) {
    Vec3f sp3C;

    if (this->unk_BCD != 0) {
        if ((this->actor.bgCheckFlags & BGCHECKFLAG_WALL) ||
            !Actor_TestFloorInDirection(&this->actor, play, 60.0f, this->actor.world.rot.y)) {
            this->actor.bgCheckFlags &= ~BGCHECKFLAG_WALL;
            this->unk_BCD = 0;
            this->unk_BBC = 0.0f;
            this->actor.speed = 0.0f;
        }
    }

    if (this->actor.parent != NULL) {
        if ((this->unk_BC4 != 5) && (this->actor.parent->id == ACTOR_EN_WIZ)) {
            Actor* parent = this->actor.parent;

            if ((parent->update == NULL) || (parent->colChkInfo.health <= 0)) {
                this->actor.colChkInfo.health = 0;
                this->unk_BC4 = 5;
                Actor_PlaySfx(&this->actor, NA_SE_EN_FREEZAD_DEAD);
                Actor_PlaySfx(&this->actor, NA_SE_EV_ICE_BROKEN);
                sp3C.x = this->actor.world.pos.x;
                sp3C.y = this->actor.world.pos.y;
                sp3C.z = this->actor.world.pos.z;
                func_809328F4(this, play, &sp3C, 30, 10.0f);
                func_809336C0(this, play);
                return;
            }

            if ((this->actor.colChkInfo.health != 0) && (this->unk_BC4 == 1)) {
                this->actor.colChkInfo.health = 0;
                this->unk_BC4 = 5;
                Actor_PlaySfx(&this->actor, NA_SE_EN_FREEZAD_DEAD);
                Actor_PlaySfx(&this->actor, NA_SE_EV_ICE_BROKEN);
                sp3C.x = this->actor.world.pos.x;
                sp3C.y = this->actor.world.pos.y;
                sp3C.z = this->actor.world.pos.z;
                func_809328F4(this, play, &sp3C, 30, 10.0f);
                func_809336C0(this, play);
                return;
            }
        }
    }

    if (this->unk_BCE != 0) {
        if (ENFZ_GET_8000(&this->actor) && (this->collider1.base.atFlags & AT_HIT)) {
            this->unk_BCD = 0;
            this->unk_BBC = 0.0f;
            this->collider1.base.acFlags &= ~AC_HIT;
            this->actor.speed = 0.0f;
            this->unk_BCA = 10;
            func_809330D4(this);
        } else if (this->collider2.base.acFlags & AC_BOUNCED) {
            this->collider2.base.acFlags &= ~AC_BOUNCED;
            this->collider1.base.acFlags &= ~AC_HIT;
        } else if (this->collider1.base.acFlags & AC_HIT) {
            this->collider1.base.acFlags &= ~AC_HIT;
            switch (this->actor.colChkInfo.damageEffect) {
                case 4:
                    this->drawDmgEffTimer = 40;
                    this->drawDmgEffAlpha = 1.0f;

                case 15:
                    Actor_ApplyDamage(&this->actor);
                    Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_XLU, 8);
                    if (this->actor.colChkInfo.health != 0) {
                        Actor_PlaySfx(&this->actor, NA_SE_EN_FREEZAD_DAMAGE);
                        sp3C.x = this->actor.world.pos.x;
                        sp3C.y = this->actor.world.pos.y;
                        sp3C.z = this->actor.world.pos.z;
                        func_809328F4(this, play, &sp3C, 10, 0.0f);
                        this->unk_BCF++;
                        break;
                    }
                    Actor_PlaySfx(&this->actor, NA_SE_EN_FREEZAD_DEAD);
                    Actor_PlaySfx(&this->actor, NA_SE_EV_ICE_BROKEN);
                    sp3C.x = this->actor.world.pos.x;
                    sp3C.y = this->actor.world.pos.y;
                    sp3C.z = this->actor.world.pos.z;
                    func_809328F4(this, play, &sp3C, 30, 10.0f);
                    func_809336C0(this, play);
                    break;

                case 2:
                    Actor_PlaySfx(&this->actor, NA_SE_EN_FREEZAD_DEAD);
                    func_80933790(this);
                    break;
            }
        }
    }
}

void func_80933014(EnFz* this) {
    s16 temp_a1 = this->actor.yawTowardsPlayer;
    s32 temp_a2 = ENFZ_GET_3000(&this->actor);
    s16 temp;
    s16 temp2;
    s32 temp_v1;

    if (!ENFZ_GET_8000(&this->actor)) {
        temp_v1 = this->actor.home.rot.y;
        if (temp_a2 != 3) {
            temp2 = temp_a1 - temp_v1;
            if (D_809346F0[temp_a2] < ABS_ALT(temp2)) {
                temp = (temp2 > 0) ? D_809346F0[temp_a2] : -D_809346F0[temp_a2];
                temp_a1 = this->actor.home.rot.y + temp;
            }
        }
    }
    Math_SmoothStepToS(&this->actor.shape.rot.y, temp_a1, 10, 0x7D0, 0);
    this->actor.world.rot.y = this->actor.shape.rot.y;
}

void func_809330D4(EnFz* this) {
    this->unk_BD6 = 2;
    this->unk_BCE = 0;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->actionFunc = func_80933104;
}

void func_80933104(EnFz* this, PlayState* play) {
    this->unk_BC0 -= 16;
    if (this->unk_BC0 > 255) {
        this->unk_BC0 = 0;
    }

    if (Math_SmoothStepToF(&this->actor.scale.y, 0, 1.0f, 0.0005f, 0.0f) == 0.0f) {
        func_80933184(this);
    }
}

void func_80933184(EnFz* this) {
    this->unk_BD6 = 0;
    this->unk_BD2 = 0;
    this->unk_BD0 = 0;
    this->unk_BCA = 100;

    this->actor.world.pos.x = this->unk_BA8;
    this->actor.world.pos.y = this->unk_BAC;
    this->actor.world.pos.z = this->unk_BB0;

    if (ENFZ_GET_4000(&this->actor)) {
        this->unk_BD6 = 2;
        this->unk_BCA = 10;
        this->unk_BD2 = 4000;
        this->actionFunc = func_80933274;
    } else {
        this->actionFunc = func_809331F8;
    }
}

void func_809331F8(EnFz* this, PlayState* play) {
    if ((this->unk_BCA == 0) && (this->actor.xzDistToPlayer < 400.0f)) {
        func_80933248(this);
    }
}

void func_80933248(EnFz* this) {
    this->unk_BD6 = 2;
    this->unk_BCA = 20;
    this->unk_BD2 = 4000;
    this->actionFunc = func_80933274;
}

void func_80933274(EnFz* this, PlayState* play) {
    if (this->unk_BCA == 0) {

        this->unk_BC0 += 8;
        if (this->unk_BC0 > 255) {
            this->unk_BC0 = 255;
        }

        if (Math_SmoothStepToF(&this->actor.scale.y, 0.008f, 1.0f, 0.0005f, 0.0f) == 0.0f) {
            if (ENFZ_GET_4000(&this->actor)) {
                func_8093389C(this);
            } else {
                func_80933324(this);
            }
        }
    }
}

void func_80933324(EnFz* this) {
    this->unk_BD6 = 1;
    this->unk_BCA = 40;
    this->unk_BCC = 1;
    this->unk_BCE = 1;
    this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    this->actor.gravity = -1.0f;
    this->actionFunc = func_80933368;
}

void func_80933368(EnFz* this, PlayState* play) {
    func_80933014(this);
    if (this->unk_BCA == 0) {
        func_809333A4(this);
    }
}

void func_809333A4(EnFz* this) {
    this->unk_BD6 = 1;
    this->unk_BCD = 1;
    this->unk_BCA = 100;
    this->unk_BBC = 4.0f;
    this->actionFunc = func_809333D8;
}

void func_809333D8(EnFz* this, PlayState* play) {
    if ((this->unk_BCA == 0) || (this->unk_BCD == 0)) {
        func_80933414(this);
    }
}

void func_80933414(EnFz* this) {
    this->unk_BD6 = 1;
    this->unk_BBC = 0.0f;
    this->actor.speed = 0.0f;
    this->unk_BCA = 40;
    this->actionFunc = func_80933444;
}

void func_80933444(EnFz* this, PlayState* play) {
    func_80933014(this);
    if (this->unk_BCA == 0) {
        func_80933480(this, play);
    }
}

void func_80933480(EnFz* this, PlayState* play) {
    this->unk_BD6 = 1;
    this->unk_BCA = 80;
    this->actionFunc = func_809334B8;
    func_80932784(this, play);
}

void func_809334B8(EnFz* this, PlayState* play) {
    Vec3f sp64;
    Vec3f sp58;
    Vec3f sp4C;
    Vec3f sp40;
    u8 sp3F;
    s16 sp3C;

    if (this->unk_BCA == 0) {
        func_809330D4(this);
        return;
    }

    if (this->unk_BCA > 10) {
        sp3F = 0;
        sp3C = 150;
        Actor_PlaySfx_Flagged(&this->actor, NA_SE_EN_FREEZAD_BREATH - SFX_FLAG);
        if ((this->unk_BCA - 10) < 16) {
            sp3C = (this->unk_BCA * 10) - 100;
        }

        sp40.x = sp40.z = 0.0f;
        sp40.y = 0.6f;

        sp58.x = this->actor.world.pos.x;
        sp58.y = this->actor.world.pos.y + 20.0f;
        sp58.z = this->actor.world.pos.z;

        Matrix_RotateYS(this->actor.shape.rot.y, MTXMODE_NEW);

        sp64.x = 0.0f;
        sp64.y = -2.0f;
        sp64.z = ((ENFZ_GET_F(&this->actor) == ENFZ_F_1)   ? 10.0f
                  : (ENFZ_GET_F(&this->actor) == ENFZ_F_2) ? 20.0f
                                                           : 0.0f) +
                 20;

        Matrix_MultVec3f(&sp64, &sp4C);

        if ((this->unk_BCA & 7) == 0) {
            sp3F = 1;
        }

        func_809340BC(this, &sp58, &sp4C, &sp40, 2.0f, 25.0f, sp3C, sp3F);

        sp58.x += sp4C.x * 0.5f;
        sp58.y += sp4C.y * 0.5f;
        sp58.z += sp4C.z * 0.5f;

        func_809340BC(this, &sp58, &sp4C, &sp40, 2.0f, 25.0f, sp3C, 0);
    }
}

void func_809336C0(EnFz* this, PlayState* play) {
    this->unk_BD6 = 0;
    this->unk_BBC = 0.0f;
    this->actor.gravity = 0.0f;
    this->actor.velocity.y = 0.0f;
    this->actor.speed = 0.0f;
    this->unk_BCC = 1;
    this->unk_BCE = 0;
    this->unk_BD8 = 1;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->unk_BD7 = 0;
    this->unk_BCA = 60;
    Actor_ChangeCategory(play, &play->actorCtx, &this->actor, ACTORCAT_PROP);
    Item_DropCollectibleRandom(play, &this->actor, &this->actor.world.pos, 0xA0);
    this->actionFunc = func_80933760;
}

void func_80933760(EnFz* this, PlayState* play) {
    if (this->unk_BCA == 0) {
        Actor_Kill(&this->actor);
    }
}

void func_80933790(EnFz* this) {
    this->unk_BD6 = 3;
    this->unk_BCE = 0;
    this->unk_BD8 = 1;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->actor.speed = 0.0f;
    this->unk_BBC = 0.0f;
    this->actionFunc = func_809337D4;
}

void func_809337D4(EnFz* this, PlayState* play) {
    Math_StepToF(&this->actor.scale.y, 0.0006f, 0.0006f);

    if (this->actor.scale.y < 0.006f) {
        this->actor.scale.x += 0.001f;
        this->actor.scale.z += 0.001f;
    }

    if (this->actor.scale.y < 0.004f) {
        this->unk_BC0 -= 20;
        if (this->unk_BC0 > 255) {
            this->unk_BC0 = 0;
        }
    }

    if (this->unk_BC0 == 0) {
        func_809336C0(this, play);
    }
}

void func_8093389C(EnFz* this) {
    this->unk_BD6 = 1;
    this->unk_BCA = 40;
    this->unk_BCC = 1;
    this->unk_BCE = 1;
    this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    this->actor.gravity = -1.0f;
    this->actionFunc = func_809338E0;
}

void func_809338E0(EnFz* this, PlayState* play) {
    Vec3f sp64;
    Vec3f sp58;
    Vec3f sp4C;
    Vec3f sp40;
    u8 sp3F;
    s16 sp3C;

    if (this->unk_BC6 & (0x80 | 0x40)) {
        func_80933014(this);
        func_80932784(this, play);
        return;
    }

    sp3F = 0;
    sp3C = 150;
    Actor_PlaySfx_Flagged(&this->actor, NA_SE_EN_FREEZAD_BREATH - SFX_FLAG);

    if ((this->unk_BC6 & 0x3F) >= 0x30) {
        sp3C = 630 - ((this->unk_BC6 & 0x3F) * 10);
    }

    sp40.x = sp40.z = 0.0f;
    sp40.y = 0.6f;

    sp58.x = this->actor.world.pos.x;
    sp58.y = this->actor.world.pos.y + 20.0f;
    sp58.z = this->actor.world.pos.z;

    Matrix_RotateYS(this->actor.shape.rot.y, MTXMODE_NEW);

    sp64.x = 0.0f;
    sp64.y = -2.0f;
    sp64.z = ((ENFZ_GET_F(&this->actor) == ENFZ_F_1)   ? 10.0f
              : (ENFZ_GET_F(&this->actor) == ENFZ_F_2) ? 20.0f
                                                       : 0.0f) +
             20;

    Matrix_MultVec3f(&sp64, &sp4C);

    if (!(this->unk_BC6 & 7)) {
        sp3F = 1;
    }

    func_809340BC(this, &sp58, &sp4C, &sp40, 2.0f, 25.0f, sp3C, sp3F);
    sp58.x += sp4C.x * 0.5f;
    sp58.y += sp4C.y * 0.5f;
    sp58.z += sp4C.z * 0.5f;
    func_809340BC(this, &sp58, &sp4C, &sp40, 2.0f, 25.0f, sp3C, 0);
}

void func_80933AF4(EnFz* this) {
    this->unk_BD6 = 1;
    this->unk_BCA = 40;
    this->unk_BCC = 1;
    this->unk_BCE = 1;
    this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    this->actor.gravity = -1.0f;
    this->actionFunc = func_80933B38;
}

void func_80933B38(EnFz* this, PlayState* play) {
}

void func_80933B48(EnFz* this, PlayState* play) {
    if (this->drawDmgEffTimer != 0) {
        if (this->drawDmgEffTimer > 0) {
            this->drawDmgEffTimer--;
        }

        if (this->drawDmgEffTimer < 20) {
            Math_SmoothStepToF(&this->drawDmgEffScale, 0.0f, 0.5f, 0.03f, 0.0f);
            this->drawDmgEffAlpha = this->drawDmgEffTimer * 0.05f;
        } else {
            Math_SmoothStepToF(&this->drawDmgEffScale, 0.5f, 0.1f, 0.02f, 0.0f);
        }
    }
}

void EnFz_Update(Actor* thisx, PlayState* play) {
    static EnFzUnkFunc D_809347AC[] = { func_80932AE8, func_80932AF4, func_80932BD4, func_80932BD4 };
    s32 pad;
    EnFz* this = (EnFz*)thisx;

    this->unk_BC6++;
    if (this->unk_BC8 != 0) {
        this->unk_BC8--;
    }

    if (this->unk_BCA != 0) {
        this->unk_BCA--;
    }

    if (this->unk_BD9 != 0) {
        this->unk_BD9--;
    }

    Actor_SetFocus(&this->actor, 50.0f);
    func_80932C98(this, play);

    this->actionFunc(this, play);

    if (this->unk_BD8 == 0) {
        Collider_UpdateCylinder(&this->actor, &this->collider1);
        Collider_UpdateCylinder(&this->actor, &this->collider2);
        if (this->unk_BCE != 0) {
            if (this->actor.colorFilterTimer == 0) {
                CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider1.base);
                CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider2.base);
            }
            CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider1.base);
        }
    }

    Math_StepToF(&this->actor.speed, this->unk_BBC, 0.2f);
    Actor_MoveWithGravity(&this->actor);
    if (this->unk_BCC != 0) {
        Actor_UpdateBgCheckInfo(play, &this->actor, 20.0f, 20.0f, 20.0f, UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4);
    }

    D_809347AC[this->unk_BD6](this);
    func_80933B48(this, play);
    func_80934178(this, play);
}

void EnFz_Draw(Actor* thisx, PlayState* play) {
    static Gfx* D_809347BC[] = { object_fz_DL_001130, object_fz_DL_0021A0, object_fz_DL_002CA0 };
    s32 pad;
    EnFz* this = (EnFz*)thisx;
    s32 sp9C = 3 - this->actor.colChkInfo.health;

    OPEN_DISPS(play->state.gfxCtx);

    if (this->actor.colChkInfo.health == 0) {
        sp9C = 2;
    }

    if (this->unk_BD7 != 0) {
        func_800B8118(&this->actor, play, 0);
        Gfx_SetupDL25_Xlu(play->state.gfxCtx);

        gSPSegment(POLY_XLU_DISP++, 0x08,
                   Gfx_TwoTexScrollEx(play->state.gfxCtx, 0, 0, play->state.frames % 128, 0x20, 0x20, 1, 0,
                                      (play->state.frames * 2) % 128, 0x20, 0x20, 0, 1, 0, 2));
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
        gDPSetCombineLERP(POLY_XLU_DISP++, TEXEL1, PRIMITIVE, PRIM_LOD_FRAC, TEXEL0, TEXEL1, TEXEL0, PRIMITIVE, TEXEL0,
                          PRIMITIVE, ENVIRONMENT, COMBINED, ENVIRONMENT, COMBINED, 0, ENVIRONMENT, 0);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, 155, 255, 255, 255);
        gDPSetEnvColor(POLY_XLU_DISP++, 200, 200, 200, this->unk_BC0);
        gSPDisplayList(POLY_XLU_DISP++, D_809347BC[sp9C]);
    }

    func_80934464(this, play);

    if (this->drawDmgEffTimer > 0) {
        s32 pad2[6];
        Vec3f bodyPartsPos[2];
        s32 pad3;

        bodyPartsPos[0] = this->actor.world.pos;
        bodyPartsPos[1] = this->actor.world.pos;
        bodyPartsPos[0].y += 20.0f;
        bodyPartsPos[1].y += 40.0f;
        if (GameInteractor_Should(VB_USE_NULL_FOR_DRAW_DAMAGE_EFFECTS, true, this, bodyPartsPos,
                                  ARRAY_COUNT(bodyPartsPos))) {
            Actor_DrawDamageEffects(play, NULL, bodyPartsPos, ARRAY_COUNT(bodyPartsPos), this->drawDmgEffScale * 4.0f,
                                    0.5f, this->drawDmgEffAlpha, ACTOR_DRAW_DMGEFF_LIGHT_ORBS);
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void func_80934018(EnFz* this, Vec3f* a, Vec3f* b, Vec3f* c, f32 arg4) {
    s16 i;
    EnFzStruct* ptr = &this->unk_23C[0];

    for (i = 0; i < ARRAY_COUNT(this->unk_23C); i++, ptr++) {
        if (ptr->unk_00 == 0) {
            ptr->unk_00 = 1;
            ptr->unk_04 = *a;
            ptr->unk_10 = *b;
            ptr->unk_1C = *c;
            ptr->unk_2E = 0;
            ptr->unk_30 = arg4 / 1000.0f;
            ptr->unk_2C = 0;
            ptr->unk_01 = 0;
            break;
        }
    }
}

void func_809340BC(EnFz* this, Vec3f* a, Vec3f* b, Vec3f* c, f32 arg4, f32 arg5, s16 arg6, u8 arg7) {
    s16 i;
    EnFzStruct* ptr = &this->unk_23C[0];

    for (i = 0; i < ARRAY_COUNT(this->unk_23C); i++, ptr++) {
        if (ptr->unk_00 == 0) {
            ptr->unk_00 = 2;
            ptr->unk_04 = *a;
            ptr->unk_10 = *b;
            ptr->unk_1C = *c;
            ptr->unk_2E = 0;
            ptr->unk_30 = arg4 / 1000.0f;
            ptr->unk_34 = arg5 / 1000.0f;
            ptr->unk_2C = arg6;
            ptr->unk_01 = 0;
            ptr->unk_38 = arg7;
            break;
        }
    }
}

void func_80934178(EnFz* this, PlayState* play) {
    s16 i;
    EnFzStruct* ptr = this->unk_23C;
    Vec3f sp64;

    for (i = 0; i < ARRAY_COUNT(this->unk_23C); i++, ptr++) {
        if (ptr->unk_00 != 0) {
            ptr->unk_04.x += ptr->unk_10.x;
            ptr->unk_04.y += ptr->unk_10.y;
            ptr->unk_04.z += ptr->unk_10.z;

            ptr->unk_01++;

            ptr->unk_10.x += ptr->unk_1C.x;
            ptr->unk_10.y += ptr->unk_1C.y;
            ptr->unk_10.z += ptr->unk_1C.z;

            if (ptr->unk_00 == 1) {
                if (ptr->unk_2E == 0) {
                    ptr->unk_2C += 10;
                    if (ptr->unk_2C >= 100) {
                        ptr->unk_2E++;
                    }
                } else {
                    ptr->unk_2C -= 3;
                    if (ptr->unk_2C <= 0) {
                        ptr->unk_2C = 0;
                        ptr->unk_00 = 0;
                    }
                }
            } else if (ptr->unk_00 == 2) {
                Math_ApproachF(&ptr->unk_30, ptr->unk_34, 0.1f, ptr->unk_34 / 10.0f);
                if (ptr->unk_2E == 0) {
                    if (ptr->unk_01 >= 7) {
                        ptr->unk_2E++;
                    }
                } else {
                    ptr->unk_10.x *= 0.75f;
                    ptr->unk_1C.y = 2.0f;
                    ptr->unk_10.z *= 0.75f;
                    ptr->unk_2C -= 17;
                    if (ptr->unk_2C <= 0) {
                        ptr->unk_2C = 0;
                        ptr->unk_00 = 0;
                    }
                }

                if ((this->unk_BD9 == 0) && (ptr->unk_2C > 100) && (ptr->unk_38 != 0)) {
                    this->collider3.dim.pos.x = ptr->unk_04.x;
                    this->collider3.dim.pos.y = ptr->unk_04.y;
                    this->collider3.dim.pos.z = ptr->unk_04.z;
                    CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider3.base);
                }

                sp64.x = ptr->unk_04.x;
                sp64.y = ptr->unk_04.y + 10.0f;
                sp64.z = ptr->unk_04.z;

                if ((ptr->unk_2E != 2) && func_809328A4(this, &sp64)) {
                    ptr->unk_2E = 2;
                    ptr->unk_10.x = 0.0f;
                    ptr->unk_10.z = 0.0f;
                }
            }
        }
    }
}

void func_80934464(EnFz* this, PlayState* play) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    s16 i;
    u8 flag = 0;
    EnFzStruct* ptr = this->unk_23C;

    OPEN_DISPS(gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    gDPSetColorDither(POLY_XLU_DISP++, G_CD_BAYER);
    gDPSetAlphaDither(POLY_XLU_DISP++, G_AD_PATTERN);

    for (i = 0; i < ARRAY_COUNT(this->unk_23C); i++, ptr++) {
        if (ptr->unk_00 > 0) {
            FrameInterpolation_RecordOpenChild(ptr, i);
            gDPPipeSync(POLY_XLU_DISP++);

            if (flag == 0) {
                gSPDisplayList(POLY_XLU_DISP++, gFrozenSteamMaterialDL);
                flag++;
            }

            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 195, 225, 235, ptr->unk_2C);
            gSPSegment(POLY_XLU_DISP++, 0x08,
                       Gfx_TwoTexScrollEx(play->state.gfxCtx, 0, (ptr->unk_01 + (i * 3)) * 3,
                                          (ptr->unk_01 + (i * 3)) * 15, 0x20, 0x40, 1, 0, 0, 0x20, 0x20, 3, 15, 0, 0));

            Matrix_Translate(ptr->unk_04.x, ptr->unk_04.y, ptr->unk_04.z, MTXMODE_NEW);
            Matrix_ReplaceRotation(&play->billboardMtxF);
            Matrix_Scale(ptr->unk_30, ptr->unk_30, 1.0f, MTXMODE_APPLY);

            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx);
            gSPDisplayList(POLY_XLU_DISP++, gFrozenSteamModelDL);
            FrameInterpolation_RecordCloseChild();
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}
