/*
 * File: z_en_rail_skb.c
 * Overlay: ovl_En_Rail_Skb
 * Description: Ikana Graveyard - Circle of Stalchildren
 */

#include "z_en_rail_skb.h"
#include "overlays/actors/ovl_En_Part/z_en_part.h"
#include "overlays/effects/ovl_Effect_Ss_Hahen/z_eff_ss_hahen.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void EnRailSkb_Init(Actor* thisx, PlayState* play);
void EnRailSkb_Destroy(Actor* thisx, PlayState* play);
void EnRailSkb_Update(Actor* thisx, PlayState* play);
void EnRailSkb_Draw(Actor* thisx, PlayState* play);
void EnRailSkb_Reset(void);

void func_80B70FA0(EnRailSkb* this);
void func_80B70FF8(EnRailSkb* this, PlayState* play);
void func_80B710E4(EnRailSkb* this, PlayState* play);
void func_80B7114C(EnRailSkb* this, PlayState* play);
void func_80B7123C(EnRailSkb* this, PlayState* play);
void func_80B712FC(EnRailSkb* this);
void func_80B71314(EnRailSkb* this, PlayState* play);
void func_80B71354(EnRailSkb* this);
void func_80B713A4(EnRailSkb* this, PlayState* play);
void func_80B71488(EnRailSkb* this);
void func_80B714D8(EnRailSkb* this, PlayState* play);
void func_80B7151C(EnRailSkb* this);
void func_80B715AC(EnRailSkb* this, PlayState* play);
void func_80B716A8(EnRailSkb* this, PlayState* play);
void func_80B717C8(EnRailSkb* this);
void func_80B717E0(EnRailSkb* this, PlayState* play);
void func_80B718C4(EnRailSkb* this, PlayState* play);
void func_80B71910(EnRailSkb* this);
void func_80B71954(EnRailSkb* this, PlayState* play);
void func_80B71A08(EnRailSkb* this);
void func_80B71A58(EnRailSkb* this, PlayState* play);
void func_80B71B6C(EnRailSkb* this);
void func_80B71BB8(EnRailSkb* this, PlayState* play);
void func_80B71D8C(EnRailSkb* this, PlayState* play, EnRailSkbUnkFunc unkFunc);
void func_80B71EA8(EnRailSkb* this, PlayState* play);
void func_80B71F3C(EnRailSkb* this, PlayState* play);
void func_80B72100(EnRailSkb* this, PlayState* play);
void func_80B723F8(EnRailSkb* this);
void func_80B72430(EnRailSkb* this, PlayState* play, s32 arg2);
void func_80B726B4(EnRailSkb* this, PlayState* play);
void func_80B72830(EnRailSkb* this, s16 arg1);
s32 func_80B7285C(EnRailSkb* this);

ActorProfile En_Rail_Skb_Profile = {
    /**/ ACTOR_EN_RAIL_SKB,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_SKB,
    /**/ sizeof(EnRailSkb),
    /**/ EnRailSkb_Init,
    /**/ EnRailSkb_Destroy,
    /**/ EnRailSkb_Update,
    /**/ EnRailSkb_Draw,
    /**/ EnRailSkb_Reset,
};

typedef enum EnRailSkbAnimation {
    /* -1 */ ENRAILSKB_ANIM_NONE = -1,
    /*  0 */ ENRAILSKB_ANIM_0,
    /*  1 */ ENRAILSKB_ANIM_1,
    /*  2 */ ENRAILSKB_ANIM_2,
    /*  3 */ ENRAILSKB_ANIM_3,
    /*  4 */ ENRAILSKB_ANIM_4,
    /*  5 */ ENRAILSKB_ANIM_5,
    /*  6 */ ENRAILSKB_ANIM_6,
    /*  7 */ ENRAILSKB_ANIM_7,
    /*  8 */ ENRAILSKB_ANIM_8,
    /*  9 */ ENRAILSKB_ANIM_9,
    /* 10 */ ENRAILSKB_ANIM_10,
    /* 11 */ ENRAILSKB_ANIM_11,
    /* 12 */ ENRAILSKB_ANIM_12,
    /* 13 */ ENRAILSKB_ANIM_MAX
} EnRailSkbAnimation;

static AnimationInfo sAnimationInfo[ENRAILSKB_ANIM_MAX] = {
    { &gStalchildWalkAnim, 0.96f, 0.0f, 0.0f, ANIMMODE_LOOP, -4.0f },         // ENRAILSKB_ANIM_0
    { &gStalchildStandUpAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, -1.0f },       // ENRAILSKB_ANIM_1
    { &gStalchildAttackAnim, 0.6f, 0.0f, 0.0f, ANIMMODE_ONCE_INTERP, 4.0f },  // ENRAILSKB_ANIM_2
    { &gStalchildStaggerAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, -4.0f },       // ENRAILSKB_ANIM_3
    { &gStalchildCollapseAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, -4.0f },      // ENRAILSKB_ANIM_4
    { &gStalchildSitLaughAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -4.0f },      // ENRAILSKB_ANIM_5
    { &gStalchildSitTapToesAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -4.0f },    // ENRAILSKB_ANIM_6
    { &gStalchildSwingOnBranchAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -4.0f }, // ENRAILSKB_ANIM_7
    { &gStalchildStandUpAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, -8.0f },       // ENRAILSKB_ANIM_8
    { &gStalchildStandUpAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, -16.0f },      // ENRAILSKB_ANIM_9
    { &gStalchildStaggerAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, -8.0f },       // ENRAILSKB_ANIM_10
    { &gStalchildSaluteAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, -8.0f },        // ENRAILSKB_ANIM_11
    { &gStalchildIdleAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -8.0f },          // ENRAILSKB_ANIM_12
};

