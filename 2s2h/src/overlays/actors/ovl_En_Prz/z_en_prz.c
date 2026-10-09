/*
 * File: z_en_prz.c
 * Overlay: ovl_En_Prz
 * Description: Skullfish - Defeated
 */

#include "z_en_prz.h"
#include "overlays/actors/ovl_En_Pr/z_en_pr.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void EnPrz_Init(Actor* thisx, PlayState* play);
void EnPrz_Destroy(Actor* thisx, PlayState* play);
void EnPrz_Update(Actor* thisx, PlayState* play);
void EnPrz_Draw(Actor* thisx, PlayState* play);

void func_80A76388(EnPrz* this);
void func_80A763E8(EnPrz* this, PlayState* play);
void func_80A76604(EnPrz* this, PlayState* play);
void func_80A76634(EnPrz* this, PlayState* play);
void func_80A76748(EnPrz* this);
void func_80A767A8(EnPrz* this, PlayState* play);
void func_80A76B14(EnPrz* this, PlayState* play);

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, 0x0),
    /* Deku Stick     */ DMG_ENTRY(0, 0x0),
    /* Horse trample  */ DMG_ENTRY(0, 0x0),
    /* Explosives     */ DMG_ENTRY(1, 0xF),
    /* Zora boomerang */ DMG_ENTRY(1, 0xF),
    /* Normal arrow   */ DMG_ENTRY(1, 0xF),
    /* UNK_DMG_0x06   */ DMG_ENTRY(0, 0x0),
    /* Hookshot       */ DMG_ENTRY(1, 0xF),
    /* Goron punch    */ DMG_ENTRY(0, 0x0),
    /* Sword          */ DMG_ENTRY(0, 0x0),
    /* Goron pound    */ DMG_ENTRY(0, 0x0),
    /* Fire arrow     */ DMG_ENTRY(1, 0xF),
    /* Ice arrow      */ DMG_ENTRY(1, 0xF),
    /* Light arrow    */ DMG_ENTRY(2, 0xF),
    /* Goron spikes   */ DMG_ENTRY(0, 0x0),
    /* Deku spin      */ DMG_ENTRY(0, 0x0),
    /* Deku bubble    */ DMG_ENTRY(1, 0xF),
    /* Deku launch    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, 0x0),
    /* Zora barrier   */ DMG_ENTRY(1, 0xF),
    /* Normal shield  */ DMG_ENTRY(0, 0x0),
    /* Light ray      */ DMG_ENTRY(0, 0x0),
    /* Thrown object  */ DMG_ENTRY(0, 0x0),
    /* Zora punch     */ DMG_ENTRY(1, 0xF),
    /* Spin attack    */ DMG_ENTRY(0, 0x0),
    /* Sword beam     */ DMG_ENTRY(0, 0x0),
    /* Normal Roll    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, 0x0),
    /* Unblockable    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, 0x0),
    /* Powder Keg     */ DMG_ENTRY(1, 0xF),
};

f32 D_80A771E0[PLAYER_FORM_MAX] = {
    40.0f, // PLAYER_FORM_FIERCE_DEITY
    40.0f, // PLAYER_FORM_GORON
    40.0f, // PLAYER_FORM_ZORA
    30.0f, // PLAYER_FORM_DEKU
    30.0f, // PLAYER_FORM_HUMAN
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK4,
        { 0x20000000, 0x00, 0x04 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_NONE,
    },
    { 10, 10, 0, { 0, 0, 0 } },
};

ActorProfile En_Prz_Profile = {
    /**/ ACTOR_EN_PRZ,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_PR,
    /**/ sizeof(EnPrz),
    /**/ EnPrz_Init,
    /**/ EnPrz_Destroy,
    /**/ EnPrz_Update,
    /**/ EnPrz_Draw,
};

