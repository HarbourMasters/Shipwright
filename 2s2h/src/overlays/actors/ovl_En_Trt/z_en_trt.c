/*
 * File: z_en_trt.c
 * Overlay: ovl_En_Trt
 * Description: Kotake
 */

#include "z_en_trt.h"
#include "objects/object_trt/object_trt.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "2s2h/GameInteractor/GameInteractor.h"
#include <libultraship/bridge/consolevariablebridge.h>

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY)

#define ENTRT_FULLY_AWAKE (1 << 0)
#define ENTRT_GIVEN_MUSHROOM (1 << 1)
#define ENTRT_TALKED (1 << 2)

void EnTrt_Init(Actor* thisx, PlayState* play);
void EnTrt_Destroy(Actor* thisx, PlayState* play);
void EnTrt_Update(Actor* thisx, PlayState* play);
void EnTrt_Draw(Actor* thisx, PlayState* play);

void EnTrt_GetCutscenes(EnTrt* this, PlayState* play);
void EnTrt_ResetItemPosition(EnTrt* this);
void EnTrt_TrackPlayer(EnTrt* this, PlayState* play);
s32 EnTrt_ReturnItemToShelf(EnTrt* this);
s32 EnTrt_FacingShopkeeperDialogResult(EnTrt* this, PlayState* play);
s32 EnTrt_TakeItemOffShelf(EnTrt* this);

void EnTrt_BeginInteraction(EnTrt* this, PlayState* play);
void EnTrt_IdleSleeping(EnTrt* this, PlayState* play);
void EnTrt_IdleAwake(EnTrt* this, PlayState* play);
void EnTrt_Hello(EnTrt* this, PlayState* play);
void EnTrt_FaceShopkeeper(EnTrt* this, PlayState* play);
void EnTrt_SetupEndInteraction(EnTrt* this, PlayState* play);
void EnTrt_StartShopping(PlayState* play, EnTrt* this);
void EnTrt_GiveRedPotionForKoume(EnTrt* this, PlayState* play);
void EnTrt_EndConversation(EnTrt* this, PlayState* play);
void EnTrt_TryToGiveRedPotion(EnTrt* this, PlayState* play);
void EnTrt_Surprised(EnTrt* this, PlayState* play);
void EnTrt_LookToShelf(EnTrt* this, PlayState* play);
void EnTrt_GivenRedPotionForKoume(EnTrt* this, PlayState* play);
void EnTrt_TryToGiveRedPotionAfterSurprised(EnTrt* this, PlayState* play);
void EnTrt_BrowseShelf(EnTrt* this, PlayState* play);
void EnTrt_SetupTalkToShopkeeper(PlayState* play, EnTrt* this);
void EnTrt_ContinueShopping(EnTrt* this, PlayState* play);
void EnTrt_ItemGiven(EnTrt* this, PlayState* play);
void EnTrt_SelectItem(EnTrt* this, PlayState* play);
void EnTrt_SetupLookToShopkeeperFromShelf(PlayState* play, EnTrt* this);
void EnTrt_LookToShopkeeperFromShelf(EnTrt* this, PlayState* play);
void EnTrt_PayForMushroom(EnTrt* this, PlayState* play);
void EnTrt_CanBuy(EnTrt* this, PlayState* play);
void EnTrt_CannotBuy(EnTrt* this, PlayState* play);
void EnTrt_BuyItemWithFanfare(EnTrt* this, PlayState* play);
void EnTrt_SetupItemGiven(EnTrt* this, PlayState* play);

void EnTrt_EyesClosed(EnTrt* this);
void EnTrt_OpenThenCloseEyes(EnTrt* this);
void EnTrt_OpenEyesThenSetToBlink(EnTrt* this);
void EnTrt_CloseEyes(EnTrt* this);
void EnTrt_OpenEyes(EnTrt* this);
void EnTrt_Blink(EnTrt* this);
void EnTrt_OpenEyes2(EnTrt* this);
void EnTrt_NodOff(EnTrt* this);

typedef enum TrtAnimation {
    /*  0 */ TRT_ANIM_IDLE,
    /*  1 */ TRT_ANIM_HALF_AWAKE,
    /*  2 */ TRT_ANIM_SLEEPING,
    /*  3 */ TRT_ANIM_WAKE_UP,
    /*  4 */ TRT_ANIM_SURPRISED,
    /*  5 */ TRT_ANIM_HANDS_ON_COUNTER,
    /*  6 */ TRT_ANIM_HOVER,
    /*  7 */ TRT_ANIM_FLY_LOOK_AROUND,
    /*  8 */ TRT_ANIM_FLY_DOWN,
    /*  9 */ TRT_ANIM_FLY,
    /* 10 */ TRT_ANIM_MAX
} TrtAnimation;

static AnimationInfoS sAnimationInfo[TRT_ANIM_MAX] = {
    { &gKotakeIdleAnim, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },           // TRT_ANIM_IDLE
    { &gKotakeHalfAwakeAnim, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },      // TRT_ANIM_HALF_AWAKE
    { &gKotakeSleepingAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },       // TRT_ANIM_SLEEPING
    { &gKotakeWakeUpAnim, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },         // TRT_ANIM_WAKE_UP
    { &gKotakeSurprisedAnim, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },      // TRT_ANIM_SURPRISED
    { &gKotakeHandsOnCounterAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 }, // TRT_ANIM_HANDS_ON_COUNTER
    { &gKotakeHoverAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },          // TRT_ANIM_HOVER
    { &gKotakeFlyLookAroundAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },  // TRT_ANIM_FLY_LOOK_AROUND
    { &gKotakeFlyDownAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },        // TRT_ANIM_FLY_DOWN
    { &gKotakeFlyAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },            // TRT_ANIM_FLY
};

ActorProfile En_Trt_Profile = {
    /**/ ACTOR_EN_TRT,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_TRT,
    /**/ sizeof(EnTrt),
    /**/ EnTrt_Init,
    /**/ EnTrt_Destroy,
    /**/ EnTrt_Update,
    /**/ EnTrt_Draw,
};

static f32 sActorScale = 0.008f;

static ShopItem sShop[] = {
    { SI_POTION_RED_1, { 24, 32, -36 } },
    { SI_POTION_GREEN_1, { 6, 32, -36 } },
    { SI_POTION_BLUE, { -12, 32, -36 } },
};

void EnTrt_ChangeAnim(SkelAnime* skelAnime, AnimationInfoS* animationInfo, s32 animIndex) {
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

bool EnTrt_TestItemSelected(PlayState* play) {
    MessageContext* msgCtx = &play->msgCtx;

    if ((msgCtx->textboxEndType == TEXTBOX_ENDTYPE_TWO_CHOICE) ||
        (msgCtx->textboxEndType == TEXTBOX_ENDTYPE_THREE_CHOICE)) {
        return CHECK_BTN_ALL(CONTROLLER1(&play->state)->press.button, BTN_A);
    }
    return CHECK_BTN_ALL(CONTROLLER1(&play->state)->press.button, BTN_A) ||
           CHECK_BTN_ALL(CONTROLLER1(&play->state)->press.button, BTN_B) ||
           CHECK_BTN_ALL(CONTROLLER1(&play->state)->press.button, BTN_CUP);
}

void EnTrt_SpawnShopItems(EnTrt* this, PlayState* play, ShopItem* shopItem) {
    s32 i;

    for (i = 0; i < ARRAY_COUNT(this->items); i++, shopItem++) {
        if (shopItem->shopItemId < 0) {
            this->items[i] = NULL;
        } else if (shopItem->shopItemId < 0) {
            this->items[i] = NULL;
        } else {
            this->items[i] =
                (EnGirlA*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_GIRLA, shopItem->spawnPos.x, shopItem->spawnPos.y,
                                      shopItem->spawnPos.z, 0, 0, 0, shopItem->shopItemId);
        }
    }
}

void EnTrt_UpdateCollider(EnTrt* this, PlayState* play) {
    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
}

void EnTrt_UpdateCursorPos(PlayState* play, EnTrt* this) {
    s16 screenPosX;
    s16 screenPosY;
    f32 xOffset = 0.0f;
    f32 yOffset = 17.0f;

    Actor_GetScreenPos(play, &this->items[this->cursorIndex]->actor, &screenPosX, &screenPosY);
    this->cursorPos.x = screenPosX + xOffset;
    this->cursorPos.y = screenPosY + yOffset;
    this->cursorPos.z = 1.2f;

    if (CVarGetInteger("gModes.MirroredWorld.State", 0)) {
        this->cursorPos.x = SCREEN_WIDTH - this->cursorPos.x;
    }
}

void EnTrt_SetupGetMushroomCutscene(EnTrt* this) {
    if (this->cutsceneState == ENTRT_CUTSCENESTATE_PLAYING) {
        if (this->csId != this->tmpGetMushroomCsId) {
            CutsceneManager_Stop(this->csId);
            if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
                CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
            }
            this->csId = this->tmpGetMushroomCsId;
            CutsceneManager_Queue(this->csId);
            this->cutsceneState = ENTRT_CUTSCENESTATE_WAITING;
        }
    }
    if (this->cutsceneState == ENTRT_CUTSCENESTATE_WAITING) {
        if (CutsceneManager_IsNext(this->csId)) {
            CutsceneManager_StartWithPlayerCsAndSetFlag(this->csId, &this->actor);
            this->cutsceneState = ENTRT_CUTSCENESTATE_PLAYING_SPECIAL;
        } else {
            CutsceneManager_Queue(this->csId);
        }
    }
}

u16 EnTrt_GetItemTextId(EnTrt* this) {
    EnGirlA* item = this->items[this->cursorIndex];

    if ((item->actor.params == SI_POTION_BLUE) && !(this->flags & ENTRT_GIVEN_MUSHROOM)) {
        return 0x880;
    }
    return item->actor.textId;
}