static ColliderJntSphElementInit sJntSphElementsInit[2] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xF7CFFFFF, 0x00, 0x04 },
            { 0x00000000, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_NONE,
            OCELEM_NONE,
        },
        { 15, { { 0, 0, 0 }, 10 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON | ACELEM_HOOKABLE,
            OCELEM_ON,
        },
        { 1, { { 0, 0, 0 }, 20 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COL_MATERIAL_HIT6,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sJntSphElementsInit),
    sJntSphElementsInit,
};

static CollisionCheckInfoInit2 sColChkInfoInit = { 2, 0, 0, 0, MASS_IMMOVABLE };

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, 0x1),
    /* Deku Stick     */ DMG_ENTRY(1, 0xD),
    /* Horse trample  */ DMG_ENTRY(0, 0x0),
    /* Explosives     */ DMG_ENTRY(1, 0xF),
    /* Zora boomerang */ DMG_ENTRY(3, 0xC),
    /* Normal arrow   */ DMG_ENTRY(3, 0xC),
    /* UNK_DMG_0x06   */ DMG_ENTRY(2, 0xF),
    /* Hookshot       */ DMG_ENTRY(1, 0x1),
    /* Goron punch    */ DMG_ENTRY(2, 0xF),
    /* Sword          */ DMG_ENTRY(1, 0xF),
    /* Goron pound    */ DMG_ENTRY(1, 0xF),
    /* Fire arrow     */ DMG_ENTRY(3, 0x2),
    /* Ice arrow      */ DMG_ENTRY(3, 0x3),
    /* Light arrow    */ DMG_ENTRY(2, 0x4),
    /* Goron spikes   */ DMG_ENTRY(1, 0xF),
    /* Deku spin      */ DMG_ENTRY(0, 0x1),
    /* Deku bubble    */ DMG_ENTRY(1, 0xD),
    /* Deku launch    */ DMG_ENTRY(2, 0xF),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, 0x1),
    /* Zora barrier   */ DMG_ENTRY(0, 0xB),
    /* Normal shield  */ DMG_ENTRY(0, 0x0),
    /* Light ray      */ DMG_ENTRY(0, 0x0),
    /* Thrown object  */ DMG_ENTRY(1, 0xF),
    /* Zora punch     */ DMG_ENTRY(1, 0xF),
    /* Spin attack    */ DMG_ENTRY(1, 0xF),
    /* Sword beam     */ DMG_ENTRY(2, 0x0),
    /* Normal Roll    */ DMG_ENTRY(4, 0x0),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, 0x0),
    /* Unblockable    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, 0x0),
    /* Powder Keg     */ DMG_ENTRY(1, 0xF),
};

// 2S2H [Port] Moved this variable out of func_80B708C0 so reset function can access
static s32 D_80B7348C = 0;

void func_80B708C0(EnRailSkb* this, PlayState* play) {
    Path* path = &play->setupPathList[ENRAILSKB_GET_PATH_INDEX(&this->actor)];
    Vec3f sp70;
    s32 phi_a3;

    this->pathPoints = Lib_SegmentedToVirtual(path->points);
    this->unk_2E0 = D_80B7348C;
    this->unk_2E8 = path->count;

    if (D_80B7348C == 0) {
        s32 i;

        for (i = 1; (i < this->unk_2E8) && (i < 10); i++) {
            D_80B7348C++;
            Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EN_RAIL_SKB, 0.0f, 0.0f, 0.0f, 0, 0, 0,
                               this->actor.params);
        }
        D_80B7348C = 0;
    }

    this->actor.world.pos.x = this->pathPoints[this->unk_2E0].x;
    this->actor.world.pos.y = this->pathPoints[this->unk_2E0].y;
    this->actor.world.pos.z = this->pathPoints[this->unk_2E0].z;

    if (this->unk_2E0 < (this->unk_2E8 - 1)) {
        phi_a3 = this->unk_2E0 + 1;
    } else {
        phi_a3 = 0;
    }

    sp70.x = this->pathPoints[phi_a3].x;
    sp70.y = this->pathPoints[phi_a3].y;
    sp70.z = this->pathPoints[phi_a3].z;

    this->actor.world.rot.y = this->actor.shape.rot.y = Math_Vec3f_Yaw(&this->actor.world.pos, &sp70);
}

s32 func_80B70AB4(Vec2f arg0, Vec2f arg1) {
    s32 phi_v1;

    if ((arg1.x * arg0.z) < (arg0.x * arg1.z)) {
        phi_v1 = 1;
    } else {
        phi_v1 = -1;
    }
    return phi_v1;
}

s32 func_80B70B04(EnRailSkb* this, Vec3f pos) {
    s32 pad;
    s32 temp_s3;
    Vec2f sp60;
    Vec2f sp58;
    s32 ret = true;
    s32 pad2;
    s32 i = 0;
    s32 j;

    sp60.x = this->pathPoints[0].z - pos.z;
    sp60.z = this->pathPoints[0].x - pos.x;
    sp58.x = this->pathPoints[1].z - pos.z;
    sp58.z = this->pathPoints[1].x - pos.x;
    j = 1;
    temp_s3 = func_80B70AB4(sp60, sp58);

    while (j != 0) {
        i++;

        if (j < (this->unk_2E8 - 1)) {
            j++;
        } else {
            j = 0;
        }

        sp60.x = this->pathPoints[i].z - pos.z;
        sp60.z = this->pathPoints[i].x - pos.x;
        sp58.x = this->pathPoints[j].z - pos.z;
        sp58.z = this->pathPoints[j].x - pos.x;

        if (func_80B70AB4(sp60, sp58) != temp_s3) {
            ret = false;
            break;
        }
    }

    return ret;
}

