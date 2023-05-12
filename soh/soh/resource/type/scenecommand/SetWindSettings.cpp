#include "SetWindSettings.h"

namespace LUS {
void* SetWindSettings::GetPointer() {
    return &settings;
}

size_t SetWindSettings::GetPointerSize() {
	return sizeof(WindSettings);
}
} // namespace LUS
