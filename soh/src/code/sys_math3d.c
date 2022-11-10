#include "global.h"
#include "vt.h"

s32 Math3D_LineVsLineClosestTwoPoints(Vec3f* lineAPointA, Vec3f* lineAPointB, Vec3f* lineBPointA, Vec3f* lineBPointB,
                                      Vec3f* lineAClosestToB, Vec3f* lineBClosestToA);
s32 Math3D_TriLineIntersect(Vec3f* v0, Vec3f* v1, Vec3f* v2, f32 nx, f32 ny, f32 nz, f32 originDist, Vec3f* linePointA,
                            Vec3f* linePointB, Vec3f* intersect, s32 fromFront);
s32 Math3D_PlaneVsPlaneNewLine(f32 planeAA, f32 planeAB, f32 planeAC, f32 planeADist, f32 planeBA, f32 planeBB,
                               f32 planeBC, f32 planeBDist, InfiniteLine* intersect);
s32 Math3D_CirSquareVsTriSquare(f32 x0, f32 y0, f32 x1, f32 y1, f32 x2, f32 y2, f32 centerX, f32 centerY, f32 radius);
s32 Math3D_SphCubeVsTriCube(Vec3f* v0, Vec3f* v1, Vec3f* v2, Vec3f* center, f32 radius);

/**
 * Creates an infinite line along the intersection of the plane defined from `planeAA`x + `planeAB`y + `planeAB`z +
 * `planeADist` = 0 and `planeBA`x + `planeBB`y + `planeBC`z + `planeBDist` = 0, and finds the closest point on that
 * intersection to the line segment `linePointA and linePointB`, outputs the intersection to `closestPoint`
 */
s32 Math3D_PlaneVsLineSegClosestPoint(f32 planeAA, f32 planeAB, f32 planeAC, f32 planeADist, f32 planeBA, f32 planeBB,
                                      f32 planeBC, f32 planeBDist, Vec3f* linePointA, Vec3f* linePointB,
                                      Vec3f* closestPoint) {
    static InfiniteLine planeIntersectLine;
    static Linef planeIntersectSeg;

    Vec3f sp34; // unused

    if (!Math3D_PlaneVsPlaneNewLine(planeAA, planeAB, planeAC, planeADist, planeBA, planeBB, planeBC, planeBDist,
                                    &planeIntersectLine)) {
        // The planes are parallel
        return false;
    }

    // create a line segment on the plane.
    Math_Vec3f_Copy(&planeIntersectSeg.a, &planeIntersectLine.point);
    planeIntersectSeg.b.x = (planeIntersectLine.dir.x * 100.0f) + planeIntersectLine.point.x;
    planeIntersectSeg.b.y = (planeIntersectLine.dir.y * 100.0f) + planeIntersectLine.point.y;
    planeIntersectSeg.b.z = (planeIntersectLine.dir.z * 100.0f) + planeIntersectLine.point.z;

    // closestPoint is a point on planeIntersect, sp34 is a point on linePointA, linePointB
    if (!Math3D_LineVsLineClosestTwoPoints(&planeIntersectSeg.a, &planeIntersectSeg.b, linePointA, linePointB,
                                           closestPoint, &sp34)) {
        return false;
    }
    return true;
}

/**
 * Finds the two points on lines A and B where the lines are closest together.
 */
s32 Math3D_LineVsLineClosestTwoPoints(Vec3f* lineAPointA, Vec3f* lineAPointB, Vec3f* lineBPointA, Vec3f* lineBPointB,
                                      Vec3f* lineAClosestToB, Vec3f* lineBClosestToA) {
    f32 sqMag;
    f32 scaleB;
    f32 lineAx;
    f32 lineAy;
    f32 lineAz;
    f32 lineBx;
    f32 lineBy;
    f32 lineBz;
    f32 compAAlongB;
    f32 compBAAlongB;
    Vec3f lineAPerpB;
    Vec3f lineBAPerpB;
    f32 tA;
    f32 tB;

    lineAx = lineAPointB->x - lineAPointA->x;
    lineAy = lineAPointB->y - lineAPointA->y;
    lineAz = lineAPointB->z - lineAPointA->z;

    lineBx = lineBPointB->x - lineBPointA->x;
    lineBy = lineBPointB->y - lineBPointA->y;
    lineBz = lineBPointB->z - lineBPointA->z;

    sqMag = SQ(lineBx) + SQ(lineBy) + SQ(lineBz);
    if (IS_ZERO(sqMag)) {
        return false;
    }

    scaleB = 1.0f / sqMag;

    compAAlongB = ((lineAx * lineBx) + (lineAy * lineBy) + (lineAz * lineBz)) * scaleB;

    compBAAlongB = ((lineBx * (lineAPointA->x - lineBPointA->x)) + (lineBy * (lineAPointA->y - lineBPointA->y)) +
                    (lineBz * (lineAPointA->z - lineBPointA->z))) *
                   scaleB;

    lineAPerpB.x = lineAx - (lineBx * compAAlongB);
    lineAPerpB.y = lineAy - (lineBy * compAAlongB);
    lineAPerpB.z = lineAz - (lineBz * compAAlongB);

    sqMag = SQXYZ(lineAPerpB);
    if (IS_ZERO(sqMag)) {
        return false;
    }

    lineBAPerpB.x = (lineAPointA->x - lineBPointA->x) - (lineBx * compBAAlongB);
    lineBAPerpB.y = (lineAPointA->y - lineBPointA->y) - (lineBy * compBAAlongB);
    lineBAPerpB.z = (lineAPointA->z - lineBPointA->z) - (lineBz * compBAAlongB);

    tA = -DOTXYZ(lineAPerpB, lineBAPerpB) / sqMag;
    lineAClosestToB->x = (lineAx * tA) + lineAPointA->x;
    lineAClosestToB->y = (lineAy * tA) + lineAPointA->y;
    lineAClosestToB->z = (lineAz * tA) + lineAPointA->z;

    tB = (compAAlongB * tA) + compBAAlongB;
    lineBClosestToA->x = (lineBx * tB) + lineBPointA->x;
    lineBClosestToA->y = (lineBy * tB) + lineBPointA->y;
    lineBClosestToA->z = (lineBz * tB) + lineBPointA->z;

    return true;
}

/**
 * Determines the closest point on the line `line` to `pos`, by forming a line perpendicular from
 * `point` to `line` closest point is placed in `closestPoint`
 */
void Math3D_LineClosestToPoint(Linef* line, Vec3f* pos, Vec3f* closestPoint) {
    f32 dirVectorSize;
    f32 t;

    dirVectorSize = Math3D_Vec3fMagnitudeSq(&line->b);
    if (IS_ZERO(dirVectorSize)) {
        osSyncPrintf(VT_COL(YELLOW, BLACK));
        // "Math3D_lineVsPosSuisenCross(): No straight line length"
        osSyncPrintf("Math3D_lineVsPosSuisenCross():直線の長さがありません\n");
        osSyncPrintf("cross = pos を返します。\n"); // "Returns cross = pos."
        osSyncPrintf(VT_RST);
        Math_Vec3f_Copy(closestPoint, pos);
    }

    t = (((pos->x - line->a.x) * line->b.x) + ((pos->y - line->a.y) * line->b.y) + ((pos->z - line->a.z) * line->b.z)) /
        dirVectorSize;
    closestPoint->x = (line->b.x * t) + line->a.x;
    closestPoint->y = (line->b.y * t) + line->a.y;
    closestPoint->z = (line->b.z * t) + line->a.z;
}

void Math3D_FindPointOnPlaneIntersect(f32 planeAAxis1Norm, f32 planeAAxis2Norm, f32 planeBAxis1Norm,
                                      f32 planeBAxis2Norm, f32 axis3Direction, f32 planeADist, f32 planeBDist,
                                      f32* axis1Point, f32* axis2Point) {
    *axis1Point = ((planeAAxis2Norm * planeBDist) - (planeBAxis2Norm * planeADist)) / axis3Direction;
    *axis2Point = ((planeBAxis1Norm * planeADist) - (planeAAxis1Norm * planeBDist)) / axis3Direction;
}

/**
 * Creates a line between the intersections of two planes defined from `planeAA`x + `planeAB`y + `planeAC`z +
 * `planeADist` = 0 and `planeBA`x + `planeBB`y + `planeBC`z + `planeBDist` = 0, and outputs the line to `intersect`.
 * Returns false if the planes are parallel.
 */
s32 Math3D_PlaneVsPlaneNewLine(f32 planeAA, f32 planeAB, f32 planeAC, f32 planeADist, f32 planeBA, f32 planeBB,
                               f32 planeBC, f32 planeBDist, InfiniteLine* intersect) {
    char pad[4];
    Vec3f planeANormal;
    Vec3f planeBNormal;
    f32 dirX;
    f32 dirY;
    f32 dirZ;

    VEC_SET(planeANormal, planeAA, planeAB, planeAC);
    VEC_SET(planeBNormal, planeBA, planeBB, planeBC);

    Math3D_Vec3f_Cross(&planeANormal, &planeBNormal, &intersect->dir);

    if (IS_ZERO(intersect->dir.x) && IS_ZERO(intersect->dir.y) && IS_ZERO(intersect->dir.z)) {
        // planes are parallel
        return false;
    }

    dirX = fabsf(intersect->dir.x);
    dirY = fabsf(intersect->dir.y);
    dirZ = fabsf(intersect->dir.z);

    if ((dirX >= dirY) && (dirX >= dirZ)) {
        Math3D_FindPointOnPlaneIntersect(planeAB, planeAC, planeBB, planeBC, intersect->dir.x, planeADist, planeBDist,
                                         &intersect->point.y, &intersect->point.z);
        intersect->point.x = 0.0f;
    } else if ((dirY >= dirX) && (dirY >= dirZ)) {
        Math3D_FindPointOnPlaneIntersect(planeAC, planeAA, planeBC, planeBA, intersect->dir.y, planeADist, planeBDist,
                                         &intersect->point.z, &intersect->point.x);
        intersect->point.y = 0.0f;
    } else {
        Math3D_FindPointOnPlaneIntersect(planeAA, planeAB, planeBA, planeBB, intersect->dir.z, planeADist, planeBDist,
                                         &intersect->point.x, &intersect->point.y);
        intersect->point.z = 0.0f;
    }
    return true;
}

/**
 * Gets the closest point on the line formed from the intersection of of the planes defined from
 * `planeAA`x + `planeAB`y + `planeAC`z + `planeADist` = 0 and
 * `planeBA`x + `planeBB`y + `planeBC`z + `planeBDist` = 0
 * the point on the intersection line closest to `point` is placed in `closestPoint`
 * returns false if the planes are parallel.
 */
s32 Math3D_PlaneVsPlaneVsLineClosestPoint(f32 planeAA, f32 planeAB, f32 planeAC, f32 planeADist, f32 planeBA,
                                          f32 planeBB, f32 planeBC, f32 planeBDist, Vec3f* point, Vec3f* closestPoint) {
    static Linef planeIntersect;

    if (!Math3D_PlaneVsPlaneNewLine(planeAA, planeAB, planeAC, planeADist, planeBA, planeBB, planeBC, planeBDist,
                                    (InfiniteLine*)&planeIntersect)) {
        return false;
    }
    Math3D_LineClosestToPoint(&planeIntersect, point, closestPoint);
    return true;
}

