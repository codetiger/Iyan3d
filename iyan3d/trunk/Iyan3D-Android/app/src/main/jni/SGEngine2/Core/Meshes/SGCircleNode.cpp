//
//  SGCircleNode
//  SGEngine2
//
//  Created by vivek on 10/05/15.
//  Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
//

#include "SGCircleNode.h"

SGCircleNode::SGCircleNode(int noOfVertices,int radius) {
    drawMode = DRAW_MODE_LINES;
    this->mesh = new Mesh();
    double theta = 0.0;
    for(int i = 0; i < noOfVertices+1; i++){
        vertexData vert;
        Vector3 direction = Vector3(-90.0,0.0,theta).rotationToDirection().normalize();
        vert.vertPosition = (direction * Vector3(radius));
        mesh->addVertex(&vert);
        theta += 360.0 / noOfVertices;
    }
    for(int i = 0; i < noOfVertices+1; i++){
        mesh->addToIndicesArray(i);
        if(i > 0)
            mesh->addToIndicesArray(i);
    }
    mesh->addToIndicesArray(0);
    updateAbsoluteTransformation();
}
SGCircleNode::~SGCircleNode() {
    if(this->mesh)
        delete this->mesh;
    
}
void SGCircleNode::update(){
    
}
