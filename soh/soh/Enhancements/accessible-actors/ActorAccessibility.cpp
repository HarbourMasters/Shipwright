#include <sstream>
#include <unordered_set>
#include <map>
#include <vector>

#include "ActorAccessibility.h"
#include "AccessibleAudioEngine.h"

#include <functions.h>
#include <variables.h>
#include <macros.h>
#include "SfxExtractor.h"
#include "AccessibleSfxCodec.h"

#include <atomic>
#include <memory>

#include <libultraship/controller/controldeck/ControlDeck.h>
#include "ship/resource/File.h"
#include "soh/Enhancements/speechsynthesizer/SpeechSynthesizer.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ObjectExtension/ObjectExtension.h"

extern "C" {
#include "overlays/actors/ovl_Shot_Sun/z_shot_sun.h"
extern PlayState* gPlayState;
extern bool freezeGame;
extern bool freezeActors;
}

const char* GetLanguageCode();

// This is the amount in DB that a sound will be reduced by when it is at the maximum distance from the player.
#define MAX_DB_REDUCTION 35

extern "C" {
// Used to tell where polygons are located.
void CollisionPoly_GetVertices(CollisionPoly* poly, Vec3s* vtxList, Vec3f* dest);
}

typedef struct {
    union {
        struct {
            s16 sceneIndex; // Corresponds directly to the game's scene indices.
            s16 roomIndex;  // Corresponds directly to the game's room indices.
        } values;
        s32 raw; // Combination of the two which can be used for dictionary lookups.
    };
} SceneAndRoom;

// Maps actors to their accessibility policies, which describe how accessibility should treat them.
typedef std::map<s16, ActorAccessibilityPolicy> SupportedActors_t;
typedef std::map<uint64_t, AccessibleActor> AccessibleActorList_t;
typedef std::vector<AccessibleActor> VAList_t; // Denotes a list of virtual actors specific to a single room.
typedef std::map<s32, VAList_t> VAZones_t; // Maps room/scene indices to their corresponding virtual actor collections.
// A list of scenes which have already been visited (since the game was launched). Used to prevent
// re-creation of terrain VAs every time the player reloads a scene.
typedef std::unordered_set<s16> SceneList_t;

enum SfxDecodeState {
    SFX_DECODE_PENDING,
    SFX_DECODE_READY,
    SFX_DECODE_FAILED,
};

struct SfxRecord {
    std::string path;
    std::shared_ptr<Ship::File> resource;
    // Decoded by the worker thread; handed to the audio engine by the game thread once ready.
    std::vector<int16_t> pcm;
    std::atomic<int> state{ SFX_DECODE_PENDING };
    bool registered = false;
};

struct A11yID {
    uint64_t id;
};

class AudioGlossaryData {
  public:
    AccessibleActorList_t accessibleActorList;
    AccessibleActorList_t::iterator current = accessibleActorList.begin();
    bool GlossaryStarted = false;
    u16 frameCount = 0;
    s16 currentScene = -1;
    s8 currentRoom = -1;
    s8 cooldown = 0;
};

class ActorAccessibility {
  public:
    bool isOn = false;
    uint64_t nextActorID = 0;
    SupportedActors_t supportedActors;
    AccessibleActorList_t accessibleActorList;
    AudioGlossaryData* glossary;
    VAZones_t vaZones;
    SceneList_t sceneList;
    AccessibleAudioEngine* audioEngine;
    SfxExtractor sfxExtractor;
    AccessibleSfxDecoder* sfxDecoder = nullptr;
    // Maps internal sfx to external (prerendered) resources. Held by pointer so the decoded
    // buffer the audio engine points at never moves.
    std::unordered_map<s16, std::unique_ptr<SfxRecord>> sfxMap;
    s16 currentScene = -1;
    s8 currentRoom = -1;
    bool currentRoomClear = false;
    u8 framesUntilChime = 0;
    Vec3f prevPos = { 0, 0, 0 };
    s16 prevYaw = 0;
    bool extractSfx = false;
    TerrainCueState* terrainCues = nullptr;
    VirtualActorList* currentSceneGlobal = nullptr;
    VirtualActorList* currentRoomLocal = nullptr;
};
static ActorAccessibility* aa;

uint64_t ActorAccessibility_GetNextID() {
    return aa->nextActorID++;
}

void ActorAccessibility_PrepareNextAudioFrame();

// Hooks for game-interactor.
void ActorAccessibility_OnActorInit(void* actor) {
    ActorAccessibility_TrackNewActor((Actor*)actor);
}
void ActorAccessibility_OnGameFrameUpdate() {
    if (gPlayState == NULL)
        return;
    if (!GameInteractor::IsSaveLoaded() && !aa->extractSfx)
        return; // Title screen, skip.

    ActorAccessibility_RunAccessibilityForAllActors(gPlayState);
}

void ActorAccessibility_OnActorDestroy(void* actor) {
    ActorAccessibility_RemoveTrackedActor((Actor*)actor);
}

void ActorAccessibility_OnGameStillFrozen() {
    if (gPlayState == NULL)
        return;
    if (aa->extractSfx)
        ActorAccessibility_HandleSoundExtractionMode(gPlayState);
}

