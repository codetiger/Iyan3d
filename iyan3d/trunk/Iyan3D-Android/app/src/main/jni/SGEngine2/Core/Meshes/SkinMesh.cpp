//
//  SkinMesh.cpp
//  SGEngine2
//
//  Created by Vivek shivam on 11/10/1936 SAKA.
//  Copyright (c) 1936 SAKA Smackall Games Pvt Ltd. All rights reserved.
//

#include "SkinMesh.h"
SkinMesh::SkinMesh()
{
    versionId = NOT_EXISTS;
    joints = new vector<Joint*>();
    RootJoints = new vector<Joint*>();
    meshType = MESH_TYPE_HEAVY;
}

SkinMesh::~SkinMesh()
{
    for(int i = 0; i < joints->size();i++){
        if((*joints)[i])
            delete (*joints)[i];
    }

    // no need to delete rootjoints after delete joints
    if(joints)
        delete joints;

    if(RootJoints)
        delete RootJoints;
}

void SkinMesh::buildAllGlobalAnimatedMatrices(Joint *joint, Joint *parentJoint)
{
    if (!joint) {
        
        for (int i = 0; i < RootJoints->size(); ++i)
            buildAllGlobalAnimatedMatrices((*RootJoints)[i], 0);

        return;
        
    } else {
        
        if (!parentJoint)
            joint->GlobalAnimatedMatrix = joint->LocalAnimatedMatrix;
        else
            joint->GlobalAnimatedMatrix = parentJoint->GlobalAnimatedMatrix * joint->LocalAnimatedMatrix;
    }
    
    for (int j = 0; j < joint->childJoints->size(); ++j)
        buildAllGlobalAnimatedMatrices((*joint->childJoints)[j], joint);
}

void SkinMesh::transferJointsToMesh(vector< shared_ptr<JointNode> > jointNodes)
{
    for (int i = 0; i < joints->size(); ++i) {
        shared_ptr<JointNode>  node = jointNodes[i];
        Joint *joint = (*joints)[i];
        joint->LocalAnimatedMatrix = Mat4();
        joint->LocalAnimatedMatrix.setRotation(node->getRotation());
        joint->LocalAnimatedMatrix.translate(node->getPosition());
        joint->LocalAnimatedMatrix.scale(node->getScale());
    }
}

void SkinMesh::recoverJointsFromMesh(vector< shared_ptr<JointNode> > jointNodes)
{
    for (int i = 0; i < jointNodes.size(); i++) {
        shared_ptr<JointNode> node= jointNodes[i];
        Joint *joint=(*joints)[i];
        node->setPosition(joint->LocalAnimatedMatrix.getTranslation());
        node->setRotation(joint->LocalAnimatedMatrix.getRotation());
        node->setScale(joint->LocalAnimatedMatrix.getScale());
        node->updateAbsoluteTransformationOfChildren();
    }
}

Joint* SkinMesh::addJoint(Joint *parent)
{
    Joint *joint =  new Joint();
    
    if (parent)
        parent->childJoints->push_back(joint);
    
    joint->Parent = parent;
    joint->Index = (joints->size() + 1) - 1;
    joints->push_back(joint);
    return joint;
}

void SkinMesh::finalize()
{
    vector< Joint* > *reOrderedBones = new vector< Joint* >();
    
    for(int i = 0; i < joints->size(); i++) {
        if(!(*joints)[i]->Parent) {
            RootJoints->push_back((*joints)[i]);
            reOrderedBones->push_back((*joints)[i]);
            (*joints)[i]->Index = reOrderedBones->size() - 1;
        }
    }
    
    for(int i = 0; i < RootJoints->size(); i++) {
        addChildrenJoints((*RootJoints)[i], reOrderedBones);
    }
    
    joints->clear();
    delete joints;
    joints = reOrderedBones;
    
    buildAllGlobalAnimatedMatrices(NULL,NULL);
    
    for(int i = 0; i < joints->size(); i++){
        Joint *joint = (*joints)[i];
        joint->GlobalInversedMatrix = joint->GlobalAnimatedMatrix;
        joint->GlobalInversedMatrix.invert();
    }
}

void SkinMesh::addChildrenJoints(Joint* parent, vector< Joint* > *reOrderedBones)
{
    for(int i = 0; i < parent->childJoints->size(); i++) {
        reOrderedBones->push_back((*parent->childJoints)[i]);
        (*parent->childJoints)[i]->Index = reOrderedBones->size() - 1;
        addChildrenJoints((*parent->childJoints)[i], reOrderedBones);
    }
}

