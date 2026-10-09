#ifndef Z_EN_HATA_H
#define Z_EN_HATA_H

#include "global.h"
#include "objects/object_hata/object_hata.h"

struct EnHata;

typedef struct EnHata {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ SkelAnime skelAnime;
    /* 0x1A0 */ Vec3s jointTable[FLAGPOLE_LIMB_MAX];
    /* 0x21E */ Vec3s morphTable[FLAGPOLE_LIMB_MAX];
    /* 0x29C */ s16 unk_29C;
    /* 0x29E */ UNK_TYPE1 pad_29E[2];
    /* 0x2A0 */ s16 unk_2A0;
    /* 0x2A4 */ Vec3f unk_2A4;
} EnHata; /* size = 0x2B0 */

#endif // Z_EN_HATA_H
