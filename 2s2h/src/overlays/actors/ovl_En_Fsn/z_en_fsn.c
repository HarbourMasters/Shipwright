/*
 * File: z_en_fsn.c
 * Overlay: ovl_En_Fsn
 * Description: Curiosity Shop Man
 */

#include "z_en_fsn.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "2s2h/GameInteractor/GameInteractor.h"
#include <libultraship/bridge/consolevariablebridge.h>

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

#define SI_NONE 0

#define ENFSN_END_CONVERSATION (1 << 0)
#define ENFSN_GIVE_ITEM (1 << 1)
#define ENFSN_GAVE_KEATONS_MASK (1 << 2)
#define ENFSN_GAVE_LETTER_TO_MAMA (1 << 3)

void EnFsn_Init(Actor* thisx, PlayState* play);
void EnFsn_Destroy(Actor* thisx, PlayState* play);
void EnFsn_Update(Actor* thisx, PlayState* play);
void EnFsn_Draw(Actor* thisx, PlayState* play);

void EnFsn_Idle(EnFsn* this, PlayState* play);
void EnFsn_ConverseBackroom(EnFsn* this, PlayState* play);
void EnFsn_Haggle(EnFsn* this, PlayState* play);
void EnFsn_BeginInteraction(EnFsn* this, PlayState* play);
void EnFsn_GiveItem(EnFsn* this, PlayState* play);
void EnFsn_AskBuyOrSell(EnFsn* this, PlayState* play);
void EnFsn_StartBuying(EnFsn* this, PlayState* play);
void EnFsn_SetupEndInteractionImmediately(EnFsn* this, PlayState* play);
void EnFsn_SetupResumeInteraction(EnFsn* this, PlayState* play);
void EnFsn_FaceShopkeeperSelling(EnFsn* this, PlayState* play);
void EnFsn_DeterminePrice(EnFsn* this, PlayState* play);
void EnFsn_SetupDeterminePrice(EnFsn* this, PlayState* play);
void EnFsn_SetupEndInteraction(EnFsn* this, PlayState* play);
void EnFsn_MakeOffer(EnFsn* this, PlayState* play);
void EnFsn_ResumeInteraction(EnFsn* this, PlayState* play);
void EnFsn_BrowseShelf(EnFsn* this, PlayState* play);
void EnFsn_ResumeShoppingInteraction(EnFsn* this, PlayState* play);
void EnFsn_AskCanBuyMore(EnFsn* this, PlayState* play);
void EnFsn_AskCanBuyAterRunningOutOfItems(EnFsn* this, PlayState* play);
void EnFsn_SelectItem(EnFsn* this, PlayState* play);
void EnFsn_LookToShopkeeperFromShelf(EnFsn* this, PlayState* play);
void EnFsn_PlayerCannotBuy(EnFsn* this, PlayState* play);

typedef enum {
    /* 0 */ ENFSN_CUTSCENESTATE_STOPPED,
    /* 1 */ ENFSN_CUTSCENESTATE_WAITING,
    /* 2 */ ENFSN_CUTSCENESTATE_PLAYING
} EnFsnCutsceneState;

ActorProfile En_Fsn_Profile = {
    /**/ ACTOR_EN_FSN,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_FSN,
    /**/ sizeof(EnFsn),
    /**/ EnFsn_Init,
    /**/ EnFsn_Destroy,
    /**/ EnFsn_Update,
    /**/ EnFsn_Draw,
};

typedef enum {
    /*  0 */ FSN_ANIM_IDLE,
    /*  1 */ FSN_ANIM_SCRATCH_BACK,
    /*  2 */ FSN_ANIM_TURN_AROUND_FORWARD,
    /*  3 */ FSN_ANIM_TURN_AROUND_REVERSE,
    /*  4 */ FSN_ANIM_HANDS_ON_COUNTER_START,
    /*  5 */ FSN_ANIM_HANDS_ON_COUNTER_LOOP,
    /*  6 */ FSN_ANIM_HAND_ON_FACE_START,
    /*  7 */ FSN_ANIM_HAND_ON_FACE_LOOP,
    /*  8 */ FSN_ANIM_LEAN_FORWARD_START,
    /*  9 */ FSN_ANIM_LEAN_FORWARD_LOOP,
    /* 10 */ FSN_ANIM_SLAM_COUNTER_START,
    /* 11 */ FSN_ANIM_SLAM_COUNTER_LOOP,
    /* 12 */ FSN_ANIM_MAKE_OFFER,
    /* 13 */ FSN_ANIM_MAX
} FsnAnimation;

static AnimationInfoS sAnimationInfo[FSN_ANIM_MAX] = {
    { &gFsnIdleAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },                // FSN_ANIM_IDLE
    { &gFsnScratchBackAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },         // FSN_ANIM_SCRATCH_BACK
    { &gFsnTurnAroundAnim, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },          // FSN_ANIM_TURN_AROUND_FORWARD
    { &gFsnTurnAroundAnim, -1.0f, 0, -1, ANIMMODE_ONCE, 0 },         // FSN_ANIM_TURN_AROUND_REVERSE
    { &gFsnHandsOnCounterStartAnim, 1.0f, 0, -1, ANIMMODE_ONCE, 0 }, // FSN_ANIM_HANDS_ON_COUNTER_START
    { &gFsnHandsOnCounterLoopAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },  // FSN_ANIM_HANDS_ON_COUNTER_LOOP
    { &gFsnHandOnFaceStartAnim, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },     // FSN_ANIM_HAND_ON_FACE_START
    { &gFsnHandOnFaceLoopAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },      // FSN_ANIM_HAND_ON_FACE_LOOP
    { &gFsnLeanForwardStartAnim, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },    // FSN_ANIM_LEAN_FORWARD_START
    { &gFsnLeanForwardLoopAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },     // FSN_ANIM_LEAN_FORWARD_LOOP
    { &gFsnSlamCounterStartAnim, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },    // FSN_ANIM_SLAM_COUNTER_START
    { &gFsnSlamCounterLoopAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },     // FSN_ANIM_SLAM_COUNTER_LOOP
    { &gFsnMakeOfferAnim, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },           // FSN_ANIM_MAKE_OFFER
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

static Vec3f sShopItemPositions[3] = {
    { -5.0f, 35.0f, -95.0f },
    { 13.0f, 35.0f, -95.0f },
    { 31.0f, 35.0f, -95.0f },
};

bool EnFsn_TestItemSelected(PlayState* play) {
    MessageContext* msgCtx = &play->msgCtx;

    if ((msgCtx->textboxEndType == TEXTBOX_ENDTYPE_TWO_CHOICE) ||
        (msgCtx->textboxEndType == TEXTBOX_ENDTYPE_THREE_CHOICE)) {
        return CHECK_BTN_ALL(CONTROLLER1(&play->state)->press.button, BTN_A);
    }
    return CHECK_BTN_ALL(CONTROLLER1(&play->state)->press.button, BTN_A) ||
           CHECK_BTN_ALL(CONTROLLER1(&play->state)->press.button, BTN_B) ||
           CHECK_BTN_ALL(CONTROLLER1(&play->state)->press.button, BTN_CUP);
}

u16 EnFsn_GetWelcome(PlayState* play) {
    switch (Player_GetMask(play)) {
        case PLAYER_MASK_NONE:
            return 0x29CC;

        case PLAYER_MASK_DEKU:
            return 0x29FC;

        case PLAYER_MASK_GORON:
        case PLAYER_MASK_ZORA:
            return 0x29FD;

        case PLAYER_MASK_KAFEIS_MASK:
            return 0x2364;

        default:
            return 0x29FE;
    }
}

void EnFsn_HandleConversationBackroom(EnFsn* this, PlayState* play) {
    switch (this->textId) {
        case 0:
            if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_PRIORITY_MAIL)) {
                this->textId = 0x29E0;
                break;
            } else {
                this->textId = 0x29E4;
                this->flags |= ENFSN_END_CONVERSATION;
            }
            break;

        case 0x29E0:
            if (GameInteractor_Should(VB_GIVE_KEATON_MASK, true, this)) {
                if (INV_CONTENT(ITEM_MASK_KEATON) == ITEM_MASK_KEATON) {
                    this->flags |= ENFSN_GIVE_ITEM;
                    this->flags |= ENFSN_GAVE_LETTER_TO_MAMA;
                    this->getItemId = GI_LETTER_TO_MAMA;
                    SET_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_PRIORITY_MAIL);
                    this->textId = 0x29F1;
                    break;
                } else {
                    this->textId = 0x29E1;
                }
            }
            break;

        case 0x29E1:
            this->textId = 0x29E2;
            break;

        case 0x29E2:
            this->flags |= ENFSN_GIVE_ITEM;
            this->flags |= ENFSN_GAVE_KEATONS_MASK;
            this->getItemId = GI_MASK_KEATON;
            this->textId = 0x29E3;
            break;

        case 0x29E3:
            if (GameInteractor_Should(VB_GIVE_LETTER_TO_MAMA, true, this)) {
                this->flags |= ENFSN_GIVE_ITEM;
                this->flags |= ENFSN_GAVE_LETTER_TO_MAMA;
                this->getItemId = GI_LETTER_TO_MAMA;
                SET_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_PRIORITY_MAIL);
                this->textId = 0x29F1;
            }
            break;

        case 0x29F1:
            this->textId = 0x29E4;
            this->flags |= ENFSN_END_CONVERSATION;
            break;

        default:
            break;
    }
    Message_StartTextbox(play, this->textId, &this->actor);
    if (this->flags & ENFSN_END_CONVERSATION) {
        if (this->flags & ENFSN_GAVE_LETTER_TO_MAMA) {
            this->flags &= ~ENFSN_GAVE_LETTER_TO_MAMA;
            Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_RECEIVED_PRIORITY_MAIL);
        }
        if (this->flags & ENFSN_GAVE_KEATONS_MASK) {
            this->flags &= ~ENFSN_GAVE_KEATONS_MASK;
            Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_RECEIVED_KEATON_MASK);
        }
        Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_CURIOSITY_SHOP_MAN);
    }
}

