#ifndef MACROS_H
#define MACROS_H

#include <endianness.h>

// Upstream TODO: Document reasoning for change
// #ifndef __GNUC__
// #define __attribute__(x)
// #endif

// #ifndef AVOID_UB
// #define BAD_RETURN(type) type
// #else
// #define BAD_RETURN(type) void
// #endif

// #define UNUSED __attribute__((unused))
// #define FALLTHROUGH __attribute__((fallthrough))

#define ARRAY_COUNT(arr) (s32)(sizeof(arr) / sizeof(arr[0]))
#define ARRAY_COUNTU(arr) (u32)(sizeof(arr) / sizeof(arr[0]))

#define PHYSICAL_TO_VIRTUAL(addr) (void*)((uintptr_t)(addr) + 0x80000000)
#define VIRTUAL_TO_PHYSICAL(addr) (uintptr_t)((u8*)(addr) - 0x80000000)
// Upstream TODO: Document reasoning for change
//#define SEGMENTED_TO_VIRTUAL(addr) PHYSICAL_TO_VIRTUAL(gSegments[SEGMENT_NUMBER(addr)] + SEGMENT_OFFSET(addr))
#define SEGMENTED_TO_VIRTUAL(addr) addr

#define SQ(x) ((x)*(x))
#define ABS(x) ((x) >= 0 ? (x) : -(x))
#define DECR(x) ((x) == 0 ? 0 : --(x))
#define CLAMP(x, min, max) ((x) < (min) ? (min) : (x) > (max) ? (max) : (x))
#define CLAMP_MAX(x, max) ((x) > (max) ? (max) : (x))
#define CLAMP_MIN(x, min) ((x) < (min) ? (min) : (x))

#define RGBA8(r, g, b, a) ((((r) & 0xFF) << 24) | (((g) & 0xFF) << 16) | (((b) & 0xFF) << 8) | (((a) & 0xFF) << 0))

#define GET_PLAYER(play) ((Player*)(play)->actorCtx.actorLists[ACTORCAT_PLAYER].head)

#define GET_ACTIVE_CAM(play) ((play)->cameraPtrs[(play)->activeCamera]) // Upstream TODO: Camera

#define LINK_IS_ADULT (gSaveContext.linkAge == LINK_AGE_ADULT)
#define LINK_IS_CHILD (gSaveContext.linkAge == LINK_AGE_CHILD)

#define YEARS_CHILD 5
#define YEARS_ADULT 17
#define LINK_AGE_IN_YEARS (!LINK_IS_ADULT ? YEARS_CHILD : YEARS_ADULT)

#define CLOCK_TIME(hr, min) ((s32)(((hr) * 60 + (min)) * (f32)0x10000 / (24 * 60) + 0.5f))

#define IS_DAY (gSaveContext.nightFlag == 0)
#define IS_NIGHT (gSaveContext.nightFlag == 1)

#define SLOT(item) gItemSlots[item]
#define INV_CONTENT(item) gSaveContext.inventory.items[SLOT(item)]
#define AMMO(item) gSaveContext.inventory.ammo[SLOT(item)]
#define BEANS_BOUGHT AMMO(ITEM_BEAN + 1)

#define ALL_EQUIP_VALUE(equip) ((s32)(gSaveContext.inventory.equipment & gEquipMasks[equip]) >> gEquipShifts[equip])
#define CUR_EQUIP_VALUE(equip) ((s32)(gSaveContext.equips.equipment & gEquipMasks[equip]) >> gEquipShifts[equip])
#define OWNED_EQUIP_FLAG(equip, value) (gBitFlags[value] << gEquipShifts[equip])
#define OWNED_EQUIP_FLAG_ALT(equip, value) ((1 << (value)) << gEquipShifts[equip])
#define CHECK_OWNED_EQUIP(equip, value) (OWNED_EQUIP_FLAG(equip, value) & gSaveContext.inventory.equipment)
#define CHECK_OWNED_EQUIP_ALT(equip, value) (gBitFlags[(value) + (equip) * 4] & gSaveContext.inventory.equipment)

