#ifndef Z_EN_ELF_H
#define Z_EN_ELF_H

#include "ultra64.h"
#include "global.h"
#include "overlays/actors/ovl_Elf_Msg/z_elf_msg.h"

struct EnElf;

typedef void (*EnElfActionFunc)(struct EnElf*, PlayState*);
typedef void (*EnElfUnkFunc)(struct EnElf*, PlayState*);

typedef struct EnElf {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ Vec3s jointTable[15];
    /* 0x01EA */ Vec3s morphTable[15];
    /* 0x0244 */ Color_RGBAf innerColor;
    /* 0x0254 */ Color_RGBAf outerColor;
    /* 0x0264 */ LightInfo lightInfoGlow;
    /* 0x0274 */ LightNode* lightNodeGlow;
    /* 0x0278 */ LightInfo lightInfoNoGlow;
    /* 0x0288 */ LightNode* lightNodeNoGlow;
    /* 0x028C */ Vec3f unk_28C;
    /* 0x0298 */ ElfMsg* elfMsg;
    /* 0x029C */ f32 unk_29C;
    /* 0x02A0 */ f32 unk_2A0;
    /* 0x02A4 */ f32 unk_2A4;
    /* 0x02A8 */ s16 unk_2A8;
    /* 0x02AA */ s16 unk_2AA;
    /* 0x02AC */ s16 unk_2AC;
    /* 0x02AE */ s16 unk_2AE;
    /* 0x02B0 */ s16 unk_2B0;
    /* 0x02B4 */ f32 unk_2B4;
    /* 0x02B8 */ f32 unk_2B8;
    /* 0x02BC */ s16 unk_2BC;
    /* 0x02BE */ u16 timer;
    /* 0x02C0 */ s16 unk_2C0;
    /* 0x02C2 */ s16 disappearTimer;
    /* 0x02C4 */ u16 fairyFlags;
    /* 0x02C6 */ u8 unk_2C6;
    /* 0x02C7 */ u8 unk_2C7;
    /* 0x02C8 */ EnElfUnkFunc func_2C8;
    /* 0x02CC */ EnElfActionFunc actionFunc;
} EnElf; // size = 0x02D0

typedef enum {
    /* 0x00 */ FAIRY_NAVI,
    /* 0x01 */ FAIRY_REVIVE_BOTTLE,
    /* 0x02 */ FAIRY_HEAL_TIMED,
    /* 0x03 */ FAIRY_KOKIRI,
    /* 0x04 */ FAIRY_SPAWNER,
    /* 0x05 */ FAIRY_REVIVE_DEATH,
    /* 0x06 */ FAIRY_HEAL,
    /* 0x07 */ FAIRY_HEAL_BIG
} FairyType;

#endif
