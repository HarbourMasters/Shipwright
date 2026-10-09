#include "global.h"

#include "2s2h/GameInteractor/GameInteractor.h"

/**
 * Branch forward if the provided weekEventReg flag is set
 *
 * Command structure:
 *  0:(u8)  cmd
 *  1:(u16) flag
 *  3:(s16) skip
 * Command size: 5
 */
s32 MsgEvent_CheckWeekEventReg(Actor* actor, PlayState* play, u8** script, MsgScriptCallback callback, s32* endScript) {
    MsgScriptCmdCheckWeekEventReg* cmd = (MsgScriptCmdCheckWeekEventReg*)*script;
    u16 flag = SCRIPT_PACK_16(cmd->flagByte, cmd->flagMask);
    s16 skip = SCRIPT_PACK_16(cmd->offsetH, cmd->offsetL);

    if (CHECK_WEEKEVENTREG(flag)) {
        *script += skip;
    }
    return false;
}

/**
 * Branch forward if the player is currently in goron form
 *
 * Command structure:
 *  0:(u8)  cmd
 *  1:(s16) skip
 * Command size: 3
 */
s32 MsgEvent_CheckGoron(Actor* actor, PlayState* play, u8** script, MsgScriptCallback callback, s32* endScript) {
    Player* player = GET_PLAYER(play);
    MsgScriptCmdCheckGoron* cmd = (MsgScriptCmdCheckGoron*)*script;
    s16 skip = SCRIPT_PACK_16(cmd->offsetH, cmd->offsetL);

    if (player->transformation == PLAYER_FORM_GORON) {
        *script += skip;
    }
    return false;
}

/**
 * Branch forward if the player is currently in zora form
 *
 * Command structure:
 *  0:(u8)  cmd
 *  1:(s16) skip
 * Command size: 3
 */
s32 MsgEvent_CheckZora(Actor* actor, PlayState* play, u8** script, MsgScriptCallback callback, s32* endScript) {
    Player* player = GET_PLAYER(play);
    MsgScriptCmdCheckZora* cmd = (MsgScriptCmdCheckZora*)*script;
    s16 skip = SCRIPT_PACK_16(cmd->offsetH, cmd->offsetL);

    if (player->transformation == PLAYER_FORM_ZORA) {
        *script += skip;
    }
    return false;
}

/**
 * Branch forward if the player is currently in deku form
 *
 * Command structure:
 *  0:(u8)  cmd
 *  1:(s16) skip
 * Command size: 3
 */
s32 MsgEvent_CheckDeku(Actor* actor, PlayState* play, u8** script, MsgScriptCallback callback, s32* endScript) {
    Player* player = GET_PLAYER(play);
    MsgScriptCmdCheckDeku* cmd = (MsgScriptCmdCheckDeku*)*script;
    s16 skip = SCRIPT_PACK_16(cmd->offsetH, cmd->offsetL);

    if (player->transformation == PLAYER_FORM_DEKU) {
        *script += skip;
    }
    return false;
}

/**
 * Branch forward if the player is currently in human form
 *
 * Command structure:
 *  0:(u8)  cmd
 *  1:(s16) skip
 * Command size: 3
 */
s32 MsgEvent_CheckHuman(Actor* actor, PlayState* play, u8** script, MsgScriptCallback callback, s32* endScript) {
    Player* player = GET_PLAYER(play);
    MsgScriptCmdCheckHuman* cmd = (MsgScriptCmdCheckHuman*)*script;
    s16 skip = SCRIPT_PACK_16(cmd->offsetH, cmd->offsetL);

    if (player->transformation == PLAYER_FORM_HUMAN) {
        *script += skip;
    }
    return false;
}

/**
 * Branch forward depending on the message choice index
 *
 * Command structure:
 *  0:(u8)  cmd
 *  1:(s16) skipChoice1
 *  3:(s16) skipChoice2
 *  5:(s16) skipChoice3
 * Command size: 7
 */
s32 MsgEvent_CheckTextChoice(Actor* actor, PlayState* play, u8** script, MsgScriptCallback callback, s32* endScript) {
    MsgScriptCmdCheckTextChoice* cmd = (MsgScriptCmdCheckTextChoice*)*script;
    s16 skip = 0;

    switch (play->msgCtx.choiceIndex) {
        case 0:
            skip = SCRIPT_PACK_16(cmd->offset0H, cmd->offset0L);
            break;

        case 1:
            skip = SCRIPT_PACK_16(cmd->offset1H, cmd->offset1L);
            break;

        case 2:
            skip = SCRIPT_PACK_16(cmd->offset2H, cmd->offset2L);
            break;

        default:
            break;
    }
    *script += skip;

    return false;
}

/**
 * Branch forward if the actor has a parent, else get item
 *
 * Command structure:
 *  0:(u8)  cmd
 *  1:(s16) getItemId
 *  3:(s16) skip
 * Command size: 5
 */
s32 MsgEvent_OfferItem(Actor* actor, PlayState* play, u8** script, MsgScriptCallback callback, s32* endScript) {
    MsgScriptCmdOfferItem* cmd = (MsgScriptCmdOfferItem*)*script;
    s32 getItemId = SCRIPT_PACK_16(cmd->itemIdH, cmd->itemIdL);
    f32 xzRange = actor->xzDistToPlayer + 1.0f;
    f32 yRange = fabsf(actor->playerHeightRel) + 1.0f;
    s16 skip = SCRIPT_PACK_16(cmd->offsetH, cmd->offsetL);

    if (Actor_HasParent(actor, play)) {
        *script += skip;
    } else {
        Actor_OfferGetItem(actor, play, getItemId, xzRange, yRange);
        return true;
    }
    return false;
}

/**
 * Branch forward if there is a talk request, else autotalk
 *
 * Command structure:
 *  0:(u8)  cmd
 *  1:(s16) skip
 * Command size: 3
 */
