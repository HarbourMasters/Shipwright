#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "macros.h"
#include "variables.h"
#include "functions.h"
#include "z64horse.h"
#include "overlays/actors/ovl_En_Arrow/z_en_arrow.h"
void Player_InitItemAction(PlayState* play, Player* thisx, PlayerItemAction itemAction);
s32 Player_UpperAction_7(Player* thisx, PlayState* play);
s32 Player_UpperAction_8(Player* thisx, PlayState* play);
}

extern bool IsBombArrowButton(s32 slot, bool isDpad);
extern void SetBombArrowButton(s32 slot, bool state, bool isDpad);

#define CVAR_NAME "gEnhancements.PlayerActions.ArrowCycle"
#define CVAR CVarGetInteger(CVAR_NAME, 0)
#define BOMB_ARROW_CVAR_NAME "gEnhancements.Equipment.BombArrows"

// Magic arrow costs based on z_player.c
static const s16 sMagicArrowCosts[] = { 4, 4, 8 };

// Button Flash Effect Configuration
static const s16 BUTTON_FLASH_DURATION = 3;
static const s16 BUTTON_FLASH_COUNT = 3;
static const s16 BUTTON_HIGHLIGHT_ALPHA = 128;

// State Variables
static s16 sButtonFlashTimer = 0;
static s16 sButtonFlashCount = 0;
static s8 sJustCycledFrames = 0;

enum ArrowCycleType {
    ARROW_CYCLE_NORMAL,
    ARROW_CYCLE_FIRE,
    ARROW_CYCLE_ICE,
    ARROW_CYCLE_LIGHT,
    ARROW_CYCLE_BOMB,
    ARROW_CYCLE_MAX
};

static const PlayerItemAction sArrowCycleToPlayerIA[] = {
    PLAYER_IA_BOW,       // ARROW_CYCLE_NORMAL
    PLAYER_IA_BOW_FIRE,  // ARROW_CYCLE_FIRE
    PLAYER_IA_BOW_ICE,   // ARROW_CYCLE_ICE
    PLAYER_IA_BOW_LIGHT, // ARROW_CYCLE_LIGHT
    PLAYER_IA_BOW,       // ARROW_CYCLE_BOMB
};

// Utility Functions
static bool IsHoldingBow(Player* player) {
    return player->heldItemAction >= PLAYER_IA_BOW && player->heldItemAction <= PLAYER_IA_BOW_LIGHT;
}

static bool IsHoldingMagicBow(Player* player) {
    return player->heldItemAction >= PLAYER_IA_BOW_FIRE && player->heldItemAction <= PLAYER_IA_BOW_LIGHT;
}

static bool IsAimingBow(Player* player) {
    return IsHoldingBow(player) && ((player->unk_AA5 == PLAYER_UNKAA5_3) || /* Aiming box in first person */
                                    (player->upperActionFunc == Player_UpperAction_7) /* Arrow pulled back on bow */);
}

static bool IsBombArrowEnhancementEnabled() {
    return CVarGetInteger(BOMB_ARROW_CVAR_NAME, 0);
}

static bool HasBombArrows() {
    return IsBombArrowEnhancementEnabled() && (INV_CONTENT(ITEM_BOMB) == ITEM_BOMB);
}

static bool IsBombArrowButton(const Player* player) {
    if (player->heldItemButton < 0) {
        return false;
    }

    if (IS_HELD_DPAD(player->heldItemButton)) {
        s32 dpadSlot = HELD_ITEM_TO_DPAD(player->heldItemButton);
        return IsBombArrowButton(dpadSlot, true);
    }

    if (player->heldItemButton == EQUIP_SLOT_B) {
        return false;
    }

    return IsBombArrowButton(player->heldItemButton, false);
}

static bool HasArrowType(ArrowCycleType cycleType) {
    switch (cycleType) {
        case ARROW_CYCLE_NORMAL:
            return true;
        case ARROW_CYCLE_FIRE:
            return (INV_CONTENT(ITEM_ARROW_FIRE) == ITEM_ARROW_FIRE);
        case ARROW_CYCLE_ICE:
            return (INV_CONTENT(ITEM_ARROW_ICE) == ITEM_ARROW_ICE);
        case ARROW_CYCLE_LIGHT:
            return (INV_CONTENT(ITEM_ARROW_LIGHT) == ITEM_ARROW_LIGHT);
        case ARROW_CYCLE_BOMB:
            return HasBombArrows();
        default:
            return false;
    }
}

