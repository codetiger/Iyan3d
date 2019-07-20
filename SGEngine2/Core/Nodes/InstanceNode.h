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
#include "MeshNode.h"

class InstanceNode : public MeshNode {
private:
    Mat4    AbsoluteTransformation;
    Vector3 position, scale, rotation;

public:
    InstanceNode(shared_ptr<Node> node);
    virtual ~InstanceNode();

    virtual void update();
};

#endif /* defined(__SGEngine2__InstanceNode__) */
