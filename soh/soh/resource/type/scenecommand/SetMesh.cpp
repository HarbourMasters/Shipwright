#include "SetMesh.h"

namespace SOH {
MeshHeader* SetMesh::GetPointer() {
    return &meshHeader;
}

size_t SetMesh::GetPointerSize() {
	return sizeof(MeshHeader);
}
} // namespace SOH
