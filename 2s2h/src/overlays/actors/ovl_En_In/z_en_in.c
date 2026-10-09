/*
 * File: z_en_in.c
 * Overlay: ovl_En_In
 * Description: Gorman Bros
 */

#include "z_en_in.h"
#include "z64horse.h"
#include "overlays/actors/ovl_En_Horse_Game_Check/z_en_horse_game_check.h"
#include "2s2h/GameInteractor/GameInteractor.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void EnIn_Init(Actor* thisx, PlayState* play);
void EnIn_Destroy(Actor* thisx, PlayState* play);
void EnIn_Update(Actor* thisx, PlayState* play);
void EnIn_Draw(Actor* thisx, PlayState* play);

void func_808F5A94(EnIn* this, PlayState* play);
void func_808F3690(EnIn* this, PlayState* play);
void func_808F5A34(EnIn* this, PlayState* play);
s32 func_808F5994(EnIn* this, PlayState* play, Vec3f* arg2, s16 arg3);

ActorProfile En_In_Profile = {
    /**/ ACTOR_EN_IN,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_IN,
    /**/ sizeof(EnIn),
    /**/ EnIn_Init,
    /**/ EnIn_Destroy,
    /**/ EnIn_Update,
    /**/ EnIn_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_HIT0,
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
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 18, 64, 0, { 0, 0, 0 } },
};

static ColliderJntSphElementInit sJntSphElementsInit[1] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xF7CFFFFF, 0x00, 0x00 },
            { 0x00000000, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_NONE,
            OCELEM_NONE,
        },
        { 12, { { 1600, 0, 0 }, 5 }, 200 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_NONE,
        OC1_NONE,
        OC2_TYPE_2,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sJntSphElementsInit),
    sJntSphElementsInit,
};

static CollisionCheckInfoInit2 sColChkInfoInit2 = { 0, 0, 0, 0, MASS_HEAVY };

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, 0x0),
    /* Deku Stick     */ DMG_ENTRY(0, 0x0),
    /* Horse trample  */ DMG_ENTRY(0, 0x0),
    /* Explosives     */ DMG_ENTRY(0, 0x0),
    /* Zora boomerang */ DMG_ENTRY(0, 0x0),
    /* Normal arrow   */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x06   */ DMG_ENTRY(0, 0x0),
    /* Hookshot       */ DMG_ENTRY(0, 0x0),
    /* Goron punch    */ DMG_ENTRY(0, 0x0),
    /* Sword          */ DMG_ENTRY(0, 0x0),
    /* Goron pound    */ DMG_ENTRY(0, 0x0),
    /* Fire arrow     */ DMG_ENTRY(0, 0x0),
    /* Ice arrow      */ DMG_ENTRY(0, 0x0),
    /* Light arrow    */ DMG_ENTRY(0, 0x0),
    /* Goron spikes   */ DMG_ENTRY(0, 0x0),
    /* Deku spin      */ DMG_ENTRY(0, 0x0),
    /* Deku bubble    */ DMG_ENTRY(0, 0x0),
    /* Deku launch    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, 0x0),
    /* Zora barrier   */ DMG_ENTRY(0, 0x0),
    /* Normal shield  */ DMG_ENTRY(0, 0x0),
    /* Light ray      */ DMG_ENTRY(0, 0x0),
    /* Thrown object  */ DMG_ENTRY(0, 0x0),
    /* Zora punch     */ DMG_ENTRY(0, 0x0),
    /* Spin attack    */ DMG_ENTRY(0, 0x0),
    /* Sword beam     */ DMG_ENTRY(0, 0x0),
    /* Normal Roll    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, 0x0),
    /* Unblockable    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, 0x0),
    /* Powder Keg     */ DMG_ENTRY(0, 0x0),
};

static AnimationInfoS sAnimationInfo[ENIN_ANIM_MAX] = {
    { &object_in_Anim_001D10, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },  // ENIN_ANIM_0
    { &object_in_Anim_001D10, 1.0f, 0, -1, ANIMMODE_LOOP, -4 }, // ENIN_ANIM_1
    { &object_in_Anim_014F8C, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },  // ENIN_ANIM_2
    { &object_in_Anim_014F8C, 1.0f, 0, -1, ANIMMODE_LOOP, -4 }, // ENIN_ANIM_3
    { &object_in_Anim_000CB0, 1.0f, 0, -1, ANIMMODE_LOOP, -4 }, // ENIN_ANIM_4
    { &object_in_Anim_0003B4, 1.0f, 0, -1, ANIMMODE_LOOP, -4 }, // ENIN_ANIM_5
    { &object_in_Anim_001BE0, 1.0f, 0, -1, ANIMMODE_LOOP, -4 }, // ENIN_ANIM_6
    { &object_in_Anim_015918, 1.0f, 0, -1, ANIMMODE_LOOP, -4 }, // ENIN_ANIM_7
    { &object_in_Anim_01C0B0, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },  // ENIN_ANIM_8
    { &object_in_Anim_01C0B0, 1.0f, 0, -1, ANIMMODE_LOOP, -4 }, // ENIN_ANIM_9
    { &object_in_Anim_01A140, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },  // ENIN_ANIM_10
    { &object_in_Anim_01A140, 1.0f, 0, -1, ANIMMODE_LOOP, -4 }, // ENIN_ANIM_11
    { &object_in_Anim_01B904, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },  // ENIN_ANIM_12
    { &object_in_Anim_01B904, 1.0f, 0, -1, ANIMMODE_LOOP, -4 }, // ENIN_ANIM_13
    { &object_in_Anim_01B3C4, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },  // ENIN_ANIM_14
    { &object_in_Anim_01B3C4, 0.0f, 0, -1, ANIMMODE_ONCE, 0 },  // ENIN_ANIM_15
    { &object_in_Anim_01B3C4, 1.0f, 0, -1, ANIMMODE_LOOP, -4 }, // ENIN_ANIM_16
    { &object_in_Anim_019EB4, 1.0f, 0, -1, ANIMMODE_ONCE, -4 }, // ENIN_ANIM_17
};

static TrackOptionsSet sTrackOptions = {
    { 0xFA0, 4, 1, 3 },
    { 0x1770, 4, 1, 6 },
    { 0xFA0, 4, 1, 3 },
    { 0x1770, 4, 1, 6 },
};

s32 EnIn_ChangeAnim(SkelAnime* skelAnime, s16 animIndex) {
    s16 endFrame;
    s32 didAnimChange = false;

    if ((animIndex > ENIN_ANIM_NONE) && (animIndex < ENIN_ANIM_MAX)) {
        didAnimChange = true;
        endFrame = sAnimationInfo[animIndex].frameCount;
        if (endFrame < 0) {
            endFrame = Animation_GetLastFrame(sAnimationInfo[animIndex].animation);
        }
        Animation_Change(skelAnime, sAnimationInfo[animIndex].animation, sAnimationInfo[animIndex].playSpeed,
                         sAnimationInfo[animIndex].startFrame, endFrame, sAnimationInfo[animIndex].mode,
                         sAnimationInfo[animIndex].morphFrames);
    }
    return didAnimChange;
}

s32 func_808F3178(EnIn* this, PlayState* play) {
    u8 prevUnk260 = this->unk260;
    u8 prevUnk261 = this->unk261;
    u8 tmp;

    this->unk260 = tmp = SubS_IsFloorAbove(play, &this->unk248, -6.0f);
    if ((this->unk260 != 0) && (prevUnk260 == 0) && tmp) {
        Actor_PlaySfx(&this->actor, NA_SE_PL_WALK_GROUND + SURFACE_SFX_OFFSET_STONE);
    }
    this->unk261 = tmp = SubS_IsFloorAbove(play, &this->unk254, -6.0f);
    if ((this->unk261 != 0) && (prevUnk261 == 0) && tmp) {
        Actor_PlaySfx(&this->actor, NA_SE_PL_WALK_GROUND + SURFACE_SFX_OFFSET_STONE);
    }

    return 0;
}

