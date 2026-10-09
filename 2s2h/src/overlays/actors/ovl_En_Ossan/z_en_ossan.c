/*
 * File: z_en_ossan.c
 * Overlay: ovl_En_Ossan
 * Description: Trading Post Shop
 */

#include "z_en_ossan.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "2s2h/GameInteractor/GameInteractor.h"
#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/BenGui/CosmeticEditor.h"
#include <libultraship/bridge/consolevariablebridge.h>

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

#define LOOKED_AT_PLAYER (1 << 0)
#define END_INTERACTION (1 << 1)

void EnOssan_Init(Actor* thisx, PlayState* play);
void EnOssan_Destroy(Actor* thisx, PlayState* play);
void EnOssan_Update(Actor* thisx, PlayState* play);

void EnOssan_CuriosityShopMan_Draw(Actor* thisx, PlayState* play);
void EnOssan_PartTimer_Draw(Actor* thisx, PlayState* play);

u16 EnOssan_CuriosityShopMan_GetWelcome(EnOssan* this, PlayState* play);

u16 EnOssan_PartTimer_GetWelcome(EnOssan* this, PlayState* play);

void EnOssan_InitShop(EnOssan* this, PlayState* play);
void EnOssan_Idle(EnOssan* this, PlayState* play);
void EnOssan_BeginInteraction(EnOssan* this, PlayState* play);
void EnOssan_Hello(EnOssan* this, PlayState* play);
void EnOssan_SetTalked(EnOssan* this);
void EnOssan_StartShopping(PlayState* play, EnOssan* this);
void EnOssan_FaceShopkeeper(EnOssan* this, PlayState* play);
void EnOssan_LookToShopkeeperFromShelf(EnOssan* this, PlayState* play);
void EnOssan_TalkToShopkeeper(EnOssan* this, PlayState* play);
void EnOssan_LookToLeftShelf(EnOssan* this, PlayState* play);
void EnOssan_LookToRightShelf(EnOssan* this, PlayState* play);
void EnOssan_BrowseLeftShelf(EnOssan* this, PlayState* play);
void EnOssan_BrowseRightShelf(EnOssan* this, PlayState* play);
void EnOssan_SelectItem(EnOssan* this, PlayState* play);
void EnOssan_BuyItemWithFanfare(EnOssan* this, PlayState* play);
void EnOssan_CannotBuy(EnOssan* this, PlayState* play);
void EnOssan_CanBuy(EnOssan* this, PlayState* play);
void EnOssan_SetupItemPurchased(EnOssan* this, PlayState* play);
void EnOssan_ItemPurchased(EnOssan* this, PlayState* play);
void EnOssan_ResetItemPosition(EnOssan* this);
void EnOssan_Blink(EnOssan* this);
void EnOssan_GetCutscenes(EnOssan* this, PlayState* play);

s32 EnOssan_ReturnItemToShelf(EnOssan* this);
s32 EnOssan_TakeItemOffShelf(EnOssan* this);

typedef enum EnOssanCutsceneState {
    /* 0 */ ENOSSAN_CUTSCENESTATE_STOPPED,
    /* 1 */ ENOSSAN_CUTSCENESTATE_WAITING,
    /* 2 */ ENOSSAN_CUTSCENESTATE_PLAYING
} EnOssanCutsceneState;

ActorProfile En_Ossan_Profile = {
    /**/ ACTOR_EN_OSSAN,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(EnOssan),
    /**/ EnOssan_Init,
    /**/ EnOssan_Destroy,
    /**/ EnOssan_Update,
    /**/ NULL,
};