static s32 GetBowItemForCycleType(ArrowCycleType cycleType) {
    switch (cycleType) {
        case ARROW_CYCLE_FIRE:
            return ITEM_BOW_FIRE;
        case ARROW_CYCLE_ICE:
            return ITEM_BOW_ICE;
        case ARROW_CYCLE_LIGHT:
            return ITEM_BOW_LIGHT;
        default:
            return ITEM_BOW;
    }
}

static s32 GetSlotForCycleType(ArrowCycleType cycleType) {
    return cycleType == ARROW_CYCLE_BOMB ? SLOT_BOMB : SLOT_BOW;
}

static bool CanCycleArrows() {
    Player* player = GET_PLAYER(gPlayState);

    // Don't allow cycling during bow minigames in specific scenes
    if (gSaveContext.minigameStatus == MINIGAME_STATUS_ACTIVE &&
        (gPlayState->sceneId == SCENE_SYATEKI_MIZU || // Town Shooting Gallery
         gPlayState->sceneId == SCENE_SYATEKI_MORI || // Swamp Shooting Gallery
         gPlayState->sceneId == SCENE_20SICHITAI2)) { // Tourist Center boat cruise
        return false;
    }

    return !gHorseIsMounted && player->rideActor == NULL && INV_CONTENT(SLOT_BOW) == ITEM_BOW &&
           (INV_CONTENT(ITEM_ARROW_FIRE) == ITEM_ARROW_FIRE || INV_CONTENT(ITEM_ARROW_ICE) == ITEM_ARROW_ICE ||
            INV_CONTENT(ITEM_ARROW_LIGHT) == ITEM_ARROW_LIGHT || HasBombArrows());
}

// Arrow Cycling Logic
static ArrowCycleType GetCurrentCycleType(Player* player) {
    if (IsBombArrowButton(player)) {
        return ARROW_CYCLE_BOMB;
    }
    switch (player->heldItemAction) {
        case PLAYER_IA_BOW_FIRE:
            return ARROW_CYCLE_FIRE;
        case PLAYER_IA_BOW_ICE:
            return ARROW_CYCLE_ICE;
        case PLAYER_IA_BOW_LIGHT:
            return ARROW_CYCLE_LIGHT;
        default:
            return ARROW_CYCLE_NORMAL;
    }
}

static ArrowCycleType GetNextArrowCycleType(Player* player) {
    ArrowCycleType current = GetCurrentCycleType(player);

    for (int offset = 1; offset < ARROW_CYCLE_MAX; offset++) {
        ArrowCycleType next = static_cast<ArrowCycleType>((current + offset) % ARROW_CYCLE_MAX);
        if (HasArrowType(next)) {
            return next;
        }
    }

    return ARROW_CYCLE_NORMAL;
}

