#ifndef Z_EN_HOLL_H
#define Z_EN_HOLL_H

#include "global.h"

struct EnHoll;

typedef void (*EnHollActionFunc)(struct EnHoll*, PlayState*);

typedef enum {
    /* 0 */ EN_HOLL_TYPE_DEFAULT,
    /* 1 */ EN_HOLL_TYPE_VERTICAL,
    /* 2 */ EN_HOLL_TYPE_TRANSPARENT,
    /* 3 */ EN_HOLL_TYPE_VERTICAL_BG_COVER,
    /* 4 */ EN_HOLL_TYPE_SCENE_CHANGER
} EnHollTypes;

typedef enum {
    /* 0 */ EN_HOLL_BEHIND,
    /* 1 */ EN_HOLL_BEFORE
} EnHollSides;

typedef enum {
    /* 0 */ EN_HOLL_ABOVE,
    /* 1 */ EN_HOLL_BELOW
} EnHollSidesVertical;

typedef struct EnHoll {
    /* 0x000 */ Actor actor;
    /* 0x144 */ s8 type;
    /* 0x145 */ u8 alpha;
    /* 0x146 */ u8 playerSide;
    /* 0x147 */ u8 bgCoverAlphaActive;
    /* 0x148 */ EnHollActionFunc actionFunc;
} EnHoll; // size = 0x14C

// Horizontal
#define EN_HOLL_BOTTOM_DEFAULT -50.0f
#define EN_HOLL_BOTTOM_IKANA -90.0f
#define EN_HOLL_HALFWIDTH_DEFAULT 150.0f 
#define EN_HOLL_HALFWIDTH_IKANA 280.0f 
#define EN_HOLL_HALFWIDTH_TRANSPARENT 200.0f
#define EN_HOLL_TOP_DEFAULT 200.0f
#define EN_HOLL_TOP_PIRATE 280.0f 

//! Transparent Halls: Being inside this plane enables the execution of more code within the actionFunc.
#define EN_HOLL_ACTIVATION_PLANE_DISTANCE 100.0f

//! Transparent Halls: Crossing this plane will change rooms/scenes.
#define EN_HOLL_LOADING_PLANE_DISTANCE 50.0f


// Vertical
#define EN_HOLL_RADIUS 120.0f

//! Holes: Being inside this plane enables the execution of more code within the actionFunc.
#define EN_HOLL_ACTIVATION_PLANE_DISTANCE_VERTICAL 200.0f

//! Holes: Crossing this plane will change rooms/scenes.
#define EN_HOLL_LOADING_PLANE_DISTANCE_VERTICAL 50.0f


#define EN_HOLL_GET_ID(thisx) ((u16)(thisx)->params >> 10)
#define EN_HOLL_GET_TYPE(thisx) (((thisx)->params >> 7) & 0x7)
#define EN_HOLL_GET_EXIT_LIST_INDEX(thisx) ((thisx)->params & 0x7F)
#define EN_HOLL_GET_Z_ACTOR_BITMASK_INDEX(thisx) ((thisx)->params & 0x7)

#define EN_HOLL_IS_VISIBLE(this) ((this->type == EN_HOLL_TYPE_DEFAULT) || (this->type == EN_HOLL_TYPE_SCENE_CHANGER))
#define EN_HOLL_IS_SCENE_CHANGER(this) (this->type == EN_HOLL_TYPE_SCENE_CHANGER)

#define EN_HOLL_SCALE_ALPHA(playerDistFromCentralPlane) ((playerDistFromCentralPlane - sTransparencyPlaneDistance) * (255 / (sTranslucencyPlaneDistance - sTransparencyPlaneDistance)))
#define EN_HOLL_SCALE_BG_COVER_ALPHA(playerDistFromCentralPlane) ((EN_HOLL_ACTIVATION_PLANE_DISTANCE_VERTICAL - playerDistFromCentralPlane) * (255 / (EN_HOLL_ACTIVATION_PLANE_DISTANCE_VERTICAL - EN_HOLL_LOADING_PLANE_DISTANCE_VERTICAL)))

#endif // Z_EN_HOLL_H
