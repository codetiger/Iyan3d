//
//  InstanceNode.h
//  SGEngine2
//
//  Created by Vivek on 21/01/15.
//  Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
//

#ifndef __SGEngine2__InstanceNode__
#define __SGEngine2__InstanceNode__

#include <iostream>
#include "../common/common.h"

class InstanceNode{

private:
        Mat4 AbsoluteTransformation;
        Vector3 position,scale,rotation;
public:
    InstanceNode();
    ~InstanceNode();
    Mat4 getAbsoluteTransformation();
    void setRotationInDegrees(Vector3 rotation);
    void setRotationInRadians(Vector3 rotation);
    void setPosition(Vector3 position);
    void setScale(Vector3 scale);
    Vector3 getRotationInRadians();
    Vector3 getRotationInDegrees();
    Vector3 getPosition();
    Vector3 getScale();
};

#endif /* defined(__SGEngine2__InstanceNode__) */
