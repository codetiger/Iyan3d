//
//  PlaneMeshNode.h
//  SGEngine2
//
//  Created by Vivek shivam on 20/03/15.
//  Copyright (c) 2014 Smackall Games Pvt Ltd. All rights reserved.
//

#ifndef __SGEngine2__PlaneMeshNode__
#define __SGEngine2__PlaneMeshNode__

#include "../common/common.h"
#include "../Nodes/MeshNode.h"

class PlaneMeshNode : public MeshNode {
    
public:
    PlaneMeshNode(float aspectRatio);
    ~PlaneMeshNode();
    void update();
};

#endif /* defined(__SGEngine2__PlaneMeshNode__) */
