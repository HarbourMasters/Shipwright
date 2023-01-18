#include "SetStartPositionList.h"

namespace Ship {
void* SetStartPositionList::GetPointer() {
    return startPositions.data();
}

size_t SetStartPositionList::GetPointerSize() {
	return startPositions.size() * sizeof(ActorEntry);
}
} // namespace Ship
