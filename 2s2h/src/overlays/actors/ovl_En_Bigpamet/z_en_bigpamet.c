/*
 * File: z_en_bigpamet.c
 * Overlay: ovl_En_Bigpamet
 * Description: Gekko & Snapper Miniboss - Snapper
 */

#include "z_en_bigpamet.h"
#include "z64quake.h"
#include "z64rumble.h"
#include "overlays/actors/ovl_En_Pametfrog/z_en_pametfrog.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define FLAGS                                                                                 \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_DRAW_CULLING_DISABLED | ACTOR_FLAG_HOOKSHOT_PULLS_PLAYER)

void EnBigpamet_Init(Actor* thisx, PlayState* play);
void EnBigpamet_Destroy(Actor* thisx, PlayState* play);
void EnBigpamet_Update(Actor* thisx, PlayState* play);
void EnBigpamet_Draw(Actor* thisx, PlayState* play);

void func_80A281DC(EnBigpamet* this, PlayState* play);
void func_80A282C8(EnBigpamet* this, PlayState* play);
void func_80A283A0(EnBigpamet* this, PlayState* play);
void func_80A2844C(EnBigpamet* this, PlayState* play);
void func_80A2855C(EnBigpamet* this, PlayState* play);
void func_80A2866C(EnBigpamet* this, PlayState* play);
void func_80A28708(EnBigpamet* this, PlayState* play);
void func_80A287E8(EnBigpamet* this, PlayState* play);
void func_80A289C8(EnBigpamet* this, PlayState* play);
void func_80A28A98(EnBigpamet* this, PlayState* play);
void func_80A28D0C(EnBigpamet* this, PlayState* play);
void func_80A28DC0(EnBigpamet* this, PlayState* play);
void func_80A28E98(EnBigpamet* this, PlayState* play);
void func_80A28EE8(EnBigpamet* this, PlayState* play);
void func_80A281B4(EnBigpamet* this);
void func_80A28274(EnBigpamet* this);
void func_80A28378(EnBigpamet* this);
void func_80A28E40(EnBigpamet* this);
void func_80A28618(EnBigpamet* this);
void func_80A2966C(Actor* thisx, PlayState* play);
void func_80A286C0(EnBigpamet* this);
void func_80A28760(EnBigpamet* this);
void func_80A28970(EnBigpamet* this);
void func_80A28A28(EnBigpamet* this);
void func_80A28D80(EnBigpamet* this);
void func_80A28ED4(EnBigpamet* this);

ActorProfile En_Bigpamet_Profile = {
    /**/ ACTOR_EN_BIGPAMET,
    /**/ ACTORCAT_BOSS,
    /**/ FLAGS,
    /**/ OBJECT_TL,
    /**/ sizeof(EnBigpamet),
    /**/ EnBigpamet_Init,
    /**/ EnBigpamet_Destroy,
    /**/ EnBigpamet_Update,
    /**/ EnBigpamet_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_HARD,
        AT_NONE | AT_TYPE_ENEMY,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xF7CFFFFF, 0x04, 0x04 },
        { 0xF7CF7FFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON | ACELEM_HOOKABLE,
        OCELEM_ON,
    },
    { 53, 50, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit sColChkInfoInit = { 1, 53, 60, 250 };

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(1, 0x0),
    /* Deku Stick     */ DMG_ENTRY(1, 0x0),
    /* Horse trample  */ DMG_ENTRY(0, 0x0),
    /* Explosives     */ DMG_ENTRY(1, 0xF),
    /* Zora boomerang */ DMG_ENTRY(1, 0x0),
    /* Normal arrow   */ DMG_ENTRY(1, 0x0),
    /* UNK_DMG_0x06   */ DMG_ENTRY(0, 0x0),
    /* Hookshot       */ DMG_ENTRY(1, 0x0),
    /* Goron punch    */ DMG_ENTRY(1, 0x0),
    /* Sword          */ DMG_ENTRY(1, 0x0),
    /* Goron pound    */ DMG_ENTRY(1, 0xF),
    /* Fire arrow     */ DMG_ENTRY(1, 0x0),
    /* Ice arrow      */ DMG_ENTRY(1, 0x0),
    /* Light arrow    */ DMG_ENTRY(1, 0x0),
    /* Goron spikes   */ DMG_ENTRY(1, 0x0),
    /* Deku spin      */ DMG_ENTRY(1, 0x0),
    /* Deku bubble    */ DMG_ENTRY(1, 0x0),
    /* Deku launch    */ DMG_ENTRY(1, 0xF),
    /* UNK_DMG_0x12   */ DMG_ENTRY(1, 0x0),
    /* Zora barrier   */ DMG_ENTRY(1, 0x0),
    /* Normal shield  */ DMG_ENTRY(0, 0x0),
    /* Light ray      */ DMG_ENTRY(0, 0x0),
    /* Thrown object  */ DMG_ENTRY(1, 0x0),
    /* Zora punch     */ DMG_ENTRY(1, 0x0),
    /* Spin attack    */ DMG_ENTRY(1, 0x0),
    /* Sword beam     */ DMG_ENTRY(1, 0x0),
    /* Normal Roll    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, 0x0),
    /* Unblockable    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, 0x0),
    /* Powder Keg     */ DMG_ENTRY(1, 0xF),
};

