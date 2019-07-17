//
//  Plane3D.h
//  SGEngine2
//
//  Created by Vivek on 21/03/15.
//  Copyright (c) 2015 Vivek. All rights reserved.
//

#ifndef __SGEngine2__Plane3D__
#define __SGEngine2__Plane3D__

#include <iostream>
#include "common.h"

class Plane3D {
private:
public:
    Plane3D();
    void setPositionAndNormal(Vector3 position, Vector3 normal);
    bool getIntersectionWithPlanes(Plane3D& o1, Plane3D& o2, Vector3& outPoint);
    bool getIntersectionWithPlane(Plane3D& other, Vector3& outLinePoint, Vector3& outLineVect);
    bool getIntersectionWithLine(Vector3& linePoint, Vector3& lineVect, Vector3& outIntersection);
    void setPosition(Vector3 pos);
    ~Plane3D();
    Vector3 position;
    Vector3 normal;

    float distanceFromOrigin;
};

#endif /* defined(__SGEngine2__Plane3D__) */
