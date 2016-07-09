//
//  AnimatedMeshNode.h
//  SGEngine2
//
//  Created by Vivek shivam on 11/10/1936 SAKA.
//  Copyright (c) 1936 SAKA Smackall Games Pvt Ltd. All rights reserved.
//

#ifndef __SGEngine2__AnimatedMeshNode__
#define __SGEngine2__AnimatedMeshNode__

#include <iostream>
#include "MeshNode.h"
#include "JointNode.h"
#include "../Meshes/SkinMesh.h"

typedef enum {
    NO_RIG,
    CHARACTER_RIG,
    TEXT_RIG
}rig_type;

class AnimatedMeshNode: public MeshNode {
private:
    vector< shared_ptr<JointNode> > jointNodes;
    
public:
    unsigned short totalFrames;
    unsigned short currentFrame;

    AnimatedMeshNode();
    ~AnimatedMeshNode();
    virtual AnimatedMesh* getMesh();
    Mesh* getMeshCache();
    void initializeMeshCache();
    void updateMeshCache();
    void updatePartOfMeshCache(int jointId = 0);
    void getAllPaintedVertices(SkinMesh *skinMesh , vector<int> &paintedVertices , int jointId);
    void setMesh(AnimatedMesh *mesh, rig_type rigType = NO_RIG);
    void updateBoundingBox();
    void update();
    virtual short getActiveMeshIndex(int index);
    virtual Mesh* getMeshByIndex(int index);
    shared_ptr<JointNode> getJointNode(int jointId);
    int getJointCount();
    void calculateJointTransforms(vertexDataHeavy *vertex , vector<Mat4> jointTransforms , Vector3 &vertPosition, Vector3 &vertNormal , rig_type rigType = CHARACTER_RIG);
    void calculateSingleJointTransforms(vertexDataHeavy* vertex, Mat4 jointTransform, int jointId, Vector3& vertPosition, Vector3& vertNormal , rig_type rigType = CHARACTER_RIG);
};


#endif /* defined(__SGEngine2__AnimatedMeshNode__) */