void func_80B70D24(EnRailSkb* this, PlayState* play) {
    Actor* actor = play->actorCtx.actorLists[ACTORCAT_PROP].first;

    while (actor != NULL) {
        if ((actor->id == ACTOR_OBJ_HAKAISI) && func_80B70B04(this, actor->world.pos)) {
            if (Flags_GetSwitch(play, OBJHAKAISI_GET_SWITCH_FLAG(actor))) {
                Actor_Kill(&this->actor);
                return;
            }
            this->unk_22C = (ObjHakaisi*)actor;
            return;
        }
        actor = actor->next;
    }
    Actor_Kill(&this->actor);
}

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(lockOnArrowOffset, 2000, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, -2000, ICHAIN_STOP),
};

void EnRailSkb_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnRailSkb* this = (EnRailSkb*)thisx;

    func_80B708C0(this, play);
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 36.0f);
    SkelAnime_Init(play, &this->skelAnime, &gStalchildSkel, &gStalchildWalkAnim, this->jointTable, this->morphTable,
                   STALCHILD_LIMB_MAX);
    Collider_InitJntSph(play, &this->collider);
    Collider_SetJntSph(play, &this->collider, &this->actor, &sJntSphInit, this->colliderElements);
    CollisionCheck_SetInfo2(&this->actor.colChkInfo, &sDamageTable, &sColChkInfoInit);

    if (Flags_GetSwitch(play, ENRAILSKB_GET_SWITCH_FLAG(&this->actor))) {
        Actor_Kill(&this->actor);
    }

    Actor_ProcessInitChain(&this->actor, sInitChain);
    this->actor.speed = 1.6f;
    this->actor.hintId = TATL_HINT_ID_STALCHILD;
    this->unk_3F2 = 0;
    this->unk_2E4 = -1;
    this->unk_3FC = 0;
    this->unk_3FA = 0;
    this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
    this->unk_3FE = 0;

    if (this->actor.parent == NULL) {
        this->unk_3F4 = 1;
        this->unk_3F6 = 1;
        this->unk_3F8 = 0;
    }

    if ((play->sceneId == SCENE_BOTI) && (gSaveContext.sceneLayer == 1) && (play->csCtx.scriptIndex == 0)) {
        this->actor.flags |= ACTOR_FLAG_FREEZE_EXCEPTION;
    }

    func_80B70FA0(this);
}

void EnRailSkb_Destroy(Actor* thisx, PlayState* play) {
    EnRailSkb* this = (EnRailSkb*)thisx;

    Collider_DestroyJntSph(play, &this->collider);
}

void func_80B70FA0(EnRailSkb* this) {
    this->unk_3F2 = 0;
    if (this->actionFunc != func_80B716A8) {
        Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENRAILSKB_ANIM_0);
    }
    this->actionFunc = func_80B70FF8;
}

void func_80B70FF8(EnRailSkb* this, PlayState* play) {
    s16 temp_v0 = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;

    if (this->actor.xzDistToPlayer < 65.0f) {
        if ((ABS_ALT(temp_v0) < 0x2AAA) && (this->unk_3F2 >= 6)) {
            func_80B71354(this);
        }
    }

    if (this->unk_3F2 < 6) {
        this->unk_3F2++;
    }

    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.world.rot.y, 1, 0x71C, 0xB6);
}

void func_80B710AC(EnRailSkb* this) {
    Actor_PlaySfx(&this->actor, NA_SE_EN_COMMON_FREEZE);
    this->actionFunc = func_80B710E4;
}

void func_80B710E4(EnRailSkb* this, PlayState* play) {
    if (this->actor.colorFilterTimer == 0) {
        func_80B70FA0(this);
    }
}

void func_80B71114(EnRailSkb* this) {
    Actor_PlaySfx(&this->actor, NA_SE_EN_COMMON_FREEZE);
    this->actionFunc = func_80B7114C;
}

void func_80B7114C(EnRailSkb* this, PlayState* play) {
    if (this->drawDmgEffTimer == 0) {
        this->drawDmgEffTimer = 0;
        this->drawDmgEffScale = 0.0f;
        this->drawDmgEffAlpha = 0.0f;
        if (this->actor.colChkInfo.health != 0) {
            Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 8);
            Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENRAILSKB_ANIM_3);
            Actor_PlaySfx(&this->actor, NA_SE_EN_STALKID_DAMAGE);
            this->unk_402 |= 1;
            func_80B712FC(this);
        } else {
            SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 40, NA_SE_EN_STALKID_DEAD);
            func_80B71488(this);
        }
    } else if (this->drawDmgEffTimer == 1) {
        func_80B726B4(this, play);
    }
}

void func_80B71228(EnRailSkb* this) {
    this->actionFunc = func_80B7123C;
}

void func_80B7123C(EnRailSkb* this, PlayState* play) {
    if (this->drawDmgEffTimer == 0) {
        this->drawDmgEffTimer = 0;
        this->drawDmgEffScale = 0.0f;
        this->drawDmgEffAlpha = 0.0f;
        if (this->actor.colChkInfo.health != 0) {
            Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 8);
            Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENRAILSKB_ANIM_3);
            Actor_PlaySfx(&this->actor, NA_SE_EN_STALKID_DAMAGE);
            this->unk_402 |= 1;
            func_80B712FC(this);
        } else {
            SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 40, NA_SE_EN_STALKID_DEAD);
            func_80B71488(this);
        }
    }
}

void func_80B712FC(EnRailSkb* this) {
    this->unk_3F2 = 0;
    this->actionFunc = func_80B71314;
}

void func_80B71314(EnRailSkb* this, PlayState* play) {
    if (this->unk_3F2 >= 6) {
        func_80B70FA0(this);
    } else {
        this->unk_3F2++;
    }
}

void func_80B71354(EnRailSkb* this) {
    Actor_PlaySfx(&this->actor, NA_SE_EN_STALKID_ATTACK);
    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENRAILSKB_ANIM_2);
    this->actionFunc = func_80B713A4;
}

