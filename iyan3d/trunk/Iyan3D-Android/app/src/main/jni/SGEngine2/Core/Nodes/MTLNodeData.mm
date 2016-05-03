//
//  OGLES2RenderManager.cpp
//  SGEngine2
//
//  Created by Harishankar on 15/11/14.
//  Copyright (c) 2014 Smackall Games Pvt Ltd. All rights reserved.
//

#import "MTLNodeData.h"

MTLNodeData::MTLNodeData(){
    
}
MTLNodeData::~MTLNodeData(){
    if(VertexBuffers) {
        [VertexBuffers removeAllObjects];
        VertexBuffers = nil;
    }
    
    if(indexBuffers) {
        [indexBuffers removeAllObjects];
        indexBuffers = nil;
    }
    
    VAOCreated = false;
}
shared_ptr<void> initMetalNodeData(){
    return make_shared<MTLNodeData>();
}
void MTLNodeData::removeVertexBuffers()
{
    
}