#define SWORD_EQUIP_TO_PLAYER(swordEquip) (swordEquip)
#define SHIELD_EQUIP_TO_PLAYER(shieldEquip) (shieldEquip)
#define TUNIC_EQUIP_TO_PLAYER(tunicEquip) ((tunicEquip) - 1)
#define BOOTS_EQUIP_TO_PLAYER(bootsEquip) ((bootsEquip) - 1)

#define CUR_UPG_VALUE(upg) ((s32)(gSaveContext.inventory.upgrades & gUpgradeMasks[upg]) >> gUpgradeShifts[upg])
#define CAPACITY(upg, value) gUpgradeCapacities[upg][value]
#define CUR_CAPACITY(upg) CAPACITY(upg, CUR_UPG_VALUE(upg))

#define CHECK_QUEST_ITEM(item) (gBitFlags[item] & gSaveContext.inventory.questItems)
#define CHECK_DUNGEON_ITEM(item, dungeonIndex) (gSaveContext.inventory.dungeonItems[dungeonIndex] & gBitFlags[item])

#define GET_GS_FLAGS(index) \
    ((gSaveContext.gsFlags[(index) >> 2] & gGsFlagsMasks[(index) & 3]) >> gGsFlagsShifts[(index) & 3])
#define SET_GS_FLAGS(index, value) \
    (gSaveContext.gsFlags[(index) >> 2] |= (value) << gGsFlagsShifts[(index) & 3])

#define HIGH_SCORE(score) (gSaveContext.highScores[score])

#define GET_EVENTCHKINF(flag) (gSaveContext.eventChkInf[(flag) >> 4] & (1 << ((flag) & 0xF)))
#define SET_EVENTCHKINF(flag) (gSaveContext.eventChkInf[(flag) >> 4] |= (1 << ((flag) & 0xF)))
#define CLEAR_EVENTCHKINF(flag) (gSaveContext.eventChkInf[(flag) >> 4] &= ~(1 << ((flag) & 0xF)))

#define GET_ITEMGETINF(flag) (gSaveContext.itemGetInf[(flag) >> 4] & (1 << ((flag) & 0xF)))
#define SET_ITEMGETINF(flag) (gSaveContext.itemGetInf[(flag) >> 4] |= (1 << ((flag) & 0xF)))

#define GET_INFTABLE(flag) (gSaveContext.infTable[(flag) >> 4] & (1 << ((flag) & 0xF)))
#define SET_INFTABLE(flag) (gSaveContext.infTable[(flag) >> 4] |= (1 << ((flag) & 0xF)))
#define CLEAR_INFTABLE(flag) (gSaveContext.infTable[(flag) >> 4] &= ~(1 << ((flag) & 0xF)))

#define GET_EVENTINF(flag) (gSaveContext.eventInf[(flag) >> 4] & (1 << ((flag) & 0xF)))
#define SET_EVENTINF(flag) (gSaveContext.eventInf[(flag) >> 4] |= (1 << ((flag) & 0xF)))
#define CLEAR_EVENTINF(flag) (gSaveContext.eventInf[(flag) >> 4] &= ~(1 << ((flag) & 0xF)))

#define B_BTN_ITEM ((gSaveContext.buttonStatus[0] == ITEM_NONE)                    \
                        ? ITEM_NONE                                                \
                        : (gSaveContext.equips.buttonItems[0] == ITEM_SWORD_KNIFE) \
                            ? ITEM_SWORD_BGS                                       \
                            : gSaveContext.equips.buttonItems[0])

#define C_BTN_ITEM(button) ((gSaveContext.buttonStatus[(button) + 1] != BTN_DISABLED) \
                                ? gSaveContext.equips.buttonItems[(button) + 1]       \
                                : ITEM_NONE)

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

extern GraphicsContext* __gfxCtx;

