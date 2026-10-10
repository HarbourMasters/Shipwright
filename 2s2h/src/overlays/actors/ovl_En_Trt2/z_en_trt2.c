/*
 * File: z_en_trt2.c
 * Overlay: ovl_En_Trt2
 * Description: Kotake in Southern Swamp and Woods of Mystery
 */

#include "z_en_trt2.h"
#include "objects/object_trt/object_trt.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY)

void EnTrt2_Init(Actor* thisx, PlayState* play);
void EnTrt2_Destroy(Actor* thisx, PlayState* play);
void EnTrt2_Update(Actor* thisx, PlayState* play);

void func_80AD46F8(EnTrt2* this);
s32 EnTrt2_HasReachedPoint(EnTrt2* this, Path* path, s32 pointIndex);
s16 func_80AD48F8(Path* path, s32 arg1, Vec3f* arg2, f32* arg3);
f32 func_80AD49B8(Path* path, s32 arg1, Vec3f* arg2, Vec3s* arg3);
void func_80AD4A78(EnTrt2* this, PlayState* play);
s32 func_80AD4B08(PlayState* play);
void func_80AD4DB4(EnTrt2* this, PlayState* play);
void func_80AD4FE4(EnTrt2* this, PlayState* play);
void func_80AD5234(EnTrt2* this, PlayState* play);
void func_80AD56E8(Actor* thisx, PlayState* play);

typedef enum Trt2Animation {
    /*  0 */ TRT2_ANIM_IDLE,
    /*  1 */ TRT2_ANIM_HALF_AWAKE,
    /*  2 */ TRT2_ANIM_SLEEPING,
    /*  3 */ TRT2_ANIM_WAKE_UP,
    /*  4 */ TRT2_ANIM_SURPRISED,
    /*  5 */ TRT2_ANIM_HANDS_ON_COUNTER,
    /*  6 */ TRT2_ANIM_HOVER,
    /*  7 */ TRT2_ANIM_FLY_LOOK_AROUND,
    /*  8 */ TRT2_ANIM_FLY_DOWN,
    /*  9 */ TRT2_ANIM_FLY,
    /* 10 */ TRT2_ANIM_MAX
} Trt2Animation;

static AnimationInfoS sAnimationInfo[TRT2_ANIM_MAX] = {
    { &gKotakeIdleAnim, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },           // TRT2_ANIM_IDLE
    { &gKotakeHalfAwakeAnim, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },      // TRT2_ANIM_HALF_AWAKE
    { &gKotakeSleepingAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },       // TRT2_ANIM_SLEEPING
    { &gKotakeWakeUpAnim, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },         // TRT2_ANIM_WAKE_UP
    { &gKotakeSurprisedAnim, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },      // TRT2_ANIM_SURPRISED
    { &gKotakeHandsOnCounterAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 }, // TRT2_ANIM_HANDS_ON_COUNTER
    { &gKotakeHoverAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },          // TRT2_ANIM_HOVER
    { &gKotakeFlyLookAroundAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },  // TRT2_ANIM_FLY_LOOK_AROUND
    { &gKotakeFlyDownAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },        // TRT2_ANIM_FLY_DOWN
    { &gKotakeFlyAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },            // TRT2_ANIM_FLY
};

ActorProfile En_Trt2_Profile = {
    /**/ ACTOR_EN_TRT2,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_TRT,
    /**/ sizeof(EnTrt2),
    /**/ EnTrt2_Init,
    /**/ EnTrt2_Destroy,
    /**/ EnTrt2_Update,
    /**/ NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK1,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_NONE,
        OCELEM_ON,
    },
    { 32, 56, 30, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 sColChkInfoInit = { 1, 0, 0, 0, MASS_IMMOVABLE };

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(1, 0x0),
    /* Deku Stick     */ DMG_ENTRY(1, 0x0),
    /* Horse trample  */ DMG_ENTRY(1, 0x0),
    /* Explosives     */ DMG_ENTRY(1, 0x1),
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

void EnTrt2_ChangeAnim(SkelAnime* skelAnime, AnimationInfoS* animationInfo, s32 animIndex) {
    f32 endFrame;

    animationInfo += animIndex;

    if (animationInfo->frameCount < 0) {
        endFrame = Animation_GetLastFrame(animationInfo->animation);
    } else {
        endFrame = animationInfo->frameCount;
    }
    Animation_Change(skelAnime, animationInfo->animation, animationInfo->playSpeed, animationInfo->startFrame, endFrame,
                     animationInfo->mode, animationInfo->morphFrames);
}

void func_80AD341C(EnTrt2* this, PlayState* play) {
    this->collider.dim.pos.x = this->actor.world.pos.x;
    this->collider.dim.pos.y = this->actor.world.pos.y;
    this->collider.dim.pos.z = this->actor.world.pos.z;
    CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
}

void func_80AD349C(EnTrt2* this) {
    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_FAILED_RECEIVED_RED_POTION_FOR_KOUME_WOODS) &&
        !CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_RED_POTION_FOR_KOUME)) {
        this->unk_3A8 = 0x88F;
    } else if (this->unk_3A8 == 0) {
        this->unk_3A8 = 0x84B;
    } else if (CHECK_WEEKEVENTREG(WEEKEVENTREG_TALKED_KOUME_INJURED)) {
        this->unk_3A8 = 0x838;
    } else if (CHECK_WEEKEVENTREG(WEEKEVENTREG_TALKED_KOUME_KIOSK_EMPTY)) {
        this->unk_3A8 = 0x84D;
    } else {
        this->unk_3A8 = 0x849;
    }
}