void func_808F322C(EnIn* this, s32 arg1) {
    if (DECR(this->unk484) == 0) {
        this->unk482++;
        if (this->unk482 >= arg1) {
            this->unk482 = 0;
            this->unk484 = Rand_S16Offset(30, 30);
        }
    }
}

void func_808F32A0(EnIn* this, PlayState* play) {
    this->colliderCylinder.dim.pos.x = this->actor.world.pos.x;
    this->colliderCylinder.dim.pos.y = this->actor.world.pos.y;
    this->colliderCylinder.dim.pos.z = this->actor.world.pos.z;
    if (this->unk23D == 0) {
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->colliderCylinder.base);
    }
}

s32 func_808F3310(EnIn* this, PlayState* play) {
    if (this->colliderCylinder.base.acFlags & AC_HIT) {
        this->colliderCylinder.base.acFlags &= ~AC_HIT;
    }
    return 0;
}

s32 func_808F3334(EnIn* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (this->colliderJntSph.base.atFlags & AT_HIT) {
        this->colliderJntSph.base.atFlags &= ~AT_HIT;
        if (this->colliderJntSph.base.atFlags & AT_BOUNCED) {
            return 0;
        }
        Actor_PlaySfx(&player->actor, NA_SE_PL_BODY_HIT);
        func_800B8D98(play, &this->actor, 3.0f, this->actor.yawTowardsPlayer, 6.0f);
    }
    return 1;
}

s32 func_808F33B8(void) {
    s32 ret = (((gSaveContext.save.day == 1) &&
                ((CURRENT_TIME >= CLOCK_TIME(5, 30)) && (CURRENT_TIME <= CLOCK_TIME(6, 0)))) ||
               (gSaveContext.save.day >= 2)) &&
              !CHECK_WEEKEVENTREG(WEEKEVENTREG_DEFENDED_AGAINST_ALIENS);

    return ret;
}

void func_808F3414(EnIn* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Vec3f point;

    if (this->unk23D == 0) {
        this->unk494 = SkelAnime_Update(&this->skelAnime);
    }
    if (SubS_AngleDiffLessEqual(this->actor.shape.rot.y, 0x2710, this->actor.yawTowardsPlayer)) {
        point.x = player->actor.world.pos.x;
        point.y = player->bodyPartsPos[PLAYER_BODYPART_HEAD].y + 3.0f;
        point.z = player->actor.world.pos.z;
        SubS_TrackPoint(&point, &this->actor.focus.pos, &this->actor.shape.rot, &this->trackTarget, &this->headRot,
                        &this->torsoRot, &sTrackOptions);
    } else {
        Math_SmoothStepToS(&this->trackTarget.x, 0, 4, 0x3E8, 1);
        Math_SmoothStepToS(&this->trackTarget.y, 0, 4, 0x3E8, 1);
        Math_SmoothStepToS(&this->headRot.x, 0, 4, 0x3E8, 1);
        Math_SmoothStepToS(&this->headRot.y, 0, 4, 0x3E8, 1);
        Math_SmoothStepToS(&this->torsoRot.x, 0, 4, 0x3E8, 1);
        Math_SmoothStepToS(&this->torsoRot.y, 0, 4, 0x3E8, 1);
    }
    func_808F322C(this, 3);
    func_808F3178(this, play);
    SubS_UpdateFidgetTables(play, this->fidgetTableY, this->fidgetTableZ, OBJECT_IN_LIMB_MAX);
}

void func_808F35AC(EnIn* this, PlayState* play) {
    this->unk4AC |= 0x10;
    if (this->unk4A4 != NULL) {
        this->unk4A4->unk4AC |= 0x10;
    }
}

void func_808F35D8(EnIn* this, PlayState* play) {
    this->unk4AC &= ~0x10;
    if (this->unk4A4 != NULL) {
        this->unk4A4->unk4AC &= ~0x10;
    }
}

void EnIn_DoNothing(EnIn* this, PlayState* play) {
}

void func_808F3618(EnIn* this, PlayState* play) {
    if (ENIN_GET_PATH_INDEX(&this->actor) != ENIN_PATH_INDEX_NONE) {
        EnIn_ChangeAnim(&this->skelAnime, ENIN_ANIM_9);
    }
    if (ENIN_GET_PATH_INDEX(&this->actor) != ENIN_PATH_INDEX_NONE) {
        this->actionFunc = func_808F3690;
    } else {
        this->actionFunc = EnIn_DoNothing;
    }
}

void func_808F3690(EnIn* this, PlayState* play) {
    s16 sp36;
    Vec3f sp28;

    Math_SmoothStepToF(&this->actor.speed, 1.0f, 0.4f, 1000.0f, 0.0f);
    sp36 = this->actor.speed * 400.0f;
    if (SubS_CopyPointFromPath(this->path, this->unk244, &sp28) && SubS_MoveActorToPoint(&this->actor, &sp28, sp36)) {
        this->unk244++;
        if (this->unk244 >= this->path->count) {
            this->unk244 = 0;
        }
    }
}

void func_808F374C(EnIn* this, PlayState* play) {
    AnimationHeader* sAnimations[ENIN_ANIM2_MAX] = {
        &object_in_Anim_015E38, // ENIN_ANIM2_0
        &object_in_Anim_016A60, // ENIN_ANIM2_1
        &object_in_Anim_0177AC, // ENIN_ANIM2_2
        &object_in_Anim_016484, // ENIN_ANIM2_3
        &object_in_Anim_0170DC, // ENIN_ANIM2_4
        &object_in_Anim_018240, // ENIN_ANIM2_5
        &object_in_Anim_0187C8, // ENIN_ANIM2_6
        &object_in_Anim_0198A8, // ENIN_ANIM2_7
    };

    if ((this->skelAnime.animation == &object_in_Anim_016484) ||
        (this->skelAnime.animation == &object_in_Anim_0170DC)) {
        if (Animation_OnFrame(&this->skelAnime, 8.0f)) {
            Audio_PlaySfx_Randomized(&this->actor.projectedPos, NA_SE_VO_IN_LASH_0, 2);
            if (Rand_ZeroOne() < 0.3f) {
                Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_IT_INGO_HORSE_NEIGH);
            }
            Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_IT_LASH);
        }
    }

    if ((this->skelAnime.animation == &object_in_Anim_0198A8) && Animation_OnFrame(&this->skelAnime, 20.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_VO_IN_CRY_0);
    }

    if (SkelAnime_Update(&this->skelAnime)) {
        this->animIndex2 %= ENIN_ANIM2_MAX;
        this->unk486 = this->animIndex2;
        Animation_Change(&this->skelAnime, sAnimations[this->animIndex2], 1.0f, 0.0f,
                         Animation_GetLastFrame(sAnimations[this->animIndex2]), ANIMMODE_ONCE, -10.0f);
    }
}

void func_808F38F8(EnIn* this, PlayState* play) {
    this->unk4A4 = NULL;
    while (true) {
        //! @bug: Infinite loop if there is only one ACTOR_EN_IN
        this->unk4A4 = (EnIn*)SubS_FindActor(play, &this->unk4A4->actor, ACTORCAT_NPC, ACTOR_EN_IN);
        if ((this->unk4A4 != NULL) && (this->unk4A4 != this)) {
            break;
        }
        this->unk4A4 = (EnIn*)this->unk4A4->actor.next;
    }
}

void func_808F395C(EnIn* this, PlayState* play) {
    if (this->unk4B0 == WEEKEVENTREG_HORSE_RACE_STATE_END) {
        this->actionFunc = func_808F5A94;
    }
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        this->actionFunc = func_808F5A34;
        this->unk48C = 1;
    } else {
        Actor_OfferTalk(&this->actor, play, 200.0f);
    }
}

