#include "SetExitList.h"

namespace Ship {
void* SetExitList::GetPointer() {
    return exits.data();
}

size_t SetExitList::GetPointerSize() {
	return exits.size() * sizeof(int16_t);
}
} // namespace Ship
