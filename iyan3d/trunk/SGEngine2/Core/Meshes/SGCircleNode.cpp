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
    drawMode = DRAW_MODE_LINES;
    vector< vertexData > mbvd;
    vector< unsigned short > mbi;

    addCircleWithAxis(1, noOfVertices, radius, mbvd, mbi);
    if(isAllAxis) {
        addCircleWithAxis(0, noOfVertices, radius, mbvd, mbi);
        addCircleWithAxis(2, noOfVertices, radius, mbvd, mbi);
    }

    this->mesh = new Mesh();
    mesh->addMeshBuffer(mbvd, mbi, 0);

    updateAbsoluteTransformation();
}

void SGCircleNode::addCircleWithAxis(int axis, int noOfVertices, float radius, vector< vertexData > &mbvd, vector< unsigned short > &mbi)
{
    int prevVertexCount = mbvd.size();
    double theta = 0.0;
    
    for(int i = 0; i < noOfVertices + 1; i++) {
        Vector3 direction;
        
        if(axis == 0)
            direction = Vector3(0.0, theta, 0.0).rotationToDirection().normalize();
        else if(axis == 1)
            direction = Vector3(-90.0, 0.0, theta).rotationToDirection().normalize();
        else if(axis == 2)
            direction = Vector3(theta, 0.0, 0.0).rotationToDirection().normalize();
        
        vertexData vert;
        vert.vertPosition = (direction * Vector3(radius));
        mbvd.push_back(vert);
        theta += 360.0 / noOfVertices;
        
        if(i != 0) {
            mbi.push_back(prevVertexCount + i - 1);
            mbi.push_back(prevVertexCount + i);
        }
    }
}

SGCircleNode::~SGCircleNode()
{    
}

void SGCircleNode::update()
{
    
}
