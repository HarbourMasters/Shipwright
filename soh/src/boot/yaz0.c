#include "global.h"
#include <string.h>

u8 sYaz0DataBuffer[0x400];
uintptr_t sYaz0CurDataEnd;
uintptr_t sYaz0CurRomStart;
u32 sYaz0CurSize;
uintptr_t sYaz0MaxPtr;

void* Yaz0_FirstDMA(void) {
    u32 pad0;
    u32 pad1;
    u32 dmaSize;
    u32 curSize;

    sYaz0MaxPtr = sYaz0CurDataEnd - 0x19;

    curSize = sYaz0CurDataEnd - (uintptr_t)sYaz0DataBuffer;
    dmaSize = (curSize > sYaz0CurSize) ? sYaz0CurSize : curSize;

    DmaMgr_DmaRomToRam(sYaz0CurRomStart, sYaz0DataBuffer, dmaSize);
    sYaz0CurRomStart += dmaSize;
    sYaz0CurSize -= dmaSize;
    return sYaz0DataBuffer;
}

void* Yaz0_NextDMA(void* curSrcPos) {
    u8* dst;
    u32 restSize;
    u32 dmaSize;

    restSize = sYaz0CurDataEnd - (uintptr_t)curSrcPos;
    dst = (restSize & 7) ? (sYaz0DataBuffer - (restSize & 7)) + 8 : sYaz0DataBuffer;

    memcpy(dst, curSrcPos, restSize);
    dmaSize = (sYaz0CurDataEnd - (uintptr_t)dst) - restSize;
    if (sYaz0CurSize < dmaSize) {
        dmaSize = sYaz0CurSize;
    }

    if (dmaSize != 0) {
        DmaMgr_DmaRomToRam(sYaz0CurRomStart, (uintptr_t)dst + restSize, dmaSize);
        sYaz0CurRomStart += dmaSize;
        sYaz0CurSize -= dmaSize;
        if (!sYaz0CurSize) {
            sYaz0MaxPtr = (uintptr_t)dst + restSize + dmaSize;
        }
    }

    return dst;
}

void Yaz0_DecompressImpl(Yaz0Header* hdr, u8* dst) {
    u32 bitIdx = 0;
    u8* src = (u8*)hdr->data;
    u8* dstEnd = dst + hdr->decSize;
    u32 chunkHeader;
    u32 nibble;
    u8* backPtr;
    u32 chunkSize;
    u32 off;

    do {
        if (bitIdx == 0) {
            if ((sYaz0MaxPtr < (uintptr_t)src) && (sYaz0CurSize != 0)) {
                src = Yaz0_NextDMA(src);
            }

            chunkHeader = *src++;
            bitIdx = 8;
        }

        if (chunkHeader & (1 << 7)) { // uncompressed
            *dst = *src;
            dst++;
            src++;
        } else { // compressed
            off = ((*src & 0xF) << 8 | *(src + 1));
            nibble = *src >> 4;
            backPtr = dst - off;
            src += 2;

            chunkSize = (nibble == 0)              // N = chunkSize; B = back offset
                            ? (u32)(*src++ + 0x12) // 3 bytes 0B BB NN
                            : nibble + 2;          // 2 bytes NB BB

            do {
                *dst++ = *(backPtr++ - 1);
                chunkSize--;
            } while (chunkSize != 0);
        }
        chunkHeader <<= 1;
        bitIdx--;
    } while (dst != dstEnd);
}

void Yaz0_Decompress(uintptr_t romStart, void* dst, size_t size) {
    sYaz0CurRomStart = romStart;
    sYaz0CurSize = size;
    sYaz0CurDataEnd = sYaz0DataBuffer + sizeof(sYaz0DataBuffer);
    Yaz0_DecompressImpl(Yaz0_FirstDMA(), dst);
}
