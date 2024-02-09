#include "SetLightList.h"

LightInfo* SetLightList::GetPointer() {
    return lightList.data();
}

size_t SetLightList::GetPointerSize() {
	return lightList.size() * sizeof(LightInfo);
}
