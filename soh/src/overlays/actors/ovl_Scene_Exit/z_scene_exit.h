#ifndef Z_SCENE_EXIT_H
#define Z_SCENE_EXIT_H

#include "ultra64.h"
#include "global.h"

struct SceneExit;

typedef struct SceneExit {
    Actor actor;
    struct SceneExitTriangle {
        Vec3f vtx[3];
    } triangles[16];
    s16 polyCount;
    Vec3f center;
    Vec3f bbMin;
    Vec3f bbMax;
} SceneExit;

void SceneExit_AddTriangle(SceneExit* this, Vec3f triangleVerts[3]);

#endif