void func_808F39DC(EnIn* this, PlayState* play) {
    u16 textId = 0;

    if (gSaveContext.save.day != 3) {
        switch (GET_WEEKEVENTREG_HORSE_RACE_STATE) {
            case WEEKEVENTREG_HORSE_RACE_STATE_2:
                textId = 0x347A;
                break;

            case WEEKEVENTREG_HORSE_RACE_STATE_3:
                textId = 0x3476;
                break;

            default:
                break;
        }
        SET_WEEKEVENTREG_HORSE_RACE_STATE(WEEKEVENTREG_HORSE_RACE_STATE_END);
    } else {
        switch (GET_WEEKEVENTREG_HORSE_RACE_STATE) {
            case WEEKEVENTREG_HORSE_RACE_STATE_2:
                textId = 0x349D;
                break;

            case WEEKEVENTREG_HORSE_RACE_STATE_3:
                textId = 0x3499;
                break;

            default:
                break;
        }
        SET_WEEKEVENTREG_HORSE_RACE_STATE(WEEKEVENTREG_HORSE_RACE_STATE_END);
    }
    this->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
    this->actor.textId = textId;
    this->actionFunc = func_808F395C;
    if (this->unk4B0 == WEEKEVENTREG_HORSE_RACE_STATE_2) {
        Actor_PlaySfx(&this->actor, NA_SE_VO_IN_LOST);
    } else {
        Actor_PlaySfx(&this->actor, NA_SE_VO_IN_JOY0);
    }
}

void func_808F3AD4(EnIn* this, PlayState* play) {
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        this->unk48C = 1;
        this->actionFunc = func_808F5A94;
    } else {
        Actor_OfferTalkExchangeEquiCylinder(&this->actor, play, 200.0f, PLAYER_IA_MINUS1);
    }
}

void func_808F3B40(EnIn* this, PlayState* play) {
    u16 textId;

    if (Actor_HasParent(&this->actor, play)) {
        this->actor.parent = NULL;
        this->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        this->actionFunc = func_808F3AD4;
        textId = (gSaveContext.save.day != 3) ? 0x3481 : 0x34A4;
        this->actor.textId = textId;
    } else {
        Actor_OfferGetItem(&this->actor, play, GI_MILK, 500.0f, 100.0f);
    }
}

void func_808F3BD4(EnIn* this, PlayState* play) {
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        this->unk48C = 1;
        this->actionFunc = func_808F5A94;
    } else {
        Actor_OfferTalkExchangeEquiCylinder(&this->actor, play, 200.0f, PLAYER_IA_MINUS1);
    }
}

void func_808F3C40(EnIn* this, PlayState* play) {
    u16 textId;

    if (Actor_HasParent(&this->actor, play)) {
        this->actor.parent = NULL;
        this->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        this->actionFunc = func_808F3BD4;
        textId = gSaveContext.save.day != 3 ? 0x346A : 0x3492;
        this->actor.textId = textId;
    } else {
        Actor_OfferGetItem(&this->actor, play, GI_MILK, 500.0f, 100.0f);
    }
}

void func_808F3CD4(EnIn* this, PlayState* play) {
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        this->unk48C = 1;
        this->actionFunc = func_808F5A94;
    } else {
        Actor_OfferTalkExchangeEquiCylinder(&this->actor, play, 200.0f, PLAYER_IA_MINUS1);
    }
}

void func_808F3D40(EnIn* this, PlayState* play) {
    u16 textId;

    if (Actor_HasParent(&this->actor, play)) {
        this->actor.parent = NULL;
        this->actionFunc = func_808F3CD4;
        textId = gSaveContext.save.day != 3 ? 0x347D : 0x34A0;
        this->actor.textId = textId;
        this->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
    } else {
        Actor_OfferGetItem(&this->actor, play, GI_MASK_GARO, 500.0f, 100.0f);
    }
}

u16 func_808F3DD4(PlayState* play, EnIn* this, u32 arg2) {
    u16 textId = 0;

    if (Player_GetMask(play) == PLAYER_MASK_CIRCUS_LEADER) {
        if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_63_40)) {
            textId = 0x34A9;
        } else if (this->unk4AC & 8) {
            textId = 0x34B1;
        } else {
            textId = 0x34AF;
        }
        return textId;
    }

    switch (arg2) {
        case 0:
            if ((GET_PLAYER_FORM == PLAYER_FORM_ZORA) || (GET_PLAYER_FORM == PLAYER_FORM_GORON)) {
                textId = 0x345C;
            } else if (GET_PLAYER_FORM == PLAYER_FORM_DEKU) {
                textId = 0x3460;
            } else if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_15_08)) {
                textId = 0x3458;
            } else {
                textId = 0x345B;
            }
            break;

        case 1:
            if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_15_10)) {
                textId = 0x3463;
            } else {
                textId = 0x346B;
            }
            break;

        case 3:
            if (GET_PLAYER_FORM == PLAYER_FORM_DEKU) {
                textId = 0x3485;
            } else if ((GET_PLAYER_FORM == PLAYER_FORM_ZORA) || (GET_PLAYER_FORM == PLAYER_FORM_GORON)) {
                textId = 0x3484;
            } else if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_56_04)) {
                textId = 0x346D;
            } else {
                textId = 0x3482;
            }
            break;

        case 4:
            if ((GET_PLAYER_FORM == PLAYER_FORM_ZORA) || (GET_PLAYER_FORM == PLAYER_FORM_GORON)) {
                textId = 0x348A;
            } else if (GET_PLAYER_FORM == PLAYER_FORM_DEKU) {
                textId = 0x348B;
            } else if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_16_01)) {
                textId = 0x3486;
            } else {
                textId = 0x3489;
            }
            break;

        case 5:
            if (func_808F33B8()) {
                textId = 0x34B3;
            } else if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_16_02)) {
                textId = 0x348E;
            } else {
                textId = 0x3493;
            }
            break;

        case 7:
            if (GET_PLAYER_FORM == PLAYER_FORM_DEKU) {
                textId = 0x34A8;
            } else if ((GET_PLAYER_FORM == PLAYER_FORM_ZORA) || (GET_PLAYER_FORM == PLAYER_FORM_GORON)) {
                textId = 0x34A7;
            } else if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_16_04)) {
                textId = 0x3495;
            } else {
                textId = 0x34A5;
            }
            break;

        default:
            break;
    }
    if (textId == 0) {
        textId = 1;
    }
    return textId;
}

void func_808F4054(PlayState* play, EnIn* this, s32 arg2, u16 textId) {
    s32 pad;

    if ((textId == 0x34AE) || (textId == 0x34B0) || (textId == 0x34B2)) {
        Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_GORMAN_BROTHERS);
    }

    switch (arg2) {
        case 1:
        case 5:
        case 7:
            if (textId == 0x3473) {
                func_808F35D8(this, play);
            }
            break;

        case 2:
            if (textId == 0x3473) {
                func_808F35D8(this, play);
            }
            break;

        case 0:
        default:
            break;
    }
}

void func_808F4108(EnIn* this, PlayState* play, u16 textId) {
    Actor_ChangeFocus(&this->actor, play, &this->unk4A4->actor);
    this->actor.textId = 0;
    this->unk4A4->actor.textId = textId;
    this->unk4A4->unk48C = 2;
    this->unk48C = 0;
}

s32 func_808F4150(PlayState* play, EnIn* this, s32 arg2, MessageContext* msgCtx) {
    Actor* thisx = &this->actor;

    if (msgCtx->choiceIndex == 0) {
        Audio_PlaySfx_MessageDecide();
        if (gSaveContext.save.saveInfo.playerData.rupees >= play->msgCtx.unk1206C) {
            Rupees_ChangeBy(-play->msgCtx.unk1206C);
            if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_57_01)) {
                func_808F4108(this, play, 0x3474);
            } else if (this->unk4AC & 8) {
                func_808F4108(this, play, 0x3475);
            } else {
                Actor_ContinueText(play, thisx, 0x3475);
            }
        } else {
            Audio_PlaySfx(NA_SE_SY_ERROR);
            Actor_ContinueText(play, thisx, 0x3473);
        }
    } else {
        Audio_PlaySfx_MessageCancel();
        Actor_ContinueText(play, thisx, 0x3472);
    }
    return 0;
}

