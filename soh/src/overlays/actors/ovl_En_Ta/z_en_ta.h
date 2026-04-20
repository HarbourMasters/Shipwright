#ifndef Z_EN_TA_H
#define Z_EN_TA_H

#include <libultraship/libultra.h>
#include "global.h"

#include "overlays/actors/ovl_En_Niw/z_en_niw.h"

struct EnTa;

typedef void (*EnTaActionFunc)(struct EnTa*, PlayState*);
typedef void (*EnTaUnkFunc)(struct EnTa*);

typedef struct EnTa {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ Vec3s jointTable[17];
    /* 0x01F6 */ Vec3s morphTable[17];
    /* 0x025C */ EnTaActionFunc actionFunc;
    /* 0x0260 */ EnTaUnkFunc animFunc;
    /* 0x0264 */ ColliderCylinder collider;
    /* 0x02B0 */ EnTaUnkFunc blinkFunc;
    /* 0x02B4 */ s16 eyeIndex;
    /* 0x02B6 */ s16 blinkTimer;
    /* 0x02B8 */ EnNiw* superCuccos[3];
    /* 0x02C4 */ s16 superCuccoTimers[3];
    /* 0x02CA */ u8 lastFoundSuperCuccoIdx;
    /* 0x02CC */ s16 timer;
    /* 0x02CE */ s16 rapidBlinks;
    /* 0x02D0 */ s16 subCamId;
    /* 0x02D2 */ s16 returnToCamId;
    /* 0x02D4 */ Vec3s headRot;
    /* 0x02DA */ Vec3s torsoRot;
    /* 0x02E0 */ u16 stateFlags;
    /* 0x02E2 */ s16 nodOffTimer;
    /* 0x02E4 */ AnimationHeader* currentAnimation;
} EnTa; // size = 0x02E8

#endif
