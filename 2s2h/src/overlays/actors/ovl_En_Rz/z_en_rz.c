/*
 * File: z_en_rz.c
 * Overlay: ovl_En_Rz
 * Description: Rosa Sisters, Judo (red) and Marilla (blue)
 */

#include "z_en_rz.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY)

void EnRz_Init(Actor* thisx, PlayState* play);
void EnRz_Destroy(Actor* thisx, PlayState* play);
void EnRz_Update(Actor* thisx, PlayState* play);
void EnRz_Draw(Actor* thisx, PlayState* play);

void EnRz_ActorShadowFunc(Actor* thisx, Lights* mapper, PlayState* play);
void EnRz_ChangeAnim(PlayState* play, EnRz* this, s16 animIndex, u8 animMode, f32 morphFrames);
s32 EnRz_SetupPath(EnRz* this, PlayState* play);
EnRz* EnRz_FindSister(EnRz* this, PlayState* play);
void func_80BFC058(EnRz* this, PlayState* play);
void func_80BFC078(EnRz* this, PlayState* play);
void func_80BFC3F8(EnRz* this, PlayState* play);
void func_80BFC674(EnRz* this, PlayState* play);
void func_80BFC7E0(EnRz* this, PlayState* play);
void EnRz_Walk(EnRz* this, PlayState* play);

#define EN_RZ_STATE_1 (1 << 0)
#define EN_RZ_STATE_2 (1 << 1)

typedef enum {
    /* 0 */ EN_RZ_ANIM_THINKING,
    /* 1 */ EN_RZ_ANIM_STANDING,
    /* 2 */ EN_RZ_ANIM_WALKING,
    /* 3 */ EN_RZ_ANIM_SITTING,
    /* 4 */ EN_RZ_ANIM_APPLAUDING,
    /* 5 */ EN_RZ_ANIM_ON_KNEES,
    /* 6 */ EN_RZ_ANIM_DANCE,
    /* 7 */ EN_RZ_ANIM_LINK_NORMAL_WAIT_FREE,
    /* 8 */ EN_RZ_ANIM_LINK_DANCE,
    /* 9 */ EN_RZ_ANIM_MAX
} EnRzAnimation;

typedef enum {
    /* 0 */ EN_RZ_PATHSTATUS_NORMAL,   //!< not near waypoint
    /* 1 */ EN_RZ_PATHSTATUS_AT_POINT, //!< no path or new waypoint
    /* 2 */ EN_RZ_PATHSTATUS_END       //!< reached end of path
} EnRzPathStatus;

ActorProfile En_Rz_Profile = {
    /**/ ACTOR_EN_RZ,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_RZ,
    /**/ sizeof(EnRz),
    /**/ EnRz_Init,
    /**/ EnRz_Destroy,
    /**/ EnRz_Update,
    /**/ EnRz_Draw,
};

