#include "SetTimeSettings.h"

namespace LUS {
TimeSettings* SetTimeSettings::GetPointer() {
    return &settings;
}

size_t SetTimeSettings::GetPointerSize() {
	return sizeof(TimeSettings);
}
} // namespace LUS
