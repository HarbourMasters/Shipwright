#include "Scene.h"

namespace SOH {
void* Scene::GetPointer() {
    // Scene is a special type that requries C++ processing. As such, we return nothing.
    return nullptr;
}

size_t Scene::GetPointerSize() {
	return 0;
}
} // namespace SOH
