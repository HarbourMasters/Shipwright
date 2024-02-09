#include "Cutscene.h"
#include <libultraship/libultra/gbi.h>

uint32_t* Cutscene::GetPointer() {
    return commands.data();
}

size_t Cutscene::GetPointerSize() {
	return commands.size() * sizeof(uint32_t);
}
