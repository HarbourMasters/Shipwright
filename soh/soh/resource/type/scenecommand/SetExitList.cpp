#include "SetExitList.h"

namespace SOH {
uint16_t* SetExitList::GetPointer() {
    return exits.data();
}

size_t SetExitList::GetPointerSize() {
	return exits.size() * sizeof(int16_t);
}
} // namespace SOH
