/*
 * File: z_en_aob_01.c
 * Overlay: ovl_En_Aob_01
 * Description: Mamamu Yan
 */

#include "z_en_aob_01.h"
#include "overlays/actors/ovl_En_Racedog/z_en_racedog.h"
#include "overlays/actors/ovl_En_Dg/z_en_dg.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void EnAob01_Init(Actor* thisx, PlayState* play);
void EnAob01_Destroy(Actor* thisx, PlayState* play);
void EnAob01_Update(Actor* thisx, PlayState* play);
void EnAob01_Draw(Actor* thisx, PlayState* play);

void EnAob01_BeforeRace_StartRace(EnAob01* this, PlayState* play);
void EnAob01_BeforeRace_RespondToPlayAgainQuestion(EnAob01* this, PlayState* play);
void EnAob01_BeforeRace_Idle(EnAob01* this, PlayState* play);
void EnAob01_BeforeRace_Talk(EnAob01* this, PlayState* play);
void EnAob01_Race_SkipNonRaceCutscene(EnAob01* this, PlayState* play);
void EnAob01_Race_HandleRaceCutscene(EnAob01* this, PlayState* play);
void EnAob01_Race_HandleRaceEndCutscene(EnAob01* this, PlayState* play);
void EnAob01_Race_StartCutscene(EnAob01* this, PlayState* play);
void EnAob01_AfterRace_GiveRaceResult(EnAob01* this, PlayState* play);
void EnAob01_AfterRace_GiveReward(EnAob01* this, PlayState* play);
void EnAob01_AfterRace_AfterGivingReward(EnAob01* this, PlayState* play);
void EnAob01_AfterRace_AskToPlayAgain(EnAob01* this, PlayState* play);
void EnAob01_AfterRace_Talk(EnAob01* this, PlayState* play);
s32 EnAob01_PlayerIsHoldingDog(EnAob01* this, PlayState* play);

ActorProfile En_Aob_01_Profile = {
    /**/ ACTOR_EN_AOB_01,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_AOB,
    /**/ sizeof(EnAob01),
    /**/ EnAob01_Init,
    /**/ EnAob01_Destroy,
    /**/ EnAob01_Update,
    /**/ EnAob01_Draw,
};

typedef enum {
    /* 0 */ EN_AOB01_ANIM_IDLE,
    /* 1 */ EN_AOB01_ANIM_LAUGH_START,
    /* 2 */ EN_AOB01_ANIM_LAUGH_LOOP,
    /* 3 */ EN_AOB01_ANIM_SURPRISE_START,
    /* 4 */ EN_AOB01_ANIM_SURPRISE_LOOP,
    /* 5 */ EN_AOB01_ANIM_IDLE_MORPH,
    /* 6 */ EN_AOB01_ANIM_MAX
} EnAob01Animation;

typedef enum {
    /* 0 */ EN_AOB01_EYE_OPEN,
    /* 1 */ EN_AOB01_EYE_HALF,
    /* 2 */ EN_AOB01_EYE_CLOSED,
    /* 3 */ EN_AOB01_EYE_MAX
} EnAob01EyeTexture;

static AnimationInfo sAnimationInfo[EN_AOB01_ANIM_MAX] = {
    { &gMamamuYanIdleAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, 0.0f },          // EN_AOB01_ANIM_IDLE
    { &gMamamuYanLaughStartAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, 0.0f },    // EN_AOB01_ANIM_LAUGH_START
    { &gMamamuYanLaughLoopAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, 0.0f },     // EN_AOB01_ANIM_LAUGH_LOOP
    { &gMamamuYanSurpriseStartAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, 0.0f }, // EN_AOB01_ANIM_SURPRISE_START
    { &gMamamuYanSurpriseLoopAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, 0.0f },  // EN_AOB01_ANIM_SURPRISE_LOOP
    { &gMamamuYanIdleAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -6.0f },         // EN_AOB01_ANIM_IDLE_MORPH
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
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

typedef struct {
    /* 0x0 */ Vec3f pos;
    /* 0xC */ s16 rotY;
    /* 0xE */ s16 pathIndex;
} DogInfo; // size = 0x10

/**
 * Stores the starting position, y-rotation, and path index for all 14 dogs in the racetrack.
 * This actor is responsible for spawning these dogs; they are not spawned as part of the scene.
 */
static DogInfo sDogInfo[RACEDOG_COUNT] = {
    { { -4130.0f, 150.0f, 1367.0f }, 84, 0 },  { { -4861.0f, 172.0f, 1606.0f }, 94, 4 },
    { { -4139.0f, 155.0f, 2133.0f }, 73, 6 },  { { -4406.0f, 144.0f, 1416.0f }, 88, 2 },
    { { -4156.0f, 155.0f, 1731.0f }, 42, 0 },  { { -4033.0f, 157.0f, 1994.0f }, -65, 1 },
    { { -4582.0f, 158.0f, 1206.0f }, 144, 2 }, { { -4595.0f, 156.0f, 1493.0f }, 61, 3 },
    { { -4526.0f, 146.0f, 1702.0f }, 61, 4 },  { { -3820.0f, 162.0f, 1965.0f }, 109, 5 },
    { { -4395.0f, 147.0f, 1569.0f }, -24, 0 }, { { -4315.0f, 150.0f, 2048.0f }, 61, 6 },
    { { -4827.0f, 168.0f, 1328.0f }, 115, 4 }, { { -4130.0f, 150.0f, 1367.0f }, 112, 0 },
};

void EnAob01_Blink(EnAob01* this, s32 maxEyeIndex) {
    if (DECR(this->blinkTimer) == 0) {
        this->eyeIndex++;
        if (this->eyeIndex >= maxEyeIndex) {
            this->eyeIndex = EN_AOB01_EYE_OPEN;
            this->blinkTimer = Rand_S16Offset(30, 30);
        }
    }
}

/**
 * Called every frame during the race in order to make in-game time pass.
 */
void EnAob01_AdvanceTime(void) {
    gSaveContext.save.time = CURRENT_TIME + (u16)R_TIME_SPEED;
    gSaveContext.save.time = CURRENT_TIME + (u16)((void)0, gSaveContext.save.timeSpeedOffset);
}

/**
 * Initializes all the paths that the dogs follow.
 */
void EnAob01_InitializeDogPaths(EnAob01* this, PlayState* play) {
    s32 pathIndex = ENAOB01_GET_STARTING_DOG_PATH_INDEX(&this->actor);
    s16 i = 0;

    if (pathIndex != ENAOB01_PATH_INDEX_NONE) {
        do {
            this->dogPaths[i] = SubS_GetPathByIndex(play, pathIndex, ENAOB01_PATH_INDEX_NONE);
            pathIndex = this->dogPaths[i]->additionalPathIndex;
            i++;
        } while (pathIndex != ADDITIONAL_PATH_INDEX_NONE);
    }
}

/**
 * Spawns the dogs that mill about the racetrack before and after each race.
 */
void EnAob01_SpawnDogs(EnAob01* this, PlayState* play) {
    s32 enDgParams;
    s16 i;

    EnAob01_InitializeDogPaths(this, play);

    for (i = 0; i < RACEDOG_COUNT; i++) {
        enDgParams = ENDG_PARAMS(this->dogPaths[sDogInfo[i].pathIndex]->additionalPathIndex, i);

        this->dogs[i] = Actor_SpawnAsChildAndCutscene(
            &play->actorCtx, play, ACTOR_EN_DG, sDogInfo[i].pos.x, sDogInfo[i].pos.y, sDogInfo[i].pos.z, 0,
            DEG_TO_BINANG(sDogInfo[i].rotY), 0, enDgParams, (u16)CS_ID_NONE, this->actor.halfDaysBits, NULL);
    }
}

/**
 * Spawns the dogs that run around the track during each race.
 */
void EnAob01_SpawnRacedogs(EnAob01* this, PlayState* play) {
    s16 i;

    for (i = 0; i < RACEDOG_COUNT; i++) {
        this->dogs[i] = Actor_SpawnAsChildAndCutscene(&play->actorCtx, play, ACTOR_EN_RACEDOG, (i * 15.0f) + -3897.0f,
                                                      130.0f, 1290.0f - (i * 10.0f), 0, 0x1555, 0,
                                                      ENRACEDOG_PARAMS(i, ENAOB01_GET_RACEDOG_PATH_INDEX(&this->actor)),
                                                      (u16)CS_ID_NONE, this->actor.halfDaysBits, NULL);
    }
}

/**
 * If the racetrack owner is at the end of her laugh start animation, this function will
 * transition her to her laugh loop animation. This function will return true if she is
 * *either* in her laugh start or laugh loop animation, false otherwise.
 */
s32 EnAob01_ProcessLaughAnim(EnAob01* this) {
    s16 curFrame = this->skelAnime.curFrame;
    s16 endFrame = Animation_GetLastFrame(sAnimationInfo[this->animIndex].animation);

    if (this->animIndex == EN_AOB01_ANIM_LAUGH_START) {
        if (curFrame == endFrame) {
            this->animIndex = EN_AOB01_ANIM_LAUGH_LOOP;
            Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, EN_AOB01_ANIM_LAUGH_LOOP);
            return true;
        }
    } else if (this->animIndex == EN_AOB01_ANIM_LAUGH_LOOP) {
        return true;
    }

    return false;
}

