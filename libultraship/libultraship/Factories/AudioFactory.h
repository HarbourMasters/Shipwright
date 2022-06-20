#include "../Audio.h"
#include "Utils/BinaryReader.h"

namespace Ship
{
    class AudioFactory
    {
    public:
        static Audio* ReadAudio(BinaryReader* reader);
    };

    class AudioSampleFactory
    {
    public:
        static AudioSample* ReadAudioSample(BinaryReader* reader);
    };

    class AudioSoundFontFactory
    {
    public:
        static AudioSoundFont* ReadAudioSoundFont(BinaryReader* reader);
    };

    class AudioSequenceFactory
    {
    public:
        static AudioSequence* ReadAudioSequence(BinaryReader* reader);
    };
}