/**
 * Finds a point on the line from starting point `v0`, and directional vector `dir`
 * which is `dist` length from the starting point.  Result is placed in `ret`
 */
void Math3D_PointOnInfiniteLine(Vec3f* v0, Vec3f* dir, f32 dist, Vec3f* ret) {
    ret->x = (dir->x * dist) + v0->x;
    ret->y = (dir->y * dist) + v0->y;
    ret->z = (dir->z * dist) + v0->z;
}

/**
 * Splits the line segment from end points `v0` and `v1`, and splits that segment
 * by `ratio` of `v0`:`v1`, places the resulting point on the line in `ret`
 */
void Math3D_LineSplitRatio(Vec3f* v0, Vec3f* v1, f32 ratio, Vec3f* ret) {
    Vec3f diff;

    Math_Vec3f_Diff(v1, v0, &diff);
    Math3D_PointOnInfiniteLine(v0, &diff, ratio, ret);
}

/**
 * Calculates the cosine between vectors `a` and `b`
 */
f32 Math3D_Cos(Vec3f* a, Vec3f* b) {
    f32 ret;

    Math3D_CosOut(a, b, &ret);
    return ret;
}

/**
 * Calculates the cosine between bectors `a` and `b` and places the result in `ret`
 * returns true if the cosine cannot be calculated because the product of the magnitudes is zero
 */
s32 Math3D_CosOut(Vec3f* a, Vec3f* b, f32* dst) {
    f32 magProduct;

    magProduct = Math3D_Vec3fMagnitude(a) * Math3D_Vec3fMagnitude(b);
    if (IS_ZERO(magProduct)) {
        *dst = 0.0f;
        return true;
    }
    *dst = ((a->x * b->x) + (a->y * b->y) + (a->z * b->z)) / magProduct;
    return false;
}

/**
 * Reflects vector `vec` across the normal vector `normal`, reflection vector is placed in
 * `reflVec`
 */
void Math3D_Vec3fReflect(Vec3f* vec, Vec3f* normal, Vec3f* reflVec) {

    f32 normScaleY;
    Vec3f negVec;
    f32 normScaleZ;
    f32 normScaleX;
    f32 vecDotNorm;

    negVec.x = vec->x * -1.0f;
    negVec.y = vec->y * -1.0f;
    negVec.z = vec->z * -1.0f;

    vecDotNorm = Math3D_Cos(&negVec, normal);

    normScaleX = normal->x * vecDotNorm;
    normScaleY = normal->y * vecDotNorm;
    normScaleZ = normal->z * vecDotNorm;

    reflVec->x = ((normScaleX + vec->x) + (normScaleX + vec->x)) + negVec.x;
    reflVec->y = ((normScaleY + vec->y) + (normScaleY + vec->y)) + negVec.y;
    reflVec->z = ((normScaleZ + vec->z) + (normScaleZ + vec->z)) + negVec.z;
}

/**
 * Checks if the point (`x`,`y`) is contained within the square formed from (`upperLeftX`,`upperLeftY`) to
 * (`lowerRightX`,`lowerRightY`)
 */
s32 Math3D_PointInSquare2D(f32 upperLeftX, f32 lowerRightX, f32 upperLeftY, f32 lowerRightY, f32 x, f32 y) {
    if (x >= upperLeftX && x <= lowerRightX && y >= upperLeftY && y <= lowerRightY) {
        return true;
    }
    return false;
}

/**
 * Checks if the square formed around the circle with center (`centerX`,`centerY`) with radius `radius`
 * touches any portion of the square formed around the triangle with vertices (`x0`,`y0`), (`x1`,`y1`),
 * and (`x2`,`y2`)
 */
s32 Math3D_CirSquareVsTriSquare(f32 x0, f32 y0, f32 x1, f32 y1, f32 x2, f32 y2, f32 centerX, f32 centerY, f32 radius) {
    f32 minX;
    f32 maxX;
    f32 minY;
    f32 maxY;

    minX = maxX = x0;
    minY = maxY = y0;

    if (x1 < minX) {
        minX = x1;
    } else if (maxX < x1) {
        maxX = x1;
    }

    if (y1 < minY) {
        minY = y1;
    } else if (maxY < y1) {
        maxY = y1;
    }

    if (x2 < minX) {
        minX = x2;
    } else if (maxX < x2) {
        maxX = x2;
    }

    if (y2 < minY) {
        minY = y2;
    } else if (maxY < y2) {
        maxY = y2;
    }

    if ((minX - radius) <= centerX && (maxX + radius) >= centerX && (minY - radius) <= centerY &&
        (maxY + radius) >= centerY) {
        return true;
    }
    return false;
}

/**
 * Checks if the cube formed around the triangle formed from `v0`, `v1`, and `v2`
 * has any portion touching the cube formed around the sphere with center `center`
 * and radius of `radius`
 */
s32 Math3D_SphCubeVsTriCube(Vec3f* v0, Vec3f* v1, Vec3f* v2, Vec3f* center, f32 radius) {
    f32 minX;
    f32 maxX;
    f32 minY;
    f32 maxY;
    f32 minZ;
    f32 maxZ;

    minX = maxX = v0->x;
    minY = maxY = v0->y;
    minZ = maxZ = v0->z;

    if (v1->x < minX) {
        minX = v1->x;
    } else if (maxX < v1->x) {
        maxX = v1->x;
    }

    if (v1->y < minY) {
        minY = v1->y;
    } else if (maxY < v1->y) {
        maxY = v1->y;
    }

    if (v1->z < minZ) {
        minZ = v1->z;
    } else if (maxZ < v1->z) {
        maxZ = v1->z;
    }

    if (v2->x < minX) {
        minX = v2->x;
    } else if (maxX < v2->x) {
        maxX = v2->x;
    }

    if (v2->y < minY) {
        minY = v2->y;
    } else if (maxY < v2->y) {
        maxY = v2->y;
    }

    if (v2->z < minZ) {
        minZ = v2->z;
    } else if (maxZ < v2->z) {
        maxZ = v2->z;
    }

    if ((center->x >= (minX - radius)) && (center->x <= (maxX + radius)) && (center->y >= (minY - radius)) &&
        (center->y <= (maxY + radius)) && (center->z >= (minZ - radius)) && (center->z <= (maxZ + radius))) {
        return true;
    }

    return false;
}

/**
 * Returns the distance squared between `a` and `b` on a single axis
 */
f32 Math3D_Dist1DSq(f32 a, f32 b) {
    return SQ(a) + SQ(b);
}

/**
 * Returns the distance between `a` and `b` on a single axis
 */
f32 Math3D_Dist1D(f32 a, f32 b) {
    return sqrtf(Math3D_Dist1DSq(a, b));
}

/**
 * Returns the distance squared between (`x0`,`y0`) and (`x1`,`x2`)
 */
f32 Math3D_Dist2DSq(f32 x0, f32 y0, f32 x1, f32 y1) {
    return Math3D_Dist1DSq(x0 - x1, y0 - y1);
}

/**
 * Returns the distance between points (`x0`,`y0`) and (`x1`,`y1`)
 */
f32 Math3D_Dist2D(f32 x0, f32 y0, f32 x1, f32 y1) {
    return sqrtf(Math3D_Dist2DSq(x0, y0, x1, y1));
}

/**
 * Returns the magntiude (length) squared of `vec`
 */
f32 Math3D_Vec3fMagnitudeSq(Vec3f* vec) {
    return SQ(vec->x) + SQ(vec->y) + SQ(vec->z);
}

/**
 * Returns the magnitude(length) of `vec`
 */
f32 Math3D_Vec3fMagnitude(Vec3f* vec) {
    return sqrt(Math3D_Vec3fMagnitudeSq(vec));
}

/**
 * Returns the distance between `a` and `b` squared.
 */
f32 Math3D_Vec3fDistSq(Vec3f* a, Vec3f* b) {
    Vec3f diff;

    Math_Vec3f_Diff(a, b, &diff);
    return Math3D_Vec3fMagnitudeSq(&diff);
}

/*
 * Calculates the distance between points `a` and `b`
 */
f32 Math3D_Vec3f_DistXYZ(Vec3f* a, Vec3f* b) {
    return Math_Vec3f_DistXYZ(a, b);
}

/*
 * Calculates the distance between `a` and `b`.
 */
f32 Math3D_DistXYZ16toF(Vec3s* a, Vec3f* b) {
    Vec3f diff;

    diff.x = a->x - b->x;
    diff.y = a->y - b->y;
    diff.z = a->z - b->z;
    return Math3D_Vec3fMagnitude(&diff);
}

/**
 * Gets the Z portion of the cross product of vectors `a - (`dx`,`dy`,z) and `b` - (`dx`,`dy`,z)
 */
f32 Math3D_Vec3fDiff_CrossZ(Vec3f* a, Vec3f* b, f32 dx, f32 dy) {
    return ((a->x - dx) * (b->y - dy)) - ((a->y - dy) * (b->x - dx));
}

/**
 * Gets the X portion of the cross product of vectors `a - (x,`dy`,`dz`) and `b` - (x,`dy`,`dz`)
 */
f32 Math3D_Vec3fDiff_CrossX(Vec3f* a, Vec3f* b, f32 dy, f32 dz) {
    return ((a->y - dy) * (b->z - dz)) - ((a->z - dz) * (b->y - dy));
}

/**
 * Gets the Y portion of the cross product of vectors `a - (`dx`,y,`dz`) and `b` - (`dx`,y,`dz`)
 */
f32 Math3D_Vec3fDiff_CrossY(Vec3f* a, Vec3f* b, f32 dz, f32 dx) {
    return ((a->z - dz) * (b->x - dx)) - ((a->x - dx) * (b->z - dz));
}

/**
 * Gets the Cross Product of vectors `a` and `b` and places the result in `ret`
 */
void Math3D_Vec3f_Cross(Vec3f* a, Vec3f* b, Vec3f* ret) {
    ret->x = (a->y * b->z) - (a->z * b->y);
    ret->y = (a->z * b->x) - (a->x * b->z);
    ret->z = (a->x * b->y) - (a->y * b->x);
}

/*
 * Calculates the normal vector to a surface with sides `vb` - `va` and `vc` - `va`
 * outputs the normal to `normal`
 */
void Math3D_SurfaceNorm(Vec3f* va, Vec3f* vb, Vec3f* vc, Vec3f* normal) {
    static Vec3f abDiff;
    static Vec3f acDiff;

    Math_Vec3f_Diff(vb, va, &abDiff);
    Math_Vec3f_Diff(vc, va, &acDiff);
    Math3D_Vec3f_Cross(&abDiff, &acDiff, normal);
}

/**
 * Creates flags relative to the faces of a cube.
 */
s32 Math3D_PointRelativeToCubeFaces(Vec3f* point, Vec3f* min, Vec3f* max) {
    s32 ret = 0;

    if (point->x > max->x) {
        ret = 1;
    }

    if (point->x < min->x) {
        ret |= 2;
    }

    if (point->y > max->y) {
        ret |= 4;
    }

    if (point->y < min->y) {
        ret |= 8;
    }

    if (point->z > max->z) {
        ret |= 0x10;
    }

    if (point->z < min->z) {
        ret |= 0x20;
    }

    return ret;
}

/**
 * Creates flags of `point` relative to the edges of a cube
 */
