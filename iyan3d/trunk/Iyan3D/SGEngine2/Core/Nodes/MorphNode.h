//
//  MorphNode.h
//  SGEngine2
//
//  Created by Vivek on 09/02/15.
//  Copyright (c) 2014 Smackall Games Pvt Ltd. All rights reserved.
//

#ifndef __SGEngine2__MorphNode__
#define __SGEngine2__MorphNode__

#include "../common/common.h"
#include "MeshNode.h"
#include "../Meshes/SkinMesh.h"
#include <map>
#define MESH_1_ARRAY_INDEX 0
#define MESH_2_ARRAY_INDEX 1
#define MORPH_MAX_BUF_DATA 2

class MorphNode : public MeshNode {
    vector<Mesh*> *meshes;
    vector<float> MorphLimits;
    short ActiveMeshes[2];
    float interpolation;
public:
    
    MorphNode();
    ~MorphNode();
    void AddMeshWithMorphValue(Mesh *mesh,float MorphLimit);
    Mesh* getMeshByIndex(u16 index);
    virtual Mesh* getMesh();
    u16 getMeshCount();
    virtual void update();
    float getInterpolationValue();
    void setInterpolationValue(float value);
    bool IsValidMeshIndex(short meshIndex1,short meshIndex2);
    bool IsValidVertexCount(int vertCount,short mehsIndex);
    short getActiveMeshIndex(int index);
};

#endif /* defined(__SGEngine2__MorphNode__) */