TexturePtr D_80A29754[] = {
    gSnapperEyeOpenTex,
    gSnapperEyeHalfTex,
    gSnapperEyeClosedTex,
    gSnapperEyeHalfTex,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_S8(hintId, 1, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 15, ICHAIN_CONTINUE),
    ICHAIN_F32(gravity, -2, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 4333, ICHAIN_CONTINUE),
    ICHAIN_U8(attentionRangeType, ATTENTION_RANGE_5, ICHAIN_STOP),
};

static s32 sTexturesDesegmented = false;
Vec3f D_80A2977C = { 0.0f, 1.0f, 0.0f };
Color_RGBA8 D_80A29788 = { 250, 250, 250, 255 };
Color_RGBA8 D_80A2978C = { 180, 180, 180, 255 };

void EnBigpamet_Init(Actor* thisx, PlayState* play) {
    EnBigpamet* this = (EnBigpamet*)thisx;
    s32 i;

    Actor_ProcessInitChain(&this->actor, sInitChain);

    SkelAnime_InitFlex(play, &this->snapperSkelAnime, &gSnapperSkel, &gSnapperIdleAnim, this->snapperJointTable,
                       this->snapperMorphTable, SNAPPER_LIMB_MAX);
    SkelAnime_InitFlex(play, &this->spikedSnapperSkelAnime, &gSpikedSnapperSkel, &gSpikedSnapperIdleAnim,
                       this->spikedSnapperJointTable, this->spikedSnapperMorphTable, SPIKED_SNAPPER_LIMB_MAX);

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 55.0f);

    Collider_InitAndSetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    CollisionCheck_SetInfo(&this->actor.colChkInfo, &sDamageTable, &sColChkInfoInit);

    if (!sTexturesDesegmented) {
        for (i = 0; i < ARRAY_COUNT(D_80A29754); i++) {
            D_80A29754[i] = Lib_SegmentedToVirtual(D_80A29754[i]);
        }
        sTexturesDesegmented = true;
    }

    this->actor.params = ENBIGPAMET_1;
    func_80A281B4(this);
}