void ActorAccessibility_Init() {
    aa = new ActorAccessibility();
    aa->glossary = new AudioGlossaryData();
    aa->isOn = CVarGetInteger(CVAR_SETTING("A11yAudioInteraction"), 0);
    if (!aa->isOn)
        return;
    // An archive from an older build holds raw PCM under .wav names, which nothing can read now.
    // Treat it as missing so the extractor runs and tells the user to delete it.
    auto formatFile =
        Ship::Context::GetRawInstance()->GetResourceManager()->GetArchiveManager()->LoadFile(A11Y_SFX_FORMAT_PATH);
    std::string format;
    if (formatFile != nullptr) {
        format.assign(formatFile->Buffer->begin(), formatFile->Buffer->end());
    }
    aa->extractSfx = format != A11Y_SFX_FORMAT_VERSION;
    if (aa->extractSfx)
        freezeGame = true;
    ActorAccessibility_InitAudio();
    ActorAccessibility_InitActors();
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnActorInit>(ActorAccessibility_OnActorInit);
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnActorDestroy>(ActorAccessibility_OnActorDestroy);

    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnPlayerUpdate>(ActorAccessibility_OnGameFrameUpdate);
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnGameStillFrozen>(ActorAccessibility_OnGameStillFrozen);
}

void ActorAccessibility_Shutdown() {
    ActorAccessibility_ShutdownAudio();
    delete aa;
}

void ActorAccessibility_InitPolicy(ActorAccessibilityPolicy* policy, const char* englishName) {
    policy->distance = 500;
    policy->ydist = 80;
    policy->englishName = englishName;
    policy->n = 20;
    policy->pitch = 1.5;
    policy->runsAlways = false;
    policy->volume = 1.0;
    policy->pitchModifier = 0.1;
    policy->aimAssist.isProvider = 0;
    policy->aimAssist.sfx = NA_SE_SY_HITPOINT_ALARM;
    policy->aimAssist.tolerance = 0.0;
}

void ActorAccessibility_InitPolicy(ActorAccessibilityPolicy* policy, const char* englishName,
                                   ActorAccessibilityCallback callback) {
    policy->callback = callback;
    policy->sound = 0;
    ActorAccessibility_InitPolicy(policy, englishName);
}

void ActorAccessibility_InitPolicy(ActorAccessibilityPolicy* policy, const char* englishName, s16 sfx) {
    policy->callback = nullptr;
    policy->sound = sfx;
    ActorAccessibility_InitPolicy(policy, englishName);
}

void ActorAccessibility_AddSupportedActor(s16 type, ActorAccessibilityPolicy policy) {
    aa->supportedActors[type] = policy;
}

void ActorAccessibility_AddTerrainCues(AccessibleActor* actor) {
    aa->terrainCues = InitTerrainCueState(actor);
}

ActorAccessibilityPolicy* ActorAccessibility_GetPolicyForActor(s16 type) {
    SupportedActors_t::iterator i = aa->supportedActors.find(type);
    if (i == aa->supportedActors.end())
        return NULL;
    return &i->second;
}

int ActorAccessibility_GetRandomStartingFrameCount(int min, int max) {
    return min + Rand_ZeroOne() * (max - min);
}

void ActorAccessibility_TrackNewActor(Actor* actor) {
    // Don't track actors for which no accessibility policy has been configured.
    ActorAccessibilityPolicy* policy = ActorAccessibility_GetPolicyForActor(actor->id);
    if (policy == NULL)
        return;
    AccessibleActor accessibleActor;
    accessibleActor.instanceID = ActorAccessibility_GetNextID();
    accessibleActor.actor = actor;
    accessibleActor.id = actor->id;
    // Stagger the start times so that all of the sounds don't play at exactly the same time.
    accessibleActor.frameCount = ActorAccessibility_GetRandomStartingFrameCount(0, policy->n);
    accessibleActor.basePitch = policy->pitch;
    accessibleActor.policy = *policy;
    accessibleActor.currentPitch = accessibleActor.policy.pitch;
    accessibleActor.baseVolume = accessibleActor.policy.volume;
    accessibleActor.currentVolume = accessibleActor.policy.volume;
    accessibleActor.sceneIndex = 0;
    accessibleActor.managedSoundSlots = 0;
    accessibleActor.aimFramesSinceAimAssist = 255;
    accessibleActor.aimFrequency = 10;

    ObjectExtension::GetInstance().Set<A11yID>(actor, A11yID{ .id = accessibleActor.instanceID });
    aa->accessibleActorList[accessibleActor.instanceID] = accessibleActor;
}

void ActorAccessibility_RemoveTrackedActor(Actor* actor) {
    const auto id = ObjectExtension::GetInstance().Get<A11yID>(actor);
    if (id == nullptr)
        return;
    AccessibleActorList_t::iterator i2 = aa->accessibleActorList.find(id->id);
    if (i2 == aa->accessibleActorList.end())
        return;
    ActorAccessibility_StopAllSoundsForActor(&i2->second);
    aa->accessibleActorList.erase(i2);
}

f32 ActorAccessibility_DBToLinear(float gain) {
    return powf(10.0, gain / 20.0f);
}

f32 ActorAccessibility_ComputeCurrentVolume(f32 maxDistance, f32 xzDistToPlayer) {
    if (maxDistance == 0)
        return 0.0;
    f32 absDistance = fabs(xzDistToPlayer);
    f32 db = LERP(0.0 - MAX_DB_REDUCTION, 0.0, (maxDistance - absDistance) / maxDistance);

    return ActorAccessibility_DBToLinear(db);
}

const char* ActorAccessibility_MapSfxToExternalAudio(s16 sfxId);
void ActorAccessibility_PlaySound(void* handle, int slot, s16 sfxId) {
    const char* path = ActorAccessibility_MapSfxToExternalAudio(sfxId);
    if (path == NULL)
        return;
    aa->audioEngine->playSound((uintptr_t)handle, slot, path);
}

