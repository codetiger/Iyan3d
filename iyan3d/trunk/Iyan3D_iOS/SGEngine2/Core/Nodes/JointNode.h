//
//  JointNode.h
//  SGEngine2
//
//  Created by Vivek shivam on 11/10/1936 SAKA.
//  Copyright (c) 1936 SAKA Smackall Games Pvt Ltd. All rights reserved.
//

#ifndef __SGEngine2__JointNode__
#define __SGEngine2__JointNode__

#include <iostream>
#include "Node.h"
class JointNode:public Node{
    
public:
    JointNode();
    ~JointNode();
    
    void update();
};

#endif /* defined(__SGEngine2__JointNode__) */
