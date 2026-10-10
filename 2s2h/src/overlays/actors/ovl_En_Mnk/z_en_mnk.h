#ifndef Z_EN_MNK_H
#define Z_EN_MNK_H

#include "global.h"
#include "objects/object_mnk/object_mnk.h"
#include "z64snap.h"

struct EnMnk;

typedef void (*EnMnkActionFunc)(struct EnMnk*, PlayState*);
typedef void (*EnMnkFunc)(PlayState*, struct EnMnk*);

#define MONKEY_GET_TYPE(thisx) (((thisx)->params & 0x780) >> 7)
#define MONKEY_GET_SWITCH_FLAG(thisx) ((thisx)->params & 0x7F)
#define MONKEY_GET_PATH_INDEX(thisx) (((thisx)->params & 0xF800) >> 11)

#define MONKEY_SWITCH_FLAG_NONE 0x7F
#define MONKEY_PATH_INDEX_NONE 0x1F

typedef enum {
    /*  0 */ MONKEY_0,
    /*  1 */ MONKEY_OUTSIDEWOODS,
    /*  2 */ MONKEY_BY_WITCH,
    /*  3 */ MONKEY_OUTSIDECHAMBER,
    /*  4 */ MONKEY_OUTSIDEPALACE,
    /*  5 */ MONKEY_TIED_UP,
    /*  6 */ MONKEY_HANGING,
    /*  7 */ MONKEY_WOODS_GUIDE,
    /*  8 */ MONKEY_8,
    /*  9 */ MONKEY_AFTER_SAVED,
    /* 10 */ MONKEY_10
} MonkeyType;

#define MONKEY_FLAGS_1 (1 << 0)
#define MONKEY_FLAGS_2 (1 << 1)
#define MONKEY_FLAGS_4 (1 << 2)
#define MONKEY_FLAGS_8 (1 << 3)
#define MONKEY_FLAGS_10 (1 << 4)
#define MONKEY_FLAGS_20 (1 << 5)
#define MONKEY_FLAGS_40 (1 << 6)
#define MONKEY_FLAGS_80 (1 << 7)
#define MONKEY_FLAGS_100 (1 << 8)
#define MONKEY_FLAGS_200 (1 << 9)
#define MONKEY_FLAGS_400 (1 << 10)
#define MONKEY_FLAGS_800 (1 << 11)
#define MONKEY_FLAGS_1000 (1 << 12)
#define MONKEY_FLAGS_2000 (1 << 13)
#define MONKEY_FLAGS_4000 (1 << 14)
#define MONKEY_FLAGS_8000 (1 << 15)

#define MONKEY_PROP_LIMB_MAX MAX((s32)OBJECT_MNK_1_LIMB_MAX, (s32)OBJECT_MNK_3_LIMB_MAX)

typedef struct EnMnk {
    /* 0x000 */ PictoActor picto;
    /* 0x148 */ SkelAnime skelAnime;
    /* 0x18C */ Vec3s jointTable[OBJECT_MNK_2_LIMB_MAX];
    /* 0x216 */ Vec3s morphTable[OBJECT_MNK_2_LIMB_MAX];
    /* 0x2A0 */ SkelAnime propSkelAnime;
    /* 0x2E4 */ Vec3s propJointTable[MONKEY_PROP_LIMB_MAX];
    /* 0x302 */ Vec3s propMorphTable[MONKEY_PROP_LIMB_MAX];
    /* 0x320 */ ColliderCylinder collider;
    /* 0x36C */ MtxF unk_36C;
    /* 0x3AC */ Path* path;
    /* 0x3B0 */ s16 csIdList[6];
    /* 0x3BC */ s16 csId;
    /* 0x3BE */ s16 animIndex;
    /* 0x3C0 */ s32 destPointIndex;
    /* 0x3C4 */ s32 pathIndex;
    /* 0x3C8 */ s32 unk_3C8;
    /* 0x3CC */ s16 unk_3CC; // beard x rot?
    /* 0x3CE */ s16 unk_3CE; // beard y rot?
    /* 0x3D0 */ union {
                    s16 cueId; // Tied monkey
                    s16 unk_3D0; // Hanging monkey
                };
    /* 0x3D4 */ u32 unk_3D4;
    /* 0x3D8 */ f32 approachPlayerRadius;
    /* 0x3DC */ s16 blinkFrame;
    /* 0x3DE */ s16 blinkTimer;
    /* 0x3E0 */ s32 unk_3E0;
    /* 0x3E4 */ u16 flags;
    /* 0x3E8 */ EnMnkActionFunc actionFunc;
} EnMnk; // size = 0x3EC

#endif // Z_EN_MNK_H
