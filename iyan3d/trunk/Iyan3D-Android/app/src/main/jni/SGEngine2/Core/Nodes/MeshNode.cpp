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

void MeshNode::updateBoundingBox()
{
    if(!mesh)
        return;
    
    BoundingBox bb;
    BoundingBox meshBoundingBox = *this->mesh->getBoundingBox();
    
    if(Children->size() <= 0) {
        for (int i = 0; i < 8; i++) {
            Vector3 edge = meshBoundingBox.getEdgeByIndex(i);
            Vector4 newEdge = AbsoluteTransformation *  Vector4(edge.x, edge.y, edge.z, 1.0);
            bb.addPointsToCalculateBoundingBox(Vector3(newEdge.x, newEdge.y, newEdge.z));
        }
    }

    if(Children->size()){
        for(unsigned short i = 0; i < Children->size();i++){
            if((*Children)[i]) {
                (*Children)[i]->updateBoundingBox();
                if((*Children)[i]->getBoundingBox().isValid()) {
                    for (int j = 0; j < 8; j++) {
                        Vector3 edge = (*Children)[i]->getBoundingBox().getEdgeByIndex(j);
                        bb.addPointsToCalculateBoundingBox(edge);
                    }
                }
            }
        }
    }
    bb.calculateEdges();
    bBox = bb;
}