void EnFsn_HandleSetupResumeInteraction(EnFsn* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_DONE) && Message_ShouldAdvance(play) &&
        (this->cutsceneState == ENFSN_CUTSCENESTATE_STOPPED)) {
        Actor_TalkOfferAccepted(&this->actor, &play->state);
        Actor_OfferTalkExchangeEquiCylinder(&this->actor, play, 400.0f, PLAYER_IA_MINUS1);
        if (ENFSN_IS_SHOP(&this->actor)) {
            this->actor.textId = 0;
        }
        this->actionFunc = EnFsn_ResumeInteraction;
    }
}

void EnFsn_UpdateCollider(EnFsn* this, PlayState* play) {
    this->collider.dim.pos.x = this->actor.world.pos.x;
    this->collider.dim.pos.y = this->actor.world.pos.y;
    this->collider.dim.pos.z = this->actor.world.pos.z;
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
}

void EnFsn_HandleLookToShopkeeperBuyingCutscene(EnFsn* this) {
    if ((this->cutsceneState == ENFSN_CUTSCENESTATE_PLAYING) && (this->lookToShopkeeperBuyingCsId != this->csId) &&
        (this->actor.textId == 0x29CE)) {
        CutsceneManager_Stop(this->csId);
        if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
            CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
        }
        this->csId = this->lookToShopkeeperBuyingCsId;
        CutsceneManager_Queue(this->csId);
        this->cutsceneState = ENFSN_CUTSCENESTATE_WAITING;
    } else if (this->cutsceneState == ENFSN_CUTSCENESTATE_WAITING) {
        if (CutsceneManager_IsNext(this->csId)) {
            CutsceneManager_Start(this->csId, &this->actor);
            this->cutsceneState = ENFSN_CUTSCENESTATE_PLAYING;
        } else {
            CutsceneManager_Queue(this->csId);
        }
    }
}

u8 EnFsn_SetCursorIndexFromNeutral(EnFsn* this) {
    if (this->itemIds[0] != -1) {
        return 0;
    }
    if (this->itemIds[1] != -1) {
        return 1;
    }
    if (this->itemIds[2] != -1) {
        return 2;
    }
    return CURSOR_INVALID;
}

void EnFsn_CursorLeftRight(EnFsn* this) {
    u8 cursorScan = this->cursorIndex;

    if (this->stickAccumX > 0) {
        if (cursorScan != this->totalSellingItems - 1) {
            while (cursorScan != this->totalSellingItems - 1) {
                cursorScan++;
                if (this->itemIds[cursorScan] != -1) {
                    this->cursorIndex = cursorScan;
                    break;
                }
            }
        } else if (this->itemIds[cursorScan] != -1) {
            this->cursorIndex = cursorScan;
        }
    } else if (this->stickAccumX < 0) {
        if (cursorScan != 0) {
            while (cursorScan != 0) {
                cursorScan--;
                if (this->itemIds[cursorScan] != -1) {
                    this->cursorIndex = cursorScan;
                    break;
                } else if (cursorScan == 0) {
                    Audio_PlaySfx(NA_SE_SY_CURSOR);
                    this->drawCursor = 0;
                    this->actionFunc = EnFsn_LookToShopkeeperFromShelf;
                    break;
                }
            }
        } else {
            Audio_PlaySfx(NA_SE_SY_CURSOR);
            this->drawCursor = 0;
            this->actionFunc = EnFsn_LookToShopkeeperFromShelf;
            if (this->itemIds[cursorScan] != -1) {
                this->cursorIndex = cursorScan;
            }
        }
    }
}

s16 EnFsn_GetSpecialItemId(void) {
    if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_BOUGHT_CURIOSITY_SHOP_SPECIAL_ITEM) && (CURRENT_DAY == 3)) {
        if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_RECOVERED_STOLEN_BOMB_BAG) &&
            !CHECK_WEEKEVENTREG(WEEKEVENTREG_SAKON_DEAD)) {
            return SI_BOMB_BAG_30_1;
        }
        return SI_MASK_ALL_NIGHT;
    }
    return SI_NONE;
}

s16 EnFsn_GetStolenItemId(u32 stolenItem) {
    switch (stolenItem) {
        case ITEM_BOTTLE:
            return SI_BOTTLE;

        case ITEM_SWORD_GREAT_FAIRY:
            return SI_SWORD_GREAT_FAIRY;

        case ITEM_SWORD_KOKIRI:
            return SI_SWORD_KOKIRI;

        case ITEM_SWORD_RAZOR:
            return SI_SWORD_RAZOR;

        case ITEM_SWORD_GILDED:
            return SI_SWORD_GILDED;

        default:
            return SI_NONE;
    }
}

s32 EnFsn_HasItemsToSell(void) {
    if (GameInteractor_Should(VB_EN_FSN_HAS_ITEMS, false)) {
        return true;
    }

    if (CURRENT_DAY != 3) {
        if ((STOLEN_ITEM_1 != STOLEN_ITEM_NONE) || (STOLEN_ITEM_2 != STOLEN_ITEM_NONE)) {
            return true;
        }
        return false;
    }

    if ((STOLEN_ITEM_1 != STOLEN_ITEM_NONE) || (STOLEN_ITEM_2 != STOLEN_ITEM_NONE) ||
        !CHECK_WEEKEVENTREG(WEEKEVENTREG_BOUGHT_CURIOSITY_SHOP_SPECIAL_ITEM)) {
        return true;
    }

    return false;
}

