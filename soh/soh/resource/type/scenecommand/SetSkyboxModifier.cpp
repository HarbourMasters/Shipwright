#include "SetSkyboxModifier.h"

namespace LUS {
SkyboxModifier* SetSkyboxModifier::GetPointer() {
    return &modifier;
}

size_t SetSkyboxModifier::GetPointerSize() {
	return sizeof(SkyboxModifier);
}
} // namespace LUS
