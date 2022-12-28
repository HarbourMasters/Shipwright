#pragma once

#include "Resource.h"
#include "ResourceFactory.h"

namespace Ship {
class TextFactory : public ResourceFactory
{
  public:
    std::shared_ptr<Resource> ReadResource(std::shared_ptr<BinaryReader> reader);
};

class TextFactoryV0 : public ResourceVersionFactory
{
  public:
    void ParseFileBinary(std::shared_ptr<BinaryReader> reader, std::shared_ptr<Resource> resource) override;
};
}; // namespace Ship
