#include "ObjectExtension.h"

ObjectExtension& ObjectExtension::GetInstance() {
    static ObjectExtension instance;
    return instance;
}

ObjectExtension::Id ObjectExtension::RegisterId() {
    return NextId++;
}

void ObjectExtension::Free(const void* object) {
    if (object == nullptr) {
        return;
    }

    std::erase_if(Data, [&object](const auto& iter) {
        auto const& [key, value] = iter;
        return key.first == object;
    });
}

extern "C" void ObjectExtension_Free(const void* object) {
    ObjectExtension::GetInstance().Free(object);
}
