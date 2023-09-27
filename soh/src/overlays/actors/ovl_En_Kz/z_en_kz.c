/*
 * File: z_en_kz.c
 * Overlay: ovl_En_Kz
 * Description: King Zora
 */

#include "z_en_kz.h"
#include "objects/object_kz/object_kz.h"
#include "soh/Enhancements/randomizer/adult_trade_shuffle.h"

#define FLAGS (ACTOR_FLAG_TARGETABLE | ACTOR_FLAG_FRIENDLY)

void EnKz_Init(Actor* thisx, PlayState* play);
void EnKz_Destroy(Actor* thisx, PlayState* play);
void EnKz_Update(Actor* thisx, PlayState* play);
void EnKz_Draw(Actor* thisx, PlayState* play);

void EnKz_PreMweepWait(EnKz* this, PlayState* play);
void EnKz_SetupMweep(EnKz* this, PlayState* play);
void EnKz_Mweep(EnKz* this, PlayState* play);
void EnKz_StopMweep(EnKz* this, PlayState* play);
void EnKz_Wait(EnKz* this, PlayState* play);
void EnKz_SetupGetItem(EnKz* this, PlayState* play);
void EnKz_StartTimer(EnKz* this, PlayState* play);

const ActorInit En_Kz_InitVars = {
    ACTOR_EN_KZ,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_KZ,
    sizeof(EnKz),
    (ActorFunc)EnKz_Init,
    (ActorFunc)EnKz_Destroy,
    (ActorFunc)EnKz_Update,
    (ActorFunc)EnKz_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_NONE,
        OCELEM_ON,
    },
    { 80, 120, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 sColChkInfoInit = { 0, 0, 0, 0, MASS_IMMOVABLE };

typedef enum {
    /* 0 */ ENKZ_ANIM_0,
    /* 1 */ ENKZ_ANIM_1,
    /* 2 */ ENKZ_ANIM_2
} EnKzAnimation;

static AnimationInfo sAnimationInfo[] = {
    { &gKzIdleAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },
    { &gKzIdleAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -10.0f },
    { &gKzMweepAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -10.0f },
};

u16 EnKz_GetTextNoMaskChild(PlayState* play, EnKz* this) {
    Player* player = GET_PLAYER(play);

    if ((IS_RANDO && Flags_GetRandomizerInf(RAND_INF_DUNGEONS_DONE_JABU_JABUS_BELLY)) ||
        (!IS_RANDO && CHECK_QUEST_ITEM(QUEST_ZORA_SAPPHIRE))) {
        // Allow turning in Ruto's letter even if you have already rescued her
        if (IS_RANDO && !Flags_GetEventChkInf(EVENTCHKINF_KING_ZORA_MOVED)) {
            player->exchangeItemId = EXCH_ITEM_LETTER_RUTO;
        }
        return 0x402B;
    } else if (Flags_GetEventChkInf(EVENTCHKINF_KING_ZORA_MOVED)) {
        return 0x401C;
    } else {
        player->exchangeItemId = EXCH_ITEM_LETTER_RUTO;
        return 0x401A;
    }
}

u16 EnKz_GetTextNoMaskAdult(PlayState* play, EnKz* this) {
    Player* player = GET_PLAYER(play);

    // this works because both ITEM_NONE and later trade items are > ITEM_FROG
    if (INV_CONTENT(ITEM_TRADE_ADULT) >= ITEM_FROG) {
        if (!Flags_GetInfTable(INFTABLE_139)) {
            if (!IS_RANDO) {
                return CHECK_OWNED_EQUIP(EQUIP_TUNIC, 2) ? 0x401F : 0x4012;
            } else {
                return 0x4012;
            }
        } else {
            return CHECK_QUEST_ITEM(QUEST_SONG_SERENADE) ? 0x4045 : 0x401A;
        }
    } else {
        player->exchangeItemId = EXCH_ITEM_PRESCRIPTION;
        return 0x4012;
    }
}

u16 EnKz_GetText(PlayState* play, Actor* thisx) {
    EnKz* this = (EnKz*)thisx;
    u16 reactionText = Text_GetFaceReaction(play, 0x1E);

    if (reactionText != 0) {
        return reactionText;
    }

    if (LINK_IS_ADULT) {
        return EnKz_GetTextNoMaskAdult(play, this);
    } else {
        return EnKz_GetTextNoMaskChild(play, this);
    }
}

s16 func_80A9C6C0(PlayState* play, Actor* thisx) {
    EnKz* this = (EnKz*)thisx;
    s16 ret = NPC_TALK_STATE_TALKING;

    switch (Message_GetState(&play->msgCtx)) {
        case TEXT_STATE_DONE:
            ret = NPC_TALK_STATE_IDLE;
            switch (this->actor.textId) {
                case 0x4012:
                    Flags_SetInfTable(INFTABLE_139);
                    ret = NPC_TALK_STATE_ACTION;
                    break;
                case 0x401B:
                    ret = !Message_ShouldAdvance(play) ? NPC_TALK_STATE_TALKING : NPC_TALK_STATE_ACTION;
                    break;
                case 0x401F:
                    Flags_SetInfTable(INFTABLE_139);
                    break;
            }
            break;
        case TEXT_STATE_DONE_FADING:
            if (this->actor.textId != 0x4014) {
                if (this->actor.textId == 0x401B && !this->sfxPlayed) {
                    Audio_PlaySoundGeneral(NA_SE_SY_CORRECT_CHIME, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                           &D_801333E8);
                    this->sfxPlayed = true;
                }
            } else if (!this->sfxPlayed) {
                Audio_PlaySoundGeneral(NA_SE_SY_TRE_BOX_APPEAR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                this->sfxPlayed = true;
            }
            break;
        case TEXT_STATE_CHOICE:
            if (!Message_ShouldAdvance(play)) {
                break;
            }
            if (this->actor.textId == 0x4014) {
                if (play->msgCtx.choiceIndex == 0) {
                    EnKz_SetupGetItem(this, play);
                    ret = NPC_TALK_STATE_ACTION;
                } else {
                    this->actor.textId = 0x4016;
                    Message_ContinueTextbox(play, this->actor.textId);
                }
            }
            break;
        case TEXT_STATE_EVENT:
            if (Message_ShouldAdvance(play)) {
                ret = NPC_TALK_STATE_ACTION;
            }
            break;
        case TEXT_STATE_NONE:
        case TEXT_STATE_DONE_HAS_NEXT:
        case TEXT_STATE_CLOSING:
        case TEXT_STATE_SONG_DEMO_DONE:
        case TEXT_STATE_8:
        case TEXT_STATE_9:
            break;
    }
    return ret;
}

void EnKz_UpdateEyes(EnKz* this) {
    if (DECR(this->blinkTimer) == 0) {
        this->eyeIdx += 1;
        if (this->eyeIdx >= 3) {
            this->blinkTimer = Rand_S16Offset(30, 30);
            this->eyeIdx = 0;
        }
    }
}

s32 func_80A9C95C(PlayState* play, EnKz* this, s16* talkState, f32 unkf, NpcGetTextIdFunc getTextId,
                  NpcUpdateTalkStateFunc updateTalkState) {
    Player* player = GET_PLAYER(play);
    s16 sp32;
    s16 sp30;
    f32 xzDistToPlayer;
    f32 yaw;

    if (Actor_ProcessTalkRequest(&this->actor, play)) {
        *talkState = NPC_TALK_STATE_TALKING;
        return 1;
    }

    if (*talkState != NPC_TALK_STATE_IDLE) {
        *talkState = updateTalkState(play, &this->actor);
        return 0;
    }

    yaw = Math_Vec3f_Yaw(&this->actor.home.pos, &player->actor.world.pos);
    yaw -= this->actor.shape.rot.y;
    if ((fabsf(yaw) > 1638.0f) || (this->actor.xzDistToPlayer < 265.0f)) {
        this->actor.flags &= ~ACTOR_FLAG_TARGETABLE;
        return 0;
    }

    this->actor.flags |= ACTOR_FLAG_TARGETABLE;

    Actor_GetScreenPos(play, &this->actor, &sp32, &sp30);
    if (!((sp32 >= -30) && (sp32 < 361) && (sp30 >= -10) && (sp30 < 241))) {
        return 0;
    }

    xzDistToPlayer = this->actor.xzDistToPlayer;
    this->actor.xzDistToPlayer = Math_Vec3f_DistXZ(&this->actor.home.pos, &player->actor.world.pos);
    if (func_8002F2CC(&this->actor, play, unkf) == 0) {
        this->actor.xzDistToPlayer = xzDistToPlayer;
        return 0;
    }
    this->actor.xzDistToPlayer = xzDistToPlayer;
    this->actor.textId = getTextId(play, &this->actor);

    return 0;
}

void func_80A9CB18(EnKz* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (func_80A9C95C(play, this, &this->interactInfo.talkState, 340.0f, EnKz_GetText, func_80A9C6C0)) {
        if (((IS_RANDO && LINK_IS_CHILD) || this->actor.textId == 0x401A) && !Flags_GetEventChkInf(EVENTCHKINF_KING_ZORA_MOVED)) {
            if (func_8002F368(play) == EXCH_ITEM_LETTER_RUTO) {
                this->actor.textId = 0x401B;
                this->sfxPlayed = false;
            } else {
                this->actor.textId = 0x401A;
            }
            player->actor.textId = this->actor.textId;
            return;
        }

        if (LINK_IS_ADULT) {
            if ((INV_CONTENT(ITEM_TRADE_ADULT) == ITEM_PRESCRIPTION) &&
                (func_8002F368(play) == EXCH_ITEM_PRESCRIPTION)) {
                if (!IS_RANDO || !Flags_GetTreasure(play, 0x1F)) {
                    this->actor.textId = 0x4014;
                    this->sfxPlayed = false;
                    player->actor.textId = this->actor.textId;
                    this->isTrading = true;
                    return;
                }
            }

            this->isTrading = false;
            if (Flags_GetInfTable(INFTABLE_139)) {
                this->actor.textId = CHECK_QUEST_ITEM(QUEST_SONG_SERENADE) ? 0x4045 : 0x401A;
                player->actor.textId = this->actor.textId;
            } else {
                if (!IS_RANDO) {
                    this->actor.textId = CHECK_OWNED_EQUIP(EQUIP_TUNIC, 2) ? 0x401F : 0x4012;
                } else {
                    this->actor.textId = 0x4012;
                }

                player->actor.textId = this->actor.textId;
            }
        }
    }
}

s32 EnKz_FollowPath(EnKz* this, PlayState* play) {
    Path* path;
    Vec3s* pointPos;
    f32 pathDiffX;
    f32 pathDiffZ;

    if ((this->actor.params & 0xFF00) == 0xFF00) {
        return 0;
    }

    path = &play->setupPathList[(this->actor.params & 0xFF00) >> 8];
    pointPos = SEGMENTED_TO_VIRTUAL(path->points);
    pointPos += this->waypoint;

    pathDiffX = pointPos->x - this->actor.world.pos.x;
    pathDiffZ = pointPos->z - this->actor.world.pos.z;
    Math_SmoothStepToS(&this->actor.world.rot.y, (Math_FAtan2F(pathDiffX, pathDiffZ) * (0x8000 / M_PI)), 0xA, 0x3E8, 1);

    if ((SQ(pathDiffX) + SQ(pathDiffZ)) < 10.0f * CVarGetInteger("gMweepSpeed", 1)) {
        this->waypoint++;
        if (this->waypoint >= path->count) {
            this->waypoint = 0;
        }
        return 1;
    }
    return 0;
}

s32 EnKz_SetMovedPos(EnKz* this, PlayState* play) {
    Path* path;
    Vec3s* lastPointPos;

    if ((this->actor.params & 0xFF00) == 0xFF00) {
        return 0;
    }

    path = &play->setupPathList[(this->actor.params & 0xFF00) >> 8];
    lastPointPos = SEGMENTED_TO_VIRTUAL(path->points);
    lastPointPos += path->count - 1;

    this->actor.world.pos.x = lastPointPos->x;
    this->actor.world.pos.y = lastPointPos->y;
    this->actor.world.pos.z = lastPointPos->z;

    return 1;
}

void EnKz_Init(Actor* thisx, PlayState* play) {
    EnKz* this = (EnKz*)thisx;
    s32 pad;

    SkelAnime_InitFlex(play, &this->skelanime, &gKzSkel, NULL, this->jointTable, this->morphTable, 12);
    ActorShape_Init(&this->actor.shape, 0.0, NULL, 0.0);
    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    CollisionCheck_SetInfo2(&this->actor.colChkInfo, NULL, &sColChkInfoInit);
    Actor_SetScale(&this->actor, 0.01);
    this->actor.targetMode = 3;
    this->interactInfo.talkState = NPC_TALK_STATE_IDLE;
    Animation_ChangeByInfo(&this->skelanime, sAnimationInfo, ENKZ_ANIM_0);

    if (!IS_RANDO) {
        if (Flags_GetEventChkInf(EVENTCHKINF_KING_ZORA_MOVED)) {
            EnKz_SetMovedPos(this, play);
        }
    } else {
        int zorasFountain = Randomizer_GetSettingValue(RSK_ZORAS_FOUNTAIN);
        switch (zorasFountain) {
            case 0:
                if (Flags_GetEventChkInf(EVENTCHKINF_KING_ZORA_MOVED)) {
                    EnKz_SetMovedPos(this, play);
                }
                break;
            case 1:
                if (LINK_IS_ADULT) {
                    EnKz_SetMovedPos(this, play);
                } else if (Flags_GetEventChkInf(EVENTCHKINF_KING_ZORA_MOVED)) {
                    EnKz_SetMovedPos(this, play);
                }
                break;
            case 2:
                EnKz_SetMovedPos(this, play);
                break;
        }
    }

    if (LINK_IS_ADULT) {
        if (!Flags_GetInfTable(INFTABLE_138)) {
            Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_BG_ICE_SHELTER,
                               this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0,
                               0x04FF);
        }
        this->actionFunc = EnKz_Wait;
    } else {
        this->actionFunc = EnKz_PreMweepWait;
    }
}

void EnKz_Destroy(Actor* thisx, PlayState* play) {
    EnKz* this = (EnKz*)thisx;

    Collider_DestroyCylinder(play, &this->collider);

    ResourceMgr_UnregisterSkeleton(&this->skelanime);
}

void EnKz_PreMweepWait(EnKz* this, PlayState* play) {
    if (this->interactInfo.talkState == NPC_TALK_STATE_ACTION) {
        Animation_ChangeByInfo(&this->skelanime, sAnimationInfo, ENKZ_ANIM_2);
        this->interactInfo.talkState = NPC_TALK_STATE_IDLE;
        this->actionFunc = EnKz_SetupMweep;
    } else {
        func_80034F54(play, this->unk_2A6, this->unk_2BE, 12);
    }
}

void EnKz_SetupMweep(EnKz* this, PlayState* play) {
    Vec3f unused = { 0.0f, 0.0f, 0.0f };
    Vec3f pos;
    Vec3f initPos;

    this->cutsceneCamera = Play_CreateSubCamera(play);
    this->gameplayCamera = play->activeCamera;
    Play_ChangeCameraStatus(play, this->gameplayCamera, CAM_STAT_WAIT);
    Play_ChangeCameraStatus(play, this->cutsceneCamera, CAM_STAT_ACTIVE);
    pos = this->actor.world.pos;
    initPos = this->actor.home.pos;
    pos.y += 60.0f;
    initPos.y += -100.0f;
    initPos.z += 260.0f;
    Play_CameraSetAtEye(play, this->cutsceneCamera, &pos, &initPos);
    func_8002DF54(play, &this->actor, 8);
    this->actor.speedXZ = 0.1f * CVarGetInteger("gMweepSpeed", 1);
    this->actionFunc = EnKz_Mweep;
}

void EnKz_Mweep(EnKz* this, PlayState* play) {
    Vec3f unused = { 0.0f, 0.0f, 0.0f };
    Vec3f pos;
    Vec3f initPos;

    pos = this->actor.world.pos;
    initPos = this->actor.home.pos;
    pos.y += 60.0f;
    initPos.y += -100.0f;
    initPos.z += 260.0f;
    Play_CameraSetAtEye(play, this->cutsceneCamera, &pos, &initPos);
    if ((EnKz_FollowPath(this, play) == 1) && (this->waypoint == 0)) {
        Animation_ChangeByInfo(&this->skelanime, sAnimationInfo, ENKZ_ANIM_1);
        Inventory_ReplaceItem(play, ITEM_LETTER_RUTO, ITEM_BOTTLE);
        EnKz_SetMovedPos(this, play);
        Flags_SetEventChkInf(EVENTCHKINF_KING_ZORA_MOVED);
        this->actor.speedXZ = 0.0;
        this->actionFunc = EnKz_StopMweep;
    }
    if (this->skelanime.curFrame == 13.0f) {
        Audio_PlayActorSound2(&this->actor, NA_SE_VO_KZ_MOVE);
    }
}

void EnKz_StopMweep(EnKz* this, PlayState* play) {
    Play_ChangeCameraStatus(play, this->gameplayCamera, CAM_STAT_ACTIVE);
    Play_ClearCamera(play, this->cutsceneCamera);
    func_8002DF54(play, &this->actor, 7);
    this->actionFunc = EnKz_Wait;
}

void EnKz_Wait(EnKz* this, PlayState* play) {
    if (this->interactInfo.talkState == NPC_TALK_STATE_ACTION) {
        this->actionFunc = EnKz_SetupGetItem;
        EnKz_SetupGetItem(this, play);
    } else {
        func_80034F54(play, this->unk_2A6, this->unk_2BE, 12);
    }
}

void EnKz_SetupGetItem(EnKz* this, PlayState* play) {
    GetItemEntry getItemEntry = (GetItemEntry)GET_ITEM_NONE;
    s32 getItemId;
    f32 xzRange;
    f32 yRange;

    if (Actor_HasParent(&this->actor, play)) {
        this->actor.parent = NULL;
        this->interactInfo.talkState = NPC_TALK_STATE_TALKING;
        this->actionFunc = EnKz_StartTimer;
    } else {
        if (IS_RANDO) {
            if (this->isTrading) {
                getItemEntry = Randomizer_GetItemFromKnownCheck(RC_ZD_TRADE_PRESCRIPTION, GI_FROG);
                getItemId = getItemEntry.getItemId;
                Randomizer_ConsumeAdultTradeItem(play, ITEM_PRESCRIPTION);
                Flags_SetTreasure(play, 0x1F);
            } else {
                getItemEntry = Randomizer_GetItemFromKnownCheck(RC_ZD_KING_ZORA_THAWED, GI_TUNIC_ZORA);
                getItemId = getItemEntry.getItemId;
            }
        } else {
            getItemId = this->isTrading ? GI_FROG : GI_TUNIC_ZORA;
        }
        yRange = fabsf(this->actor.yDistToPlayer) + 1.0f;
        xzRange = this->actor.xzDistToPlayer + 1.0f;
        if (!IS_RANDO || getItemEntry.getItemId == GI_NONE) {
            func_8002F434(&this->actor, play, getItemId, xzRange, yRange);
        } else {
            GiveItemEntryFromActor(&this->actor, play, getItemEntry, xzRange, yRange);
        }
    }
}

void EnKz_StartTimer(EnKz* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_DONE) && Message_ShouldAdvance(play)) {
        if (INV_CONTENT(ITEM_TRADE_ADULT) == ITEM_FROG && !IS_RANDO) {
            func_80088AA0(180); // start timer2 with 3 minutes
            gSaveContext.eventInf[1] &= ~1;
        }
        this->interactInfo.talkState = NPC_TALK_STATE_IDLE;
        this->actionFunc = EnKz_Wait;
    }
}

