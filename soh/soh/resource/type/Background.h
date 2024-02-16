#pragma once

#include "resource/Resource.h"

namespace SOH {
class Background : public LUS::Resource<uint8_t> {
  public:
    using Resource::Resource;

    Background() : Resource(std::shared_ptr<LUS::ResourceInitData>()) {}

    uint8_t* GetPointer();
    size_t GetPointerSize();

    std::vector<uint8_t> Data;
};
}; // namespace LUS
