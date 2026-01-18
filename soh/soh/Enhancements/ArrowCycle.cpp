#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "macros.h"
#include "variables.h"
#include "functions.h"
#include "overlays/actors/ovl_En_Arrow/z_en_arrow.h"

s32 func_808351D4(Player* thisx, PlayState* play); // Arrow nocked
s32 func_808353D8(Player* thisx, PlayState* play); // Aiming in first person
void Player_InitItemAction(PlayState* play, Player* thisx, PlayerItemAction itemAction);

extern PlayState* gPlayState;
}

#define CVAR_ARROW_CYCLE_NAME CVAR_ENHANCEMENT("BowArrowCycle")
#define CVAR_ARROW_CYCLE_DEFAULT 0
#define CVAR_ARROW_CYCLE_VALUE CVarGetInteger(CVAR_ARROW_CYCLE_NAME, CVAR_ARROW_CYCLE_DEFAULT)

static const s16 sMagicArrowCosts[] = { 4, 4, 8 };

#define MINIGAME_STATUS_ACTIVE 1

static const s16 BUTTON_FLASH_DURATION = 3;
static const s16 BUTTON_FLASH_COUNT = 3;
static const s16 BUTTON_HIGHLIGHT_ALPHA = 128;

static s16 sButtonFlashTimer = 0;
static s16 sButtonFlashCount = 0;
static s16 sJustCycledFrames = 0;

static const PlayerItemAction sArrowCycleOrder[] = {
    PLAYER_IA_BOW,
    PLAYER_IA_BOW_FIRE,
    PLAYER_IA_BOW_ICE,
    PLAYER_IA_BOW_LIGHT,
};

static bool IsHoldingBow(Player* player) {
    return player->heldItemAction >= PLAYER_IA_BOW && player->heldItemAction <= PLAYER_IA_BOW_LIGHT;
}

static bool IsHoldingMagicBow(Player* player) {
    return player->heldItemAction >= PLAYER_IA_BOW_FIRE && player->heldItemAction <= PLAYER_IA_BOW_LIGHT;
}

static bool IsAimingBow(Player* player) {
    return IsHoldingBow(player) && ((player->unk_6AD == 2) || (player->upperActionFunc == func_808351D4));
}

static bool HasArrowType(PlayerItemAction itemAction) {
    switch (itemAction) {
        case PLAYER_IA_BOW:
            return true;
        case PLAYER_IA_BOW_FIRE:
            return (INV_CONTENT(ITEM_ARROW_FIRE) == ITEM_ARROW_FIRE);
        case PLAYER_IA_BOW_ICE:
            return (INV_CONTENT(ITEM_ARROW_ICE) == ITEM_ARROW_ICE);
        case PLAYER_IA_BOW_LIGHT:
            return (INV_CONTENT(ITEM_ARROW_LIGHT) == ITEM_ARROW_LIGHT);
        default:
            return false;
    }
}

static s32 GetBowItemForArrow(PlayerItemAction itemAction) {
    switch (itemAction) {
        case PLAYER_IA_BOW_FIRE:
            return ITEM_BOW_ARROW_FIRE;
        case PLAYER_IA_BOW_ICE:
            return ITEM_BOW_ARROW_ICE;
        case PLAYER_IA_BOW_LIGHT:
            return ITEM_BOW_ARROW_LIGHT;
        default:
            return ITEM_BOW;
    }
}

static bool CanCycleArrows() {
    Player* player = GET_PLAYER(gPlayState);

    // don't allow cycling during minigames
    if (gSaveContext.minigameState == MINIGAME_STATUS_ACTIVE) {
        return false;
    }

    return !(player->stateFlags1 & PLAYER_STATE1_ON_HORSE) && player->rideActor == NULL &&
           INV_CONTENT(SLOT_BOW) == ITEM_BOW &&
           (INV_CONTENT(ITEM_ARROW_FIRE) == ITEM_ARROW_FIRE || INV_CONTENT(ITEM_ARROW_ICE) == ITEM_ARROW_ICE ||
            INV_CONTENT(ITEM_ARROW_LIGHT) == ITEM_ARROW_LIGHT);
}

static s8 GetNextArrowType(s8 currentArrowType) {
    int currentIndex = 0;
    for (int i = 0; i < (int)ARRAY_COUNT(sArrowCycleOrder); i++) {
        if (sArrowCycleOrder[i] == currentArrowType) {
            currentIndex = i;
            break;
        }
    }

    for (int offset = 1; offset <= (int)ARRAY_COUNT(sArrowCycleOrder); offset++) {
        int nextIndex = (currentIndex + offset) % ARRAY_COUNT(sArrowCycleOrder);
        if (HasArrowType(sArrowCycleOrder[nextIndex])) {
            return sArrowCycleOrder[nextIndex];
        }
    }

    return PLAYER_IA_BOW;
}