/**
 * If the racetrack owner is at the end of her idle animation, this function will
 * transition her to her surprise start animation. If the racetrack owner is at the
 * end of her surprise start animation, this function will transition her to her
 * surprise loop animation. This function will return true if she is *either* in
 * her surprise start or surprise loop animation, false otherwise.
 */
s32 EnAob01_ProcessSurpriseAnim(EnAob01* this) {
    s16 curFrame = this->skelAnime.curFrame;
    s16 endFrame = Animation_GetLastFrame(sAnimationInfo[this->animIndex].animation);

    if ((this->animIndex == EN_AOB01_ANIM_IDLE) || (this->animIndex == EN_AOB01_ANIM_IDLE_MORPH)) {
        if (curFrame == endFrame) {
            this->animIndex = EN_AOB01_ANIM_SURPRISE_START;
            Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, EN_AOB01_ANIM_SURPRISE_START);
            return true;
        }
    } else if (this->animIndex == EN_AOB01_ANIM_SURPRISE_START) {
        if (curFrame == endFrame) {
            this->animIndex = EN_AOB01_ANIM_SURPRISE_LOOP;
            Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, EN_AOB01_ANIM_SURPRISE_LOOP);
            return true;
        }
    } else if (this->animIndex == EN_AOB01_ANIM_SURPRISE_LOOP) {
        return true;
    }

    return false;
}

/**
 * If the racetrack owner is at the end of her current animation, this function will
 * transition her to her the idle animation. This function will return true if she is
 * in her idle animation, false otherwise.
 */
s32 EnAob01_ProcessIdleAnim(EnAob01* this) {
    s16 curFrame = this->skelAnime.curFrame;
    s16 endFrame = Animation_GetLastFrame(sAnimationInfo[this->animIndex].animation);

    if ((this->animIndex != EN_AOB01_ANIM_IDLE) && (this->animIndex != EN_AOB01_ANIM_IDLE_MORPH)) {
        if (curFrame == endFrame) {
            this->animIndex = EN_AOB01_ANIM_IDLE_MORPH;
            Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, EN_AOB01_ANIM_IDLE_MORPH);
            return true;
        }
    } else {
        return true;
    }

    return false;
}

void EnAob01_UpdateCollision(EnAob01* this, PlayState* play) {
    this->collider.dim.pos.x = this->actor.world.pos.x;
    this->collider.dim.pos.y = this->actor.world.pos.y;
    this->collider.dim.pos.z = this->actor.world.pos.z;
    CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
}

/**
 * Handles all parts of the conversation with the racetrack owner before the race begins.
 */
