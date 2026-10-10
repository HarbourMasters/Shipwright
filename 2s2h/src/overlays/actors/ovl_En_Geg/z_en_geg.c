/*
 * File: z_en_geg.c
 * Overlay: ovl_En_Geg
 * Description: Goron With Don Gero's Mask
 */

#include "z_en_geg.h"
#include "overlays/actors/ovl_En_Bom/z_en_bom.h"
#include "overlays/effects/ovl_Effect_Ss_Hahen/z_eff_ss_hahen.h"
#include "objects/object_taisou/object_taisou.h"
#include "objects/object_hakugin_demo/object_hakugin_demo.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "GameInteractor/GameInteractor.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void EnGeg_Init(Actor* thisx, PlayState* play);
void EnGeg_Destroy(Actor* thisx, PlayState* play);
void EnGeg_Update(Actor* thisx, PlayState* play);
void EnGeg_Draw(Actor* thisx, PlayState* play);

void func_80BB217C(EnGeg* this, PlayState* play);
void func_80BB221C(EnGeg* this, PlayState* play);
void func_80BB2520(EnGeg* this, PlayState* play);
void func_80BB26EC(EnGeg* this, PlayState* play);
void func_80BB27D4(EnGeg* this, PlayState* play);
void func_80BB2944(EnGeg* this, PlayState* play);
void func_80BB2A54(EnGeg* this, PlayState* play);
void func_80BB2B1C(EnGeg* this, PlayState* play);
void func_80BB2E00(EnGeg* this, PlayState* play);
void func_80BB2F7C(EnGeg* this, PlayState* play);
void func_80BB30B4(EnGeg* this, PlayState* play);
void func_80BB31B8(EnGeg* this, PlayState* play);
void func_80BB32AC(EnGeg* this, PlayState* play);
void func_80BB3318(EnGeg* this, PlayState* play);
void func_80BB347C(EnGeg* this, PlayState* play);

ActorProfile En_Geg_Profile = {
    /**/ ACTOR_EN_GEG,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_OF1D_MAP,
    /**/ sizeof(EnGeg),
    /**/ EnGeg_Init,
    /**/ EnGeg_Destroy,
    /**/ EnGeg_Update,
    /**/ EnGeg_Draw,
};

static ColliderSphereInit sSphereInit = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_SPHERE,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x20000000, 0x00, 0x04 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 0, { { 0, 0, 0 }, 0 }, 100 },
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_HIT1,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK1,
        { 0x00000000, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 0, 0, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 sColChkInfoInit = { 0, 0, 0, 0, MASS_IMMOVABLE };

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(1, 0x0),
    /* Deku Stick     */ DMG_ENTRY(1, 0x0),
    /* Horse trample  */ DMG_ENTRY(1, 0x0),
    /* Explosives     */ DMG_ENTRY(1, 0x0),
    /* Zora boomerang */ DMG_ENTRY(1, 0x0),
    /* Normal arrow   */ DMG_ENTRY(1, 0x0),
    /* UNK_DMG_0x06   */ DMG_ENTRY(1, 0x0),
    /* Hookshot       */ DMG_ENTRY(1, 0x0),
    /* Goron punch    */ DMG_ENTRY(1, 0x0),
    /* Sword          */ DMG_ENTRY(1, 0x0),
    /* Goron pound    */ DMG_ENTRY(1, 0x0),
    /* Fire arrow     */ DMG_ENTRY(1, 0x0),
    /* Ice arrow      */ DMG_ENTRY(1, 0x0),
    /* Light arrow    */ DMG_ENTRY(1, 0x0),
    /* Goron spikes   */ DMG_ENTRY(1, 0x0),
    /* Deku spin      */ DMG_ENTRY(1, 0x0),
    /* Deku bubble    */ DMG_ENTRY(1, 0x0),
    /* Deku launch    */ DMG_ENTRY(1, 0x0),
    /* UNK_DMG_0x12   */ DMG_ENTRY(1, 0x0),
    /* Zora barrier   */ DMG_ENTRY(1, 0x0),
    /* Normal shield  */ DMG_ENTRY(1, 0x0),
    /* Light ray      */ DMG_ENTRY(1, 0x0),
    /* Thrown object  */ DMG_ENTRY(1, 0x0),
    /* Zora punch     */ DMG_ENTRY(1, 0x0),
    /* Spin attack    */ DMG_ENTRY(1, 0x0),
    /* Sword beam     */ DMG_ENTRY(1, 0x0),
    /* Normal Roll    */ DMG_ENTRY(1, 0x0),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(1, 0x0),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(1, 0x0),
    /* Unblockable    */ DMG_ENTRY(1, 0x0),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(1, 0x0),
    /* Powder Keg     */ DMG_ENTRY(1, 0x0),
};

typedef enum {
    /*   -1 */ ENGEG_ANIM_NONE = -1,
    /* 0x00 */ ENGEG_ANIM_0,
    /* 0x01 */ ENGEG_ANIM_1,
    /* 0x02 */ ENGEG_ANIM_2,
    /* 0x03 */ ENGEG_ANIM_3,
    /* 0x04 */ ENGEG_ANIM_4,
    /* 0x05 */ ENGEG_ANIM_5,
    /* 0x06 */ ENGEG_ANIM_6,
    /* 0x07 */ ENGEG_ANIM_7,
    /* 0x08 */ ENGEG_ANIM_8,
    /* 0x09 */ ENGEG_ANIM_9,
    /* 0x0A */ ENGEG_ANIM_10,
    /* 0x0B */ ENGEG_ANIM_11,
    /* 0x0C */ ENGEG_ANIM_12,
    /* 0x0D */ ENGEG_ANIM_13,
    /* 0x0E */ ENGEG_ANIM_14,
    /* 0x0F */ ENGEG_ANIM_15,
    /* 0x10 */ ENGEG_ANIM_16,
    /* 0x11 */ ENGEG_ANIM_17,
    /* 0x12 */ ENGEG_ANIM_18,
    /* 0x13 */ ENGEG_ANIM_19,
    /* 0x14 */ ENGEG_ANIM_20,
    /* 0x15 */ ENGEG_ANIM_MAX
} EnGegAnimation;

