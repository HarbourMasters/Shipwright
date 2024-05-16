#include "soh/resource/importer/AudioSequenceFactory.h"
#include "soh/resource/type/AudioSequence.h"
#include "spdlog/spdlog.h"

namespace SOH {
std::shared_ptr<Ship::IResource> ResourceFactoryBinaryAudioSequenceV2::ReadResource(std::shared_ptr<Ship::File> file) {
    if (!FileHasValidFormatAndReader(file)) {
        return nullptr;
    }

    auto audioSequence = std::make_shared<AudioSequence>(file->InitData);
    auto reader = std::get<std::shared_ptr<Ship::BinaryReader>>(file->Reader);

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

    return audioSequence;
}
} // namespace SOH
