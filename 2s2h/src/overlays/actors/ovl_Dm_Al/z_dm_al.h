#ifndef Z_DM_AL_H
#define Z_DM_AL_H

#include "global.h"
#include "objects/object_al/object_al.h"

struct DmAl;

typedef void (*DmAlActionFunc)(struct DmAl*, PlayState*);

typedef struct DmAl {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ DmAlActionFunc actionFunc;
    /* 0x18C */ MtxF shawlMatrices[6];
    /* 0x30C */ Vec3s jointTable[MADAME_AROMA_LIMB_MAX];
    /* 0x3AE */ Vec3s morphTable[MADAME_AROMA_LIMB_MAX];
    /* 0x450 */ u8 cueId;
    /* 0x454 */ s32 animIndex;
    /* 0x458 */ s32 prevAnimIndex;
    /* 0x45C */ s32 isCutscenePlaying;
} DmAl; // size = 0x460

#endif // Z_DM_AL_H
