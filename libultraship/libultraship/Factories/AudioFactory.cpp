#include "AudioFactory.h"

namespace Ship
{
    Audio* AudioFactory::ReadAudio(BinaryReader* reader)
    {
        Audio* audio = new Audio();
        Version version = (Version)reader->ReadUInt32();

        switch (version)
        {
        case Version::Roy:
        {
            AudioV1 audioFac = AudioV1();
            audioFac.ParseFileBinary(reader, audio);
        }
        break;
        default:
            // VERSION NOT SUPPORTED
            break;
        }

        return audio;
    }

    AudioSample* AudioSampleFactory::ReadAudioSample(BinaryReader* reader)
    {
        AudioSample* audioSample = new AudioSample();
        Version version = (Version)reader->ReadUInt32();

        switch (version)
        {
        case Version::Deckard: // OTRTODO: Remove this line after we merge in that refactor
        case Version::Roy:
        {
            AudioSampleV1 audioSampleFac = AudioSampleV1();
            audioSampleFac.ParseFileBinary(reader, audioSample);
        }
        break;
        default:
            // VERSION NOT SUPPORTED
            break;
        }

        return audioSample;
    }

    AudioSoundFont* AudioSoundFontFactory::ReadAudioSoundFont(BinaryReader* reader)
    {
        AudioSoundFont* audioSF = new AudioSoundFont();
        Version version = (Version)reader->ReadUInt32();

        switch (version)
        {
        case Version::Deckard: // OTRTODO: Remove this line after we merge in that refactor
        case Version::Roy:
        {
            AudioSoundFontV1 audioSFFac = AudioSoundFontV1();
            audioSFFac.ParseFileBinary(reader, audioSF);
        }
        break;
        default:
            // VERSION NOT SUPPORTED
            break;
        }

        return audioSF;
    }
};