#include "EndMarker.h"

namespace LUS {
Marker* EndMarker::GetPointer() {
    return &endMarker;
}

size_t EndMarker::GetPointerSize() {
	return sizeof(Marker);
}
} // namespace LUS
