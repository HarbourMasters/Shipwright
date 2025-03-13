#include "ActorListIndex.h"
#include "soh/ShipInit.hpp"
#include "soh/Enhancements/game-interactor/GameInteractor.h"

ActorExtensionId actorListIndexActorExt = 0;
int16_t currentActorListIndex = -1;

static RegisterShipInitFunc initFunc(
    []() {
        if (actorListIndexActorExt == 0) {
            actorListIndexActorExt = ActorExtension_CreateForAll(sizeof(int16_t));
        }
    },
    {});

int16_t GetActorListIndex(Actor* actor) {
    int16_t* listIndex = (int16_t*)ActorExtension_Get(actor, actorListIndexActorExt);
    if (listIndex == nullptr) {
        return -1;
    }
    return *listIndex;
}

void SetActorListIndex(Actor* actor, int16_t index) {
    int16_t* listIndex = (int16_t*)ActorExtension_Get(actor, actorListIndexActorExt);

    if (listIndex == nullptr) {
        assert(false);
    } else {
        *listIndex = index;
    }
}