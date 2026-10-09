#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/Rando/Rando.h"
#include "2s2h/CustomMessage/CustomMessage.h"
#include "2s2h/CustomItem/CustomItem.h"
#include "2s2h/ShipInit.hpp"
#include <spdlog/spdlog.h>

extern "C" {
#include "functions.h"
#include "variables.h"
#include "assets/objects/object_gi_melody/object_gi_melody.h"
#include "overlays/actors/ovl_En_Osn/z_en_osn.h"

void EnOsn_Talk(EnOsn* enOsn, PlayState* play);
void EnOsn_Idle(EnOsn* enOsn, PlayState* play);
void PlayerCall_Init(Actor* thisx, PlayState* play);
void PlayerCall_Update(Actor* thisx, PlayState* play);
void PlayerCall_Draw(Actor* thisx, PlayState* play);
void Player_StopHorizontalMovement(Player* player);
}

#define CVAR_NAME "gEnhancements.Cutscenes.SkipStoryCutscenes"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

#define OSN_STATE_END_CONVERSATION (1 << 5)

void RegisterSkipLearningSongOfHealing() {
    // TODO: Currently forced on for rando, maybe won't be when you shuffle only song locations (need to override
    // learning mechanism)
    COND_VB_SHOULD(VB_OSN_TEACH_SONG_OF_HEALING, CVAR || IS_RANDO, {
        EnOsn* enOsn = va_arg(args, EnOsn*);
        Player* player = GET_PLAYER(gPlayState);

        *should = false;

        // Transform the player into human form if we're not in rando
        if (!IS_RANDO) {
            s16 objectId = OBJECT_LINK_NUTS;
            gActorOverlayTable[ACTOR_PLAYER].profile->objectId = objectId;
            func_8012F73C(&gPlayState->objectCtx, player->actor.objectSlot, objectId);
            player->actor.objectSlot = Object_GetSlot(&gPlayState->objectCtx, GAMEPLAY_KEEP);
            gSaveContext.save.playerForm = PLAYER_FORM_HUMAN;
            s32 objectSlot = Object_GetSlot(&gPlayState->objectCtx, gActorOverlayTable[ACTOR_PLAYER].profile->objectId);
            player->actor.objectSlot = objectSlot;
            player->actor.shape.rot.z = GET_PLAYER_FORM + 1;
            player->actor.init = PlayerCall_Init;
            player->actor.update = PlayerCall_Update;
            player->actor.draw = PlayerCall_Draw;
            gSaveContext.save.equippedMask = PLAYER_MASK_NONE;
            Player_StopHorizontalMovement(player);

            TransitionFade_SetColor(&gPlayState->unk_18E48, 0x000000);
            R_TRANS_FADE_FLASH_ALPHA_STEP = -1;
            Player_PlaySfx(GET_PLAYER(gPlayState), NA_SE_SY_TRANSFORM_MASK_FLASH);
        }

        // The last thing that he says after teaching you the song.
        enOsn->textId = 0x1FCD;
        enOsn->stateFlags |= OSN_STATE_END_CONVERSATION;
        Message_StartTextbox(gPlayState, enOsn->textId, &enOsn->actor);
        enOsn->actionFunc = EnOsn_Talk;

        // Wait till the conversation ends, then end the interaction (for some reason without this step you get soft
        // locked)
        static int hookId = 0;
        GameInteractor::Instance->UnregisterGameHookForPtr<GameInteractor::OnActorUpdate>(hookId);
        hookId = GameInteractor::Instance->RegisterGameHookForPtr<GameInteractor::OnActorUpdate>(
            (uintptr_t)enOsn, [](Actor* actor) {
                EnOsn* enOsn = (EnOsn*)actor;
                if (enOsn->actionFunc == EnOsn_Idle) {
                    Player_SetCsActionWithHaltedActors(gPlayState, &enOsn->actor, PLAYER_CSACTION_END);

                    GameInteractor::Instance->UnregisterGameHookForPtr<GameInteractor::OnActorUpdate>(hookId);
                }
            });

        if (GameInteractor_Should(VB_GIVE_ITEM_FROM_OSN, true, enOsn)) {
            // Queue up the item gives
            GameInteractor::Instance->events.emplace_back(
                GIEventGiveItem{ .showGetItemCutscene = true,
                                 .giveItem =
                                     [](Actor* actor, PlayState* play) {
                                         if (CUSTOM_ITEM_FLAGS & CustomItem::GIVE_ITEM_CUTSCENE) {
                                             CustomMessage::SetActiveCustomMessage("You received the Song of Healing!",
                                                                                   { .textboxType = 2 });
                                         } else {
                                             CustomMessage::StartTextbox(
                                                 "You received the Song of Healing!\x1C\x02\x10", { .textboxType = 2 });
                                         }
                                         Item_Give(gPlayState, ITEM_SONG_HEALING);
                                     },
                                 .drawItem =
                                     [](Actor* actor, PlayState* play) {
                                         Matrix_Scale(30.0f, 30.0f, 30.0f, MTXMODE_APPLY);
                                         Rando::DrawItem(RI_SONG_HEALING);
                                     } });
            GameInteractor::Instance->events.emplace_back(GIEventGiveItem{
                .showGetItemCutscene = true,
                .param = GID_MASK_DEKU,
                .giveItem =
                    [](Actor* actor, PlayState* play) {
                        if (CUSTOM_ITEM_FLAGS & CustomItem::GIVE_ITEM_CUTSCENE) {
                            CustomMessage::SetActiveCustomMessage("You received the Deku Mask!", { .textboxType = 2 });
                        } else {
                            CustomMessage::StartTextbox("You received the Deku Mask!\x1C\x02\x10",
                                                        { .textboxType = 2 });
                        }
                        Item_Give(gPlayState, ITEM_MASK_DEKU);
                    },
            });
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterSkipLearningSongOfHealing, { CVAR_NAME, "IS_RANDO" });
