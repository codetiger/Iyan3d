//
//  EmptyNode.h
//  SGEngine2
//
//  Created by Vivek on 21/01/15.
//  Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
//

#ifndef __SGEngine2__EmptyNode__
#define __SGEngine2__EmptyNode__

#include <iostream>
#include "../common/common.h"
#include "Node.h"

class EmptyNode : public Node{
public:
    EmptyNode();
    ~EmptyNode();
    void update();
    void updateBoundingBox();
};

#endif /* defined(__SGEngine2__EmptyNode__) */