static TexturePtr sEyeTextures[] = {
    object_rz_Tex_00BC50, object_rz_Tex_00C190, object_rz_Tex_00C590, object_rz_Tex_00C990, object_rz_Tex_00CD90,
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

void EnRz_Init(Actor* thisx, PlayState* play) {
    EnRz* this = (EnRz*)thisx;
    s16 csId = this->actor.csId;
    s32 i;

    for (i = 0; i < ARRAY_COUNT(this->csIdList); i++) {
        this->csIdList[i] = csId;
        if (csId != CS_ID_NONE) {
            this->actor.csId = csId;
            csId = CutsceneManager_GetAdditionalCsId(this->actor.csId);
        }
    }

    Actor_SetScale(&this->actor, 0.01f);
    ActorShape_Init(&this->actor.shape, 0.0f, EnRz_ActorShadowFunc, 20.0f);

    //! @bug this alignment is because of player animations, but should be using ALIGN16
    SkelAnime_InitFlex(play, &this->skelAnime, &object_rz_Skel_00D8D8, &gRosaSistersStandingAnim,
                       (void*)((uintptr_t)this->jointTable & ~0xF), (void*)((uintptr_t)this->morphTable & ~0xF),
                       OBJECT_RZ_LIMB_MAX);
    Animation_PlayLoop(&this->skelAnime, &gRosaSistersStandingAnim);

    Collider_InitAndSetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;

    this->actionFunc = func_80BFC058;
    EnRz_SetupPath(this, play);
    this->animIndex = EN_RZ_ANIM_MAX;
    this->actor.attentionRangeType = ATTENTION_RANGE_0;
    this->actor.terminalVelocity = -9.0f;
    this->actor.gravity = -1.0f;

    switch (EN_RZ_GET_TYPE(thisx)) {
        case EN_RZ_TYPE_1:
            if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_77_04)) {
                Actor_Kill(&this->actor);
                return;
            }
            EnRz_ChangeAnim(play, this, EN_RZ_ANIM_SITTING, ANIMMODE_LOOP, 0.0f);
            this->actionFunc = func_80BFC674;
            this->actor.shape.yOffset = -1500.0f;
            break;

        case EN_RZ_TYPE_2:
            this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_77_04)) {
                EnRz_ChangeAnim(play, this, EN_RZ_ANIM_LINK_DANCE, ANIMMODE_LOOP, 0.0f);
            } else {
                EnRz_ChangeAnim(play, this, EN_RZ_ANIM_DANCE, ANIMMODE_LOOP, 0.0f);
            }
            this->actionFunc = func_80BFC3F8;
            this->sister = EnRz_FindSister(this, play);
            this->actor.cullingVolumeDistance = 300.0f;
            break;

        default: // EN_RZ_TYPE_0
            this->stateFlags |= EN_RZ_STATE_2;
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_77_04)) {
                Actor_Kill(&this->actor);
                return;
            }
            EnRz_ChangeAnim(play, this, EN_RZ_ANIM_WALKING, ANIMMODE_LOOP, 0.0f);
            this->actionFunc = EnRz_Walk;
            if (EN_RZ_GET_SISTER(&this->actor) == EN_RZ_JUDO) {
                this->actor.textId = 0x291C;
            } else { // EN_RZ_MARILLA
                this->actor.textId = 0x2920;
            }
            break;
    }

    if (EN_RZ_GET_SISTER(&this->actor) == EN_RZ_JUDO) {
        this->cueType = CS_CMD_ACTOR_CUE_550;
    } else { // EN_RZ_MARILLA
        this->cueType = CS_CMD_ACTOR_CUE_551;
    }

    this->cueId = 0;
}

/**
 * Custom shadow draw function of type ActorShadowFunc.
 */