void EnBigpamet_Destroy(Actor* thisx, PlayState* play) {
    EnBigpamet* this = (EnBigpamet*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void func_80A2768C(EnBigpamet* this) {
    if (this->unk_29C != 0) {
        this->unk_29C++;
        if (this->unk_29C == 4) {
            this->unk_29C = 0;
        }
    } else if (Rand_ZeroOne() < 0.05f) {
        this->unk_29C = 1;
    }
}

void func_80A276F4(EnBigpamet* this) {
    if (this->actor.parent->params == GEKKO_ON_SNAPPER) {
        this->actor.parent->shape.rot.y = this->actor.shape.rot.y;
        this->actor.parent->shape.rot.z = this->actor.shape.rot.z;
        Matrix_SetTranslateRotateYXZ(this->actor.world.pos.x, this->unk_2AC, this->actor.world.pos.z,
                                     &this->actor.shape.rot);
        Matrix_MultVecY(46.0f, &this->actor.parent->world.pos);
    } else if (this->actor.parent->params == GEKKO_REAR_ON_SNAPPER) {
        this->actor.parent->world.pos.x = this->actor.world.pos.x;
        this->actor.parent->world.pos.z = this->actor.world.pos.z;
    }
}

void func_80A2778C(EnBigpamet* this) {
    f32 temp_fs1;
    s16 temp_s1;
    s16 temp_s2;
    s32 i;
    EnBigpametStruct* ptr;

    this->unk_2A2 = 40;

    for (ptr = &this->unk_2FC[0], i = 0; i < ARRAY_COUNT(this->unk_2FC); i++, ptr++) {
        temp_s2 = (s32)Rand_Next() >> 0x10;
        temp_s1 = Rand_S16Offset(0x1800, 0x2800);
        temp_fs1 = Rand_ZeroFloat(5.0f) + 7.0f;

        ptr->unk_0C.x = (temp_fs1 * Math_CosS(temp_s1)) * Math_SinS(temp_s2);
        ptr->unk_0C.y = (temp_fs1 * Math_SinS(temp_s1)) + 3.0f;
        ptr->unk_0C.z = (temp_fs1 * Math_CosS(temp_s1)) * Math_CosS(temp_s2);

        ptr->unk_18.x = (s32)Rand_Next() >> 0x10;
        ptr->unk_18.y = (s32)Rand_Next() >> 0x10;
        ptr->unk_18.z = (s32)Rand_Next() >> 0x10;

        ptr->unk_00.x = (Math_SinS(temp_s2) * 40.0f) + this->actor.world.pos.x;
        ptr->unk_00.y = this->actor.floorHeight;
        ptr->unk_00.z = (Math_CosS(temp_s2) * 40.0f) + this->actor.world.pos.z;

        ptr->unk_20 = Rand_ZeroFloat(0.0025000002f) + 0.002f;
    }

    Actor_PlaySfx(&this->actor, NA_SE_EN_B_PAMET_BREAK);
}

void func_80A27970(EnBigpamet* this, PlayState* play2) {
    Vec3f sp9C;
    f32 temp_fs1 = this->actor.depthInWater + this->actor.world.pos.y;
    s32 bgId;
    s32 i;
    s16 temp_s0;
    f32 temp_fs0;
    CollisionPoly* sp84;
    PlayState* play = play2;

    for (i = 0; i < 2; i++) {
        temp_fs0 = Rand_ZeroFloat(30.0f) + 30.0f;
        temp_s0 = Rand_Next() >> 0x10;

        sp9C.x = (Math_SinS(temp_s0) * temp_fs0) + this->actor.world.pos.x;
        sp9C.y = Rand_ZeroFloat(10.0f) + this->actor.floorHeight + 8.0f;
        sp9C.z = (Math_CosS(temp_s0) * temp_fs0) + this->actor.world.pos.z;

        if (BgCheck_EntityRaycastFloor5_2(play, &play->colCtx, &sp84, &bgId, &this->actor, &sp9C) < temp_fs1) {
            sp9C.y = temp_fs1;
            EffectSsGSplash_Spawn(play, &sp9C, NULL, NULL, 0, Rand_S16Offset(1000, 200));
        } else {
            func_800B12F0(play, &sp9C, &D_80A2977C, &gZeroVec3f, Rand_S16Offset(950, 100), Rand_S16Offset(10, 10), 20);
        }
    }
}

void func_80A27B58(EnBigpamet* this) {
    EnBigpametStruct* ptr;
    s32 i;
    Vec3f sp64;
    f32 temp_fs0;
    s16 temp_s2;
    f32 temp_fs2;
    s16 temp_s1;
    f32 temp_fs3;
    s16 var_s1;

    sp64.x = (Math_SinS(this->actor.wallYaw + 0x8000) * 50.0f) + this->actor.world.pos.x;
    sp64.y = this->actor.world.pos.y;
    sp64.z = (Math_CosS(this->actor.wallYaw + 0x8000) * 50.0f) + this->actor.world.pos.z;

    this->unk_2A2 = 40;

    for (ptr = &this->unk_2FC[0], i = 0; i < ARRAY_COUNT(this->unk_2FC); i++, ptr++) {
        temp_s1 = (s32)Rand_CenteredFloat(0x5000) + this->actor.wallYaw;
        temp_s2 = Rand_S16Offset(0x1000, 0x3000);

        temp_fs2 = Math_SinS(temp_s2);
        temp_fs3 = Math_CosS(temp_s2);
        temp_fs0 = Rand_ZeroFloat(5.0f) + 7.0f;

        ptr->unk_0C.x = Math_SinS(temp_s1) * (temp_fs0 * temp_fs3);
        ptr->unk_0C.y = (temp_fs0 * temp_fs2) + 3.0f;
        ptr->unk_0C.z = Math_CosS(temp_s1) * (temp_fs0 * temp_fs3);

        ptr->unk_18.x = (s32)Rand_Next() >> 0x10;
        ptr->unk_18.y = (s32)Rand_Next() >> 0x10;
        ptr->unk_18.z = (s32)Rand_Next() >> 0x10;

        if ((s16)(temp_s1 - this->actor.wallYaw) > 0) {
            var_s1 = this->actor.wallYaw + 0x4000;
        } else {
            var_s1 = this->actor.wallYaw - 0x4000;
        }

        ptr->unk_00.x = (Math_SinS(var_s1) * (60.0f * temp_fs3)) + sp64.x;
        ptr->unk_00.y = (60.0f * temp_fs2) + sp64.y;
        ptr->unk_00.z = (Math_CosS(var_s1) * (60.0f * temp_fs3)) + sp64.z;

        ptr->unk_20 = Rand_ZeroFloat(0.0025000002f) + 0.002f;
    }

    Actor_PlaySfx(&this->actor, NA_SE_EN_B_PAMET_BREAK);
}

void func_80A27DD8(EnBigpamet* this, PlayState* play) {
    s32 i;
    Vec3f pos;
    Vec3f sp8C;
    f32 temp_fs0;
    f32 temp_fs4;
    f32 temp_fs5;
    s16 temp_s0;

    temp_fs4 = Math_SinS(this->actor.wallYaw + 0x4000);
    temp_fs5 = Math_CosS(this->actor.wallYaw + 0x4000);

    sp8C.x = Math_SinS(this->actor.wallYaw + 0x8000) * 50.0f + this->actor.world.pos.x;
    sp8C.y = this->actor.world.pos.y;
    sp8C.z = (Math_CosS(this->actor.wallYaw + 0x8000) * 50.0f) + this->actor.world.pos.z;

    sp8C.x += (this->actor.world.pos.x - sp8C.x) * 0.3f;
    sp8C.z += (this->actor.world.pos.z - sp8C.z) * 0.3f;

    for (i = 0; i < 4; i++) {
        temp_fs0 = Rand_ZeroFloat(60.0f) + 50.0f;
        temp_s0 = Rand_Next() >> 0x11;

        pos.x = (Math_CosS(temp_s0) * temp_fs0 * temp_fs4) + sp8C.x;
        pos.y = (Math_SinS(temp_s0) * temp_fs0) + sp8C.y;
        pos.z = (Math_CosS(temp_s0) * temp_fs0 * temp_fs5) + sp8C.z;

        func_800B12F0(play, &pos, &gZeroVec3f, &gZeroVec3f, Rand_S16Offset(950, 100), Rand_S16Offset(20, 10), 20);
    }
}

void func_80A27FE8(EnBigpamet* this, PlayState* play) {
    Vec3f pos;
    s16 sp32;

    if (this->actor.depthInWater > 0.0f) {
        pos.x = this->actor.world.pos.x;
        pos.z = this->actor.world.pos.z;
        pos.y = this->actor.world.pos.y + this->actor.depthInWater;

        EffectSsGRipple_Spawn(play, &pos, 500, 900, 0);
        pos.y += 8.0f;

        if (this->actionFunc != func_80A28D0C) {
            sp32 = (s32)Rand_CenteredFloat(0x8000) + this->actor.world.rot.y;
            pos.x -= 55.0f * Math_SinS(sp32);
            pos.z -= 55.0f * Math_CosS(sp32);
        }
        EffectSsGSplash_Spawn(play, &pos, NULL, NULL, 0, Rand_S16Offset(1400, 200));
    }
}

void func_80A2811C(EnBigpamet* this, PlayState* play) {
    Vec3f pos;

    if ((this->actor.depthInWater > 0.0f) && ((play->gameplayFrames % 14) == 0)) {
        pos.x = this->actor.world.pos.x;
        pos.y = this->actor.world.pos.y + this->actor.depthInWater;
        pos.z = this->actor.world.pos.z;

        EffectSsGRipple_Spawn(play, &pos, 500, 900, 0);
    }
}

void func_80A281B4(EnBigpamet* this) {
    this->actor.draw = NULL;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->actionFunc = func_80A281DC;
}

void func_80A281DC(EnBigpamet* this, PlayState* play) {
    WaterBox* waterBox;
    f32 sp28;

    if (this->actor.parent->params == GEKKO_GET_SNAPPER) {
        if (WaterBox_GetSurface1(play, &play->colCtx, this->actor.world.pos.x, this->actor.world.pos.z, &sp28,
                                 &waterBox)) {
            this->actor.depthInWater = sp28 - this->actor.world.pos.y;
        } else {
            this->actor.depthInWater = this->actor.world.pos.y;
        }

        func_80A2778C(this);
        func_80A27970(this, play);
        func_80A28274(this);
    }
}

void func_80A28274(EnBigpamet* this) {
    Animation_PlayLoop(&this->snapperSkelAnime, &gSnapperIdleAnim);
    this->actor.draw = EnBigpamet_Draw;
    this->unk_29E = 20;
    this->actionFunc = func_80A282C8;
}

void func_80A282C8(EnBigpamet* this, PlayState* play) {
    if (Math_SmoothStepToF(&this->actor.world.pos.y, this->actor.floorHeight, 0.7f, 30.0f, 10.0f) < 1.0f) {
        this->unk_29E--;
        if (this->unk_29E >= 16) {
            func_80A27970(this, play);
        }
        if (this->unk_29E == 0) {
            func_80A28378(this);
        }
    } else {
        func_80A27970(this, play);
    }
}

void func_80A28378(EnBigpamet* this) {
    this->actor.parent->params = GEKKO_INIT_SNAPPER;
    this->actor.speed = 0.0f;
    this->actionFunc = func_80A283A0;
}

void func_80A283A0(EnBigpamet* this, PlayState* play) {
    SkelAnime_Update(&this->snapperSkelAnime);
    if (this->actor.parent->params == GEKKO_ON_SNAPPER) {
        func_80A28E40(this);
    }
}

void func_80A283F0(EnBigpamet* this) {
    Animation_PlayLoop(&this->snapperSkelAnime, &gSnapperWalkAnim);
    this->actor.speed = 1.0f;
    this->actor.world.rot.y = this->actor.shape.rot.y;
    this->actor.params = ENBIGPAMET_1;
    this->actionFunc = func_80A2844C;
}

void func_80A2844C(EnBigpamet* this, PlayState* play) {
    SkelAnime_Update(&this->snapperSkelAnime);

    if (this->actor.parent->params == GEKKO_RETURN_TO_SNAPPER) {
        Math_ScaledStepToS(&this->actor.shape.rot.y, Actor_WorldYawTowardActor(&this->actor, this->actor.parent),
                           0x400);
        this->actor.world.rot.y = this->actor.shape.rot.y;
    } else if (this->actor.parent->params == GEKKO_JUMP_ON_SNAPPER) {
        this->actor.speed = 0.0f;
    } else if (this->actor.parent->params == GEKKO_ON_SNAPPER) {
        func_80A28E40(this);
    }
}

void func_80A284E4(EnBigpamet* this) {
    Animation_MorphToPlayOnce(&this->snapperSkelAnime, &gSnapperRetreatIntoShellAnim, -3.0f);
    this->unk_29E = 0;
    this->unk_2A8 = 1.0f;
    this->unk_2A4 = 1.0f;
    this->actor.speed = 0.0f;
    Actor_PlaySfx(&this->actor, NA_SE_EN_B_PAMET_VOICE);
    Actor_PlaySfx(this->actor.parent, NA_SE_EN_FROG_VOICE1);
    this->actionFunc = func_80A2855C;
}

void func_80A2855C(EnBigpamet* this, PlayState* play) {
    if (SkelAnime_Update(&this->snapperSkelAnime)) {
        func_80A28618(this);
    } else if (this->snapperSkelAnime.curFrame > 2.0f) {
        this->unk_2A8 = 1.5f - ((this->snapperSkelAnime.curFrame - 2.0f) * 0.23333333f);
        this->unk_2A4 = 1.5f - ((this->snapperSkelAnime.curFrame - 2.0f) * 0.083333336f);
    } else {
        f32 curFrame = this->snapperSkelAnime.curFrame;

        this->unk_2A4 = this->unk_2A8 = (0.25f * curFrame) + 1.0f;
    }
}

void func_80A28618(EnBigpamet* this) {
    this->actor.draw = func_80A2966C;
    this->unk_2A8 = 0.5f;
    this->actor.speed = 0.0f;
    Actor_PlaySfx(&this->actor, NA_SE_EN_PAMET_CUTTER_ON);
    this->actionFunc = func_80A2866C;
}

void func_80A2866C(EnBigpamet* this, PlayState* play) {
    this->unk_2A8 += 0.2f;
    if (this->unk_2A8 > 1.1f) {
        func_80A286C0(this);
    }
}

void func_80A286C0(EnBigpamet* this) {
    this->collider.elem.acDmgInfo.dmgFlags = 0xF7CF7FFF;
    this->collider.base.atFlags |= AT_ON;
    this->actor.shape.rot.z = 0x680;
    this->unk_29E = 15;
    this->unk_2A8 = 1.0f;
    this->actionFunc = func_80A28708;
}

void func_80A28708(EnBigpamet* this, PlayState* play) {
    this->unk_29E--;
    this->actor.shape.rot.y += 0x3B00;
    Actor_PlaySfx_Flagged(&this->actor, NA_SE_EN_B_PAMET_ROLL - SFX_FLAG);
    if (this->unk_29E == 0) {
        func_80A28760(this);
    }
}

void func_80A28760(EnBigpamet* this) {
    this->actor.speed = 15.0f;
    if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
        s16 temp_v1 = this->actor.yawTowardsPlayer - this->actor.wallYaw;

        this->actor.bgCheckFlags &= ~BGCHECKFLAG_WALL;

        if (temp_v1 > 0x3C00) {
            this->actor.world.rot.y = this->actor.wallYaw + 0x3C00;
        } else if (temp_v1 < -0x3C00) {
            this->actor.world.rot.y = this->actor.wallYaw - 0x3C00;
        } else {
            this->actor.world.rot.y = this->actor.yawTowardsPlayer;
        }
    } else {
        this->actor.world.rot.y = this->actor.yawTowardsPlayer;
    }
    this->unk_29E = 0;
    this->actionFunc = func_80A287E8;
}

void func_80A287E8(EnBigpamet* this, PlayState* play) {
    s16 quakeIndex;

    this->actor.shape.rot.y += 0x3B00;
    Actor_PlaySfx_Flagged(&this->actor, NA_SE_EN_B_PAMET_ROLL - SFX_FLAG);
    this->unk_29E++;
    this->unk_29E = CLAMP_MAX(this->unk_29E, 20);
    if (this->collider.base.atFlags & AT_HIT) {
        this->collider.base.atFlags &= ~AT_HIT;
        if ((s16)(this->actor.yawTowardsPlayer - this->actor.world.rot.y) > 0) {
            this->actor.world.rot.y -= 0x300;
        } else {
            this->actor.world.rot.y += 0x300;
        }
    }

    if (play->gameplayFrames & 1) {
        func_80A27FE8(this, play);
    }

    if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
        quakeIndex = Quake_Request(GET_ACTIVE_CAM(play), QUAKE_TYPE_3);

        this->actor.velocity.y = this->unk_29E * 0.375f;

        Quake_SetSpeed(quakeIndex, 20000);
        Quake_SetPerturbations(quakeIndex, 15, 0, 0, 0);
        Quake_SetDuration(quakeIndex, 10);

        Rumble_Request(this->actor.xyzDistToPlayerSq, 180, 20, 100);

        func_80A27B58(this);
        func_80A27DD8(this, play);
        func_80A28970(this);
    }
}

