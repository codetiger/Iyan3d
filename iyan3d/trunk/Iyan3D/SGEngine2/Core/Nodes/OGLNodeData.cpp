//
//  OGLNodeData.cpp
//  SGEngine2
//
//  Created by Vivek on 27/12/14.
//  Copyright (c) 2014 Smackall Games Pvt Ltd. All rights reserved.
//

#include "OGLNodeData.h"

OGLNodeData::OGLNodeData(){
    IndexBufLocations.clear();
    vertexBufLocations.clear();
}
OGLNodeData::~OGLNodeData(){
    for(int i = 0; i < vertexBufLocations.size();i++)
        glDeleteBuffers(1,&vertexBufLocations[i]);
    
    for(int i = 0; i < IndexBufLocations.size();i++)
        glDeleteBuffers(1,&IndexBufLocations[i]);
    
    vertexBufLocations.clear();
    IndexBufLocations.clear();
}
void OGLNodeData::removeVertexBuffers()
{
    for(int i = 0; i < vertexBufLocations.size();i++)
        glDeleteBuffers(1,&vertexBufLocations[i]);
    vertexBufLocations.clear();
}
