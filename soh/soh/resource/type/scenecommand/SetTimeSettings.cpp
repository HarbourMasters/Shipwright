#include "SetTimeSettings.h"

TimeSettings* SetTimeSettings::GetPointer() {
    return &settings;
}

size_t SetTimeSettings::GetPointerSize() {
	return sizeof(TimeSettings);
}
