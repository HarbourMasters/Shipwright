#include "SetEchoSettings.h"

EchoSettings* SetEchoSettings::GetPointer() {
    return &settings;
}

size_t SetEchoSettings::GetPointerSize() {
	return sizeof(EchoSettings);
}
