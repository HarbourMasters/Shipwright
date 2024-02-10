#include "soh/resource/importer/AudioSequenceFactory.h"
#include "soh/resource/type/AudioSequence.h"
#include "spdlog/spdlog.h"

namespace SOH {
std::shared_ptr<LUS::IResource> ResourceFactoryBinaryAudioSequenceV0::ReadResource(std::shared_ptr<LUS::File> file) {
    if (!FileHasValidFormatAndReader(file)) {
        return nullptr;
    }

    auto audioSequence = std::make_shared<AudioSequence>(file->InitData);

    audioSequence->sequence.seqDataSize = file->Reader->ReadInt32();
    audioSequence->sequenceData.reserve(audioSequence->sequence.seqDataSize);
    for (uint32_t i = 0; i < audioSequence->sequence.seqDataSize; i++) {
        audioSequence->sequenceData.push_back(file->Reader->ReadChar());
    }
    audioSequence->sequence.seqData = audioSequence->sequenceData.data();
    
    audioSequence->sequence.seqNumber = file->Reader->ReadUByte();
    audioSequence->sequence.medium = file->Reader->ReadUByte();
    audioSequence->sequence.cachePolicy = file->Reader->ReadUByte();

    audioSequence->sequence.numFonts = file->Reader->ReadUInt32();
    for (uint32_t i = 0; i < 16; i++) {
        audioSequence->sequence.fonts[i] = 0;
    }
    for (uint32_t i = 0; i < audioSequence->sequence.numFonts; i++) {
        audioSequence->sequence.fonts[i] = file->Reader->ReadUByte();
    }

    return audioSequence;
}
} // namespace SOH