void ActorAccessibility_StopSound(void* handle, int slot) {
    aa->audioEngine->stopSound((uintptr_t)handle, slot);
}
void ActorAccessibility_StopAllSounds(void* handle) {
    aa->audioEngine->stopAllSounds((uintptr_t)handle);
}
void ActorAccessibility_SetSoundPitch(void* handle, int slot, float pitch) {
    aa->audioEngine->setPitch((uintptr_t)handle, slot, pitch);
}
void ActorAccessibility_SetPitchBehindModifier(void* handle, int slot, float mod) {
    aa->audioEngine->setPitchBehindModifier((uintptr_t)handle, slot, mod);
}
void ActorAccessibility_SetSoundPos(void* handle, int slot, Vec3f* pos, f32 distToPlayer, f32 maxDistance) {
    aa->audioEngine->setSoundPosition((uintptr_t)handle, slot, pos->x, pos->y, pos->z, distToPlayer, maxDistance);
}
void ActorAccessibility_SetSoundVolume(void* handle, int slot, float volume) {
    aa->audioEngine->setVolume((uintptr_t)handle, slot, volume);
}
void ActorAccessibility_SetSoundPan(void* handle, int slot, float pan) {
    aa->audioEngine->setPan((uintptr_t)handle, slot, pan);
}
void ActorAccessibility_SetSoundFilter(void* handle, int slot, float cutoff) {
    aa->audioEngine->setFilter((uintptr_t)handle, slot, cutoff);
}
void ActorAccessibility_SeekSound(void* handle, int slot, size_t offset) {
    aa->audioEngine->seekSound((uintptr_t)handle, slot, offset);
}

void ActorAccessibility_ConfigureSoundForActor(AccessibleActor* actor, int slot) {
    ActorAccessibility_SetSoundPitch(actor, slot, actor->policy.pitch);
    ActorAccessibility_SetPitchBehindModifier(actor, slot, actor->policy.pitchModifier);
    ActorAccessibility_SetSoundPos(actor, slot, &actor->projectedPos, actor->xyzDistToPlayer, actor->policy.distance);
    ActorAccessibility_SetSoundVolume(actor, slot, actor->policy.volume);
    actor->managedSoundSlots |= 1 << slot;
}

void ActorAccessibility_PlaySoundForActor(AccessibleActor* actor, int slot, s16 sfxId) {
    if (slot < 0 || slot >= AAE_SLOTS_PER_HANDLE)
        return;
    ActorAccessibility_PlaySound(actor, slot, sfxId);
    ActorAccessibility_ConfigureSoundForActor(actor, slot);
}

void ActorAccessibility_StopSoundForActor(AccessibleActor* actor, int slot) {
    if (slot < 0 || slot >= AAE_SLOTS_PER_HANDLE)
        return;
    ActorAccessibility_StopSound(actor, slot);
    actor->managedSoundSlots &= ~(1 << slot);
}

void ActorAccessibility_StopAllSoundsForActor(AccessibleActor* actor) {
    ActorAccessibility_StopAllSounds(actor);
    actor->managedSoundSlots = 0;
}

void ActorAccessibility_CopyParamsFromRealActor(AccessibleActor* actor) {
    Player* player = GET_PLAYER(actor->play);
    if (actor->actor == NULL)
        return;
    actor->projectedPos = actor->actor->projectedPos;
    actor->xzDistToPlayer = actor->actor->xzDistToPlayer;
    actor->isDrawn = actor->actor->isDrawn;
    actor->pos = actor->actor->world.pos;
    actor->xyzDistToPlayer = sqrtf(actor->actor->xyzDistToPlayerSq);
}

void ActorAccessibility_StopAllVirtualActors(VirtualActorList* list) {
    if (list == NULL)
        return;

    VAList_t* val = (VAList_t*)list;
    for (auto i = val->begin(); i != val->end(); i++)
        ActorAccessibility_StopAllSounds((void*)&(*i));
}

