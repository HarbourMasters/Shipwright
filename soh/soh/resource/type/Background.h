#pragma once

#include "resource/Resource.h"

namespace Ship {
class Background : public Resource {
  public:
    void* GetPointer();
    size_t GetPointerSize();

    std::vector<uint8_t> Data;
};
}; // namespace Ship
