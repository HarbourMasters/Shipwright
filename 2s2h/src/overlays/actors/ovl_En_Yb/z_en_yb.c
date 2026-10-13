/*
 * File: z_en_yb.c
 * Overlay: ovl_En_Yb
 * Description: Kamaro the Dancing Ghost
 */

#include "z_en_yb.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "2s2h/GameInteractor/GameInteractor.h"

#define FLAGS                                                                                  \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_UPDATE_DURING_OCARINA)

void EnYb_Init(Actor* thisx, PlayState* play);
void EnYb_Destroy(Actor* thisx, PlayState* play);
void EnYb_Update(Actor* thisx, PlayState* play);
void EnYb_Draw(Actor* thisx, PlayState* play);

void EnYb_Idle(EnYb* this, PlayState* play);
void EnYb_TeachingDanceFinish(EnYb* this, PlayState* play);
void EnYb_SetupLeaving(EnYb* this, PlayState* play);

void EnYb_UpdateAnimation(EnYb* this, PlayState* play);
void EnYb_FinishTeachingCutscene(EnYb* this);
void EnYb_Disappear(EnYb* this, PlayState* play);
void EnYb_ReceiveMask(EnYb* this, PlayState* play);
void EnYb_Talk(EnYb* this, PlayState* play);
void EnYb_TeachingDance(EnYb* this, PlayState* play);
void EnYb_WaitForMidnight(EnYb* this, PlayState* play);

void EnYb_ActorShadowFunc(Actor* thisx, Lights* mapper, PlayState* play);
void EnYb_ChangeAnim(PlayState* play, EnYb* this, s16 animIndex, u8 animMode, f32 morphFrames);
s32 EnYb_CanTalk(EnYb* this, PlayState* play);

ActorProfile En_Yb_Profile = {
    /**/ ACTOR_EN_YB,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_YB,
    /**/ sizeof(EnYb),
    /**/ EnYb_Init,
    /**/ EnYb_Destroy,
    /**/ EnYb_Update,
    /**/ EnYb_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_ENEMY,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 20, 40, 0, { 0, 0, 0 } },
};

static AnimationHeader* gYbUnusedAnimations[] = { &object_yb_Anim_000200 };

static PlayerAnimationHeader* gPlayerAnimations[] = {
    &gPlayerAnim_link_normal_wait_free,
    &gPlayerAnim_alink_dance_loop,
};

static Vec3f D_80BFB2E8 = { 0.0f, 0.5f, 0.0f };

static Vec3f D_80BFB2F4 = { 500.0f, -500.0f, 0.0f };

static Vec3f D_80BFB300 = { 500.0f, -500.0f, 0.0f };

void EnYb_Init(Actor* thisx, PlayState* play) {
    EnYb* this = (EnYb*)thisx;
    s16 csId;
    s32 i;

    Actor_SetScale(&this->actor, 0.01f);
    ActorShape_Init(&this->actor.shape, 0.0f, EnYb_ActorShadowFunc, 20.0f);

    //! @bug this alignment is because of player animations, but should be using ALIGN16
    SkelAnime_InitFlex(play, &this->skelAnime, &gYbSkel, &object_yb_Anim_000200,
                       (void*)((uintptr_t)this->jointTable & ~0xF), (void*)((uintptr_t)this->morphTable & ~0xF),
                       YB_LIMB_MAX);

    Animation_PlayLoop(&this->skelAnime, &object_yb_Anim_000200);

    Collider_InitAndSetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    this->actionFunc = EnYb_Idle;
    this->animIndex = 3; // gets overwritten to 2 in EnYb_ChangeAnim later
    this->actor.terminalVelocity = -9.0f;
    this->actor.gravity = -1.0f;

    EnYb_ChangeAnim(play, this, 2, ANIMMODE_LOOP, 0.0f);

    csId = this->actor.csId;
    for (i = 0; i < ARRAY_COUNT(this->csIdList); i++) {
        this->csIdList[i] = csId;
        if (csId != CS_ID_NONE) {
            this->actor.csId = csId;
            csId = CutsceneManager_GetAdditionalCsId(this->actor.csId);
        }
    }

    this->csIdIndex = -1;
    this->actor.csId = this->csIdList[0];

    // between midnight and morning start spawned
    if (CURRENT_TIME < CLOCK_TIME(6, 0)) {
        this->alpha = 255;
    } else { // else (night 6pm to midnight): wait to appear
        this->alpha = 0;
        this->actionFunc = EnYb_WaitForMidnight;
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    }

    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_82_04)) {
        Actor_Kill(&this->actor);
    }
}

