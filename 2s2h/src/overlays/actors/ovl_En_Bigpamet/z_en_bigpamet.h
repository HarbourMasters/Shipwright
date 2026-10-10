#ifndef Z_EN_BIGPAMET_H
#define Z_EN_BIGPAMET_H

#include "global.h"
#include "objects/object_tl/object_tl.h"

struct EnBigpamet;

typedef void (*EnBigpametActionFunc)(struct EnBigpamet*, PlayState*);

typedef enum {
    /* 0 */ ENBIGPAMET_0,    
    /* 1 */ ENBIGPAMET_1
} EnBigpametParam;

typedef struct {
    /* 0x00 */ Vec3f unk_00;
    /* 0x0C */ Vec3f unk_0C;
    /* 0x18 */ Vec3s unk_18;
    /* 0x20 */ f32 unk_20;
} EnBigpametStruct; // size = 0x24

typedef struct EnBigpamet {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime snapperSkelAnime;
    /* 0x188 */ Vec3s snapperJointTable[SNAPPER_LIMB_MAX];
    /* 0x1D6 */ Vec3s snapperMorphTable[SNAPPER_LIMB_MAX];
    /* 0x224 */ SkelAnime spikedSnapperSkelAnime;
    /* 0x268 */ Vec3s spikedSnapperJointTable[SPIKED_SNAPPER_LIMB_MAX];
    /* 0x280 */ Vec3s spikedSnapperMorphTable[SPIKED_SNAPPER_LIMB_MAX];
    /* 0x298 */ EnBigpametActionFunc actionFunc;
    /* 0x29C */ u8 unk_29C;
    /* 0x29E */ s16 unk_29E;
    /* 0x2A0 */ UNK_TYPE1 pad2A0[2];
    /* 0x2A2 */ s16 unk_2A2;
    /* 0x2A4 */ f32 unk_2A4;
    /* 0x2A8 */ f32 unk_2A8;
    /* 0x2AC */ f32 unk_2AC;
    /* 0x2B0 */ ColliderCylinder collider;
    /* 0x2FC */ EnBigpametStruct unk_2FC[20];
} EnBigpamet; // size = 0x5CC

#endif // Z_EN_BIGPAMET_H
