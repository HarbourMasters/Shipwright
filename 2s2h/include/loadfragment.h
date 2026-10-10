#ifndef LOADFRAGMENT_H
#define LOADFRAGMENT_H

#include "PR/ultratypes.h"
#include "libc/stdint.h"
#include "libc/stddef.h"

extern s32 gOverlayLogSeverity;

#define RELOC_SECTION(reloc) ((reloc) >> 30)
#define RELOC_OFFSET(reloc) ((reloc) & 0xFFFFFF)
#define RELOC_TYPE_MASK(reloc) ((reloc) & 0x3F000000)
#define RELOC_TYPE_SHIFT 24

/* MIPS Relocation Types */
#define R_MIPS_32 2
#define R_MIPS_26 4
#define R_MIPS_HI16 5
#define R_MIPS_LO16 6

typedef enum {
    /* 0 */ RELOC_SECTION_NULL,
    /* 1 */ RELOC_SECTION_TEXT,
    /* 2 */ RELOC_SECTION_DATA,
    /* 3 */ RELOC_SECTION_RODATA,
    /* 4 */ RELOC_SECTION_MAX
} RelocSectionId;

typedef struct OverlayRelocationSection {
    /* 0x00 */ size_t textSize;
    /* 0x04 */ size_t dataSize;
    /* 0x08 */ size_t rodataSize;
    /* 0x0C */ size_t bssSize;
    /* 0x10 */ u32 numRelocations;
    /* 0x14 */ u32 relocations[1]; // array count is numRelocations
} OverlayRelocationSection; // size >= 0x18

// Fragment overlay load functions
size_t Overlay_Load(uintptr_t vromStart, uintptr_t vromEnd, void* ramStart, void* ramEnd, void* allocatedRamAddr);
void* Overlay_AllocateAndLoad(uintptr_t vromStart, uintptr_t vromEnd, void* vramStart, void* vramEnd);

#endif