void EnFsn_GetShopItemIds(EnFsn* this) {
    u32 stolenItem1 = STOLEN_ITEM_1;
    u32 stolenItem2 = STOLEN_ITEM_2;
    s16 itemId;

    this->stolenItem1 = this->stolenItem2 = 0;
    this->itemIds[0] = this->itemIds[1] = this->itemIds[2] = 0;

    itemId = EnFsn_GetSpecialItemId();
    this->itemIds[this->totalSellingItems] = itemId;
    if (itemId != SI_NONE) {
        this->totalSellingItems++;
    }
    itemId = EnFsn_GetStolenItemId(stolenItem1);
    this->itemIds[this->totalSellingItems] = itemId;
    if (itemId != SI_NONE) {
        this->stolenItem1 = this->totalSellingItems;
        this->totalSellingItems++;
    }
    itemId = EnFsn_GetStolenItemId(stolenItem2);
    this->itemIds[this->totalSellingItems] = itemId;
    if (itemId != SI_NONE) {
        this->stolenItem2 = this->totalSellingItems;
        this->totalSellingItems++;
    }
    this->numSellingItems = this->totalSellingItems;
}

void EnFsn_SpawnShopItems(EnFsn* this, PlayState* play) {
    s32 i;

    EnFsn_GetShopItemIds(this);
    for (i = 0; i < this->totalSellingItems; i++) {
        if (this->itemIds[i] < 0) {
            this->items[i] = NULL;
        } else {
            this->items[i] =
                (EnGirlA*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_GIRLA, sShopItemPositions[i].x,
                                      sShopItemPositions[i].y, sShopItemPositions[i].z, 0, 0, 0, this->itemIds[i]);
        }
    }
}

void EnFsn_EndInteraction(EnFsn* this, PlayState* play) {
    if (this->cutsceneState == ENFSN_CUTSCENESTATE_PLAYING) {
        CutsceneManager_Stop(this->csId);
        this->cutsceneState = ENFSN_CUTSCENESTATE_STOPPED;
    }
    Actor_TalkOfferAccepted(&this->actor, &play->state);
    play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
    play->msgCtx.stateTimer = 4;
    Interface_SetHudVisibility(HUD_VISIBILITY_ALL);
    this->drawCursor = 0;
    this->stickLeftPrompt.isEnabled = false;
    this->stickRightPrompt.isEnabled = false;
    play->interfaceCtx.bButtonInterfaceDoActionActive = false;
    play->interfaceCtx.bButtonInterfaceDoAction = 0;
    this->actor.textId = 0;
    this->actionFunc = EnFsn_Idle;
}

s32 EnFsn_TestEndInteraction(EnFsn* this, PlayState* play, Input* input) {
    if (CHECK_BTN_ALL(input->press.button, BTN_B)) {
        this->actor.textId = (CURRENT_DAY == 3) ? 0x29DF : 0x29D1;
        Message_StartTextbox(play, this->actor.textId, &this->actor);
        Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_CURIOSITY_SHOP_MAN);
        this->actionFunc = EnFsn_SetupEndInteraction;
        return true;
    }
    return false;
}

s32 EnFsn_TestCancelOption(EnFsn* this, PlayState* play, Input* input) {
    if (CHECK_BTN_ALL(input->press.button, BTN_B)) {
        this->actionFunc = this->prevActionFunc;
        Message_ContinueTextbox(play, this->items[this->cursorIndex]->actor.textId);
        return true;
    }
    return false;
}

void EnFsn_UpdateCursorPos(EnFsn* this, PlayState* play) {
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

s32 EnFsn_FacingShopkeeperDialogResult(EnFsn* this, PlayState* play) {
    switch (play->msgCtx.choiceIndex) {
        case 0:
            Audio_PlaySfx_MessageDecide();
            if (CURRENT_DAY != 3) {
                this->actor.textId = 0x29FB;
            } else if (CHECK_WEEKEVENTREG(WEEKEVENTREG_BOUGHT_CURIOSITY_SHOP_SPECIAL_ITEM)) {
                this->actor.textId = 0x29FF;
            } else if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_RECOVERED_STOLEN_BOMB_BAG) &&
                       !CHECK_WEEKEVENTREG(WEEKEVENTREG_SAKON_DEAD)) {
                this->actor.textId = 0x29D7;
            } else {
                this->actor.textId = 0x29D8;
            }
            Message_StartTextbox(play, this->actor.textId, &this->actor);
            return true;

        case 1:
            Audio_PlaySfx_MessageCancel();
            this->actor.textId = (CURRENT_DAY == 3) ? 0x29DF : 0x29D1;
            Message_StartTextbox(play, this->actor.textId, &this->actor);
            Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_CURIOSITY_SHOP_MAN);
            this->actionFunc = EnFsn_SetupEndInteraction;
            return true;

        default:
            return false;
    }
}

s32 EnFsn_HasPlayerSelectedItem(EnFsn* this, PlayState* play, Input* input) {
    if (EnFsn_TestEndInteraction(this, play, input)) {
        return true;
    }
    if (EnFsn_TestItemSelected(play)) {
        if (!this->items[this->cursorIndex]->isOutOfStock) {
            this->prevActionFunc = this->actionFunc;
            Message_ContinueTextbox(play, this->items[this->cursorIndex]->choiceTextId);
            Audio_PlaySfx(NA_SE_SY_DECIDE);
            this->stickLeftPrompt.isEnabled = false;
            this->stickRightPrompt.isEnabled = false;
            this->drawCursor = 0;
            this->actionFunc = EnFsn_SelectItem;
        } else {
            Audio_PlaySfx(NA_SE_SY_ERROR);
        }
        return true;
    }
    return false;
}

void EnFsn_UpdateJoystickInputState(EnFsn* this, PlayState* play) {
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

void EnFsn_PositionSelectedItem(EnFsn* this) {
    Vec3f selectedItemPosition = { 13.0f, 38.0f, -71.0f };
    u8 i = this->cursorIndex;
    EnGirlA* item = this->items[i];
    Vec3f worldPos;

    VEC3F_LERPIMPDST(&worldPos, &sShopItemPositions[i], &selectedItemPosition, this->shopItemSelectedTween);

    item->actor.world.pos.x = worldPos.x;
    item->actor.world.pos.y = worldPos.y;
    item->actor.world.pos.z = worldPos.z;
}

/*
 *    Returns true if animation has completed
 */
s32 EnFsn_TakeItemOffShelf(EnFsn* this) {
    Math_ApproachF(&this->shopItemSelectedTween, 1.0f, 1.0f, 0.15f);
    if (this->shopItemSelectedTween >= 0.85f) {
        this->shopItemSelectedTween = 1.0f;
    }
    EnFsn_PositionSelectedItem(this);
    if (this->shopItemSelectedTween == 1.0f) {
        return true;
    }
    return false;
}

/*
 *    Returns true if animation has completed
 */
s32 EnFsn_ReturnItemToShelf(EnFsn* this) {
    Math_ApproachF(&this->shopItemSelectedTween, 0.0f, 1.0f, 0.15f);
    if (this->shopItemSelectedTween <= 0.15f) {
        this->shopItemSelectedTween = 0.0f;
    }
    EnFsn_PositionSelectedItem(this);
    if (this->shopItemSelectedTween == 0.0f) {
        return true;
    }
    return false;
}

void EnFsn_UpdateItemSelectedProperty(EnFsn* this) {
    EnGirlA** items;
    s32 i;

    for (items = this->items, i = 0; i < this->totalSellingItems; items++, i++) {
        if (this->actionFunc != EnFsn_SelectItem && this->actionFunc != EnFsn_PlayerCannotBuy &&
            this->drawCursor == 0) {
            (*items)->isSelected = false;
        } else {
            (*items)->isSelected = (i == this->cursorIndex) ? true : false;
        }
    }
}

void EnFsn_UpdateCursorAnim(EnFsn* this) {
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

void EnFsn_UpdateStickDirectionPromptAnim(EnFsn* this) {
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

void EnFsn_InitShop(EnFsn* this, PlayState* play) {
    if (EnFsn_HasItemsToSell()) {
        EnFsn_SpawnShopItems(this, play);

        this->cursorPos.y = this->cursorPos.x = 100.0f;
        this->stickAccumY = 0;
        this->stickAccumX = 0;

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
        this->stickLeftPrompt.isEnabled = 0;

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
        this->stickRightPrompt.isEnabled = 0;

        this->arrowAnimState = 0;
        this->stickAnimState = 0;
        this->stickAnimTween = this->arrowAnimTween = 0.0f;
    }
    this->blinkTimer = 20;
    this->animIndex = FSN_ANIM_HANDS_ON_COUNTER_START;
    this->eyeTexIndex = 0;
    SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, this->animIndex);
    this->actionFunc = EnFsn_Idle;
}

void EnFsn_Idle(EnFsn* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (this->animIndex == FSN_ANIM_HANDS_ON_COUNTER_START) {
        s16 curFrame = this->skelAnime.curFrame;
        s16 endFrame = Animation_GetLastFrame(sAnimationInfo[this->animIndex].animation);

        if (curFrame == endFrame) {
            this->animIndex = FSN_ANIM_HANDS_ON_COUNTER_LOOP;
            SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, this->animIndex);
        }
        return;
    }

    if (this->flags & ENFSN_HAGGLE) {
        this->actionFunc = EnFsn_Haggle;
        return;
    }

    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        if (this->cutsceneState == ENFSN_CUTSCENESTATE_STOPPED) {
            if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
                CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
            }
            this->csId = this->lookToShopkeeperCsId;
            CutsceneManager_Queue(this->csId);
            this->cutsceneState = ENFSN_CUTSCENESTATE_WAITING;
        }
        this->actor.textId = EnFsn_GetWelcome(play);
        Message_StartTextbox(play, this->actor.textId, &this->actor);
        player->actor.world.pos.x = 1.0f;
        player->actor.world.pos.z = -34.0f;
        this->actionFunc = EnFsn_BeginInteraction;
    } else if (((player->actor.world.pos.x >= -50.0f) && (player->actor.world.pos.x <= 15.0f)) &&
               (player->actor.world.pos.y > 0.0f) &&
               ((player->actor.world.pos.z >= -35.0f) && (player->actor.world.pos.z <= -20.0f))) {
        Actor_OfferTalk(&this->actor, play, 400.0f);
    }
}

