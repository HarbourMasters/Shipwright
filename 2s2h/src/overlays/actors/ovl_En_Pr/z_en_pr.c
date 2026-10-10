/*
 * File: z_en_pr.c
 * Overlay: ovl_En_Pr
 * Description: Desbreko
 */

#include "z_en_pr.h"
#include "overlays/actors/ovl_En_Clear_Tag/z_en_clear_tag.h"
#include "overlays/actors/ovl_En_Prz/z_en_prz.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void EnPr_Init(Actor* thisx, PlayState* play2);
void EnPr_Destroy(Actor* thisx, PlayState* play);
void EnPr_Update(Actor* thisx, PlayState* play);
void EnPr_Draw(Actor* thisx, PlayState* play);

void func_80A326F0(EnPr* this);
void func_80A32740(EnPr* this, PlayState* play);
void func_80A32854(EnPr* this);
void func_80A3289C(EnPr* this, PlayState* play);
void func_80A3295C(EnPr* this);
void func_80A32984(EnPr* this, PlayState* play);
void func_80A32A40(EnPr* this, PlayState* play);
void func_80A32AF8(EnPr* this);
void func_80A32B20(EnPr* this, PlayState* play);
void func_80A32CDC(EnPr* this);
void func_80A32D28(EnPr* this, PlayState* play);
void func_80A32EA4(EnPr* this, PlayState* play);
void func_80A32F48(EnPr* this, PlayState* play);

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
    /* Fire arrow     */ DMG_ENTRY(0, 0x0),
    /* Ice arrow      */ DMG_ENTRY(0, 0x0),
    /* Light arrow    */ DMG_ENTRY(2, 0x4),
    /* Goron spikes   */ DMG_ENTRY(0, 0x0),
    /* Deku spin      */ DMG_ENTRY(0, 0x0),
    /* Deku bubble    */ DMG_ENTRY(1, 0xF),
    /* Deku launch    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, 0x0),
    /* Zora barrier   */ DMG_ENTRY(1, 0x5),
    /* Normal shield  */ DMG_ENTRY(0, 0x0),
    /* Light ray      */ DMG_ENTRY(0, 0x0),
    /* Thrown object  */ DMG_ENTRY(1, 0xF),
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

f32 D_80A338C0[PLAYER_FORM_MAX] = {
    30.0f, // PLAYER_FORM_FIERCE_DEITY
    30.0f, // PLAYER_FORM_GORON
    30.0f, // PLAYER_FORM_ZORA
    15.0f, // PLAYER_FORM_DEKU
    15.0f, // PLAYER_FORM_HUMAN
};

ActorProfile En_Pr_Profile = {
    /**/ ACTOR_EN_PR,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_PR,
    /**/ sizeof(EnPr),
    /**/ EnPr_Init,
    /**/ EnPr_Destroy,
    /**/ EnPr_Update,
    /**/ EnPr_Draw,
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
    { 18, 20, 5, { 0, 0, 0 } },
};

typedef enum EnPrAnimation {
    /* 0 */ ENPR_ANIM_0,
    /* 1 */ ENPR_ANIM_1,
    /* 2 */ ENPR_ANIM_2,
    /* 3 */ ENPR_ANIM_3,
    /* 4 */ ENPR_ANIM_4,
    /* 5 */ ENPR_ANIM_MAX
} EnPrAnimation;

static AnimationHeader* sAnimations[ENPR_ANIM_MAX] = {
    &object_pr_Anim_0021E8, // ENPR_ANIM_0
    &object_pr_Anim_001E10, // ENPR_ANIM_1
    &object_pr_Anim_0021E8, // ENPR_ANIM_2
    &object_pr_Anim_000740, // ENPR_ANIM_3
    &object_pr_Anim_000268, // ENPR_ANIM_4
};

static u8 sAnimationModes[ENPR_ANIM_MAX] = {
    ANIMMODE_LOOP, // ENPR_ANIM_0
    ANIMMODE_ONCE, // ENPR_ANIM_1
    ANIMMODE_LOOP, // ENPR_ANIM_2
    ANIMMODE_LOOP, // ENPR_ANIM_3
    ANIMMODE_ONCE, // ENPR_ANIM_4
};