static AnimationInfoS sAnimationInfo[ENGEG_ANIM_MAX] = {
    { &gGoronLyingDownIdleAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },                     // ENGEG_ANIM_0
    { &gGoronLyingDownIdleAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -4 },                    // ENGEG_ANIM_1
    { &gGoronUnrollAnim, 2.0f, 0, -1, ANIMMODE_ONCE, 0 },                            // ENGEG_ANIM_2
    { &gGoronUnrollAnim, 2.0f, 0, -1, ANIMMODE_ONCE, -4 },                           // ENGEG_ANIM_3
    { &gGoronUnrollAnim, -2.0f, 0, -1, ANIMMODE_ONCE, -4 },                          // ENGEG_ANIM_4
    { &gGoronShiverAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },                            // ENGEG_ANIM_5
    { &gGoronShiverAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -4 },                           // ENGEG_ANIM_6
    { &gGoronDropKegAnim, 1.0f, 0, -1, ANIMMODE_ONCE, -4 },                          // ENGEG_ANIM_7
    { &gGoronAthleticsSquatSideToSideAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },          // ENGEG_ANIM_8
    { &gGoronAthleticsDoubleArmSideBendAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },        // ENGEG_ANIM_9
    { &gGoronAthleticsShakeLimbsAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },               // ENGEG_ANIM_10
    { &gGoronAthleticsSingleArmSideBendAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },        // ENGEG_ANIM_11
    { &gGoronAthleticsHamstringStretchSittingAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },  // ENGEG_ANIM_12
    { &gGoronAthleticsCheerAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },                    // ENGEG_ANIM_13
    { &gGoronAthleticsShoutAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },                    // ENGEG_ANIM_14
    { &gGoronAthleticsHamstringStretchStandingAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 }, // ENGEG_ANIM_15
    { &gGoronCoverEarsAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },                         // ENGEG_ANIM_16
    { &gGoronSpringLookAroundAnim, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },                  // ENGEG_ANIM_17
    { &gGoronSpringLookAroundLoopAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -4 },             // ENGEG_ANIM_18
    { &gGoronShiveringSurprisedAnim, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },                // ENGEG_ANIM_19
    { &gGoronStandingIdleAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },                      // ENGEG_ANIM_20
};

u16 func_80BB16D0(EnGeg* this) {
    switch (this->unk_496) {
        case 0xD5E:
            return 0xD5F;

        case 0xD5F:
            return 0xD60;

        case 0xD60:
            return 0xD61;

        case 0xD62:
            return 0xD63;

        case 0xD64:
            return 0xD65;

        case 0xD66:
            return 0xD67;

        case 0xD67:
            return 0xD68;

        case 0xD68:
            return 0xD69;

        case 0xD6A:
            return 0xD6B;

        case 0xD6B:
            return 0xD6C;

        case 0xD6C:
            return 0xD6D;

        case 0xD6E:
            return 0xD6F;

        case 0xD70:
            return 0xD71;

        case 0xD71:
            return 0xD72;

        case 0xD73:
            return 0xD74;

        case 0xD74:
            return 0xD75;

        case 0xD89:
            return 0xD8A;

        default:
            return 0;
    }
}

void func_80BB178C(EnGeg* this, PlayState* play) {
    Vec3f sp34 = this->actor.world.pos;
    Collider* collider;

    if (this->unk_230 & 1) {
        this->colliderSphere.dim.worldSphere.center.x = sp34.x;
        this->colliderSphere.dim.worldSphere.center.y = sp34.y;
        this->colliderSphere.dim.worldSphere.center.y += TRUNCF_BINANG(this->actor.shape.yOffset);
        this->colliderSphere.dim.worldSphere.center.z = sp34.z;
        this->colliderSphere.dim.modelSphere.radius = 20;
        this->colliderSphere.dim.worldSphere.radius =
            this->colliderSphere.dim.modelSphere.radius * this->colliderSphere.dim.scale;
        collider = &this->colliderSphere.base;
    } else {
        f32 radius = 24.0f;
        f32 height = 62.0f;

        this->colliderCylinder.dim.pos.x = sp34.x;
        this->colliderCylinder.dim.pos.y = sp34.y;
        this->colliderCylinder.dim.pos.z = sp34.z;
        this->colliderCylinder.dim.radius = radius;
        this->colliderCylinder.dim.height = height;
        collider = &this->colliderCylinder.base;
    }

    if (collider != NULL) {
        CollisionCheck_SetOC(play, &play->colChkCtx, collider);
        Actor_UpdateBgCheckInfo(play, &this->actor, 30.0f, 12.0f, 0.0f, UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4);
    }
}