void ActorAccessibility_RunAccessibilityForActor(PlayState* play, AccessibleActor* actor) {
    actor->play = play;
    if (actor->actor != nullptr) {
        ActorAccessibility_CopyParamsFromRealActor(actor);
    } else {
        Player* player = GET_PLAYER(play);
        f32 w = 0.0f;
        // Set actor->projectedPos.
        SkinMatrix_Vec3fMtxFMultXYZW(&play->viewProjectionMtxF, &actor->pos, &actor->projectedPos, &w);
        actor->xzDistToPlayer = Math_Vec3f_DistXZ(&actor->pos, &player->actor.world.pos);
        actor->xyzDistToPlayer = Math_Vec3f_DistXYZ(&actor->pos, &player->actor.world.pos);
        actor->yDistToPlayer = fabs((actor->pos.y) - (player->actor.world.pos.y));
    }

    if (actor->policy.aimAssist.isProvider) {
        Player* player = GET_PLAYER(play);
        if ((player->stateFlags1 & PLAYER_STATE1_FIRST_PERSON) &&
            ((actor->policy.aimAssist.isProvider & AIM_CUP) ||
             (player->stateFlags1 & (PLAYER_STATE1_USING_BOOMERANG | PLAYER_STATE1_ITEM_IN_HAND)))) {
            bool aim = false;
            f32 dist = 1000;
            if (player->unk_6AD == 2) {
                switch (player->heldItemAction) {
                    case PLAYER_IA_BOW:
                    case PLAYER_IA_BOW_FIRE:
                    case PLAYER_IA_BOW_ICE:
                    case PLAYER_IA_BOW_LIGHT:
                    case PLAYER_IA_BOW_0C:
                    case PLAYER_IA_BOW_0D:
                    case PLAYER_IA_BOW_0E:
                        aim = actor->policy.aimAssist.isProvider & AIM_BOW;
                        break;
                    case PLAYER_IA_SLINGSHOT:
                        aim = actor->policy.aimAssist.isProvider & AIM_SLING;
                        break;
                    case PLAYER_IA_HOOKSHOT:
                        dist = 380;
                        aim = actor->policy.aimAssist.isProvider & AIM_HOOK;
                        break;
                    case PLAYER_IA_LONGSHOT:
                        dist = 770;
                        aim = actor->policy.aimAssist.isProvider & AIM_HOOK;
                        break;
                    case PLAYER_IA_BOOMERANG:
                        dist = 380;
                        aim = actor->policy.aimAssist.isProvider & AIM_BOOM;
                        break;
                    case PLAYER_IA_NONE:
                        aim = actor->policy.aimAssist.isProvider & AIM_CUP;
                        break;
                }
            } else {
                aim = actor->policy.aimAssist.isProvider & AIM_CUP;
            }
            if (aim && actor->xyzDistToPlayer < dist) {
                auto aimAssistProps = ActorAccessibility_ProvideAimAssistForActor(actor);
                if (++actor->aimFramesSinceAimAssist >= actor->aimFrequency) {
                    actor->aimFramesSinceAimAssist = 0;
                    ActorAccessibility_PlaySoundForActor(actor, 7, actor->policy.aimAssist.sfx);
                }
                ActorAccessibility_SetSoundPitch(actor, 7, aimAssistProps.pitch);
                ActorAccessibility_SetSoundVolume(actor, 7, aimAssistProps.volume);
                ActorAccessibility_SetSoundPan(actor, 7, aimAssistProps.pan);
            }
        } else {
            // Make sure there's no delay the next time you draw your bow or whatever.
            actor->aimFramesSinceAimAssist = 255;
        }
    }

    actor->frameCount++;
    if (aa->glossary->GlossaryStarted) {
        aa->glossary->frameCount++;
    }

    if (actor->actor != NULL && fabs(actor->actor->yDistToPlayer) > actor->policy.ydist) {
        return;
    } else if (!actor->policy.runsAlways && actor->xyzDistToPlayer > actor->policy.distance) {
        return;
    } else if (actor->isDrawn == 0 && actor->id != ACTOR_EN_HOLL && actor->id != ACTOR_EN_KAKASI2 &&
               actor->id != ACTOR_EN_IT && actor->id != ACTOR_EN_OKARINA_TAG && actor->id != ACTOR_EN_WONDER_ITEM &&
               actor->id != ACTOR_SHOT_SUN && !aa->glossary->GlossaryStarted) {
        return;
    }

    for (int i = 0; i < AAE_SLOTS_PER_HANDLE; i++) {
        if (actor->managedSoundSlots & (1 << i)) {
            ActorAccessibility_SetSoundPos(actor, i, &actor->projectedPos, i == 7 ? 0 : actor->xyzDistToPlayer,
                                           i == 7 ? 1 : actor->policy.distance);
            // Judgement call: pitch changes are rare enough that it doesn't make sense to pay the cost of updating it
            // every frame. If you want a pitch change, call the function as needed.
        }
    }

    if (actor->frameCount % actor->policy.n == 0) {
        if (actor->policy.callback != nullptr) {
            actor->policy.callback(actor);
        } else if (actor->policy.sound != 0) {
            ActorAccessibility_PlaySoundForActor(actor, 0, actor->policy.sound);
        }
    }
}

void ActorAccessibility_RunAccessibilityForAllActors(PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (aa->currentScene != play->sceneNum) {
        if (aa->terrainCues)
            ActorAccessibility_StopAllSounds(aa->terrainCues);
        ActorAccessibility_StopAllVirtualActors(aa->currentSceneGlobal);
        ActorAccessibility_StopAllVirtualActors(aa->currentRoomLocal);
        ActorAccessibility_InterpretCurrentScene(play);
        aa->currentSceneGlobal = ActorAccessibility_GetVirtualActorList(play->sceneNum, -1);
        aa->currentRoomLocal = ActorAccessibility_GetVirtualActorList(play->sceneNum, play->roomCtx.curRoom.num);

        aa->currentScene = play->sceneNum;
        aa->currentRoom = play->roomCtx.curRoom.num;
        aa->currentRoomClear = Flags_GetClear(play, aa->currentRoom);
    } else if (aa->currentRoom != play->roomCtx.curRoom.num) {
        ActorAccessibility_StopAllVirtualActors(aa->currentRoomLocal);
        ActorAccessibility_AnnounceRoomNumber(play);
        aa->currentRoomLocal = ActorAccessibility_GetVirtualActorList(play->sceneNum, play->roomCtx.curRoom.num);
        aa->currentRoom = play->roomCtx.curRoom.num;
        aa->currentRoomClear = Flags_GetClear(play, aa->currentRoom);
    }

    if (aa->glossary->currentScene != play->sceneNum || aa->glossary->currentRoom != play->roomCtx.curRoom.num) {
        if (aa->glossary->GlossaryStarted) {
            aa->glossary->cooldown = 0;
            aa->glossary->GlossaryStarted = false;
            freezeActors = false;
        }
    }
    if (player->stateFlags1 & PLAYER_STATE1_IN_CUTSCENE) {
        return;
    }
    ActorAccessibility_AudioGlossary(play);
    if (aa->glossary->GlossaryStarted) {
        return;
    }

    ActorAccessibility_GeneralHelper(play);

    // Real actors.
    for (AccessibleActorList_t::iterator i = aa->accessibleActorList.begin(); i != aa->accessibleActorList.end(); i++)
        ActorAccessibility_RunAccessibilityForActor(play, &i->second);

    if (aa->terrainCues) {
        RunTerrainCueState(aa->terrainCues, play);
    }

    // Virtual actors for the current room and scene.
    VAList_t* list = (VAList_t*)aa->currentRoomLocal;
    for (VAList_t::iterator i = list->begin(); i != list->end(); i++)
        ActorAccessibility_RunAccessibilityForActor(play, &(*i));
    // Scene-global virtual actors. Most of these are automatically generated VAs from polygons, because there's no way
    // to sort these into rooms.
    list = (VAList_t*)aa->currentSceneGlobal;
    for (VAList_t::iterator i = list->begin(); i != list->end(); i++)
        ActorAccessibility_RunAccessibilityForActor(play, &(*i));

    // Processes external audio engine.
    ActorAccessibility_PrepareNextAudioFrame();
}

