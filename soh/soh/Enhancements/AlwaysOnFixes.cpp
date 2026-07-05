#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "macros.h"
#include "functions.h"
#include "variables.h"
#include "src/overlays/actors/ovl_En_Go2/z_en_go2.h"
#include "src/overlays/actors/ovl_En_Test/z_en_test.h"
extern void Player_UseItem(PlayState*, Player*, s32);
extern PlayState* gPlayState;
}

// Vanilla EnTest_Destroy restores the field/combat BGM only when no other
// En_Test (Stalfos) remains nearby, but it checks with Actor_FindNearby,
// which also matches actors that were already killed this frame (update == NULL)
// and are still in the actor list. When the last Stalfos of a group dies in the
// same frame as its siblings, those siblings are still listed, so vanilla
// believes an enemy remains and the combat BGM never restores (most visible in
// the Master Quest Water Temple Stalfos group). This hook re-runs the restore
// check counting only LIVING En_Test actors, so the BGM restores correctly.
static bool EnTest_HasLivingNearby(Actor* refActor) {
    Actor* actor = gPlayState->actorCtx.actorLists[ACTORCAT_ENEMY].head;
    while (actor != NULL) {
        if (actor != refActor && actor->id == ACTOR_EN_TEST && actor->update != NULL &&
            Actor_WorldDistXYZToActor(refActor, actor) <= 8000.0f) {
            return true;
        }
        actor = actor->next;
    }
    return false;
}

void RegisterAlwaysOnFixes() {
    // Dying or using Din's Fire in the Outside Temple of Time area crashes the game.
    // In vanilla this can never happen, but with CrowdControl, Sail, Unrestricted Items
    // and others this *can* happen. Because it checks for a camId of -1, this code path
    // shouldn't ever influence vanilla play regardless.
    COND_VB_SHOULD(VB_SHOULD_LOAD_BG_IMAGE, true, {
        int32_t* camId = va_arg(args, int*);
        if (*camId == -1) {
            *should = false;
        }
    });

    // Vanilla bug: `Actor_Item_Shield` (dropped Deku Shield when burning) assumes that segment 12
    // contains Link display list `gCullBackDList`. If an actor is drawn between player and shield
    // that uses segment 12 (such as Jabu-Jabu tentacles), the game will crash on Deku Shield drop.
    // Fix: Re-set segment 12 to the required display list.
    COND_VB_SHOULD(VB_ITEMSHIELD_DRAW, true, {
        GraphicsContext* __gfxCtx = gPlayState->state.gfxCtx;
        gSPSegment(POLY_OPA_DISP++, 0x0C, (uintptr_t)SEGMENTED_TO_VIRTUAL(gCullBackDList));
    });

    // Vanilla bug: If Hookshot doesn't spawn, player is softlocked. (eg. use as child, no memory left)
    // Fix: Change item to none if no spawn. (Ranged weapon state is removed by `Player_InitItemAction`)
    COND_VB_SHOULD(VB_INIT_HOOKSHOT_IA, true, {
        Player* player = va_arg(args, Player*);
        if (player->heldActor == NULL) {
            Player_UseItem(gPlayState, player, 0xFF);
        }
    });

    // Vanilla bug: When pulling out Hookshot, if `this->actor.parent` is set but not Hookshot, player
    // is locked into repeated fly-land-fly. Possible with enemies that grab player and set themselves
    // as parent (such as Moblin in water, eaten by Like like that despawns falling through En_Holl).
    // Fix: Ensure that parent actor has Hookshot actor ID before starting flying.
    COND_VB_SHOULD(VB_PREVENT_HOOKSHOT_PARENT_SOFTLOCK, true, {
        s16* parentId = va_arg(args, s16*);
        if (*parentId != ACTOR_ARMS_HOOK) {
            *should = false;
        }
    });

    // Vanilla bug: If player starts talking with Goron Link for the first time (before getting tunic)
    // but moves out of range, player will softlock because the text cannot progress to the question
    // choice textbox when Goron Link is asleep (UpdateTalkState cannot run).
    // Fix: Allow updating talkState even when Goron Link is asleep.
    COND_VB_SHOULD(VB_PREVENT_GORON_LINK_SOFTLOCK, true, {
        EnGo2* GoronLink = va_arg(args, EnGo2*);
        if (GoronLink->interactInfo.talkState == NPC_TALK_STATE_TALKING) {
            *should = true;
        }
    });

    COND_ID_HOOK(OnActorDestroy, ACTOR_EN_TEST, true, [](void* refActor) {
        Actor* actor = reinterpret_cast<Actor*>(refActor);
        if (actor->params != STALFOS_TYPE_2 && !EnTest_HasLivingNearby(actor)) {
            func_800F5B58();
        }
    });
}

static RegisterShipInitFunc initAlwaysOnFixes(RegisterAlwaysOnFixes, { "" });