void func_80B713A4(EnRailSkb* this, PlayState* play) {
    if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
        if ((this->actor.xzDistToPlayer > 65.0f) || (Player_GetMask(play) == PLAYER_MASK_CAPTAIN)) {
            func_80B70FA0(this);
        } else {
            Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENRAILSKB_ANIM_2);
        }
    }

    if (this->actor.xzDistToPlayer > 65.0f) {
        Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.world.rot.y, 1, 0x71C, 0xB6);
    } else {
        Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 2, 0xAAA, 0xB6);
    }
}

void func_80B71488(EnRailSkb* this) {
    this->unk_402 |= 0x40;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    if (this->unk_2E0 != 0) {
        this->unk_2E4 = this->unk_2E0 - 1;
    } else {
        this->unk_2E4 = this->unk_2E8 - 1;
    }
    this->actionFunc = func_80B714D8;
}

void func_80B714D8(EnRailSkb* this, PlayState* play) {
    if (this->actor.draw != NULL) {
        this->actor.draw = NULL;
    }

    if (this->unk_2E0 == this->unk_2E4) {
        func_80B7151C(this);
    }
}

void func_80B7151C(EnRailSkb* this) {
    this->actor.shape.yOffset = -5000.0f;
    this->actor.colChkInfo.health = 2;
    this->unk_402 = 0;
    this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    Actor_PlaySfx(&this->actor, NA_SE_EN_STALKID_APPEAR);
    this->actor.draw = EnRailSkb_Draw;
    this->actor.shape.shadowAlpha = 0;
    this->actor.shape.rot.y = this->actor.world.rot.y;
    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENRAILSKB_ANIM_1);
    this->actionFunc = func_80B715AC;
}

void func_80B715AC(EnRailSkb* this, PlayState* play) {
    if (Math_SmoothStepToF(&this->actor.shape.yOffset, 0.0f, 0.5f, 500.0f, 10.0f) == 0.0f) {
        func_80B70FA0(this);
    } else {
        func_80B72430(this, play, 1);
    }

    if (this->actor.shape.shadowAlpha != 255) {
        if (this->actor.shape.shadowAlpha >= 235) {
            this->actor.shape.shadowAlpha = 255;
        } else {
            this->actor.shape.shadowAlpha += 20;
        }
    }
}

void func_80B71650(EnRailSkb* this) {
    this->unk_3FE = 0;
    if (this->actionFunc != func_80B70FF8) {
        Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENRAILSKB_ANIM_0);
    }
    this->actionFunc = func_80B716A8;
}

void func_80B716A8(EnRailSkb* this, PlayState* play) {
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->unk_3FE = 1;
        func_80B71D8C(this, play, func_80B723F8);
        if (!func_80B7285C(this)) {
            Message_StartTextbox(play, 0x13EC, &this->actor);
            this->unk_400 = 0x13EC;
            func_80B72830(this, 1);
        } else {
            Message_StartTextbox(play, 0x13F5, &this->actor);
            this->unk_400 = 0x13F5;
        }
        Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENRAILSKB_ANIM_12);
        func_80B717C8(this);
    } else if ((this->actor.xzDistToPlayer < 100.0f) && !(this->collider.base.acFlags & AC_HIT)) {
        Actor_OfferTalk(&this->actor, play, 100.0f);
    }
    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.world.rot.y, 1, 0x71C, 0xB6);
}

void func_80B717C8(EnRailSkb* this) {
    this->unk_3F2 = 0;
    this->actionFunc = func_80B717E0;
}

void func_80B717E0(EnRailSkb* this, PlayState* play) {
    this->unk_3FA = 0;

    switch (Message_GetState(&play->msgCtx)) {
        case TEXT_STATE_NONE:
        case TEXT_STATE_NEXT:
        case TEXT_STATE_CLOSING:
            break;

        case TEXT_STATE_FADING:
            if ((play->gameplayFrames % 2) != 0) {
                this->unk_3FA = 1;
            }
            break;

        case TEXT_STATE_CHOICE:
            func_80B72100(this, play);
            break;

        case TEXT_STATE_EVENT:
            func_80B71F3C(this, play);
            break;

        case TEXT_STATE_DONE:
            if (Message_ShouldAdvance(play)) {
                func_80B71650(this);
            }
            break;

        default:
            break;
    }
    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 1, 0x71C, 0xB6);
}

void func_80B718B0(EnRailSkb* this) {
    this->actionFunc = func_80B718C4;
}

void func_80B718C4(EnRailSkb* this, PlayState* play) {
    func_80B70D24(this, play);
    if (this->actor.parent == NULL) {
        if (this->unk_22C != NULL) {
            ObjHakaisi* hakasi = this->unk_22C;

            hakasi->unk_198 = 1;
        }
    }
    func_80B71910(this);
}

void func_80B71910(EnRailSkb* this) {
    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENRAILSKB_ANIM_0);
    this->actionFunc = func_80B71954;
}

void func_80B71954(EnRailSkb* this, PlayState* play) {
    s16 sp36 = Math_Vec3f_Yaw(&this->actor.world.pos, &this->unk_22C->dyna.actor.world.pos);
    f32 sp30 = Math_Vec3f_DistXZ(&this->actor.world.pos, &this->unk_22C->dyna.actor.world.pos);

    Math_SmoothStepToS(&this->actor.shape.rot.y, sp36, 1, 0x71C, 0xB6);
    this->actor.world.rot = this->actor.shape.rot;
    if (sp30 < 80.0f) {
        func_80B71A08(this);
    }
    Actor_MoveWithGravity(&this->actor);
}

void func_80B71A08(EnRailSkb* this) {
    Actor_PlaySfx(&this->actor, NA_SE_EN_STALKID_ATTACK);
    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENRAILSKB_ANIM_2);
    this->actionFunc = func_80B71A58;
}

