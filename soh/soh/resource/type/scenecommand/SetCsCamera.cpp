#include "SetCsCamera.h"

CsCamera* SetCsCamera::GetPointer() {
    return &csCamera;
}

size_t SetCsCamera::GetPointerSize() {
	return sizeof(CsCamera);
}