s32 func_80BB18FC(EnGeg* this, Actor* actor) {
    if (actor->bgCheckFlags & BGCHECKFLAG_GROUND) {
        f32 sp24 = Math_Vec3f_DistXZ(&this->actor.world.pos, &actor->world.pos);
        f32 sp20 = Math_Vec3f_DiffY(&this->actor.world.pos, &actor->world.pos);

        if ((sp24 < 150.0f) && (fabsf(sp20) < 5.0f)) {
            this->unk_230 |= 0x20;
            actor->speed = 0.0f;
            actor->velocity.y = 0.0f;
            this->actor.child = actor;
            actor->parent = &this->actor;
            return true;
        }
    }
    return false;
}

Vec3f func_80BB19C0(EnGeg* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Vec3f sp40;
    Vec3f sp34;
    s16 sp32 = player->actor.world.rot.y + 0x4000;
    s16 sp30;
    f32 sp2C;
    f32 sp28;

    sp40.x = (Math_SinS(sp32) * 50.0f) + player->actor.world.pos.x;
    sp40.y = player->actor.world.pos.y;
    sp40.z = (Math_CosS(sp32) * 50.0f) + player->actor.world.pos.z;

    sp2C = Math_Vec3f_DistXZ(&this->actor.world.pos, &sp40);
    sp30 = player->actor.world.rot.y - 0x4000;

    sp34.x = (Math_SinS(sp30) * 50.0f) + player->actor.world.pos.x;
    sp34.y = player->actor.world.pos.y;
    sp34.z = (Math_CosS(sp30) * 50.0f) + player->actor.world.pos.z;

    if (Math_Vec3f_DistXZ(&this->actor.world.pos, &sp34) < sp2C) {
        return sp40;
    } else {
        return sp34;
    }
}

u8 func_80BB1B14(EnGeg* this, PlayState* play) {
    Actor* explosive;
    Actor* mm = SubS_FindActor(play, NULL, ACTORCAT_ITEMACTION, ACTOR_EN_MM);

    if (mm != NULL) {
        this->unk_4B0 = Math_Vec3f_Yaw(&this->actor.world.pos, &mm->world.pos);
        if (func_80BB18FC(this, mm)) {
            return 1;
        }
    }

    explosive = play->actorCtx.actorLists[ACTORCAT_EXPLOSIVES].first;

    while (explosive != NULL) {
        if ((explosive->id == ACTOR_EN_BOM) && func_80BB18FC(this, explosive)) {
            this->unk_4B0 = Math_Vec3f_Yaw(&this->actor.world.pos, &explosive->world.pos);
            if (this->unk_230 & 0x200) {
                return 0;
            }

            if (((EnBom*)explosive)->isPowderKeg == 0) {
                return 2;
            }

            return 3;
        }
        explosive = explosive->next;
    }

    this->unk_230 &= ~0x200;
    return 0;
}

void func_80BB1C1C(EnGeg* this) {
    u16 i;

    this->csIdList[0] = this->actor.csId;

    for (i = 1; i < ARRAY_COUNT(this->csIdList); i++) {
        this->csIdList[i] = CutsceneManager_GetAdditionalCsId(this->csIdList[i - 1]);
    }
}

void func_80BB1C8C(EnGeg* this) {
    if (DECR(this->unk_240) == 0) {
        this->unk_23E++;
        if (this->unk_23E >= 3) {
            this->unk_240 = Rand_S16Offset(30, 30);
            this->unk_23E = 0;
        }
    }
}

void func_80BB1D04(EnGeg* this) {
    f32 temp;

    if (this->unk_230 & 1) {
        temp = this->actor.shape.yOffset;
    } else {
        temp = 58.0f;
    }

    this->actor.focus.pos.x = this->actor.world.pos.x;
    this->actor.focus.pos.y = temp + this->actor.world.pos.y;
    this->actor.focus.pos.z = this->actor.world.pos.z;
    this->actor.focus.rot.x = this->actor.world.rot.x;
    this->actor.focus.rot.y = this->actor.world.rot.y;
    this->actor.focus.rot.z = this->actor.world.rot.z;
}

s32 func_80BB1D64(EnGeg* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Vec3f sp40;
    Vec3f sp34;
    s16 yaw = (this->actor.yawTowardsPlayer - this->unk_46E) - this->actor.shape.rot.y;

    Math_SmoothStepToS(&this->unk_46A, yaw, 4, 0x2AA8, 1);
    this->unk_46A = CLAMP(this->unk_46A, -0x1FFE, 0x1FFE);

    yaw = (this->actor.yawTowardsPlayer - this->unk_46A) - this->actor.shape.rot.y;
    Math_SmoothStepToS(&this->unk_46E, yaw, 4, 0x2AA8, 1);
    this->unk_46E = CLAMP(this->unk_46E, -0x1C70, 0x1C70);

    if (this->unk_230 & 0x20) {
        sp40 = player->actor.world.pos;
    } else {
        sp40 = player->actor.world.pos;
        sp40.y = player->bodyPartsPos[PLAYER_BODYPART_HEAD].y + 3.0f;
    }

    sp34 = this->actor.world.pos;
    sp34.y += 70.0f;

    yaw = Math_Vec3f_Pitch(&sp34, &sp40);
    Math_SmoothStepToS(&this->unk_468, yaw - this->unk_46C, 4, 0x2AA8, 1);
    this->unk_468 = CLAMP(this->unk_468, -0x1C70, 0x1C70);

    yaw = Math_Vec3f_Pitch(&sp34, &sp40);
    Math_SmoothStepToS(&this->unk_46C, yaw - this->unk_468, 4, 0x2AA8, 1);
    this->unk_46C = CLAMP(this->unk_46C, -0x1C70, 0x1C70);

    return true;
}

