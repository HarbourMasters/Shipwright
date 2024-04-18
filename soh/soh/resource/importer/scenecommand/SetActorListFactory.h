#pragma once

#include "soh/resource/importer/scenecommand/SceneCommandFactory.h"

namespace SOH {
class SetActorListFactory : public SceneCommandFactoryBinaryV0 {
  public:
    std::shared_ptr<LUS::IResource>
    ReadResource(std::shared_ptr<LUS::ResourceInitData> initData, std::shared_ptr<LUS::BinaryReader> reader) override;
};

class SetActorListFactoryXML : public SceneCommandFactoryXMLV0 {
  public:
    std::shared_ptr<LUS::IResource>
    ReadResource(std::shared_ptr<LUS::ResourceInitData> initData, tinyxml2::XMLElement* reader) override;
};

void LogActorListAsXML(std::shared_ptr<LUS::IResource> resource);
} // namespace SOH
