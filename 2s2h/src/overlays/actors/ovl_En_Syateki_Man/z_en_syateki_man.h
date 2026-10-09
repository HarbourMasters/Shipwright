#ifndef Z_EN_SYATEKI_MAN_H
#define Z_EN_SYATEKI_MAN_H

#include "global.h"
#include "objects/object_shn/object_shn.h"
#include "overlays/effects/ovl_Effect_Ss_Extra/z_eff_ss_extra.h"

struct EnSyatekiMan;

typedef void (*EnSyatekiManActionFunc)(struct EnSyatekiMan*, PlayState*);

#define SG_MAN_GET_PATH_INDEX(thisx) (((thisx)->params & 0xFF00) >> 8)

// These values are used to modify the shooting gallery man's internal score. They are
// defined in terms of the scores used by EffectSsExtra because EffectSsExtra_Update
// updates the score displayed to the player, and defining them like this will prevent
// these two different scores from becoming desynchronized. If you intend to modify these
// values, then you'll also need to make sure that EffectSsExtra_Spawn in the appropriate
// actor is called with the correct parameter for the index; failing to do this will
// cause the two different score variables to fall out of sync.
#define SG_POINTS_DEKU_NORMAL EXTRA_SCORE_30
#define SG_POINTS_DEKU_BONUS EXTRA_SCORE_100
#define SG_POINTS_GUAY EXTRA_SCORE_60
#define SG_POINTS_WOLFOS EXTRA_SCORE_100

// After getting a perfect score in the Swamp Shooting Gallery, the player is rewarded
// additional points for every second remaining on the minigame timer.
#define SG_BONUS_POINTS_PER_SECOND 10

typedef enum {
    /* 0 */ SG_GAME_STATE_NONE,             // None of the states below apply.
    /* 1 */ SG_GAME_STATE_RUNNING,          // The shooting game is in-progress.
    /* 2 */ SG_GAME_STATE_EXPLAINING_RULES, // For the Town Shooting Gallery, this state is also used for explaining the current high score.
    /* 3 */ SG_GAME_STATE_NOT_PLAYING,      // Either the player said "No" to playing, or they said "Yes" but don't have enough rupees.
    /* 4 */ SG_GAME_STATE_ONE_MORE_GAME,    // The player failed to get a new high score (Town) and/or perfect score (Swamp and Town).
    /* 5 */ SG_GAME_STATE_GIVING_BONUS,     // The player gets bonus points at the end of the Swamp game if they get a perfect score.
    /* 6 */ SG_GAME_STATE_ENDED,            // The player got a new high score and/or perfect score (Town), or the game is over (Swamp).
    /* 7 */ SG_GAME_STATE_MOVING_PLAYER     // The player is automatically moving towards the spot to play the game.
} ShootingGalleryGameState;

typedef enum {
    /*  0 */ SG_OCTO_STATE_APPEARING,    // The Octoroks will begin to appear on the next frame.
    /*  1 */ SG_OCTO_STATE_APPEARED,     // The Octoroks are appearing from underwater or have already appeared and are floating.
    /* 70 */ SG_OCTO_STATE_INITIAL = 70, // The initial state of the game. The Octoroks have not appeared yet.
    /* 80 */ SG_OCTO_STATE_HIDING = 80   // The Octoroks are in the process of hiding underwater.
} ShootingGalleryOctorokState;

typedef enum {
    /* 0 */ SG_PATH_TYPE_GUAY,
    /* 1 */ SG_PATH_TYPE_DEKU_NORMAL,
    /* 2 */ SG_PATH_TYPE_WOLFOS,
    /* 3 */ SG_PATH_TYPE_DEKU_BONUS
} ShootingGalleryPathType;

typedef struct EnSyatekiMan {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ EnSyatekiManActionFunc actionFunc;
    /* 0x18C */ Path* path;
    /* 0x190 */ s32 octorokFlags;
    /* 0x194 */ s32 swampTargetActorListIndex;
    /* 0x198 */ Vec3s jointTable[BURLY_GUY_LIMB_MAX];
    /* 0x1F8 */ Vec3s morphTable[BURLY_GUY_LIMB_MAX];
    /* 0x258 */ Vec3s headRot;
    /* 0x25E */ Vec3s torsoRot;
    /* 0x264 */ s16 eyeIndex;
    /* 0x266 */ s16 blinkTimer;
    /* 0x268 */ UNK_TYPE1 unk268[0x2];
    /* 0x26A */ s16 shootingGameState;
    /* 0x26C */ union {
                   s16 guayAppearTimer;
                   s16 octorokState;
               };
    /* 0x26E */ union {
                   s16 bonusDekuScrubHitCounter;
                   s16 lastHitOctorokType;
               };
    /* 0x270 */ s16 talkWaitTimer;
    /* 0x272 */ s16 dekuScrubFlags;
    /* 0x274 */ s16 guayFlags;
    /* 0x276 */ s16 wolfosFlags;
    /* 0x278 */ s16 dekuScrubHitCounter;
    /* 0x27A */ s16 guayHitCounter;
    /* 0x27C */ s16 currentWave;
    /* 0x27E */ s16 flagsIndex; // Used for Octoroks in Town and Guays in Swamp
    /* 0x280 */ s16 score;
    /* 0x282 */ s16 talkFlags;
    /* 0x284 */ s16 prevTextId;
} EnSyatekiMan; // size = 0x288

#endif // Z_EN_SYATEKI_MAN_H
