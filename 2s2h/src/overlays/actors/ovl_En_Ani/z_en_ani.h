#ifndef Z_EN_ANI_H
#define Z_EN_ANI_H

#include "global.h"
#include "objects/object_ani/object_ani.h"

struct EnAni;

typedef void (*EnAniActionFunc)(struct EnAni*, PlayState*);
typedef void (*EnAniBlinkFunc)(struct EnAni*);


typedef struct EnAni {
    /* 0x000 */ Actor actor;
    /* 0x144 */ ColliderCylinder collider1;
    /* 0x190 */ ColliderCylinder collider2;
    /* 0x1DC */ SkelAnime skelAnime;
    /* 0x220 */ Vec3s jointTable[ANI_LIMB_MAX];
    /* 0x280 */ Vec3s morphTable[ANI_LIMB_MAX];
    /* 0x2E0 */ Vec3s headRot;
    /* 0x2E6 */ Vec3s torsoRot; // unused 
    /* 0x2EC */ u16 stateFlags;
    /* 0x2EE */ s16 unk2EE; // assigned to zero in init, never used after
    /* 0x2F0 */ s16 treeReachTimer;
    /* 0x2F4 */ EnAniBlinkFunc blinkFunc;
    /* 0x2F8 */ s16 eyeState;
    /* 0x2FA */ s16 blinkTimer;
    /* 0x2FC */ EnAniActionFunc actionFunc;
} EnAni; // size = 0x300

typedef enum EnAniType {
  /* 1 */ ANI_TYPE_TREE_HANGING = 1,
  /* 2 */ ANI_TYPE_STANDING // unfinshed and unused. Never read as 2, only taken as the "else" case from `ANI_TYPE_TREE_HANGING`
} EnAniType;

#define ANI_GET_TYPE(thisx) ((thisx)->params & 0xFF) 

#endif // Z_EN_ANI_H
