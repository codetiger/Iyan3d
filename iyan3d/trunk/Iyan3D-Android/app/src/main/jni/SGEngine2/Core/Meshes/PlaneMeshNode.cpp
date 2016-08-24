//
//  PlaneMeshNode.cpp
//  SGEngine2
//
//  Created by Vivek shivam on 20/03/15.
//  Copyright (c) 2014 Smackall Games Pvt Ltd. All rights reserved.
//

#include "PlaneMeshNode.h"

PlaneMeshNode::PlaneMeshNode(float aspectRatio)
{
    this->mesh = new Mesh();
    vector<Vector3> vertPosition;
    vector<Vector3> vertNormal;
    vector<Vector2> texCoord1;
    float width = 6.0 * aspectRatio;
    float xCoord = width/2.0;
    float yCoord = 3.0;
    //Front
    vertPosition.push_back(Vector3(xCoord,yCoord, 0.0)); // Right Top Corner
    vertPosition.push_back(Vector3(xCoord,-yCoord, 0.0)); // Right Bottom
    vertPosition.push_back(Vector3(-xCoord,-yCoord, 0.0)); // Left Bottom
    vertPosition.push_back(Vector3(-xCoord,yCoord, 0.0)); // Left Top Corner
    
    vertNormal.push_back(Vector3(0.0,0.0,1.0));
    vertNormal.push_back(Vector3(0.0,0.0,1.0));
    vertNormal.push_back(Vector3(0.0,0.0,1.0));
    vertNormal.push_back(Vector3(0.0,0.0,1.0));

    texCoord1.push_back(Vector2(0.0,0.0));
    texCoord1.push_back(Vector2(0.0,1.0));
    texCoord1.push_back(Vector2(1.0,1.0));
    texCoord1.push_back(Vector2(1.0,0.0));
    vector< vertexData > mbvd;
    vector< unsigned short > mbi;
    
    for(int i = 0; i < vertPosition.size();i++){
        vertexData vData;
        vData.vertPosition = vertPosition[i];
        vData.vertNormal = vertNormal[i];
        vData.texCoord1 = texCoord1[i];
        mbvd.push_back(vData);
    }
    vertPosition.clear();
    vertNormal.clear();
    texCoord1.clear();
    
    unsigned short indices[6] = {
        0,3,1,
        2,1,3
    };
    for(int i = 0; i < 6; i++)
        mbi.push_back(indices[i]);
    this->mesh->addMeshBuffer(mbvd, mbi, 0);
    updateAbsoluteTransformation();
}

PlaneMeshNode::~PlaneMeshNode()
{
    if(this->mesh)
        delete this->mesh;
}

void PlaneMeshNode::update()
{
    
}
