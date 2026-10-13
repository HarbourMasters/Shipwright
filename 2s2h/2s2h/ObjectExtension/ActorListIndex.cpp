#include "ActorListIndex.h"
#include "2s2h/ObjectExtension/ObjectExtension.h"

struct ActorListIndex {
    s16 index = -1;
};
static ObjectExtension::Register<ActorListIndex> ActorListIndexRegister;
s16 currentActorListIndex = -1;

int16_t GetActorListIndex(const Actor* actor) {
    const ActorListIndex* index = ObjectExtension::GetInstance().Get<ActorListIndex>(actor);
    return index != nullptr ? index->index : -1;
}

void SetActorListIndex(const Actor* actor, int16_t index) {
    ObjectExtension::GetInstance().Set<ActorListIndex>(actor, ActorListIndex{ index });
}
