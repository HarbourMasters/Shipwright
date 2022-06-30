#include "AudioFactory.h"

namespace Ship
{
    Audio* AudioFactory::ReadAudio(BinaryReader* reader, bool readFullHeader)
    {
        Audio* audio = new Audio();
        Version version = (Version)reader->ReadUInt32();

        switch (version)
        {
        case Version::Rachael:
        {
            AudioV2 audioFac = AudioV2();
            audioFac.ParseFileBinary(reader, audio, readFullHeader);
        }
        break;
        default:
            // VERSION NOT SUPPORTED
            break;
        }

        return audio;
    }

    AudioSample* AudioSampleFactory::ReadAudioSample(BinaryReader* reader, bool readFullHeader)
    {
        AudioSample* audioSample = new AudioSample();
        Version version = (Version)reader->ReadUInt32();

        switch (version)
        {
        case Version::Rachael:
        {
            AudioSampleV2 audioSampleFac = AudioSampleV2();
            audioSampleFac.ParseFileBinary(reader, audioSample, readFullHeader);
        }
        break;
        default:
            // VERSION NOT SUPPORTED
            break;
        }

        return audioSample;
    }

    AudioSoundFont* AudioSoundFontFactory::ReadAudioSoundFont(BinaryReader* reader, bool readFullHeader)
    {
        AudioSoundFont* audioSF = new AudioSoundFont();
        Version version = (Version)reader->ReadUInt32();

        switch (version)
        {
        case Version::Rachael:
        {
            AudioSoundFontV2 audioSFFac = AudioSoundFontV2();
            audioSFFac.ParseFileBinary(reader, audioSF, readFullHeader);
        }
        break;
        default:
            // VERSION NOT SUPPORTED
            break;
        }

        return audioSF;
    }

    AudioSequence* AudioSequenceFactory::ReadAudioSequence(BinaryReader* reader, bool readFullHeader)
    {
        AudioSequence* audioSeq = new AudioSequence();
        Version version = (Version)reader->ReadUInt32();

        switch (version)
        {
        case Version::Rachael:
        {
            AudioSequenceV2 audioSeqFac = AudioSequenceV2();
            audioSeqFac.ParseFileBinary(reader, audioSeq, readFullHeader);
        }
        break;
        default:
            // VERSION NOT SUPPORTED
            break;
        }

        return audioSeq;
    }
};