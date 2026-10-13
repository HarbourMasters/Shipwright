/*
 * File: z_en_sob1.c
 * Overlay: ovl_En_Sob1
 * Description: Shops (Zora, Goron, and Bomb)
 */

#include "z_en_sob1.h"
#include "objects/object_mastergolon/object_mastergolon.h"
#include "objects/object_masterzoora/object_masterzoora.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/BenGui/CosmeticEditor.h"
#include <libultraship/bridge/consolevariablebridge.h>

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void EnSob1_Init(Actor* thisx, PlayState* play);
void EnSob1_Destroy(Actor* thisx, PlayState* play);
void EnSob1_Update(Actor* thisx, PlayState* play);
void EnSob1_Reset(void);

void EnSob1_ZoraShopkeeper_Draw(Actor* thisx, PlayState* play);
void EnSob1_GoronShopkeeper_Draw(Actor* thisx, PlayState* play);
void EnSob1_BombShopkeeper_Draw(Actor* thisx, PlayState* play);

void EnSob1_ZoraShopkeeper_Init(EnSob1* this, PlayState* play);
void EnSob1_GoronShopkeeper_Init(EnSob1* this, PlayState* play);
void EnSob1_BombShopkeeper_Init(EnSob1* this, PlayState* play);

void EnSob1_InitShop(EnSob1* this, PlayState* play);
void EnSob1_Idle(EnSob1* this, PlayState* play);
void EnSob1_Walk(EnSob1* this, PlayState* play);
void EnSob1_Walking(EnSob1* this, PlayState* play);
void EnSob1_Hello(EnSob1* this, PlayState* play);
void EnSob1_StartShopping(PlayState* play, EnSob1* this);
void EnSob1_SetupIdle(EnSob1* this, PlayState* play);
void EnSob1_FaceShopkeeper(EnSob1* this, PlayState* play);
void EnSob1_LookToShelf(EnSob1* this, PlayState* play);
void EnSob1_EndingInteraction(EnSob1* this, PlayState* play);
void EnSob1_BrowseShelf(EnSob1* this, PlayState* play);
void EnSob1_TalkingToShopkeeper(EnSob1* this, PlayState* play);
void EnSob1_SelectItem(EnSob1* this, PlayState* play);
void EnSob1_LookToShopkeeperFromShelf(EnSob1* this, PlayState* play);
void EnSob1_BuyItemWithFanfare(EnSob1* this, PlayState* play);
void EnSob1_CanBuy(EnSob1* this, PlayState* play);
void EnSob1_CannotBuy(EnSob1* this, PlayState* play);
void EnSob1_SetupItemPurchased(EnSob1* this, PlayState* play);
void EnSob1_ContinueShopping(EnSob1* this, PlayState* play);
void EnSob1_ResetItemPosition(EnSob1* this);

void EnSob1_Blink(EnSob1* this);

s32 EnSob1_TakeItemOffShelf(EnSob1* this);
s32 EnSob1_ReturnItemToShelf(EnSob1* this);
s16 EnSob1_GetDistSqAndOrient(Path* path, s32 pointIndex, Vec3f* pos, f32* distSq);

typedef enum BombShopkeeperAnimation {
    /* 0 */ BOMB_SHOPKEEPER_ANIM_WALK,
    /* 1 */ BOMB_SHOPKEEPER_ANIM_SIT_AT_COUNTER_START,
    /* 2 */ BOMB_SHOPKEEPER_ANIM_SIT_AT_COUNTER_LOOP,
    /* 3 */ BOMB_SHOPKEEPER_ANIM_MAX
} BombShopkeeperAnimation;

static AnimationInfoS sAnimationInfo[BOMB_SHOPKEEPER_ANIM_MAX] = {
    { &gBombShopkeeperWalkAnim, 2.0f, 0, -1, ANIMMODE_LOOP, 20 },
    { &gBombShopkeeperSitAtCounterStartAnim, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },
    { &gBombShopkeeperSitAtCounterLoopAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },
};

ActorProfile En_Sob1_Profile = {
    /**/ ACTOR_EN_OSSAN,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(EnSob1),
    /**/ EnSob1_Init,
    /**/ EnSob1_Destroy,
    /**/ EnSob1_Update,
    /**/ NULL,
    /**/ EnSob1_Reset,
};

static s16 sObjectIds[][3] = {
    { OBJECT_ZO, OBJECT_ID_MAX, OBJECT_MASTERZOORA },
    { OBJECT_OF1D_MAP, OBJECT_ID_MAX, OBJECT_MASTERGOLON },
    { OBJECT_RSN, OBJECT_ID_MAX, OBJECT_ID_MAX },
    { OBJECT_OF1D_MAP, OBJECT_ID_MAX, OBJECT_MASTERGOLON },
};

static u16 sFacingShopkeeperTextIds[] = { 0x12D6, 0x0BC0, 0x0640, 0x0BC0 };

static u16 sNoRoomTextIds[] = { 0x12E3, 0x0BD3, 0x0641, 0x0BD3 };

static u16 sBuySuccessTextIds[] = { 0x12E6, 0x0BD6, 0x0647, 0x0BD6 };

static u16 sCannotGetNowTextIds[] = { 0x12E1, 0x0BD2, 0x0645, 0x0BD2 };

static u16 sNeedRupeesTextIds[] = { 0x12E5, 0x0BD5, 0x0646, 0x0BD5 };

static u16 sNeedEmptyBottleTextIds[] = { 0x12E4, 0x0BD4, 0x0645, 0x0BD4 };

static u16 sCannotGetNow2TextIds[] = { 0x12E1, 0x0BD1, 0x0641, 0x0BD1 };

static u16 sNoRoom2TextIds[] = { 0x12E3, 0x0BD3, 0x0641, 0x0BD3 };

static f32 sActorScales[] = { 0.01f, 0.01f, 0.01f, 0.01f };

static ShopItem sShops[][3] = {
    {
        { SI_POTION_RED_4, { 1258, 42, 325 } },
        { SI_ARROWS_SMALL_1, { 1240, 42, 325 } },
        { SI_SHIELD_HERO_3, { 1222, 42, 325 } },
    },
    {
        { SI_POTION_RED_5, { -57, 42, -62 } },
        { SI_ARROWS_SMALL_2, { -75, 42, -62 } },
        { SI_BOMB_2, { -93, 42, -62 } },
    },
    {
        { SI_BOMB_BAG_20_2, { 221, -7, 73 } },
        { SI_BOMBCHU, { 203, -7, 69 } },
        { SI_BOMB_1, { 185, -7, 65 } },
    },
    {
        { SI_POTION_RED_6, { -57, 42, -62 } },
        { SI_ARROWS_SMALL_3, { -75, 42, -62 } },
        { SI_BOMB_3, { -93, 42, -62 } },
    },
};

static EnSob1XZRange sPosXZRanges[] = {
    { 1170.0f, 1230.0f, 360.0f, 380.0f },
    { -142.0f, -72.0f, -20.0f, 0.0f },
    { 138.0f, 200.0f, 72.0f, 160.0f },
    { -142.0f, -72.0f, -20.0f, 0.0f },
};

static Vec3f sSelectedItemPositions[] = {
    { 1240.0f, 45.0f, 349.0f },
    { -75.0f, 45.0f, -38.0f },
    { 197.0f, -4.0f, 93.0f },
    { -75.0f, 45.0f, -38.0f },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(lockOnArrowOffset, 500, ICHAIN_STOP),
};

static EnSob1ActionFunc sInitFuncs[] = {
    EnSob1_ZoraShopkeeper_Init,
    EnSob1_GoronShopkeeper_Init,
    EnSob1_BombShopkeeper_Init,
    EnSob1_GoronShopkeeper_Init,
};

static Vec3f sPosOffset[] = {
    { 0.0f, 0.0f, 0.0f },
    { 0.0f, -4.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f },
    { 0.0f, -4.0f, 0.0f },
};

void EnSob1_ChangeAnim(SkelAnime* skelAnime, AnimationInfoS* animations, s32 animIndex) {
    f32 endFrame;

    animations += animIndex;
    if (animations->frameCount < 0) {
        endFrame = Animation_GetLastFrame(animations->animation);
    } else {
        endFrame = animations->frameCount;
    }
    Animation_Change(skelAnime, animations->animation, animations->playSpeed, animations->startFrame, endFrame,
                     animations->mode, animations->morphFrames);
}

