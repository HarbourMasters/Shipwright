#ifndef Z_OBJ_MAKEKINSUTA_H
#define Z_OBJ_MAKEKINSUTA_H

#include "global.h"

struct ObjMakekinsuta;

#define OBJMAKEKINSUTA_GET_1F(thisx) (((thisx)->params >> 8) & 0x1F)
#define OBJMAKEKINSUTA_GETS_3(params) ((params & 3) & 0xFF)
// #region 2S2H [Port] The original calculation is (((params & 0x3FC) >> 2) & 0xFF), but this results in values well
// over 100. This value is ultimately used to left shift 1 to get the treasure flag. Bit shifts greater than the bit
// width are undefined behavior. On N64, this likely truncates to a value below 32. The correction below matches other
// uses of the Skulltula Token chest flag (see ENSI_GET_CHEST_FLAG and inline uses in z_tg_sw.c):
#define OBJMAKEKINSUTA_GETS_3FC(params) (((params & 0xFC) >> 2) & 0xFF)
// #endregion
#define OBJMAKEKINSUTA_GET_SWITCH_FLAG(thisx) ((thisx)->params & 0x7F)

typedef struct ObjMakekinsuta {
    /* 0x000 */ Actor actor;
    /* 0x144 */ s8 unk144;
} ObjMakekinsuta; // size = 0x148

#endif // Z_OBJ_MAKEKINSUTA_H
