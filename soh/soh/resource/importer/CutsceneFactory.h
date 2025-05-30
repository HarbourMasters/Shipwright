#pragma once

#include "Resource.h"
#include "ResourceFactoryBinary.h"

namespace SOH {
class ResourceFactoryBinaryCutsceneV0 final : public Ship::ResourceFactoryBinary {
  public:
    std::shared_ptr<Ship::IResource> ReadResource(std::shared_ptr<Ship::File> file,
                                                  std::shared_ptr<Ship::ResourceInitData> initData) override;
};
} // namespace SOH
