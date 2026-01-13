#pragma once

#include <cstdint>
#include <ship/resource/Resource.h>

namespace SOH {

typedef struct {
    char* seqData;
    uint32_t seqDataSize;
    uint16_t seqNumber;
    uint8_t medium;
    uint8_t cachePolicy;
    uint32_t numFonts;
    uint8_t fonts[16];
} Sequence;

class AudioSequence : public Ship::Resource<Sequence> {
  public:
    using Resource::Resource;

    AudioSequence() : Resource(std::shared_ptr<Ship::ResourceInitData>()) {
    }
    ~AudioSequence();

    Sequence* GetPointer();
    size_t GetPointerSize();

    Sequence sequence;
};
}; // namespace SOH