#include "2s2h/resource/importer/scenecommand/SetMinimapChestsFactory.h"
#include "2s2h/resource/type/scenecommand/SetMinimapChests.h"

namespace SOH {

std::shared_ptr<Ship::IResource> SetMinimapChestsFactory::ReadResource(std::shared_ptr<Ship::ResourceInitData> initData,
                                                                       std::shared_ptr<Ship::BinaryReader> reader) {
    auto chests = std::make_shared<SetMinimapChests>(initData);

    ReadCommandId(chests, reader);

    size_t size = reader->ReadUInt32();

    chests->chests.reserve(size);

    for (size_t i = 0; i < size; i++) {
        MinimapChestData d;
        d.unk_0 = reader->ReadUInt16();
        d.unk_2 = reader->ReadUInt16();
        d.unk_4 = reader->ReadUInt16();
        d.unk_6 = reader->ReadUInt16();
        d.unk_8 = reader->ReadUInt16();
        chests->chests.emplace_back(d);
    }

    return chests;
}
} // namespace SOH
