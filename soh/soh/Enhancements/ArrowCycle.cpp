
#include "public/bridge/consolevariablebridge.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "macros.h"
#include "variables.h"
#include "functions.h"
#include "overlays/actors/ovl_En_Arrow/z_en_arrow.h"
s32 func_808351D4(Player* thisx, PlayState* play); // Arrow nocked
s32 func_808353D8(Player* thisx, PlayState* play); // Aiming in first person
void Player_InitItemAction(PlayState* play, Player* thisx, PlayerItemAction itemAction);
}

#define CVAR_NAME "gEnhancements.BowArrowCycle"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

// Magic arrow costs based on z_player.c
static const s16 sMagicArrowCosts[] = { 4, 4, 8 };

// Minigame status constants
#define MINIGAME_STATUS_ACTIVE 1

// Button Flash Effect Configuration
static const s16 BUTTON_FLASH_DURATION = 3;
static const s16 BUTTON_FLASH_COUNT = 3;
static const s16 BUTTON_HIGHLIGHT_ALPHA = 128;

// State Variables
static s16 sButtonFlashTimer = 0;
static s16 sButtonFlashCount = 0;
static s8 sJustCycledFrames = 0;

// Arrow cycling order - using PlayerItemAction values
static const PlayerItemAction sArrowCycleOrder[] = {
    PLAYER_IA_BOW,       // Normal
    PLAYER_IA_BOW_FIRE,  // Fire
    PLAYER_IA_BOW_ICE,   // Ice
    PLAYER_IA_BOW_LIGHT, // Light
};

// Utility Functions
static bool IsHoldingBow(Player* player) {
    return player->heldItemAction >= PLAYER_IA_BOW && player->heldItemAction <= PLAYER_IA_BOW_LIGHT;
}

static bool IsHoldingMagicBow(Player* player) {
    return player->heldItemAction >= PLAYER_IA_BOW_FIRE && player->heldItemAction <= PLAYER_IA_BOW_LIGHT;
}

static bool IsAimingBow(Player* player) {
    return IsHoldingBow(player) && ((player->unk_6AD == 2) || /* Aiming box in first person */
                                    (player->upperActionFunc == func_808351D4) /* Arrow pulled back on bow */);
}

static bool HasArrowType(PlayerItemAction itemAction) {
    switch (itemAction) {
        case PLAYER_IA_BOW:
            return true; // Normal arrows are always available
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

    // Don't allow cycling during bow minigames in specific scenes
    if (gSaveContext.minigameState == MINIGAME_STATUS_ACTIVE &&
        (gPlayState->sceneNum == SCENE_SHOOTING_GALLERY ||       // Shooting Gallery
         gPlayState->sceneNum == SCENE_BOMBCHU_BOWLING_ALLEY)) { // Bombchu Bowling Alley
        return false;
    }

    return !(player->stateFlags1 & PLAYER_STATE1_ON_HORSE) && player->rideActor == NULL &&
           INV_CONTENT(SLOT_BOW) == ITEM_BOW &&
           (INV_CONTENT(ITEM_ARROW_FIRE) == ITEM_ARROW_FIRE || INV_CONTENT(ITEM_ARROW_ICE) == ITEM_ARROW_ICE ||
            INV_CONTENT(ITEM_ARROW_LIGHT) == ITEM_ARROW_LIGHT);
}

// Arrow Cycling Logic
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

// UI Update Functions
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

    // Update C-buttons
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

    // Update D-pad (if DpadEquips is enabled)
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
    s32 bowItem = GetBowItemForArrow(static_cast<PlayerItemAction>(arrowType));

    // Update C-buttons
    for (s32 i = 1; i <= 3; i++) {
        if ((gSaveContext.equips.buttonItems[i] == ITEM_BOW) ||
            (gSaveContext.equips.buttonItems[i] >= ITEM_BOW_ARROW_FIRE &&
             gSaveContext.equips.buttonItems[i] <= ITEM_BOW_ARROW_LIGHT)) {
            gSaveContext.equips.buttonItems[i] = bowItem;
            gSaveContext.equips.cButtonSlots[i - 1] = SLOT_BOW;
            Interface_LoadItemIcon1(play, i);
            gSaveContext.buttonStatus[i] = BTN_ENABLED;
            sButtonFlashTimer = BUTTON_FLASH_DURATION;
            sButtonFlashCount = 0;
        }
    }

    // Update D-pad (if DpadEquips is enabled)
    if (CVarGetInteger(CVAR_ENHANCEMENT("DpadEquips"), 0)) {
        for (s32 i = 4; i <= 7; i++) {
            if ((gSaveContext.equips.buttonItems[i] == ITEM_BOW) ||
                (gSaveContext.equips.buttonItems[i] >= ITEM_BOW_ARROW_FIRE &&
                 gSaveContext.equips.buttonItems[i] <= ITEM_BOW_ARROW_LIGHT)) {
                gSaveContext.equips.buttonItems[i] = bowItem;
                gSaveContext.equips.cButtonSlots[i - 1] = SLOT_BOW;
                // D-pad items are handled differently, no need to reload icon
                gSaveContext.buttonStatus[i] = BTN_ENABLED;
                sButtonFlashTimer = BUTTON_FLASH_DURATION;
                sButtonFlashCount = 0;
            }
        }
    }

    UpdateFlashEffect(play);
}

