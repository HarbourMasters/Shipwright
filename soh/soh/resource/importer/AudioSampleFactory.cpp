#include "soh/resource/importer/AudioSampleFactory.h"
#include "soh/resource/type/AudioSample.h"
#include "spdlog/spdlog.h"

#define DRWAV_IMPLEMENTATION
#include <dr_libs/wav.h>


namespace LUS {
std::shared_ptr<IResource>
AudioSampleFactory::ReadResource(std::shared_ptr<ResourceInitData> initData, std::shared_ptr<BinaryReader> reader) {
    auto resource = std::make_shared<AudioSample>(initData);
    std::shared_ptr<ResourceVersionFactory> factory = nullptr;

    switch (resource->GetInitData()->ResourceVersion) {
    case 2:
        factory = std::make_shared<AudioSampleFactoryV0>();
        break;
    case 3:
        factory = std::make_shared<AudioSampleFactoryV1>(); //wav
        break;
    }

    if (factory == nullptr) {
        SPDLOG_ERROR("Failed to load AudioSample with version {}", resource->GetInitData()->ResourceVersion);
	return nullptr;
    }

    factory->ParseFileBinary(reader, resource);

    return resource;
}

void LUS::AudioSampleFactoryV0::ParseFileBinary(std::shared_ptr<BinaryReader> reader,
                                                 std::shared_ptr<IResource> resource)
{
    std::shared_ptr<AudioSample> audioSample = std::static_pointer_cast<AudioSample>(resource);
    ResourceVersionFactory::ParseFileBinary(reader, audioSample);

    audioSample->sample.codec = reader->ReadUByte();
    audioSample->sample.medium = reader->ReadUByte();
    audioSample->sample.unk_bit26 = reader->ReadUByte();
    audioSample->sample.unk_bit25 = reader->ReadUByte();
    audioSample->sample.size = reader->ReadUInt32();

    audioSample->audioSampleData.reserve(audioSample->sample.size);
    for (uint32_t i = 0; i < audioSample->sample.size; i++) {
        audioSample->audioSampleData.push_back(reader->ReadUByte());
    }
    audioSample->sample.sampleAddr = audioSample->audioSampleData.data();

    audioSample->loop.start = reader->ReadUInt32();
    audioSample->loop.end = reader->ReadUInt32();
    audioSample->loop.count = reader->ReadUInt32();

    audioSample->loopStateCount = reader->ReadUInt32();
    for (int i = 0; i < 16; i++) {
        audioSample->loop.state[i] = 0;
    }
    for (uint32_t i = 0; i < audioSample->loopStateCount; i++) {
        audioSample->loop.state[i] = reader->ReadInt16();
    }
    audioSample->sample.loop = &audioSample->loop;

    audioSample->book.order = reader->ReadInt32();
    audioSample->book.npredictors = reader->ReadInt32();
    audioSample->bookDataCount = reader->ReadUInt32();

    audioSample->bookData.reserve(audioSample->bookDataCount);
    for (uint32_t i = 0; i < audioSample->bookDataCount; i++) {
        audioSample->bookData.push_back(reader->ReadInt16());
    }
    audioSample->book.book = audioSample->bookData.data();
    audioSample->sample.book = &audioSample->book;
}

void LUS::AudioSampleFactoryV1::ParseFileBinary(std::shared_ptr<BinaryReader> reader,
                                                 std::shared_ptr<IResource> resource)
{
    std::shared_ptr<AudioSample> audioSample = std::static_pointer_cast<AudioSample>(resource);
    ResourceVersionFactory::ParseFileBinary(reader, audioSample);

    reader->Seek(52, SeekOffsetType::Start);
    if (reader->ReadChar() == 'W' &&
        reader->ReadChar() == 'A' &&
        reader->ReadChar() == 'V' &&
        reader->ReadChar() == 'E') {
        reader->Seek(-12, SeekOffsetType::Current);
        u32 curr = reader->GetBaseAddress();
        reader->Seek(0, SeekOffsetType::End);
        u32 end = reader->GetBaseAddress();
        reader->Seek(curr, SeekOffsetType::Start);
        u32 len = end - curr + 1;
        u8* strem2 = (u8 *) malloc(sizeof(u8) * len);
        reader->Read((char *) strem2, len);
        drwav_uint32 channels;
        drwav_uint32 sampleRate;
        drwav_uint64 totalPcm;
        drwav_int16* pcmData =
            drwav_open_memory_and_read_pcm_frames_s16(strem2, len, &channels, &sampleRate, &totalPcm, NULL);
        audioSample->sample.size = totalPcm;
        audioSample->sample.sampleAddr = (uint8_t*)pcmData;
        audioSample->sample.codec = 5; //CODEC_S16
        audioSample->loop.start = 0;
        audioSample->loop.end = audioSample->sample.size;
        audioSample->loop.count = 0;
        
        drwav pWav;
        pWav.pMetadata = NULL;
        drwav_smpl drwavSmpl;
        if (drwav_init_memory_with_metadata(&pWav, (const void*) strem2, len, DRWAV_SEQUENTIAL, NULL) &&
            pWav.pMetadata != NULL &&
            pWav.pMetadata->data.smpl.pLoops != NULL) {
                audioSample->loop.start = pWav.pMetadata->data.smpl.pLoops->firstSampleByteOffset;
                audioSample->loop.end = pWav.pMetadata->data.smpl.pLoops->lastSampleByteOffset;
                audioSample->loop.count = pWav.pMetadata->data.smpl.pLoops->playCount;
        }
        audioSample->sample.loop = &audioSample->loop;
        audioSample->sample.sampleRateMagicValue = 'RIFF';
        audioSample->sample.book = &audioSample->book;
        audioSample->sample.sampleRate = sampleRate;

        drwav_uninit(&pWav);
        free(strem2);

    }
    else {
        SPDLOG_ERROR("Corrupted wav");
    }

}
} // namespace LUS