void EnRz_ActorShadowFunc(Actor* thisx, Lights* mapper, PlayState* play) {
    Vec3f oldPos;
    EnRz* this = (EnRz*)thisx;

    if (this->animIndex == EN_RZ_ANIM_LINK_DANCE) {
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

void EnRz_ChangeAnim(PlayState* play, EnRz* this, s16 animIndex, u8 animMode, f32 morphFrames) {
    // N.B. both of these arrays must be the same length due to how the animations are chosen
    static AnimationHeader* sJudoAnimations[] = {
        &gRosaSistersThinkingAnim,             // EN_RZ_ANIM_THINKING
        &gRosaSistersStandingAnim,             // EN_RZ_ANIM_STANDING
        &gRosaSistersWalkingWhileThinkingAnim, // EN_RZ_ANIM_WALKING
        &gRosaSistersSittingAnim,              // EN_RZ_ANIM_SITTING
        &gRosaSistersApplaudingAnim,           // EN_RZ_ANIM_APPLAUDING
        &gRosaSistersOnKneesAnim,              // EN_RZ_ANIM_ON_KNEES
        &gRosaSistersDancingAnim,              // EN_RZ_ANIM_DANCE
    };
    static AnimationHeader* sMarillaAnimations[] = {
        &gRosaSistersStandingAnim,   // EN_RZ_ANIM_THINKING
        &gRosaSistersStandingAnim,   // EN_RZ_ANIM_STANDING
        &gRosaSistersWalkingAnim,    // EN_RZ_ANIM_WALKING
        &gRosaSistersSittingAnim,    // EN_RZ_ANIM_SITTING
        &gRosaSistersApplaudingAnim, // EN_RZ_ANIM_APPLAUDING
        &gRosaSistersOnKneesAnim,    // EN_RZ_ANIM_ON_KNEES
        &gRosaSistersDancingAnim,    // EN_RZ_ANIM_DANCE
    };
    static PlayerAnimationHeader* sPlayerAnimations[] = {
        &gPlayerAnim_link_normal_wait_free, // EN_RZ_ANIM_LINK_NORMAL_WAIT_FREE
        &gPlayerAnim_alink_dance_loop,      // EN_RZ_ANIM_LINK_DANCE
    };
    f32 endFrame;
    AnimationHeader** animationPtr;

    if (EN_RZ_GET_SISTER(&this->actor) == EN_RZ_JUDO) {
        animationPtr = sJudoAnimations;
    } else { // EN_RZ_MARILLA
        animationPtr = sMarillaAnimations;
    }

    if ((animIndex >= EN_RZ_ANIM_THINKING) && (animIndex < EN_RZ_ANIM_MAX) &&
        ((animIndex != this->animIndex) || (animMode != ANIMMODE_LOOP))) {
        if (animIndex >= ARRAY_COUNT(sJudoAnimations)) {
            endFrame = Animation_GetLastFrame(sPlayerAnimations[animIndex - ARRAY_COUNT(sJudoAnimations)]);
            if (animMode == ANIMMODE_LOOP) {
                PlayerAnimation_Change(play, &this->skelAnime,
                                       sPlayerAnimations[animIndex - ARRAY_COUNT(sJudoAnimations)],
                                       PLAYER_ANIM_ADJUSTED_SPEED, 0.0f, endFrame, ANIMMODE_LOOP, morphFrames);
            } else {
                PlayerAnimation_Change(play, &this->skelAnime,
                                       sPlayerAnimations[animIndex - ARRAY_COUNT(sJudoAnimations)],
                                       PLAYER_ANIM_ADJUSTED_SPEED, 0.0f, endFrame, ANIMMODE_LOOP, morphFrames);
            }
        } else {
            Animation_Change(&this->skelAnime, animationPtr[animIndex], 1.0f, 0.0f,
                             Animation_GetLastFrame(animationPtr[animIndex]), animMode, morphFrames);
        }

        this->animIndex = animIndex;
    }
}

void EnRz_ChangeAnimLoopMorph(PlayState* play, EnRz* this, s16 animIndex) {
    EnRz_ChangeAnim(play, this, animIndex, ANIMMODE_LOOP, -5.0f);
}

void EnRz_ChangeAnimLoop(PlayState* play, EnRz* this, s16 animIndex) {
    EnRz_ChangeAnim(play, this, animIndex, ANIMMODE_LOOP, 0.0f);
}

s32 EnRz_SetupPath(EnRz* this, PlayState* play) {
    if (EN_RZ_GET_PATH_INDEX(&this->actor) != EN_RZ_PATH_INDEX_NONE) {
        this->path = &play->setupPathList[EN_RZ_GET_PATH_INDEX(&this->actor)];
        if (this->path != NULL) {
            Path* path = this->path;
            Vec3s* points = (Vec3s*)Lib_SegmentedToVirtual(path->points);
            f32 diffX = points->x - this->actor.world.pos.x;
            f32 diffY = points->z - this->actor.world.pos.z;

            if ((SQ(diffX) + SQ(diffY)) < SQ(10.0f)) {
                this->curPointIndex = 0;
                this->stateFlags &= ~EN_RZ_STATE_1;
            } else {
                this->curPointIndex = path->count - 1;
                this->stateFlags |= EN_RZ_STATE_1;
                return true;
            }
        }
    } else {
        this->path = NULL;
        this->curPointIndex = 0;
    }
    return false;
}

EnRzPathStatus EnRz_GetPathStatus(EnRz* this) {
    Path* path = this->path;
    Vec3s* curPoint;
    f32 diffX;
    f32 diffZ;

    if (path == NULL) {
        return EN_RZ_PATHSTATUS_AT_POINT;
    }

    curPoint = &((Vec3s*)Lib_SegmentedToVirtual(path->points))[this->curPointIndex];
    diffX = curPoint->x - this->actor.world.pos.x;
    diffZ = curPoint->z - this->actor.world.pos.z;
    this->actor.world.rot.y = Math_Atan2S(diffX, diffZ);
    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.world.rot.y, 2, 0x7D0, 0xC8);

    if ((SQ(diffX) + SQ(diffZ)) < SQ(10.0f)) {
        if (this->stateFlags & EN_RZ_STATE_1) {
            this->curPointIndex--;
            if (this->curPointIndex < 0) {
                return EN_RZ_PATHSTATUS_END;
            }
        } else {
            this->curPointIndex++;
            if (this->curPointIndex >= path->count) {
                return EN_RZ_PATHSTATUS_END;
            }
        }
        return EN_RZ_PATHSTATUS_AT_POINT;
    }
    return EN_RZ_PATHSTATUS_NORMAL;
}

s32 EnRz_CanTalk(EnRz* this, PlayState* play) {
    if ((this->actor.xzDistToPlayer < 80.0f) && (fabsf(this->actor.playerHeightRel) < 20.0f) &&
        Player_IsFacingActor(&this->actor, 0x3000, play)) {
        return true;
    }
    return false;
}

s32 func_80BFBCEC(EnRz* this, PlayState* play) {
    s32 pad;
    Player* player = GET_PLAYER(play);

    if ((this->actor.xzDistToPlayer < 100.0f) && Actor_IsFacingPlayer(&this->actor, 0x3000) &&
        (player->stateFlags2 & PLAYER_STATE2_2000000)) {
        return true;
    }
    return false;
}

s32 EnRz_UpdateSkelAnime(EnRz* this, PlayState* play) {
    if (this->animIndex < EN_RZ_ANIM_LINK_NORMAL_WAIT_FREE) {
        return SkelAnime_Update(&this->skelAnime);
    } else {
        return PlayerAnimation_Update(play, &this->skelAnime);
    }
}

EnRz* EnRz_FindSister(EnRz* this, PlayState* play) {
    Actor* npc = play->actorCtx.actorLists[ACTORCAT_NPC].first;

    while (npc != NULL) {
        if ((npc->id == ACTOR_EN_RZ) && (EN_RZ_GET_TYPE(&this->actor) == EN_RZ_GET_TYPE(npc))) {
            if (&this->actor != npc) {
                return (EnRz*)npc;
            }
        }
        npc = npc->next;
    }
    return NULL;
}

void func_80BFBDFC(PlayState* play) {
    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_ROSA_SISTERS_HEART_PIECE)) {
        Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_RECEIVED_ROSA_SISTERS_HP);
    }
    Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_ROSA_SISTERS);
}

