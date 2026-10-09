/**
 * @file loadfragment2.c
 *
 * Functions used to process and relocate dynamically loadable code segments (overlays).
 *
 * @note:
 *     These are for specific fragment overlays with the .ovl file extension
 */
#include "global.h"
#include "system_malloc.h"
#include "loadfragment.h"

s32 gOverlayLogSeverity = 2;

// Extract MIPS register rs from an instruction word
#define MIPS_REG_RS(insn) (((insn) >> 0x15) & 0x1F)

// Extract MIPS register rt from an instruction word
#define MIPS_REG_RT(insn) (((insn) >> 0x10) & 0x1F)

// Extract MIPS jump target from an instruction word
#define MIPS_JUMP_TARGET(insn) (((insn)&0x03FFFFFF) << 2)

/**
 * Performs runtime relocation of overlay files, loadable code segments.
 *
 * Overlays are expected to be loadable anywhere in direct-mapped cached (KSEG0) memory, with some appropriate
 * alignment requirements; memory addresses in such code must be updated once loaded to execute properly.
 * When compiled, overlays are given 'fake' KSEG0 RAM addresses larger than the total possible available main memory
 * (>= 0x80800000), such addresses are referred to as Virtual RAM (VRAM) to distinguish them. When loading the overlay,
 * the relocation table produced at compile time is consulted to determine where and how to update these VRAM addresses
 * to correct RAM addresses based on the location the overlay was loaded at, enabling the code to execute at this
 * address as if it were compiled to run at this address.
 *
 * Each relocation is represented by a packed 32-bit value, formatted in the following way:
 *  - [31:30]  2-bit section id, taking values from the `RelocSectionId` enum.
 *  - [29:24]  6-bit relocation type describing which relocation operation should be performed. Same as ELF32 MIPS.
 *  - [23: 0]  24-bit section-relative offset indicating where in the section to apply this relocation.
 *
 * @param allocatedRamAddress Memory address the binary was loaded at.
 * @param ovlRelocs Overlay relocation section containing overlay section layout and runtime relocations.
 * @param vramStart Virtual RAM address that the overlay was compiled at.
 */