s32 func_808F4270(PlayState* play, EnIn* this, s32 arg2, MessageContext* msgCtx, s32 arg4) {
    s32 pad;
    s32 fee = (play->msgCtx.unk1206C != 0xFFFF) ? play->msgCtx.unk1206C : 10;

    if (msgCtx->choiceIndex == 0) {
        Audio_PlaySfx_MessageDecide();
        if (gSaveContext.save.saveInfo.playerData.rupees >= fee) {
            Rupees_ChangeBy(-fee);
            if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_57_01)) {
                if (arg4) {
                    Actor_ContinueText(play, &this->actor, 0x3474);
                } else {
                    func_808F4108(this, play, 0x3474);
                }
            } else {
                if (arg4) {
                    Actor_ContinueText(play, &this->actor, 0x3475);
                } else {
                    func_808F4108(this, play, 0x3475);
                }
            }
        } else {
            Audio_PlaySfx(NA_SE_SY_ERROR);
            if (arg4) {
                Actor_ContinueText(play, &this->actor, 0x3473);
            } else {
                func_808F4108(this, play, 0x3473);
            }
        }
    } else {
        Audio_PlaySfx_MessageCancel();
        Actor_ContinueText(play, &this->actor, 0x3472);
    }
    return 0;
}

s32 func_808F43E0(EnIn* this) {
    this->unk48C = 0;
    this->actor.textId = 0;
    SET_WEEKEVENTREG_HORSE_RACE_STATE(WEEKEVENTREG_HORSE_RACE_STATE_END);
    return 0;
}

