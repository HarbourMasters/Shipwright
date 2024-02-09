#include "EndMarker.h"

Marker* EndMarker::GetPointer() {
    return &endMarker;
}

size_t EndMarker::GetPointerSize() {
	return sizeof(Marker);
}
