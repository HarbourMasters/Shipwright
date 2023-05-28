#include "SetTimeSettings.h"

namespace LUS {
void* SetTimeSettings::GetPointer() {
    return &settings;
}

size_t SetTimeSettings::GetPointerSize() {
	return sizeof(TimeSettings);
}
} // namespace LUS