s32 Math3D_PointRelativeToCubeEdges(Vec3f* point, Vec3f* min, Vec3f* max) {
    s32 ret = 0;

    if ((-min->x + max->y) < (-point->x + point->y)) {
        ret |= 1;
    }

    if ((-point->x + point->y) < (-max->x + min->y)) {
        ret |= 2;
    }

    if ((max->x + max->y) < (point->x + point->y)) {
        ret |= 4;
    }

    if ((point->x + point->y) < (min->x + min->y)) {
        ret |= 8;
    }

    if ((-min->z + max->y) < (-point->z + point->y)) {
        ret |= 0x10;
    }

    if ((-point->z + point->y) < (-max->z + min->y)) {
        ret |= 0x20;
    }

    if ((max->z + max->y) < (point->z + point->y)) {
        ret |= 0x40;
    }

    if ((point->z + point->y) < (min->z + min->y)) {
        ret |= 0x80;
    }

    if ((-min->z + max->x) < (-point->z + point->x)) {
        ret |= 0x100;
    }

    if ((-point->z + point->x) < (-max->z + min->x)) {
        ret |= 0x200;
    }

    if ((max->z + max->x) < (point->z + point->x)) {
        ret |= 0x400;
    }

    if ((point->z + point->x) < (min->z + min->x)) {
        ret |= 0x800;
    }
    return ret;
}

/**
 * Creates flags for `point` relative to the vertices of a cube
 */
s32 Math3D_PointRelativeToCubeVertices(Vec3f* point, Vec3f* min, Vec3f* max) {
    s32 ret = 0;

    if ((max->x + max->y + max->z) < (point->x + point->y + point->z)) {
        ret = 1;
    }

    if ((-min->x + max->y + max->z) < (-point->x + point->y + point->z)) {
        ret |= 2;
    }

    if ((-min->x + max->y - min->z) < (-point->x + point->y - point->z)) {
        ret |= 4;
    }

    if ((max->x + max->y - min->z) < (point->x + point->y - point->z)) {
        ret |= 8;
    }

    if ((max->x - min->y + max->z) < (point->x - point->y + point->z)) {
        ret |= 0x10;
    }

    //! @bug: The next 2 conditions are the same check.
    if ((-min->x - min->y + max->z) < (-point->x - point->y + point->z)) {
        ret |= 0x20;
    }

    if ((-min->x - min->y + max->z) < (-point->x - point->y + point->z)) {
        ret |= 0x40;
    }

    if ((-min->x - min->y - min->z) < (-point->x - point->y - point->z)) {
        ret |= 0x80;
    }
    return ret;
}

/**
 * Checks if a line segment with endpoints `a` and `b` intersect a cube
 */
s32 Math3D_LineVsCube(Vec3f* min, Vec3f* max, Vec3f* a, Vec3f* b) {
    static Vec3f triVtx0;
    static Vec3f triVtx1;
    static Vec3f triVtx2;
    static Vec3f intersectPoint;

    s32 flags[2];

    flags[0] = flags[1] = 0;
    flags[0] = Math3D_PointRelativeToCubeFaces(a, min, max);
    if (!flags[0]) {
        return true;
    }

    flags[1] = Math3D_PointRelativeToCubeFaces(b, min, max);
    if (!flags[1]) {
        return true;
    }

    if (flags[0] & flags[1]) {
        return false;
    }

    flags[0] |= (Math3D_PointRelativeToCubeEdges(a, min, max) << 8);
    flags[1] |= (Math3D_PointRelativeToCubeEdges(b, min, max) << 8);
    if (flags[0] & flags[1]) {
        return false;
    }

    flags[0] |= (Math3D_PointRelativeToCubeVertices(a, min, max) << 0x18);
    flags[1] |= (Math3D_PointRelativeToCubeVertices(b, min, max) << 0x18);
    if (flags[0] & flags[1]) {
        return false;
    }

    // face 1
    triVtx0.x = min->x;
    triVtx0.y = min->y;
    triVtx0.z = min->z;
    triVtx1.x = min->x;
    triVtx1.y = min->y;
    triVtx1.z = max->z;
    triVtx2.x = min->x;
    triVtx2.y = max->y;
    triVtx2.z = max->z;
    if (Math3D_TriLineIntersect(&triVtx0, &triVtx1, &triVtx2, -1.0f, 0.0f, 0.0f, min->x, a, b, &intersectPoint, 0)) {
        return true;
    }

    triVtx0.x = min->x;
    triVtx0.y = min->y;
    triVtx0.z = min->z;
    triVtx1.x = min->x;
    triVtx1.y = max->y;
    triVtx1.z = max->z;
    triVtx2.x = min->x;
    triVtx2.y = max->y;
    triVtx2.z = min->z;
    if (Math3D_TriLineIntersect(&triVtx0, &triVtx1, &triVtx2, -1.0f, 0.0f, 0.0f, min->x, a, b, &intersectPoint, 0)) {
        return true;
    }

    // face 2
    triVtx0.x = min->x;
    triVtx0.y = max->y;
    triVtx0.z = max->z;
    triVtx1.x = min->x;
    triVtx1.y = min->y;
    triVtx1.z = max->z;
    triVtx2.x = max->x;
    triVtx2.y = max->y;
    triVtx2.z = max->z;
    if (Math3D_TriLineIntersect(&triVtx0, &triVtx1, &triVtx2, 0.0f, 0.0f, 1.0f, -max->z, a, b, &intersectPoint, 0)) {
        return true;
    }
    triVtx0.x = max->x;
    triVtx0.y = max->y;
    triVtx0.z = max->z;
    triVtx1.x = min->x;
    triVtx1.y = min->y;
    triVtx1.z = max->z;
    triVtx2.x = max->x;
    //! @bug trVtx1.y should be triVtx2.y, prevents a tri on the cube from being checked.
    triVtx1.y = min->y;
    triVtx2.z = max->z;
    if (Math3D_TriLineIntersect(&triVtx0, &triVtx1, &triVtx2, 0.0f, 0.0f, 1.0f, -max->z, a, b, &intersectPoint, 0)) {
        return true;
    }

    // face 3
    triVtx0.x = max->x;
    triVtx0.y = max->y;
    triVtx0.z = max->z;
    triVtx1.x = min->x;
    triVtx1.y = max->y;
    triVtx1.z = min->z;
    triVtx2.x = min->x;
    triVtx2.y = max->y;
    triVtx2.z = max->z;
    if (Math3D_TriLineIntersect(&triVtx0, &triVtx1, &triVtx2, 0.0f, 1.0f, 0.0f, -max->y, a, b, &intersectPoint, 0)) {
        return true;
    }
    triVtx0.x = max->x;
    triVtx0.y = max->y;
    triVtx0.z = max->z;
    triVtx1.x = max->x;
    triVtx1.y = max->y;
    triVtx1.z = min->z;
    triVtx2.x = min->x;
    triVtx2.y = max->y;
    triVtx2.z = min->z;
    if (Math3D_TriLineIntersect(&triVtx0, &triVtx1, &triVtx2, 0.0f, 1.0f, 0.0f, -max->y, a, b, &intersectPoint, 0)) {
        return true;
    }

    // face 4
    triVtx0.x = min->x;
    triVtx0.y = min->y;
    triVtx0.z = min->z;
    triVtx1.x = min->x;
    triVtx1.y = max->y;
    triVtx1.z = min->z;
    triVtx2.x = max->x;
    triVtx2.y = max->y;
    triVtx2.z = min->z;
    if (Math3D_TriLineIntersect(&triVtx0, &triVtx1, &triVtx2, 0.0f, 0.0f, -1.0f, min->z, a, b, &intersectPoint, 0)) {
        return true;
    }
    triVtx0.x = min->x;
    triVtx0.y = min->y;
    triVtx0.z = min->z;
    triVtx1.x = max->x;
    triVtx1.y = max->y;
    triVtx1.z = min->z;
    triVtx2.x = max->x;
    triVtx2.y = min->y;
    triVtx2.z = min->z;

    // face 5
    if (Math3D_TriLineIntersect(&triVtx0, &triVtx1, &triVtx2, 0.0f, 0.0f, -1.0f, min->z, a, b, &intersectPoint, 0)) {
        return true;
    }
    triVtx0.x = min->x;
    triVtx0.y = min->y;
    triVtx0.z = min->z;
    triVtx1.x = max->x;
    triVtx1.y = min->y;
    triVtx1.z = min->z;
    triVtx2.x = max->x;
    triVtx2.y = min->y;
    triVtx2.z = max->z;
    if (Math3D_TriLineIntersect(&triVtx0, &triVtx1, &triVtx2, 0.0f, -1.0f, 0.0f, min->y, a, b, &intersectPoint, 0)) {
        return true;
    }
    triVtx0.x = min->x;
    triVtx0.y = min->y;
    triVtx0.z = min->z;
    triVtx1.x = max->x;
    triVtx1.y = min->y;
    triVtx1.z = max->z;
    triVtx2.x = min->x;
    triVtx2.y = min->y;
    triVtx2.z = max->z;

    // face 6
    if (Math3D_TriLineIntersect(&triVtx0, &triVtx1, &triVtx2, 0.0f, -1.0f, 0.0f, min->y, a, b, &intersectPoint, 0)) {
        return true;
    }
    triVtx0.x = max->x;
    triVtx0.y = max->y;
    triVtx0.z = max->z;
    triVtx1.x = max->x;
    triVtx1.y = min->y;
    triVtx1.z = min->z;
    triVtx2.x = max->x;
    triVtx2.y = max->y;
    triVtx2.z = min->z;
    if (Math3D_TriLineIntersect(&triVtx0, &triVtx1, &triVtx2, 1.0f, 0.0f, 0.0f, -max->x, a, b, &intersectPoint, 0)) {
        return true;
    }
    triVtx0.x = max->x;
    triVtx0.y = max->y;
    triVtx0.z = max->z;
    triVtx1.x = max->x;
    triVtx1.y = min->y;
    triVtx1.z = max->z;
    triVtx2.x = max->x;
    triVtx2.y = min->y;
    triVtx2.z = min->z;
    if (Math3D_TriLineIntersect(&triVtx0, &triVtx1, &triVtx2, 1.0f, 0.0f, 0.0f, -max->x, a, b, &intersectPoint, 0)) {
        return true;
    }

    return false;
}

/**
 * Checks if a line segment with endpoints `a` and `b` intersect a cube
 */
s32 Math3D_LineVsCubeShort(Vec3s* min, Vec3s* max, Vec3s* a, Vec3s* b) {
    static Vec3f minF;
    static Vec3f maxF;
    static Vec3f aF;
    static Vec3f bF;

    minF.x = min->x;
    minF.y = min->y;
    minF.z = min->z;
    maxF.x = max->x;
    maxF.y = max->y;
    maxF.z = max->z;
    aF.x = a->x;
    aF.y = a->y;
    aF.z = a->z;
    bF.x = b->x;
    bF.y = b->y;
    bF.z = b->z;
    return Math3D_LineVsCube(&minF, &maxF, &aF, &bF);
}

/**
 * Rotates the xz plane around the y axis `angle` degrees.
 * outputs the plane equation `a``pointOnPlane->x` + 0y + `c``pointOnPlane->z`+`d` = 0
 */
void Math3D_RotateXZPlane(Vec3f* pointOnPlane, s16 angle, f32* a, f32* c, f32* d) {
    *a = Math_SinS(angle) * 32767.0f;
    *c = Math_CosS(angle) * 32767.0f;
    *d = -((*a * pointOnPlane->x) + (*c * pointOnPlane->z));
}