typedef enum FsnAnimation {
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

static AnimationInfoS sCuriosityShopManAnimationInfo[FSN_ANIM_MAX] = {
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

typedef enum AniAnimation {
    /*  0 */ ANI_ANIM_STANDING_NORMAL_LOOP_1,
    /*  1 */ ANI_ANIM_STANDING_NORMAL_LOOP_2,
    /*  2 */ ANI_ANIM_STANDING_NORMAL_ONCE_FORWARD_1,
    /*  3 */ ANI_ANIM_STANDING_NORMAL_ONCE_REVERSE,
    /*  4 */ ANI_ANIM_STANDING_NORMAL_ONCE_FORWARD_2,
    /*  5 */ ANI_ANIM_STANDING_NORMAL_LOOP_3,
    /*  6 */ ANI_ANIM_STANDING_NORMAL_ONCE_FORWARD_3,
    /*  7 */ ANI_ANIM_STANDING_NORMAL_LOOP_4,
    /*  8 */ ANI_ANIM_APOLOGY_START,
    /*  9 */ ANI_ANIM_APOLOGY_LOOP,
    /* 10 */ ANI_ANIM_STANDING_NORMAL_ONCE_FORWARD_4,
    /* 11 */ ANI_ANIM_STANDING_NORMAL_LOOP_5,
    /* 12 */ ANI_ANIM_MAX
} AniAnimation;

static AnimationInfoS sPartTimerAnimationInfo[ANI_ANIM_MAX] = {
    { &gAniStandingNormalAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -10 },       // ANI_ANIM_STANDING_NORMAL_LOOP_1
    { &gAniStandingNormalAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -10 },       // ANI_ANIM_STANDING_NORMAL_LOOP_2
    { &gAniStandingNormalAnim, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },         // ANI_ANIM_STANDING_NORMAL_ONCE_FORWARD_1
    { &gAniStandingNormalAnim, -1.0f, 0, -1, ANIMMODE_ONCE, 0 },        // ANI_ANIM_STANDING_NORMAL_ONCE_REVERSE
    { &gAniStandingNormalAnim, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },         // ANI_ANIM_STANDING_NORMAL_ONCE_FORWARD_2
    { &gAniStandingNormalAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },         // ANI_ANIM_STANDING_NORMAL_LOOP_3
    { &gAniStandingNormalAnim, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },         // ANI_ANIM_STANDING_NORMAL_ONCE_FORWARD_3
    { &gAniStandingNormalAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },         // ANI_ANIM_STANDING_NORMAL_LOOP_4
    { &gAniHandBehindHeadApologyAnim, 1.0f, 0, -1, ANIMMODE_ONCE, -5 }, // ANI_ANIM_APOLOGY_START
    { &gAniHandBehindHeadApologyAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -5 }, // ANI_ANIM_APOLOGY_LOOP
    { &gAniStandingNormalAnim, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },         // ANI_ANIM_STANDING_NORMAL_ONCE_FORWARD_4
    { &gAniStandingNormalAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },         // ANI_ANIM_STANDING_NORMAL_LOOP_5
};

static s16 sObjectIds[] = { OBJECT_FSN, OBJECT_ANI };

static AnimationInfoS* sAnimationInfoList[] = { sCuriosityShopManAnimationInfo, sPartTimerAnimationInfo };

static f32 sActorScales[] = { 0.01f, 0.01f };

static ShopItem sShops[][8] = {
    { { SI_NUTS_1, { 50, 68, -195 } },
      { SI_STICK_1, { 50, 92, -195 } },
      { SI_ARROWS_LARGE_1, { 80, 68, -195 } },
      { SI_ARROWS_MEDIUM_1, { 80, 92, -195 } },
      { SI_FAIRY_1, { -50, 68, -195 } },
      { SI_POTION_GREEN_2, { -50, 92, -195 } },
      { SI_SHIELD_HERO_1, { -80, 68, -195 } },
      { SI_POTION_RED_2, { -80, 92, -195 } } },

    { { SI_NUTS_2, { 50, 68, -195 } },
      { SI_STICK_2, { 50, 92, -195 } },
      { SI_ARROWS_LARGE_2, { 80, 68, -195 } },
      { SI_ARROWS_MEDIUM_2, { 80, 92, -195 } },
      { SI_FAIRY_2, { -50, 68, -195 } },
      { SI_POTION_GREEN_3, { -50, 92, -195 } },
      { SI_SHIELD_HERO_2, { -80, 68, -195 } },
      { SI_POTION_RED_3, { -80, 92, -195 } } },
};

static u16 sWelcomeHumanTextIds[] = { 0x06A4, 0x06C1 };

static u16 sTalkOptionTextIds[] = { 0x06AB, 0x06C8 };

static u16 sWelcomeGoronFirstTimeTextIds[] = { 0x06A5, 0x06C2 };

static u16 sWelcomeZoraFirstTimeTextIds[] = { 0x06A7, 0x06C4 };

static u16 sWelcomeDekuFirstTimeTextIds[] = { 0x06A9, 0x06C6 };

static u16 sWelcomeGoronTextIds[] = { 0x06A6, 0x06C3 };

static u16 sWelcomeZoraTextIds[] = { 0x06A8, 0x06C5 };

static u16 sWelcomeDekuTextIds[] = { 0x06AA, 0x06C7 };

static u16 sNeedEmptyBottleTextIds[] = { 0x06BC, 0x06D9 };

static u16 sNeedRupeesTextIds[] = { 0x06BD, 0x06DA };

static u16 sNoRoomTextIds[] = { 0x06BE, 0x06DB };

static u16 sBuySuccessTextIds[] = { 0x06BF, 0x06DC };

static u16 sCannotGetNowTextIds[] = { 0x06C0, 0x06DD };

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(lockOnArrowOffset, 500, ICHAIN_STOP),
};

void EnOssan_SetupAction(EnOssan* this, EnOssanActionFunc action) {
    this->actionFunc = action;
}

bool EnOssan_TestItemSelected(PlayState* play) {
    MessageContext* msgCtx = &play->msgCtx;

    if ((msgCtx->textboxEndType == TEXTBOX_ENDTYPE_TWO_CHOICE) ||
        (msgCtx->textboxEndType == TEXTBOX_ENDTYPE_THREE_CHOICE)) {
        return CHECK_BTN_ALL(CONTROLLER1(&play->state)->press.button, BTN_A);
    }
    return CHECK_BTN_ALL(CONTROLLER1(&play->state)->press.button, BTN_A) ||
           CHECK_BTN_ALL(CONTROLLER1(&play->state)->press.button, BTN_B) ||
           CHECK_BTN_ALL(CONTROLLER1(&play->state)->press.button, BTN_CUP);
}

void EnOssan_CheckValidSpawn(EnOssan* this) {
    switch (gSaveContext.save.day) {
        case 1:
        case 2:
            if ((CURRENT_TIME <= CLOCK_TIME(21, 30)) && (CURRENT_TIME > CLOCK_TIME(6, 0))) {
                if (this->actor.params != ENOSSAN_CURIOSITY_SHOP_MAN) {
                    Actor_Kill(&this->actor);
                }
            } else if (this->actor.params == ENOSSAN_CURIOSITY_SHOP_MAN) {
                Actor_Kill(&this->actor);
            }
            break;

        case 3:
            if (this->actor.params == ENOSSAN_CURIOSITY_SHOP_MAN) {
                Actor_Kill(&this->actor);
            }
            if ((CURRENT_TIME > CLOCK_TIME(22, 0)) || (CURRENT_TIME < CLOCK_TIME(6, 0))) {
                if (this->actor.params != ENOSSAN_CURIOSITY_SHOP_MAN) {
                    Actor_Kill(&this->actor);
                }
            }
            break;

        default:
            break;
    }
}

void EnOssan_RotateHead(EnOssan* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (this->actor.params == ENOSSAN_PART_TIME_WORKER) {
        if (player->transformation == PLAYER_FORM_ZORA) {
            Math_SmoothStepToS(&this->partTimerHeadRot.y, this->headRot.y, 3, 0x7D0, 0);
        } else if (this->flags & LOOKED_AT_PLAYER) {
            Math_SmoothStepToS(&this->partTimerHeadRot.y, 0x1F40, 3, 0x7D0, 0);
        } else {
            Math_SmoothStepToS(&this->partTimerHeadRot.y, this->headRot.y, 3, 0x7D0, 0);
            if (ABS_ALT(this->partTimerHeadRot.y - this->headRot.y) < 16) {
                this->flags |= LOOKED_AT_PLAYER;
            }
        }
    }
}

void EnOssan_SpawnShopItems(EnOssan* this, PlayState* play, ShopItem* shop) {
    s32 i;

    for (i = 0; i < ARRAY_COUNT(this->items); i++, shop++) {
        if (shop->shopItemId < 0) {
            this->items[i] = NULL;
        } else {
            this->items[i] = (EnGirlA*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_GIRLA, shop->spawnPos.x,
                                                   shop->spawnPos.y, shop->spawnPos.z, 0, 0, 0, shop->shopItemId);
        }
    }
}

void EnOssan_Init(Actor* thisx, PlayState* play) {
    EnOssan* this = (EnOssan*)thisx;
    s16 objectId;

    if ((this->actor.params > ENOSSAN_PART_TIME_WORKER) && (this->actor.params < ENOSSAN_CURIOSITY_SHOP_MAN)) {
        //! @bug: Impossible to reach, && should be an ||
        Actor_Kill(&this->actor);
        return;
    }
    objectId = sObjectIds[this->actor.params];
    this->objectSlot = Object_GetSlot(&play->objectCtx, objectId);
    if (this->objectSlot <= OBJECT_SLOT_NONE) {
        Actor_Kill(&this->actor);
        return;
    }
    EnOssan_CheckValidSpawn(this);
    Actor_ProcessInitChain(&this->actor, sInitChain);
    EnOssan_SetupAction(this, EnOssan_InitShop);
}

void EnOssan_Destroy(Actor* thisx, PlayState* play) {
    EnOssan* this = (EnOssan*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void EnOssan_UpdateCursorPos(PlayState* play, EnOssan* this) {
    s16 screenPosX;
    s16 screenPosY;

    Actor_GetScreenPos(play, &this->items[this->cursorIndex]->actor, &screenPosX, &screenPosY);
    this->cursorPos.x = screenPosX;
    this->cursorPos.y = screenPosY;

    if (CVarGetInteger("gModes.MirroredWorld.State", 0)) {
        this->cursorPos.x = SCREEN_WIDTH - this->cursorPos.x;
    }
}

void EnOssan_EndInteraction(PlayState* play, EnOssan* this) {
    Player* player = GET_PLAYER(play);

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
    if (this->cutsceneState == ENOSSAN_CUTSCENESTATE_PLAYING) {
        CutsceneManager_Stop(this->csId);
        this->cutsceneState = ENOSSAN_CUTSCENESTATE_STOPPED;
    }
    if (this->actor.params == ENOSSAN_CURIOSITY_SHOP_MAN) {
        // EnOssan_BeginInteraction includes the animation of him turning around, before being set to idle
        EnOssan_SetupAction(this, EnOssan_BeginInteraction);
    } else {
        EnOssan_SetupAction(this, EnOssan_Idle);
    }
}

s32 EnOssan_TestEndInteraction(EnOssan* this, PlayState* play, Input* input) {
    if (CHECK_BTN_ALL(input->press.button, BTN_B)) {
        EnOssan_EndInteraction(play, this);
        return true;
    }
    return false;
}

s32 EnOssan_TestCancelOption(EnOssan* this, PlayState* play, Input* input) {
    if (CHECK_BTN_ALL(input->press.button, BTN_B)) {
        this->actionFunc = this->prevActionFunc;
        Message_ContinueTextbox(play, this->items[this->cursorIndex]->actor.textId);
        return true;
    }
    return false;
}

void EnOssan_SetupStartShopping(PlayState* play, EnOssan* this, u8 skipHello) {
    Interface_SetAButtonDoAction(play, DO_ACTION_NEXT);
    if (!skipHello) {
        EnOssan_SetupAction(this, EnOssan_Hello);
    } else {
        EnOssan_StartShopping(play, this);
    }
}

void EnOssan_StartShopping(PlayState* play, EnOssan* this) {
    EnOssan_SetupAction(this, EnOssan_FaceShopkeeper);
    Message_ContinueTextbox(play, 0x640);
    Interface_SetAButtonDoAction(play, DO_ACTION_DECIDE);
    this->stickRightPrompt.isEnabled = true;
    this->stickLeftPrompt.isEnabled = true;
}

void EnOssan_SetupLookToShopkeeperFromShelf(PlayState* play, EnOssan* this) {
    Audio_PlaySfx(NA_SE_SY_CURSOR);
    this->drawCursor = 0;
    EnOssan_SetupAction(this, EnOssan_LookToShopkeeperFromShelf);
}

void EnOssan_Idle(EnOssan* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    SubS_UpdateFidgetTables(play, this->fidgetTableY, this->fidgetTableZ, ENOSSAN_LIMB_MAX);
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        player->stateFlags2 |= PLAYER_STATE2_20000000;
        EnOssan_SetupAction(this, EnOssan_BeginInteraction);
        if (this->cutsceneState == ENOSSAN_CUTSCENESTATE_STOPPED) {
            if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
                CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
            }
            this->csId = this->lookToShopkeeperCsId;
            CutsceneManager_Queue(this->csId);
            this->cutsceneState = ENOSSAN_CUTSCENESTATE_WAITING;
        }
    } else {
        if ((this->actor.xzDistToPlayer < 100.0f) && (player->actor.world.pos.x >= -40.0f) &&
            (player->actor.world.pos.x <= 40.0f) && (player->actor.world.pos.z >= -91.0f) &&
            (player->actor.world.pos.z <= -60.0f)) {
            Actor_OfferTalk(&this->actor, play, 100.0f);
        }
        if (this->actor.params == ENOSSAN_PART_TIME_WORKER) {
            Math_SmoothStepToS(&this->partTimerHeadRot.y, 0x1F40, 3, 0x7D0, 0);
        }
    }
}

void EnOssan_BeginInteraction(EnOssan* this, PlayState* play) {
    AnimationInfoS* animationInfo = sAnimationInfoList[this->actor.params];
    s16 curFrame = this->skelAnime.curFrame;
    s16 endFrame = Animation_GetLastFrame(animationInfo[this->animIndex].animation);

    if (this->animIndex == FSN_ANIM_TURN_AROUND_REVERSE) {
        endFrame = 0;
    }
    if (this->cutsceneState == ENOSSAN_CUTSCENESTATE_WAITING) {
        if (CutsceneManager_IsNext(this->csId)) {
            CutsceneManager_StartWithPlayerCsAndSetFlag(this->csId, &this->actor);
            this->cutsceneState = ENOSSAN_CUTSCENESTATE_PLAYING;
        } else {
            CutsceneManager_Queue(this->csId);
        }
    }
    if (this->actor.params == ENOSSAN_CURIOSITY_SHOP_MAN) {
        if (curFrame == endFrame) {
            switch (this->animIndex) {
                case FSN_ANIM_SCRATCH_BACK:
                    this->animIndex = FSN_ANIM_TURN_AROUND_FORWARD;
                    SubS_ChangeAnimationByInfoS(&this->skelAnime, animationInfo, FSN_ANIM_TURN_AROUND_FORWARD);
                    break;

                case FSN_ANIM_TURN_AROUND_FORWARD:
                    EnOssan_SetTalked(this);
                    this->textId = EnOssan_CuriosityShopMan_GetWelcome(this, play);
                    SubS_ChangeAnimationByInfoS(&this->skelAnime, animationInfo, this->animIndex);
                    break;

                case FSN_ANIM_HANDS_ON_COUNTER_START:
                case FSN_ANIM_HAND_ON_FACE_START:
                case FSN_ANIM_LEAN_FORWARD_START:
                case FSN_ANIM_SLAM_COUNTER_START:
                    this->animIndex++;
                    SubS_ChangeAnimationByInfoS(&this->skelAnime, animationInfo, this->animIndex);
                    Message_StartTextbox(play, this->textId, &this->actor);
                    EnOssan_SetupStartShopping(play, this, false);
                    break;

                case FSN_ANIM_HANDS_ON_COUNTER_LOOP:
                case FSN_ANIM_HAND_ON_FACE_LOOP:
                case FSN_ANIM_LEAN_FORWARD_LOOP:
                case FSN_ANIM_SLAM_COUNTER_LOOP:
                    this->animIndex = FSN_ANIM_TURN_AROUND_REVERSE;
                    SubS_ChangeAnimationByInfoS(&this->skelAnime, animationInfo, FSN_ANIM_TURN_AROUND_REVERSE);
                    break;

                case FSN_ANIM_TURN_AROUND_REVERSE:
                    this->animIndex = FSN_ANIM_SCRATCH_BACK;
                    SubS_ChangeAnimationByInfoS(&this->skelAnime, animationInfo, FSN_ANIM_SCRATCH_BACK);
                    EnOssan_SetupAction(this, EnOssan_Idle);
                    break;

                default:
                    this->animIndex = FSN_ANIM_SCRATCH_BACK;
                    SubS_ChangeAnimationByInfoS(&this->skelAnime, animationInfo, FSN_ANIM_SCRATCH_BACK);
                    EnOssan_SetupAction(this, EnOssan_Idle);
                    break;
            }
        }
    } else {
        EnOssan_SetTalked(this);
        this->textId = EnOssan_PartTimer_GetWelcome(this, play);
        Message_StartTextbox(play, this->textId, &this->actor);
        EnOssan_SetupStartShopping(play, this, false);
    }
}

void EnOssan_UpdateJoystickInputState(PlayState* play, EnOssan* this) {
    s8 stickX = CONTROLLER1(&play->state)->rel.stick_x;
    s8 stickY = CONTROLLER1(&play->state)->rel.stick_y;

    stickX *= GameInteractor_InvertControl(GI_INVERT_SHOP_X);

    this->moveHorizontal = this->moveVertical = false;

    if (this->stickAccumX == 0) {
        if ((stickX > 30) || (stickX < -30)) {
            this->stickAccumX = stickX;
            this->moveHorizontal = true;
        }
    } else if ((stickX <= 30) && (stickX >= -30)) {
        this->stickAccumX = 0;
    } else if ((this->stickAccumX * stickX) < 0) { // Stick has swapped directions
        this->stickAccumX = stickX;
        this->moveHorizontal = true;
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
            this->moveVertical = true;
        }
    } else if ((stickY <= 30) && (stickY >= -30)) {
        this->stickAccumY = 0;
    } else if ((this->stickAccumY * stickY) < 0) { // Stick has swapped directions
        this->stickAccumY = stickY;
        this->moveVertical = true;
    } else {
        this->stickAccumY += stickY;
        if (this->stickAccumY > 2000) {
            this->stickAccumY = 2000;
        } else if (this->stickAccumY < -2000) {
            this->stickAccumY = -2000;
        }
    }
}

