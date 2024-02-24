#include "SetLightList.h"

namespace SOH {
LightInfo* SetLightList::GetPointer() {
    return lightList.data();
}

size_t SetLightList::GetPointerSize() {
	return lightList.size() * sizeof(LightInfo);
}
} // namespace SOH
