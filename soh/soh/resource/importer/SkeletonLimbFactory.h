#pragma once

#include "Resource.h"
#include "ResourceFactoryBinary.h"
#include "ResourceFactoryXML.h"

namespace SOH {
class ResourceFactoryBinarySkeletonLimbV0 : public ShipDK::ResourceFactoryBinary {
  public:
    std::shared_ptr<ShipDK::IResource> ReadResource(std::shared_ptr<ShipDK::File> file) override;
};

class ResourceFactoryXMLSkeletonLimbV0 : public ShipDK::ResourceFactoryXML {
  public:
    std::shared_ptr<ShipDK::IResource> ReadResource(std::shared_ptr<ShipDK::File> file) override;  
};
} // namespace SOH
