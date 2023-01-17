#include "Cutscene.h"
#include <libultraship/libultra/gbi.h>

namespace Ship {
void* Cutscene::GetPointer() {
    return commands.data();
}

size_t Cutscene::GetPointerSize() {
	return commands.size() * sizeof(uint32_t);
}
} // namespace Ship