u8 EnOssan_SetCursorIndexFromNeutral(EnOssan* this, u8 shelfOffset) {
    u8 i;

    // if cursor is on the top shelf
    if (this->cursorIndex & 1) {
        // scan top shelf
        for (i = shelfOffset + 1; i < shelfOffset + 4; i += 2) {
            if (this->items[i] != NULL) {
                return i;
            }
        }
        // scan bottom shelf
        for (i = shelfOffset; i < shelfOffset + 4; i += 2) {
            if (this->items[i] != NULL) {
                return i;
            }
        }
    } else {
        // scan bottom shelf
        for (i = shelfOffset; i < shelfOffset + 4; i += 2) {
            if (this->items[i] != NULL) {
                return i;
            }
        }
        // scan top shelf
        for (i = shelfOffset + 1; i < shelfOffset + 4; i += 2) {
            if (this->items[i] != NULL) {
                return i;
            }
        }
    }
    return CURSOR_INVALID;
}

u8 EnOssan_CursorRight(EnOssan* this, u8 cursorIndex, u8 shelfSlotMin) {
    u8 end = shelfSlotMin + 4;

    while ((cursorIndex >= shelfSlotMin) && (cursorIndex < end)) {
        cursorIndex -= 2;
        if ((cursorIndex >= shelfSlotMin) && (cursorIndex < end)) {
            if (this->items[cursorIndex] != NULL) {
                return cursorIndex;
            }
        }
    }
    return CURSOR_INVALID;
}

u8 EnOssan_CursorLeft(EnOssan* this, u8 cursorIndex, u8 shelfSlotMax) {
    while (cursorIndex < shelfSlotMax) {
        cursorIndex += 2;
        if (cursorIndex < shelfSlotMax) {
            if (this->items[cursorIndex] != NULL) {
                return cursorIndex;
            }
        }
    }
    return CURSOR_INVALID;
}

