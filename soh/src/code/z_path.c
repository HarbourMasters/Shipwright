#include "global.h"

Path* Path_GetByIndex(PlayState* play, s16 index, s16 max) {
    Path* path;

    if (index != max) {
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
        return -1.0;
    }

    pointPos = SEGMENTED_TO_VIRTUAL(path->points);
    pointPos = &pointPos[waypoint];

    dx = pointPos->x - actor->world.pos.x;
    dz = pointPos->z - actor->world.pos.z;

    *yaw = Math_FAtan2F(dx, dz) * (32768 / M_PI);

    return SQ(dx) + SQ(dz);
}

void Path_CopyLastPoint(Path* path, Vec3f* dest) {
    Vec3s* pointPos;

    if (path != NULL) {
        pointPos = &((Vec3s*)SEGMENTED_TO_VIRTUAL(path->points))[path->count - 1];

        dest->x = pointPos->x;
        dest->y = pointPos->y;
        dest->z = pointPos->z;
    }
}
