#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/CustomMessage/CustomMessage.h"
#include "2s2h/CustomItem/CustomItem.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
#include "functions.h"
#include "overlays/actors/ovl_Bg_Haka_Curtain/z_bg_haka_curtain.h"
#include "overlays/actors/ovl_En_Po_Composer/z_en_po_composer.h"
#include "overlays/actors/ovl_Obj_Hgdoor/z_obj_hgdoor.h"
void func_80B6DE80(BgHakaCurtain* bgHakaCurtain);
void EnPoComposer_SetupPlayCurse(EnPoComposer* enPoComposer);
}

#define CVAR_NAME "gEnhancements.Cutscenes.SkipStoryCutscenes"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void skipHealingPamelasFather() {
    /*
     * Transition to house entrance. This does mean the player misses a minor optional interaction
     * where Pamela and her father embrace and Tatl yells at Link for killing the moment, but this
     * is easier than replicating all actor states in the skip.
     */
    GameInteractor::Instance->events.emplace_back(GIEventTransition{
        .entrance = ENTRANCE(MUSIC_BOX_HOUSE, 0),
        .cutsceneIndex = 0,
        .transitionTrigger = TRANS_TRIGGER_START,
        .transitionType = TRANS_TYPE_INSTANT,
    });
    SET_WEEKEVENTREG(WEEKEVENTREG_75_20); // Flag for healing Gibdo dad
}

void RegisterSkipIkanaCurseCutscenes() {
    // Must use the queue hook for the skip. If the start hook is used, cutscene cues will mess up the curtain movement.
    COND_VB_SHOULD(VB_QUEUE_CUTSCENE, CVAR, {
        s16* csId = va_arg(args, s16*);
        if (gPlayState->sceneId == SCENE_HAKASHITA && *csId == 16) { // Flat appears in his tomb Beneath the Graveyard
            BgHakaCurtain* bgHakaCurtain = (BgHakaCurtain*)Actor_FindNearby(
                gPlayState, &GET_PLAYER(gPlayState)->actor, ACTOR_BG_HAKA_CURTAIN, ACTORCAT_BG, 99999.9f);
            if (bgHakaCurtain != nullptr) {
                func_80B6DE80(bgHakaCurtain); // Raise the curtain
            }
            *should = false;
        }
    });

    // Pamela's Father
    COND_VB_SHOULD(VB_START_CUTSCENE, CVAR, {
        s16* csId = va_arg(args, s16*);
        if (gPlayState->sceneId == SCENE_MUSICHOUSE) { // Music Box House
            if (*csId == 9) {                          // Gibdo dad busts out
                ObjHgdoor* hgDoor = va_arg(args, ObjHgdoor*);
                ObjHgdoor* otherDoor = (ObjHgdoor*)hgDoor->dyna.actor.child;
                hgDoor->rotation = otherDoor->rotation = 0x5555;  // Open the doors
                hgDoor->dyna.actor.parent->colChkInfo.health = 1; // The Gibdo dad moves once this is set
                *should = false;

                Player* player = GET_PLAYER(gPlayState);
                player->speedXZ = 0.0f;
                player->actor.freezeTimer = 20;
            } else if (*csId == 10 || *csId == 12) { // Failed to heal Pamela's father
                /*
                 * The WEEKEVENTREG_61_02 flag marks that the player has failed to heal Pamela's father this cycle.
                 * This only determines whether to play the first failure cutscene or the repeat failure cutscene.
                 * The WEEKEVENTREG_59_01 and WEEKEVENTREG_61_04 flags interact. WEEKEVENTREG_61_04 seems to
                 * indicate that Pamela is in a state where she is outside, or she will come outside. With that,
                 * WEEKEVENTREG_59_01 determines whether Pamela on a path to/from the well, or if she is hiding
                 * inside on a timer. We clear WEEKEVENTREG_59_01 to ensure that Pamela is still hiding inside
                 * when the player gets kicked out.
                 */
                CLEAR_WEEKEVENTREG(WEEKEVENTREG_59_01);
                SET_WEEKEVENTREG(WEEKEVENTREG_61_02);
                SET_WEEKEVENTREG(WEEKEVENTREG_61_04);
                // Kick out the player
                GameInteractor::Instance->events.emplace_back(GIEventTransition{
                    .entrance = ENTRANCE(IKANA_CANYON, 2),
                    .cutsceneIndex = 0,
                    .transitionTrigger = TRANS_TRIGGER_START,
                    .transitionType = TRANS_TYPE_FADE_BLACK_FAST,
                });
                *should = false;
            } else if (*csId == 11) { // Heal Pamela's father for the first time
                skipHealingPamelasFather();
                if (GameInteractor_Should(VB_GIVE_ITEM_FROM_DMCHAR05, true, ITEM_MASK_GIBDO)) {
                    GameInteractor::Instance->events.emplace_back(GIEventGiveItem{
                        .showGetItemCutscene = !CVarGetInteger("gEnhancements.Cutscenes.SkipGetItemCutscenes", 0),
                        .param = GID_MASK_GIBDO,
                        .giveItem =
                            [](Actor* actor, PlayState* play) {
                                if (CUSTOM_ITEM_FLAGS & CustomItem::GIVE_ITEM_CUTSCENE) {
                                    CustomMessage::SetActiveCustomMessage("You received the Gibdo Mask!",
                                                                          { .textboxType = 2 });
                                } else {
                                    CustomMessage::StartTextbox("You received the Gibdo Mask!\x1C\x02\x10",
                                                                { .textboxType = 2 });
                                }
                                Item_Give(gPlayState, ITEM_MASK_GIBDO);
                            },
                    });
                }
                *should = false;
            } else if (*csId == 13) { // Heal Pamela's father subsequent times (no mask drop)
                skipHealingPamelasFather();
                *should = false;
            }
        }
    });

    // Automatically init Sharp into the curse playing state
    COND_ID_HOOK(OnActorInit, ACTOR_EN_PO_COMPOSER, CVAR, [](Actor* actor) {
        if (!(actor->params & 0x8000)) { // Is Sharp
            EnPoComposer* enPoComposer = (EnPoComposer*)actor;
            enPoComposer->visible = true;
            enPoComposer->lightColor.a = 255;
            EnPoComposer_SetupPlayCurse(enPoComposer);
            SET_WEEKEVENTREG(WEEKEVENTREG_14_02); // Have encountered Sharp
        }
    });

    // Healing Sharp's curse
    COND_VB_SHOULD(VB_START_CUTSCENE, CVAR, {
        s16* csId = va_arg(args, s16*);
        if (gPlayState->sceneId == SCENE_IKANA && *csId == 20) { // Played Song of Storms for Sharp
            // Reload the scene so that all actor states are appropriate
            GameInteractor::Instance->events.emplace_back(GIEventTransition{
                .entrance = ENTRANCE(IKANA_CANYON, 14),
                .cutsceneIndex = 0,
                .transitionTrigger = TRANS_TRIGGER_START,
                .transitionType = TRANS_TYPE_INSTANT,
            });
            SET_WEEKEVENTREG(WEEKEVENTREG_14_04); // Healed Sharp
            *should = false;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterSkipIkanaCurseCutscenes, { CVAR_NAME });
