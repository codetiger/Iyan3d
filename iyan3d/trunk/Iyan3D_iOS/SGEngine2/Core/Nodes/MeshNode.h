//
//  MeshNode.h
//  SGEngine2
//
//  Created by Harishankar on 14/11/14.
//  Copyright (c) 2014 Smackall Games Pvt Ltd. All rights reserved.
//

#ifndef __SGEngine2__MeshNode__
#define __SGEngine2__MeshNode__

#include "../common/common.h"
#include "Node.h"
#include "../Meshes/Mesh.h"
#include "../Meshes/SkinMesh.h"

class MeshNode : public Node {
    
public:
    Mesh *mesh;
    
    MeshNode();
    virtual ~MeshNode();
    virtual Mesh* getMesh();
    virtual void update();
    virtual short getActiveMeshIndex(int index);
    virtual Mesh* getMeshByIndex(int index);
};

#endif /* defined(__SGEngine2__MeshNode__) */