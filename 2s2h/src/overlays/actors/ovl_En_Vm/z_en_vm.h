#ifndef Z_EN_VM_H
#define Z_EN_VM_H

#include "global.h"
#include "assets/objects/object_vm/object_vm.h"

struct EnVm;

typedef void (*EnVmActionFunc)(struct EnVm*, PlayState*);

#define ENVM_GET_FF00(thisx) (((thisx)->params >> 8) & 0xFF)

#define ENVM_FF00_0 0
#define ENVM_FF00_5 5
#define ENVM_FF00_FF 0xFF

typedef struct EnVm {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ Vec3s jointTable[BEAMOS_LIMB_MAX];
    /* 0x1CA */ Vec3s morphTable[BEAMOS_LIMB_MAX];
    /* 0x20C */ EnVmActionFunc actionFunc;
    /* 0x210 */ u8 unk_210;
    /* 0x212 */ s16 unk_212;
    /* 0x214 */ s16 unk_214;
    /* 0x216 */ s16 unk_216;
    /* 0x218 */ s16 unk_218;
    /* 0x21C */ f32 unk_21C;
    /* 0x220 */ f32 unk_220;
    /* 0x224 */ f32 unk_224;
    /* 0x228 */ Vec3f unk_228;
    /* 0x234 */ Vec3f unk_234;
    /* 0x240 */ ColliderJntSph colliderJntSph;
    /* 0x260 */ ColliderJntSphElement colliderJntSphElements[2];
    /* 0x2E0 */ ColliderTris colliderTris;
    /* 0x300 */ ColliderTrisElement colliderTrisElements[1];
} EnVm; // size = 0x35C

#endif // Z_EN_VM_H
