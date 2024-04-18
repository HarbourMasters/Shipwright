#include "soh/resource/importer/scenecommand/SetRoomListFactory.h"
#include "soh/resource/type/scenecommand/SetRoomList.h"
#include "spdlog/spdlog.h"

namespace SOH {
std::shared_ptr<LUS::IResource>
SetRoomListFactory::ReadResource(std::shared_ptr<LUS::ResourceInitData> initData, std::shared_ptr<LUS::BinaryReader> reader) {
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

    //LogRoomListAsXML(setRoomList);

    return setRoomList;
}

std::shared_ptr<LUS::IResource> SetRoomListFactoryXML::ReadResource(std::shared_ptr<LUS::ResourceInitData> initData,
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

void LogRoomListAsXML(std::shared_ptr<LUS::IResource> resource) {
    std::shared_ptr<SetRoomList> setRoomList = std::static_pointer_cast<SetRoomList>(resource);

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* root = doc.NewElement("SetRoomList");
    doc.InsertFirstChild(root);

    for (size_t i = 0; i < setRoomList->numRooms; i++) {
        tinyxml2::XMLElement* entry = doc.NewElement("RoomEntry");
        entry->SetAttribute("Path", setRoomList->fileNames[i].c_str());
        root->InsertEndChild(entry);
    }

    tinyxml2::XMLPrinter printer;
    doc.Accept(&printer);

    SPDLOG_INFO("{}: {}", resource->GetInitData()->Path, printer.CStr());
}
} // namespace SOH