void EnOssan_Hello(EnOssan* this, PlayState* play) {
    AnimationInfoS* animationInfo = sAnimationInfoList[this->actor.params];
    u8 talkState = Message_GetState(&play->msgCtx);
    s32 pad;
    Player* player = GET_PLAYER(play);

    EnOssan_RotateHead(this, play);
    if ((talkState == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        if ((this->animIndex == ANI_ANIM_APOLOGY_LOOP) && (this->actor.params == ENOSSAN_PART_TIME_WORKER)) {
            this->animIndex = ANI_ANIM_STANDING_NORMAL_LOOP_2;
            SubS_ChangeAnimationByInfoS(&this->skelAnime, animationInfo, ANI_ANIM_STANDING_NORMAL_LOOP_2);
        }
        this->flags &= ~LOOKED_AT_PLAYER;
        if (player->transformation == PLAYER_FORM_DEKU) {
            EnOssan_EndInteraction(play, this);
        } else if (this->flags & END_INTERACTION) {
            this->flags &= ~END_INTERACTION;
            EnOssan_EndInteraction(play, this);
        } else if (!EnOssan_TestEndInteraction(this, play, CONTROLLER1(&play->state))) {
            EnOssan_StartShopping(play, this);
        } else {
            return;
        }
    }
    if ((talkState == TEXT_STATE_AWAITING_NEXT) && (this->actor.params == ENOSSAN_PART_TIME_WORKER) &&
        (player->transformation == PLAYER_FORM_ZORA) && Message_ShouldAdvance(play)) {
        this->animIndex = ANI_ANIM_APOLOGY_LOOP;
        SubS_ChangeAnimationByInfoS(&this->skelAnime, animationInfo, ANI_ANIM_APOLOGY_LOOP);
    }
    if ((this->animIndex == FSN_ANIM_SLAM_COUNTER_LOOP) && Animation_OnFrame(&this->skelAnime, 18.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EV_HANKO);
    }
}

s32 EnOssan_FacingShopkeeperDialogResult(EnOssan* this, PlayState* play) {
    AnimationInfoS* animationInfo = sAnimationInfoList[this->actor.params];
    Player* player = GET_PLAYER(play);

    switch (play->msgCtx.choiceIndex) {
        case 0:
            Audio_PlaySfx_MessageDecide();
            if ((this->actor.params == ENOSSAN_PART_TIME_WORKER) && (player->transformation == PLAYER_FORM_ZORA)) {
                this->animIndex = ANI_ANIM_APOLOGY_LOOP;
                SubS_ChangeAnimationByInfoS(&this->skelAnime, animationInfo, ANI_ANIM_APOLOGY_LOOP);
            }
            EnOssan_SetupAction(this, EnOssan_TalkToShopkeeper);
            Message_ContinueTextbox(play, sTalkOptionTextIds[this->actor.params]);
            Interface_SetAButtonDoAction(play, DO_ACTION_DECIDE);
            this->stickLeftPrompt.isEnabled = false;
            this->stickRightPrompt.isEnabled = false;
            return true;

        case 1:
            Audio_PlaySfx_MessageCancel();
            EnOssan_EndInteraction(play, this);
            return true;

        default:
            return false;
    }
}

void EnOssan_FaceShopkeeper(EnOssan* this, PlayState* play) {
    u8 talkState = Message_GetState(&play->msgCtx);
    s32 pad;
    Player* player = GET_PLAYER(play);
    u8 cursorIndex;

    if (this->cutsceneState == ENOSSAN_CUTSCENESTATE_STOPPED) {
        if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
            CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
        }
        this->csId = this->lookToShopkeeperCsId;
        CutsceneManager_Queue(this->csId);
        this->cutsceneState = ENOSSAN_CUTSCENESTATE_WAITING;
    } else {
        if (talkState == TEXT_STATE_CHOICE) {
            Interface_SetAButtonDoAction(play, DO_ACTION_DECIDE);
            if (EnOssan_TestEndInteraction(this, play, CONTROLLER1(&play->state))) {
                return;
            }

            if (Message_ShouldAdvance(play) && EnOssan_FacingShopkeeperDialogResult(this, play)) {
                return;
            }

            if (this->stickAccumX < 0) {
                cursorIndex = EnOssan_SetCursorIndexFromNeutral(this, 4);
                if (cursorIndex != CURSOR_INVALID) {
                    this->cursorIndex = cursorIndex;
                    EnOssan_SetupAction(this, EnOssan_LookToLeftShelf);
                    Interface_SetAButtonDoAction(play, DO_ACTION_DECIDE);
                    this->stickLeftPrompt.isEnabled = false;
                    Audio_PlaySfx(NA_SE_SY_CURSOR);
                }
            } else if (this->stickAccumX > 0) {
                cursorIndex = EnOssan_SetCursorIndexFromNeutral(this, 0);
                if (cursorIndex != CURSOR_INVALID) {
                    this->cursorIndex = cursorIndex;
                    EnOssan_SetupAction(this, EnOssan_LookToRightShelf);
                    Interface_SetAButtonDoAction(play, DO_ACTION_DECIDE);
                    this->stickRightPrompt.isEnabled = false;
                    Audio_PlaySfx(NA_SE_SY_CURSOR);
                }
            }
        }
        if ((this->actor.params == ENOSSAN_PART_TIME_WORKER) && (player->transformation != PLAYER_FORM_ZORA)) {
            Math_SmoothStepToS(&this->partTimerHeadRot.y, 0x1F40, 3, 0x7D0, 0);
        }
    }
}

void EnOssan_TalkToShopkeeper(EnOssan* this, PlayState* play) {
    AnimationInfoS* animationInfo = sAnimationInfoList[this->actor.params];

    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        if ((this->animIndex == ANI_ANIM_APOLOGY_LOOP) && (this->actor.params == ENOSSAN_PART_TIME_WORKER)) {
            this->animIndex = ANI_ANIM_STANDING_NORMAL_LOOP_2;
            SubS_ChangeAnimationByInfoS(&this->skelAnime, animationInfo, ANI_ANIM_STANDING_NORMAL_LOOP_2);
        }
        EnOssan_StartShopping(play, this);
    }
}

void EnOssan_LookToLeftShelf(EnOssan* this, PlayState* play) {
    if (this->cutsceneState == ENOSSAN_CUTSCENESTATE_PLAYING) {
        CutsceneManager_Stop(this->csId);
        this->cutsceneState = ENOSSAN_CUTSCENESTATE_STOPPED;
    }
    if (this->cutsceneState == ENOSSAN_CUTSCENESTATE_STOPPED) {
        if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
            CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
        }
        this->csId = this->lookToLeftShelfCsId;
        CutsceneManager_Queue(this->csId);
        this->cutsceneState = ENOSSAN_CUTSCENESTATE_WAITING;
    } else {
        if (this->cutsceneState == ENOSSAN_CUTSCENESTATE_WAITING) {
            if (CutsceneManager_IsNext(this->csId)) {
                CutsceneManager_StartWithPlayerCsAndSetFlag(this->csId, &this->actor);
                this->cutsceneState = ENOSSAN_CUTSCENESTATE_PLAYING;
                EnOssan_UpdateCursorPos(play, this);
                EnOssan_SetupAction(this, EnOssan_BrowseLeftShelf);
                Message_ContinueTextbox(play, this->items[this->cursorIndex]->actor.textId);
            } else {
                CutsceneManager_Queue(this->csId);
            }
        }
        this->stickAccumX = 0;
    }
}

void EnOssan_LookToRightShelf(EnOssan* this, PlayState* play) {
    if (this->cutsceneState == ENOSSAN_CUTSCENESTATE_PLAYING) {
        CutsceneManager_Stop(this->csId);
        this->cutsceneState = ENOSSAN_CUTSCENESTATE_STOPPED;
    }
    if (this->cutsceneState == ENOSSAN_CUTSCENESTATE_STOPPED) {
        if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
            CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
        }
        this->csId = this->lookToRightShelfCsId;
        CutsceneManager_Queue(this->csId);
        this->cutsceneState = ENOSSAN_CUTSCENESTATE_WAITING;
    } else {
        if (this->cutsceneState == ENOSSAN_CUTSCENESTATE_WAITING) {
            if (CutsceneManager_IsNext(this->csId)) {
                CutsceneManager_StartWithPlayerCsAndSetFlag(this->csId, &this->actor);
                this->cutsceneState = ENOSSAN_CUTSCENESTATE_PLAYING;
                EnOssan_UpdateCursorPos(play, this);
                EnOssan_SetupAction(this, EnOssan_BrowseRightShelf);
                Message_ContinueTextbox(play, this->items[this->cursorIndex]->actor.textId);
            } else {
                CutsceneManager_Queue(this->csId);
            }
        }
        this->stickAccumX = 0;
    }
}

void EnOssan_CursorUpDown(EnOssan* this) {
    u8 curTemp = this->cursorIndex;
    u8 curScanTemp;

    if (this->stickAccumY < 0) {
        curTemp &= (u8)~1;
        if (this->items[curTemp] != NULL) {
            this->cursorIndex = curTemp;
            return;
        }
        // cursorIndex on right shelf
        if (curTemp < 4) {
            curScanTemp = curTemp + 2;
            if (curScanTemp >= 4) {
                curScanTemp = 0;
            }
            while (curScanTemp != curTemp) {
                if (this->items[curScanTemp] != NULL) {
                    this->cursorIndex = curScanTemp;
                    return;
                }
                curScanTemp += 2;
                if (curScanTemp >= 4) {
                    curScanTemp = 0;
                }
            }
        } else {
            // cursorIndex on left shelf
            curScanTemp = curTemp + 2;
            if (curScanTemp >= 8) {
                curScanTemp = 4;
            }
            while (curScanTemp != curTemp) {
                if (this->items[curScanTemp] != NULL) {
                    this->cursorIndex = curScanTemp;
                    return;
                }
                curScanTemp += 2;
                if (curScanTemp >= 8) {
                    curScanTemp = 4;
                }
            }
        }
    } else if (this->stickAccumY > 0) {
        curTemp |= 1;
        if (this->items[curTemp] != NULL) {
            this->cursorIndex = curTemp;
            return;
        }
        // cursorIndex on right shelf
        if (curTemp < 4) {
            curScanTemp = curTemp + 2;
            if (curScanTemp >= 4) {
                curScanTemp = 1;
            }
            while (curScanTemp != curTemp) {
                if (this->items[curScanTemp] != NULL) {
                    this->cursorIndex = curScanTemp;
                    return;
                }
                curScanTemp += 2;
                if (curScanTemp >= 4) {
                    curScanTemp = 1;
                }
            }
        } else {
            // cursorIndex on left shelf
            curScanTemp = curTemp + 2;
            if (curScanTemp >= 8) {
                curScanTemp = 5;
            }
            while (curScanTemp != curTemp) {
                if (this->items[curScanTemp] != NULL) {
                    this->cursorIndex = curScanTemp;
                    return;
                }
                curScanTemp += 2;
                if (curScanTemp >= 8) {
                    curScanTemp = 5;
                }
            }
        }
    }
}

s32 EnOssan_HasPlayerSelectedItem(PlayState* play, EnOssan* this, Input* input) {
    EnGirlA* item = this->items[this->cursorIndex];

    if (EnOssan_TestEndInteraction(this, play, input)) {
        return true;
    }
    if (EnOssan_TestItemSelected(play)) {
        if (!item->isOutOfStock) {
            this->prevActionFunc = this->actionFunc;
            Message_ContinueTextbox(play, this->items[this->cursorIndex]->choiceTextId);
            this->stickLeftPrompt.isEnabled = false;
            this->stickRightPrompt.isEnabled = false;
            Audio_PlaySfx(NA_SE_SY_DECIDE);
            this->drawCursor = 0;
            EnOssan_SetupAction(this, EnOssan_SelectItem);
        } else {
            Audio_PlaySfx(NA_SE_SY_ERROR);
        }
        return true;
    }
    return false;
}

