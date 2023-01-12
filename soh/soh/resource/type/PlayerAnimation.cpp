#include "PlayerAnimation.h"
#include <libultraship/libultra/gbi.h>

namespace Ship {
void* PlayerAnimation::GetPointer() {
    return limbRotData.data();
}

size_t PlayerAnimation::GetPointerSize() {
	return limbRotData.size() * sizeof(int16_t);
}
} // namespace Ship