#define WORK_DISP          __gfxCtx->work.p
#define POLY_OPA_DISP      __gfxCtx->polyOpa.p
#define POLY_XLU_DISP      __gfxCtx->polyXlu.p
// #region SOH [General]
// Upstream TODO: Document reasoning for these only existing in SoH
#define WORLD_OVERLAY_DISP __gfxCtx->worldOverlay.p
#define POLY_KAL_DISP      __gfxCtx->polyKal.p
// #endregion
#define OVERLAY_DISP       __gfxCtx->overlay.p

// __gfxCtx shouldn't be used directly.
// Use the DISP macros defined above when writing to display buffers.
// #region SOH [General]
// Augmented to provide debug information in debug build and support interpolation
#ifndef NDEBUG
#define OPEN_DISPS(gfxCtx) \
    { \
        void FrameInterpolation_RecordOpenChild(const void* a, int b); \
        FrameInterpolation_RecordOpenChild(__FILE__, __LINE__); \
        GraphicsContext* __gfxCtx; \
        Gfx* dispRefs[4]; \
        __gfxCtx = gfxCtx; \
        (void)__gfxCtx; \
        Graph_OpenDisps(dispRefs, gfxCtx, __FILE__, __LINE__)
#else
#define OPEN_DISPS(gfxCtx) \
    { \
        void FrameInterpolation_RecordOpenChild(const void* a, int b); \
        FrameInterpolation_RecordOpenChild(__FILE__, __LINE__); \
        GraphicsContext* __gfxCtx; \
        __gfxCtx = gfxCtx; \
        (void)__gfxCtx;
#endif

#ifndef NDEBUG
#define CLOSE_DISPS(gfxCtx) \
    {void FrameInterpolation_RecordCloseChild(void); \
    FrameInterpolation_RecordCloseChild();} \
    Graph_CloseDisps(dispRefs, gfxCtx, __FILE__, __LINE__); \
    } \
    (void)0
#else
#define CLOSE_DISPS(gfxCtx) \
    {void FrameInterpolation_RecordCloseChild(void); \
    FrameInterpolation_RecordCloseChild();} \
    (void)0; \
    } \
    (void)0
#endif
// #endregion

/**
 * `x` vertex x
 * `y` vertex y
 * `z` vertex z
 * `s` texture s coordinate
 * `t` texture t coordinate
 * `crnx` red component of color vertex, or x component of normal vertex
 * `cgny` green component of color vertex, or y component of normal vertex
 * `cbnz` blue component of color vertex, or z component of normal vertex
 * `a` alpha
 */
#define VTX(x,y,z,s,t,crnx,cgny,cbnz,a) { { { x, y, z }, 0, { s, t }, { crnx, cgny, cbnz, a } } }

#define VTX_T(x,y,z,s,t,cr,cg,cb,a) { { x, y, z }, 0, { s, t }, { cr, cg, cb, a } }

// #region SOH [WiiU]
#ifdef __WIIU__
#define ASSERT(expression) (void)((!!(expression)) || (_assert(#expression, __FILE__, (unsigned)(__LINE__)), 0))
#else
#define ASSERT(expression) (void)((!!(expression)) || (__assert(#expression, __FILE__, (unsigned)(__LINE__)), 0))
#endif
// #endregion