void EnOssan_BrowseLeftShelf(EnOssan* this, PlayState* play) {
    u8 talkState = Message_GetState(&play->msgCtx);
    s32 pad;
    u8 prevCursorIndex = this->cursorIndex;
    u8 cursorIndex;

    if (!EnOssan_ReturnItemToShelf(this)) {
        this->delayTimer = 3;
    } else if (this->delayTimer != 0) {
        this->delayTimer--;
    } else {
        this->drawCursor = 0xFF;
        this->stickRightPrompt.isEnabled = true;
        EnOssan_UpdateCursorPos(play, this);
        if (talkState == TEXT_STATE_EVENT) {
            Interface_SetAButtonDoAction(play, DO_ACTION_DECIDE);
            if (!EnOssan_HasPlayerSelectedItem(play, this, CONTROLLER1(&play->state))) {
                if (this->moveHorizontal) {
                    if (this->stickAccumX > 0) {
                        cursorIndex = EnOssan_CursorRight(this, this->cursorIndex, 4);
                        if (cursorIndex != CURSOR_INVALID) {
                            this->cursorIndex = cursorIndex;
                        } else {
                            EnOssan_SetupLookToShopkeeperFromShelf(play, this);
                            return;
                        }
                    } else if (this->stickAccumX < 0) {
                        cursorIndex = EnOssan_CursorLeft(this, this->cursorIndex, 8);
                        if (cursorIndex != CURSOR_INVALID) {
                            this->cursorIndex = cursorIndex;
                        }
                    }
                } else {
                    if ((this->stickAccumX > 0) && (this->stickAccumX > 500)) {
                        cursorIndex = EnOssan_CursorRight(this, this->cursorIndex, 4);
                        if (cursorIndex != CURSOR_INVALID) {
                            this->cursorIndex = cursorIndex;
                        } else {
                            EnOssan_SetupLookToShopkeeperFromShelf(play, this);
                            return;
                        }
                    } else if ((this->stickAccumX < 0) && (this->stickAccumX < -500)) {
                        cursorIndex = EnOssan_CursorLeft(this, this->cursorIndex, 8);
                        if (cursorIndex != CURSOR_INVALID) {
                            this->cursorIndex = cursorIndex;
                        }
                    }
                }
                EnOssan_CursorUpDown(this);
                if (this->cursorIndex != prevCursorIndex) {
                    Message_ContinueTextbox(play, this->items[this->cursorIndex]->actor.textId);
                    Audio_PlaySfx(NA_SE_SY_CURSOR);
                }
            }
        }
    }
}

void EnOssan_BrowseRightShelf(EnOssan* this, PlayState* play) {
    u8 talkState = Message_GetState(&play->msgCtx);
    s32 pad;
    u8 prevCursorIndex = this->cursorIndex;
    u8 cursorIndex;

    if (!EnOssan_ReturnItemToShelf(this)) {
        this->delayTimer = 3;
    } else if (this->delayTimer != 0) {
        this->delayTimer--;
    } else {
        this->drawCursor = 0xFF;
        this->stickLeftPrompt.isEnabled = true;
        EnOssan_UpdateCursorPos(play, this);
        if (talkState == TEXT_STATE_EVENT) {
            Interface_SetAButtonDoAction(play, DO_ACTION_DECIDE);
            if (!EnOssan_HasPlayerSelectedItem(play, this, CONTROLLER1(&play->state))) {
                if (this->moveHorizontal != 0) {
                    if (this->stickAccumX < 0) {
                        cursorIndex = EnOssan_CursorRight(this, this->cursorIndex, 0);
                        if (cursorIndex != CURSOR_INVALID) {
                            this->cursorIndex = cursorIndex;
                        } else {
                            EnOssan_SetupLookToShopkeeperFromShelf(play, this);
                            return;
                        }
                    } else if (this->stickAccumX > 0) {
                        cursorIndex = EnOssan_CursorLeft(this, this->cursorIndex, 4);
                        if (cursorIndex != CURSOR_INVALID) {
                            this->cursorIndex = cursorIndex;
                        }
                    }
                } else {
                    if ((this->stickAccumX < 0) && (this->stickAccumX < -500)) {
                        cursorIndex = EnOssan_CursorRight(this, this->cursorIndex, 0);
                        if (cursorIndex != CURSOR_INVALID) {
                            this->cursorIndex = cursorIndex;
                        } else {
                            EnOssan_SetupLookToShopkeeperFromShelf(play, this);
                            return;
                        }
                    } else if ((this->stickAccumX > 0) && (this->stickAccumX > 500)) {
                        cursorIndex = EnOssan_CursorLeft(this, this->cursorIndex, 4);
                        if (cursorIndex != CURSOR_INVALID) {
                            this->cursorIndex = cursorIndex;
                        }
                    }
                }
                EnOssan_CursorUpDown(this);
                if (this->cursorIndex != prevCursorIndex) {
                    Message_ContinueTextbox(play, this->items[this->cursorIndex]->actor.textId);
                    Audio_PlaySfx(NA_SE_SY_CURSOR);
                }
            }
        }
    }
}

void EnOssan_LookToShopkeeperFromShelf(EnOssan* this, PlayState* play) {
    if (this->cutsceneState == ENOSSAN_CUTSCENESTATE_PLAYING) {
        CutsceneManager_Stop(this->csId);
        this->cutsceneState = ENOSSAN_CUTSCENESTATE_STOPPED;
    }
    if (this->cutsceneState == ENOSSAN_CUTSCENESTATE_STOPPED) {
        if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
            CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
        }
        this->csId = this->lookToShopKeeperFromShelfCsId;
        CutsceneManager_Queue(this->csId);
        this->cutsceneState = ENOSSAN_CUTSCENESTATE_WAITING;
    } else if (this->cutsceneState == ENOSSAN_CUTSCENESTATE_WAITING) {
        if (CutsceneManager_IsNext(this->csId)) {
            CutsceneManager_StartWithPlayerCsAndSetFlag(this->csId, &this->actor);
            this->cutsceneState = ENOSSAN_CUTSCENESTATE_PLAYING;
            EnOssan_UpdateCursorPos(play, this);
            EnOssan_StartShopping(play, this);
        } else {
            CutsceneManager_Queue(this->csId);
        }
    }
}

void EnOssan_SetupBuyItemWithFanfare(PlayState* play, EnOssan* this) {
    Player* player = GET_PLAYER(play);

    Actor_OfferGetItem(&this->actor, play, this->items[this->cursorIndex]->getItemId, 300.0f, 300.0f);
    play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
    play->msgCtx.stateTimer = 4;
    player->stateFlags2 &= ~PLAYER_STATE2_20000000;
    Interface_SetHudVisibility(HUD_VISIBILITY_ALL);
    this->drawCursor = 0;
    EnOssan_SetupAction(this, EnOssan_BuyItemWithFanfare);
}

void EnOssan_SetupCannotBuy(PlayState* play, EnOssan* this, u16 textId) {
    Message_ContinueTextbox(play, textId);
    EnOssan_SetupAction(this, EnOssan_CannotBuy);
}

void EnOssan_SetupBuy(PlayState* play, EnOssan* this, u16 textId) {
    Message_ContinueTextbox(play, textId);
    EnOssan_SetupAction(this, EnOssan_CanBuy);
}

void EnOssan_HandleCanBuyItem(PlayState* play, EnOssan* this) {
    EnGirlA* item = this->items[this->cursorIndex];

    switch (item->canBuyFunc(play, item)) {
        case CANBUY_RESULT_SUCCESS_1:
            if (this->cutsceneState == ENOSSAN_CUTSCENESTATE_PLAYING) {
                CutsceneManager_Stop(this->csId);
                this->cutsceneState = ENOSSAN_CUTSCENESTATE_STOPPED;
            }
            Audio_PlaySfx_MessageDecide();
            item->buyFanfareFunc(play, item);
            EnOssan_SetupBuyItemWithFanfare(play, this);
            this->drawCursor = 0;
            this->shopItemSelectedTween = 0.0f;
            item->boughtFunc(play, item);
            break;

        case CANBUY_RESULT_SUCCESS_2:
            Audio_PlaySfx_MessageDecide();
            item->buyFunc(play, item);
            EnOssan_SetupBuy(play, this, sBuySuccessTextIds[this->actor.params]);
            this->drawCursor = 0;
            this->shopItemSelectedTween = 0.0f;
            item->boughtFunc(play, item);
            break;

        case CANBUY_RESULT_NO_ROOM:
        case CANBUY_RESULT_NO_ROOM_2:
            Audio_PlaySfx(NA_SE_SY_ERROR);
            EnOssan_SetupCannotBuy(play, this, sNoRoomTextIds[this->actor.params]);
            break;

        case CANBUY_RESULT_NEED_EMPTY_BOTTLE:
            Audio_PlaySfx(NA_SE_SY_ERROR);
            EnOssan_SetupCannotBuy(play, this, sNeedEmptyBottleTextIds[this->actor.params]);
            break;

        case CANBUY_RESULT_NEED_RUPEES:
            Audio_PlaySfx(NA_SE_SY_ERROR);
            EnOssan_SetupCannotBuy(play, this, sNeedRupeesTextIds[this->actor.params]);
            break;

        case CANBUY_RESULT_CANNOT_GET_NOW_2:
            Audio_PlaySfx(NA_SE_SY_ERROR);
            EnOssan_SetupCannotBuy(play, this, sCannotGetNowTextIds[this->actor.params]);
            break;

        case CANBUY_RESULT_CANNOT_GET_NOW:
            Audio_PlaySfx(NA_SE_SY_ERROR);
            EnOssan_SetupCannotBuy(play, this, sNoRoomTextIds[this->actor.params]);
            break;

        default:
            break;
    }
}

