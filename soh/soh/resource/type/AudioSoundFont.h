#pragma once

#include <cstdint>
#include <vector>
#include "Resource.h"
#include "soh/resource/type/AudioSample.h"
#include <libultraship/libultra/types.h>

namespace SOH {

typedef struct {
    /* 0x0 */ s16 delay;
    /* 0x2 */ s16 arg;
} AdsrEnvelope; // size = 0x4

typedef struct {
    /* 0x00 */ Sample* sample;
    /* 0x04 */ union {
        u32 tuningAsU32;
        f32 tuning;// frequency scale factor
    };
} SoundFontSound; // size = 0x8

typedef struct {
    /* 0x00 */ u8 loaded;
    /* 0x01 */ u8 normalRangeLo;
    /* 0x02 */ u8 normalRangeHi;
    /* 0x03 */ u8 releaseRate;
    /* 0x04 */ AdsrEnvelope* envelope;
    /* 0x08 */ SoundFontSound lowNotesSound;
    /* 0x10 */ SoundFontSound normalNotesSound;
    /* 0x18 */ SoundFontSound highNotesSound;
} Instrument; // size = 0x20

typedef struct {
    /* 0x00 */ u8 releaseRate;
    /* 0x01 */ u8 pan;
    /* 0x02 */ u8 loaded;
    /* 0x04 */ SoundFontSound sound;
    /* 0x14 */ AdsrEnvelope* envelope;
} Drum; // size = 0x14

typedef struct {
    /* 0x00 */ u8 numInstruments;
    /* 0x01 */ u8 numDrums;
    /* 0x02 */ u8 sampleBankId1;
    /* 0x03 */ u8 sampleBankId2;
    /* 0x04 */ u16 numSfx;
    /* 0x08 */ Instrument** instruments;
    /* 0x0C */ Drum** drums;
    /* 0x10 */ SoundFontSound* soundEffects;
    s32 fntIndex;
} SoundFont; // size = 0x14

class AudioSoundFont : public Ship::Resource<SoundFont> {
public:
    using Resource::Resource;

    AudioSoundFont() : Resource(std::shared_ptr<Ship::ResourceInitData>()) {}

    SoundFont* GetPointer();
    size_t GetPointerSize();

    int8_t medium;
    int8_t cachePolicy;
    uint16_t data1;
    uint16_t data2;
    uint16_t data3;

    std::vector<Drum> drums;
    std::vector<Drum*> drumAddresses;
    std::vector<uint32_t> drumEnvelopeCounts;
    std::vector<std::vector<AdsrEnvelope>> drumEnvelopeArrays;

    std::vector<Instrument> instruments;
    std::vector<Instrument*> instrumentAddresses;
    std::vector<uint32_t> instrumentEnvelopeCounts;
    std::vector<std::vector<AdsrEnvelope>> instrumentEnvelopeArrays;

    std::vector<SoundFontSound> soundEffects;

    SoundFont soundFont;
};
}; // namespace LUS
