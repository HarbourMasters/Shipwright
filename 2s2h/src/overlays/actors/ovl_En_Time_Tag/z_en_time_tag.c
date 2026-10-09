/*
 * File: z_en_time_tag.c
 * Overlay: ovl_En_Time_Tag
 * Description: Various text-, time- and event-based triggers. There are 4 variations of this actor.
 *  - Song of Soaring engraving
 *  - Clocktown Rooftop Oath to Order event
 *  - Mikau's and Lulu's diary
 *  - Kick out of scene event after a certain time
 */

#include "z_en_time_tag.h"
#include "overlays/actors/ovl_En_Elf/z_en_elf.h"
#include "2s2h/GameInteractor/GameInteractor.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void EnTimeTag_Init(Actor* thisx, PlayState* play);
void EnTimeTag_Destroy(Actor* thisx, PlayState* play);
void EnTimeTag_Update(Actor* thisx, PlayState* play);

void EnTimeTag_RooftopOath_DoNothing(EnTimeTag* this, PlayState* play);
void EnTimeTag_RooftopOath_Cutscene(EnTimeTag* this, PlayState* play);
void EnTimeTag_RooftopOath_Wait(EnTimeTag* this, PlayState* play);

void EnTimeTag_SoaringEngraving_GetSong(EnTimeTag* this, PlayState* play);
void EnTimeTag_SoaringEngraving_StartCutscene(EnTimeTag* this, PlayState* play);
void EnTimeTag_SoaringEngraving_Wait(EnTimeTag* this, PlayState* play);

void EnTimeTag_Diary_AfterOcarina(EnTimeTag* this, PlayState* play);
void EnTimeTag_Diary_TeachEvanSongSnippets(EnTimeTag* this, PlayState* play);
void EnTimeTag_Diary_Cutscene(EnTimeTag* this, PlayState* play);
void EnTimeTag_Diary_Wait(EnTimeTag* this, PlayState* play);

void EnTimeTag_KickOut_DoNothing(EnTimeTag* this, PlayState* play);
void EnTimeTag_KickOut_Transition(EnTimeTag* this, PlayState* play);
void EnTimeTag_KickOut_WaitForTrigger(EnTimeTag* this, PlayState* play);
void EnTimeTag_KickOut_WaitForTime(EnTimeTag* this, PlayState* play);

ActorProfile En_Time_Tag_Profile = {
    /**/ ACTOR_EN_TIME_TAG,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(EnTimeTag),
    /**/ EnTimeTag_Init,
    /**/ EnTimeTag_Destroy,
    /**/ EnTimeTag_Update,
    /**/ NULL,
};

void EnTimeTag_Init(Actor* thisx, PlayState* play) {
    EnTimeTag* this = (EnTimeTag*)thisx;

    this->actionFunc = EnTimeTag_KickOut_WaitForTime;

    switch (TIMETAG_GET_TYPE(&this->actor)) {
        case TIMETAG_KICKOUT_FINAL_HOURS:
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_CLOCK_TOWER_OPENED) || (CURRENT_DAY != 3)) {
                Actor_Kill(&this->actor);
                return;
            }
            // Overwrite kickout time to midnight
            TIMETAG_KICKOUT_HOUR(&this->actor) = 0;
            TIMETAG_KICKOUT_MINUTE(&this->actor) = 0;
            break;

        case TIMETAG_ROOFTOP_OATH:
            this->actionFunc = EnTimeTag_RooftopOath_Wait;
            this->actor.flags |= ACTOR_FLAG_UPDATE_DURING_OCARINA;
            break;

        case TIMETAG_SOARING_ENGRAVING:
            this->actionFunc = EnTimeTag_SoaringEngraving_Wait;
            this->actor.flags |= ACTOR_FLAG_UPDATE_DURING_OCARINA;
            if (CHECK_QUEST_ITEM(QUEST_SONG_SOARING)) {
                this->actor.textId = 0xC02;
            } else {
                this->actor.textId = 0;
            }
            break;

        case TIMETAG_DIARY:
            this->actionFunc = EnTimeTag_Diary_Wait;
            this->actor.textId = 0;
            TIMETAG_DIARY_TIMER(&this->actor) = 0;
            break;

        default:
            break;
    }
}