void EnAob01_BeforeRace_HandleConversation(EnAob01* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    switch (this->textId) {
        case 0:
        case 0x3524: // You can't play if you can't pay!
        case 0x3535: // Really?
        case 0x3548: // I can't let you play. (Goron)
        case 0x3549: // I can't let you play. (Zora)
        case 0x354A: // I can't let you play. (Deku)
            switch (gSaveContext.save.day) {
                case 1:
                    if (!gSaveContext.save.isNight) {
                        if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_TALKED_DOGGY_RACETRACK_OWNER_DAY_1)) {
                            SET_WEEKEVENTREG(WEEKEVENTREG_TALKED_DOGGY_RACETRACK_OWNER_DAY_1);
                            this->textId = 0x3520; // This is the Doggy Racetrack. (Day 1)
                        } else {
                            this->textId = 0x352F; // What do you want?
                        }
                    } else {
                        if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_TALKED_DOGGY_RACETRACK_OWNER_NIGHT_1)) {
                            SET_WEEKEVENTREG(WEEKEVENTREG_TALKED_DOGGY_RACETRACK_OWNER_NIGHT_1);
                            this->textId = 0x3530; // This is the Doggy Racetrack. (Night 1)
                        } else {
                            this->textId = 0x352F; // What do you want?
                        }
                    }
                    break;

                case 2:
                    if (!gSaveContext.save.isNight) {
                        if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_TALKED_DOGGY_RACETRACK_OWNER_DAY_2)) {
                            SET_WEEKEVENTREG(WEEKEVENTREG_TALKED_DOGGY_RACETRACK_OWNER_DAY_2);
                            this->textId = 0x3531; // This is the Doggy Racetrack. (Day 2)
                        } else {
                            this->textId = 0x352F; // What do you want?
                        }
                    } else {
                        if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_TALKED_DOGGY_RACETRACK_OWNER_NIGHT_2)) {
                            SET_WEEKEVENTREG(WEEKEVENTREG_TALKED_DOGGY_RACETRACK_OWNER_NIGHT_2);
                            this->textId = 0x3532; // This is the Doggy Racetrack. (Night 2)
                        } else {
                            this->textId = 0x352F; // What do you want?
                        }
                    }
                    break;

                case 3:
                    if (!gSaveContext.save.isNight) {
                        if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_TALKED_DOGGY_RACETRACK_OWNER_DAY_3)) {
                            SET_WEEKEVENTREG(WEEKEVENTREG_TALKED_DOGGY_RACETRACK_OWNER_DAY_3);
                            this->textId = 0x3533; // This is the Doggy Racetrack. (Day 3)
                        } else {
                            this->textId = 0x352F; // What do you want?
                        }
                    } else {
                        if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_TALKED_DOGGY_RACETRACK_OWNER_NIGHT_3)) {
                            SET_WEEKEVENTREG(WEEKEVENTREG_TALKED_DOGGY_RACETRACK_OWNER_NIGHT_3);
                            this->textId = 0x3534; // This is the Doggy Racetrack. (Night 3)
                        } else {
                            this->textId = 0x352F; // What do you want?
                        }
                    }
                    break;
            }
            break;

        case 0x3520:               // This is the Doggy Racetrack. (Day 1)
        case 0x352F:               // What do you want?
        case 0x3530:               // This is the Doggy Racetrack. (Night 1)
        case 0x3531:               // This is the Doggy Racetrack. (Day 2)
        case 0x3532:               // This is the Doggy Racetrack. (Night 2)
        case 0x3533:               // This is the Doggy Racetrack. (Day 3)
        case 0x3534:               // This is the Doggy Racetrack. (Night 3)
            this->textId = 0x3521; // Do you want to try the dog race?
            break;

        case 0x3521: // Do you want to try the dog race?
            if (this->stateFlags & ENAOB01_FLAG_PLAYER_CONFIRMED_CHOICE) {
                this->stateFlags &= ~ENAOB01_FLAG_PLAYER_CONFIRMED_CHOICE;

                switch (player->transformation) {
                    case PLAYER_FORM_GORON:
                        this->textId = 0x3548; // I can't let you play. (Goron)
                        this->stateFlags |= ENAOB01_FLAG_CONVERSATION_OVER;
                        break;

                    case PLAYER_FORM_ZORA:
                        this->textId = 0x3549; // I can't let you play. (Zora)
                        this->stateFlags |= ENAOB01_FLAG_CONVERSATION_OVER;
                        break;

                    case PLAYER_FORM_DEKU:
                        this->textId = 0x354A; // I can't let you play. (Deku)
                        this->stateFlags |= ENAOB01_FLAG_CONVERSATION_OVER;
                        break;

                    case PLAYER_FORM_HUMAN:
                        if (gSaveContext.save.saveInfo.playerData.rupees < 10) {
                            this->textId = 0x3524; // You can't play if you can't pay!
                            this->stateFlags |= ENAOB01_FLAG_CONVERSATION_OVER;
                        } else {
                            this->textId = 0x3522; // Bring me the fastest dog!
                            this->stateFlags |= ENAOB01_FLAG_PLAYER_TOLD_TO_PICK_A_DOG;
                            this->stateFlags |= ENAOB01_FLAG_CONVERSATION_OVER;
                        }
                }
            } else {
                this->stateFlags |= ENAOB01_FLAG_CONVERSATION_OVER;
                this->textId = 0x3535; // Is that dog fine?
            }
            break;

        case 0x3522: // Bring me the fastest dog!
        case 0x3523: // Hurry up and bring me a dog!
            if (this->stateFlags & ENAOB01_FLAG_TALKING_TO_PLAYER_HOLDING_DOG) {
                this->textId = 0x3525; // Is that dog okay?
                break;
            }

            this->textId = 0x3523; // Hurry up and bring me a dog!
            this->stateFlags |= ENAOB01_FLAG_LAUGH;
            this->stateFlags |= ENAOB01_FLAG_CONVERSATION_OVER;
            this->animIndex = EN_AOB01_ANIM_LAUGH_START;
            Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, EN_AOB01_ANIM_LAUGH_START);
            break;

        case 0x3525: // Is that dog okay?
        case 0x3526: // Don't wait! Hurry up and choose!
            if (this->stateFlags & ENAOB01_FLAG_PLAYER_TOLD_TO_PICK_A_DOG) {
                if (this->stateFlags & ENAOB01_FLAG_TALKING_TO_PLAYER_HOLDING_DOG) {
                    this->textId = 0x3525; // Is that dog okay?
                    break;
                }

                this->textId = 0x3523; // Hurry up and bring me a dog!
                this->stateFlags |= ENAOB01_FLAG_LAUGH;
                this->stateFlags |= ENAOB01_FLAG_CONVERSATION_OVER;
                this->animIndex = EN_AOB01_ANIM_LAUGH_START;
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, EN_AOB01_ANIM_LAUGH_START);
                break;
            }

            if (this->stateFlags & ENAOB01_FLAG_PLAYER_CONFIRMED_CHOICE) {
                this->stateFlags &= ~ENAOB01_FLAG_PLAYER_CONFIRMED_CHOICE;
                this->textId = 0x3527; // If you win, you'll get more than you bet.
                break;
            }

            this->textId = 0x3526; // Don't wait! Hurry up and choose!
            this->stateFlags |= ENAOB01_FLAG_LAUGH;
            this->stateFlags |= ENAOB01_FLAG_PLAYER_TOLD_TO_PICK_A_DOG;
            this->stateFlags |= ENAOB01_FLAG_CONVERSATION_OVER;
            this->animIndex = EN_AOB01_ANIM_LAUGH_START;
            Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, EN_AOB01_ANIM_LAUGH_START);
            break;

        case 0x3527:               // If you win, you'll get more than you bet.
            this->textId = 0x3528; // How much will you bet?
            break;

        case 0x3528: // How much will you bet?
            if (gSaveContext.save.saveInfo.playerData.rupees < this->rupeesBet) {
                this->textId = 0x3536; // You don't have enough rupees for that bet.
                this->stateFlags |= ENAOB01_FLAG_LAUGH;
                this->animIndex = EN_AOB01_ANIM_LAUGH_START;
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, EN_AOB01_ANIM_LAUGH_START);
                break;
            }

            if (this->rupeesBet == 0) {
                this->textId = 0x3537; // You can't race if you don't bet.
                this->stateFlags |= ENAOB01_FLAG_LAUGH;
                this->animIndex = EN_AOB01_ANIM_LAUGH_START;
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, EN_AOB01_ANIM_LAUGH_START);
                break;
            }

            gSaveContext.unk_3F5C = this->rupeesBet;
            play->msgCtx.rupeesTotal = this->rupeesBet;
            this->textId = 0x3529; // Is that bet okay?
            break;

        case 0x3529: // Is that bet okay?
            if (this->stateFlags & ENAOB01_FLAG_PLAYER_CONFIRMED_CHOICE) {
                this->stateFlags &= ~ENAOB01_FLAG_PLAYER_CONFIRMED_CHOICE;
                Rupees_ChangeBy(-this->rupeesBet);
                Player_SetCsActionWithHaltedActors(play, NULL, PLAYER_CSACTION_WAIT);
                play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
                play->msgCtx.stateTimer = 4;
                this->actionFunc = EnAob01_BeforeRace_StartRace;
                return;
            }
            this->textId = 0x3528; // How much will you bet?
            break;

        case 0x3536:               // You don't have enough rupees for that bet.
        case 0x3537:               // You can't race if you don't bet.
            this->textId = 0x3528; // How much will you bet?
            break;
    }

    Message_StartTextbox(play, this->textId, &this->actor);
}

