#include "SetEchoSettings.h"

namespace SOH {
EchoSettings* SetEchoSettings::GetPointer() {
    return &settings;
}

size_t SetEchoSettings::GetPointerSize() {
	return sizeof(EchoSettings);
}
} // namespace SOH