void EnTimeTag_Destroy(Actor* thisx, PlayState* play) {
}

void EnTimeTag_RooftopOath_DoNothing(EnTimeTag* this, PlayState* play) {
}

void EnTimeTag_RooftopOath_Cutscene(EnTimeTag* this, PlayState* play) {
    if (CutsceneManager_IsNext(this->actor.csId)) {
        CutsceneManager_StartWithPlayerCs(this->actor.csId, &this->actor);

        this->actionFunc = EnTimeTag_RooftopOath_DoNothing;
        gSaveContext.timerStates[TIMER_ID_MOON_CRASH] = TIMER_STATE_OFF;

        if (GameInteractor_Should(VB_MEET_MOON_REQUIREMENTS, CHECK_QUEST_ITEM(QUEST_REMAINS_ODOLWA) &&
                                                                 CHECK_QUEST_ITEM(QUEST_REMAINS_GOHT) &&
                                                                 CHECK_QUEST_ITEM(QUEST_REMAINS_GYORG) &&
                                                                 CHECK_QUEST_ITEM(QUEST_REMAINS_TWINMOLD))) {
            SET_WEEKEVENTREG(WEEKEVENTREG_25_02);
        }
    } else {
        CutsceneManager_Queue(this->actor.csId);
    }
}

void EnTimeTag_RooftopOath_Wait(EnTimeTag* this, PlayState* play) {
    Actor* thisx = &this->actor;

    if ((play->msgCtx.ocarinaMode == OCARINA_MODE_EVENT) && (play->msgCtx.lastPlayedSong == OCARINA_SONG_OATH)) {
        if (this->actor.csId != CS_ID_NONE) {
            this->actionFunc = EnTimeTag_RooftopOath_Cutscene;
            CutsceneManager_Queue(thisx->csId);
            gSaveContext.timerStates[TIMER_ID_MOON_CRASH] = TIMER_STATE_OFF;
        }
        play->msgCtx.ocarinaMode = OCARINA_MODE_END;
    }
}

void EnTimeTag_SoaringEngraving_GetSong(EnTimeTag* this, PlayState* play) {
    if (CutsceneManager_GetCurrentCsId() != this->actor.csId) {
        this->actionFunc = EnTimeTag_SoaringEngraving_Wait;
        this->actor.textId = 0xC02;
        Item_Give(play, ITEM_SONG_SOARING);
    }
}

void EnTimeTag_SoaringEngraving_StartCutscene(EnTimeTag* this, PlayState* play) {
    if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
        CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
        CutsceneManager_Queue(this->actor.csId);
    } else if (CutsceneManager_IsNext(this->actor.csId)) {
        CutsceneManager_Start(this->actor.csId, &this->actor);
        this->actionFunc = EnTimeTag_SoaringEngraving_GetSong;
    } else {
        CutsceneManager_Queue(this->actor.csId);
    }
}

void EnTimeTag_SoaringEngraving_SubsequentInteraction(EnTimeTag* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        Message_CloseTextbox(play);
        this->actionFunc = EnTimeTag_SoaringEngraving_Wait;
    }
}

void EnTimeTag_SoaringEngraving_Wait(EnTimeTag* this, PlayState* play) {
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        if (this->actor.textId == 0) {
            this->actionFunc = EnTimeTag_SoaringEngraving_StartCutscene;
        } else {
            this->actionFunc = EnTimeTag_SoaringEngraving_SubsequentInteraction;
        }
    } else if ((this->actor.xzDistToPlayer < 100.0f) && Player_IsFacingActor(&this->actor, 0x3000, play) &&
               (Flags_GetSwitch(play, TIMETAG_SOARING_GET_SWITCH_FLAG(&this->actor)) ||
                CHECK_QUEST_ITEM(QUEST_SONG_SOARING))) {
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
        Actor_OfferTalk(&this->actor, play, 110.0f);
    }
}

void EnTimeTag_Diary_AfterOcarina(EnTimeTag* this, PlayState* play) {
    if (TIMETAG_DIARY_TIMER(&this->actor) > 0) {
        TIMETAG_DIARY_TIMER(&this->actor)--;
        return;
    }

    if (TIMETAG_DIARY_SONG(&this->actor) != TIMETAG_DIARY_SONG_EVAN_PART1) {
        // TIMETAG_DIARY_SONG_EVAN_PART2
        Message_ContinueTextbox(play, 0x1230);
    } else {
        // TIMETAG_DIARY_SONG_EVAN_PART1
        Message_ContinueTextbox(play, 0x122D);
    }

    this->actionFunc = EnTimeTag_Diary_Cutscene;
}

