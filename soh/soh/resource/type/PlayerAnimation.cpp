#include "PlayerAnimation.h"
#include <libultraship/libultra/gbi.h>

namespace SOH {
int16_t* PlayerAnimation::GetPointer() {
    return limbRotData.data();
}

size_t PlayerAnimation::GetPointerSize() {
	return limbRotData.size() * sizeof(int16_t);
}
} // namespace SOH
