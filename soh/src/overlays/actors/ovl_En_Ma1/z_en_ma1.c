/*
 * File: z_en_ma1.c
 * Overlay: En_Ma1
 * Description: Child Malon
 */

#include "z_en_ma1.h"
#include "objects/object_ma1/object_ma1.h"

#define FLAGS (ACTOR_FLAG_TARGETABLE | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_WHILE_CULLED | ACTOR_FLAG_DRAW_WHILE_CULLED | ACTOR_FLAG_NO_FREEZE_OCARINA)

void EnMa1_Init(Actor* thisx, PlayState* play);
void EnMa1_Destroy(Actor* thisx, PlayState* play);
void EnMa1_Update(Actor* thisx, PlayState* play);
void EnMa1_Draw(Actor* thisx, PlayState* play);

u16 EnMa1_GetText(PlayState* play, Actor* this);
s16 func_80AA0778(PlayState* play, Actor* this);

void func_80AA0D88(EnMa1* this, PlayState* play);
void func_80AA0EA0(EnMa1* this, PlayState* play);
void func_80AA0EFC(EnMa1* this, PlayState* play);
void func_80AA0F44(EnMa1* this, PlayState* play);
void func_80AA106C(EnMa1* this, PlayState* play);
void func_80AA10EC(EnMa1* this, PlayState* play);
void func_80AA1150(EnMa1* this, PlayState* play);
void EnMa1_DoNothing(EnMa1* this, PlayState* play);
void EnMa1_WaitForSongGive(EnMa1* this, PlayState* play);

