#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/CustomMessage/CustomMessage.h"
#include "2s2h/CustomItem/CustomItem.h"
#include "2s2h/Rando/Rando.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
#include "functions.h"
}

#define CVAR_NAME "gEnhancements.Cutscenes.SkipStoryCutscenes"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

/*
 * Utility function made common so that rando actor behavior can access it while also doing other things. Mimics the
 * flags and transitions set by func_808BA10C in z_door_warp1.c.
 */
void HandleGiantsCutsceneSkip() {
    GIEventTransition transition;
    switch (gPlayState->sceneId) {
        case SCENE_MITURIN_BS: // Odolwa's Lair
            SET_WEEKEVENTREG(WEEKEVENTREG_CLEARED_WOODFALL_TEMPLE);
            SET_WEEKEVENTREG(WEEKEVENTREG_ENTERED_WOODFALL_TEMPLE_PRISON);
            transition.entrance = ENTRANCE(WOODFALL_TEMPLE, 1);
            transition.cutsceneIndex = 0;
            break;
        case SCENE_HAKUGIN_BS: // Goht's Lair
            SET_WEEKEVENTREG(WEEKEVENTREG_CLEARED_SNOWHEAD_TEMPLE);
            transition.entrance = ENTRANCE(MOUNTAIN_VILLAGE_SPRING, 7);
            transition.cutsceneIndex = 0;
            break;
        case SCENE_SEA_BS: // Gyorg's Lair
            SET_WEEKEVENTREG(WEEKEVENTREG_CLEARED_GREAT_BAY_TEMPLE);
            transition.entrance = ENTRANCE(ZORA_CAPE, 9);
            transition.cutsceneIndex = 0xFFF0;
            break;
        case SCENE_INISIE_BS: // Twinmold's Lair
            SET_WEEKEVENTREG(WEEKEVENTREG_CLEARED_STONE_TOWER_TEMPLE);
            transition.entrance = ENTRANCE(IKANA_CANYON, 15);
            transition.cutsceneIndex = 0xFFF2;
            break;
    }

    /*
     * At the point of transition, the previous scene's information is lost, but we need it to set the proper Giants
     * flags. Here, we store the target transition information so that we can circumvent the Giants' Chamber cutscene.
     * We're not actually using the queued transition normally, but using its values to alter a Giants' Chamber
     * transition.
     */
    transition.transitionType = gPlayState->sceneId;
    GameInteractor::Instance->events.emplace_back(transition);
}

// Only reached if the cutscene is skipped
void handleGiantsCheck(SceneId sceneId) {
    /*
     * This whole block comes from func_808B9CE8 in z_door_warp1.c. unk_EA8[0] represents which particular Giants have
     * been freed (e.g. Woodfall Giant). unk_EA8[1] represents the total number of Giants freed (0-4). These flags only
     * seem to matter for Giants' Chamber cutscenes. The Clock Tower scene instead checks for Boss Remains that the
     * player has.
     */
    if (IS_RANDO) {
        switch (sceneId) {
            case SCENE_MITURIN_BS:
                // Mark Woodfall Giant as freed
                gSaveContext.save.saveInfo.unk_EA8[0] =
                    (((void)0, gSaveContext.save.saveInfo.unk_EA8[0]) & 0xFFFFFF00) |
                    (((u8)gSaveContext.save.saveInfo.unk_EA8[1]) & 0xFF);
                break;
            case SCENE_HAKUGIN_BS:
                // Mark Snowhead Giant as freed
                gSaveContext.save.saveInfo.unk_EA8[0] =
                    (((void)0, gSaveContext.save.saveInfo.unk_EA8[0]) & 0xFFFF00FF) |
                    ((((u8)gSaveContext.save.saveInfo.unk_EA8[1]) & 0xFF) << 8);
                break;
            case SCENE_INISIE_BS:
                // Mark Stone Tower Giant as freed
                gSaveContext.save.saveInfo.unk_EA8[0] =
                    (((void)0, gSaveContext.save.saveInfo.unk_EA8[0]) & 0xFF00FFFF) |
                    ((((u8)gSaveContext.save.saveInfo.unk_EA8[1]) & 0xFF) << 0x10);
                break;
            case SCENE_SEA_BS:
                // Mark Great Bay Giant as freed
                gSaveContext.save.saveInfo.unk_EA8[0] =
                    (((void)0, gSaveContext.save.saveInfo.unk_EA8[0]) & 0x00FFFFFF) |
                    ((((u8)gSaveContext.save.saveInfo.unk_EA8[1]) & 0xFF) << 0x18);
                break;
            default:
                break;
        }
        // This is a fancy way of incrementing the flag that represents the total number of Giants freed.
        gSaveContext.save.saveInfo.unk_EA8[1] = (gSaveContext.save.saveInfo.unk_EA8[1] & 0xFFFFFF00) |
                                                ((((u8)gSaveContext.save.saveInfo.unk_EA8[1]) + 1) & 0xFF);
    }

    // The Oath to Order check only occurs when freeing a Giant for the first time.
    if (IS_RANDO) {
        if (!RANDO_SAVE_CHECKS[RC_GIANTS_CHAMBER_OATH_TO_ORDER].cycleObtained) {
            RANDO_SAVE_CHECKS[RC_GIANTS_CHAMBER_OATH_TO_ORDER].eligible = true;
        }
    } else if (gSaveContext.save.saveInfo.unk_EA8[1] == 1) {
        GameInteractor::Instance->events.emplace_back(GIEventGiveItem{
            .showGetItemCutscene = !CVarGetInteger("gEnhancements.Cutscenes.SkipGetItemCutscenes", 0),
            .giveItem =
                [](Actor* actor, PlayState* play) {
                    if (CUSTOM_ITEM_FLAGS & CustomItem::GIVE_ITEM_CUTSCENE) {
                        CustomMessage::SetActiveCustomMessage("You learned the Oath to Order!", { .textboxType = 2 });
                    } else {
                        CustomMessage::StartTextbox("You learned the Oath to Order!\x1C\x02\x10", { .textboxType = 2 });
                    }
                    Item_Give(gPlayState, ITEM_SONG_OATH);
                },
            .drawItem =
                [](Actor* actor, PlayState* play) {
                    Matrix_Scale(30.0f, 30.0f, 30.0f, MTXMODE_APPLY);
                    Rando::DrawItem(RI_SONG_OATH);
                } });
    }
}