void EnSob1_SetupAction(EnSob1* this, EnSob1ActionFunc action) {
    this->actionFunc = action;
}

bool EnSob1_TestItemSelected(PlayState* play) {
    MessageContext* msgCtx = &play->msgCtx;

    if ((msgCtx->textboxEndType == TEXTBOX_ENDTYPE_TWO_CHOICE) ||
        (msgCtx->textboxEndType == TEXTBOX_ENDTYPE_THREE_CHOICE)) {
        return CHECK_BTN_ALL(CONTROLLER1(&play->state)->press.button, BTN_A);
    }
    return CHECK_BTN_ALL(CONTROLLER1(&play->state)->press.button, BTN_A) ||
           CHECK_BTN_ALL(CONTROLLER1(&play->state)->press.button, BTN_B) ||
           CHECK_BTN_ALL(CONTROLLER1(&play->state)->press.button, BTN_CUP);
}

u16 EnSob1_GetTalkOption(EnSob1* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (this->shopType == BOMB_SHOP) {
        if ((gSaveContext.save.day == 1) && (CURRENT_TIME >= CLOCK_TIME(6, 0))) {
            return 0x648;
        } else if (CHECK_WEEKEVENTREG(WEEKEVENTREG_RECOVERED_STOLEN_BOMB_BAG)) {
            return 0x649;
        } else {
            return 0x64A;
        }
    } else if (this->shopType == ZORA_SHOP) {
        switch (player->transformation) {
            case PLAYER_FORM_DEKU:
                return 0x12D8;

            case PLAYER_FORM_GORON:
                return 0x12D9;

            case PLAYER_FORM_ZORA:
                return 0x12DA;

            default:
                return 0x12D7;
        }
    } else if (this->shopType == GORON_SHOP) {
        if (player->transformation != PLAYER_FORM_GORON) {
            return 0xBC1;
        }
        return 0xBC2;
    } else if (this->shopType == GORON_SHOP_SPRING) {
        if (player->transformation != PLAYER_FORM_GORON) {
            return 0xBC3;
        }
        return 0xBC4;
    }
    return 0;
}

u16 EnSob1_GetWelcome(EnSob1* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (this->shopType == BOMB_SHOP) {
        switch (Player_GetMask(play)) {
            case PLAYER_MASK_NONE:
            case PLAYER_MASK_BUNNY:
            case PLAYER_MASK_POSTMAN:
                return 0x644;

            // 2S2H [FD Enhancement] - Treat FD as any other non-human form
            case PLAYER_MASK_FIERCE_DEITY:
            case PLAYER_MASK_GORON:
            case PLAYER_MASK_ZORA:
            case PLAYER_MASK_DEKU:
                return 0x64B;

            case PLAYER_MASK_TRUTH:
            case PLAYER_MASK_ALL_NIGHT:
            case PLAYER_MASK_KEATON:
            case PLAYER_MASK_ROMANI:
            case PLAYER_MASK_GREAT_FAIRY:
            case PLAYER_MASK_DON_GERO:
            case PLAYER_MASK_KAMARO:
            case PLAYER_MASK_BREMEN:
            case PLAYER_MASK_SCENTS:
                return 0x685;

            case PLAYER_MASK_GARO:
            case PLAYER_MASK_CIRCUS_LEADER:
            case PLAYER_MASK_GIBDO:
            case PLAYER_MASK_CAPTAIN:
                return 0x686;

            case PLAYER_MASK_COUPLE:
                return 0x687;

            case PLAYER_MASK_STONE:
                return 0x688;

            case PLAYER_MASK_BLAST:
                return 0x689;

            case PLAYER_MASK_KAFEIS_MASK:
                return 0x68A;

            default:
                break;
        }
    } else if (this->shopType == ZORA_SHOP) {
        switch (player->transformation) {
            case PLAYER_FORM_HUMAN:
                if (CHECK_WEEKEVENTREG(WEEKEVENTREG_TALKED_ZORA_SHOPKEEPER_AS_HUMAN)) {
                    return 0x12CF;
                } else {
                    SET_WEEKEVENTREG(WEEKEVENTREG_TALKED_ZORA_SHOPKEEPER_AS_HUMAN);
                    return 0x12CE;
                }

            case PLAYER_FORM_DEKU:
                if (CHECK_WEEKEVENTREG(WEEKEVENTREG_TALKED_ZORA_SHOPKEEPER_AS_DEKU)) {
                    return 0x12D1;
                } else {
                    SET_WEEKEVENTREG(WEEKEVENTREG_TALKED_ZORA_SHOPKEEPER_AS_DEKU);
                    return 0x12D0;
                }

            case PLAYER_FORM_GORON:
                if (CHECK_WEEKEVENTREG(WEEKEVENTREG_TALKED_ZORA_SHOPKEEPER_AS_GORON)) {
                    return 0x12D3;
                } else {
                    SET_WEEKEVENTREG(WEEKEVENTREG_TALKED_ZORA_SHOPKEEPER_AS_GORON);
                    return 0x12D2;
                }

            case PLAYER_FORM_ZORA:
                if (CHECK_WEEKEVENTREG(WEEKEVENTREG_TALKED_ZORA_SHOPKEEPER_AS_ZORA)) {
                    return 0x12D5;
                } else {
                    SET_WEEKEVENTREG(WEEKEVENTREG_TALKED_ZORA_SHOPKEEPER_AS_ZORA);
                    return 0x12D4;
                }

            default:
                return 0x12CE;
        }
    } else if (this->shopType == GORON_SHOP) {
        if (player->transformation != PLAYER_FORM_GORON) {
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_TALKED_GORON_SHOPKEEPER_AS_NON_GORON)) {
                return 0xBB9;
            } else {
                SET_WEEKEVENTREG(WEEKEVENTREG_TALKED_GORON_SHOPKEEPER_AS_NON_GORON);
                return 0xBB8;
            }
        } else {
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_TALKED_GORON_SHOPKEEPER_AS_GORON)) {
                return 0xBBB;
            } else {
                SET_WEEKEVENTREG(WEEKEVENTREG_TALKED_GORON_SHOPKEEPER_AS_GORON);
                return 0xBBA;
            }
        }
    } else if (this->shopType == GORON_SHOP_SPRING) {
        if (player->transformation != PLAYER_FORM_GORON) {
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_TALKED_GORON_SHOPKEEPER_SPRING_AS_NON_GORON)) {
                return 0xBBD;
            } else {
                SET_WEEKEVENTREG(WEEKEVENTREG_TALKED_GORON_SHOPKEEPER_SPRING_AS_NON_GORON);
                return 0xBBC;
            }
        } else {
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_TALKED_GORON_SHOPKEEPER_SPRING_AS_GORON)) {
                return 0xBBF;
            } else {
                SET_WEEKEVENTREG(WEEKEVENTREG_TALKED_GORON_SHOPKEEPER_SPRING_AS_GORON);
                return 0xBBE;
            }
        }
    }

    return 0;
}

u16 EnSob1_GetGoodbye(EnSob1* this) {
    if (this->shopType == BOMB_SHOP) {
        if (gSaveContext.save.day == 1) {
            return 0x64C;
        } else if (gSaveContext.save.day == 2) {
            return 0x64D;
        } else if (!gSaveContext.save.isNight) {
            return 0x64E;
        } else {
            return 0x64F;
        }
    }
    return 0x64C;
}

void EnSob1_BombShopkeeper_EndInteraction(EnSob1* this, PlayState* play) {
    this->drawCursor = 0;
    this->stickLeftPrompt.isEnabled = false;
    this->stickRightPrompt.isEnabled = false;
    this->goodbyeTextId = EnSob1_GetGoodbye(this);
    Message_StartTextbox(play, this->goodbyeTextId, &this->actor);
    EnSob1_SetupAction(this, EnSob1_EndingInteraction);
}