const ActorInit En_Ma1_InitVars = {
    ACTOR_EN_MA1,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_MA1,
    sizeof(EnMa1),
    (ActorFunc)EnMa1_Init,
    (ActorFunc)EnMa1_Destroy,
    (ActorFunc)EnMa1_Update,
    (ActorFunc)EnMa1_Draw,
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
    { 18, 46, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 sColChkInfoInit = { 0, 0, 0, 0, MASS_IMMOVABLE };

typedef enum {
    /* 0 */ ENMA1_ANIM_0,
    /* 1 */ ENMA1_ANIM_1,
    /* 2 */ ENMA1_ANIM_2,
    /* 3 */ ENMA1_ANIM_3
} EnMa1Animation;

static AnimationFrameCountInfo sAnimationInfo[] = {
    { &gMalonChildIdleAnim, 1.0f, ANIMMODE_LOOP, 0.0f },
    { &gMalonChildIdleAnim, 1.0f, ANIMMODE_LOOP, -10.0f },
    { &gMalonChildSingAnim, 1.0f, ANIMMODE_LOOP, 0.0f },
    { &gMalonChildSingAnim, 1.0f, ANIMMODE_LOOP, -10.0f },
};

static Vec3f D_80AA16B8 = { 800.0f, 0.0f, 0.0f };

static void* sMouthTextures[] = {
    gMalonChildNeutralMouthTex,
    gMalonChildSmilingMouthTex,
    gMalonChildTalkingMouthTex,
};

static void* sEyeTextures[] = {
    gMalonChildEyeOpenTex,
    gMalonChildEyeHalfTex,
    gMalonChildEyeClosedTex,
};

bool Randomizer_ObtainedMalonHCReward() {
    return Flags_GetEventChkInf(EVENTCHKINF_OBTAINED_POCKET_EGG);
}

u16 EnMa1_GetText(PlayState* play, Actor* thisx) {
    // Special case for Malon Hyrule Castle Text. Placing it here at the beginning
    // has the added benefit of circumventing mask text if wearing bunny hood.
    if (gSaveContext.n64ddFlag && play->sceneNum == SCENE_HYRULE_CASTLE) {
        return Randomizer_ObtainedMalonHCReward() ? 0x2044 : 0x2043;
    }
    u16 faceReaction = Text_GetFaceReaction(play, 0x17);

    if (faceReaction != 0) {
        return faceReaction;
    }
    if (!gSaveContext.n64ddFlag) {
        if (CHECK_QUEST_ITEM(QUEST_SONG_EPONA)) {
            return 0x204A;
        }
    }
    if (Flags_GetEventChkInf(EVENTCHKINF_INVITED_TO_SING_WITH_CHILD_MALON)) {
        return 0x2049;
    }
    if (Flags_GetEventChkInf(EVENTCHKINF_SPOKE_TO_CHILD_MALON_AT_RANCH)) {
        if ((Flags_GetInfTable(INFTABLE_CHILD_MALON_SAID_EPONA_WAS_AFRAID_OF_YOU))) {
            return 0x2049;
        } else {
            return 0x2048;
        }
    }
    if (Flags_GetEventChkInf(EVENTCHKINF_TALON_RETURNED_FROM_CASTLE)) {
        return 0x2047;
    }
    if (Flags_GetEventChkInf(EVENTCHKINF_OBTAINED_POCKET_EGG)) {
        return 0x2044;
    }
    if (Flags_GetInfTable(INFTABLE_MET_CHILD_MALON_AT_CASTLE_OR_MARKET)) {
        if (Flags_GetInfTable(INFTABLE_ENTERED_HYRULE_CASTLE)) {
            return 0x2043;
        } else {
            return 0x2042;
        }
    }
    return 0x2041;
}

s16 func_80AA0778(PlayState* play, Actor* thisx) {
    s16 ret = NPC_TALK_STATE_TALKING;

    switch (Message_GetState(&play->msgCtx)) {
        case TEXT_STATE_CLOSING:
            switch (thisx->textId) {
                case 0x2041:
                    Flags_SetInfTable(INFTABLE_MET_CHILD_MALON_AT_CASTLE_OR_MARKET);
                    Flags_SetEventChkInf(EVENTCHKINF_SPOKE_TO_CHILD_MALON_AT_CASTLE_OR_MARKET);
                    ret = NPC_TALK_STATE_IDLE;
                    break;
                case 0x2043:
                    ret = NPC_TALK_STATE_TALKING;
                    break;
                case 0x2047:
                    Flags_SetEventChkInf(EVENTCHKINF_SPOKE_TO_CHILD_MALON_AT_RANCH);
                    ret = NPC_TALK_STATE_IDLE;
                    break;
                case 0x2048:
                    Flags_SetInfTable(INFTABLE_CHILD_MALON_SAID_EPONA_WAS_AFRAID_OF_YOU);
                    ret = NPC_TALK_STATE_IDLE;
                    break;
                case 0x2049:
                    Flags_SetEventChkInf(EVENTCHKINF_INVITED_TO_SING_WITH_CHILD_MALON);
                    ret = NPC_TALK_STATE_IDLE;
                    break;
                case 0x2061:
                    ret = NPC_TALK_STATE_ACTION;
                    break;
                default:
                    ret = NPC_TALK_STATE_IDLE;
                    break;
            }
            break;
        case TEXT_STATE_CHOICE:
        case TEXT_STATE_EVENT:
            if (Message_ShouldAdvance(play)) {
                ret = NPC_TALK_STATE_ACTION;
            }
            break;
        case TEXT_STATE_DONE:
            if (Message_ShouldAdvance(play)) {
                ret = NPC_TALK_STATE_ITEM_GIVEN;
            }
            break;
        case TEXT_STATE_NONE:
        case TEXT_STATE_DONE_HAS_NEXT:
        case TEXT_STATE_DONE_FADING:
        case TEXT_STATE_SONG_DEMO_DONE:
        case TEXT_STATE_8:
        case TEXT_STATE_9:
            ret = NPC_TALK_STATE_TALKING;
            break;
    }
    return ret;
}

s32 func_80AA08C4(EnMa1* this, PlayState* play) {
    if ((this->actor.shape.rot.z == 3) && (gSaveContext.sceneSetupIndex == 5)) {
        return 1;
    }
    if (!LINK_IS_CHILD) {
        return 0;
    }
    // Causes Malon to appear in the market if you haven't met her yet.
    if (((play->sceneNum == SCENE_MARKET_NIGHT) || (play->sceneNum == SCENE_MARKET_DAY)) &&
        !Flags_GetEventChkInf(EVENTCHKINF_TALON_RETURNED_FROM_CASTLE) && !Flags_GetInfTable(INFTABLE_ENTERED_HYRULE_CASTLE)) {
        return 1;
    }
    if ((play->sceneNum == SCENE_HYRULE_CASTLE) &&  // if we're at hyrule castle
        (!Flags_GetEventChkInf(EVENTCHKINF_TALON_RETURNED_FROM_CASTLE) || // and talon hasn't left
         (gSaveContext.n64ddFlag &&
          !Randomizer_ObtainedMalonHCReward()))) { // or we're rando'd and haven't gotten malon's HC check
        if (Flags_GetInfTable(INFTABLE_ENTERED_HYRULE_CASTLE)) {    // if we've met malon
            return 1;                              // make her appear at the castle
        } else {                                   // if we haven't met malon
            Flags_SetInfTable(INFTABLE_ENTERED_HYRULE_CASTLE);     // set the flag for meeting malon
            return 0;                              // don't make her appear at the castle
        }
    }
    // Malon asleep in her bed if Talon has left Hyrule Castle and it is nighttime.
    if ((play->sceneNum == SCENE_LON_LON_BUILDINGS) && IS_NIGHT && (Flags_GetEventChkInf(EVENTCHKINF_TALON_RETURNED_FROM_CASTLE))) {
        return 1;
    }
    // Don't spawn Malon if none of the above are true and we are not in Lon Lon Ranch.
    if (play->sceneNum != SCENE_LON_LON_RANCH) {
        return 0;
    }
    // If we've gotten this far, we're in Lon Lon Ranch. Spawn Malon if it is daytime, Talon has left Hyrule Castle, and
    // either we are not randomized, or we are and we have received Malon's item at Hyrule Castle.
    if ((this->actor.shape.rot.z == 3) && IS_DAY && (Flags_GetEventChkInf(EVENTCHKINF_TALON_RETURNED_FROM_CASTLE)) && 
        ((gSaveContext.n64ddFlag && Randomizer_ObtainedMalonHCReward()) || !gSaveContext.n64ddFlag)) {
        return 1;
    }
    return 0;
}

void EnMa1_UpdateEyes(EnMa1* this) {
    if (DECR(this->blinkTimer) == 0) {
        this->eyeIndex += 1;
        if (this->eyeIndex >= 3) {
            this->blinkTimer = Rand_S16Offset(30, 30);
            this->eyeIndex = 0;
        }
    }
}

void EnMa1_ChangeAnim(EnMa1* this, s32 index) {
    f32 frameCount = Animation_GetLastFrame(sAnimationInfo[index].animation);

    Animation_Change(&this->skelAnime, sAnimationInfo[index].animation, 1.0f, 0.0f, frameCount,
                     sAnimationInfo[index].mode, sAnimationInfo[index].morphFrames);
}

void func_80AA0AF4(EnMa1* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 trackingMode;

    if ((this->interactInfo.talkState == NPC_TALK_STATE_IDLE) && (this->skelAnime.animation == &gMalonChildSingAnim)) {
        trackingMode = NPC_TRACKING_NONE;
    } else {
        trackingMode = NPC_TRACKING_PLAYER_AUTO_TURN;
    }

    this->interactInfo.trackPos = player->actor.world.pos;
    this->interactInfo.trackPos.y -= -10.0f;

    Npc_TrackPoint(&this->actor, &this->interactInfo, 0, trackingMode);
}

void func_80AA0B74(EnMa1* this) {
    if (this->skelAnime.animation == &gMalonChildSingAnim) {
        if (this->interactInfo.talkState == NPC_TALK_STATE_IDLE) {
            if (this->unk_1E0 != 0) {
                this->unk_1E0 = 0;
                func_800F6584(0);
            }
        } else {
            if (this->unk_1E0 == 0) {
                this->unk_1E0 = 1;
                func_800F6584(1);
            }
        }
    }
}

void EnMa1_Init(Actor* thisx, PlayState* play) {
    EnMa1* this = (EnMa1*)thisx;
    s32 pad;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 18.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &gMalonChildSkel, NULL, NULL, NULL, 0);
    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    CollisionCheck_SetInfo2(&this->actor.colChkInfo, DamageTable_Get(22), &sColChkInfoInit);

    if (gSaveContext.n64ddFlag) { // Skip Malon's multiple textboxes before getting an item
        Flags_SetInfTable(INFTABLE_ENTERED_HYRULE_CASTLE);
        Flags_SetInfTable(INFTABLE_MET_CHILD_MALON_AT_CASTLE_OR_MARKET);
        Flags_SetEventChkInf(EVENTCHKINF_SPOKE_TO_CHILD_MALON_AT_CASTLE_OR_MARKET);
    }

    if (!func_80AA08C4(this, play)) {
        Actor_Kill(&this->actor);
        return;
    }

    Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 0.0f, 0.0f, 4);
    Actor_SetScale(&this->actor, 0.01f);
    this->actor.targetMode = 6;
    this->interactInfo.talkState = NPC_TALK_STATE_IDLE;

   // To avoid missing a check, we want Malon to have the actionFunc for singing, but not reacting to Ocarina, if any of
   // the following are true.
   // 1. Talon has not left Hyrule Castle.
   // 2. We are Randomized and have not obtained Malon's Weird Egg Check.
   // 3. We are not Randomized and have obtained Epona's Song
    if (!Flags_GetEventChkInf(EVENTCHKINF_TALON_RETURNED_FROM_CASTLE) || (gSaveContext.n64ddFlag && !Randomizer_ObtainedMalonHCReward()) || (CHECK_QUEST_ITEM(QUEST_SONG_EPONA) && !gSaveContext.n64ddFlag) ||
        (gSaveContext.n64ddFlag && Flags_GetTreasure(play, 0x1F))) {
        this->actionFunc = func_80AA0D88;
        EnMa1_ChangeAnim(this, ENMA1_ANIM_2);
    // If none of the above conditions were true, set Malon up to teach Epona's Song.
    } else {
        if (gSaveContext.n64ddFlag) { // Skip straight to "let's sing it together" textbox in the ranch
            Flags_SetEventChkInf(EVENTCHKINF_INVITED_TO_SING_WITH_CHILD_MALON);
        }

        this->actionFunc = func_80AA0F44;
        EnMa1_ChangeAnim(this, ENMA1_ANIM_2);
    }
}

