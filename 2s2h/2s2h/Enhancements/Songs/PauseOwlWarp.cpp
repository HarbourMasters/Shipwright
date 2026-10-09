#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"
#include "2s2h/ShipUtils.h"
#include "2s2h/Rando/Logic/Logic.h"

extern "C" {
#include "overlays/kaleido_scope/ovl_kaleido_scope/z_kaleido_scope.h"
#include "z64horse.h"

extern f32 sPauseMenuVerticalOffset;
extern u16 sCursorPointsToOcarinaModes[];
extern u16 sOwlWarpPauseItems[];
extern s16 sInDungeonScene;
extern s32 gHorseIsMounted;

extern bool BetterOwlWarp_IsCutoffOnSide(s16 cursorPoint, PauseContext* pauseCtx);
extern bool BetterOwlWarp_NextCursorPoint(s16* cursorPoint, PauseContext* pauseCtx);
}

#define CVAR_NAME "gEnhancements.Songs.PauseOwlWarp"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

extern "C" bool PauseOwlWarp_IsOwlWarpEnabled() {
    return CVAR && CHECK_QUEST_ITEM(QUEST_SONG_SOARING) && INV_CONTENT(ITEM_OCARINA_OF_TIME) == ITEM_OCARINA_OF_TIME &&
           gSaveContext.save.saveInfo.playerData.owlActivationFlags != 0 &&
           gPlayState->pauseCtx.debugEditor == DEBUG_EDITOR_NONE &&
           gPlayState->interfaceCtx.restrictions.songOfSoaring == 0 &&
           (!IS_RANDO || Rando::Logic::canPlaySong(OCARINA_SONG_SOARING));
}

static void HandleConfirmingState(PauseContext* pauseCtx, Input* input) {
    if (Message_ShouldAdvance(gPlayState)) {
        if (gPlayState->msgCtx.choiceIndex == 0) { // Yes
            Player* player = GET_PLAYER(gPlayState);

            // Handle Epona's state before warping to prevent her from following the player
            if (gHorseIsMounted && (player->stateFlags1 & PLAYER_STATE1_800000) && player->rideActor != NULL) {
                // Save Epona's current position to horse data so she can be found later in the current scene
                if (CHECK_QUEST_ITEM(QUEST_SONG_EPONA)) {
                    gSaveContext.save.saveInfo.horseData.sceneId = gPlayState->sceneId;
                    gSaveContext.save.saveInfo.horseData.pos.x = player->rideActor->world.pos.x;
                    gSaveContext.save.saveInfo.horseData.pos.y = player->rideActor->world.pos.y;
                    gSaveContext.save.saveInfo.horseData.pos.z = player->rideActor->world.pos.z;
                    gSaveContext.save.saveInfo.horseData.yaw = player->rideActor->shape.rot.y;
                }

                // Clear horse mounting state to prevent Epona from spawning at warp destination
                gHorseIsMounted = false;
            }

            // Clear pictograph/camera event flag to prevent UI state from persisting after warp
            CLEAR_EVENTINF(EVENTINF_41);

            Interface_SetAButtonDoAction(gPlayState, DO_ACTION_NONE);
            pauseCtx->state = PAUSE_STATE_UNPAUSE_SETUP;
            sPauseMenuVerticalOffset = -6240.0f;
            Audio_PlaySfx_PauseMenuOpenOrClose(SFX_PAUSE_MENU_CLOSE);
            gPlayState->msgCtx.ocarinaMode = sCursorPointsToOcarinaModes[pauseCtx->cursorPoint[PAUSE_WORLD_MAP]];
            Audio_PlaySfx(NA_SE_SY_DECIDE);

            Message_CloseTextbox(gPlayState);

            gPlayState->nextEntrance = sOwlWarpEntrancesForMods[pauseCtx->cursorPoint[PAUSE_WORLD_MAP]];
            gPlayState->transitionTrigger = TRANS_TRIGGER_START;
            gPlayState->transitionType = TRANS_TYPE_FADE_WHITE;
        } else { // No
            Interface_SetAButtonDoAction(gPlayState, DO_ACTION_WARP);
            Message_CloseTextbox(gPlayState);
            Audio_PlaySfx(NA_SE_SY_MESSAGE_PASS);
            pauseCtx->itemDescriptionOn = false;
        }
    } else if (CHECK_BTN_ALL(input->press.button, BTN_START) || CHECK_BTN_ALL(input->press.button, BTN_B)) {
        Interface_SetAButtonDoAction(gPlayState, DO_ACTION_WARP);
        Message_CloseTextbox(gPlayState);
        Audio_PlaySfx(NA_SE_SY_MESSAGE_PASS);
        pauseCtx->itemDescriptionOn = false;
    }
}

