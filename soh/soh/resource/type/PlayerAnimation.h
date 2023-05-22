#pragma once

#include <vector>
#include <string>
#include "Resource.h"
#include "Vec2f.h"
#include "Vec3f.h"
#include "Color3b.h"

namespace LUS {

class PlayerAnimation : public Resource {
  public:
    using Resource::Resource;

    PlayerAnimation() : Resource(std::shared_ptr<ResourceInitData>()) {}

    void* GetPointer();
    size_t GetPointerSize();

    std::vector<int16_t> limbRotData;
};
} // namespace LUS