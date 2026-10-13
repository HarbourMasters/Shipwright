#ifndef MACROS_H
#define MACROS_H

#include "PR/ultratypes.h"

#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 240
#define SCREEN_WIDTH_HIRES 640
#define SCREEN_HEIGHT_HIRES 480

#define HIRES_BUFFER_WIDTH  576
#define HIRES_BUFFER_HEIGHT 454

#define PROJECTED_TO_SCREEN_X(projectedPos, invW) ((projectedPos).x * (invW) * (SCREEN_WIDTH / 2) + (SCREEN_WIDTH / 2))
#define PROJECTED_TO_SCREEN_Y(projectedPos, invW) ((projectedPos).y * (invW) * (-SCREEN_HEIGHT / 2) + (SCREEN_HEIGHT / 2))

#define ARRAY_COUNT(arr) (s32)(sizeof(arr) / sizeof(arr[0]))
#define ARRAY_COUNTU(arr) (u32)(sizeof(arr) / sizeof(arr[0]))

#define ARRAY_COUNT_2D(arr) (s32)(sizeof(arr) / sizeof(arr[0][0]))

#define CLOCK_TIME(hr, min) (s32)(((hr) * 60 + (min)) * 0x10000 / (24 * 60))
#define CLOCK_TIME_MINUTE  (CLOCK_TIME(0, 1))
#define CLOCK_TIME_HOUR (CLOCK_TIME(1, 0))
#define DAY_LENGTH (CLOCK_TIME(24, 0))

#define TIME_TO_HOURS_F(time) ((time) * (24.0f / 0x10000))
#define TIME_TO_HOURS_F_ALT(time) (TIME_TO_MINUTES_F(time) / 60.0f)
#define TIME_TO_MINUTES_F(time) ((time) * ((24.0f * 60.0f) / 0x10000)) // 0.021972656f
#define TIME_TO_MINUTES_ALT_F(time) ((time) / (0x10000 / (24.0f * 60.0f)))
#define TIME_TO_SECONDS_F(time) ((time) * ((24.0f * 60.0f * 60.0f) / 0x10000))

#define CLOCK_TIME_F(hr, min) (((hr) * 60.0f + (min)) * (0x10000 / (24.0f * 60.0f)))
#define CLOCK_TIME_ALT_F(hr, min) (((hr) * 60.0f + (min)) / (24.0f * 60.0f / 0x10000))
#define CLOCK_TIME_ALT2_F(hr, min) ((((hr) + (min) / 60.0f) * 60.0f) / (24.0f * 60.0f / 0x10000))
#define CLOCK_TIME_HOUR_F (CLOCK_TIME_F(1, 0))

#define CAPACITY(upg, value) gUpgradeCapacities[upg][value]
#define CUR_CAPACITY(upg) CAPACITY(upg, CUR_UPG_VALUE(upg))

// To be used with `Magic_Add`, but ensures enough magic is added to fill the magic bar to capacity
#define MAGIC_FILL_TO_CAPACITY (((void)0, gSaveContext.magicFillTarget) + (gSaveContext.save.saveInfo.playerData.isDoubleMagicAcquired + 1) * MAGIC_NORMAL_METER)

#define CHECK_FLAG_ALL(flags, mask) (((flags) & (mask)) == (mask))

#define BIT_FLAG_TO_SHIFT(flag) \
    ((flag & 0x80) ? 7 : \
    (flag & 0x40) ? 6 : \
    (flag & 0x20) ? 5 : \
    (flag & 0x10) ? 4 : \
    (flag & 0x8) ? 3 : \
    (flag & 0x4) ? 2 : \
    (flag & 0x2) ? 1 : \
    (flag & 0x1) ? 0 : \
    0)

#define DECR(x) ((x) == 0 ? 0 : --(x))

//! checks min first
#define CLAMP(x, min, max) ((x) < (min) ? (min) : (x) > (max) ? (max) : (x))
//! checks max first
#define CLAMP_ALT(x, min, max) ((x) > (max) ? (max) : (x) < (min) ? (min) : (x))
#define CLAMP_MAX(x, max) ((x) > (max) ? (max) : (x))
#define CLAMP_MIN(x, min) ((x) < (min) ? (min) : (x))

#define SWAP(type, a, b)    \
    {                       \
        type _temp = (a);   \
        (a) = (b);          \
        (b) = _temp;        \
    }                       \
    (void)0

// #region 2S2H [Port]
#define D_01000000_TO_SEGMENTED (0x01000000 | 1)
// Compute 0x0E segment value as compile time constant for 32bit and 64bit
#define D_0E000000_TO_SEGMENTED(member) ((0x0E000000 + offsetof(GfxMasterList, member)) | 1)
#define D_0F000000_TO_SEGMENTED (0x0F000000 | 1)
// Used to compare animations. The statically linked global animations can end up with different pointer addresses if an animation gets set from one file and then compared against the static animation in another
#define BEN_ANIM_EQUAL(anim1, anim2) ((anim1 == NULL) ? ((anim2 == NULL) ? true : false) : ((anim2 == NULL) ? false : strcmp(anim1, anim2) == 0))
// #endregion

#endif // MACROS_H