void func_80B71A58(EnRailSkb* this, PlayState* play) {
    s16 sp36 = Math_Vec3f_Yaw(&this->actor.world.pos, &this->unk_22C->dyna.actor.world.pos);

    if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_STALKID_ATTACK);
        Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENRAILSKB_ANIM_2);
        if (this->unk_2E8 < this->unk_22C->dyna.actor.colChkInfo.health) {
            this->unk_22C->dyna.actor.colChkInfo.health--;
        } else {
            this->unk_22C->dyna.actor.colChkInfo.health--;
            func_80B71B6C(this);
        }
    }

    if (Animation_OnFrame(&this->skelAnime, 3.0f)) {
        if (this->unk_2E8 < this->unk_22C->dyna.actor.colChkInfo.health) {
            this->unk_22C->dyna.actor.colChkInfo.health--;
        } else {
            this->unk_22C->dyna.actor.colChkInfo.health--;
            func_80B71B6C(this);
        }
    }
    Math_SmoothStepToS(&this->actor.shape.rot.y, sp36, 1, 0x71C, 0xB6);
    this->actor.world.rot = this->actor.shape.rot;
}

void func_80B71B6C(EnRailSkb* this) {
    this->unk_3F2 = 10;
    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENRAILSKB_ANIM_0);
    this->actionFunc = func_80B71BB8;
}

void func_80B71BB8(EnRailSkb* this, PlayState* play) {
    s32 pad;
    s32 i;
    f32 sp34 = Math_Vec3f_DistXZ(&this->actor.world.pos, &this->unk_22C->dyna.actor.world.pos);

    if (this->unk_3F2 > 0) {
        this->unk_3F2--;
        return;
    }

    Math_SmoothStepToS(&this->actor.shape.rot.y,
                       Math_Vec3f_Yaw(&this->actor.world.pos, &this->unk_22C->dyna.actor.world.pos), 1, 0x71C, 0xB6);

    if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) && (this->unk_22C->dyna.actor.colChkInfo.health == 0)) {
        Actor_MoveWithGravity(&this->actor);
    } else {
        this->actor.velocity.y += this->actor.gravity;
        this->actor.world.pos.y += this->actor.velocity.y;
        Math_SmoothStepToF(&this->actor.world.pos.x, this->unk_22C->dyna.actor.world.pos.x, 0.6f, 1.6f, 0.1f);
        Math_SmoothStepToF(&this->actor.world.pos.z, this->unk_22C->dyna.actor.world.pos.z, 0.6f, 1.6f, 0.1f);
    }

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) {
        Actor_Kill(&this->actor);
        return;
    }

    if ((sp34 < 50.0f) && (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_STALKID_ATTACK);
        Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENRAILSKB_ANIM_9);
        this->actor.velocity.y = 10.0f;

        for (i = 0; i < 4; i++) {
            func_80B72430(this, play, 0);
        }
    }
}

void func_80B71D8C(EnRailSkb* this, PlayState* play, EnRailSkbUnkFunc unkFunc) {
    Actor* actor = play->actorCtx.actorLists[ACTORCAT_ENEMY].first;

    while (actor != NULL) {
        if (actor->id == ACTOR_EN_RAIL_SKB) {
            unkFunc((EnRailSkb*)actor);
        }
        actor = actor->next;
    }
}

void func_80B71DF0(EnRailSkb* this) {
    if (this->actionFunc == func_80B714D8) {
        this->actor.shape.yOffset = -5000.0f;
        this->actor.colChkInfo.health = 2;
        this->unk_402 = 0;
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
        this->actor.draw = EnRailSkb_Draw;
        this->actor.shape.shadowAlpha = 0;
        this->actor.shape.rot.y = this->actor.world.rot.y;
    } else if ((this->actionFunc == func_80B7114C) && (this->drawDmgEffTimer > 1)) {
        this->drawDmgEffTimer = 0;
    }

    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENRAILSKB_ANIM_11);
    this->actionFunc = func_80B71EA8;
}

void func_80B71EA8(EnRailSkb* this, PlayState* play) {
    if (Math_SmoothStepToF(&this->actor.shape.yOffset, 0.0f, 0.5f, 500.0f, 10.0f) != 0.0f) {
        func_80B72430(this, play, 1);
    }

    if (this->actor.shape.shadowAlpha != 255) {
        if (this->actor.shape.shadowAlpha >= 235) {
            this->actor.shape.shadowAlpha = 255;
        } else {
            this->actor.shape.shadowAlpha += 20;
        }
    }
}

void func_80B71F3C(EnRailSkb* this, PlayState* play) {
    if (((this->unk_400 == 0x13ED) || (this->unk_400 == 0x13F5)) && (this->unk_3F2 != 1)) {
        func_80B71D8C(this, play, func_80B71DF0);
        func_80B717C8(this);
        this->unk_3F2 = 1;
    }

    if (Message_ShouldAdvance(play)) {
        switch (this->unk_400) {
            case 0x13EC:
                Message_StartTextbox(play, 0x13ED, &this->actor);
                this->unk_400 = 0x13ED;
                break;

            case 0x13ED:
                Message_StartTextbox(play, 0x13EE, &this->actor);
                this->unk_400 = 0x13EE;
                break;

            case 0x13EE:
                Message_StartTextbox(play, 0x13EF, &this->actor);
                this->unk_400 = 0x13EF;
                break;

            case 0x13EF:
            case 0x13F5:
                Message_StartTextbox(play, 0x13F0, &this->actor);
                this->unk_400 = 0x13F0;
                break;

            case 0x13F1:
                Message_StartTextbox(play, 0x13F2, &this->actor);
                this->unk_400 = 0x13F2;
                break;

            case 0x13F2:
                if (this->unk_3FC == 1) {
                    Message_StartTextbox(play, 0x13F4, &this->actor);
                    this->unk_400 = 0x13F4;
                } else {
                    Message_CloseTextbox(play);
                    func_80B71D8C(this, play, func_80B71650);
                }
                break;

            case 0x13F3:
                Message_StartTextbox(play, 0x13F2, &this->actor);
                this->unk_400 = 0x13F2;
                this->unk_3FC = 1;
                break;

            case 0x13F4:
                Message_CloseTextbox(play);
                func_80B71D8C(this, play, func_80B718B0);
                break;

            default:
                break;
        }
    }
}

