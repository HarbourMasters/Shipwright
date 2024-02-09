#include "SetMesh.h"

MeshHeader* SetMesh::GetPointer() {
    return &meshHeader;
}

size_t SetMesh::GetPointerSize() {
	return sizeof(MeshHeader);
}
