#include "Scene.h"

namespace LUS {
void* Scene::GetPointer() {
    // Scene is a special type that requries C++ processing. As such, we return nothing.
    return nullptr;
}

size_t Scene::GetPointerSize() {
	return 0;
}
} // namespace LUS
