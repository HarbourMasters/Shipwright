#ifndef ENDIANESS_H
#define ENDIANESS_H

#ifdef __cplusplus
namespace Ship
{
    enum class Endianness
    {
        Little = 0,
        Big = 1,

#if (defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)) || defined(__BIG_ENDIAN__)
        Native = Big,
#else
        Native = Little,
#endif
    };
}
#endif

#ifdef _MSC_VER
#include <stdlib.h>

#define BSWAP16 _byteswap_ushort
#define BSWAP32 _byteswap_ulong
#define BSWAP64 _byteswap_uint64

#define BOMSWAP16 _byteswap_ushort
#define BOMSWAP32 _byteswap_ulong
#define BOMSWAP64 _byteswap_uint64

#define BOMSWAP16_CONST(x) \
    ((((x) >> 8) & 0x00FF) | (((x) << 8) & 0xFF00))
#define BOMSWAP32_CONST(x) \
    ((((x) >> 24) & 0x000000FF) | (((x) >>  8) & 0x0000FF00) | \
     (((x) <<  8) & 0x00FF0000) | (((x) << 24) & 0xFF000000))
#define BOMSWAP64_CONST(x) \
    ((((x) >> 56) & 0x00000000000000FF) | (((x) >> 40) & 0x000000000000FF00) | \
     (((x) >> 24) & 0x0000000000FF0000) | (((x) >>  8) & 0x00000000FF000000) | \
     (((x) <<  8) & 0x000000FF00000000) | (((x) << 24) & 0x0000FF0000000000) | \
     (((x) << 40) & 0x00FF000000000000) | (((x) << 56) & 0xFF00000000000000))
#else

#define BSWAP16 __builtin_bswap16
#define BSWAP32 __builtin_bswap32
#define BSWAP64 __builtin_bswap64

#define BOMSWAP16 __builtin_bswap16
#define BOMSWAP32 __builtin_bswap32
#define BOMSWAP64 __builtin_bswap64

#define BOMSWAP16_CONST __builtin_bswap16
#define BOMSWAP32_CONST __builtin_bswap32
#define BOMSWAP64_CONST __builtin_bswap64
#endif

#if (defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)) || defined(__BIG_ENDIAN__)
#ifndef IS_BIGENDIAN
#define IS_BIGENDIAN
#endif
#endif

#ifdef IS_BIGENDIAN
#define BE16SWAP(x) (x)
#define BE32SWAP(x) (x)
#define BE64SWAP(x) (x)
#define LE16SWAP(x) BOMSWAP16(x)
#define LE32SWAP(x) BOMSWAP32(x)
#define LE64SWAP(x) BOMSWAP64(x)

#define BE16SWAP_CONST(x) (x)
#define BE32SWAP_CONST(x) (x)
#define BE64SWAP_CONST(x) (x)
#define LE16SWAP_CONST(x) BOMSWAP16_CONST(x)
#define LE32SWAP_CONST(x) BOMSWAP32_CONST(x)
#define LE64SWAP_CONST(x) BOMSWAP64_CONST(x)
#else
#define BE16SWAP(x) BOMSWAP16(x)
#define BE32SWAP(x) BOMSWAP32(x)
#define BE64SWAP(x) BOMSWAP64(x)
#define LE16SWAP(x) (x)
#define LE32SWAP(x) (x)
#define LE64SWAP(x) (x)

#define BE16SWAP_CONST(x) BOMSWAP16_CONST(x)
#define BE32SWAP_CONST(x) BOMSWAP32_CONST(x)
#define BE64SWAP_CONST(x) BOMSWAP64_CONST(x)
#define LE16SWAP_CONST(x) (x)
#define LE32SWAP_CONST(x) (x)
#define LE64SWAP_CONST(x) (x)
#endif

#endif
