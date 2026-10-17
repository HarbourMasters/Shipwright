#include "CheckTrackerKnown.h"

#include <cmath>
#include <unordered_map>
#include <unordered_set>

#include <libultraship/bridge/consolevariablebridge.h>

#include "soh/OTRGlobals.h"
#include "soh/SaveManager.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/randomizer/randomizer.h"
#include "soh/Enhancements/randomizer/static_data.h"

extern "C" {
#include "functions.h"
#include "macros.h"
#include "variables.h"
#include "overlays/actors/ovl_En_Ex_Item/z_en_ex_item.h"
#include "overlays/actors/ovl_En_G_Switch/z_en_g_switch.h"
#include "overlays/actors/ovl_En_Si/z_en_si.h"
#include "overlays/actors/ovl_Item_B_Heart/z_item_b_heart.h"
#include "overlays/actors/ovl_Item_Etcetera/z_item_etcetera.h"
extern PlayState* gPlayState;
float OTRGetAspectRatio(void);
void EnGSwitch_RandomizerDraw(Actor* thisx, PlayState* play);
}

namespace CheckTracker {

// Consecutive gameplay frames an item must pass every test before it is marked.
static constexpr int kKnownFramesRequired = 8;
// Smallest projected item radius, in native 240-line screen lines, that counts as readable.
static constexpr float kKnownMinScreenLines = 8.0f;
// The four edge rays aim at this fraction of the item's radius from its center.
static constexpr float kKnownRayOffsetFraction = 0.8f;
// Approximate drawn radius of a shuffled item model.
static constexpr float kKnownItemRadius = 15.0f;
// Heart containers draw larger than other items.
static constexpr float kKnownHeartContainerRadius = 25.0f;
// EnItem00 drops rest on the ground, so their center sits above the actor position.
static constexpr float kKnownGroundedYOffset = 15.0f;

struct KnownCandidate {
    RandomizerCheck rc = RC_UNKNOWN_CHECK;
    GetItemEntry drawnEntry = {};
    float radius = kKnownItemRadius;
    float yOffset = 0.0f;
};

// Keyed by check, not actor, so nothing dangles across frames and duplicate actors count once.
static std::unordered_map<RandomizerCheck, int> knownFrames;
static int16_t lastSceneNum = -1;

static bool KnownTypeAllowed(RandomizerCheck rc) {
    switch (Rando::StaticData::GetLocation(rc)->GetRCType()) {
        case RCTYPE_STANDARD:
        case RCTYPE_SKULL_TOKEN:
        case RCTYPE_BOSS_HEART_OR_OTHER_REWARD:
        case RCTYPE_FREESTANDING:
        case RCTYPE_SILVER:
        case RCTYPE_POT:
        case RCTYPE_CRATE:
        case RCTYPE_NLCRATE:
        case RCTYPE_SMALL_CRATE:
        case RCTYPE_ROCK:
        case RCTYPE_BOULDER:
        case RCTYPE_TREE:
        case RCTYPE_NLTREE:
        case RCTYPE_BUSH:
        case RCTYPE_BEEHIVE:
        case RCTYPE_WONDER_ITEM:
            return true;
        default:
            return false;
    }
}

// Resolves an actor's check and the item entry its draw function renders.
static bool IdentifyCandidate(Actor* actor, KnownCandidate& candidate) {
    switch (actor->id) {
        case ACTOR_EN_ITEM00: {
            EnItem00* item00 = reinterpret_cast<EnItem00*>(actor);
            if (actor->params != ITEM00_SOH_DUMMY) {
                return false;
            }
            if (item00->randoCheck != RC_UNKNOWN_CHECK) {
                candidate.rc = item00->randoCheck;
            } else if (item00->randoInf != RAND_INF_MAX) {
                candidate.rc = OTRGlobals::Instance->gRandomizer->GetCheckFromRandomizerInf(item00->randoInf);
            } else {
                // Heart piece and small key drops only stamp the entry, not the check.
                candidate.rc = OTRGlobals::Instance->gRandomizer->GetCheckFromActor(actor->id, gPlayState->sceneNum,
                                                                                    item00->ogParams);
            }
            candidate.drawnEntry = item00->itemEntry;
            candidate.yOffset = kKnownGroundedYOffset;
            break;
        }
        case ACTOR_EN_SI: {
            candidate.rc =
                OTRGlobals::Instance->gRandomizer->GetCheckFromActor(actor->id, gPlayState->sceneNum, actor->params);
            candidate.drawnEntry = reinterpret_cast<EnSi*>(actor)->sohGetItemEntry;
            break;
        }
        case ACTOR_ITEM_B_HEART: {
            candidate.rc =
                OTRGlobals::Instance->gRandomizer->GetCheckFromActor(actor->id, gPlayState->sceneNum, actor->params);
            candidate.drawnEntry = reinterpret_cast<ItemBHeart*>(actor)->sohItemEntry;
            candidate.radius = kKnownHeartContainerRadius;
            break;
        }
        case ACTOR_ITEM_ETCETERA: {
            // Excludes the chest-game rupees: they draw through the Lens of Truth.
            int32_t type = actor->params & 0xFF;
            if (type != ITEM_ETC_LETTER && type != ITEM_ETC_ARROW_FIRE) {
                return false;
            }
            candidate.rc =
                OTRGlobals::Instance->gRandomizer->GetCheckFromActor(actor->id, gPlayState->sceneNum, actor->params);
            candidate.drawnEntry = reinterpret_cast<ItemEtcetera*>(actor)->sohItemEntry;
            break;
        }
        case ACTOR_EN_EX_ITEM: {
            // Only the prizes on display at the Bombchu Bowling counter; the other variants fly straight to Link.
            EnExItem* exItem = reinterpret_cast<EnExItem*>(actor);
            switch (exItem->type) {
                case EXITEM_BOMB_BAG_COUNTER:
                    candidate.rc = RC_MARKET_BOMBCHU_BOWLING_FIRST_PRIZE;
                    break;
                case EXITEM_HEART_PIECE_COUNTER:
                    candidate.rc = RC_MARKET_BOMBCHU_BOWLING_SECOND_PRIZE;
                    break;
                default:
                    return false;
            }
            candidate.drawnEntry = exItem->sohItemEntry;
            break;
        }
        case ACTOR_EN_G_SWITCH: {
            // The randomized draw is only installed on shuffled, uncollected silver rupees.
            EnGSwitch* silver = reinterpret_cast<EnGSwitch*>(actor);
            if (silver->type != ENGSWITCH_SILVER_RUPEE || actor->draw != EnGSwitch_RandomizerDraw) {
                return false;
            }
            candidate.rc = OTRGlobals::Instance->gRandomizer->IdentifySilver(gPlayState->sceneNum, actor->world.pos)
                               .randomizerCheck;
            if (candidate.rc == RC_UNKNOWN_CHECK) {
                return false;
            }
            candidate.drawnEntry = OTRGlobals::Instance->gRandoContext->GetFinalGIEntry(candidate.rc, true, GI_NONE);
            break;
        }
        default:
            return false;
    }
    return candidate.rc != RC_UNKNOWN_CHECK;
}

static bool PassesFilter(const KnownCandidate& candidate) {
    if (!KnownTypeAllowed(candidate.rc) ||
        OTRGlobals::Instance->gRandoContext->GetItemLocation(candidate.rc)->GetCheckStatus() != RCSHOW_UNCHECKED) {
        return false;
    }
    // Unobtainable items draw as a blue rupee; only mark when the drawn model matches the truth (disguises match
    // both sides).
    GetItemEntry truth = OTRGlobals::Instance->gRandoContext->GetFinalGIEntry(candidate.rc, false, GI_NONE);
    return candidate.drawnEntry.modIndex == truth.modIndex && candidate.drawnEntry.getItemId == truth.getItemId;
}

static bool InSight(Actor* actor, const KnownCandidate& candidate) {
    // projectedPos and projectedW are clip coordinates from Actor_DrawAll, earlier this frame.
    float w = actor->projectedW;
    if (w <= 0.0f) {
        return false;
    }
    // The renderer scales clip x by (4:3 / aspect), so a wider window shows more to each side.
    float xBound = w * OTRGetAspectRatio() / (4.0f / 3.0f);
    if (std::fabs(actor->projectedPos.x) > xBound || std::fabs(actor->projectedPos.y) > w) {
        return false;
    }
    float screenLines =
        candidate.radius / (w * std::tan(DEG_TO_RAD(gPlayState->view.fovy) / 2.0f)) * (SCREEN_HEIGHT / 2);
    if (screenLines < kKnownMinScreenLines) {
        return false;
    }
    // Play_SetFog passes (fogNear, 1000) to Gfx_SetFog: below 0 everything is fogged, from 1000 nothing is, and
    // otherwise gSPFogPosition starts the fog at NDC depth (fogNear - 500) / 500.
    s16 fogNear = gPlayState->lightCtx.fogNear;
    if (fogNear < 0 || (fogNear < 1000 && actor->projectedPos.z / w >= (fogNear - 500) / 500.0f)) {
        return false;
    }
    return actor->room == gPlayState->roomCtx.curRoom.num || actor->room < 0;
}

static bool RayClear(const Vec3f& eye, const Vec3f& target, Actor* item) {
    Vec3f from = eye;
    Vec3f to = target;
    Vec3f hitPos;
    CollisionPoly* poly;
    s32 bgId;
    return !BgCheck_AnyLineTest3(&gPlayState->colCtx, &from, &to, &hitPos, &poly, true, true, true, true, &bgId);
}

static Vec3f Cross(const Vec3f& a, const Vec3f& b) {
    return { a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x };
}

static bool Normalize(Vec3f& v) {
    float length = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    if (length < 0.001f) {
        return false;
    }
    v = { v.x / length, v.y / length, v.z / length };
    return true;
}

// Casts to the item center and four points around it, square to the view direction.
static bool RaysClear(Actor* actor, const KnownCandidate& candidate) {
    const View& view = gPlayState->view;
    Vec3f forward = { view.lookAt.x - view.eye.x, view.lookAt.y - view.eye.y, view.lookAt.z - view.eye.z };
    Vec3f right = Cross(forward, view.up);
    Vec3f up = Cross(right, forward);
    if (!Normalize(right) || !Normalize(up)) {
        return false;
    }
    Vec3f center = actor->world.pos;
    center.y += candidate.yOffset;
    float offset = candidate.radius * kKnownRayOffsetFraction;
    const Vec3f targets[] = {
        center,
        { center.x + right.x * offset, center.y + right.y * offset, center.z + right.z * offset },
        { center.x - right.x * offset, center.y - right.y * offset, center.z - right.z * offset },
        { center.x + up.x * offset, center.y + up.y * offset, center.z + up.z * offset },
        { center.x - up.x * offset, center.y - up.y * offset, center.z - up.z * offset },
    };
    for (const Vec3f& target : targets) {
        if (!RayClear(view.eye, target, actor)) {
            return false;
        }
    }
    return true;
}

void CheckTrackerKnownFrame(int saveSectionId) {
    if (gPlayState == nullptr || !GameInteractor::IsSaveLoaded() || !IS_RANDO || IS_BOSS_RUSH ||
        !CVarGetInteger(CVAR_TRACKER_CHECK("KnownItems"), 0) ||
        CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("MysteriousShuffle"), 0)) {
        knownFrames.clear();
        return;
    }
    if (gPlayState->sceneNum != lastSceneNum) {
        lastSceneNum = gPlayState->sceneNum;
        knownFrames.clear();
    }
    // Play_Draw skips Actor_DrawAll while paused, so isDrawn and projectedPos are stale.
    if (gPlayState->pauseCtx.state != 0 || gDbgCamEnabled || CVarGetInteger(CVAR_CHEAT("NoClip"), 0)) {
        knownFrames.clear();
        return;
    }