/*
in ResourceMgr_LoadAudioSample we used to have
--------------
    if (cachedCustomSFs.find(path) != cachedCustomSFs.end())
        return cachedCustomSFs[path];

    SoundFontSample* cSample = ReadCustomSample(path);

    if (cSample != nullptr)
        return cSample;
--------------
before the rest of the standard sample reading, this is the ReadCustomSample code we used to have

extern "C" SoundFontSample* ReadCustomSample(const char* path) {

    if (!ExtensionCache.contains(path))
        return nullptr;

    ExtensionEntry entry = ExtensionCache[path];

    auto sampleRaw = LUS::Context::GetInstance()->GetResourceManager()->LoadFile(entry.path);
    uint32_t* strem = (uint32_t*)sampleRaw->Buffer.get();
    uint8_t* strem2 = (uint8_t*)strem;

    SoundFontSample* sampleC = new SoundFontSample;

    if (entry.ext == "wav") {
        drwav_uint32 channels;
        drwav_uint32 sampleRate;
        drwav_uint64 totalPcm;
        drmp3_int16* pcmData =
            drwav_open_memory_and_read_pcm_frames_s16(strem2, sampleRaw->BufferSize, &channels, &sampleRate, &totalPcm, NULL);
        sampleC->size = totalPcm;
        sampleC->sampleAddr = (uint8_t*)pcmData;
        sampleC->codec = CODEC_S16;

        sampleC->loop = new AdpcmLoop;
        sampleC->loop->start = 0;
        sampleC->loop->end = sampleC->size - 1;
        sampleC->loop->count = 0;
        sampleC->sampleRateMagicValue = 'RIFF';
        sampleC->sampleRate = sampleRate;

        cachedCustomSFs[path] = sampleC;
        return sampleC;
    } else if (entry.ext == "mp3") {
        drmp3_config mp3Info;
        drmp3_uint64 totalPcm;
        drmp3_int16* pcmData =
            drmp3_open_memory_and_read_pcm_frames_s16(strem2, sampleRaw->BufferSize, &mp3Info, &totalPcm, NULL);

        sampleC->size = totalPcm * mp3Info.channels * sizeof(short);
        sampleC->sampleAddr = (uint8_t*)pcmData;
        sampleC->codec = CODEC_S16;

        sampleC->loop = new AdpcmLoop;
        sampleC->loop->start = 0;
        sampleC->loop->end = sampleC->size;
        sampleC->loop->count = 0;
        sampleC->sampleRateMagicValue = 'RIFF';
        sampleC->sampleRate = mp3Info.sampleRate;

        cachedCustomSFs[path] = sampleC;
        return sampleC;
    }

    return nullptr;
}

*/