void EnPr_Init(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnPr* this = (EnPr*)thisx;
    EnPrz* prz;
    s32 i;
    s32 temp_s4;

    this->actor.colChkInfo.health = 3;
    this->actor.colChkInfo.mass = 50;
    this->actor.hintId = TATL_HINT_ID_DESBREKO;

    Collider_InitAndSetCylinder(play, &this->collider, &this->actor, &sCylinderInit);

    this->unk_208 = 255;
    this->unk_2CC = 0.01f;

    Actor_SetScale(&this->actor, 0.01f);

    this->actor.attentionRangeType = ATTENTION_RANGE_3;
    this->unk_2B8 = this->actor.world.pos.y;
    this->actor.shape.yOffset = 1500.0f;
    this->actor.colChkInfo.damageTable = &sDamageTable;

    SkelAnime_InitFlex(play, &this->skelAnime, &object_pr_Skel_0038B8, &object_pr_Anim_0021E8, this->jointTable,
                       this->morphTable, OBJECT_PR_1_LIMB_MAX);
    this->unk_2C8 = this->actor.world.rot.z * 20.0f;

    if (this->unk_2C8 < 80.0f) {
        this->unk_2C8 = 80.0f;
    }
    this->actor.world.rot.z = 0;

    temp_s4 = ENPR_GET_FF00(&this->actor);
    if (temp_s4 > ENPR_FF00_MAX) {
        temp_s4 = ENPR_FF00_MAX;
    } else if (temp_s4 < ENPR_FF00_MIN) {
        temp_s4 = ENPR_FF00_MIN;
    }

    this->unk_2F8 = &this->actor;

    for (i = 0; i < temp_s4; i++) {
        prz = (EnPrz*)Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EN_PRZ, this->actor.world.pos.x,
                                         this->actor.world.pos.y, this->actor.world.pos.z, 0, this->actor.world.rot.y,
                                         0, i + 1);
        if (prz != NULL) {
            prz->unk_220 = this->unk_2F8;
            this->unk_2F8 = &prz->actor;
        }
    }

    this->unk_2D0 = 255;
    this->unk_20E = 30;
    this->unk_216 = this->actor.world.rot.y;
    this->unk_214 = this->actor.world.rot.y;
    this->unk_2B4 = this->actor.world.pos.y;

    func_80A326F0(this);
}

