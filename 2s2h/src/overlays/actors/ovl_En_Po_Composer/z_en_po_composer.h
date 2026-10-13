#ifndef Z_EN_PO_COMPOSER_H
#define Z_EN_PO_COMPOSER_H

#include "global.h"
#include "objects/object_po_composer/object_po_composer.h"

#define POE_COMPOSER_IS_FLAT(thisx) ((thisx)->params & 0x8000)
#define POE_COMPOSER_4000(thisx)    ((thisx)->params & 0x4000)

typedef enum {
    /*  1 */ POE_COMPOSER_CUEID_IDLE = 1,
    /*  2 */ POE_COMPOSER_CUEID_APPEAR_SPIN,
    /*  3 */ POE_COMPOSER_CUEID_IDLE2,
    /*  4 */ POE_COMPOSER_CUEID_RAISE_ARMS,
    /*  5 */ POE_COMPOSER_CUEID_LOWER_ARMS,
    /*  6 */ POE_COMPOSER_CUEID_PLAY_CURSE,
    /*  7 */ POE_COMPOSER_CUEID_ROLL,
    /*  8 */ POE_COMPOSER_CUEID_APPEAR,
    /*  9 */ POE_COMPOSER_CUEID_DISAPPEAR,
    /* 10 */ POE_COMPOSER_CUEID_DONE,
    /* 99 */ POE_COMPOSER_CUEID_NONE = 99
} EnPoComposerCueId;

typedef enum {
    /* 0 */ SHARP_CS_ENCOUNTER_FIRST,
    /* 1 */ SHARP_CS_ENCOUNTER_OTHER,
    /* 2 */ SHARP_CS_SONG_HEALING,
    /* 3 */ SHARP_CS_SONG_STORMS,
    /* 4 */ SHARP_CS_MAX
} SharpCsIdIndex;

struct EnPoComposer;

typedef void (*EnPoComposerActionFunc)(struct EnPoComposer*, PlayState*);

typedef struct EnPoComposer {
    /* 0x000 */ Actor actor;
    /* 0x144 */ ColliderCylinder bodyCollider;
    /* 0x190 */ ColliderJntSph lanternCollider;
    /* 0x1B0 */ ColliderJntSphElement lanternColliderElements[1];
    /* 0x1F0 */ SkelAnime skelAnime;
    /* 0x234 */ EnPoComposerActionFunc actionFunc;
    /* 0x238 */ MtxF lanternMtxF;
    /* 0x278 */ LightNode* lightNode;
    /* 0x27C */ LightInfo lightInfo;
    /* 0x28A */ UNK_TYPE1 unk_28A[0x6];
    /* 0x290 */ s32 sharpCsNum;
    /* 0x294 */ Vec3s jointTable[POE_COMPOSER_LIMB_MAX];
    /* 0x2DC */ Vec3s morphTable[POE_COMPOSER_LIMB_MAX];
    /* 0x324 */ s16 csIds[SHARP_CS_MAX];
    /* 0x32C */ s16 actionTimer;
    /* 0x32E */ s16 csCueTimer;
    /* 0x330 */ s16 inCutscene;
    /* 0x332 */ s16 visible;
    /* 0x334 */ u16 cueId;
    /* 0x336 */ Color_RGBA8 lightColor;
    /* 0x33A */ Color_RGBA8 envColor;
} EnPoComposer; // size = 0x340

#endif // Z_EN_PO_COMPOSER_H