void EnMa1_Destroy(Actor* thisx, PlayState* play) {
    EnMa1* this = (EnMa1*)thisx;

    SkelAnime_Free(&this->skelAnime, play);
    Collider_DestroyCylinder(play, &this->collider);
}

void func_80AA0D88(EnMa1* this, PlayState* play) {
    if (this->interactInfo.talkState != NPC_TALK_STATE_IDLE) {
        if (this->skelAnime.animation != &gMalonChildIdleAnim) {
            EnMa1_ChangeAnim(this, ENMA1_ANIM_1);
        }
    } else {
        if (this->skelAnime.animation != &gMalonChildSingAnim) {
            EnMa1_ChangeAnim(this, ENMA1_ANIM_3);
        }
    }

    // We want to Kill Malon's Actor outside of randomizer when Talon is freed. In Randomizer we don't kill Malon's
    // Actor here, otherwise if we wake up Talon first and then get her check she will spontaneously
    // disappear.
    if ((play->sceneNum == SCENE_HYRULE_CASTLE) && (!gSaveContext.n64ddFlag && Flags_GetEventChkInf(EVENTCHKINF_TALON_RETURNED_FROM_CASTLE))) {
        Actor_Kill(&this->actor);
    // We want Malon to give the Weird Egg Check (see function below) in the following situations:
    // 1. Talon as not left Hyrule Castle (Vanilla) OR
    // 2. We haven't obtained Malon's Weird Egg Check (Randomizer only) OR
    // 3. We have Epona's Song? (Vanilla only, not sure why it's here but I didn't write that one)
    } else if ((!Flags_GetEventChkInf(EVENTCHKINF_TALON_RETURNED_FROM_CASTLE) || (gSaveContext.n64ddFlag && !Randomizer_ObtainedMalonHCReward())) || (CHECK_QUEST_ITEM(QUEST_SONG_EPONA) && !gSaveContext.n64ddFlag)) {
        if (this->interactInfo.talkState == NPC_TALK_STATE_ACTION) {
            this->actionFunc = func_80AA0EA0;
            play->msgCtx.stateTimer = 4;
            play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
        }
    }
}

