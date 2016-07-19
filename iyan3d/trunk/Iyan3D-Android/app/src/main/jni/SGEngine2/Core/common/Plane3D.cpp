//
//  Plane3D.h
//  SGEngine2
//
//  Created by Vivek on 21/03/15.
//  Copyright (c) 2015 Vivek. All rights reserved.
//

#include "Plane3D.h"
Plane3D::Plane3D()
{
}

void Plane3D::setPositionAndNormal(Vector3 position, Vector3 normal)
{
    this->normal = normal;
    distanceFromOrigin = -position.dotProduct(normal);
}

Plane3D::~Plane3D()
{
}

bool Plane3D::getIntersectionWithPlanes(Plane3D& o1, Plane3D& o2, Vector3& outPoint)
{
    Vector3 linePoint, lineVect;
    if (getIntersectionWithPlane(o1, linePoint, lineVect))
        return o2.getIntersectionWithLine(linePoint, lineVect, outPoint);

    return false;
}

bool Plane3D::getIntersectionWithPlane(Plane3D& other, Vector3& outLinePoint, Vector3& outLineVect)
{
    float fn00 = normal.getLength();
    float fn01 = normal.dotProduct(other.normal);
    float fn11 = other.normal.getLength();
    double det = fn00 * fn11 - fn01 * fn01;

    if (fabs(det) < ROUNDING_ERROR_f64)
        return false;

    double invdet = 1.0 / det;
    double fc0 = (fn11 * -distanceFromOrigin + fn01 * other.distanceFromOrigin) * invdet;
    double fc1 = (fn00 * -other.distanceFromOrigin + fn01 * distanceFromOrigin) * invdet;

    outLineVect = normal.crossProduct(other.normal);
    outLinePoint = normal * (double)fc0 + other.normal * (double)fc1;
    return true;
}

bool Plane3D::getIntersectionWithLine(Vector3& linePoint, Vector3& lineVect, Vector3& outIntersection)
{
    float t2 = normal.dotProduct(lineVect);
    if (t2 == 0)
        return false;
    float t = -(normal.dotProduct(linePoint) + distanceFromOrigin) / t2;
    outIntersection = linePoint + (lineVect * t);
    return true;
}

void Plane3D::setPosition(Vector3 pos)
{
    distanceFromOrigin = -position.dotProduct(normal);
}