void EnOssan_SelectItem(EnOssan* this, PlayState* play) {
    u8 talkState = Message_GetState(&play->msgCtx);

    if (EnOssan_TakeItemOffShelf(this) && (talkState == TEXT_STATE_CHOICE)) {
        Interface_SetAButtonDoAction(play, DO_ACTION_DECIDE);
        if (!EnOssan_TestCancelOption(this, play, CONTROLLER1(&play->state)) && Message_ShouldAdvance(play)) {
            switch (play->msgCtx.choiceIndex) {
                case 0:
                    EnOssan_HandleCanBuyItem(play, this);
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

void EnOssan_CannotBuy(EnOssan* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        this->actionFunc = this->prevActionFunc;
        Message_ContinueTextbox(play, this->items[this->cursorIndex]->actor.textId);
    }
}

void EnOssan_CanBuy(EnOssan* this, PlayState* play) {
    EnGirlA* item;

    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        this->shopItemSelectedTween = 0.0f;
        EnOssan_ResetItemPosition(this);
        item = this->items[this->cursorIndex];
        item->restockFunc(play, item);
        this->actionFunc = this->prevActionFunc;
        Message_ContinueTextbox(play, this->items[this->cursorIndex]->actor.textId);
    }
}

void EnOssan_BuyItemWithFanfare(EnOssan* this, PlayState* play) {
    if (Actor_HasParent(&this->actor, play)) {
        this->actor.parent = NULL;
        EnOssan_SetupAction(this, EnOssan_SetupItemPurchased);
    } else {
        Actor_OfferGetItem(&this->actor, play, this->items[this->cursorIndex]->getItemId, 300.0f, 300.0f);
    }
}

void EnOssan_SetupItemPurchased(EnOssan* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_DONE) && Message_ShouldAdvance(play)) {
        play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
        play->msgCtx.stateTimer = 4;
        EnOssan_SetupAction(this, EnOssan_ItemPurchased);
        if (this->cutsceneState == ENOSSAN_CUTSCENESTATE_STOPPED) {
            if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
                CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
            }
            this->csId = this->lookToShopkeeperCsId;
            CutsceneManager_Queue(this->csId);
        }
        Actor_OfferTalkExchangeEquiCylinder(&this->actor, play, 400.0f, PLAYER_IA_MINUS1);
    }
}

void EnOssan_ContinueShopping(EnOssan* this, PlayState* play) {
    u8 talkState = Message_GetState(&play->msgCtx);
    Player* player = GET_PLAYER(play);
    EnGirlA* item;

    if (talkState == TEXT_STATE_CHOICE) {
        Interface_SetAButtonDoAction(play, DO_ACTION_DECIDE);
        if (Message_ShouldAdvance(play)) {
            EnOssan_ResetItemPosition(this);
            item = this->items[this->cursorIndex];
            item->restockFunc(play, item);
            if (!EnOssan_TestEndInteraction(this, play, CONTROLLER1(&play->state))) {
                switch (play->msgCtx.choiceIndex) {
                    case 0:
                        Audio_PlaySfx_MessageDecide();
                        player->actor.shape.rot.y = BINANG_ROT180(player->actor.shape.rot.y);
                        player->stateFlags2 |= PLAYER_STATE2_20000000;
                        Message_StartTextbox(play, this->textId, &this->actor);
                        EnOssan_SetupStartShopping(play, this, true);
                        Actor_OfferTalkExchangeEquiCylinder(&this->actor, play, 100.0f, PLAYER_IA_MINUS1);
                        break;

                    case 1:
                    default:
                        Audio_PlaySfx_MessageCancel();
                        EnOssan_EndInteraction(play, this);
                        break;
                }
            }
        }
    } else if ((talkState == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        EnOssan_ResetItemPosition(this);
        item = this->items[this->cursorIndex];
        item->restockFunc(play, item);
        player->actor.shape.rot.y = BINANG_ROT180(player->actor.shape.rot.y);
        player->stateFlags2 |= PLAYER_STATE2_20000000;
        Message_StartTextbox(play, this->textId, &this->actor);
        EnOssan_SetupStartShopping(play, this, true);
        Actor_OfferTalkExchangeEquiCylinder(&this->actor, play, 100.0f, PLAYER_IA_MINUS1);
    }
}

void EnOssan_ItemPurchased(EnOssan* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (this->cutsceneState == ENOSSAN_CUTSCENESTATE_STOPPED) {
        if (CutsceneManager_IsNext(this->csId)) {
            CutsceneManager_StartWithPlayerCsAndSetFlag(this->csId, &this->actor);
            player->stateFlags2 |= PLAYER_STATE2_20000000;
            EnOssan_SetupAction(this, EnOssan_ContinueShopping);
            this->cutsceneState = ENOSSAN_CUTSCENESTATE_PLAYING;
        } else {
            if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
                CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
            }
            this->csId = this->lookToShopkeeperCsId;
            CutsceneManager_Queue(this->csId);
        }
    }
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        Message_ContinueTextbox(play, 0x642);
    } else {
        Actor_OfferTalkExchangeEquiCylinder(&this->actor, play, 400.0f, PLAYER_IA_MINUS1);
    }
}

void EnOssan_PositionSelectedItem(EnOssan* this) {
    static Vec3f sSelectedItemPosition[] = {
        { 35.0f, 68.0f, -130.0f },
        { -35.0f, 68.0f, -130.0f },
    };
    EnGirlA* item;
    u8 i = this->cursorIndex;
    u8 i2;
    ShopItem* shopItem = &sShops[this->actor.params][i];
    Vec3f worldPos;

    item = this->items[i];
    i2 = i >> 2;
    VEC3F_LERPIMPDST(&worldPos, &shopItem->spawnPos, &sSelectedItemPosition[i2], this->shopItemSelectedTween);

    item->actor.world.pos.x = worldPos.x;
    item->actor.world.pos.y = worldPos.y;
    item->actor.world.pos.z = worldPos.z;
}

void EnOssan_ResetItemPosition(EnOssan* this) {
    this->shopItemSelectedTween = 0.0f;
    EnOssan_PositionSelectedItem(this);
}

/*
 * Returns true if animation has completed
 */
s32 EnOssan_TakeItemOffShelf(EnOssan* this) {
    Math_ApproachF(&this->shopItemSelectedTween, 1.0f, 1.0f, 0.15f);
    if (this->shopItemSelectedTween >= 0.85f) {
        this->shopItemSelectedTween = 1.0f;
    }
    EnOssan_PositionSelectedItem(this);
    if (this->shopItemSelectedTween == 1.0f) {
        return true;
    }
    return false;
}

/*
 * Returns true if animation has completed
 */
s32 EnOssan_ReturnItemToShelf(EnOssan* this) {
    Math_ApproachF(&this->shopItemSelectedTween, 0.0f, 1.0f, 0.15f);
    if (this->shopItemSelectedTween <= 0.15f) {
        this->shopItemSelectedTween = 0.0f;
    }
    EnOssan_PositionSelectedItem(this);
    if (this->shopItemSelectedTween == 0.0f) {
        return true;
    }
    return false;
}

void EnOssan_UpdateItemSelectedProperty(EnOssan* this) {
    EnGirlA** items = this->items;
    EnGirlA* item;
    s32 i;

    for (i = 0; i < ARRAY_COUNT(this->items); i++, items++) {
        item = *items;
        if (item != NULL) {
            if ((this->actionFunc != EnOssan_SelectItem) && (this->actionFunc != EnOssan_CannotBuy) &&
                (this->drawCursor == 0)) {
                item->isSelected = false;
            } else {
                item->isSelected = (this->cursorIndex == i) ? true : false;
            }
        }
    }
}

