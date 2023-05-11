#include "SetEchoSettings.h"

namespace LUS {
void* SetEchoSettings::GetPointer() {
    return &settings;
}

size_t SetEchoSettings::GetPointerSize() {
	return sizeof(EchoSettings);
}
} // namespace LUS
