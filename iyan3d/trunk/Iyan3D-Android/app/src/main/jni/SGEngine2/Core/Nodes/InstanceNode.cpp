//
//  InstanceNode.cpp
//  SGEngine2
//
//  Created by Vivek on 21/01/15.
//  Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
//

#include "InstanceNode.h"
InstanceNode::InstanceNode(shared_ptr<Node> node){
    original = node;
    type = NODE_TYPE_INSTANCED;
}

InstanceNode::~InstanceNode(){
    
    if(this->original) {
        for( int i = 0; i < this->original->instancedNodes.size(); i ++) {
            if(this->original->instancedNodes[i]->getID() == this->getID()){
                this->original->instancedNodes.erase(this->original->instancedNodes.begin() + i);
            }
        }
        
        this->original = shared_ptr<Node>();
    }
}

void InstanceNode::update()
{
    
}