void EnPr_Destroy(Actor* thisx, PlayState* play) {
    EnPr* this = (EnPr*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void EnPr_ChangeAnim(EnPr* this, s32 animIndex) {
    f32 playSpeed;

    this->animIndex = animIndex;
    playSpeed = 1.0f;
    this->animEndFrame = Animation_GetLastFrame(sAnimations[animIndex]);

    if (this->animIndex == ENPR_ANIM_2) {
        playSpeed = 2.0f;
    }

    Animation_Change(&this->skelAnime, sAnimations[this->animIndex], playSpeed, 0.0f, this->animEndFrame,
                     sAnimationModes[this->animIndex], -2.0f);
}

s32 func_80A324E0(EnPr* this, PlayState* play) {
    CollisionPoly* sp54;
    Vec3f sp48;
    s32 bgId;
    WaterBox* waterBox;

    if (BgCheck_EntityLineTest1(&play->colCtx, &this->actor.world.pos, &this->unk_2E0, &sp48, &sp54, 1, 0, 0, 1,
                                &bgId)) {
        return 1;
    }

    if (WaterBox_GetSurface1(play, &play->colCtx, this->actor.world.pos.x, this->actor.world.pos.z, &this->unk_2B4,
                             &waterBox)) {
        if ((this->unk_2B4 - 30.0f) < this->actor.world.pos.y) {
            this->unk_2B8 = this->unk_2B4 - 30.0f;
            return 2;
        }
    }

    if (this->actor.world.pos.y < (this->actor.floorHeight + 20.0f)) {
        this->unk_2B8 = this->actor.floorHeight + 20.0f;
        return 2;
    }
    return 0;
}

s32 func_80A325E4(EnPr* this) {
    s16 temp = this->unk_214 * 0.2f;
    s32 abs = ABS_ALT(temp) & 0xFFFF;

    Math_SmoothStepToS(&this->actor.world.rot.y, this->unk_22C, 1, abs + 1, 0);
    this->unk_216 = this->actor.world.rot.y - this->unk_22C;

    if (this->unk_216 > 10000) {
        this->unk_216 = 10000;
    } else if (this->unk_216 < -10000) {
        this->unk_216 = -10000;
    }

    if (ABS_ALT(BINANG_SUB(this->actor.world.rot.y, this->unk_22C)) > 0x100) {
        return true;
    }
    this->unk_216 = 0;
    return false;
}

void func_80A326F0(EnPr* this) {
    if (this->animIndex != ENPR_ANIM_0) {
        EnPr_ChangeAnim(this, ENPR_ANIM_0);
    }
    this->unk_206 = 0;
    this->actor.speed = 1.0f;
    this->actionFunc = func_80A32740;
}

void func_80A32740(EnPr* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    switch (func_80A324E0(this, play)) {
        case 1:
            func_80A3295C(this);
            return;

        case 2:
            this->unk_206 = 3;
            this->actionFunc = func_80A32A40;
            break;
    }

    if (this->unk_2C8 < sqrtf(SQ(this->actor.world.pos.x - this->actor.home.pos.x) +
                              SQ(this->actor.world.pos.z - this->actor.home.pos.z))) {
        func_80A32854(this);
    } else if ((this->unk_20E == 0) && (player->stateFlags1 & PLAYER_STATE1_8000000)) {
        if (sqrtf(SQ(player->actor.world.pos.x - this->actor.home.pos.x) +
                  SQ(player->actor.world.pos.z - this->actor.home.pos.z)) < this->unk_2C8) {
            func_80A32AF8(this);
        }
    }
}

void func_80A32854(EnPr* this) {
    if (this->animIndex != ENPR_ANIM_0) {
        EnPr_ChangeAnim(this, ENPR_ANIM_0);
    }
    this->unk_206 = 1;
    this->actionFunc = func_80A3289C;
}

void func_80A3289C(EnPr* this, PlayState* play) {
    this->unk_22C = Math_Vec3f_Yaw(&this->actor.world.pos, &this->actor.home.pos);
    func_80A325E4(this);

    if (sqrtf(SQ(this->actor.world.pos.x - this->actor.home.pos.x) +
              SQ(this->actor.world.pos.z - this->actor.home.pos.z)) < (((BREG(53) * 0.1f) + 0.5f) * this->unk_2C8)) {
        this->unk_206 = 3;
        this->actionFunc = func_80A32A40;
    }
}

void func_80A3295C(EnPr* this) {
    this->unk_22C = BINANG_ADD(this->actor.world.rot.y, 0x4000);
    this->unk_206 = 2;
    this->actionFunc = func_80A32984;
}

void func_80A32984(EnPr* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if ((player->stateFlags1 & PLAYER_STATE1_8000000) && (this->unk_20E == 0)) {
        this->unk_22C = this->actor.world.rot.y;
        func_80A32AF8(this);
    } else if (!func_80A325E4(this)) {
        if (func_80A324E0(this, play)) {
            this->unk_22C += 0x1000;
        } else {
            if (this->animIndex != ENPR_ANIM_0) {
                EnPr_ChangeAnim(this, ENPR_ANIM_0);
            }
            this->unk_206 = 3;
            this->actionFunc = func_80A32A40;
        }
    }
}

void func_80A32A40(EnPr* this, PlayState* play) {
    Vec3f sp34;
    WaterBox* waterBox;

    Math_Vec3f_Copy(&sp34, &this->actor.world.pos);
    sp34.y = Rand_CenteredFloat(50.0f) + this->actor.home.pos.y;

    if (WaterBox_GetSurface1(play, &play->colCtx, this->actor.world.pos.x, this->actor.world.pos.z, &this->unk_2B4,
                             &waterBox)) {
        if (sp34.y < (this->unk_2B4 - 30.0f)) {
            this->unk_2B8 = sp34.y;
        } else {
            this->unk_2B8 = this->unk_2B4 - 30.0f;
        }
    }

    this->unk_206 = 0;
    this->actionFunc = func_80A32740;
}

void func_80A32AF8(EnPr* this) {
    this->unk_206 = 4;
    this->skelAnime.playSpeed = 2.0f;
    this->actionFunc = func_80A32B20;
}

void func_80A32B20(EnPr* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    this->actor.speed = BREG(57) + 3.0f;
    Math_SmoothStepToS(&this->unk_22C, this->actor.yawTowardsPlayer, 1 + BREG(49), 0x3E8 + BREG(50), BREG(51));
    this->unk_2B8 = D_80A338C0[GET_PLAYER_FORM] + player->actor.world.pos.y;
    func_80A324E0(this, play);

    if (!(player->stateFlags1 & PLAYER_STATE1_8000000)) {
        this->skelAnime.playSpeed = 1.0f;
        this->actor.speed = 1.0f;
        func_80A32854(this);
    } else if (this->unk_2C8 < sqrtf(SQ(player->actor.world.pos.x - this->actor.home.pos.x) +
                                     SQ(player->actor.world.pos.z - this->actor.home.pos.z))) {
        this->skelAnime.playSpeed = 1.0f;
        this->actor.speed = 1.0f;
        func_80A32854(this);
    } else if (!func_80A325E4(this) && (this->actor.xzDistToPlayer < 200.0f) &&
               (fabsf(player->actor.world.pos.y - this->actor.world.pos.y) < 80.0f)) {
        this->skelAnime.playSpeed = 1.0f;
        func_80A32CDC(this);
    }
}

void func_80A32CDC(EnPr* this) {
    EnPr_ChangeAnim(this, ENPR_ANIM_3);
    this->unk_206 = 5;
    this->unk_20A = 400;
    this->unk_2C0 = 0.0f;
    this->actionFunc = func_80A32D28;
}

void func_80A32D28(EnPr* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (!(player->stateFlags1 & PLAYER_STATE1_8000000)) {
        this->skelAnime.playSpeed = 1.0f;
        this->actor.speed = 1.0f;
        func_80A32854(this);
    } else {
        Math_SmoothStepToS(&this->unk_22C, this->actor.yawTowardsPlayer, 1 + BREG(49), 0x3E8 + BREG(50), BREG(51));
        func_80A325E4(this);
        this->unk_2B8 = D_80A338C0[GET_PLAYER_FORM] + player->actor.world.pos.y;
        func_80A324E0(this, play);
        if (this->unk_2C8 < sqrtf(SQ(player->actor.world.pos.x - this->actor.home.pos.x) +
                                  SQ(player->actor.world.pos.z - this->actor.home.pos.z))) {
            this->skelAnime.playSpeed = 1.0f;
            this->actor.speed = 1.0f;
            func_80A32854(this);
        }
    }
}

void func_80A32E60(EnPr* this) {
    EnPr_ChangeAnim(this, ENPR_ANIM_4);
    this->unk_206 = 6;
    this->actor.speed = 0.0f;
    this->actionFunc = func_80A32EA4;
}

void func_80A32EA4(EnPr* this, PlayState* play) {
    f32 curFrame = this->skelAnime.curFrame;

    if (curFrame >= this->animEndFrame) {
        if (this->actor.colChkInfo.health <= 0) {
            this->unk_206 = 7;
            this->unk_20A = 50;
            this->actor.flags |= ACTOR_FLAG_LOCK_ON_DISABLED;
            this->unk_2C4 = 0.0f;
            Enemy_StartFinishingBlow(play, &this->actor);
            Actor_PlaySfx(&this->actor, NA_SE_EN_BUBLEWALK_DEAD);
            this->unk_216 = 0;
            this->actionFunc = func_80A32F48;
        } else {
            func_80A326F0(this);
        }
    }
}

void func_80A32F48(EnPr* this, PlayState* play) {
    WaterBox* waterBox;

    if (this->unk_208 > 0) {
        this->unk_208 -= 2;
    } else {
        this->unk_208 = 0;
    }

    if (WaterBox_GetSurface1(play, &play->colCtx, this->actor.world.pos.x, this->actor.world.pos.z, &this->unk_2B4,
                             &waterBox)) {
        if ((this->unk_2B4 - 100.0f) < this->actor.world.pos.y) {
            this->unk_212 += 0xBB8;
            this->unk_2C4 = 2.0f * Math_SinS(this->unk_212);
        }
        Math_ApproachF(&this->actor.world.pos.y, (this->unk_2B4 - 16.0f) + this->unk_2C4, 0.5f, 2.0f);
        Math_SmoothStepToS(&this->actor.world.rot.z, 0x7700, 0x12C, 0x3E8, 0x3E8);
    }

    if ((this->unk_20A == 0) && (this->unk_208 == 0)) {
        this->unk_2D2 = 1;
    }

    if (this->unk_2D2 != 0) {
        Math_SmoothStepToS(&this->unk_2D0, 0, 1, 15, 50);
        if (this->unk_2D0 < 2) {
            Actor_Kill(&this->actor);
        }
    }
}

void func_80A33098(EnPr* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if ((this->unk_206 != 7) && (this->unk_206 != 6)) {
        if (this->collider.base.acFlags & AC_HIT) {
            this->collider.base.acFlags &= ~AC_HIT;
            if (this->actor.colChkInfo.damageEffect == 4) {
                this->drawDmgEffAlpha = 40;
                this->drawDmgEffType = ACTOR_DRAW_DMGEFF_LIGHT_ORBS;
                Actor_Spawn(&play->actorCtx, play, ACTOR_EN_CLEAR_TAG, this->actor.focus.pos.x, this->actor.focus.pos.y,
                            this->actor.focus.pos.z, 0, 0, 0, CLEAR_TAG_PARAMS(CLEAR_TAG_LARGE_LIGHT_RAYS));
            }

            if ((player->stateFlags1 & PLAYER_STATE1_8000000) && (this->actor.colChkInfo.damageEffect == 5)) {
                this->drawDmgEffAlpha = 40;
                this->drawDmgEffType = ACTOR_DRAW_DMGEFF_ELECTRIC_SPARKS_MEDIUM;
            }

            if ((this->unk_206 != 6) && (this->unk_206 != 7)) {
                Actor_ApplyDamage(&this->actor);
                Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 8);
                Actor_PlaySfx(&this->actor, NA_SE_EN_BUBLEWALK_DAMAGE);
                func_80A32E60(this);
            }
        }
    }
}

void EnPr_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnPr* this = (EnPr*)thisx;
    s32 i;

    SkelAnime_Update(&this->skelAnime);

    if (this->unk_20A != 0) {
        this->unk_20A--;
    }

    if (this->unk_20E != 0) {
        this->unk_20E--;
    }

    if (this->unk_20C != 0) {
        this->unk_20C--;
    }

    if (this->unk_210 != 0) {
        this->unk_210--;
    }

    this->actionFunc(this, play);

    Actor_SetFocus(&this->actor, 20.0f);
    func_80A33098(this, play);
    Math_Vec3f_Copy(&this->unk_2E0, &this->actor.world.pos);

    this->unk_2E0.x += (Math_SinS(this->actor.world.rot.y) * 70.0f);
    this->unk_2E0.y = this->unk_2D4.y - 10.0f;
    this->unk_2E0.z += Math_CosS(this->actor.world.rot.y) * 70.0f;

    Math_SmoothStepToS(&this->unk_214, this->unk_216, 1, 0x7D0, 0);

    if (this->unk_210 == 0) {
        this->unk_210 = Rand_S16Offset(10, 30);
        this->unk_204 = 1;
    }

    Actor_MoveWithGravity(&this->actor);

    if (this->unk_206 != 7) {
        Vec3f sp50;
        s16 pitch;

        Math_ApproachF(&this->actor.world.pos.y, this->unk_2B8, 0.3f, BREG(11) + 1.0f);
        Math_Vec3f_Copy(&sp50, &this->actor.world.pos);
        sp50.y = this->unk_2B8;
        pitch = Math_Vec3f_Pitch(&this->actor.world.pos, &sp50) * 0.3f;

        if (fabsf(this->actor.world.pos.y - this->unk_2B8) > 8.0f) {
            Math_SmoothStepToS(&this->actor.world.rot.x, pitch, 1, 0x1F4 + BREG(48), 0);
        } else {
            Math_SmoothStepToS(&this->actor.world.rot.x, 0, 1, 0x1F4 + BREG(52), 0);
        }
    }

    Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 30.0f, 20.0f,
                            UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_8 |
                                UPDBGCHECKINFO_FLAG_10);
    Math_Vec3s_Copy(&this->actor.shape.rot, &this->actor.world.rot);

    if (this->unk_206 != 7) {
        Collider_UpdateCylinder(&this->actor, &this->collider);
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);

        if (this->unk_204 != 0) {
            Vec3f sp40;

            Math_Vec3f_Copy(&sp40, &this->unk_2D4);
            this->unk_204 = 0;
            sp40.x += Rand_CenteredFloat(20.0f);
            sp40.y += Rand_CenteredFloat(5.0f);
            sp40.z += Rand_CenteredFloat(20.0f);

            for (i = 0; i < (s32)Rand_ZeroFloat(5.0f) + 5; i++) {
                EffectSsBubble_Spawn(play, &sp40, 0.0f, 5.0f, 5.0f, Rand_ZeroFloat(0.03f) + 0.07f);
            }
        }
    }
}

