#ifndef Z_EN_VM_H
#define Z_EN_VM_H

#include "ultra64.h"
#include "global.h"

struct EnVm;

typedef void (*EnVmActionFunc)(struct EnVm*, PlayState*);

typedef struct EnVm {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ Vec3s jointTable[11];
    /* 0x01D2 */ Vec3s morphTable[11];
    /* 0x0214 */ EnVmActionFunc actionFunc;
    /* 0x0218 */ f32 beamSightRange;
    /* 0x021C */ s32 unk_21C;
    /* 0x0220 */ s32 timer;
    /* 0x0224 */ Vec3f beamPos1;
    /* 0x0230 */ Vec3f beamPos2;
    /* 0x023C */ Vec3f beamPos3;
    /* 0x0248 */ Vec3f beamScale;
    /* 0x0254 */ Vec3s beamRot;
    /* 0x025A */ s16 beamTexScroll;
    /* 0x025C */ s16 headRotY;
    /* 0x025E */ s16 unk_25E;
    /* 0x0260 */ s16 unk_260;
    /* 0x0264 */ f32 beamSpeed;
    /* 0x0268 */ ColliderCylinder colliderCylinder;
    /* 0x02B4 */ ColliderQuad colliderQuad1;
    /* 0x0334 */ ColliderQuad colliderQuad2;
} EnVm; // size = 0x03B4

typedef enum {
    /* 0x00 */ BEAMOS_LARGE,
    /* 0x01 */ BEAMOS_SMALL
} BeamosType;

#endif