void EnFsn_Haggle(EnFsn* this, PlayState* play) {
    s16 curFrame = this->skelAnime.curFrame;
    s16 endFrame = Animation_GetLastFrame(sAnimationInfo[this->animIndex].animation);

    if (this->flags & ENFSN_ANGRY) {
        this->flags &= ~ENFSN_ANGRY;
        this->animIndex = FSN_ANIM_SLAM_COUNTER_LOOP;
        SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, this->animIndex);
    } else {
        if ((this->animIndex == FSN_ANIM_SLAM_COUNTER_LOOP) && Animation_OnFrame(&this->skelAnime, 18.0f)) {
            Actor_PlaySfx(&this->actor, NA_SE_EV_HANKO);
        }
        if (this->flags & ENFSN_CALM_DOWN) {
            this->flags &= ~ENFSN_CALM_DOWN;
            this->animIndex = FSN_ANIM_HANDS_ON_COUNTER_LOOP;
            SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, this->animIndex);
        } else if (this->flags & ENFSN_OFFER_FINAL_PRICE) {
            this->flags &= ~ENFSN_OFFER_FINAL_PRICE;
            this->animIndex = FSN_ANIM_MAKE_OFFER;
            SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, this->animIndex);
        } else {
            if (this->animIndex == FSN_ANIM_MAKE_OFFER) {
                if (curFrame == endFrame) {
                    this->animIndex = FSN_ANIM_HANDS_ON_COUNTER_LOOP;
                    SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, this->animIndex);
                } else {
                    if (Animation_OnFrame(&this->skelAnime, 28.0f)) {
                        Actor_PlaySfx(&this->actor, NA_SE_EV_HANKO);
                    }
                    return;
                }
            }
            if (!(this->flags & ENFSN_HAGGLE)) {
                this->actionFunc = EnFsn_Idle;
            }
        }
    }
}

void EnFsn_BeginInteraction(EnFsn* this, PlayState* play) {
    if (this->cutsceneState == ENFSN_CUTSCENESTATE_WAITING) {
        if (CutsceneManager_IsNext(this->csId)) {
            CutsceneManager_StartWithPlayerCsAndSetFlag(this->csId, &this->actor);
            this->cutsceneState = ENFSN_CUTSCENESTATE_PLAYING;
            if (Player_GetMask(play) == PLAYER_MASK_NONE) {
                Interface_SetAButtonDoAction(play, DO_ACTION_NEXT);
                if (EnFsn_HasItemsToSell()) {
                    this->actionFunc = EnFsn_AskBuyOrSell;
                } else {
                    this->isSelling = false;
                    this->actionFunc = EnFsn_StartBuying;
                }
            } else {
                this->actionFunc = EnFsn_SetupEndInteractionImmediately;
            }
        } else {
            CutsceneManager_Queue(this->csId);
        }
    }
}

void EnFsn_StartBuying(EnFsn* this, PlayState* play) {
    u8 talkState = Message_GetState(&play->msgCtx);
    Player* player = GET_PLAYER(play);

    EnFsn_HandleLookToShopkeeperBuyingCutscene(this);
    if ((talkState == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        switch (this->actor.textId) {
            case 0x29CC:
                this->actor.textId = 0x29CD;
                Message_StartTextbox(play, this->actor.textId, &this->actor);
                break;

            case 0x29CD:
                this->actor.textId = 0x29CE;
                EnFsn_HandleLookToShopkeeperBuyingCutscene(this);
                Message_StartTextbox(play, this->actor.textId, &this->actor);
                break;

            case 0x29CE:
                this->actor.textId = 0xFF;
                Message_StartTextbox(play, this->actor.textId, &this->actor);
                this->actionFunc = EnFsn_DeterminePrice;
                break;

            case 0x29CF:
                player->exchangeItemAction = PLAYER_IA_NONE;
                this->actionFunc = EnFsn_SetupDeterminePrice;
                break;

            default:
                break;
        }
    }
}

void EnFsn_AskBuyOrSell(EnFsn* this, PlayState* play) {
    u8 talkState = Message_GetState(&play->msgCtx);

    if (talkState == TEXT_STATE_EVENT) {
        if (Message_ShouldAdvance(play)) {
            switch (this->actor.textId) {
                case 0x29CC:
                    if (CURRENT_DAY != 3) {
                        this->actor.textId = 0x29FA;
                    } else {
                        this->actor.textId = 0x29D2;
                    }
                    Message_StartTextbox(play, this->actor.textId, &this->actor);
                    break;

                case 0x29D2:
                    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_BOUGHT_CURIOSITY_SHOP_SPECIAL_ITEM)) {
                        this->actor.textId = 0x2A01;
                    } else if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_RECOVERED_STOLEN_BOMB_BAG) &&
                               !CHECK_WEEKEVENTREG(WEEKEVENTREG_SAKON_DEAD)) {
                        this->actor.textId = 0x29D3;
                    } else {
                        this->actor.textId = 0x29D4;
                    }
                    Message_StartTextbox(play, this->actor.textId, &this->actor);
                    break;

                case 0x29D3:
                case 0x29D4:
                case 0x29FA:
                case 0x2A01:
                    this->actor.textId = 0x29D5;
                    Message_StartTextbox(play, this->actor.textId, &this->actor);
                    break;

                default:
                    break;
            }
        }
    } else if (talkState == TEXT_STATE_CHOICE) {
        Interface_SetAButtonDoAction(play, DO_ACTION_DECIDE);
        if (!EnFsn_TestEndInteraction(this, play, CONTROLLER1(&play->state)) && Message_ShouldAdvance(play)) {
            switch (play->msgCtx.choiceIndex) {
                case 0:
                    Audio_PlaySfx_MessageDecide();
                    this->isSelling = true;
                    this->stickLeftPrompt.isEnabled = false;
                    this->stickRightPrompt.isEnabled = true;
                    this->actor.textId = 0x29D6;
                    Message_StartTextbox(play, this->actor.textId, &this->actor);
                    this->actionFunc = EnFsn_FaceShopkeeperSelling;
                    break;

                case 1:
                    Audio_PlaySfx_MessageDecide();
                    this->isSelling = false;
                    this->actor.textId = 0x29CE;
                    EnFsn_HandleLookToShopkeeperBuyingCutscene(this);
                    Message_StartTextbox(play, this->actor.textId, &this->actor);
                    this->actionFunc = EnFsn_StartBuying;
                    break;

                default:
                    break;
            }
        }
    }
}

