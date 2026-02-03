/**
 * custom_items_common.c - Shared state and utilities for custom items
 *
 * Contains:
 *   - Global CustomItemState struct instance
 *   - Common utility functions used by multiple items
 *   - Frame update handlers for active items
 */

#include "custom_items.h"
#include "z64.h"
#include "macros.h"
#include "functions.h"
#include "variables.h"
#include <math.h>
#include "helpers/fx_helper.h"
#include "helpers/camera_helper.h"

// Global custom items state
CustomItemState gCustomItemState = {
    .timer1 = 0,
    .timer2 = 0,
    .globalCooldownTimer = 0,
    .rocsFeatherJumpActive = 0,
    .rocsJumpCount = 0,
    .dekuLeafActive = 0,
    .dekuLeafMode = 0,
    .dekuLeafGliding = 0,
    .dekuLeafBlowing = 0,
    .dekuLeafAnimTimer = 0,
    .dekuLeafBlowTimer = 0,
    .ballAndChainThrown = 0,
    .ballAndChainFirstPersonActive = 0,
    .spinnerActive = 0,
    .spinnerSpinAttackTimer = 0,
    .spinnerWallBumpTimer = 0,
    .gustJarState = 0,
    .gustJarTarget = NULL,
    .gustJarMode = 0,
    .gustJarAmmoType = 0,
    .gustJarProjectileActive = 0,
    .gustJarProjPos = { 0, 0, 0 },
    .gustJarProjYaw = 0,
    .gustJarProjPitch = 0,
    .gustJarTimer = 0,
    .gustJarFirstPersonActive = 0,
    .gustJarAimMode = 0,
    .gustJarPrevCameraMode = 0,
    .gustJarEquipped = 0,
    .gustJarButtonMask = 0,
    .gustJarPrevInvincibility = 0,
    .gustJarPotActor = NULL,
    .gustJarScaleCacheCount = 0,
    .shovelActive = 0,
    .shovelAnimating = 0,
    .shovelAnimTimer = 0,
    .shovelHoleActor = NULL,
    .demiseDestructionActive = 0,
    .beetleActive = 0,
    .beetleState = 0,
    .beetleFirstPersonActive = 0,
    .beetlePos = { 0, 0, 0 },
    .beetleRot = { 0, 0, 0 },
    .beetleGrabbed = NULL,
    .beetleWingScale = 1.0f,
    .beetleWingDir = -1,
    .beetleTimer = 0,
    .beetleStartPos = { 0, 0, 0 },
    .beetleSubCamId = SUBCAM_FREE,
    .bombArrowActive = 0,
    .bombArrowState = 0,
    .bombArrowBombActor = NULL,
    .bombArrowArrowActor = NULL,
    .bombArrowFirstPersonActive = 0,
    .bombArrowButtonMask = 0,
    .fireRodActive = 0,
    .fireRodState = 0,
    .fireRodPrevSword = 0,
    .fireRodMatrixValid = 0,
    .fireRodProjActive = 0,
    .fireRodProjCount = 0,
    .fireRodFlameActive = 0,
    .fireRodFlameTimer = 0,
    .fireRodFirstPerson = 0,
    .fireRodButtonMask = 0,
    // Ice Rod
    .iceRodActive = 0,
    .iceRodState = 0,
    .iceRodPrevSword = 0,
    .iceRodMatrixValid = 0,
    .iceRodProjActive = 0,
    .iceRodProjCount = 0,
    .iceRodWaveActive = 0,
    .iceRodWaveTimer = 0,
    .iceRodFirstPerson = 0,
    .iceRodButtonMask = 0,
    // Light Rod
    .lightRodActive = 0,
    .lightRodState = 0,
    .lightRodPrevSword = 0,
    .lightRodMatrixValid = 0,
    .lightRodProjActive = 0,
    .lightRodProjCount = 0,
    .lightRodBeamActive = 0,
    .lightRodBeamTimer = 0,
    .lightRodFirstPerson = 0,
    .lightRodButtonMask = 0,
    // Dominion Rod
    .dominionRodActive = 0,
    .dominionRodState = 0,
    .dominionRodFirstPersonActive = 0,
    .dominionRodOrbPos = { 0, 0, 0 },
    .dominionRodOrbRot = { 0, 0, 0 },
    .dominionRodControlledActor = NULL,
    .dominionRodTimer = 0,
    .dominionRodStartPos = { 0, 0, 0 },
    .dominionRodLightNode = NULL,
    .dominionRodButtonMask = 0,
    .dominionRodControlType = 0,
    .dominionRodControlVel = { 0, 0, 0 },
    .dominionRodDamagePaused = 0,
    .dominionRodPrevInvincibility = 0,
    // Dominion Rod Actor-Specific
    .dominionRodActorHomePos = { 0, 0, 0 },
    .dominionRodFlameTimer = 0,
    .dominionRodAttackCooldown = 0,
    .dominionRodSpikeInvulnerable = 0,
    .dominionRodFlameActor = NULL,
    // Cane of Somaria
    .somariaActive = 0,
    .somariaBlocks = { NULL, NULL, NULL },
    .somariaBlockCount = 0,
    .somariaOldestSlot = 0,
    .somariaButtonMask = 0,
    .somariaCooldown = 0,
    .somariaSelectedType = 0,
    .somariaAnimating = 0,
    .somariaAnimTimer = 0,
    .somariaActionType = 0,
    // Hylia's Grace
    .hyliasGraceActive = 0,
    .hyliasGraceState = 0,
    .hyliasGraceSubPhase = 0,
    .hyliasGraceTimer = 0,
    .hyliasGraceCooldown = 0,
    .hyliasGraceFairy = NULL,
    // Zonai Permafrost
    .zonaiPermafrostActive = 0,
    .zonaiPermafrostState = 0,
    .zonaiPermafrostSubPhase = 0,
    .zonaiPermafrostTimer = 0,
    .zonaiPermafrostSavedTimeIncr = 0,
    // Time Gate
    .timeGateActive = 0,
    .timeGateState = 0,
    .timeGateSubPhase = 0,
    .timeGateTimer = 0,
    .timeGatePromptShown = 0,
    // Mogma Mitts
    .mogmaMittsActive = 0,
    .mogmaMittsDrainTick = 0,
    // Whip
    .whipActive = 0,
    .whipState = 0,
    .whipTipPos = { 0, 0, 0 },
    .whipAttachPos = { 0, 0, 0 },
    .whipAttachNormal = { 0, 0, 0 },
    .whipTimer = 0,
    .whipSwingAngle = 0.0f,
    .whipSwingVel = 0.0f,
    .whipSwingYaw = 0,
    .whipRopeLength = 0.0f,
    .whipAttachedBgId = 0,
    .whipPullTarget = NULL,
    .whipRageTarget = NULL,
    .whipRageTimer = 0,
    .whipRageOrigSpeed = 0.0f,
    .whipPrevInvinc = 0,
    .whipExtendYaw = 0,
    .whipExtendPitch = 0,
    .whipFirstPersonActive = 0,
    // Desire Sensor
    .desireSensorActive = 0,
    .desireSensorState = 0,
    .desireSensorTimer = 0,
    .desireSensorResult = 0,
    .sharedProjectilePos = { 0, 0, 0 }
};