void EnGeg_UpdateSkelAnime(EnGeg* this, PlayState* play) {
    gSegments[6] = OS_K0_TO_PHYSICAL(play->objectCtx.slots[this->objectSlot].segment);
    SkelAnime_Update(&this->skelAnime);
}

void EnGeg_ChangeAnim(EnGeg* this, PlayState* play) {
    gSegments[6] = OS_K0_TO_PHYSICAL(play->objectCtx.slots[this->objectSlot].segment);
    SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, this->animIndex);
}

s32 func_80BB2088(EnGeg* this, PlayState* play) {
    if (DECR(this->unk_242) != 0) {
        this->unk_468 = 0;
        this->unk_46A = 0;
        this->unk_230 &= ~2;
        this->unk_46C = 0;
        this->unk_46E = 0;
        return true;
    }

    if (Actor_IsFacingAndNearPlayer(&this->actor, 300.0f, 0x7FF8) &&
        ((this->animIndex == ENGEG_ANIM_5) || ((this->animIndex == ENGEG_ANIM_13) && (this->unk_496 == 0xD69)))) {
        this->unk_230 |= 2;
        func_80BB1D64(this, play);
    } else {
        if (this->unk_230 & 2) {
            this->unk_242 = 20;
        }
        this->unk_230 &= ~2;
        this->unk_468 = 0;
        this->unk_46A = 0;
        this->unk_46C = 0;
        this->unk_46E = 0;
    }

    return true;
}

void func_80BB217C(EnGeg* this, PlayState* play) {
    if (Object_IsLoaded(&play->objectCtx, this->objectSlot)) {
        this->animIndex = ENGEG_ANIM_5;
        EnGeg_ChangeAnim(this, play);
        Actor_SetScale(&this->actor, 0.01f);
        this->unk_230 = 0;
        this->actor.shape.shadowScale = 20.0f;
        this->actor.gravity = -1.0f;
        func_80BB1C1C(this);
        this->actionFunc = func_80BB221C;
        this->actor.attentionRangeType = ATTENTION_RANGE_3;
    }
}

void func_80BB221C(EnGeg* this, PlayState* play) {
    u8 sp27 = func_80BB1B14(this, play);

    if (sp27 != 0) {
        this->unk_230 &= ~8;
        if (Actor_TalkOfferAccepted(&this->actor, &play->state) && (this->unk_230 & 4)) {
            if (sp27 == 1) {
                this->unk_496 = 0xD66;
                this->nextCsId = this->csIdList[3];
            } else if (sp27 == 2) {
                this->unk_496 = 0xD64;
                this->nextCsId = this->csIdList[2];
                this->unk_230 &= ~4;
            } else if (sp27 == 3) {
                this->unk_230 |= 0x200;
                this->unk_496 = 0xD64;
                this->nextCsId = this->csIdList[2];
                this->unk_230 &= ~4;
            }
            Message_StartTextbox(play, this->unk_496, &this->actor);
            this->actionFunc = func_80BB2520;
            this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        } else if (this->actor.xzDistToPlayer < 300.0f) {
            this->unk_230 |= 4;
            this->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
            Actor_OfferTalk(&this->actor, play, 300.0f);
        }
    } else {
        this->unk_230 &= ~4;
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_35_40)) {
            if (Actor_TalkOfferAccepted(&this->actor, &play->state) && (this->unk_230 & 8)) {
                this->unk_496 = 0xD62;
                Message_StartTextbox(play, this->unk_496, &this->actor);
                this->unk_230 &= ~8;
                this->actionFunc = func_80BB27D4;
            } else if ((this->actor.xzDistToPlayer < 300.0f) && this->actor.isLockedOn) {
                Actor_OfferTalk(&this->actor, play, 300.0f);
                this->unk_230 |= 8;
            }
        } else if (Actor_TalkOfferAccepted(&this->actor, &play->state) && (this->unk_230 & 8)) {
            SET_WEEKEVENTREG(WEEKEVENTREG_35_40);
            this->unk_496 = 0xD5E;
            this->nextCsId = this->csIdList[0];
            Message_StartTextbox(play, this->unk_496, &this->actor);
            this->actionFunc = func_80BB2520;
            this->unk_230 &= ~8;
            this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        } else if (this->actor.xzDistToPlayer < 300.0f) {
            this->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
            Actor_OfferTalk(&this->actor, play, 300.0f);
            this->unk_230 |= 8;
        }
    }
}

void func_80BB2520(EnGeg* this, PlayState* play) {
    if (this->unk_230 & 0x10) {
        CutsceneManager_Stop(this->csId);
        this->unk_230 &= ~0x10;
        if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
            CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
        }
        this->csId = this->nextCsId;
        CutsceneManager_Queue(this->csId);
        return;
    }

    if (CutsceneManager_IsNext(this->csId)) {
        CutsceneManager_StartWithPlayerCsAndSetFlag(this->csId, &this->actor);
        this->unk_230 |= 0x10;

        switch (this->unk_496) {
            case 0xD5E:
            case 0xD5F:
                this->actionFunc = func_80BB26EC;
                break;

            case 0xD64:
                this->unk_230 &= ~0x20;
                this->actionFunc = func_80BB2A54;
                break;

            case 0xD66:
                this->objectSlot = Object_GetSlot(&play->objectCtx, OBJECT_OF1D_MAP);
                if (this->objectSlot > OBJECT_SLOT_NONE) {
                    this->animIndex = ENGEG_ANIM_19;
                    EnGeg_ChangeAnim(this, play);
                }
                this->unk_230 |= 0x20;
                this->actionFunc = func_80BB2944;
                break;

            case 0xD67:
                this->csId = this->csIdList[5];
                this->actionFunc = func_80BB2B1C;
                break;

            case 0xD69:
            case 0xD72:
            case 0xD75:
            case 0xD8B:
                this->objectSlot = Object_GetSlot(&play->objectCtx, OBJECT_OF1D_MAP);
                if (this->objectSlot > OBJECT_SLOT_NONE) {
                    this->animIndex = ENGEG_ANIM_4;
                    EnGeg_ChangeAnim(this, play);
                }
                this->actionFunc = func_80BB2E00;
                break;

            default:
                break;
        }
    } else {
        if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
            CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
        }
        this->csId = this->nextCsId;
        CutsceneManager_Queue(this->csId);
    }
}

