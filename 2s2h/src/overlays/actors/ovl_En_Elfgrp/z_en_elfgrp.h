#ifndef Z_EN_ELFGRP_H
#define Z_EN_ELFGRP_H

#include "global.h"
#include "overlays/actors/ovl_Bg_Dy_Yoseizo/z_bg_dy_yoseizo.h"

struct EnElfgrp;

typedef void (*EnElfgrpActionFunc)(struct EnElfgrp*, PlayState*);

#define ENELFGRP_GET_TYPE(thisx) ((thisx)->params & 0xF) //!< Same type as Great Fairies
#define ENELFGRP_GET_SWITCH_FLAG_PARAMS(thisx) (((thisx)->params & 0xFE00) >> 9)
#define ENELFGRP_GET_SWITCHFLAG_ROT(thisx) ((thisx)->home.rot.z)

typedef enum ElfgrpType {
    /* 0 */ ENELFGRP_TYPE_MAGIC = GREAT_FAIRY_TYPE_MAGIC,
    /* 1 */ ENELFGRP_TYPE_POWER = GREAT_FAIRY_TYPE_POWER,
    /* 2 */ ENELFGRP_TYPE_WISDOM = GREAT_FAIRY_TYPE_WISDOM,
    /* 3 */ ENELFGRP_TYPE_COURAGE = GREAT_FAIRY_TYPE_COURAGE,
    /* 4 */ ENELFGRP_TYPE_KINDNESS = GREAT_FAIRY_TYPE_KINDNESS,
    /* 5 */ ENELFGRP_TYPE_MAX
} ElfgrpType;

typedef struct EnElfgrp {
    /* 0x000 */ Actor actor;
    /* 0x144 */ s16 timer;
    /* 0x146 */ u8 talkedOnceFlag;
    /* 0x147 */ u8 type;
    /* 0x148 */ s8 unk_148; // set and not used
    /* 0x14A */ u16 stateFlags;
    /* 0x14C */ EnElfgrpActionFunc actionFunc;
} EnElfgrp; // size = 0x150

#endif // Z_EN_ELFGRP_H
