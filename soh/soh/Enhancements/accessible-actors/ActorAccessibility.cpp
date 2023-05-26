#include "ActorAccessibility.h"
#include <map>
#include <random>

#include <functions.h>
#include <overlays/actors/ovl_Obj_Switch/z_obj_switch.h>
const int MAX_DB_REDUCTION = 35;//This is the amount in DB that a sound will be reduced by when it is at the maximum distance from the player. A reduction of -35 DB pretty much makes a sound inaudible.
//One potential use of virtual actors is to place sounds at static platforms or other things that aren't represented by actors.
typedef std::map<s16, ActorAccessibilityPolicy> SupportedActors_t;
typedef std::map<Actor*, uint64_t> TrackedActors_t;
typedef std::map<uint64_t, AccessibleActor> AccessibleActorList_t;
SupportedActors_t SupportedActors;
TrackedActors_t TrackedActors;
AccessibleActorList_t AccessibleActorList;
uint64_t ActorAccessibility_GetNextID() {
    static uint64_t NextActorID = 0;
    uint64_t result = NextActorID;
    NextActorID++;
    return result;
}

void ActorAccessibility_AddSupportedActor(s16 type, const char* englishName, ActorAccessibilityCallback callback, int frames, float distance, f32 pitch, f32 volume, s16 sfx) {
    ActorAccessibilityPolicy policy;
    policy.callback = callback;

policy.englishName = englishName;
    policy.n = frames;
    policy.distance = distance;
    policy.pitch = pitch;
    policy.volume = volume;
    policy.sound = sfx;

    SupportedActors[type] = policy;

}

ActorAccessibilityPolicy* ActorAccessibility_GetPolicyForActor(s16 type) {
    SupportedActors_t::iterator i = SupportedActors.find(type);
     if (i == SupportedActors.end())
         return NULL;
     return &(i->second);

 }
int ActorAccessibility_GetRandomStartingFrameCount(int min, int max) {
     static std::mt19937 gen;
    std::uniform_int_distribution<> dist(min, max);
     return dist(gen);

 }

void ActorAccessibility_TrackNewActor(Actor* actor, ObjSwitch* sw) {
        // Don't track actors for which no accessibility policy has been configured.
        ActorAccessibilityPolicy* policy = ActorAccessibility_GetPolicyForActor(actor->id);
        if (policy == NULL)
            return;
        AccessibleActor state;
        state.instanceID = ActorAccessibility_GetNextID();
        state.actor = actor;
        state.id = actor->id;
        state.params = actor->params;
        state.flags = actor->flags;
        state.scaley = actor->scale.y;
        if (sw != NULL && sw->eyeTexIndex != NULL) {
            state.eyeTex = sw->eyeTexIndex;

        }
        //Stagger the start times so that all of the sounds don't play at exactly the same time.
        state.frameCount = ActorAccessibility_GetRandomStartingFrameCount(0, policy->n);
        state.basePitch = policy->pitch;

        state.currentPitch = policy->pitch;
        state.baseVolume = policy->volume;
        state.currentVolume = policy->volume;

        state.currentVolume = 1.0;
        state.currentReverb = 0;
        state.policy = *policy;

        TrackedActors[actor] = state.instanceID;
        AccessibleActorList[state.instanceID] = state;

    }
    void ActorAccessibility_RemoveTrackedActor(Actor* actor) {
        TrackedActors_t::iterator i = TrackedActors.find(actor);
        if (i == TrackedActors.end())
            return;
        uint64_t id = i->second;
        TrackedActors.erase(i);
        AccessibleActorList_t::iterator i2 = AccessibleActorList.find(id);
        if (i2 == AccessibleActorList.end())
            return;
        AccessibleActorList.erase(i2);

    }

    f32 ActorAccessibility_DBToLinear(float gain) {
        return (float)pow(10.0, gain / 20.0f);
    }
    f32 ActorAccessibility_ComputeCurrentVolume(AccessibleActor* actor) {
        if (actor->policy.distance == 0)
            return 0.0;
        f32 db = (abs(actor->xzDistToPlayer) / actor->policy.distance) * MAX_DB_REDUCTION;

        return ActorAccessibility_DBToLinear(db * -1);

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
        ActorAccessibility_CopyParamsFromRealActor(actor);
        actor->currentVolume = ActorAccessibility_ComputeCurrentVolume(actor);

        actor->frameCount--;
        if (actor->frameCount > 0)
            return;
        actor->frameCount = actor->policy.n;
        if (actor->xzDistToPlayer > actor->policy.distance) {
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
        for (AccessibleActorList_t::iterator i = AccessibleActorList.begin(); i != AccessibleActorList.end(); i++)
            ActorAccessibility_RunAccessibilityForActor(play, &i->second);

    }