void EnFsn_SetupDeterminePrice(EnFsn* this, PlayState* play) {
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->actor.textId = 0xFF;
        Message_StartTextbox(play, this->actor.textId, &this->actor);
        this->actionFunc = EnFsn_DeterminePrice;
    }
}

void EnFsn_DeterminePrice(EnFsn* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    PlayerItemAction itemAction;
    u8 buttonItem;

    if (Message_GetState(&play->msgCtx) == TEXT_STATE_PAUSE_MENU) {
        itemAction = func_80123810(play);

        if (itemAction > PLAYER_IA_NONE) {
            // #region 2S2H [Dpad]
            if (IS_HELD_DPAD(player->heldItemButton)) {
                buttonItem = DPAD_GET_CUR_FORM_BTN_ITEM(HELD_ITEM_TO_DPAD(player->heldItemButton));
            } else {
                buttonItem = GET_CUR_FORM_BTN_ITEM(player->heldItemButton);
            }
            // #endregion
            this->price = (buttonItem < ITEM_MOONS_TEAR) ? gItemPrices[buttonItem] : 0;
            if (this->price > 0) {
                player->actor.textId = 0x29EF;
                player->exchangeItemAction = buttonItem;
                this->actionFunc = EnFsn_MakeOffer;
            } else {
                player->actor.textId = 0x29CF;
                this->actionFunc = EnFsn_StartBuying;
            }
            this->actor.textId = player->actor.textId;
            Message_CloseTextbox(play);
        } else if (itemAction <= PLAYER_IA_MINUS1) {
            if (CURRENT_DAY == 3) {
                this->actor.textId = 0x29DF;
            } else {
                this->actor.textId = 0x29D1;
            }
            Message_StartTextbox(play, this->actor.textId, &this->actor);
            Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_CURIOSITY_SHOP_MAN);
            this->actionFunc = EnFsn_SetupEndInteraction;
        }
    }
}

void EnFsn_MakeOffer(EnFsn* this, PlayState* play) {
    u8 talkState = Message_GetState(&play->msgCtx);
    Player* player = GET_PLAYER(play);

    if (talkState == TEXT_STATE_CHOICE && Message_ShouldAdvance(play)) {
        switch (play->msgCtx.choiceIndex) {
            case 0:
                Audio_PlaySfx_MessageDecide();
                play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
                play->msgCtx.stateTimer = 4;
                if (this->cutsceneState == ENFSN_CUTSCENESTATE_PLAYING) {
                    CutsceneManager_Stop(this->csId);
                    this->cutsceneState = ENFSN_CUTSCENESTATE_STOPPED;
                }
                switch (this->price) {
                    case 5:
                        this->getItemId = GI_RUPEE_BLUE;
                        break;

                    case 10:
                        this->getItemId = GI_RUPEE_10;
                        break;

                    case 20:
                        this->getItemId = GI_RUPEE_RED;
                        break;

                    case 50:
                        this->getItemId = GI_RUPEE_PURPLE;
                        break;

                    case 200:
                        this->getItemId = GI_RUPEE_HUGE;
                        break;

                    default:
                        break;
                }
                this->actionFunc = EnFsn_GiveItem;
                break;

            case 1:
                Audio_PlaySfx_MessageCancel();
                player->exchangeItemAction = PLAYER_IA_NONE;
                this->actionFunc = EnFsn_SetupDeterminePrice;
                break;
        }
    }
}

void EnFsn_GiveItem(EnFsn* this, PlayState* play) {
    if (Actor_HasParent(&this->actor, play)) {
        if ((this->isSelling == true) && (this->items[this->cursorIndex]->getItemId == GI_MASK_ALL_NIGHT)) {
            Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_RECEIVED_ALL_NIGHT_MASK);
            Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_CURIOSITY_SHOP_MAN);
        }
        this->actor.parent = NULL;
        if (ENFSN_IS_SHOP(&this->actor) && !this->isSelling) {
            Player_UpdateBottleHeld(play, GET_PLAYER(play), ITEM_BOTTLE, PLAYER_IA_BOTTLE_EMPTY);
        }
        this->actionFunc = EnFsn_SetupResumeInteraction;
    } else if (this->isSelling == true) {
        Actor_OfferGetItem(&this->actor, play, this->items[this->cursorIndex]->getItemId, 300.0f, 300.0f);
    } else {
        Actor_OfferGetItem(&this->actor, play, this->getItemId, 300.0f, 300.0f);
    }
}

void EnFsn_SetupResumeInteraction(EnFsn* this, PlayState* play) {
    if (CHECK_QUEST_ITEM(QUEST_BOMBERS_NOTEBOOK)) {
        if (play->msgCtx.bombersNotebookEventQueueCount == 0) {
            EnFsn_HandleSetupResumeInteraction(this, play);
        }
    } else {
        EnFsn_HandleSetupResumeInteraction(this, play);
    }
}

void EnFsn_ResumeInteraction(EnFsn* this, PlayState* play) {
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        if (ENFSN_IS_SHOP(&this->actor)) {
            if (!this->isSelling) {
                this->csId = this->lookToShopkeeperBuyingCsId;
                this->actor.textId = 0x29D0;
            } else {
                this->csId = this->lookToShopkeeperCsId;
                this->actor.textId = (this->numSellingItems <= 0) ? 0x29DE : 0x29D6;
            }
            Message_StartTextbox(play, this->actor.textId, &this->actor);
            if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
                CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
            }
            CutsceneManager_Queue(this->csId);
            this->actionFunc = EnFsn_ResumeShoppingInteraction;
        } else {
            EnFsn_HandleConversationBackroom(this, play);
            this->actionFunc = EnFsn_ConverseBackroom;
        }
    } else {
        Actor_OfferTalkExchangeEquiCylinder(&this->actor, play, 400.0f, PLAYER_IA_MINUS1);
    }
}

void EnFsn_ResumeShoppingInteraction(EnFsn* this, PlayState* play) {
    if (this->cutsceneState == ENFSN_CUTSCENESTATE_STOPPED) {
        if (CutsceneManager_IsNext(this->csId)) {
            CutsceneManager_StartWithPlayerCsAndSetFlag(this->csId, &this->actor);
            this->cutsceneState = ENFSN_CUTSCENESTATE_PLAYING;
            if (!this->isSelling) {
                this->actionFunc = EnFsn_AskCanBuyMore;
            } else if (this->actor.textId != 0x29D6) {
                this->actionFunc = EnFsn_AskCanBuyAterRunningOutOfItems;
            } else {
                Interface_SetAButtonDoAction(play, DO_ACTION_DECIDE);
                this->stickLeftPrompt.isEnabled = false;
                this->stickRightPrompt.isEnabled = true;
                this->actionFunc = EnFsn_FaceShopkeeperSelling;
            }
        } else {
            if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
                CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
            }
            this->csId = !this->isSelling ? this->lookToShopkeeperBuyingCsId : this->lookToShopkeeperCsId;
            CutsceneManager_Queue(this->csId);
        }
    }
}

