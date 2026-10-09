#ifndef Z_EN_DNS_H
#define Z_EN_DNS_H

#include "global.h"
#include "objects/object_dns/object_dns.h"

struct EnDns;

typedef void (*EnDnsActionFunc)(struct EnDns*, PlayState*);

#define ENDNS_GET_7(thisx) ((thisx)->params & 7)
#define ENDNS_GET_4000(thisx) ((thisx)->params & 0x4000)
#define ENDNS_GET_8000(thisx) ((thisx)->params & 0x8000)

typedef enum {
    /* 0 */ ENDNS_GET_7_0,
    /* 1 */ ENDNS_GET_7_1,
    /* 2 */ ENDNS_GET_7_2,
    /* 3 */ ENDNS_GET_7_3
} EnDnsParam;

typedef struct EnDns {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ EnDnsActionFunc actionFunc;
    /* 0x18C */ ColliderCylinder collider;
    /* 0x1D8 */ u8 cueId;
    /* 0x1DC */ s32 msgScriptPos;
    /* 0x1E0 */ MsgScript* msgScript;
    /* 0x1E4 */ Gfx* unk_1E4[KINGS_CHAMBER_DEKU_GUARD_LIMB_MAX];
    /* 0x218 */ Vec3f unk_218;
    /* 0x224 */ Vec3s unk_224;
    /* 0x22A */ Vec3s jointTable[KINGS_CHAMBER_DEKU_GUARD_LIMB_MAX];
    /* 0x278 */ Vec3s morphTable[KINGS_CHAMBER_DEKU_GUARD_LIMB_MAX];
    /* 0x2C6 */ u16 unk_2C6;
    /* 0x2C8 */ u16 cueType;
    /* 0x2CA */ UNK_TYPE1 unk_2CA[0x2];
    /* 0x2CC */ s16 unk_2CC;
    /* 0x2CE */ s16 unk_2CE;
    /* 0x2D0 */ s16 unk_2D0;
    /* 0x2D2 */ s16 unk_2D2;
    /* 0x2D4 */ s16 unk_2D4;
    /* 0x2D6 */ s16 unk_2D6;
    /* 0x2D8 */ s16 unk_2D8;
    /* 0x2DA */ s16 unk_2DA;
    /* 0x2DC */ s16 unk_2DC;
    /* 0x2DE */ s16 blinkTimer;
    /* 0x2E0 */ s16 eyeIndex;
    /* 0x2E4 */ f32 unk_2E4;
    /* 0x2E8 */ UNK_TYPE1 unk_2E8[0x4];
    /* 0x2EC */ f32 unk_2EC;
    /* 0x2F0 */ f32 animCurFrame;
    /* 0x2F4 */ MsgScriptCallback msgScriptCallback;
    /* 0x2F8 */ s32 animIndex;
    /* 0x2FC */ s32 unk_2FC;
} EnDns; // size = 0x300

#endif // Z_EN_DNS_H