s32 MsgEvent_Autotalk(Actor* actor, PlayState* play, u8** script, MsgScriptCallback callback, s32* endScript) {
    MsgScriptCmdAutotalk* cmd = (MsgScriptCmdAutotalk*)*script;
    f32 xzRange = actor->xzDistToPlayer + 1.0f;
    f32 yRange = fabsf(actor->playerHeightRel) + 1.0f;
    f32 xzDist;
    s16 skip = SCRIPT_PACK_16(cmd->offsetH, cmd->offsetL);

    if (Actor_TalkOfferAccepted(actor, &play->state)) {
        *script += skip;
    } else {
        actor->flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        xzDist = actor->xzDistToPlayer;
        actor->xzDistToPlayer = 0.0f;
        Actor_OfferTalkExchange(actor, play, xzRange, yRange, PLAYER_IA_NONE);
        actor->xzDistToPlayer = xzDist;
        return true;
    }
    return false;
}

/**
 * Branch forward if the player currently has more rupees
 *
 * Command structure:
 *  0:(u8)  cmd
 *  1:(s16) rupees
 *  3:(s16) skip
 * Command size: 5
 */
s32 MsgEvent_CheckRupees(Actor* actor, PlayState* play, u8** script, MsgScriptCallback callback, s32* endScript) {
    MsgScriptCmdCheckRupees* cmd = (MsgScriptCmdCheckRupees*)*script;
    s16 rupees = SCRIPT_PACK_16(cmd->rupeesH, cmd->rupeesL);
    s16 skip = SCRIPT_PACK_16(cmd->offsetH, cmd->offsetL);

    if (gSaveContext.save.saveInfo.playerData.rupees >= rupees) {
        *script += skip;
    }
    return false;
}

/**
 * Branch forward if the callback is null or returns non-zero
 *
 * Command structure:
 *  0:(u8)  cmd
 *  1:(s16) skip
 * Command size: 3
 */
s32 MsgEvent_CheckCallback(Actor* actor, PlayState* play, u8** script, MsgScriptCallback callback, s32* endScript) {
    MsgScriptCmdCheckCallback* cmd = (MsgScriptCmdCheckCallback*)*script;
    s16 skip = SCRIPT_PACK_16(cmd->offsetH, cmd->offsetL);

    if ((callback == NULL) || (callback(actor, play) != 0)) {
        *script += skip;
    } else {
        return true;
    }
    return false;
}

/**
 * Branch forward based on time of day
 *
 * Command structure:
 *  0:(u8)  cmd
 *  1:(s16) skipDay1
 *  3:(s16) skipNight1
 *  5:(s16) skipDay2
 *  7:(s16) skipNight2
 *  9:(s16) skipDay3
 *  11:(s16) skipNight3
 * Command size: 13
 */
s32 MsgEvent_CheckDay(Actor* actor, PlayState* play, u8** script, MsgScriptCallback callback, s32* endScript) {
    MsgScriptCmdCheckDay* cmd = (MsgScriptCmdCheckDay*)*script;
    s16 skip = 0;

    switch (gSaveContext.save.day) {
        case 1:
            if (!gSaveContext.save.isNight) {
                skip = SCRIPT_PACK_16(cmd->offsetDay1H, cmd->offsetDay1L);
            } else {
                skip = SCRIPT_PACK_16(cmd->offsetNight1H, cmd->offsetNight1L);
            }
            break;

        case 2:
            if (!gSaveContext.save.isNight) {
                skip = SCRIPT_PACK_16(cmd->offsetDay2H, cmd->offsetDay2L);
            } else {
                skip = SCRIPT_PACK_16(cmd->offsetNight2H, cmd->offsetNight2L);
            }
            break;

        case 3:
            if (!gSaveContext.save.isNight) {
                skip = SCRIPT_PACK_16(cmd->offsetDay3H, cmd->offsetDay3L);
            } else {
                skip = SCRIPT_PACK_16(cmd->offsetNight3H, cmd->offsetNight3L);
            }
            break;

        default:
            break;
    }
    *script += skip;
    return false;
}

/**
 * Wait on choice text, skip forward when text closes.
 *
 * Command structure:
 *  0:(u8)  cmd
 *  1:(s16) skip
 * Command size: 3
 */
s32 MsgEvent_AwaitTextJump(Actor* actor, PlayState* play, u8** script, MsgScriptCallback callback, s32* endScript) {
    MsgScriptCmdAwaitTextJump* cmd = (MsgScriptCmdAwaitTextJump*)*script;
    s16 skip;

    switch (Message_GetState(&play->msgCtx)) {
        case TEXT_STATE_CHOICE:
        case TEXT_STATE_EVENT:
            if (!Message_ShouldAdvance(play)) {
                return true;
            }
        case TEXT_STATE_CLOSING:
            skip = SCRIPT_PACK_16(cmd->offsetH, cmd->offsetL);
            break;

        default:
            return true;
    }
    *script += skip;
    return false;
}

/**
 * Waits for text to advance
 *
 * Command structure:
 *  0:(u8)  cmd
 * Command size: 1
 */
s32 MsgEvent_AwaitText(Actor* actor, PlayState* play, u8** script, MsgScriptCallback callback, s32* endScript) {
    switch (Message_GetState(&play->msgCtx)) {
        case TEXT_STATE_CHOICE:
        case TEXT_STATE_EVENT:
            if (!Message_ShouldAdvance(play)) {
                return true;
            }
            break;

        case TEXT_STATE_CLOSING:
            break;

        default:
            return true;
    }
    return false;
}

/**
 * Waits for text to end
 *
 * Command structure:
 *  0:(u8)  cmd
 * Command size: 1
 */
s32 MsgEvent_AwaitTextEnd(Actor* actor, PlayState* play, u8** script, MsgScriptCallback callback, s32* endScript) {
    s32 state = Message_GetState(&play->msgCtx);

    *endScript = false;

    switch (state) {
        case TEXT_STATE_CHOICE:
        case TEXT_STATE_EVENT:
            if (!Message_ShouldAdvance(play)) {
                break;
            }
        case TEXT_STATE_CLOSING:
            *endScript = true;
            break;

        default:
            return true;
    }
    return true;
}

/**
 *
 * Command structure:
 *  0:(u8)  cmd
 *  1:(u16) textId
 * Command size: 3
 */
s32 MsgEvent_BeginText(Actor* actor, PlayState* play, u8** script, MsgScriptCallback callback, s32* endScript) {
    MsgScriptCmdBeginText* cmd = (MsgScriptCmdBeginText*)*script;

    Message_StartTextbox(play, SCRIPT_PACK_16(cmd->textIdH, cmd->textIdL), NULL);
    return false;
}

