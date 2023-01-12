#include "SetCsCamera.h"

namespace Ship {
void* SetCsCamera::GetPointer() {
    return &csCamera;
}

size_t SetCsCamera::GetPointerSize() {
	return sizeof(CsCamera);
}
} // namespace Ship