void EnSob1_SpawnShopItems(EnSob1* this, PlayState* play, ShopItem* shopItem) {
    s32 i;

    for (i = 0; i < ARRAY_COUNT(this->items); i++, shopItem++) {
        if (shopItem->shopItemId < 0) {
            this->items[i] = NULL;
        } else {
            this->items[i] =
                (EnGirlA*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_GIRLA, shopItem->spawnPos.x, shopItem->spawnPos.y,
                                      shopItem->spawnPos.z, 0, 0, 0, shopItem->shopItemId);
        }
    }
}

s32 EnSob1_GetObjectIndices(EnSob1* this, PlayState* play, s16* objectIds) {
    if (objectIds[1] != OBJECT_ID_MAX) {
        this->unusedObjectSlot = Object_GetSlot(&play->objectCtx, objectIds[1]);
        if (this->unusedObjectSlot <= OBJECT_SLOT_NONE) {
            return false;
        }
    } else {
        this->unusedObjectSlot = OBJECT_SLOT_NONE;
    }
    if (objectIds[2] != OBJECT_ID_MAX) {
        this->shopkeeperAnimObjectSlot = Object_GetSlot(&play->objectCtx, objectIds[2]);
        if (this->shopkeeperAnimObjectSlot <= OBJECT_SLOT_NONE) {
            return false;
        }
    } else {
        this->shopkeeperAnimObjectSlot = OBJECT_SLOT_NONE;
    }
    return true;
}

void EnSob1_Init(Actor* thisx, PlayState* play) {
    EnSob1* this = (EnSob1*)thisx;
    s32 pad;
    s16* objectIds;

    switch (ENSOB1_GET_SHOPTYPE(thisx)) {
        case ZORA_SHOP:
            this->shopType = ZORA_SHOP;
            break;

        case GORON_SHOP:
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_SNOWHEAD_TEMPLE)) {
                this->shopType = GORON_SHOP_SPRING;
            } else {
                this->shopType = GORON_SHOP;
            }
            break;

        case BOMB_SHOP:
            this->shopType = BOMB_SHOP;
            break;

        default:
            Actor_Kill(&this->actor);
            return;
    }

    objectIds = sObjectIds[this->shopType];
    this->mainObjectSlot = Object_GetSlot(&play->objectCtx, objectIds[0]);
    if (this->mainObjectSlot <= OBJECT_SLOT_NONE) {
        Actor_Kill(&this->actor);
        return;
    }
    if (!EnSob1_GetObjectIndices(this, play, objectIds)) {
        Actor_Kill(&this->actor);
        return;
    }
    Actor_ProcessInitChain(&this->actor, sInitChain);
    EnSob1_SetupAction(this, EnSob1_InitShop);
}

void EnSob1_Destroy(Actor* thisx, PlayState* play) {
    EnSob1* this = (EnSob1*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void EnSob1_UpdateCursorPos(PlayState* play, EnSob1* this) {
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

void EnSob1_EndInteraction(PlayState* play, EnSob1* this) {
    Player* player = GET_PLAYER(play);

    if (this->cutsceneState == ENSOB1_CUTSCENESTATE_PLAYING) {
        CutsceneManager_Stop(this->csId);
        this->cutsceneState = ENSOB1_CUTSCENESTATE_STOPPED;
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
    EnSob1_SetupAction(this, EnSob1_Idle);
}

s32 EnSob1_TestEndInteraction(EnSob1* this, PlayState* play, Input* input) {
    if (CHECK_BTN_ALL(input->press.button, BTN_B)) {
        if (this->shopType == BOMB_SHOP) {
            EnSob1_BombShopkeeper_EndInteraction(this, play);
        } else {
            EnSob1_EndInteraction(play, this);
        }
        return true;
    }
    return false;
}

s32 EnSob1_TestCancelOption(EnSob1* this, PlayState* play, Input* input) {
    if (CHECK_BTN_ALL(input->press.button, BTN_B)) {
        this->actionFunc = this->prevActionFunc;
        Message_ContinueTextbox(play, this->items[this->cursorIndex]->actor.textId);
        return true;
    }
    return false;
}

void EnSob1_SetupStartShopping(PlayState* play, EnSob1* this, u8 skipHello) {
    Interface_SetAButtonDoAction(play, DO_ACTION_NEXT);
    if (!skipHello) {
        EnSob1_SetupAction(this, EnSob1_Hello);
    } else {
        EnSob1_StartShopping(play, this);
    }
}

void EnSob1_StartShopping(PlayState* play, EnSob1* this) {
    EnSob1_SetupAction(this, EnSob1_FaceShopkeeper);
    Message_ContinueTextbox(play, sFacingShopkeeperTextIds[this->shopType]);
    Interface_SetAButtonDoAction(play, DO_ACTION_DECIDE);
    this->stickLeftPrompt.isEnabled = false;
    this->stickRightPrompt.isEnabled = true;
}

void EnSob1_TalkToShopkeeper(PlayState* play, EnSob1* this) {
    EnSob1_SetupAction(this, EnSob1_TalkingToShopkeeper);
    this->talkOptionTextId = EnSob1_GetTalkOption(this, play);
    Message_ContinueTextbox(play, this->talkOptionTextId);
    Interface_SetAButtonDoAction(play, DO_ACTION_DECIDE);
    this->stickLeftPrompt.isEnabled = false;
    this->stickRightPrompt.isEnabled = false;
}

void EnSob1_SetupLookToShopkeeperFromShelf(PlayState* play, EnSob1* this) {
    Audio_PlaySfx(NA_SE_SY_CURSOR);
    this->drawCursor = 0;
    EnSob1_SetupAction(this, EnSob1_LookToShopkeeperFromShelf);
}

void EnSob1_EndingInteraction(EnSob1* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_DONE) && Message_ShouldAdvance(play)) {
        EnSob1_EndInteraction(play, this);
    }
}

void EnSob1_SetupWalk(EnSob1* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if ((player->actor.world.pos.x >= 0.0f) && (player->actor.world.pos.x <= 390.0f) &&
        (player->actor.world.pos.z >= 72.0f) && (player->actor.world.pos.z <= 365.0f)) {
        EnSob1_SetupAction(this, EnSob1_Walk);
    }
}

void EnSob1_Idle(EnSob1* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    this->headRotTarget = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        if (this->cutsceneState == ENSOB1_CUTSCENESTATE_STOPPED) {
            if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
                CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
            }
            this->csId = this->lookFowardCsId;
            CutsceneManager_Queue(this->csId);
            this->cutsceneState = ENSOB1_CUTSCENESTATE_WAITING;
        }
        player->stateFlags2 |= PLAYER_STATE2_20000000;
        this->welcomeTextId = EnSob1_GetWelcome(this, play);
        Message_StartTextbox(play, this->welcomeTextId, &this->actor);
        if (ENSOB1_GET_SHOPTYPE(&this->actor) == BOMB_SHOP) {
            this->headRotTarget = -0x2000;
        }
        EnSob1_SetupStartShopping(play, this, false);
    } else {
        if ((player->actor.world.pos.x >= this->posXZRange.xMin &&
             player->actor.world.pos.x <= this->posXZRange.xMax) &&
            (player->actor.world.pos.z >= this->posXZRange.zMin &&
             player->actor.world.pos.z <= this->posXZRange.zMax)) {
            Actor_OfferTalk(&this->actor, play, 400.0f);
        }
        if (this->wasTalkedToWhileWalking == true) {
            this->wasTalkedToWhileWalking = false;
            EnSob1_SetupStartShopping(play, this, false);
        }
    }
}

