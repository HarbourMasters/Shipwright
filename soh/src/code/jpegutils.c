#include "global.h"

void JpegUtils_ProcessQuantizationTable(u8* dqt, JpegQuantizationTable* qt, u8 count) {
    u8 i;

    for (i = 0; i < count; i++) {
        u8 j;

        dqt++;
        for (j = 0; j < 64; j++) {
            qt[i].table[j] = *dqt++;
        }
    }
}

s32 JpegUtils_ParseHuffmanCodesLengths(u8* ptr, u8* codesLengths) {
    u8 off = 1;
    s16 count = 0;
    s16 idx = 1;

    while (off < 0x11) {
        while (idx <= ptr[off - 1]) {
            codesLengths[count++] = off;
            idx++;
        }
        idx = 1;
        off++;
    }

    codesLengths[count] = 0;
    return count;
}

s32 JpegUtils_GetHuffmanCodes(u8* codesLengths, u16* codes) {
    s16 idx = 0;
    u16 code = 0;
    u8 lastLen = codesLengths[0];

    while (true) {
        while (true) {
            codes[idx++] = code++;

            if (codesLengths[idx] != lastLen) {
                break;
            }
        }

        if (codesLengths[idx] == 0) {
            break;
        }

        while (true) {
            if (code <<= 1, codesLengths[idx] == ++lastLen) {
                break;
            }
        }
    }

    return idx;
}

s32 JpegUtils_SetHuffmanTable(u8* data, JpegHuffmanTable* ht, u16* codes) {
    u8 idx;
    u16 codeOff = 0;

    for (idx = 0; idx < 0x10; idx++) {
        if (data[idx]) {
            ht->codeOffs[idx] = codeOff;
            ht->codesA[idx] = codes[codeOff];
            codeOff += data[idx] - 1;
            ht->codesB[idx] = codes[codeOff];
            codeOff++;
        } else {
            ht->codesB[idx] = 0xFFFF;
        }
    }

    return codeOff;
}

u32 JpegUtils_ProcessHuffmanTableImpl(u8* data, JpegHuffmanTable* ht, u8* codesLengths, u16* codes, u8 isAc) {
    s16 ret;
    s32 count = JpegUtils_ParseHuffmanCodesLengths(data, codesLengths);
    s32 temp;

    ret = count;
    if (count == 0 || (isAc && count > 0x100) || (!isAc && count > 0x10)) {
        return 0;
    }
    if (ret != JpegUtils_GetHuffmanCodes(codesLengths, codes)) {
        return 0;
    }
    if (temp = JpegUtils_SetHuffmanTable(data, ht, codes), temp != ret) {
        return 0;
    }

    return ret;
}

u32 JpegUtils_ProcessHuffmanTable(u8* dht, JpegHuffmanTable* ht, u8* codesLengths, u16* codes, u8 count) {
    u8 idx;
    u32 codeCount;

    for (idx = 0; idx < count; idx++) {
        u32 ac = (*dht++ >> 4);

        codeCount = JpegUtils_ProcessHuffmanTableImpl(dht, &ht[idx], codesLengths, codes, ac);
        if (codeCount == 0) {
            return 1;
        }

        dht += 0x10;
        ht[idx].symbols = dht;
        dht += codeCount;
    }
    return 0;
}

void JpegUtils_SetHuffmanTableOld(u8* data, JpegHuffmanTableOld* ht, u8* codesLengths, u16* codes, s16 count, u8 isAc) {
    s16 idx;
    u8 a;

    for (idx = 0; idx < count; idx++) {
        a = data[idx];
        if (isAc) {
            ht->acCodes[a] = codes[idx];
            ht->codeOffs[a] = codesLengths[idx];
        } else {
            ht->dcCodes[a] = codes[idx];
            ht->codeOffs[a] = codesLengths[idx];
        }
    }
}

u32 JpegUtils_ProcessHuffmanTableImplOld(u8* dht, JpegHuffmanTableOld* ht, u8* codesLengths, u16* codes) {
    u8 isAc = *dht++ >> 4;
    s16 count2;
    s32 count;

    count2 = count = JpegUtils_ParseHuffmanCodesLengths(dht, codesLengths);

    if (count == 0 || (isAc && count > 0x100) || (!isAc && count > 0x10)) {
        return 1;
    }

    if (JpegUtils_GetHuffmanCodes(codesLengths, codes) != count2) {
        return 1;
    }

    JpegUtils_SetHuffmanTableOld(dht + 0x10, ht, codesLengths, codes, count2, isAc);

    return 0;
}
