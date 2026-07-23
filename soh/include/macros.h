#ifndef MACROS_H
#define MACROS_H

#include <ship/utils/binarytools/endianness.h>

// Upstream TODO: Document reasoning for change
// #ifndef __GNUC__
// #define __attribute__(x)
// #endif

// this was removed from the LUS rcp.h in https://github.com/Kenix3/libultraship/pull/833/
// it is still used in graph.c and fault.c
#define HW_REG(reg, type) *(volatile type*)((reg) | 0xA0000000)

// SoH [Port] Always use the AVOID_UB version (we don't set AVOID_UB while building yet)
/*
#ifndef AVOID_UB
#define BAD_RETURN(type) type
#else
#define BAD_RETURN(type) void
#endif
*/
#define BAD_RETURN(type) void

// Upstream TODO: Document reasoning for change
// #define UNUSED __attribute__((unused))
// #define FALLTHROUGH __attribute__((fallthrough))

#define ARRAY_COUNT(arr) (s32)(sizeof(arr) / sizeof(arr[0]))
#define ARRAY_COUNTU(arr) (u32)(sizeof(arr) / sizeof(arr[0]))
#define ARRAY_COUNT_2D(arr) (s32)(sizeof(arr) / sizeof(arr[0][0]))

#define PHYSICAL_TO_VIRTUAL(addr) (void*)((uintptr_t)(addr) + 0x80000000)
#define VIRTUAL_TO_PHYSICAL(addr) (uintptr_t)((u8*)(addr) - 0x80000000)
// Upstream TODO: Document reasoning for change
//#define SEGMENTED_TO_VIRTUAL(addr) PHYSICAL_TO_VIRTUAL(gSegments[SEGMENT_NUMBER(addr)] + SEGMENT_OFFSET(addr))
#define SEGMENTED_TO_VIRTUAL(addr) addr

#define RGBA8(r, g, b, a) ((((r) & 0xFF) << 24) | (((g) & 0xFF) << 16) | (((b) & 0xFF) << 8) | (((a) & 0xFF) << 0))

#define GET_EVENTCHKINF(flag) (gSaveContext.eventChkInf[(flag) >> 4] & (1 << ((flag) & 0xF)))
#define SET_EVENTCHKINF(flag) (gSaveContext.eventChkInf[(flag) >> 4] |= (1 << ((flag) & 0xF)))
#define CLEAR_EVENTCHKINF(flag) (gSaveContext.eventChkInf[(flag) >> 4] &= ~(1 << ((flag) & 0xF)))

#define GET_ITEMGETINF(flag) (gSaveContext.itemGetInf[(flag) >> 4] & (1 << ((flag) & 0xF)))
#define SET_ITEMGETINF(flag) (gSaveContext.itemGetInf[(flag) >> 4] |= (1 << ((flag) & 0xF)))
#define CLEAR_ITEMGETINF(flag) (gSaveContext.itemGetInf[(flag) >> 4] &= ~(1 << ((flag) & 0xF)))

#define GET_INFTABLE(flag) (gSaveContext.infTable[(flag) >> 4] & (1 << ((flag) & 0xF)))
#define SET_INFTABLE(flag) (gSaveContext.infTable[(flag) >> 4] |= (1 << ((flag) & 0xF)))
#define CLEAR_INFTABLE(flag) (gSaveContext.infTable[(flag) >> 4] &= ~(1 << ((flag) & 0xF)))

#define GET_EVENTINF(flag) (gSaveContext.eventInf[(flag) >> 4] & (1 << ((flag) & 0xF)))
#define SET_EVENTINF(flag) (gSaveContext.eventInf[(flag) >> 4] |= (1 << ((flag) & 0xF)))
#define CLEAR_EVENTINF(flag) (gSaveContext.eventInf[(flag) >> 4] &= ~(1 << ((flag) & 0xF)))

#define CHECK_BTN_ALL(state, combo) (~((state) | ~(combo)) == 0)
#define CHECK_BTN_ANY(state, combo) (((state) & (combo)) != 0)

