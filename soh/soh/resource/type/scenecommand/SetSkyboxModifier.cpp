#include "SetSkyboxModifier.h"

namespace SOH {
SkyboxModifier* SetSkyboxModifier::GetPointer() {
    return &modifier;
}

size_t SetSkyboxModifier::GetPointerSize() {
	return sizeof(SkyboxModifier);
}
} // namespace SOH
