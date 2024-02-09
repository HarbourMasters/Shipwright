#include "SetStartPositionList.h"

ActorEntry* SetStartPositionList::GetPointer() {
    return startPositions.data();
}

size_t SetStartPositionList::GetPointerSize() {
	return startPositions.size() * sizeof(ActorEntry);
}