#define CHECK_FLAG_ALL(flags, mask) (((flags) & (mask)) == (mask))

// #region SOH [General]
// Logging changes
#ifndef NDEBUG
#define LOG(exp, value, format)         \
    do {                                            \
        LogUtils_LogThreadId(__FILE__, __FILE__);           \
        osSyncPrintf(exp " = " format "\n", value); \
    } while (0)
#else
#define LOG(exp, value, format) ((void)0)
#endif

#ifndef NDEBUG
#define LOG_STRING(string) LOG(#string, string, "%s")
#define LOG_ADDRESS(exp, value) LOG(exp, value, "%p")
#define LOG_TIME(exp, value) LOG(exp, value, "%lld")
#define LOG_NUM(exp, value) LOG(exp, value, "%d")
#define LOG_HEX(exp, value) LOG(exp, value, "%x")
#define LOG_FLOAT(exp, value) LOG(exp, value, "%f")
#else
#define LOG_STRING(string) ((void)0)
#define LOG_ADDRESS(exp, value) ((void)0)
#define LOG_TIME(exp, value) ((void)0)
#define LOG_NUM(exp, value) ((void)0)
#define LOG_HEX(exp, value) ((void)0)
#define LOG_FLOAT(exp, value) ((void)0)
#endif

// LogUtils as macro
#ifndef NDEBUG
#define LOG_POINTER(val, max, ptr, name) LogUtils_LogPointer(val, max, ptr, name, __FILE__, __LINE__)
#define LOG_CHECK_BOUNDARY(name, value, unk) LogUtils_CheckBoundary(name, value, unk, __FILE__, __LINE__)
#define LOG_CHECK_NULL_POINTER(exp, ptr) LogUtils_CheckNullPointer(exp, ptr,__FILE__, __LINE__)
#define LOG_CHECK_VALID_POINTER(exp, ptr) LogUtils_CheckValidPointer(exp, ptr,__FILE__, __LINE__)
#define LOG_THREAD_ID() LogUtils_LogThreadId(__FILE__, __LINE__)
#define LOG_HUNGUP_THREAD() LogUtils_HungupThread(__FILE__, __LINE__)
#else
#define LOG_POINTER(val, max, ptr, name) ((void)0)
#define LOG_CHECKBOUNDARY(name, value, unk) ((void)0)
#define LOG_CHECK_NULL_POINTER(exp, ptr) ((void)0)
#define LOG_CHECK_VALID_POINTER(exp, ptr) ((void)0)
#define LOG_THREAD_ID() ((void)0)
#define LOG_HUNGUP_THREAD() ((void)0)
#endif
// #endregion


#define SET_NEXT_GAMESTATE(curState, newInit, newStruct) \
    do {                                                 \
        (curState)->init = newInit;                      \
        (curState)->size = sizeof(newStruct);            \
    } while (0)

#define SET_FULLSCREEN_VIEWPORT(view)      \
    {                                      \
        Viewport viewport;                 \
        viewport.bottomY = SCREEN_HEIGHT;  \
        viewport.rightX = SCREEN_WIDTH;    \
        viewport.topY = 0;                 \
        viewport.leftX = 0;                \
        View_SetViewport(view, &viewport); \
    }                                      \
    (void)0

// #region SOH [General]
#define OFFSETOF(structure, member) ((size_t)&(((structure*)0)->member))

#define MEDIAN3(a1, a2, a3)                                                    \
    (((a2) >= (a1)) ? (((a3) >= (a2)) ? (a2) : (((a1) >= (a3)) ? (a1) : (a3))) \
                    : (((a2) >= (a3)) ? (a2) : (((a3) >= (a1)) ? (a1) : (a3))))

#define MATRIX_TOMTX(dest) Matrix_ToMtx(dest, __FILE__, __LINE__)
#ifdef __cplusplus
#define MATRIX_NEWMTX(gfxCtx) Matrix_NewMtx(gfxCtx, const_cast<char*>(__FILE__), __LINE__)
#else
#define MATRIX_NEWMTX(gfxCtx) Matrix_NewMtx(gfxCtx, __FILE__, __LINE__)
#endif

