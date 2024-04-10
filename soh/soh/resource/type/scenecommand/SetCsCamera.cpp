#include "SetCsCamera.h"

namespace SOH {
CsCamera* SetCsCamera::GetPointer() {
    return &csCamera;
}

size_t SetCsCamera::GetPointerSize() {
	return sizeof(CsCamera);
}
} // namespace SOH