void func_80AD3530(EnTrt2* this, PlayState* play) {
    s16 phi_a1;
    f32 sp30;

    func_80AD46F8(this);
    if (this->path != NULL) {
        phi_a1 = func_80AD48F8(this->path, this->unk_1E4, &this->actor.world.pos, &sp30);
        if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
            phi_a1 = this->actor.wallYaw;
        }
        Math_SmoothStepToS(&this->actor.world.rot.y, phi_a1, 4, 0x3E8, 1);
        this->actor.shape.rot.y = this->actor.world.rot.y;
        if (EnTrt2_HasReachedPoint(this, this->path, this->unk_1E4)) {
            if (this->unk_1E4 >= (this->path->count - 1)) {
                this->unk_1E4 = 0;
            } else {
                this->unk_1E4++;
            }
        }
        Math_ApproachF(&this->actor.speed, 1.5f, 0.2f, 1.0f);
    }

    Actor_MoveWithGravity(&this->actor);

    if (DECR(this->unk_3AE) == 0) {
        this->unk_3AE = Rand_S16Offset(20, 20);
        EnTrt2_ChangeAnim(&this->skelAnime, sAnimationInfo, TRT2_ANIM_FLY_LOOK_AROUND);
        this->unk_3B2 = 5;
    }
}

void func_80AD3664(EnTrt2* this, PlayState* play) {
    if (CutsceneManager_IsNext(this->csId)) {
        CutsceneManager_StartWithPlayerCs(this->csId, &this->actor);
        if (this->unk_3D9 == 0) {
            this->unk_3B2 = 1;
        } else {
            this->unk_3B2 = 2;
        }
    } else {
        CutsceneManager_Queue(this->csId);
        return;
    }
    Actor_PlaySfx_Flagged(&this->actor, NA_SE_EN_KOTAKE_FLY - SFX_FLAG);
}

void func_80AD36EC(EnTrt2* this, PlayState* play) {
    Vec3s sp30;
    f32 sp2C;

    if (this->path != NULL) {
        sp2C = func_80AD49B8(this->path, this->unk_1E4, &this->actor.world.pos, &sp30);
        this->actor.shape.rot.y += 0x1000;
        Math_ApproachF(&this->actor.velocity.y, 0.5f, 0.2f, 1.0f);
        if (sp2C < 5.0f) {
            if (this->unk_1E4 >= (this->path->count - 1)) {
                this->unk_1E4 = 0;
                this->unk_3D9 = 1;
                this->actor.velocity.y = 0.0f;
                this->path = SubS_GetPathByIndex(play, this->path->additionalPathIndex, PATH_INDEX_NONE);
                CutsceneManager_Stop(this->csId);
                this->csId = CutsceneManager_GetAdditionalCsId(this->csId);
                CutsceneManager_Queue(this->csId);
                this->unk_3B2 = 0;
            } else {
                this->unk_1E4++;
            }
        }
    }
    Actor_MoveWithGravity(&this->actor);
    Actor_PlaySfx_Flagged(&this->actor, NA_SE_EN_KOTAKE_FLY - SFX_FLAG);
    if ((this->actor.shape.rot.y >= 0x2800) && (this->actor.shape.rot.y < 0x3800)) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_KOTAKE_ROLL);
    }
}

void func_80AD381C(EnTrt2* this, PlayState* play) {
    if ((CURRENT_DAY == 2) || (CURRENT_DAY == 3)) {
        if (func_80AD4B08(play) == 1) {
            this->actor.world.pos.y -= 50.0f;
            this->unk_3D9 = 0;
            this->unk_3B2 = 0;
            this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
            this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
        }
    } else {
        Actor_Kill(&this->actor);
    }
}

