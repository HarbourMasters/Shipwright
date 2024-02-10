#pragma once

#include "resource/Resource.h"
#include "resource/ResourceFactoryBinary.h"

class ResourceFactoryBinaryBackgroundV0 : public LUS::ResourceFactoryBinary {
  public:
    std::shared_ptr<LUS::IResource> ReadResource(std::shared_ptr<LUS::File> file) override;
};