void func_80B72100(EnRailSkb* this, PlayState* play) {
    if (Message_ShouldAdvance(play)) {
        if (play->msgCtx.choiceIndex == 0) {
            Audio_PlaySfx_MessageDecide();
            Message_StartTextbox(play, 0x13F1, &this->actor);
            this->unk_400 = 0x13F1;
        } else {
            Audio_PlaySfx_MessageDecide();
            Message_StartTextbox(play, 0x13F3, &this->actor);
            this->unk_400 = 0x13F3;
        }
    }
}

void func_80B72190(EnRailSkb* this, PlayState* play) {
    EnRailSkb* parent;
    Vec3f sp38;
    s32 pad;
    s16 sp32;

    if ((this->actionFunc != func_80B714D8) &&
        (Animation_OnFrame(&this->skelAnime, 8.0f) || Animation_OnFrame(&this->skelAnime, 15.0f))) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_STALKID_WALK);
    }

    if (this->actor.parent == NULL) {
        this->unk_3F4 = this->unk_3F6;
        this->unk_3F6 = 1;

        if ((this->actionFunc == func_80B713A4) || (this->actionFunc == func_80B710E4) ||
            (this->actionFunc == func_80B7114C) || (this->actionFunc == func_80B7123C) ||
            (this->actionFunc == func_80B717E0)) {
            this->unk_3F6 = 0;
        }

        if (this->unk_3F4 == 0) {
            return;
        }
    } else {
        parent = (EnRailSkb*)this->actor.parent;

        if ((this->actionFunc == func_80B713A4) || (this->actionFunc == func_80B710E4) ||
            (this->actionFunc == func_80B7114C) || (this->actionFunc == func_80B7123C) ||
            (this->actionFunc == func_80B717E0)) {
            parent->unk_3F6 = 0;
        }

        if (parent->unk_3F4 == 0) {
            return;
        }
    }

    if ((this->actionFunc != func_80B71EA8) && (this->actionFunc != func_80B71954) &&
        (this->actionFunc != func_80B71A58) && (this->actionFunc != func_80B71BB8)) {
        sp38.x = this->pathPoints[this->unk_2E0].x;
        sp38.y = this->pathPoints[this->unk_2E0].y;
        sp38.z = this->pathPoints[this->unk_2E0].z;
        sp32 = Math_Vec3f_Yaw(&this->actor.world.pos, &sp38);

        if (Math_Vec3f_DistXZ(&this->actor.world.pos, &sp38) > 100.0f) {
            Math_SmoothStepToS(&this->actor.world.rot.y, sp32, 10, 0x3000, 0x100);
        } else if (this->unk_2E0 < (this->unk_2E8 - 1)) {
            this->unk_2E0++;
        } else {
            this->unk_2E0 = 0;
        }
        Actor_MoveWithGravity(&this->actor);
    }
}

void func_80B723F8(EnRailSkb* this) {
    this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE);
    this->actor.flags |= (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
    this->actor.flags |= ACTOR_FLAG_FREEZE_EXCEPTION;
    this->actor.hintId = TATL_HINT_ID_NONE;
    this->actor.textId = 0;
}

void func_80B72430(EnRailSkb* this, PlayState* play, s32 arg2) {
    Vec3f sp5C = this->actor.world.pos;
    Vec3f sp50 = { 0.0f, 8.0f, 0.0f };
    Vec3f sp44 = { 0.0f, -1.5f, 0.0f };
    s16 sp42;
    s32 pad;

    if ((play->gameplayFrames & arg2) == 0) {
        sp42 = Rand_Next();

        sp5C.x += 15.0f * Math_SinS(sp42);
        sp5C.y = this->actor.floorHeight;
        sp5C.z += 15.0f * Math_CosS(sp42);

        sp44.x = Rand_Centered();
        sp44.z = Rand_Centered();

        sp50.y += Rand_Centered() * 4.0f;

        EffectSsHahen_Spawn(play, &sp5C, &sp50, &sp44, 0, (Rand_Next() & 7) + 10, HAHEN_OBJECT_DEFAULT, 10, NULL);
        func_800BBFB0(play, &sp5C, 10.0f, 1, 150, 0, 1);
    }
}

void func_80B725C0(EnRailSkb* this, PlayState* play) {
    if (this->drawDmgEffTimer > 0) {
        this->drawDmgEffTimer--;
    }

    if (this->actionFunc != func_80B7114C) {
        if (this->drawDmgEffTimer < 20) {
            if (this->actionFunc == func_80B7123C) {
                Math_SmoothStepToF(&this->drawDmgEffScale, 0.0f, 0.5f, 0.03f, 0.0f);
            } else {
                Math_SmoothStepToF(&this->drawDmgEffScale, 0.0f, 0.5f, 0.01f, 0.0f);
            }
            this->drawDmgEffAlpha = this->drawDmgEffTimer * 0.05f;
        } else {
            Math_SmoothStepToF(&this->drawDmgEffScale, 0.5f, 0.1f, 0.02f, 0.0f);
        }
    }
}

