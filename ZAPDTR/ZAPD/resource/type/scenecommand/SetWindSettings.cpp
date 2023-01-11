#include "SetWindSettings.h"

namespace Ship {
void* SetWindSettings::GetPointer() {
    return &settings;
}

size_t SetWindSettings::GetPointerSize() {
	return sizeof(WindSettings);
}
} // namespace Ship