    std::unordered_set<RandomizerCheck> passing;
    for (int i = 0; i < ACTORCAT_MAX; i++) {
        for (Actor* actor = gPlayState->actorCtx.actorLists[i].head; actor != nullptr; actor = actor->next) {
            if (!actor->isDrawn) {
                continue;
            }
            KnownCandidate candidate;
            if (!IdentifyCandidate(actor, candidate) || passing.contains(candidate.rc) || !PassesFilter(candidate) ||
                !InSight(actor, candidate) || !RaysClear(actor, candidate)) {
                continue;
            }
            passing.insert(candidate.rc);
        }
    }

    // A check missing from this frame's passing set starts over.
    std::unordered_map<RandomizerCheck, int> counts;
    bool anyMarked = false;
    for (RandomizerCheck rc : passing) {
        auto it = knownFrames.find(rc);
        int frames = (it == knownFrames.end() ? 0 : it->second) + 1;
        if (frames >= kKnownFramesRequired) {
            OTRGlobals::Instance->gRandoContext->GetItemLocation(rc)->SetCheckStatus(RCSHOW_KNOWN);
            anyMarked = true;
        } else {
            counts[rc] = frames;
        }
    }
    knownFrames.swap(counts);
    if (anyMarked) {
        SaveManager::Instance->SaveSection(gSaveContext.fileNum, saveSectionId, true);
    }
}

void CheckTrackerKnownTeardown() {
    knownFrames.clear();
    lastSceneNum = -1;
}

} // namespace CheckTracker
