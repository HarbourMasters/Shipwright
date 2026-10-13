#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/CustomMessage/CustomMessage.h"
#include "2s2h/CustomItem/CustomItem.h"

extern "C" {
#include "z64actor.h"
#include "functions.h"
#include "z64item.h"

#include "overlays/actors/ovl_Obj_Tsubo/z_obj_tsubo.h"
}

// This file houses various examples of using the systems we have built to modify the game in various ways
void RegisterDemoBehavior() {
    // Demonstrates some capabilities of CustomItem
    GameInteractor::Instance->RegisterGameHookForID<GameInteractor::ShouldActorInit>(
        ACTOR_EN_ITEM00, [](Actor* actor, bool* should) {
            // South Clock Town PoH
            if (actor->params != 2566) {
                return;
            }
            *should = false;

            // Spawn a dummy custom item, will just be killed when you touch it and run it's action func
            CustomItem::Spawn(
                actor->world.pos.x, actor->world.pos.y, actor->world.pos.z, actor->shape.rot.y,
                CustomItem::KILL_ON_TOUCH, GID_RUPEE_GREEN,
                [](Actor* actor, PlayState* play) {
                    Player* player = GET_PLAYER(play);

                    // You can put whatever you want here. For instance you can use the GI queue to queue up a different
                    // kind of item give:
                    GameInteractor::Instance->events.push_back(GIEventGiveItem{
                        .showGetItemCutscene = true,
                        .param = GID_RUPEE_GREEN,
                        .giveItem =
                            [](Actor* actor, PlayState* play) {
                                if (CUSTOM_ITEM_FLAGS & CustomItem::GIVE_ITEM_CUTSCENE) {
                                    CustomMessage::SetActiveCustomMessage("You found Greg!");
                                } else {
                                    CustomMessage::StartTextbox(
                                        "You found Greg! (While you were jumping or something) \x1C\x02\x10");
                                }

                                Rupees_ChangeBy(1);
                            },
                        .drawItem =
                            [](Actor* actor, PlayState* play) {
                                Matrix_Scale(30.0f, 30.0f, 30.0f, MTXMODE_APPLY);
                                GetItem_Draw(play, CUSTOM_ITEM_PARAM);
                            },
                    });

                    // Or you can spawn another CustomItem to give an item that way, but the GI queue is more flexible
                    // and reliable
                    CustomItem::Spawn(
                        player->actor.world.pos.x, player->actor.world.pos.y, player->actor.world.pos.z, 0,
                        CustomItem::GIVE_ITEM_CUTSCENE | CustomItem::HIDE_TILL_OVERHEAD | CustomItem::KEEP_ON_PLAYER, 0,
                        [](Actor* actor, PlayState* play) {
                            CustomMessage::SetActiveCustomMessage("Loser! You thought it was Greg but you get nothing.",
                                                                  {
                                                                      .textboxType = 2,
                                                                  });
                        },
                        [](Actor* actor, PlayState* play) {
                            // Draw Nothing
                        });
                },
                [](Actor* actor, PlayState* play) {
                    Matrix_Scale(30.0f, 30.0f, 30.0f, MTXMODE_APPLY);
                    GetItem_Draw(play, actor->home.rot.z);
                });
        });

    // Example doing a give item while a player is in the middle of an action (backflip)
    GameInteractor::Instance->RegisterGameHookForID<GameInteractor::OnActorUpdate>(ACTOR_PLAYER, [](Actor* actor) {
        Player* player = (Player*)actor;
        static int backflipTimer = 0;

        if (player->stateFlags2 & PLAYER_STATE2_80000) {
            backflipTimer++;

            if (backflipTimer == 4) {
                player->actor.freezeTimer = 30;
                CustomItem::Spawn(player->actor.world.pos.x, player->actor.world.pos.y, player->actor.world.pos.z, 0,
                                  CustomItem::GIVE_OVERHEAD | CustomItem::KEEP_ON_PLAYER, GID_RUPEE_GREEN,
                                  [](Actor* actor, PlayState* play) {
                                      Rupees_ChangeBy(1);
                                      CustomMessage::StartTextbox("Nice flip!\x1C\x02\x10", {
                                                                                                .textboxType = 2,
                                                                                            });
                                  });
            }
        } else {
            backflipTimer = 0;
        }
    });

    GameInteractor::Instance->RegisterGameHookForID<GameInteractor::OnActorKill>(ACTOR_OBJ_TSUBO, [](Actor* actor) {
        if (actor->room == gPlayState->roomCtx.curRoom.num) {
            // Open a custom message
            CustomMessage::StartTextbox("STOP BREAKING MY POTS!", {
                                                                      .textboxType = 2,
                                                                      .icon = 0x3D,
                                                                  });

            // Pot Hydra
            // GameInteractor::Instance->events.push_back(GIEventSpawnActor{
            //     .actorId = ACTOR_OBJ_TSUBO,
            //     .posX = 50.0f,
            //     .posY = 0,
            //     .posZ = -40.0f, // Behind the player
            //     .relativeCoords = true,
            // });
            // GameInteractor::Instance->events.push_back(GIEventSpawnActor{
            //     .actorId = ACTOR_OBJ_TSUBO,
            //     .posX = -50.0f,
            //     .posY = 0,
            //     .posZ = -50.0f, // Behind the player
            //     .relativeCoords = true,
            // });
        }
    });

    // Full message replacement (Sign in South Clock Town)
    GameInteractor::Instance->RegisterGameHookForID<GameInteractor::OnOpenText>(
        0x1C18, [](u16* textId, bool* loadFromMessageTable) {
            CustomMessage::Entry entry = {
                .icon = 0x10,
                .msg = "Hello World\n"
                       "You are in Clock Town\n"
                       "            - Moon",
            };

            CustomMessage::LoadCustomMessageIntoFont(entry);
            *loadFromMessageTable = false;
        });

    // Partial message replacement (Title Card for South Clock Town)
    GameInteractor::Instance->RegisterGameHookForID<GameInteractor::OnOpenText>(
        0x104, [](u16* textId, bool* loadFromMessageTable) {
            auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);

            CustomMessage::Replace(&entry.msg, "Clock Town", "Hyrule");

            CustomMessage::LoadCustomMessageIntoFont(entry);
            *loadFromMessageTable = false;
        });
}
