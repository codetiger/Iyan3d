//
//  Frustum.h
//  SGEngine2
//
//  Created by Vivek on 21/03/15.
//  Copyright (c) 2015 Vivek. All rights reserved.
//

#ifndef __SGEngine2__Frustum__
#define __SGEngine2__Frustum__

#include <iostream>
#include "common.h"
#include "Plane3D.h"

class Frustum {

private:
public:
    Plane3D planes[F_PLANE_COUNT];

    Frustum();
    ~Frustum();
    void constructWithProjViewMatrix(Mat4 ProjViewMat);
    Vector3 getFarLeftUp();
    Vector3 getFarLeftDown();
    Vector3 getFarRightUp();
    Vector3 getFarRightDown();
    Vector3 getNearLeftUp();
    Vector3 getNearLeftDown();
    Vector3 getNearRightUp();
    Vector3 getNearRightDown();
};

#endif /* defined(__SGEngine2__Frustum__) */
