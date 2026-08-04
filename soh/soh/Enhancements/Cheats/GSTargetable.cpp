#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"
#include <spdlog/spdlog.h>
#include <vector>

extern "C" {
#include "functions.h"
#include "macros.h"
#include "src/overlays/actors/ovl_En_Sw/z_en_sw.h"

extern PlayState* gPlayState;
}

static constexpr int32_t CVAR_GSTARGETABLE_DEFAULT = 0;
#define CVAR_GSTARGETABLE_NAME CVAR_CHEAT("GSTargetable")
#define CVAR_GSTARGETABLE_VALUE CVarGetInteger(CVAR_GSTARGETABLE_NAME, CVAR_GSTARGETABLE_DEFAULT)

static void OnActorInitGSTargetable(void* refActor) {
    EnSw* enSw = reinterpret_cast<EnSw*>(refActor);

    if (enSw->actor.naviEnemyId == 0x20) {
        // Enable Targeting this Gold Skulltula, if visible by default
        if (enSw->actor.scale.x >= 0.0139999995f) {
            enSw->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
        }

        // By default Gold Skulltulas are categorized as NPCs (blue cursor) which feels wrong.
        // Change the category to Misc (green cursor) instead.
        // It might be possible to change the category to Enemy but doing so will likely affect Clear Rooms.
        Actor_ChangeCategory(gPlayState, &gPlayState->actorCtx, &enSw->actor, ACTORCAT_MISC);
    }
}

static void OnEnemyDefeatGSTargetable(void* refActor) {
    EnSw* enSw = reinterpret_cast<EnSw*>(refActor);

    if (enSw->actor.naviEnemyId == 0x20) {
        // Disable Targeting immediately when the Gold Skulltula is defeated (like regular Skullwalltulas)
        enSw->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    }
}

static void OnActorUpdateGSTargetable(void* refActor) {
    EnSw* enSw = reinterpret_cast<EnSw*>(refActor);

    // Handle Night GS Spawning/Despawning
    if ((enSw->actor.naviEnemyId == 0x20) && (((enSw->actor.params & 0xE000) >> 0xD) == 2) &&
        (enSw->actor.colChkInfo.health > 0)) {
        if (enSw->actor.scale.x < 0.0139999995f) {
            // Night GS Despawn
            enSw->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        }
        if (enSw->actor.scale.x >= 0.0139999995f) {
            // Night GS Spawn
            enSw->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
        }
    }
}

static void UpdateGSTargetable() {
    if (gPlayState != nullptr) {
        if (CVAR_GSTARGETABLE_VALUE) {
            SPDLOG_DEBUG("GSTargetable has been toggled on");

            // Find all Gold Skulltulas that are in NPC category
            std::vector<Actor*> goldSkulltulasInNPCCategory;

            Actor* actorNPC = gPlayState->actorCtx.actorLists[ACTORCAT_NPC].head;
            while (actorNPC != nullptr) {
                if ((actorNPC->id == ACTOR_EN_SW) && (actorNPC->naviEnemyId == 0x20)) {
                    goldSkulltulasInNPCCategory.push_back(actorNPC);
                }
                actorNPC = actorNPC->next;
            }

            // Move all NPC Gold Skulltulas to Misc category
            for (auto& actor : goldSkulltulasInNPCCategory) {
                Actor_ChangeCategory(gPlayState, &gPlayState->actorCtx, actor, ACTORCAT_MISC);
            }

            // Make all Gold Skulltulas in Misc category targetable, if visible
            Actor* actorMisc = gPlayState->actorCtx.actorLists[ACTORCAT_MISC].head;
            while (actorMisc != nullptr) {
                if ((actorMisc->id == ACTOR_EN_SW) && (actorMisc->naviEnemyId == 0x20) &&
                    (actorMisc->scale.x >= 0.0139999995f)) {
                    actorMisc->flags |= ACTOR_FLAG_ATTENTION_ENABLED;
                }
                actorMisc = actorMisc->next;
            }
        } else {
            SPDLOG_DEBUG("GSTargetable has been toggled off");

            // Make all Gold Skulltulas in NPC category not targetable
            Actor* actorNPC = gPlayState->actorCtx.actorLists[ACTORCAT_NPC].head;
            while (actorNPC != nullptr) {
                if ((actorNPC->id == ACTOR_EN_SW) && (actorNPC->naviEnemyId == 0x20)) {
                    actorNPC->flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
                }
                actorNPC = actorNPC->next;
            }

            // Make all Gold Skulltulas in Misc category not targetable
            Actor* actorMisc = gPlayState->actorCtx.actorLists[ACTORCAT_MISC].head;
            while (actorMisc != nullptr) {
                if ((actorMisc->id == ACTOR_EN_SW) && (actorMisc->naviEnemyId == 0x20)) {
                    actorMisc->flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
                }
                actorMisc = actorMisc->next;
            }
        }
    }
}

static void RegisterGSTargetable() {
    UpdateGSTargetable();

    COND_ID_HOOK(OnActorInit, ACTOR_EN_SW, CVAR_GSTARGETABLE_VALUE, OnActorInitGSTargetable);
    COND_ID_HOOK(OnEnemyDefeat, ACTOR_EN_SW, CVAR_GSTARGETABLE_VALUE, OnEnemyDefeatGSTargetable);
    COND_ID_HOOK(OnActorUpdate, ACTOR_EN_SW, CVAR_GSTARGETABLE_VALUE, OnActorUpdateGSTargetable);
}

static RegisterShipInitFunc initFunc(RegisterGSTargetable, { CVAR_GSTARGETABLE_NAME });