void func_80BB26EC(EnGeg* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        switch (this->unk_496) {
            case 0xD5E:
                this->nextCsId = this->csIdList[1];
                this->actionFunc = func_80BB2520;
                break;

            case 0xD61:
                CutsceneManager_Stop(this->csId);
                play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
                play->msgCtx.stateTimer = 4;
                this->unk_230 &= ~0x10;
                this->actionFunc = func_80BB221C;
                return;

            default:
                break;
        }

        this->unk_496 = func_80BB16D0(this);
        Message_StartTextbox(play, this->unk_496, &this->actor);
    }
}

void func_80BB27D4(EnGeg* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        switch (this->unk_496) {
            case 0xD63:
                play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
                play->msgCtx.stateTimer = 4;
                this->actionFunc = func_80BB221C;
                break;

            case 0xD69:
                this->nextCsId = this->csIdList[6];
                play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
                play->msgCtx.stateTimer = 4;
                this->actionFunc = func_80BB2520;
                break;

            case 0xD6D:
            case 0xD6F:
            case 0xD8A:
                if (GameInteractor_Should(VB_GIVE_DON_GERO_MASK, true, this)) {
                    play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
                    play->msgCtx.stateTimer = 4;
                    this->actionFunc = func_80BB31B8;
                }
                break;

            case 0xD72:
            case 0xD75:
            case 0xD8B:
                play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
                play->msgCtx.stateTimer = 4;
                this->unk_230 &= ~0x10;
                this->nextCsId = this->csIdList[7];
                this->actionFunc = func_80BB2520;
                break;

            default:
                this->unk_496 = func_80BB16D0(this);
                Message_StartTextbox(play, this->unk_496, &this->actor);
                break;
        }
    }
}

void func_80BB2944(EnGeg* this, PlayState* play) {
    u8 talkState = Message_GetState(&play->msgCtx);
    s16 curFrame = this->skelAnime.curFrame;
    s16 endFrame = Animation_GetLastFrame(sAnimationInfo[this->animIndex].animation);

    if (this->animIndex == ENGEG_ANIM_19) {
        if (curFrame == endFrame) {
            this->animIndex = ENGEG_ANIM_6;
            EnGeg_ChangeAnim(this, play);
        }
    } else if ((talkState == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        if (this->unk_496 == 0xD67) {
            play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
            play->msgCtx.stateTimer = 4;
            this->nextCsId = this->csIdList[4];
            this->actionFunc = func_80BB2520;
        } else {
            this->unk_496 = func_80BB16D0(this);
            Message_ContinueTextbox(play, this->unk_496);
        }
    }
}

void func_80BB2A54(EnGeg* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        if (this->unk_496 == 0xD65) {
            CutsceneManager_Stop(this->csId);
            this->unk_230 &= ~0x10;
            this->unk_244 = 65;
            play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
            play->msgCtx.stateTimer = 4;
            this->actionFunc = func_80BB347C;
        } else {
            this->unk_496 = func_80BB16D0(this);
            Message_StartTextbox(play, this->unk_496, &this->actor);
        }
    }
}

void func_80BB2B1C(EnGeg* this, PlayState* play) {
    Vec3f sp74;
    f32 temp_f20;
    s16 i;
    f32 sp68;

    this->actor.child->world.pos = this->unk_4B4;
    this->actor.child->shape.rot = this->actor.shape.rot;

    if (CutsceneManager_GetCurrentCsId() != this->csIdList[4]) {
        if (CutsceneManager_IsNext(this->csId)) {
            SET_WEEKEVENTREG(WEEKEVENTREG_37_08);
            if (this->actor.child != NULL) {
                Actor_Kill(this->actor.child);
            }
            this->unk_230 |= 0x10;
            CutsceneManager_StartWithPlayerCsAndSetFlag(this->csId, &this->actor);
            this->unk_496 = 0xD68;
            Message_ContinueTextbox(play, this->unk_496);
            this->objectSlot = Object_GetSlot(&play->objectCtx, OBJECT_TAISOU);
            if (this->objectSlot > OBJECT_SLOT_NONE) {
                this->animIndex = ENGEG_ANIM_13;
                EnGeg_ChangeAnim(this, play);
            }
            this->actionFunc = func_80BB27D4;
        } else {
            if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
                CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
            }
            CutsceneManager_Queue(this->csId);
        }
    } else {
        temp_f20 = this->unk_4E0 * 0.005f;
        sp68 = this->unk_4E0 * 0.07f;

        sp74.x = Rand_Centered() * temp_f20;
        sp74.y = Rand_Centered() * temp_f20;
        sp74.z = Rand_Centered() * temp_f20;

        this->actor.child->scale.x *= 0.98f;
        this->actor.child->scale.y *= 0.98f;
        this->actor.child->scale.z *= 0.98f;

        if (this->unk_4E0 > 70) {
            for (i = 0; i < ARRAY_COUNT(this->unk_4C0); i++) {
                this->unk_4C0[i].x *= temp_f20;
                this->unk_4C0[i].y *= temp_f20;
                this->unk_4C0[i].z *= temp_f20;
                EffectSsHahen_Spawn(play, &this->unk_4B4, &this->unk_4C0[i], &sp74, HAHEN_SMALL, sp68, GAMEPLAY_KEEP,
                                    15, gameplay_keep_DL_06AB30);
            }
        }
        this->unk_4E0--;
    }
    AudioSfx_LowerSfxSettingsReverb(&this->actor.projectedPos, true);
    Audio_PlaySfx_WithSfxSettingsReverb(&this->actor.projectedPos, NA_SE_EN_GOLON_SIRLOIN_EAT - SFX_FLAG);
}

