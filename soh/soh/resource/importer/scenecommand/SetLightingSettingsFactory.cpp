#include "soh/resource/importer/scenecommand/SetLightingSettingsFactory.h"
#include "soh/resource/type/scenecommand/SetLightingSettings.h"
#include "spdlog/spdlog.h"

namespace SOH {
std::shared_ptr<LUS::IResource> SetLightingSettingsFactory::ReadResource(std::shared_ptr<LUS::ResourceInitData> initData,
                                                                   std::shared_ptr<LUS::BinaryReader> reader) {
    auto setLightingSettings = std::make_shared<SetLightingSettings>(initData);

    ReadCommandId(setLightingSettings, reader);

    uint32_t count = reader->ReadInt32();
    setLightingSettings->settings.reserve(count);

    for (uint32_t i = 0; i < count; i++) {
        EnvLightSettings lightSettings;
        lightSettings.ambientColor[0] = reader->ReadInt8();
        lightSettings.ambientColor[1] = reader->ReadInt8();
        lightSettings.ambientColor[2] = reader->ReadInt8();

        lightSettings.light1Dir[0] = reader->ReadInt8();
        lightSettings.light1Dir[1] = reader->ReadInt8();
        lightSettings.light1Dir[2] = reader->ReadInt8();

        lightSettings.light1Color[0] = reader->ReadInt8();
        lightSettings.light1Color[1] = reader->ReadInt8();
        lightSettings.light1Color[2] = reader->ReadInt8();

        lightSettings.light2Dir[0] = reader->ReadInt8();
        lightSettings.light2Dir[1] = reader->ReadInt8();
        lightSettings.light2Dir[2] = reader->ReadInt8();

        lightSettings.light2Color[0] = reader->ReadInt8();
        lightSettings.light2Color[1] = reader->ReadInt8();
        lightSettings.light2Color[2] = reader->ReadInt8();

        lightSettings.fogColor[0] = reader->ReadInt8();
        lightSettings.fogColor[1] = reader->ReadInt8();
        lightSettings.fogColor[2] = reader->ReadInt8();

        lightSettings.fogNear = reader->ReadInt16();
        lightSettings.fogFar = reader->ReadUInt16();
        setLightingSettings->settings.push_back(lightSettings);
    }

    //LogLightingSettingsAsXML(setLightingSettings);

    return setLightingSettings;
}

std::shared_ptr<LUS::IResource> SetLightingSettingsFactoryXML::ReadResource(std::shared_ptr<LUS::ResourceInitData> initData,
                                                                   tinyxml2::XMLElement* reader) {
    auto setLightingSettings = std::make_shared<SetLightingSettings>(initData);

    setLightingSettings->cmdId = SceneCommandID::SetLightingSettings;

    auto child = reader->FirstChildElement();

    while (child != nullptr) {
        std::string childName = child->Name();
        if (childName == "LightingSetting") {
            EnvLightSettings lightSettings;
            lightSettings.ambientColor[0] = child->IntAttribute("AmbientColorR");
            lightSettings.ambientColor[1] = child->IntAttribute("AmbientColorG");
            lightSettings.ambientColor[2] = child->IntAttribute("AmbientColorB");

            lightSettings.light1Dir[0] = child->IntAttribute("Light1DirX");
            lightSettings.light1Dir[1] = child->IntAttribute("Light1DirY");
            lightSettings.light1Dir[2] = child->IntAttribute("Light1DirZ");
            lightSettings.light1Color[0] = child->IntAttribute("Light1ColorR");
            lightSettings.light1Color[1] = child->IntAttribute("Light1ColorG");
            lightSettings.light1Color[2] = child->IntAttribute("Light1ColorB");

            lightSettings.light2Dir[0] = child->IntAttribute("Light2DirX");
            lightSettings.light2Dir[1] = child->IntAttribute("Light2DirY");
            lightSettings.light2Dir[2] = child->IntAttribute("Light2DirZ");
            lightSettings.light2Color[0] = child->IntAttribute("Light2ColorR");
            lightSettings.light2Color[1] = child->IntAttribute("Light2ColorG");
            lightSettings.light2Color[2] = child->IntAttribute("Light2ColorB");

            lightSettings.fogColor[0] = child->IntAttribute("FogColorR");
            lightSettings.fogColor[1] = child->IntAttribute("FogColorG");
            lightSettings.fogColor[2] = child->IntAttribute("FogColorB");
            lightSettings.fogNear = child->IntAttribute("FogNear");
            lightSettings.fogFar = child->IntAttribute("FogFar");
            setLightingSettings->settings.push_back(lightSettings);
        }

        child = child->NextSiblingElement();
    }

    return setLightingSettings;
}

void LogLightingSettingsAsXML(std::shared_ptr<LUS::IResource> resource) {
    std::shared_ptr<SetLightingSettings> setLightingSettings = std::static_pointer_cast<SetLightingSettings>(resource);

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* root = doc.NewElement("SetLightingSettings");
    doc.InsertFirstChild(root);

    for (size_t i = 0; i < setLightingSettings->settings.size(); i++) {
        tinyxml2::XMLElement* entry = doc.NewElement("LightingSetting");
        entry->SetAttribute("AmbientColorR", setLightingSettings->settings[i].ambientColor[0]);
        entry->SetAttribute("AmbientColorG", setLightingSettings->settings[i].ambientColor[1]);
        entry->SetAttribute("AmbientColorB", setLightingSettings->settings[i].ambientColor[2]);

        entry->SetAttribute("Light1DirX", setLightingSettings->settings[i].light1Dir[0]);
        entry->SetAttribute("Light1DirY", setLightingSettings->settings[i].light1Dir[1]);
        entry->SetAttribute("Light1DirZ", setLightingSettings->settings[i].light1Dir[2]);
        entry->SetAttribute("Light1ColorR", setLightingSettings->settings[i].light1Color[0]);
        entry->SetAttribute("Light1ColorG", setLightingSettings->settings[i].light1Color[1]);
        entry->SetAttribute("Light1ColorB", setLightingSettings->settings[i].light1Color[2]);

        entry->SetAttribute("Light2DirX", setLightingSettings->settings[i].light2Dir[0]);
        entry->SetAttribute("Light2DirY", setLightingSettings->settings[i].light2Dir[1]);
        entry->SetAttribute("Light2DirZ", setLightingSettings->settings[i].light2Dir[2]);
        entry->SetAttribute("Light2ColorR", setLightingSettings->settings[i].light2Color[0]);
        entry->SetAttribute("Light2ColorG", setLightingSettings->settings[i].light2Color[1]);
        entry->SetAttribute("Light2ColorB", setLightingSettings->settings[i].light2Color[2]);

        entry->SetAttribute("FogColorR", setLightingSettings->settings[i].fogColor[0]);
        entry->SetAttribute("FogColorG", setLightingSettings->settings[i].fogColor[1]);
        entry->SetAttribute("FogColorB", setLightingSettings->settings[i].fogColor[2]);
        entry->SetAttribute("FogNear", setLightingSettings->settings[i].fogNear);
        entry->SetAttribute("FogFar", setLightingSettings->settings[i].fogFar);

        root->InsertEndChild(entry);
    }

    tinyxml2::XMLPrinter printer;
    doc.Accept(&printer);

    SPDLOG_INFO("{}: {}", resource->GetInitData()->Path, printer.CStr());
}
} // namespace SOH
