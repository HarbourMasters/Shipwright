#ifndef ICHAIN_H
#define ICHAIN_H

typedef struct {
    u32 cont:   1;
    u32 type:   4;
    u32 offset: 11;
    s32 value:  16;
} InitChainEntry;

typedef enum {
    /* 0x0 */ ICHAINTYPE_U8,            // sets byte
    /* 0x1 */ ICHAINTYPE_S8,
    /* 0x2 */ ICHAINTYPE_U16,           // sets short
    /* 0x3 */ ICHAINTYPE_S16,
    /* 0x4 */ ICHAINTYPE_U32,           // sets word
    /* 0x5 */ ICHAINTYPE_S32,
    /* 0x6 */ ICHAINTYPE_F32,           // sets float
    /* 0x7 */ ICHAINTYPE_F32_DIV1000,   // sets float divided by 1000
    /* 0x8 */ ICHAINTYPE_VEC3F,         // sets Vec3f members
    /* 0x9 */ ICHAINTYPE_VEC3F_DIV1000, // sets Vec3f members divided by 1000
    /* 0xA */ ICHAINTYPE_VEC3S          // sets Vec3s members
} InitChainType;

/**
 * ICHAIN macros generate an init chain entry of the following form:
 * * (e >> 31) & 0x0001 == Continue Parsing after this entry
 * * (e >> 27) & 0x000F == Type
 * * (e >> 16) & 0x07FF == Offset from start of instance to write initial value
 * *  e        & 0xFFFF == Initial Value
 *
 * Arguments:
 * * type ----- value from enum `InitChainType`
 * * member --- name of member inside `Actor` structure to use as the offset
 * * value ---- s16 value to use
 * * cont ----- ICHAIN_CONTINUE (or ICHAIN_STOP) to continue (or stop) parsing
 */
#define ICHAIN(type, member, value, cont)      \
        { cont, type, OFFSETOF(Actor, member), value }

#define ICHAIN_U8(member, val, cont)            ICHAIN(ICHAINTYPE_U8, member, val, cont)
#define ICHAIN_S8(member, val, cont)            ICHAIN(ICHAINTYPE_S8, member, val, cont)
#define ICHAIN_U16(member, val, cont)           ICHAIN(ICHAINTYPE_U16, member, val, cont)
#define ICHAIN_S16(member, val, cont)           ICHAIN(ICHAINTYPE_S16, member, val, cont)
#define ICHAIN_U32(member, val, cont)           ICHAIN(ICHAINTYPE_U32, member, val, cont)
#define ICHAIN_S32(member, val, cont)           ICHAIN(ICHAINTYPE_S32, member, val, cont)
#define ICHAIN_F32(member, val, cont)           ICHAIN(ICHAINTYPE_F32, member, val, cont)
#define ICHAIN_F32_DIV1000(member, val, cont)   ICHAIN(ICHAINTYPE_F32_DIV1000, member, val, cont)
#define ICHAIN_VEC3F(member, val, cont)         ICHAIN(ICHAINTYPE_VEC3F, member, val, cont)
#define ICHAIN_VEC3F_DIV1000(member, val, cont) ICHAIN(ICHAINTYPE_VEC3F_DIV1000, member, val, cont)
#define ICHAIN_VEC3S(member, val, cont)         ICHAIN(ICHAINTYPE_VEC3S, member, val, cont)

#define ICHAIN_CONTINUE 1
#define ICHAIN_STOP     0

#endif
