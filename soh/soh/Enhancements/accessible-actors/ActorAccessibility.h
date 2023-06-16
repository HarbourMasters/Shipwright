#pragma once
#include <z64.h>

#ifdef __cplusplus
extern "C" {
struct AccessibleActor;
typedef struct AccessibleActor AccessibleActor;
//A callback that is run regularely as the game progresses in order to provide accessibility services for an actor.

typedef void (*ActorAccessibilityCallback)(AccessibleActor*);
//A callback which allows AccessibleActor instances to initialize custom user data (called once per instantiation).
typedef bool (*ActorAccessibilityUserDataInit)(AccessibleActor*);
//A callback that can be used to clean up user data when an actor is destroyed.
typedef void (*ActorAccessibilityUserDataCleanup)(AccessibleActor*);

struct VirtualActorList;
typedef struct VirtualActorList VirtualActorList;

typedef struct {

const char* englishName;

    ActorAccessibilityCallback callback; // If set, it will be called once every n frames. If null, then sfx will be
                                         // played once every n frames.
    s16 sound;                           // The ID of a sound to play. Ignored if the callback is set.

    int n;        // How often to run the callback in frames.
    f32 distance; // Maximum distance from player before the actor should be considered out of range.
    f32 pitch;
    f32 volume;
    bool runsAlways;//If set, then the distance policy is ignored.
    ActorAccessibilityUserDataInit initUserData;
    ActorAccessibilityUserDataCleanup cleanupUserData;

} ActorAccessibilityPolicy;

// Accessible actor object. This can be a "real" actor (one that corresponds to an actual actor in the game) or a
// "virtual" actor (which does not actually exist in the game, but is used to create extra sounds for the player).
// One potential use of virtual actors is to place sounds at static platforms or other things that aren't represented by
// actors.

struct AccessibleActor
{
    uint64_t instanceID;

    Actor* actor; // This can be null for a virtual actor.
    s16 id;       // For real actors, we copy the ID of the actor. For virtual actors we have our own table of values which are out of range for real actors.
    f32 xzDistToPlayer;
    PosRot world;
    Vec3f projectedPos;
    PlayState* play;
    u8 isDrawn;//Do we just never play accessibility sounds for actors that aren't drawn?

    int frameCount; // Incremented every time the callback is called. The callback is free to modify this. Can be used
                    // to implement playback of sounds at regular intervals.
    f32 baseVolume;
    f32 currentVolume;
    f32 basePitch;
    
    f32 currentPitch;
    s8 currentReverb;
    // Add more state as needed.
    ActorAccessibilityPolicy policy;//A copy, so it can be customized on a per-actor basis if needed.
    void* userData;//Set by the policy. Can be anything.
};

void ActorAccessibility_Init();
void ActorAccessibility_InitPolicy(ActorAccessibilityPolicy& policy, const char* englishName, ActorAccessibilityCallback callback, s16 sfx = NULL);

void ActorAccessibility_TrackNewActor(Actor* actor);
        void ActorAccessibility_RemoveTrackedActor(Actor* actor);
void ActorAccessibility_AddSupportedActor(s16 type, ActorAccessibilityPolicy policy);

            void ActorAccessibility_RunAccessibilityForActor(PlayState* play, AccessibleActor* actor);
        void ActorAccessibility_RunAccessibilityForAllActors(PlayState* play);
        void ActorAccessibility_PlaySpecialSound(AccessibleActor* actor, s16 sfxId);
        f32 ActorAccessibility_ComputeCurrentVolume(f32 maxDistance, f32 xzDistToPlayer);
        // Computes a relative angle based on Link's (or some other actor's) current angle.
        Vec3s ActorAccessibility_ComputeRelativeAngle(Vec3s& origin, Vec3s& offset);
        void ActorAccessibility_InitCues();

        }
//Stuff related to lists of virtual actors.
typedef enum {
//Similar to the game's actual actor table. Values here start at 10000 just to be extra safe.
VA_INITIAL=1000,
VA_PROTOTYPE,//Remove this one once this thing is working.
VA_CRAWLSPACE,
VA_TERRAIN_CUE,
VA_WALL_CUE,

} VIRTUAL_ACTOR_TABLE;
const s16 EVERYWHERE = -32768;//Denotes a virtual actor that is global/ omnipresent.

    //Get the list of virtual actors for a given scene and room index.
VirtualActorList* ActorAccessibility_GetVirtualActorList(s16 sceneNum, s8 roomNum);
AccessibleActor* ActorAccessibility_AddVirtualActor(VirtualActorList* list, VIRTUAL_ACTOR_TABLE type, PosRot where);

#endif