/**
 * Once the player's rupees are done counting down, this function will
 * reload the racetrack to start the race.
 */
void EnAob01_BeforeRace_StartRace(EnAob01* this, PlayState* play) {
    if (gSaveContext.rupeeAccumulator == 0) {
        SET_WEEKEVENTREG(WEEKEVENTREG_KICKOUT_WAIT);
        CLEAR_WEEKEVENTREG(WEEKEVENTREG_KICKOUT_TIME_PASSED);
        this->stateFlags |= ENAOB01_FLAG_STARTED_RACE;
        Environment_ForcePlaySequence(NA_BGM_HORSE);
        play->nextEntrance = ENTRANCE(DOGGY_RACETRACK, 1);
        play->transitionType = TRANS_TYPE_64;
        gSaveContext.nextTransitionType = TRANS_TYPE_64;
        SET_EVENTINF_DOG_RACE_STATE(EVENTINF_DOG_RACE_STATE_STARTED);
        SET_EVENTINF_DOG_RACE_SELECTED_DOG_INDEX(this->selectedDogIndex);
        play->transitionTrigger = TRANS_TRIGGER_START;
    }
}

/**
 * Handles the player's response to the question of whether or not they want to play again.
 * Once the textbox closes, the racetrack owner will go back to being idle.
 */
