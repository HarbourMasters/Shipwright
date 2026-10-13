#include "global.h"

Path* Path_GetByIndex(PlayState* play, s16 index, s16 indexNone) {
    Path* path;

    if (index != indexNone) {
        path = &play->setupPathList[index];
    } else {
        path = NULL;
    }

    return path;
}

f32 Path_OrientAndGetDistSq(Actor* actor, Path* path, s16 waypoint, s16* yaw) {
    f32 dx;
    f32 dz;
    Vec3s* pointPos;

    if (path == NULL) {
        return -1.0f;
    }

    pointPos = Lib_SegmentedToVirtual(path->points);
    pointPos = &pointPos[waypoint];

    dx = pointPos->x - actor->world.pos.x;
    dz = pointPos->z - actor->world.pos.z;

    *yaw = Math_Atan2S(dx, dz);

    return SQ(dx) + SQ(dz);
}

void Path_CopyLastPoint(Path* path, Vec3f* dest) {
    Vec3s* pointPos;

    if (path != NULL) {
        pointPos = &((Vec3s*)Lib_SegmentedToVirtual(path->points))[path->count - 1];

        dest->x = pointPos->x;
        dest->y = pointPos->y;
        dest->z = pointPos->z;
    }
}
