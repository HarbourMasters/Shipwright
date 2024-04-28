#pragma once

#include "resource/Resource.h"
#include "resource/ResourceFactoryBinary.h"

namespace SOH {
class ResourceFactoryBinaryBackgroundV0 : public Ship::ResourceFactoryBinary {
  public:
    std::shared_ptr<Ship::IResource> ReadResource(std::shared_ptr<Ship::File> file) override;
};
} // namespace SOH