s32 func_808F4414(PlayState* play, EnIn* this, s32 arg2) {
    u16 textId = this->actor.textId;
    s32 ret = true;
    MessageContext* msgCtx = &play->msgCtx;
    s32 pad[2];

    switch (textId) {
        case 0x34AF:
            Actor_ContinueText(play, &this->actor, 0x34B0);
            ret = false;
            break;

        case 0x34A9:
            func_808F4108(this, play, 0x34AA);
            SET_WEEKEVENTREG(WEEKEVENTREG_63_40);
            ret = false;
            break;

        case 0x34AA:
            func_808F4108(this, play, 0x34AB);
            ret = false;
            break;

        case 0x34AB:
            func_808F4108(this, play, 0x34AC);
            ret = false;
            break;

        case 0x34AC:
            func_808F4108(this, play, 0x34AD);
            ret = false;
            break;

        case 0x34AD:
            Actor_ContinueText(play, &this->actor, 0x34AE);
            ret = false;
            break;

        case 0x34B1:
            Actor_ContinueText(play, &this->actor, 0x34B2);
            ret = false;
            break;

        default:
            break;
    }

    switch (arg2) {
        case 0:
            switch (textId) {
                case 0x3458:
                    SET_WEEKEVENTREG(WEEKEVENTREG_15_08);
                    Actor_ContinueText(play, &this->actor, 0x3459);
                    ret = false;
                    break;

                case 0x3459:
                    Actor_ContinueText(play, &this->actor, 0x345A);
                    ret = false;
                    break;

                case 0x345A:
                    Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_GORMAN_BROTHERS);
                    ret = true;
                    break;

                case 0x345B:
                    Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_GORMAN_BROTHERS);
                    ret = true;
                    break;

                case 0x345C:
                    Actor_ContinueText(play, &this->actor, 0x345D);
                    ret = false;
                    break;

                case 0x345D:
                    Actor_ContinueText(play, &this->actor, 0x345E);
                    ret = false;
                    break;

                case 0x345E:
                    Actor_ContinueText(play, &this->actor, 0x345F);
                    ret = false;
                    break;

                case 0x345F:
                    Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_GORMAN_BROTHERS);
                    ret = true;
                    break;

                case 0x3460:
                    Actor_ContinueText(play, &this->actor, 0x3461);
                    ret = false;
                    break;

                case 0x3461:
                    Actor_ContinueText(play, &this->actor, 0x3462);
                    ret = false;
                    break;

                case 0x3462:
                    Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_GORMAN_BROTHERS);
                    ret = true;
                    break;

                default:
                    break;
            }
            break;

        case 1:
            switch (textId) {
                case 0x3463:
                    SET_WEEKEVENTREG(WEEKEVENTREG_15_10);
                    Actor_ContinueText(play, &this->actor, 0x3464);
                    ret = false;
                    break;

                case 0x3464:
                    Actor_ContinueText(play, &this->actor, 0x3465);
                    ret = false;
                    break;

                case 0x3465:
                    Actor_ContinueText(play, &this->actor, 0x3466);
                    ret = false;
                    break;

                case 0x3466:
                    if (GameInteractor_Should(VB_BUY_GORMAN_MILK, false, &ret, this)) {
                        break;
                    }
                    if (msgCtx->choiceIndex == 0) {
                        Audio_PlaySfx_MessageDecide();
                        if (gSaveContext.save.saveInfo.playerData.rupees >= play->msgCtx.unk1206C) {
                            if (Inventory_HasEmptyBottle()) {
                                this->actionFunc = func_808F3C40;
                                Actor_OfferGetItem(&this->actor, play, GI_MILK, 500.0f, 100.0f);
                                Rupees_ChangeBy(-play->msgCtx.unk1206C);
                                ret = true;
                            } else {
                                Actor_ContinueText(play, &this->actor, 0x3469);
                                ret = false;
                            }
                        } else {
                            Audio_PlaySfx(NA_SE_SY_ERROR);
                            Actor_ContinueText(play, &this->actor, 0x3468);
                            ret = false;
                        }
                    } else {
                        Audio_PlaySfx_MessageCancel();
                        Actor_ContinueText(play, &this->actor, 0x3467);
                        ret = false;
                    }
                    break;

                case 0x3467:
                case 0x3468:
                case 0x3469:
                    Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_GORMAN_BROTHERS);
                    ret = true;
                    break;

                case 0x346B:
                    Actor_ContinueText(play, &this->actor, 0x346C);
                    ret = false;
                    break;

                case 0x346C:
                    Actor_ContinueText(play, &this->actor, 0x3466);
                    ret = false;
                    break;

                case 0x346A:
                    this->actionFunc = func_808F5A94;
                    Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_GORMAN_BROTHERS);
                    break;
            }
            break;

        case 2:
            switch (textId) {
                case 0x346E:
                    func_808F4108(this, play, 0x346F);
                    ret = false;
                    break;

                case 0x3470:
                    Actor_ContinueText(play, &this->actor, 0x3471);
                    ret = false;
                    break;

                case 0x3483:
                    Actor_ContinueText(play, &this->actor, 0x3471);
                    ret = false;
                    break;

                case 0x3471:
                    func_808F4150(play, this, arg2, msgCtx);
                    ret = false;
                    break;

                case 0x3472:
                    func_808F43E0(this);
                    CLEAR_WEEKEVENTREG(WEEKEVENTREG_56_08);
                    Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_GORMAN_BROTHERS);
                    ret = true;
                    break;

                case 0x3473:
                    CLEAR_WEEKEVENTREG(WEEKEVENTREG_56_08);
                    Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_GORMAN_BROTHERS);
                    break;

                case 0x3475:
                    SET_WEEKEVENTREG_HORSE_RACE_STATE(WEEKEVENTREG_HORSE_RACE_STATE_START);
                    Environment_ForcePlaySequence(NA_BGM_HORSE);
                    play->nextEntrance = ENTRANCE(GORMAN_TRACK, 5);
                    play->transitionType = TRANS_TYPE_FADE_WHITE_FAST;
                    play->transitionTrigger = TRANS_TRIGGER_START;
                    SET_WEEKEVENTREG(WEEKEVENTREG_57_01);
                    break;

                case 0x3478:
                    if (msgCtx->choiceIndex == 0) {
                        func_808F4150(play, this, arg2, msgCtx);
                        ret = false;
                    } else {
                        Audio_PlaySfx_MessageCancel();
                        CLEAR_WEEKEVENTREG(WEEKEVENTREG_56_08);
                        func_808F4108(this, play, 0x3479);
                        ret = false;
                    }
                    break;

                case 0x347B:
                    func_808F4108(this, play, 0x347C);
                    CLEAR_WEEKEVENTREG(WEEKEVENTREG_56_08);
                    ret = false;
                    break;

                default:
                    break;
            }
            break;

        case 3:
            switch (textId) {
                case 0x346D:
                    func_808F4108(this, play, 0x346E);
                    SET_WEEKEVENTREG(WEEKEVENTREG_56_04);
                    SET_WEEKEVENTREG(WEEKEVENTREG_56_08);
                    ret = false;
                    break;

                case 0x346F:
                    func_808F4108(this, play, 0x3470);
                    ret = false;
                    break;

                case 0x3482:
                    func_808F4108(this, play, 0x3483);
                    SET_WEEKEVENTREG(WEEKEVENTREG_56_08);
                    ret = false;
                    break;

                case 0x3484:
                    Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_GORMAN_BROTHERS);
                    break;

                case 0x3485:
                    Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_GORMAN_BROTHERS);
                    break;

                case 0x3474:
                    func_808F4108(this, play, 0x3475);
                    ret = false;
                    break;

                case 0x3476:
                    Actor_ContinueText(play, &this->actor, 0x3477);
                    EnIn_ChangeAnim(&this->skelAnime, ENIN_ANIM_1);
                    EnIn_ChangeAnim(&this->unk4A4->skelAnime, ENIN_ANIM_7);
                    ret = false;
                    break;

                case 0x3477:
                    SET_WEEKEVENTREG(WEEKEVENTREG_56_08);
                    func_808F4108(this, play, 0x3478);
                    ret = false;
                    break;

                case 0x347A:
                    EnIn_ChangeAnim(&this->skelAnime, ENIN_ANIM_1);
                    EnIn_ChangeAnim(&this->unk4A4->skelAnime, ENIN_ANIM_7);
                    if (GameInteractor_Should(VB_HAVE_GARO_MASK, INV_CONTENT(ITEM_MASK_GARO) == ITEM_MASK_GARO)) {
                        Actor_ContinueText(play, &this->actor, 0x347E);
                        ret = false;
                    } else {
                        SET_WEEKEVENTREG(WEEKEVENTREG_56_08);
                        func_808F4108(this, play, 0x347B);
                        ret = false;
                    }
                    break;

                case 0x347E:
                    func_808F35D8(this, play);
                    if (Inventory_HasEmptyBottle()) {
                        this->actionFunc = func_808F3B40;
                        Actor_OfferGetItem(&this->actor, play, GI_MILK, 500.0f, 100.0f);
                        ret = true;
                    } else {
                        Actor_ContinueText(play, &this->actor, 0x347F);
                        ret = false;
                    }
                    break;

                case 0x347F:
                    Actor_ContinueText(play, &this->actor, 0x3480);
                    ret = false;
                    break;

                case 0x3480:
                    func_808F43E0(this);
                    Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_GORMAN_BROTHERS);
                    ret = true;
                    break;

                case 0x3479:
                    func_808F43E0(this);
                    func_808F35D8(this, play);
                    Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_GORMAN_BROTHERS);
                    ret = true;
                    break;

                case 0x347C:
                    this->actionFunc = func_808F3D40;
                    Actor_OfferGetItem(&this->actor, play, GI_MASK_GARO, 500.0f, 100.0f);
                    func_808F35D8(this, play);
                    ret = true;
                    break;

                case 0x3481:
                    this->actionFunc = func_808F5A34;
                    func_808F43E0(this);
                    Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_GORMAN_BROTHERS);
                    ret = true;
                    break;

                case 0x347D:
                    this->actionFunc = func_808F5A34;
                    func_808F43E0(this);
                    Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_GORMAN_BROTHERS);
                    Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_RECEIVED_GAROS_MASK);
                    ret = true;
                    break;

                default:
                    break;
            }
            break;

        case 4:
            switch (textId) {
                case 0x3486:
                    Actor_ContinueText(play, &this->actor, 0x3487);
                    SET_WEEKEVENTREG(WEEKEVENTREG_16_01);
                    ret = false;
                    break;

                case 0x3487:
                    Actor_ContinueText(play, &this->actor, 0x3488);
                    ret = false;
                    break;

                case 0x3488:
                    Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_GORMAN_BROTHERS);
                    ret = true;
                    break;

                case 0x3489:
                    Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_GORMAN_BROTHERS);
                    break;

                case 0x348A:
                    Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_GORMAN_BROTHERS);
                    break;

                case 0x348B:
                    Actor_ContinueText(play, &this->actor, 0x348C);
                    ret = false;
                    break;

                case 0x348C:
                    Actor_ContinueText(play, &this->actor, 0x348D);
                    ret = false;
                    break;

                case 0x348D:
                    Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_GORMAN_BROTHERS);
                    ret = true;
                    break;

                default:
                    break;
            }
            break;

        case 5:
            switch (textId) {
                case 0x3468:
                case 0x3469:
                case 0x3491:
                    Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_GORMAN_BROTHERS);
                    break;

                case 0x348E:
                case 0x34B3:
                    Actor_ContinueText(play, &this->actor, 0x348F);
                    SET_WEEKEVENTREG(WEEKEVENTREG_16_02);
                    ret = false;
                    break;

                case 0x3493:
                    Actor_ContinueText(play, &this->actor, 0x3494);
                    ret = false;
                    break;

                case 0x348F:
                case 0x3494:
                    Actor_ContinueText(play, &this->actor, 0x3490);
                    ret = false;
                    break;

                case 0x3490:
                    if (GameInteractor_Should(VB_BUY_GORMAN_MILK, false, &ret, this)) {
                        break;
                    }
                    if (msgCtx->choiceIndex == 0) {
                        Audio_PlaySfx_MessageDecide();
                        if (gSaveContext.save.saveInfo.playerData.rupees >= play->msgCtx.unk1206C) {
                            if (Inventory_HasEmptyBottle()) {
                                this->actionFunc = func_808F3C40;
                                Actor_OfferGetItem(&this->actor, play, GI_MILK, 500.0f, 100.0f);
                                Rupees_ChangeBy(-play->msgCtx.unk1206C);
                                ret = true;
                            } else {
                                Actor_ContinueText(play, &this->actor, 0x3469);
                                ret = false;
                            }
                        } else {
                            Audio_PlaySfx(NA_SE_SY_ERROR);
                            Actor_ContinueText(play, &this->actor, 0x3468);
                            ret = false;
                        }
                    } else {
                        Audio_PlaySfx_MessageCancel();
                        Actor_ContinueText(play, &this->actor, 0x3491);
                        ret = false;
                    }
                    break;

                case 0x3492:
                    this->actionFunc = func_808F5A94;
                    Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_GORMAN_BROTHERS);
                    break;
            }
            break;

        case 7:
            switch (textId) {
                case 0x34A8:
                    Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_GORMAN_BROTHERS);
                    break;

                case 0x34A7:
                    Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_GORMAN_BROTHERS);
                    break;

                case 0x3495:
                    func_808F4108(this, play, 0x3496);
                    SET_WEEKEVENTREG(WEEKEVENTREG_16_04);
                    SET_WEEKEVENTREG(WEEKEVENTREG_56_08);
                    ret = false;
                    break;

                case 0x3497:
                    func_808F4108(this, play, 0x3498);
                    ret = false;
                    break;

                case 0x34A4:
                    this->actionFunc = func_808F5A34;
                    func_808F43E0(this);
                    Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_GORMAN_BROTHERS);
                    ret = true;
                    break;

                case 0x34A5:
                    func_808F4108(this, play, 0x34A6);
                    SET_WEEKEVENTREG(WEEKEVENTREG_56_08);
                    ret = false;
                    break;

                case 0x3473:
                    CLEAR_WEEKEVENTREG(WEEKEVENTREG_56_08);
                    Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_GORMAN_BROTHERS);
                    break;

                case 0x3474:
                    Actor_ContinueText(play, &this->actor, 0x3475);
                    ret = false;
                    break;

                case 0x3475:
                    SET_WEEKEVENTREG_HORSE_RACE_STATE(WEEKEVENTREG_HORSE_RACE_STATE_START);
                    Environment_ForcePlaySequence(NA_BGM_HORSE);
                    play->nextEntrance = ENTRANCE(GORMAN_TRACK, 5);
                    play->transitionType = TRANS_TYPE_FADE_WHITE_FAST;
                    play->transitionTrigger = TRANS_TRIGGER_START;
                    SET_WEEKEVENTREG(WEEKEVENTREG_57_01);
                    break;

                case 0x349D:
                    EnIn_ChangeAnim(&this->skelAnime, ENIN_ANIM_1);
                    EnIn_ChangeAnim(&this->unk4A4->skelAnime, ENIN_ANIM_7);
                    if (GameInteractor_Should(VB_HAVE_GARO_MASK, INV_CONTENT(ITEM_MASK_GARO) == ITEM_MASK_GARO)) {
                        Actor_ContinueText(play, &this->actor, 0x34A1);
                        ret = false;
                    } else {
                        SET_WEEKEVENTREG(WEEKEVENTREG_56_08);
                        func_808F4108(this, play, 0x349E);
                        ret = false;
                    }
                    break;

                case 0x349F:
                    this->actionFunc = func_808F3D40;
                    Actor_OfferGetItem(&this->actor, play, GI_MASK_GARO, 500.0f, 100.0f);
                    func_808F35D8(this, play);
                    ret = true;
                    break;

                case 0x34A0:
                    this->actionFunc = func_808F5A34;
                    Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_GORMAN_BROTHERS);
                    Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_RECEIVED_GAROS_MASK);
                    func_808F43E0(this);
                    ret = true;
                    break;

                case 0x34A1:
                    func_808F35D8(this, play);
                    if (Inventory_HasEmptyBottle()) {
                        this->actionFunc = func_808F3B40;
                        Actor_OfferGetItem(&this->actor, play, GI_MILK, 500.0f, 100.0f);
                        ret = true;
                    } else {
                        Actor_ContinueText(play, &this->actor, 0x34A2);
                        ret = false;
                    }
                    break;

                case 0x34A2:
                    Actor_ContinueText(play, &this->actor, 0x34A3);
                    ret = false;
                    break;

                case 0x34A3:
                    func_808F43E0(this);
                    Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_GORMAN_BROTHERS);
                    ret = true;
                    break;

                case 0x3499:
                    Actor_ContinueText(play, &this->actor, 0x349A);
                    EnIn_ChangeAnim(&this->skelAnime, ENIN_ANIM_1);
                    EnIn_ChangeAnim(&this->unk4A4->skelAnime, ENIN_ANIM_7);
                    ret = false;
                    break;

                case 0x349A:
                    Actor_ContinueText(play, &this->actor, 0x349B);
                    ret = false;
                    break;

                case 0x349B:
                    if (msgCtx->choiceIndex == 0) {
                        func_808F4270(play, this, arg2, msgCtx, true);
                        ret = false;
                    } else {
                        Audio_PlaySfx_MessageCancel();
                        Actor_ContinueText(play, &this->actor, 0x349C);
                        ret = false;
                    }
                    break;

                case 0x349C:
                    func_808F43E0(this);
                    func_808F35D8(this, play);
                    Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_GORMAN_BROTHERS);
                    ret = true;
                    break;

                default:
                    break;
            }
            break;

        case 6:
            switch (textId) {
                case 0x3496:
                    func_808F4108(this, play, 0x3497);
                    ret = false;
                    break;

                case 0x3498:
                case 0x34A6:
                    Actor_ContinueText(play, &this->actor, 0x3471);
                    ret = false;
                    break;

                case 0x3471:
                    func_808F4270(play, this, arg2, msgCtx, false);
                    ret = false;
                    break;

                case 0x3472:
                    func_808F43E0(this);
                    CLEAR_WEEKEVENTREG(WEEKEVENTREG_56_08);
                    Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_GORMAN_BROTHERS);
                    ret = true;
                    break;

                case 0x349E:
                    func_808F4108(this, play, 0x349F);
                    CLEAR_WEEKEVENTREG(WEEKEVENTREG_56_08);
                    ret = false;
                    break;

                default:
                    break;
            }
            break;

        default:
            break;
    }
    return ret;
}

