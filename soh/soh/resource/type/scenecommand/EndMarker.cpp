#include "EndMarker.h"

namespace Ship {
void* EndMarker::GetPointer() {
    return &endMarker;
}

size_t EndMarker::GetPointerSize() {
	return sizeof(Marker);
}
} // namespace Ship
