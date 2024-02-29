#include "SetWindSettings.h"

namespace SOH {
WindSettings* SetWindSettings::GetPointer() {
    return &settings;
}

size_t SetWindSettings::GetPointerSize() {
	return sizeof(WindSettings);
}
} // namespace SOH
