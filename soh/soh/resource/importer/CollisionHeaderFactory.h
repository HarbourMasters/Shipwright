#pragma once

#include "Resource.h"
#include "ResourceFactory.h"

namespace Ship {
class CollisionHeaderFactory : public ResourceFactory {
  public:
    std::shared_ptr<Resource> ReadResource(uint32_t version, std::shared_ptr<BinaryReader> reader);
};

class CollisionHeaderFactoryV0 : public ResourceVersionFactory {
  public:
    void ParseFileBinary(std::shared_ptr<BinaryReader> reader, std::shared_ptr<Resource> resource) override;
};
}; // namespace Ship