u16 EnTrt_GetItemChoiceTextId(EnTrt* this) {
    EnGirlA* item = this->items[this->cursorIndex];

    if ((item->actor.params == SI_POTION_BLUE) && !CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_FREE_BLUE_POTION)) {
        this->textId = 0x881;
        return 0x881;
    }
    return item->choiceTextId;
}

void EnTrt_EndInteraction(PlayState* play, EnTrt* this) {
    Player* player = GET_PLAYER(play);

    if (this->cutsceneState == ENTRT_CUTSCENESTATE_PLAYING) {
        CutsceneManager_Stop(this->csId);
        this->cutsceneState = ENTRT_CUTSCENESTATE_STOPPED;
    }
    Actor_TalkOfferAccepted(&this->actor, &play->state);
    play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
    play->msgCtx.stateTimer = 4;
    Interface_SetHudVisibility(HUD_VISIBILITY_ALL);
    this->drawCursor = 0;
    this->stickLeftPrompt.isEnabled = false;
    this->stickRightPrompt.isEnabled = false;
    player->stateFlags2 &= ~PLAYER_STATE2_20000000;
    play->interfaceCtx.bButtonInterfaceDoActionActive = false;
    play->interfaceCtx.bButtonInterfaceDoAction = 0;
    this->textId = 0x834;
    this->timer = 80;
    this->flags |= ENTRT_FULLY_AWAKE;
    this->sleepSoundTimer = 10;
    this->actor.textId = 0;
    this->actionFunc = EnTrt_IdleSleeping;
    this->blinkFunc = EnTrt_NodOff;
}

s32 EnTrt_TestEndInteraction(EnTrt* this, PlayState* play, Input* input) {
    if (CHECK_BTN_ALL(input->press.button, BTN_B)) {
        EnTrt_EndInteraction(play, this);
        return true;
    }
    return false;
}

s32 EnTrt_TestCancelOption(EnTrt* this, PlayState* play, Input* input) {
    if (CHECK_BTN_ALL(input->press.button, BTN_B)) {
        this->actionFunc = this->prevActionFunc;
        Message_ContinueTextbox(play, EnTrt_GetItemTextId(this));
        return true;
    }
    return false;
}

void EnTrt_SetupStartShopping(PlayState* play, EnTrt* this, u8 skipHello) {
    Interface_SetAButtonDoAction(play, DO_ACTION_NEXT);
    if (!skipHello) {
        this->actionFunc = EnTrt_Hello;
    } else {
        EnTrt_StartShopping(play, this);
    }
}

void EnTrt_StartShopping(PlayState* play, EnTrt* this) {
    Message_ContinueTextbox(play, 0x83E);
    Interface_SetAButtonDoAction(play, DO_ACTION_DECIDE);
    this->stickLeftPrompt.isEnabled = false;
    this->stickRightPrompt.isEnabled = true;
    this->actionFunc = EnTrt_FaceShopkeeper;
}

void EnTrt_UpdateJoystickInputState(PlayState* play, EnTrt* this) {
    s8 stickX = CONTROLLER1(&play->state)->rel.stick_x;
    s8 stickY = CONTROLLER1(&play->state)->rel.stick_y;

    stickX *= GameInteractor_InvertControl(GI_INVERT_SHOP_X);

    if (this->stickAccumX == 0) {
        if ((stickX > 30) || (stickX < -30)) {
            this->stickAccumX = stickX;
        }
    } else if ((stickX <= 30) && (stickX >= -30)) {
        this->stickAccumX = 0;
    } else if ((this->stickAccumX * stickX) < 0) { // Stick has swapped directions
        this->stickAccumX = stickX;
    } else {
        this->stickAccumX += stickX;
        if (this->stickAccumX > 2000) {
            this->stickAccumX = 2000;
        } else if (this->stickAccumX < -2000) {
            this->stickAccumX = -2000;
        }
    }
    if (this->stickAccumY == 0) {
        if ((stickY > 30) || (stickY < -30)) {
            this->stickAccumY = stickY;
        }
    } else if ((stickY <= 30) && (stickY >= -30)) {
        this->stickAccumY = 0;
    } else if ((this->stickAccumY * stickY) < 0) { // Stick has swapped directions
        this->stickAccumY = stickY;
    } else {
        this->stickAccumY += stickY;
        if (this->stickAccumY > 2000) {
            this->stickAccumY = 2000;
        } else if (this->stickAccumY < -2000) {
            this->stickAccumY = -2000;
        }
    }
}

u8 EnTrt_SetCursorIndexFromNeutral(EnTrt* this, u8 shelfOffset) {
    if (this->items[shelfOffset] != NULL) {
        return shelfOffset;
    }
    return CURSOR_INVALID;
}

void EnTrt_Hello(EnTrt* this, PlayState* play) {
    u8 talkState = Message_GetState(&play->msgCtx);

    if (this->cutsceneState == ENTRT_CUTSCENESTATE_WAITING) {
        if (CutsceneManager_IsNext(this->csId)) {
            CutsceneManager_StartWithPlayerCsAndSetFlag(this->csId, &this->actor);
            this->cutsceneState = ENTRT_CUTSCENESTATE_PLAYING;
        } else {
            CutsceneManager_Queue(this->csId);
        }
    }
    if ((talkState == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        Audio_PlaySfx(NA_SE_SY_MESSAGE_PASS);
        if (!EnTrt_TestEndInteraction(this, play, CONTROLLER1(&play->state))) {
            EnTrt_StartShopping(play, this);
        }
    }
}

void EnTrt_GetMushroom(EnTrt* this, PlayState* play) {
    u8 talkState = Message_GetState(&play->msgCtx);
    Player* player = GET_PLAYER(play);

    this->tmpGetMushroomCsId = this->getMushroomCsId;
    if (this->cutsceneState != ENTRT_CUTSCENESTATE_PLAYING_SPECIAL) {
        EnTrt_SetupGetMushroomCutscene(this);
        if (this->cutsceneState == ENTRT_CUTSCENESTATE_PLAYING_SPECIAL) {
            player->stateFlags2 &= ~PLAYER_STATE2_20000000;
        }
    } else if ((talkState == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        switch (this->textId) {
            case 0x883:
                this->textId = 0x884;
                Message_StartTextbox(play, this->textId, &this->actor);
                SET_WEEKEVENTREG(WEEKEVENTREG_GAVE_KOTAKE_MUSHROOM);
                Player_UpdateBottleHeld(play, GET_PLAYER(play), ITEM_BOTTLE, PLAYER_IA_BOTTLE_EMPTY);
                break;

            case 0x888:
                this->textId = 0x889;
                Message_StartTextbox(play, this->textId, &this->actor);
                break;

            case 0x889:
                if (this->cutsceneState == ENTRT_CUTSCENESTATE_PLAYING_SPECIAL) {
                    CutsceneManager_Stop(this->csId);
                    this->cutsceneState = ENTRT_CUTSCENESTATE_STOPPED;
                }
                play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
                play->msgCtx.stateTimer = 4;
                this->actionFunc = EnTrt_PayForMushroom;
                break;

            default:
                this->cutsceneState = ENTRT_CUTSCENESTATE_PLAYING;
                EnTrt_EndInteraction(play, this);
                break;
        }
    }
}

void EnTrt_PayForMushroom(EnTrt* this, PlayState* play) {
    if (Actor_HasParent(&this->actor, play)) {
        this->actor.parent = NULL;
        Player_UpdateBottleHeld(play, GET_PLAYER(play), ITEM_BOTTLE, PLAYER_IA_BOTTLE_EMPTY);
        this->actionFunc = EnTrt_SetupItemGiven;
    } else {
        Actor_OfferGetItem(&this->actor, play, GI_RUPEE_RED, 300.0f, 300.0f);
    }
}

void EnTrt_Goodbye(EnTrt* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        switch (this->textId) {
            case 0x886:
                this->textId = 0x887;
                Message_ContinueTextbox(play, this->textId);
                break;

            case 0x887:
            case 0x88B:
                EnTrt_EndInteraction(play, this);
                break;

            default:
                break;
        }
    }
}

void EnTrt_StartRedPotionConversation(EnTrt* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        if (this->textId == 0x88F) {
            if (Inventory_HasEmptyBottle() || !CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_KOTAKE_BOTTLE)) {
                if (this->cutsceneState == ENTRT_CUTSCENESTATE_PLAYING) {
                    CutsceneManager_Stop(this->csId);
                    this->cutsceneState = ENTRT_CUTSCENESTATE_STOPPED;
                }
                play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
                play->msgCtx.stateTimer = 4;
                this->actionFunc = EnTrt_GiveRedPotionForKoume;
            } else {
                this->prevTextId = this->textId;
                this->textId = 0x88E;
                SET_WEEKEVENTREG(WEEKEVENTREG_FAILED_RECEIVED_RED_POTION_FOR_KOUME_SHOP);
                Message_StartTextbox(play, this->textId, &this->actor);
                this->actionFunc = EnTrt_EndConversation;
            }
        } else {
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_SAVED_KOUME)) {
                this->textId = 0x83D;
                EnTrt_SetupStartShopping(play, this, 0);
            } else if (CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_RED_POTION_FOR_KOUME)) {
                this->textId = 0x83B;
                if (Inventory_HasItemInBottle(ITEM_POTION_RED)) {
                    EnTrt_SetupStartShopping(play, this, false);
                } else {
                    this->actionFunc = EnTrt_TryToGiveRedPotion;
                }
            } else if (CHECK_WEEKEVENTREG(WEEKEVENTREG_TALKED_KOUME_INJURED)) {
                this->timer = 30;
                this->textId = 0x838;
                this->cutsceneState = ENTRT_CUTSCENESTATE_PLAYING_SPECIAL;
                this->actionFunc = EnTrt_Surprised;
                return;
            } else if (CHECK_WEEKEVENTREG(WEEKEVENTREG_TALKED_KOUME_KIOSK_EMPTY)) {
                this->textId = 0x835;
                EnTrt_SetupStartShopping(play, this, false);
            }
            Message_StartTextbox(play, this->textId, &this->actor);
        }
    }
}