void func_80AD38B8(EnTrt2* this, PlayState* play) {
    Vec3s sp30;

    if (this->unk_3D9 == 2) {
        Actor_Kill(&this->actor);
    }

    if (this->path != NULL) {
        func_80AD49B8(this->path, this->unk_1E4, &this->actor.world.pos, &sp30);
        Math_SmoothStepToS(&this->actor.world.rot.y, sp30.y, 4, 0x3E8, 1);
        this->actor.shape.rot.y = this->actor.world.rot.y;
        Math_SmoothStepToS(&this->actor.shape.rot.x, sp30.x, 4, 0x3E8, 1);
        this->actor.world.rot.x = -this->actor.shape.rot.x;
        if (EnTrt2_HasReachedPoint(this, this->path, this->unk_1E4)) {
            if (this->unk_1E4 >= (this->path->count - 1)) {
                CutsceneManager_Stop(this->csId);
                this->unk_3D9 = 2;
            } else {
                this->unk_1E4++;
            }
        }

        if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
            if (this->unk_1E4 >= (this->path->count - 1)) {
                CutsceneManager_Stop(this->csId);
                this->unk_3D9 = 2;
            } else {
                sp30.y = this->actor.wallYaw;
            }
        }
        Math_ApproachF(&this->actor.speed, 5.0f, 0.2f, 1.0f);
    }

    Actor_MoveWithoutGravity(&this->actor);
    Actor_PlaySfx_Flagged(&this->actor, NA_SE_EN_KOTAKE_FLY - SFX_FLAG);
}

void func_80AD3A24(EnTrt2* this, PlayState* play) {
    func_80AD46F8(this);
    if (this->actor.speed > 0.05f) {
        Math_ApproachF(&this->actor.speed, 0.0f, 0.2f, 1.0f);
    } else if (DECR(this->unk_3AE) == 0) {
        this->unk_3AE = Rand_S16Offset(100, 50);
        EnTrt2_ChangeAnim(&this->skelAnime, sAnimationInfo, TRT2_ANIM_HOVER);
        this->unk_3B2 = 4;
    }
    Actor_MoveWithGravity(&this->actor);
}

void func_80AD3AE4(EnTrt2* this, PlayState* play) {
    if (this->actor.world.pos.y < this->actor.home.pos.y) {
        Math_ApproachF(&this->actor.velocity.y, 0.5f, 0.2f, 0.1f);
    } else {
        this->actor.velocity.y = 0.0f;
        EnTrt2_ChangeAnim(&this->skelAnime, sAnimationInfo, TRT2_ANIM_HOVER);
        this->unk_3B2 = 4;
    }
    Actor_MoveWithGravity(&this->actor);
}

void func_80AD3B6C(EnTrt2* this, PlayState* play) {
    if (DECR(this->unk_3B0) == 0) {
        this->unk_3B0 = 10;
        this->actor.velocity.y = -1.0f;
        EnTrt2_ChangeAnim(&this->skelAnime, sAnimationInfo, TRT2_ANIM_FLY_DOWN);
        this->unk_3B2 = 8;
    }
}

void func_80AD3BE4(EnTrt2* this, PlayState* play) {
    s16 sp2E = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;

    Math_ScaledStepToS(&this->unk_3C0, sp2E, 0x190);
    Math_SmoothStepToS(&this->unk_3C0, sp2E, 4, 0x2710, 0);
    this->actor.world.rot.y += this->unk_3C0;
    this->actor.shape.rot.y = this->actor.world.rot.y;
    if (this->actor.world.pos.y < 5.0f) {
        EnTrt2_ChangeAnim(&this->skelAnime, sAnimationInfo, TRT2_ANIM_FLY);
        this->unk_3B2 = 9;
    }
}

void func_80AD3C94(EnTrt2* this, PlayState* play) {
    if (this->actor.world.pos.y < 5.0f) {
        func_80AD4A78(this, play);
        if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
            this->unk_3B2 = 0xF;
        }
    }
}

