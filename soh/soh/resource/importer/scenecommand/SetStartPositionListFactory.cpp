#include "soh/resource/importer/scenecommand/SetStartPositionListFactory.h"
#include "soh/resource/type/scenecommand/SetStartPositionList.h"
#include "spdlog/spdlog.h"

namespace SOH {
std::shared_ptr<LUS::IResource> SetStartPositionListFactory::ReadResource(std::shared_ptr<LUS::ResourceInitData> initData,
                                                                    std::shared_ptr<LUS::BinaryReader> reader) {
    auto setStartPositionList = std::make_shared<SetStartPositionList>(initData);

    ReadCommandId(setStartPositionList, reader);
	
    setStartPositionList->numStartPositions = reader->ReadUInt32();
    setStartPositionList->startPositions.reserve(setStartPositionList->numStartPositions);
    for (uint32_t i = 0; i < setStartPositionList->numStartPositions; i++) {
        ActorEntry entry;

        entry.id = reader->ReadUInt16();
        entry.pos.x = reader->ReadInt16();
        entry.pos.y = reader->ReadInt16();
        entry.pos.z = reader->ReadInt16();
        entry.rot.x = reader->ReadInt16();
        entry.rot.y = reader->ReadInt16();
        entry.rot.z = reader->ReadInt16();
        entry.params = reader->ReadUInt16();

        setStartPositionList->startPositions.push_back(entry);
    }

    return setStartPositionList;
}
} // namespace SOH
