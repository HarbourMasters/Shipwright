#ifndef Z_EN_HEISHI2_H
#define Z_EN_HEISHI2_H

#include "ultra64.h"
#include "global.h"


struct EnHeishi2;

typedef void (*EnHeishi2ActionFunc)(struct EnHeishi2*, PlayState*);

typedef struct EnHeishi2 {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ Vec3s jointTable[17];
    /* 0x01F6 */ Vec3s morphTable[17];
    /* 0x025C */ EnHeishi2ActionFunc actionFunc;
    /* 0x0260 */ Vec3s unk_260;
    /* 0x0266 */ char unk_266[0x06];
    /* 0x026C */ Vec3s unk_26C; // padding inbetween these
    /* 0x0274 */ Vec3f unk_274; 
    /* 0x0280 */ Vec3f unk_280; // camera related
    /* 0x028C */ Vec3f unk_28C; // camera related
    /* 0x0298 */ Vec3f unk_298; // camera related
    /* 0x02A4 */ char unk_2A4[0x18];
    /* 0x02BC */ Vec3f unk_2BC; // camera related
    /* 0x02C8 */ char unk_2C8[0x18];
    /* 0x02E0 */ f32 unk_2E0;
    /* 0x02E4 */ f32 unk_2E4;
    /* 0x02E8 */ char unk_2E8[0x4];
    /* 0x02EC */ f32 unk_2EC;
    /* 0x02F0 */ u16 unk_2F0;
    /* 0x02F2 */ s16 unk_2F2[0x5]; // starts counting down when guard starts to open gate.
    /* 0x02FC */ s16 unk_2FC;
    /* 0x02FE */ s16 type; // copy of actor params at init
    /* 0x0300 */ s16 unk_300; // mask related
    /* 0x0302 */ char unk_302[0x6];
    /* 0x0308 */ u8 audioFlag;
    /* 0x0309 */ u8 unk_309;
    /* 0x030A */ u8 unk_30A;
    /* 0x030B */ u8 unk_30B;
    /* 0x030C */ u8 unk_30C;
    /* 0x030D */ char unk_30D;
    /* 0x030E */ s16 unk_30E;
    /* 0x0310 */ s16 cameraId;
    /* 0x0314 */ Actor* gate; // first BgGateShutter that can be found in the actor list
    /* 0x0318 */ char unk_318[0x18];
    /* 0x0330 */ MtxF mtxf_330;
    /* 0x0370 */ char unk_370[0x28];
    /* 0x0398 */ ColliderCylinder collider;
} EnHeishi2; // size = 0x03E4

#endif
