#include "soh/resource/importer/scenecommand/SetPathwaysFactory.h"
#include "soh/resource/type/scenecommand/SetPathways.h"
#include "spdlog/spdlog.h"
#include <libultraship/libultraship.h>

namespace SOH {
std::shared_ptr<LUS::IResource>
SetPathwaysFactory::ReadResource(std::shared_ptr<LUS::ResourceInitData> initData, std::shared_ptr<LUS::BinaryReader> reader) {
    auto setPathways = std::make_shared<SetPathways>(initData);

    ReadCommandId(setPathways, reader);
	
    setPathways->numPaths = reader->ReadUInt32();
    setPathways->paths.reserve(setPathways->numPaths);
    for (uint32_t i = 0; i < setPathways->numPaths; i++) {
        std::string pathFileName = reader->ReadString();
        auto path = std::static_pointer_cast<Path>(LUS::Context::GetInstance()->GetResourceManager()->LoadResourceProcess(pathFileName.c_str()));
        setPathways->paths.push_back(path->GetPointer());
    }

    //LogPathwaysAsXML(setPathways);

    return setPathways;
}

std::shared_ptr<LUS::IResource> SetPathwaysFactoryXML::ReadResource(std::shared_ptr<LUS::ResourceInitData> initData,
                                                                   tinyxml2::XMLElement* reader) {
    auto setPathways = std::make_shared<SetPathways>(initData);

    setPathways->cmdId = SceneCommandID::SetPathways;

    auto child = reader->FirstChildElement();

    while (child != nullptr) {
        std::string childName = child->Name();
        if (childName == "Pathway") {
            std::string pathFileName = child->Attribute("FilePath");
            auto path = std::static_pointer_cast<Path>(LUS::Context::GetInstance()->GetResourceManager()->LoadResourceProcess(pathFileName.c_str()));
            setPathways->paths.push_back(path->GetPointer());
            setPathways->pathFileNames.push_back(pathFileName);
        }

        child = child->NextSiblingElement();
    }

    setPathways->numPaths = setPathways->paths.size();

    return setPathways;
}

void LogPathwaysAsXML(std::shared_ptr<LUS::IResource> resource) {
    std::shared_ptr<SetPathways> setPathways = std::static_pointer_cast<SetPathways>(resource);

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* root = doc.NewElement("SetPathways");
    doc.InsertFirstChild(root);

    for (size_t i = 0; i < setPathways->pathFileNames.size(); i++) {
        tinyxml2::XMLElement* pathway = doc.NewElement("Pathway");
        pathway->SetAttribute("FilePath", setPathways->pathFileNames[i].c_str());
        root->InsertEndChild(pathway);
    }

    tinyxml2::XMLPrinter printer;
    doc.Accept(&printer);

    SPDLOG_INFO("{}: {}", resource->GetInitData()->Path, printer.CStr());
}
} // namespace SOH
