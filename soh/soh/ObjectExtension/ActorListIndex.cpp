#include "ActorListIndex.h"
#include "soh/ObjectExtension/ObjectExtension.h"

struct ActorListIndex {
    s16 index = -1;
};
static ObjectExtension::Register<ActorListIndex> ActorListIndexRegister;

int16_t GetActorListIndex(const Actor* actor) {
    const ActorListIndex* index = ObjectExtension::GetInstance().Get<ActorListIndex>(actor);
    return index != nullptr ? index->index : ActorListIndex{}.index;
}

void SetActorListIndex(const Actor* actor, int16_t index) {
    ObjectExtension::GetInstance().Set<ActorListIndex>(actor, ActorListIndex{ index });
}