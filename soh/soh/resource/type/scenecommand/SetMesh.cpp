#include "SetMesh.h"

namespace LUS {
MeshHeader* SetMesh::GetPointer() {
    return &meshHeader;
}

size_t SetMesh::GetPointerSize() {
	return sizeof(MeshHeader);
}
} // namespace LUS
