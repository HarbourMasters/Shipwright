#include "SetWindSettings.h"

WindSettings* SetWindSettings::GetPointer() {
    return &settings;
}

size_t SetWindSettings::GetPointerSize() {
	return sizeof(WindSettings);
}