void func_80B726B4(EnRailSkb* this, PlayState* play) {
    static Color_RGBA8 D_80B734B0 = { 170, 255, 255, 255 };
    static Color_RGBA8 D_80B734B4 = { 200, 200, 255, 255 };
    static Vec3f D_80B734B8 = { 0.0f, -1.0f, 0.0f };
    Vec3f sp84;
    s32 i;
    s32 bodyPartsCount;
    s16 yaw;

    if (this->unk_402 & 2) {
        bodyPartsCount = ENRAILSKB_BODYPART_MAX - 1;
    } else {
        bodyPartsCount = ENRAILSKB_BODYPART_MAX;
    }

    SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 30, NA_SE_EV_ICE_BROKEN);

    for (i = 0; i < bodyPartsCount; i++) {
        yaw = Math_Vec3f_Yaw(&this->actor.world.pos, &this->bodyPartsPos[i]);

        sp84.x = Math_SinS(yaw) * 3.0f;
        sp84.z = Math_CosS(yaw) * 3.0f;
        sp84.y = (Rand_ZeroOne() * 4.0f) + 4.0f;

        EffectSsEnIce_Spawn(play, &this->bodyPartsPos[i], 0.6f, &sp84, &D_80B734B8, &D_80B734B0, &D_80B734B4, 30);
    }
}

void func_80B72830(EnRailSkb* this, s16 arg1) {
    if (this->actor.parent == NULL) {
        this->unk_3F8 = arg1;
    } else {
        ((EnRailSkb*)this->actor.parent)->unk_3F8 = arg1;
    }
}

s32 func_80B7285C(EnRailSkb* this) {
    s32 phi_v1;

    if (this->actor.parent == NULL) {
        phi_v1 = this->unk_3F8;
    } else {
        phi_v1 = ((EnRailSkb*)this->actor.parent)->unk_3F8;
    }
    return phi_v1;
}

void func_80B72880(EnRailSkb* this, PlayState* play) {
    if ((this->actionFunc == func_80B70FF8) || (this->actionFunc == func_80B716A8)) {
        if (this->actionFunc != func_80B716A8) {
            if (Player_GetMask(play) == PLAYER_MASK_CAPTAIN) {
                this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE);
                this->actor.flags |= (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
                this->actor.flags |= ACTOR_FLAG_FREEZE_EXCEPTION;
                this->actor.hintId = TATL_HINT_ID_NONE;
                this->actor.textId = 0;
                func_80B71650(this);
            }
        } else if (Player_GetMask(play) != PLAYER_MASK_CAPTAIN) {
            this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
            this->actor.flags &= ~ACTOR_FLAG_FREEZE_EXCEPTION;
            this->actor.flags |= (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE);
            this->actor.hintId = TATL_HINT_ID_STALCHILD;
            this->actor.textId = 0;
            func_80B70FA0(this);
        }
    }
}

void func_80B72970(EnRailSkb* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 pad;

    if ((this->actionFunc == func_80B714D8) || (this->actionFunc == func_80B7123C) ||
        (this->actionFunc == func_80B715AC) || (this->actionFunc == func_80B71954) ||
        (this->actionFunc == func_80B71A58) || (this->actionFunc == func_80B71BB8) || (this->unk_3FE == 1)) {
        return;
    }

    if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;
        if (this->actionFunc == func_80B7114C) {
            switch (this->actor.colChkInfo.damageEffect) {
                case 1:
                case 3:
                case 4:
                case 12:
                case 13:
                    return;

                default:
                    if (this->drawDmgEffTimer >= 2) {
                        func_80B726B4(this, play);
                    }
                    this->drawDmgEffTimer = 0;
                    break;
            }
        }

        if ((Actor_ApplyDamage(&this->actor) == 0) && (this->actor.colChkInfo.damageEffect != 3) &&
            (this->actor.colChkInfo.damageEffect != 4)) {
            SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 40, NA_SE_EN_STALKID_DEAD);
            func_80B71488(this);
            return;
        }

        switch (this->actor.colChkInfo.damageEffect) {
            case 11:
                this->drawDmgEffTimer = 40;
                this->drawDmgEffType = ACTOR_DRAW_DMGEFF_ELECTRIC_SPARKS_SMALL;
                this->drawDmgEffAlpha = 1.0f;
                this->drawDmgEffScale = 0.0f;
                Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_BLUE, 120, COLORFILTER_BUFFLAG_OPA, 40);
                func_80B710AC(this);
                break;

            case 1:
                Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_BLUE, 120, COLORFILTER_BUFFLAG_OPA, 40);
                func_80B710AC(this);
                break;

            case 2:
                this->drawDmgEffTimer = 80;
                this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
                this->drawDmgEffAlpha = 1.0f;
                this->drawDmgEffScale = 0.0f;
                Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 8);
                Actor_PlaySfx(&this->actor, NA_SE_EN_STALKID_DAMAGE);
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENRAILSKB_ANIM_3);
                this->unk_402 |= 1;
                func_80B712FC(this);
                break;

            case 3:
                if (this->actor.colChkInfo.health != 0) {
                    Actor_PlaySfx(&this->actor, NA_SE_EN_STALKID_DAMAGE);
                    this->drawDmgEffTimer = 80;
                } else {
                    this->drawDmgEffTimer = 3;
                }
                this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FROZEN_SFX;
                this->drawDmgEffAlpha = 1.0f;
                this->drawDmgEffScale = 0.5f;
                Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 8);
                func_80B71114(this);
                break;

            case 4:
                this->drawDmgEffTimer = 40;
                this->drawDmgEffType = ACTOR_DRAW_DMGEFF_LIGHT_ORBS;
                this->drawDmgEffAlpha = 1.0f;
                this->drawDmgEffScale = 0.5f;
                Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 8);
                Actor_PlaySfx(&this->actor, NA_SE_EN_STALKID_DAMAGE);
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENRAILSKB_ANIM_3);
                func_80B71228(this);
                break;

            case 12:
            case 14:
                this->unk_402 |= 1;

            case 15:
                if ((player->meleeWeaponAnimation == PLAYER_MWA_RIGHT_SLASH_1H) ||
                    (player->meleeWeaponAnimation == PLAYER_MWA_LEFT_COMBO_2H) ||
                    (player->meleeWeaponAnimation == PLAYER_MWA_BACKSLASH_RIGHT) ||
                    (player->meleeWeaponAnimation == PLAYER_MWA_BACKSLASH_LEFT)) {
                    this->unk_402 |= 1;
                }

            case 13:
                Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 8);
                Actor_PlaySfx(&this->actor, NA_SE_EN_STALKID_DAMAGE);
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENRAILSKB_ANIM_3);
                func_80B712FC(this);
                break;

            default:
                break;
        }
    }

    if (this->actionFunc == func_80B713A4) {
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
    }

    if (this->actionFunc != func_80B71314) {
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
    }

    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
}

