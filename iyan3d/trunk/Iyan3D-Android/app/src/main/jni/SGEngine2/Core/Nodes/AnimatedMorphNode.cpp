//
//  AnimatedMorphNode.cpp
//  SGEngine2
//
//  Created by Vivek shivam on 11/10/1936 SAKA.
//  Copyright (c) 1936 SAKA Smackall Games Pvt Ltd. All rights reserved.
//

#include "AnimatedMorphNode.h"
#define MESH_1_ARRAY_INDEX 0
#define MESH_2_ARRAY_INDEX 1

AnimatedMorphNode::AnimatedMorphNode(){
    this->RigMesh = NULL;
    meshes = new vector<Mesh*>();
    type = NODE_TYPE_MORPH_SKINNED;
    interpolation = 0.0;
    ActiveMeshes[MESH_1_ARRAY_INDEX] = MESH_1_ARRAY_INDEX;
    ActiveMeshes[MESH_2_ARRAY_INDEX] = MESH_2_ARRAY_INDEX;
}
AnimatedMorphNode::~AnimatedMorphNode(){
    for(int i = 0;i < meshes->size();i++){
        if((*meshes)[i])
            delete (*meshes)[i];
    }
    MorphLimits.clear();
    if(meshes){
        meshes->clear();
        delete meshes;
    }
}
void AnimatedMorphNode::setRigMeshWithMorphValue(AnimatedMesh *RigMesh,float morphValue){
    setMesh(RigMesh);
    this->RigMesh = RigMesh;
    AddMeshWithMorphValue(RigMesh,morphValue);
}
void AnimatedMorphNode::AddMeshWithMorphValue(Mesh *mesh,float morphValue){
    if(this->RigMesh == NULL){
        Logger::log(ERROR,"AnimatedMorphNode", "Set Rigged Mesh Before Adding Other Meshes");
        return;
    }
    meshes->push_back(mesh);
    MorphLimits.push_back(morphValue);
}
Mesh* AnimatedMorphNode::getMeshByIndex(u16 index){
    return (Mesh*)((*meshes)[index]);
}
short AnimatedMorphNode::getActiveMeshIndex(int index){
    return ActiveMeshes[index];
}
AnimatedMesh* AnimatedMorphNode::getMesh(){
    return RigMesh;
}
u16 AnimatedMorphNode::getMeshCount(){
    return meshes->size();
}
void AnimatedMorphNode::update(){
    AnimatedMeshNode::update();
    for (int i = 0;i < meshes->size() - 1;i++){
        if(interpolation >= MorphLimits[i] && interpolation < MorphLimits[i + 1]){
            ActiveMeshes[MESH_1_ARRAY_INDEX] = i;
            ActiveMeshes[MESH_2_ARRAY_INDEX] = i + 1;
            break;
        }
    }
}
float AnimatedMorphNode::getInterpolationValue(){
    return interpolation;
}
void AnimatedMorphNode::setInterpolationValue(float value){
    interpolation = value;
}
bool AnimatedMorphNode::IsValidMeshIndex(short meshIndex1,short meshIndex2){
    if(meshIndex1 >= meshes->size()){
        Logger::log(ERROR,"AnimatedMorphNode", "MeshIndex1 above Available Limit ");
        return false;
    }
    if(meshIndex2 >= meshes->size()){
        Logger::log(ERROR,"AnimatedMorphNode", "MeshIndex2 above Available Limit ");
        return false;
    }
    return true;
}
bool AnimatedMorphNode::IsValidVertexCount(int vertCount,short meshIndex){
    if((*meshes)[0]->getVerticesCount() == vertCount)
        return true;
    Logger::log(ERROR,"AnimatedMorphNode", "VerticesCount Not Matching For mehsIndex"+ to_string(meshIndex));
    return false;
}

