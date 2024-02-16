#include "SetEntranceList.h"

namespace SOH {
EntranceEntry* SetEntranceList::GetPointer() {
    return entrances.data();
}

size_t SetEntranceList::GetPointerSize() {
	return entrances.size() * sizeof(EntranceEntry);
}
} // namespace SOH