void EnPrz_Init(Actor* thisx, PlayState* play) {
    EnPrz* this = (EnPrz*)thisx;

    this->unk_20C = 0.01f;
    this->unk_1E4 = this->actor.world.rot.y;

    Actor_SetScale(&this->actor, 0.01f);

    this->actor.attentionRangeType = ATTENTION_RANGE_3;
    this->unk_1E8 = 255;
    this->actor.hintId = TATL_HINT_ID_SKULLFISH;
    this->actor.colChkInfo.damageTable = &sDamageTable;
    this->actor.colChkInfo.health = 1;

    SkelAnime_InitFlex(play, &this->skelAnime, &object_pr_Skel_004188, &object_pr_Anim_004340, this->jointTable,
                       this->morphTable, OBJECT_PR_2_LIMB_MAX);

    this->unk_1E6 = ENPRZ_GET(&this->actor);
    this->actor.shape.yOffset = 500.0f;
    this->actor.flags |= ACTOR_FLAG_LOCK_ON_DISABLED;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;

    Collider_InitAndSetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    Math_Vec3f_Copy(&this->unk_1D8, &this->actor.world.pos);

    this->unk_220 = &this->actor;
    this->unk_1EC = 255;
    this->unk_202 = Rand_CenteredFloat((this->unk_1E6 * 100.0f) + 1000.0f);

    func_80A76388(this);
}

void EnPrz_Destroy(Actor* thisx, PlayState* play) {
}

typedef enum EnPrzAnimation {
    /* 0 */ ENPRZ_ANIM_0,
    /* 1 */ ENPRZ_ANIM_1,
    /* 2 */ ENPRZ_ANIM_MAX
} EnPrzAnimation;

static AnimationHeader* sAnimations[ENPRZ_ANIM_MAX] = {
    &object_pr_Anim_004340, // ENPRZ_ANIM_0
    &object_pr_Anim_004274, // ENPRZ_ANIM_1
};

static u8 sAnimationModes[ENPRZ_ANIM_MAX] = {
    ANIMMODE_LOOP, // ENPRZ_ANIM_0
    ANIMMODE_LOOP, // ENPRZ_ANIM_1
};

void EnPrz_ChangeAnim(EnPrz* this, s32 animIndex) {
    this->animIndex = animIndex;
    Animation_Change(&this->skelAnime, sAnimations[animIndex], 1.0f, 0.0f,
                     Animation_GetLastFrame(sAnimations[animIndex]), sAnimationModes[animIndex], -2.0f);
}

s32 func_80A75FA4(EnPrz* this, PlayState* play) {
    WaterBox* waterBox;
    f32 temp_f0;

    if (WaterBox_GetSurface1(play, &play->colCtx, this->actor.world.pos.x, this->actor.world.pos.z, &this->unk_210,
                             &waterBox)) {
        temp_f0 = BREG(10) + (this->unk_210 - 10.0f);
        if (temp_f0 < this->actor.world.pos.y) {
            this->unk_1D8.y = temp_f0;
            return 1;
        }
    } else {
        temp_f0 = this->actor.floorHeight + 35.0f;
        if (this->actor.world.pos.y < temp_f0) {
            this->unk_1D8.y = temp_f0;
            return 2;
        }
    }

    return 0;
}

void func_80A76070(EnPrz* this, s16 arg1, PlayState* play) {
    s16 temp_s0 = arg1 - this->actor.world.rot.y;

    temp_s0 *= 0.01f;

    if (temp_s0 > 100) {
        temp_s0 = 100;
    }

    if (temp_s0 < -100) {
        temp_s0 = -100;
    }

    if (fabsf(this->actor.world.rot.y - (f32)arg1) < (Rand_CenteredFloat(20.0f) + 100.0f)) {
        Math_ApproachZeroF(&this->unk_218, 0.5f, 20.0f);
    } else {
        Math_ApproachF(&this->unk_218, temp_s0, 0.5f, 5.0f);
    }

    temp_s0 = fabsf(this->unk_218) * 0.1f;
    if (temp_s0 < 2) {
        temp_s0 = 2;
    }

    Math_SmoothStepToS(&this->actor.world.rot.y, arg1, temp_s0, this->unk_202 + 0x1388, 0);
    func_80A75FA4(this, play);
    Math_ApproachF(&this->actor.world.pos.y, this->unk_1D8.y, 0.5f, 1.0f);
    temp_s0 = Math_Vec3f_Pitch(&this->actor.world.pos, &this->unk_1D8) * 0.5f;

    if (fabsf(this->actor.world.pos.y - this->unk_208) > 4.0f) {
        Math_SmoothStepToS(&this->actor.world.rot.x, temp_s0, 1, BREG(48) + 0x1F4, 0);
    } else {
        Math_SmoothStepToS(&this->actor.world.rot.x, 0, 1, BREG(52) + 0xBB8, 0);
    }
}

