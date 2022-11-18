#ifndef Z_EN_SYATEKI_MAN_H
#define Z_EN_SYATEKI_MAN_H

#include "ultra64.h"
#include "global.h"

struct EnSyatekiMan;

typedef void (*EnSyatekiManActionFunc) (struct EnSyatekiMan*, PlayState*);
typedef void (*EnSyatekiManOtherFunc) (struct EnSyatekiMan*);

typedef struct EnSyatekiMan {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ Vec3s jointTable[9];
    /* 0x01C6 */ Vec3s morphTable[9];
    /* 0x01FC */ EnSyatekiManActionFunc actionFunc;
    /* 0x0200 */ Vec3s headRot;
    /* 0x0206 */ Vec3s bodyRot;
    /* 0x020C */ s16 eyeState; // Unused
    /* 0x020E */ s16 blinkTimer; // Unused
    /* 0x0210 */ s16 textIdx;
    /* 0x0212 */ s16 numTextBox;
    /* 0x0214 */ s16 gameResult;
    /* 0x0216 */ s16 timer;
    /* 0x0218 */ s32 getItemId;
    /* 0x021C */ u8 cameraHold;
    /* 0x0220 */ Actor* tempGallery;
    /* 0x0224 */ EnSyatekiManOtherFunc blinkFunc; // Seems to be part of a blink system with unk_20C and unk_20E, but it's unused.
    /* 0x0228 */ s16 csCam;
    /*        */ GetItemEntry getItemEntry;
} EnSyatekiMan; // size = 0x022C

#endif
