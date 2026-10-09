#ifndef Z_DM_NB_H
#define Z_DM_NB_H

#include "global.h"
#include "objects/object_nb/object_nb.h"

struct DmNb;

typedef void (*DmNbActionFunc)(struct DmNb*, PlayState*);

typedef struct DmNb {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ DmNbActionFunc actionFunc;
    /* 0x18C */ Vec3s jointTable[NB_LIMB_MAX];
    /* 0x1BC */ Vec3s morphTable[NB_LIMB_MAX];
    /* 0x1EC */ u8 cueId;
    /* 0x1F0 */ s32 animIndex;
    /* 0x1F4 */ s32 prevAnimIndex;
    /* 0x1F8 */ s32 isCutscenePlaying;
} DmNb; // size = 0x1FC

#endif // Z_DM_NB_H