/*
 * Defines a plane from verticies `va`, `vb`, and `vc`.  Normal components are output to
 * `nx`, `ny`, and `nz`.  Distance from the origin is output to `originDist`
 * Satisifes the plane equation NxVx + NyVy + NzVz + D = 0
 */
void Math3D_DefPlane(Vec3f* va, Vec3f* vb, Vec3f* vc, f32* nx, f32* ny, f32* nz, f32* originDist) {
    static Vec3f normal;

    f32 normMagnitude;
    f32 normMagInv;

    Math3D_SurfaceNorm(va, vb, vc, &normal);
    normMagnitude = sqrtf(SQ(normal.x) + SQ(normal.y) + SQ(normal.z));
    if (!IS_ZERO(normMagnitude)) {
        normMagInv = 1.0f / normMagnitude;
        *nx = normal.x * normMagInv;
        *ny = normal.y * normMagInv;
        *nz = normal.z * normMagInv;
        *originDist = -((*nx * va->x) + (*ny * va->y) + (*nz * va->z));
    } else {
        *originDist = 0.0f;
        *nz = 0.0f;
        *ny = 0.0f;
        *nx = 0.0f;
    }
}

/*
 * Returns the answer to the plane equation with elements specified by arguments.
 */
f32 Math3D_Planef(f32 nx, f32 ny, f32 nz, f32 originDist, Vec3f* pointOnPlane) {
    return (nx * pointOnPlane->x) + (ny * pointOnPlane->y) + (nz * pointOnPlane->z) + originDist;
}

/*
 * Returns the answer to the plane equation
 */
f32 Math3D_Plane(Plane* plane, Vec3f* pointOnPlane) {
    return (plane->normal.x * pointOnPlane->x) + (plane->normal.y * pointOnPlane->y) +
           (plane->normal.z * pointOnPlane->z) + plane->originDist;
}

/*
 * Calculates the absolute distance from a point `p` to the plane defined as
 * `nx`, `ny`, `nz`, and `originDist`
 */
f32 Math3D_UDistPlaneToPos(f32 nx, f32 ny, f32 nz, f32 originDist, Vec3f* p) {

    if (IS_ZERO(sqrtf(SQ(nx) + SQ(ny) + SQ(nz)))) {
        osSyncPrintf(VT_COL(YELLOW, BLACK));
        // "Math3DLengthPlaneAndPos(): Normal size is near zero %f %f %f"
        osSyncPrintf("Math3DLengthPlaneAndPos():法線size がゼロ近いです%f %f %f\n", nx, ny, nz);
        osSyncPrintf(VT_RST);
        return 0.0f;
    }
    return fabsf(Math3D_DistPlaneToPos(nx, ny, nz, originDist, p));
}

/*
 * Calculates the signed distance from a point `p` to a plane defined as
 * `nx`, `ny`, `nz`, and `originDist`
 */
f32 Math3D_DistPlaneToPos(f32 nx, f32 ny, f32 nz, f32 originDist, Vec3f* p) {
    f32 normMagnitude;

    normMagnitude = sqrtf(SQ(nx) + SQ(ny) + SQ(nz));
    if (IS_ZERO(normMagnitude)) {
        osSyncPrintf(VT_COL(YELLOW, BLACK));
        // "Math3DSignedLengthPlaneAndPos(): Normal size is close to zero %f %f %f"
        osSyncPrintf("Math3DSignedLengthPlaneAndPos():法線size がゼロ近いです%f %f %f\n", nx, ny, nz);
        osSyncPrintf(VT_RST);
        return 0.0f;
    }
    return Math3D_Planef(nx, ny, nz, originDist, p) / normMagnitude;
}

/**
 * Checks if the point defined by (`z`,`x`) is within distance of the triangle defined from `v0`,`v1`, and `v2`
 */
s32 Math3D_TriChkPointParaYImpl(Vec3f* v0, Vec3f* v1, Vec3f* v2, f32 z, f32 x, f32 detMax, f32 chkDist, f32 ny) {
    f32 detv0v1;
    f32 detv1v2;
    f32 detv2v0;
    f32 distToEdgeSq;
    f32 chkDistSq;

    // first check if the point is within range of the triangle.
    if (!Math3D_CirSquareVsTriSquare(v0->z, v0->x, v1->z, v1->x, v2->z, v2->x, z, x, chkDist)) {
        return false;
    }

    // check if the point is within `chkDist` units of any vertex of the triangle.
    chkDistSq = SQ(chkDist);
    if (((SQ(v0->z - z) + SQ(v0->x - x)) < chkDistSq) || ((SQ(v1->z - z) + SQ(v1->x - x)) < chkDistSq) ||
        ((SQ(v2->z - z) + SQ(v2->x - x)) < chkDistSq)) {

        return true;
    }

    // Calculate the determinant of each face of the triangle to the point.
    // If all the of determinants are within abs(`detMax`), return true.
    detv0v1 = ((v0->z - z) * (v1->x - x)) - ((v0->x - x) * (v1->z - z));
    detv1v2 = ((v1->z - z) * (v2->x - x)) - ((v1->x - x) * (v2->z - z));
    detv2v0 = ((v2->z - z) * (v0->x - x)) - ((v2->x - x) * (v0->z - z));

    if (((detMax >= detv0v1) && (detMax >= detv1v2) && (detMax >= detv2v0)) ||
        ((-detMax <= detv0v1) && (-detMax <= detv1v2) && (-detMax <= detv2v0))) {
        return true;
    }

    if (fabsf(ny) > 0.5f) {
        // Do a check on each face of the triangle, if the point is within `chkDist` units return true.
        if (Math3D_PointDistToLine2D(z, x, v0->z, v0->x, v1->z, v1->x, &distToEdgeSq) && (distToEdgeSq < chkDistSq)) {
            return true;
        }

        if (Math3D_PointDistToLine2D(z, x, v1->z, v1->x, v2->z, v2->x, &distToEdgeSq) && (distToEdgeSq < chkDistSq)) {
            return true;
        }

        if (Math3D_PointDistToLine2D(z, x, v2->z, v2->x, v0->z, v0->x, &distToEdgeSq) && (distToEdgeSq < chkDistSq)) {
            return true;
        }
    }
    return false;
}

s32 Math3D_TriChkPointParaYDeterminate(Vec3f* v0, Vec3f* v1, Vec3f* v2, f32 z, f32 x, f32 detMax, f32 ny) {
    return Math3D_TriChkPointParaYImpl(v0, v1, v2, z, x, detMax, 1.0f, ny);
}

s32 Math3D_TriChkPointParaYSlopedY(Vec3f* v0, Vec3f* v1, Vec3f* v2, f32 z, f32 x) {
    return Math3D_TriChkPointParaYImpl(v0, v1, v2, z, x, 300.0f, 1.0f, 0.6f);
}

/**
 * Performs the triangle and point check parallel to the Y axis, outputs the y coordinate of the point to `yIntersect`
 */
s32 Math3D_TriChkPointParaYIntersectDist(Vec3f* v0, Vec3f* v1, Vec3f* v2, f32 nx, f32 ny, f32 nz, f32 originDist, f32 z,
                                         f32 x, f32* yIntersect, f32 chkDist) {
    if (IS_ZERO(ny)) {
        return false;
    }

    if (Math3D_TriChkPointParaYImpl(v0, v1, v2, z, x, 300.0f, chkDist, ny)) {
        *yIntersect = (f32)((((-nx * x) - (nz * z)) - originDist) / ny);
        return true;
    }

    return false;
}

s32 Math3D_TriChkPointParaYIntersectInsideTri(Vec3f* v0, Vec3f* v1, Vec3f* v2, f32 nx, f32 ny, f32 nz, f32 originDist,
                                              f32 z, f32 x, f32* yIntersect, f32 chkDist) {
    if (IS_ZERO(ny)) {
        return false;
    }

    if (Math3D_TriChkPointParaYImpl(v0, v1, v2, z, x, 0.0f, chkDist, ny)) {
        *yIntersect = (f32)((((-nx * x) - (nz * z)) - originDist) / ny);
        return true;
    }

    return false;
}

s32 Math3D_TriChkPointParaY(Vec3f* v0, Vec3f* v1, Vec3f* v2, f32 ny, f32 z, f32 x) {
    if (IS_ZERO(ny)) {
        return false;
    }
    if (Math3D_TriChkPointParaYImpl(v0, v1, v2, z, x, 300.0f, 1.0f, ny)) {
        return true;
    }
    return false;
}

s32 Math3D_TriChkLineSegParaYIntersect(Vec3f* v0, Vec3f* v1, Vec3f* v2, f32 nx, f32 ny, f32 nz, f32 originDist, f32 z,
                                       f32 x, f32* yIntersect, f32 y0, f32 y1) {
    f32 pointADist;
    f32 pointBDist;
    Vec3f planePos;

    if (IS_ZERO(ny)) {
        return false;
    }

    planePos.x = x;
    planePos.y = y0;
    planePos.z = z;

    pointADist = Math3D_Planef(nx, ny, nz, originDist, &planePos);
    planePos.y = y1;
    pointBDist = Math3D_Planef(nx, ny, nz, originDist, &planePos);
    if (((pointADist > 0.0f) && (pointBDist > 0.0f)) || ((pointADist < 0.0f) && (pointBDist < 0.0f))) {
        return false;
    }

    if (Math3D_TriChkPointParaYImpl(v0, v1, v2, z, x, 300.0f, 1.0f, ny)) {
        *yIntersect = (((-nx * x) - (nz * z)) - originDist) / ny;
        return true;
    }

    return false;
}

s32 Math3D_TriChkPointParaYDist(Vec3f* v0, Vec3f* v1, Vec3f* v2, Plane* plane, f32 z, f32 x, f32 chkDist) {
    if (IS_ZERO(plane->normal.y)) {
        return false;
    }

    if (Math3D_TriChkPointParaYImpl(v0, v1, v2, z, x, 0.0f, chkDist, plane->normal.y)) {
        return true;
    }

    return false;
}

s32 Math3D_TriChkPointParaXImpl(Vec3f* v0, Vec3f* v1, Vec3f* v2, f32 y, f32 z, f32 detMax, f32 chkDist, f32 nx) {
    f32 detv0v1;
    f32 detv1v2;
    f32 detv2v0;
    f32 distToEdgeSq;
    f32 chkDistSq;

    if (!Math3D_CirSquareVsTriSquare(v0->y, v0->z, v1->y, v1->z, v2->y, v2->z, y, z, chkDist)) {
        return false;
    }

    chkDistSq = SQ(chkDist);

    if (((SQ(v0->y - y) + SQ(v0->z - z)) < chkDistSq) || ((SQ(v1->y - y) + SQ(v1->z - z)) < chkDistSq) ||
        ((SQ(v2->y - y) + SQ(v2->z - z)) < chkDistSq)) {
        return true;
    }

    detv0v1 = ((v0->y - y) * (v1->z - z)) - ((v0->z - z) * (v1->y - y));
    detv1v2 = ((v1->y - y) * (v2->z - z)) - ((v1->z - z) * (v2->y - y));
    detv2v0 = ((v2->y - y) * (v0->z - z)) - ((v2->z - z) * (v0->y - y));

    if (((detv0v1 <= detMax) && (detv1v2 <= detMax) && (detv2v0 <= detMax)) ||
        ((-detMax <= detv0v1) && (-detMax <= detv1v2) && (-detMax <= detv2v0))) {
        return true;
    }

    if (fabsf(nx) > 0.5f) {

        if (Math3D_PointDistToLine2D(y, z, v0->y, v0->z, v1->y, v1->z, &distToEdgeSq) && (distToEdgeSq < chkDistSq)) {
            return true;
        }

        if (Math3D_PointDistToLine2D(y, z, v1->y, v1->z, v2->y, v2->z, &distToEdgeSq) && (distToEdgeSq < chkDistSq)) {
            return true;
        }

        if (Math3D_PointDistToLine2D(y, z, v2->y, v2->z, v0->y, v0->z, &distToEdgeSq) && (distToEdgeSq < chkDistSq)) {
            return true;
        }
    }
    return false;
}