s32 func_80A762C0(EnPrz* this, PlayState* play) {
    CollisionPoly* sp54;
    Vec3f sp48;
    s32 bgId;
    Vec3f sp38;

    Math_Vec3f_Copy(&sp38, &this->actor.world.pos);

    sp38.x += Math_SinS(this->actor.world.rot.y) * 40.0f;
    sp38.z += Math_CosS(this->actor.world.rot.y) * 40.0f;

    if (BgCheck_EntityLineTest1(&play->colCtx, &this->actor.world.pos, &sp38, &sp48, &sp54, true, false, false, true,
                                &bgId)) {
        return true;
    }
    return false;
}

void func_80A76388(EnPrz* this) {
    this->actor.speed = Rand_CenteredFloat(1.0f) + 4.0f;
    EnPrz_ChangeAnim(this, ENPRZ_ANIM_0);
    this->unk_1EA = 1;
    this->actionFunc = func_80A763E8;
}

void func_80A763E8(EnPrz* this, PlayState* play) {
    Actor* sp3C = this->actor.parent;
    Player* player = GET_PLAYER(play);
    s16 yaw;
    s32 pad;

    if (&this->actor == this->unk_220) {
        return;
    }

    if ((player->stateFlags1 & PLAYER_STATE1_8000000) && (this->unk_1F2 == 0)) {
        func_80A76748(this);
        return;
    }

    if (func_80A762C0(this, play)) {
        func_80A76604(this, play);
        return;
    }

    if ((this->unk_220 != NULL) && (this->unk_220->update == NULL)) {
        this->unk_220 = this->actor.parent;
    }

    if ((this->unk_1EE == 0) && (this->unk_1F2 == 0)) {
        s16 rot;

        this->unk_214 = Rand_CenteredFloat(40.0f);

        rot = BINANG_ROT180(this->unk_220->world.rot.y);
        this->unk_1D8.x += Math_SinS(rot) * this->unk_214;
        this->unk_1D8.y = Rand_CenteredFloat(40.0f) + (this->unk_220->world.pos.y + 40.0f);
        rot = BINANG_ROT180(this->unk_220->world.rot.y);
        this->unk_1D8.z += Math_CosS(rot) * this->unk_214;

        this->unk_1EE = Rand_S16Offset(20, 30);
    } else if (sqrtf(SQ(this->actor.world.pos.x - sp3C->world.pos.x) +
                     SQ(this->actor.world.pos.z - sp3C->world.pos.z)) > 100.0f) {
        Math_Vec3f_Copy(&this->unk_1D8, &sp3C->world.pos);
    }

    //! FAKE:
    if (1) {}

    if ((Rand_ZeroOne() < 0.1f) && (this->unk_1C8 == 0)) {
        this->unk_1C8 = 1;
    }

    yaw = Math_Vec3f_Yaw(&this->actor.world.pos, &this->unk_1D8);
    func_80A76070(this, yaw, play);
}

void func_80A76604(EnPrz* this, PlayState* play) {
    this->unk_1EE = 0;
    this->unk_1E4 = BINANG_ADD(this->actor.world.rot.y, 0x4000);
    this->unk_1EA = 2;
    this->actionFunc = func_80A76634;
}

void func_80A76634(EnPrz* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    func_80A76070(this, this->unk_1E4, play);

    if (ABS_ALT(BINANG_SUB(this->actor.world.rot.y, this->unk_1E4)) < 0x100) {
        if (func_80A762C0(this, play) != 0) {
            this->unk_1E4 += 0x1500;
            this->unk_1E4 += TRUNCF_BINANG(Rand_ZeroFloat(5000.0f));
        } else if ((player->stateFlags1 & PLAYER_STATE1_8000000) && (player->actor.floorHeight < 30.0f)) {
            this->actionFunc = func_80A763E8;
        } else {
            this->unk_1EE = 10;
            this->unk_1EA = 1;
            this->actionFunc = func_80A763E8;
        }
    }
}

