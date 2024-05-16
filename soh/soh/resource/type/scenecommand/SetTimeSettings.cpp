#include "SetTimeSettings.h"

namespace SOH {
TimeSettings* SetTimeSettings::GetPointer() {
    return &settings;
}

size_t SetTimeSettings::GetPointerSize() {
	return sizeof(TimeSettings);
}
} // namespace SOH