s32 func_808F5674(PlayState* play, EnIn* this, s32 arg2) {
    s32 pad;
    s32 ret = false;

    switch (Message_GetState(&play->msgCtx)) {
        case TEXT_STATE_CLOSING:
            func_808F4054(play, this, arg2, this->actor.textId);
            ret = true;
            break;

        case TEXT_STATE_CHOICE:
        case TEXT_STATE_EVENT:
            if (Message_ShouldAdvance(play) && func_808F4414(play, this, arg2)) {
                Message_CloseTextbox(play);
                ret = true;
            }
            break;

        default:
            break;
    }
    return ret;
}

s32 func_808F5728(PlayState* play, EnIn* this, s32 arg2, s32* arg3) {
    s16 rotDiff;
    s16 yawDiff;

    if (*arg3 == 4) {
        return 0;
    }
    if (*arg3 == 2) {
        Message_StartTextbox(play, this->actor.textId, &this->actor);
        *arg3 = 1;
        return 0;
    }
    if (*arg3 == 3) {
        Message_ContinueTextbox(play, this->actor.textId);
        *arg3 = 1;
        return 0;
    }
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        *arg3 = 1;
        return 1;
    }
    if (*arg3 == 1) {
        Player* player = GET_PLAYER(play);

        func_808F5994(this, play, &player->actor.world.pos, 0xC80);
    } else {
        rotDiff = this->actor.home.rot.y - this->actor.world.rot.y;
        if (rotDiff > 0x320) {
            this->actor.world.rot.y += 0x320;
        } else if (rotDiff < -0x320) {
            this->actor.world.rot.y -= 0x320;
        } else {
            this->actor.world.rot.y += rotDiff;
        }
        this->actor.shape.rot.y = this->actor.world.rot.y;
    }
    if (*arg3 == 1) {
        if (func_808F5674(play, this, arg2)) {
            *arg3 = 0;
        }
        return 0;
    }
    if (!Actor_OnScreen(play, &this->actor)) {
        return 0;
    }
    yawDiff = ABS_ALT(BINANG_SUB(this->actor.yawTowardsPlayer, this->actor.shape.rot.y));
    if (yawDiff >= 0x4300) {
        return 0;
    }
    if (this->actor.xyzDistToPlayerSq > SQ(160.0f) && !this->actor.isLockedOn) {
        return 0;
    }
    if (this->actor.xyzDistToPlayerSq <= SQ(80.0f)) {
        if (Actor_OfferTalk(&this->actor, play, 80.0f)) {
            this->actor.textId = func_808F3DD4(play, this, arg2);
        }
    } else if (Actor_OfferTalkNearColChkInfoCylinder(&this->actor, play)) {
        this->actor.textId = func_808F3DD4(play, this, arg2);
    }
    return 0;
}

s32 func_808F5994(EnIn* this, PlayState* play, Vec3f* arg2, s16 arg3) {
    s32 ret = 0;
    s16 yaw = Math_Vec3f_Yaw(&this->actor.world.pos, arg2) - this->actor.world.rot.y;

    if (yaw > arg3) {
        this->actor.world.rot.y += arg3;
    } else if (yaw < -arg3) {
        this->actor.world.rot.y -= arg3;
    } else {
        this->actor.world.rot.y += yaw;
        ret = 1;
    }
    this->actor.shape.rot.y = this->actor.world.rot.y;

    return ret;
}

void func_808F5A34(EnIn* this, PlayState* play) {
    if (gSaveContext.save.day != 3) {
        func_808F5728(play, this, 3, &this->unk48C);
    } else {
        func_808F5728(play, this, 7, &this->unk48C);
    }
}

void func_808F5A94(EnIn* this, PlayState* play) {
    if (Horse_IsActive(play, &play->actorCtx)) {
        if (gSaveContext.save.day == 3) {
            func_808F5728(play, this, 7, &this->unk48C);
        } else {
            func_808F5728(play, this, 3, &this->unk48C);
        }
    } else {
        if (gSaveContext.save.day == 3) {
            func_808F5728(play, this, 5, &this->unk48C);
        } else {
            func_808F5728(play, this, 1, &this->unk48C);
        }
    }
}