void EnKz_Update(Actor* thisx, PlayState* play) {
    EnKz* this = (EnKz*)thisx;
    s32 pad;

    if (LINK_IS_ADULT && !Flags_GetInfTable(INFTABLE_138)) {
        Flags_SetInfTable(INFTABLE_138);
    }
    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    SkelAnime_Update(&this->skelanime);
    EnKz_UpdateEyes(this);
    Actor_MoveForward(&this->actor);
    if (this->actionFunc != EnKz_StartTimer) {
        func_80A9CB18(this, play);
    }
    this->actionFunc(this, play);
}

s32 EnKz_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnKz* this = (EnKz*)thisx;

    if (limbIndex == 8 || limbIndex == 9 || limbIndex == 10) {
        rot->y += Math_SinS(this->unk_2A6[limbIndex]) * 200.0f;
        rot->z += Math_CosS(this->unk_2BE[limbIndex]) * 200.0f;
    }
    if (limbIndex) {}
    return false;
}

void EnKz_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    EnKz* this = (EnKz*)thisx;
    Vec3f mult = { 2600.0f, 0.0f, 0.0f };

    if (limbIndex == 11) {
        Matrix_MultVec3f(&mult, &this->actor.focus.pos);
    }
}

void EnKz_Draw(Actor* thisx, PlayState* play) {
    static void* sEyeSegments[] = {
        gKzEyeOpenTex,
        gKzEyeHalfTex,
        gKzEyeClosedTex,
    };
    EnKz* this = (EnKz*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(sEyeSegments[this->eyeIdx]));
    Gfx_SetupDL_37Opa(play->state.gfxCtx);
    SkelAnime_DrawSkeletonOpa(play, &this->skelanime, EnKz_OverrideLimbDraw, EnKz_PostLimbDraw, this);

    CLOSE_DISPS(play->state.gfxCtx);
}
