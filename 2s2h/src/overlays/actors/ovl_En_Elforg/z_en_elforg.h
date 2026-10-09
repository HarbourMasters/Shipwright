#ifndef Z_EN_ELFORG_H
#define Z_EN_ELFORG_H

#include "global.h"
#include "overlays/actors/ovl_Bg_Dy_Yoseizo/z_bg_dy_yoseizo.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define STRAY_FAIRY_TYPE(thisx) ((thisx)->params & 0xF)
#define STRAY_FAIRY_GET_NON_DUNGEON_AREA(thisx) (((thisx)->params & 0x1C0) >> 6)
#define STRAY_FAIRY_GET_FLAG(thisx) (((thisx)->params & 0xFE00) >> 9)
#define STRAY_FAIRY_SPARKLE_COUNT(thisx) ((thisx)->home.rot.x)

//! @note `nonDungeonArea` does not always use the enum
#define STRAY_FAIRY_PARAMS(flag, nonDungeonArea, type) ((((flag) & 0x7F) << 9) | (((nonDungeonArea) & 7) << 6) | ((type) & 0xF))

#define STRAY_FAIRY_FLAG_MOVES_QUICKLY_TO_HOME (1 << 0)
#define STRAY_FAIRY_FLAG_SPARKLES_AND_SHRINKS (1 << 1)
#define STRAY_FAIRY_FLAG_CIRCLES_QUICKLY_IN_FOUNTAIN (1 << 2)
#define STRAY_FAIRY_FLAG_GREAT_FAIRYS_MASK_EQUIPPED (1 << 3)

typedef enum StrayFairyType {
    /* 0 */ STRAY_FAIRY_TYPE_FREE_FLOATING,        // The ones just floating around
    /* 1 */ STRAY_FAIRY_TYPE_FAIRY_FOUNTAIN,       // The ones already present when you enter a Fairy Fountain
    /* 2 */ STRAY_FAIRY_TYPE_BUBBLE,               // The ones trapped in bubbles
    /* 3 */ STRAY_FAIRY_TYPE_CLOCK_TOWN,           // The free-floating Stray Fairies in Clock Town
    /* 4 */ STRAY_FAIRY_TYPE_ENEMY,                // The ones trapped inside enemies
    /* 5 */ STRAY_FAIRY_TYPE_COLLIDER,             // Unused in retail. The fairy is hidden until the collider is hit
    /* 6 */ STRAY_FAIRY_TYPE_CHEST,                // The ones in treasure chests
    /* 7 */ STRAY_FAIRY_TYPE_COLLECTIBLE,          // The ones in boxes, pots, beehives, etc.
    /* 8 */ STRAY_FAIRY_TYPE_RETURNING_TO_FOUNTAIN // The ones you "turn in" by walking into a Fairy Fountain
} StrayFairyType;

// Corresponds to the Great Fairy types
typedef enum StrayFairyArea {
    /* 0 */ STRAY_FAIRY_AREA_CLOCK_TOWN = GREAT_FAIRY_TYPE_MAGIC,
    /* 1 */ STRAY_FAIRY_AREA_WOODFALL = GREAT_FAIRY_TYPE_POWER,
    /* 2 */ STRAY_FAIRY_AREA_SNOWHEAD = GREAT_FAIRY_TYPE_WISDOM,
    /* 3 */ STRAY_FAIRY_AREA_GREAT_BAY = GREAT_FAIRY_TYPE_COURAGE,
    /* 4 */ STRAY_FAIRY_AREA_STONE_TOWER = GREAT_FAIRY_TYPE_KINDNESS,
    /* 5 */ STRAY_FAIRY_AREA_MAX
} StrayFairyArea;

struct EnElforg;

typedef void (*EnElforgActionFunc)(struct EnElforg*, PlayState*);

typedef struct EnElforg {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ Vec3s jointTable[STRAY_FAIRY_LIMB_MAX];
    /* 0x1C4 */ ColliderCylinder collider;
    /* 0x210 */ Actor* enemy;
    /* 0x214 */ u16 strayFairyFlags;
    /* 0x216 */ s16 direction; // negative when facing right, positive when facing left
    /* 0x218 */ s16 area;
    /* 0x21C */ s32 timer;
    /* 0x220 */ s32 secondaryTimer;
    /* 0x224 */ f32 targetSpeedXZ;
    /* 0x228 */ f32 targetDistanceFromHome;
    /* 0x22C */ EnElforgActionFunc actionFunc;
} EnElforg; // size = 0x230

#endif // Z_EN_ELFORG_H