void ActorAccessibility_GeneralHelper(PlayState* play) {
    Player* player = GET_PLAYER(play);
    if (player == nullptr)
        return;

    // Report when a room is completed.
    if (!aa->currentRoomClear && Flags_GetClear(play, aa->currentRoom)) {
        aa->currentRoomClear = Flags_GetClear(play, aa->currentRoom);
        ActorAccessibility_AnnounceRoomNumber(play);
    }

    if (player->actor.wallPoly && player->actor.speedXZ > 0 &&
        (player->yDistToLedge == 0 || player->yDistToLedge >= 79.0f)) {
        f32 movedsq = SQ(aa->prevPos.x - player->actor.world.pos.x) + SQ(aa->prevPos.z - player->actor.world.pos.z);
        if (movedsq < 0.125) {
            ActorAccessibility_PlaySound(nullptr, 3, NA_SE_IT_WALL_HIT_SOFT);
            ActorAccessibility_SetSoundVolume(nullptr, 3, 0.5);
        } else if (movedsq < 9) {
            ActorAccessibility_PlaySound(nullptr, 3, NA_SE_IT_SHIELD_POSTURE);
            ActorAccessibility_SetSoundVolume(nullptr, 3, 0.6);
        } else {
            ActorAccessibility_PlaySound(nullptr, 3, NA_SE_PL_WALK_WALL);
            ActorAccessibility_SetSoundVolume(nullptr, 3, std::max(0.3f, 10.0f / movedsq));
        }
    }

    bool compassOn = false;
    if (aa->prevPos.x == player->actor.world.pos.x && aa->prevPos.z == player->actor.world.pos.z) {
        if (ABS(aa->prevYaw - player->yaw) > 0x400) {
            compassOn = true;
            aa->prevYaw = player->yaw;
        }
    } else {
        aa->prevPos = player->actor.world.pos;
    }

    if (aa->framesUntilChime > 0) {
        aa->framesUntilChime--;
    } else {
        if (!compassOn) {
            OSContPad* trackerButtonsPressed =
                std::dynamic_pointer_cast<LUS::ControlDeck>(Ship::Context::GetRawInstance()->GetControlDeck())
                    ->GetPads();
            compassOn = trackerButtonsPressed != nullptr && (trackerButtonsPressed[0].button & BTN_DDOWN) &&
                        (trackerButtonsPressed[0].button & BTN_L);
        }
        if (compassOn) {
            ActorAccessibility_PlaySound(nullptr, 0, NA_SE_EV_SHIP_BELL);
            ActorAccessibility_SetSoundPitch(nullptr, 0, 1.5f + Math_CosS(player->yaw) / 2);
            ActorAccessibility_SetSoundPan(nullptr, 0, -Math_SinS(player->yaw));
            s16 range = ABS(((player->yaw + 0xA000) & 0x3FFF) - 0x2000);
            aa->framesUntilChime = range <= 0x400 ? 10 : range <= 0x1000 ? 20 : 30;
        }
    }

    if (fabs(player->unk_860 - 25) < 24.0 && player->heldItemId == 0) {
        ActorAccessibility_PlaySound(nullptr, 1, NA_SE_SY_WARNING_COUNT_N);
    }

    if (Player_HoldsHookshot(player) && player->heldActor != NULL && player->actor.scale.y >= 0.0f &&
        (player->stateFlags1 & PLAYER_STATE1_FIRST_PERSON)) {
        CollisionPoly* colPoly;
        s32 bgId;
        Vec3f firstHit;
        f32 hookshotLength = ((player->heldItemAction == PLAYER_IA_HOOKSHOT) ? 380.0f : 770.0f) *
                             CVarGetFloat(CVAR_CHEAT("HookshotReachMultiplier"), 1.0f);
        Vec3f hookshotEnd = player->heldActor->world.pos;
        hookshotEnd.x +=
            Math_SinS(player->heldActor->world.rot.y) * Math_SinS(-player->heldActor->world.rot.x) * hookshotLength;
        hookshotEnd.y += Math_SinS(-player->heldActor->world.rot.x) * hookshotLength;
        hookshotEnd.z +=
            Math_CosS(player->heldActor->world.rot.y) * Math_CosS(-player->heldActor->world.rot.x) * hookshotLength;
        if (BgCheck_AnyLineTest3(&play->colCtx, &player->heldActor->world.pos, &hookshotEnd, &firstHit, &colPoly, 1, 1,
                                 1, 1, &bgId)) {
            if (SurfaceType_IsHookshotSurface(&play->colCtx, colPoly, bgId)) {
                ActorAccessibility_PlaySound(nullptr, 2, NA_SE_IT_HOOKSHOT_STICK_OBJ);
                ActorAccessibility_SetSoundVolume(nullptr, 2, 0.5f);
            }
        }
    }
}

