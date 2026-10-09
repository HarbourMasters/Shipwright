#ifndef Z_BG_DY_YOSEIZO_H
#define Z_BG_DY_YOSEIZO_H

#include "global.h"
#include "overlays/actors/ovl_En_Dy_Extra/z_en_dy_extra.h"
#include "objects/object_dy_obj/object_dy_obj.h"

struct BgDyYoseizo;

typedef void (*BgDyYoseizoActionFunc)(struct BgDyYoseizo*, PlayState*);

#define GREAT_FAIRY_GET_SWITCHFLAG(thisx) (((thisx)->params & 0xFE00) >> 9)
#define GREAT_FAIRY_GET_TYPE(thisx) ((thisx)->params & 0xF)
#define GREAT_FAIRY_ROTZ(thisx) ((thisx)->home.rot.z)

typedef enum GreatFairyType {
    /* 0 */ GREAT_FAIRY_TYPE_MAGIC,
    /* 1 */ GREAT_FAIRY_TYPE_POWER,
    /* 2 */ GREAT_FAIRY_TYPE_WISDOM,
    /* 3 */ GREAT_FAIRY_TYPE_COURAGE,
    /* 4 */ GREAT_FAIRY_TYPE_KINDNESS
} GreatFairyType;

#define BG_DY_YOSEIZO_EFFECT_COUNT 200

typedef struct BgDyYoseizoEffect {
    /* 0x00 */ u8 alive; // drawn if 1, respawn if 0
    /* 0x04 */ Vec3f pos;
    /* 0x10 */ Vec3f velocity;
    /* 0x1C */ Vec3f accel;
    /* 0x28 */ Color_RGB8 primColor;
    /* 0x2B */ Color_RGB8 envColor;
    /* 0x2E */ s16 alpha;
    /* 0x30 */ f32 scale;
    /* 0x34 */ s16 timer; // lifetime
    /* 0x36 */ s16 type; // 0 is general radiance, else is directed towards Player
    /* 0x38 */ s16 pitch;
    /* 0x3A */ s16 yaw;
    /* 0x3C */ s16 roll; // all three are f32 in OoT
} BgDyYoseizoEffect; // size = 0x40

typedef struct BgDyYoseizo {
    /* 0x0000 */ Actor actor;
    /* 0x0144 */ BgDyYoseizoActionFunc actionFunc;
    /* 0x0148 */ SkelAnime skelAnime;
    /* 0x018C */ Vec3s jointTable[GREAT_FAIRY_LIMB_MAX];
    /* 0x0234 */ Vec3s morphTable[GREAT_FAIRY_LIMB_MAX];
    /* 0x02DC */ Vec3s headRot;
    /* 0x02DC */ Vec3s torsoRot;
    /* 0x02E8 */ EnDyExtra* beam;
    /* 0x02EC */ f32 unk2EC; // unused
    /* 0x02F0 */ f32 unk2F0;
    /* 0x02F4 */ f32 unk2F4;
    /* 0x02F8 */ union {
        s16 unk2F8;
        s16 csId; // used on BgDyYoseizo_TrainPlayer
    };
    /* 0x02FA */ s16 eyeIndex;
    /* 0x02FC */ s16 mouthIndex;
    /* 0x02FE */ s16 blinkTimer;
    /* 0x0300 */ s16 timer;
    /* 0x0302 */ u16 unk302;
    /* 0x0304 */ BgDyYoseizoEffect effects[BG_DY_YOSEIZO_EFFECT_COUNT];
} BgDyYoseizo; // size = 0x3504

#endif // Z_BG_DY_YOSEIZO_H
