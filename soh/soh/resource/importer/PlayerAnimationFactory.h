#pragma once

#include "Resource.h"
#include "ResourceFactory.h"

class ResourceFactoryBinaryPlayerAnimationV0 : public LUS::ResourceFactory {
  public:
    std::shared_ptr<LUS::IResource> ReadResource(std::shared_ptr<LUS::File> file) override;
};