void func_80AA0EA0(EnMa1* this, PlayState* play) {
    if (Actor_HasParent(&this->actor, play)) {
        this->actor.parent = NULL;
        this->actionFunc = func_80AA0EFC;
    } else {
        if (!gSaveContext.n64ddFlag) {
            func_8002F434(&this->actor, play, GI_WEIRD_EGG, 120.0f, 10.0f);
        } else {
            GetItemEntry getItemEntry = Randomizer_GetItemFromKnownCheck(RC_HC_MALON_EGG, GI_WEIRD_EGG);
            GiveItemEntryFromActor(&this->actor, play, getItemEntry, 120.0f, 10.0f);
        }
    }
}

void func_80AA0EFC(EnMa1* this, PlayState* play) {
    if (this->interactInfo.talkState == NPC_TALK_STATE_ITEM_GIVEN) {
        this->interactInfo.talkState = NPC_TALK_STATE_IDLE;
        this->actionFunc = func_80AA0D88;
        Flags_SetEventChkInf(EVENTCHKINF_OBTAINED_POCKET_EGG);
        play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
    }
}

void GivePlayerRandoRewardMalon(EnMa1* malon, PlayState* play, RandomizerCheck check) {
    GetItemEntry getItemEntry = Randomizer_GetItemFromKnownCheck(check, RG_EPONAS_SONG);
    // Prevents flag from getting set if we weren't able to get the item (i.e. Player is holding shield
    // when closing the textbox).
    if (malon->actor.parent != NULL && malon->actor.parent->id == GET_PLAYER(play)->actor.id &&
        !Flags_GetTreasure(play, 0x1F)) {
        Flags_SetTreasure(play, 0x1F);
        // puts malon in the action that vanilla has her in after learning the song
        // (confirmed via breakpoints in a vanilla save).
        malon->actionFunc = func_80AA0D88;
    } else if (!Flags_GetTreasure(play, 0x1F)) {
        GiveItemEntryFromActor(&malon->actor, play, getItemEntry, 10000.0f, 100.0f);
    }
    // make malon sing again after giving the item.
    malon->interactInfo.talkState = NPC_TALK_STATE_IDLE;
    malon->unk_1E0 = 1;
}

