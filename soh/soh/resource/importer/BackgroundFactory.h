#pragma once

#include "resource/Resource.h"
#include "resource/ResourceFactory.h"

namespace LUS {
class BackgroundFactory : public ResourceFactory {
  public:
    std::shared_ptr<IResource>
    ReadResource(std::shared_ptr<ResourceInitData> initData, std::shared_ptr<BinaryReader> reader) override;
};

class BackgroundFactoryV0 : public ResourceVersionFactory {
  public:
    void ParseFileBinary(std::shared_ptr<BinaryReader> reader, std::shared_ptr<IResource> resource) override;
};
}; // namespace LUS