void EnAob01_BeforeRace_RespondToPlayAgainQuestion(EnAob01* this, PlayState* play) {
    u8 talkState = Message_GetState(&play->msgCtx);

    if (talkState == TEXT_STATE_CHOICE) {
        if (Message_ShouldAdvance(play)) {
            switch (play->msgCtx.choiceIndex) {
                case 0:
                    if (gSaveContext.save.saveInfo.playerData.rupees < 10) {
                        Audio_PlaySfx(NA_SE_SY_ERROR);
                        this->textId = 0x3524; // You can't play if you can't pay!
                        Message_StartTextbox(play, this->textId, &this->actor);
                    } else {
                        Audio_PlaySfx_MessageDecide();
                        this->stateFlags |= ENAOB01_FLAG_PLAYER_TOLD_TO_PICK_A_DOG;
                        this->stateFlags |= ENAOB01_FLAG_CONVERSATION_OVER;
                        this->textId = 0x3522; // Bring me the fastest dog!
                        Message_StartTextbox(play, this->textId, &this->actor);
                        this->actionFunc = EnAob01_BeforeRace_Talk;
                    }
                    break;

                case 1:
                    Audio_PlaySfx_MessageCancel();
                    this->textId = 0x3535; // Really?
                    Message_StartTextbox(play, this->textId, &this->actor);
                    break;
            }
        }
    } else if ((talkState == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
        play->msgCtx.stateTimer = 4;
        this->textId = 0;
        this->actionFunc = EnAob01_BeforeRace_Idle;
    }
}

/**
 * Handles various things that are common to all action functions, like tracking the
 * player, blinking, handling collision, etc.
 */
void EnAob01_UpdateCommon(EnAob01* this, PlayState* play) {
    static TrackOptionsSet sTrackOptions = {
        { 0xFA0, 4, 1, 3 },
        { 0x1770, 4, 1, 6 },
        { 0xFA0, 4, 1, 3 },
        { 0x1770, 4, 1, 6 },
    };
    Player* player = GET_PLAYER(play);
    Vec3f point;

    SkelAnime_Update(&this->skelAnime);

    if (SubS_AngleDiffLessEqual(this->actor.shape.rot.y, 0x36B0, this->actor.yawTowardsPlayer)) {
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

    EnAob01_Blink(this, EN_AOB01_EYE_MAX);
    SubS_UpdateFidgetTables(play, this->fidgetTableY, this->fidgetTableZ, MAMAMU_YAN_LIMB_MAX);
    EnAob01_UpdateCollision(this, play);

    // This specific code ensures that in-game time passes during the race.
    if (player->stateFlags1 & PLAYER_STATE1_20) {
        EnAob01_AdvanceTime();
    }
}

/**
 * Stand around and wait until either the player talks to the racetrack owner or until
 * the player bumps into them while holding a dog. In either case, the racetrack owner
 * will then start talking with the player.
 */
void EnAob01_BeforeRace_Idle(EnAob01* this, PlayState* play) {
    if (EnAob01_ProcessIdleAnim(this)) {
        if (EnAob01_PlayerIsHoldingDog(this, play) && !(this->stateFlags & ENAOB01_FLAG_PLAYER_CAN_TALK)) {
            if (this->collider.base.ocFlags2 & OC2_HIT_PLAYER) {
                this->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
                Actor_OfferTalk(&this->actor, play, 100.0f);
                this->stateFlags |= ENAOB01_FLAG_TALKING_TO_PLAYER_HOLDING_DOG;
                this->actionFunc = EnAob01_BeforeRace_Talk;
            }
        } else if (Actor_TalkOfferAccepted(&this->actor, &play->state) &&
                   (this->stateFlags & ENAOB01_FLAG_PLAYER_CAN_TALK)) {
            this->stateFlags &= ~ENAOB01_FLAG_PLAYER_CAN_TALK;
            this->prevTrackTarget = this->trackTarget;
            this->prevHeadRot = this->headRot;
            this->prevTorsoRot = this->torsoRot;
            EnAob01_BeforeRace_HandleConversation(this, play);
            this->actionFunc = EnAob01_BeforeRace_Talk;
        } else {
            this->stateFlags &= ~ENAOB01_FLAG_PLAYER_CAN_TALK;
            if ((this->actor.xzDistToPlayer < 100.0f) && !(this->collider.base.ocFlags2 & OC2_HIT_PLAYER)) {
                this->stateFlags |= ENAOB01_FLAG_PLAYER_CAN_TALK;
                Actor_OfferTalk(&this->actor, play, 100.0f);
            }
        }
    }
}

/**
 * Handles the actual action of talking with the player before the race begins.
 */
void EnAob01_BeforeRace_Talk(EnAob01* this, PlayState* play) {
    u8 talkState = Message_GetState(&play->msgCtx);

    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 4, 0xFA0, 1);

    //! @bug: This block of code acts as a failsafe for when the player triggered this conversation by
    //! bumping into the racetrack owner while holding the dog and, at the same time, threw or dropped
    //! the dog frame-perfectly by pressing A. In this case, the code successfully returns the racetrack
    //! owner to her idle state. Unfortunately, the player can also drop the dog by shielding. If this
    //! shield drop is done frame-perfectly, then the below code will not function properly and will
    //! instead softlock the game.
    if ((this->stateFlags & ENAOB01_FLAG_TALKING_TO_PLAYER_HOLDING_DOG) && !EnAob01_PlayerIsHoldingDog(this, play)) {
        if ((this->textId != 0) && (this->textId != 0x3535) && (this->textId != 0x3524) && (this->textId != 0x3548) &&
            (this->textId != 0x3549) && (this->textId != 0x354A)) {
            this->textId = 0x3523; // Hurry up and bring me a dog!
        }

        this->actor.textId = 0;
        this->stateFlags &= ~ENAOB01_FLAG_TALKING_TO_PLAYER_HOLDING_DOG;
        this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        this->actionFunc = EnAob01_BeforeRace_Idle;
        return;
    }

    if (this->stateFlags & ENAOB01_FLAG_LAUGH) {
        if (!EnAob01_ProcessLaughAnim(this)) {
            return;
        }
    } else if (!EnAob01_ProcessIdleAnim(this)) {
        return;
    }

    if (this->stateFlags & ENAOB01_FLAG_TALKING_TO_PLAYER_HOLDING_DOG) {
        if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
            this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
            Player_SetAutoLockOnActor(play, &this->actor);
            if (this->stateFlags & ENAOB01_FLAG_PLAYER_TOLD_TO_PICK_A_DOG) {
                EnAob01_BeforeRace_HandleConversation(this, play);
                this->stateFlags &= ~ENAOB01_FLAG_PLAYER_TOLD_TO_PICK_A_DOG;
            } else {
                this->stateFlags |= ENAOB01_FLAG_CONVERSATION_OVER;
                this->stateFlags |= ENAOB01_FLAG_LAUGH;
                this->animIndex = EN_AOB01_ANIM_LAUGH_START;
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, EN_AOB01_ANIM_LAUGH_START);
                // Put my dog down!
                Message_StartTextbox(play, 0x354B, &this->actor);
            }

            this->stateFlags &= ~ENAOB01_FLAG_TALKING_TO_PLAYER_HOLDING_DOG;
        }
    } else if (talkState == TEXT_STATE_CHOICE) {
        if (Message_ShouldAdvance(play)) {
            this->stateFlags &= ~ENAOB01_FLAG_LAUGH;
            switch (play->msgCtx.choiceIndex) {
                case 0:
                    Audio_PlaySfx_MessageDecide();
                    this->stateFlags |= ENAOB01_FLAG_PLAYER_CONFIRMED_CHOICE;
                    EnAob01_BeforeRace_HandleConversation(this, play);
                    break;

                case 1:
                    Audio_PlaySfx_MessageCancel();
                    EnAob01_BeforeRace_HandleConversation(this, play);
                    break;
            }
        }
    } else if (talkState == TEXT_STATE_EVENT) {
        if (Message_ShouldAdvance(play)) {
            this->stateFlags &= ~ENAOB01_FLAG_LAUGH;
            if (this->stateFlags & ENAOB01_FLAG_CONVERSATION_OVER) {
                this->stateFlags &= ~ENAOB01_FLAG_CONVERSATION_OVER;
                play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
                play->msgCtx.stateTimer = 4;
                this->actionFunc = EnAob01_BeforeRace_Idle;
            } else {
                EnAob01_BeforeRace_HandleConversation(this, play);
            }
        }
    } else if ((talkState == TEXT_STATE_INPUT_RUPEES) && Message_ShouldAdvance(play)) {
        this->stateFlags &= ~ENAOB01_FLAG_LAUGH;
        this->rupeesBet = play->msgCtx.rupeesSelected;
        EnAob01_BeforeRace_HandleConversation(this, play);
    }
}

/**
 * Checks all dog instances to see if any of them have reached a certain point along the racetrack,
 * as specified by the cutscene's custom value. Once one of them has reached that point, this function
 * will switch to following that dog and set the csId to correspond to the next cutscene in the race
 * sequence; this usually appears to the user as a cut to a different camera angle. Returns true if any
 * dog has reached the specified point, false otherwise.
 */
s32 EnAob01_Race_DogHasReachedNextCutsceneTrigger(EnAob01* this, PlayState* play) {
    Actor* npc = play->actorCtx.actorLists[ACTORCAT_NPC].first;

    while (npc != NULL) {
        if ((npc->id == ACTOR_EN_RACEDOG) &&
            (CutsceneManager_GetCutsceneCustomValue(this->csId) == ((EnRacedog*)npc)->currentPoint)) {
            CutsceneManager_Stop(this->csId);
            this->dogToFollow = npc;
            this->csId = CutsceneManager_GetAdditionalCsId(this->csId);
            return true;
        }
        npc = npc->next;
    }

    return false;
}

/**
 * Finds the dog that the player selected and starts following it. Returns true if it finds the player's
 * selected dog and false otherwise, though nothing actually cares about this return value, and it should
 * always be true in normal function (it is impossible to start a race without selecting a dog).
 */
s32 EnAob01_Race_FollowSelectedDog(EnAob01* this, PlayState* play) {
    Actor* npc = play->actorCtx.actorLists[ACTORCAT_NPC].first;

    while (npc != NULL) {
        if ((npc->id == ACTOR_EN_RACEDOG) && (((EnRacedog*)npc)->index == ((EnRacedog*)npc)->selectedDogIndex)) {
            this->dogToFollow = npc;
            return true;
        }
        npc = npc->next;
    }

    return false;
}

/**
 * Returns true if the race should be ended, which can happen in either of the following circumstances:
 * - All `RACEDOG_COUNT` dogs have finished the race.
 * - At least 10 dogs have finished the race, and at least 500 frames have passed since the 10th place
 *   dog finished. This is controlled by the forceRaceEndTimer, which is set in EnAob01_Init.
 */