s32 Math3D_TriChkPointParaXDeterminate(Vec3f* v0, Vec3f* v1, Vec3f* v2, f32 y, f32 z, f32 detMax, f32 nx) {
    return Math3D_TriChkPointParaXImpl(v0, v1, v2, y, z, detMax, 1.0f, nx);
}

s32 Math3D_TriChkPointParaXIntersect(Vec3f* v0, Vec3f* v1, Vec3f* v2, f32 nx, f32 ny, f32 nz, f32 originDist, f32 y,
                                     f32 z, f32* xIntersect) {
    if (IS_ZERO(nx)) {
        return false;
    }

    if (Math3D_TriChkPointParaXImpl(v0, v1, v2, y, z, 300.0f, 1.0f, nx)) {
        *xIntersect = (((-ny * y) - (nz * z)) - originDist) / nx;
        return true;
    }
    return false;
}

s32 Math3D_TriChkPointParaX(Vec3f* v0, Vec3f* v1, Vec3f* v2, f32 nx, f32 y, f32 z) {
    if (IS_ZERO(nx)) {
        return false;
    }
    if (Math3D_TriChkPointParaXImpl(v0, v1, v2, y, z, 300.0f, 1.0f, nx)) {
        return true;
    }
    return false;
}

s32 Math3D_TriChkLineSegParaXIntersect(Vec3f* v0, Vec3f* v1, Vec3f* v2, f32 nx, f32 ny, f32 nz, f32 originDist, f32 y,
                                       f32 z, f32* xIntersect, f32 x0, f32 x1) {
    static Vec3f planePos;

    f32 pointADist;
    f32 pointBDist;

    if (IS_ZERO(nx)) {
        return false;
    }

    planePos.x = x0;
    planePos.y = y;
    planePos.z = z;
    pointADist = Math3D_Planef(nx, ny, nz, originDist, &planePos);

    planePos.x = x1;
    pointBDist = Math3D_Planef(nx, ny, nz, originDist, &planePos);

    if (((pointADist > 0.0f) && (pointBDist > 0.0f)) || ((pointADist < 0.0f) && (pointBDist < 0.0f))) {
        return false;
    }

    if (Math3D_TriChkPointParaXImpl(v0, v1, v2, y, z, 300.0f, 1.0f, nx)) {
        *xIntersect = (((-ny * y) - (nz * z)) - originDist) / nx;
        return true;
    }
    return false;
}

s32 Math3D_TriChkPointParaXDist(Vec3f* v0, Vec3f* v1, Vec3f* v2, Plane* plane, f32 y, f32 z, f32 chkDist) {
    if (IS_ZERO(plane->normal.x)) {
        return false;
    }
    if (Math3D_TriChkPointParaXImpl(v0, v1, v2, y, z, 0.0f, chkDist, plane->normal.x)) {
        return true;
    }
    return false;
}

s32 Math3D_TriChkPointParaZImpl(Vec3f* v0, Vec3f* v1, Vec3f* v2, f32 x, f32 y, f32 detMax, f32 chkDist, f32 nz) {
    f32 detv0v1;
    f32 detv1v2;
    f32 detv2v0;
    f32 distToEdgeSq;
    f32 chkDistSq;

    if (!Math3D_CirSquareVsTriSquare(v0->x, v0->y, v1->x, v1->y, v2->x, v2->y, x, y, chkDist)) {
        return false;
    }

    chkDistSq = SQ(chkDist);

    if (((SQ(x - v0->x) + SQ(y - v0->y)) < chkDistSq) || ((SQ(x - v1->x) + SQ(y - v1->y)) < chkDistSq) ||
        ((SQ(x - v2->x) + SQ(y - v2->y)) < chkDistSq)) {
        // Distance from any vertex to a point is less than chkDist
        return true;
    }

    detv0v1 = ((v0->x - x) * (v1->y - y)) - ((v0->y - y) * (v1->x - x));
    detv1v2 = ((v1->x - x) * (v2->y - y)) - ((v1->y - y) * (v2->x - x));
    detv2v0 = ((v2->x - x) * (v0->y - y)) - ((v2->y - y) * (v0->x - x));

    if (((detMax >= detv0v1) && (detMax >= detv1v2) && (detMax >= detv2v0)) ||
        ((-detMax <= detv0v1) && (-detMax <= detv1v2) && (-detMax <= detv2v0))) {
        return true;
    }

    if (fabsf(nz) > 0.5f) {

        if (Math3D_PointDistToLine2D(x, y, v0->x, v0->y, v1->x, v1->y, &distToEdgeSq) && (distToEdgeSq < chkDistSq)) {
            return true;
        }

        if (Math3D_PointDistToLine2D(x, y, v1->x, v1->y, v2->x, v2->y, &distToEdgeSq) && (distToEdgeSq < chkDistSq)) {
            return true;
        }

        if (Math3D_PointDistToLine2D(x, y, v2->x, v2->y, v0->x, v0->y, &distToEdgeSq) && (distToEdgeSq < chkDistSq)) {
            return true;
        }
    }

    return false;
}

s32 Math3D_TriChkPointParaZDeterminate(Vec3f* v0, Vec3f* v1, Vec3f* v2, f32 x, f32 y, f32 detMax, f32 nz) {
    return Math3D_TriChkPointParaZImpl(v0, v1, v2, x, y, detMax, 1.0f, nz);
}

s32 Math3D_TriChkPointParaZIntersect(Vec3f* v0, Vec3f* v1, Vec3f* v2, f32 nx, f32 ny, f32 nz, f32 originDist, f32 x,
                                     f32 y, f32* zIntersect) {

    if (IS_ZERO(nz)) {
        return false;
    }

    if (Math3D_TriChkPointParaZImpl(v0, v1, v2, x, y, 300.0f, 1.0f, nz)) {
        *zIntersect = (f32)((((-nx * x) - (ny * y)) - originDist) / nz);
        return true;
    }
    return false;
}

s32 Math3D_TriChkPointParaZ(Vec3f* v0, Vec3f* v1, Vec3f* v2, f32 nz, f32 x, f32 y) {
    if (IS_ZERO(nz)) {
        return false;
    }
    if (Math3D_TriChkPointParaZImpl(v0, v1, v2, x, y, 300.0f, 1.0f, nz)) {
        return true;
    }
    return false;
}

s32 Math3D_TriChkLineSegParaZIntersect(Vec3f* v0, Vec3f* v1, Vec3f* v2, f32 nx, f32 ny, f32 nz, f32 originDist, f32 x,
                                       f32 y, f32* zIntersect, f32 z0, f32 z1) {
    static Vec3f planePos;

    f32 pointADist;
    f32 pointBDist;

    if (IS_ZERO(nz)) {
        return false;
    }
    planePos.x = x;
    planePos.y = y;
    planePos.z = z0;
    pointADist = Math3D_Planef(nx, ny, nz, originDist, &planePos);

    planePos.z = z1;
    pointBDist = Math3D_Planef(nx, ny, nz, originDist, &planePos);
    if (((pointADist > 0.0f) && (pointBDist > 0.0f)) || ((pointADist < 0.0f) && (pointBDist < 0.0f))) {
        // points on the line segment are on the same side of the plane
        return false;
    }

    if (Math3D_TriChkPointParaZImpl(v0, v1, v2, x, y, 300.0f, 1.0f, nz)) {
        *zIntersect = (((-nx * x) - (ny * y)) - originDist) / nz;
        return true;
    }
    return false;
}

s32 Math3D_TriChkLineSegParaZDist(Vec3f* v0, Vec3f* v1, Vec3f* v2, Plane* plane, f32 x, f32 y, f32 chkDist) {
    if (IS_ZERO(plane->normal.z)) {
        return false;
    }
    if (Math3D_TriChkPointParaZImpl(v0, v1, v2, x, y, 0.0f, chkDist, plane->normal.z)) {
        return true;
    }
    return false;
}

s32 Math3D_LineSegFindPlaneIntersect(f32 pointADist, f32 pointBDist, Vec3f* pointA, Vec3f* pointB, Vec3f* intersect) {
    f32 distDiff;

    distDiff = pointADist - pointBDist;
    if (IS_ZERO(distDiff)) {
        // both points lie on the plane.
        *intersect = *pointB;
        return false;
    }

    if (pointADist == 0.0f) {
        // pointA is on the plane
        *intersect = *pointA;
    } else if (pointBDist == 0.0f) {
        // pointB is on the plane
        *intersect = *pointB;
    } else {
        // place the point at the intersection point.
        Math3D_LineSplitRatio(pointA, pointB, pointADist / distDiff, intersect);
    }
    return true;
}

/**
 * Determines if the line segement from `linePointA` to `linePointB` crosses the plane
 * from `nx` + `ny` + `nz` + `originDist` = 0.  If fromFront is set, then detection will only
 * be true if point A crosses from the front of the plane
 */
s32 Math3D_LineSegVsPlane(f32 nx, f32 ny, f32 nz, f32 originDist, Vec3f* linePointA, Vec3f* linePointB,
                          Vec3f* intersect, s32 fromFront) {
    f32 pointADist;
    f32 pointBDist;

    pointADist = Math3D_Planef(nx, ny, nz, originDist, linePointA);
    pointBDist = Math3D_Planef(nx, ny, nz, originDist, linePointB);

    if ((pointADist * pointBDist) > 0.0f) {
        *intersect = *linePointB;
        return false;
    }

    if (fromFront && (pointADist < 0.0f) && (pointBDist > 0.0f)) {
        *intersect = *linePointB;
        return false;
    }

    return Math3D_LineSegFindPlaneIntersect(pointADist, pointBDist, linePointA, linePointB, intersect);
}

/*
 * Determines if the line formed by `linePiontA` and `linePointB` intersect with Triangle formed from
 * vertices `v0`, `v1`, and `v2` with normal vector `nx`, `ny`, and `nz` with plane distance from origin
 * `originDist` Outputs the intersection point at to `intersect`
 * Returns 1 if the line intersects with the triangle, 0 otherwise
 */
s32 Math3D_TriLineIntersect(Vec3f* v0, Vec3f* v1, Vec3f* v2, f32 nx, f32 ny, f32 nz, f32 originDist, Vec3f* linePointA,
                            Vec3f* linePointB, Vec3f* intersect, s32 fromFront) {

    if (!Math3D_LineSegVsPlane(nx, ny, nz, originDist, linePointA, linePointB, intersect, fromFront)) {
        return false;
    }

    if (((nx == 0.0f) || (Math3D_TriChkPointParaX(v0, v1, v2, nx, intersect->y, intersect->z))) &&
        ((ny == 0.0f) || (Math3D_TriChkPointParaY(v0, v1, v2, ny, intersect->z, intersect->x))) &&
        ((nz == 0.0f) || (Math3D_TriChkPointParaZ(v0, v1, v2, nz, intersect->x, intersect->y)))) {
        return true;
    }

    *intersect = *linePointB;
    return false;
}

