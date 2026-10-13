#ifndef Z_OBJ_KENDO_KANBAN_H
#define Z_OBJ_KENDO_KANBAN_H

#include "global.h"

struct ObjKendoKanban;

typedef void (*ObjKendoKanbanActionFunc)(struct ObjKendoKanban*, PlayState*);

#define OBJKENDOKANBAN_GET_BOARD_FRAGMENTS(thisx) ((thisx)->params & 0xF)

typedef struct ObjKendoKanban {
    /* 0x000 */ Actor actor;
    /* 0x144 */ ColliderCylinder colliderCylinder;
    /* 0x190 */ ColliderTris colliderTris;
    /* 0x1B0 */ ColliderTrisElement colliderTrisElements[2];
    /* 0x268 */ ObjKendoKanbanActionFunc actionFunc;
    /* 0x26C */ Vec3f cornerPos[4];
    /* 0x29C */ Vec3f cornerPoints[4];
    /* 0x2CC */ Vec3f centerPoint;
    /* 0x2D8 */ Vec3f centerPos;
    /* 0x2E4 */ Vec3f rootCornerPos;
    /* 0x2F0 */ Vec3f rotAxis;
    /* 0x2FC */ s32 indexLastRootCornerPos;
    /* 0x300 */ s16 hasNewRootCornerPos;
    /* 0x302 */ s16 rotAngle;
    /* 0x304 */ s16 angularVelocity;
    /* 0x306 */ UNK_TYPE1 pad306[2];
    /* 0x308 */ s16 numBounces;
    /* 0x30A */ s16 unk_30A;
    /* 0x30C */ u8 boardFragments;
} ObjKendoKanban; // size = 0x310

#endif // Z_OBJ_KENDO_KANBAN_H
