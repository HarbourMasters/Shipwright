#include "SetCutscenes.h"

namespace SOH {
uint32_t* SetCutscenes::GetPointer() {
    if (cutscene == nullptr) {
        return nullptr;
    }
    return cutscene->GetPointer();
}

size_t SetCutscenes::GetPointerSize() {
    if (cutscene == nullptr) {
        return 0;
    }
	return cutscene->GetPointerSize();
}
} // namespace SOH
