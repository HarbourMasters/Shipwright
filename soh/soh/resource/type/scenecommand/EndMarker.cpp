#include "EndMarker.h"

namespace LUS {
void* EndMarker::GetPointer() {
    return &endMarker;
}

size_t EndMarker::GetPointerSize() {
	return sizeof(Marker);
}
} // namespace LUS