void EnTimeTag_Diary_TeachEvanSongSnippets(EnTimeTag* this, PlayState* play) {
    if ((play->msgCtx.ocarinaStaff->state == 0) && (play->msgCtx.msgMode == MSGMODE_SONG_DEMONSTRATION_DONE)) {
        TIMETAG_DIARY_TIMER(&this->actor) = 5;
        this->actionFunc = EnTimeTag_Diary_AfterOcarina;
        play->msgCtx.msgLength = 0;
        play->msgCtx.msgMode = MSGMODE_NONE;
    }
}

void EnTimeTag_Diary_Cutscene(EnTimeTag* this, PlayState* play) {
    switch (Message_GetState(&play->msgCtx)) {
        case TEXT_STATE_EVENT:
            if (Message_ShouldAdvance(play)) {
                switch (play->msgCtx.currentTextId) {
                    case 0x101C: // Lulu diary part 1
                    case 0x101D: // Lulu diary part 2
                    case 0x101E: // Lulu diary part 3
                    case 0x122D: // Mikau diary part 2
                        Message_ContinueTextbox(play, play->msgCtx.currentTextId + 1);
                        break;

                    case 0x101F: // Lulu diary part 4
                    case 0x122A: // Can not read Zora script
                    case 0x1230: // Mikau diary part 4
                        Message_CloseTextbox(play);
                        this->actionFunc = EnTimeTag_Diary_Wait;
                        if (CutsceneManager_GetCurrentCsId() == this->actor.csId) {
                            CutsceneManager_Stop(this->actor.csId);
                        }
                        break;

                    case 0x122B: // Mikau diary part 1
                        Message_DisplayOcarinaStaff(play, OCARINA_ACTION_DEMONSTRATE_EVAN_PART1_SECOND_HALF);
                        this->actionFunc = EnTimeTag_Diary_TeachEvanSongSnippets;
                        TIMETAG_DIARY_SONG(&this->actor) = TIMETAG_DIARY_SONG_EVAN_PART1;
                        break;

                    case 0x122E: // Mikau diary part 3
                        Message_DisplayOcarinaStaff(play, OCARINA_ACTION_DEMONSTRATE_EVAN_PART2_SECOND_HALF);
                        this->actionFunc = EnTimeTag_Diary_TeachEvanSongSnippets;
                        TIMETAG_DIARY_SONG(&this->actor) = TIMETAG_DIARY_SONG_EVAN_PART2;
                        break;

                    default:
                        break;
                }
            }
            break;

        case TEXT_STATE_CLOSING:
            this->actionFunc = EnTimeTag_Diary_Wait;
            break;
    }

    if (TIMETAG_DIARY_TIMER(&this->actor) != 0) {
        if (this->actor.csId == CS_ID_NONE) {
            TIMETAG_DIARY_TIMER(&this->actor) = 0;
        } else if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
            CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
            CutsceneManager_Queue(this->actor.csId);
        } else if (CutsceneManager_IsNext(this->actor.csId)) {
            CutsceneManager_Start(this->actor.csId, &this->actor);
            TIMETAG_DIARY_TIMER(&this->actor) = 0;
        } else {
            CutsceneManager_Queue(this->actor.csId);
        }
    }
}

void EnTimeTag_Diary_Wait(EnTimeTag* this, PlayState* play) {
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        if (GET_PLAYER_FORM == PLAYER_FORM_ZORA) {
            if (TIMETAG_DIARY_GET_TYPE(&this->actor) == TIMETAG_DIARY_LULU) {
                Message_StartTextbox(play, 0x101C, &this->actor);
            } else {
                // TIMETAG_DIARY_MIKAU
                Message_StartTextbox(play, 0x122B, &this->actor);
            }
            TIMETAG_DIARY_TIMER(&this->actor) = 1;

            //! FAKE:
            if (1) {}
        } else {
            // unable to read Zora script
            Message_StartTextbox(play, 0x122A, &this->actor);

            ((EnElf*)GET_PLAYER(play)->tatlActor)->unk_264 |= 4;
            Actor_ChangeFocus(&this->actor, play, GET_PLAYER(play)->tatlActor);
            TIMETAG_DIARY_TIMER(&this->actor) = 0;
        }
        this->actionFunc = EnTimeTag_Diary_Cutscene;
    } else if ((this->actor.xzDistToPlayer < 100.0f) && Player_IsFacingActor(&this->actor, 0x3000, play)) {
        Actor_OfferTalk(&this->actor, play, 110.0f);
    }
}

