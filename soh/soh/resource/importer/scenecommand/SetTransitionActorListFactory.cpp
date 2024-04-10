#include "soh/resource/importer/scenecommand/SetTransitionActorListFactory.h"
#include "soh/resource/type/scenecommand/SetTransitionActorList.h"
#include "spdlog/spdlog.h"

namespace SOH {
std::shared_ptr<LUS::IResource> SetTransitionActorListFactory::ReadResource(std::shared_ptr<LUS::ResourceInitData> initData,
                                                                      std::shared_ptr<LUS::BinaryReader> reader) {
    auto setTransitionActorList = std::make_shared<SetTransitionActorList>(initData);
    
    ReadCommandId(setTransitionActorList, reader);
	
    setTransitionActorList->numTransitionActors = reader->ReadUInt32();
    setTransitionActorList->transitionActorList.reserve(setTransitionActorList->numTransitionActors);
    for (uint32_t i = 0; i < setTransitionActorList->numTransitionActors; i++) {
        TransitionActorEntry entry;

        entry.sides[0].room = reader->ReadUByte();
        entry.sides[0].effects = reader->ReadUByte();
        entry.sides[1].room = reader->ReadUByte();
        entry.sides[1].effects = reader->ReadUByte();
        entry.id = reader->ReadInt16();
        entry.pos.x = reader->ReadInt16();
        entry.pos.y = reader->ReadInt16();
        entry.pos.z = reader->ReadInt16();
        entry.rotY = reader->ReadInt16();
        entry.params = reader->ReadUInt16();

        setTransitionActorList->transitionActorList.push_back(entry);
    }

    return setTransitionActorList;
}
} // namespace SOH
