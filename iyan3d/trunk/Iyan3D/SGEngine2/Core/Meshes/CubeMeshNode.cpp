//
//  CubeMeshNode.cpp
//  SGEngine2
//
//  Created by Harishankar on 14/11/14.
//  Copyright (c) 2014 Smackall Games Pvt Ltd. All rights reserved.
//

#include "CubeMeshNode.h"
#include "../common/Vector3.h"

CubeMeshNode::CubeMeshNode() {
    this->mesh = new Mesh();

    // Front
    vector<Vector3> vertPosition;
    vector<Vector3> vertNormal;
    vector<Vector2> texCoord1;
    
    //Front
    vertPosition.push_back(Vector3(2.0, 2.0, 2.0));
    vertPosition.push_back(Vector3(2.0, -2.0, 2.0));
    vertPosition.push_back(Vector3(-2.0, -2.0, 2.0));
    vertPosition.push_back(Vector3(-2.0, 2.0, 2.0));
    
    vertNormal.push_back(Vector3(  0.0,  0.0, 1.0 ));
    vertNormal.push_back(Vector3(  0.0,  0.0, 1.0 ));
    vertNormal.push_back(Vector3(  0.0, 0.0, 1.0 ));
    vertNormal.push_back(Vector3(  0.0, 0.0, 1.0 ));

    // Right
    vertPosition.push_back(Vector3(-2.0, -2.0, 2.0));
    vertPosition.push_back(Vector3(-2.0, 2.0, 2.0));
    vertPosition.push_back(Vector3(-2.0, -2.0, -2.0));
    vertPosition.push_back(Vector3(-2.0, 2.0, -2.0));
    
    vertNormal.push_back(Vector3(  -1.0,  0.0, 0.0 ));
    vertNormal.push_back(Vector3( -1.0,  0.0, 0.0 ));
    vertNormal.push_back(Vector3(  -1.0, 0.0, 0.0 ));
    vertNormal.push_back(Vector3(  -1.0, 0.0,  0.0 ));

    
    //Left
    vertPosition.push_back(Vector3(2.0, 2.0, 2.0));
    vertPosition.push_back(Vector3(2.0, -2.0, 2.0));
    vertPosition.push_back(Vector3(2.0, 2.0, -2.0));
    vertPosition.push_back(Vector3(2.0, -2.0, -2.0));
    
    vertNormal.push_back(Vector3( 1.0,  0.0, 0.0 ));
    vertNormal.push_back(Vector3( 1.0,  0.0,  0.0 ));
    vertNormal.push_back(Vector3( 1.0, 0.0,  0.0 ));
    vertNormal.push_back(Vector3( 1.0, 0.0,  0.0 ));

    
    //Rear
    vertPosition.push_back(Vector3(2.0, 2.0, -2.0));
    vertPosition.push_back(Vector3(-2.0, 2.0, -2.0));
    vertPosition.push_back(Vector3(2.0, -2.0, -2.0));
    vertPosition.push_back(Vector3(-2.0, -2.0, -2.0));
    
    vertNormal.push_back(Vector3( 0.0,  0.0,  -1.0 ));
    vertNormal.push_back(Vector3( 0.0,  0.0,  -1.0 ));
    vertNormal.push_back(Vector3( 0.0, 0.0,  -1.0 ));
    vertNormal.push_back(Vector3(  0.0, 0.0,  -1.0 ));
    
    //Top
    vertPosition.push_back(Vector3(2.0, 2.0, 2.0));
    vertPosition.push_back(Vector3(-2.0, 2.0, 2.0));
    vertPosition.push_back(Vector3(2.0, 2.0, -2.0));
    vertPosition.push_back(Vector3(2.0, 2.0, -2.0));
    
    vertNormal.push_back(Vector3( 0.0, 1.0,  0.0 ));
    vertNormal.push_back(Vector3( 0.0, 1.0,  0.0 ));
    vertNormal.push_back(Vector3(  0.0, 1.0,  0.0 ));
    vertNormal.push_back(Vector3(  0.0, 1.0,  0.0 ));


    //Bottom
    vertPosition.push_back(Vector3(2.0, -2.0, 2.0));
    vertPosition.push_back(Vector3(-2.0, -2.0, 2.0));
    vertPosition.push_back(Vector3(2.0, -2.0, -2.0));
    vertPosition.push_back(Vector3(-2.0, -2.0, -2.0));
    
    vertNormal.push_back(Vector3( 0.0, -1.0, 0.0));
    vertNormal.push_back(Vector3( 0.0, -1.0, 0.0));
    vertNormal.push_back(Vector3( 0.0, -1.0, 0.0));
    vertNormal.push_back(Vector3( 0.0, -1.0, 0.0));


    texCoord1.push_back(Vector2(0.0, 1.0));
    texCoord1.push_back(Vector2(0.0, 0.0));
    texCoord1.push_back(Vector2(0.0, 1.0));
    texCoord1.push_back(Vector2(1.0, 0.0));
    
    texCoord1.push_back(Vector2(1.0, 1.0));
    texCoord1.push_back(Vector2(0.0, 1.0));
    texCoord1.push_back(Vector2(1.0, 0.0));
    texCoord1.push_back(Vector2(0.0, 0.0));
    
    texCoord1.push_back(Vector2(1.0, 1.0));
    texCoord1.push_back(Vector2(0.0, 1.0));
    texCoord1.push_back(Vector2(1.0, 0.0));
    texCoord1.push_back(Vector2(0.0, 0.0));
    
    texCoord1.push_back(Vector2(1.0, 1.0));
    texCoord1.push_back(Vector2(0.0, 1.0));
    texCoord1.push_back(Vector2(1.0, 0.0));
    texCoord1.push_back(Vector2(0.0, 0.0));
    
    texCoord1.push_back(Vector2(1.0, 1.0));
    texCoord1.push_back(Vector2(0.0, 1.0));
    texCoord1.push_back(Vector2(1.0, 0.0));
    texCoord1.push_back(Vector2(0.0, 0.0));

    texCoord1.push_back(Vector2(0.0, 1.0));
    texCoord1.push_back(Vector2(1.0, 1.0));
    texCoord1.push_back(Vector2(0.0, 0.0));
    texCoord1.push_back(Vector2(1.0, 0.0));

    
    for(int i = 0; i < vertPosition.size();i++){
        vertexData vData;
        vData.vertPosition = vertPosition[i];
        vData.vertNormal = vertNormal[i];
        vData.texCoord1 = texCoord1[i];
        this->mesh->addVertex(&vData);
    }
    vertPosition.clear();
    vertNormal.clear();
    texCoord1.clear();
    
    unsigned short indices[36] = {
        0,1,3,
        2,1,3,
        4,5,6,
        7,5,6,
        8,9,10,
        11,9,10,
        12,13,14,
        15,13,14,
        16,17,18,
        19,17,18,
        20,21,22,
        23,21,22
    };
    
    for(int i = 0; i < 36; i++)
        this->mesh->addToIndicesArray(indices[i]);
    updateAbsoluteTransformation();
}
CubeMeshNode::~CubeMeshNode() {
    if(this->mesh)
        delete this->mesh;
    this->mesh = NULL;
}
void CubeMeshNode::update(){
    
}
