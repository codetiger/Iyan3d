//
//  JointNode.cpp
//  SGEngine2
//
//  Created by Vivek shivam on 11/10/1936 SAKA.
//  Copyright (c) 1936 SAKA Smackall Games Pvt Ltd. All rights reserved.
//

#include "JointNode.h"

JointNode::JointNode()
{
    Parent = shared_ptr<JointNode>();
}

JointNode::~JointNode()
{
    
}

void JointNode::update()
{
    
}

void JointNode::updateBoundingBox()
{
    if(Parent)
        Parent->updateBoundingBox();
}