void EnTrt_GiveRedPotionForKoume(EnTrt* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (Actor_HasParent(&this->actor, play)) {
        this->actor.parent = NULL;
        if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_KOTAKE_BOTTLE)) {
            SET_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_KOTAKE_BOTTLE);
        }
        SET_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_RED_POTION_FOR_KOUME);
        player->stateFlags2 &= ~PLAYER_STATE2_20000000;
        this->actionFunc = EnTrt_GivenRedPotionForKoume;
    } else if (CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_KOTAKE_BOTTLE)) {
        Actor_OfferGetItem(&this->actor, play, GI_POTION_RED, 300.0f, 300.0f);
    } else {
        Actor_OfferGetItem(&this->actor, play, GI_POTION_RED_BOTTLE, 300.0f, 300.0f);
    }
}

void EnTrt_GivenRedPotionForKoume(EnTrt* this, PlayState* play) {
    //! @bug: player is set to NULL not PLAYER
    // 2S2H [Port] - Opt to comment out later reference of player to avoid bad access
    Player* player = NULL;

    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_DONE) && Message_ShouldAdvance(play)) {
        if (this->cutsceneState == ENTRT_CUTSCENESTATE_STOPPED) {
            if (CutsceneManager_IsNext(this->csId)) {
                CutsceneManager_StartWithPlayerCsAndSetFlag(this->csId, &this->actor);
                // 2S2H [Port] - player is always NULL by this point
                // player->stateFlags2 |= PLAYER_STATE2_20000000;
                //! @bug: EnTrt_ContinueShopping gets overwritten by EnTrt_ItemGiven
                this->actionFunc = EnTrt_ContinueShopping;
                this->cutsceneState = ENTRT_CUTSCENESTATE_PLAYING;
            } else {
                if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
                    CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
                }
                this->csId = this->lookToShopkeeperCsId;
                CutsceneManager_Queue(this->csId);
            }
        }
        Actor_OfferTalkExchangeEquiCylinder(&this->actor, play, 400.0f, PLAYER_IA_MINUS1);
        this->actionFunc = EnTrt_ItemGiven;
    }
}

void EnTrt_EndConversation(EnTrt* this, PlayState* play) {
    u8 talkState = Message_GetState(&play->msgCtx);

    if (talkState == TEXT_STATE_EVENT) {
        if (Message_ShouldAdvance(play)) {
            EnTrt_EndInteraction(play, this);
        }
    } else if (talkState == TEXT_STATE_DONE) {
        if (Message_ShouldAdvance(play)) {
            EnTrt_EndInteraction(play, this);
        }
    }
}

s32 EnTrt_FacingShopkeeperDialogResult(EnTrt* this, PlayState* play) {
    switch (play->msgCtx.choiceIndex) {
        case 0:
            Audio_PlaySfx_MessageDecide();
            EnTrt_SetupTalkToShopkeeper(play, this);
            return true;

        case 1:
            Audio_PlaySfx_MessageCancel();
            EnTrt_EndInteraction(play, this);
            return true;

        default:
            return false;
    }
}

void EnTrt_FaceShopkeeper(EnTrt* this, PlayState* play) {
    u8 talkState = Message_GetState(&play->msgCtx);
    u8 cursorIndex;

    if (this->cutsceneState == ENTRT_CUTSCENESTATE_WAITING) {
        if (CutsceneManager_IsNext(this->csId)) {
            CutsceneManager_StartWithPlayerCsAndSetFlag(this->csId, &this->actor);
            this->cutsceneState = ENTRT_CUTSCENESTATE_PLAYING;
        } else {
            CutsceneManager_Queue(this->csId);
        }
    }
    if (this->cutsceneState == ENTRT_CUTSCENESTATE_STOPPED) {
        if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
            CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
        }
        this->csId = this->lookForwardCutscene;
        CutsceneManager_Queue(this->csId);
        this->cutsceneState = ENTRT_CUTSCENESTATE_WAITING;
    } else if (talkState == TEXT_STATE_CHOICE) {
        Interface_SetAButtonDoAction(play, DO_ACTION_DECIDE);
        if (!EnTrt_TestEndInteraction(this, play, CONTROLLER1(&play->state))) {
            if ((!Message_ShouldAdvance(play) || !EnTrt_FacingShopkeeperDialogResult(this, play)) &&
                (this->stickAccumX > 0)) {
                cursorIndex = EnTrt_SetCursorIndexFromNeutral(this, 2);
                if (cursorIndex != CURSOR_INVALID) {
                    this->cursorIndex = cursorIndex;
                    this->actionFunc = EnTrt_LookToShelf;
                    Interface_SetAButtonDoAction(play, DO_ACTION_DECIDE);
                    this->stickRightPrompt.isEnabled = false;
                    Audio_PlaySfx(NA_SE_SY_CURSOR);
                }
            }
        }
    }
}

void EnTrt_LookToShelf(EnTrt* this, PlayState* play) {
    if (this->cutsceneState == ENTRT_CUTSCENESTATE_PLAYING) {
        CutsceneManager_Stop(this->csId);
        this->cutsceneState = ENTRT_CUTSCENESTATE_STOPPED;
    }
    if (this->cutsceneState == ENTRT_CUTSCENESTATE_STOPPED) {
        if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
            CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
        }
        this->csId = this->lookToShelfCsId;
        CutsceneManager_Queue(this->csId);
        this->cutsceneState = ENTRT_CUTSCENESTATE_WAITING;
    } else if (this->cutsceneState == ENTRT_CUTSCENESTATE_WAITING) {
        if (CutsceneManager_IsNext(this->csId)) {
            CutsceneManager_StartWithPlayerCsAndSetFlag(this->csId, &this->actor);
            this->cutsceneState = ENTRT_CUTSCENESTATE_PLAYING;
            EnTrt_UpdateCursorPos(play, this);
            this->actionFunc = EnTrt_BrowseShelf;
            Message_ContinueTextbox(play, EnTrt_GetItemTextId(this));
        } else {
            CutsceneManager_Queue(this->csId);
        }
    }
}

void EnTrt_CursorLeftRight(PlayState* play, EnTrt* this) {
    u8 curTemp = this->cursorIndex;

    if (this->stickAccumX < 0) {
        if (curTemp != 2) {
            curTemp++;
        } else {
            EnTrt_SetupLookToShopkeeperFromShelf(play, this);
        }
        if (this->items[curTemp] != NULL) {
            this->cursorIndex = curTemp;
        }
    } else if (this->stickAccumX > 0) {
        if (curTemp != 0) {
            curTemp--;
        }
        if (this->items[curTemp] != NULL) {
            this->cursorIndex = curTemp;
        }
    }
}

s32 EnTrt_HasPlayerSelectedItem(PlayState* play, EnTrt* this, Input* input) {
    EnGirlA* item = this->items[this->cursorIndex];

    if (EnTrt_TestEndInteraction(this, play, input)) {
        return true;
    }
    if (EnTrt_TestItemSelected(play)) {
        if ((item->actor.params != SI_POTION_BLUE) || (this->flags & ENTRT_GIVEN_MUSHROOM)) {
            this->prevActionFunc = this->actionFunc;
            Message_ContinueTextbox(play, EnTrt_GetItemChoiceTextId(this));
            Audio_PlaySfx(NA_SE_SY_DECIDE);
            this->stickLeftPrompt.isEnabled = false;
            this->stickRightPrompt.isEnabled = false;
            this->drawCursor = 0;
            this->actionFunc = EnTrt_SelectItem;
        } else {
            Audio_PlaySfx(NA_SE_SY_ERROR);
        }
        return true;
    }
    return false;
}

void EnTrt_BrowseShelf(EnTrt* this, PlayState* play) {
    u8 talkState = Message_GetState(&play->msgCtx);
    s32 pad;
    u8 prevCursorIdx = this->cursorIndex;
    u8 cursorIndex;

    if (!EnTrt_ReturnItemToShelf(this)) {
        this->delayTimer = 3;
    } else if (this->delayTimer != 0) {
        this->delayTimer--;
    } else {
        this->drawCursor = 0xFF;
        this->stickLeftPrompt.isEnabled = true;
        EnTrt_UpdateCursorPos(play, this);
        if (talkState == TEXT_STATE_EVENT) {
            Interface_SetAButtonDoAction(play, DO_ACTION_DECIDE);
            if (!EnTrt_HasPlayerSelectedItem(play, this, CONTROLLER1(&play->state))) {
                EnTrt_CursorLeftRight(play, this);
                if (this->cursorIndex != prevCursorIdx) {
                    Message_ContinueTextbox(play, EnTrt_GetItemTextId(this));
                    Audio_PlaySfx(NA_SE_SY_CURSOR);
                }
            }
        }
    }
}

void EnTrt_SetupBuyItemWithFanfare(PlayState* play, EnTrt* this) {
    Player* player = GET_PLAYER(play);

    Actor_OfferGetItem(&this->actor, play, this->items[this->cursorIndex]->getItemId, 300.0f, 300.0f);
    play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
    play->msgCtx.stateTimer = 4;
    player->stateFlags2 &= ~PLAYER_STATE2_20000000;
    Interface_SetHudVisibility(HUD_VISIBILITY_ALL);
    this->drawCursor = 0;
    this->actionFunc = EnTrt_BuyItemWithFanfare;
}

void EnTrt_SetupCannotBuy(PlayState* play, EnTrt* this, u16 textId) {
    Message_ContinueTextbox(play, textId);
    this->actionFunc = EnTrt_CannotBuy;
}

void EnTrt_SetupCanBuy(PlayState* play, EnTrt* this, u16 textId) {
    Message_ContinueTextbox(play, textId);
    this->actionFunc = EnTrt_CanBuy;
}

