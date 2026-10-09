#ifndef Z_EN_DNP_H
#define Z_EN_DNP_H

#include "global.h"
#include "objects/object_dnq/object_dnq.h"

struct EnDnp;

typedef void (*EnDnpActionFunc)(struct EnDnp*, PlayState*);

#define DEKU_PRINCESS_GET_TYPE(thisx) ((thisx)->params & 7)

#define DEKU_PRINCESS_PARAMS(type) ((type) & 7)

typedef enum {
    /* 0 */ DEKU_PRINCESS_TYPE_WOODFALL_TEMPLE,
    /* 1 */ DEKU_PRINCESS_TYPE_RELEASED_FROM_BOTTLE,
    /* 2 */ DEKU_PRINCESS_TYPE_DEKU_KINGS_CHAMBER
} EnDnpType;

typedef struct EnDnp {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ EnDnpActionFunc actionFunc;
    /* 0x18C */ ColliderCylinder collider;
    /* 0x1D8 */ Vec3f unk_1D8;
    /* 0x1E4 */ Vec3s unk_1E4;
    /* 0x1EA */ Vec3s jointTable[DEKU_PRINCESS_LIMB_MAX];
    /* 0x286 */ Vec3s morphTable[DEKU_PRINCESS_LIMB_MAX];
    /* 0x322 */ u16 unk_322;
    /* 0x324 */ u8 cueId;
    /* 0x328 */ s32 msgScriptPos;
    /* 0x32C */ UNK_TYPE1 unk_32C[0x2];
    /* 0x32E */ s16 unk_32E;
    /* 0x330 */ s16 unk_330;
    /* 0x332 */ s16 unk_332;
    /* 0x334 */ s16 blinkTimer;
    /* 0x336 */ s16 eyeIndex;
    /* 0x338 */ s16 unk_338;
    /* 0x33A */ UNK_TYPE1 unk_33A[0x6];
    /* 0x340 */ s32 animIndex;
} EnDnp; // size = 0x344

#endif // Z_EN_DNP_H
