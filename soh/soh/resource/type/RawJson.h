#pragma once

#include "Resource.h"

namespace SOH {
class RawJson : public LUS::Resource<void> {
  public:
    using Resource::Resource;

    RawJson();

    void* GetPointer() override;
    size_t GetPointerSize() override;

    std::vector<uint8_t> Data;
};
}; // namespace LUS