void RegisterSkipGiantsChamber() {
    /*
     * Skip Giants' Chamber cutscenes. This is forced on for rando for now, as the first scene the player sees contains
     * a song tutorial prompt. The other cutscenes do not, but it might seem weird to force the skip for only the first
     * one and not others.
     */
    COND_VB_SHOULD(VB_PLAY_TRANSITION_CS, CVAR || IS_RANDO, {
        if (gSaveContext.save.entrance == ENTRANCE(GIANTS_CHAMBER, 0)) {
            /*
             * The warp gate processing silently queues up an event transition with information for the particular
             * area the player is in (Woodfall, Great Bay, etc.). This is necessary because the previous scene's
             * information is lost during a transition, and we want to skip the Giants' Chamber that we are otherwise
             * about to go to. We quietly use the queued transition to modify the transition in progress. The queued
             * transition must then be erased from the event queue, otherwise it will repeat once.
             */
            auto it = std::find_if(GameInteractor::Instance->events.begin(), GameInteractor::Instance->events.end(),
                                   [](const GIEvent& v) { return std::holds_alternative<GIEventTransition>(v); });
            if (it != GameInteractor::Instance->events.end()) {
                GIEventTransition transition = std::get<GIEventTransition>(*it);
                gSaveContext.save.entrance = transition.entrance;
                gSaveContext.save.cutsceneIndex = transition.cutsceneIndex;
                GameInteractor::Instance->events.erase(it);
                handleGiantsCheck((SceneId)transition.transitionType);
            }
        } else if (gSaveContext.save.entrance == ENTRANCE(WOODFALL, 0) && gSaveContext.save.cutsceneIndex == 0xFFF0) {
            // Odolwa's Lair repeat warps go straight to the Woodfall clear cutscene. Skip that too.
            SET_WEEKEVENTREG(WEEKEVENTREG_CLEARED_WOODFALL_TEMPLE);
            SET_WEEKEVENTREG(WEEKEVENTREG_ENTERED_WOODFALL_TEMPLE_PRISON);
            gSaveContext.save.entrance = ENTRANCE(WOODFALL_TEMPLE, 1);
            gSaveContext.save.cutsceneIndex = 0;
        }
    });

    // Handle Giants' Chamber cutscene skip for non-rando. Rando has its own skip with additional check processing.
    COND_VB_SHOULD(VB_GIVE_ITEM_FROM_OFFER, CVAR && !IS_RANDO, {
        GetItemId* item = va_arg(args, GetItemId*);
        Actor* actor = va_arg(args, Actor*);
        if (actor->id == ACTOR_DOOR_WARP1) {
            HandleGiantsCutsceneSkip();
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterSkipGiantsChamber, { CVAR_NAME, "IS_RANDO" });
