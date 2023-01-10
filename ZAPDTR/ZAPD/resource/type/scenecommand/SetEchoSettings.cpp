#include "SetEchoSettings.h"

namespace Ship {
void* SetEchoSettings::GetPointer() {
    return &settings;
}

size_t SetEchoSettings::GetPointerSize() {
	return sizeof(EchoSettings);
}
} // namespace Ship