// This is a variation of KaleidoScope_UpdateWorldMapCursor that deals with the warp points instead of region points
// and supports mirror mode
static void UpdateCursorForOwlWarpPoints(PauseContext* pauseCtx) {
    if ((pauseCtx->state == PAUSE_STATE_MAIN) && (pauseCtx->mainState == PAUSE_MAIN_STATE_IDLE) &&
        (pauseCtx->pageIndex == PAUSE_MAP)) {
        InterfaceContext* interfaceCtx = &gPlayState->interfaceCtx;
        s16 oldCursorPoint = pauseCtx->cursorPoint[PAUSE_WORLD_MAP];
        s16 nextCursorPoint = pauseCtx->cursorPoint[PAUSE_WORLD_MAP];
        bool mirrorWorldActive = CVarGetInteger("gModes.MirroredWorld.State", 0);
        bool goingLeft = pauseCtx->stickAdjX < -30;
        bool goingRight = pauseCtx->stickAdjX > 30;
        bool cursorMoving = goingLeft || goingRight;

        // Handle moving off page buttons
        if (pauseCtx->cursorSpecialPos == PAUSE_CURSOR_PAGE_LEFT && goingRight) {
            pauseCtx->cursorSpecialPos = 0;
            pauseCtx->cursorPoint[PAUSE_WORLD_MAP] = mirrorWorldActive ? OWL_WARP_STONE_TOWER + 1 : REGION_NONE;
            nextCursorPoint = mirrorWorldActive ? OWL_WARP_GREAT_BAY_COAST : OWL_WARP_STONE_TOWER;
            Audio_PlaySfx(NA_SE_SY_CURSOR);
        } else if (pauseCtx->cursorSpecialPos == PAUSE_CURSOR_PAGE_RIGHT && goingLeft) {
            pauseCtx->cursorSpecialPos = 0;
            pauseCtx->cursorPoint[PAUSE_WORLD_MAP] = mirrorWorldActive ? REGION_NONE : OWL_WARP_STONE_TOWER + 1;
            nextCursorPoint = mirrorWorldActive ? OWL_WARP_STONE_TOWER : OWL_WARP_GREAT_BAY_COAST;
            Audio_PlaySfx(NA_SE_SY_CURSOR);
        }

        // Handle updating cursor color and A button action
        if (pauseCtx->cursorSpecialPos == 0) {
            pauseCtx->cursorColorSet = PAUSE_CURSOR_COLOR_SET_BLUE;

            if (gSaveContext.buttonStatus[EQUIP_SLOT_A] == BTN_DISABLED) {
                gSaveContext.buttonStatus[EQUIP_SLOT_A] = BTN_ENABLED;
                gSaveContext.hudVisibility = HUD_VISIBILITY_IDLE;
                Interface_SetHudVisibility(HUD_VISIBILITY_ALL);
            }
            if (interfaceCtx->aButtonDoActionDelayed != DO_ACTION_WARP) {
                Interface_SetAButtonDoAction(gPlayState, DO_ACTION_WARP);
            }
        } else {
            pauseCtx->cursorColorSet = PAUSE_CURSOR_COLOR_SET_WHITE;

            if (gSaveContext.buttonStatus[EQUIP_SLOT_A] != BTN_DISABLED) {
                gSaveContext.buttonStatus[EQUIP_SLOT_A] = BTN_DISABLED;
                gSaveContext.hudVisibility = HUD_VISIBILITY_IDLE;
                Interface_SetHudVisibility(HUD_VISIBILITY_ALL);
            }
            if (interfaceCtx->aButtonDoActionDelayed != DO_ACTION_INFO) {
                Interface_SetAButtonDoAction(gPlayState, DO_ACTION_INFO);
            }
        }

        // Handle mirror mode flip and starting cursor movement
        if (pauseCtx->cursorSpecialPos == 0) {
            if (CVarGetInteger("gEnhancements.Songs.BetterOwlWarpMenu", 0)) {
                cursorMoving = BetterOwlWarp_NextCursorPoint(&nextCursorPoint, pauseCtx);
            } else {
                nextCursorPoint = oldCursorPoint;
            }

            if (cursorMoving) {
                pauseCtx->cursorShrinkRate = 4.0f;

                if (mirrorWorldActive) {
                    goingLeft = !goingLeft;
                    goingRight = !goingRight;
                }
            }
        }

        // Actually move the cursor
        if (pauseCtx->cursorSpecialPos == 0 &&
            BetterOwlWarp_IsCutoffOnSide(pauseCtx->cursorPoint[PAUSE_WORLD_MAP], pauseCtx)) {
            KaleidoScope_MoveCursorToSpecialPos(
                gPlayState, (mirrorWorldActive == goingRight) ? PAUSE_CURSOR_PAGE_LEFT : PAUSE_CURSOR_PAGE_RIGHT);
            pauseCtx->cursorItem[PAUSE_MAP] = PAUSE_ITEM_NONE;
        } else if (oldCursorPoint != nextCursorPoint) {
            pauseCtx->cursorPoint[PAUSE_WORLD_MAP] = nextCursorPoint;
        } else if (goingRight) {
            do {
                pauseCtx->cursorPoint[PAUSE_WORLD_MAP]++;
                if (pauseCtx->cursorPoint[PAUSE_WORLD_MAP] > OWL_WARP_STONE_TOWER) {
                    KaleidoScope_MoveCursorToSpecialPos(gPlayState, mirrorWorldActive ? PAUSE_CURSOR_PAGE_LEFT
                                                                                      : PAUSE_CURSOR_PAGE_RIGHT);
                    pauseCtx->cursorItem[PAUSE_MAP] = PAUSE_ITEM_NONE;
                    break;
                }
            } while (!pauseCtx->worldMapPoints[pauseCtx->cursorPoint[PAUSE_WORLD_MAP]]);
        } else if (goingLeft) {
            do {
                pauseCtx->cursorPoint[PAUSE_WORLD_MAP]--;
                if (pauseCtx->cursorPoint[PAUSE_WORLD_MAP] <= REGION_NONE) {
                    KaleidoScope_MoveCursorToSpecialPos(gPlayState, mirrorWorldActive ? PAUSE_CURSOR_PAGE_RIGHT
                                                                                      : PAUSE_CURSOR_PAGE_LEFT);
                    pauseCtx->cursorItem[PAUSE_MAP] = PAUSE_ITEM_NONE;
                    break;
                }
            } while (!pauseCtx->worldMapPoints[pauseCtx->cursorPoint[PAUSE_WORLD_MAP]]);
        }

        // Updates name panel and cursor slot
        if (pauseCtx->cursorSpecialPos == 0) {
            // Offset from `ITEM_MAP_POINT_GREAT_BAY` is to get the correct ordering in `map_name_static`
            pauseCtx->cursorItem[PAUSE_MAP] =
                sOwlWarpPauseItems[pauseCtx->cursorPoint[PAUSE_WORLD_MAP]] - ITEM_MAP_POINT_GREAT_BAY;
            // Used as cursor vtxIndex
            pauseCtx->cursorSlot[PAUSE_MAP] = 31 + pauseCtx->cursorPoint[PAUSE_WORLD_MAP];
        }

        if (!pauseCtx->worldMapPoints[pauseCtx->cursorPoint[PAUSE_WORLD_MAP]]) {
            pauseCtx->cursorItem[PAUSE_MAP] = PAUSE_ITEM_NONE;
        }
        if (oldCursorPoint != pauseCtx->cursorPoint[PAUSE_WORLD_MAP]) {
            Audio_PlaySfx(NA_SE_SY_CURSOR);
        }
    }
}

