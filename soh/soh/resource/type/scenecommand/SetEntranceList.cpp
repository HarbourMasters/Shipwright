#include "SetEntranceList.h"

EntranceEntry* SetEntranceList::GetPointer() {
    return entrances.data();
}

size_t SetEntranceList::GetPointerSize() {
	return entrances.size() * sizeof(EntranceEntry);
}
