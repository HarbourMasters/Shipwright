#include "soh/resource/importer/scenecommand/SetEntranceListFactory.h"
#include "soh/resource/type/scenecommand/SetEntranceList.h"
#include "spdlog/spdlog.h"

namespace SOH {
std::shared_ptr<LUS::IResource>
SetEntranceListFactory::ReadResource(std::shared_ptr<LUS::ResourceInitData> initData, std::shared_ptr<LUS::BinaryReader> reader) {
    auto setEntranceList = std::make_shared<SetEntranceList>(initData);

    ReadCommandId(setEntranceList, reader);
	
    setEntranceList->numEntrances = reader->ReadUInt32();
    setEntranceList->entrances.reserve(setEntranceList->numEntrances);
    for (uint32_t i = 0; i < setEntranceList->numEntrances; i++) {
	EntranceEntry entranceEntry;

	entranceEntry.spawn = reader->ReadInt8();
	entranceEntry.room = reader->ReadInt8();
		
        setEntranceList->entrances.push_back(entranceEntry);
    }

    return setEntranceList;
}
} // namespace SOH
