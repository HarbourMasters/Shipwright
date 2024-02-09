#include "Path.h"

PathData* Path::GetPointer() {
    return pathData.data();
}

size_t Path::GetPointerSize() {
    return pathData.size() * sizeof(PathData);
}
