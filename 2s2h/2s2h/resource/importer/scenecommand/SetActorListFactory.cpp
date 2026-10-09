#include "2s2h/resource/importer/scenecommand/SetActorListFactory.h"
#include "2s2h/resource/type/scenecommand/SetActorList.h"

namespace SOH {
std::shared_ptr<Ship::IResource> SetActorListFactory::ReadResource(std::shared_ptr<Ship::ResourceInitData> initData,
                                                                   std::shared_ptr<Ship::BinaryReader> reader) {
    auto setActorList = std::make_shared<SetActorList>(initData);

    ReadCommandId(setActorList, reader);

    setActorList->numActors = reader->ReadUInt32();
    setActorList->actorList.reserve(setActorList->numActors);
    for (uint32_t i = 0; i < setActorList->numActors; i++) {
        ActorEntry entry;

        entry.id = reader->ReadUInt16();
        entry.pos.x = reader->ReadInt16();
        entry.pos.y = reader->ReadInt16();
        entry.pos.z = reader->ReadInt16();
        entry.rot.x = reader->ReadInt16();
        entry.rot.y = reader->ReadInt16();
        entry.rot.z = reader->ReadInt16();
        entry.params = reader->ReadUInt16();

        setActorList->actorList.push_back(entry);
    }

    return setActorList;
}
} // namespace SOH