/**
 *
 * Command structure:
 *  0:(u8)  cmd
 *  1:(u16) textId
 * Command size: 3
 */
s32 MsgEvent_ContinueText(Actor* actor, PlayState* play, u8** script, MsgScriptCallback callback, s32* endScript) {
    MsgScriptCmdContinueText* cmd = (MsgScriptCmdContinueText*)*script;

    Message_ContinueTextbox(play, SCRIPT_PACK_16(cmd->textIdH, cmd->textIdL));
    return false;
}

/**
 * Script Terminator
 *
 * Command structure:
 *  0:(u8)  cmd
 * Command size: 1
 */
s32 MsgEvent_Done(Actor* actor, PlayState* play, u8** script, MsgScriptCallback callback, s32* endScript) {
    *endScript = true;
    return true;
}

/**
 * Sets weekEventReg flags
 *
 * Command structure:
 *  0:(u8)  cmd
 *  1:(u16) flag
 * Command size: 3
 */
s32 MsgEvent_SetWeekEventReg(Actor* actor, PlayState* play, u8** script, MsgScriptCallback callback, s32* endScript) {
    MsgScriptCmdSetWeekEventReg* cmd = (MsgScriptCmdSetWeekEventReg*)*script;
    u16 flag = SCRIPT_PACK_16(cmd->flagByte, cmd->flagMask);

    SET_WEEKEVENTREG(flag);
    return false;
}

/**
 *
 * Command structure:
 *  0:(u8)  cmd
 * Command size: 1
 */
s32 MsgEvent_CloseText(Actor* actor, PlayState* play, u8** script, MsgScriptCallback callback, s32* endScript) {
    Message_CloseTextbox(play);
    return false;
}

/**
 *
 * Command structure:
 *  0:(u8)  cmd
 *  0:(u16) flag
 * Command size: 3
 */
s32 MsgEvent_SetCollectible(Actor* actor, PlayState* play, u8** script, MsgScriptCallback callback, s32* endScript) {
    MsgScriptCmdSetCollectible* cmd = (MsgScriptCmdSetCollectible*)*script;
    s32 flag = SCRIPT_PACK_16(cmd->flagH, cmd->flagL);

    if (!Flags_GetCollectible(play, flag)) {
        Flags_SetCollectible(play, flag);
    }
    actor->parent = NULL;
    return false;
}

/**
 *
 * Command structure:
 *  0:(u8)  cmd
 *  0:(s16) rupees
 * Command size: 3
 */
s32 MsgEvent_ChangeRupees(Actor* actor, PlayState* play, u8** script, MsgScriptCallback callback, s32* endScript) {
    MsgScriptCmdChangeRupees* cmd = (MsgScriptCmdChangeRupees*)*script;
    s16 rupees = SCRIPT_PACK_16(cmd->rupeesH, cmd->rupeesL);

    Rupees_ChangeBy(rupees);
    return false;
}

/**
 * Pause the message system
 *
 * Command structure:
 *  0:(u8)  cmd
 * Command size: 1
 */
s32 MsgEvent_Pause(Actor* actor, PlayState* play, u8** script, MsgScriptCallback callback, s32* endScript) {
    play->msgCtx.msgMode = MSGMODE_PAUSED;
    return false;
}

/**
 * Unsets ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED for the actor executing the cmd
 *
 * Command structure:
 *  0:(u8)  cmd
 * Command size: 1
 */
s32 MsgEvent_UnsetAutotalk(Actor* actor, PlayState* play, u8** script, MsgScriptCallback callback, s32* endScript) {
    actor->flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
    return false;
}

/**
 * Sets player focus & talk actor to the child of the actor executing the cmd
 *
 * Command structure:
 *  0:(u8)  cmd
 * Command size: 1
 */
s32 MsgEvent_FocusToChild(Actor* actor, PlayState* play, u8** script, MsgScriptCallback callback, s32* endScript) {
    if (actor->child != NULL) {
        Actor_ChangeFocus(actor, play, actor->child);
    }
    return false;
}

/**
 * Sets player focus & talk actor to the actor executing the cmd
 *
 * Command structure:
 *  0:(u8)  cmd
 * Command size: 1
 */
s32 MsgEvent_FocusToSelf(Actor* actor, PlayState* play, u8** script, MsgScriptCallback callback, s32* endScript) {
    if (actor->child != NULL) {
        Actor_ChangeFocus(actor->child, play, actor);
    }
    return false;
}

/**
 * Unconditional branch by `skip`
 *
 * Command structure:
 *  0:(u8)  cmd
 *  1:(s16) skip
 * Command size: 3
 */
s32 MsgEvent_Jump(Actor* actor, PlayState* play, u8** script, MsgScriptCallback callback, s32* endScript) {
    MsgScriptCmdJump* cmd = (MsgScriptCmdJump*)*script;
    s16 skip = SCRIPT_PACK_16(cmd->offsetH, cmd->offsetL);

    *script += skip;
    return false;
}

/**
 * Branch forwards if the specified quest item is obtained
 *
 * Command structure:
 *  0:(u8)  cmd
 *  1:(u16) questItem
 *  3:(s16) skip
 * Command size: 5
 */
s32 MsgEvent_CheckQuestItem(Actor* actor, PlayState* play, u8** script, MsgScriptCallback callback, s32* endScript) {
    MsgScriptCmdCheckQuestItem* cmd = (MsgScriptCmdCheckQuestItem*)*script;
    u16 questItem = SCRIPT_PACK_16(cmd->questItemH, cmd->questItemL);
    s16 skip = SCRIPT_PACK_16(cmd->offsetH, cmd->offsetL);

    if (CHECK_QUEST_ITEM(questItem)) {
        *script += skip;
    }
    return false;
}

/**
 * Branch forwards if event_inf flag is set
 *
 * Command structure:
 *  0:(u8)  cmd
 *  1:(u16) event_inf flag
 *  3:(s16) skip
 * Command size: 5
 */
