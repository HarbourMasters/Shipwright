#include "RawJson.h"

namespace SOH {
RawJson::RawJson() : Resource(std::shared_ptr<LUS::ResourceInitData>()) {
}

void* RawJson::GetPointer() {
    return &Data;
}

size_t RawJson::GetPointerSize() {
    return DataSize * sizeof(char);
}
} // namespace SOH
