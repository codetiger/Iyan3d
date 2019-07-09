//
//  OGLNodeData.h
//  SGEngine2
//
//  Created by Vivek on 27/12/14.
//  Copyright (c) 2014 Smackall Games Pvt Ltd. All rights reserved.
//

#ifndef __SGEngine2__OGLNodeData__
#define __SGEngine2__OGLNodeData__

#include <stdio.h>
#include <iostream>
#include "APIData.h"
#include "../Material/OGLMaterial.h"

class OGLNodeData: public APIData{
public:
    bool VAOCreated;
    std::vector<u_int32_t> vertexArrayLocations;
    std::vector<u_int32_t> vertexBufLocations;
    std::vector<u_int32_t> IndexBufLocations;
    OGLNodeData();
    ~OGLNodeData();
    void setShaderMaterial(Material *mat);
    void removeVertexBuffers();

};

#endif /* defined(__SGEngine2__OGLNodeData__) */