void EnTrt_HandleCanBuyItem(PlayState* play, EnTrt* this) {
    EnGirlA* item = this->items[this->cursorIndex];
    EnGirlA* item2;

    switch (item->canBuyFunc(play, item)) {
        case CANBUY_RESULT_SUCCESS_1:
            if (this->cutsceneState == ENTRT_CUTSCENESTATE_PLAYING) {
                CutsceneManager_Stop(this->csId);
                this->cutsceneState = ENTRT_CUTSCENESTATE_STOPPED;
            }
            Audio_PlaySfx_MessageDecide();
            item2 = this->items[this->cursorIndex];
            item2->buyFanfareFunc(play, item2);
            EnTrt_SetupBuyItemWithFanfare(play, this);
            this->drawCursor = 0;
            this->shopItemSelectedTween = 0.0f;
            item->boughtFunc(play, item);
            break;

        case CANBUY_RESULT_SUCCESS_2:
            Audio_PlaySfx_MessageDecide();
            item->buyFunc(play, item);
            EnTrt_SetupCanBuy(play, this, 0x848);
            this->drawCursor = 0;
            this->shopItemSelectedTween = 0.0f;
            item->boughtFunc(play, item);
            break;

        case CANBUY_RESULT_NO_ROOM:
            Audio_PlaySfx(NA_SE_SY_ERROR);
            EnTrt_SetupCannotBuy(play, this, 0x641);
            break;

        case CANBUY_RESULT_NEED_EMPTY_BOTTLE:
            Audio_PlaySfx(NA_SE_SY_ERROR);
            EnTrt_SetupCannotBuy(play, this, 0x846);
            break;

        case CANBUY_RESULT_NEED_RUPEES:
            Audio_PlaySfx(NA_SE_SY_ERROR);
            EnTrt_SetupCannotBuy(play, this, 0x847);
            break;

        case CANBUY_RESULT_CANNOT_GET_NOW:
            Audio_PlaySfx(NA_SE_SY_ERROR);
            EnTrt_SetupCannotBuy(play, this, 0x643);
            break;

        default:
            break;
    }
}

void EnTrt_SelectItem(EnTrt* this, PlayState* play) {
    EnGirlA* item = this->items[this->cursorIndex];
    u8 talkState = Message_GetState(&play->msgCtx);

    if (EnTrt_TakeItemOffShelf(this)) {
        if (talkState == TEXT_STATE_CHOICE) {
            Interface_SetAButtonDoAction(play, DO_ACTION_DECIDE);
            if (!EnTrt_TestCancelOption(this, play, CONTROLLER1(&play->state)) && Message_ShouldAdvance(play)) {
                switch (play->msgCtx.choiceIndex) {
                    case 0:
                        EnTrt_HandleCanBuyItem(play, this);
                        break;

                    case 1:
                        Audio_PlaySfx_MessageCancel();
                        this->actionFunc = this->prevActionFunc;
                        Message_ContinueTextbox(play, EnTrt_GetItemTextId(this));
                        break;

                    default:
                        break;
                }
            }
        } else if ((talkState == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
            if (!Inventory_HasEmptyBottle()) {
                Audio_PlaySfx(NA_SE_SY_ERROR);
                EnTrt_SetupCannotBuy(play, this, 0x846);
            } else {
                if (this->cutsceneState == ENTRT_CUTSCENESTATE_PLAYING) {
                    CutsceneManager_Stop(this->csId);
                    this->cutsceneState = ENTRT_CUTSCENESTATE_STOPPED;
                }
                EnTrt_SetupBuyItemWithFanfare(play, this);
                this->drawCursor = 0;
                this->shopItemSelectedTween = 0.0f;
                item->boughtFunc(play, item);
                SET_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_FREE_BLUE_POTION);
            }
        }
    }
}

void EnTrt_IdleSleeping(EnTrt* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_FAILED_RECEIVED_RED_POTION_FOR_KOUME_SHOP) &&
        !CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_RED_POTION_FOR_KOUME)) {
        this->textId = 0x88F;
    } else if (!(this->flags & ENTRT_TALKED)) {
        this->textId = 0x834;
    } else {
        this->textId = 0x83E;
    }
    if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_GAVE_KOTAKE_MUSHROOM)) {
        this->talkOptionTextId = 0x845;
    } else if (this->flags & ENTRT_GIVEN_MUSHROOM) {
        this->talkOptionTextId = 0x882;
    } else {
        this->talkOptionTextId = 0x885;
    }
    this->prevTextId = this->textId;

    if ((player->transformation == PLAYER_FORM_GORON) || (player->transformation == PLAYER_FORM_ZORA) ||
        (player->transformation == PLAYER_FORM_DEKU)) {
        this->textId = 0x850;
    }
    if (Player_GetMask(play) == PLAYER_MASK_SCENTS) {
        this->textId = 0x890;
    }
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        if (player->transformation == PLAYER_FORM_HUMAN) {
            this->flags |= ENTRT_TALKED;
        }
        if (this->cutsceneState == ENTRT_CUTSCENESTATE_STOPPED) {
            if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
                CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
            }
            this->csId = this->lookForwardCutscene;
            CutsceneManager_Queue(this->csId);
            this->cutsceneState = ENTRT_CUTSCENESTATE_WAITING;
        }
        player->stateFlags2 |= PLAYER_STATE2_20000000;
        this->timer = 45;
        this->actionFunc = EnTrt_BeginInteraction;
    } else if (((player->actor.world.pos.x >= -50.0f) && (player->actor.world.pos.x <= -25.0f)) &&
               ((player->actor.world.pos.z >= -19.0f) && (player->actor.world.pos.z <= 30.0f))) {
        Actor_OfferTalk(&this->actor, play, 200.0f);
    }
    if (DECR(this->timer) == 0) {
        this->timer = 40;
        EnTrt_ChangeAnim(&this->skelAnime, sAnimationInfo, TRT_ANIM_HALF_AWAKE);
        this->animIndex = TRT_ANIM_HALF_AWAKE;
        this->actionFunc = EnTrt_IdleAwake;
        this->blinkFunc = EnTrt_OpenThenCloseEyes;
    }
    if (DECR(this->sleepSoundTimer) == 0) {
        Actor_PlaySfx_Flagged(&this->actor, NA_SE_EN_KOTAKE_SLEEP - SFX_FLAG);
    }
}

void EnTrt_IdleAwake(EnTrt* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    this->flags &= ~ENTRT_FULLY_AWAKE;
    if ((player->transformation == PLAYER_FORM_HUMAN) || (player->transformation == PLAYER_FORM_FIERCE_DEITY)) {
        if (Player_GetMask(play) == PLAYER_MASK_SCENTS) {
            this->textId = 0x890;
        } else {
            this->textId = this->prevTextId;
        }
    } else {
        this->textId = 0x850;
    }
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        if (this->cutsceneState == ENTRT_CUTSCENESTATE_STOPPED) {
            if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
                CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
            }
            this->csId = this->lookForwardCutscene;
            CutsceneManager_Queue(this->csId);
            this->cutsceneState = ENTRT_CUTSCENESTATE_WAITING;
        }
        player->stateFlags2 |= PLAYER_STATE2_20000000;
        if (player->transformation == PLAYER_FORM_HUMAN) {
            this->flags |= ENTRT_TALKED;
        }
        EnTrt_ChangeAnim(&this->skelAnime, sAnimationInfo, TRT_ANIM_SLEEPING);
        this->blinkFunc = EnTrt_EyesClosed;
        this->timer = 45;
        this->actionFunc = EnTrt_BeginInteraction;
    } else if ((player->actor.world.pos.x >= -50.0f) && (player->actor.world.pos.x <= -25.0f) &&
               (player->actor.world.pos.z >= -19.0f) && (player->actor.world.pos.z <= 30.0f)) {
        Actor_OfferTalk(&this->actor, play, 200.0f);
    }
    if (DECR(this->timer) == 0) {
        this->timer = Rand_S16Offset(150, 100);
        EnTrt_ChangeAnim(&this->skelAnime, sAnimationInfo, TRT_ANIM_SLEEPING);
        this->animIndex = TRT_ANIM_SLEEPING;
        this->sleepSoundTimer = 10;
        this->actor.textId = 0;
        this->actionFunc = EnTrt_IdleSleeping;
        this->blinkFunc = EnTrt_EyesClosed;
    }
}

