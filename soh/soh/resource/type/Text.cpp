#include "Text.h"

namespace SOH {
MessageEntry* Text::GetPointer() {
    return messages.data();
}

size_t Text::GetPointerSize() {
	return messages.size() * sizeof(MessageEntry);
}
} // namespace SOH
