//
//  Frustum.h
//  SGEngine2
//
//  Created by Vivek on 21/03/15.
//  Copyright (c) 2015 Vivek. All rights reserved.
//

#include "Frustum.h"
Frustum::Frustum()
{
}

Frustum::~Frustum()
{
}

void Frustum::constructWithProjViewMatrix(Mat4 ProjViewMat)
{
    planes[F_LEFT_PLANE].normal.x = ProjViewMat[3] + ProjViewMat[0];
    planes[F_LEFT_PLANE].normal.y = ProjViewMat[7] + ProjViewMat[4];
    planes[F_LEFT_PLANE].normal.z = ProjViewMat[11] + ProjViewMat[8];
    planes[F_LEFT_PLANE].distanceFromOrigin = ProjViewMat[15] + ProjViewMat[12];

    // right clipping plane
    planes[F_RIGHT_PLANE].normal.x = ProjViewMat[3] - ProjViewMat[0];
    planes[F_RIGHT_PLANE].normal.y = ProjViewMat[7] - ProjViewMat[4];
    planes[F_RIGHT_PLANE].normal.z = ProjViewMat[11] - ProjViewMat[8];
    planes[F_RIGHT_PLANE].distanceFromOrigin = ProjViewMat[15] - ProjViewMat[12];

    // top clipping plane
    planes[F_TOP_PLANE].normal.x = ProjViewMat[3] - ProjViewMat[1];
    planes[F_TOP_PLANE].normal.y = ProjViewMat[7] - ProjViewMat[5];
    planes[F_TOP_PLANE].normal.z = ProjViewMat[11] - ProjViewMat[9];
    planes[F_TOP_PLANE].distanceFromOrigin = ProjViewMat[15] - ProjViewMat[13];

    // bottom clipping plane
    planes[F_BOTTOM_PLANE].normal.x = ProjViewMat[3] + ProjViewMat[1];
    planes[F_BOTTOM_PLANE].normal.y = ProjViewMat[7] + ProjViewMat[5];
    planes[F_BOTTOM_PLANE].normal.z = ProjViewMat[11] + ProjViewMat[9];
    planes[F_BOTTOM_PLANE].distanceFromOrigin = ProjViewMat[15] + ProjViewMat[13];

    // far clipping plane
    planes[F_FAR_PLANE].normal.x = ProjViewMat[3] - ProjViewMat[2];
    planes[F_FAR_PLANE].normal.y = ProjViewMat[7] - ProjViewMat[6];
    planes[F_FAR_PLANE].normal.z = ProjViewMat[11] - ProjViewMat[10];
    planes[F_FAR_PLANE].distanceFromOrigin = ProjViewMat[15] - ProjViewMat[14];

    // near clipping plane
    planes[F_NEAR_PLANE].normal.x = ProjViewMat[2];
    planes[F_NEAR_PLANE].normal.y = ProjViewMat[6];
    planes[F_NEAR_PLANE].normal.z = ProjViewMat[10];
    planes[F_NEAR_PLANE].distanceFromOrigin = ProjViewMat[14];

    for (int i = 0; i != F_PLANE_COUNT; ++i) {
        Vector3 norm = planes[i].normal;
        float len = -(1.0 / sqrtf((norm.x * norm.x) + (norm.y * norm.y) + (norm.z * norm.z)));
        planes[i].normal *= len;
        planes[i].distanceFromOrigin *= len;
    }
}

Vector3 Frustum::getFarLeftUp()
{
    Vector3 p;
    planes[F_FAR_PLANE].getIntersectionWithPlanes(planes[F_TOP_PLANE], planes[F_LEFT_PLANE], p);
    return p;
}

Vector3 Frustum::getFarLeftDown()
{
    Vector3 p;
    planes[F_FAR_PLANE].getIntersectionWithPlanes(planes[F_BOTTOM_PLANE], planes[F_LEFT_PLANE], p);
    return p;
}

Vector3 Frustum::getFarRightUp()
{
    Vector3 p;
    planes[F_FAR_PLANE].getIntersectionWithPlanes(planes[F_TOP_PLANE], planes[F_RIGHT_PLANE], p);

    return p;
}

Vector3 Frustum::getFarRightDown()
{
    Vector3 p;
    planes[F_FAR_PLANE].getIntersectionWithPlanes(planes[F_BOTTOM_PLANE], planes[F_RIGHT_PLANE], p);

    return p;
}

Vector3 Frustum::getNearLeftUp()
{
    Vector3 p;
    planes[F_NEAR_PLANE].getIntersectionWithPlanes(planes[F_TOP_PLANE], planes[F_LEFT_PLANE], p);

    return p;
}

Vector3 Frustum::getNearLeftDown()
{
    Vector3 p;
    planes[F_NEAR_PLANE].getIntersectionWithPlanes(planes[F_BOTTOM_PLANE], planes[F_LEFT_PLANE], p);

    return p;
}

Vector3 Frustum::getNearRightUp()
{
    Vector3 p;
    planes[F_NEAR_PLANE].getIntersectionWithPlanes(planes[F_TOP_PLANE], planes[F_RIGHT_PLANE], p);

    return p;
}

Vector3 Frustum::getNearRightDown()
{
    Vector3 p;
    planes[F_NEAR_PLANE].getIntersectionWithPlanes(planes[F_BOTTOM_PLANE], planes[F_RIGHT_PLANE], p);

    return p;
}
