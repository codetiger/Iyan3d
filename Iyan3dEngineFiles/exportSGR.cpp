//
//  exportSGR.cpp
//  Iyan3D
//
//  Created by vivek on 6/11/14.
//  Copyright (c) 2014 Smackall Games. All rights reserved.
//

#include "HeaderFiles/exportSGR.h"

exportSGR::exportSGR()
{
    
}
exportSGR::~exportSGR()
{
    
}
bool exportSGR::writeSGR(std::string filePath,shared_ptr<AnimatedMeshNode> node)
{
    ofstream filePointer(filePath,ios::out | ios::binary);
    FileHelper::resetSeekPosition();
    
    SkinMesh *skinnedMesh = (SkinMesh*)node->getMesh();
    Mesh *mesh = node->getMesh();
    unsigned int vertexCount = mesh->getVerticesCountInMeshBuffer(0);
    u16 versionIdentifier = 0; // 0 for HighPoly models
    FileHelper::writeShort(&filePointer, versionIdentifier);// write VertexCount
    FileHelper::writeInt(&filePointer, vertexCount);// write VertexCount
    for(int i = 0; i < vertexCount; i++){
        vertexData *vertex = mesh->getLiteVerticesForMeshBuffer(0, i);
        Mat4 vertexMat;
        vertexMat.translate(vertex->vertPosition);
        vertexMat = node->getAbsoluteTransformation() * vertexMat;
        Vector3 vertexPosition = vertexMat.getTranslation();
        
        // write vertex position,normal,UV Coords
        FileHelper::writeFloat(&filePointer,vertexPosition.x);
        FileHelper::writeFloat(&filePointer,vertexPosition.y);
        FileHelper::writeFloat(&filePointer,vertexPosition.z);
        FileHelper::writeFloat(&filePointer,vertex->vertNormal.x);
        FileHelper::writeFloat(&filePointer,vertex->vertNormal.y);
        FileHelper::writeFloat(&filePointer,vertex->vertNormal.z);
        FileHelper::writeFloat(&filePointer,vertex->texCoord1.x);
        FileHelper::writeFloat(&filePointer,1 - vertex->texCoord1.y);
    }
    
    
    unsigned int indicesCount = mesh->getIndicesCount(0);
    FileHelper::writeInt(&filePointer, indicesCount); // write IndexCount
    
    unsigned short *indicesArr = mesh->getIndicesArray(0);
    for(int k = 0; k < indicesCount; k++) {
        unsigned int index = indicesArr[k];
        FileHelper::writeInt(&filePointer, index);    // write Vertex Index
    }
    
    unsigned short boneCount = node->getJointCount();
    FileHelper::writeShort(&filePointer, boneCount);    // write boneCount
    
    for(int j = 0; j < boneCount; j++) {
        if(j == 0)
            FileHelper::writeShort(&filePointer, -1); // write -1 for pivot bone
        else if(j == 1)
            FileHelper::writeShort(&filePointer, 0); // write 0 for hip bone
        else
            FileHelper::writeShort(&filePointer, node->getJointNode(j)->getParent()->getID()); // write bone ParentId
        
        Mat4 jointMatrix = (*skinnedMesh->joints)[j]->LocalAnimatedMatrix;
        for(int k = 0;k < 16;k++) // write jointMatrix
            FileHelper::writeFloat(&filePointer, jointMatrix[k]);
        
        unsigned short boneWeightCount = (*skinnedMesh->joints)[j]->PaintedVertices->size();
        FileHelper::writeShort(&filePointer, boneWeightCount); // write boneWeightCount
        
        for(int w = 0; w < boneWeightCount; w++) {    // write vertexIndex and strength
            unsigned int vertexIndex = (*(*skinnedMesh->joints)[j]->PaintedVertices)[w]->vertexId;
            unsigned short strength = (*(*skinnedMesh->joints)[j]->PaintedVertices)[w]->weight * 255.0;
            FileHelper::writeInt(&filePointer, vertexIndex);
            FileHelper::writeShort(&filePointer, strength);
        }
    }
    //node.reset();
    return true;
}
