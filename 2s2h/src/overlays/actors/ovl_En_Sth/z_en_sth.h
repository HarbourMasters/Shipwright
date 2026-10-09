#ifndef Z_EN_STH_H
#define Z_EN_STH_H

#include "global.h"
#include "overlays/actors/ovl_En_Si/z_en_si.h"
#include "objects/object_sth/object_sth.h"
#include "objects/object_ahg/object_ahg.h" 
#include "objects/object_mask_truth/object_mask_truth.h"

struct EnSth;

typedef void (*EnSthActionFunc)(struct EnSth*, PlayState*);

typedef enum {
    /* 1 */ STH_TYPE_UNUSED_1 = 1,
    /* 2 */ STH_TYPE_SWAMP_SPIDER_HOUSE_CURED, // cursed is EnSsh
    /* 3 */ STH_TYPE_MOON_LOOKING, // South Clock Town, looking up at the moon
    /* 4 */ STH_TYPE_OCEANSIDE_SPIDER_HOUSE_GREET, // looking for shelter
    /* 5 */ STH_TYPE_OCEANSIDE_SPIDER_HOUSE_PANIC // shelter was not enough
    // Other values: Actor will spawn and animate with arm waving, no further interaction.
} EnSthType;

// Note: Vanilla types usually have 0xFEXX typing, but this upper section is unused by the code, reason unknown
#define STH_GET_TYPE(thisx) ((thisx)->params & 0xF)
#define STH_GET_SWAMP_BODY(thisx) ((thisx)->params & 0x100)

// The get item ID for the reward for Oceanside Spider House (wallet, or rupees) is set here
#define STH_GI_ID(thisx) ((thisx)->home.rot.z)

// This actor has its own flags system
#define STH_FLAG_DRAW_MASK_OF_TRUTH             (1 << 0)
#define STH_FLAG_OCEANSIDE_SPIDER_HOUSE_GREET   (1 << 1)
#define STH_FLAG_SWAMP_SPIDER_HOUSE_SAVED       (1 << 2) // set, but not read 
#define STH_FLAG_DISABLE_HEAD_TRACK             (1 << 3)

typedef struct EnSth {
    /* 0x000 */ Actor actor;
    /* 0x144 */ ColliderCylinder collider;
    /* 0x190 */ SkelAnime skelAnime;
    /* 0x1D4 */ Vec3s jointTable[STH_LIMB_MAX];
    /* 0x234 */ Vec3s morphTable[STH_LIMB_MAX];
    /* 0x294 */ Vec3s headRot;
    /* 0x29A */ s16 animIndex;
    /* 0x29C */ u16 sthFlags;
    /* 0x29E */ u8 mainObjectSlot;
    /* 0x29F */ u8 maskOfTruthObjectSlot;
    /* 0x2A0 */ EnSthActionFunc actionFunc;
} EnSth; // size = 0x2A4

#endif // Z_EN_STH_H