s32 EnAob01_Race_IsRaceOver(EnAob01* this, PlayState* play) {
    Actor* npc = play->actorCtx.actorLists[ACTORCAT_NPC].first;
    s16 finishedCount = 0;

    while (npc != NULL) {
        if ((npc->id == ACTOR_EN_RACEDOG) && (((EnRacedog*)npc)->raceStatus == RACEDOG_RACE_STATUS_FINISHED)) {
            finishedCount++;
        }
        npc = npc->next;
    }

    if (finishedCount >= RACEDOG_COUNT) {
        return true;
    }

    if ((finishedCount >= 10) && (DECR(this->forceRaceEndTimer) == 0)) {
        return true;
    }

    return false;
}

/**
 * If the current cutscene has a positive length, and if the current cutscene is over (in other words,
 * if the length has elapsed), this function will set the csId to the next cutscene in the race sequence
 * and return true. In all other cases, it will return false. Note that *all* of the race cutscenes have
 * a length of -1, so in practice, this function never does anything and always returns false.
 */
s32 EnAob01_Race_CurrentCutsceneHasEnded(EnAob01* this) {
    if ((CutsceneManager_GetLength(this->csId) > 0) && (CutsceneManager_GetCurrentCsId() != this->csId)) {
        this->csId = CutsceneManager_GetAdditionalCsId(this->csId);
        return true;
    }
    return false;
}

/**
 * The only way we end up inside this function is if we tried to start a cutscene with a custom
 * value of 255. This does not correspond to a point on the racetrack, nor is it the special value
 * of 99, which is used to signal the race ending cutscene. Thus, whatever cutscene we're trying
 * to play here is almost certainly not a race cutscene, so this function just skips it and tries
 * to play the next cutscene.
 */
void EnAob01_Race_SkipNonRaceCutscene(EnAob01* this, PlayState* play) {
    CutsceneManager_Stop(this->csId);
    this->csId = CutsceneManager_GetAdditionalCsId(this->csId);
    this->actionFunc = EnAob01_Race_StartCutscene;
}

/**
 * Wait around doing nothing until the next cutscene is triggered (in practice, this only happens
 * when a dog reaches the next cutscene trigger), at which point we queue up the next cutscene.
 */
void EnAob01_Race_HandleRaceCutscene(EnAob01* this, PlayState* play) {
    if (EnAob01_Race_DogHasReachedNextCutsceneTrigger(this, play) || EnAob01_Race_CurrentCutsceneHasEnded(this)) {
        CutsceneManager_Queue(this->csId);
        this->actionFunc = EnAob01_Race_StartCutscene;
    }
}

/**
 * Waits until the race is over, then reloads the racetrack to return it
 * to its regular playable state.
 */
void EnAob01_Race_HandleRaceEndCutscene(EnAob01* this, PlayState* play) {
    this->stateFlags |= ENAOB01_FLAG_STARTED_RACE;
    if (EnAob01_Race_IsRaceOver(this, play)) {
        if (AudioSeq_GetActiveSeqId(SEQ_PLAYER_BGM_MAIN) != NA_BGM_HORSE_GOAL) {
            play->nextEntrance = ENTRANCE(DOGGY_RACETRACK, 1);
            SET_EVENTINF_DOG_RACE_STATE(EVENTINF_DOG_RACE_STATE_ENDED);
            play->transitionType = TRANS_TYPE_64;
            gSaveContext.nextTransitionType = TRANS_TYPE_FADE_WHITE;
            play->transitionTrigger = TRANS_TRIGGER_START;
        }
    }
}

/**
 * Starts the cutscene specified by the csId and switches to an appropriate
 * action function based on the cutscene's custom value.
 */
void EnAob01_Race_StartCutscene(EnAob01* this, PlayState* play) {
    if (CutsceneManager_IsNext(this->csId)) {
        CutsceneManager_Start(this->csId, this->dogToFollow);
        switch (CutsceneManager_GetCutsceneCustomValue(this->csId)) {
            case 255:
                this->actionFunc = EnAob01_Race_SkipNonRaceCutscene;
                break;

            case 99:
                this->actionFunc = EnAob01_Race_HandleRaceEndCutscene;
                break;

            default:
                this->actionFunc = EnAob01_Race_HandleRaceCutscene;
        }
    } else {
        CutsceneManager_Queue(this->csId);
    }
}

/**
 * Tells the player how their selected dog did in the race and gives the player
 * rupees depending on their performance. If their dog placed 1st, they will
 * receive triple the rupees they bet. If their dog places 2nd, they will receive
 * double the rupees they bet. If their dog places 3rd through 5th, they will
 * receive the same number of rupees they bet.
 */
void EnAob01_AfterRace_GiveRaceResult(EnAob01* this, PlayState* play) {
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        Player_SetAutoLockOnActor(play, &this->actor);
        this->rupeesBet = gSaveContext.unk_3F5C;
        switch (GET_EVENTINF_DOG_RACE_RACE_STANDING) {
            case 1:
                this->textId = 0x352A; // You took 1st place!
                this->stateFlags |= ENAOB01_FLAG_SURPRISE;
                this->rupeesBet *= 3;
                Rupees_ChangeBy(this->rupeesBet);
                play->msgCtx.rupeesTotal = this->rupeesBet;
                break;

            case 2:
                this->textId = 0x352B; // You took 2nd place!
                this->stateFlags |= ENAOB01_FLAG_SURPRISE;
                this->rupeesBet *= 2;
                Rupees_ChangeBy(this->rupeesBet);
                play->msgCtx.rupeesTotal = this->rupeesBet;
                break;

            case 3:
            case 4:
            case 5:
                this->textId = 0x352C; // I'll give you back what you bet.
                Rupees_ChangeBy(this->rupeesBet);
                break;

            default:
                this->textId = 0x352D; // Bad choice!
                this->stateFlags |= ENAOB01_FLAG_LAUGH;
                this->animIndex = EN_AOB01_ANIM_LAUGH_START;
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, EN_AOB01_ANIM_LAUGH_START);
                break;
        }

        Message_StartTextbox(play, this->textId, &this->actor);
        this->actionFunc = EnAob01_AfterRace_Talk;
    } else if (this->actor.xzDistToPlayer < 100.0f) {
        Actor_OfferTalk(&this->actor, play, 100.0f);
    }
}

/**
 * Gives the player the reward for winning 150 or more rupees from the dog race. The first
 * time this happens, the player will receive a Heart Piece; any other time, they will
 * instead receive a Red Rupee.
 */