void func_808F5B58(EnIn* this, PlayState* play) {
    if (Horse_IsActive(play, &play->actorCtx)) {
        if (((Player_GetMask(play) == PLAYER_MASK_CIRCUS_LEADER) && CHECK_WEEKEVENTREG(WEEKEVENTREG_63_40)) ||
            CHECK_WEEKEVENTREG(WEEKEVENTREG_56_08)) {
            if (gSaveContext.save.day == 3) {
                func_808F5728(play, this, 6, &this->unk48C);
            } else {
                func_808F5728(play, this, 2, &this->unk48C);
            }
        }
    } else if ((Player_GetMask(play) != PLAYER_MASK_CIRCUS_LEADER) ||
               ((Player_GetMask(play) == PLAYER_MASK_CIRCUS_LEADER) && CHECK_WEEKEVENTREG(WEEKEVENTREG_63_40))) {
        if (gSaveContext.save.day == 3) {
            func_808F5728(play, this, 4, &this->unk48C);
        } else {
            func_808F5728(play, this, 0, &this->unk48C);
        }
    }
}

void func_808F5C98(EnIn* this, PlayState* play) {
    if (this->unk4B0 == WEEKEVENTREG_HORSE_RACE_STATE_END) {
        this->actionFunc = func_808F5B58;
    }
    if (((Player_GetMask(play) == PLAYER_MASK_CIRCUS_LEADER) && CHECK_WEEKEVENTREG(WEEKEVENTREG_63_40)) ||
        CHECK_WEEKEVENTREG(WEEKEVENTREG_56_08)) {
        if (gSaveContext.save.day != 3) {
            func_808F5728(play, this, 2, &this->unk48C);
        } else {
            func_808F5728(play, this, 6, &this->unk48C);
        }
    }
    if (this->unk4A8 == 2) {
        if (this->unk4B0 == WEEKEVENTREG_HORSE_RACE_STATE_2) {
            Actor_PlaySfx(&this->actor, NA_SE_VO_IN_LOST);
        } else {
            Actor_PlaySfx(&this->actor, NA_SE_VO_IN_JOY0);
        }
        this->unk4A8 = 3;
    } else if (this->unk4A8 < 3) {
        this->unk4A8++;
    }
}

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(cullingVolumeScale, 1200, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 300, ICHAIN_STOP),
};

void EnIn_Init(Actor* thisx, PlayState* play) {
    EnIn* this = (EnIn*)thisx;
    s32 pad[2];
    s16 type;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &object_in_Skel_014EA8, NULL, this->jointTable, this->morphTable,
                       OBJECT_IN_LIMB_MAX);
    EnIn_ChangeAnim(&this->skelAnime, ENIN_ANIM_0);
    Collider_InitCylinder(play, &this->colliderCylinder);
    Collider_SetCylinder(play, &this->colliderCylinder, &this->actor, &sCylinderInit);
    CollisionCheck_SetInfo2(&this->actor.colChkInfo, &sDamageTable, &sColChkInfoInit2);
    this->unk48A = 0;
    this->unk48C = 0;
    this->unk4AC = 0;
    type = ENIN_GET_TYPE(thisx);
    this->unk4B0 = GET_WEEKEVENTREG_HORSE_RACE_STATE;
    if ((type == ENIN_HORSE_RIDER_BLUE_SHIRT) || (type == ENIN_BLUE_SHIRT)) {
        this->unk4AC |= 8;
    }
    if ((type == ENIN_HORSE_RIDER_YELLOW_SHIRT) || (type == ENIN_HORSE_RIDER_BLUE_SHIRT)) {
        ActorShape_Init(&this->actor.shape, 0.0f, NULL, 0.0f);
        this->animIndex2 = ENIN_ANIM2_1;
        Animation_Change(&this->skelAnime, &object_in_Anim_016A60, 1.0f, 0.0f,
                         Animation_GetLastFrame(&object_in_Anim_016A60), ANIMMODE_ONCE, 0.0f);
        Actor_SetScale(&this->actor, 0.01f);
        this->unk23C = 0;
        this->unk23D = 1;
        this->actionFunc = func_808F374C;
    } else {
        Collider_InitJntSph(play, &this->colliderJntSph);
        Collider_SetJntSph(play, &this->colliderJntSph, &this->actor, &sJntSphInit, &this->colliderJntSphElement);
        Actor_SetScale(&this->actor, 0.01f);
        this->actor.gravity = -4.0f;
        this->path = SubS_GetPathByIndex(play, ENIN_GET_PATH_INDEX(&this->actor), ENIN_PATH_INDEX_NONE);
        this->unk23D = 0;
        if ((type == ENIN_YELLOW_SHIRT) || (type == ENIN_BLUE_SHIRT)) {
            if ((GET_WEEKEVENTREG_HORSE_RACE_STATE == WEEKEVENTREG_HORSE_RACE_STATE_2) ||
                (GET_WEEKEVENTREG_HORSE_RACE_STATE == WEEKEVENTREG_HORSE_RACE_STATE_3)) {
                CLEAR_WEEKEVENTREG(WEEKEVENTREG_56_08);
                this->unk4A8 = 0;
                this->unk4AC |= 2;
                func_808F35AC(this, play);
                this->unk23C = 0;
                D_801BDAA0 = false;

                if (GET_WEEKEVENTREG_HORSE_RACE_STATE == WEEKEVENTREG_HORSE_RACE_STATE_2) {
                    EnIn_ChangeAnim(&this->skelAnime, ENIN_ANIM_6);
                } else {
                    EnIn_ChangeAnim(&this->skelAnime, ENIN_ANIM_4);
                }
                if (GET_WEEKEVENTREG_HORSE_RACE_STATE == WEEKEVENTREG_HORSE_RACE_STATE_2) {
                    this->skelAnime.curFrame = ((Rand_ZeroOne() * 0.6f) + 0.2f) * this->skelAnime.endFrame;
                }
                if (this->unk4AC & 8) {
                    this->actionFunc = func_808F39DC;
                } else {
                    this->actionFunc = func_808F5C98;
                }
            } else {
                if (GET_WEEKEVENTREG_HORSE_RACE_STATE != WEEKEVENTREG_HORSE_RACE_STATE_START) {
                    CLEAR_WEEKEVENTREG(WEEKEVENTREG_56_08);
                    this->unk23C = 0;
                    this->unk4AC |= 2;
                    if (type == ENIN_BLUE_SHIRT) {
                        if (func_808F33B8()) {
                            EnIn_ChangeAnim(&this->skelAnime, ENIN_ANIM_0);
                            this->actionFunc = func_808F5A94;
                        } else {
                            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_ESCORTED_CREMIA)) {
                                Actor_Spawn(&play->actorCtx, play, ACTOR_EN_KANBAN, this->actor.world.pos.x,
                                            this->actor.world.pos.y, this->actor.world.pos.z, this->actor.shape.rot.x,
                                            this->actor.shape.rot.y, this->actor.shape.rot.z, 0xF);
                                Actor_Kill(&this->actor);
                            } else {
                                EnIn_ChangeAnim(&this->skelAnime, ENIN_ANIM_0);
                                this->actionFunc = func_808F5A94;
                            }
                        }
                    } else {
                        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_ESCORTED_CREMIA)) {
                            Actor_Kill(&this->actor);
                        } else {
                            EnIn_ChangeAnim(&this->skelAnime, ENIN_ANIM_7);
                            this->actionFunc = func_808F5B58;
                        }
                    }
                } else {
                    Actor_Kill(&this->actor);
                }
            }
        } else {
            this->actionFunc = func_808F3618;
        }
    }
}

void EnIn_Destroy(Actor* thisx, PlayState* play) {
    EnIn* this = (EnIn*)thisx;

    Collider_DestroyCylinder(play, &this->colliderCylinder);
}

