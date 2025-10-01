#pragma once

#include "soh/resource/importer/scenecommand/SceneCommandFactory.h"

namespace SOH {
class SetEchoSettingsFactory final : public SceneCommandFactoryBinaryV0 {
  public:
    std::shared_ptr<Ship::IResource> ReadResource(std::shared_ptr<Ship::ResourceInitData> initData,
                                                  std::shared_ptr<Ship::BinaryReader> reader) override;
};

class SetEchoSettingsFactoryXML final : public SceneCommandFactoryXMLV0 {
  public:
    std::shared_ptr<Ship::IResource> ReadResource(std::shared_ptr<Ship::ResourceInitData> initData,
                                                  tinyxml2::XMLElement* reader) override;
};
} // namespace SOH