s32 EnPr_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnPr* this = (EnPr*)thisx;

    if (limbIndex == OBJECT_PR_1_LIMB_02) {
        rot->y += this->unk_214;
    }
    return false;
}

void EnPr_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    Vec3f sp24 = { 0.0f, 0.0f, 0.0f };
    EnPr* this = (EnPr*)thisx;

    if (limbIndex == OBJECT_PR_1_LIMB_02) {
        Matrix_Translate(0.0f, 0.0f, 0.0f, MTXMODE_APPLY);
        Matrix_MultVec3f(&sp24, &this->unk_2D4);
    }

    if ((limbIndex == OBJECT_PR_1_LIMB_NONE) || (limbIndex == OBJECT_PR_1_LIMB_01) ||
        (limbIndex == OBJECT_PR_1_LIMB_02) || (limbIndex == OBJECT_PR_1_LIMB_03) ||
        (limbIndex == OBJECT_PR_1_LIMB_04) || (limbIndex == OBJECT_PR_1_LIMB_05) ||
        (limbIndex == OBJECT_PR_1_LIMB_06) || (limbIndex == OBJECT_PR_1_LIMB_07) ||
        (limbIndex == OBJECT_PR_1_LIMB_08) || (limbIndex == OBJECT_PR_1_LIMB_09)) {
        Matrix_MultZero(&this->bodyPartsPos[this->bodyPartsCount]);
        this->bodyPartsCount++;
        if (this->bodyPartsCount >= ENPR_BODYPART_MAX) {
            this->bodyPartsCount = 0;
        }
    }
}

