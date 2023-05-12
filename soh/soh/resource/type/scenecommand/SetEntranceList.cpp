#include "SetEntranceList.h"

namespace LUS {
void* SetEntranceList::GetPointer() {
    return entrances.data();
}

size_t SetEntranceList::GetPointerSize() {
	return entrances.size() * sizeof(EntranceEntry);
}
} // namespace LUS
