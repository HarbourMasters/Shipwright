#ifndef Z_DOOR_WARP1_H
#define Z_DOOR_WARP1_H

#include "global.h"
#include "overlays/actors/ovl_Dm_Hina/z_dm_hina.h"

struct DoorWarp1;

typedef void (*DoorWarp1ActionFunc)(struct DoorWarp1*, PlayState*);

#define DOORWARP1_GET_FF(thisx) ((thisx)->params & 0xFF)
#define DOORWARP1_GET_FF00_1(thisx) (((thisx)->params >> 8) & 0xFF)
#define DOORWARP1_GET_FF00_2(thisx) ((thisx)->params & 0xFF00)
#define DOORWARP1_GET_FF00_3(thisx) ((thisx)->params >> 8)

typedef enum {
    /* 0 */ ENDOORWARP1_FF_0,
    /* 1 */ ENDOORWARP1_FF_1,
    /* 2 */ ENDOORWARP1_FF_2,
    /* 3 */ ENDOORWARP1_FF_3,
    /* 4 */ ENDOORWARP1_FF_4,
    /* 5 */ ENDOORWARP1_FF_5,
    /* 6 */ ENDOORWARP1_FF_6
} DoorWarp1Param;

typedef struct DoorWarp1 {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ SkelAnime skelAnime;
    /* 0x1A0 */ DmHina* unk_1A0;
    /* 0x1A4 */ f32 unk_1A4;
    /* 0x1A8 */ f32 unk_1A8;
    /* 0x1AC */ f32 unk_1AC;
    /* 0x1B0 */ f32 unk_1B0;
    /* 0x1B4 */ f32 unk_1B4;
    /* 0x1B8 */ f32 unk_1B8;
    /* 0x1BC */ f32 unk_1BC;
    /* 0x1C0 */ f32 unk_1C0;
    /* 0x1C4 */ s16 unk_1C4;
    /* 0x1C6 */ s16 unk_1C6;
    /* 0x1C8 */ s16 unk_1C8;
    /* 0x1CA */ s16 unk_1CA;
    /* 0x1CC */ s16 unk_1CC;
    /* 0x1CE */ s16 unk_1CE;
    /* 0x1D0 */ u16 unk_1D0;
    /* 0x1D2 */ s8 unk_1D2;
    /* 0x1D3 */ u8 unk_1D3;
    /* 0x1D4 */ u8 unk_1D4;
    /* 0x1D8 */ DoorWarp1ActionFunc actionFunc;
    /* 0x1DC */ LightNode* unk_1DC;
    /* 0x1E0 */ LightInfo unk_1E0;
    /* 0x1F0 */ LightNode* unk_1F0;
    /* 0x1F4 */ LightInfo unk_1F4;
    /* 0x202 */ u8 unk_202;
    /* 0x203 */ u8 unk_203;
    /* 0x204 */ f32 unk_204;
    /* 0x208 */ u8 cueId;
} DoorWarp1; // size = 0x20C

#endif // Z_DOOR_WARP1_H
