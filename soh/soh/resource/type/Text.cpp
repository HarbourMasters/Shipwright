#include "Text.h"

namespace Ship {
void* Text::GetPointer() {
    return messages.data();
}

size_t Text::GetPointerSize() {
	return messages.size() * sizeof(MessageEntry);
}
} // namespace Ship