s32 CustomItems_IsBlocked(Player* p, PlayState* play) {
    if (p == NULL || play == NULL) return true;
    if (p->stateFlags1 & CUSTOM_BLOCKING_STATE1_FLAGS) return true;
    if (p->csAction != 0) return true;
    if (play->transitionTrigger == TRANS_TRIGGER_START) return true;
    if (p->stateFlags3 & PLAYER_STATE3_FLYING_WITH_HOOKSHOT) return true;
    return false;
}

// Quick check if item is in any C-button slot
static u8 IsItemEquipped(u8 itemId) {
    for (u8 i = 1; i <= 8; i++) {
        if (gSaveContext.equips.buttonItems[i] == itemId) return 1;
    }
    return 0;
}

// Cleanup items that were unequipped while active - only runs if needed
static void CustomItems_CleanupUnequipped(Player* p, PlayState* play) {
    if (gCustomItemState.spinnerActive && !IsItemEquipped(ITEM_SPINNER))
        Handle_Spinner(p, play);
    if (gCustomItemState.gustJarEquipped && !IsItemEquipped(ITEM_GUST_JAR))
        Handle_GustJar(p, play);
    if (gCustomItemState.ballAndChainThrown && !IsItemEquipped(ITEM_BALL_AND_CHAIN))
        Handle_BallAndChain(p, play);
    if (gCustomItemState.shovelActive && !IsItemEquipped(ITEM_SHOVEL))
        Handle_Shovel(p, play);
    if (gCustomItemState.demiseDestructionActive && !IsItemEquipped(ITEM_DEMISE_DESTRUCTION))
        Handle_DemiseDestruction(p, play);
    if (gCustomItemState.dekuLeafGliding && !IsItemEquipped(ITEM_DEKU_LEAF))
        Handle_DekuLeaf(p, play);
    if (gCustomItemState.beetleActive && !IsItemEquipped(ITEM_BEETLE))
        Handle_Beetle(p, play);
    if (gCustomItemState.bombArrowActive && !IsItemEquipped(ITEM_BOMB_ARROWS))
        Handle_BombArrows(p, play);
    if (gCustomItemState.fireRodFirstPerson && !IsItemEquipped(ITEM_ROD_FIRE))
        Handle_FireRod(p, play);
    if (gCustomItemState.iceRodFirstPerson && !IsItemEquipped(ITEM_ROD_ICE))
        Handle_IceRod(p, play);
    if (gCustomItemState.lightRodFirstPerson && !IsItemEquipped(ITEM_ROD_LIGHT))
        Handle_LightRod(p, play);
    if (gCustomItemState.dominionRodActive && !IsItemEquipped(ITEM_DOMINION_ROD))
        Handle_DominionRod(p, play);
    if (gCustomItemState.somariaActive && !IsItemEquipped(ITEM_CANE_OF_SOMARIA))
        Handle_CaneOfSomaria(p, play);
    if (gCustomItemState.hyliasGraceActive && !IsItemEquipped(ITEM_HYLIAS_GRACE))
        Handle_HyliasGrace(p, play);
    if (gCustomItemState.zonaiPermafrostActive && !IsItemEquipped(ITEM_ZONAI_PERMAFROST))
        Handle_ZonaiPermafrost(p, play);
    if (gCustomItemState.timeGateActive && !IsItemEquipped(ITEM_TIME_GATE))
        Handle_TimeGate(p, play);
    if (gCustomItemState.mogmaMittsActive && !IsItemEquipped(ITEM_MOGMA_MITTS))
        Handle_MogmaMitts(p, play);
    if (gCustomItemState.whipActive && !IsItemEquipped(ITEM_WHIP))
        Handle_Whip(p, play);
    if (gCustomItemState.desireSensorActive && !IsItemEquipped(ITEM_DESIRE_SENSOR))
        Handle_DesireSensor(p, play);
}

