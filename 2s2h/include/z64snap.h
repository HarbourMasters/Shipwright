#ifndef Z64SNAP_H
#define Z64SNAP_H

#include "PR/ultratypes.h"
#include "z64actor.h"

typedef s32 (*PictoValidationFunc)(struct PlayState*, Actor*);

typedef struct {
    /* 0x000 */ Actor actor;
    /* 0x144 */ PictoValidationFunc validationFunc;
} PictoActor;

// Picto validation flags

typedef enum {
    // Used externally, set and read by actors
    /* 0x00 */ PICTO_VALID_0,
    /* 0x01 */ PICTO_VALID_IN_SWAMP,
    /* 0x02 */ PICTO_VALID_MONKEY,
    /* 0x03 */ PICTO_VALID_BIG_OCTO,
    /* 0x04 */ PICTO_VALID_LULU_HEAD,
    /* 0x05 */ PICTO_VALID_LULU_RIGHT_ARM,
    /* 0x06 */ PICTO_VALID_LULU_LEFT_ARM, // Need all three of these to qualify as a good picture
    /* 0x07 */ PICTO_VALID_SCARECROW,
    /* 0x08 */ PICTO_VALID_TINGLE,
    /* 0x09 */ PICTO_VALID_PIRATE_GOOD,
    /* 0x0A */ PICTO_VALID_DEKU_KING,
    /* 0x0B */ PICTO_VALID_PIRATE_TOO_FAR, // overlaps with PICTO_VALID_PIRATE_GOOD, but that is checked first

    // Used internally, test for failures of position, angle etc.
    /* 0x3B */ PICTO_VALID_BEHIND_COLLISION = 0x3B,
    /* 0x3C */ PICTO_VALID_BEHIND_BG,
    /* 0x3D */ PICTO_VALID_NOT_IN_VIEW,
    /* 0x3E */ PICTO_VALID_BAD_ANGLE,
    /* 0x3F */ PICTO_VALID_BAD_DISTANCE
} PictoValidFlag;

// The subregion of the picto photo that will set the flag for an actor being in the photo
#define PICTO_VALID_WIDTH 150
#define PICTO_VALID_HEIGHT 105
#define PICTO_VALID_TOPLEFT_X ((SCREEN_WIDTH - PICTO_VALID_WIDTH) / 2)
#define PICTO_VALID_TOPLEFT_Y ((SCREEN_HEIGHT - PICTO_VALID_HEIGHT) / 2)

s32 Snap_RecordPictographedActors(PlayState* play);
void Snap_SetFlag(s32 flag);
void Snap_UnsetFlag(s32 flag);
u32 Snap_CheckFlag(s32 flag);
s16 Snap_AbsS(s16 val);
s32 Snap_ValidatePictograph(PlayState* play, Actor* actor, s32 flag, Vec3f* pos, Vec3s* rot, f32 distanceMin, f32 distanceMax, s16 angleRange);

#endif