#define gDPSetTileCustom(pkt, fmt, siz, width, height, pal, cms, cmt, masks, maskt, shifts, shiftt)                    \
    do {                                                                                                               \
        gDPPipeSync(pkt);                                                                                              \
        gDPTileSync(pkt);                                                                                              \
        gDPSetTile(pkt, fmt, siz, (((width)*siz##_TILE_BYTES) + 7) >> 3, 0, G_TX_LOADTILE, 0, cmt, maskt, shiftt, cms, \
                   masks, shifts);                                                                                     \
        gDPTileSync(pkt);                                                                                              \
        gDPSetTile(pkt, fmt, siz, (((width)*siz##_TILE_BYTES) + 7) >> 3, 0, G_TX_RENDERTILE, pal, cmt, maskt, shiftt,  \
                   cms, masks, shifts);                                                                                \
        gDPSetTileSize(pkt, G_TX_RENDERTILE, 0, 0, ((width)-1) << G_TEXTURE_IMAGE_FRAC,                                \
                       ((height)-1) << G_TEXTURE_IMAGE_FRAC);                                                          \
    } while (0)

// #region SOH [General]
#define OFFSETOF(structure, member) ((size_t)&(((structure*)0)->member))

#define MEDIAN3(a1, a2, a3)                                                    \
    (((a2) >= (a1)) ? (((a3) >= (a2)) ? (a2) : (((a1) >= (a3)) ? (a1) : (a3))) \
                    : (((a2) >= (a3)) ? (a2) : (((a3) >= (a1)) ? (a1) : (a3))))

#define MATRIX_TOMTX(dest) Matrix_ToMtx(dest, __FILE__, __LINE__)
#define MATRIX_NEWMTX(gfxCtx) Matrix_NewMtx(gfxCtx, __FILE__, __LINE__)
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
// #endregion

// #region SOH [Enhancements]
#define CHECK_EQUIPMENT_AGE(i, j) (CVar_GetS32("gTimelessEquipment", 0) || (gEquipAgeReqs[i][j] == 9) || (gEquipAgeReqs[i][j] == ((void)0, gSaveContext.linkAge)))
#define CHECK_SLOT_AGE(slotIndex) (CVar_GetS32("gTimelessEquipment", 0) || (gSlotAgeReqs[slotIndex] == 9) || gSlotAgeReqs[slotIndex] == ((void)0, gSaveContext.linkAge))
#define CHECK_ITEM_AGE(itemIndex) (CVar_GetS32("gTimelessEquipment", 0) || (gItemAgeReqs[itemIndex] == 9) || (gItemAgeReqs[itemIndex] == gSaveContext.linkAge))

#define DPAD_ITEM(button) ((gSaveContext.buttonStatus[(button) + 5] != BTN_DISABLED) \
                                ? gSaveContext.equips.buttonItems[(button) + 4]       \
                                : ITEM_NONE)
// #endregion

// #region SOH [Randomizer]
#define NUM_TRIALS 6
#define NUM_SHOP_ITEMS 64
#define NUM_SCRUBS 46
#define FOREST_TEMPLE_SMALL_KEY_MAX (ResourceMgr_IsSceneMasterQuest(SCENE_BMORI1) ? 6 : 5)
#define FIRE_TEMPLE_SMALL_KEY_MAX (ResourceMgr_IsSceneMasterQuest(SCENE_HIDAN) ? 5 : 8)
#define WATER_TEMPLE_SMALL_KEY_MAX (ResourceMgr_IsSceneMasterQuest(SCENE_MIZUSIN) ? 2 : 6)
#define SPIRIT_TEMPLE_SMALL_KEY_MAX (ResourceMgr_IsSceneMasterQuest(SCENE_JYASINZOU) ? 7 : 5)
#define SHADOW_TEMPLE_SMALL_KEY_MAX (ResourceMgr_IsSceneMasterQuest(SCENE_HAKADAN) ? 6 : 5)
#define BOTTOM_OF_THE_WELL_SMALL_KEY_MAX (ResourceMgr_IsSceneMasterQuest(SCENE_HAKADANCH) ? 2 : 3)
#define GERUDO_TRAINING_GROUNDS_SMALL_KEY_MAX (ResourceMgr_IsSceneMasterQuest(SCENE_MEN) ? 3 : 9)
#define GERUDO_FORTRESS_SMALL_KEY_MAX 4
#define GANONS_CASTLE_SMALL_KEY_MAX (ResourceMgr_IsSceneMasterQuest(SCENE_GANONTIKA) ? 3 : 2)
#define TREASURE_GAME_SMALL_KEY_MAX 6
// #endregion

#endif
