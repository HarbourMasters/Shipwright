#include "soh/resource/importer/scenecommand/SetPathwaysFactory.h"
#include "soh/resource/type/scenecommand/SetPathways.h"
#include "soh/resource/logging/SceneCommandLoggers.h"
#include "spdlog/spdlog.h"
#include <libultraship/libultraship.h>

namespace SOH {
std::shared_ptr<Ship::IResource>
SetPathwaysFactory::ReadResource(std::shared_ptr<Ship::ResourceInitData> initData, std::shared_ptr<Ship::BinaryReader> reader) {
    auto setPathways = std::make_shared<SetPathways>(initData);

    ReadCommandId(setPathways, reader);
	
    setPathways->numPaths = reader->ReadUInt32();
    setPathways->paths.reserve(setPathways->numPaths);
    for (uint32_t i = 0; i < setPathways->numPaths; i++) {
        std::string pathFileName = reader->ReadString();
        auto path = std::static_pointer_cast<Path>(Ship::Context::GetInstance()->GetResourceManager()->LoadResourceProcess(pathFileName.c_str()));
        setPathways->paths.push_back(path->GetPointer());
        setPathways->pathFileNames.push_back(pathFileName);
    }

    if (CVarGetInteger(CVAR_DEVELOPER_TOOLS("ResourceLogging"), 0)) {
        LogPathwaysAsXML(setPathways);
    }

    return setPathways;
}

std::shared_ptr<Ship::IResource> SetPathwaysFactoryXML::ReadResource(std::shared_ptr<Ship::ResourceInitData> initData,
                                                                   tinyxml2::XMLElement* reader) {
    auto setPathways = std::make_shared<SetPathways>(initData);

    setPathways->cmdId = SceneCommandID::SetPathways;

    auto child = reader->FirstChildElement();

    while (child != nullptr) {
        std::string childName = child->Name();
        if (childName == "Pathway") {
            std::string pathFileName = child->Attribute("FilePath");
            auto path = std::static_pointer_cast<Path>(Ship::Context::GetInstance()->GetResourceManager()->LoadResourceProcess(pathFileName.c_str()));
            setPathways->paths.push_back(path->GetPointer());
            setPathways->pathFileNames.push_back(pathFileName);
        }

        child = child->NextSiblingElement();
    }

    setPathways->numPaths = setPathways->paths.size();

    return setPathways;
}
} // namespace SOH