void func_80AD3CEC(EnTrt2* this, PlayState* play) {
    u8 talkState = Message_GetState(&play->msgCtx);

    func_80AD46F8(this);
    if (this->unk_3D8) {
        Message_StartTextbox(play, this->unk_3A8, &this->actor);
        this->unk_3D8 = false;
    } else if ((talkState == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
        play->msgCtx.stateTimer = 4;
        EnTrt2_ChangeAnim(&this->skelAnime, sAnimationInfo, TRT2_ANIM_HOVER);
        this->unk_3B2 = 4;
    }
}

void func_80AD3DA4(EnTrt2* this, PlayState* play) {
    this->actor.velocity.y = 0.0f;
    Message_StartTextbox(play, this->unk_3A8, &this->actor);

    if (this->unk_3A8 == 0x838) {
        this->unk_3B2 = 11;
        return;
    } else if (this->unk_3A8 == 0x88F) {
        if (Inventory_HasEmptyBottle()) {
            this->unk_3B2 = 11;
        } else {
            this->unk_3B2 = 10;
        }
    } else {
        this->unk_3B2 = 10;
    }
}

void func_80AD3E34(EnTrt2* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        if (Inventory_HasEmptyBottle()) {
            play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
            play->msgCtx.stateTimer = 4;
            this->unk_3B2 = 12;
        } else {
            SET_WEEKEVENTREG(WEEKEVENTREG_FAILED_RECEIVED_RED_POTION_FOR_KOUME_WOODS);
            this->unk_3A8 = 0x88E;
            Message_StartTextbox(play, this->unk_3A8, &this->actor);
            this->unk_3B2 = 10;
        }
    }
}

void func_80AD3EF0(EnTrt2* this, PlayState* play) {
    u8 talkState = Message_GetState(&play->msgCtx);

    if (talkState == TEXT_STATE_DONE) {
        if (Message_ShouldAdvance(play)) {
            if ((Inventory_HasEmptyBottle() && !CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_RED_POTION_FOR_KOUME)) ||
                !CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_KOTAKE_BOTTLE)) {
                this->unk_3B2 = 12;
            } else {
                SET_WEEKEVENTREG(WEEKEVENTREG_FAILED_RECEIVED_RED_POTION_FOR_KOUME_WOODS);
                this->unk_3A8 = 0x88E;
                Message_StartTextbox(play, this->unk_3A8, &this->actor);
                this->unk_3B2 = 10;
            }
        }
    } else if ((talkState == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
        play->msgCtx.stateTimer = 4;
        this->unk_3B2 = 12;
    }
}

void func_80AD3FF4(EnTrt2* this, PlayState* play) {
    if (Actor_HasParent(&this->actor, play)) {
        if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_KOTAKE_BOTTLE)) {
            SET_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_KOTAKE_BOTTLE);
        }
        SET_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_RED_POTION_FOR_KOUME);
        this->actor.parent = NULL;
        this->unk_3B2 = 14;
    } else if (CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_KOTAKE_BOTTLE)) {
        Actor_OfferGetItem(&this->actor, play, GI_POTION_RED, 300.0f, 300.0f);
    } else {
        Actor_OfferGetItem(&this->actor, play, GI_POTION_RED_BOTTLE, 300.0f, 300.0f);
    }
}

void func_80AD40AC(EnTrt2* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_DONE) && Message_ShouldAdvance(play)) {
        Actor_OfferTalkExchangeEquiCylinder(&this->actor, play, 400.0f, PLAYER_IA_MINUS1);
        this->unk_3B2 = 13;
    }
}

void func_80AD4110(EnTrt2* this, PlayState* play) {
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->unk_3A8 = 0x84C;
        Message_ContinueTextbox(play, this->unk_3A8);
        this->unk_3B2 = 10;
    } else {
        Actor_OfferTalkExchangeEquiCylinder(&this->actor, play, 400.0f, PLAYER_IA_MINUS1);
    }
}

void func_80AD417C(EnTrt2* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        if (this->unk_3A8 == 0x84B) {
            func_80AD349C(this);
            func_80AD3DA4(this, play);
        } else {
            play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
            play->msgCtx.stateTimer = 4;
            if (this->unk_3A8 == 0x84C) {
                EnTrt2_ChangeAnim(&this->skelAnime, sAnimationInfo, TRT2_ANIM_HOVER);
                this->path = SubS_GetPathByIndex(play, ENTRT2_GET_PATH_INDEX(&this->actor), ENTRT2_PATH_INDEX_NONE);
                this->unk_3B2 = 18;
            } else if (this->unk_3A8 == 0x88F) {
                this->unk_3A8 = 0x88E;
                Message_StartTextbox(play, this->unk_3A8, &this->actor);
            } else {
                this->actor.textId = 0;
                this->unk_3B2 = 15;
            }
        }
    }
}

void func_80AD4298(EnTrt2* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (CutsceneManager_IsNext(this->csId)) {
        CutsceneManager_StartWithPlayerCs(this->csId, &this->actor);
        player->stateFlags1 |= PLAYER_STATE1_20;
        this->unk_3B2 = 6;
    } else {
        if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
            CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
        }
        CutsceneManager_Queue(this->csId);
    }
}

void func_80AD431C(EnTrt2* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    player->stateFlags1 &= ~PLAYER_STATE1_20;
    Actor_Kill(&this->actor);
}