void func_80BB2E00(EnGeg* this, PlayState* play) {
    s16 curFrame = this->skelAnime.curFrame;
    s16 endFrame = Animation_GetLastFrame(sAnimationInfo[this->animIndex].animation);

    if (this->animIndex == ENGEG_ANIM_2) {
        Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 5, 0x1000, 0x100);
        this->actor.world.rot.y = this->actor.shape.rot.y;
        if (curFrame == endFrame) {
            CutsceneManager_Stop(this->csId);
            this->animIndex = ENGEG_ANIM_20;
            EnGeg_ChangeAnim(this, play);
            this->actionFunc = func_80BB30B4;
        } else if (Animation_OnFrame(&this->skelAnime, 24.0f)) {
            Actor_PlaySfx(&this->actor, NA_SE_EN_GOLON_STAND_IMT);
        }
    } else if (this->animIndex == ENGEG_ANIM_4) {
        if (Animation_OnFrame(&this->skelAnime, 0.0f)) {
            this->unk_230 |= 1;
            this->actor.shape.yOffset = 14.0f;
            if (this->unk_496 == 0xD69) {
                this->unk_4E4 = func_80BB19C0(this, play);
                this->actionFunc = func_80BB2F7C;
            } else {
                this->actionFunc = func_80BB3318;
            }
        } else if (Animation_OnFrame(&this->skelAnime, 24.0f)) {
            Actor_PlaySfx(&this->actor, NA_SE_EN_GOLON_SIT_IMT);
        }
    }
}

void func_80BB2F7C(EnGeg* this, PlayState* play) {
    Math_SmoothStepToS(&this->actor.world.rot.y, Math_Vec3f_Yaw(&this->actor.world.pos, &this->unk_4E4), 4, 0x3E8, 1);
    this->actor.shape.rot.y = this->actor.world.rot.y;

    if ((this->actor.xzDistToPlayer < 150.0f) && (fabsf(this->actor.playerHeightRel) < 10.0f) &&
        (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        this->animIndex = ENGEG_ANIM_2;
        this->actor.speed = 0.0f;
        this->unk_230 &= ~1;
        this->actor.shape.yOffset = 0.0f;
        EnGeg_ChangeAnim(this, play);
        this->actionFunc = func_80BB2E00;
    } else {
        this->actor.speed = 5.0f;
        Actor_MoveWithGravity(&this->actor);
    }

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        if (this->unk_230 & 0x80) {
            Actor_PlaySfx_Flagged(&this->actor, NA_SE_EN_GOLON_SIRLOIN_ROLL - SFX_FLAG);
        } else {
            this->unk_230 |= 0x80;
            Actor_PlaySfx(&this->actor, NA_SE_EN_EYEGOLE_ATTACK);
        }
    }
}

void func_80BB30B4(EnGeg* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        if (player->transformation == PLAYER_FORM_GORON) {
            this->unk_496 = 0xD6A;
        } else if (Player_GetMask(play) == PLAYER_MASK_DON_GERO) {
            this->unk_496 = 0xD89;
        } else {
            this->unk_496 = 0xD6E;
        }
        Message_StartTextbox(play, this->unk_496, &this->actor);
        this->actionFunc = func_80BB27D4;
        this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
    } else if (this->actor.xzDistToPlayer < 150.0f) {
        this->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        Actor_OfferTalk(&this->actor, play, 150.0f);
    }
}

void func_80BB31B8(EnGeg* this, PlayState* play) {
    s32 getItemId = GI_MASK_DON_GERO;

    if (INV_CONTENT(ITEM_MASK_DON_GERO) == ITEM_MASK_DON_GERO) {
        if (Player_GetMask(play) == PLAYER_MASK_DON_GERO) {
            this->unk_496 = 0xD8B;
            getItemId = GI_RUPEE_PURPLE;
        } else {
            this->unk_496 = 0xD73;
            getItemId = GI_RUPEE_PURPLE;
        }
    } else {
        this->unk_496 = 0xD70;
    }

    if (Actor_HasParent(&this->actor, play)) {
        this->actor.parent = NULL;
        SET_WEEKEVENTREG(WEEKEVENTREG_61_01);
        if (getItemId == GI_MASK_DON_GERO) {
            this->unk_230 |= 0x40;
        }
        this->actionFunc = func_80BB32AC;
    } else {
        Actor_OfferGetItem(&this->actor, play, getItemId, 300.0f, 300.0f);
    }
}

