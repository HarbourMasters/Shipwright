#include "SetActorList.h"

namespace LUS {
void* SetActorList::GetPointer() {
    return actorList.data();
}

size_t SetActorList::GetPointerSize() {
	return actorList.size() * sizeof(ActorEntry);
}
} // namespace LUS