// UI Update Functions
static void UpdateButtonAlpha(s16 flashAlpha, bool isButtonBow, s16* buttonAlpha) {
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

    // Update C-buttons
    UpdateButtonAlpha(flashAlpha,
                      (GET_CUR_FORM_BTN_ITEM(EQUIP_SLOT_C_LEFT) == ITEM_BOW) ||
                          (GET_CUR_FORM_BTN_ITEM(EQUIP_SLOT_C_LEFT) >= ITEM_BOW_FIRE &&
                           GET_CUR_FORM_BTN_ITEM(EQUIP_SLOT_C_LEFT) <= ITEM_BOW_LIGHT),
                      &play->interfaceCtx.cLeftAlpha);

    UpdateButtonAlpha(flashAlpha,
                      (GET_CUR_FORM_BTN_ITEM(EQUIP_SLOT_C_DOWN) == ITEM_BOW) ||
                          (GET_CUR_FORM_BTN_ITEM(EQUIP_SLOT_C_DOWN) >= ITEM_BOW_FIRE &&
                           GET_CUR_FORM_BTN_ITEM(EQUIP_SLOT_C_DOWN) <= ITEM_BOW_LIGHT),
                      &play->interfaceCtx.cDownAlpha);

    UpdateButtonAlpha(flashAlpha,
                      (GET_CUR_FORM_BTN_ITEM(EQUIP_SLOT_C_RIGHT) == ITEM_BOW) ||
                          (GET_CUR_FORM_BTN_ITEM(EQUIP_SLOT_C_RIGHT) >= ITEM_BOW_FIRE &&
                           GET_CUR_FORM_BTN_ITEM(EQUIP_SLOT_C_RIGHT) <= ITEM_BOW_LIGHT),
                      &play->interfaceCtx.cRightAlpha);

    // Update D-pad
    UpdateButtonAlpha(flashAlpha,
                      (DPAD_GET_CUR_FORM_BTN_ITEM(EQUIP_SLOT_D_RIGHT) == ITEM_BOW) ||
                          (DPAD_GET_CUR_FORM_BTN_ITEM(EQUIP_SLOT_D_RIGHT) >= ITEM_BOW_FIRE &&
                           DPAD_GET_CUR_FORM_BTN_ITEM(EQUIP_SLOT_D_RIGHT) <= ITEM_BOW_LIGHT),
                      &play->interfaceCtx.shipInterface.dpad.dRightAlpha);

    UpdateButtonAlpha(flashAlpha,
                      (DPAD_GET_CUR_FORM_BTN_ITEM(EQUIP_SLOT_D_LEFT) == ITEM_BOW) ||
                          (DPAD_GET_CUR_FORM_BTN_ITEM(EQUIP_SLOT_D_LEFT) >= ITEM_BOW_FIRE &&
                           DPAD_GET_CUR_FORM_BTN_ITEM(EQUIP_SLOT_D_LEFT) <= ITEM_BOW_LIGHT),
                      &play->interfaceCtx.shipInterface.dpad.dLeftAlpha);

    UpdateButtonAlpha(flashAlpha,
                      (DPAD_GET_CUR_FORM_BTN_ITEM(EQUIP_SLOT_D_DOWN) == ITEM_BOW) ||
                          (DPAD_GET_CUR_FORM_BTN_ITEM(EQUIP_SLOT_D_DOWN) >= ITEM_BOW_FIRE &&
                           DPAD_GET_CUR_FORM_BTN_ITEM(EQUIP_SLOT_D_DOWN) <= ITEM_BOW_LIGHT),
                      &play->interfaceCtx.shipInterface.dpad.dDownAlpha);

    UpdateButtonAlpha(flashAlpha,
                      (DPAD_GET_CUR_FORM_BTN_ITEM(EQUIP_SLOT_D_UP) == ITEM_BOW) ||
                          (DPAD_GET_CUR_FORM_BTN_ITEM(EQUIP_SLOT_D_UP) >= ITEM_BOW_FIRE &&
                           DPAD_GET_CUR_FORM_BTN_ITEM(EQUIP_SLOT_D_UP) <= ITEM_BOW_LIGHT),
                      &play->interfaceCtx.shipInterface.dpad.dUpAlpha);
}

static void UpdateEquippedBow(PlayState* play, ArrowCycleType cycleType) {
    s32 bowItem = GetBowItemForCycleType(cycleType);
    s32 slot = GetSlotForCycleType(cycleType);

    // Update C-buttons
    for (s32 i = EQUIP_SLOT_C_LEFT; i <= EQUIP_SLOT_C_RIGHT; i++) {
        if ((BUTTON_ITEM_EQUIP(0, i) == ITEM_BOW) ||
            (BUTTON_ITEM_EQUIP(0, i) >= ITEM_BOW_FIRE && BUTTON_ITEM_EQUIP(0, i) <= ITEM_BOW_LIGHT)) {
            BUTTON_ITEM_EQUIP(0, i) = bowItem;
            C_SLOT_EQUIP(0, i) = slot;
            Interface_LoadItemIcon(play, i);
            gSaveContext.buttonStatus[i] = BTN_ENABLED;
            sButtonFlashTimer = BUTTON_FLASH_DURATION;
            sButtonFlashCount = 0;

            SetBombArrowButton(i, cycleType == ARROW_CYCLE_BOMB, false);
        }
    }

    // Update D-pad
    for (s32 i = EQUIP_SLOT_D_RIGHT; i <= EQUIP_SLOT_D_UP; i++) {
        if ((DPAD_BUTTON_ITEM_EQUIP(0, i) == ITEM_BOW) ||
            (DPAD_BUTTON_ITEM_EQUIP(0, i) >= ITEM_BOW_FIRE && DPAD_BUTTON_ITEM_EQUIP(0, i) <= ITEM_BOW_LIGHT)) {
            DPAD_BUTTON_ITEM_EQUIP(0, i) = bowItem;
            DPAD_SLOT_EQUIP(0, i) = slot;
            Interface_Dpad_LoadItemIcon(play, i);
            gSaveContext.shipSaveContext.dpad.status[i] = BTN_ENABLED;
            sButtonFlashTimer = BUTTON_FLASH_DURATION;
            sButtonFlashCount = 0;

            SetBombArrowButton(i, cycleType == ARROW_CYCLE_BOMB, true);
        }
    }

    UpdateFlashEffect(play);
}