void func_80AD434C(EnTrt2* this, PlayState* play) {
    this->actor.shape.rot.y += 0x1000;
    Math_ApproachF(&this->actor.velocity.y, 1.5f, 0.2f, 0.1f);

    if (this->actor.world.pos.y > 200.0f) {
        if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
            this->unk_3B2 = 0x13;
            CutsceneManager_Stop(this->csId);
        } else {
            s32 i;
            Vec3f sp68;
            Vec3f sp5C = { 0.0f, 0.5f, 0.0f };

            for (i = (s32)this->skelAnime.animLength - (s32)this->skelAnime.curFrame; i >= 0; i -= 2) {
                sp68.x = Rand_CenteredFloat(60.0f) + this->actor.world.pos.x;
                sp68.z = Rand_CenteredFloat(60.0f) + this->actor.world.pos.z;
                sp68.y = Rand_CenteredFloat(50.0f) + (this->actor.world.pos.y + 20.0f);
                func_800B3030(play, &sp68, &sp5C, &sp5C, 100, 0, 3);
            }

            Actor_PlaySfx_Flagged(&this->actor, NA_SE_EN_COMMON_EXTINCT_LEV - SFX_FLAG);
        }
    } else if (this->actor.world.pos.y < 5.0f) {
        func_80AD4A78(this, play);
    }

    Actor_MoveWithGravity(&this->actor);

    if ((this->actor.shape.rot.y >= 0x2800) && (this->actor.shape.rot.y < 0x3800)) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_KOTAKE_ROLL);
    }
}

void func_80AD4550(EnTrt2* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    u8 talkState = Message_GetState(&play->msgCtx);

    if ((player->transformation != PLAYER_FORM_HUMAN) && (player->transformation != PLAYER_FORM_FIERCE_DEITY)) {
        EnTrt2_ChangeAnim(&this->skelAnime, sAnimationInfo, TRT2_ANIM_FLY_LOOK_AROUND);
        this->unk_3B2 = 17;
    }

    if ((talkState == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
        play->msgCtx.stateTimer = 4;
    }
}

void func_80AD4608(EnTrt2* this) {
    s16 temp = this->unk_3B6 - 1;

    if (temp != 0) {
        this->unk_3B6 = temp;
        return;
    }

    temp = this->unk_3B8 + 1;
    if (temp >= 3) {
        this->unk_3B8 = 0;
        this->unk_3B6 = (s32)(Rand_ZeroOne() * 60.0f) + 20;
    } else {
        this->unk_3B8 = temp;
        this->unk_3B6 = 1;
    }
}

void func_80AD469C(EnTrt2* this, PlayState* play) {
    SkelAnime_InitFlex(play, &this->skelAnime, &gKotakeSkel, &gKotakeHoverAnim, NULL, NULL, 0);
    this->actor.draw = func_80AD56E8;
}

void func_80AD46F8(EnTrt2* this) {
    if ((this->actor.home.pos.y + 60.0f) < this->actor.world.pos.y) {
        this->actor.velocity.y = -0.2f;
    } else if (this->actor.world.pos.y <= (this->actor.home.pos.y + 50.0f)) {
        this->actor.velocity.y = 0.2f;
    }
}

s32 EnTrt2_HasReachedPoint(EnTrt2* this, Path* path, s32 pointIndex) {
    Vec3s* points = Lib_SegmentedToVirtual(path->points);
    s32 count = path->count;
    s32 index = pointIndex;
    s32 reached = false;
    f32 diffX;
    f32 diffZ;
    f32 px;
    f32 pz;
    f32 d;
    Vec3f point;

    Math_Vec3s_ToVec3f(&point, &points[index]);

    if (index == 0) {
        diffX = points[1].x - points[0].x;
        diffZ = points[1].z - points[0].z;
    } else if (index == (count - 1)) {
        diffX = points[count - 1].x - points[count - 2].x;
        diffZ = points[count - 1].z - points[count - 2].z;
    } else {
        diffX = points[index + 1].x - points[index - 1].x;
        diffZ = points[index + 1].z - points[index - 1].z;
    }

    Math3D_RotateXZPlane(&point, RAD_TO_BINANG(Math_FAtan2F(diffX, diffZ)), &px, &pz, &d);

    if (((px * this->actor.world.pos.x) + (pz * this->actor.world.pos.z) + d) > 0.0f) {
        reached = true;
    }

    return reached;
}

s16 func_80AD48F8(Path* path, s32 arg1, Vec3f* arg2, f32* arg3) {
    Vec3s* points;
    f32 phi_f14;
    f32 phi_f12;

    if (path != NULL) {
        points = Lib_SegmentedToVirtual(path->points);
        points = &points[arg1];
        phi_f14 = points->x - arg2->x;
        phi_f12 = points->z - arg2->z;
    } else {
        phi_f14 = 0.0f;
        phi_f12 = 0.0f;
    }
    *arg3 = SQ(phi_f14) + SQ(phi_f12);
    return RAD_TO_BINANG(Math_Atan2F_XY(phi_f12, phi_f14));
}

f32 func_80AD49B8(Path* path, s32 arg1, Vec3f* arg2, Vec3s* arg3) {
    s32 pad;
    Vec3f sp20;
    Vec3s* points;

    if (path != NULL) {
        points = Lib_SegmentedToVirtual(path->points);
        points = &points[arg1];

        sp20.x = points->x;
        sp20.y = points->y;
        sp20.z = points->z;
    }
    arg3->y = Math_Vec3f_Yaw(arg2, &sp20);
    arg3->x = Math_Vec3f_Pitch(arg2, &sp20);
    return sp20.y - arg2->y;
}

void func_80AD4A78(EnTrt2* this, PlayState* play) {
    Vec3f sp34;

    sp34.x = Rand_CenteredFloat(15.0f) + this->actor.world.pos.x;
    sp34.y = this->actor.world.pos.y;
    sp34.z = Rand_CenteredFloat(15.0f) + this->actor.world.pos.z;
    Actor_SpawnFloorDustRing(play, &this->actor, &sp34, 50.0f, 0, 2.0f, 0, 0, false);
}

s32 func_80AD4B08(PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 ret;

    if (SurfaceType_GetSceneExitIndex(&play->colCtx, player->actor.floorPoly, player->actor.floorBgId) == 0x15) {
        ret = true;
    } else {
        ret = false;
    }
    return ret;
}

s32 func_80AD4B4C(EnTrt2* this, PlayState* play) {
    s32 sp24 = false;
    Player* player = GET_PLAYER(play);

    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        sp24 = true;
        this->actor.speed = 0.0f;
        func_80AD349C(this);
        this->unk_3B4 = this->unk_3B2;
        if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
            if ((player->transformation != PLAYER_FORM_HUMAN) && (player->transformation != PLAYER_FORM_FIERCE_DEITY)) {
                this->unk_3A8 = 0x84F;
            }
            func_80AD3DA4(this, play);
        } else if ((player->transformation == PLAYER_FORM_HUMAN) ||
                   (player->transformation == PLAYER_FORM_FIERCE_DEITY)) {
            this->unk_3B2 = 16;
            this->actor.velocity.y = 1.5f;
        } else {
            this->unk_3A8 = 0x84F;
            this->unk_3D8 = true;
            EnTrt2_ChangeAnim(&this->skelAnime, sAnimationInfo, TRT2_ANIM_FLY_LOOK_AROUND);
            this->unk_3B2 = 7;
        }
    }
    return sp24;
}

