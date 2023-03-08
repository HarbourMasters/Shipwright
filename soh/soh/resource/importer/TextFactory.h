#pragma once

#include "Resource.h"
#include "ResourceFactory.h"

namespace Ship {
class TextFactory : public ResourceFactory
{
  public:
    std::shared_ptr<Resource> ReadResource(uint32_t version, std::shared_ptr<BinaryReader> reader);
    std::shared_ptr<Resource> ReadResourceXML(uint32_t version, tinyxml2::XMLElement* reader);
};

class TextFactoryV0 : public ResourceVersionFactory
{
  public:
    void ParseFileBinary(std::shared_ptr<BinaryReader> reader, std::shared_ptr<Resource> resource) override;
    void ParseFileXML(tinyxml2::XMLElement* reader, std::shared_ptr<Resource> resource) override;
};
}; // namespace Ship