// Core Arrow Cycling Function
static void CycleToNextArrow(PlayState* play, Player* player) {
    ArrowCycleType nextType = GetNextArrowCycleType(player);
    PlayerItemAction nextIA = sArrowCycleToPlayerIA[nextType];

    if (player->heldActor != NULL && player->heldActor->id == ACTOR_EN_ARROW) {
        EnArrow* arrow = (EnArrow*)player->heldActor;

        if (arrow->actor.child != NULL) {
            Actor_Kill(arrow->actor.child);
        }

        // Killing the held arrow while parent is still set does not consume bomb-arrow ammo
        Actor_Kill(&arrow->actor);
    }

    Player_InitItemAction(play, player, nextIA);
    UpdateEquippedBow(play, nextType);
    Audio_PlaySfx(NA_SE_PL_CHANGE_ARMS);
}

void ArrowCycleMain() {
    if (sJustCycledFrames > 0) {
        sJustCycledFrames--;
    }

    if (gPlayState == nullptr || !CanCycleArrows()) {
        return;
    }

    UpdateFlashEffect(gPlayState);

    Player* player = GET_PLAYER(gPlayState);
    Input* input = CONTROLLER1(&gPlayState->state);

    // Block camera changes when cycling arrows while drawing the bow
    if ((player->stateFlags3 & PLAYER_STATE3_40) && player->unk_ACE == 0) {
        return;
    }

    if (IsAimingBow(player) && CHECK_BTN_ANY(input->press.button, BTN_R) && player->heldActor != NULL &&
        player->heldActor->id == ACTOR_EN_ARROW) {
        if (IsHoldingMagicBow(player) && gSaveContext.magicState != MAGIC_STATE_IDLE && player->heldActor == NULL) {
            Audio_PlaySfx(NA_SE_SY_ERROR);
            return;
        }
        EnArrow* heldArrow = (EnArrow*)player->heldActor;

        // If the held arrow itself is magical, then we should "restore" the consumed magic upon cycling
        if (ARROW_IS_MAGICAL(heldArrow->actor.params)) {
            Magic_Add(gPlayState, sMagicArrowCosts[ARROW_GET_MAGIC_FROM_TYPE(heldArrow->actor.params)]);
        }

        CycleToNextArrow(gPlayState, player);
        // Track that we just cycled for 2 frames to prevent held R input from triggering the shield action when in
        // Z-Target mode as the arrow is respawned (Player_UpperAction_8)
        sJustCycledFrames = 2;
    }
}

// Registration and Hooks
void RegisterArrowCycle() {
    COND_VB_SHOULD(VB_SHIELD_FROM_BUTTON_HOLD, CVAR, {
        if (CanCycleArrows()) {
            Player* player = GET_PLAYER(gPlayState);
            Input* input = CONTROLLER1(&gPlayState->state);

            // Suppress Shield input when holding an arrow in Z-Target mode
            if (IsHoldingBow(player) && sJustCycledFrames > 0 && CHECK_BTN_ANY(input->cur.button, BTN_R)) {
                *should = false;
            }
        }
    });

    COND_VB_SHOULD(VB_EXIT_FIRST_PERSON_MODE_FROM_BUTTON, CVAR, {
        if (CanCycleArrows()) {
            Player* player = GET_PLAYER(gPlayState);
            Input* input = CONTROLLER1(&gPlayState->state);

            // Suppress Shield input first person cancel when aiming the bow
            if (IsAimingBow(player) && CHECK_BTN_ANY(input->cur.button, BTN_R) && player->heldActor != NULL &&
                player->heldActor->id == ACTOR_EN_ARROW) {
                *should = false;
            }
        }
    });

    COND_ID_HOOK(OnActorUpdate, ACTOR_PLAYER, CVAR, [](Actor* actor) { ArrowCycleMain(); });
}

static RegisterShipInitFunc initFunc(RegisterArrowCycle, { CVAR_NAME });
