#include "../Audio.h"
#include "Utils/BinaryReader.h"

namespace Ship
{
    class AudioFactory
    {
    public:
        static Audio* ReadAudio(BinaryReader* reader, bool readFullHeader);
    };

    class AudioSampleFactory
    {
    public:
        static AudioSample* ReadAudioSample(BinaryReader* reader, bool readFullHeader);
    };

    class AudioSoundFontFactory
    {
    public:
        static AudioSoundFont* ReadAudioSoundFont(BinaryReader* reader, bool readFullHeader);
    };

    class AudioSequenceFactory
    {
    public:
        static AudioSequence* ReadAudioSequence(BinaryReader* reader, bool readFullHeader);
    };
}