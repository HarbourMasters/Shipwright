#ifndef Z_EN_RUPPECROW_H
#define Z_EN_RUPPECROW_H

#include "global.h"
#include "objects/object_crow/object_crow.h"

#define ENRUPPECROW_GET_PATH_INDEX(thisx) (((thisx)->params & 0xFC00) >> 0xA)

#define ENRUPPECROW_PATH_INDEX_NONE 0x3F

#define ENRUPPECROW_RUPEE_COUNT 20

struct EnRuppecrow;

typedef void (*EnRuppecrowActionFunc)(struct EnRuppecrow*, PlayState*);

typedef enum EnRuppecrowBodyPart {
    /* 0 */ ENRUPPECROW_BODYPART_0,
    /* 1 */ ENRUPPECROW_BODYPART_1,
    /* 2 */ ENRUPPECROW_BODYPART_2,
    /* 3 */ ENRUPPECROW_BODYPART_3,
    /* 4 */ ENRUPPECROW_BODYPART_MAX
} EnRuppecrowBodyPart;

typedef struct EnRuppecrow {
    /* 0x000 */ Actor actor;
    /* 0x144 */ EnItem00* rupees[ENRUPPECROW_RUPEE_COUNT];
    /* 0x194 */ SkelAnime skelAnime;
    /* 0x1D8 */ EnRuppecrowActionFunc actionFunc;
    /* 0x1DC */ UNK_TYPE1 unk_1DC[0x4];
    /* 0x1E0 */ Vec3s jointTable[OBJECT_CROW_LIMB_MAX];
    /* 0x216 */ Vec3s morphTable[OBJECT_CROW_LIMB_MAX];
    /* 0x24C */ Path* path;
    /* 0x250 */ s32 currentPoint;
    /* 0x254 */ ColliderJntSph collider;
    /* 0x274 */ ColliderJntSphElement colliderElements[1];
    /* 0x2B4 */ u16 isGoingCounterClockwise;
    /* 0x2B8 */ f32 speedModifier;
    /* 0x2BC */ s16 rupeeIndex;
    /* 0x2BE */ s16 yOffset;
    /* 0x2C0 */ UNK_TYPE1 unk_2C0[0x4];
    /* 0x2C4 */ u8 currentEffect; 
    /* 0x2C8 */ f32 unk_2C8; // set but not used
    /* 0x2CC */ f32 unk_2CC; // set but not used
    /* 0x2D0 */ f32 iceSfxTimer;
    /* 0x2D4 */ Vec3f bodyPartsPos[ENRUPPECROW_BODYPART_MAX];
} EnRuppecrow; // size = 0x304

#endif // Z_EN_RUPPECROW_H
