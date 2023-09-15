#include "SetTransitionActorList.h"

namespace LUS {
TransitionActorEntry* SetTransitionActorList::GetPointer() {
    return transitionActorList.data();
}

size_t SetTransitionActorList::GetPointerSize() {
	return transitionActorList.size() * sizeof(TransitionActorEntry);
}
} // namespace LUS
