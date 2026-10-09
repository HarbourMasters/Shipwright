#ifndef Z_EN_MAG_H
#define Z_EN_MAG_H

#include "global.h"

struct EnMag;

typedef struct EnMag {
    /* 0x00000 */ Actor actor;
    /* 0x00144 */ UNK_TYPE1 unk144[0x2C];
    /* 0x00170 */ Font font;
    /* 0x11F00 */ s16 unk11F00; // Set and not used.
    /* 0x11F02 */ s16 unk11F02; // Set and not used.
    /* 0x11F04 */ s16 state; // State of whole actor, uses EnMagState enum
    /* 0x11F06 */ s16 appearEffectPrimLodFrac;
    /* 0x11F08 */ s16 appearEffectPrimColor[3];
    /* 0x11F08 */ s16 appearEffectEnvColor[3];
    /* 0x11F14 */ s16 appearEffectAlpha;
    /* 0x11F16 */ s16 displayEffectPrimLodFrac;
    /* 0x11F18 */ s16 displayEffectPrimColor[3];
    /* 0x11F18 */ s16 displayEffectEnvColor[3];
    /* 0x11F24 */ s16 displayEffectAlpha;
    /* 0x11F26 */ s16 majorasMaskAlpha;
    /* 0x11F28 */ s16 majorasMaskEnvColor[3];
    /* 0x11F2E */ s16 mainTitleAlpha;
    /* 0x11F30 */ s16 subtitleAlpha;
    /* 0x11F32 */ s16 unk11F32; // Set but not used, likely a spare alpha.
    /* 0x11F34 */ s16 copyrightAlpha;
    /* 0x11F36 */ s16 effectScrollSs[6];
    /* 0x11F42 */ UNK_TYPE1 unk11F42[2];
    /* 0x11F44 */ s16 effectScrollTs[6];
    /* 0x11F50 */ UNK_TYPE1 unk11F50[4];
    /* 0x11F54 */ s16 unk11F54; // Set but not used.
    /* 0x11F56 */ s16 unk11F56; // Set but not used.
    /* 0x11F58 */ s16 unk11F58; // Set but not used.
    /* 0x11F5A */ s16 unk11F5A; // Set but not used.
    /* 0x11F5C */ s32 unk11F5C; // Set but not used.
    /* 0x11F60 */ s32 unk11F60; // Set but not used.
    /* 0x11F64 */ s16 majorasMaskEffectsFadeInTimer;
    /* 0x11F66 */ s16 majorasMaskFadeInTimer;
    /* 0x11F68 */ s16 mainTitleAlphaFadeInTimer;
    /* 0x11F6A */ s16 effectAlphaFadeInTimer;
    /* 0x11F6C */ s16 subtitleFadeInTimer;
    /* 0x11F6E */ s16 copyrightFadeInTimer;
    /* 0x11F70 */ s16 fadeOutTimer;
    /* 0x11F72 */ s16 delayTimer; // Delays start of next action in Update.
    /* 0x11F74 */ UNK_TYPE1 unk11F74[4];
} EnMag; // size = 0x11F78

#endif // Z_EN_MAG_H