#define MATRIX_CHECKFLOATS(mf) Matrix_CheckFloats(mf, __FILE__, __LINE__)

#define ZELDA_ARENA_MALLOC_DEBUG(size) ZeldaArena_MallocDebug(size, __FILE__, __LINE__)
#define ZELDA_ARENA_MALLOC_RDEBUG(size) ZeldaArena_MallocRDebug(size, __FILE__, __LINE__)
#define ZELDA_ARENA_REALLOC_DEBUG(ptr, newSize) ZeldaArena_ReallocDebug(ptr, newSize, __FILE__, __LINE__)
#define ZELDA_ARENA_FREE_DEBUG(ptr) ZeldaArena_FreeDebug(ptr, __FILE__, __LINE__)

#define SYSTEM_ARENA_MALLOC_DEBUG(size) SystemArena_MallocDebug(size, __FILE__, __LINE__)
#define SYSTEM_ARENA_MALLOC_RDEBUG(size) SystemArena_MallocRDebug(size, __FILE__, __LINE__)
#define SYSTEM_ARENA_REALLOC_DEBUG(ptr, newSize) SystemArena_ReallocDebug(ptr, newSize, __FILE__, __LINE__)
#define SYSTEM_ARENA_FREE_DEBUG(ptr) SystemArena_FreeDebug(ptr, __FILE__, __LINE__)

#define DEBUG_ARENA_MALLOC_DEBUG(size) DebugArena_MallocDebug(size, __FILE__, __LINE__)
#define DEBUG_ARENA_MALLOC_RDEBUG(size) DebugArena_MallocRDebug(size, __FILE__, __LINE__)
#define DEBUG_ARENA_REALLOC_DEBUG(ptr, newSize) DebugArena_ReallocDebug(ptr, newSize, __FILE__, __LINE__)
#define DEBUG_ARENA_FREE_DEBUG(ptr) DebugArena_FreeDebug(ptr, __FILE__, __LINE__)

#define GAMESTATE_ALLOC_MC(gameState, size) GameState_Alloc(gameState, size, __FILE__, __LINE__)
#define GAMESTATE_MALLOC_DEBUG(gameState, size) GameAlloc_MallocDebug(gameState, size, __FILE__, __LINE__)

#define BGCHECK_POS_ERROR_CHECK(vec3f) BgCheck_PosErrorCheck(vec3f, __FILE__, __LINE__)

#define SEG_ADDR(seg, addr) (addr | (seg << 24) | 1)

// Upstream TODO: Bring back decomp file/line macro use in src (but ignore the args for our needs)
#define SYSTEM_ARENA_MALLOC(size, file, line) SystemArena_MallocDebug(size, __FILE__, __LINE__)
#define SYSTEM_ARENA_FREE(ptr, file, line) SystemArena_FreeDebug(ptr, __FILE__, __LINE__)
// #endregion

#define DPAD_ITEM(button) ((gSaveContext.buttonStatus[(button) + 5] != BTN_DISABLED) \
                                ? gSaveContext.equips.buttonItems[(button) + 4]       \
                                : ITEM_NONE)
// #endregion

// #region SOH [Randomizer]
#define NUM_TRIALS 6
#define NUM_SHOP_ITEMS 64
#define NUM_SCRUBS 46

#ifdef __cplusplus
#define DUNGEON_ITEMS_CAN_BE_OUTSIDE_DUNGEON(rsk) \
    (OTRGlobals::Instance->gRandoContext->GetOption(rsk).IsNot(RO_DUNGEON_ITEM_LOC_STARTWITH) && \
    OTRGlobals::Instance->gRandoContext->GetOption(rsk).IsNot(RO_DUNGEON_ITEM_LOC_VANILLA) && \
    OTRGlobals::Instance->gRandoContext->GetOption(rsk).IsNot(RO_DUNGEON_ITEM_LOC_OWN_DUNGEON))
#endif
// #endregion

#endif
