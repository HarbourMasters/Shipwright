#pragma once

#include "Resource.h"
#include "ResourceFactoryBinary.h"
#include "ResourceFactoryXML.h"

namespace SOH {
class ResourceFactoryBinaryTextV0 : public LUS::ResourceFactoryBinary {
  public:
    std::shared_ptr<LUS::IResource> ReadResource(std::shared_ptr<LUS::File> file) override;
};

class ResourceFactoryXMLTextV0 : public LUS::ResourceFactoryXML {
  public:
    std::shared_ptr<LUS::IResource> ReadResource(std::shared_ptr<LUS::File> file) override;  
};
} // namespace SOH