void EnSob1_UpdateJoystickInputState(PlayState* play, EnSob1* this) {
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

u8 EnSob1_SetCursorIndexFromNeutral(EnSob1* this, u8 shelfOffset) {
    if (this->items[shelfOffset] != NULL) {
        return shelfOffset;
    }
    return CURSOR_INVALID;
}

void EnSob1_Hello(EnSob1* this, PlayState* play) {
    u8 talkState = Message_GetState(&play->msgCtx);

    if (this->cutsceneState == ENSOB1_CUTSCENESTATE_WAITING) {
        if (CutsceneManager_IsNext(this->csId)) {
            CutsceneManager_StartWithPlayerCsAndSetFlag(this->csId, &this->actor);
            this->cutsceneState = ENSOB1_CUTSCENESTATE_PLAYING;
        } else {
            CutsceneManager_Queue(this->csId);
        }
    }
    if ((talkState == TEXT_STATE_EVENT) && Message_ShouldAdvance(play) &&
        !EnSob1_TestEndInteraction(this, play, CONTROLLER1(&play->state))) {
        if (this->welcomeTextId == 0x68A) { // Welcome text when wearing Kafei's mask
            EnSob1_EndInteraction(play, this);
        } else {
            EnSob1_StartShopping(play, this);
        }
    }
}

s32 EnSob1_FacingShopkeeperDialogResult(EnSob1* this, PlayState* play) {
    switch (play->msgCtx.choiceIndex) {
        case 0:
            Audio_PlaySfx_MessageDecide();
            EnSob1_TalkToShopkeeper(play, this);
            return true;

        case 1:
            Audio_PlaySfx_MessageCancel();
            if (this->shopType == BOMB_SHOP) {
                EnSob1_BombShopkeeper_EndInteraction(this, play);
            } else {
                EnSob1_EndInteraction(play, this);
            }
            return true;
    }
    return false;
}

void EnSob1_FaceShopkeeper(EnSob1* this, PlayState* play) {
    s32 pad[2];
    u8 talkState = Message_GetState(&play->msgCtx);
    u8 cursorIndex;

    if (this->cutsceneState == ENSOB1_CUTSCENESTATE_WAITING) {
        if (CutsceneManager_IsNext(this->csId)) {
            CutsceneManager_StartWithPlayerCsAndSetFlag(this->csId, &this->actor);
            this->cutsceneState = ENSOB1_CUTSCENESTATE_PLAYING;
        } else {
            CutsceneManager_Queue(this->csId);
        }
    }
    if (this->cutsceneState == ENSOB1_CUTSCENESTATE_STOPPED) {
        if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
            CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
        }
        this->csId = this->lookFowardCsId;
        CutsceneManager_Queue(this->csId);
        this->cutsceneState = ENSOB1_CUTSCENESTATE_WAITING;
    } else {
        if (talkState == TEXT_STATE_CHOICE) {
            Interface_SetAButtonDoAction(play, DO_ACTION_DECIDE);
            if (!EnSob1_TestEndInteraction(this, play, CONTROLLER1(&play->state))) {
                if (!Message_ShouldAdvance(play) || !EnSob1_FacingShopkeeperDialogResult(this, play)) {
                    if (this->stickAccumX > 0) {
                        cursorIndex = EnSob1_SetCursorIndexFromNeutral(this, 2);
                        if (cursorIndex != CURSOR_INVALID) {
                            this->cursorIndex = cursorIndex;
                            EnSob1_SetupAction(this, EnSob1_LookToShelf);
                            Interface_SetAButtonDoAction(play, DO_ACTION_DECIDE);
                            this->stickRightPrompt.isEnabled = false;
                            Audio_PlaySfx(NA_SE_SY_CURSOR);
                        }
                    }
                }
            }
        }
    }
}

void EnSob1_TalkingToShopkeeper(EnSob1* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        EnSob1_StartShopping(play, this);
    }
}

void EnSob1_LookToShopkeeperFromShelf(EnSob1* this, PlayState* play) {
    if (this->cutsceneState == ENSOB1_CUTSCENESTATE_PLAYING) {
        CutsceneManager_Stop(this->csId);
        this->cutsceneState = ENSOB1_CUTSCENESTATE_STOPPED;
    }
    if (this->cutsceneState == ENSOB1_CUTSCENESTATE_STOPPED) {
        if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
            CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
        }
        this->csId = this->lookToShopkeeperCsId;
        CutsceneManager_Queue(this->csId);
        this->cutsceneState = ENSOB1_CUTSCENESTATE_WAITING;
    } else if (this->cutsceneState == ENSOB1_CUTSCENESTATE_WAITING) {
        if (CutsceneManager_IsNext(this->csId)) {
            CutsceneManager_StartWithPlayerCsAndSetFlag(this->csId, &this->actor);
            this->cutsceneState = ENSOB1_CUTSCENESTATE_PLAYING;
            EnSob1_StartShopping(play, this);
        } else {
            CutsceneManager_Queue(this->csId);
        }
    }
}

void EnSob1_EndWalk(EnSob1* this, PlayState* play) {
    s32 pad;
    f32 distSq;
    s16 curFrame = this->skelAnime.curFrame / this->skelAnime.playSpeed;
    s16 endFrame = Animation_GetLastFrame(&gBombShopkeeperWalkAnim) / TRUNCF_BINANG(this->skelAnime.playSpeed);

    Math_SmoothStepToS(&this->actor.world.rot.y,
                       EnSob1_GetDistSqAndOrient(this->path, this->waypoint - 1, &this->actor.world.pos, &distSq), 4,
                       0x3E8, 1);
    this->actor.shape.rot.y = this->actor.world.rot.y;
    Math_ApproachF(&this->actor.speed, 0.5f, 0.2f, 1.0f);
    if (distSq < 12.0f) {
        this->actor.speed = 0.0f;
        if (endFrame == curFrame) {
            EnSob1_ChangeAnim(&this->skelAnime, sAnimationInfo, BOMB_SHOPKEEPER_ANIM_SIT_AT_COUNTER_START);
            EnSob1_SetupAction(this, EnSob1_SetupIdle);
        }
    }
    Actor_MoveWithGravity(&this->actor);
}

void EnSob1_SetupIdle(EnSob1* this, PlayState* play) {
    s16 curFrame = this->skelAnime.curFrame;
    s16 endFrame = Animation_GetLastFrame(&gBombShopkeeperSitAtCounterStartAnim);

    if (endFrame == curFrame) {
        EnSob1_ChangeAnim(&this->skelAnime, sAnimationInfo, BOMB_SHOPKEEPER_ANIM_SIT_AT_COUNTER_LOOP);
        EnSob1_SetupAction(this, EnSob1_Idle);
    }
    EnSob1_Walking(this, play);
}

void EnSob1_Walk(EnSob1* this, PlayState* play) {
    s32 pad;
    f32 distSq;

    if (this->cutsceneState == ENSOB1_CUTSCENESTATE_WAITING) {
        if (CutsceneManager_IsNext(this->csId)) {
            CutsceneManager_StartWithPlayerCsAndSetFlag(this->csId, &this->actor);
            this->cutsceneState = ENSOB1_CUTSCENESTATE_PLAYING;
        } else {
            CutsceneManager_Queue(this->csId);
        }
    }
    if (this->path != NULL) {
        Math_SmoothStepToS(&this->actor.world.rot.y,
                           EnSob1_GetDistSqAndOrient(this->path, this->waypoint, &this->actor.world.pos, &distSq), 4,
                           0x3E8, 1);
        this->actor.shape.rot.y = this->actor.world.rot.y;
        this->actor.speed = 2.0f;
        if (distSq < SQ(5.0f)) {
            this->waypoint++;
            if ((this->path->count - 1) < this->waypoint) {
                this->actor.speed = 0.0f;
                EnSob1_SetupAction(this, EnSob1_EndWalk);
            }
        }
    }
    Actor_MoveWithGravity(&this->actor);
    EnSob1_Walking(this, play);
}

void EnSob1_Walking(EnSob1* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (this->cutsceneState == ENSOB1_CUTSCENESTATE_WAITING) {
        if (CutsceneManager_IsNext(this->csId)) {
            CutsceneManager_StartWithPlayerCsAndSetFlag(this->csId, &this->actor);
            this->cutsceneState = ENSOB1_CUTSCENESTATE_PLAYING;
        } else {
            CutsceneManager_Queue(this->csId);
        }
    }
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        if (this->cutsceneState == ENSOB1_CUTSCENESTATE_STOPPED) {
            if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
                CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
            }
            this->csId = this->lookFowardCsId;
            CutsceneManager_Queue(this->csId);
            this->cutsceneState = ENSOB1_CUTSCENESTATE_WAITING;
        }
        player->stateFlags2 |= PLAYER_STATE2_20000000;
        this->welcomeTextId = EnSob1_GetWelcome(this, play);
        Message_StartTextbox(play, this->welcomeTextId, &this->actor);
        this->wasTalkedToWhileWalking = true;
    } else {
        if ((player->actor.world.pos.x >= this->posXZRange.xMin &&
             player->actor.world.pos.x <= this->posXZRange.xMax) &&
            (player->actor.world.pos.z >= this->posXZRange.zMin &&
             player->actor.world.pos.z <= this->posXZRange.zMax)) {
            Actor_OfferTalk(&this->actor, play, 400.0f);
        }
    }
}

