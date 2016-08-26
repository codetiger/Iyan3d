//
//  SkinMesh.h
//  SGEngine2
//
//  Created by Vivek shivam on 11/10/1936 SAKA.
//  Copyright (c) 1936 SAKA Smackall Games Pvt Ltd. All rights reserved.
//

#ifndef __SGEngine2__SkinMesh__
#define __SGEngine2__SkinMesh__

#include <iostream>
#include "Joint.h"
#include "../Nodes/JointNode.h"

class SkinMesh:public Mesh {
public:
    int versionId;
    
    SkinMesh();
    ~SkinMesh();
    vector<Joint*> *joints;
    vector<Joint*> *RootJoints;
    
    Joint* getJointById(short jointId);
    Joint* addJoint(Joint *parent);
    void buildAllGlobalAnimatedMatrices(Joint *joint,Joint *parentJoint);
    void transferJointsToMesh(vector< shared_ptr<JointNode> > joints);
    void recoverJointsFromMesh(vector< shared_ptr<JointNode> > jointNodes);
    void createJointNodes(vector< shared_ptr<JointNode> > jointNodes);
    void finalize();
    void reverseJointsOrder();
};

#endif /* defined(__SGEngine2__SkinMesh__) */
