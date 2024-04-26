#pragma once

#include "Resource.h"
#include "ResourceFactoryBinary.h"

namespace SOH {
class ResourceFactoryBinaryPlayerAnimationV0 : public ShipDK::ResourceFactoryBinary {
  public:
    std::shared_ptr<ShipDK::IResource> ReadResource(std::shared_ptr<ShipDK::File> file) override;
};
} // namespace SOH