void ActorAccessibility_AudioGlossary(PlayState* play) {
    if (aa->glossary->GlossaryStarted) {
        freezeActors = true;
        AccessibleActor glossaryActor = (*aa->glossary->current).second;
        ActorAccessibility_CopyParamsFromRealActor(&glossaryActor);
        glossaryActor.policy.distance = glossaryActor.xzDistToPlayer * 3;
        glossaryActor.policy.ydist = 1000;
        glossaryActor.frameCount = aa->glossary->frameCount;
        ActorAccessibility_RunAccessibilityForActor(play, &glossaryActor);
    }
    if (aa->glossary->cooldown != 0) {
        aa->glossary->cooldown--;
        return;
    }

    OSContPad* trackerButtonsPressed =
        std::dynamic_pointer_cast<LUS::ControlDeck>(Ship::Context::GetRawInstance()->GetControlDeck())->GetPads();
    bool comboStartGlossary = trackerButtonsPressed != nullptr && trackerButtonsPressed[0].button & BTN_DUP &&
                              trackerButtonsPressed[0].button & BTN_L;
    if (comboStartGlossary) {
        aa->glossary->GlossaryStarted = true;
        aa->glossary->current = aa->accessibleActorList.begin();
        aa->glossary->currentScene = play->sceneNum;
        aa->glossary->currentRoom = play->roomCtx.curRoom.num;
        SpeechSynthesizer::Instance->Speak((*aa->glossary->current).second.policy.englishName, GetLanguageCode());
        return;
    }
    bool comboNextGlossary = trackerButtonsPressed != nullptr && trackerButtonsPressed[0].button & BTN_DRIGHT &&
                             trackerButtonsPressed[0].button & BTN_L;
    if (comboNextGlossary && aa->glossary->GlossaryStarted) {
        aa->glossary->current++;
        if (aa->glossary->current == aa->accessibleActorList.end()) {
            aa->glossary->current = aa->accessibleActorList.begin();
        };
        aa->glossary->cooldown = 5;
        SpeechSynthesizer::Instance->Speak((*aa->glossary->current).second.policy.englishName, GetLanguageCode());
    }
    bool comboPrevGlossary = trackerButtonsPressed != nullptr && trackerButtonsPressed[0].button & BTN_DLEFT &&
                             trackerButtonsPressed[0].button & BTN_L;
    if (comboPrevGlossary && aa->glossary->GlossaryStarted) {
        if (aa->glossary->current != aa->accessibleActorList.begin()) {
            aa->glossary->current--;
        }
        aa->glossary->cooldown = 5;

        SpeechSynthesizer::Instance->Speak((*aa->glossary->current).second.policy.englishName, GetLanguageCode());
    }
    bool comboDisableGlossary = trackerButtonsPressed != nullptr && trackerButtonsPressed[0].button & BTN_DDOWN &&
                                trackerButtonsPressed[0].button & BTN_L;
    if (comboDisableGlossary) {
        aa->glossary->cooldown = 0;
        aa->glossary->GlossaryStarted = false;
        freezeActors = false;
    }
    // Processes external audio engine.
    ActorAccessibility_PrepareNextAudioFrame();
}

// Virtual actor config.
VirtualActorList* ActorAccessibility_GetVirtualActorList(s16 sceneNum, s8 roomNum) {
    SceneAndRoom sr;
    sr.values.sceneIndex = sceneNum;
    sr.values.roomIndex = roomNum;

    return (VirtualActorList*)&aa->vaZones[sr.raw];
}

AccessibleActor* ActorAccessibility_AddVirtualActor(VirtualActorList* list, VIRTUAL_ACTOR_TABLE type, Vec3f where) {
    ActorAccessibilityPolicy* policy = ActorAccessibility_GetPolicyForActor(type);
    AccessibleActor actor;
    actor.actor = nullptr;
    actor.basePitch = 1.0;
    actor.baseVolume = 1.0;
    actor.currentPitch = 1.0;
    actor.currentVolume = 1.0;
    actor.frameCount = 0;
    actor.id = (s16)type;
    actor.instanceID = ActorAccessibility_GetNextID();
    actor.isDrawn = 1;
    actor.play = nullptr;
    actor.pos = where;
    actor.sceneIndex = 0;
    actor.managedSoundSlots = 0;
    actor.aimFramesSinceAimAssist = 0;
    actor.aimFrequency = 10;
    actor.policy = *policy;

    VAList_t* l = (VAList_t*)list;
    l->push_back(actor);
    return &(*l)[l->size() - 1];
}