void EnRz_Destroy(Actor* thisx, PlayState* play) {
    EnRz* this = (EnRz*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

s32 func_80BFBE70(EnRz* this, PlayState* play) {
    u16 cueId;

    if ((EN_RZ_GET_SISTER(&this->actor) == EN_RZ_JUDO) && (this->animIndex == EN_RZ_ANIM_APPLAUDING)) {
        Actor_PlaySfx_Flagged(&this->actor, NA_SE_EV_CLAPPING_2P - SFX_FLAG);
    }

    if (Cutscene_IsCueInChannel(play, this->cueType)) {
        Cutscene_ActorTranslateAndYaw(&this->actor, play, Cutscene_GetCueChannel(play, this->cueType));
        cueId = play->csCtx.actorCues[Cutscene_GetCueChannel(play, this->cueType)]->id;
        if (this->cueId != cueId) {
            this->cueId = cueId;
            switch (this->cueId) {
                case 1:
                    EnRz_ChangeAnimLoop(play, this, EN_RZ_ANIM_STANDING);
                    break;

                case 2:
                    EnRz_ChangeAnimLoop(play, this, EN_RZ_ANIM_ON_KNEES);
                    break;

                case 3:
                    EnRz_ChangeAnimLoop(play, this, EN_RZ_ANIM_APPLAUDING);
                    break;

                case 4:
                    EnRz_ChangeAnimLoop(play, this, EN_RZ_ANIM_LINK_DANCE);
                    break;

                default:
                    break;
            }
        }
        return true;
    }

    return false;
}

s32 func_80BFBFAC(EnRz* this, PlayState* play) {
    if (this->actor.csId == CS_ID_NONE) {
        Message_StartTextbox(play, 0x2925, &this->actor);
        this->actionFunc = func_80BFC078;
    } else if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
        CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
        CutsceneManager_Queue(this->actor.csId);
        return false;
    } else if (CutsceneManager_IsNext(this->actor.csId)) {
        CutsceneManager_Start(this->actor.csId, &this->actor);
        return true;
    } else {
        CutsceneManager_Queue(this->actor.csId);
    }
    return false;
}

void func_80BFC058(EnRz* this, PlayState* play) {
    EnRz_UpdateSkelAnime(this, play);
}

void func_80BFC078(EnRz* this, PlayState* play) {
    s32 pad;
    Vec3f seqPos;

    EnRz_UpdateSkelAnime(this, play);

    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        switch (play->msgCtx.currentTextId) {
            case 0x2927:
            case 0x2928:
                Message_ContinueTextbox(play, play->msgCtx.currentTextId + 1);
                SET_WEEKEVENTREG(WEEKEVENTREG_77_04);
                break;

            default:
                Message_CloseTextbox(play);
                this->actionFunc = func_80BFC3F8;
                if (this->animIndex != EN_RZ_ANIM_LINK_DANCE) {
                    EnRz_ChangeAnimLoopMorph(play, this, EN_RZ_ANIM_DANCE);
                    if (this->sister != NULL) {
                        EnRz_ChangeAnimLoopMorph(play, this->sister, EN_RZ_ANIM_DANCE);
                    }
                }
                break;
        }
    }

    if (EN_RZ_GET_SISTER(&this->actor) == EN_RZ_JUDO) {
        seqPos.x = this->actor.projectedPos.x;
        seqPos.y = this->actor.projectedPos.y;
        seqPos.z = this->actor.projectedPos.z;
        Audio_PlaySequenceAtPos(SEQ_PLAYER_BGM_SUB, &seqPos, NA_BGM_ROSA_SISTERS, 900.0f);
    }
}

