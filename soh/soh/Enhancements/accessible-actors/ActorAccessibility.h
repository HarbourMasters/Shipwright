#pragma once
#include <z64.h>

struct AccessibleActor;
// A callback that is run regularely as the game progresses in order to provide accessibility services for an actor.

typedef void (*ActorAccessibilityCallback)(AccessibleActor*);

struct VirtualActorList;

#define AIM_ALL 0x0F
#define AIM_BOW 0x01
#define AIM_SLING 0x02
#define AIM_SHOOT 0x03
#define AIM_HOOK 0x04
#define AIM_BOOM 0x08
#define AIM_CUP 0x10

struct ActorAccessibilityPolicy {
    const char* englishName;
    ActorAccessibilityCallback callback; // If set, it will be called once every n frames.
                                         // If null, then sfx will be played once every n frames.
    s16 sound;                           // The ID of a sound to play. Ignored if the callback is set.
    bool runsAlways;                     // If set, then the distance policy is ignored.

    u8 n;         // How often to run callback in frames.
    f32 distance; // Maximum xz distance from player before actor should be considered out of range.
    f32 ydist;    // Maximum y distance from player before actor should be considered out of range.
    f32 pitch;
    f32 volume;
    f32 pitchModifier;
    // Aim assist settings.
    struct {
        u8 isProvider; // determines whether or not this actor supports aim assist.
        s16 sfx;       // The sound to play when this actor provides aim assist. Uses sound slot 9.
        f32 tolerance; // How close to center of actor does Link have to aim to consider it lined up.
    } aimAssist;
};

// Accessible actor object. This can be a "real" actor (one that corresponds to an actual actor in the game) or a
// "virtual" actor (which does not actually exist in the game, but is used to create extra sounds for the player).
// One potential use of virtual actors is to place sounds at static platforms or other things that aren't represented by
// actors.

struct AccessibleActor {
    uint64_t instanceID;

    Actor* actor; // null for virtual actors
    s16 id;       // For real actors, copy actor ID. For virtual actors we have our own table of values which
                  // are out of range for real actors.
    f32 yDistToPlayer;
    f32 xzDistToPlayer;
    f32 xyzDistToPlayer;
    Vec3f pos;
    Vec3f projectedPos;
    PlayState* play;
    u8 isDrawn; // Do we just never play accessibility sounds for actors that aren't drawn?

    u16 frameCount; // Incremented every time the callback is called. The callback is free to modify this. Can be used
                    // to implement playback of sounds at regular intervals.
    f32 baseVolume;
    f32 currentVolume;
    f32 basePitch;

    f32 currentPitch;
    s16 sceneIndex; // If this actor represents a scene transition, then this will contain the destination scene index.
                    // Zero otherwise.
    u8 managedSoundSlots; // These have their attenuation and panning parameters updated every frame automatically.
    u8 aimFramesSinceAimAssist; // Used for rate-based vertical aim assist.
    u8 aimFrequency; // How often the sound will be played. Lower frequencies indicate vertical aim is getting closer.

    ActorAccessibilityPolicy policy; // A copy, so it can be customized on a per-actor basis if needed.
};

struct AimAssistProps {
    f32 pitch;
    f32 volume;
    f32 pan;
};

struct TerrainCueState;
void DeleteTerrainCueState(TerrainCueState*);
TerrainCueState* InitTerrainCueState(AccessibleActor*);
void RunTerrainCueState(TerrainCueState*, PlayState*);

// Initialize accessibility.
void ActorAccessibility_Init();
void ActorAccessibility_InitActors();
void ActorAccessibility_Shutdown();
void ActorAccessibility_InitPolicy(ActorAccessibilityPolicy* policy, const char* englishName);
void ActorAccessibility_InitPolicy(ActorAccessibilityPolicy* policy, const char* englishName,
                                   ActorAccessibilityCallback callback);
void ActorAccessibility_InitPolicy(ActorAccessibilityPolicy* policy, const char* englishName, s16 sfx);

uint64_t ActorAccessibility_GetNextID();
void ActorAccessibility_TrackNewActor(Actor* actor);
void ActorAccessibility_RemoveTrackedActor(Actor* actor);
void ActorAccessibility_AddSupportedActor(s16 type, ActorAccessibilityPolicy policy);
void ActorAccessibility_AddTerrainCues(AccessibleActor* actor);

