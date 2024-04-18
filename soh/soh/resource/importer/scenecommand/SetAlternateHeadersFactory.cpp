#include "soh/resource/importer/scenecommand/SetAlternateHeadersFactory.h"
#include "soh/resource/type/scenecommand/SetAlternateHeaders.h"
#include "spdlog/spdlog.h"
#include "libultraship/libultraship.h"

namespace SOH {
std::shared_ptr<LUS::IResource> SetAlternateHeadersFactory::ReadResource(std::shared_ptr<LUS::ResourceInitData> initData,
                                                                   std::shared_ptr<LUS::BinaryReader> reader) {
    auto setAlternateHeaders = std::make_shared<SetAlternateHeaders>(initData);

    ReadCommandId(setAlternateHeaders, reader);
	
    setAlternateHeaders->numHeaders = reader->ReadUInt32();
    setAlternateHeaders->headers.reserve(setAlternateHeaders->numHeaders);
    for (uint32_t i = 0; i < setAlternateHeaders->numHeaders; i++) {
        auto headerName = reader->ReadString();
        if (!headerName.empty()) {
            setAlternateHeaders->headers.push_back(std::static_pointer_cast<Scene>(LUS::Context::GetInstance()->GetResourceManager()->LoadResourceProcess(headerName.c_str())));
        } else {
            setAlternateHeaders->headers.push_back(nullptr);
        }
    }

    //LogAlternateHeadersAsXML(setAlternateHeaders);

    return setAlternateHeaders;
}

std::shared_ptr<LUS::IResource> SetAlternateHeadersFactoryXML::ReadResource(std::shared_ptr<LUS::ResourceInitData> initData,
                                                                   tinyxml2::XMLElement* reader) {
    auto setAlternateHeaders = std::make_shared<SetAlternateHeaders>(initData);

    setAlternateHeaders->cmdId = SceneCommandID::SetAlternateHeaders;

    auto child = reader->FirstChildElement();

    while (child != nullptr) {
        std::string childName = child->Name();
        if (childName == "AlternateHeader") {
            for (uint32_t i = 0; i < setAlternateHeaders->numHeaders; i++) {
	            auto headerName = std::string(child->Attribute("HeaderName"));
	            if (!headerName.empty()) {
	            	setAlternateHeaders->headers.push_back(std::static_pointer_cast<Scene>(LUS::Context::GetInstance()->GetResourceManager()->LoadResourceProcess(headerName.c_str())));
	            } else {
	            	setAlternateHeaders->headers.push_back(nullptr);
	            }
            }
        }

        child = child->NextSiblingElement();
    }

    setAlternateHeaders->numHeaders = setAlternateHeaders->headers.size();

    return setAlternateHeaders;
}

void LogAlternateHeadersAsXML(std::shared_ptr<LUS::IResource> resource) {
    std::shared_ptr<SetAlternateHeaders> setAlternateHeaders = std::static_pointer_cast<SetAlternateHeaders>(resource);

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* root = doc.NewElement("SetAlternateHeaders");
    doc.InsertFirstChild(root);

    tinyxml2::XMLPrinter printer;
    doc.Accept(&printer);

    SPDLOG_INFO("{}: {}", resource->GetInitData()->Path, printer.CStr());
}
} // namespace SOH
