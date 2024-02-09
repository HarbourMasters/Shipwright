#pragma once

#include "Resource.h"
#include "ResourceFactory.h"


class CollisionHeaderFactory : public ResourceFactory {
  public:
    std::shared_ptr<LUS::IResource>
    ReadResource(std::shared_ptr<LUS::ResourceInitData> initData, std::shared_ptr<BinaryReader> reader) override;
    std::shared_ptr<LUS::IResource>
    ReadResourceXML(std::shared_ptr<LUS::ResourceInitData> initData, tinyxml2::XMLElement *reader) override;
};

class CollisionHeaderFactoryV0 : public ResourceVersionFactory {
  public:
    void ParseFileBinary(std::shared_ptr<BinaryReader> reader, std::shared_ptr<LUS::IResource> resource) override;
    void ParseFileXML(tinyxml2::XMLElement* reader, std::shared_ptr<LUS::IResource> resource) override;
};
}; // namespace LUS