void Overlay_Relocate(void* allocatedRamAddr, OverlayRelocationSection* ovlRelocs, uintptr_t vramStart) {
    u32 sections[RELOC_SECTION_MAX];
    u32* relocDataP;
    u32 reloc;
    uintptr_t relocatedAddress;
    u32 i;
    u32* luiInstRef;
    u32 isLoNeg;
    u32* regValP;
    //! MIPS ELF relocation does not generally require tracking register values, so at first glance it appears this
    //! register tracking was an unnecessary complication. However there is a bug in the IDO compiler that can cause
    //! relocations to be emitted in the wrong order under rare circumstances when the compiler attempts to reuse a
    //! previous HI16 relocation for a different LO16 relocation as an optimization. This register tracking is likely
    //! a workaround to prevent improper matching of unrelated HI16 and LO16 relocations that would otherwise arise
    //! due to the incorrect ordering.
    u32* luiRefs[32];
    u32 luiVals[32];
    uintptr_t allocu32 = (uintptr_t)allocatedRamAddr;
    uintptr_t vramu32 = (uintptr_t)vramStart;

    if (gOverlayLogSeverity >= 3) {}

    sections[RELOC_SECTION_NULL] = 0;
    sections[RELOC_SECTION_TEXT] = allocu32;
    sections[RELOC_SECTION_DATA] = allocu32 + ovlRelocs->textSize;
    sections[RELOC_SECTION_RODATA] = sections[RELOC_SECTION_DATA] + ovlRelocs->dataSize;

    for (i = 0; i < ovlRelocs->numRelocations; i++) {
        // This will always resolve to a 32-bit aligned address as each section
        // containing code or pointers must be aligned to at least 4 bytes and the
        // MIPS ABI defines the offset of both 16-bit and 32-bit relocations to be
        // the start of the 32-bit word containing the target.
        reloc = ovlRelocs->relocations[i];
        relocDataP = (u32*)(sections[RELOC_SECTION(reloc)] + RELOC_OFFSET(reloc));

        switch (RELOC_TYPE_MASK(reloc)) {
            case R_MIPS_32 << RELOC_TYPE_SHIFT:
                // Handles 32-bit address relocation, used for things such as jump tables and pointers in data.
                // Just relocate the full address

                // Check address is valid for relocation
                if ((*relocDataP & 0x0F000000) == 0) {
                    *relocDataP = *relocDataP - vramu32 + allocu32;
                } else if (gOverlayLogSeverity >= 3) {
                }
                break;

            case R_MIPS_26 << RELOC_TYPE_SHIFT:
                // Handles 26-bit address relocation, used for jumps and jals.
                // Extract the address from the target field of the J-type MIPS instruction.
                // Relocate the address and update the instruction.

                if (1) {
                    *relocDataP =
                        (*relocDataP & 0xFC000000) |
                        (((PHYS_TO_K0(MIPS_JUMP_TARGET(*relocDataP)) - vramu32 + allocu32) & 0x0FFFFFFF) >> 2);
                } else if (gOverlayLogSeverity >= 3) {
                    // Segment pointer 26 %08x
                    // "セグメントポインタ26です %08x\n"
                }
                break;

            case R_MIPS_HI16 << RELOC_TYPE_SHIFT:
                // Handles relocation for a hi/lo pair, part 1.
                // Store the reference to the LUI instruction (hi) using the `rt` register of the instruction.
                // This will be updated later in the `R_MIPS_LO16` section.

                luiRefs[(*relocDataP >> 0x10) & 0x1F] = relocDataP;
                luiVals[(*relocDataP >> 0x10) & 0x1F] = *relocDataP;
                break;

            case R_MIPS_LO16 << RELOC_TYPE_SHIFT:
                // Handles relocation for a hi/lo pair, part 2.
                // Grab the stored LUI (hi) from the `R_MIPS_HI16` section using the `rs` register of the instruction.
                // The full address is calculated, relocated, and then used to update both the LUI and lo instructions.
                // If the lo part is negative, add 1 to the LUI value.
                // Note: The lo instruction is assumed to have a signed immediate.

                luiInstRef = luiRefs[(*relocDataP >> 0x15) & 0x1F];
                regValP = &luiVals[(*relocDataP >> 0x15) & 0x1F];

                // Check address is valid for relocation
                if ((((*luiInstRef << 0x10) + (s16)*relocDataP) & 0x0F000000) == 0) {
                    relocatedAddress = ((*regValP << 0x10) + (s16)*relocDataP) - vramu32 + allocu32;
                    isLoNeg = (relocatedAddress & 0x8000) ? 1 : 0;
                    *luiInstRef = (*luiInstRef & 0xFFFF0000) | (((relocatedAddress >> 0x10) & 0xFFFF) + isLoNeg);
                    *relocDataP = (*relocDataP & 0xFFFF0000) | (relocatedAddress & 0xFFFF);
                } else if (gOverlayLogSeverity >= 3) {
                }
                break;
        }
    }
}

size_t Overlay_Load(uintptr_t vromStart, uintptr_t vromEnd, void* ramStart, void* ramEnd, void* allocatedRamAddr) {
    return 0;
#if 0
    uintptr_t vramStart = (uintptr_t)ramStart;
    uintptr_t vramEnd = (uintptr_t)ramEnd;
    s32 size = vromEnd - vromStart;
    uintptr_t end;
    OverlayRelocationSection* ovlRelocs;

    if (gOverlayLogSeverity >= 3) {}
    if (gOverlayLogSeverity >= 3) {}

    end = (uintptr_t)allocatedRamAddr + size;
    DmaMgr_SendRequest0(allocatedRamAddr, vromStart, size);

    ovlRelocs = (OverlayRelocationSection*)(end - ((s32*)end)[-1]);

    if (gOverlayLogSeverity >= 3) {}
    if (gOverlayLogSeverity >= 3) {}

    Overlay_Relocate(allocatedRamAddr, ovlRelocs, vramStart);

    if (ovlRelocs->bssSize != 0) {
        if (gOverlayLogSeverity >= 3) {}
        bzero((void*)end, ovlRelocs->bssSize);
    }

    size = vramEnd - vramStart;

    osWritebackDCache(allocatedRamAddr, size);
    osInvalICache(allocatedRamAddr, size);

    if (gOverlayLogSeverity >= 3) {}

    return size;
#endif
}

void* Overlay_AllocateAndLoad(uintptr_t vromStart, uintptr_t vromEnd, void* vramStart, void* vramEnd) {
    void* allocatedRamAddr = SystemArena_MallocR((uintptr_t)vramEnd - (uintptr_t)vramStart);

    if (allocatedRamAddr != NULL) {
        Overlay_Load(vromStart, vromEnd, vramStart, vramEnd, allocatedRamAddr);
    }

    return allocatedRamAddr;
}
