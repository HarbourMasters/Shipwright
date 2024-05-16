#include "soh/resource/importer/scenecommand/SetLightListFactory.h"
#include "soh/resource/type/scenecommand/SetLightList.h"
#include "soh/resource/logging/SceneCommandLoggers.h"
#include "spdlog/spdlog.h"

namespace SOH {
std::shared_ptr<Ship::IResource>
SetLightListFactory::ReadResource(std::shared_ptr<Ship::ResourceInitData> initData, std::shared_ptr<Ship::BinaryReader> reader) {
    auto setLightList = std::make_shared<SetLightList>(initData);

    ReadCommandId(setLightList, reader);
    
    setLightList->numLights = reader->ReadUInt32();
    setLightList->lightList.reserve(setLightList->numLights);
    for (uint32_t i = 0; i < setLightList->numLights; i++) {
        LightInfo light;

        light.type = reader->ReadUByte();
		
	    light.params.point.x = reader->ReadInt16();
        light.params.point.y = reader->ReadInt16();
        light.params.point.z = reader->ReadInt16();

        light.params.point.color[0] = reader->ReadUByte(); // r
        light.params.point.color[1] = reader->ReadUByte(); // g
        light.params.point.color[2] = reader->ReadUByte(); // b

        light.params.point.drawGlow = reader->ReadUByte();
	    light.params.point.radius = reader->ReadInt16();

        setLightList->lightList.push_back(light);
    }

    if (CVarGetInteger(CVAR_DEVELOPER_TOOLS("ResourceLogging"), 0)) {
        LogLightListAsXML(setLightList);
    }

    return setLightList;
}

std::shared_ptr<Ship::IResource> SetLightListFactoryXML::ReadResource(std::shared_ptr<Ship::ResourceInitData> initData,
                                                                   tinyxml2::XMLElement* reader) {
    auto setLightList = std::make_shared<SetLightList>(initData);

    setLightList->cmdId = SceneCommandID::SetLightList;

    auto child = reader->FirstChildElement();

    while (child != nullptr) {
        std::string childName = child->Name();
        if (childName == "LightInfo") {
            LightInfo light;
            light.type = child->IntAttribute("Type");
            if (false/*light.type == LIGHT_DIRECTIONAL*/) {
                light.params.dir.x = child->IntAttribute("X");
                light.params.dir.y = child->IntAttribute("Y");
                light.params.dir.z = child->IntAttribute("Z");
                light.params.dir.color[0] = child->IntAttribute("ColorR");
                light.params.dir.color[1] = child->IntAttribute("ColorG");
                light.params.dir.color[2] = child->IntAttribute("ColorB");
            } else {
                light.params.point.x = child->IntAttribute("X");
                light.params.point.y = child->IntAttribute("Y");
                light.params.point.z = child->IntAttribute("Z");
                light.params.point.color[0] = child->IntAttribute("ColorR");
                light.params.point.color[1] = child->IntAttribute("ColorG");
                light.params.point.color[2] = child->IntAttribute("ColorB");
                light.params.point.drawGlow = child->IntAttribute("DrawGlow");
                light.params.point.radius = child->IntAttribute("Radius");
            }
            setLightList->lightList.push_back(light);
        }

        child = child->NextSiblingElement();
    }

    setLightList->numLights = setLightList->lightList.size();

    return setLightList;
}
} // namespace SOH