void EnTimeTag_KickOut_DoNothing(EnTimeTag* this, PlayState* play) {
}

void EnTimeTag_KickOut_Transition(EnTimeTag* this, PlayState* play) {
    if (Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) {
        play->nextEntrance = play->setupExitList[TIMETAG_KICKOUT_GET_EXIT_INDEX(&this->actor)];
        play->transitionTrigger = TRANS_TRIGGER_START;
        if (TIMETAG_GET_TYPE(&this->actor) == TIMETAG_KICKOUT_DOOR) {
            Actor_PlaySfx(&this->actor, NA_SE_OC_DOOR_OPEN);
        }
        this->actionFunc = EnTimeTag_KickOut_DoNothing;
    }
}

/**
 * Setup a request to kickout, but wait for an external system to unset
 * both `WEEKEVENTREG_KICKOUT_WAIT` and `WEEKEVENTREG_KICKOUT_TIME_PASSED`
 */
void EnTimeTag_KickOut_WaitForTrigger(EnTimeTag* this, PlayState* play) {
    if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_KICKOUT_WAIT) && !CHECK_WEEKEVENTREG(WEEKEVENTREG_KICKOUT_TIME_PASSED)) {
        Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_WAIT);
        Message_StartTextbox(play, 0x1883 + TIMETAG_KICKOUT_GET_TEXT(&this->actor), NULL);
        this->actionFunc = EnTimeTag_KickOut_Transition;
    }
}

/**
 * Wait for a certain time to pass, then trigger the kickout event.
 * If an external system sets `WEEKEVENTREG_KICKOUT_WAIT`, then instead of triggering the kickout event now,
 * store the kickout by going to `EnTimeTag_KickOut_WaitForTrigger`.
 * If the time has passed but the kickout is being stored, then `WEEKEVENTREG_KICKOUT_TIME_PASSED` is set here
 * to indicate to external systems that the time has passed and is waiting for a trigger.
 */
void EnTimeTag_KickOut_WaitForTime(EnTimeTag* this, PlayState* play) {
    s16 hour;
    s16 minute;

    if ((play->sceneId == SCENE_YADOYA) &&
        GameInteractor_Should(VB_CHECK_FOR_ROOM_KEY, INV_CONTENT(ITEM_ROOM_KEY) == ITEM_ROOM_KEY)) {
        // Having the room key allows you to stay in Stock Pot Inn
        return;
    }

    hour = TIME_TO_HOURS_F(CURRENT_TIME);
    minute = (s32)TIME_TO_MINUTES_F(CURRENT_TIME) % 60;

    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_KICKOUT_WAIT)) {
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_KICKOUT_TIME_PASSED)) {
            this->actionFunc = EnTimeTag_KickOut_WaitForTrigger;
        } else if ((hour == TIMETAG_KICKOUT_HOUR(&this->actor)) && (minute == TIMETAG_KICKOUT_MINUTE(&this->actor))) {
            SET_WEEKEVENTREG(WEEKEVENTREG_KICKOUT_TIME_PASSED);
        }
    } else if ((hour == TIMETAG_KICKOUT_HOUR(&this->actor)) && (minute == TIMETAG_KICKOUT_MINUTE(&this->actor)) &&
               !Play_InCsMode(play)) {
        Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_WAIT);
        Message_StartTextbox(play, 0x1883 + TIMETAG_KICKOUT_GET_TEXT(&this->actor), NULL);
        this->actionFunc = EnTimeTag_KickOut_Transition;
    }
}

void EnTimeTag_Update(Actor* thisx, PlayState* play) {
    EnTimeTag* this = (EnTimeTag*)thisx;

    this->actionFunc(this, play);
}