static void HandlePauseOwlWarp(PauseContext* pauseCtx) {
    // Initialize worldMapPoints based on owl activation flags
    for (int i = OWL_WARP_STONE_TOWER; i >= OWL_WARP_GREAT_BAY_COAST; i--) {
        pauseCtx->worldMapPoints[i] = (gSaveContext.save.saveInfo.playerData.owlActivationFlags >> i) & 1;
    }

    // Special condition for when only the 15th owl statue is activated
    if (gSaveContext.save.saveInfo.playerData.owlActivationFlags == (1 << 15)) {
        for (int i = REGION_GREAT_BAY; i < REGION_STONE_TOWER; i++) {
            if ((gSaveContext.save.saveInfo.regionsVisited >> i) & 1) {
                pauseCtx->worldMapPoints[i] = true;
            }
        }

        // Always set Great Bay since that is accessible from the "default index" for index warp
        // when loading from a save/scene change and never touching the pause map screen
        pauseCtx->worldMapPoints[OWL_WARP_GREAT_BAY_COAST] = true;
    }

    // Ensure cursor starts at an activated point and is not on the broken stone tower region point
    if (!pauseCtx->worldMapPoints[pauseCtx->cursorPoint[PAUSE_WORLD_MAP]] ||
        pauseCtx->cursorPoint[PAUSE_WORLD_MAP] > OWL_WARP_STONE_TOWER) {
        for (int i = OWL_WARP_GREAT_BAY_COAST; i <= OWL_WARP_STONE_TOWER; i++) {
            if (pauseCtx->worldMapPoints[i]) {
                pauseCtx->cursorPoint[PAUSE_WORLD_MAP] = i;
                break;
            }
        }
    }

    Input* input = &gPlayState->state.input[0];

    if (pauseCtx->state == PAUSE_STATE_MAIN && pauseCtx->pageIndex == PAUSE_MAP && pauseCtx->mapPageRoll == 0) {
        if (pauseCtx->itemDescriptionOn) {
            HandleConfirmingState(pauseCtx, input);
        } else {
            UpdateCursorForOwlWarpPoints(pauseCtx);

            if (CHECK_BTN_ALL(input->press.button, BTN_A) && pauseCtx->cursorSpecialPos == 0 &&
                pauseCtx->worldMapPoints[pauseCtx->cursorPoint[PAUSE_WORLD_MAP]]) {
                pauseCtx->itemDescriptionOn = true;
                Audio_PlaySfx(NA_SE_SY_DECIDE);
                // Use Kaleido's open text variant that sets the dark black message box
                func_801514B0(gPlayState, 0x1B93, 3);
                gPlayState->msgCtx.choiceIndex = 0;
            }
        }
    }
}

static void RegisterPauseOwlWarp() {
    COND_HOOK(OnKaleidoUpdate, CVAR, [](PauseContext* pauseCtx) {
        if (!sInDungeonScene && PauseOwlWarp_IsOwlWarpEnabled() && CHECK_QUEST_ITEM(QUEST_SONG_SOARING)) {
            HandlePauseOwlWarp(pauseCtx);
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterPauseOwlWarp, { CVAR_NAME });