s32 MsgEvent_CheckEventInf(Actor* actor, PlayState* play, u8** script, MsgScriptCallback callback, s32* endScript) {
    MsgScriptCmdCheckEventInf* cmd = (MsgScriptCmdCheckEventInf*)*script;
    s16 skip = SCRIPT_PACK_16(cmd->offsetH, cmd->offsetL);

    if (gSaveContext.eventInf[cmd->flagByte] & cmd->flagMask) {
        *script += skip;
    }
    return false;
}

/**
 * Sets event_inf flag
 *
 * Command structure:
 *  0:(u8)  cmd
 *  1:(u16) event_inf flag
 * Command size: 3
 */
s32 MsgEvent_SetEventInf(Actor* actor, PlayState* play, u8** script, MsgScriptCallback callback, s32* endScript) {
    MsgScriptCmdSetEventInf* cmd = (MsgScriptCmdSetEventInf*)*script;

    gSaveContext.eventInf[cmd->flagByte] |= cmd->flagMask;
    return false;
}

/**
 * Unsets event_inf flag
 *
 * Command structure:
 *  0:(u8)  cmd
 *  1:(u16) event_inf flag
 * Command size: 3
 */
s32 MsgEvent_UnsetEventInf(Actor* actor, PlayState* play, u8** script, MsgScriptCallback callback, s32* endScript) {
    MsgScriptCmdUnsetEventInf* cmd = (MsgScriptCmdUnsetEventInf*)*script;

    gSaveContext.eventInf[cmd->flagByte] &= cmd->flagMask ^ 0xFF;
    return false;
}

/**
 *
 * Command structure:
 *  0:(u8)  cmd
 *  1:(s16) input player item-action
 *  3:(s16) skip if player item-action matches input player item-action
 *  5:(s16) skip if player item-action does not match input player item-action
 *  7:(s16) skip if player item-action is negative
 * Command size: 9
 */
s32 MsgEvent_CheckItemAction(Actor* actor, PlayState* play, u8** script, MsgScriptCallback callback, s32* endScript) {
    MsgScriptCmdCheckItemAction* cmd = (MsgScriptCmdCheckItemAction*)*script;
    PlayerItemAction checkItemAction = SCRIPT_PACK_16(cmd->itemActionH, cmd->itemActionL);
    s16 skip;
    PlayerItemAction curItemAction;

    switch (Message_GetState(&play->msgCtx)) {
        case TEXT_STATE_CHOICE:
        case TEXT_STATE_EVENT:
            if (!Message_ShouldAdvance(play)) {
                return true;
            }
        case TEXT_STATE_PAUSE_MENU:
            curItemAction = func_80123810(play);

            if (curItemAction == PLAYER_IA_NONE) {
                return true;
            } else if (curItemAction < 0) {
                skip = SCRIPT_PACK_16(cmd->offsetContinueH, cmd->offsetContinueL);
            } else if (curItemAction == checkItemAction) {
                skip = SCRIPT_PACK_16(cmd->offsetEqualH, cmd->offsetEqualL);
            } else {
                skip = SCRIPT_PACK_16(cmd->offsetDefaultH, cmd->offsetDefaultL);
            }
            break;

        default:
            return true;
    }
    *script += skip;
    return false;
}

/**
 * Branches forward if song is obtained
 *
 * Command structure:
 *  0:(u8)  cmd
 *  1:(u16) song
 *  3:(s16) skip
 * Command size: 5
 */
s32 MsgEvent_CheckHasSong(Actor* actor, PlayState* play, u8** script, MsgScriptCallback callback, s32* endScript) {
    MsgScriptCmdCheckHasSong* cmd = (MsgScriptCmdCheckHasSong*)*script;
    u16 song = SCRIPT_PACK_16(cmd->songH, cmd->songL);
    s16 skip = SCRIPT_PACK_16(cmd->offsetH, cmd->offsetL);

    if (CHECK_QUEST_ITEM(QUEST_SONG_SONATA + song)) {
        *script += skip;
    }
    return false;
}

/**
 * Branches forward if the player's current mask matches
 *
 * Command structure:
 *  0:(u8)  cmd
 *  1:(s16) mask
 *  3:(s16) skip
 * Command size: 5
 */
s32 MsgEvent_CheckWornMask(Actor* actor, PlayState* play, u8** script, MsgScriptCallback callback, s32* endScript) {
    MsgScriptCmdCheckWornMask* cmd = (MsgScriptCmdCheckWornMask*)*script;
    s32 mask = SCRIPT_PACK_16(cmd->maskH, cmd->maskL);
    s16 skip = SCRIPT_PACK_16(cmd->offsetH, cmd->offsetL);

    if (Player_GetMask(play) == mask) {
        *script += skip;
    }
    return false;
}

/**
 * Branches forward if the current time is greater than the provided time
 *
 * Command structure:
 *  0:(u8)  cmd
 *  1:(u8)  hour
 *  1:(u8)  min
 *  3:(s16) skip
 * Command size: 5
 */
s32 MsgEvent_CheckAfterTime(Actor* actor, PlayState* play, u8** script, MsgScriptCallback callback, s32* endScript) {
    MsgScriptCmdCheckAfterTime* cmd = (MsgScriptCmdCheckAfterTime*)*script;
    s16 skip = SCRIPT_PACK_16(cmd->offsetH, cmd->offsetL);
    f32 f;
    u16 time;
    u16 now;

    SCRIPT_CALC_TIME(cmd->hr, cmd->min, time, f);
    now = SCRIPT_TIME_NOW;

    if (time < now) {
        *script += skip;
    }
    return false;
}

/**
 * Branches forward if the current time is less or equal to the provided time
 *
 * Command structure:
 *  0:(u8)  cmd
 *  1:(u8)  hour
 *  1:(u8)  min
 *  3:(s16) skip
 * Command size: 5
 */
s32 MsgEvent_CheckBeforeTime(Actor* actor, PlayState* play, u8** script, MsgScriptCallback callback, s32* endScript) {
    MsgScriptCmdCheckBeforeTime* cmd = (MsgScriptCmdCheckBeforeTime*)*script;
    s16 skip = SCRIPT_PACK_16(cmd->offsetH, cmd->offsetL);
    f32 f;
    u16 time;
    u16 now;

    SCRIPT_CALC_TIME(cmd->hr, cmd->min, time, f);
    now = SCRIPT_TIME_NOW;

    if (time >= now) {
        *script += skip;
    }
    return false;
}

