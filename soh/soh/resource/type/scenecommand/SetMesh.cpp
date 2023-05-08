#include "SetMesh.h"

namespace LUS {
void* SetMesh::GetPointer() {
    return &meshHeader;
}

size_t SetMesh::GetPointerSize() {
	return sizeof(MeshHeader);
}
} // namespace LUS