void EnPr_Draw(Actor* thisx, PlayState* play) {
    EnPr* this = (EnPr*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    if (this->unk_2D2 == 0) {
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, this->unk_208, this->unk_208, this->unk_208, this->unk_2D0);
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, this->unk_2D0);

        Scene_SetRenderModeXlu(play, 0, 1);
        SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                              EnPr_OverrideLimbDraw, EnPr_PostLimbDraw, &this->actor);
    } else {
        gDPPipeSync(POLY_XLU_DISP++);
        gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, this->unk_2D0);

        Scene_SetRenderModeXlu(play, 1, 2);
        POLY_XLU_DISP = SkelAnime_DrawFlex(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                           this->skelAnime.dListCount, NULL, NULL, &this->actor, POLY_XLU_DISP);
    }

    if (this->drawDmgEffAlpha != 0) {
        //! @bug: `this->drawDmgEffAlpha` is not decremented, so once active; the damage effect will remain until the
        //! actor is killed. This only affects zora barrier as the only other damage effect, light arrows, kills in one
        //! hit.
        f32 drawDmgEffAlpha = this->drawDmgEffAlpha * 0.05f;

        this->unk_238 = 0.8f;
        this->unk_234 = 0.8f;
        Actor_DrawDamageEffects(play, &this->actor, this->bodyPartsPos, ENPR_BODYPART_MAX, 0.8f, 0.8f, drawDmgEffAlpha,
                                this->drawDmgEffType);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}
