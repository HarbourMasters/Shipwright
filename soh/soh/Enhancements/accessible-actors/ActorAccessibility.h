#pragma once
#include <z64.h>

#ifdef __cplusplus
extern "C" {
typedef struct {
    int frameCount; // Incremented every time the callback is called. The callback is free to modify this. Can be used
                    // to implement playback of sounds at regular intervals.
    //Add more state as needed.

}AccessibleActorState;
	void ActorAccessibility_Init();
void ActorAccessibility_TrackNewActor(Actor* actor);
        void ActorAccessibility_RemoveTrackedActor(Actor* actor);

        void ActorAccessibility_RunAccessibilityForActor(Actor* actor);
        void ActorAccessibility_RunAccessibilityForAllActors();

}
#endif
