#pragma once

#include "resource/Resource.h"
#include "resource/ResourceFactory.h"


class BackgroundFactory : public ResourceFactory {
  public:
    std::shared_ptr<LUS::IResource>
    ReadResource(std::shared_ptr<LUS::ResourceInitData> initData, std::shared_ptr<BinaryReader> reader) override;
};

class BackgroundFactoryV0 : public ResourceVersionFactory {
  public:
    void ParseFileBinary(std::shared_ptr<BinaryReader> reader, std::shared_ptr<LUS::IResource> resource) override;
};
}; // namespace LUS
