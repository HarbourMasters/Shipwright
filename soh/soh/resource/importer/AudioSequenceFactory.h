#pragma once

#include <ship/resource/Resource.h>
#include <ship/resource/ResourceFactoryBinary.h>
#include <ship/resource/ResourceFactoryXML.h>

namespace SOH {
class ResourceFactoryBinaryAudioSequenceV2 final : public Ship::ResourceFactoryBinary {
  public:
    std::shared_ptr<Ship::IResource> ReadResource(std::shared_ptr<Ship::File> file,
                                                  std::shared_ptr<Ship::ResourceInitData> initData) override;
};

class ResourceFactoryXMLAudioSequenceV0 final : public Ship::ResourceFactoryXML {
  public:
    std::shared_ptr<Ship::IResource> ReadResource(std::shared_ptr<Ship::File> file,
                                                  std::shared_ptr<Ship::ResourceInitData> initData) override;
};

} // namespace SOH