/**
 * Branches forward if switch flag is set
 *
 * Command structure:
 *  0:(u8)  cmd
 *  1:(s16) flag
 *  3:(s16) skip
 * Command size: 5
 */
s32 MsgEvent_CheckSwitchFlag(Actor* actor, PlayState* play, u8** script, MsgScriptCallback callback, s32* endScript) {
    MsgScriptCmdCheckSwitchFlag* cmd = (MsgScriptCmdCheckSwitchFlag*)*script;
    s16 flag = SCRIPT_PACK_16(cmd->flagH, cmd->flagL);
    s16 skip = SCRIPT_PACK_16(cmd->offsetH, cmd->offsetL);

    if (Flags_GetSwitch(play, flag)) {
        *script += skip;
    }
    return false;
}

/**
 * Sets switch flag
 *
 * Command structure:
 *  0:(u8)  cmd
 *  1:(s16) flag
 * Command size: 3
 */
s32 MsgEvent_SetSwitchFlag(Actor* actor, PlayState* play, u8** script, MsgScriptCallback callback, s32* endScript) {
    MsgScriptCmdSetSwitchFlag* cmd = (MsgScriptCmdSetSwitchFlag*)*script;
    s16 flag = SCRIPT_PACK_16(cmd->flagH, cmd->flagL);

    Flags_SetSwitch(play, flag);
    return false;
}

/**
 * Branches forward if item is in inventory
 *
 * Command structure:
 *  0:(u8)  cmd
 *  1:(u16) item
 *  3:(s16) skip
 * Command size: 5
 */
s32 MsgEvent_CheckItem(Actor* actor, PlayState* play, u8** script, MsgScriptCallback callback, s32* endScript) {
    MsgScriptCmdCheckItem* cmd = (MsgScriptCmdCheckItem*)*script;
    u16 item = SCRIPT_PACK_16(cmd->itemH, cmd->itemL);
    s16 skip = SCRIPT_PACK_16(cmd->offsetH, cmd->offsetL);

    if (INV_CONTENT(item) == item) {
        *script += skip;
    }
    return false;
}

/**
 * Branches forward if current time is within the specified interval
 *
 * Command structure:
 *  0:(u8)  cmd
 *  1:(u16) startTime
 *  3:(u16) endTime
 *  5:(s16) skip
 * Command size: 7
 */
s32 MsgEvent_CheckBetweenTime(Actor* actor, PlayState* play, u8** script, MsgScriptCallback callback, s32* endScript) {
    MsgScriptCmdCheckBetweenTime* cmd = (MsgScriptCmdCheckBetweenTime*)*script;
    s16 skip = SCRIPT_PACK_16(cmd->offsetH, cmd->offsetL);
    f32 f;
    u16 startTime;
    u16 endTime;
    u16 now;

    SCRIPT_CALC_TIME(cmd->startHr, cmd->startMin, startTime, f);
    SCRIPT_CALC_TIME(cmd->endHr, cmd->endMin, endTime, f);
    endTime--;
    now = SCRIPT_TIME_NOW;

    if ((startTime >= now) || (now >= endTime)) {
        *script += skip;
    }
    return false;
}

/**
 * Branches forward if the current day matches
 *
 * Command structure:
 *  0:(u8)  cmd
 *  1:(s16) day
 *  3:(s16) skip
 * Command size: 5
 */
s32 MsgEvent_CheckOnDay(Actor* actor, PlayState* play, u8** script, MsgScriptCallback callback, s32* endScript) {
    MsgScriptCmdCheckOnDay* cmd = (MsgScriptCmdCheckOnDay*)*script;
    s16 day = SCRIPT_PACK_16(cmd->dayH, cmd->dayL);
    s16 skip = SCRIPT_PACK_16(cmd->offsetH, cmd->offsetL);

    if (gSaveContext.save.day == day) {
        *script += skip;
    }
    return false;
}

/**
 * Branches forward if callback exists and returns non-zero
 *
 * Command structure:
 *  0:(u8)  cmd
 *  1:(s16) skip
 * Command size: 3
 */
s32 MsgEvent_CheckCallbackContinue(Actor* actor, PlayState* play, u8** script, MsgScriptCallback callback,
                                   s32* endScript) {
    MsgScriptCmdCheckCallbackContinue* cmd = (MsgScriptCmdCheckCallbackContinue*)*script;
    s16 skip = SCRIPT_PACK_16(cmd->offsetH, cmd->offsetL);

    if ((callback != NULL) && (callback(actor, play) != 0)) {
        *script += skip;
    }
    return false;
}

/**
 * Skip forward if a powder keg is in inventory or if a powder keg actor exists
 *
 * Command structure:
 *  0:(u8)  cmd
 *  1:(s16) skip
 * Command size: 3
 */
s32 MsgEvent_CheckHasPowderKeg(Actor* actor, PlayState* play, u8** script, MsgScriptCallback callback, s32* endScript) {
    MsgScriptCmdCheckHasPowderKeg* cmd = (MsgScriptCmdCheckHasPowderKeg*)*script;
    s16 skip = SCRIPT_PACK_16(cmd->offsetH, cmd->offsetL);

    if (GameInteractor_Should(VB_POWDER_KEG_CHECK_HAS,
                              (AMMO(ITEM_POWDER_KEG) != 0) || (play->actorCtx.flags & ACTORCTX_FLAG_0))) {
        *script += skip;
    }
    return false;
}

/**
 * Deletes the specified item
 *
 * Command structure:
 *  0:(u8)  cmd
 *  1:(s16) item
 * Command size: 3
 */
s32 MsgEvent_DeleteItem(Actor* actor, PlayState* play, u8** script, MsgScriptCallback callback, s32* endScript) {
    MsgScriptCmdDeleteItem* cmd = (MsgScriptCmdDeleteItem*)*script;
    s16 item = SCRIPT_PACK_16(cmd->itemH, cmd->itemL);

    if (!GameInteractor_Should(VB_MSG_SCRIPT_DEL_ITEM, true, actor, item)) {
        return false;
    }

    Inventory_DeleteItem(item, SLOT(item));
    return false;
}

/**
 * Runs the callback and selects a skip based on the result, default skip is the first one
 *
 * Command structure:
 *  0:(u8)  cmd
 *  1:(s16) skip
 *  3:(s16) skip
 *  5:(s16) skip
 * Command size: 7
 */
