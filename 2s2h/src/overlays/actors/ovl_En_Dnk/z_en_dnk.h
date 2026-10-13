#ifndef Z_EN_DNK_H
#define Z_EN_DNK_H

#include "global.h"
#include "objects/object_dnk/object_dnk.h"
#include "objects/object_hintnuts/object_hintnuts.h"
#include "objects/object_dekunuts/object_dekunuts.h"

struct EnDnk;

typedef void (*EnDnkActionFunc)(struct EnDnk*, PlayState*);

#define ENDNK_GET_3(thisx) ((thisx)->params & 0x3)
#define ENDNK_GET_3C(thisx) ((thisx)->params & 0x3C)

typedef enum {
    /* 0 */ ENDNK_GET_3_0,
    /* 1 */ ENDNK_GET_3_1,
    /* 2 */ ENDNK_GET_3_2,
    /* 3 */ ENDNK_GET_3_3,
    /* 4 */ ENDNK_GET_3_4
} EnDnkParam;

#define DNK_LIMB_MAX MAX(MAX((s32)DEKU_PALACE_GUARD_LIMB_MAX, (s32)OBJECT_HINTNUTS_LIMB_MAX), (s32)DEKU_SCRUB_LIMB_MAX)

typedef struct EnDnk {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ EnDnkActionFunc actionFunc;
    /* 0x18C */ UNK_TYPE1 unk_18C[0x4];
    /* 0x190 */ ColliderCylinder collider;
    /* 0x1DC */ Vec3s jointTable[DNK_LIMB_MAX];
    /* 0x21E */ Vec3s morphTable[DNK_LIMB_MAX];
    /* 0x260 */ Gfx* unk_260[DEKU_PALACE_GUARD_LIMB_MAX];
    /* 0x28C */ u16 unk_28C;
    /* 0x28E */ s8 objectSlot;
    /* 0x290 */ s16 unk_290;
    /* 0x292 */ s16 unk_292;
    /* 0x294 */ s16 unk_294;
    /* 0x296 */ s16 unk_296;
    /* 0x298 */ s16 unk_298;
    /* 0x29A */ UNK_TYPE1 unk_29A[0x4];
    /* 0x29E */ s16 unk_29E;
    /* 0x2A0 */ s16 unk_2A0;
    /* 0x2A2 */ s16 unk_2A2;
} EnDnk; // size = 0x2A4

#endif // Z_EN_DNK_H
