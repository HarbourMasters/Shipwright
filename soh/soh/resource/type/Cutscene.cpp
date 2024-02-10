#include "Cutscene.h"
#include <libultraship/libultra/gbi.h>

namespace LUS {
uint32_t* Cutscene::GetPointer() {
    return commands.data();
}

size_t Cutscene::GetPointerSize() {
	return commands.size() * sizeof(uint32_t);
}
} // namespace LUS