void CustomItems_Update(Player* p, PlayState* play) {
    if (gCustomItemState.demiseDestructionActive) {
        Handle_DemiseDestruction(p, play);
        return;
    }

    // Hylia's Grace fairy mode blocks all other custom items
    if (gCustomItemState.hyliasGraceActive) {
        Handle_HyliasGrace(p, play);
        return;
    }

    // Desire Sensor blocks all other custom items during sensing/result
    if (gCustomItemState.desireSensorActive) {
        Handle_DesireSensor(p, play);
        return;
    }

    // Beetle flying blocks all other custom items
    if (gCustomItemState.beetleActive &&
        (gCustomItemState.beetleState == 2 || gCustomItemState.beetleState == 3)) {
        Handle_Beetle(p, play);
        return;
    }

    // Time Gate blocks all other custom items during casting/hovering
    if (gCustomItemState.timeGateActive) {
        Handle_TimeGate(p, play);
        return;
    }

    // Zonai Permafrost must run before IsBlocked check (CASTING sets IN_ITEM_CS)
    // During CASTING: block other items. During ACTIVE: let other items also update.
    if (gCustomItemState.zonaiPermafrostActive) {
        Handle_ZonaiPermafrost(p, play);
        if (gCustomItemState.zonaiPermafrostState != 2 /* ZPERM_STATE_ACTIVE */) {
            return;
        }
    }

    if (CustomItems_IsBlocked(p, play)) return;

    if (gCustomItemState.globalCooldownTimer > 0) gCustomItemState.globalCooldownTimer--;
    if (gCustomItemState.hyliasGraceCooldown > 0) gCustomItemState.hyliasGraceCooldown--;

    CustomItems_CleanupUnequipped(p, play);

    for (u8 i = 1; i <= 8; i++) {
        u8 item = gSaveContext.equips.buttonItems[i];
        if (item < ITEM_ROCS_FEATHER || item > ITEM_PENDING_3) continue;

        switch (item) {
            case ITEM_ROCS_FEATHER:      Handle_RocsFeather(p, play); break;
            case ITEM_ROCS_CAPE:         Handle_RocsCape(p, play); break;
            case ITEM_DEKU_LEAF:         Handle_DekuLeaf(p, play); break;
            case ITEM_SPINNER:           Handle_Spinner(p, play); break;
            case ITEM_GUST_JAR:          Handle_GustJar(p, play); break;
            case ITEM_BALL_AND_CHAIN:    Handle_BallAndChain(p, play); break;
            case ITEM_SHOVEL:            Handle_Shovel(p, play); break;
            case ITEM_DEMISE_DESTRUCTION: Handle_DemiseDestruction(p, play); break;
            case ITEM_BEETLE:            Handle_Beetle(p, play); break;
            case ITEM_BOMB_ARROWS:       Handle_BombArrows(p, play); break;
            case ITEM_ROD_FIRE:          Handle_FireRod(p, play); break;
            case ITEM_ROD_ICE:           Handle_IceRod(p, play); break;
            case ITEM_ROD_LIGHT:         Handle_LightRod(p, play); break;
            case ITEM_DOMINION_ROD:      Handle_DominionRod(p, play); break;
            case ITEM_CANE_OF_SOMARIA:   Handle_CaneOfSomaria(p, play); break;
            case ITEM_MOGMA_MITTS:       Handle_MogmaMitts(p, play); break;
            case ITEM_HYLIAS_GRACE:      Handle_HyliasGrace(p, play); break;
            case ITEM_ZONAI_PERMAFROST:
                // Only call from switch when idle; early-run handles active states
                if (!gCustomItemState.zonaiPermafrostActive)
                    Handle_ZonaiPermafrost(p, play);
                break;
            case ITEM_TIME_GATE:         Handle_TimeGate(p, play); break;
            case ITEM_WHIP:              Handle_Whip(p, play); break;
            case ITEM_DESIRE_SENSOR:     Handle_DesireSensor(p, play); break;
            default: break;
        }
    }
}

