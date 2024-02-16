#include "EndMarker.h"

namespace SOH {
Marker* EndMarker::GetPointer() {
    return &endMarker;
}

size_t EndMarker::GetPointerSize() {
	return sizeof(Marker);
}
} // namespace SOH