void EnOssan_UpdateCursorAnim(EnOssan* this) {
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

void EnOssan_UpdateStickDirectionPromptAnim(EnOssan* this) {
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

void EnOssan_WaitForBlink(EnOssan* this) {
    s16 decr = this->blinkTimer - 1;

    if (decr != 0) {
        this->blinkTimer = decr;
    } else {
        this->blinkFunc = EnOssan_Blink;
    }
}

void EnOssan_Blink(EnOssan* this) {
    s16 decr = this->blinkTimer - 1;
    s16 eyeTexIndexTemp;

    if (decr != 0) {
        this->blinkTimer = decr;
        return;
    }
    eyeTexIndexTemp = this->eyeTexIndex + 1;
    if (eyeTexIndexTemp > 2) {
        this->eyeTexIndex = 0;
        this->blinkTimer = (s32)(Rand_ZeroOne() * 60.0f) + 20;
        this->blinkFunc = EnOssan_WaitForBlink;
    } else {
        this->eyeTexIndex = eyeTexIndexTemp;
        this->blinkTimer = 1;
    }
}

void EnOssan_CuriosityShopMan_Init(EnOssan* this, PlayState* play) {
    SkelAnime_InitFlex(play, &this->skelAnime, &gFsnSkel, &gFsnIdleAnim, this->jointTable, this->morphTable,
                       ENOSSAN_LIMB_MAX);

    // #region 2S2H [Port] We need to manually patch this skeleton because OBJECT_FSN lists the wrong amount. Same as in
    // en_fsn
    this->skelAnime.limbCount = ENOSSAN_LIMB_MAX;
    this->actor.draw = EnOssan_CuriosityShopMan_Draw;
}

void EnOssan_PartTimer_Init(EnOssan* this, PlayState* play) {
    SkelAnime_InitFlex(play, &this->skelAnime, &gAniSkel, &gAniStandingNormalAnim, this->jointTable, this->morphTable,
                       ANI_LIMB_MAX);
    this->actor.draw = EnOssan_PartTimer_Draw;
}

u16 EnOssan_CuriosityShopMan_GetWelcome(EnOssan* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    u16 textId = Text_GetFaceReaction(play, FACE_REACTION_SET_CURIOSITY_SHOP_MAN);

    if (textId != 0) {
        this->animIndex = FSN_ANIM_HANDS_ON_COUNTER_START;
        this->flags |= END_INTERACTION;
        return textId;
    }

    switch (player->transformation) {
        case PLAYER_FORM_DEKU:
            this->animIndex = FSN_ANIM_SLAM_COUNTER_START;
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_TALKED_CURIOSITY_SHOP_MAN_AS_DEKU)) {
                return sWelcomeDekuTextIds[ENOSSAN_CURIOSITY_SHOP_MAN];
            }
            return sWelcomeDekuFirstTimeTextIds[ENOSSAN_CURIOSITY_SHOP_MAN];

        case PLAYER_FORM_ZORA:
            this->animIndex = FSN_ANIM_LEAN_FORWARD_START;
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_TALKED_CURIOSITY_SHOP_MAN_AS_ZORA)) {
                return sWelcomeZoraTextIds[ENOSSAN_CURIOSITY_SHOP_MAN];
            }
            return sWelcomeZoraFirstTimeTextIds[ENOSSAN_CURIOSITY_SHOP_MAN];

        case PLAYER_FORM_GORON:
            this->animIndex = FSN_ANIM_HAND_ON_FACE_START;
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_TALKED_CURIOSITY_SHOP_MAN_AS_GORON)) {
                return sWelcomeGoronTextIds[ENOSSAN_CURIOSITY_SHOP_MAN];
            }
            return sWelcomeGoronFirstTimeTextIds[ENOSSAN_CURIOSITY_SHOP_MAN];

        default:
            this->animIndex = FSN_ANIM_HANDS_ON_COUNTER_START;
            return sWelcomeHumanTextIds[ENOSSAN_CURIOSITY_SHOP_MAN];
    }
}

u16 EnOssan_PartTimer_GetWelcome(EnOssan* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    u16 textId = Text_GetFaceReaction(play, FACE_REACTION_SET_PART_TIMER);

    if (textId != 0) {
        this->flags |= END_INTERACTION;
        return textId;
    }

    switch (player->transformation) {
        case PLAYER_FORM_DEKU:
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_TALKED_PART_TIMER_AS_DEKU)) {
                return sWelcomeDekuTextIds[ENOSSAN_PART_TIME_WORKER];
            }
            return sWelcomeDekuFirstTimeTextIds[ENOSSAN_PART_TIME_WORKER];

        case PLAYER_FORM_ZORA:
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_TALKED_PART_TIMER_AS_ZORA)) {
                return sWelcomeZoraTextIds[ENOSSAN_PART_TIME_WORKER];
            }
            return sWelcomeZoraFirstTimeTextIds[ENOSSAN_PART_TIME_WORKER];

        case PLAYER_FORM_GORON:
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_TALKED_PART_TIMER_AS_GORON)) {
                return sWelcomeGoronTextIds[ENOSSAN_PART_TIME_WORKER];
            }
            return sWelcomeGoronFirstTimeTextIds[ENOSSAN_PART_TIME_WORKER];

        default:
            return sWelcomeHumanTextIds[ENOSSAN_PART_TIME_WORKER];
    }
}

void EnOssan_SetTalked(EnOssan* this) {
    switch (this->textId) {
        case 0x06A9:
            SET_WEEKEVENTREG(WEEKEVENTREG_TALKED_CURIOSITY_SHOP_MAN_AS_DEKU);
            break;

        case 0x06C6:
            SET_WEEKEVENTREG(WEEKEVENTREG_TALKED_PART_TIMER_AS_DEKU);
            break;

        case 0x06A7:
            SET_WEEKEVENTREG(WEEKEVENTREG_TALKED_CURIOSITY_SHOP_MAN_AS_ZORA);
            break;

        case 0x06C4:
            SET_WEEKEVENTREG(WEEKEVENTREG_TALKED_PART_TIMER_AS_ZORA);
            break;

        case 0x06A5:
            SET_WEEKEVENTREG(WEEKEVENTREG_TALKED_CURIOSITY_SHOP_MAN_AS_GORON);
            break;

        case 0x06C2:
            SET_WEEKEVENTREG(WEEKEVENTREG_TALKED_PART_TIMER_AS_GORON);
            break;

        default:
            break;
    }
}

void EnOssan_InitShop(EnOssan* this, PlayState* play) {
    static EnOssanActionFunc sInitFuncs[] = { EnOssan_CuriosityShopMan_Init, EnOssan_PartTimer_Init };
    ShopItem* shopItems;

    if (Object_IsLoaded(&play->objectCtx, this->objectSlot)) {
        this->actor.flags &= ~ACTOR_FLAG_UPDATE_CULLING_DISABLED;
        this->actor.objectSlot = this->objectSlot;
        Actor_SetObjectDependency(play, &this->actor);
        shopItems = sShops[this->actor.params];
        ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 20.0f);
        sInitFuncs[this->actor.params](this, play);
        this->textId = sWelcomeHumanTextIds[this->actor.params];
        EnOssan_GetCutscenes(this, play);

        this->cursorPos.y = this->cursorPos.x = 100.0f;
        this->cutsceneState = ENOSSAN_CUTSCENESTATE_STOPPED;
        this->csId = this->lookToShopkeeperCsId;
        this->actor.colChkInfo.mass = MASS_IMMOVABLE;
        this->actor.colChkInfo.cylRadius = 50;
        this->stickAccumX = this->stickAccumY = 0;

        this->cursorIndex = 0;
        this->cursorPos.z = 1.5f;
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
        this->stickRightPrompt.arrowColor.g = 255;
        this->stickRightPrompt.arrowColor.b = 0;
        this->stickRightPrompt.arrowColor.a = 200;
        this->stickRightPrompt.arrowTexX = 290.0f;
        this->stickRightPrompt.arrowTexY = 91.0f;
        this->stickRightPrompt.texZ = 1.0f;
        this->stickRightPrompt.isEnabled = false;

        this->arrowAnimState = 0;
        this->stickAnimState = 0;
        this->arrowAnimTween = 0.0f;
        this->stickAnimTween = 0.0f;
        this->shopItemSelectedTween = 0.0f;

        Actor_SetScale(&this->actor, sActorScales[this->actor.params]);
        this->animIndex = 1; // FSN_ANIM_SCRATCH_BACK and ANI_ANIM_STANDING_NORMAL_LOOP_2
        SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfoList[this->actor.params], 1);
        EnOssan_SpawnShopItems(this, play, shopItems);
        this->blinkTimer = 20;
        this->eyeTexIndex = 0;
        this->blinkFunc = EnOssan_WaitForBlink;
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        EnOssan_SetupAction(this, EnOssan_Idle);
    }
}

void EnOssan_GetCutscenes(EnOssan* this, PlayState* play) {
    this->lookToShopkeeperCsId = this->actor.csId;
    this->lookToLeftShelfCsId = CutsceneManager_GetAdditionalCsId(this->lookToShopkeeperCsId);
    this->lookToRightShelfCsId = CutsceneManager_GetAdditionalCsId(this->lookToLeftShelfCsId);
    this->lookToShopKeeperFromShelfCsId = CutsceneManager_GetAdditionalCsId(this->lookToRightShelfCsId);
}