void EnSob1_ItemPurchased(EnSob1* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (this->cutsceneState == ENSOB1_CUTSCENESTATE_STOPPED) {
        if (CutsceneManager_IsNext(this->csId)) {
            CutsceneManager_StartWithPlayerCsAndSetFlag(this->csId, &this->actor);
            player->stateFlags2 |= PLAYER_STATE2_20000000;
            EnSob1_SetupAction(this, EnSob1_ContinueShopping);
            this->cutsceneState = ENSOB1_CUTSCENESTATE_PLAYING;
        } else {
            if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
                CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
            }
            this->csId = this->lookToShopkeeperCsId;
            CutsceneManager_Queue(this->csId);
        }
    }
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        Message_ContinueTextbox(play, 0x647);
    } else {
        Actor_OfferTalkExchangeEquiCylinder(&this->actor, play, 400.0f, PLAYER_IA_MINUS1);
    }
}

void EnSob1_LookToShelf(EnSob1* this, PlayState* play) {
    if (this->cutsceneState == ENSOB1_CUTSCENESTATE_PLAYING) {
        CutsceneManager_Stop(this->csId);
        this->cutsceneState = ENSOB1_CUTSCENESTATE_STOPPED;
    }
    if (this->cutsceneState == ENSOB1_CUTSCENESTATE_STOPPED) {
        if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
            CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
        }
        this->csId = this->lookToShelfCsId;
        CutsceneManager_Queue(this->csId);
        this->cutsceneState = ENSOB1_CUTSCENESTATE_WAITING;
    } else if (this->cutsceneState == ENSOB1_CUTSCENESTATE_WAITING) {
        if (CutsceneManager_IsNext(this->csId)) {
            CutsceneManager_StartWithPlayerCsAndSetFlag(this->csId, &this->actor);
            this->cutsceneState = ENSOB1_CUTSCENESTATE_PLAYING;
            EnSob1_UpdateCursorPos(play, this);
            EnSob1_SetupAction(this, EnSob1_BrowseShelf);
            Message_ContinueTextbox(play, this->items[this->cursorIndex]->actor.textId);
        } else {
            CutsceneManager_Queue(this->csId);
        }
    }
}

void EnSob1_CursorLeftRight(PlayState* play, EnSob1* this) {
    u8 curTemp = this->cursorIndex;

    if (this->stickAccumX < 0) {
        if (curTemp != 2) {
            curTemp++;
        } else {
            EnSob1_SetupLookToShopkeeperFromShelf(play, this);
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

s32 EnSob1_HasPlayerSelectedItem(PlayState* play, EnSob1* this, Input* input) {
    EnGirlA* item = this->items[this->cursorIndex];

    if (EnSob1_TestEndInteraction(this, play, input)) {
        return true;
    }
    if (EnSob1_TestItemSelected(play)) {
        if (!item->isOutOfStock) {
            this->prevActionFunc = this->actionFunc;
            Message_ContinueTextbox(play, this->items[this->cursorIndex]->choiceTextId);
            Audio_PlaySfx(NA_SE_SY_DECIDE);
            this->stickLeftPrompt.isEnabled = false;
            this->stickRightPrompt.isEnabled = false;
            this->drawCursor = 0;
            EnSob1_SetupAction(this, EnSob1_SelectItem);
        } else {
            Audio_PlaySfx(NA_SE_SY_ERROR);
        }
        return true;
    }
    return false;
}

void EnSob1_BrowseShelf(EnSob1* this, PlayState* play) {
    u8 talkState = Message_GetState(&play->msgCtx);
    s32 pad;
    u8 prevCursorIndex = this->cursorIndex;
    u8 cursorIndex;

    if (!EnSob1_ReturnItemToShelf(this)) {
        this->delayTimer = 3;
    } else if (this->delayTimer != 0) {
        this->delayTimer--;
    } else {
        this->drawCursor = 0xFF;
        this->stickLeftPrompt.isEnabled = true;
        EnSob1_UpdateCursorPos(play, this);
        if (talkState == TEXT_STATE_EVENT) {
            Interface_SetAButtonDoAction(play, DO_ACTION_DECIDE);
            if (!EnSob1_HasPlayerSelectedItem(play, this, CONTROLLER1(&play->state))) {
                EnSob1_CursorLeftRight(play, this);
                cursorIndex = this->cursorIndex;
                if (cursorIndex != prevCursorIndex) {
                    Message_ContinueTextbox(play, this->items[cursorIndex]->actor.textId);
                    Audio_PlaySfx(NA_SE_SY_CURSOR);
                }
            }
        }
    }
}

void EnSob1_SetupBuyItemWithFanfare(PlayState* play, EnSob1* this) {
    Player* player = GET_PLAYER(play);

    Actor_OfferGetItem(&this->actor, play, this->items[this->cursorIndex]->getItemId, 300.0f, 300.0f);
    play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
    play->msgCtx.stateTimer = 4;
    player->stateFlags2 &= ~PLAYER_STATE2_20000000;
    Interface_SetHudVisibility(HUD_VISIBILITY_ALL);
    this->drawCursor = 0;
    EnSob1_SetupAction(this, EnSob1_BuyItemWithFanfare);
}

void EnSob1_SetupCannotBuy(PlayState* play, EnSob1* this, u16 textId) {
    Message_ContinueTextbox(play, textId);
    EnSob1_SetupAction(this, EnSob1_CannotBuy);
}

void EnSob1_SetupCanBuy(PlayState* play, EnSob1* this, u16 textId) {
    Message_ContinueTextbox(play, textId);
    EnSob1_SetupAction(this, EnSob1_CanBuy);
}

void EnSob1_HandleCanBuyItem(PlayState* play, EnSob1* this) {
    EnGirlA* item = this->items[this->cursorIndex];
    EnGirlA* item2;

    switch (item->canBuyFunc(play, item)) {
        case CANBUY_RESULT_SUCCESS_1:
            if (this->cutsceneState == ENSOB1_CUTSCENESTATE_PLAYING) {
                CutsceneManager_Stop(this->csId);
                this->cutsceneState = ENSOB1_CUTSCENESTATE_STOPPED;
            }
            Audio_PlaySfx_MessageDecide();
            item2 = this->items[this->cursorIndex];
            item2->buyFanfareFunc(play, item2);
            EnSob1_SetupBuyItemWithFanfare(play, this);
            this->drawCursor = 0;
            this->shopItemSelectedTween = 0.0f;
            item->boughtFunc(play, item);
            break;

        case CANBUY_RESULT_SUCCESS_2:
            Audio_PlaySfx_MessageDecide();
            item->buyFunc(play, item);
            if ((this->shopType == GORON_SHOP) && (item->actor.params == SI_POTION_RED_5)) {
                EnSob1_SetupCanBuy(play, this, 0xBD7);
            } else if ((this->shopType == ZORA_SHOP) && (item->actor.params == SI_POTION_RED_4)) {
                EnSob1_SetupCanBuy(play, this, 0x12E7);
            } else if ((this->shopType == GORON_SHOP_SPRING) && (item->actor.params == SI_POTION_RED_6)) {
                EnSob1_SetupCanBuy(play, this, 0xBD7);
            } else {
                EnSob1_SetupCanBuy(play, this, sBuySuccessTextIds[this->shopType]);
            }
            this->drawCursor = 0;
            this->shopItemSelectedTween = 0.0f;
            item->boughtFunc(play, item);
            break;

        case CANBUY_RESULT_NO_ROOM:
            Audio_PlaySfx(NA_SE_SY_ERROR);
            EnSob1_SetupCannotBuy(play, this, sNoRoomTextIds[this->shopType]);
            break;

        case CANBUY_RESULT_NEED_EMPTY_BOTTLE:
            Audio_PlaySfx(NA_SE_SY_ERROR);
            EnSob1_SetupCannotBuy(play, this, sNeedEmptyBottleTextIds[this->shopType]);
            break;

        case CANBUY_RESULT_NEED_RUPEES:
            Audio_PlaySfx(NA_SE_SY_ERROR);
            EnSob1_SetupCannotBuy(play, this, sNeedRupeesTextIds[this->shopType]);
            break;

        case CANBUY_RESULT_CANNOT_GET_NOW:
            Audio_PlaySfx(NA_SE_SY_ERROR);
            EnSob1_SetupCannotBuy(play, this, sCannotGetNowTextIds[this->shopType]);
            break;

        case CANBUY_RESULT_CANNOT_GET_NOW_2:
            Audio_PlaySfx(NA_SE_SY_ERROR);
            EnSob1_SetupCannotBuy(play, this, sCannotGetNow2TextIds[this->shopType]);
            break;

        case CANBUY_RESULT_NO_ROOM_2:
            Audio_PlaySfx(NA_SE_SY_ERROR);
            EnSob1_SetupCannotBuy(play, this, sNoRoom2TextIds[this->shopType]);
            break;

        case CANBUY_RESULT_ALREADY_HAVE:
            Audio_PlaySfx(NA_SE_SY_ERROR);
            EnSob1_SetupCannotBuy(play, this, 0x658);
            break;

        case CANBUY_RESULT_HAVE_BETTER:
            Audio_PlaySfx(NA_SE_SY_ERROR);
            EnSob1_SetupCannotBuy(play, this, 0x659);
            break;

        default:
            break;
    }
}

void EnSob1_SelectItem(EnSob1* this, PlayState* play) {
    u8 talkState = Message_GetState(&play->msgCtx);

    if (EnSob1_TakeItemOffShelf(this) && (talkState == TEXT_STATE_CHOICE)) {
        Interface_SetAButtonDoAction(play, DO_ACTION_DECIDE);
        if (!EnSob1_TestCancelOption(this, play, CONTROLLER1(&play->state)) && Message_ShouldAdvance(play)) {
            switch (play->msgCtx.choiceIndex) {
                case 0:
                    EnSob1_HandleCanBuyItem(play, this);
                    break;

                case 1:
                    Audio_PlaySfx_MessageCancel();
                    this->actionFunc = this->prevActionFunc;
                    Message_ContinueTextbox(play, this->items[this->cursorIndex]->actor.textId);
                    break;
            }
        }
    }
}

void EnSob1_CannotBuy(EnSob1* this, PlayState* play) {
    if (Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) {
        if (Message_ShouldAdvance(play)) {
            this->actionFunc = this->prevActionFunc;
            Message_ContinueTextbox(play, this->items[this->cursorIndex]->actor.textId);
        }
    }
}

void EnSob1_CanBuy(EnSob1* this, PlayState* play) {
    EnGirlA* item;

    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        this->shopItemSelectedTween = 0.0f;
        EnSob1_ResetItemPosition(this);
        item = this->items[this->cursorIndex];
        item->restockFunc(play, item);
        this->actionFunc = this->prevActionFunc;
        Message_ContinueTextbox(play, this->items[this->cursorIndex]->actor.textId);
    }
}

void EnSob1_BuyItemWithFanfare(EnSob1* this, PlayState* play) {
    if (Actor_HasParent(&this->actor, play)) {
        this->actor.parent = NULL;
        EnSob1_SetupAction(this, EnSob1_SetupItemPurchased);
    } else {
        Actor_OfferGetItem(&this->actor, play, this->items[this->cursorIndex]->getItemId, 300.0f, 300.0f);
    }
}

void EnSob1_SetupItemPurchased(EnSob1* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_DONE) && Message_ShouldAdvance(play)) {
        play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
        play->msgCtx.stateTimer = 4;
        EnSob1_SetupAction(this, EnSob1_ItemPurchased);
        if (this->cutsceneState == ENSOB1_CUTSCENESTATE_STOPPED) {
            if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
                CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
            }
            this->csId = this->lookToShopkeeperCsId;
            CutsceneManager_Queue(this->csId);
        }
        Actor_OfferTalkExchangeEquiCylinder(&this->actor, play, 400.0f, PLAYER_IA_MINUS1);
    }
}