void func_80A76748(EnPrz* this) {
    this->actor.speed = Rand_CenteredFloat(1.0f) + 3.0f;
    this->unk_1EE = 0;
    this->unk_1EA = 3;
    this->skelAnime.playSpeed = 2.0f;
    this->actionFunc = func_80A767A8;
}

void func_80A767A8(EnPrz* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    EnPr* pr = (EnPr*)this->actor.parent;
    f32 distXZ;
    s32 pad[2];

    if (func_80A762C0(this, play)) {
        EnPrz_ChangeAnim(this, ENPRZ_ANIM_0);
        this->actor.speed = Rand_CenteredFloat(1.0f) + 4.0f;
        func_80A76604(this, play);
        return;
    }

    distXZ = sqrtf(SQ(player->actor.world.pos.x - this->actor.parent->home.pos.x) +
                   SQ(player->actor.world.pos.z - this->actor.parent->home.pos.z));

    if (!(player->stateFlags1 & PLAYER_STATE1_8000000) || (pr->unk_2C8 < distXZ)) {
        this->unk_1F2 = 100;
        this->skelAnime.playSpeed = 1.0f;
        func_80A76388(this);
        return;
    }

    if ((this->actor.xzDistToPlayer < 200.0f) && (fabsf(player->actor.world.pos.y - this->actor.world.pos.y) < 80.0f)) {
        if (this->animIndex != ENPRZ_ANIM_1) {
            EnPrz_ChangeAnim(this, ENPRZ_ANIM_1);
        }
        this->skelAnime.playSpeed = 1.0f;
    } else {
        if (this->animIndex != ENPRZ_ANIM_0) {
            EnPrz_ChangeAnim(this, ENPRZ_ANIM_0);
        }
        this->skelAnime.playSpeed = 2.0f;
    }

    if (this->unk_1EE == 0) {
        this->unk_1EE = Rand_S16Offset(2, 3);
        Math_Vec3f_Copy(&this->unk_1D8, &player->actor.world.pos);
    }

    if ((Rand_ZeroOne() < 0.1f) && (this->unk_1C8 == 0)) {
        this->unk_1C8 = 1;
    }

    this->unk_1D8.y =
        (player->actor.world.pos.y + D_80A771E0[GET_PLAYER_FORM]) + Rand_CenteredFloat((2.0f * this->unk_1E6) + 1.0f);
    func_80A76070(this, Math_Vec3f_Yaw(&this->actor.world.pos, &this->unk_1D8), play);
}

void func_80A76A1C(EnPrz* this) {
    this->unk_1E8 = 0;
    this->actor.flags |= ACTOR_FLAG_LOCK_ON_DISABLED;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;

    Actor_PlaySfx(&this->actor, NA_SE_EN_BUBLEWALK_DEAD);

    this->unk_1EE = Rand_S16Offset(100, 30);
    this->unk_1FC = 0x4000;

    if (Rand_ZeroOne() < 0.5f) {
        this->unk_1FC = -0x4000;
    }

    this->unk_1FE = this->actor.world.rot.y;
    this->actor.speed = Rand_ZeroFloat(0.5f);
    this->actor.world.rot.y = Rand_CenteredFloat(0x8000);

    Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 30);
    this->unk_1EE = 50;
    EnPrz_ChangeAnim(this, ENPRZ_ANIM_0);
    this->unk_1EA = 7;
    this->actionFunc = func_80A76B14;
}

void func_80A76B14(EnPrz* this, PlayState* play) {
    WaterBox* waterBox;
    s32 i;
    Vec3f sp6C;
    s32 phi_s0 = false;

    Math_SmoothStepToS(&this->actor.shape.rot.x, 0, 5, 0x2710, 0x3E8);
    Math_SmoothStepToS(&this->actor.shape.rot.z, this->unk_1FC, 5, 0x2710, 0x3E8);
    Math_SmoothStepToS(&this->actor.shape.rot.y, this->unk_1FE, 5, 0x2710, 0x3E8);

    if (WaterBox_GetSurface1(play, &play->colCtx, this->actor.world.pos.x, this->actor.world.pos.z, &this->unk_210,
                             &waterBox)) {
        if ((this->unk_210 - 15.0f) <= this->actor.world.pos.y) {
            phi_s0 = true;
        } else {
            Math_ApproachF(&this->actor.world.pos.y, this->unk_210 - 14.0f, 0.3f, 1.0f);
        }
    }

    if ((this->unk_1EE == 0) || phi_s0) {
        Math_SmoothStepToS(&this->unk_1EC, 0, 1, 15, 50);

        if (this->unk_1EC < 2) {
            for (i = 0; i < 10; i++) {
                Math_Vec3f_Copy(&sp6C, &this->actor.world.pos);

                sp6C.x += Rand_CenteredFloat(20.0f);
                sp6C.y += Rand_CenteredFloat(5.0f);
                sp6C.z += Rand_CenteredFloat(20.0f);

                EffectSsBubble_Spawn(play, &sp6C, 0.0f, 5.0f, 5.0f, Rand_ZeroFloat(0.03f) + 0.07f);
            }
            Actor_Kill(&this->actor);
        }
    }
}