void ActorAccessibility_InterpretCurrentScene(PlayState* play) {
    if (aa->sceneList.contains(play->sceneNum))
        return; // Scene interpretation already complete for this scene
    aa->sceneList.insert(play->sceneNum);
    VirtualActorList* list = ActorAccessibility_GetVirtualActorList(play->sceneNum, -1); // Scene-global VAs
    if (list == NULL)
        return;
    for (int i = 0; i < play->colCtx.colHeader->numPolygons; i++) {
        CollisionPoly* poly = &play->colCtx.colHeader->polyList[i];
        // checks if climable
        if ((SurfaceType_GetWallFlags(&play->colCtx, poly, BGCHECK_SCENE) == 8 ||
             SurfaceType_GetWallFlags(&play->colCtx, poly, BGCHECK_SCENE) == 3)) {
            ActorAccessibility_PolyToVirtualActor(play, poly, VA_CLIMB, list);
        } else if (SurfaceType_GetSceneExitIndex(&play->colCtx, poly, BGCHECK_SCENE) != 0) {
            ActorAccessibility_PolyToVirtualActor(play, poly, VA_AREA_CHANGE, list);
        } else if (func_80041EA4(&play->colCtx, poly, BGCHECK_SCENE) == 12) {
            ActorAccessibility_PolyToVirtualActor(play, poly, VA_VOID, list);
        } else if (SurfaceType_IsHookshotSurface(&play->colCtx, poly, BGCHECK_SCENE)) {
            ActorAccessibility_PolyToVirtualActor(play, poly, VA_MARKER, list);
        }
    }
}

// Convert poly to VA.
void ActorAccessibility_PolyToVirtualActor(PlayState* play, CollisionPoly* poly, VIRTUAL_ACTOR_TABLE va,
                                           VirtualActorList* destination) {
    Vec3f polyVerts[3];
    CollisionPoly_GetVertices(poly, play->colCtx.colHeader->vtxList, polyVerts);
    f32 minX = std::min(polyVerts[0].x, std::min(polyVerts[1].x, polyVerts[2].x));
    f32 maxX = std::max(polyVerts[0].x, std::max(polyVerts[1].x, polyVerts[2].x));
    f32 minZ = std::min(polyVerts[0].z, std::min(polyVerts[1].z, polyVerts[2].z));
    f32 maxZ = std::max(polyVerts[0].z, std::max(polyVerts[1].z, polyVerts[2].z));
    Vec3f where = {
        .x = maxX - (maxX - minX) / 2,
        .y = std::min(polyVerts[0].y, std::min(polyVerts[1].y, polyVerts[2].y)),
        .z = maxZ - (maxZ - minZ) / 2,
    };
    AccessibleActor* actor = ActorAccessibility_AddVirtualActor(destination, va, where);
    if (actor == NULL)
        return;
    if (va == VA_MARKER) {
        actor->policy.sound = 0;
    }

    if (va == VA_AREA_CHANGE) {
        if (play->sceneNum != SCENE_GROTTOS && play->sceneNum != SCENE_FAIRYS_FOUNTAIN) {
            u32 sceneIndex = SurfaceType_GetSceneExitIndex(&play->colCtx, poly, BGCHECK_SCENE);
            s16 nextEntranceIndex = play->setupExitList[sceneIndex - 1];
            actor->sceneIndex = gEntranceTable[nextEntranceIndex].scene;
        }
    } else if (SurfaceType_IsHookshotSurface(&play->colCtx, poly, BGCHECK_SCENE)) {
        if (va == VA_MARKER) {
            f32 maxY = std::max(polyVerts[0].y, std::max(polyVerts[1].y, polyVerts[2].y));
            actor->pos.y = (actor->pos.y + maxY) / 2;
        }
        actor->policy.aimAssist.isProvider = AIM_HOOK;
    }
}

void ActorAccessibility_AnnounceRoomNumber(PlayState* play) {
    std::stringstream ss;
    ss << "Room" << (int)play->roomCtx.curRoom.num;
    if (Flags_GetClear(play, play->roomCtx.curRoom.num))
        ss << " completed" << std::endl;
    SpeechSynthesizer::Instance->Speak(ss.str().c_str(), GetLanguageCode());
}

AimAssistProps ActorAccessibility_ProvideAimAssistForActor(AccessibleActor* actor) {
    Player* player = GET_PLAYER(actor->play);
    s32 angle = player->actor.focus.rot.x;
    angle = angle / -14000.0 * 16384;
    f32 cos_angle = Math_CosS(angle);
    f32 slope = cos_angle == 0.0f ? 0.0f : Math_SinS(angle) / cos_angle;
    f32 x = actor->pos.x, z = actor->pos.z, xzDist = actor->xzDistToPlayer;
    s32 yHeight = actor->pos.y + 25;
    if (actor->id == ACTOR_BG_MIZU_MOVEBG) {
        x += Math_SinS(actor->actor->shape.rot.y) * 50;
        z += Math_CosS(actor->actor->shape.rot.y) * 50;
        xzDist = sqrtf(SQ(player->actor.world.pos.x - x) + SQ(player->actor.world.pos.z - z));
    } else if (actor->id == ACTOR_SHOT_SUN) {
        ShotSun* sun = (ShotSun*)actor->actor;
        x = sun->hitboxPos.x;
        z = sun->hitboxPos.z;
        yHeight = sun->hitboxPos.y + 55;
        xzDist = sqrtf(SQ(player->actor.world.pos.x - x) + SQ(player->actor.world.pos.z - z));
    }
    s32 yIntercept = slope * xzDist + player->actor.focus.pos.y;
    AimAssistProps aimAssistProps;
    if (yIntercept > yHeight + 25) {
        aimAssistProps.pitch = 1.5;
    } else if (yIntercept < yHeight - 25) {
        aimAssistProps.pitch = 0.5;
    } else {
        aimAssistProps.pitch = 1.0;
    }
    s32 yDiff = fabs(yIntercept - yHeight);
    if (yIntercept - yHeight > 0) {
        s32 correction = 100.0f - 100.0f / std::max(slope, 1.0f);
        yDiff = std::max(yDiff - correction, 0);
    }
    if (yDiff > 300) {
        actor->aimFrequency = 30;
    } else {
        actor->aimFrequency = 1 + (uint8_t)(yDiff / 5);
    }
    s16 yawdiff = player->yaw - Math_Atan2S(z - player->actor.world.pos.z, x - player->actor.world.pos.x);
    if (yawdiff > -0x1000 && yawdiff < 0x1000) {
        aimAssistProps.volume = 1.0 - (yawdiff * yawdiff) / (float)0x2000000;
    } else if (yawdiff > -0x2000 && yawdiff < 0x2000) {
        aimAssistProps.volume = 0.4;
    } else {
        aimAssistProps.volume = 0.2;
    }
    aimAssistProps.pan = std::min(std::max(yawdiff / (float)0x1000, -1.0f), 1.0f);
    return aimAssistProps;
}

