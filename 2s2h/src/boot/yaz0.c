#include "global.h"
#include "fault.h"
#include <stdio.h>
#include <string.h>

// 2S2H [Port] We don't use any Yaz0 data. Applies to everything removed in this file.
#if 0
u8 sYaz0DataBuffer[0x400] ALIGNED(16);
u8* sYaz0CurDataEnd;
uintptr_t sYaz0CurRomStart;
u32 sYaz0CurSize;
u8* sYaz0MaxPtr;
void* gYaz0DecompressDstEnd;
#endif

void* Yaz0_FirstDMA() {
#if 0
    u32 pad0;
    u32 dmaSize;
    u32 curSize;

    sYaz0MaxPtr = sYaz0CurDataEnd - 0x19;

    curSize = (u32)sYaz0CurDataEnd - (u32)sYaz0DataBuffer;
    dmaSize = (curSize > sYaz0CurSize) ? sYaz0CurSize : curSize;

    DmaMgr_DmaRomToRam(sYaz0CurRomStart, sYaz0DataBuffer, dmaSize);
    sYaz0CurRomStart += dmaSize;
    sYaz0CurSize -= dmaSize;
#endif
    return NULL;
}

void* Yaz0_NextDMA(void* curSrcPos) {
#if 0
    u8* dst;
    u32 restSize;
    u32 dmaSize;
    OSPri oldPri;

    restSize = (u32)sYaz0CurDataEnd - (u32)curSrcPos;

    dst = (restSize & 7) ? (sYaz0DataBuffer - (restSize & 7)) + 8 : sYaz0DataBuffer;

    memcpy(dst, curSrcPos, restSize);
    dmaSize = ((u32)sYaz0CurDataEnd - (u32)dst) - restSize;
    if (sYaz0CurSize < dmaSize) {
        dmaSize = sYaz0CurSize;
    }

    if (dmaSize != 0) {
        DmaMgr_DmaRomToRam(sYaz0CurRomStart, dst + restSize, dmaSize);
        sYaz0CurRomStart += dmaSize;
        sYaz0CurSize -= dmaSize;
        if (!sYaz0CurSize) {
            sYaz0MaxPtr = dst + restSize + dmaSize;
        }
    } else {
        oldPri = osGetThreadPri(NULL);
        osSetThreadPri(NULL, 0x7F);
        osSyncPrintf("圧縮展開異常\n");
        osSetThreadPri(NULL, oldPri);
    }

    return dst;
#endif
}

typedef struct {
    /* 0x0 */ u32 magic; // Yaz0
    /* 0x4 */ u32 decSize;
    /* 0x8 */ u32 compInfoOffset;   // only used in mio0
    /* 0xC */ u32 uncompDataOffset; // only used in mio0
} Yaz0Header;                       // size = 0x10

#define YAZ0_MAGIC 0x59617A30 // "Yaz0"

s32 Yaz0_DecompressImpl(u8* src, u8* dst) {
#if 0
    u32 bitIdx = 0;
    u8* dstEnd;
    u32 chunkHeader = 0;
    u32 nibble;
    u8* backPtr;
    s32 chunkSize;
    u32 off;
    u32 magic;

    magic = ((Yaz0Header*)src)->magic;

    if (magic != YAZ0_MAGIC) {
        return -1;
    }

    dstEnd = dst + ((Yaz0Header*)src)->decSize;
    src += sizeof(Yaz0Header);

    do {
        if (bitIdx == 0) {
            if ((sYaz0MaxPtr < src) && (sYaz0CurSize != 0)) {
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

    gYaz0DecompressDstEnd = dstEnd;

    return 0;
#endif
}

void Yaz0_Decompress(uintptr_t romStart, void* dst, size_t size) {

#if 0
    s32 status;
    u32 pad;
    char sp80[0x50];
    char sp30[0x50];

    if (sYaz0CurDataEnd != NULL) {
        while (sYaz0CurDataEnd != NULL) {
            Sleep_Usec(10);
        }
    }

    sYaz0CurDataEnd = sYaz0DataBuffer + sizeof(sYaz0DataBuffer);
    sYaz0CurRomStart = romStart;
    sYaz0CurSize = size;
    status = Yaz0_DecompressImpl(Yaz0_FirstDMA(), dst);

    if (status != 0) {
        sprintf(sp80, "slidma slidstart_szs ret=%d", status);
        sprintf(sp30, "src:%08lx dst:%08lx siz:%08lx", romStart, dst, size);
        Fault_AddHungupAndCrashImpl(sp80, sp30);
    }

    sYaz0CurDataEnd = NULL;
#endif
}
