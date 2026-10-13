#ifndef Z_BG_KIN2_PICTURE_H
#define Z_BG_KIN2_PICTURE_H

#include "global.h"

#define BG_KIN2_PICTURE_SKULLTULA_COLLECTED(thisx) (((thisx)->params >> 5) & 1)
// #region 2S2H [Port] The original calculation is ((u8)(((thisx & 0x3FC)) >> 2)), but this results in values well
// over 100. This value is ultimately used to left shift 1 to get the treasure flag. Bit shifts greater than the bit
// width are undefined behavior. On N64, this likely truncates to a value below 32. The correction below matches other
// uses of the Skulltula Token chest flag (see ENSI_GET_CHEST_FLAG and inline uses in z_tg_sw.c):
#define BG_KIN2_PICTURE_GET_3FC(thisx) ((u8)(((thisx & 0xFC)) >> 2))
// #endregion
#define BG_KIN2_PICTURE_SKULLTULA_SPAWN_PARAM(thisx) ((((thisx)->params & 0x1F) << 2) | 0xFF03)

struct BgKin2Picture;

typedef void (*BgKin2PictureActionFunc)(struct BgKin2Picture*, PlayState*);

typedef struct BgKin2Picture {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ ColliderTris colliderTris;
    /* 0x17C */ ColliderTrisElement colliderElement[2];
    /* 0x234 */ BgKin2PictureActionFunc actionFunc;
    /* 0x238 */ s16 step;
    /* 0x23A */ s8 paintingTimer; // Used for when painting is shaking and for timing Gold Skulltula spawn.
    /* 0x23B */ s8 landTimer;
    /* 0x23C */ s16 xOffsetAngle;
    /* 0x23E */ s16 yOffsetAngle;
    /* 0x240 */ s8 cutsceneStarted;
    /* 0x241 */ s8 hasSpawnedDust;
    /* 0x242 */ s8 skulltulaNoiseTimer;
} BgKin2Picture; // size = 0x244

#endif // Z_BG_KIN2_PICTURE_H