s32 MsgEvent_CheckCallbackMulti(Actor* actor, PlayState* play, u8** script, MsgScriptCallback callback,
                                s32* endScript) {
    MsgScriptCmdCheckCallbackMulti* cmd = (MsgScriptCmdCheckCallbackMulti*)*script;
    s32 ret = 1;
    s16 skip;

    if (callback != NULL) {
        ret = callback(actor, play);
    }
    switch (ret) {
        case 3:
            skip = SCRIPT_PACK_16(cmd->offset3H, cmd->offset3L);
            break;

        case 2:
            skip = SCRIPT_PACK_16(cmd->offset2H, cmd->offset2L);
            break;

        case 1:
            skip = SCRIPT_PACK_16(cmd->offset1H, cmd->offset1L);
            break;

        default:
            return true;
    }
    *script += skip;
    return false;
}

/**
 * Close the current textbox and set the player actor text id
 *
 * Command structure:
 *  0:(u8)  cmd
 *  1:(u16) textId
 * Command size: 3
 */
s32 MsgEvent_PlayerTalk(Actor* actor, PlayState* play, u8** script, MsgScriptCallback callback, s32* endScript) {
    Player* player = GET_PLAYER(play);
    MsgScriptCmdPlayerTalk* cmd = (MsgScriptCmdPlayerTalk*)*script;
    u16 textId = SCRIPT_PACK_16(cmd->textIdH, cmd->textIdL);

    player->actor.textId = textId;
    Message_CloseTextbox(play);
    return false;
}

/**
 * Queue a bomber's notebook event
 *
 * Command structure:
 *  0:(u8)  cmd
 *  1:(u16) event
 * Command size: 3
 */
s32 MsgEvent_NotebookEvent(Actor* actor, PlayState* play, u8** script, MsgScriptCallback callback, s32* endScript) {
    MsgScriptCmdBombersNotebook* cmd = (MsgScriptCmdBombersNotebook*)*script;
    u16 event = SCRIPT_PACK_16(cmd->eventH, cmd->eventL);

    Message_BombersNotebookQueueEvent(play, event);
    return false;
}

/**
 *
 *
 * Command structure:
 *  0:(u8)  cmd
 * Command size: 1
 */
s32 MsgEvent_AwaitTextDone(Actor* actor, PlayState* play, u8** script, MsgScriptCallback callback, s32* endScript) {
    switch (Message_GetState(&play->msgCtx)) {
        case TEXT_STATE_CLOSING:
            break;

        case TEXT_STATE_CHOICE:
        case TEXT_STATE_EVENT:
            if (!Message_ShouldAdvance(play)) {
                return true;
            }

        case TEXT_STATE_DONE:
            if (!Message_ShouldAdvance(play)) {
                return true;
            }
            break;

        default:
            return true;
    }
    return false;
}

/**
 *
 * Command structure:
 *  0:(u8)  cmd
 *  1:(s16) skip
 * Command size: 3
 */
s32 MsgEvent_Jump3(Actor* actor, PlayState* play, u8** script, MsgScriptCallback callback, s32* endScript) {
    MsgScriptCmdJump3* cmd = (MsgScriptCmdJump3*)*script;
    s16 skip = SCRIPT_PACK_16(cmd->offsetH, cmd->offsetL);

    *script += skip;
    *script += 3;
    return true;
}

/**
 * Plays Decide sfx
 *
 * Command structure:
 *  0:(u8)  cmd
 * Command size: 1
 */
s32 MsgEvent_PlayDecide(Actor* actor, PlayState* play, u8** script, MsgScriptCallback callback, s32* endScript) {
    Audio_PlaySfx_MessageDecide();
    return false;
}

/**
 * Plays Cancel sfx
 *
 * Command structure:
 *  0:(u8)  cmd
 * Command size: 1
 */
s32 MsgEvent_PlayCancel(Actor* actor, PlayState* play, u8** script, MsgScriptCallback callback, s32* endScript) {
    Audio_PlaySfx_MessageCancel();
    return false;
}

/**
 * Plays NA_SE_SY_ERROR sfx
 *
 * Command structure:
 *  0:(u8)  cmd
 * Command size: 1
 */
s32 MsgEvent_PlayError(Actor* actor, PlayState* play, u8** script, MsgScriptCallback callback, s32* endScript) {
    Audio_PlaySfx(NA_SE_SY_ERROR);
    return false;
}

typedef s32 (*MsgScriptCmdHandler)(struct Actor*, struct PlayState*, u8**, MsgScriptCallback, s32*);