static void UpdateButtonAlpha(s16 flashAlpha, bool isButtonBow, u16* buttonAlpha) {
    if (isButtonBow) {
        *buttonAlpha = flashAlpha;
        if (sButtonFlashTimer == 0) {
            *buttonAlpha = 255;
        }
    }
}

static void UpdateFlashEffect(PlayState* play) {
    if (sButtonFlashTimer <= 0) {
        return;
    }

    sButtonFlashTimer--;
    s16 flashAlpha = (sButtonFlashTimer % 3) ? BUTTON_HIGHLIGHT_ALPHA : 255;

    if (sButtonFlashTimer == 0 && sButtonFlashCount < BUTTON_FLASH_COUNT - 1) {
        sButtonFlashTimer = BUTTON_FLASH_DURATION;
        sButtonFlashCount++;
    }
    UpdateButtonAlpha(flashAlpha,
                      (gSaveContext.equips.buttonItems[1] == ITEM_BOW) ||
                          (gSaveContext.equips.buttonItems[1] >= ITEM_BOW_ARROW_FIRE &&
                           gSaveContext.equips.buttonItems[1] <= ITEM_BOW_ARROW_LIGHT),
                      &play->interfaceCtx.cLeftAlpha);

    UpdateButtonAlpha(flashAlpha,
                      (gSaveContext.equips.buttonItems[2] == ITEM_BOW) ||
                          (gSaveContext.equips.buttonItems[2] >= ITEM_BOW_ARROW_FIRE &&
                           gSaveContext.equips.buttonItems[2] <= ITEM_BOW_ARROW_LIGHT),
                      &play->interfaceCtx.cDownAlpha);

    UpdateButtonAlpha(flashAlpha,
                      (gSaveContext.equips.buttonItems[3] == ITEM_BOW) ||
                          (gSaveContext.equips.buttonItems[3] >= ITEM_BOW_ARROW_FIRE &&
                           gSaveContext.equips.buttonItems[3] <= ITEM_BOW_ARROW_LIGHT),
                      &play->interfaceCtx.cRightAlpha);

    if (CVarGetInteger(CVAR_ENHANCEMENT("DpadEquips"), 0)) {
        UpdateButtonAlpha(flashAlpha,
                          (gSaveContext.equips.buttonItems[4] == ITEM_BOW) ||
                              (gSaveContext.equips.buttonItems[4] >= ITEM_BOW_ARROW_FIRE &&
                               gSaveContext.equips.buttonItems[4] <= ITEM_BOW_ARROW_LIGHT),
                          &play->interfaceCtx.dpadRightAlpha);

        UpdateButtonAlpha(flashAlpha,
                          (gSaveContext.equips.buttonItems[5] == ITEM_BOW) ||
                              (gSaveContext.equips.buttonItems[5] >= ITEM_BOW_ARROW_FIRE &&
                               gSaveContext.equips.buttonItems[5] <= ITEM_BOW_ARROW_LIGHT),
                          &play->interfaceCtx.dpadLeftAlpha);

        UpdateButtonAlpha(flashAlpha,
                          (gSaveContext.equips.buttonItems[6] == ITEM_BOW) ||
                              (gSaveContext.equips.buttonItems[6] >= ITEM_BOW_ARROW_FIRE &&
                               gSaveContext.equips.buttonItems[6] <= ITEM_BOW_ARROW_LIGHT),
                          &play->interfaceCtx.dpadDownAlpha);

        UpdateButtonAlpha(flashAlpha,
                          (gSaveContext.equips.buttonItems[7] == ITEM_BOW) ||
                              (gSaveContext.equips.buttonItems[7] >= ITEM_BOW_ARROW_FIRE &&
                               gSaveContext.equips.buttonItems[7] <= ITEM_BOW_ARROW_LIGHT),
                          &play->interfaceCtx.dpadUpAlpha);
    }
}

