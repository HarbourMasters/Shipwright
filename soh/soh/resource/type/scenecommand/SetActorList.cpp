#include "SetActorList.h"

namespace SOH {
ActorEntry* SetActorList::GetPointer() {
    return actorList.data();
}

size_t SetActorList::GetPointerSize() {
	return actorList.size() * sizeof(ActorEntry);
}
} // namespace SOH
