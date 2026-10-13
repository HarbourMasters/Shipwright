#ifndef Z_EN_BIGPO_H
#define Z_EN_BIGPO_H

#include "global.h"
#include "objects/object_bigpo/object_bigpo.h"

struct EnBigpo;

typedef void (*EnBigPoActionFunc)(struct EnBigpo*, PlayState*);

#define BIG_POE_GET_SWITCH_FLAG(thisx) (u8)((thisx)->params >> 0x8)

#define BIG_POE_SWITCH_FLAG_NONE 0xFF

typedef enum BigPoeBodyPart {
    /* 0 */ BIG_POE_BODYPART_0,
    /* 1 */ BIG_POE_BODYPART_1,
    /* 2 */ BIG_POE_BODYPART_2,
    /* 3 */ BIG_POE_BODYPART_3,
    /* 4 */ BIG_POE_BODYPART_4,
    /* 5 */ BIG_POE_BODYPART_5,
    /* 6 */ BIG_POE_BODYPART_6,
    /* 7 */ BIG_POE_BODYPART_7,
    /* 8 */ BIG_POE_BODYPART_8,
    /* 9 */ BIG_POE_BODYPART_MAX
} BigPoeBodyPart;

typedef struct EnBigpoFireEffect {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ LightNode* light;
    /* 0x10 */ LightInfo info; // size 0xE
} EnBigpoFireEffect; // size = 0x20

// idleTimer gets reused:
// * after dampe reveals a fire, 8 minutes of frames before it goes away again
// * used by flames and regular bigpo to count frames during the appearance cutscene
// * when idle flying around, frames until next attack
// * when spinning in/out of reality, counts frames from start
// * in burning death, counts frames from death start
// * after scoop spawned, idle timer used to count down to actor disapear

typedef struct EnBigpo {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ Vec3s jointTable[BIG_POE_LIMB_MAX];
    /* 0x1C4 */ Vec3s morphTable[BIG_POE_LIMB_MAX];
    /* 0x200 */ EnBigPoActionFunc actionFunc;
    /* 0x204 */ u8 storePrevBgm;
    /* 0x206 */ s16 idleTimer; // frame counter
    /* 0x208 */ s16 unk208; // facing rotY?
    /* 0x20A */ s16 angularVelocity;
    /* 0x20C */ s16 unk20C; // is this counting the number of frames the player is ztargeting them?
    /* 0x20E */ s16 subCamId;
    /* 0x210 */ s16 switchFlag;
    /* 0x212 */ s16 hoverHeightCycleTimer; // sin wave up and down bobbing
    /* 0x214 */ f32 fireRadius; // distance from center during conjunction cutscene
    /* 0x218 */ f32 savedHeight; // actual height while flying moves as part of bobbing
    /* 0x21C */ f32 drawDmgEffAlpha;
    /* 0x220 */ f32 drawDmgEffScale;
    /* 0x224 */ Vec3f bodyPartsPos[BIG_POE_BODYPART_MAX];
    /* 0x290 */ Color_RGBA8 mainColor;
    /* 0x294 */ Color_RGBA8 lanternColor;
    /* 0x298 */ UNK_TYPE1 pad298[0x14];
    /* 0x2AC */ ColliderCylinder collider;
    /* 0x2F8 */ MtxF drawMtxF;
    // the three fires that merge to become big po
    //   also the fires dampe digs up under his house
    /* 0x338 */ EnBigpoFireEffect fires[3];
} EnBigpo; // size = 0x398

// well version is "regular" (spawns automatically), dampe basement version is "summoned"
// on room enter, 3 "possiblefire" are turned into "chosenfire" at random
typedef enum EnBigpoType {
  /* 0 */ BIG_POE_TYPE_REGULAR,
  /* 1 */ BIG_POE_TYPE_SUMMONED,
  /* 2 */ BIG_POE_TYPE_POSSIBLE_FIRE,
  /* 3 */ BIG_POE_TYPE_CHOSEN_FIRE,
  /* 4 */ BIG_POE_TYPE_REVEALED_FIRE,
  /* 5 */ BIG_POE_TYPE_UNK5
} EnBigpoType;

#endif // Z_EN_BIGPO_H