void EnRailSkb_Update(Actor* thisx, PlayState* play) {
    EnRailSkb* this = (EnRailSkb*)thisx;

    this->actionFunc(this, play);

    func_80B72190(this, play);
    Actor_UpdateBgCheckInfo(play, &this->actor, 15.0f, 30.0f, 60.0f, UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4);
    func_80B72970(this, play);

    if ((this->actionFunc != func_80B710E4) && (this->actionFunc != func_80B7114C) &&
        (this->actionFunc != func_80B7123C)) {
        SkelAnime_Update(&this->skelAnime);
    }

    func_80B72880(this, play);
    func_80B725C0(this, play);
}

s32 EnRailSkb_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnRailSkb* this = (EnRailSkb*)thisx;
    s16 abs;

    if (limbIndex == STALCHILD_LIMB_HEAD) {
        OPEN_DISPS(play->state.gfxCtx);

        abs = fabsf(Math_SinS(play->state.frames * 6000) * 95.0f) + 160.0f;

        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetEnvColor(POLY_OPA_DISP++, abs, abs, abs, 255);

        CLOSE_DISPS(play->state.gfxCtx);
    } else if (limbIndex == STALCHILD_LIMB_RIBCAGE) {
        Vec3f sp24 = { 0.0f, 1000.0f, 0.0f };

        Matrix_MultVec3f(&sp24, &this->actor.focus.pos);
    } else if ((limbIndex == STALCHILD_LIMB_LOWER_JAW) && (this->unk_3FA == 1)) {
        Matrix_RotateZS(1820, MTXMODE_APPLY);
    }

    if (((limbIndex == STALCHILD_LIMB_HEAD) || (limbIndex == STALCHILD_LIMB_LOWER_JAW)) && (this->unk_402 & 2)) {
        *dList = NULL;
    }

    return false;
}

void EnRailSkb_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    static Vec3f D_80B734D0 = { 800.0f, 1200.0f, 0.0f };
    EnRailSkb* this = (EnRailSkb*)thisx;

    if (!(this->unk_402 & 0x80)) {
        Collider_UpdateSpheres(limbIndex, &this->collider);

        if ((limbIndex == STALCHILD_LIMB_HEAD) && (this->unk_402 & 1) && !(this->unk_402 & 2)) {
            Actor_SpawnBodyParts(&this->actor, play, ENPART_PARAMS(ENPART_TYPE_1), dList);
            this->unk_402 |= 2;
        } else if ((this->unk_402 & 0x40) && ((limbIndex != STALCHILD_LIMB_HEAD) || !(this->unk_402 & 1)) &&
                   (limbIndex != STALCHILD_LIMB_LOWER_JAW)) {
            Actor_SpawnBodyParts(&this->actor, play, ENPART_PARAMS(ENPART_TYPE_1), dList);
        }

        if (this->drawDmgEffTimer != 0) {
            if ((limbIndex == STALCHILD_LIMB_PELVIS) || (limbIndex == STALCHILD_LIMB_RIGHT_THIGH) ||
                (limbIndex == STALCHILD_LIMB_RIGHT_SHIN) || (limbIndex == STALCHILD_LIMB_RIGHT_FOOT) ||
                (limbIndex == STALCHILD_LIMB_LEFT_THIGH) || (limbIndex == STALCHILD_LIMB_LEFT_SHIN) ||
                (limbIndex == STALCHILD_LIMB_LEFT_FOOT) || (limbIndex == STALCHILD_LIMB_RIGHT_UPPER_ARM) ||
                (limbIndex == STALCHILD_LIMB_RIGHT_LOWER_ARM) || (limbIndex == STALCHILD_LIMB_RIGHT_HAND) ||
                (limbIndex == STALCHILD_LIMB_LEFT_UPPER_ARM) || (limbIndex == STALCHILD_LIMB_LEFT_LOWER_ARM) ||
                (limbIndex == STALCHILD_LIMB_LEFT_HAND)) {
                Matrix_MultZero(&this->bodyPartsPos[this->bodyPartsCount]);
                this->bodyPartsCount++;
            } else if ((limbIndex == STALCHILD_LIMB_HEAD) && !(this->unk_402 & 2)) {
                Matrix_MultVec3f(&D_80B734D0, &this->bodyPartsPos[this->bodyPartsCount]);
                this->bodyPartsCount++;
            }
        }
    }
}

void EnRailSkb_Draw(Actor* thisx, PlayState* play) {
    EnRailSkb* this = (EnRailSkb*)thisx;

    this->bodyPartsCount = 0;
    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    SkelAnime_DrawOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, EnRailSkb_OverrideLimbDraw,
                      EnRailSkb_PostLimbDraw, &this->actor);
    if (this->drawDmgEffTimer > 0) {
        Actor_DrawDamageEffects(play, &this->actor, this->bodyPartsPos, this->bodyPartsCount, this->drawDmgEffScale,
                                0.5f, this->drawDmgEffAlpha, this->drawDmgEffType);
    }

    if ((this->unk_402 & 0x40) && !(this->unk_402 & 0x80)) {
        this->unk_402 |= 0x80;
    }
}

void EnRailSkb_Reset(void) {
    D_80B7348C = 0;
}