// Core Arrow Cycling Function
static void CycleToNextArrow(PlayState* play, Player* player) {
    s8 nextArrow = GetNextArrowType(player->heldItemAction);

    if (player->heldActor != NULL && player->heldActor->id == ACTOR_EN_ARROW) {
        EnArrow* arrow = (EnArrow*)player->heldActor;

        if (arrow->actor.child != NULL) {
            Actor_Kill(arrow->actor.child);
        }

        Actor_Kill(&arrow->actor);
    }

    Player_InitItemAction(play, player, static_cast<PlayerItemAction>(nextArrow));
    UpdateEquippedBow(play, nextArrow);
    Audio_PlaySoundGeneral(NA_SE_PL_CHANGE_ARMS, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                           &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
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
    Input* input = &gPlayState->state.input[0];

    // Block camera changes when cycling arrows while drawing the bow
    if ((player->stateFlags3 & PLAYER_STATE3_MIDAIR) && player->unk_834 == 0) {
        return;
    }

    if (IsAimingBow(player) && CHECK_BTN_ANY(input->press.button, BTN_R)) {
        if (IsHoldingMagicBow(player) && gSaveContext.magicState != MAGIC_STATE_IDLE && player->heldActor == NULL) {
            Audio_PlaySoundGeneral(NA_SE_SY_ERROR, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                   &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
            return;
        }

        if (player->heldActor != NULL && player->heldActor->id == ACTOR_EN_ARROW) {
            EnArrow* heldArrow = (EnArrow*)player->heldActor;

            // If the held arrow itself is magical, then we should "restore" the consumed magic upon cycling
            if (heldArrow->actor.params >= ARROW_FIRE && heldArrow->actor.params <= ARROW_LIGHT) {
                s32 magicArrowType = heldArrow->actor.params - ARROW_FIRE;
                Magic_RequestChange(gPlayState, sMagicArrowCosts[magicArrowType], MAGIC_ADD);
            }
        }

        // Ensure magic state is reset to IDLE before cycling to prevent the error sound
        if (gSaveContext.magicState != MAGIC_STATE_IDLE) {
            gSaveContext.magicState = MAGIC_STATE_IDLE;
        }

        CycleToNextArrow(gPlayState, player);
        // Track that we just cycled for 2 frames to prevent held R input from triggering the shield action when in
        // Z-Target mode as the arrow is respawned (func_808353D8)
        sJustCycledFrames = 2;
    }
}

// Registration and Hooks
void RegisterArrowCycle() {
    COND_ID_HOOK(OnActorUpdate, ACTOR_PLAYER, CVAR, [](void* actor) { ArrowCycleMain(); });

    // Suppress shield input when aiming the bow and R is pressed
    COND_VB_SHOULD(VB_EXECUTE_PLAYER_ACTION_FUNC, CVAR, {
        Player* player = (Player*)va_arg(args, void*);
        Input* input = (Input*)va_arg(args, void*);
        if (IsAimingBow(player) && CHECK_BTN_ANY(input->press.button, BTN_R)) {
            // In first person mode: always block shield input to allow arrow cycling
            // In Z-target mode: only block during brief window after cycling to prevent shield action when arrow
            // respawns
            if ((player->stateFlags1 & PLAYER_STATE1_FIRST_PERSON) ||
                (sJustCycledFrames > 0 && (player->stateFlags1 & PLAYER_STATE1_Z_TARGETING))) {
                *should = false;
            }
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterArrowCycle, { CVAR_NAME });