void EnSob1_ContinueShopping(EnSob1* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    EnGirlA* item;

    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        EnSob1_ResetItemPosition(this);
        item = this->items[this->cursorIndex];
        item->restockFunc(play, item);
        player->actor.shape.rot.y += 0x8000;
        player->stateFlags2 |= PLAYER_STATE2_20000000;
        Message_StartTextbox(play, this->welcomeTextId, &this->actor);
        EnSob1_SetupStartShopping(play, this, true);
        Actor_OfferTalkExchangeEquiCylinder(&this->actor, play, 200.0f, PLAYER_IA_MINUS1);
    }
}

void EnSob1_PositionSelectedItem(EnSob1* this) {
    Vec3f selectedItemPosition = sSelectedItemPositions[this->shopType];
    u8 i = this->cursorIndex;
    EnGirlA* item;
    ShopItem* shopItem = &sShops[this->shopType][i];
    Vec3f worldPos;

    item = this->items[i];

    VEC3F_LERPIMPDST(&worldPos, &shopItem->spawnPos, &selectedItemPosition, this->shopItemSelectedTween);

    item->actor.world.pos.x = worldPos.x;
    item->actor.world.pos.y = worldPos.y;
    item->actor.world.pos.z = worldPos.z;
}

void EnSob1_ResetItemPosition(EnSob1* this) {
    this->shopItemSelectedTween = 0.0f;
    EnSob1_PositionSelectedItem(this);
}

/*
 * Returns true when animation has completed
 */
s32 EnSob1_TakeItemOffShelf(EnSob1* this) {
    Math_ApproachF(&this->shopItemSelectedTween, 1.0f, 1.0f, 0.15f);
    if (this->shopItemSelectedTween >= 0.85f) {
        this->shopItemSelectedTween = 1.0f;
    }
    EnSob1_PositionSelectedItem(this);
    if (this->shopItemSelectedTween == 1.0f) {
        return true;
    }
    return false;
}

/*
 * Returns true when animation has completed
 */
s32 EnSob1_ReturnItemToShelf(EnSob1* this) {
    Math_ApproachF(&this->shopItemSelectedTween, 0.0f, 1.0f, 0.15f);
    if (this->shopItemSelectedTween <= 0.15f) {
        this->shopItemSelectedTween = 0.0f;
    }
    EnSob1_PositionSelectedItem(this);
    if (this->shopItemSelectedTween == 0.0f) {
        return true;
    }
    return false;
}

void EnSob1_UpdateItemSelectedProperty(EnSob1* this) {
    EnGirlA** items = this->items;
    EnGirlA* item;
    s32 i;

    for (i = 0; i < ARRAY_COUNT(this->items); i++, items++) {
        item = *items;
        if (item != NULL) {
            if ((this->actionFunc != EnSob1_SelectItem) && (this->actionFunc != EnSob1_CannotBuy) &&
                (this->drawCursor == 0)) {
                item->isSelected = false;
            } else {
                item->isSelected = (this->cursorIndex == i) ? true : false;
            }
        }
    }
}