void EnTrt_BeginInteraction(EnTrt* this, PlayState* play) {
    s16 curFrame = this->skelAnime.curFrame / this->skelAnime.playSpeed;
    s16 endFrame = Animation_GetLastFrame(&gKotakeWakeUpAnim) / TRUNCF_BINANG(this->skelAnime.playSpeed);

    if (this->cutsceneState == ENTRT_CUTSCENESTATE_WAITING) {
        if (CutsceneManager_IsNext(this->csId)) {
            CutsceneManager_StartWithPlayerCsAndSetFlag(this->csId, &this->actor);
            this->cutsceneState = ENTRT_CUTSCENESTATE_PLAYING_SPECIAL;
        } else {
            CutsceneManager_Queue(this->csId);
        }
    } else if (this->cutsceneState == ENTRT_CUTSCENESTATE_PLAYING_SPECIAL) {
        if (this->animIndex != TRT_ANIM_HANDS_ON_COUNTER) {
            if (curFrame == endFrame) {
                EnTrt_ChangeAnim(&this->skelAnime, sAnimationInfo, TRT_ANIM_WAKE_UP);
                this->animIndex = TRT_ANIM_WAKE_UP;
                this->blinkFunc = EnTrt_OpenEyesThenSetToBlink;
                this->timer = 10;
                this->cutsceneState = ENTRT_CUTSCENESTATE_PLAYING;
                Actor_PlaySfx(&this->actor, NA_SE_EN_KOTAKE_SURPRISED2);
            }
        } else {
            this->blinkFunc = EnTrt_OpenEyesThenSetToBlink;
            this->timer = 10;
            this->cutsceneState = ENTRT_CUTSCENESTATE_PLAYING;
        }
    } else if (DECR(this->timer) == 0) {
        this->timer = Rand_S16Offset(40, 20);
        EnTrt_ChangeAnim(&this->skelAnime, sAnimationInfo, TRT_ANIM_HANDS_ON_COUNTER);
        Message_StartTextbox(play, this->textId, &this->actor);
        this->animIndex = TRT_ANIM_HANDS_ON_COUNTER;
        switch (this->textId) {
            case 0x834:
                if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_SAVED_KOUME) &&
                    !CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_RED_POTION_FOR_KOUME) &&
                    !CHECK_WEEKEVENTREG(WEEKEVENTREG_TALKED_KOUME_INJURED) &&
                    !CHECK_WEEKEVENTREG(WEEKEVENTREG_TALKED_KOUME_KIOSK_EMPTY)) {
                    Interface_SetAButtonDoAction(play, DO_ACTION_DECIDE);
                    this->stickLeftPrompt.isEnabled = false;
                    this->stickRightPrompt.isEnabled = true;
                    this->actionFunc = EnTrt_Hello;
                } else {
                    this->actionFunc = EnTrt_StartRedPotionConversation;
                }
                break;

            case 0x83E:
                Interface_SetAButtonDoAction(play, DO_ACTION_DECIDE);
                this->stickLeftPrompt.isEnabled = false;
                this->stickRightPrompt.isEnabled = true;
                this->actionFunc = EnTrt_FaceShopkeeper;
                break;

            case 0x850:
            case 0x890:
                this->actionFunc = EnTrt_SetupEndInteraction;
                break;

            case 0x88F:
                this->actionFunc = EnTrt_StartRedPotionConversation;
                break;

            default:
                break;
        }
    }
}

void EnTrt_Surprised(EnTrt* this, PlayState* play) {
    if (this->cutsceneState == ENTRT_CUTSCENESTATE_WAITING) {
        if (CutsceneManager_IsNext(this->csId)) {
            CutsceneManager_StartWithPlayerCsAndSetFlag(this->csId, &this->actor);
            this->cutsceneState = ENTRT_CUTSCENESTATE_PLAYING_SPECIAL;
        } else {
            CutsceneManager_Queue(this->csId);
        }
    } else if (this->cutsceneState == ENTRT_CUTSCENESTATE_PLAYING_SPECIAL) {
        if (DECR(this->timer) == 0) {
            EnTrt_ChangeAnim(&this->skelAnime, sAnimationInfo, TRT_ANIM_SURPRISED);
            this->animIndex = TRT_ANIM_SURPRISED;
            this->blinkFunc = EnTrt_OpenEyes2;
            Actor_PlaySfx(&this->actor, NA_SE_EN_KOTAKE_SURPRISED);
            this->timer = 30;
            this->cutsceneState = ENTRT_CUTSCENESTATE_PLAYING;
        }
    } else if (DECR(this->timer) == 0) {
        this->timer = Rand_S16Offset(40, 20);
        EnTrt_ChangeAnim(&this->skelAnime, sAnimationInfo, TRT_ANIM_HANDS_ON_COUNTER);
        Message_StartTextbox(play, this->textId, &this->actor);
        this->animIndex = TRT_ANIM_HANDS_ON_COUNTER;
        this->actionFunc = EnTrt_TryToGiveRedPotionAfterSurprised;
    }
}

void EnTrt_TryToGiveRedPotionAfterSurprised(EnTrt* this, PlayState* play) {
    u8 talkState = Message_GetState(&play->msgCtx);

    this->blinkFunc = EnTrt_Blink;
    if ((talkState == TEXT_STATE_DONE) && Message_ShouldAdvance(play)) {
        if (Inventory_HasEmptyBottle() || !CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_KOTAKE_BOTTLE)) {
            if (this->cutsceneState == ENTRT_CUTSCENESTATE_PLAYING) {
                CutsceneManager_Stop(this->csId);
                this->cutsceneState = ENTRT_CUTSCENESTATE_STOPPED;
            }
            this->actionFunc = EnTrt_GiveRedPotionForKoume;
        } else {
            this->prevTextId = this->textId;
            this->textId = 0x88E;
            SET_WEEKEVENTREG(WEEKEVENTREG_FAILED_RECEIVED_RED_POTION_FOR_KOUME_SHOP);
            Message_StartTextbox(play, this->textId, &this->actor);
            this->actionFunc = EnTrt_EndConversation;
        }
    }
}

void EnTrt_TryToGiveRedPotion(EnTrt* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        if (this->textId == 0x83C) {
            if (Inventory_HasEmptyBottle()) {
                if (this->cutsceneState == ENTRT_CUTSCENESTATE_PLAYING) {
                    CutsceneManager_Stop(this->csId);
                    this->cutsceneState = ENTRT_CUTSCENESTATE_STOPPED;
                }
                play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
                play->msgCtx.stateTimer = 4;
                this->actionFunc = EnTrt_GiveRedPotionForKoume;
            } else {
                this->prevTextId = this->textId;
                this->textId = 0x88E;
                SET_WEEKEVENTREG(WEEKEVENTREG_FAILED_RECEIVED_RED_POTION_FOR_KOUME_SHOP);
                Message_StartTextbox(play, this->textId, &this->actor);
                this->actionFunc = EnTrt_EndConversation;
            }
        } else {
            this->textId = 0x83C;
            Message_StartTextbox(play, this->textId, &this->actor);
        }
    }
}

void EnTrt_ItemGiven(EnTrt* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (this->cutsceneState == ENTRT_CUTSCENESTATE_STOPPED) {
        if (CutsceneManager_IsNext(this->csId)) {
            CutsceneManager_StartWithPlayerCsAndSetFlag(this->csId, &this->actor);
            player->stateFlags2 |= PLAYER_STATE2_20000000;
            this->actionFunc = EnTrt_ContinueShopping;
            this->cutsceneState = ENTRT_CUTSCENESTATE_PLAYING;
        } else {
            if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
                CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
            }
            this->csId = this->lookToShopkeeperCsId;
            CutsceneManager_Queue(this->csId);
        }
    }
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        switch (this->textId) {
            case 0x889:
                this->textId = 0x88A;
                break;

            case 0x881:
                this->textId = 0x88C;
                break;

            case 0x838:
            case 0x83C:
            case 0x88E:
            case 0x88F:
                this->textId = 0x83A;
                break;

            default:
                this->textId = 0x849;
                // note break is missing
                // break;
        }
        Message_ContinueTextbox(play, this->textId);
    } else {
        Actor_OfferTalkExchangeEquiCylinder(&this->actor, play, 400.0f, PLAYER_IA_MINUS1);
    }
}

void EnTrt_SetupEndInteraction(EnTrt* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        EnTrt_EndInteraction(play, this);
    }
}

void EnTrt_ShopkeeperGone(EnTrt* this, PlayState* play) {
    u8 talkState = Message_GetState(&play->msgCtx);
    Player* player = GET_PLAYER(play);

    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        Message_StartTextbox(play, this->textId, &this->actor);
    } else {
        if ((player->actor.world.pos.x >= -50.0f) && (player->actor.world.pos.x <= 50.0f) &&
            (player->actor.world.pos.z >= -19.0f) && (player->actor.world.pos.z <= 30.0f)) {
            Actor_OfferTalk(&this->actor, play, 200.0f);
        }
    }
    if ((talkState == TEXT_STATE_DONE) && Message_ShouldAdvance(play)) {
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_WOODFALL_TEMPLE)) {
            play->nextEntrance = ENTRANCE(SOUTHERN_SWAMP_CLEARED, 5);
        } else {
            play->nextEntrance = ENTRANCE(SOUTHERN_SWAMP_POISONED, 5);
        }
        play->transitionType = TRANS_TYPE_64;
        gSaveContext.nextTransitionType = TRANS_TYPE_64;
        play->transitionTrigger = TRANS_TRIGGER_START;
    }
}

void EnTrt_CannotBuy(EnTrt* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        this->actionFunc = this->prevActionFunc;
        Message_ContinueTextbox(play, EnTrt_GetItemTextId(this));
    }
}

void EnTrt_CanBuy(EnTrt* this, PlayState* play) {
    EnGirlA* item;

    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        this->shopItemSelectedTween = 0.0f;
        EnTrt_ResetItemPosition(this);
        item = this->items[this->cursorIndex];
        item->restockFunc(play, item);
        this->actionFunc = this->prevActionFunc;
        Message_ContinueTextbox(play, EnTrt_GetItemTextId(this));
    }
}

void EnTrt_BuyItemWithFanfare(EnTrt* this, PlayState* play) {
    if (Actor_HasParent(&this->actor, play)) {
        this->actor.parent = NULL;
        this->actionFunc = EnTrt_SetupItemGiven;
    } else {
        Actor_OfferGetItem(&this->actor, play, this->items[this->cursorIndex]->getItemId, 300.0f, 300.0f);
    }
}

void EnTrt_SetupItemGiven(EnTrt* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_DONE) && Message_ShouldAdvance(play)) {
        this->actionFunc = EnTrt_ItemGiven;
        if (this->cutsceneState == ENTRT_CUTSCENESTATE_STOPPED) {
            if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
                CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
            }
            this->csId = this->lookToShopkeeperCsId;
            CutsceneManager_Queue(this->csId);
        }
        Actor_OfferTalkExchangeEquiCylinder(&this->actor, play, 400.0f, PLAYER_IA_MINUS1);
    }
}