MsgScriptCmdHandler sMsgScriptCmdHandlers[] = {
    MsgEvent_CheckWeekEventReg,     // MSCRIPT_CMD_ID_CHECK_WEEK_EVENT_REG
    MsgEvent_CheckGoron,            // MSCRIPT_CMD_ID_CHECK_GORON
    MsgEvent_CheckZora,             // MSCRIPT_CMD_ID_CHECK_ZORA
    MsgEvent_CheckDeku,             // MSCRIPT_CMD_ID_CHECK_DEKU
    MsgEvent_CheckHuman,            // MSCRIPT_CMD_ID_CHECK_HUMAN
    MsgEvent_CheckTextChoice,       // MSCRIPT_CMD_ID_CHECK_TEXT_CHOICE
    MsgEvent_OfferItem,             // MSCRIPT_CMD_ID_OFFER_ITEM
    MsgEvent_Autotalk,              // MSCRIPT_CMD_ID_AUTOTALK
    MsgEvent_CheckRupees,           // MSCRIPT_CMD_ID_CHECK_RUPEES
    MsgEvent_CheckCallback,         // MSCRIPT_CMD_ID_CHECK_CALLBACK
    MsgEvent_CheckDay,              // MSCRIPT_CMD_ID_CHECK_DAY
    MsgEvent_AwaitTextJump,         // MSCRIPT_CMD_ID_AWAIT_TEXT_JUMP
    MsgEvent_AwaitText,             // MSCRIPT_CMD_ID_AWAIT_TEXT
    MsgEvent_AwaitTextEnd,          // MSCRIPT_CMD_ID_AWAIT_TEXT_END
    MsgEvent_BeginText,             // MSCRIPT_CMD_ID_BEGIN_TEXT
    MsgEvent_ContinueText,          // MSCRIPT_CMD_ID_CONTINUE_TEXT
    MsgEvent_Done,                  // MSCRIPT_CMD_ID_DONE
    MsgEvent_SetWeekEventReg,       // MSCRIPT_CMD_ID_SET_WEEK_EVENT_REG
    MsgEvent_CloseText,             // MSCRIPT_CMD_ID_CLOSE_TEXT
    MsgEvent_SetCollectible,        // MSCRIPT_CMD_ID_SET_COLLECTIBLE
    MsgEvent_ChangeRupees,          // MSCRIPT_CMD_ID_CHANGE_RUPEES
    MsgEvent_Pause,                 // MSCRIPT_CMD_ID_PAUSE
    MsgEvent_UnsetAutotalk,         // MSCRIPT_CMD_ID_UNSET_AUTOTALK
    MsgEvent_FocusToChild,          // MSCRIPT_CMD_ID_FOCUS_TO_CHILD
    MsgEvent_FocusToSelf,           // MSCRIPT_CMD_ID_FOCUS_TO_SELF
    MsgEvent_Jump,                  // MSCRIPT_CMD_ID_JUMP
    MsgEvent_CheckQuestItem,        // MSCRIPT_CMD_ID_CHECK_QUEST_ITEM
    MsgEvent_CheckEventInf,         // MSCRIPT_CMD_ID_CHECK_EVENT_INF
    MsgEvent_SetEventInf,           // MSCRIPT_CMD_ID_SET_EVENT_INF
    MsgEvent_UnsetEventInf,         // MSCRIPT_CMD_ID_UNSET_EVENT_INF
    MsgEvent_CheckItemAction,       // MSCRIPT_CMD_ID_CHECK_ITEM_ACTION
    MsgEvent_CheckHasSong,          // MSCRIPT_CMD_ID_CHECK_HAS_SONG
    MsgEvent_CheckWornMask,         // MSCRIPT_CMD_ID_CHECK_WORN_MASK
    MsgEvent_CheckAfterTime,        // MSCRIPT_CMD_ID_CHECK_AFTER_TIME
    MsgEvent_CheckBeforeTime,       // MSCRIPT_CMD_ID_CHECK_BEFORE_TIME
    MsgEvent_CheckSwitchFlag,       // MSCRIPT_CMD_ID_CHECK_SWITCH_FLAG
    MsgEvent_SetSwitchFlag,         // MSCRIPT_CMD_ID_SET_SWITCH_FLAG
    MsgEvent_CheckItem,             // MSCRIPT_CMD_ID_CHECK_ITEM
    MsgEvent_CheckBetweenTime,      // MSCRIPT_CMD_ID_CHECK_BETWEEN_TIME
    MsgEvent_CheckOnDay,            // MSCRIPT_CMD_ID_CHECK_ON_DAY
    MsgEvent_CheckCallbackContinue, // MSCRIPT_CMD_ID_CHECK_CALLBACK_CONTINUE
    MsgEvent_CheckHasPowderKeg,     // MSCRIPT_CMD_ID_CHECK_HAS_POWDER_KEG
    MsgEvent_DeleteItem,            // MSCRIPT_CMD_ID_DELETE_ITEM
    MsgEvent_CheckCallbackMulti,    // MSCRIPT_CMD_ID_CHECK_CALLBACK_MULTI
    MsgEvent_PlayerTalk,            // MSCRIPT_CMD_ID_PLAYER_TALK
    MsgEvent_NotebookEvent,         // MSCRIPT_CMD_ID_NOTEBOOK_EVENT
    MsgEvent_AwaitTextDone,         // MSCRIPT_CMD_ID_AWAIT_TEXT_DONE
    MsgEvent_Jump3,                 // MSCRIPT_CMD_ID_JUMP_3
    MsgEvent_PlayDecide,            // MSCRIPT_CMD_ID_PLAY_DECIDE
    MsgEvent_PlayCancel,            // MSCRIPT_CMD_ID_PLAY_CANCEL
    MsgEvent_PlayError,             // MSCRIPT_CMD_ID_PLAY_ERROR
};

