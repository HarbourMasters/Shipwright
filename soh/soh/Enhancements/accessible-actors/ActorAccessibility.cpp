#include "ActorAccessibility.h"
#include "AccessibleAudioEngine.h"
#include "soh/OTRGlobals.h"

#include <map>
#include <random>
#include <vector>
#include <functions.h>
#include <macros.h>
#include "ResourceType.h"
#include "SfxExtractor.h"

#include <sstream>
#define MAX_DB_REDUCTION 20// This is the amount in DB that a sound will be reduced by when it is at the maximum distance
                        // from the player.

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
typedef struct {
    std::string hexName;
    std::shared_ptr<LUS::IResource> resource;
}SfxRecord;

class ActorAccessibility {
  public:
      bool isOn = false;
    uint64_t nextActorID;
    SupportedActors_t supportedActors;
    TrackedActors_t trackedActors;
    AccessibleActorList_t accessibleActorList;
    VAZones_t vaZones;
    AccessibleAudioEngine* audioEngine;
    SfxExtractor sfxExtractor;
    std::unordered_map<s16, SfxRecord> sfxMap;//Maps internal sfx to external (prerendered) resources.

};
static ActorAccessibility* aa;

uint64_t ActorAccessibility_GetNextID() {
    uint64_t result = aa->nextActorID;
    aa->nextActorID++;
    return result;
}
void ActorAccessibility_Init() {
    aa = new ActorAccessibility();
    ActorAccessibility_InitAudio();
    ActorAccessibility_InitActors();

}
void ActorAccessibility_Shutdown() {
    ActorAccessibility_ShutdownAudio();
    delete aa;
}
    void ActorAccessibility_InitPolicy(ActorAccessibilityPolicy * policy, const char* englishName,
                                       ActorAccessibilityCallback callback, s16 sfx) {
    policy->callback = callback;
    policy->distance = 500;
    policy->englishName = englishName;
    policy->n = 20;
    policy->pitch = 1.5;
    policy->runsAlways = false;
    policy->sound = sfx;
    policy->volume = 1.0;
    policy->initUserData = NULL;
    policy->cleanupUserData = NULL;

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

void ActorAccessibility_TrackNewActor(Actor* actor) {
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
    bool ActorAccessibility_IsRealActor(AccessibleActor* actor) {
        return actor->actor != NULL;

    }
    void ActorAccessibility_CopyParamsFromRealActor(AccessibleActor* actor) {
        if (actor->actor == NULL)
            return;
        actor->projectedPos = actor->actor->projectedPos;
        actor->xzDistToPlayer = actor->actor->xzDistToPlayer;
        actor->isDrawn = actor->actor->isDrawn;

    }

    void ActorAccessibility_RunAccessibilityForActor(PlayState* play, AccessibleActor* actor) {
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
            Player* player = GET_PLAYER(play);
            actor->xzDistToPlayer = Math_Vec3f_DistXZ(&actor->world.pos, &player->actor.world.pos);

        }
            actor->currentVolume = ActorAccessibility_ComputeCurrentVolume(actor->policy.distance, actor->xzDistToPlayer);

        actor->frameCount--;
        if (actor->frameCount > 0)
            return;
        actor->frameCount = actor->policy.n;
        if (!actor->policy.runsAlways && actor->xzDistToPlayer > actor->policy.distance) {
            return;
        }
        if (actor->isDrawn == 0)
            return;

        if (actor->policy.callback != NULL)
            actor->policy.callback(actor);
        else
            ActorAccessibility_PlaySpecialSound(actor, actor->policy.sound);

    }
    void ActorAccessibility_RunAccessibilityForAllActors(PlayState* play) {
            //Entirely exclude the title screen.
        if (play->sceneNum == 81)
            return;

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
        actor.play = NULL;
        actor.world = where;
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
        delete aa->audioEngine;
    }
    void ActorAccessibility_MixAccessibleAudioWithGameAudio(int16_t* ogBuffer, uint32_t nFrames) {
        aa->audioEngine->mix(ogBuffer, nFrames);

    }
    bool ActorAccessibility_MapSfxToExternalAudio(s16 sfxId)
    {
        SfxRecord* record;
        auto it = aa->sfxMap.find(sfxId);
        if (it == aa->sfxMap.end())
        {
            SfxRecord tempRecord;
            std::string fullPath = SfxExtractor::getExternalFileName(sfxId);
            auto list = LUS::Context::GetInstance()->GetResourceManager()->GetArchive()->ListFiles("accessibility/audio/*");
            
            auto res = LUS::Context::GetInstance()->GetResourceManager()->LoadFile(fullPath);

            if (res == nullptr)
            return false;//Resource doesn't exist, user's gotta run the extractor.
            //tempRecord.resource = res;
            std::stringstream ss;
            ss << std::setw(4) << std::setfill('0') << std::hex << sfxId;
            tempRecord.hexName = ss.str();
            aa->sfxMap[sfxId] = tempRecord;
            record = &aa->sfxMap[sfxId];

        } else
            record = &it->second;

        return true;

    }
    void ActorAccessibility_PlayExternalSound(void* handle, const char* path, bool looping) {
        aa->audioEngine->playSound((uint64_t)handle, path, looping);

    }
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
