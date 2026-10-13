#ifndef Z_EN_TITE_H
#define Z_EN_TITE_H

#include "global.h"
#include "objects/object_tite/object_tite.h"

struct EnTite;

typedef void (*EnTiteActionFunc)(struct EnTite*, PlayState*);

typedef enum EnTiteParam {
    /* -4 */ ENTITE_MINUS_4 = -4,
    /* -3 */ ENTITE_MINUS_3,
    /* -2 */ ENTITE_MINUS_2,
    /* -1 */ ENTITE_MINUS_1,
    /*  0 */ ENTITE_MINUS_0
} EnTiteParam;

typedef enum EnTiteBodyPart {
    /* 0 */ ENTITE_BODYPART_0,
    /* 1 */ ENTITE_BODYPART_1,
    /* 2 */ ENTITE_BODYPART_2,
    /* 3 */ ENTITE_BODYPART_3,
    /* 4 */ ENTITE_BODYPART_4,
    /* 5 */ ENTITE_BODYPART_5,
    /* 6 */ ENTITE_BODYPART_6,
    /* 7 */ ENTITE_BODYPART_7,
    /* 8 */ ENTITE_BODYPART_8,
    /* 9 */ ENTITE_BODYPART_MAX
} EnTiteBodyPart;

typedef struct EnTite {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ Vec3s jointTable[OBJECT_TITE_LIMB_MAX];
    /* 0x21E */ Vec3s morphTable[OBJECT_TITE_LIMB_MAX];
    /* 0x2B4 */ EnTiteActionFunc actionFunc;
    /* 0x2B8 */ u8 unk_2B8;
    /* 0x2B9 */ u8 unk_2B9;
    /* 0x2BA */ s8 unk_2BA;
    /* 0x2BB */ u8 drawDmgEffType;
    /* 0x2BC */ s16 unk_2BC;
    /* 0x2BE */ s16 unk_2BE;
    /* 0x2C0 */ s32 updBgCheckInfoFlags;
    /* 0x2C4 */ f32 drawDmgEffAlpha;
    /* 0x2C8 */ f32 drawDmgEffScale;
    /* 0x2CC */ f32 drawDmgEffFrozenSteamScale;
    /* 0x2D0 */ Vec3f bodyPartsPos[ENTITE_BODYPART_MAX];
    /* 0x33C */ Vec3f unk_33C[ENTITE_BODYPART_MAX];
    /* 0x3A8 */ Gfx* unk_3A8;
    /* 0x3AC */ ColliderSphere collider;
} EnTite; // size = 0x404

#endif // Z_EN_TITE_H
