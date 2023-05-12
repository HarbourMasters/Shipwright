#include "Path.h"

namespace LUS {
void* Path::GetPointer() {
    return pathData.data();
}

size_t Path::GetPointerSize() {
	return pathData.size() * sizeof(PathData);
}
} // namespace LUS