// External audio engine stuff.
bool ActorAccessibility_InitAudio() {
    try {
        aa->audioEngine = new AccessibleAudioEngine();
        aa->sfxDecoder = new AccessibleSfxDecoder();
    } catch (...) {
        delete aa->audioEngine;
        aa->audioEngine = NULL;
        aa->sfxDecoder = NULL;
        return false;
    }
    return true;
}

void ActorAccessibility_ShutdownAudio() {
    if (aa->isOn) {
        // Terrain cues stop their sounds as they're destroyed, so they have to go before the engine.
        if (aa->terrainCues) {
            DeleteTerrainCueState(aa->terrainCues);
            aa->terrainCues = NULL;
        }
        // Join the decoder next; its jobs write into records the map owns.
        delete aa->sfxDecoder;
        aa->sfxDecoder = NULL;
        delete aa->audioEngine;
        aa->audioEngine = NULL;
        // The engine's resource manager went with it, so the registrations are gone too.
        aa->sfxMap.clear();
        aa->isOn = false;
    }
}

void ActorAccessibility_MixAccessibleAudioWithGameAudio(int16_t* ogBuffer, uint32_t nFrames) {
    if (aa->isOn) {
        aa->audioEngine->mix(ogBuffer, nFrames);
    }
}

// Map one of the game's sfx to a path which as understood by the external audio engine. The returned token is a
// short hex string that can be passed directly to the audio engine.
const char* ActorAccessibility_MapSfxToExternalAudio(s16 sfxId) {
    if (aa->sfxDecoder == NULL)
        return NULL; // Audio init failed.

    SfxRecord* record;
    auto it = aa->sfxMap.find(sfxId);
    if (it == aa->sfxMap.end()) {
        std::string fullPath = SfxExtractor::getExternalFileName(sfxId);
        auto res = Ship::Context::GetRawInstance()->GetResourceManager()->GetArchiveManager()->LoadFile(fullPath);

        if (res == nullptr)
            return NULL; // Resource doesn't exist, user's gotta run the extractor.

        auto newRecord = std::make_unique<SfxRecord>();
        newRecord->resource = res;
        std::stringstream ss;
        ss << std::setw(4) << std::setfill('0') << std::hex << sfxId;
        newRecord->path = ss.str();
        record = newRecord.get();
        aa->sfxMap.insert({ sfxId, std::move(newRecord) });

        // Vorbis decoding is too slow to do inline, so the sound stays silent until it lands.
        aa->sfxDecoder->enqueue([record] {
            bool ok = AccessibleSfx_DecodeVorbis(reinterpret_cast<const uint8_t*>(record->resource->Buffer->data()),
                                                 record->resource->Buffer->size(), record->pcm);
            // The encoded copy is dead weight once decoded.
            record->resource = nullptr;
            record->state.store(ok ? SFX_DECODE_READY : SFX_DECODE_FAILED, std::memory_order_release);
        });
        return NULL;
    }

    record = it->second.get();
    if (!record->registered) {
        if (record->state.load(std::memory_order_acquire) != SFX_DECODE_READY) {
            return NULL; // Still decoding, or it failed and never will be.
        }
        // Registration has to happen on the thread that drives the engine; miniaudio is built
        // here with MA_NO_THREADING, so its internal locks are no-ops.
        // Matching the engine's rate keeps miniaudio from inserting a resampler.
        ma_resource_manager_register_decoded_data(&aa->audioEngine->resourceManager, record->path.c_str(),
                                                  record->pcm.data(), record->pcm.size(), ma_format_s16, 1,
                                                  AAE_SAMPLE_RATE);
        record->registered = true;
    }

    return record->path.c_str();
}

// Call once per frame to tell the audio engine to start working on the latest batch of queued instructions.
void ActorAccessibility_PrepareNextAudioFrame() {
    aa->audioEngine->prepare();
}

void ActorAccessibility_HandleSoundExtractionMode(PlayState* play) {
    aa->sfxExtractor.frameCallback();
}

void ActorAccessibility_DoSoundExtractionStep() {
    aa->sfxExtractor.captureCallback();
}

static ObjectExtension::Register<A11yID> RegisterA11yID;
