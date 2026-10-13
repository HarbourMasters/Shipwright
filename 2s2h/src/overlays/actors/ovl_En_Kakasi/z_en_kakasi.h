#ifndef Z_EN_KAKASI_H
#define Z_EN_KAKASI_H

#ifndef __cplusplus
#include "global.h"
#endif
#include "z64snap.h"

struct EnKakasi;

typedef void (*EnKakasiActionFunc)(struct EnKakasi*, PlayState*);

typedef struct EnKakasi {
    /* 0x000 */ PictoActor picto;
    /* 0x148 */ EnKakasiActionFunc actionFunc;
    /* 0x14C */ SkelAnime skelAnime;
    /* 0x190 */ s16 unk190; // camera index for song teaching angles?
    /* 0x192 */ s16 postTeachTimer;
    /* 0x194 */ s16 aboveGroundStatus; // from params, changed to 2 in init
    /* 0x196 */ s16 unkState196;
    /* 0x19A */ UNK_TYPE1 pad198[0x8];
    /* 0x1A0 */ s32 animIndex;
    /* 0x1A4 */ s32 unkCounter1A4; // counter, counts up to F while he digs away, reused elsewhere
    /* 0x1A8 */ s32 unkState1A8;
    /* 0x1AC */ s16 talkState;
    /* 0x1AE */ s16 csIdList[3];
    /* 0x1B4 */ f32 animEndFrame;
    /* 0x1B8 */ f32 unkHeight;
    /* 0x1BC */ Vec3f unk1BC; // set by post limbdraw func for one limb
    /* 0x1C8 */ UNK_TYPE1 pad1C8[0x3C];
    /* 0x204 */ s16 unk204; // set to A, F, 0x14, 0x28 frequently
    /* 0x206 */ UNK_TYPE1 pad206[2];
    /* 0x208 */ s16 subCamId;
    /* 0x20C */ f32 subCamFov;
    /* 0x210 */ f32 subCamFovNext;
    /* 0x214 */ Vec3f subCamEye;
    /* 0x220 */ Vec3f subCamAt;
    /* 0x22C */ Vec3f unk22C;
    /* 0x238 */ Vec3f subCamEyeNext;
    /* 0x244 */ Vec3f subCamAtNext;
    /* 0x250 */ f32 songSummonDist;
    /* 0x254 */ ColliderCylinder collider;
} EnKakasi; // size = 0x2A0

#define ENKAKASI_ABOVE_GROUND_TYPE 2

#define KAKASI_GET_SUMMON_DISTANCE(thisx) (((thisx)->params >> 0x8) & 0xFF) 
#define KAKASI_GET_ABOVE_GROUND(thisx) ((thisx)->params & 0x1) 
#define KAKASI_GET_ATTENTION_RANGE_TYPE(thisx) ((thisx)->world.rot.x - 1)

// 2S2H [Enhancements] Externed for time savers
void EnKakasi_DancingNightAway(EnKakasi* thisx, PlayState* play);

#endif // Z_EN_KAKASI_H
