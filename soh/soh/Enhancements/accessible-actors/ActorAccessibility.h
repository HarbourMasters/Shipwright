#pragma once
#include <z64.h>

#ifdef __cplusplus
extern "C" {
#endif
    #define NUM_MANAGED_SOUND_SLOTS 10 //How many auto-managed sound slots can any given actor have? this can differ from AAE_SLOTS_PER_HANDLE, but cannot be greater.
struct AccessibleActor;
typedef struct AccessibleActor AccessibleActor;
// A callback that is run regularely as the game progresses in order to provide accessibility services for an actor.

typedef void (*ActorAccessibilityCallback)(AccessibleActor*);
// A callback which allows AccessibleActor instances to initialize custom user data (called once per instantiation).
typedef bool (*ActorAccessibilityUserDataInit)(AccessibleActor*);
// A callback that can be used to clean up user data when an actor is destroyed.
typedef void (*ActorAccessibilityUserDataCleanup)(AccessibleActor*);

struct VirtualActorList;
typedef struct VirtualActorList VirtualActorList;

typedef struct {

    const char* englishName;

    ActorAccessibilityCallback callback; // If set, it will be called once every n frames. If null, then sfx will be
                                         // played once every n frames.
    s16 sound;                           // The ID of a sound to play. Ignored if the callback is set.

    int n;        // How often to run the callback in frames.
    f32 distance; // Maximum xz distance from player before the actor should be considered out of range.
    f32 ydist; // Maximum y distance from player before the actor should be considered out of range.
    f32 pitch;
    f32 volume;
    f32 pitchModifier;
    bool runsAlways; // If set, then the distance policy is ignored.
    ActorAccessibilityUserDataInit initUserData;
    ActorAccessibilityUserDataCleanup cleanupUserData;
    //u8 param;

} ActorAccessibilityPolicy;

// Accessible actor object. This can be a "real" actor (one that corresponds to an actual actor in the game) or a
// "virtual" actor (which does not actually exist in the game, but is used to create extra sounds for the player).
// One potential use of virtual actors is to place sounds at static platforms or other things that aren't represented by
// actors.

struct AccessibleActor {
    uint64_t instanceID;

    Actor* actor; // This can be null for a virtual actor.
    s16 id; // For real actors, we copy the ID of the actor. For virtual actors we have our own table of values which
            // are out of range for real actors.
    f32 yDistToPlayer;
    f32 xzDistToPlayer;
    f32 xyzDistToPlayer;
    PosRot world;
    Vec3f projectedPos;
    PlayState* play;
    u8 isDrawn; // Do we just never play accessibility sounds for actors that aren't drawn?

    int frameCount; // Incremented every time the callback is called. The callback is free to modify this. Can be used
                    // to implement playback of sounds at regular intervals.
    f32 baseVolume;
    f32 currentVolume;
    f32 basePitch;

    f32 currentPitch;
    s8 currentReverb;
    s16 sceneIndex;//If this actor represents a scene transition, then this will contain the destination scene index. Zero otherwise.

    s16 variety;

    bool managedSoundSlots[NUM_MANAGED_SOUND_SLOTS];//These have their attenuation and panning parameters updated every frame automatically.

    // Add more state as needed.
    ActorAccessibilityPolicy policy; // A copy, so it can be customized on a per-actor basis if needed.
    void* userData;                  // Set by the policy. Can be anything.
};
// Initialize accessibility.
void ActorAccessibility_Init();
void ActorAccessibility_InitActors();
void ActorAccessibility_Shutdown();

void ActorAccessibility_InitPolicy(ActorAccessibilityPolicy* policy, const char* englishName,
                                   ActorAccessibilityCallback callback, s16 sfx);

void ActorAccessibility_TrackNewActor(Actor* actor);
void ActorAccessibility_RemoveTrackedActor(Actor* actor);
void ActorAccessibility_AddSupportedActor(s16 type, ActorAccessibilityPolicy policy);

void ActorAccessibility_RunAccessibilityForActor(PlayState* play, AccessibleActor* actor);
void ActorAccessibility_RunAccessibilityForAllActors(PlayState* play);
void ActorAccessibility_PlaySpecialSound(AccessibleActor* actor, s16 sfxId);
/*
*Play sounds (usually from the game) using the external sound engine. This is probably not the function you want to call most of the time (see below).
* handle: pointer to an arbitrary object. This object can be anything as it's only used as a classifier, but it's recommended that you use an AccessibleActor* as your handle whenever possible. Using AccessibleActor* as the handle gives you automatic cleanup when the actor is killed.
* slot: Allows multiple sounds to be assigned to a single handle. The maximum number of slots per actor is 10 by default (but can be controlled by modifying AAE_SLOTS_PER_HANDLE).
* sfxId: one of the game's sfx IDs. Note that this plays prerendered sounds which you must have previously prepared.
 *looping: whether to play the sound just once or on a continuous loop.
*/
void ActorAccessibility_PlaySound(void* actor, int slot, s16 sfxId, bool looping);
//Stop a sound. Todo: consider making this a short fade instead of just cutting it off.
void ActorAccessibility_StopSound(void* handle, int slot);
void ActorAccessibility_StopAllSounds(void* handle);

void ActorAccessibility_SetSoundPitch(void* handle, int slot, float pitch);
//When we don't have access to something super fancy (such as HRTF), blind-accessible games generally use a change in pitch to tell the player that an object is behind the player.
void ActorAccessibility_SetPitchBehindModifier(void* handle, int slot, float mod);

void ActorAccessibility_SetListenerPos(Vec3f* pos, Vec3f* rot);
void ActorAccessibility_SetSoundPos(void* handle, int slot, Vec3f* pos, f32 distToPlayer, f32 maxDistance);

void ActorAccessibility_SetSoundVolume(void* handle, int slot, float volume);
void ActorAccessibility_SetSoundPan(void* handle, int slot, Vec3f* projectedPos);
void ActorAccessibility_SetSoundFilter(void* handle, int slot, float cutoff);
void ActorAccessibility_SeekSound(void* handle, int slot, size_t offset);

    /*
* Play a sound on behalf of an AccessibleActor.
* This version includes automatic sound management: pitch, panning and attenuation parameters will be updated automatically based on the actor's position.
* 
*/
void ActorAccessibility_PlaySoundForActor(AccessibleActor* actor, int slot, s16 sfxId, bool looping);
void ActorAccessibility_StopSoundForActor(AccessibleActor* actor, int slot);
void ActorAccessibility_StopAllSoundsForActor(AccessibleActor* actor);
f32 ActorAccessibility_ComputeCurrentVolume(f32 maxDistance, f32 xzDistToPlayer);
    // Computes a relative angle based on Link's (or some other actor's) current angle.
Vec3s ActorAccessibility_ComputeRelativeAngle(Vec3s* origin, Vec3s* offset);
void ActorAccessibility_InitCues();
// Stuff related to lists of virtual actors.
typedef enum {
    // Similar to the game's actual actor table. Values here start at 10000 just to be extra safe.
    VA_INITIAL = 1000,
    VA_PROTOTYPE, // Remove this one once this thing is working.
    VA_CRAWLSPACE,
    VA_TERRAIN_CUE,
    VA_WALL_CUE,
    VA_CLIMB,
    VA_DOOR,
    VA_AREA_CHANGE,
    VA_MARKER,
    VA_SPIKE,
    VA_GENERAL_HELPER, // Room announcements, action icon and other misc help.
    VA_AUDIO_COMPASS,//Points north.

    VA_FINAL,

} VIRTUAL_ACTOR_TABLE;

typedef enum {
    AREA_KOKIRI = 2000,
    AREA_HYRULE_FIELD,
    AREA_HYRULE_CASTLE,
    AREA_ToT,
    AREA_ZORA,
    AREA_VILLAGE,
    AREA_GORON,
    AREA_DM,
    AREA_GRAVEYARD,
    AREA_LAKE,
    AREA_GERUDO,
    AREA_OASIS,

} AREA_MARKER_TABLE;

#define EVERYWHERE -32768 // Denotes a virtual actor that is global/ omnipresent.

// Get the list of virtual actors for a given scene and room index.
VirtualActorList* ActorAccessibility_GetVirtualActorList(s16 sceneNum, s8 roomNum);
AccessibleActor* ActorAccessibility_AddVirtualActor(VirtualActorList* list, VIRTUAL_ACTOR_TABLE type, PosRot where);
//Parses the loaded seen and converts select polygons (like ladders, spikes and scene exits) into virtual actors.
void ActorAccessibility_InterpretCurrentScene(PlayState* play);
//Convert a collision polygon into a virtual actor.
void ActorAccessibility_PolyToVirtualActor(PlayState* play, CollisionPoly* poly, VIRTUAL_ACTOR_TABLE va, VirtualActorList* destination);
//Report which room of a dungeon the player is in.
 void ActorAccessibility_AnnounceRoomNumber(PlayState* play);
// 
    // External audio engine stuff.
//  Initialize the accessible audio engine.
bool ActorAccessibility_InitAudio();
void ActorAccessibility_ShutdownAudio();
// Combine the games' audio with the output from AccessibleAudioEngine. To be called exclusively from the audio thread.
void ActorAccessibility_MixAccessibleAudioWithGameAudio(int16_t* ogBuffer, uint32_t nFrames);
void ActorAccessibility_HandleSoundExtractionMode(PlayState* play);
//This is called by the audio thread when it's ready to try to pull sfx from the game.
void ActorAccessibility_DoSoundExtractionStep();

#ifdef __cplusplus
}

#endif
