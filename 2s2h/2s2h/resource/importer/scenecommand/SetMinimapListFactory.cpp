#include "2s2h/resource/importer/scenecommand/SetMinimapListFactory.h"
#include "2s2h/resource/type/scenecommand/SetMinimapList.h"

namespace SOH {
std::shared_ptr<Ship::IResource> SetMinimapListFactory::ReadResource(std::shared_ptr<Ship::ResourceInitData> initData,
                                                                     std::shared_ptr<Ship::BinaryReader> reader) {
    auto mapList = std::make_shared<SetMinimapList>(initData);

    ReadCommandId(mapList, reader);

    size_t size = reader->ReadUInt32();

    mapList->list.scale = reader->ReadUInt16();
    mapList->entries.reserve(size);

    for (size_t i = 0; i < size; i++) {
        MinimapEntryData data;
        data.mapId = reader->ReadUInt16();
        data.unk2 = reader->ReadUInt16();
        data.unk4 = reader->ReadUInt16();
        data.unk6 = reader->ReadUInt16();
        data.unk8 = reader->ReadUInt16();
        mapList->entries.emplace_back(data);
    }
    mapList->list.entry = mapList->entries.data();

    return mapList;
}
} // namespace SOH