s32 func_80AD4C4C(EnTrt2* this) {
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        if (this->actor.xzDistToPlayer < 100.0f) {
            return true;
        }
    } else if ((this->actor.xzDistToPlayer > 50.0f) && (this->actor.xzDistToPlayer < 200.0f)) {
        return true;
    }
    return false;
}

s32 func_80AD4CCC(EnTrt2* this, PlayState* play) {
    s16 sp1E = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
    Player* player = GET_PLAYER(play);

    if (((this->unk_3B2 == 4) || (this->unk_3B2 == 5)) && this->actor.isLockedOn &&
        !(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) &&
        ((player->transformation == PLAYER_FORM_HUMAN) || (player->transformation == PLAYER_FORM_FIERCE_DEITY))) {
        this->actor.speed = 0.0f;
        this->actor.velocity.y = 1.5f;
        this->unk_3B2 = 16;
        return true;
    }

    if (func_80AD4C4C(this) && this->actor.isLockedOn && (sp1E < 0x4000) && (sp1E > -0x4000)) {
        Actor_OfferTalkNearColChkInfoCylinder(&this->actor, play);
    }

    return true;
}

void func_80AD4DB4(EnTrt2* this, PlayState* play) {
    static Vec3f D_80AD5904 = { 0.0f, 50.0f, 0.0f };

    this->actor.flags &= ~ACTOR_FLAG_UPDATE_CULLING_DISABLED;
    Actor_SetObjectDependency(play, &this->actor);
    Actor_SetScale(&this->actor, 0.008f);
    this->path = SubS_GetPathByIndex(play, ENTRT2_GET_PATH_INDEX(&this->actor), ENTRT2_PATH_INDEX_NONE);
    this->unk_3AE = Rand_S16Offset(100, 50);
    this->unk_3B0 = 10;
    this->unk_3A8 = 0;

    this->actor.world.pos.x += D_80AD5904.x;
    this->actor.world.pos.y += D_80AD5904.y;
    this->actor.world.pos.z += D_80AD5904.z;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 20.0f);
    func_80AD469C(this, play);
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    this->actor.colChkInfo.cylRadius = 50;

    this->unk_3C0 = 0;
    this->unk_3D8 = false;
    this->csId = this->actor.csId;
    this->unk_3B6 = 20;
    this->unk_3B8 = 0;
    this->unk_3BC = func_80AD4608;

    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_SAVED_KOUME)) {
        Actor_Kill(&this->actor);
        return;
    }

    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_RED_POTION_FOR_KOUME)) {
        Actor_Kill(&this->actor);
        return;
    }

    if ((play->sceneId == SCENE_20SICHITAI) || (play->sceneId == SCENE_20SICHITAI2)) {
        if (gSaveContext.save.day == 2) {
            if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_15_80)) {
                SET_WEEKEVENTREG(WEEKEVENTREG_15_80);
                this->unk_3B2 = 3;
            } else {
                Actor_Kill(&this->actor);
                return;
            }
        } else {
            Actor_Kill(&this->actor);
            return;
        }
    } else if (gSaveContext.save.day == 2) {
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_15_80)) {
            this->unk_3B2 = 4;
        } else {
            Actor_Kill(&this->actor);
            return;
        }
    } else if (gSaveContext.save.day == 3) {
        this->unk_3B2 = 4;
    }
    this->actionFunc = func_80AD4FE4;
}

