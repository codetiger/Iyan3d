//
//  CubeMeshNode.h
//  SGEngine2
//
//  Created by Harishankar on 14/11/14.
//  Copyright (c) 2014 Smackall Games Pvt Ltd. All rights reserved.
//

#ifndef __SGEngine2__CubeMeshNode__
#define __SGEngine2__CubeMeshNode__

#include "../common/common.h"

#include "../Nodes/MeshNode.h"

class CubeMeshNode : public MeshNode {
    
public:
    CubeMeshNode();
    ~CubeMeshNode();
    void update();
};

#endif /* defined(__SGEngine2__CubeMeshNode__) */
