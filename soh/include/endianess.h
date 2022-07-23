#ifndef ENDIANESS_H
#define ENDIANESS_H

#ifdef _MSC_VER
#define BOMSWAP16 _byteswap_ushort
#define BOMSWAP32 _byteswap_ulong
#define BOMSWAP64 _byteswap_uint64
#else
#define BOMSWAP16 __builtin_bswap16
#define BOMSWAP32 __builtin_bswap32
#define BOMSWAP64 __builtin_bswap64
#endif

#if (defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)) || defined(__BIG_ENDIAN__)
#define IS_BIGENDIAN
#endif

#endif