void EnSob1_UpdateCursorAnim(EnSob1* this) {
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

void EnSob1_UpdateStickDirectionPromptAnim(EnSob1* this) {
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

s16 EnSob1_GetDistSqAndOrient(Path* path, s32 pointIndex, Vec3f* pos, f32* distSq) {
    Vec3s* points;
    f32 diffX;
    f32 diffZ;

    if (path != NULL) {
        points = Lib_SegmentedToVirtual(path->points);
        points = &points[pointIndex];
        diffX = points->x - pos->x;
        diffZ = points->z - pos->z;
    } else {
        diffX = 0.0f;
        diffZ = 0.0f;
    }
    *distSq = SQ(diffX) + SQ(diffZ);
    return RAD_TO_BINANG(Math_Atan2F_XY(diffZ, diffX));
}

void EnSob1_GetCutscenes(EnSob1* this) {
    this->lookFowardCsId = this->actor.csId;
    this->lookToShelfCsId = CutsceneManager_GetAdditionalCsId(this->lookFowardCsId);
    this->lookToShopkeeperCsId = CutsceneManager_GetAdditionalCsId(this->lookToShelfCsId);
}

void EnSob1_WaitForBlink(EnSob1* this) {
    s16 decr = this->blinkTimer - 1;

    if (decr != 0) {
        this->blinkTimer = decr;
    } else {
        this->blinkFunc = EnSob1_Blink;
    }
}

void EnSob1_Blink(EnSob1* this) {
    s16 decr = this->blinkTimer - 1;
    s16 eyeTextureIndexTemp;

    if (decr != 0) {
        this->blinkTimer = decr;
        return;
    }
    eyeTextureIndexTemp = this->eyeTexIndex + 1;
    if (eyeTextureIndexTemp > 2) {
        this->eyeTexIndex = 0;
        this->blinkTimer = (s32)(Rand_ZeroOne() * 60.0f) + 20;
        this->blinkFunc = EnSob1_WaitForBlink;
    } else {
        this->eyeTexIndex = eyeTextureIndexTemp;
        this->blinkTimer = 1;
    }
}

void EnSob1_ChangeObject(EnSob1* this, PlayState* play) {
    gSegments[0x06] = OS_K0_TO_PHYSICAL(play->objectCtx.slots[this->shopkeeperAnimObjectSlot].segment);
}

s32 EnSob1_AreObjectsLoaded(EnSob1* this, PlayState* play) {
    if (Object_IsLoaded(&play->objectCtx, this->mainObjectSlot)) {
        if ((this->unusedObjectSlot > OBJECT_SLOT_NONE) && !Object_IsLoaded(&play->objectCtx, this->unusedObjectSlot)) {
            return false;
        }
        if ((this->shopkeeperAnimObjectSlot > OBJECT_SLOT_NONE) &&
            !Object_IsLoaded(&play->objectCtx, this->shopkeeperAnimObjectSlot)) {
            return false;
        }
        return true;
    }
    return false;
}

void EnSob1_ZoraShopkeeper_Init(EnSob1* this, PlayState* play) {
    SkelAnime_InitFlex(play, &this->skelAnime, &gZoraSkel, NULL, this->jointTable, this->morphTable, ZORA_LIMB_MAX);
    gSegments[6] = OS_K0_TO_PHYSICAL(play->objectCtx.slots[this->shopkeeperAnimObjectSlot].segment);
    Animation_Change(&this->skelAnime, &gZoraShopkeeperAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gZoraShopkeeperAnim),
                     ANIMMODE_LOOP, 0.0f);
    this->actor.draw = EnSob1_ZoraShopkeeper_Draw;
    this->changeObjectFunc = EnSob1_ChangeObject;
}

void EnSob1_GoronShopkeeper_Init(EnSob1* this, PlayState* play) {
    SkelAnime_InitFlex(play, &this->skelAnime, &gGoronSkel, NULL, this->jointTable, this->morphTable, GORON_LIMB_MAX);
    gSegments[6] = OS_K0_TO_PHYSICAL(play->objectCtx.slots[this->shopkeeperAnimObjectSlot].segment);
    Animation_Change(&this->skelAnime, &gGoronShopkeeperAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gGoronShopkeeperAnim),
                     ANIMMODE_LOOP, 0.0f);
    this->actor.draw = EnSob1_GoronShopkeeper_Draw;
    this->changeObjectFunc = EnSob1_ChangeObject;
}

void EnSob1_BombShopkeeper_Init(EnSob1* this, PlayState* play) {
    SkelAnime_InitFlex(play, &this->skelAnime, &gBombShopkeeperSkel, &gBombShopkeeperWalkAnim, this->jointTable,
                       this->morphTable, BOMB_SHOPKEEPER_LIMB_MAX);
    this->actor.draw = EnSob1_BombShopkeeper_Draw;
    this->changeObjectFunc = NULL;
    this->skelAnime.playSpeed = 2.0f;
}

void EnSob1_InitShop(EnSob1* this, PlayState* play) {
    ShopItem* shopItems;
    EnSob1XZRange* xzRange;
    Vec3f* posOffset;

    if (EnSob1_AreObjectsLoaded(this, play)) {
        this->actor.flags &= ~ACTOR_FLAG_UPDATE_CULLING_DISABLED;
        this->actor.objectSlot = this->mainObjectSlot;
        Actor_SetObjectDependency(play, &this->actor);
        posOffset = &sPosOffset[this->shopType];
        this->actor.world.pos.x += posOffset->x;
        this->actor.world.pos.y += posOffset->y;
        this->actor.world.pos.z += posOffset->z;
        shopItems = sShops[this->shopType];
        if ((this->shopType == BOMB_SHOP) && CHECK_WEEKEVENTREG(WEEKEVENTREG_RECOVERED_STOLEN_BOMB_BAG)) {
            sShops[this->shopType][0].shopItemId = SI_BOMB_BAG_30_2;
        }

        this->cutsceneState = ENSOB1_CUTSCENESTATE_STOPPED;
        EnSob1_GetCutscenes(this);
        this->csId = this->lookFowardCsId;
        ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 20.0f);
        sInitFuncs[this->shopType](this, play);
        this->actor.colChkInfo.mass = MASS_IMMOVABLE;
        this->actor.colChkInfo.cylRadius = 50;
        this->wasTalkedToWhileWalking = false;
        this->waypoint = 0;

        if (this->shopType == BOMB_SHOP) {
            this->path = SubS_GetPathByIndex(play, ENSOB1_GET_PATH_INDEX(&this->actor), ENSOB1_PATH_INDEX_NONE);
        }
        if (this->shopType == BOMB_SHOP) {
            EnSob1_SetupAction(this, EnSob1_SetupWalk);
        } else {
            EnSob1_SetupAction(this, EnSob1_Idle);
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

        this->arrowAnimState = 0;
        this->stickAnimState = 0;
        this->arrowAnimTween = 0.0f;
        this->stickAnimTween = 0.0f;
        this->shopItemSelectedTween = 0.0f;

        this->actor.gravity = 0.0f;
        this->posXZRange = sPosXZRanges[this->shopType];
        Actor_SetScale(&this->actor, sActorScales[this->shopType]);
        EnSob1_SpawnShopItems(this, play, shopItems);
        this->headRot = this->headRotTarget = 0;
        this->blinkTimer = 20;
        this->eyeTexIndex = 0;
        this->blinkFunc = EnSob1_WaitForBlink;
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    }
}

void EnSob1_Update(Actor* thisx, PlayState* play) {
    EnSob1ActionFunc changeObjectFunc;
    EnSob1* this = (EnSob1*)thisx;

    if (this->actionFunc != EnSob1_InitShop) {
        this->blinkFunc(this);
        EnSob1_UpdateJoystickInputState(play, this);
        EnSob1_UpdateItemSelectedProperty(this);
        EnSob1_UpdateStickDirectionPromptAnim(this);
        EnSob1_UpdateCursorAnim(this);
        Math_StepToS(&this->headRot, this->headRotTarget, 0x190);
        this->actionFunc(this, play);
        Actor_SetFocus(&this->actor, 90.0f);
        changeObjectFunc = this->changeObjectFunc;
        if (changeObjectFunc != NULL) {
            changeObjectFunc(this, play);
        }
        SkelAnime_Update(&this->skelAnime);
    } else {
        this->actionFunc(this, play);
    }
}

