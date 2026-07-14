#ifndef Z_EN_HOLL_H
#define Z_EN_HOLL_H

#include <libultraship/libultra.h>
#include "global.h"

#define ENHOLL_GET_TYPE(thisx)          PARAMS_GET_U((thisx)->params, 6, 3)
#define ENHOLL_GET_SWITCH_FLAG(thisx)   PARAMS_GET_U((thisx)->params, 0, 6)

/**
 * Two kinds of holls:
 *   Horizontal (H):
 *     For the player to move horizontally between rooms.
 *     Cuboid-shaped:
 *      - height: 200 units high plus 50 units into the ground
 *      - width: 400 units wide or, if narrow, 200 units wide
 *   Vertical (V):
 *     For the player to move vertically between rooms.
 *     Cylinder-shaped, radius of 120 units.
 *
 * Three kinds of visual effects:
 *   Visible: A plane is drawn at the holl location, and fades away as the player approaches.
 *   Bg Cover: The background geometry (e.g. room geometry but not actors) fades black as the player approaches.
 *   Invisible: Approaching the holl does not cause any particular visual effect.
 */
typedef enum EnHollType {
    /* 0 */ ENHOLL_H_VISIBLE_NARROW,
    /* 1 */ ENHOLL_V_DOWN_BGCOVER_LARGE, // Only allows downwards transitions. 500 units radius. Used in fire temple drops.
    /* 2 */ ENHOLL_V_INVISIBLE,
    /* 3 */ ENHOLL_H_BGCOVER_SWITCHFLAG, // Enabled if a switch flag is set. Used in Dampe's race.
    /* 4 */ ENHOLL_H_INVISIBLE,
    /* 5 */ ENHOLL_V_BGCOVER,
    /* 6 */ ENHOLL_H_INVISIBLE_NARROW
} EnHollType;

struct EnHoll;

typedef void (*EnHollActionFunc)(struct EnHoll*, PlayState*);

typedef struct EnHoll {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ s16 planeAlpha;
    /* 0x014E */ u8 side;
    /* 0x014F */ u8 resetBgCoverAlpha;
    /* 0x0150 */ EnHollActionFunc actionFunc;
} EnHoll; // size = 0x0154

#endif
