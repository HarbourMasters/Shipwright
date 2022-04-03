#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <list>
#include "readwrite.h"

#include "yaz0.h"

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

/* internal declarations */
int yaz0_encode_internal(const u8* src, int srcSize, u8* Data);

int yaz0_get_size(u8* src) { return U32(src + 0x4); }

u32 toDWORD(u32 d) {
  u8 w1 = d & 0xFF;
  u8 w2 = (d >> 8) & 0xFF;
  u8 w3 = (d >> 16) & 0xFF;
  u8 w4 = d >> 24;
  return (w1 << 24) | (w2 << 16) | (w3 << 8) | w4;
}

// simple and straight encoding scheme for Yaz0
u32 longest_match_brute(const u8* src, int size, int pos, u32* pMatchPos) {
  int startPos = pos - 0x1000;
  int max_match_size = size - pos;
  u32 best_match_size = 0;
  u32 best_match_pos = 0;

  if (max_match_size < 3) return 0;

  if (startPos < 0) startPos = 0;

  if (max_match_size > 0x111) max_match_size = 0x111;

  for (int i = startPos; i < pos; i++) {
    int current_size;
    for (current_size = 0; current_size < max_match_size; current_size++) {
      if (src[i + current_size] != src[pos + current_size]) {
	break;
      }
    }
    if (current_size > best_match_size) {
      best_match_size = current_size;
      best_match_pos = i;
      if (best_match_size == 0x111) break;
    }
  }
  *pMatchPos = best_match_pos;
  return best_match_size;
}

u32 longest_match_rabinkarp(const u8* src, int size, int pos, u32* match_pos) {
  int startPos = pos - 0x1000;
  int max_match_size = size - pos;
  u32 best_match_size = 0;
  u32 best_match_pos = 0;

  if (max_match_size < 3) return 0;

  if (startPos < 0) startPos = 0;

  if (max_match_size > 0x111) max_match_size = 0x111;

  int find_hash = src[pos] << 16 | src[pos + 1] << 8 | src[pos + 2];
  int current_hash = src[startPos] << 16 | src[startPos + 1] << 8 | src[startPos + 2];

  for (int i = startPos; i < pos; i++) {
    if(current_hash == find_hash) {
      int current_size;
      for (current_size = 3; current_size < max_match_size; current_size++) {
        if (src[i + current_size] != src[pos + current_size]) {
          break;
        }
      }
      if (current_size > best_match_size) {
        best_match_size = current_size;
        best_match_pos = i;
        if (best_match_size == 0x111) break;
      }
    }
    current_hash = (current_hash << 8 | src[i + 3]) & 0xFFFFFF;
  }
  *match_pos = best_match_pos;

  return best_match_size;
}

int yaz0_encode_internal(const u8* src, int srcSize, u8* Data) {
  int srcPos = 0;

  int bitmask = 0x80;
  u8 currCodeByte = 0;
  int currCodeBytePos = 0;
  int pos = currCodeBytePos + 1;

  while (srcPos < srcSize) {
    u32 numBytes;
    u32 matchPos;

    numBytes = longest_match_rabinkarp(src, srcSize, srcPos, &matchPos);
    //fprintf(stderr, "pos %x len %x pos %x\n", srcPos, (int)numBytes, (int)matchPos);
    if (numBytes < 3) {
      //fprintf(stderr, "single byte %02x\n", src[srcPos]);
      Data[pos++] = src[srcPos++];
      currCodeByte |= bitmask;
    } else {
      // RLE part
      u32 dist = srcPos - matchPos - 1;

      if (numBytes >= 0x12)  // 3 byte encoding
      {
        Data[pos++] = dist >> 8;    // 0R
        Data[pos++] = dist & 0xFF;  // FF
        if (numBytes > 0xFF + 0x12) numBytes = 0xFF + 0x12;
        Data[pos++] = numBytes - 0x12;
      } else  // 2 byte encoding
      {
        Data[pos++] = ((numBytes - 2) << 4) | (dist >> 8);
        Data[pos++] = dist & 0xFF;
      }
      srcPos += numBytes;
    }
    bitmask >>= 1;
    // write eight codes
    if (!bitmask) {
      Data[currCodeBytePos] = currCodeByte;
      currCodeBytePos = pos++;

      currCodeByte = 0;
      bitmask = 0x80;
    }
  }
  if (bitmask) {
    Data[currCodeBytePos] = currCodeByte;
  }

  return pos;
}

std::vector<uint8_t> yaz0_encode_fast(const u8* src, int src_size) {
  std::vector<uint8_t> buffer;
  std::vector<std::list<uint32_t>> lut;
  lut.resize(0x1000000);

  for (int i = 0; i < src_size - 3; ++i) {
    lut[src[i + 0] << 16 | src[i + 1] << 8 | src[i + 2]].push_back(i);
  }

  return buffer;
}

std::vector<uint8_t> yaz0_encode(const u8* src, int src_size) {
  std::vector<uint8_t> buffer(src_size * 10 / 8 + 16);
  u8* dst = buffer.data();

  // write 4 bytes yaz0 header
  memcpy(dst, "Yaz0", 4);

  // write 4 bytes uncompressed size
  W32(dst + 4, src_size);

  // encode
  int dst_size = yaz0_encode_internal(src, src_size, dst + 16);
  int aligned_size = (dst_size + 31) & -16;
  buffer.resize(aligned_size);

#if 0
  std::vector<uint8_t> decompressed(src_size);
  yaz0_decode(buffer.data(), decompressed.data(), src_size);
  if(memcmp(src, decompressed.data(), src_size)) {
    fprintf(stderr, "Decompressed buffer is different from original\n");
  }
#endif

  return buffer;
}

void yaz0_decode(const uint8_t* source, uint8_t* decomp, int32_t decompSize) {
  uint32_t srcPlace = 0, dstPlace = 0;
  uint32_t i, dist, copyPlace, numBytes;
  uint8_t codeByte, byte1, byte2;
  uint8_t bitCount = 0;

  source += 0x10;
  while (dstPlace < decompSize) {
    /* If there are no more bits to test, get a new byte */
    if (!bitCount) {
      codeByte = source[srcPlace++];
      bitCount = 8;
    }

    /* If bit 7 is a 1, just copy 1 byte from source to destination */
    /* Else do some decoding */
    if (codeByte & 0x80) {
      decomp[dstPlace++] = source[srcPlace++];
    } else {
      /* Get 2 bytes from source */
      byte1 = source[srcPlace++];
      byte2 = source[srcPlace++];

      /* Calculate distance to move in destination */
      /* And the number of bytes to copy */
      dist = ((byte1 & 0xF) << 8) | byte2;
      copyPlace = dstPlace - (dist + 1);
      numBytes = byte1 >> 4;

      /* Do more calculations on the number of bytes to copy */
      if (!numBytes)
        numBytes = source[srcPlace++] + 0x12;
      else
        numBytes += 2;

      /* Copy data from a previous point in destination */
      /* to current point in destination */
      for (i = 0; i < numBytes; i++) decomp[dstPlace++] = decomp[copyPlace++];
    }

    /* Set up for the next read cycle */
    codeByte = codeByte << 1;
    bitCount--;
  }
}