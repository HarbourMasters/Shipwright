#include "SetActorList.h"

namespace Ship {
void* SetActorList::GetPointer() {
    return actorList.data();
}

size_t SetActorList::GetPointerSize() {
	return actorList.size() * sizeof(ActorEntry);
}
} // namespace Ship
