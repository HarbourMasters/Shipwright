#pragma once

#include "Resource.h"
#include "ResourceFactory.h"

namespace Ship {
class SkeletonFactory : public ResourceFactory
{
  public:
    std::shared_ptr<Resource> ReadResource(std::shared_ptr<ResourceManager> resourceMgr,
                                           std::shared_ptr<ResourceInitData> initData,
                                           std::shared_ptr<BinaryReader> reader) override;
    std::shared_ptr<Resource> ReadResourceXML(std::shared_ptr<ResourceManager> resourceMgr,
                                              std::shared_ptr<ResourceInitData> initData,
                                              tinyxml2::XMLElement* reader) override;
};

class SkeletonFactoryV0 : public ResourceVersionFactory
{
  public:
    void ParseFileBinary(std::shared_ptr<BinaryReader> reader, std::shared_ptr<Resource> resource) override;
    void ParseFileXML(tinyxml2::XMLElement* reader, std::shared_ptr<Resource> resource) override;
};
}; // namespace Ship

