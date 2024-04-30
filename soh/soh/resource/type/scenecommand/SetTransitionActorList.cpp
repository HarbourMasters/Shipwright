#include "SetTransitionActorList.h"

namespace SOH {
TransitionActorEntry* SetTransitionActorList::GetPointer() {
    return transitionActorList.data();
}

size_t SetTransitionActorList::GetPointerSize() {
	return transitionActorList.size() * sizeof(TransitionActorEntry);
}
} // namespace SOH