void EnTrt_ContinueShopping(EnTrt* this, PlayState* play) {
    u8 talkState = Message_GetState(&play->msgCtx);
    Player* player = GET_PLAYER(play);
    EnGirlA* item;

    if (talkState == TEXT_STATE_CHOICE) {
        Interface_SetAButtonDoAction(play, DO_ACTION_DECIDE);
        if (Message_ShouldAdvance(play)) {
            EnTrt_ResetItemPosition(this);
            item = this->items[this->cursorIndex];
            item->restockFunc(play, item);
            if (!EnTrt_TestEndInteraction(this, play, CONTROLLER1(&play->state))) {
                switch (play->msgCtx.choiceIndex) {
                    case 0:
                        Audio_PlaySfx_MessageDecide();
                        player->actor.shape.rot.y = BINANG_ROT180(player->actor.shape.rot.y);
                        player->stateFlags2 |= PLAYER_STATE2_20000000;
                        Message_StartTextbox(play, this->textId, &this->actor);
                        EnTrt_SetupStartShopping(play, this, true);
                        Actor_OfferTalkExchangeEquiCylinder(&this->actor, play, 400.0f, PLAYER_IA_MINUS1);
                        break;

                    case 1:
                    default:
                        Audio_PlaySfx_MessageCancel();
                        EnTrt_EndInteraction(play, this);
                        break;
                }
            }
        }
    } else if (talkState == TEXT_STATE_EVENT) {
        if (Message_ShouldAdvance(play)) {
            EnTrt_ResetItemPosition(this);
            item = this->items[this->cursorIndex];
            item->restockFunc(play, item);
            EnTrt_EndInteraction(play, this);
        }
    }
}

void EnTrt_PositionSelectedItem(EnTrt* this) {
    static Vec3f sSelectedItemPosition = { 6.0f, 35.0f, -12.0f };
    u8 i = this->cursorIndex;
    EnGirlA* item;
    ShopItem* shopItem = &sShop[i];
    Vec3f worldPos;

    item = this->items[i];

    VEC3F_LERPIMPDST(&worldPos, &shopItem->spawnPos, &sSelectedItemPosition, this->shopItemSelectedTween);

    item->actor.world.pos.x = worldPos.x;
    item->actor.world.pos.y = worldPos.y;
    item->actor.world.pos.z = worldPos.z;
}

void EnTrt_ResetItemPosition(EnTrt* this) {
    this->shopItemSelectedTween = 0.0f;
    EnTrt_PositionSelectedItem(this);
}

/*
 * Returns true when animation has completed
 */
s32 EnTrt_TakeItemOffShelf(EnTrt* this) {
    Math_ApproachF(&this->shopItemSelectedTween, 1.0f, 1.0f, 0.15f);
    if (this->shopItemSelectedTween >= 0.85f) {
        this->shopItemSelectedTween = 1.0f;
    }
    EnTrt_PositionSelectedItem(this);
    if (this->shopItemSelectedTween == 1.0f) {
        return true;
    }
    return false;
}

/*
 * Returns true when animation has completed
 */
s32 EnTrt_ReturnItemToShelf(EnTrt* this) {
    Math_ApproachF(&this->shopItemSelectedTween, 0.0f, 1.0f, 0.15f);
    if (this->shopItemSelectedTween <= 0.15f) {
        this->shopItemSelectedTween = 0.0f;
    }
    EnTrt_PositionSelectedItem(this);
    if (this->shopItemSelectedTween == 0.0f) {
        return true;
    }
    return false;
}

void EnTrt_UpdateItemSelectedProperty(EnTrt* this) {
    EnGirlA** items = this->items;
    EnGirlA* item;
    s32 i;

    for (i = 0; i < ARRAY_COUNT(this->items); i++, items++) {
        item = *items;
        if (item != NULL) {
            if ((this->actionFunc != EnTrt_SelectItem) && (this->actionFunc != EnTrt_CannotBuy) &&
                (this->drawCursor == 0)) {
                item->isSelected = false;
            } else {
                item->isSelected = this->cursorIndex == i ? true : false;
            }
        }
    }
}

void EnTrt_UpdateCursorAnim(EnTrt* this) {
    f32 t = this->cursorAnimTween;

    if (this->cursorAnimState == 0) {
        t += 0.05f;
        if (t >= 1.0f) {
            t = 1.0f;
            this->cursorAnimState = 1;
        }
    } else {
        t -= 0.05f;
        if (t <= 0.0f) {
            t = 0.0f;
            this->cursorAnimState = 0;
        }
    }
    this->cursorColor.r = COL_CHAN_MIX(0, 0.0f, t);
    this->cursorColor.g = COL_CHAN_MIX(80, 80.0f, t);
    this->cursorColor.b = COL_CHAN_MIX(255, 0.0f, t);
    this->cursorColor.a = COL_CHAN_MIX(255, 0.0f, t);
    this->cursorAnimTween = t;
}

void EnTrt_UpdateStickDirectionPromptAnim(EnTrt* this) {
    f32 arrowAnimTween = this->arrowAnimTween;
    f32 stickAnimTween = this->stickAnimTween;
    s32 maxColor = 255; //! FAKE:

    if (this->arrowAnimState == 0) {
        arrowAnimTween += 0.05f;
        if (arrowAnimTween > 1.0f) {
            arrowAnimTween = 1.0f;
            this->arrowAnimState = 1;
        }
    } else {
        arrowAnimTween -= 0.05f;
        if (arrowAnimTween < 0.0f) {
            arrowAnimTween = 0.0f;
            this->arrowAnimState = 0;
        }
    }
    this->arrowAnimTween = arrowAnimTween;

    if (this->stickAnimState == 0) {
        stickAnimTween += 0.1f;
        if (stickAnimTween > 1.0f) {
            stickAnimTween = 1.0f;
            this->stickAnimState = 1;
        }
    } else {
        stickAnimTween = 0.0f;
        this->stickAnimState = 0;
    }
    this->stickAnimTween = stickAnimTween;

    this->stickLeftPrompt.arrowColor.r = COL_CHAN_MIX(255, 155.0f, arrowAnimTween);
    this->stickLeftPrompt.arrowColor.g = COL_CHAN_MIX(maxColor, 155.0f, arrowAnimTween);
    this->stickLeftPrompt.arrowColor.b = COL_CHAN_MIX(0, -100.0f, arrowAnimTween);
    this->stickLeftPrompt.arrowColor.a = COL_CHAN_MIX(200, 50.0f, arrowAnimTween);

    this->stickRightPrompt.arrowTexX = 290.0f;

    this->stickRightPrompt.arrowColor.r = COL_CHAN_MIX(maxColor, 155.0f, arrowAnimTween);
    this->stickRightPrompt.arrowColor.g = COL_CHAN_MIX(255, 155.0f, arrowAnimTween);
    this->stickRightPrompt.arrowColor.b = COL_CHAN_MIX(0, -100.0f, arrowAnimTween);
    this->stickRightPrompt.arrowColor.a = COL_CHAN_MIX(200, 50.0f, arrowAnimTween);

    this->stickLeftPrompt.arrowTexX = 33.0f;

    this->stickRightPrompt.stickTexX = 274.0f;
    this->stickRightPrompt.stickTexX += 8.0f * stickAnimTween;

    this->stickLeftPrompt.stickTexX = 49.0f;
    this->stickLeftPrompt.stickTexX -= 8.0f * stickAnimTween;

    this->stickRightPrompt.arrowTexY = 91.0f;
    this->stickLeftPrompt.arrowTexY = 91.0f;

    this->stickRightPrompt.stickTexY = 95.0f;
    this->stickLeftPrompt.stickTexY = 95.0f;
}

void EnTrt_OpenEyes(EnTrt* this) {
    if (this->eyeTexIndex <= 0) {
        this->eyeTexIndex = 0;
    } else {
        this->eyeTexIndex--;
    }
}

void EnTrt_CloseEyes(EnTrt* this) {
    if (this->eyeTexIndex >= 2) {
        this->eyeTexIndex = 2;
    } else {
        this->eyeTexIndex++;
    }
}

void EnTrt_Blink(EnTrt* this) {
    s16 decr = this->blinkTimer - 1;
    s16 eyeTexIndex;

    if (decr != 0) {
        this->blinkTimer = decr;
        return;
    }
    eyeTexIndex = this->eyeTexIndex + 1;
    if (eyeTexIndex > 2) {
        this->eyeTexIndex = 0;
        this->blinkTimer = (s32)(Rand_ZeroOne() * 60.0f) + 20;
    } else {
        this->eyeTexIndex = eyeTexIndex;
        this->blinkTimer = 1;
    }
}

void EnTrt_NodOff(EnTrt* this) {
    s16 decr = this->blinkTimer - 1;
    s16 eyeTexIndex;

    if (decr != 0) {
        this->blinkTimer = decr;
        return;
    }
    eyeTexIndex = this->eyeTexIndex + 1;
    if (eyeTexIndex > 2) {
        this->eyeTexIndex = 1;
        this->blinkTimer = 20;
    } else {
        this->eyeTexIndex = eyeTexIndex;
        this->blinkTimer = 3;
    }
}

void EnTrt_OpenThenCloseEyes(EnTrt* this) {
    if (this->skelAnime.curFrame >= 40.0f) {
        EnTrt_CloseEyes(this);
    } else if (this->skelAnime.curFrame >= 35.0f) {
        this->eyeTexIndex = 1;
    } else if (this->skelAnime.curFrame >= 10.0f) {
        EnTrt_OpenEyes(this);
    }
}

void EnTrt_EyesClosed(EnTrt* this) {
    this->eyeTexIndex = 2;
}

void EnTrt_OpenEyes2(EnTrt* this) {
    EnTrt_OpenEyes(this);
}

void EnTrt_OpenEyesThenSetToBlink(EnTrt* this) {
    if (this->skelAnime.curFrame >= 7.0f) {
        EnTrt_OpenEyes(this);
        if (this->eyeTexIndex == 0) {
            this->blinkFunc = EnTrt_Blink;
        }
    }
}

