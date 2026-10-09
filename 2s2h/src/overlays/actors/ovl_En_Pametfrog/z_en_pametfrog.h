#ifndef Z_EN_PAMETFROG_H
#define Z_EN_PAMETFROG_H

#include "global.h"
#include "objects/object_bigslime/object_bigslime.h"

struct EnPametfrog;

typedef void (*EnPametfrogActionFunc)(struct EnPametfrog*, PlayState*);

typedef enum {
    /* 0 */ GEKKO_PRE_SNAPPER,
    /* 1 */ GEKKO_GET_SNAPPER,
    /* 2 */ GEKKO_INIT_SNAPPER,
    /* 3 */ GEKKO_ON_SNAPPER,
    /* 4 */ GEKKO_REAR_ON_SNAPPER,
    /* 5 */ GEKKO_CUTSCENE,
    /* 6 */ GEKKO_FALL_OFF_SNAPPER,
    /* 7 */ GEKKO_RETURN_TO_SNAPPER,
    /* 8 */ GEKKO_JUMP_ON_SNAPPER,
    /* 9 */ GEKKO_DEFEAT
} EnPametfrogState;

typedef enum GekkoBodyPart {
    /* -1 */ GEKKO_BODYPART_NONE = -1,
    /*  0 */ GEKKO_BODYPART_WAIST,
    /*  1 */ GEKKO_BODYPART_LEFT_SHIN,
    /*  2 */ GEKKO_BODYPART_LEFT_FOOT,
    /*  3 */ GEKKO_BODYPART_RIGHT_SHIN,
    /*  4 */ GEKKO_BODYPART_RIGHT_FOOT,
    /*  5 */ GEKKO_BODYPART_LEFT_UPPER_ARM,
    /*  6 */ GEKKO_BODYPART_LEFT_FOREARM,
    /*  7 */ GEKKO_BODYPART_LEFT_HAND,
    /*  8 */ GEKKO_BODYPART_RIGHT_UPPER_ARM,
    /*  9 */ GEKKO_BODYPART_RIGHT_FOREARM,
    /* 10 */ GEKKO_BODYPART_RIGHT_HAND,
    /* 11 */ GEKKO_BODYPART_JAW,
    /* 12 */ GEKKO_BODYPART_MAX
} GekkoBodyPart;

typedef struct EnPametfrog {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ Vec3s jointTable[GEKKO_LIMB_MAX];
    /* 0x218 */ Vec3s morphTable[GEKKO_LIMB_MAX];
    /* 0x2A8 */ EnPametfrogActionFunc actionFunc;
    /* 0x2AC */ u8 drawDmgEffType;
    /* 0x2AD */ u8 wallPauseTimer; // Gekko stops 10 times along wall/ceiling after being blown off of Snapper
    /* 0x2AE */ u8 unk_2AE; // True/False
    /* 0x2B0 */ s16 csId;
    /* 0x2B2 */ s16 params;
    /* 0x2B4 */ s16 quakeIndex;
    /* 0x2B6 */ s16 timer;
    /* 0x2B8 */ s16 spinYaw;
    /* 0x2BA */ s16 subCamId;
    /* 0x2BC */ s16 freezeTimer;
    /* 0x2C0 */ f32 wallRotation;
    /* 0x2C4 */ f32 drawDmgEffAlpha;
    /* 0x2C8 */ f32 drawDmgEffScale;
    /* 0x2CC */ f32 drawDmgEffFrozenSteamScale; 
    /* 0x2D0 */ Vec3f unk_2D0; // MtxF xz/yz/zz
    /* 0x2DC */ Vec3f unk_2DC; // MtxF xy/yy/zy: wallNorm/floorNorm/Base of Gekko walking???
    /* 0x2E8 */ Vec3f unk_2E8; // MtxF xx/yx/zx
    /* 0x2F4 */ Vec3f bodyPartsPos[GEKKO_BODYPART_MAX]; 
    /* 0x384 */ ColliderJntSph collider;
    /* 0x3A4 */ ColliderJntSphElement colElement[2];
} EnPametfrog; // size = 0x424

#endif // Z_EN_PAMETFROG_H