/*
 * Creates a TriNorm output to `tri`, and calculates the normal vector and plane from vertices
 * `va`, `vb`, and `vc`
 */
void Math3D_TriNorm(TriNorm* tri, Vec3f* va, Vec3f* vb, Vec3f* vc) {
    tri->vtx[0] = *va;
    tri->vtx[1] = *vb;
    tri->vtx[2] = *vc;
    Math3D_DefPlane(va, vb, vc, &tri->plane.normal.x, &tri->plane.normal.y, &tri->plane.normal.z,
                    &tri->plane.originDist);
}

/*
 * Determines if point `point` lies within `sphere`
 */
s32 Math3D_PointInSph(Sphere16* sphere, Vec3f* point) {

    if (Math3D_DistXYZ16toF(&sphere->center, point) < sphere->radius) {
        return true;
    }
    return false;
}

/**
 * Determines the distance from point (`x0`,`y0`) to the line fromed from (`x1`,`y1`) and (`x2`,`y2`)
 * Distance squared is output to `lineLenSq`, returns true if the point perpendicular from (`x0`,`y0`)
 * is contained within the segement between (`x1`,`y1`) and (`x2`,`y2`)
 */
s32 Math3D_PointDistToLine2D(f32 x0, f32 y0, f32 x1, f32 y1, f32 x2, f32 y2, f32* lineLenSq) {
    static Vec3f perpendicularPoint;

    f32 perpendicularRatio;
    f32 xDiff;
    f32 distSq;
    f32 yDiff;
    s32 ret = false;

    xDiff = x2 - x1;
    yDiff = y2 - y1;
    distSq = SQ(xDiff) + SQ(yDiff);
    if (IS_ZERO(distSq)) {
        *lineLenSq = 0.0f;
        return false;
    }

    perpendicularRatio = (((x0 - x1) * xDiff) + (y0 - y1) * yDiff) / distSq;
    if (perpendicularRatio >= 0.0f && perpendicularRatio <= 1.0f) {
        ret = true;
    }
    perpendicularPoint.x = (xDiff * perpendicularRatio) + x1;
    perpendicularPoint.y = (yDiff * perpendicularRatio) + y1;
    *lineLenSq = SQ(perpendicularPoint.x - x0) + SQ(perpendicularPoint.y - y0);
    return ret;
}

/**
 * Determines if the line `line` is touching the sphere `sphere` at any point in the line.
 */
s32 Math3D_LineVsSph(Sphere16* sphere, Linef* line) {
    static Vec3f sphLinePerpendicularPoint;

    Vec3f lineDiff;
    f32 temp_f0_2;
    f32 lineLenSq;

    if ((Math3D_PointInSph(sphere, &line->a)) || (Math3D_PointInSph(sphere, &line->b))) {
        // either point of the line is in the sphere.
        return true;
    }
    lineDiff.x = line->b.x - line->a.x;
    lineDiff.y = line->b.y - line->a.y;
    lineDiff.z = line->b.z - line->a.z;

    lineLenSq = SQ(lineDiff.x) + SQ(lineDiff.y) + SQ(lineDiff.z);
    if (IS_ZERO(lineLenSq)) {
        // line length is "0"
        return false;
    }
    temp_f0_2 = ((((sphere->center.x - line->a.x) * lineDiff.x) + ((sphere->center.y - line->a.y) * lineDiff.y)) +
                 ((sphere->center.z - line->a.z) * lineDiff.z)) /
                lineLenSq;
    if ((temp_f0_2 < 0.0f) || (temp_f0_2 > 1.0f)) {
        return false;
    }

    sphLinePerpendicularPoint.x = (lineDiff.x * temp_f0_2) + line->a.x;
    sphLinePerpendicularPoint.y = (lineDiff.y * temp_f0_2) + line->a.y;
    sphLinePerpendicularPoint.z = (lineDiff.z * temp_f0_2) + line->a.z;

    if (SQ(sphLinePerpendicularPoint.x - sphere->center.x) + SQ(sphLinePerpendicularPoint.y - sphere->center.y) +
            SQ(sphLinePerpendicularPoint.z - sphere->center.z) <=
        SQ((f32)sphere->radius)) {
        return true;
    }
    return false;
}

/**
 * Gets the surface point of `sphere` intersecting with `tri` generated from the line formed from the
 * sphere's surface to the midpoint of the line formed from the first two vertices of the tri
 */
void Math3D_GetSphVsTriIntersectPoint(Sphere16* sphere, TriNorm* tri, Vec3f* intersectPoint) {
    static Vec3f v0v1Center;
    static Vec3f sphereCenter;

    f32 dist;
    f32 splitRatio;

    v0v1Center.x = ((tri->vtx[0].x + tri->vtx[1].x) * 0.5f);
    v0v1Center.y = ((tri->vtx[0].y + tri->vtx[1].y) * 0.5f);
    v0v1Center.z = ((tri->vtx[0].z + tri->vtx[1].z) * 0.5f);
    sphereCenter.x = sphere->center.x;
    sphereCenter.y = sphere->center.y;
    sphereCenter.z = sphere->center.z;
    dist = Math3D_Vec3f_DistXYZ(&v0v1Center, &sphereCenter);
    // Distance from the sphere's center to the center of the line formed from v0->v1
    if (IS_ZERO(dist)) {
        intersectPoint->x = sphereCenter.x;
        intersectPoint->y = sphereCenter.y;
        intersectPoint->z = sphereCenter.z;
        return;
    }
    splitRatio = sphere->radius / dist;
    Math3D_LineSplitRatio(&sphereCenter, &v0v1Center, splitRatio, intersectPoint);
}

/**
 * Determines if `sphere` and `tri` and touching, and outputs the intersection point to `intersectPoint`
 */
s32 Math3D_TriVsSphIntersect(Sphere16* sphere, TriNorm* tri, Vec3f* intersectPoint) {
    static Linef triTestLine;
    static Vec3f sphereCenter;
    static Vec3f sphPlanePos;

    f32 radius;
    f32 nx;
    f32 ny;
    f32 nz;
    f32 planeDist;

    sphereCenter.x = sphere->center.x;
    sphereCenter.y = sphere->center.y;
    sphereCenter.z = sphere->center.z;
    radius = sphere->radius;

    if (!Math3D_SphCubeVsTriCube(&tri->vtx[0], &tri->vtx[1], &tri->vtx[2], &sphereCenter, radius)) {
        return false;
    }

    planeDist = Math3D_UDistPlaneToPos(tri->plane.normal.x, tri->plane.normal.y, tri->plane.normal.z,
                                       tri->plane.originDist, &sphereCenter);
    if (radius < planeDist) {
        // the point that lies within the plane of the triangle which is perpendicular to the sphere's center is more
        // than the radius of the sphere, the plane never crosses the sphere.
        return false;
    }

    // tests if any of the edges of the triangle are intersecting the sphere
    triTestLine.a = tri->vtx[0];
    triTestLine.b = tri->vtx[1];
    if (Math3D_LineVsSph(sphere, &triTestLine)) {
        Math3D_GetSphVsTriIntersectPoint(sphere, tri, intersectPoint);
        return true;
    }

    triTestLine.a = tri->vtx[1];
    triTestLine.b = tri->vtx[2];
    if (Math3D_LineVsSph(sphere, &triTestLine)) {
        Math3D_GetSphVsTriIntersectPoint(sphere, tri, intersectPoint);
        return true;
    }

    triTestLine.a = tri->vtx[2];
    triTestLine.b = tri->vtx[0];
    if (Math3D_LineVsSph(sphere, &triTestLine)) {
        Math3D_GetSphVsTriIntersectPoint(sphere, tri, intersectPoint);
        return true;
    }

    nx = tri->plane.normal.x * planeDist;
    ny = tri->plane.normal.y * planeDist;
    nz = tri->plane.normal.z * planeDist;

    if (Math3D_Planef(tri->plane.normal.x, tri->plane.normal.y, tri->plane.normal.z, tri->plane.originDist,
                      &sphereCenter) > 0.0f) {
        sphPlanePos.x = sphereCenter.x - nx;
        sphPlanePos.y = sphereCenter.y - ny;
        sphPlanePos.z = sphereCenter.z - nz;
    } else {
        sphPlanePos.x = sphereCenter.x + nx;
        sphPlanePos.y = sphereCenter.y + ny;
        sphPlanePos.z = sphereCenter.z + nz;
    }

    if (fabsf(tri->plane.normal.y) > 0.5f) {
        if (Math3D_TriChkPointParaYDeterminate(&tri->vtx[0], &tri->vtx[1], &tri->vtx[2], sphPlanePos.z, sphPlanePos.x,
                                               0.0f, tri->plane.normal.y)) {
            Math3D_GetSphVsTriIntersectPoint(sphere, tri, intersectPoint);
            return true;
        }
    } else if (fabsf(tri->plane.normal.x) > 0.5f) {
        if (Math3D_TriChkPointParaXDeterminate(&tri->vtx[0], &tri->vtx[1], &tri->vtx[2], sphPlanePos.y, sphPlanePos.z,
                                               0.0f, tri->plane.normal.x)) {
            Math3D_GetSphVsTriIntersectPoint(sphere, tri, intersectPoint);
            return true;
        }
    } else if (Math3D_TriChkPointParaZDeterminate(&tri->vtx[0], &tri->vtx[1], &tri->vtx[2], sphPlanePos.x,
                                                  sphPlanePos.y, 0.0f, tri->plane.normal.z)) {
        Math3D_GetSphVsTriIntersectPoint(sphere, tri, intersectPoint);
        return true;
    }
    return false;
}

/*
 * Checks if point `point` is within cylinder `cyl`
 * Returns 1 if the point is inside the cylinder, 0 otherwise.
 */
s32 Math3D_PointInCyl(Cylinder16* cyl, Vec3f* point) {
    f32 bottom;
    f32 top;
    f32 x;
    f32 z;

    x = cyl->pos.x - point->x;
    z = cyl->pos.z - point->z;
    bottom = (f32)cyl->pos.y + cyl->yShift;
    top = cyl->height + bottom;

    if ((SQ(x) + SQ(z)) < SQ(cyl->radius) && (bottom < point->y) && (point->y < top)) {
        return true;
    } else {
        return false;
    }
}