void func_80BFC19C(EnRz* this, PlayState* play) {
    EnRz_UpdateSkelAnime(this, play);
    if (!func_80BFBE70(this, play)) {
        this->actionFunc = func_80BFC3F8;
        EnRz_ChangeAnimLoop(play, this, EN_RZ_ANIM_LINK_DANCE);
        if (this->sister != NULL) {
            EnRz_ChangeAnimLoop(play, this->sister, EN_RZ_ANIM_LINK_DANCE);
        }
    }
}

void func_80BFC214(EnRz* this, PlayState* play) {
    EnRz_UpdateSkelAnime(this, play);
    if (!func_80BFBE70(this, play) && func_80BFBFAC(this, play)) {
        this->actionFunc = func_80BFC19C;
    }
}

void func_80BFC270(EnRz* this, PlayState* play) {
    EnRz_UpdateSkelAnime(this, play);
    if (Actor_HasParent(&this->actor, play)) {
        this->actor.parent = NULL;
        this->actionFunc = func_80BFC214;
        func_80BFBDFC(play);
    } else {
        Actor_OfferGetItem(&this->actor, play, GI_HEART_PIECE, 2000.0f, 1000.0f);
    }
}

void func_80BFC2F4(EnRz* this, PlayState* play) {
    EnRz_UpdateSkelAnime(this, play);
    if (!func_80BFBE70(this, play)) {
        Message_CloseTextbox(play);
        Actor_OfferGetItem(&this->actor, play, GI_HEART_PIECE, 2000.0f, 1000.0f);
        this->actionFunc = func_80BFC270;
    }
}

