#include "ActorAccessibility.h"
#include "AccessibleAudioEngine.h"
#include "soh/OTRGlobals.h"

#include <map>
#include <random>
#include <vector>
#include <functions.h>
#include <variables.h>
#include <macros.h>
#include "ResourceType.h"
#include "SfxExtractor.h"

#include <sstream>
#include "File.h"
#include <unordered_set>
#include "soh/Enhancements/speechsynthesizer/SpeechSynthesizer.h"
#include "soh/Enhancements/tts/tts.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/audio/AudioDecoder.h"
extern "C" {
extern PlayState* gPlayState;
extern bool freezeGame;
}

const char* GetLanguageCode();

#define MAX_DB_REDUCTION 35// This is the amount in DB that a sound will be reduced by when it is at the maximum distance
                        // from the player.
extern "C" {
                        void CollisionPoly_GetVertices(CollisionPoly* poly, Vec3s* vtxList, Vec3f* dest);//Used to tell where polygons are located.
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
    typedef std::map<s16, ActorAccessibilityPolicy> SupportedActors_t;//Maps actors to their accessibility policies, which describe how accessibility should treat them.
typedef std::map<Actor*, uint64_t> TrackedActors_t;//Maps real actors to internal IDs specific to accessibility.
typedef std::map<uint64_t, AccessibleActor> AccessibleActorList_t;//Maps internal IDs to wrapped actor objects. These actors can be real or virtual.
typedef std::vector<AccessibleActor> VAList_t;//Denotes a list of virtual actors specific to a single room.
typedef std::map<s32, VAList_t> VAZones_t;//Maps room/ scene indices to their corresponding virtual actor collections.
typedef std::unordered_set<s16> SceneList_t;//A list of scenes which have already been visited (since the game was launched). Used to prevent re-creation of terrain VAs every time the player reloads a scene.

typedef struct {
    std::string path;
    std::shared_ptr<LUS::File> resource;
    std::shared_ptr<s16*> decodedSample;//Set if the record is for a raw sample as opposed to a SFX.

}SfxRecord;

class ActorAccessibility {
  public:
      int isOn = 0;
    uint64_t nextActorID = 0;
    SupportedActors_t supportedActors;
    TrackedActors_t trackedActors;
    AccessibleActorList_t accessibleActorList;
    VAZones_t vaZones;
    SceneList_t sceneList;
    AccessibleAudioEngine* audioEngine;
    SfxExtractor sfxExtractor;
    std::unordered_map<s16, SfxRecord> sfxMap;//Maps internal sfx to external (prerendered) resources.
    std::unordered_map<std::string, SfxRecord> sampleMap;//Similar to above, but this one maps raw audio samples as opposed to SFX.
    int extractSfx = 0;
};
static ActorAccessibility* aa;

uint64_t ActorAccessibility_GetNextID() {
    uint64_t result = aa->nextActorID;
    aa->nextActorID++;
    return result;
}

// Hooks for game-interactor.
void ActorAccessibility_OnActorInit(void* actor) {
    ActorAccessibility_TrackNewActor((Actor*)actor);
}
void ActorAccessibility_OnGameFrameUpdate() {
    if (gPlayState == NULL)
        return;

    ActorAccessibility_RunAccessibilityForAllActors(gPlayState);
}
void ActorAccessibility_OnActorDestroy(void* actor)
{
    ActorAccessibility_RemoveTrackedActor((Actor*) actor);

}
void ActorAccessibility_OnGameStillFrozen()
{
    if (gPlayState == NULL)
        return;
    if (aa->extractSfx)
        ActorAccessibility_HandleSoundExtractionMode(gPlayState);

}
    void ActorAccessibility_Init() {

    aa = new ActorAccessibility();
    aa->isOn = CVarGetInteger("gA11yAudioInteraction", 0);
    if (!aa->isOn)
        return;
    aa->extractSfx = CVarGetInteger("gExtractSfx", 0);
    if (aa->extractSfx)
        freezeGame = true;
    ActorAccessibility_InitAudio();
    ActorAccessibility_InitActors();
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnActorInit>(ActorAccessibility_OnActorInit);
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnActorDestroy>(ActorAccessibility_OnActorDestroy);

    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnGameFrameUpdate>(ActorAccessibility_OnGameFrameUpdate);
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnGameStillFrozen>(ActorAccessibility_OnGameStillFrozen);

    }
void ActorAccessibility_Shutdown() {
    ActorAccessibility_ShutdownAudio();
    delete aa;
}
    void ActorAccessibility_InitPolicy(ActorAccessibilityPolicy * policy, const char* englishName,
                                       ActorAccessibilityCallback callback, s16 sfx) {
    policy->callback = callback;
    policy->distance = 500;
    policy->ydist = 80;
    policy->englishName = englishName;
    policy->n = 20;
    policy->pitch = 1.5;
    policy->runsAlways = false;
    policy->sound = sfx;
    policy->volume = 1.0;
    policy->initUserData = NULL;
    policy->cleanupUserData = NULL;
    policy->pitchModifier = 0.1;

}

    void ActorAccessibility_AddSupportedActor(s16 type, ActorAccessibilityPolicy policy) {
    aa->supportedActors[type] = policy;

}

ActorAccessibilityPolicy* ActorAccessibility_GetPolicyForActor(s16 type) {
    SupportedActors_t::iterator i = aa->supportedActors.find(type);
     if (i == aa->supportedActors.end())
         return NULL;
     return &(i->second);

 }
int ActorAccessibility_GetRandomStartingFrameCount(int min, int max) {
     static std::mt19937 gen;
    std::uniform_int_distribution<> dist(min, max);
     return dist(gen);

 }

    void ActorAccessibility_TrackNewActor(Actor * actor) {
        // Don't track actors for which no accessibility policy has been configured.
        ActorAccessibilityPolicy* policy = ActorAccessibility_GetPolicyForActor(actor->id);
        if (policy == NULL)
            return;
        AccessibleActor accessibleActor;
        accessibleActor.instanceID = ActorAccessibility_GetNextID();
        accessibleActor.actor = actor;
        accessibleActor.id = actor->id;
        //Stagger the start times so that all of the sounds don't play at exactly the same time.
        accessibleActor.frameCount = ActorAccessibility_GetRandomStartingFrameCount(0, policy->n);
        accessibleActor.basePitch = policy->pitch;
        accessibleActor.policy = *policy;
        accessibleActor.currentPitch = accessibleActor.policy.pitch;
        accessibleActor.baseVolume = accessibleActor.policy.volume;
        accessibleActor.currentVolume = accessibleActor.policy.volume;
        accessibleActor.currentReverb = 0;
        accessibleActor.sceneIndex = 0;
        for (int i = 0; i < NUM_MANAGED_SOUND_SLOTS; i++)
            accessibleActor.managedSoundSlots[i] = false;
        aa->trackedActors[actor] = accessibleActor.instanceID;
        aa->accessibleActorList[accessibleActor.instanceID] = accessibleActor;
        AccessibleActor& savedActor = aa->accessibleActorList[accessibleActor.instanceID];
        if (policy->initUserData && !policy->initUserData(&savedActor)) {
            ActorAccessibility_RemoveTrackedActor(actor);

            return; // Probably a malloc error preventing user data initialization.
        }

    }
    void ActorAccessibility_RemoveTrackedActor(Actor* actor) {
        TrackedActors_t::iterator i = aa->trackedActors.find(actor);
        if (i == aa->trackedActors.end())
            return;
        uint64_t id = i->second;
        aa->trackedActors.erase(i);
        AccessibleActorList_t::iterator i2 = aa->accessibleActorList.find(id);
        if (i2 == aa->accessibleActorList.end())
            return;
        if (i2->second.policy.cleanupUserData)
            i2->second.policy.cleanupUserData(&i2->second);
        ActorAccessibility_StopAllSoundsForActor(&i2->second);
        aa->accessibleActorList.erase(i2);

    }

    f32 ActorAccessibility_DBToLinear(float gain) {
        return (float)pow(10.0, gain / 20.0f);
    }
    f32 ActorAccessibility_ComputeCurrentVolume(f32 maxDistance, f32 xzDistToPlayer) {
        if (maxDistance == 0)
            return 0.0;
        f32 absDistance = fabs(xzDistToPlayer);

        f32 db = LERP(0.0 - MAX_DB_REDUCTION, 0.0, (maxDistance - absDistance) / maxDistance);

        return ActorAccessibility_DBToLinear(db);

    }

    void ActorAccessibility_PlaySpecialSound(AccessibleActor* actor, s16 sfxId) {
        Audio_PlaySoundGeneral(sfxId, &actor->projectedPos, 4, &actor->currentPitch, &actor->currentVolume, &actor->currentReverb);
    }
    const char* ActorAccessibility_MapSfxToExternalAudio(s16 sfxId);
    void ActorAccessibility_PlaySound(void* handle, int slot, s16 sfxId, bool looping)
    {
        const char* path = ActorAccessibility_MapSfxToExternalAudio(sfxId);
        if (path == NULL)
            return;
        aa->audioEngine->playSound((uintptr_t)handle, slot, path, looping);
    }
    const char* ActorAccessibility_MapRawSampleToExternalAudio(const char* name);

    void ActorAccessibility_PlayRawSample(void* handle, int slot, const char* name, bool looping)
    {
        const char* path = ActorAccessibility_MapRawSampleToExternalAudio(name);
        if (path == NULL)
            return;
        aa->audioEngine->playSound((uintptr_t)handle, slot, path, looping);

    }
    void ActorAccessibility_StopSound(void* handle, int slot)
    {
        aa->audioEngine->stopSound((uintptr_t) handle, slot);

    }
    void ActorAccessibility_StopAllSounds(void* handle)
    {
        aa->audioEngine->stopAllSounds((uintptr_t)handle);
    }
    void ActorAccessibility_SetSoundPitch(void* handle, int slot, float pitch)
    {
        aa->audioEngine->setPitch((uintptr_t)handle, slot, pitch);

    }
    void ActorAccessibility_SetPitchBehindModifier(void* handle, int slot, float mod)
    {
        aa->audioEngine->setPitchBehindModifier((uintptr_t) handle, slot, mod);

    }
    void ActorAccessibility_SetSoundPos(void* handle, int slot, Vec3f* pos, f32 distToPlayer, f32 maxDistance) {
        aa->audioEngine->setSoundPosition((uintptr_t) handle, slot, pos->x, pos->y, pos->z, distToPlayer, maxDistance);
    }
    void ActorAccessibility_SetSoundVolume(void* handle, int slot, float volume)
    {
        aa->audioEngine->setVolume((uintptr_t)handle, slot, volume);

    }
    void ActorAccessibility_SetSoundPan(void* handle, int slot, Vec3f* projectedPos)
    {
        float pan = projectedPos->x / 270;
        if (pan < -1.0)
            pan = -1.0;
        if (pan > 1.0)
            pan = 1.0;
        aa->audioEngine->setPan((uintptr_t)handle, slot, pan);

    }
    void ActorAccessibility_SetSoundFilter(void* handle, int slot, float cutoff)
    {
        aa->audioEngine->setFilter((uintptr_t)handle, slot, cutoff);

    }
    void ActorAccessibility_SeekSound(void* handle, int slot, size_t offset)
    {
        aa->audioEngine->seekSound((uintptr_t)handle, slot, offset);

    }
    void ActorAccessibility_ConfigureSoundForActor(AccessibleActor* actor, int slot)
    {
        ActorAccessibility_SetSoundPitch(actor, slot, actor->policy.pitch);
        ActorAccessibility_SetPitchBehindModifier(actor, slot, actor->policy.pitchModifier);
        ActorAccessibility_SetSoundPos(actor, slot, &actor->projectedPos, actor->xyzDistToPlayer,
                                       actor->policy.distance);
        ActorAccessibility_SetSoundVolume(actor, slot, actor->policy.volume);
        actor->managedSoundSlots[slot] = true;
    }
    void ActorAccessibility_PlaySoundForActor(AccessibleActor* actor, int slot, s16 sfxId, bool looping)
    {
        if (slot < 0 || slot > NUM_MANAGED_SOUND_SLOTS)
            return;
        ActorAccessibility_PlaySound(actor, slot, sfxId, looping);
        ActorAccessibility_ConfigureSoundForActor(actor, slot);

    }
    void ActorAccessibility_PlaySampleForActor(AccessibleActor* actor, int slot, const char* name, bool looping)
    {
        if (slot < 0 || slot > NUM_MANAGED_SOUND_SLOTS)
            return;
        ActorAccessibility_PlayRawSample(actor, slot, name, looping);
        ActorAccessibility_ConfigureSoundForActor(actor, slot);

    }
    void ActorAccessibility_StopSoundForActor(AccessibleActor* actor, int slot)
    {
        if (slot < 0 || slot >= NUM_MANAGED_SOUND_SLOTS)
            return;
        ActorAccessibility_StopSound(actor, slot);
        actor->managedSoundSlots[slot] = false;
    }
    void ActorAccessibility_StopAllSoundsForActor(AccessibleActor* actor)

    {
        ActorAccessibility_StopAllSounds(actor);
        for (int i = 0; i < NUM_MANAGED_SOUND_SLOTS; i++)
            actor->managedSoundSlots[i] = false;
    }

    bool ActorAccessibility_IsRealActor(AccessibleActor* actor) {
        return actor->actor != NULL;

    }
    void ActorAccessibility_CopyParamsFromRealActor(AccessibleActor* actor) {
        Player* player = GET_PLAYER(actor->play);
        if (actor->actor == NULL)
            return;
        actor->projectedPos = actor->actor->projectedPos;
        actor->xzDistToPlayer = actor->actor->xzDistToPlayer;
        actor->isDrawn = actor->actor->isDrawn;
        actor->xyzDistToPlayer = Math_Vec3f_DistXYZ(&actor->actor->world.pos, &player->actor.world.pos);
    }
    void ActorAccessibility_PrepareNextAudioFrame();

    void ActorAccessibility_RunAccessibilityForActor(PlayState* play, AccessibleActor* actor) {
        Player* player = GET_PLAYER(play);
        actor->play = play;
        if(ActorAccessibility_IsRealActor(actor))
            ActorAccessibility_CopyParamsFromRealActor(actor);
            
        else {
//Perform calculations that the game would normally take care of for real actors.
            f32 w = 0.0f;
            //Set actor->projectedPos.
            SkinMatrix_Vec3fMtxFMultXYZW(&play->viewProjectionMtxF, &actor->world.pos, &actor->projectedPos,
                                         &w);
//Set actor->xzDistToPlayer.
            
            actor->xzDistToPlayer = Math_Vec3f_DistXZ(&actor->world.pos, &player->actor.world.pos);
            actor->xyzDistToPlayer = Math_Vec3f_DistXYZ(&actor->world.pos, &player->actor.world.pos);
            actor->yDistToPlayer = fabs((actor->world.pos.y) - (player->actor.world.pos.y));
        }
        
        if (actor->actor != NULL && fabs(actor->actor->yDistToPlayer) > actor->policy.ydist) {
            return;
        }
        //Send sound parameters to the new audio engine. Eventually remove the old stuff once all actors are carried over.
            for (int i = 0; i < NUM_MANAGED_SOUND_SLOTS; i++)
            {
                if (actor->managedSoundSlots[i])
                {
            ActorAccessibility_SetSoundPos(actor, i, &actor->projectedPos, actor->xyzDistToPlayer, actor->policy.distance); 
           //Judgement call: pitch changes are rare enough that it doesn't make sense to pay the cost of updating it every frame. If you want a pitch change, call the function as needed.

            }
            }
        actor->frameCount++;
        if (actor->frameCount != 1 && (actor->frameCount - 1) % actor->policy.n != 0)
            return;
        if (!actor->policy.runsAlways && actor->xyzDistToPlayer > actor->policy.distance) {
            return;
        }
        if (actor->isDrawn == 0)
            return;

        if (actor->policy.callback != NULL)
            actor->policy.callback(actor);
        else
            ActorAccessibility_PlaySoundForActor(actor, 0, actor->policy.sound, false);

    }
    void ActorAccessibility_RunAccessibilityForAllActors(PlayState* play) {
            //Entirely exclude the title screen.
        /*if (play->sceneNum == 81)
            return;*/
        
        Player* player = GET_PLAYER(play);
        
        
        if (player->stateFlags1 & PLAYER_STATE1_IN_CUTSCENE) {
            return;
        }
        //Real actors.
        for (AccessibleActorList_t::iterator i = aa->accessibleActorList.begin(); i != aa->accessibleActorList.end(); i++)
            ActorAccessibility_RunAccessibilityForActor(play, &i->second);
//Virtual actors in the "everywhere" group.
        VAList_t* list = (VAList_t*)ActorAccessibility_GetVirtualActorList(EVERYWHERE, 0);

        for (VAList_t::iterator i = list->begin(); i != list->end(); i++)
            ActorAccessibility_RunAccessibilityForActor(play, &(*i));
//Virtual actors for the current room and scene.
        list = (VAList_t*)ActorAccessibility_GetVirtualActorList(play->sceneNum, play->roomCtx.curRoom.num);
        for (VAList_t::iterator i = list->begin(); i != list->end(); i++)
            ActorAccessibility_RunAccessibilityForActor(play, &(*i));
        //Scene-global virtual actors. Most of these are automatically generated VAs from polygons, because there's no way to sort these into rooms.
        list = (VAList_t*)ActorAccessibility_GetVirtualActorList(play->sceneNum, -1);
        for (VAList_t::iterator i = list->begin(); i != list->end(); i++)
            ActorAccessibility_RunAccessibilityForActor(play, &(*i));

        //Processes external audio engine.
        ActorAccessibility_PrepareNextAudioFrame();
    }
//Virtual actor config.
    VirtualActorList* ActorAccessibility_GetVirtualActorList(s16 sceneNum, s8 roomNum)
    {
        SceneAndRoom sr;
        sr.values.sceneIndex = sceneNum;
        sr.values.roomIndex = roomNum;
        if (sceneNum == EVERYWHERE)
            sr.values.sceneIndex = EVERYWHERE;

        VAList_t* l = &aa->vaZones[sr.raw];
        return (VirtualActorList*)l;

    }
    AccessibleActor* ActorAccessibility_AddVirtualActor(VirtualActorList* list, VIRTUAL_ACTOR_TABLE type, PosRot where)
    {
        ActorAccessibilityPolicy* policy = ActorAccessibility_GetPolicyForActor(type);
        if (policy == NULL)
            return NULL;

        AccessibleActor actor;
        actor.actor = NULL;
        actor.basePitch = 1.0;
        actor.baseVolume = 1.0;
        actor.currentPitch = 1.0;
        actor.currentReverb = 1.0;
        actor.currentVolume = 1.0;
        actor.frameCount = 0;
        actor.id = (s16)type;
        actor.instanceID = ActorAccessibility_GetNextID();
        actor.isDrawn = 1;
      //  actor.variety = variety;
        actor.play = NULL;
        actor.world = where;
        actor.sceneIndex = 0;
        for (int i = 0; i < NUM_MANAGED_SOUND_SLOTS; i++)
            actor.managedSoundSlots[i] = 0;

        actor.policy = *policy;
        VAList_t* l = (VAList_t*)list;
        l->push_back(actor);
        size_t index = l->size() - 1;
        AccessibleActor* savedActor = &(*l)[l->size() - 1];
        if (policy->initUserData && !policy->initUserData(savedActor)) {
            l->pop_back();

            return NULL; // Probably a malloc error preventing user data initialization.
        }
        return savedActor;

    }
    void ActorAccessibility_InterpretCurrentScene(PlayState* play)
    {
        if (aa->sceneList.contains(play->sceneNum))
            return;//Scene interpretation already complete for this scene.
        aa->sceneList.insert(play->sceneNum);
        VirtualActorList* list = ActorAccessibility_GetVirtualActorList(play->sceneNum, -1);//Scene-global VAs.
        if (list == NULL)
            return;
        for (int i = 0; i < play->colCtx.colHeader->numPolygons; i++)
        {
            CollisionPoly* poly = &play->colCtx.colHeader->polyList[i];
            // checks if climable
            if ((func_80041DB8(&play->colCtx, poly, BGCHECK_SCENE) == 8 || func_80041DB8(&play->colCtx, poly, BGCHECK_SCENE) == 3)) {
            ActorAccessibility_PolyToVirtualActor(play, poly, VA_CLIMB, list);
            }
            if (SurfaceType_IsWallDamage(&play->colCtx, poly, BGCHECK_SCENE)) {
            ActorAccessibility_PolyToVirtualActor(play, poly, VA_SPIKE, list);
            }
            if (SurfaceType_GetSceneExitIndex(&play->colCtx, poly, BGCHECK_SCENE) != 0)
            ActorAccessibility_PolyToVirtualActor(play, poly, VA_AREA_CHANGE, list);

       }

    }
//Convert poly to VA.
    void ActorAccessibility_PolyToVirtualActor(PlayState* play, CollisionPoly* poly, VIRTUAL_ACTOR_TABLE va, VirtualActorList* destination)
    {
       Vec3f polyVerts[3];
       CollisionPoly_GetVertices(poly, play->colCtx.colHeader->vtxList, polyVerts);
       PosRot where;
       where.pos.y = std::min(polyVerts[0].y, std::min(polyVerts[1].y, polyVerts[2].y));
       f32 minX = std::min(polyVerts[0].x, std::min(polyVerts[1].x, polyVerts[2].x));
              f32 maxX = std::max(polyVerts[0].x, std::max(polyVerts[1].x, polyVerts[2].x));
       f32 minZ = std::min(polyVerts[0].z, std::min(polyVerts[1].z, polyVerts[2].z));
              f32 maxZ = std::max(polyVerts[0].z, std::max(polyVerts[1].z, polyVerts[2].z));
       where.pos.x = maxX - ((maxX - minX) / 2);
              where.pos.z = maxZ - ((maxZ - minZ) / 2);
       where.rot = { 0, 0, 0 };
       AccessibleActor* actor = ActorAccessibility_AddVirtualActor(destination, va, where);
       if (actor == NULL)
            return;
       if (va == VA_AREA_CHANGE) {
            actor->sceneIndex = SurfaceType_GetSceneExitIndex(&play->colCtx, poly, BGCHECK_SCENE);
            s16 nextEntranceIndex = play->setupExitList[actor->sceneIndex - 1];
            actor->sceneIndex = gEntranceTable[nextEntranceIndex].scene;
       }
    }
    void ActorAccessibility_AnnounceRoomNumber(PlayState* play)
    {
       std::stringstream ss;
       ss << "Room" << (int)play->roomCtx.curRoom.num;
       if (Flags_GetClear(play, play->roomCtx.curRoom.num))
            ss << ", completed." << std::endl;
       else
       ss << "." << std::endl;
       SpeechSynthesizer::Instance->Speak(ss.str().c_str(), GetLanguageCode());

    }
        //External audio engine stuff.
    bool ActorAccessibility_InitAudio() {
        try {
            aa->audioEngine = new AccessibleAudioEngine();
        } catch (...) {
            aa->audioEngine = NULL;
            return false;
        }
        return true;
    }
    void ActorAccessibility_ShutdownAudio() {
        if (aa->isOn == 0)
            return;
        delete aa->audioEngine;
    }
    void ActorAccessibility_MixAccessibleAudioWithGameAudio(int16_t* ogBuffer, uint32_t nFrames) {
        if (aa->isOn == 0)
            return;

        aa->audioEngine->mix(ogBuffer, nFrames);

    }
    // Map one of the game's sfx to a path which as understood by the external audio engine. The returned token is a
    // short hex string that can be passed directly to the audio engine.
    const char* ActorAccessibility_MapSfxToExternalAudio(s16 sfxId)
    {
        SfxRecord* record;
        auto it = aa->sfxMap.find(sfxId);
        if (it == aa->sfxMap.end())
        {
            SfxRecord tempRecord;
            std::string fullPath = SfxExtractor::getExternalFileName(sfxId);
            auto res = LUS::Context::GetInstance()->GetResourceManager()->LoadFile(fullPath);

if(res == nullptr)
return NULL;//Resource doesn't exist, user's gotta run the extractor.
            tempRecord.resource = res;
            std::stringstream ss;
            ss << std::setw(4) << std::setfill('0') << std::hex << sfxId;
            tempRecord.path = ss.str();
            aa->sfxMap[sfxId] = tempRecord;
            record = &aa->sfxMap[sfxId];
            aa->audioEngine->cacheDecodedSample(record->path, record->resource->Buffer.data(),
                                                record->resource->Buffer.size());
        } else
            record = &it->second;

        return record->path.c_str();


    }
//Map the path to a raw sample to the external audio engine.
    const char* ActorAccessibility_MapRawSampleToExternalAudio(const char* name)
    {
        SfxRecord* record;
        std::string key(name);
        auto it = aa->sampleMap.find(key);
        if (it == aa->sampleMap.end()) {
            SfxRecord tempRecord;
            std::stringstream ss;
            ss << "audio/samples/" << key;
            std::string fullPath = ss.str();
            auto res = LUS::Context::GetInstance()->GetResourceManager()->LoadResource(fullPath);
            if (res == nullptr)
return NULL; // Resource doesn't exist, user's gotta run the extractor.
            AudioDecoder decoder;
            decoder.setSample((LUS::AudioSample*)res.get());
            s16* wav;
            size_t wavSize = decoder.decodeToWav(&wav);

            tempRecord.path = key;
            tempRecord.decodedSample = std::make_shared<s16*>(wav);
            aa->sampleMap[key] = tempRecord;
            record = &aa->sampleMap[key];
            aa->audioEngine->cacheDecodedSample(record->path, wav,
                                                wavSize);
        } else
            record = &it->second;

        return record->path.c_str();
    }
    // Call once per frame to tell the audio engine to start working on the latest batch of queued instructions.

    void ActorAccessibility_PrepareNextAudioFrame() {
        aa->audioEngine->prepare();

    }
    void ActorAccessibility_HandleSoundExtractionMode(PlayState* play)
    {
        aa->sfxExtractor.frameCallback();

    }

    void ActorAccessibility_DoSoundExtractionStep()
    {
        aa->sfxExtractor.captureCallback();

    }


