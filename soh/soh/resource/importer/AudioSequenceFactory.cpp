#include "soh/resource/importer/AudioSequenceFactory.h"
#include "soh/resource/importer/AudioSoundFontFactory.h"
#include "soh/resource/type/AudioSequence.h"
#include "spdlog/spdlog.h"
#include <ship/resource/ResourceManager.h>
#include <tinyxml2.h>

#include <ship/Context.h>
#include <ship/resource/archive/Archive.h>
#include <ship/utils/binarytools/BinaryWriter.h>
#include <type_traits>

namespace SOH {
std::shared_ptr<Ship::IResource>
ResourceFactoryBinaryAudioSequenceV2::ReadResource(std::shared_ptr<Ship::File> file,
                                                   std::shared_ptr<Ship::ResourceInitData> initData) {
    if (!FileHasValidFormatAndReader(file, initData)) {
        return nullptr;
    }

    auto audioSequence = std::make_shared<AudioSequence>(initData);
    auto reader = std::get<std::shared_ptr<Ship::BinaryReader>>(file->Reader);

    audioSequence->sequence.seqDataSize = reader->ReadUInt32();
    audioSequence->sequence.seqData = new char[audioSequence->sequence.seqDataSize];
    for (int32_t i = 0; i < audioSequence->sequence.seqDataSize; i++) {
        audioSequence->sequence.seqData[i] = reader->ReadChar();
    }

    audioSequence->sequence.seqNumber = reader->ReadUByte();
    audioSequence->sequence.medium = reader->ReadUByte();
    audioSequence->sequence.cachePolicy = reader->ReadUByte();

    audioSequence->sequence.numFonts = reader->ReadUInt32();
    for (int32_t i = 0; i < 16; i++) {
        audioSequence->sequence.fonts[i] = 0;
    }
    for (int32_t i = 0; i < audioSequence->sequence.numFonts; i++) {
        audioSequence->sequence.fonts[i] = reader->ReadUByte();
    }

    return audioSequence;
}

template <typename T> static void WriteInsnOneArg(Ship::BinaryWriter* writer, uint8_t opcode, T arg) {
    static_assert(std::is_fundamental<T>::value);
    writer->Write(opcode);
    writer->Write(arg);
}

template <typename T1, typename T2>
static void WriteInsnTwoArg(Ship::BinaryWriter* writer, uint8_t opcode, T1 arg1, T2 arg2) {
    static_assert(std::is_fundamental<T1>::value && std::is_fundamental<T2>::value);
    writer->Write(opcode);
    writer->Write(arg1);
    writer->Write(arg2);
}

template <typename T1, typename T2, typename T3>
static void WriteInsnThreeArg(Ship::BinaryWriter* writer, uint8_t opcode, T1 arg1, T2 arg2, T3 arg3) {
    static_assert(std::is_fundamental<T1>::value && std::is_fundamental<T2>::value);
    writer->Write(opcode);
    writer->Write(arg1);
    writer->Write(arg2);
    writer->Write(arg3);
}

static void WriteInsnNoArg(Ship::BinaryWriter* writer, uint8_t opcode) {
    writer->Write(opcode);
}

static void WriteLegato(Ship::BinaryWriter* writer) {
    WriteInsnNoArg(writer, 0xC4);
}

static void WriteNoLegato(Ship::BinaryWriter* writer) {
    WriteInsnNoArg(writer, 0xC5);
}

static void WriteMuteBhv(Ship::BinaryWriter* writer, uint8_t arg) {
    WriteInsnOneArg(writer, 0xD3, arg);
}

static void WriteMuteScale(Ship::BinaryWriter* writer, uint8_t arg) {
    WriteInsnOneArg(writer, 0xD5, arg);
}

static void WriteInitchan(Ship::BinaryWriter* writer, uint16_t channels) {
    WriteInsnOneArg(writer, 0xD7, channels);
}

static void WriteLdchan(Ship::BinaryWriter* writer, uint8_t channel, uint16_t offset) {
    WriteInsnOneArg(writer, 0x90 | channel, offset);
}

static void WriteVolSHeader(Ship::BinaryWriter* writer, uint8_t vol) {
    WriteInsnOneArg(writer, 0xDB, vol);
}

static void WriteVolCHeader(Ship::BinaryWriter* writer, uint8_t vol) {
    WriteInsnOneArg(writer, 0xDF, vol);
}

static void WriteTempo(Ship::BinaryWriter* writer, uint8_t tempo) {
    WriteInsnOneArg(writer, 0xDD, tempo);
}

static void WriteJump(Ship::BinaryWriter* writer, uint16_t offset) {
    WriteInsnOneArg(writer, 0xFB, offset);
}

static void WriteDisablecan(Ship::BinaryWriter* writer, uint16_t channels) {
    WriteInsnOneArg(writer, 0xD6, channels);
}

static void WriteNoshort(Ship::BinaryWriter* writer) {
    WriteInsnNoArg(writer, 0xC4);
}

static void WriteLdlayer(Ship::BinaryWriter* writer, uint8_t layer, uint16_t offset) {
    WriteInsnOneArg(writer, 0x88 | layer, offset);
}

static void WritePan(Ship::BinaryWriter* writer, uint8_t pan) {
    WriteInsnOneArg(writer, 0xDD, pan);
}

static void WriteBend(Ship::BinaryWriter* writer, uint8_t bend) {
    WriteInsnOneArg(writer, 0xD3, bend);
}

static void WriteInstrument(Ship::BinaryWriter* writer, uint8_t instrument) {
    WriteInsnOneArg(writer, 0xC1, instrument);
}

static void WriteTranspose(Ship::BinaryWriter* writer, int8_t transpose) {
    WriteInsnOneArg(writer, 0xC2, transpose);
}

static void WriteDelay(Ship::BinaryWriter* writer, uint16_t delay) {
    if (delay > 0x7F) {
        WriteInsnOneArg(writer, 0xFD, static_cast<uint16_t>(delay | 0x8000));
    } else {
        WriteInsnOneArg(writer, 0xFD, static_cast<uint8_t>(delay));
    }
}

template <typename T> static void WriteLDelay(Ship::BinaryWriter* writer, T delay) {
    WriteInsnOneArg(writer, 0xC0, delay);
}

template <typename T> static void WriteNotedv(Ship::BinaryWriter* writer, uint8_t note, T delay, uint8_t velocity) {
    WriteInsnTwoArg(writer, note, delay, velocity);
}

static void WriteNotedvg(Ship::BinaryWriter* writer, uint8_t note, uint16_t delay, uint8_t velocity, uint8_t gateTime) {
    if (delay > 0x7F) {
        WriteInsnThreeArg(writer, note, static_cast<uint16_t>(delay | 0x8000), velocity, gateTime);
    } else {
        WriteInsnThreeArg(writer, note, static_cast<uint8_t>(delay), velocity, gateTime);
    }
}

static void WriteMonoSingleSeq(Ship::BinaryWriter* writer, uint16_t delay, uint8_t tempo, bool looped) {
    uint16_t channelStart;
    uint16_t channelPlaceholderOff;
    uint16_t loopPoint;
    uint16_t layerPlaceholderOff;
    uint16_t layerStart;
    if (looped) {
        delay = 0x7FFF;
    }
    // Write seq header

    // These two values are always the same in OOT and MM
    WriteMuteBhv(writer, 0x20);
    WriteMuteScale(writer, 0x32);

    // We only have one channel
    WriteInitchan(writer, 0b11);
    // Store the current position so we can write the address of the channel when we are ready.
    channelPlaceholderOff = writer->GetBaseAddress();
    // Store the current position so we can loop here after the song ends.
    loopPoint = writer->GetBaseAddress();
    WriteLdchan(writer, 0, 0); // Fill in the actual address later

    WriteVolSHeader(writer, 127); // Max volume
    WriteTempo(writer, tempo);

    WriteDelay(writer, delay);
    if (looped) {
        WriteJump(writer, loopPoint);
    }
    WriteDisablecan(writer, 0b11);
    writer->Write(static_cast<uint8_t>(0xFF));

    // Fill in the ldchan from before
    channelStart = writer->GetBaseAddress();
    writer->Seek(channelPlaceholderOff, Ship::SeekOffsetType::Start);
    WriteLdchan(writer, 0, channelStart);
    writer->Seek(channelStart, Ship::SeekOffsetType::Start);

    // Channel header
    layerPlaceholderOff = writer->GetBaseAddress();
    WriteNoshort(writer);
    WriteLdlayer(writer, 0, 0);
    WritePan(writer, 64);
    WriteVolCHeader(writer, 127); // Max volume
    WriteBend(writer, 0);
    WriteInstrument(writer, 0);
    WriteDelay(writer, delay);
    writer->Write(static_cast<uint8_t>(0xFF));

    layerStart = writer->GetBaseAddress();
    writer->Seek(layerPlaceholderOff, Ship::SeekOffsetType::Start);
    WriteLdlayer(writer, 0, layerStart);
    writer->Seek(layerStart, Ship::SeekOffsetType::Start);

    // Note layer
    WriteLegato(writer);
    WriteNotedvg(writer, 39, 0x7FFF - 1, static_cast<uint8_t>(0x7F), static_cast<uint8_t>(1));
    writer->Write(static_cast<uint8_t>(0xFF));
}

static void WriteStereoSingleSeq(Ship::BinaryWriter* writer, uint16_t delay, uint8_t tempo, bool looped) {
    uint16_t lChannelStart;
    uint16_t rChannelStart;
    uint16_t channelPlaceholderOff;
    uint16_t loopPoint;
    uint16_t lLayerPlaceholderOff;
    uint16_t rLayerPlaceholderOff;
    uint16_t lLayerOffset;
    uint16_t rLayerOffset;

    uint16_t layerStart;
    // Write seq header
    if (looped) {
        delay = 0x7FFF;
    }
    // These two values are always the same in OOT and MM
    WriteMuteBhv(writer, 0x20);
    WriteMuteScale(writer, 0x32);

    // We only have one channel
    WriteInitchan(writer, 0b11);
    // Store the current position so we can write the address of the channel when we are ready.
    channelPlaceholderOff = writer->GetBaseAddress();
    // Store the current position so we can loop here after the song ends.
    loopPoint = writer->GetBaseAddress();
    // Left note channel
    WriteLdchan(writer, 0, 0); // Fill in the actual address later
    // Right note channel
    WriteLdchan(writer, 1, 0); // Fill in the actual address later

    WriteVolSHeader(writer, 127); // Max volume
    WriteTempo(writer, tempo);

    WriteDelay(writer, delay);
    if (looped) {
        WriteJump(writer, loopPoint);
    }
    WriteDisablecan(writer, 0b11);
    writer->Write(static_cast<uint8_t>(0xFF));

    lChannelStart = writer->GetBaseAddress();
    // Left Channel header
    WriteNoshort(writer);
    lLayerPlaceholderOff = writer->GetBaseAddress();
    WriteLdlayer(writer, 0, 0);
    WritePan(writer, 0);
    WriteVolCHeader(writer, 127); // Max volume
    WriteBend(writer, 0);
    WriteInstrument(writer, 0);
    WriteDelay(writer, delay);
    writer->Write(static_cast<uint8_t>(0xFF));

    rChannelStart = writer->GetBaseAddress();
    // Right Channel header
    WriteNoshort(writer);
    rLayerPlaceholderOff = writer->GetBaseAddress();
    WriteLdlayer(writer, 1, 0);
    WritePan(writer, 127);
    WriteVolCHeader(writer, 127); // Max volume
    WriteBend(writer, 0);
    WriteInstrument(writer, 1);
    WriteDelay(writer, delay);
    writer->Write(static_cast<uint8_t>(0xFF));
    uint16_t placeHolder = writer->GetBaseAddress();
    writer->Seek(channelPlaceholderOff, Ship::SeekOffsetType::Start);
    WriteLdchan(writer, 0, lChannelStart);
    WriteLdchan(writer, 1, rChannelStart);
    writer->Seek(placeHolder, Ship::SeekOffsetType::Start);

    // Left Note layer
    lLayerOffset = writer->GetBaseAddress();
    WriteLegato(writer);
    WriteNotedvg(writer, 39, 0x7FFF - 1, static_cast<uint8_t>(0x7F), static_cast<uint8_t>(1));
    writer->Write(static_cast<uint8_t>(0xFF));

    // Right Note layer
    rLayerOffset = writer->GetBaseAddress();
    WriteLegato(writer);
    WriteNotedvg(writer, 39, 0x7FFF - 1, static_cast<uint8_t>(0x7F), static_cast<uint8_t>(1));
    writer->Write(static_cast<uint8_t>(0xFF));

    writer->Seek(lLayerPlaceholderOff, Ship::SeekOffsetType::Start);
    WriteLdlayer(writer, 0, lLayerOffset);
    writer->Seek(rLayerPlaceholderOff, Ship::SeekOffsetType::Start);
    WriteLdlayer(writer, 1, rLayerOffset);
}

std::shared_ptr<Ship::IResource>
ResourceFactoryXMLAudioSequenceV0::ReadResource(std::shared_ptr<Ship::File> file,
                                                std::shared_ptr<Ship::ResourceInitData> initData) {
    if (!FileHasValidFormatAndReader(file, initData)) {
        return nullptr;
    }

    auto sequence = std::make_shared<AudioSequence>(initData);
    auto child = std::get<std::shared_ptr<tinyxml2::XMLDocument>>(file->Reader)->FirstChildElement();
    unsigned int i = 0;

    sequence->sequence.medium =
        ResourceFactoryXMLSoundFontV0::MediumStrToInt(child->Attribute("Medium"), initData->Path.c_str());
    sequence->sequence.cachePolicy =
        ResourceFactoryXMLSoundFontV0::CachePolicyToInt(child->Attribute("CachePolicy"), initData->Path.c_str());
    sequence->sequence.seqDataSize = child->IntAttribute("Size");
    sequence->sequence.seqNumber = child->IntAttribute("Index");
    bool streamed = child->BoolAttribute("Streamed");

    memset(sequence->sequence.fonts, 0, sizeof(sequence->sequence.fonts));

    tinyxml2::XMLElement* fontsElement = child->FirstChildElement();
    tinyxml2::XMLElement* fontElement = fontsElement->FirstChildElement();
    while (fontElement != nullptr) {
        sequence->sequence.fonts[i] = fontElement->IntAttribute("FontIdx");
        fontElement = fontElement->NextSiblingElement();
        i++;
    }
    sequence->sequence.numFonts = i;

    const char* path = child->Attribute("Path");
    std::shared_ptr<Ship::File> seqFile;
    if (path != nullptr) {
        seqFile = Ship::Context::GetInstance()->GetResourceManager()->GetArchiveManager()->LoadFile(path);
    }

    if (!streamed) {
        sequence->sequence.seqDataSize = seqFile->Buffer.get()->size();
        sequence->sequence.seqData = new char[seqFile->Buffer.get()->size()];
        memcpy(sequence->sequence.seqData, seqFile->Buffer.get()->data(), seqFile->Buffer.get()->size());
    } else {
        // setting numFonts to -1 tells the game's audio engine the sound font to used is CRC64 encoded in the font
        // indicies.
        sequence->sequence.numFonts = -1;
        if (path != nullptr) {
            sequence->sequence.seqDataSize = seqFile->Buffer.get()->size();
            sequence->sequence.seqData = new char[seqFile->Buffer.get()->size()];
            memcpy(sequence->sequence.seqData, seqFile->Buffer.get()->data(), seqFile->Buffer.get()->size());
        } else {
            unsigned int length = child->UnsignedAttribute("Length");
            bool looped = child->BoolAttribute("Looped", true);
            bool stereo = child->BoolAttribute("Stereo", false);
            Ship::BinaryWriter writer = Ship::BinaryWriter();
            writer.SetEndianness(Ship::Endianness::Big);

            // 1 second worth of ticks can be found by using `ticks = 60 / (bpm * 48)`
            // Get the number of ticks per second and then divide the length by this number to get the number of ticks
            // for the song.
            constexpr uint8_t TEMPO = 1;
            constexpr float TEMPO_F = TEMPO;
            // Use floats for this first calculation so we can round up
            float delayF = length / (60.0f / (TEMPO_F * 48.0f));
            // Convert to u16. This way this value is encoded changes depending on the value.
            // It can be at most 0xFFFF so store it in a u16 for now.
            uint16_t delay;
            if (delayF >= 65535.0f) {
                delay = 0x7FFF;
            } else {
                delay = delayF;
            }
            if (stereo) {
                WriteStereoSingleSeq(&writer, delay, TEMPO, looped);
            } else {
                WriteMonoSingleSeq(&writer, delay, TEMPO, looped);
            }
            sequence->sequence.seqDataSize = writer.ToVector().size();
            sequence->sequence.seqData = new char[sequence->sequence.seqDataSize];
            memcpy(sequence->sequence.seqData, writer.ToVector().data(), sequence->sequence.seqDataSize);
        }
    }

    return sequence;
}
} // namespace SOH