s32 CustomItems_OverrideDraw(Player* p, PlayState* play) {
    CustomItems_DrawDekuLeaf(p, play);
    CustomItems_DrawSpinner(p, play);
    CustomItems_DrawFireRod(p, play);  // Call unconditionally like spinner
    CustomItems_DrawIceRod(p, play);   // Ice Rod draw
    CustomItems_DrawLightRod(p, play); // Light Rod draw

    if (gCustomItemState.gustJarMode > 0 || p->heldItemAction == ITEM_GUST_JAR) {
        CustomItems_DrawGustJar(p, play);
    }
    if (gCustomItemState.ballAndChainThrown) {
        CustomItems_DrawBallChain(p, play);
    }
    if (gCustomItemState.shovelActive || gCustomItemState.shovelAnimating) {
        CustomItems_DrawShovel(p, play);
    }
    if (gCustomItemState.beetleActive) {
        CustomItems_DrawBeetle(p, play);
    }
    if (gCustomItemState.dominionRodActive) {
        CustomItems_DrawDominionRod(p, play);
    }
    if (gCustomItemState.somariaActive) {
        CustomItems_DrawCaneOfSomaria(p, play);
    }
    if (gCustomItemState.mogmaMittsActive) {
        CustomItems_DrawMogmaMitts(p, play);
    }
    if (gCustomItemState.whipActive) {
        CustomItems_DrawWhip(p, play);
    }

    // Draw reticle for items using first-person aiming mode
    // Bomb Arrows - red reticle (255, 0, 0)
    if (gCustomItemState.bombArrowActive && gCustomItemState.bombArrowFirstPersonActive) {
        CustomItems_DrawBombArrowsReticle(p, play);
    }
    // Gust Jar - cyan reticle (0, 255, 255)
    if (gCustomItemState.gustJarFirstPersonActive) {
        FirstPerson_DrawReticle(p, play, 0.0f, 0, 255, 255);
    }
    // Ball and Chain - orange reticle (255, 150, 0)
    if (gCustomItemState.ballAndChainFirstPersonActive) {
        FirstPerson_DrawReticle(p, play, 0.0f, 255, 150, 0);
    }
    // Beetle - green reticle (0, 255, 0) - only during aiming state
    if (gCustomItemState.beetleFirstPersonActive && gCustomItemState.beetleState == 1) {
        FirstPerson_DrawReticle(p, play, 0.0f, 0, 255, 0);
    }
    // Fire Rod - orange-red reticle (255, 100, 0)
    if (gCustomItemState.fireRodFirstPerson) {
        FirstPerson_DrawReticle(p, play, 0.0f, 255, 100, 0);
    }
    // Ice Rod - cyan reticle (0, 200, 255)
    if (gCustomItemState.iceRodFirstPerson) {
        FirstPerson_DrawReticle(p, play, 0.0f, 0, 200, 255);
    }
    // Light Rod - golden-yellow reticle (255, 255, 0)
    if (gCustomItemState.lightRodFirstPerson) {
        FirstPerson_DrawReticle(p, play, 0.0f, 255, 255, 0);
    }
    // Whip - red reticle (255, 0, 0)
    if (gCustomItemState.whipFirstPersonActive) {
        FirstPerson_DrawReticle(p, play, 0.0f, 255, 0, 0);
    }
    // Dominion Rod - red reticle (255, 50, 50)
    if (gCustomItemState.dominionRodFirstPersonActive) {
        CustomItems_DrawDominionRodReticle(p, play);
    }

    return 0;
}