void func_80A28970(EnBigpamet* this) {
    Actor_PlaySfx(&this->actor, NA_SE_EN_PAMET_CUTTER_OFF);
    this->actor.shape.rot.z = 0;
    this->collider.base.atFlags &= ~AT_ON;
    this->collider.elem.acDmgInfo.dmgFlags = 0xF7CFFFFF;
    this->actor.speed = 0.0f;
    this->actionFunc = func_80A289C8;
}

void func_80A289C8(EnBigpamet* this, PlayState* play) {
    this->unk_2A8 -= 0.1f;
    if (this->unk_2A8 < 0.5f) {
        func_80A28A28(this);
    }
    func_80A27DD8(this, play);
}

void func_80A28A28(EnBigpamet* this) {
    this->actor.draw = EnBigpamet_Draw;
    Animation_MorphToPlayOnce(&this->snapperSkelAnime, &gSnapperEmergeFromShellAnim, -3.0f);
    this->unk_2A8 = 0.1f;
    this->unk_2A4 = 1.0f;
    this->actor.world.rot.y = this->actor.shape.rot.y;
    this->actionFunc = func_80A28A98;
}

void func_80A28A98(EnBigpamet* this, PlayState* play) {
    f32 curFrame;

    Math_ScaledStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 0x600);

    if (SkelAnime_Update(&this->snapperSkelAnime)) {
        if ((this->actor.parent->params == GEKKO_RETURN_TO_SNAPPER) ||
            (this->actor.parent->params == GEKKO_JUMP_ON_SNAPPER)) {
            func_80A283F0(this);
        } else {
            func_80A284E4(this);
        }
    } else {
        curFrame = this->snapperSkelAnime.curFrame;

        if (curFrame > 7.0f) {
            this->unk_2A8 = 1.5f - ((curFrame - 7.0f) * (1.0f / 6.0f));
            this->unk_2A4 = 1.5f - ((curFrame - 7.0f) * (1.0f / 6.0f));
        } else {
            this->unk_2A8 = (0.2f * curFrame) + 0.1f;
            this->unk_2A4 = ((1.0f / 14.0f) * curFrame) + 1.0f;
        }
    }
}