void func_80BB32AC(EnGeg* this, PlayState* play) {
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        Message_StartTextbox(play, this->unk_496, &this->actor);
        this->actionFunc = func_80BB27D4;
    } else {
        Actor_OfferTalkExchangeEquiCylinder(&this->actor, play, 400.0f, PLAYER_IA_MINUS1);
    }
}

void func_80BB3318(EnGeg* this, PlayState* play) {
    static Vec3f D_80BB4044[] = {
        { -550.0f, 8.0f, 550.0f },
        { 220.0f, 43.0f, 525.0f },
    };
    s16 sp46;
    f32 sp40;

    if (this->unk_4D8 < 2) {
        sp46 = Math_Vec3f_Yaw(&this->actor.world.pos, &D_80BB4044[this->unk_4D8]);
        sp40 = Math_Vec3f_DistXZ(&this->actor.world.pos, &D_80BB4044[this->unk_4D8]);
        Math_SmoothStepToS(&this->actor.world.rot.y, sp46, 4, 0x3E8, 1);
        if (sp40 < 20.0f) {
            this->unk_4D8++;
        }
        this->actor.shape.rot.y = this->actor.world.rot.y;
        func_800AE930(&play->colCtx, Effect_GetByIndex(this->unk_4DC), &this->actor.world.pos, 18.0f,
                      this->actor.shape.rot.y, this->actor.floorPoly, this->actor.floorBgId);
    }

    if (CutsceneManager_GetCurrentCsId() != this->csIdList[7]) {
        func_800AEF44(Effect_GetByIndex(this->unk_4DC));
        Actor_Kill(&this->actor);
    } else {
        Math_ApproachF(&this->actor.speed, 10.0f, 0.2f, 1.0f);
        Actor_MoveWithGravity(&this->actor);
    }

    Actor_PlaySfx_Flagged(&this->actor, NA_SE_EN_GOLON_SIRLOIN_ROLL - SFX_FLAG);
}

void func_80BB347C(EnGeg* this, PlayState* play) {
    if (DECR(this->unk_244) == 0) {
        this->unk_244 = 65;
        this->actionFunc = func_80BB221C;
    }
}

void EnGeg_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnGeg* this = (EnGeg*)thisx;
    s32 pad2;
    s32 sp34[] = { 0x3E, 0xF64 };

    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_61_01)) {
        Actor_Kill(&this->actor);
        return;
    }

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 0.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &gGoronSkel, &gGoronUnrollAnim, this->jointTable, this->morphTable,
                       GORON_LIMB_MAX);

    Collider_InitCylinder(play, &this->colliderCylinder);
    Collider_SetCylinder(play, &this->colliderCylinder, &this->actor, &sCylinderInit);
    Collider_InitSphere(play, &this->colliderSphere);
    Collider_SetSphere(play, &this->colliderSphere, &this->actor, &sSphereInit);
    CollisionCheck_SetInfo2(&this->actor.colChkInfo, &sDamageTable, &sColChkInfoInit);

    if (this->actor.update != NULL) {
        this->objectSlot = Object_GetSlot(&play->objectCtx, OBJECT_OF1D_MAP);
        if (this->objectSlot <= OBJECT_SLOT_NONE) {
            Actor_Kill(&this->actor);
        }
    }

    Effect_Add(play, &this->unk_4DC, EFFECT_TIRE_MARK, 0, 0, &sp34);
    thisx->draw = NULL;
    this->unk_4E0 = 100;
    this->actor.draw = EnGeg_Draw;
    this->actionFunc = func_80BB217C;
}

void EnGeg_Destroy(Actor* thisx, PlayState* play) {
    EnGeg* this = (EnGeg*)thisx;

    Collider_DestroyCylinder(play, &this->colliderCylinder);
    Collider_DestroySphere(play, &this->colliderSphere);
    Effect_Destroy(play, this->unk_4DC);
}

void EnGeg_Update(Actor* thisx, PlayState* play) {
    EnGeg* this = (EnGeg*)thisx;

    this->actionFunc(this, play);
    EnGeg_UpdateSkelAnime(this, play);
    func_80BB2088(this, play);
    func_80BB1C8C(this);
    SubS_UpdateFidgetTables(play, this->fidgetTableY, this->fidgetTableZ, ENGEG_FIDGET_TABLE_LEN);
    func_80BB1D04(this);
    func_80BB178C(this, play);
}

s32 func_80BB3728(s16 arg0, s16 arg1, Vec3f* arg2, Vec3s* arg3, s32 arg4, s32 arg5) {
    Vec3f sp7C;
    Vec3f sp70 = gZeroVec3f;
    Vec3s sp68;
    MtxF sp28;

    Matrix_MultVec3f(&sp70, &sp7C);
    Matrix_Get(&sp28);
    Matrix_MtxFToYXZRot(&sp28, &sp68, false);
    *arg2 = sp7C;

    if (!arg4 && !arg5) {
        arg3->x = sp68.x;
        arg3->y = sp68.y;
        arg3->z = sp68.z;
        return true;
    }

    if (arg5) {
        sp68.z = arg0;
        sp68.y = arg1;
    }
    Math_SmoothStepToS(&arg3->x, sp68.x, 3, 0x2AA8, 0xB6);
    Math_SmoothStepToS(&arg3->y, sp68.y, 3, 0x2AA8, 0xB6);
    Math_SmoothStepToS(&arg3->z, sp68.z, 3, 0x2AA8, 0xB6);

    return true;
}