void EnFsn_LookToShelf(EnFsn* this, PlayState* play) {
    if (this->cutsceneState == ENFSN_CUTSCENESTATE_PLAYING) {
        CutsceneManager_Stop(this->csId);
        if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
            CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
        }
        this->csId = this->lookToShelfCsId;
        CutsceneManager_Queue(this->csId);
        this->cutsceneState = ENFSN_CUTSCENESTATE_WAITING;
    } else if (this->cutsceneState == ENFSN_CUTSCENESTATE_WAITING) {
        if (CutsceneManager_IsNext(this->csId)) {
            CutsceneManager_StartWithPlayerCsAndSetFlag(this->csId, &this->actor);
            this->cutsceneState = ENFSN_CUTSCENESTATE_PLAYING;
            EnFsn_UpdateCursorPos(this, play);
            this->actionFunc = EnFsn_BrowseShelf;
            Message_ContinueTextbox(play, this->items[this->cursorIndex]->actor.textId);
        } else {
            CutsceneManager_Queue(this->csId);
        }
    }
}

void EnFsn_BrowseShelf(EnFsn* this, PlayState* play) {
    u8 talkstate = Message_GetState(&play->msgCtx);
    s32 pad;
    u8 prevCursorIdx = this->cursorIndex;

    if (!EnFsn_ReturnItemToShelf(this)) {
        this->delayTimer = 3;
    } else if (this->delayTimer != 0) {
        this->delayTimer--;
    } else {
        this->drawCursor = 0xFF;
        this->stickLeftPrompt.isEnabled = true;
        EnFsn_UpdateCursorPos(this, play);
        if (talkstate == TEXT_STATE_EVENT) {
            Interface_SetAButtonDoAction(play, DO_ACTION_DECIDE);
            if (!EnFsn_HasPlayerSelectedItem(this, play, CONTROLLER1(&play->state))) {
                EnFsn_CursorLeftRight(this);
                if (this->cursorIndex != prevCursorIdx) {
                    Audio_PlaySfx(NA_SE_SY_CURSOR);
                    Message_ContinueTextbox(play, this->items[this->cursorIndex]->actor.textId);
                }
            }
        }
    }
}

void EnFsn_LookToShopkeeperFromShelf(EnFsn* this, PlayState* play) {
    if (this->cutsceneState == ENFSN_CUTSCENESTATE_PLAYING) {
        CutsceneManager_Stop(this->csId);
        if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
            CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
        }
        this->csId = this->lookToShopkeeperFromShelfCsId;
        CutsceneManager_Queue(this->csId);
        this->cutsceneState = ENFSN_CUTSCENESTATE_WAITING;
    } else if (this->cutsceneState == ENFSN_CUTSCENESTATE_WAITING) {
        if (CutsceneManager_IsNext(this->csId)) {
            CutsceneManager_StartWithPlayerCsAndSetFlag(this->csId, &this->actor);
            this->cutsceneState = ENFSN_CUTSCENESTATE_PLAYING;
            this->stickLeftPrompt.isEnabled = false;
            this->stickRightPrompt.isEnabled = true;
            this->actor.textId = 0x29D6;
            Message_ContinueTextbox(play, this->actor.textId);
            this->actionFunc = EnFsn_FaceShopkeeperSelling;
        } else {
            CutsceneManager_Queue(this->csId);
        }
    }
}

void EnFsn_HandleCanPlayerBuyItem(EnFsn* this, PlayState* play) {
    EnGirlA* item = this->items[this->cursorIndex];

    switch (item->canBuyFunc(play, item)) {
        case CANBUY_RESULT_SUCCESS_2:
            Audio_PlaySfx_MessageDecide();
            SET_WEEKEVENTREG(WEEKEVENTREG_BOUGHT_CURIOSITY_SHOP_SPECIAL_ITEM);
            // fallthrough
        case CANBUY_RESULT_SUCCESS_1:
            if (this->cutsceneState == ENFSN_CUTSCENESTATE_PLAYING) {
                CutsceneManager_Stop(this->csId);
                this->cutsceneState = ENFSN_CUTSCENESTATE_STOPPED;
            }
            Audio_PlaySfx_MessageDecide();
            item = this->items[this->cursorIndex];
            item->buyFanfareFunc(play, item);
            Actor_OfferGetItem(&this->actor, play, this->items[this->cursorIndex]->getItemId, 300.0f, 300.0f);
            play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
            play->msgCtx.stateTimer = 4;
            Interface_SetHudVisibility(HUD_VISIBILITY_ALL);
            this->drawCursor = 0;
            this->shopItemSelectedTween = 0.0f;
            item = this->items[this->cursorIndex];
            item->boughtFunc(play, item);
            if (this->stolenItem1 == this->cursorIndex) {
                SET_STOLEN_ITEM_1(STOLEN_ITEM_NONE);
            } else if (this->stolenItem2 == this->cursorIndex) {
                SET_STOLEN_ITEM_2(STOLEN_ITEM_NONE);
            }
            this->numSellingItems--;
            this->itemIds[this->cursorIndex] = -1;
            this->actionFunc = EnFsn_GiveItem;
            break;

        case CANBUY_RESULT_NEED_RUPEES:
            Audio_PlaySfx(NA_SE_SY_ERROR);
            Message_ContinueTextbox(play, 0x29F0);
            this->actionFunc = EnFsn_PlayerCannotBuy;
            break;

        case CANBUY_RESULT_CANNOT_GET_NOW:
            Audio_PlaySfx(NA_SE_SY_ERROR);
            Message_ContinueTextbox(play, 0x29DD);
            this->actionFunc = EnFsn_PlayerCannotBuy;
            break;

        default:
            break;
    }
}

void EnFsn_SetupEndInteraction(EnFsn* this, PlayState* play) {
    u8 talkState = Message_GetState(&play->msgCtx);

    if (((talkState == TEXT_STATE_EVENT) || (talkState == TEXT_STATE_DONE)) && Message_ShouldAdvance(play)) {
        if (CHECK_QUEST_ITEM(QUEST_BOMBERS_NOTEBOOK)) {
            if (play->msgCtx.bombersNotebookEventQueueCount == 0) {
                EnFsn_EndInteraction(this, play);
            } else {
                play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
                play->msgCtx.stateTimer = 4;
            }
        } else {
            EnFsn_EndInteraction(this, play);
        }
    }
}

void EnFsn_SelectItem(EnFsn* this, PlayState* play) {
    u8 talkState = Message_GetState(&play->msgCtx);

    if (EnFsn_TakeItemOffShelf(this) && (talkState == TEXT_STATE_CHOICE)) {
        Interface_SetAButtonDoAction(play, DO_ACTION_DECIDE);
        if (!EnFsn_TestCancelOption(this, play, CONTROLLER1(&play->state)) && Message_ShouldAdvance(play)) {
            switch (play->msgCtx.choiceIndex) {
                case 0:
                    EnFsn_HandleCanPlayerBuyItem(this, play);
                    break;

                case 1:
                    Audio_PlaySfx_MessageCancel();
                    this->actionFunc = this->prevActionFunc;
                    Message_ContinueTextbox(play, this->items[this->cursorIndex]->actor.textId);
                    break;

                default:
                    break;
            }
        }
    }
}

void EnFsn_PlayerCannotBuy(EnFsn* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        this->actionFunc = this->prevActionFunc;
        Message_ContinueTextbox(play, this->items[this->cursorIndex]->actor.textId);
    }
}