void EnYb_Destroy(Actor* thisx, PlayState* play) {
    EnYb* this = (EnYb*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void func_80BFA2FC(PlayState* play) {
    if (GameInteractor_Should(VB_HAVE_KAMAROS_MASK, INV_CONTENT(ITEM_MASK_KAMARO) == ITEM_MASK_KAMARO)) {
        Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_RECEIVED_KAMAROS_MASK);
    }
    Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_KAMARO);
}

/**
 * Custom shadow draw function of type ActorShadowFunc.
 */
void EnYb_ActorShadowFunc(Actor* thisx, Lights* mapper, PlayState* play) {
    Vec3f oldPos;
    EnYb* this = (EnYb*)thisx;

    if (this->alpha > 0) {
        if (this->animIndex == 2) {
            f32 tempScale = (((27.0f - this->shadowPos.y) + this->actor.world.pos.y) * ((1 / 2.25f) * 0.001f)) + 0.01f;
            this->actor.scale.x = tempScale;
        }
        Math_Vec3f_Copy(&oldPos, &this->actor.world.pos);
        Math_Vec3f_Copy(&this->actor.world.pos, &this->shadowPos);
        func_800B4AEC(play, &this->actor, 50.0f);

        if (oldPos.y < this->actor.floorHeight) {
            this->actor.world.pos.y = this->actor.floorHeight;
        } else {
            this->actor.world.pos.y = oldPos.y;
        }

        ActorShadow_DrawCircle(&this->actor, mapper, play);
        Math_Vec3f_Copy(&this->actor.world.pos, &oldPos);
        this->actor.scale.x = 0.01f;
    }
}

void EnYb_ChangeAnim(PlayState* play, EnYb* this, s16 animIndex, u8 animMode, f32 morphFrames) {
    s32 pad;

    if ((animIndex < 0) || (animIndex > 2)) {
        return;
    }

    if ((animIndex == this->animIndex) && (animMode == ANIMMODE_LOOP)) {
        return;
    }

    if (animIndex > 0) {
        if (animMode == ANIMMODE_LOOP) {
            PlayerAnimation_Change(play, &this->skelAnime, gPlayerAnimations[animIndex - 1], 1.0f, 0.0f,
                                   Animation_GetLastFrame(gPlayerAnimations[animIndex - 1]), ANIMMODE_LOOP,
                                   morphFrames);
        } else {
            // unused case, (only called once with animMode = ANIMMODE_LOOP)
            PlayerAnimation_Change(play, &this->skelAnime, gPlayerAnimations[animIndex - 1], 1.0f, 0.0f,
                                   Animation_GetLastFrame(gPlayerAnimations[animIndex - 1]), ANIMMODE_LOOP,
                                   morphFrames);
        }
    } else {
        // unused case, (only called once with animIndex = 2)
        Animation_Change(&this->skelAnime, gYbUnusedAnimations[animIndex], 1.0f, 0.0f,
                         Animation_GetLastFrame(gYbUnusedAnimations[animIndex]), animMode, morphFrames);
    }
    this->animIndex = animIndex;
}

s32 EnYb_CanTalk(EnYb* this, PlayState* play) {
    if ((this->actor.xzDistToPlayer < 100.0f) && Player_IsFacingActor(&this->actor, 0x3000, play) &&
        Actor_IsFacingPlayer(&this->actor, 0x3000)) {
        return true;
    } else {
        return false;
    }
}

void EnYb_UpdateAnimation(EnYb* this, PlayState* play) {
    if (this->animIndex <= 0) {
        SkelAnime_Update(&this->skelAnime);
    } else {
        PlayerAnimation_Update(play, &this->skelAnime);
    }
}

void EnYb_FinishTeachingCutscene(EnYb* this) {
    if (this->csIdIndex != -1) {
        if (CutsceneManager_GetCurrentCsId() == this->csIdList[this->csIdIndex]) {
            CutsceneManager_Stop(this->csIdList[this->csIdIndex]);
        }
        this->csIdIndex = -1;
    }
}

void EnYb_ChangeCutscene(EnYb* this, s16 csIdIndex) {
    EnYb_FinishTeachingCutscene(this);
    this->csIdIndex = csIdIndex;
}

/**
 * Sets a flag that enables the Kamaro dancing proximity music at night.
 */
void EnYb_EnableProximityMusic(EnYb* this) {
    Actor_PlaySeq_FlaggedKamaroDance(&this->actor);
}