s32 EnGeg_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    return false;
}

void EnGeg_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    static Vec3f D_80BB407C = { -1500.0f, 1500.0f, 0.0f };
    EnGeg* this = (EnGeg*)thisx;
    Vec3f sp38 = { 1.0f, 5.0f, -0.5f };
    Vec3f sp2C = { -1.0f, 5.0f, -0.5f };

    if (limbIndex == GORON_LIMB_HEAD) {
        if (!(this->unk_230 & 0x40)) {
            Gfx_SetupDL25_Opa(play->state.gfxCtx);

            OPEN_DISPS(play->state.gfxCtx);

            gSPDisplayList(POLY_OPA_DISP++, gGoronDonGeroMaskDL);

            CLOSE_DISPS(play->state.gfxCtx);
        }

        sp38.x += Rand_Centered();
        sp38.y += 2.0f * Rand_Centered();
        sp38.z += Rand_Centered();

        sp2C.x += Rand_Centered();
        sp2C.y += 2.0f * Rand_Centered();
        sp2C.z += Rand_Centered();

        Matrix_MultVec3f(&D_80BB407C, &this->unk_4B4);
        Matrix_Push();
        Matrix_RotateYS(this->actor.shape.rot.y, MTXMODE_NEW);
        Matrix_MultVec3f(&sp38, &this->unk_4C0[0]);
        Matrix_MultVec3f(&sp2C, &this->unk_4C0[1]);
        Matrix_Pop();
    }
}

void EnGeg_TransformLimbDraw(PlayState* play, s32 limbIndex, Actor* thisx) {
    EnGeg* this = (EnGeg*)thisx;
    s32 phi_v0;
    s32 phi_v1;

    switch (limbIndex) {
        case GORON_LIMB_HEAD:
            if (this->unk_230 & 2) {
                phi_v1 = true;
            } else {
                phi_v1 = false;
            }

            if (this->unk_242 != 0) {
                phi_v0 = true;
            } else {
                phi_v0 = false;
            }

            func_80BB3728(this->unk_468 + this->unk_46C + 0x4000,
                          this->unk_46A + this->unk_46E + this->actor.shape.rot.y + 0x4000, &this->unk_470,
                          &this->unk_47C, phi_v0, phi_v1);
            Matrix_Pop();
            Matrix_Translate(this->unk_470.x, this->unk_470.y, this->unk_470.z, MTXMODE_NEW);
            Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
            Matrix_RotateYS(this->unk_47C.y, MTXMODE_APPLY);
            Matrix_RotateXS(this->unk_47C.x, MTXMODE_APPLY);
            Matrix_RotateZS(this->unk_47C.z, MTXMODE_APPLY);
            Matrix_Push();
            break;

        case GORON_LIMB_BODY:
            if (this->unk_230 & 2) {
                phi_v1 = true;
            } else {
                phi_v1 = false;
            }

            if (this->unk_242 != 0) {
                phi_v0 = true;
            } else {
                phi_v0 = false;
            }

            func_80BB3728(this->unk_46C + 0x4000, this->unk_46E + this->actor.shape.rot.y + 0x4000, &this->unk_484,
                          &this->unk_490, phi_v0, phi_v1);
            Matrix_Pop();
            Matrix_Translate(this->unk_484.x, this->unk_484.y, this->unk_484.z, MTXMODE_NEW);
            Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
            Matrix_RotateYS(this->unk_490.y, MTXMODE_APPLY);
            Matrix_RotateXS(this->unk_490.x, MTXMODE_APPLY);
            Matrix_RotateZS(this->unk_490.z, MTXMODE_APPLY);
            Matrix_Push();
            break;

        default:
            break;
    }
}

void func_80BB3BE0(EnGeg* this, PlayState* play) {
    static TexturePtr D_80BB4088[] = {
        gGoronEyeOpenTex,
        gGoronEyeHalfTex,
        gGoronEyeClosedTex,
        gGoronEyeClosed2Tex,
    };
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(D_80BB4088[this->unk_23E]));
    gDPPipeSync(POLY_OPA_DISP++);

    SkelAnime_DrawTransformFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                   this->skelAnime.dListCount, EnGeg_OverrideLimbDraw, EnGeg_PostLimbDraw,
                                   EnGeg_TransformLimbDraw, &this->actor);

    CLOSE_DISPS(play->state.gfxCtx);
}

void func_80BB3CB4(EnGeg* this, PlayState* play) {
    f32 sp24 = play->state.frames * this->actor.speed * 1400.0f;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    Matrix_Translate(this->actor.world.pos.x, this->actor.world.pos.y + this->actor.shape.yOffset,
                     this->actor.world.pos.z, MTXMODE_NEW);
    Matrix_RotateYS(this->actor.shape.rot.y, MTXMODE_APPLY);
    Matrix_Translate(0.0f, -this->actor.shape.yOffset, 0.0f, MTXMODE_APPLY);
    Matrix_RotateZS(this->actor.shape.rot.z, MTXMODE_APPLY);
    Matrix_Translate(0.0f, this->actor.shape.yOffset, 0.0f, MTXMODE_APPLY);
    Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
    Matrix_RotateXS(sp24, MTXMODE_APPLY);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, gGoronRolledUpDL);

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnGeg_Draw(Actor* thisx, PlayState* play) {
    EnGeg* this = (EnGeg*)thisx;

    if (this->unk_230 & 1) {
        func_80BB3CB4(this, play);
    } else {
        func_80BB3BE0(this, play);
    }
}