static void UpdateEquippedBow(PlayState* play, s8 arrowType) {
    s32 bowItem = GetBowItemForArrow((PlayerItemAction)arrowType);
    bool dpadEnabled = CVarGetInteger(CVAR_ENHANCEMENT("DpadEquips"), 0);
    s32 maxButton = dpadEnabled ? 7 : 3;

    for (s32 i = 1; i <= maxButton; i++) {
        if ((gSaveContext.equips.buttonItems[i] == ITEM_BOW) ||
            (gSaveContext.equips.buttonItems[i] >= ITEM_BOW_ARROW_FIRE &&
             gSaveContext.equips.buttonItems[i] <= ITEM_BOW_ARROW_LIGHT)) {
            gSaveContext.equips.buttonItems[i] = bowItem;
            gSaveContext.equips.cButtonSlots[i - 1] = SLOT_BOW;

            if (i <= 3) {
                Interface_LoadItemIcon1(play, i);
            }

            gSaveContext.buttonStatus[i] = BTN_ENABLED;
            sButtonFlashTimer = BUTTON_FLASH_DURATION;
            sButtonFlashCount = 0;
        }
    }

    UpdateFlashEffect(play);
}

static void CycleToNextArrow(PlayState* play, Player* player) {
    s8 nextArrow = GetNextArrowType(player->heldItemAction);

    if (player->heldActor != NULL && player->heldActor->id == ACTOR_EN_ARROW) {
        EnArrow* arrow = (EnArrow*)player->heldActor;

        if (arrow->actor.child != NULL) {
            Actor_Kill(arrow->actor.child);
        }

        Actor_Kill(&arrow->actor);
    }

    Player_InitItemAction(play, player, (PlayerItemAction)nextArrow);
    UpdateEquippedBow(play, nextArrow);
    Audio_PlaySoundGeneral(NA_SE_PL_CHANGE_ARMS, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                           &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
    sJustCycledFrames = 2;
}

void ArrowCycleMain() {
    if (gPlayState == nullptr || !CanCycleArrows()) {
        return;
    }

    if (sJustCycledFrames > 0) {
        sJustCycledFrames--;
    }

    UpdateFlashEffect(gPlayState);

    Player* player = GET_PLAYER(gPlayState);
    Input* input = &gPlayState->state.input[0];

    if (IsAimingBow(player) && CHECK_BTN_ANY(input->press.button, BTN_R)) {
        if (IsHoldingMagicBow(player) && gSaveContext.magicState != MAGIC_STATE_IDLE && player->heldActor == NULL) {
            Audio_PlaySoundGeneral(NA_SE_SY_ERROR, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                   &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
            return;
        }

        // reset magic state to IDLE before cycling to prevent error sound
        gSaveContext.magicState = MAGIC_STATE_IDLE;

        CycleToNextArrow(gPlayState, player);
    }
}

void RegisterArrowCycle() {
    COND_ID_HOOK(OnActorUpdate, ACTOR_PLAYER, CVAR_ARROW_CYCLE_VALUE, [](void* actor) { ArrowCycleMain(); });

    // suppress shield input when R is held while aiming to allow arrow cycling
    COND_VB_SHOULD(VB_EXECUTE_PLAYER_ACTION_FUNC, CVAR_ARROW_CYCLE_VALUE, {
        Player* player = (Player*)va_arg(args, void*);
        Input* input = (Input*)va_arg(args, void*);
        if ((IsAimingBow(player) || sJustCycledFrames > 0) && CHECK_BTN_ANY(input->cur.button, BTN_R)) {
            input->cur.button &= ~BTN_R;
            input->press.button &= ~BTN_R;
        }
    });

    // don't consume magic on draw, but check if we have enough to fire
    COND_VB_SHOULD(VB_PLAYER_ARROW_MAGIC_CONSUMPTION, CVAR_ARROW_CYCLE_VALUE, {
        Player* player = va_arg(args, Player*);
        int32_t magicArrowType = va_arg(args, int32_t);
        int32_t* arrowType = va_arg(args, int32_t*);

        if (gSaveContext.magic < sMagicArrowCosts[magicArrowType]) {
            *arrowType = ARROW_NORMAL;
        }

        *should = false;
    });

    COND_VB_SHOULD(VB_EN_ARROW_MAGIC_CONSUMPTION, CVAR_ARROW_CYCLE_VALUE, {
        EnArrow* arrow = va_arg(args, EnArrow*);

        if (arrow->actor.params < ARROW_FIRE || arrow->actor.params > ARROW_LIGHT) {
            return;
        }

        int32_t magicArrowType = arrow->actor.params - ARROW_FIRE;
        Magic_RequestChange(gPlayState, sMagicArrowCosts[magicArrowType], MAGIC_CONSUME_NOW);
    });
}

static RegisterShipInitFunc initFunc(RegisterArrowCycle, { CVAR_ARROW_CYCLE_NAME });