void func_80AD4FE4(EnTrt2* this, PlayState* play) {
    static EnTrt2UnkFunc2 D_80AD5910[] = {
        func_80AD3664, func_80AD36EC, func_80AD38B8, func_80AD381C, func_80AD3530, func_80AD3A24, func_80AD434C,
        func_80AD3CEC, func_80AD3BE4, func_80AD3C94, func_80AD417C, func_80AD3EF0, func_80AD3FF4, func_80AD4110,
        func_80AD40AC, func_80AD4550, func_80AD3B6C, func_80AD3AE4, func_80AD4298, func_80AD431C, func_80AD3E34,
    };

    this->unk_3BC(this);

    D_80AD5910[this->unk_3B2](this, play);
    Actor_MoveWithGravity(&this->actor);

    if (play->sceneId != SCENE_20SICHITAI) {
        Actor_UpdateBgCheckInfo(play, &this->actor, 26.0f, 10.0f, 0.0f, UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4);
    }

    Actor_SetFocus(&this->actor, 90.0f);
    SkelAnime_Update(&this->skelAnime);
}

static InitChainEntry sInitChain[] = {
    ICHAIN_U8(attentionRangeType, ATTENTION_RANGE_3, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 500, ICHAIN_STOP),
};

void EnTrt2_Init(Actor* thisx, PlayState* play) {
    EnTrt2* this = (EnTrt2*)thisx;

    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    CollisionCheck_SetInfo2(&this->actor.colChkInfo, &sDamageTable, &sColChkInfoInit);
    Actor_ProcessInitChain(&this->actor, sInitChain);
    this->actionFunc = func_80AD4DB4;
}

void EnTrt2_Destroy(Actor* thisx, PlayState* play) {
    EnTrt2* this = (EnTrt2*)thisx;

    SkelAnime_Free(&this->skelAnime, play);
    Collider_DestroyCylinder(play, &this->collider);
}

void EnTrt2_Update(Actor* thisx, PlayState* play) {
    EnTrt2* this = (EnTrt2*)thisx;

    if ((this->unk_3B2 != 6) && (this->unk_3B2 != 10) && (this->unk_3B2 != 13) && (this->unk_3B2 != 14) &&
        (this->unk_3B2 != 16) && (this->unk_3B2 != 8) && (this->unk_3B2 != 9)) {
        func_80AD4B4C(this, play);
    }

    this->actionFunc(this, play);

    func_80AD341C(this, play);
    func_80AD5234(this, play);

    if ((this->unk_3B2 != 6) && (this->unk_3B2 != 10) && (this->unk_3B2 != 13) && (this->unk_3B2 != 14) &&
        (this->unk_3B2 != 16) && (this->unk_3B2 != 8) && (this->unk_3B2 != 9)) {
        func_80AD4CCC(this, play);
    }
}