void ActorAccessibility_RunAccessibilityForActor(PlayState* play, AccessibleActor* actor);
void ActorAccessibility_RunAccessibilityForAllActors(PlayState* play);
/*
 *Play sounds (usually from the game) using the external sound engine. This is probably not the function you want to
 *call most of the time (see below). handle: pointer to an arbitrary object. This object can be anything as it's only
 *used as a classifier, but it's recommended that you use an AccessibleActor* as your handle whenever possible. Using
 *AccessibleActor* as the handle gives you automatic cleanup when the actor is killed. slot: Allows multiple sounds to
 *be assigned to a single handle. The maximum number of slots per actor is 10 by default (but can be controlled by
 *modifying AAE_SLOTS_PER_HANDLE). sfxId: one of the game's sfx IDs. Note that this plays prerendered sounds which you
 *must have previously prepared. looping: whether to play the sound just once or on a continuous loop.
 */
void ActorAccessibility_PlaySound(void* actor, int slot, s16 sfxId);
// Stop a sound. Todo: consider making this a short fade instead of just cutting it off.
void ActorAccessibility_StopSound(void* handle, int slot);
void ActorAccessibility_StopAllSounds(void* handle);

void ActorAccessibility_SetSoundPitch(void* handle, int slot, float pitch);
// When we don't have access to something super fancy (such as HRTF), blind-accessible games generally use a change in
// pitch to tell the player that an object is behind the player.
void ActorAccessibility_SetPitchBehindModifier(void* handle, int slot, float mod);

void ActorAccessibility_SetListenerPos(Vec3f* pos, Vec3f* rot);
void ActorAccessibility_SetSoundPos(void* handle, int slot, Vec3f* pos, f32 distToPlayer, f32 maxDistance);

void ActorAccessibility_SetSoundVolume(void* handle, int slot, float volume);
void ActorAccessibility_SetSoundPan(void* handle, int slot, float pan);
void ActorAccessibility_SetSoundFilter(void* handle, int slot, float cutoff);
void ActorAccessibility_SeekSound(void* handle, int slot, size_t offset);

/*
 * Play a sound on behalf of an AccessibleActor.
 * This version includes automatic sound management: pitch, panning and attenuation parameters will be updated
 * automatically based on the actor's position.
 *
 */
void ActorAccessibility_PlaySoundForActor(AccessibleActor* actor, int slot, s16 sfxId);

void ActorAccessibility_StopSoundForActor(AccessibleActor* actor, int slot);
void ActorAccessibility_StopAllSoundsForActor(AccessibleActor* actor);
f32 ActorAccessibility_ComputeCurrentVolume(f32 maxDistance, f32 xzDistToPlayer);
// Computes a relative angle based on Link's (or some other actor's) current angle.
Vec3s ActorAccessibility_ComputeRelativeAngle(Vec3s* origin, Vec3s* offset);
// Stuff related to lists of virtual actors.
typedef enum {
    // Similar to the game's actual actor table
    VA_INITIAL = 1000,
    VA_CRAWLSPACE,
    VA_TERRAIN_CUE,
    VA_WALL_CUE,
    VA_VOID,
    VA_CLIMB,
    VA_DOOR,
    VA_AREA_CHANGE,
    VA_MARKER,
    VA_MAX,
} VIRTUAL_ACTOR_TABLE;

// Get the list of virtual actors for a given scene and room index.
VirtualActorList* ActorAccessibility_GetVirtualActorList(s16 sceneNum, s8 roomNum);
AccessibleActor* ActorAccessibility_AddVirtualActor(VirtualActorList* list, VIRTUAL_ACTOR_TABLE type, Vec3f where);
// Parses the loaded seen and converts select polygons (like ladders, spikes and scene exits) into virtual actors.
void ActorAccessibility_InterpretCurrentScene(PlayState* play);
// Convert a collision polygon into a virtual actor.
void ActorAccessibility_PolyToVirtualActor(PlayState* play, CollisionPoly* poly, VIRTUAL_ACTOR_TABLE va,
                                           VirtualActorList* destination);
// Report which room of a dungeon the player is in.
void ActorAccessibility_AnnounceRoomNumber(PlayState* play);
// Aim cue support.
AimAssistProps ActorAccessibility_ProvideAimAssistForActor(AccessibleActor* actor);
// External audio engine stuff.
//  Initialize the accessible audio engine.
bool ActorAccessibility_InitAudio();
void ActorAccessibility_ShutdownAudio();
// Combine the games' audio with the output from AccessibleAudioEngine. To be called exclusively from the audio thread.
void ActorAccessibility_MixAccessibleAudioWithGameAudio(int16_t* ogBuffer, uint32_t nFrames);
void ActorAccessibility_HandleSoundExtractionMode(PlayState* play);
// This is called by the audio thread when it's ready to try to pull sfx from the game.
void ActorAccessibility_DoSoundExtractionStep();

void ActorAccessibility_GeneralHelper(PlayState* play);
void ActorAccessibility_AudioGlossary(PlayState* play);
