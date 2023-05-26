#pragma once
#include <z64.h>
#include <overlays/actors/ovl_Obj_Switch/z_obj_switch.h>

#ifdef __cplusplus
extern "C" {
struct AccessibleActor;
typedef struct AccessibleActor AccessibleActor;

typedef void (*ActorAccessibilityCallback)(AccessibleActor*);

typedef struct {

const char* englishName;

    ActorAccessibilityCallback callback; // If set, it will be called once every n frames. If null, then sfx will be
                                         // played once every n frames.
    s16 sound;                           // The ID of a sound to play. Ignored if the callback is set.

    int n;        // How often to run the callback in frames.
    f32 distance; // Maximum distance from player before the actor should be considered out of range.
    f32 pitch;
    f32 volume;

} ActorAccessibilityPolicy;
// Accessible actor object. This can be a "real" actor (one that corresponds to an actual actor in the game) or a
// "virtual" actor (which does not actually exist in the game, but is used to create extra sounds for the player).

struct AccessibleActor
{
    uint64_t instanceID;

    Actor* actor; // This can be null for a fake actor.
    s16 id;       // For real actors, we copy the ID of the actor. For fake actors we have our own table of values.
    s16 params;
    u32 flags;
    f32 scaley;
    s16 eyeTex;
    f32 xzDistToPlayer;
    Vec3f projectedPos;
    PlayState* play;
    ObjSwitch* sw;
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
};


void ActorAccessibility_Init();
void ActorAccessibility_TrackNewActor(Actor* actor, ObjSwitch* sw);
        void ActorAccessibility_RemoveTrackedActor(Actor* actor);
void ActorAccessibility_AddSupportedActor(s16 type, const char* englishName, ActorAccessibilityCallback callback, int frames = 20,
                                          float distance = 1000, f32 pitch = 1.0, f32 volume = 1.0, s16 sfx = 0);

            void ActorAccessibility_RunAccessibilityForActor(PlayState* play, AccessibleActor* actor);
        void ActorAccessibility_RunAccessibilityForAllActors(PlayState* play);
        void ActorAccessibility_PlaySpecialSound(AccessibleActor* actor, s16 sfxId);
        }
#endif
