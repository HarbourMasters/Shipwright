#include "SetActorList.h"

namespace LUS {
ActorEntry* SetActorList::GetPointer() {
    return actorList.data();
}

size_t SetActorList::GetPointerSize() {
	return actorList.size() * sizeof(ActorEntry);
}
} // namespace LUS