void EnYb_Disappear(EnYb* this, PlayState* play) {
    s32 pad;
    Vec3f sp60;
    s32 i;

    EnYb_UpdateAnimation(this, play);
    for (i = 3; i >= 0; i--) {
        sp60.x = Rand_CenteredFloat(60.0f) + this->actor.world.pos.x;
        sp60.z = Rand_CenteredFloat(60.0f) + this->actor.world.pos.z;
        sp60.y = Rand_CenteredFloat(50.0f) + (this->actor.world.pos.y + 20.0f);
        func_800B3030(play, &sp60, &D_80BFB2E8, &D_80BFB2E8, 100, 0, 2);
    }

    SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 20, NA_SE_EN_EXTINCT);
    if (this->alpha > 10) {
        this->alpha -= 10;
    } else {
        Actor_Kill(&this->actor);
    }
}

void EnYb_SetupLeaving(EnYb* this, PlayState* play) {
    EnYb_UpdateAnimation(this, play);
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        this->actionFunc = EnYb_Talk;
        // I am counting on you
        Message_StartTextbox(play, 0x147D, &this->actor);
        func_80BFA2FC(play);
    } else {
        Actor_OfferTalkExchange(&this->actor, play, 1000.0f, 1000.0f, PLAYER_IA_MINUS1);
    }
    EnYb_EnableProximityMusic(this);
}

void EnYb_ReceiveMask(EnYb* this, PlayState* play) {
    EnYb_UpdateAnimation(this, play);
    // Player is parent: receiving the Kamaro mask
    if (Actor_HasParent(&this->actor, play)) {
        this->actor.parent = NULL;
        this->actionFunc = EnYb_SetupLeaving;
        this->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        Actor_OfferTalkExchange(&this->actor, play, 1000.0f, 1000.0f, PLAYER_IA_MINUS1);
    } else {
        Actor_OfferGetItem(&this->actor, play, GI_MASK_KAMARO, 10000.0f, 100.0f);
    }
    EnYb_EnableProximityMusic(this);
}

void EnYb_Talk(EnYb* this, PlayState* play) {
    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 2, 0x1000, 0x200);
    this->actor.world.rot.y = this->actor.shape.rot.y;
    EnYb_UpdateAnimation(this, play);

    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        switch (play->msgCtx.currentTextId) {
            case 0x147D: // I am counting on you
                Message_CloseTextbox(play);
                this->actionFunc = EnYb_Disappear;
                SET_WEEKEVENTREG(WEEKEVENTREG_82_04);
                break;

            case 0x147C: // Spread my dance across the world
                if (Player_GetMask(play) == PLAYER_MASK_KAMARO) {
                    Message_CloseTextbox(play);
                    this->actionFunc = EnYb_Idle;

                } else if (GameInteractor_Should(VB_HAVE_KAMAROS_MASK,
                                                 INV_CONTENT(ITEM_MASK_KAMARO) == ITEM_MASK_KAMARO)) {
                    Message_ContinueTextbox(play, 0x147D); // I am counting on you
                    func_80BFA2FC(play);

                } else {
                    Message_CloseTextbox(play);
                    this->actionFunc = EnYb_ReceiveMask;
                    EnYb_ReceiveMask(this, play);
                }
                break;

            default:
                Message_CloseTextbox(play);
                this->actionFunc = EnYb_Idle;
                break;
        }
    }
    EnYb_EnableProximityMusic(this);
}

void EnYb_TeachingDanceFinish(EnYb* this, PlayState* play) {
    EnYb_UpdateAnimation(this, play);
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->actionFunc = EnYb_Talk;
        // Spread my dance across the world
        Message_StartTextbox(play, 0x147C, &this->actor);
        this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
    } else {
        Actor_OfferTalkExchange(&this->actor, play, 1000.0f, 1000.0f, PLAYER_IA_MINUS1);
    }
    EnYb_EnableProximityMusic(this);
}

// dancing countdown
void EnYb_TeachingDance(EnYb* this, PlayState* play) {
    EnYb_UpdateAnimation(this, play);

    if (this->teachingCutsceneTimer > 0) {
        this->teachingCutsceneTimer--;
    } else {
        EnYb_FinishTeachingCutscene(this);
        this->actionFunc = EnYb_TeachingDanceFinish;
        this->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        Actor_OfferTalkExchange(&this->actor, play, 1000.0f, 1000.0f, PLAYER_IA_MINUS1);
    }
    EnYb_EnableProximityMusic(this);
}

