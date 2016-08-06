//
//  SGCircleNode
//  SGEngine2
//
//  Created by vivek on 10/05/15.
//  Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
//

#include "SGCircleNode.h"

SGCircleNode::SGCircleNode(int noOfVertices, float radius, bool isAllAxis)
{
    drawMode = DRAW_MODE_LINE_LOOP;
    this->mesh = new Mesh();
    
    addCircleWithAxis(mesh, 1, noOfVertices, radius);
    if(isAllAxis) {
        addCircleWithAxis(mesh, 0, noOfVertices, radius);
        addCircleWithAxis(mesh, 2, noOfVertices, radius);
    }
    
    updateAbsoluteTransformation();
}

void SGCircleNode::addCircleWithAxis(Mesh *m, int axis, int noOfVertices, float radius)
{
    int prevVertexCount = m->getVerticesCount();
    double theta = 0.0;
    
    for(int i = 0; i < noOfVertices + 1; i++) {
        vertexData vert;
        Vector3 direction;
        
        if(axis == 0)
            direction = Vector3(0.0, theta, 0.0).rotationToDirection().normalize();
        else if(axis == 1)
            direction = Vector3(-90.0, 0.0, theta).rotationToDirection().normalize();
        else if(axis == 2)
            direction = Vector3(theta, 0.0, 0.0).rotationToDirection().normalize();
        
        vert.vertPosition = (direction * Vector3(radius));
        m->addVertex(&vert);
        theta += 360.0 / noOfVertices;
    }

    for(int i = 0; i < noOfVertices + 1; i++) {
        m->addToIndicesArray(prevVertexCount + i);
    }
    
    m->setOptimization(false, false, false);
}

SGCircleNode::~SGCircleNode()
{
    if(this->mesh)
        delete this->mesh;
    
}

void SGCircleNode::update()
{
    
}
