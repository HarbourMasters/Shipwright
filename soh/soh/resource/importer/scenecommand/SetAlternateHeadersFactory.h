#pragma once

#include "soh/resource/importer/scenecommand/SceneCommandFactory.h"

namespace SOH {
class SetAlternateHeadersFactory : public SceneCommandFactoryBinaryV0 {
  public:
    std::shared_ptr<Ship::IResource>
    ReadResource(std::shared_ptr<Ship::ResourceInitData> initData, std::shared_ptr<Ship::BinaryReader> reader) override;
};

class SetAlternateHeadersFactoryXML : public SceneCommandFactoryXMLV0 {
  public:
    std::shared_ptr<Ship::IResource>
    ReadResource(std::shared_ptr<Ship::ResourceInitData> initData, tinyxml2::XMLElement* reader) override;
};
} // namespace SOH
