#pragma once

#include "resource/Resource.h"

namespace LUS {
class Background : public Resource {
  public:
    using Resource::Resource;

    Background() : Resource(std::shared_ptr<ResourceInitData>()) {}

    void* GetPointer();
    size_t GetPointerSize();

    std::vector<uint8_t> Data;
};
}; // namespace LUS
