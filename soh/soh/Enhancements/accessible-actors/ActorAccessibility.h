#pragma once
#include <z64.h>

#ifdef __cplusplus
extern "C" {
#endif
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
    f32 distance; // Maximum distance from player before the actor should be considered out of range.
    f32 pitch;
    f32 volume;
    bool runsAlways; // If set, then the distance policy is ignored.
    ActorAccessibilityUserDataInit initUserData;
    ActorAccessibilityUserDataCleanup cleanupUserData;
    u8 param;

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
    f32 xzDistToPlayer;
    PosRot world;
    Vec3f projectedPos;
    PlayState* play;
    u8 isDrawn; // Do we just never play accessibility sounds for actors that aren't drawn?

    int frameCount; // Incremented every time the callback is called. The callback is free to modify this. Can be used
                    // to implement playback of sounds at regular intervals.
    f32 baseVolume;
    f32 currentVolume;
    f32 basePitch;
    s16 variety;

    f32 currentPitch;
    s8 currentReverb;
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
// External audio engine stuff.
//  Initialize the accessible audio engine.
bool ActorAccessibility_InitAudio();
void ActorAccessibility_ShutdownAudio();
// Combine the games' audio with the output from AccessibleAudioEngine. To be called exclusively from the audio thread.
void ActorAccessibility_MixAccessibleAudioWithGameAudio(int16_t* ogBuffer, uint32_t nFrames);
// Play a sound using the external audio engine.
void ActorAccessibility_PlayExternalSound(void* handle, const char* path);
// Call once per frame to tell the audio engine to start working on the latest batch of queued instructions.
void ActorAccessibility_PrepareNextAudioFrame();
// Call once at start of game to build a complete cache of the game's audio samples. This differs from the game's own
// sample cache as this is fully decoded for use with the external player.
void ActorAccessibility_DecodeAndCacheSamples();
//Decode an audio sample to a wav file. This is used for R&D; it is not called during normal opperation.
void ActorAccessibility_DecodeSampleToFile(SoundFontSample* sample, const char* path);
void ActorAccessibility_HandleSoundExtractionMode(PlayState* play);
//This is called by the audio thread when it's ready to try to pull sfx from the game.
void ActorAccessibility_DoSoundExtractionStep();
void ActorAccessibility_ReportSynthesisLoop(int noteLength);
void ActorAccessibility_LatencyControl(int value);

#ifdef __cplusplus
}

#endif