void func_80AA0F44(EnMa1* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (this->interactInfo.talkState != NPC_TALK_STATE_IDLE) {
        if (this->skelAnime.animation != &gMalonChildIdleAnim) {
            EnMa1_ChangeAnim(this, ENMA1_ANIM_1);
        }
    } else {
        if (this->skelAnime.animation != &gMalonChildSingAnim) {
            EnMa1_ChangeAnim(this, ENMA1_ANIM_3);
        }
    }

    if (Flags_GetEventChkInf(EVENTCHKINF_INVITED_TO_SING_WITH_CHILD_MALON)) {
        // When the player pulls out the Ocarina while close to Malon
        if (player->stateFlags2 & 0x1000000) {
            player->stateFlags2 |= 0x2000000;
            player->unk_6A8 = &this->actor;
            this->actor.textId = 0x2061;
            Message_StartTextbox(play, this->actor.textId, NULL);
            this->interactInfo.talkState = NPC_TALK_STATE_TALKING;
            this->actor.flags |= ACTOR_FLAG_WILL_TALK;
            // when rando'ed, skip to the Item Giving. Otherwise go to the song teaching code.
            this->actionFunc = gSaveContext.n64ddFlag ? func_80AA1150 : func_80AA106C;
        } else if (this->actor.xzDistToPlayer < 30.0f + (f32)this->collider.dim.radius) {
            // somehow flags that the player is close to malon so that pulling out the Ocarina
            // triggers the code above this.
            player->stateFlags2 |= 0x800000;
        }
        // If rando'ed, a textbox is closing, it's malon's 'my mom wrote this song' text, AND we do have an ocarina
        // in our inventory. This allows us to grant the check when talking to malon with the ocarina in our inventory.
        if (gSaveContext.n64ddFlag && (Actor_TextboxIsClosing(&this->actor, play) && play->msgCtx.textId == 0x2049) &&
            (INV_CONTENT(ITEM_OCARINA_FAIRY) != ITEM_NONE || INV_CONTENT(ITEM_OCARINA_TIME) != ITEM_NONE)) {
            this->actionFunc = EnMa1_WaitForSongGive;
        }
    }
}

void func_80AA106C(EnMa1* this, PlayState* play) {
    GET_PLAYER(play)->stateFlags2 |= 0x800000;
    if (this->interactInfo.talkState == NPC_TALK_STATE_ACTION) {
        Audio_OcaSetInstrument(2);
        func_8010BD58(play, OCARINA_ACTION_TEACH_EPONA);
        this->actor.flags &= ~ACTOR_FLAG_WILL_TALK;
        this->actionFunc = func_80AA10EC;
    }
}

void func_80AA10EC(EnMa1* this, PlayState* play) {
    GET_PLAYER(play)->stateFlags2 |= 0x800000;
    if (Message_GetState(&play->msgCtx) == TEXT_STATE_SONG_DEMO_DONE) {
        func_8010BD58(play, OCARINA_ACTION_PLAYBACK_EPONA);
        this->actionFunc = func_80AA1150;
    }
}

void EnMa1_WaitForSongGive(EnMa1* this, PlayState* play) {
    // Actually give the song check.
    GivePlayerRandoRewardMalon(this, play, RC_SONG_FROM_MALON);
}

