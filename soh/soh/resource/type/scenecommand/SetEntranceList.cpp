#include "SetEntranceList.h"

namespace LUS {
EntranceEntry* SetEntranceList::GetPointer() {
    return entrances.data();
}

size_t SetEntranceList::GetPointerSize() {
	return entrances.size() * sizeof(EntranceEntry);
}
} // namespace LUS
