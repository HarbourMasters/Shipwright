#pragma once

#include "resource/Resource.h"
#include "resource/ResourceFactoryBinary.h"

namespace SOH {
class ResourceFactoryBinaryBackgroundV0 : public ShipDK::ResourceFactoryBinary {
  public:
    std::shared_ptr<ShipDK::IResource> ReadResource(std::shared_ptr<ShipDK::File> file) override;
};
} // namespace SOH
