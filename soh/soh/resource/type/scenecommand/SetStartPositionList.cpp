#include "SetStartPositionList.h"

namespace LUS {
void* SetStartPositionList::GetPointer() {
    return startPositions.data();
}

size_t SetStartPositionList::GetPointerSize() {
	return startPositions.size() * sizeof(ActorEntry);
}
} // namespace LUS
