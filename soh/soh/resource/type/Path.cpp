#include "Path.h"

namespace Ship {
void* Path::GetPointer() {
    return pathData.data();
}

size_t Path::GetPointerSize() {
	return pathData.size() * sizeof(PathData);
}
} // namespace Ship
