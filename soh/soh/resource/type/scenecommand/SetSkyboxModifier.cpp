#include "SetSkyboxModifier.h"

SkyboxModifier* SetSkyboxModifier::GetPointer() {
    return &modifier;
}

size_t SetSkyboxModifier::GetPointerSize() {
	return sizeof(SkyboxModifier);
}