void EnAob01_AfterRace_GiveReward(EnAob01* this, PlayState* play) {
    u8 talkState = Message_GetState(&play->msgCtx);

    if (EnAob01_ProcessIdleAnim(this)) {
        if ((talkState == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
            this->rupeesBet = 0;
            play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
            play->msgCtx.stateTimer = 4;
        }

        if (Actor_HasParent(&this->actor, play)) {
            this->trackTarget = this->prevTrackTarget;
            this->headRot = this->prevHeadRot;
            this->torsoRot = this->prevTorsoRot;
            this->actor.parent = NULL;
            this->actor.shape.rot.y = this->actor.world.rot.y;
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_DOGGY_RACETRACK_HEART_PIECE)) {
                this->actionFunc = EnAob01_AfterRace_AfterGivingReward;
            } else {
                SET_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_DOGGY_RACETRACK_HEART_PIECE);
                this->actionFunc = EnAob01_AfterRace_AfterGivingReward;
            }
        } else if (CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_DOGGY_RACETRACK_HEART_PIECE)) {
            Actor_OfferGetItem(&this->actor, play, GI_RUPEE_RED, 300.0f, 300.0f);
        } else {
            Actor_OfferGetItem(&this->actor, play, GI_HEART_PIECE, 300.0f, 300.0f);
        }
    }
}

/**
 * If the current time is after the racetrack closes, kick the player out.
 * Otherwise, ask the player if they want to play again. This function is
 * only used when the player has won the 150 rupee reward.
 */
void EnAob01_AfterRace_AfterGivingReward(EnAob01* this, PlayState* play) {
    u8 talkState = Message_GetState(&play->msgCtx);

    if (((talkState == TEXT_STATE_EVENT) || (talkState == TEXT_STATE_DONE)) && Message_ShouldAdvance(play)) {
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_KICKOUT_TIME_PASSED)) {
            CLEAR_WEEKEVENTREG(WEEKEVENTREG_KICKOUT_TIME_PASSED);
        }

        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_KICKOUT_WAIT)) {
            CLEAR_WEEKEVENTREG(WEEKEVENTREG_KICKOUT_WAIT);
        }

        this->textId = 0;
        Actor_OfferTalkExchangeEquiCylinder(&this->actor, play, 400.0f, PLAYER_IA_MINUS1);
        SET_EVENTINF_DOG_RACE_STATE(EVENTINF_DOG_RACE_STATE_NOT_STARTED);
        this->actionFunc = EnAob01_AfterRace_AskToPlayAgain;
    }
}

/**
 * Ask if the player wants to play again. This function is only used when
 * the player has won the 150 rupee reward.
 */
void EnAob01_AfterRace_AskToPlayAgain(EnAob01* this, PlayState* play) {
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->textId = 0x354C; // Want to play again?
        Message_ContinueTextbox(play, this->textId);
        this->actionFunc = EnAob01_BeforeRace_RespondToPlayAgainQuestion;
    } else {
        Actor_OfferTalkExchangeEquiCylinder(&this->actor, play, 400.0f, PLAYER_IA_MINUS1);
    }
}

/**
 * Handles the conversation the racetrack owner has with the player after the race
 * is over. If the player won 150 or more rupees, this function will transition to
 * the action function that gives them the reward. If the player did *not* win 150
 * or more rupees, then if the current time is after the racetrack closes, this
 * kicks the player out. Otherwise, this asks the player if they want to play again.
 * Much of the logic here is duplicated in the above two functions.
 */
void EnAob01_AfterRace_Talk(EnAob01* this, PlayState* play) {
    u8 talkState = Message_GetState(&play->msgCtx);

    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 4, 0xFA0, 1);

    if (this->stateFlags & ENAOB01_FLAG_LAUGH) {
        if (!EnAob01_ProcessLaughAnim(this)) {
            return;
        }
    } else if (this->stateFlags & ENAOB01_FLAG_SURPRISE) {
        if (!EnAob01_ProcessSurpriseAnim(this)) {
            return;
        }
    }

    if ((talkState == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        this->stateFlags &= ~ENAOB01_FLAG_LAUGH;
        this->stateFlags &= ~ENAOB01_FLAG_SURPRISE;
        if (this->rupeesBet >= 150) {
            this->textId = 0x352E; // You won over 150 rupees!
            Message_StartTextbox(play, this->textId, &this->actor);
            this->actionFunc = EnAob01_AfterRace_GiveReward;
        } else {
            this->trackTarget = this->prevTrackTarget;
            this->headRot = this->prevHeadRot;
            this->torsoRot = this->prevTorsoRot;

            this->rupeesBet = 0;
            this->actor.shape.rot.y = this->actor.world.rot.y;
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_KICKOUT_TIME_PASSED)) {
                CLEAR_WEEKEVENTREG(WEEKEVENTREG_KICKOUT_TIME_PASSED);
            }

            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_KICKOUT_WAIT)) {
                CLEAR_WEEKEVENTREG(WEEKEVENTREG_KICKOUT_WAIT);
            }

            this->textId = 0x354C; // Want to play again?
            Message_StartTextbox(play, this->textId, &this->actor);
            this->actionFunc = EnAob01_BeforeRace_RespondToPlayAgainQuestion;
            SET_EVENTINF_DOG_RACE_STATE(EVENTINF_DOG_RACE_STATE_NOT_STARTED);
        }
    }
}

s32 EnAob01_PlayerIsHoldingDog(EnAob01* this, PlayState* play) {
    Actor* dog = play->actorCtx.actorLists[ACTORCAT_ENEMY].first;

    while (dog != NULL) {
        if (dog->id == ACTOR_EN_DG) {
            this->selectedDogIndex = ((EnDg*)dog)->selectedDogIndex;
            if (this->selectedDogIndex == -1) {
                return false;
            }

            if (this->selectedDogIndex == ENDG_GET_INDEX(dog)) {
                return true;
            }
        }

        dog = dog->next;
    }

    return false;
}

/**
 * Moves the player and racetrack owner to the center of the racetrack
 * (out of the way of the dogs) and stops drawing the player.
 */
void EnAob01_Race_HidePlayer(EnAob01* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    player->actor.world.pos.x = -4134.0f;
    player->actor.world.pos.z = 1464.0f;
    player->actor.shape.rot.y = player->actor.world.rot.y;
    player->actor.draw = NULL;
    player->stateFlags1 |= PLAYER_STATE1_20;
    this->actor.world.pos.x = -4308.0f;
    this->actor.world.pos.z = 1620.0f;
    this->actor.prevPos = this->actor.world.pos;
}

/**
 * Generates a randomized list of dog text offsets and stores them in weekEventRegs.
 * EnDg uses these text offsets to determine what text to show when the player picks
 * the dog up while equipped with the Mask of Truth, and EnRacedog uses them to
 * determine what condition the dog is in.
 */
