#include "soh/resource/importer/scenecommand/SetRoomListFactory.h"
#include "soh/resource/type/scenecommand/SetRoomList.h"
#include "soh/resource/logging/SceneCommandLoggers.h"
#include "spdlog/spdlog.h"

namespace SOH {
std::shared_ptr<Ship::IResource>
SetRoomListFactory::ReadResource(std::shared_ptr<Ship::ResourceInitData> initData, std::shared_ptr<Ship::BinaryReader> reader) {
    auto setRoomList = std::make_shared<SetRoomList>(initData);

    ReadCommandId(setRoomList, reader);
	
    setRoomList->numRooms = reader->ReadInt32();
    setRoomList->rooms.reserve(setRoomList->numRooms);
    for (uint32_t i = 0; i < setRoomList->numRooms; i++) {
        RomFile room;

        setRoomList->fileNames.push_back(reader->ReadString());

        room.fileName = (char*)setRoomList->fileNames.back().c_str();
        room.vromStart = reader->ReadInt32();
        room.vromEnd = reader->ReadInt32();
		
        setRoomList->rooms.push_back(room);
    }

    if (CVarGetInteger(CVAR_DEVELOPER_TOOLS("ResourceLogging"), 0)) {
        LogRoomListAsXML(setRoomList);
    }

    return setRoomList;
}

std::shared_ptr<Ship::IResource> SetRoomListFactoryXML::ReadResource(std::shared_ptr<Ship::ResourceInitData> initData,
                                                                   tinyxml2::XMLElement* reader) {
    auto setRoomList = std::make_shared<SetRoomList>(initData);

    setRoomList->cmdId = SceneCommandID::SetRoomList;

    auto child = reader->FirstChildElement();

    while (child != nullptr) {
        std::string childName = child->Name();
        if (childName == "RoomEntry") {
            RomFile room;

	        setRoomList->fileNames.push_back(child->Attribute("Path"));

	        room.fileName = (char*)setRoomList->fileNames.back().c_str();
	        room.vromStart = child->IntAttribute("VromStart");
	        room.vromEnd = child->IntAttribute("VromEnd");

            setRoomList->rooms.push_back(room);
        }

        child = child->NextSiblingElement();
    }

    setRoomList->numRooms = setRoomList->rooms.size();

    return setRoomList;
}
} // namespace SOH
