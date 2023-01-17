#include "SetMesh.h"

namespace Ship {
void* SetMesh::GetPointer() {
    return &meshHeader;
}

size_t SetMesh::GetPointerSize() {
	return sizeof(MeshHeader);
}
} // namespace Ship
