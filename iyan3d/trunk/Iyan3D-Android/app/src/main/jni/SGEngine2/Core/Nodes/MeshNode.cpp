//
//  MeshNode.cpp
//  SGEngine2
//
//  Created by Harishankar on 14/11/14.
//  Copyright (c) 2014 Smackall Games Pvt Ltd. All rights reserved.
//

#include "MeshNode.h"

MeshNode::MeshNode() {
    type = NODE_TYPE_MESH;
}
MeshNode::~MeshNode() {
    if(mesh && mesh->getVerticesCount())
        delete mesh;
    mesh = NULL;
}
Mesh* MeshNode::getMesh(){
    return this->mesh;
}
void MeshNode::update(){
    
}
short MeshNode::getActiveMeshIndex(int index){
    return index;
}
Mesh* MeshNode::getMeshByIndex(int index){
    return mesh;
}
