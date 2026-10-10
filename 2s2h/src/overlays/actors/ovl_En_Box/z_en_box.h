#ifndef Z_EN_BOX_H
#define Z_EN_BOX_H

#include "global.h"
#include "objects/object_box/object_box.h"

struct EnBox;
struct struct_80867BDC_a0;

typedef void (*EnBoxActionFunc)(struct EnBox*, PlayState*);
typedef void (*EnBoxUnkFunc)(struct struct_80867BDC_a0* arg0, PlayState* play);

typedef struct struct_80867BDC_a0 {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ EnBoxUnkFunc unk_0C;
    /* 0x10 */ EnBoxUnkFunc unk_10;
    /* 0x14 */ s32 unk_14;
    /* 0x18 */ s32 unk_18;
    /* 0x1C */ s32 unk_1C;
    /* 0x20 */ s32 unk_20;
} struct_80867BDC_a0; // size 0x24

typedef enum {
    /*  0 */ ENBOX_TYPE_BIG,                    // big
    /*  1 */ ENBOX_TYPE_BIG_ROOM_CLEAR,         // appear on room clear, store temp clear as permanent clear
    /*  2 */ ENBOX_TYPE_BIG_ORNATE,             // boss key chest
    /*  3 */ ENBOX_TYPE_BIG_SWITCH_FLAG_FALL,   // falling, appear on switch flag set
    /*  4 */ ENBOX_TYPE_BIG_INVISIBLE,          // big, revealed with lens of truth or when opened
    /*  5 */ ENBOX_TYPE_SMALL,                  // small
    /*  6 */ ENBOX_TYPE_SMALL_INVISIBLE,        // small, revealed with lens of truth or when opened
    /*  7 */ ENBOX_TYPE_SMALL_ROOM_CLEAR,       // use room clear, store temp clear as perm clear
    /*  8 */ ENBOX_TYPE_SMALL_SWITCH_FLAG_FALL, // falling, appear on switch flag set
    /*  9 */ ENBOX_TYPE_BIG_SONG_ZELDAS_LULLABY,// NOT IMPLEMENTED, behaves like big chest
    /* 10 */ ENBOX_TYPE_BIG_SONG_SUNS,          // NOT IMPLEMENTED, behaves like big chest
    /* 11 */ ENBOX_TYPE_BIG_SWITCH_FLAG,        // big, appear on switch flag set
    /* 12 */ ENBOX_TYPE_SMALL_SWITCH_FLAG       // small, appear on switch flag set
} EnBoxType;

#define ENBOX_GET_TYPE(thisx) (((thisx)->params >> 12) & 0xF)
#define ENBOX_GET_ITEM(thisx) (((thisx)->params >> 5) & 0x7F)
#define ENBOX_GET_CHEST_FLAG(thisx) ((thisx)->params & 0x1F)
#define ENBOX_GET_SWITCH_FLAG(thisx) ((thisx)->world.rot.z)
// Codegen in EnTorch_Init() requires leaving out the & 0x7F for `item`
#define ENBOX_PARAMS(type, item, chestFlag) ((((type) & 0xF) << 12) | ((item) << 5) | ((chestFlag) & 0x1F))

typedef struct EnBox {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ SkelAnime skelAnime;
    /* 0x1A0 */ s32 unk_1A0;
    /* 0x1A4 */ UNK_TYPE1 unk_1A4[0x04];
    /* 0x1A8 */ f32 unk_1A8;
    /* 0x1AC */ EnBoxActionFunc actionFunc;
    /* 0x1B0 */ Vec3s jointTable[OBJECT_BOX_CHEST_LIMB_MAX];
    /* 0x1CE */ Vec3s morphTable[OBJECT_BOX_CHEST_LIMB_MAX];
    /* 0x1EC */ s16 unk_1EC;
    /* 0x1EE */ u8 movementFlags;
    /* 0x1EF */ u8 alpha;
    /* 0x1F0 */ u8 switchFlag;
    /* 0x1F1 */ u8 type;
    /* 0x1F2 */ u8 iceSmokeTimer;
    /* 0x1F3 */ s8 unk_1F3;
    /* 0x1F4 */ struct_80867BDC_a0 unk_1F4;
    /* 0x218 */ s16 csId1;
    /* 0x21A */ s16 csId2;
    /* 0x21C */ s32 getItemId;
    /* 0x220 */ s32 collectableFlag;
} EnBox; // size = 0x224


#endif // Z_EN_BOX_H