void EnFsn_AskCanBuyMore(EnFsn* this, PlayState* play) {
    u8 talkState = Message_GetState(&play->msgCtx);

    if (this->cutsceneState == ENFSN_CUTSCENESTATE_STOPPED) {
        if (CutsceneManager_IsNext(this->csId)) {
            CutsceneManager_StartWithPlayerCsAndSetFlag(this->csId, &this->actor);
            this->cutsceneState = ENFSN_CUTSCENESTATE_PLAYING;
        } else {
            if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
                CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
            }
            this->csId = this->lookToShopkeeperCsId;
            CutsceneManager_Queue(this->csId);
        }
    }
    if (talkState == TEXT_STATE_CHOICE) {
        if (Message_ShouldAdvance(play)) {
            switch (play->msgCtx.choiceIndex) {
                case 0:
                    Audio_PlaySfx_MessageDecide();
                    this->actor.textId = 0xFF;
                    Message_StartTextbox(play, this->actor.textId, &this->actor);
                    this->actionFunc = EnFsn_DeterminePrice;
                    break;

                case 1:
                    Audio_PlaySfx_MessageCancel();
                    this->actor.textId = (CURRENT_DAY == 3) ? 0x29DF : 0x29D1;
                    Message_StartTextbox(play, this->actor.textId, &this->actor);
                    Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_CURIOSITY_SHOP_MAN);
                    break;

                default:
                    break;
            }
        }
    } else if (((talkState == TEXT_STATE_EVENT) || (talkState == TEXT_STATE_DONE)) && Message_ShouldAdvance(play)) {
        if (CHECK_QUEST_ITEM(QUEST_BOMBERS_NOTEBOOK)) {
            if (play->msgCtx.bombersNotebookEventQueueCount == 0) {
                EnFsn_EndInteraction(this, play);
            } else {
                play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
                play->msgCtx.stateTimer = 4;
            }
        } else {
            EnFsn_EndInteraction(this, play);
        }
    }
}

void EnFsn_AskCanBuyAterRunningOutOfItems(EnFsn* this, PlayState* play) {
    u8 talkState = Message_GetState(&play->msgCtx);

    if (this->cutsceneState == ENFSN_CUTSCENESTATE_STOPPED) {
        if (CutsceneManager_IsNext(this->csId)) {
            CutsceneManager_StartWithPlayerCsAndSetFlag(this->csId, &this->actor);
            this->cutsceneState = ENFSN_CUTSCENESTATE_PLAYING;
        } else {
            if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
                CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
            }
            this->csId = this->lookToShopkeeperCsId;
            CutsceneManager_Queue(this->csId);
        }
    }
    if (talkState == TEXT_STATE_CHOICE) {
        if (Message_ShouldAdvance(play)) {
            switch (play->msgCtx.choiceIndex) {
                case 0:
                    Audio_PlaySfx_MessageDecide();
                    this->isSelling = false;
                    this->actor.textId = 0x29CE;
                    Message_StartTextbox(play, this->actor.textId, &this->actor);
                    this->actionFunc = EnFsn_StartBuying;
                    break;

                case 1:
                    Audio_PlaySfx_MessageCancel();
                    this->actor.textId = (CURRENT_DAY == 3) ? 0x29DF : 0x29D1;
                    Message_StartTextbox(play, this->actor.textId, &this->actor);
                    Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_CURIOSITY_SHOP_MAN);
                    break;

                default:
                    break;
            }
        }
    } else if (((talkState == TEXT_STATE_EVENT) || (talkState == TEXT_STATE_DONE)) && Message_ShouldAdvance(play)) {
        if (CHECK_QUEST_ITEM(QUEST_BOMBERS_NOTEBOOK)) {
            if (play->msgCtx.bombersNotebookEventQueueCount == 0) {
                EnFsn_EndInteraction(this, play);
            } else {
                play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
                play->msgCtx.stateTimer = 4;
            }
        } else {
            EnFsn_EndInteraction(this, play);
        }
    }
}

void EnFsn_FaceShopkeeperSelling(EnFsn* this, PlayState* play) {
    u8 talkState = Message_GetState(&play->msgCtx);
    u8 cursorIndex;

    if (talkState == TEXT_STATE_CHOICE) {
        Interface_SetAButtonDoAction(play, DO_ACTION_DECIDE);
        if (!EnFsn_TestEndInteraction(this, play, CONTROLLER1(&play->state)) &&
            (!Message_ShouldAdvance(play) || !EnFsn_FacingShopkeeperDialogResult(this, play)) &&
            this->stickAccumX > 0) {
            cursorIndex = EnFsn_SetCursorIndexFromNeutral(this);
            if (cursorIndex != CURSOR_INVALID) {
                this->cursorIndex = cursorIndex;
                this->actionFunc = EnFsn_LookToShelf;
                Interface_SetAButtonDoAction(play, DO_ACTION_DECIDE);
                this->stickRightPrompt.isEnabled = false;
                Audio_PlaySfx(NA_SE_SY_CURSOR);
            }
        }
    } else if ((talkState == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        this->actor.textId = 0x29D6;
        Message_StartTextbox(play, this->actor.textId, &this->actor);
        //! FAKE:
        if (play) {}
    }
}

void EnFsn_SetupEndInteractionImmediately(EnFsn* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        EnFsn_EndInteraction(this, play);
    }
}

void EnFsn_IdleBackroom(EnFsn* this, PlayState* play) {
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->textId = 0;
        EnFsn_HandleConversationBackroom(this, play);
        this->actionFunc = EnFsn_ConverseBackroom;
    } else if ((this->actor.xzDistToPlayer < 100.0f) || this->actor.isLockedOn) {
        Actor_OfferTalk(&this->actor, play, 100.0f);
    }
}

void EnFsn_ConverseBackroom(EnFsn* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        if (this->flags & ENFSN_END_CONVERSATION) {
            this->flags &= ~ENFSN_END_CONVERSATION;
            play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
            play->msgCtx.stateTimer = 4;
            this->actionFunc = EnFsn_IdleBackroom;
        } else if (this->flags & ENFSN_GIVE_ITEM) {
            this->flags &= ~ENFSN_GIVE_ITEM;
            play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
            play->msgCtx.stateTimer = 4;
            this->actionFunc = EnFsn_GiveItem;
        } else {
            EnFsn_HandleConversationBackroom(this, play);
        }
    }
}

void EnFsn_GetCutscenes(EnFsn* this) {
    this->lookToShopkeeperCsId = this->actor.csId;
    this->lookToShelfCsId = CutsceneManager_GetAdditionalCsId(this->lookToShopkeeperCsId);
    this->lookToShopkeeperFromShelfCsId = CutsceneManager_GetAdditionalCsId(this->lookToShelfCsId);
    this->lookToShopkeeperBuyingCsId = CutsceneManager_GetAdditionalCsId(this->lookToShopkeeperFromShelfCsId);
}

void EnFsn_Blink(EnFsn* this) {
    s16 decr = this->blinkTimer - 1;

    if (decr >= 3) {
        this->eyeTexIndex = 0;
        this->blinkTimer = decr;
    } else if (decr == 0) {
        this->eyeTexIndex = 2;
        this->blinkTimer = (s32)(Rand_ZeroOne() * 60.0f) + 20;
    } else {
        this->eyeTexIndex = 1;
        this->blinkTimer = decr;
    }
}

void EnFsn_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnFsn* this = (EnFsn*)thisx;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 20.0f);

    // #region 2S2H [Port] ENFSN_LIMB_MAX originally had a +1 because the game incorrectly thought the skel had 18 limbs
    // when it only has 17. We also need to patch the count in skelAnime because it is reading the wrong number from the
    // skeleton header.
    SkelAnime_InitFlex(play, &this->skelAnime, &gFsnSkel, &gFsnIdleAnim, this->jointTable, this->morphTable,
                       ENFSN_LIMB_MAX);
    this->skelAnime.limbCount = ENFSN_LIMB_MAX;

    if (ENFSN_IS_SHOP(&this->actor)) {
        this->actor.shape.rot.y = BINANG_ROT180(this->actor.shape.rot.y);
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        EnFsn_GetCutscenes(this);
        EnFsn_InitShop(this, play);
    } else {
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_RECOVERED_STOLEN_BOMB_BAG) || CHECK_WEEKEVENTREG(WEEKEVENTREG_SAKON_DEAD)) {
            Actor_Kill(&this->actor);
            return;
        }
        Collider_InitCylinder(play, &this->collider);
        Collider_InitAndSetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
        this->blinkTimer = 20;
        this->eyeTexIndex = 0;
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
        this->actor.attentionRangeType = ATTENTION_RANGE_0;
        this->animIndex = FSN_ANIM_IDLE;
        SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, this->animIndex);
        this->actionFunc = EnFsn_IdleBackroom;
    }
}

