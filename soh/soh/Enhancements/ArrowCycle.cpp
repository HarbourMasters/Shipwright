
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

// Arrow Cycling Logic - Fixed to use PlayerItemAction directly
static PlayerItemAction GetNextArrowType(s8 currentItemAction) {
    int currentIndex = 0;
    for (int i = 0; i < (int)ARRAY_COUNT(sArrowCycleOrder); i++) {
        if (sArrowCycleOrder[i] == (PlayerItemAction)currentItemAction) {
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

    return PLAYER_IA_BOW; // Fallback to normal arrows
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

static void UpdateEquippedBow(PlayState* play, PlayerItemAction itemAction) {
    s32 bowItem = GetBowItemForArrow(itemAction);

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

// Core Arrow Cycling Function - Fixed to use PlayerItemAction directly
static void CycleToNextArrow(PlayState* play, Player* player) {
    PlayerItemAction nextItemAction = GetNextArrowType(player->heldItemAction);

    // Store current aiming state
    bool wasAiming = IsAimingBow(player);
    UpperActionFunc currentUpperAction = player->upperActionFunc;
    s32 currentUnk834 = player->unk_834;
    s32 currentUnk836 = player->unk_836;
    s32 currentUnk860 = player->unk_860;

    // Kill the current arrow actor if it exists
    if (player->heldActor != NULL && player->heldActor->id == ACTOR_EN_ARROW) {
        EnArrow* arrow = (EnArrow*)player->heldActor;

        if (arrow->actor.child != NULL) {
            Actor_Kill(arrow->actor.child);
        }

        Actor_Kill(&arrow->actor);
        player->heldActor = NULL;
    }

    // Update the heldItemAction directly without calling Player_InitItemAction
    player->heldItemAction = nextItemAction;
    player->itemAction = nextItemAction;

    // Restore aiming state if we were aiming
    if (wasAiming) {
        player->upperActionFunc = currentUpperAction;
        player->unk_834 = currentUnk834;
        player->unk_836 = currentUnk836;
        player->unk_860 = currentUnk860;
        player->stateFlags1 |= PLAYER_STATE1_READY_TO_FIRE;
    }

    // Create new arrow actor if we were aiming
    if (wasAiming && player->unk_860 >= 0) {
        s32 arrowType;
        s32 magicArrowType;

        // Determine arrow type based on new heldItemAction using the correct formula
        if (play->shootingGalleryStatus != 0) {
            arrowType = play->shootingGalleryStatus;
        } else {
            // Convert PlayerItemAction to ArrowType for OoT
            switch (nextItemAction) {
                case PLAYER_IA_BOW:
                    arrowType = ARROW_NORMAL;
                    break;
                case PLAYER_IA_BOW_FIRE:
                    arrowType = ARROW_FIRE;
                    break;
                case PLAYER_IA_BOW_ICE:
                    arrowType = ARROW_ICE;
                    break;
                case PLAYER_IA_BOW_LIGHT:
                    arrowType = ARROW_LIGHT;
                    break;
                default:
                    arrowType = ARROW_NORMAL;
                    break;
            }
        }

        // Calculate magic arrow type for cost array (Fire=0, Ice=1, Light=2)
        magicArrowType = arrowType - ARROW_FIRE;

        if (arrowType >= ARROW_FIRE && arrowType <= ARROW_LIGHT) { // Fire, Ice, Light arrows
            if (!Magic_RequestChange(play, sMagicArrowCosts[magicArrowType], MAGIC_CONSUME_NOW)) {
                arrowType = ARROW_NORMAL; // Fall back to normal arrows
            }
        }

        // Create the arrow actor with the correct type
        Actor* newArrow = Actor_SpawnAsChild(&play->actorCtx, &player->actor, play, ACTOR_EN_ARROW,
                                             player->actor.world.pos.x, player->actor.world.pos.y,
                                             player->actor.world.pos.z, 0, player->actor.shape.rot.y, 0, arrowType);

        if (newArrow != NULL) {
            player->heldActor = newArrow;

            // Force the arrow to spawn its child actor immediately for visual effects
            EnArrow* arrow = (EnArrow*)newArrow;

            // Spawn the appropriate child actor based on arrow type for immediate visual effects
            if (arrowType >= ARROW_FIRE && arrowType <= ARROW_LIGHT) {
                s16 elementalActorIds[] = { ACTOR_ARROW_FIRE, ACTOR_ARROW_ICE, ACTOR_ARROW_LIGHT };
                s16 childActorId = elementalActorIds[arrowType - ARROW_FIRE];

                Actor_SpawnAsChild(&play->actorCtx, &arrow->actor, play, childActorId, arrow->actor.world.pos.x,
                                   arrow->actor.world.pos.y, arrow->actor.world.pos.z, 0, 0, 0, 0);
            }
        }
    }

    UpdateEquippedBow(play, nextItemAction);
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
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnActorUpdate>([](void* actor) {
        if (((Actor*)actor)->id == ACTOR_PLAYER) {
            ArrowCycleMain();
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterArrowCycle, { CVAR_NAME });