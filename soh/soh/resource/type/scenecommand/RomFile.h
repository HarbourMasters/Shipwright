#pragma once

#include <cstdint>

namespace LUS {
    typedef struct {
        /* 0x00 */ uintptr_t vromStart;
        /* 0x04 */ uintptr_t vromEnd;
        char* fileName;
    } RomFile; // size = 0x8
}