//
//  AnimatedMorphNode.h
//  SGEngine2
//
//  Created by Vivek shivam on 11/10/1936 SAKA.
//  Copyright (c) 1936 SAKA Smackall Games Pvt Ltd. All rights reserved.
//

#ifndef __SGEngine2__AnimatedMorphNode__
#define __SGEngine2__AnimatedMorphNode__

#include <iostream>
#include "AnimatedMeshNode.h"

class AnimatedMorphNode : public AnimatedMeshNode{

public:
    AnimatedMesh *RigMesh;
    vector<Mesh*> *meshes;
    vector<float> MorphLimits;
    short ActiveMeshes[2];
    float interpolation;
    
    AnimatedMorphNode();
    ~AnimatedMorphNode();
    void setRigMeshWithMorphValue(AnimatedMesh *RigMesh,float morphValue);
    void AddMeshWithMorphValue(Mesh *mesh,float morphValue);
    Mesh* getMeshByIndex(u16 index);
    virtual AnimatedMesh* getMesh();
    u16 getMeshCount();
    virtual void update();
    float getInterpolationValue();
    void setInterpolationValue(float value);
    bool IsValidMeshIndex(short meshIndex1,short meshIndex2);
    bool IsValidVertexCount(int vertCount,short mehsIndex);
    short getActiveMeshIndex(int index);
};


#endif /* defined(__SGEngine2__AnimatedMorphNode__) */
