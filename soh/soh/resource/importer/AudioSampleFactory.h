#pragma once

#include "Resource.h"
#include "ResourceFactory.h"

namespace LUS {
class AudioSampleFactory : public ResourceFactory
{
  public:
    std::shared_ptr<IResource>
    ReadResource(std::shared_ptr<ResourceInitData> initData, std::shared_ptr<BinaryReader> reader) override;
};

class AudioSampleFactoryV0 : public ResourceVersionFactory
{
  public:
    void ParseFileBinary(std::shared_ptr<BinaryReader> reader, std::shared_ptr<IResource> resource) override;
};
}; // namespace LUS
