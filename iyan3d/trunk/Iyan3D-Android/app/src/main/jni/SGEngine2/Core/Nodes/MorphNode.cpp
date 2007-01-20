//
//  MeshNode.cpp
//  SGEngine2
//
//  Created by Harishankar on 14/11/14.
//  Copyright (c) 2014 Smackall Games Pvt Ltd. All rights reserved.
//

#include "MorphNode.h"

MorphNode::MorphNode(){
    meshes = new vector<Mesh*>();
    type = NODE_TYPE_MORPH;
    interpolation = 0.0;
    ActiveMeshes[MESH_1_ARRAY_INDEX] = MESH_1_ARRAY_INDEX;
    ActiveMeshes[MESH_2_ARRAY_INDEX] = MESH_2_ARRAY_INDEX;
}
MorphNode::~MorphNode(){
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
void MorphNode::AddMeshWithMorphValue(Mesh *mesh,float MorphLimit){
    meshes->push_back(mesh);
    MorphLimits.push_back(MorphLimit);
}
Mesh* MorphNode::getMeshByIndex(u16 index){
    return (*meshes)[index];
}
Mesh* MorphNode::getMesh(){
    return (*meshes)[0];
}
u16 MorphNode::getMeshCount(){
    return meshes->size();
}
void MorphNode::update(){
    for (int i = 0;i < meshes->size() - 1;i++){
        if(interpolation >= MorphLimits[i] && interpolation < MorphLimits[i + 1]){
            ActiveMeshes[MESH_1_ARRAY_INDEX] = i;
            ActiveMeshes[MESH_2_ARRAY_INDEX] = i + 1;
            break;
        }
    }
}
float MorphNode::getInterpolationValue(){
    return interpolation;
}
void MorphNode::setInterpolationValue(float value){
    interpolation = value;
}
bool MorphNode::IsValidMeshIndex(short meshIndex1,short meshIndex2){
    if(meshIndex1 >= meshes->size()){
        Logger::log(ERROR,"MorphNode", "MeshIndex1 above Available Limit ");
        return false;
    }
    if(meshIndex2 >= meshes->size()){
        Logger::log(ERROR,"MorphNode", "MeshIndex2 above Available Limit ");
        return false;
    }
    return true;
}
bool MorphNode::IsValidVertexCount(int vertCount,short meshIndex){
    if((*meshes)[0]->getVerticesCount() == vertCount)
        return true;
    Logger::log(ERROR,"MorphNode", "VerticesCount Not Matching For mehsIndex"+ to_string(meshIndex));
    return false;
}
short MorphNode::getActiveMeshIndex(int index){
    return ActiveMeshes[index];
}