void func_80BFC36C(EnRz* this, PlayState* play) {
    EnRz_UpdateSkelAnime(this, play);
    if (func_80BFBFAC(this, play)) {
        SET_WEEKEVENTREG(WEEKEVENTREG_77_04);
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_ROSA_SISTERS_HEART_PIECE)) {
            this->actionFunc = func_80BFC214;
        } else {
            this->actionFunc = func_80BFC2F4;
            SET_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_ROSA_SISTERS_HEART_PIECE);
        }
        this->actor.csId = this->csIdList[1];
    }
}

void func_80BFC3F8(EnRz* this, PlayState* play) {
    s32 pad;
    Vec3f seqPos;

    EnRz_UpdateSkelAnime(this, play);

    if (this->sister == NULL) {
        this->sister = EnRz_FindSister(this, play);
    }

    if (!func_80BFBE70(this, play)) {
        if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
            this->actionFunc = func_80BFC078;

            if (CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED)) {
                this->actionFunc = func_80BFC36C;
                this->actor.csId = this->csIdList[0];
                this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
            } else if (Player_GetMask(play) == PLAYER_MASK_KAMARO) {
                if (CHECK_WEEKEVENTREG(WEEKEVENTREG_77_04)) {
                    Message_StartTextbox(play, 0x2925, &this->actor);
                    func_80BFBDFC(play);
                } else {
                    Message_StartTextbox(play, 0x2926, &this->actor);
                    func_80BFBDFC(play);
                }
            } else {
                Message_StartTextbox(play, 0x2926, &this->actor);
                func_80BFBDFC(play);
            }

        } else if (EnRz_CanTalk(this, play)) {
            if (func_80BFBCEC(this, play) && !CHECK_WEEKEVENTREG(WEEKEVENTREG_77_04) && this->sister != NULL) {
                this->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
                Actor_OfferTalkExchange(&this->actor, play, 1000.0f, 1000.0f, PLAYER_IA_MINUS1);
            } else {
                this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
                Actor_OfferTalk(&this->actor, play, 120.0f);
            }
        }

        if (EN_RZ_GET_SISTER(&this->actor) == EN_RZ_JUDO) {
            seqPos.x = this->actor.projectedPos.x;
            seqPos.y = this->actor.projectedPos.y;
            seqPos.z = this->actor.projectedPos.z;
            Audio_PlaySequenceAtPos(SEQ_PLAYER_BGM_SUB, &seqPos, NA_BGM_ROSA_SISTERS, 900.0f);
        }
    }
}

void func_80BFC608(EnRz* this, PlayState* play) {
    EnRz_UpdateSkelAnime(this, play);

    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        Message_CloseTextbox(play);
        this->actionFunc = func_80BFC674;
    }
}

void func_80BFC674(EnRz* this, PlayState* play) {
    EnRz_UpdateSkelAnime(this, play);

    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->actionFunc = func_80BFC608;
        if (Player_GetMask(play) == PLAYER_MASK_KAMARO) {
            Message_StartTextbox(play, 0x2925, &this->actor);
        } else {
            Message_StartTextbox(play, 0x2924, &this->actor);
        }
    } else if (EnRz_CanTalk(this, play)) {
        Actor_OfferTalk(&this->actor, play, 120.0f);
    }
}

void func_80BFC728(EnRz* this, PlayState* play) {
    EnRz_UpdateSkelAnime(this, play);

    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        Message_CloseTextbox(play);
        this->actionFunc = func_80BFC7E0;
        this->actor.textId++;
        if (EN_RZ_GET_SISTER(&this->actor) == EN_RZ_JUDO) {
            if (this->actor.textId >= 0x2920) {
                this->actor.textId = 0x291C;
            }
        } else if (this->actor.textId >= 0x2924) {
            this->actor.textId = 0x2920;
        }
    }
}