void func_80AD5234(EnTrt2* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Vec3f sp40;
    Vec3f sp34;

    Math_SmoothStepToS(&this->unk_3D6, this->actor.yawTowardsPlayer - this->actor.shape.rot.y, 4, 0x38E0, 1);
    this->unk_3D6 = CLAMP(this->unk_3D6, -0x38E0, 0x38E0);

    sp40 = player->actor.world.pos;

    if (this->unk_3B2 == 8) {
        sp40.y = player->bodyPartsPos[PLAYER_BODYPART_HEAD].y + 3.0f;
    } else {
        sp40.y = player->bodyPartsPos[PLAYER_BODYPART_HEAD].y + 45.0f;
    }

    sp34 = this->actor.world.pos;

    Math_SmoothStepToS(&this->unk_3D4, Math_Vec3f_Pitch(&sp34, &sp40), 4, 0x1C70, 1);
    this->unk_3D4 = CLAMP(this->unk_3D4, -0x1C70, 0x1C70);
}

void func_80AD5394(s16 arg0, s16 arg1, Vec3f* arg2, Vec3s* arg3, s32 arg4) {
    Vec3f sp7C;
    Vec3f sp70 = gZeroVec3f;
    Vec3s sp68;
    MtxF sp28;

    Matrix_MultVec3f(&sp70, &sp7C);
    Matrix_Get(&sp28);
    Matrix_MtxFToYXZRot(&sp28, &sp68, false);

    *arg2 = sp7C;

    if (arg4) {
        sp68.x += arg0;
        sp68.y += arg1;
        Math_SmoothStepToS(&arg3->x, sp68.x, 4, 0x1FFE, 1);
        Math_SmoothStepToS(&arg3->y, sp68.y, 4, 0x1FFE, 1);
        Math_SmoothStepToS(&arg3->z, sp68.z, 4, 0x1FFE, 1);
    } else {
        arg3->x = sp68.x;
        arg3->y = sp68.y;
        arg3->z = sp68.z;
    }
}

s32 EnTrt2_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnTrt2* this = (EnTrt2*)thisx;

    if ((limbIndex == KOTAKE_LIMB_TORSO_LIMB) || (limbIndex == KOTAKE_LIMB_LEFT_HAND) ||
        (limbIndex == KOTAKE_LIMB_RIGHT_HAND)) {
        rot->y += TRUNCF_BINANG(Math_SinS(this->unk_33C[limbIndex])) * 200;
        rot->z += TRUNCF_BINANG(Math_CosS(this->unk_372[limbIndex])) * 200;
    }
    return false;
}

void EnTrt2_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    EnTrt2* this = (EnTrt2*)thisx;
    Vec3f sp30 = { 0.0f, -30.0f, 0.0f };
    s32 phi_v0 = false;

    if (this->unk_3B2 >= 8) {
        phi_v0 = true;
    }

    if (limbIndex == KOTAKE_LIMB_HEAD) {
        func_80AD5394(this->unk_3D4, this->unk_3D6, &this->unk_3C8, &this->unk_3C2, phi_v0);
        Matrix_Translate(this->unk_3C8.x, this->unk_3C8.y, this->unk_3C8.z, MTXMODE_NEW);
        Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
        Matrix_RotateYS(this->unk_3C2.y, MTXMODE_APPLY);
        Matrix_RotateXS(this->unk_3C2.x, MTXMODE_APPLY);
        Matrix_RotateZS(this->unk_3C2.z, MTXMODE_APPLY);
        Matrix_MultVec3f(&sp30, &this->actor.focus.pos);
    }
}

void EnTrt2_TransformLimbDraw(PlayState* play, s32 limbIndex, Actor* thisx) {
    EnTrt2* this = (EnTrt2*)thisx;

    if (limbIndex == KOTAKE_LIMB_HEAD) {
        Matrix_Translate(this->unk_3C8.x, this->unk_3C8.y, this->unk_3C8.z, MTXMODE_NEW);
        Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
        Matrix_RotateYS(this->unk_3C2.y, MTXMODE_APPLY);
        Matrix_RotateXS(this->unk_3C2.x, MTXMODE_APPLY);
        Matrix_RotateZS(this->unk_3C2.z, MTXMODE_APPLY);
    }
}

void func_80AD56E8(Actor* thisx, PlayState* play) {
    static TexturePtr D_80AD5978[] = {
        gKotakeEyeOpenTex,
        gKotakeEyeHalfTex,
        gKotakeEyeClosedTex,
    };
    s32 pad;
    EnTrt2* this = (EnTrt2*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(D_80AD5978[this->unk_3B8]));
    gSPSegment(POLY_OPA_DISP++, 0x09, Lib_SegmentedToVirtual(D_80AD5978[this->unk_3B8]));

    SkelAnime_DrawTransformFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                   this->skelAnime.dListCount, EnTrt2_OverrideLimbDraw, EnTrt2_PostLimbDraw,
                                   EnTrt2_TransformLimbDraw, &this->actor);

    CLOSE_DISPS(play->state.gfxCtx);
}