void EnTrt_TalkToShopkeeper(EnTrt* this, PlayState* play) {
    u8 talkState = Message_GetState(&play->msgCtx);
    Player* player = GET_PLAYER(play);
    PlayerItemAction itemAction;

    if (talkState == TEXT_STATE_EVENT) {
        if (Message_ShouldAdvance(play)) {
            if ((this->talkOptionTextId == 0x845) || (this->talkOptionTextId == 0x882)) {
                Message_ContinueTextbox(play, 0xFF);
            } else {
                EnTrt_StartShopping(play, this);
            }
        }
    } else if (talkState == TEXT_STATE_PAUSE_MENU) {
        itemAction = func_80123810(play);
        if (itemAction > PLAYER_IA_NONE) {
            if (itemAction == PLAYER_IA_BOTTLE_MUSHROOM) {
                if (CHECK_WEEKEVENTREG(WEEKEVENTREG_GAVE_KOTAKE_MUSHROOM)) {
                    player->actor.textId = 0x888;
                } else {
                    player->actor.textId = 0x883;
                }
                this->textId = player->actor.textId;
                player->exchangeItemAction = itemAction;
                this->actionFunc = EnTrt_GetMushroom;
            } else {
                if (this->flags & ENTRT_GIVEN_MUSHROOM) {
                    player->actor.textId = 0x88B;
                } else {
                    player->actor.textId = 0x886;
                }
                this->textId = player->actor.textId;
                this->actionFunc = EnTrt_Goodbye;
            }
            Message_CloseTextbox(play);
        } else if (itemAction <= PLAYER_IA_MINUS1) {
            if (this->flags & ENTRT_GIVEN_MUSHROOM) {
                this->textId = 0x88B;
            } else {
                this->textId = 0x886;
            }
            Message_StartTextbox(play, this->textId, &this->actor);
            this->actionFunc = EnTrt_Goodbye;
        }
    }
}

void EnTrt_SetupTalkToShopkeeper(PlayState* play, EnTrt* this) {
    this->actionFunc = EnTrt_TalkToShopkeeper;
    Message_ContinueTextbox(play, this->talkOptionTextId);
    Interface_SetAButtonDoAction(play, DO_ACTION_DECIDE);
    this->stickLeftPrompt.isEnabled = false;
    this->stickRightPrompt.isEnabled = false;
}

void EnTrt_SetupLookToShopkeeperFromShelf(PlayState* play, EnTrt* this) {
    Audio_PlaySfx(NA_SE_SY_CURSOR);
    this->drawCursor = 0;
    this->actionFunc = EnTrt_LookToShopkeeperFromShelf;
}

void EnTrt_LookToShopkeeperFromShelf(EnTrt* this, PlayState* play) {
    if (this->cutsceneState == ENTRT_CUTSCENESTATE_PLAYING) {
        CutsceneManager_Stop(this->csId);
        this->cutsceneState = ENTRT_CUTSCENESTATE_STOPPED;
    }
    if (this->cutsceneState == ENTRT_CUTSCENESTATE_STOPPED) {
        if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
            CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
        }
        this->csId = this->lookToShopkeeperCsId;
        CutsceneManager_Queue(this->csId);
        this->cutsceneState = ENTRT_CUTSCENESTATE_WAITING;
    } else if (this->cutsceneState == ENTRT_CUTSCENESTATE_WAITING) {
        if (CutsceneManager_IsNext(this->csId)) {
            CutsceneManager_StartWithPlayerCsAndSetFlag(this->csId, &this->actor);
            this->cutsceneState = ENTRT_CUTSCENESTATE_PLAYING;
            EnTrt_StartShopping(play, this);
        } else {
            CutsceneManager_Queue(this->csId);
        }
    }
}

void EnTrt_InitShopkeeper(EnTrt* this, PlayState* play) {
    SkelAnime_InitFlex(play, &this->skelAnime, &gKotakeSkel, &gKotakeSleepingAnim, NULL, NULL, 0);
    if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_SAVED_KOUME) &&
        !CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_RED_POTION_FOR_KOUME) && (gSaveContext.save.day >= 2)) {
        this->actor.draw = NULL;
    } else {
        this->actor.draw = EnTrt_Draw;
    }
}

void EnTrt_InitShop(EnTrt* this, PlayState* play) {
    EnTrt_GetCutscenes(this, play);
    this->csId = this->lookForwardCutscene;
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 20.0f);
    EnTrt_InitShopkeeper(this, play);
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    this->actor.colChkInfo.cylRadius = 50;
    this->timer = Rand_S16Offset(40, 20);
    if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_SAVED_KOUME) &&
        !CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_RED_POTION_FOR_KOUME) && gSaveContext.save.day >= 2) {
        this->textId = 0x84A;
        this->actionFunc = EnTrt_ShopkeeperGone;
    } else {
        this->sleepSoundTimer = 10;
        this->actor.textId = 0;
        this->actionFunc = EnTrt_IdleSleeping;
    }

    this->cursorPos.y = this->cursorPos.x = 100.0f;
    this->stickAccumY = 0;
    this->stickAccumX = 0;

    this->cursorIndex = 0;
    this->cursorPos.z = 1.2f;
    this->cursorColor.r = 0;
    this->cursorColor.g = 80;
    this->cursorColor.b = 255;
    this->cursorColor.a = 255;
    this->cursorAnimTween = 0.0f;
    this->cursorAnimState = 0;
    this->drawCursor = 0;

    this->stickLeftPrompt.stickColor.r = 200;
    this->stickLeftPrompt.stickColor.g = 200;
    this->stickLeftPrompt.stickColor.b = 200;
    this->stickLeftPrompt.stickColor.a = 180;
    this->stickLeftPrompt.stickTexX = 49.0f;
    this->stickLeftPrompt.stickTexY = 95.0f;
    this->stickLeftPrompt.arrowColor.r = 255;
    this->stickLeftPrompt.arrowColor.g = 255;
    this->stickLeftPrompt.arrowColor.b = 0;
    this->stickLeftPrompt.arrowColor.a = 200;
    this->stickLeftPrompt.arrowTexX = 33.0f;
    this->stickLeftPrompt.arrowTexY = 91.0f;
    this->stickLeftPrompt.texZ = 1.0f;
    this->stickLeftPrompt.isEnabled = false;

    this->stickRightPrompt.stickColor.r = 200;
    this->stickRightPrompt.stickColor.g = 200;
    this->stickRightPrompt.stickColor.b = 200;
    this->stickRightPrompt.stickColor.a = 180;
    this->stickRightPrompt.stickTexX = 274.0f;
    this->stickRightPrompt.stickTexY = 95.0f;
    this->stickRightPrompt.arrowColor.r = 255;
    this->stickRightPrompt.arrowColor.g = 0;
    this->stickRightPrompt.arrowColor.b = 0;
    this->stickRightPrompt.arrowColor.a = 200;
    this->stickRightPrompt.arrowTexX = 290.0f;
    this->stickRightPrompt.arrowTexY = 91.0f;
    this->stickRightPrompt.texZ = 1.0f;
    this->stickRightPrompt.isEnabled = false;

    this->arrowAnimTween = 0.0f;
    this->stickAnimTween = 0.0f;
    this->arrowAnimState = 0;
    this->stickAnimState = 0;
    this->shopItemSelectedTween = 0.0f;

    this->actor.gravity = 0.0f;
    Actor_SetScale(&this->actor, sActorScale);
    EnTrt_SpawnShopItems(this, play, sShop);
    this->blinkTimer = 20;
    this->eyeTexIndex = 0;
    this->blinkFunc = EnTrt_EyesClosed;
    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_GAVE_KOTAKE_MUSHROOM)) {
        this->flags |= ENTRT_GIVEN_MUSHROOM;
    }

    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
}

void EnTrt_GetCutscenes(EnTrt* this, PlayState* play) {
    this->lookForwardCutscene = this->actor.csId;
    this->lookToShelfCsId = CutsceneManager_GetAdditionalCsId(this->lookForwardCutscene);
    this->lookToShopkeeperCsId = CutsceneManager_GetAdditionalCsId(this->lookToShelfCsId);
    this->getMushroomCsId = CutsceneManager_GetAdditionalCsId(this->lookToShopkeeperCsId);
}

