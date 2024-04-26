#pragma once

#include "resource/Resource.h"

namespace SOH {
class Background : public ShipDK::Resource<uint8_t> {
  public:
    using Resource::Resource;

    Background() : Resource(std::shared_ptr<ShipDK::ResourceInitData>()) {}

    uint8_t* GetPointer();
    size_t GetPointerSize();

    std::vector<uint8_t> Data;
};
}; // namespace LUS