void func_80A28B98(EnBigpamet* this, PlayState* play) {
    Actor* collectible;

    Animation_PlayLoop(&this->snapperSkelAnime, &gSnapperDeathAnim);

    this->collider.base.atFlags &= ~AT_ON;
    this->collider.elem.acDmgInfo.dmgFlags = 0xF7CFFFFF;
    this->collider.base.atFlags &= ~(AT_HIT | AT_BOUNCED);
    this->collider.base.acFlags &= ~AC_ON;

    this->actor.velocity.y = 22.0f;
    this->actor.speed = 5.0f;

    if ((this->actor.draw == func_80A2966C) && (this->actionFunc != func_80A28DC0)) {
        this->actor.draw = EnBigpamet_Draw;
    } else if (this->collider.base.ac != NULL) {
        this->actor.world.rot.y = BINANG_ROT180(Actor_WorldYawTowardActor(&this->actor, this->collider.base.ac));
    }

    this->actor.shape.rot.y = this->actor.world.rot.y;
    this->actor.bgCheckFlags &= ~BGCHECKFLAG_GROUND;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->actor.params = ENBIGPAMET_0;

    if ((this->actor.parent->params == GEKKO_ON_SNAPPER) || (this->actor.parent->params == GEKKO_REAR_ON_SNAPPER) ||
        (this->actor.parent->params == GEKKO_JUMP_ON_SNAPPER)) {
        this->actor.parent->params = GEKKO_CUTSCENE;
    }

    collectible = Item_DropCollectible(play, &this->actor.world.pos, ITEM00_ARROWS_10);
    if (collectible != NULL) {
        collectible->velocity.y = 15.0f;
        collectible->world.rot.y = Actor_WorldYawTowardPoint(&this->actor, &this->actor.home.pos);
    }

    Actor_PlaySfx(&this->actor, NA_SE_EN_B_PAMET_REVERSE);
    this->actionFunc = func_80A28D0C;
}

