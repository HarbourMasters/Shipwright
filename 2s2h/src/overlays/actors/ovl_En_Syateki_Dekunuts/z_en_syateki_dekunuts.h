#ifndef Z_EN_SYATEKI_DEKUNUTS_H
#define Z_EN_SYATEKI_DEKUNUTS_H

#include "global.h"
#include "objects/object_dekunuts/object_dekunuts.h"

#define SG_DEKU_GET_TYPE(thisx) ((thisx)->params & 0xF)
#define SG_DEKU_GET_INDEX(thisx) (((thisx)->params & 0xF0) >> 4)
#define SG_DEKU_GET_ADDITIONAL_PATH_INDEX_LIMIT(thisx) (((thisx)->params & 0xFF00) >> 8)
#define SG_DEKU_PARAMS(additionalPathIndexLimit, index, type) (((additionalPathIndexLimit << 8) & 0xFF00) | ((index << 4) & 0xF0) | (type & 0xF))

typedef enum {
    /* 0 */ SG_DEKU_TYPE_NORMAL, // Worth 30 points
    /* 1 */ SG_DEKU_TYPE_BONUS   // Worth 100 points
} ShootingGalleryDekuScrubType;

struct EnSyatekiDekunuts;

typedef void (*EnSyatekiDekunutsActionFunc)(struct EnSyatekiDekunuts*, PlayState*);

typedef struct EnSyatekiDekunuts {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ EnSyatekiDekunutsActionFunc actionFunc;
    /* 0x18C */ ColliderCylinder collider;
    /* 0x1D8 */ s16 timer;
    /* 0x1DA */ s16 waitTimer;
    /* 0x1CD */ s16 unk_1DC; // Initialized, but never used
    /* 0x1DE */ UNK_TYPE1 unk_1DE[0x4];
    /* 0x1E2 */ s16 isAlive;
    /* 0x1E4 */ Vec3s* flowerPos;
    /* 0x1E8 */ s16 index;
    /* 0x1EA */ s16 flowerCount;
    /* 0x1EC */ s16 shouldDrawFlowers;
    /* 0x1EE */ s16 timeToBurrow;
    /* 0x1F0 */ s16 headdressType;
    /* 0x1F2 */ s16 headdressRotZ;
    /* 0x1F4 */ Vec3s jointTable[DEKU_SCRUB_LIMB_MAX];
    /* 0x230 */ Vec3s morphTable[DEKU_SCRUB_LIMB_MAX];
    /* 0x26C */ UNK_TYPE1 unk_26C[0x4];
} EnSyatekiDekunuts; // size = 0x270

#endif // Z_EN_SYATEKI_DEKUNUTS_H