s32 Math3D_CylVsLineSeg(Cylinder16* cyl, Vec3f* linePointA, Vec3f* linePointB, Vec3f* intersectA, Vec3f* intersectB) {
    Vec3f cylToPtA;
    Vec3f cylToPtB;
    Vec3f ptAToPtB;
    f32 fracA;
    f32 fracB;
    f32 fracBase;
    f32 zero = 0.0f;
    f32 pad;
    f32 cylRadiusSq;
    f32 radSqDiff;
    f32 distCent2;
    f32 dot2AB;
    s32 sideIntA;
    s32 sideIntB;
    s32 intBeyondA;
    s32 intBeyondB;
    s32 intFlags = 0;
    Vec3f intPts[4];
    s32 count;
    s32 i;

    if (Math3D_PointInCyl(cyl, linePointA) && Math3D_PointInCyl(cyl, linePointB)) {
        // both points are in the cylinder
        *intersectA = *linePointA;
        *intersectB = *linePointB;
        return 2;
    }

    cylToPtA.x = linePointA->x - cyl->pos.x;
    cylToPtA.y = linePointA->y - cyl->pos.y - cyl->yShift;
    cylToPtA.z = linePointA->z - cyl->pos.z;
    cylToPtB.x = linePointB->x - cyl->pos.x;
    cylToPtB.y = linePointB->y - cyl->pos.y - cyl->yShift;
    cylToPtB.z = linePointB->z - cyl->pos.z;
    Math_Vec3f_Diff(&cylToPtB, &cylToPtA, &ptAToPtB);
    cylRadiusSq = SQ(cyl->radius);

    /**
     * This section checks for intersections with the cylinder's base and top
     */
    if (!IS_ZERO(ptAToPtB.y)) {
        // fraction of length along AB to reach y = 0
        fracBase = -cylToPtA.y / ptAToPtB.y;
        if ((0.0f <= fracBase) && (fracBase <= 1.0f)) {
            f32 baseIntX = (ptAToPtB.x * fracBase) + cylToPtA.x;
            f32 baseIntZ = (ptAToPtB.z * fracBase) + cylToPtA.z;

            if (SQ(baseIntX) + SQ(baseIntZ) < cylRadiusSq) {
                // adds base intersection point to intPts and sets its flag
                intPts[0].x = cyl->pos.x + baseIntX;
                intPts[0].y = (f32)cyl->pos.y + cyl->yShift;
                intPts[0].z = cyl->pos.z + baseIntZ;
                intFlags |= 1;
            }
        }
        // fraction of length along AB to reach y = cyl->height
        fracA = (cyl->height - cylToPtA.y) / ptAToPtB.y;
        if ((0.0f <= fracA) && (fracA <= 1.0f)) {
            f32 topIntX = ptAToPtB.x * fracA + cylToPtA.x;
            f32 topIntZ = ptAToPtB.z * fracA + cylToPtA.z;

            if (SQ(topIntX) + SQ(topIntZ) < cylRadiusSq) {
                // adds top intersection point to intPts and sets its flag
                intPts[1].x = cyl->pos.x + topIntX;
                intPts[1].y = (f32)cyl->pos.y + cyl->yShift + cyl->height;
                intPts[1].z = cyl->pos.z + topIntZ;
                intFlags |= 2;
            }
        }
    }
    /**
     * This section finds the points of intersection of the infinite line containing AB with the side of the infinite
     * cylinder containing cyl. Intersection points beyond the bounds of the segment and cylinder are filtered out
     * afterward.
     */
    radSqDiff = SQXZ(cylToPtA) - cylRadiusSq;
    if (!IS_ZERO(2.0f * SQXZ(ptAToPtB))) {
        dot2AB = 2.0f * DOTXZ(ptAToPtB, cylToPtA);
        if (SQ(dot2AB) < 4.0f * SQXZ(ptAToPtB) * radSqDiff) {
            // Line's closest xz-approach is outside cylinder. No intersections.
            return 0;
        }
        if (SQ(dot2AB) - (4.0f * SQXZ(ptAToPtB) * radSqDiff) > zero) {
            sideIntA = sideIntB = 1;
        } else {
            // Line is tangent in xz-plane. At most 1 side intersection.
            sideIntA = 1;
            sideIntB = 0;
        }
        distCent2 = sqrtf(SQ(dot2AB) - (4.0f * SQXZ(ptAToPtB) * radSqDiff));
        if (sideIntA == 1) {
            // fraction of length along AB for side intersection closer to A
            fracA = (distCent2 - dot2AB) / (2.0f * SQXZ(ptAToPtB));
        }
        if (sideIntB == 1) {
            // fraction of length along AB for side intersection closer to B
            fracB = (-dot2AB - distCent2) / (2.0f * SQXZ(ptAToPtB));
        }
    } else if (!IS_ZERO(2.0f * DOTXZ(ptAToPtB, cylToPtA))) {
        // Used if the line segment is nearly vertical. Unclear what it's calculating.
        fracA = -radSqDiff / (2.0f * DOTXZ(ptAToPtB, cylToPtA));
        sideIntA = 1;
        sideIntB = 0;
    } else {
        return 0;
    }
    // checks for intersection points outside the bounds of the segment
    if (!sideIntB) {
        if (fracA < 0.0f || 1.0f < fracA) {
            return 0;
        }
    } else {
        intBeyondA = fracA < 0.0f || 1.0f < fracA;
        intBeyondB = fracB < 0.0f || 1.0f < fracB;
        if (intBeyondA && intBeyondB) {
            return 0;
        }
        if (intBeyondA) {
            sideIntA = 0;
        }
        if (intBeyondB) {
            sideIntB = 0;
        }
    }
    // checks for intersection points outside the bounds of the cylinder
    if ((sideIntA == 1) &&
        ((fracA * ptAToPtB.y + cylToPtA.y) < 0.0f || cyl->height < (fracA * ptAToPtB.y + cylToPtA.y))) {
        sideIntA = 0;
    }
    if ((sideIntB == 1) &&
        ((fracB * ptAToPtB.y + cylToPtA.y) < 0.0f || cyl->height < (fracB * ptAToPtB.y + cylToPtA.y))) {
        sideIntB = 0;
    }
    if (sideIntA == 0 && sideIntB == 0) {
        return 0;
    }
    // Adds intersection points to intPts and sets side A and side B flags
    if (sideIntA == 1 && sideIntB == 1) {
        intPts[2].x = (fracA * ptAToPtB.x + cylToPtA.x) + cyl->pos.x;
        intPts[2].y = (fracA * ptAToPtB.y + cylToPtA.y) + cyl->pos.y + cyl->yShift;
        intPts[2].z = (fracA * ptAToPtB.z + cylToPtA.z) + cyl->pos.z;
        intFlags |= 4;
        intPts[3].x = (fracB * ptAToPtB.x + cylToPtA.x) + cyl->pos.x;
        intPts[3].y = (fracB * ptAToPtB.y + cylToPtA.y) + cyl->pos.y + cyl->yShift;
        intPts[3].z = (fracB * ptAToPtB.z + cylToPtA.z) + cyl->pos.z;
        intFlags |= 8;
    } else if (sideIntA == 1) {
        intPts[2].x = (fracA * ptAToPtB.x + cylToPtA.x) + cyl->pos.x;
        intPts[2].y = (fracA * ptAToPtB.y + cylToPtA.y) + cyl->pos.y + cyl->yShift;
        intPts[2].z = (fracA * ptAToPtB.z + cylToPtA.z) + cyl->pos.z;
        intFlags |= 4;
    } else if (sideIntB == 1) {
        intPts[2].x = (fracB * ptAToPtB.x + cylToPtA.x) + cyl->pos.x;
        intPts[2].y = (fracB * ptAToPtB.y + cylToPtA.y) + cyl->pos.y + cyl->yShift;
        intPts[2].z = (fracB * ptAToPtB.z + cylToPtA.z) + cyl->pos.z;
        intFlags |= 4;
    }

    /**
     * Places the found intersection points into intersectA and intersectB. IntersectA is always closer to point A
     */
    for (count = 0, i = 0; i < 4; i++) {
        if (intFlags & (1 << i)) {
            if (count == 0) {
                *intersectA = intPts[i];
            } else if (count == 1) {
                if (Math3D_Vec3fDistSq(intersectA, linePointA) < Math3D_Vec3fDistSq(intersectA, &intPts[i])) {
                    *intersectB = intPts[i];
                } else {
                    *intersectB = *intersectA;
                    *intersectA = intPts[i];
                }
                break;
            }
            count++;
        }
    }
    return count;
}

/*
 * Determines if `cyl` and `tri` are touching.  The point of intersection
 * is placed in `intersect` Returns 1 if they are touching, 0 otherwise.
 */
s32 Math3D_CylTriVsIntersect(Cylinder16* cyl, TriNorm* tri, Vec3f* intersect) {
    static Sphere16 topSphere;
    static Sphere16 bottomSphere;
    static Vec3f cylIntersectA;
    static Vec3f cylIntersectB;

    f32 yIntersect;
    f32 cylTop;
    f32 cylBottom;
    f32 minDistSq;
    f32 radiusTodistFromCylYIntersectTov0v1;
    f32 distFromPointAToIntersectASq;
    Vec3f cylIntersectCenter;
    Vec3f midpointv0v1;
    Vec3f diffMidpointIntersect;
    f32 distFromCylYIntersectTov0v1;
    s32 pad;

    cylBottom = (f32)cyl->pos.y + cyl->yShift;
    cylTop = cyl->height + cylBottom;

    if (((tri->vtx[0].y < cylBottom) && (tri->vtx[1].y < cylBottom) && (tri->vtx[2].y < cylBottom)) ||
        ((cylTop < tri->vtx[0].y) && (cylTop < tri->vtx[1].y) && (cylTop < tri->vtx[2].y))) {
        // If all of the verticies are below or all of the verticies are above the cylinder.
        return false;
    }

    minDistSq = 1.e38f;
    if (Math3D_CylVsLineSeg(cyl, &tri->vtx[0], &tri->vtx[1], &cylIntersectA, &cylIntersectB)) {
        distFromPointAToIntersectASq = Math3D_Vec3fDistSq(&cylIntersectA, &tri->vtx[0]);
        minDistSq = distFromPointAToIntersectASq;
        *intersect = cylIntersectA;
    }

    if (Math3D_CylVsLineSeg(cyl, &tri->vtx[2], &tri->vtx[1], &cylIntersectA, &cylIntersectB)) {
        distFromPointAToIntersectASq = Math3D_Vec3fDistSq(&cylIntersectA, &tri->vtx[2]);
        if (distFromPointAToIntersectASq < minDistSq) {
            *intersect = cylIntersectA;
            minDistSq = distFromPointAToIntersectASq;
        }
    }

    if (Math3D_CylVsLineSeg(cyl, &tri->vtx[0], &tri->vtx[2], &cylIntersectA, &cylIntersectB)) {
        distFromPointAToIntersectASq = Math3D_Vec3fDistSq(&cylIntersectA, &tri->vtx[0]);
        if (distFromPointAToIntersectASq < minDistSq) {
            *intersect = cylIntersectA;
            minDistSq = distFromPointAToIntersectASq;
        }
    }

    if (minDistSq != (f32)1.e38f) {
        return true;
    }

    if (Math3D_TriChkLineSegParaYIntersect(&tri->vtx[0], &tri->vtx[1], &tri->vtx[2], tri->plane.normal.x,
                                           tri->plane.normal.y, tri->plane.normal.z, tri->plane.originDist, cyl->pos.z,
                                           cyl->pos.x, &yIntersect, cylBottom, cylTop)) {

        cylIntersectCenter.x = cyl->pos.x;
        cylIntersectCenter.y = yIntersect;
        cylIntersectCenter.z = cyl->pos.z;

        midpointv0v1.x = (tri->vtx[0].x + tri->vtx[1].x) * 0.5f;
        midpointv0v1.y = (tri->vtx[0].y + tri->vtx[1].y) * 0.5f;
        midpointv0v1.z = (tri->vtx[0].z + tri->vtx[1].z) * 0.5f;

        Math_Vec3f_Diff(&midpointv0v1, &cylIntersectCenter, &diffMidpointIntersect);
        distFromCylYIntersectTov0v1 = sqrtf(SQ(diffMidpointIntersect.x) + SQ(diffMidpointIntersect.z));

        if (IS_ZERO(distFromCylYIntersectTov0v1)) {
            Math_Vec3f_Copy(intersect, &midpointv0v1);
            return true;
        }

        radiusTodistFromCylYIntersectTov0v1 = cyl->radius / distFromCylYIntersectTov0v1;
        Math3D_PointOnInfiniteLine(&cylIntersectCenter, &diffMidpointIntersect, radiusTodistFromCylYIntersectTov0v1,
                                   intersect);
        return true;
    }

    topSphere.center.x = bottomSphere.center.x = cyl->pos.x;
    topSphere.center.z = bottomSphere.center.z = cyl->pos.z;
    topSphere.center.y = cylTop;
    bottomSphere.center.y = cylBottom;
    topSphere.radius = bottomSphere.radius = cyl->radius;

    if ((Math3D_TriVsSphIntersect(&topSphere, tri, intersect)) ||
        (Math3D_TriVsSphIntersect(&bottomSphere, tri, intersect))) {
        return true;
    }
    return false;
}