void func_80BFC7E0(EnRz* this, PlayState* play) {
    EnRz_UpdateSkelAnime(this, play);

    if (this->timer > 0) {
        this->timer--;
    } else {
        this->actionFunc = EnRz_Walk;
        EnRz_ChangeAnimLoopMorph(play, this, EN_RZ_ANIM_WALKING);
    }

    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->actionFunc = func_80BFC728;
        EnRz_ChangeAnimLoopMorph(play, this, EN_RZ_ANIM_THINKING);
        this->actor.speed = 0.0f;
        func_80BFBDFC(play);
    } else if (EnRz_CanTalk(this, play)) {
        Actor_OfferTalk(&this->actor, play, 120.0f);
    }
}

void EnRz_StopToThink(EnRz* this, PlayState* play) {
    this->timer = 100;
    this->actionFunc = func_80BFC7E0;
    this->actor.speed = 0.0f;
    EnRz_ChangeAnimLoopMorph(play, this, EN_RZ_ANIM_THINKING);
}

void EnRz_Walk(EnRz* this, PlayState* play) {
    EnRz_UpdateSkelAnime(this, play);
    this->actor.speed = 1.5f;

    switch (EnRz_GetPathStatus(this)) {
        case EN_RZ_PATHSTATUS_END:
            EnRz_SetupPath(this, play);
            EnRz_StopToThink(this, play);
            break;

        case EN_RZ_PATHSTATUS_AT_POINT:
            EnRz_StopToThink(this, play);
            break;

        default:
            break;
    }

    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->actionFunc = func_80BFC728;
        EnRz_ChangeAnimLoopMorph(play, this, EN_RZ_ANIM_THINKING);
        this->actor.speed = 0.0f;
        func_80BFBDFC(play);
    } else if (EnRz_CanTalk(this, play)) {
        Actor_OfferTalk(&this->actor, play, 120.0f);
    }
}

void EnRz_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnRz* this = (EnRz*)thisx;

    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    Actor_MoveWithGravity(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 40.0f, 25.0f, 40.0f, UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4);

    this->actionFunc(this, play);

    if (DECR(this->blinkTimer) == 0) {
        this->blinkTimer = Rand_S16Offset(60, 60);
    }

    this->eyeIndex = this->blinkTimer;
    if (this->eyeIndex > 2) {
        this->eyeIndex = 0;
    }
}

void EnRz_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    static Vec3f sFocusOffsetPos = { 500.0f, -500.0f, 0.0f };
    EnRz* this = (EnRz*)thisx;

    if (limbIndex == OBJECT_RZ_LIMB_0B) {
        Matrix_MultVec3f(&sFocusOffsetPos, &thisx->focus.pos);
    }
    if (limbIndex == OBJECT_RZ_LIMB_03) {
        Matrix_MultVec3f(&gZeroVec3f, &this->shadowPos);
    }
}

void EnRz_Draw(Actor* thisx, PlayState* play) {
    EnRz* this = (EnRz*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    if (EN_RZ_GET_SISTER(&this->actor) == EN_RZ_JUDO) {
        AnimatedMat_DrawStepOpa(play, Lib_SegmentedToVirtual(&object_rz_Matanimheader_00D768), 0);
    } else { // EN_RZ_MARILLA
        AnimatedMat_DrawStepOpa(play, Lib_SegmentedToVirtual(&object_rz_Matanimheader_00D768), 1);
    }

    if (this->animIndex == EN_RZ_ANIM_APPLAUDING) {
        gSPSegment(POLY_OPA_DISP++, 0x09, Lib_SegmentedToVirtual(sEyeTextures[4]));
    } else if (this->stateFlags & EN_RZ_STATE_2) {
        gSPSegment(POLY_OPA_DISP++, 0x09, Lib_SegmentedToVirtual(sEyeTextures[3]));
    } else {
        gSPSegment(POLY_OPA_DISP++, 0x09, Lib_SegmentedToVirtual(sEyeTextures[this->eyeIndex]));
    }

    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount, NULL,
                          EnRz_PostLimbDraw, &this->actor);

    CLOSE_DISPS(play->state.gfxCtx);
}
