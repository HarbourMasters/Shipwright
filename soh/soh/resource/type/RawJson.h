#pragma once

#include "resource/Resource.h"
#include <nlohmann/json.hpp>

namespace SOH {

class RawJson : public LUS::Resource<void> {
  public:
    using Resource::Resource;

    RawJson();

    void* GetPointer() override;
    size_t GetPointerSize() override;

    nlohmann::json Data;
    size_t DataSize;
};
}; // namespace SOH
