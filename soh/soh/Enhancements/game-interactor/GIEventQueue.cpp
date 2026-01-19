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
    if (!gPlayState) {
        return;
    }

    Player* player = GET_PLAYER(gPlayState);

    if (!player) {
        return;
    }

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

        s16 flags = CustomItem::HIDE_TILL_OVERHEAD | CustomItem::KEEP_ON_PLAYER;

        // If the player is climbing or in the air, deliver the item without a cutscene but freeze the player
        if (!e->showGetItemCutscene ||
            (player->stateFlags1 &
             (PLAYER_STATE1_CHARGING_SPIN_ATTACK | PLAYER_STATE1_HANGING_OFF_LEDGE | PLAYER_STATE1_CLIMBING_LEDGE |
              PLAYER_STATE1_JUMPING | PLAYER_STATE1_FREEFALL | PLAYER_STATE1_FIRST_PERSON |
              PLAYER_STATE1_CARRYING_ACTOR | PLAYER_STATE1_CLIMBING_LADDER | PLAYER_STATE1_IN_WATER)) ||
            (Player_GetExplosiveHeld(player) > -1)) {
            flags |= CustomItem::GIVE_OVERHEAD;
        } else {
            flags |= CustomItem::GIVE_ITEM_CUTSCENE;
        }
        enItem00 = CustomItem::Spawn(
            player->actor.world.pos.x, player->actor.world.pos.y, player->actor.world.pos.z, 0, flags, e->param,
            [](Actor* actor, PlayState* play) {
                const auto& nextEvent = GameInteractor::Instance->currentEvent;
                if (auto e = std::get_if<GIEventGiveItem>(&nextEvent)) {
                    e->giveItem(actor, play);
                    GameInteractor::Instance->currentEvent = GIEventNone{};
                }
            },
            e->drawItem);
        enItem00->actor.destroy = [](Actor* actor, PlayState* play) {
            if (!(CUSTOM_ITEM_FLAGS & CustomItem::CALLED_ACTION)) {
                // Event was not handled, requeue it
                GameInteractor::Instance->events.push_back(GameInteractor::Instance->currentEvent);
            }
        };
    }

    GameInteractor::Instance->events.erase(GameInteractor::Instance->events.begin());
}

static RegisterShipInitFunc initFunc(
    []() {
        COND_HOOK(OnGameStateMainStart, true, []() {
            // Cleanup all hooks at the start of each frame
            GameInteractor::Instance->RemoveAllQueuedHooks();
            ProcessEvents();
        });

        COND_HOOK(OnLoadGame, true, [](int32_t fileNum) {
            GameInteractor::Instance->currentEvent = GIEventNone{};
            GameInteractor::Instance->events.clear();
        });
    },
    {});
