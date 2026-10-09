#include "2s2h/resource/importer/scenecommand/SetActorCutsceneListFactory.h"
#include "2s2h/resource/type/scenecommand/SetActorCutsceneList.h"

namespace SOH {
std::shared_ptr<Ship::IResource>
SetActorCutsceneListFactory::ReadResource(std::shared_ptr<Ship::ResourceInitData> initData,
                                          std::shared_ptr<Ship::BinaryReader> reader) {
    auto setActorCsList = std::make_shared<SetActorCutsceneList>(initData);
    ReadCommandId(setActorCsList, reader);

    uint32_t numEntries = reader->ReadUInt32();
    setActorCsList->entries.reserve(numEntries);

    for (uint32_t i = 0; i < numEntries; i++) {
        CutsceneEntry e;
        e.priority = reader->ReadInt16();
        e.length = reader->ReadInt16();
        e.csCamId = reader->ReadInt16();
        e.scriptIndex = reader->ReadInt16();
        e.additionalCsId = reader->ReadInt16();
        e.endSfx = reader->ReadUByte();
        e.customValue = reader->ReadUByte();
        e.hudVisibility = reader->ReadInt16();
        e.endCam = reader->ReadUByte();
        e.letterboxSize = reader->ReadUByte();
        setActorCsList->entries.emplace_back(e);
    }
    return setActorCsList;
}
} // namespace SOH