u8 sMsgScriptCmdSizes[] = {
    sizeof(MsgScriptCmdCheckWeekEventReg),     // MSCRIPT_CMD_ID_CHECK_WEEK_EVENT_REG
    sizeof(MsgScriptCmdCheckGoron),            // MSCRIPT_CMD_ID_CHECK_GORON
    sizeof(MsgScriptCmdCheckZora),             // MSCRIPT_CMD_ID_CHECK_ZORA
    sizeof(MsgScriptCmdCheckDeku),             // MSCRIPT_CMD_ID_CHECK_DEKU
    sizeof(MsgScriptCmdCheckHuman),            // MSCRIPT_CMD_ID_CHECK_HUMAN
    sizeof(MsgScriptCmdCheckTextChoice),       // MSCRIPT_CMD_ID_CHECK_TEXT_CHOICE
    sizeof(MsgScriptCmdOfferItem),             // MSCRIPT_CMD_ID_OFFER_ITEM
    sizeof(MsgScriptCmdAutotalk),              // MSCRIPT_CMD_ID_AUTOTALK
    sizeof(MsgScriptCmdCheckRupees),           // MSCRIPT_CMD_ID_CHECK_RUPEES
    sizeof(MsgScriptCmdCheckCallback),         // MSCRIPT_CMD_ID_CHECK_CALLBACK
    sizeof(MsgScriptCmdCheckDay),              // MSCRIPT_CMD_ID_CHECK_DAY,
    sizeof(MsgScriptCmdAwaitTextJump),         // MSCRIPT_CMD_ID_AWAIT_TEXT_JUMP
    sizeof(MsgScriptCmdBase),                  // MSCRIPT_CMD_ID_AWAIT_TEXT
    sizeof(MsgScriptCmdBase),                  // MSCRIPT_CMD_ID_AWAIT_TEXT_END
    sizeof(MsgScriptCmdBeginText),             // MSCRIPT_CMD_ID_BEGIN_TEXT
    sizeof(MsgScriptCmdContinueText),          // MSCRIPT_CMD_ID_CONTINUE_TEXT
    sizeof(MsgScriptCmdBase),                  // MSCRIPT_CMD_ID_DONE
    sizeof(MsgScriptCmdSetWeekEventReg),       // MSCRIPT_CMD_ID_SET_WEEK_EVENT_REG
    sizeof(MsgScriptCmdBase),                  // MSCRIPT_CMD_ID_CLOSE_TEXT
    sizeof(MsgScriptCmdSetCollectible),        // MSCRIPT_CMD_ID_SET_COLLECTIBLE
    sizeof(MsgScriptCmdChangeRupees),          // MSCRIPT_CMD_ID_CHANGE_RUPEES
    sizeof(MsgScriptCmdBase),                  // MSCRIPT_CMD_ID_PAUSE
    sizeof(MsgScriptCmdBase),                  // MSCRIPT_CMD_ID_UNSET_AUTOTALK
    sizeof(MsgScriptCmdBase),                  // MSCRIPT_CMD_ID_FOCUS_TO_CHILD
    sizeof(MsgScriptCmdBase),                  // MSCRIPT_CMD_ID_FOCUS_TO_SELF
    sizeof(MsgScriptCmdJump),                  // MSCRIPT_CMD_ID_JUMP
    sizeof(MsgScriptCmdCheckQuestItem),        // MSCRIPT_CMD_ID_CHECK_QUEST_ITEM
    sizeof(MsgScriptCmdCheckEventInf),         // MSCRIPT_CMD_ID_CHECK_EVENT_INF
    sizeof(MsgScriptCmdSetEventInf),           // MSCRIPT_CMD_ID_SET_EVENT_INF
    sizeof(MsgScriptCmdUnsetEventInf),         // MSCRIPT_CMD_ID_UNSET_EVENT_INF
    sizeof(MsgScriptCmdCheckItemAction),       // MSCRIPT_CMD_ID_CHECK_ITEM_ACTION
    sizeof(MsgScriptCmdCheckHasSong),          // MSCRIPT_CMD_ID_CHECK_HAS_SONG
    sizeof(MsgScriptCmdCheckWornMask),         // MSCRIPT_CMD_ID_CHECK_WORN_MASK
    sizeof(MsgScriptCmdCheckAfterTime),        // MSCRIPT_CMD_ID_CHECK_AFTER_TIME
    sizeof(MsgScriptCmdCheckBeforeTime),       // MSCRIPT_CMD_ID_CHECK_BEFORE_TIME
    sizeof(MsgScriptCmdCheckSwitchFlag),       // MSCRIPT_CMD_ID_CHECK_SWITCH_FLAG
    sizeof(MsgScriptCmdSetSwitchFlag),         // MSCRIPT_CMD_ID_SET_SWITCH_FLAG
    sizeof(MsgScriptCmdCheckItem),             // MSCRIPT_CMD_ID_CHECK_ITEM
    sizeof(MsgScriptCmdCheckBetweenTime),      // MSCRIPT_CMD_ID_CHECK_BETWEEN_TIME
    sizeof(MsgScriptCmdCheckOnDay),            // MSCRIPT_CMD_ID_CHECK_ON_DAY
    sizeof(MsgScriptCmdCheckCallbackContinue), // MSCRIPT_CMD_ID_CHECK_CALLBACK_CONTINUE
    sizeof(MsgScriptCmdCheckHasPowderKeg),     // MSCRIPT_CMD_ID_CHECK_HAS_POWDER_KEG
    sizeof(MsgScriptCmdDeleteItem),            // MSCRIPT_CMD_ID_DELETE_ITEM
    sizeof(MsgScriptCmdCheckCallbackMulti),    // MSCRIPT_CMD_ID_CHECK_CALLBACK_MULTI
    sizeof(MsgScriptCmdPlayerTalk),            // MSCRIPT_CMD_ID_PLAYER_TALK
    sizeof(MsgScriptCmdBombersNotebook),       // MSCRIPT_CMD_ID_NOTEBOOK_EVENT
    sizeof(MsgScriptCmdBase),                  // MSCRIPT_CMD_ID_AWAIT_TEXT_DONE
    sizeof(MsgScriptCmdJump3),                 // MSCRIPT_CMD_ID_JUMP_3
    sizeof(MsgScriptCmdBase),                  // MSCRIPT_CMD_ID_PLAY_DECIDE
    sizeof(MsgScriptCmdBase),                  // MSCRIPT_CMD_ID_PLAY_CANCEL
    sizeof(MsgScriptCmdBase),                  // MSCRIPT_CMD_ID_PLAY_ERROR
};

/**
 * Runs a message event script
 *
 * @param actor         The actor associated with the cmd
 * @param play          Play game state
 * @param script        The script to run
 * @param callback      Callback function used by various commands for different purposes
 * @param[in,out] pos   Position to resume the cmd from, the point at which the cmd stops executing is also
 *                      written out here
 * @return s32  Whether the cmd has reached an endpoint
 */
s32 MsgEvent_RunScript(Actor* actor, PlayState* play, MsgScript* script, MsgScriptCallback callback, s32* pos) {
    u8* start;
    u8* cur;
    s32 scriptDone = false;
    s32 i;
    s32 cmdLen;
    u8 cmdId;
    s32 stop;

    start = script;
    script += *pos;

    if (sREG(95) != 0) {}

    cmdLen = 0;
    do {
        // Skip data from previous command
        script += cmdLen;

        // Get command id
        cmdId = *script;

        // Get command length
        if (cmdId < ARRAY_COUNTU(sMsgScriptCmdSizes)) {
            cmdLen = sMsgScriptCmdSizes[cmdId];
        } else {
            cmdLen = -1;
            //! @bug command handler still runs even if cmdId is invalid
        }

        // Debug loop?
        if (sREG(95) != 0) {
            for (i = 0; i < cmdLen; i++) {}
        }

        // Run command handler
        stop = GameInteractor_Should(VB_EXEC_MSG_EVENT, true, cmdId, actor, script, &callback) &&
               sMsgScriptCmdHandlers[cmdId](actor, play, &script, callback, &scriptDone);
    } while (!stop);

    cur = script;
    if (!scriptDone) {
        *pos = cur - start;
    } else {
        *pos = 0;
    }
    return scriptDone;
}