void EnTrt_DrawCursor(PlayState* play, EnTrt* this, f32 x, f32 y, f32 z, u8 drawCursor) {
    s32 ulx;
    s32 uly;
    s32 lrx;
    s32 lry;
    f32 w;
    s32 dsdx;
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx);

    if (drawCursor != 0) {
        Gfx_SetupDL39_Overlay(play->state.gfxCtx);
        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, this->cursorColor.r, this->cursorColor.g, this->cursorColor.b,
                        this->cursorColor.a);
        gDPLoadTextureBlock_4b(OVERLAY_DISP++, gSelectionCursorTex, G_IM_FMT_IA, 16, 16, 0, G_TX_MIRROR | G_TX_WRAP,
                               G_TX_MIRROR | G_TX_WRAP, 4, 4, G_TX_NOLOD, G_TX_NOLOD);
        w = 16.0f * z;
        ulx = (x - w) * 4.0f;
        uly = (y - w + -12.0f) * 4.0f;
        lrx = (x + w) * 4.0f;
        lry = (y + w + -12.0f) * 4.0f;
        dsdx = (1.0f / z) * 1024.0f;
        gSPTextureRectangle(OVERLAY_DISP++, ulx, uly, lrx, lry, G_TX_RENDERTILE, 0, 0, dsdx, dsdx);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnTrt_DrawTextRec(PlayState* play, s32 r, s32 g, s32 b, s32 a, f32 x, f32 y, f32 z, s32 s, s32 t, f32 dx, f32 dy) {
    f32 unk;
    s32 ulx;
    s32 uly;
    s32 lrx;
    s32 lry;
    f32 w;
    f32 h;
    s32 dsdx;
    s32 dtdy;

    (void)"../z_en_trt.c";

    OPEN_DISPS(play->state.gfxCtx);

    gDPPipeSync(OVERLAY_DISP++);
    gDPSetPrimColor(OVERLAY_DISP++, 0, 0, r, g, b, a);

    w = 8.0f * z;
    ulx = (x - w) * 4.0f;
    lrx = (x + w) * 4.0f;

    h = 12.0f * z;
    uly = (y - h) * 4.0f;
    lry = (y + h) * 4.0f;

    unk = 1024 * (1.0f / z);
    dsdx = unk * dx;
    dtdy = dy * unk;

    gSPTextureRectangle(OVERLAY_DISP++, ulx, uly, lrx, lry, G_TX_RENDERTILE, s, t, dsdx, dtdy);

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnTrt_DrawStickDirectionPrompt(PlayState* play, EnTrt* this) {
    s32 drawStickRightPrompt = this->stickLeftPrompt.isEnabled;
    s32 drawStickLeftPrompt = this->stickRightPrompt.isEnabled;

    if (CVarGetInteger("gModes.MirroredWorld.State", 0) && (drawStickLeftPrompt != drawStickRightPrompt)) {
        drawStickLeftPrompt = !drawStickLeftPrompt;
        drawStickRightPrompt = !drawStickRightPrompt;
    }

    (void)"../z_en_trt.c";

    OPEN_DISPS(play->state.gfxCtx);

    if (drawStickRightPrompt || drawStickLeftPrompt) {
        Gfx_SetupDL39_Overlay(play->state.gfxCtx);
        gDPSetCombineMode(OVERLAY_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
        gDPLoadTextureBlock(OVERLAY_DISP++, gArrowCursorTex, G_IM_FMT_IA, G_IM_SIZ_8b, 16, 24, 0,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 4, G_TX_NOMASK, G_TX_NOLOD,
                            G_TX_NOLOD);
        if (drawStickRightPrompt) {
            EnTrt_DrawTextRec(play, this->stickLeftPrompt.arrowColor.r, this->stickLeftPrompt.arrowColor.g,
                              this->stickLeftPrompt.arrowColor.b, this->stickLeftPrompt.arrowColor.a,
                              this->stickLeftPrompt.arrowTexX, this->stickLeftPrompt.arrowTexY,
                              this->stickLeftPrompt.texZ, 0, 0, -1.0f, 1.0f);
        }
        if (drawStickLeftPrompt) {
            EnTrt_DrawTextRec(play, this->stickRightPrompt.arrowColor.r, this->stickRightPrompt.arrowColor.g,
                              this->stickRightPrompt.arrowColor.b, this->stickRightPrompt.arrowColor.a,
                              this->stickRightPrompt.arrowTexX, this->stickRightPrompt.arrowTexY,
                              this->stickRightPrompt.texZ, 0, 0, 1.0f, 1.0f);
        }
        gDPLoadTextureBlock(OVERLAY_DISP++, gControlStickTex, G_IM_FMT_IA, G_IM_SIZ_8b, 16, 16, 0,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 4, G_TX_NOMASK, G_TX_NOLOD,
                            G_TX_NOLOD);
        if (drawStickRightPrompt) {
            EnTrt_DrawTextRec(play, this->stickLeftPrompt.stickColor.r, this->stickLeftPrompt.stickColor.g,
                              this->stickLeftPrompt.stickColor.b, this->stickLeftPrompt.stickColor.a,
                              this->stickLeftPrompt.stickTexX, this->stickLeftPrompt.stickTexY,
                              this->stickLeftPrompt.texZ, 0, 0, -1.0f, 1.0f);
        }
        if (drawStickLeftPrompt) {
            EnTrt_DrawTextRec(play, this->stickRightPrompt.stickColor.r, this->stickRightPrompt.stickColor.g,
                              this->stickRightPrompt.stickColor.b, this->stickRightPrompt.stickColor.a,
                              this->stickRightPrompt.stickTexX, this->stickRightPrompt.stickTexY,
                              this->stickRightPrompt.texZ, 0, 0, 1.0f, 1.0f);
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(lockOnArrowOffset, 500, ICHAIN_STOP),
};

void EnTrt_Init(Actor* thisx, PlayState* play) {
    EnTrt* this = (EnTrt*)thisx;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    EnTrt_InitShop(this, play);
}

void EnTrt_Destroy(Actor* thisx, PlayState* play) {
    EnTrt* this = (EnTrt*)thisx;

    SkelAnime_Free(&this->skelAnime, play);
    Collider_DestroyCylinder(play, &this->collider);
}

void EnTrt_Update(Actor* thisx, PlayState* play) {
    EnTrt* this = (EnTrt*)thisx;

    this->blinkFunc(this);
    EnTrt_UpdateJoystickInputState(play, this);
    EnTrt_UpdateItemSelectedProperty(this);
    EnTrt_UpdateStickDirectionPromptAnim(this);
    EnTrt_UpdateCursorAnim(this);
    EnTrt_TrackPlayer(this, play);
    this->actionFunc(this, play);
    Actor_SetFocus(&this->actor, 90.0f);
    SkelAnime_Update(&this->skelAnime);
    EnTrt_UpdateCollider(this, play);
}

void EnTrt_TrackPlayer(EnTrt* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Vec3f playerPos;
    Vec3f pos;

    Math_SmoothStepToS(&this->headYaw, this->actor.yawTowardsPlayer - this->actor.shape.rot.y, 4, 0x38E0, 1);
    this->headYaw = CLAMP(this->headYaw, -0x38E0, 0x38E0);

    playerPos = player->actor.world.pos;
    playerPos.y = player->bodyPartsPos[PLAYER_BODYPART_HEAD].y + 3.0f;
    pos = this->actor.world.pos;
    Math_SmoothStepToS(&this->headPitch, Math_Vec3f_Pitch(&pos, &playerPos), 4, 0x1C70, 1);
    this->headPitch = CLAMP(this->headPitch, -0x1C70, 0x1C70);
}

void EnTrt_UpdateLimb(s16 pitch, s16 yaw, Vec3f* pos, Vec3s* rot, s32 overrideRot) {
    Vec3f newPos;
    Vec3f zeroVec = gZeroVec3f;
    Vec3s newRot;
    MtxF currentState;

    Matrix_MultVec3f(&zeroVec, &newPos);
    Matrix_Get(&currentState);
    Matrix_MtxFToYXZRot(&currentState, &newRot, false);
    *pos = newPos;
    if (overrideRot) {
        newRot.x += pitch;
        newRot.y += yaw;
        Math_SmoothStepToS(&rot->x, newRot.x, 4, 0x1FFE, 1);
        Math_SmoothStepToS(&rot->y, newRot.y, 4, 0x1FFE, 1);
        Math_SmoothStepToS(&rot->z, newRot.z, 4, 0x1FFE, 1);
    } else {
        rot->x = newRot.x;
        rot->y = newRot.y;
        rot->z = newRot.z;
    }
}

s32 EnTrt_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnTrt* this = (EnTrt*)thisx;
    s32 i;

    for (i = 0; i < ARRAY_COUNT(this->items); i++) {
        this->items[i]->actor.scale.x = 0.2f;
        this->items[i]->actor.scale.y = 0.2f;
        this->items[i]->actor.scale.z = 0.2f;
    }

    if (limbIndex == KOTAKE_LIMB_BROOM) {
        *dList = NULL;
    }
    return false;
}

void EnTrt_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    EnTrt* this = (EnTrt*)thisx;
    s32 overrideRot;

    overrideRot = false;
    if (this->flags & ENTRT_FULLY_AWAKE) {
        overrideRot = true;
    }
    if (limbIndex == KOTAKE_LIMB_HEAD) {
        EnTrt_UpdateLimb(this->headPitch, this->headYaw, &this->headPos, &this->headRot, overrideRot);
        Matrix_Translate(this->headPos.x, this->headPos.y, this->headPos.z, MTXMODE_NEW);
        Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
        Matrix_RotateYS(this->headRot.y, MTXMODE_APPLY);
        Matrix_RotateXS(this->headRot.x, MTXMODE_APPLY);
        Matrix_RotateZS(this->headRot.z, MTXMODE_APPLY);
    }
}

void EnTrt_TransformLimbDraw(PlayState* play, s32 limbIndex, Actor* thisx) {
    EnTrt* this = (EnTrt*)thisx;

    if (limbIndex == KOTAKE_LIMB_HEAD) {
        Matrix_Translate(this->headPos.x, this->headPos.y, this->headPos.z, MTXMODE_NEW);
        Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
        Matrix_RotateYS(this->headRot.y, MTXMODE_APPLY);
        Matrix_RotateXS(this->headRot.x, MTXMODE_APPLY);
        Matrix_RotateZS(this->headRot.z, MTXMODE_APPLY);
    }
}

void EnTrt_Draw(Actor* thisx, PlayState* play) {
    static TexturePtr sEyeTextures[] = { gKotakeEyeOpenTex, gKotakeEyeHalfTex, gKotakeEyeClosedTex };
    EnTrt* this = (EnTrt*)thisx;
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(sEyeTextures[this->eyeTexIndex]));
    gSPSegment(POLY_OPA_DISP++, 0x09, Lib_SegmentedToVirtual(sEyeTextures[this->eyeTexIndex]));
    SkelAnime_DrawTransformFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                   this->skelAnime.dListCount, EnTrt_OverrideLimbDraw, EnTrt_PostLimbDraw,
                                   EnTrt_TransformLimbDraw, &this->actor);
    EnTrt_DrawCursor(play, this, this->cursorPos.x, this->cursorPos.y, this->cursorPos.z, this->drawCursor);
    EnTrt_DrawStickDirectionPrompt(play, this);

    CLOSE_DISPS(play->state.gfxCtx);
}
