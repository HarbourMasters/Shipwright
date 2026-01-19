#include "GameInteractor.h"
#include "soh/Enhancements/custom-item/CustomItem.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "variables.h"
#include "macros.h"
#include "functions.h"

extern SaveContext gSaveContext;
extern PlayState* gPlayState;
}

void ProcessEvents() {
    Player* player = GET_PLAYER(gPlayState);

    // If the player has a message active, stop
    if (gPlayState->msgCtx.msgMode != 0) {
        return;
    }

    // If the player is in a blocking cutscene, stop
    if (Player_InBlockingCsMode(gPlayState, player)) {
        return;
    }

    // If player is dead, stop
    if (player->stateFlags1 & PLAYER_STATE1_DEAD) {
        return;
    }

    // If there is an event active, stop
    const auto& currentEvent = GameInteractor::Instance->currentEvent;
    if (auto e = std::get_if<GIEventNone>(&currentEvent)) {
        // no-op
    } else {
        return;
    }

    // If there are no events that need to happen, stop
    if (GameInteractor::Instance->events.empty()) {
        return;
    }

    GameInteractor::Instance->currentEvent = GameInteractor::Instance->events.front();
    const auto& nextEvent = GameInteractor::Instance->currentEvent;

    if (auto e = std::get_if<GIEventGiveItem>(&nextEvent)) {
        EnItem00* enItem00;
        // If the player is climbing or in the air, deliver the item without a cutscene but freeze the player
        if (!e->showGetItemCutscene ||
            (player->stateFlags1 &
             (PLAYER_STATE1_CHARGING_SPIN_ATTACK | PLAYER_STATE1_HANGING_OFF_LEDGE | PLAYER_STATE1_CLIMBING_LEDGE |
              PLAYER_STATE1_JUMPING | PLAYER_STATE1_FREEFALL | PLAYER_STATE1_FIRST_PERSON |
              PLAYER_STATE1_CLIMBING_LADDER | PLAYER_STATE1_IN_WATER)) ||
            (Player_GetExplosiveHeld(player) > -1)) {
            enItem00 = CustomItem::Spawn(
                player->actor.world.pos.x, player->actor.world.pos.y, player->actor.world.pos.z, 0,
                CustomItem::GIVE_OVERHEAD | CustomItem::HIDE_TILL_OVERHEAD | CustomItem::KEEP_ON_PLAYER, e->param,
                [](Actor* actor, PlayState* play) {
                    Player* player = GET_PLAYER(gPlayState);
                    const auto& nextEvent = GameInteractor::Instance->currentEvent;
                    if (auto e = std::get_if<GIEventGiveItem>(&nextEvent)) {
                        e->giveItem(actor, play);
                        if (e->showGetItemCutscene) {
                            player->actor.freezeTimer = 30;
                        }
                    }
                },
                e->drawItem);
        } else {
            enItem00 = CustomItem::Spawn(
                player->actor.world.pos.x, player->actor.world.pos.y, player->actor.world.pos.z, 0,
                CustomItem::GIVE_ITEM_CUTSCENE | CustomItem::HIDE_TILL_OVERHEAD | CustomItem::KEEP_ON_PLAYER, e->param,
                e->giveItem, e->drawItem);
        }
        enItem00->actor.destroy = [](Actor* actor, PlayState* play) {
            if (!(CUSTOM_ITEM_FLAGS & CustomItem::CALLED_ACTION)) {
                // Event was not handled, requeue it
                GameInteractor::Instance->events.push_back(GameInteractor::Instance->currentEvent);
            }

            GameInteractor::Instance->currentEvent = GIEventNone{};
        };
    }

    GameInteractor::Instance->events.erase(GameInteractor::Instance->events.begin());
}

static RegisterShipInitFunc initFunc(
    []() {
        COND_HOOK(OnGameStateMainStart, true, []() {
            // Cleanup all hooks at the start of each frame
            GameInteractor::Instance->RemoveAllQueuedHooks();
        });

        COND_HOOK(OnPlayerUpdate, true, ProcessEvents);
    },
    {});