void EnSob1_DrawCursor(PlayState* play, EnSob1* this, f32 x, f32 y, f32 z, u8 drawCursor) {
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

void EnSob1_DrawTextRec(PlayState* play, s32 r, s32 g, s32 b, s32 a, f32 x, f32 y, f32 z, s32 s, s32 t, f32 dx,
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

    (void)"../z_en_soB1.c";

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

void EnSob1_DrawStickDirectionPrompt(PlayState* play, EnSob1* this) {
    s32 drawStickRightPrompt = this->stickLeftPrompt.isEnabled;
    s32 drawStickLeftPrompt = this->stickRightPrompt.isEnabled;

    if (CVarGetInteger("gModes.MirroredWorld.State", 0) && (drawStickLeftPrompt != drawStickRightPrompt)) {
        drawStickLeftPrompt = !drawStickLeftPrompt;
        drawStickRightPrompt = !drawStickRightPrompt;
    }

    (void)"../z_en_soB1.c";

    OPEN_DISPS(play->state.gfxCtx);

    if (drawStickRightPrompt || drawStickLeftPrompt) {
        Gfx_SetupDL39_Overlay(play->state.gfxCtx);
        gDPSetCombineMode(OVERLAY_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
        gDPLoadTextureBlock(OVERLAY_DISP++, gArrowCursorTex, G_IM_FMT_IA, G_IM_SIZ_8b, 16, 24, 0,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 4, G_TX_NOMASK, G_TX_NOLOD,
                            G_TX_NOLOD);
        if (drawStickRightPrompt) {
            EnSob1_DrawTextRec(play, this->stickLeftPrompt.arrowColor.r, this->stickLeftPrompt.arrowColor.g,
                               this->stickLeftPrompt.arrowColor.b, this->stickLeftPrompt.arrowColor.a,
                               this->stickLeftPrompt.arrowTexX, this->stickLeftPrompt.arrowTexY,
                               this->stickLeftPrompt.texZ, 0, 0, -1.0f, 1.0f);
        }
        if (drawStickLeftPrompt) {
            EnSob1_DrawTextRec(play, this->stickRightPrompt.arrowColor.r, this->stickRightPrompt.arrowColor.g,
                               this->stickRightPrompt.arrowColor.b, this->stickRightPrompt.arrowColor.a,
                               this->stickRightPrompt.arrowTexX, this->stickRightPrompt.arrowTexY,
                               this->stickRightPrompt.texZ, 0, 0, 1.0f, 1.0f);
        }
        gDPLoadTextureBlock(OVERLAY_DISP++, gControlStickTex, G_IM_FMT_IA, G_IM_SIZ_8b, 16, 16, 0,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 4, G_TX_NOMASK, G_TX_NOLOD,
                            G_TX_NOLOD);
        if (drawStickRightPrompt) {
            EnSob1_DrawTextRec(play, this->stickLeftPrompt.stickColor.r, this->stickLeftPrompt.stickColor.g,
                               this->stickLeftPrompt.stickColor.b, this->stickLeftPrompt.stickColor.a,
                               this->stickLeftPrompt.stickTexX, this->stickLeftPrompt.stickTexY,
                               this->stickLeftPrompt.texZ, 0, 0, -1.0f, 1.0f);
        }
        if (drawStickLeftPrompt) {
            EnSob1_DrawTextRec(play, this->stickRightPrompt.stickColor.r, this->stickRightPrompt.stickColor.g,
                               this->stickRightPrompt.stickColor.b, this->stickRightPrompt.stickColor.a,
                               this->stickRightPrompt.stickTexX, this->stickRightPrompt.stickTexY,
                               this->stickRightPrompt.texZ, 0, 0, 1.0f, 1.0f);
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

s32 EnSob1_ZoraShopkeeper_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                           Actor* thisx) {
    EnSob1* this = (EnSob1*)thisx;

    if (limbIndex == ZORA_LIMB_HEAD) {
        rot->x += this->headRot;
    }
    return false;
}

s32 EnSob1_GoronShopkeeper_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                            Actor* thisx) {
    if (limbIndex == GORON_LIMB_HEAD) {
        Matrix_Translate(CVarGetFloat(CVAR_COSMETIC("Silly.GoronNeckLength"), 0.0f), 0.0f, 0.0f, MTXMODE_APPLY);
    }
    return false;
}

s32 EnSob1_BombShopkeeper_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                           Actor* thisx) {
    EnSob1* this = (EnSob1*)thisx;

    if (limbIndex == BOMB_SHOPKEEPER_LIMB_HEAD) {
        Matrix_RotateXS(this->headRot, MTXMODE_APPLY);
    }
    return false;
}

void EnSob1_BombShopkeeper_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    OPEN_DISPS(play->state.gfxCtx);

    if (limbIndex == BOMB_SHOPKEEPER_LIMB_LEFT_HAND) {
        if (GameInteractor_Should(VB_DRAW_ITEM_FROM_SOB1, true, thisx)) {
            gSPDisplayList(POLY_OPA_DISP++, gBombShopkeeperBombDL);
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

Gfx* EnSob1_EndDList(GraphicsContext* gfxCtx) {
    Gfx* gfxHead = GRAPH_ALLOC(gfxCtx, sizeof(Gfx));
    Gfx* gfx = gfxHead;

    gSPEndDisplayList(gfx++);

    return gfxHead;
}

void EnSob1_ZoraShopkeeper_Draw(Actor* thisx, PlayState* play) {
    static TexturePtr sZoraShopkeeperEyeTextures[] = { gZoraEyeOpenTex, gZoraEyeHalfTex, gZoraEyeClosedTex };
    EnSob1* this = (EnSob1*)thisx;
    s32 pad;
    s32 i;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
    gSPSegment(POLY_OPA_DISP++, 0x0C, EnSob1_EndDList(play->state.gfxCtx));
    gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(sZoraShopkeeperEyeTextures[this->eyeTexIndex]));
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnSob1_ZoraShopkeeper_OverrideLimbDraw, NULL, &this->actor);
    for (i = 0; i < ARRAY_COUNT(this->items); i++) {
        this->items[i]->actor.scale.x = 0.2f;
        this->items[i]->actor.scale.y = 0.2f;
        this->items[i]->actor.scale.z = 0.2f;
    }
    EnSob1_DrawCursor(play, this, this->cursorPos.x, this->cursorPos.y, this->cursorPos.z, this->drawCursor);
    EnSob1_DrawStickDirectionPrompt(play, this);

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnSob1_GoronShopkeeper_Draw(Actor* thisx, PlayState* play) {
    static TexturePtr sGoronShopkeeperEyeTextures[] = { gGoronEyeOpenTex, gGoronEyeHalfTex, gGoronEyeClosedTex };
    EnSob1* this = (EnSob1*)thisx;
    s32 pad;
    s32 i;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(sGoronShopkeeperEyeTextures[this->eyeTexIndex]));
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnSob1_GoronShopkeeper_OverrideLimbDraw, NULL, &this->actor);
    for (i = 0; i < ARRAY_COUNT(this->items); i++) {
        this->items[i]->actor.scale.x = 0.2f;
        this->items[i]->actor.scale.y = 0.2f;
        this->items[i]->actor.scale.z = 0.2f;
    }
    EnSob1_DrawCursor(play, this, this->cursorPos.x, this->cursorPos.y, this->cursorPos.z, this->drawCursor);
    EnSob1_DrawStickDirectionPrompt(play, this);

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnSob1_BombShopkeeper_Draw(Actor* thisx, PlayState* play) {
    EnSob1* this = (EnSob1*)thisx;
    s32 pad;
    u32 frames;
    s32 i;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(gBombShopkeeperEyeTex));
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnSob1_BombShopkeeper_OverrideLimbDraw, EnSob1_BombShopkeeper_PostLimbDraw, &this->actor);
    for (i = 0; i < ARRAY_COUNT(this->items); i++) {
        this->items[i]->actor.scale.x = 0.2f;
        this->items[i]->actor.scale.y = 0.2f;
        this->items[i]->actor.scale.z = 0.2f;
    }
    EnSob1_DrawCursor(play, this, this->cursorPos.x, this->cursorPos.y, this->cursorPos.z, this->drawCursor);
    EnSob1_DrawStickDirectionPrompt(play, this);
    frames = play->gameplayFrames;
    Gfx_SetupDL25_Xlu(play->state.gfxCtx);
    Matrix_ReplaceRotation(&play->billboardMtxF);
    Matrix_Scale(1.0f, 1.0f, 1.0f, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
    gSPSegment(POLY_XLU_DISP++, 0x08,
               Gfx_TwoTexScrollEx(play->state.gfxCtx, 0, 0, 0, 32, 64, 1, 0, -frames * 20, 32, 128, 0, 0, 0, -20));
    gDPSetPrimColor(POLY_XLU_DISP++, 128, 128, 255, 255, 0, 255);
    gDPSetEnvColor(POLY_XLU_DISP++, 255, 0, 0, 0);

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnSob1_Reset(void) {
    sShops[BOMB_SHOP][0].shopItemId = SI_BOMB_BAG_20_2;
}
