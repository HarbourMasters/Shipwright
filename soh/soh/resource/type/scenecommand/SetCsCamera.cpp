#include "SetCsCamera.h"

namespace LUS {
void* SetCsCamera::GetPointer() {
    return &csCamera;
}

size_t SetCsCamera::GetPointerSize() {
	return sizeof(CsCamera);
}
} // namespace LUS
