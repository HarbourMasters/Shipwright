#include "SetEchoSettings.h"

namespace LUS {
EchoSettings* SetEchoSettings::GetPointer() {
    return &settings;
}

size_t SetEchoSettings::GetPointerSize() {
	return sizeof(EchoSettings);
}
} // namespace LUS
