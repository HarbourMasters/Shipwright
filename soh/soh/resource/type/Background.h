#pragma once

#include "resource/Resource.h"

namespace LUS {
class Background : public Resource<uint8_t> {
  public:
    using Resource::Resource;

    Background() : Resource(std::shared_ptr<ResourceInitData>()) {}

    uint8_t* GetPointer();
    size_t GetPointerSize();

    std::vector<uint8_t> Data;
};
}; // namespace LUS