void func_80A28D0C(EnBigpamet* this, PlayState* play) {
    SkelAnime_Update(&this->snapperSkelAnime);
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_HIPLOOP_LAND);
        func_80A27FE8(this, play);
        func_80A28D80(this);
    }
    this->actor.shape.rot.x += 0x800;
    this->actor.shape.rot.z += 0x1000;
}

void func_80A28D80(EnBigpamet* this) {
    this->actor.draw = func_80A2966C;
    this->actor.speed = 0.0f;
    this->actor.shape.rot.x = 0;
    this->actor.shape.rot.z = 0;
    this->unk_2A8 = 0.0f;
    this->collider.base.acFlags |= AC_ON;
    this->actionFunc = func_80A28DC0;
}

void func_80A28DC0(EnBigpamet* this, PlayState* play) {
    this->actor.shape.rot.x = (s32)Rand_Next() >> 0x16;
    this->actor.shape.rot.z = (s32)Rand_Next() >> 0x16;
    if ((this->actor.parent->params == GEKKO_RETURN_TO_SNAPPER) ||
        (this->actor.parent->params == GEKKO_JUMP_ON_SNAPPER)) {
        func_80A28A28(this);
    } else if (this->actor.parent->params == GEKKO_DEFEAT) {
        func_80A28ED4(this);
    }
}

