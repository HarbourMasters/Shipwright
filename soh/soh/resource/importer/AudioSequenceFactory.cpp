#include "soh/resource/importer/AudioSequenceFactory.h"
#include "soh/resource/type/AudioSequence.h"
#include "spdlog/spdlog.h"

namespace Ship {
std::shared_ptr<Resource> AudioSequenceFactory::ReadResource(std::shared_ptr<ResourceManager> resourceMgr,
                                                             std::shared_ptr<ResourceInitData> initData,
                                                             std::shared_ptr<BinaryReader> reader) {
    auto resource = std::make_shared<AudioSequence>(resourceMgr, initData);
    std::shared_ptr<ResourceVersionFactory> factory = nullptr;

    switch (resource->InitData->ResourceVersion) {
    case 2:
	    factory = std::make_shared<AudioSequenceFactoryV0>();
	    break;
    }

    if (factory == nullptr)
    {
        SPDLOG_ERROR("Failed to load AudioSequence with version {}", resource->InitData->ResourceVersion);
	return nullptr;
    }

    factory->ParseFileBinary(reader, resource);

    return resource;
}

void Ship::AudioSequenceFactoryV0::ParseFileBinary(std::shared_ptr<BinaryReader> reader,
                                                   std::shared_ptr<Resource> resource) {
    std::shared_ptr<AudioSequence> audioSequence = std::static_pointer_cast<AudioSequence>(resource);
    ResourceVersionFactory::ParseFileBinary(reader, audioSequence);

    audioSequence->sequence.seqDataSize = reader->ReadInt32();
    audioSequence->sequenceData.reserve(audioSequence->sequence.seqDataSize);
    for (uint32_t i = 0; i < audioSequence->sequence.seqDataSize; i++) {
        audioSequence->sequenceData.push_back(reader->ReadChar());
    }
    audioSequence->sequence.seqData = audioSequence->sequenceData.data();
    
    audioSequence->sequence.seqNumber = reader->ReadUByte();
    audioSequence->sequence.medium = reader->ReadUByte();
    audioSequence->sequence.cachePolicy = reader->ReadUByte();

    audioSequence->sequence.numFonts = reader->ReadUInt32();
    for (uint32_t i = 0; i < 16; i++) {
        audioSequence->sequence.fonts[i] = 0;
    }
    for (uint32_t i = 0; i < audioSequence->sequence.numFonts; i++) {
        audioSequence->sequence.fonts[i] = reader->ReadUByte();
    }
}
} // namespace Ship