void EnYb_Idle(EnYb* this, PlayState* play) {
    s32 pad;
    Player* player = GET_PLAYER(play);

    EnYb_UpdateAnimation(this, play);
    if ((this->actor.xzDistToPlayer < 180.0f) && (fabsf(this->actor.playerHeightRel) < 50.0f) &&
        (play->msgCtx.ocarinaMode == OCARINA_MODE_EVENT) && (play->msgCtx.lastPlayedSong == OCARINA_SONG_HEALING) &&
        (GET_PLAYER_FORM == PLAYER_FORM_HUMAN)) {
        this->actionFunc = EnYb_TeachingDance;
        this->teachingCutsceneTimer = 200;
        EnYb_ChangeCutscene(this, 0);
    } else if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        func_80BFA2FC(play);
        this->actionFunc = EnYb_Talk;
        if (Player_GetMask(play) == PLAYER_MASK_KAMARO) {
            // I have taught you, go use it
            Message_StartTextbox(play, 0x147C, &this->actor);
        } else {
            // regular talk to him first dialogue
            Message_StartTextbox(play, 0x147B, &this->actor);
        }
    } else if (EnYb_CanTalk(this, play)) {
        Actor_OfferTalk(&this->actor, play, 120.0f);
    }

    if (this->playerOcarinaOut & 1) {
        if (!(player->stateFlags2 & PLAYER_STATE2_USING_OCARINA)) {
            this->playerOcarinaOut &= ~1;
        }
    } else if ((player->stateFlags2 & PLAYER_STATE2_USING_OCARINA) && (this->actor.xzDistToPlayer < 180.0f) &&
               (fabsf(this->actor.playerHeightRel) < 50.0f)) {
        this->playerOcarinaOut |= 1;
        Actor_PlaySfx(&this->actor, NA_SE_SY_TRE_BOX_APPEAR);
    }

    EnYb_EnableProximityMusic(this);
}

void EnYb_WaitForMidnight(EnYb* this, PlayState* play) {
    if (CURRENT_TIME < CLOCK_TIME(6, 0)) {
        EnYb_UpdateAnimation(this, play);
        this->alpha += 5;
        if (this->alpha > 250) {
            this->alpha = 255;
            this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
            this->actionFunc = EnYb_Idle;
        }
        EnYb_EnableProximityMusic(this);
    }
}

void EnYb_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnYb* this = (EnYb*)thisx;

    if (CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_ATTENTION_ENABLED)) {
        Collider_UpdateCylinder(&this->actor, &this->collider);
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    }
    if (CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_ATTENTION_ENABLED)) {
        Actor_MoveWithGravity(&this->actor);
        Actor_UpdateBgCheckInfo(play, &this->actor, 40.0f, 25.0f, 40.0f, UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4);
    }

    this->actionFunc(this, play);

    if ((this->csIdIndex != -1) && (CutsceneManager_GetCurrentCsId() != this->csIdList[this->csIdIndex])) {
        if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
            CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
            CutsceneManager_Queue(this->csIdList[this->csIdIndex]);
        } else if (CutsceneManager_IsNext(this->csIdList[this->csIdIndex])) {
            if (this->csIdIndex == 0) {
                CutsceneManager_StartWithPlayerCs(this->csIdList[this->csIdIndex], &this->actor);
            }
        } else {
            CutsceneManager_Queue(this->csIdList[this->csIdIndex]);
        }
    }
}

void EnYb_PostLimbDrawOpa(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    EnYb* this = (EnYb*)thisx;

    if (limbIndex == YB_LIMB_HEAD) {
        Matrix_MultVec3f(&D_80BFB2F4, &this->actor.focus.pos);
    }
    if (limbIndex == YB_LIMB_LEGS_ROOT) {
        Matrix_MultVec3f(&gZeroVec3f, &this->shadowPos);
    }
}

void EnYb_PostLimbDrawXlu(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx, Gfx** gfx) {
    EnYb* this = (EnYb*)thisx;

    if (limbIndex == YB_LIMB_HEAD) {
        Matrix_MultVec3f(&D_80BFB300, &this->actor.focus.pos);
    }
    if (limbIndex == YB_LIMB_LEGS_ROOT) {
        Matrix_MultVec3f(&gZeroVec3f, &this->shadowPos);
    }
}

void EnYb_Draw(Actor* thisx, PlayState* play) {
    EnYb* this = (EnYb*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    if (this->alpha != 0) {
        if (this->alpha < 255) {
            if (this->alpha > 128) {
                Gfx_SetupDL71(POLY_XLU_DISP++);
                Scene_SetRenderModeXlu(play, 2, 2);
            } else {
                Gfx_SetupDL72(POLY_XLU_DISP++);
                Scene_SetRenderModeXlu(play, 1, 2);
            }
            gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, this->alpha);

            POLY_XLU_DISP =
                SkelAnime_DrawFlex(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                   this->skelAnime.dListCount, NULL, EnYb_PostLimbDrawXlu, &this->actor, POLY_XLU_DISP);

        } else {
            Gfx_SetupDL25_Opa(play->state.gfxCtx);
            Scene_SetRenderModeXlu(play, 0, 1);
            SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                  this->skelAnime.dListCount, NULL, EnYb_PostLimbDrawOpa, &this->actor);
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}