void func_80A28E40(EnBigpamet* this) {
    Animation_MorphToPlayOnce(&this->snapperSkelAnime, &gSnapperRearUpAnim, -2.0f);
    this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    this->actor.speed = 0.0f;
    this->actionFunc = func_80A28E98;
}

void func_80A28E98(EnBigpamet* this, PlayState* play) {
    if (SkelAnime_Update(&this->snapperSkelAnime)) {
        func_80A284E4(this);
    }
}

void func_80A28ED4(EnBigpamet* this) {
    this->actionFunc = func_80A28EE8;
}

void func_80A28EE8(EnBigpamet* this, PlayState* play) {
    Vec3f pos;
    s16 sp3A;

    this->actor.shape.rot.x = (s32)Rand_Next() >> 0x16;
    this->actor.shape.rot.z = (s32)Rand_Next() >> 0x16;

    if (Math_StepToF(&this->actor.scale.x, 0.0f, 0.00075f)) {
        sp3A = BINANG_ROT180(Camera_GetCamDirYaw(GET_ACTIVE_CAM(play)));

        pos.x = (Math_SinS(sp3A) * 20.0f) + this->actor.world.pos.x;
        pos.y = this->actor.world.pos.y + 25.0f;
        pos.z = (Math_CosS(sp3A) * 20.0f) + this->actor.world.pos.z;

        func_800B0DE0(play, &pos, &gZeroVec3f, &gZeroVec3f, &D_80A29788, &D_80A2978C, 800, 50);
        SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 50, NA_SE_EN_NPC_FADEAWAY);
        Actor_Kill(&this->actor);
    }

    this->actor.scale.y = this->actor.scale.x;
    this->actor.scale.z = this->actor.scale.x;
}

void func_80A29028(EnBigpamet* this, PlayState* play) {
    if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;
        if ((this->actor.colChkInfo.damageEffect == 0xF) && (this->actor.parent->params != GEKKO_JUMP_ON_SNAPPER) &&
            (this->actionFunc != func_80A28E98)) {
            func_80A28B98(this, play);
        }
    }
}

void func_80A29094(EnBigpamet* this) {
    EnBigpametStruct* ptr = &this->unk_2FC[0];
    s32 i;

    for (i = 0; i < ARRAY_COUNT(this->unk_2FC); i++, ptr++) {
        ptr->unk_0C.y -= 1.0f;

        Math_Vec3f_Sum(&ptr->unk_00, &ptr->unk_0C, &ptr->unk_00);

        ptr->unk_18.x += (s16)((Rand_Next() >> 0x17) + 0x700);
        ptr->unk_18.y += (s16)((Rand_Next() >> 0x17) + 0x900);
        ptr->unk_18.z += (s16)((Rand_Next() >> 0x17) + 0xB00);
    }
}

void EnBigpamet_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnBigpamet* this = (EnBigpamet*)thisx;

    func_80A2768C(this);
    func_80A29028(this, play);

    this->actionFunc(this, play);

    if (this->unk_2A2 > 0) {
        this->unk_2A2--;
        func_80A29094(this);
    }

    if ((this->actionFunc != func_80A281DC) && (this->actionFunc != func_80A282C8)) {
        Actor_MoveWithGravity(&this->actor);
        Actor_UpdateBgCheckInfo(play, &this->actor, 40.0f, 50.0f, 60.0f,
                                UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_4 |
                                    UPDBGCHECKINFO_FLAG_8 | UPDBGCHECKINFO_FLAG_10);
        func_80A276F4(this);
        Actor_SetFocus(&this->actor, 25.0f);

        Collider_UpdateCylinder(&this->actor, &this->collider);

        if (this->collider.base.atFlags & AT_ON) {
            CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
        }

        if (this->collider.base.acFlags & AC_ON) {
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
        }

        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);

        if (this->actionFunc != func_80A287E8) {
            func_80A2811C(this, play);
        }
    }
}