void EnPrz_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnPrz* this = (EnPrz*)thisx;
    s32 sp44 = false;
    Vec3f sp38;

    if (this->unk_1EA != 7) {
        SkelAnime_Update(&this->skelAnime);
    }

    this->actionFunc(this, play);

    if (this->unk_1EE != 0) {
        this->unk_1EE--;
    }

    if (this->unk_1F2 != 0) {
        this->unk_1F2--;
    }

    Actor_MoveWithGravity(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 10.0f, 10.0f,
                            UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_8 |
                                UPDBGCHECKINFO_FLAG_10);

    if (this->unk_1EA != 7) {
        Math_Vec3s_Copy(&this->actor.shape.rot, &this->actor.world.rot);
    }

    if (this->unk_1EA != 7) {
        if (this->collider.base.acFlags & AC_HIT) {
            Actor_ApplyDamage(&this->actor);
            if (this->actor.colChkInfo.health <= 0) {
                sp44 = true;
            }
        }

        if (sp44 || (this->actor.parent == NULL) || (this->actor.parent->colChkInfo.health <= 0)) {
            this->collider.base.acFlags &= ~AC_HIT;
            if (this->unk_1EA != 7) {
                func_80A76A1C(this);
                return;
            }
        }

        Collider_UpdateCylinder(&this->actor, &this->collider);
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);

        if (this->unk_1C8 != 0) {
            Math_Vec3f_Copy(&sp38, &this->unk_1CC);
            this->unk_1C8 = 0;

            sp38.x += Rand_CenteredFloat(20.0f);
            sp38.y += Rand_CenteredFloat(5.0f);
            sp38.z += Rand_CenteredFloat(20.0f);

            EffectSsBubble_Spawn(play, &sp38, 0.0f, 5.0f, 5.0f, Rand_ZeroFloat(0.03f) + 0.07f);
        }
    }
}

s32 EnPrz_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnPrz* this = (EnPrz*)thisx;

    if (limbIndex == OBJECT_PR_2_LIMB_02) {
        rot->y += TRUNCF_BINANG(this->unk_218) * -100;
    }
    return false;
}

void EnPrz_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    Vec3f sZeroVec = { 0.0f, 0.0f, 0.0f };
    EnPrz* this = (EnPrz*)thisx;

    if (limbIndex == OBJECT_PR_2_LIMB_02) {
        Matrix_Translate(0.0f, 0.0f, 0.0f, MTXMODE_APPLY);
        Matrix_MultVec3f(&sZeroVec, &this->unk_1CC);
    }
}

void EnPrz_Draw(Actor* thisx, PlayState* play) {
    EnPrz* this = (EnPrz*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    if (this->unk_1EC == 255) {
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, this->unk_1E8, this->unk_1E8, this->unk_1E8, this->unk_1EC);
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, this->unk_1EC);

        Scene_SetRenderModeXlu(play, 0, 1);
        SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                              EnPrz_OverrideLimbDraw, EnPrz_PostLimbDraw, &this->actor);
    } else {
        gDPPipeSync(POLY_XLU_DISP++);
        gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, this->unk_1EC);

        Scene_SetRenderModeXlu(play, 1, 2);
        POLY_XLU_DISP = SkelAnime_DrawFlex(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                           this->skelAnime.dListCount, NULL, NULL, &this->actor, POLY_XLU_DISP);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}
