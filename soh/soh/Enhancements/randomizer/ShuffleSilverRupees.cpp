#include "ShuffleSilverRupees.h"

extern "C" {
#include "variables.h"
#include "overlays/actors/ovl_En_G_Switch/z_en_g_switch.h"
extern PlayState* gPlayState;
}

void EnGSwitch_RandomizerInit(void* actorRef) {
    Actor* actor = static_cast<Actor*>(actorRef);
    int16_t sceneNum = gPlayState->sceneNum;
    f32 param1 = actor->world.pos.x;
    f32 param2 = actor->world.pos.z;

    //Two particular silver rupees in Master Quest Shadow Temple have
    // the same X and Z coordinates. We have to make an exception here
    // and look them up by X and Y coordinates instead.
    if (sceneNum == SCENE_SHADOW_TEMPLE && param1 == 2110.0f && param2 == 3372.0f) {
        param2 = actor->world.pos.y;
    }
    EnGSwitch* srActor = static_cast<EnGSwitch*>(actorRef);
    srActor->srIdentity = IdentifySilverRupee(sceneNum, param1, param2);
}