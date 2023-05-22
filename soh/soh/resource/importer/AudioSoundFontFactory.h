#pragma once

#include "Resource.h"
#include "ResourceFactory.h"

namespace LUS {
class AudioSoundFontFactory : public ResourceFactory
{
  public:
    std::shared_ptr<Resource>
    ReadResource(std::shared_ptr<ResourceInitData> initData, std::shared_ptr<BinaryReader> reader) override;
};

class AudioSoundFontFactoryV0 : public ResourceVersionFactory
{
  public:
    void ParseFileBinary(std::shared_ptr<BinaryReader> reader, std::shared_ptr<Resource> resource) override;
};
}; // namespace LUS