/*
 * Determines if `cyl` and `tri` are touching.
 */
s32 Math3D_CylVsTri(Cylinder16* cyl, TriNorm* tri) {
    Vec3f intersect;

    return Math3D_CylTriVsIntersect(cyl, tri, &intersect);
}

/*
 * Deteremines if two spheres are touching.
 */
s32 Math3D_SphVsSph(Sphere16* sphereA, Sphere16* sphereB) {
    f32 overlapSize;

    return Math3D_SphVsSphOverlap(sphereA, sphereB, &overlapSize);
}

/*
 * Determines if two spheres are touching.  The amount that they're overlapping is placed in `overlapSize`
 */
s32 Math3D_SphVsSphOverlap(Sphere16* sphereA, Sphere16* sphereB, f32* overlapSize) {
    f32 centerDist;

    return Math3D_SphVsSphOverlapCenter(sphereA, sphereB, overlapSize, &centerDist);
}

/*
 * Determines if two spheres are touching  The distance from the centers is placed in `centerDist`,
 * and the amount that they're overlapping is placed in `overlapSize`
 */
s32 Math3D_SphVsSphOverlapCenter(Sphere16* sphereA, Sphere16* sphereB, f32* overlapSize, f32* centerDist) {
    Vec3f diff;

    diff.x = (f32)sphereA->center.x - (f32)sphereB->center.x;
    diff.y = (f32)sphereA->center.y - (f32)sphereB->center.y;
    diff.z = (f32)sphereA->center.z - (f32)sphereB->center.z;

    *centerDist = sqrt(SQ(diff.x) + SQ(diff.y) + SQ(diff.z));

    *overlapSize = (((f32)sphereA->radius + (f32)sphereB->radius) - *centerDist);
    if (*overlapSize > 0.008f) {
        return true;
    }

    *overlapSize = 0.0f;
    return false;
}

/**
 * Checks if `sph` and `cyl` are touching, output the amount of overlap to `overlapSize`
 */
s32 Math3D_SphVsCylOverlapDist(Sphere16* sph, Cylinder16* cyl, f32* overlapSize) {
    f32 centerDist;

    return Math3D_SphVsCylOverlapCenterDist(sph, cyl, overlapSize, &centerDist);
}

/**
 * Checks if `sph` and `cyl` are touching, output the xz distance of the centers to `centerDist`, and the amount of
 * overlap to `overlapSize`
 */
s32 Math3D_SphVsCylOverlapCenterDist(Sphere16* sph, Cylinder16* cyl, f32* overlapSize, f32* centerDist) {
    static Cylinderf cylf;
    static Spheref sphf;

    f32 x;
    f32 z;
    f32 combinedRadius;
    f32 cylBottom;
    f32 cylTop;
    f32 sphBottom;
    f32 sphTop;

    if (sph->radius <= 0 || cyl->radius <= 0) {
        // either radius is 0
        return false;
    }
    sphf.center.y = sph->center.y;
    sphf.radius = sph->radius;
    cylf.pos.y = cyl->pos.y;
    cylf.yShift = cyl->yShift;
    cylf.height = cyl->height;
    x = (f32)sph->center.x - cyl->pos.x;
    z = (f32)sph->center.z - cyl->pos.z;
    combinedRadius = (f32)sph->radius + cyl->radius;
    *centerDist = sqrtf(SQ(x) + SQ(z));
    if (combinedRadius < *centerDist) {
        // if the combined radii is less than the distance to the centers, they cannot be touching.
        return false;
    }

    cylBottom = (cylf.pos.y + cylf.yShift);
    cylTop = cylBottom + cylf.height;
    sphBottom = sphf.center.y - sphf.radius;
    sphTop = sphf.center.y + sphf.radius;

    if ((sphTop >= cylBottom) && (sphBottom <= cylTop)) {
        // if the cylinder and sphere are intersecting on the xz plane, check if they're intersecting on
        // the y axis.
        *overlapSize = combinedRadius - *centerDist;
        return true;
    }
    return false;
}

/*
 * returns 1 if cylinder `ca` is outside cylinder `cb`.
 * Sets `deadSpace` to the mininum space between the cylinders not occupied by the other.
 */
s32 Math3D_CylOutsideCyl(Cylinder16* ca, Cylinder16* cb, f32* deadSpace) {
    f32 xzDist;

    return Math3D_CylOutsideCylDist(ca, cb, deadSpace, &xzDist);
}

/*
 * returns 1 if cylinder `ca` is outside cylinder `cb`.
 * Sets `xzDist` to the xz distance between the centers of the cylinders.
 * Sets `deadSpace` to the mininum space between the cylinders not occupied by the other.
 */
s32 Math3D_CylOutsideCylDist(Cylinder16* ca, Cylinder16* cb, f32* deadSpace, f32* xzDist) {
    static Cylinderf caf;
    static Cylinderf cbf;

    Math_Vec3s_ToVec3f(&caf.pos, &ca->pos);
    caf.radius = ca->radius;
    caf.yShift = ca->yShift;
    caf.height = ca->height;

    Math_Vec3s_ToVec3f(&cbf.pos, &cb->pos);
    cbf.radius = cb->radius;
    cbf.yShift = cb->yShift;
    cbf.height = cb->height;

    *xzDist = sqrtf(SQ(caf.pos.x - cbf.pos.x) + SQ(caf.pos.z - cbf.pos.z));

    // The combined radix are within the xz distance
    if ((caf.radius + cbf.radius) < *xzDist) {
        return false;
    }

    // top of ca < bottom of cb or top of cb < bottom of ca
    if (((caf.pos.y + caf.yShift) + caf.height) < (cbf.pos.y + cbf.yShift) ||
        (((cbf.pos.y + cbf.yShift) + cbf.height) < (caf.pos.y + caf.yShift))) {
        return false;
    }

    *deadSpace = caf.radius + cbf.radius - *xzDist;
    return true;
}

/*
 * Determines if triangle `ta` intersects with triangle `tb` the point of
 * intersection is output to `intersect.
 * Returns 1 is the triangles intersect, 0 otherwise
 */

s32 Math3D_TriVsTriIntersect(TriNorm* ta, TriNorm* tb, Vec3f* intersect) {
    f32 dist0;
    f32 dist1;
    f32 dist2;

    dist0 = Math3D_Plane(&ta->plane, &tb->vtx[0]);
    dist1 = Math3D_Plane(&ta->plane, &tb->vtx[1]);
    dist2 = Math3D_Plane(&ta->plane, &tb->vtx[2]);

    if (((dist0 > 0.0f) && (dist1 > 0.0f) && (dist2 > 0.0f)) ||
        (((dist0 < 0.0f) && (dist1 < 0.0f)) && (dist2 < 0.0f))) {
        return false;
    }

    dist0 = Math3D_Plane(&tb->plane, &ta->vtx[0]);
    dist1 = Math3D_Plane(&tb->plane, &ta->vtx[1]);
    dist2 = Math3D_Plane(&tb->plane, &ta->vtx[2]);

    if ((((dist0 > 0.0f) && (dist1 > 0.0f)) && (dist2 > 0.0f)) ||
        ((dist0 < 0.0f) && (dist1 < 0.0f) && (dist2 < 0.0f))) {
        return false;
    }

    if (Math3D_TriLineIntersect(&tb->vtx[0], &tb->vtx[1], &tb->vtx[2], tb->plane.normal.x, tb->plane.normal.y,
                                tb->plane.normal.z, tb->plane.originDist, &ta->vtx[0], &ta->vtx[1], intersect, 0)) {
        return true;
    }
    if (Math3D_TriLineIntersect(&tb->vtx[0], &tb->vtx[1], &tb->vtx[2], tb->plane.normal.x, tb->plane.normal.y,
                                tb->plane.normal.z, tb->plane.originDist, &ta->vtx[1], &ta->vtx[2], intersect, 0)) {
        return true;
    }
    if (Math3D_TriLineIntersect(&tb->vtx[0], &tb->vtx[1], &tb->vtx[2], tb->plane.normal.x, tb->plane.normal.y,
                                tb->plane.normal.z, tb->plane.originDist, &ta->vtx[2], &ta->vtx[0], intersect, 0)) {
        return true;
    }
    if (Math3D_TriLineIntersect(&ta->vtx[0], &ta->vtx[1], &ta->vtx[2], ta->plane.normal.x, ta->plane.normal.y,
                                ta->plane.normal.z, ta->plane.originDist, &tb->vtx[0], &tb->vtx[1], intersect,
                                0) == 1) {
        return true;
    }
    if (Math3D_TriLineIntersect(&ta->vtx[0], &ta->vtx[1], &ta->vtx[2], ta->plane.normal.x, ta->plane.normal.y,
                                ta->plane.normal.z, ta->plane.originDist, &tb->vtx[1], &tb->vtx[2], intersect,
                                0) == 1) {
        return true;
    }
    if (Math3D_TriLineIntersect(&ta->vtx[0], &ta->vtx[1], &ta->vtx[2], ta->plane.normal.x, ta->plane.normal.y,
                                ta->plane.normal.z, ta->plane.originDist, &tb->vtx[2], &tb->vtx[0], intersect,
                                0) == 1) {
        return true;
    }
    return false;
}

s32 Math3D_XZInSphere(Sphere16* sphere, f32 x, f32 z) {
    f32 xDiff;
    f32 zDiff;

    xDiff = sphere->center.x - x;
    zDiff = sphere->center.z - z;
    if ((SQ(xDiff) + SQ(zDiff)) <= SQ(sphere->radius)) {
        return true;
    }
    return false;
}

s32 Math3D_XYInSphere(Sphere16* sphere, f32 x, f32 y) {
    f32 xDiff;
    f32 yDiff;

    xDiff = sphere->center.x - x;
    yDiff = sphere->center.y - y;
    if ((SQ(xDiff) + SQ(yDiff)) <= SQ(sphere->radius)) {
        return true;
    }
    return false;
}

s32 Math3D_YZInSphere(Sphere16* sphere, f32 y, f32 z) {
    f32 yDiff;
    f32 zDiff;

    yDiff = sphere->center.y - y;
    zDiff = sphere->center.z - z;
    if ((SQ(yDiff) + SQ(zDiff)) <= SQ(sphere->radius)) {
        return true;
    }
    return false;
}

void Math3D_DrawSphere(PlayState* play, Sphere16* sph) {
}

void Math3D_DrawCylinder(PlayState* play, Cylinder16* cyl) {
}
