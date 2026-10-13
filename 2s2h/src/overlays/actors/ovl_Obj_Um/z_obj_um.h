#ifndef Z_OBJ_UM_H
#define Z_OBJ_UM_H

#include "global.h"
#include "objects/object_um/object_um.h"
#include "overlays/actors/ovl_En_Horse/z_en_horse.h"

struct ObjUm;

typedef void (*ObjUmActionFunc)(struct ObjUm*, PlayState*);

#define MILK_POTS_COUNT 3

typedef enum ObjUmAnimation {
    /* -1 */ OBJ_UM_ANIM_MINUS_1 = -1,
    /*  0 */ OBJ_UM_ANIM_TROT,
    /*  1 */ OBJ_UM_ANIM_GALLOP,
    /*  2 */ OBJ_UM_ANIM_IDLE,
    /*  3 */ OBJ_UM_ANIM_3, // NULL pointer
    /*  4 */ OBJ_UM_ANIM_LOOK_BACK
} ObjUmAnimation;

typedef struct ObjUm {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ ObjUmActionFunc actionFunc;
    /* 0x160 */ SkelAnime skelAnime;
    /* 0x1A4 */ Vec3s jointTable[UM_LIMB_MAX];
    /* 0x228 */ Vec3s morphTable[UM_LIMB_MAX];
    /* 0x2AC */ s16 wheelRot;
    /* 0x2AE */ s16 type;
    /* 0x2B0 */ s16 initialPathIndex;
    /* 0x2B4 */ s32 unk_2B4;
    /* 0x2B8 */ EnHorse* donkey;
    /* 0x2BC */ s32 pathIndex;
    /* 0x2BE */ s32 pointIndex;
    /* 0x2C4 */ Vec3f unk_2C4;
    /* 0x2D0 */ Vec3f unk_2D0;
    /* 0x2DC */ Vec3f unk_2DC;
    /* 0x2E8 */ Vec3f unk_2E8;
    /* 0x2F4 */ s32 flags;
    /* 0x2BC */ Vec3s unk_2F8;
    /* 0x2FE */ Vec3s unk_2FE;
    /* 0x304 */ ObjUmAnimation animIndex;
    /* 0x308 */ Vec3f unk_308;
    /* 0x314 */ s32 potsLife[MILK_POTS_COUNT];
    /* 0x320 */ s32 wasPotHit[MILK_POTS_COUNT]; // resets to false in the same frame
    /* 0x32C */ Vec3f potPos[MILK_POTS_COUNT];
    /* 0x350 */ s32 unk_350; // unused counter?
    /* 0x354 */ s32 unk_354; // unused?
    /* 0x358 */ EnHorse* bandit1;
    /* 0x35C */ EnHorse* bandit2;
    /* 0x360 */ Vec3f unk_360[16];
    /* 0x420 */ s32 unk_420; // ?
    /* 0x424 */ ColliderCylinder banditsCollisions[2];
    /* 0x4BC */ Vec3f cartBedPos;
    /* 0x4C8 */ u16 lastTime;
    /* 0x4CC */ s32 unk_4CC;
    /* 0x4D0 */ s32 eyeTexIndex;
    /* 0x4D4 */ s32 mouthTexIndex;
    /* 0x4D8 */ s32 unk_4D8;
    /* 0x4DC */ s32 unk_4DC;
    /* 0x4E0 */ s32 areAllPotsBroken; // true when all of the pots have been broken
} ObjUm; // size = 0x4E4

typedef enum {
    /* 0 */ OBJ_UM_TYPE_TERMINA_FIELD,
    /* 1 */ OBJ_UM_TYPE_RANCH,
    /* 2 */ OBJ_UM_TYPE_PRE_MILK_RUN, // milk road, pre-minigame
    /* 3 */ OBJ_UM_TYPE_MILK_RUN_MINIGAME,
    /* 4 */ OBJ_UM_TYPE_POST_MILK_RUN // milk road, post-minigame
} ObjUmType;

#define OBJ_UM_GET_TYPE(thisx) (((thisx)->params & 0xFF00) >> 8)
#define OBJ_UM_GET_PATH_INDEX(thisx) ((thisx)->params & 0xFF)

#define OBJ_UM_FLAG_NONE (0)
#define OBJ_UM_FLAG_0001 (1 << 0)
#define OBJ_UM_FLAG_MOVING (1 << 1)
#define OBJ_UM_FLAG_0004 (1 << 2)
#define OBJ_UM_FLAG_WAITING (1 << 3) // Waiting in the Ranch
#define OBJ_UM_FLAG_0010 (1 << 4)
#define OBJ_UM_FLAG_DRAWN_FLOOR (1 << 5)
#define OBJ_UM_FLAG_0040 (1 << 6)
#define OBJ_UM_FLAG_PLAYING_MINIGAME (1 << 7)
#define OBJ_UM_FLAG_0100 (1 << 8)
#define OBJ_UM_FLAG_0200 (1 << 9) // Something bandit1
#define OBJ_UM_FLAG_0400 (1 << 10) // Something bandit2
#define OBJ_UM_FLAG_LOOK_BACK (1 << 11)
#define OBJ_UM_FLAG_1000 (1 << 12)
#define OBJ_UM_FLAG_MINIGAME_FINISHED (1 << 13)

#endif // Z_OBJ_UM_H