void func_80A292A8(EnBigpamet* this, PlayState* play) {
    s32 i;
    EnBigpametStruct* ptr;

    if (this->unk_2A2 > 0) {
        OPEN_DISPS(play->state.gfxCtx);

        Gfx_SetupDL25_Opa(play->state.gfxCtx);

        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0x80, 255, 255, 255, 255);
        gDPSetEnvColor(POLY_OPA_DISP++, 255, 255, 255, 255);

        for (ptr = &this->unk_2FC[0], i = 0; i < ARRAY_COUNT(this->unk_2FC); i++, ptr++) {
            Matrix_SetTranslateRotateYXZ(ptr->unk_00.x, ptr->unk_00.y, ptr->unk_00.z, &ptr->unk_18);
            Matrix_Scale(ptr->unk_20, ptr->unk_20, ptr->unk_20, MTXMODE_APPLY);

            MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
            gSPDisplayList(POLY_OPA_DISP++, gameplay_keep_DL_06AB30);
        }

        CLOSE_DISPS(play->state.gfxCtx);
    }
}

s32 EnBigpamet_OverrideLimbDraw2(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnBigpamet* this = (EnBigpamet*)thisx;

    if ((this->actionFunc == func_80A2855C) || (this->actionFunc == func_80A28A98)) {
        if (limbIndex == SNAPPER_LIMB_HEAD) {
            Matrix_Scale(this->unk_2A4, this->unk_2A8, this->unk_2A4, MTXMODE_APPLY);
        } else if ((limbIndex == SNAPPER_LIMB_BACK_RIGHT_LEG) || (limbIndex == SNAPPER_LIMB_BACK_LEFT_LEG) ||
                   (limbIndex == SNAPPER_LIMB_FRONT_RIGHT_LEG) || (limbIndex == SNAPPER_LIMB_FRONT_LEFT_LEG)) {
            Matrix_Scale(this->unk_2A4, this->unk_2A8, this->unk_2A8, MTXMODE_APPLY);
        }
    }
    return false;
}

void EnBigpamet_PostLimbDraw2(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    EnBigpamet* this = (EnBigpamet*)thisx;

    if (limbIndex == SNAPPER_LIMB_BODY) {
        this->unk_2AC = Matrix_GetCurrent()->yw;
    }
}

void EnBigpamet_Draw(Actor* thisx, PlayState* play) {
    EnBigpamet* this = (EnBigpamet*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, D_80A29754[this->unk_29C]);

    SkelAnime_DrawFlexOpa(play, this->snapperSkelAnime.skeleton, this->snapperSkelAnime.jointTable,
                          this->snapperSkelAnime.dListCount, EnBigpamet_OverrideLimbDraw2, EnBigpamet_PostLimbDraw2,
                          &this->actor);
    func_80A292A8(this, play);

    CLOSE_DISPS(play->state.gfxCtx);
}

s32 EnBigpamet_OverrideLimbDraw1(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnBigpamet* this = (EnBigpamet*)thisx;

    if (limbIndex == SPIKED_SNAPPER_LIMB_BODY) {
        if (this->actionFunc == func_80A28D0C) {
            pos->y -= 2500.0f;
        } else {
            pos->y -= 700.0f;
        }
    }

    if ((this->unk_2A8 != 1.0f) && (limbIndex == SPIKED_SNAPPER_LIMB_SPIKES)) {
        Matrix_Scale(1.0f, this->unk_2A8, this->unk_2A8, MTXMODE_APPLY);
    }

    return false;
}

void EnBigpamet_PostLimbDraw1(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    EnBigpamet* this = (EnBigpamet*)thisx;

    if (limbIndex == SPIKED_SNAPPER_LIMB_BODY) {
        this->unk_2AC = Matrix_GetCurrent()->yw;
    }
}

void func_80A2966C(Actor* thisx, PlayState* play) {
    EnBigpamet* this = (EnBigpamet*)thisx;

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    SkelAnime_DrawFlexOpa(play, this->spikedSnapperSkelAnime.skeleton, this->spikedSnapperSkelAnime.jointTable,
                          this->spikedSnapperSkelAnime.dListCount, EnBigpamet_OverrideLimbDraw1,
                          EnBigpamet_PostLimbDraw1, &this->actor);
    func_80A292A8(this, play);
}
