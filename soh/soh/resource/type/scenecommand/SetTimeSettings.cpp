#include "SetTimeSettings.h"

namespace Ship {
void* SetTimeSettings::GetPointer() {
    return &settings;
}

size_t SetTimeSettings::GetPointerSize() {
	return sizeof(TimeSettings);
}
} // namespace Ship
