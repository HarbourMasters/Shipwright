#ifndef __READWRITE_H__
#define __READWRITE_H__

#include <stdint.h>

/* variables */
union {
  uint32_t u;
  float f;
} __u32_f32_union__;

#define U32(x)                                                          \
  ((uint32_t)((((uint8_t*)(x))[0] << 24) | (((uint8_t*)(x))[1] << 16) | \
              (((uint8_t*)(x))[2] << 8) | ((uint8_t*)(x))[3]))
#define U16(x) ((uint16_t)(((*((uint8_t*)(x))) << 8) | ((uint8_t*)(x))[1]))
#define U8(x) ((uint8_t)((uint8_t*)(x))[0])
#define S32(x) ((int32_t)(U32(x)))
#define S16(x) ((int16_t)(U16(x)))
#define F32(x) (((__u32_f32_union__.u = U32(x)) & 0) + __u32_f32_union__.f)

#define W32(x, v)                              \
  {                                            \
    *((uint8_t*)x + 3) = ((v)&0xFF);           \
    *((uint8_t*)x + 2) = (((v) >> 8) & 0xFF);  \
    *((uint8_t*)x + 1) = (((v) >> 16) & 0xFF); \
    *((uint8_t*)x + 0) = (((v) >> 24) & 0xFF); \
  }

#endif