void EnAob01_InitializeDogTextOffsets(void) {
    u8 i;
    u8 rand;
    u8 dogTextOffsetsTemp;
    u8 packedDogTextOffsets[(RACEDOG_COUNT + 1) / 2];
    u8 dogTextOffsets[RACEDOG_COUNT] = {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
    };

    // Swap around the dog text offsets randomly 14 times.
    for (i = 0; i < RACEDOG_COUNT; i++) {
        rand = Rand_ZeroFloat(RACEDOG_COUNT);

        dogTextOffsetsTemp = dogTextOffsets[i];
        dogTextOffsets[i] = dogTextOffsets[rand];
        dogTextOffsets[rand] = dogTextOffsetsTemp;
    }

    // Clear out all weekEventRegs related to the dog text offsets.
    for (i = 0; i < ARRAY_COUNT(packedDogTextOffsets); i++) {
        CLEAR_WEEKEVENTREG_DOG_RACE_TEXT_OFFSETS(i);
        packedDogTextOffsets[i] = 0;
    }

    // Go through all the dog text offsets and pack them into weekEventRegs.
    for (i = 0; i < RACEDOG_COUNT; i++) {
        u8 dogTextOffsetsTemp = dogTextOffsets[i];
        u8 j = i / 2;

        if (i % 2) {
            //! @note sets text offsets for both even and odd entry.
            //! Requires `RACEDOG_COUNT` to be even to work properly.
            packedDogTextOffsets[j] |= dogTextOffsetsTemp << 4;
            SET_WEEKEVENTREG_DOG_RACE_TEXT_OFFSETS(j, packedDogTextOffsets[j]);
        } else {
            packedDogTextOffsets[j] |= dogTextOffsetsTemp;
        }
    }
}

void EnAob01_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnAob01* this = (EnAob01*)thisx;

    ActorShape_Init(&this->actor.shape, 0.0f, NULL, 0.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &gMamamuYanSkel, NULL, this->jointTable, this->morphTable,
                       MAMAMU_YAN_LIMB_MAX);
    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    this->animIndex = EN_AOB01_ANIM_IDLE;
    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, EN_AOB01_ANIM_IDLE);
    Actor_SetScale(&this->actor, 0.01f);

    switch (GET_EVENTINF_DOG_RACE_STATE) {
        case EVENTINF_DOG_RACE_STATE_NOT_STARTED:
            EnAob01_InitializeDogTextOffsets();
            EnAob01_SpawnDogs(this, play);
            this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
            this->actionFunc = EnAob01_BeforeRace_Idle;
            break;

        case EVENTINF_DOG_RACE_STATE_STARTED:
            this->forceRaceEndTimer = 500;
            EnAob01_SpawnRacedogs(this, play);
            this->actor.draw = NULL;
            this->csId = this->actor.csId;
            EnAob01_Race_FollowSelectedDog(this, play);
            CutsceneManager_Queue(this->csId);
            this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
            EnAob01_Race_HidePlayer(this, play);
            this->actionFunc = EnAob01_Race_StartCutscene;
            break;

        case EVENTINF_DOG_RACE_STATE_ENDED:
            EnAob01_InitializeDogTextOffsets();
            EnAob01_SpawnDogs(this, play);
            this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
            this->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
            this->actionFunc = EnAob01_AfterRace_GiveRaceResult;
            break;
    }
}

void EnAob01_Destroy(Actor* thisx, PlayState* play) {
    EnAob01* this = (EnAob01*)thisx;

    if (!(this->stateFlags & ENAOB01_FLAG_STARTED_RACE)) {
        CLEAR_WEEKEVENTREG(WEEKEVENTREG_KICKOUT_WAIT);
    }

    Collider_DestroyCylinder(play, &this->collider);
}

void EnAob01_Update(Actor* thisx, PlayState* play) {
    EnAob01* this = (EnAob01*)thisx;

    this->actionFunc(this, play);
    EnAob01_UpdateCommon(this, play);
}

s32 EnAob01_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnAob01* this = (EnAob01*)thisx;
    TexturePtr eyeTextures[] = {
        gMamamuYanEyeOpenTex,
        gMamamuYanEyeHalfTex,
        gMamamuYanEyeClosedTex,
    };

    if (limbIndex == MAMAMU_YAN_LIMB_HEAD) {
        OPEN_DISPS(play->state.gfxCtx);

        *dList = gMamamuYanHeadDL;

        gSPSegment(POLY_OPA_DISP++, 0x0A, Lib_SegmentedToVirtual(eyeTextures[this->eyeIndex]));

        CLOSE_DISPS(play->state.gfxCtx);
    }

    if (limbIndex == MAMAMU_YAN_LIMB_HEAD) {
        Matrix_Translate(1500.0f, 0.0f, 0.0f, MTXMODE_APPLY);
        Matrix_RotateXS(this->headRot.y, MTXMODE_APPLY);
        Matrix_RotateZS(this->headRot.x * -1, MTXMODE_APPLY);
        Matrix_Translate(-1500.0f, 0.0f, 0.0f, MTXMODE_APPLY);
    }

    if (limbIndex == MAMAMU_YAN_LIMB_TORSO) {
        Matrix_RotateXS(this->torsoRot.y * -1, MTXMODE_APPLY);
        Matrix_RotateZS(this->torsoRot.x * -1, MTXMODE_APPLY);
    }

    if ((limbIndex == MAMAMU_YAN_LIMB_TORSO) || (limbIndex == MAMAMU_YAN_LIMB_LEFT_UPPER_ARM) ||
        (limbIndex == MAMAMU_YAN_LIMB_RIGHT_UPPER_ARM)) {
        rot->y += TRUNCF_BINANG(Math_SinS(this->fidgetTableY[limbIndex])) * 200;
        rot->z += TRUNCF_BINANG(Math_CosS(this->fidgetTableZ[limbIndex])) * 200;
    }

    return false;
}

void EnAob01_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    static Vec3f sFocusOffset = { 0.0f, 0.0f, 0.0f };
    EnAob01* this = (EnAob01*)thisx;

    if (limbIndex == MAMAMU_YAN_LIMB_HEAD) {
        Matrix_MultVec3f(&sFocusOffset, &this->actor.focus.pos);
    }
}

void EnAob01_TransformLimbDraw(PlayState* play, s32 limbIndex, Actor* thisx) {
}

void EnAob01_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    EnAob01* this = (EnAob01*)thisx;
    Vec3f pos;
    Vec3f scale;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, Gfx_EnvColor(play->state.gfxCtx, 50, 80, 0, 0));
    gSPSegment(POLY_OPA_DISP++, 0x09, Gfx_EnvColor(play->state.gfxCtx, 50, 80, 0, 0));
    gDPPipeSync(POLY_OPA_DISP++);

    SkelAnime_DrawTransformFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                   this->skelAnime.dListCount, EnAob01_OverrideLimbDraw, EnAob01_PostLimbDraw,
                                   EnAob01_TransformLimbDraw, &this->actor);

    if (this->actor.draw != NULL) {
        Gfx_SetupDL25_Xlu(play->state.gfxCtx);
        pos = this->actor.world.pos;
        scale.x = 0.5f;
        scale.y = 0.5f;
        scale.z = 0.5f;
        func_800BC620(&pos, &scale, 255, play);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}
