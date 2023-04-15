#pragma once

#include <cstdint>
#include <vector>
#include "Resource.h"
#include <libultraship/libultra/types.h>

namespace Ship {

typedef struct {
    char* seqData;
    int32_t seqDataSize;
    uint16_t seqNumber;
    uint8_t medium;
    uint8_t cachePolicy;
    int32_t numFonts;
    uint8_t fonts[16];
} Sequence;

class AudioSequence : public Resource {
public:
    using Resource::Resource;

    void* GetPointer();
    size_t GetPointerSize();

    Sequence sequence;
    std::vector<char> sequenceData;
};
}; // namespace Ship