void EnFsn_Destroy(Actor* thisx, PlayState* play) {
    EnFsn* this = (EnFsn*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void EnFsn_Update(Actor* thisx, PlayState* play) {
    EnFsn* this = (EnFsn*)thisx;

    this->actionFunc(this, play);
    Actor_MoveWithGravity(&this->actor);
    Actor_TrackPlayer(play, &this->actor, &this->headRot, &this->torsoRot, this->actor.focus.pos);
    SubS_UpdateFidgetTables(play, this->fidgetTableY, this->fidgetTableZ, ENFSN_LIMB_MAX);
    EnFsn_Blink(this);
    if (ENFSN_IS_SHOP(&this->actor) && EnFsn_HasItemsToSell()) {
        EnFsn_UpdateJoystickInputState(this, play);
        EnFsn_UpdateItemSelectedProperty(this);
        EnFsn_UpdateStickDirectionPromptAnim(this);
        EnFsn_UpdateCursorAnim(this);
    }
    SkelAnime_Update(&this->skelAnime);
    if (ENFSN_IS_BACKROOM(&this->actor)) {
        EnFsn_UpdateCollider(this, play);
    }
}

void EnFsn_DrawCursor(EnFsn* this, PlayState* play, f32 x, f32 y, f32 z, u8 drawCursor) {
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

void EnFsn_DrawTextRec(PlayState* play, s32 r, s32 g, s32 b, s32 a, f32 x, f32 y, f32 z, s32 s, s32 t, f32 dx, f32 dy) {
    f32 unk;
    s32 ulx;
    s32 uly;
    s32 lrx;
    s32 lry;
    f32 w;
    f32 h;
    s32 dsdx;
    s32 dtdy;

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

void EnFsn_DrawStickDirectionPrompts(EnFsn* this, PlayState* play) {
    s32 drawStickRightPrompt = this->stickLeftPrompt.isEnabled;
    s32 drawStickLeftPrompt = this->stickRightPrompt.isEnabled;

    if (CVarGetInteger("gModes.MirroredWorld.State", 0) && (drawStickLeftPrompt != drawStickRightPrompt)) {
        drawStickLeftPrompt = !drawStickLeftPrompt;
        drawStickRightPrompt = !drawStickRightPrompt;
    }

    OPEN_DISPS(play->state.gfxCtx);

    if (drawStickRightPrompt || drawStickLeftPrompt) {
        Gfx_SetupDL39_Overlay(play->state.gfxCtx);
        gDPSetCombineMode(OVERLAY_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
        gDPLoadTextureBlock(OVERLAY_DISP++, gArrowCursorTex, G_IM_FMT_IA, G_IM_SIZ_8b, 16, 24, 0,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 4, G_TX_NOMASK, G_TX_NOLOD,
                            G_TX_NOLOD);
        if (drawStickRightPrompt) {
            EnFsn_DrawTextRec(play, this->stickLeftPrompt.arrowColor.r, this->stickLeftPrompt.arrowColor.g,
                              this->stickLeftPrompt.arrowColor.b, this->stickLeftPrompt.arrowColor.a,
                              this->stickLeftPrompt.arrowTexX, this->stickLeftPrompt.arrowTexY,
                              this->stickLeftPrompt.texZ, 0, 0, -1.0f, 1.0f);
        }
        if (drawStickLeftPrompt) {
            EnFsn_DrawTextRec(play, this->stickRightPrompt.arrowColor.r, this->stickRightPrompt.arrowColor.g,
                              this->stickRightPrompt.arrowColor.b, this->stickRightPrompt.arrowColor.a,
                              this->stickRightPrompt.arrowTexX, this->stickRightPrompt.arrowTexY,
                              this->stickRightPrompt.texZ, 0, 0, 1.0f, 1.0f);
        }
        gDPLoadTextureBlock(OVERLAY_DISP++, gControlStickTex, G_IM_FMT_IA, G_IM_SIZ_8b, 16, 16, 0,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 4, G_TX_NOMASK, G_TX_NOLOD,
                            G_TX_NOLOD);
        if (drawStickRightPrompt) {
            EnFsn_DrawTextRec(play, this->stickLeftPrompt.stickColor.r, this->stickLeftPrompt.stickColor.g,
                              this->stickLeftPrompt.stickColor.b, this->stickLeftPrompt.stickColor.a,
                              this->stickLeftPrompt.stickTexX, this->stickLeftPrompt.stickTexY,
                              this->stickLeftPrompt.texZ, 0, 0, -1.0f, 1.0f);
        }
        if (drawStickLeftPrompt) {
            EnFsn_DrawTextRec(play, this->stickRightPrompt.stickColor.r, this->stickRightPrompt.stickColor.g,
                              this->stickRightPrompt.stickColor.b, this->stickRightPrompt.stickColor.a,
                              this->stickRightPrompt.stickTexX, this->stickRightPrompt.stickTexY,
                              this->stickRightPrompt.texZ, 0, 0, 1.0f, 1.0f);
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

s32 EnFsn_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnFsn* this = (EnFsn*)thisx;
    s32 fidgetIndex;

    if (limbIndex == FSN_LIMB_HEAD) {
        Matrix_RotateXS(this->headRot.y, MTXMODE_APPLY);
    }
    if (ENFSN_IS_BACKROOM(&this->actor)) {
        switch (limbIndex) {
            case FSN_LIMB_TORSO:
                fidgetIndex = 0;
                break;

            case FSN_LIMB_LEFT_HAND:
                fidgetIndex = 1;
                break;

            case FSN_LIMB_HEAD:
                fidgetIndex = 2;
                break;

            default:
                fidgetIndex = 9;
                break;
        }
        if (fidgetIndex < 9) {
            rot->y += TRUNCF_BINANG(Math_SinS(this->fidgetTableY[fidgetIndex]) * 200.0f);
            rot->z += TRUNCF_BINANG(Math_CosS(this->fidgetTableZ[fidgetIndex]) * 200.0f);
        }
    }
    if (limbIndex == FSN_LIMB_TOUPEE) {
        *dList = NULL;
    }
    return false;
}

void EnFsn_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    EnFsn* this = (EnFsn*)thisx;

    if (limbIndex == FSN_LIMB_HEAD) {
        this->actor.focus.pos.x = this->actor.world.pos.x;
        this->actor.focus.pos.y = this->actor.world.pos.y + 60.0f;
        this->actor.focus.pos.z = this->actor.world.pos.z;

        OPEN_DISPS(play->state.gfxCtx);

        gSPDisplayList(POLY_OPA_DISP++, gFsnGlassesFrameDL);
        gSPDisplayList(POLY_OPA_DISP++, gFsnGlassesLensesDL);

        CLOSE_DISPS(play->state.gfxCtx);
    }
}

void EnFsn_Draw(Actor* thisx, PlayState* play) {
    static TexturePtr sEyeTextures[] = { gFsnEyeOpenTex, gFsnEyeHalfTex, gFsnEyeClosedTex };
    s32 pad;
    EnFsn* this = (EnFsn*)thisx;
    s16 i;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL37_Opa(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(sEyeTextures[this->eyeTexIndex]));
    gSPSegment(POLY_OPA_DISP++, 0x09, Lib_SegmentedToVirtual(sEyeTextures[this->eyeTexIndex]));
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnFsn_OverrideLimbDraw, EnFsn_PostLimbDraw, &this->actor);

    for (i = 0; i < this->totalSellingItems; i++) {
        this->items[i]->actor.scale.x = 0.2f;
        this->items[i]->actor.scale.y = 0.2f;
        this->items[i]->actor.scale.z = 0.2f;
    }

    EnFsn_DrawCursor(this, play, this->cursorPos.x, this->cursorPos.y, this->cursorPos.z, this->drawCursor);
    EnFsn_DrawStickDirectionPrompts(this, play);

    CLOSE_DISPS(play->state.gfxCtx);
}