void EnIn_Update(Actor* thisx, PlayState* play) {
    EnIn* this = (EnIn*)thisx;

    func_808F3310(this, play);
    func_808F3334(this, play);
    if (this->unk4AC & 2) {
        this->unk4AC &= ~2;
        func_808F38F8(this, play);
    }
    if (Player_GetMask(play) == PLAYER_MASK_CIRCUS_LEADER) {
        this->unk4AC |= 0x40;
    } else {
        this->unk4AC &= ~0x40;
    }
    this->actionFunc(this, play);
    Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_4);
    func_808F3414(this, play);
    func_808F32A0(this, play);
}

void func_808F6334(EnIn* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 talkState = Message_GetState(&play->msgCtx);

    this->unk4C4 += (this->unk4C0 != 0.0f) ? 40.0f : -40.0f;
    this->unk4C4 = CLAMP(this->unk4C4, 0.0f, 80.0f);

    Matrix_Translate(this->unk4C4, 0.0f, 0.0f, MTXMODE_APPLY);
    if ((&this->actor == player->talkActor) &&
        ((play->msgCtx.currentTextId < 0xFF) || (play->msgCtx.currentTextId > 0x200)) &&
        (talkState == TEXT_STATE_FADING) && (this->prevTalkState == TEXT_STATE_FADING)) {
        if (!(play->state.frames & 1)) {
            this->unk4C0 = (this->unk4C0 != 0.0f) ? 0.0f : 1.0f;
        }
    } else {
        this->unk4C0 = 0.0f;
    }
    this->prevTalkState = talkState;
}

s32 EnIn_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnIn* this = (EnIn*)thisx;
    s32 pad;
    Gfx* sp50[OBJECT_IN_LIMB_MAX] = {
        NULL,                // OBJECT_IN_LIMB_NONE
        NULL,                // OBJECT_IN_LIMB_01
        object_in_DL_0149A8, // OBJECT_IN_LIMB_02
        object_in_DL_014AE0, // OBJECT_IN_LIMB_03
        object_in_DL_014C30, // OBJECT_IN_LIMB_04
        object_in_DL_0145D8, // OBJECT_IN_LIMB_05
        object_in_DL_014710, // OBJECT_IN_LIMB_06
        object_in_DL_014860, // OBJECT_IN_LIMB_07
        object_in_DL_014420, // OBJECT_IN_LIMB_08
        object_in_DL_012A78, // OBJECT_IN_LIMB_09
        object_in_DL_013DE0, // OBJECT_IN_LIMB_0A
        object_in_DL_013F10, // OBJECT_IN_LIMB_0B
        object_in_DL_014040, // OBJECT_IN_LIMB_0C
        object_in_DL_0137A0, // OBJECT_IN_LIMB_0D
        object_in_DL_0138D0, // OBJECT_IN_LIMB_0E
        object_in_DL_013A00, // OBJECT_IN_LIMB_0F
        object_in_DL_012DF8, // OBJECT_IN_LIMB_10
        object_in_DL_013670, // OBJECT_IN_LIMB_11
        object_in_DL_013540, // OBJECT_IN_LIMB_12
        object_in_DL_013440, // OBJECT_IN_LIMB_13
    };

    if ((this->unk23C != 0) && (limbIndex != OBJECT_IN_LIMB_10)) {
        if (sp50[limbIndex] != NULL) {
            *dList = sp50[limbIndex];
        }
    }
    if ((this->unk4AC & 4) && (limbIndex == OBJECT_IN_LIMB_10)) {
        *dList = object_in_DL_01C528;
    }

    OPEN_DISPS(play->state.gfxCtx);

    if (limbIndex == OBJECT_IN_LIMB_10) {
        TexturePtr sp38[] = { object_in_Tex_0035E0, object_in_Tex_004820, object_in_Tex_004C20, object_in_Tex_0043E0 };

        gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(sp38[this->unk482]));
        gSPSegment(POLY_OPA_DISP++, 0x09, Lib_SegmentedToVirtual(object_in_Tex_003520));
    } else {
        AnimatedMat_DrawStep(play, Lib_SegmentedToVirtual(object_in_Matanimheader_001C30), this->unk4AC & 8 ? 1 : 0);
    }

    CLOSE_DISPS(play->state.gfxCtx);

    if (limbIndex == OBJECT_IN_LIMB_10) {
        Matrix_Translate(1500.0f, 0.0f, 0.0f, MTXMODE_APPLY);
        Matrix_RotateXS(this->headRot.y, MTXMODE_APPLY);
        Matrix_RotateZS(-this->headRot.x, MTXMODE_APPLY);
        Matrix_Translate(-1500.0f, 0.0f, 0.0f, MTXMODE_APPLY);
        func_808F6334(this, play);
    }

    if (limbIndex == OBJECT_IN_LIMB_09) {
        Matrix_RotateYS(this->torsoRot.y, MTXMODE_APPLY);
        Matrix_RotateXS(this->torsoRot.x, MTXMODE_APPLY);
    }

    if ((limbIndex == OBJECT_IN_LIMB_09) || (limbIndex == OBJECT_IN_LIMB_0A) || (limbIndex == OBJECT_IN_LIMB_0D)) {
        rot->y += TRUNCF_BINANG(Math_SinS(this->fidgetTableY[limbIndex]) * 200.0f);
        rot->z += TRUNCF_BINANG(Math_CosS(this->fidgetTableZ[limbIndex]) * 200.0f);
    }

    if (this->unk4AC & 0x40) {
        if (limbIndex == OBJECT_IN_LIMB_12) {
            rot->x = 0x1F40;
            rot->y = -0x3E8;
            rot->z = 0x1D4C;
        } else if (limbIndex == OBJECT_IN_LIMB_11) {
            rot->x = -0x1F40;
            rot->y = 0x3E8;
            rot->z = 0x1D4C;
        } else if (limbIndex == OBJECT_IN_LIMB_13) {
            rot->x = 0;
            rot->y = 0x7FFF;
            rot->z = -0x2328;
        }
    }
    return 0;
}

void EnIn_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    EnIn* this = (EnIn*)thisx;
    Vec3f sp50 = { 1600.0f, 0.0f, 0.0f };
    Vec3f sp44 = { 0.0f, 0.0f, 0.0f };

    if (limbIndex == OBJECT_IN_LIMB_10) {
        Matrix_MultVec3f(&sp50, &this->unk4B4);
        Math_Vec3f_Copy(&this->actor.focus.pos, &this->unk4B4);
    }
    if (this->unk23D == 0) {
        Collider_UpdateSpheres(limbIndex, &this->colliderJntSph);
        if (limbIndex == OBJECT_IN_LIMB_04) {
            Matrix_MultVec3f(&sp44, &this->unk248);
        }
        if (limbIndex == OBJECT_IN_LIMB_07) {
            Matrix_MultVec3f(&sp44, &this->unk254);
        }
        if (this->unk23C == 0) {
            if (!(this->unk4AC & 8)) {
                OPEN_DISPS(play->state.gfxCtx);

                if (limbIndex == OBJECT_IN_LIMB_0C) {
                    gSPDisplayList(POLY_OPA_DISP++, object_in_DL_007A70);
                }
                if (limbIndex == OBJECT_IN_LIMB_0F) {
                    gSPDisplayList(POLY_OPA_DISP++, object_in_DL_007C48);
                }

                CLOSE_DISPS(play->state.gfxCtx);
            }
        }
    }
    if (this->unk4AC & 0x20) {
        OPEN_DISPS(play->state.gfxCtx);

        if (limbIndex == OBJECT_IN_LIMB_0C) {
            gSPDisplayList(POLY_OPA_DISP++, object_in_DL_007C48);
        }

        CLOSE_DISPS(play->state.gfxCtx);
    }
}

void EnIn_Draw(Actor* thisx, PlayState* play) {
    EnIn* this = (EnIn*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    gDPPipeSync(POLY_OPA_DISP++);
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnIn_OverrideLimbDraw, EnIn_PostLimbDraw, &this->actor);

    CLOSE_DISPS(play->state.gfxCtx);
}