void EnOssan_Update(Actor* thisx, PlayState* play) {
    EnOssan* this = (EnOssan*)thisx;

    if (this->actionFunc != EnOssan_InitShop) {
        this->blinkFunc(this);
        EnOssan_UpdateJoystickInputState(play, this);
        EnOssan_UpdateItemSelectedProperty(this);
        EnOssan_UpdateStickDirectionPromptAnim(this);
        EnOssan_UpdateCursorAnim(this);
        Actor_TrackPlayer(play, &this->actor, &this->headRot, &this->torsoRot, this->actor.focus.pos);
        this->actionFunc(this, play);
        Actor_SetFocus(&this->actor, 90.0f);
        SkelAnime_Update(&this->skelAnime);
    } else {
        this->actionFunc(this, play);
    }
}

void EnOssan_DrawCursor(PlayState* play, EnOssan* this, f32 x, f32 y, f32 z, u8 drawCursor) {
    s32 ulx;
    s32 uly;
    s32 lrx;
    s32 lry;
    f32 w;
    s32 dsdx;

    (void)"../z_en_oB1.c";

    OPEN_DISPS(play->state.gfxCtx);

    if (drawCursor != 0) {
        Gfx_SetupDL39_Overlay(play->state.gfxCtx);
        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, this->cursorColor.r, this->cursorColor.g, this->cursorColor.b,
                        this->cursorColor.a);
        gDPLoadTextureBlock_4b(OVERLAY_DISP++, gSelectionCursorTex, G_IM_FMT_IA, 16, 16, 0, G_TX_MIRROR | G_TX_WRAP,
                               G_TX_MIRROR | G_TX_WRAP, 4, 4, G_TX_NOLOD, G_TX_NOLOD);
        w = 16.0f * z;
        ulx = (x - w) * 4.0f;
        uly = (y - w) * 4.0f;
        lrx = (x + w) * 4.0f;
        lry = (y + w) * 4.0f;
        dsdx = (1.0f / z) * 1024.0f;
        gSPTextureRectangle(OVERLAY_DISP++, ulx, uly, lrx, lry, G_TX_RENDERTILE, 0, 0, dsdx, dsdx);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnOssan_DrawTextRec(PlayState* play, s32 r, s32 g, s32 b, s32 a, f32 x, f32 y, f32 z, s32 s, s32 t, f32 dx,
                         f32 dy) {
    f32 unk;
    s32 ulx;
    s32 uly;
    s32 lrx;
    s32 lry;
    f32 w;
    f32 h;
    s32 dsdx;
    s32 dtdy;

    (void)"../z_en_oB1.c";

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

void EnOssan_DrawStickDirectionPrompts(PlayState* play, EnOssan* this) {
    s32 drawStickRightPrompt = this->stickLeftPrompt.isEnabled;
    s32 drawStickLeftPrompt = this->stickRightPrompt.isEnabled;

    if (CVarGetInteger("gModes.MirroredWorld.State", 0) && (drawStickLeftPrompt != drawStickRightPrompt)) {
        drawStickLeftPrompt = !drawStickLeftPrompt;
        drawStickRightPrompt = !drawStickRightPrompt;
    }

    (void)"../z_en_oB1.c";

    OPEN_DISPS(play->state.gfxCtx);

    if (drawStickRightPrompt || drawStickLeftPrompt) {
        Gfx_SetupDL39_Overlay(play->state.gfxCtx);
        gDPSetCombineMode(OVERLAY_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
        gDPLoadTextureBlock(OVERLAY_DISP++, gArrowCursorTex, G_IM_FMT_IA, G_IM_SIZ_8b, 16, 24, 0,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 4, G_TX_NOMASK, G_TX_NOLOD,
                            G_TX_NOLOD);
        if (drawStickRightPrompt) {
            EnOssan_DrawTextRec(play, this->stickLeftPrompt.arrowColor.r, this->stickLeftPrompt.arrowColor.g,
                                this->stickLeftPrompt.arrowColor.b, this->stickLeftPrompt.arrowColor.a,
                                this->stickLeftPrompt.arrowTexX, this->stickLeftPrompt.arrowTexY,
                                this->stickLeftPrompt.texZ, 0, 0, -1.0f, 1.0f);
        }
        if (drawStickLeftPrompt) {
            EnOssan_DrawTextRec(play, this->stickRightPrompt.arrowColor.r, this->stickRightPrompt.arrowColor.g,
                                this->stickRightPrompt.arrowColor.b, this->stickRightPrompt.arrowColor.a,
                                this->stickRightPrompt.arrowTexX, this->stickRightPrompt.arrowTexY,
                                this->stickRightPrompt.texZ, 0, 0, 1.0f, 1.0f);
        }
        gDPLoadTextureBlock(OVERLAY_DISP++, gControlStickTex, G_IM_FMT_IA, G_IM_SIZ_8b, 16, 16, 0,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 4, G_TX_NOMASK, G_TX_NOLOD,
                            G_TX_NOLOD);
        if (drawStickRightPrompt) {
            EnOssan_DrawTextRec(play, this->stickLeftPrompt.stickColor.r, this->stickLeftPrompt.stickColor.g,
                                this->stickLeftPrompt.stickColor.b, this->stickLeftPrompt.stickColor.a,
                                this->stickLeftPrompt.stickTexX, this->stickLeftPrompt.stickTexY,
                                this->stickLeftPrompt.texZ, 0, 0, -1.0f, 1.0f);
        }
        if (drawStickLeftPrompt) {
            EnOssan_DrawTextRec(play, this->stickRightPrompt.stickColor.r, this->stickRightPrompt.stickColor.g,
                                this->stickRightPrompt.stickColor.b, this->stickRightPrompt.stickColor.a,
                                this->stickRightPrompt.stickTexX, this->stickRightPrompt.stickTexY,
                                this->stickRightPrompt.texZ, 0, 0, 1.0f, 1.0f);
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

s32 EnOssan_CuriosityShopMan_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                              Actor* thisx) {
    EnOssan* this = (EnOssan*)thisx;

    if (limbIndex == FSN_LIMB_HEAD) {
        Matrix_RotateXS(this->headRot.y, MTXMODE_APPLY);
        Matrix_Translate(CVarGetFloat(CVAR_COSMETIC("Silly.GoronNeckLength"), 0.0f), 0.0f, 0.0f, MTXMODE_APPLY);
    }
    return false;
}

s32 EnOssan_PartTimer_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                       Actor* thisx) {
    EnOssan* this = (EnOssan*)thisx;

    if (limbIndex == ANI_LIMB_HEAD) {
        Matrix_RotateXS(this->partTimerHeadRot.y, MTXMODE_APPLY);
        Matrix_RotateZS(this->partTimerHeadRot.x, MTXMODE_APPLY);
        Matrix_Translate(CVarGetFloat(CVAR_COSMETIC("Silly.GoronNeckLength"), 0.0f), 0.0f, 0.0f, MTXMODE_APPLY);
    }
    return false;
}

void EnOssan_CuriosityShopMan_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    EnOssan* this = (EnOssan*)thisx;

    if ((limbIndex == FSN_LIMB_PELVIS) || (limbIndex == FSN_LIMB_LEFT_UPPER_ARM) ||
        (limbIndex == FSN_LIMB_RIGHT_UPPER_ARM)) {
        rot->y += TRUNCF_BINANG(Math_SinS(this->fidgetTableY[limbIndex])) * 200;
        rot->z += TRUNCF_BINANG(Math_CosS(this->fidgetTableZ[limbIndex])) * 200;
    }
}

void EnOssan_PartTimer_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    static Vec3f sFocusOffset = { 800.0f, 500.0f, 0.0f };
    EnOssan* this = (EnOssan*)thisx;

    if (limbIndex == ANI_LIMB_HEAD) {
        Matrix_MultVec3f(&sFocusOffset, &this->actor.focus.pos);
    }
}

void EnOssan_CuriosityShopMan_Draw(Actor* thisx, PlayState* play) {
    static TexturePtr sEyeTextures[] = { gFsnEyeOpenTex, gFsnEyeHalfTex, gFsnEyeClosedTex };
    s32 pad;
    EnOssan* this = (EnOssan*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(sEyeTextures[this->eyeTexIndex]));
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnOssan_CuriosityShopMan_OverrideLimbDraw, EnOssan_CuriosityShopMan_PostLimbDraw,
                          &this->actor);
    EnOssan_DrawCursor(play, this, this->cursorPos.x, this->cursorPos.y, this->cursorPos.z, this->drawCursor);
    EnOssan_DrawStickDirectionPrompts(play, this);

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnOssan_PartTimer_Draw(Actor* thisx, PlayState* play) {
    static TexturePtr sEyeTextures[] = { gAniOpenEyeTex, gAniClosingEyeTex, gAniClosedEyeTex };
    s32 pad;
    EnOssan* this = (EnOssan*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(sEyeTextures[this->eyeTexIndex]));
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnOssan_PartTimer_OverrideLimbDraw, EnOssan_PartTimer_PostLimbDraw, &this->actor);
    EnOssan_DrawCursor(play, this, this->cursorPos.x, this->cursorPos.y, this->cursorPos.z, this->drawCursor);
    EnOssan_DrawStickDirectionPrompts(play, this);

    CLOSE_DISPS(play->state.gfxCtx);
}