// Sets an Ocarina State necessary to not softlock in rando.
// This function should only be called in rando.
void EnMa1_EndTeachSong(EnMa1* this, PlayState* play) {
    if (play->csCtx.state == CS_STATE_IDLE) {
        this->actionFunc = func_80AA0F44;
        play->msgCtx.ocarinaMode = OCARINA_MODE_04;
    }

    if (gSaveContext.n64ddFlag) {
        // Transition to the giving the song check on the next update run.
        this->actionFunc = EnMa1_WaitForSongGive;
    }
}

void func_80AA1150(EnMa1* this, PlayState* play) {
    GET_PLAYER(play)->stateFlags2 |= 0x800000;

    // When rando'ed, trigger the "song learned" Ocarina mode.
    if (gSaveContext.n64ddFlag && (Message_GetState(&play->msgCtx) == TEXT_STATE_CLOSING)) {
        play->msgCtx.ocarinaMode = OCARINA_MODE_03;
    }

    if (play->msgCtx.ocarinaMode == OCARINA_MODE_03) {
        if (!gSaveContext.n64ddFlag) {
            play->nextEntranceIndex = 0x157;
            gSaveContext.nextCutsceneIndex = 0xFFF1;
            play->fadeTransition = 42;
            play->sceneLoadFlag = 0x14;
            this->actionFunc = EnMa1_DoNothing;
        } else {
            // When rando'ed, skip the cutscene, play the chime, reset some flags,
            // and give the song on next update.
            func_80078884(NA_SE_SY_CORRECT_CHIME);
            this->actionFunc = EnMa1_EndTeachSong;
            this->actor.flags &= ~ACTOR_FLAG_WILL_TALK;
            play->msgCtx.ocarinaMode = OCARINA_MODE_00;
        }
    }
}

void EnMa1_DoNothing(EnMa1* this, PlayState* play) {
}

void EnMa1_Update(Actor* thisx, PlayState* play) {
    EnMa1* this = (EnMa1*)thisx;
    s32 pad;

    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    SkelAnime_Update(&this->skelAnime);
    EnMa1_UpdateEyes(this);
    this->actionFunc(this, play);
    if (this->actionFunc != EnMa1_DoNothing) {
        Npc_UpdateTalking(play, &this->actor, &this->interactInfo.talkState, (f32)this->collider.dim.radius + 30.0f,
                          EnMa1_GetText, func_80AA0778);
    }
    func_80AA0B74(this);
    func_80AA0AF4(this, play);
}

s32 EnMa1_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnMa1* this = (EnMa1*)thisx;
    Vec3s vec;

    if ((limbIndex == 2) || (limbIndex == 5)) {
        *dList = NULL;
    }
    if (limbIndex == 15) {
        Matrix_Translate(1400.0f, 0.0f, 0.0f, MTXMODE_APPLY);
        vec = this->interactInfo.headRot;
        Matrix_RotateX((vec.y / 32768.0f) * M_PI, MTXMODE_APPLY);
        Matrix_RotateZ((vec.x / 32768.0f) * M_PI, MTXMODE_APPLY);
        Matrix_Translate(-1400.0f, 0.0f, 0.0f, MTXMODE_APPLY);
    }
    if (limbIndex == 8) {
        vec = this->interactInfo.torsoRot;
        Matrix_RotateX((-vec.y / 32768.0f) * M_PI, MTXMODE_APPLY);
        Matrix_RotateZ((-vec.x / 32768.0f) * M_PI, MTXMODE_APPLY);
    }
    return false;
}

void EnMa1_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    EnMa1* this = (EnMa1*)thisx;
    Vec3f vec = D_80AA16B8;

    if (limbIndex == 15) {
        Matrix_MultVec3f(&vec, &this->actor.focus.pos);
    }
}

void EnMa1_Draw(Actor* thisx, PlayState* play) {
    EnMa1* this = (EnMa1*)thisx;
    Camera* camera;
    f32 distFromCamera;
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx);

    camera = GET_ACTIVE_CAM(play);
    distFromCamera = Math_Vec3f_DistXZ(&this->actor.world.pos, &camera->eye);
    func_800F6268(distFromCamera, NA_BGM_LONLON);
    Gfx_SetupDL_25Opa(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(sMouthTextures[this->mouthIndex]));
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(sEyeTextures[this->eyeIndex]));

    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnMa1_OverrideLimbDraw, EnMa1_PostLimbDraw, this);

    CLOSE_DISPS(play->state.gfxCtx);
}
