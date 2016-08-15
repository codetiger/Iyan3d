//
//  MeshRW.cpp
//  Iyan3D
//
//  Created by Karthik on 04/08/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#include "MeshRW.h"
#include "FileHelper.h"

void MeshRW::writeMeshData(ofstream *filePointer, Mesh *mesh)
{
    int meshBufferCount = mesh->getMeshBufferCount();
    
    FileHelper::writeInt(filePointer, meshBufferCount);
    
    for(int i = 0; i < meshBufferCount; i++) {
        
        vector< vertexData > vertices = mesh->getLiteVerticesArray(i);
        unsigned short* indices = mesh->getIndicesArray(i);
        int verticesCount = (int)vertices.size();
        int indicesCount = mesh->getIndicesCount(i);

        FileHelper::writeInt(filePointer, verticesCount);
        
        for(int j = 0; j < verticesCount; j++) {
            vertexData v = vertices[j];
            FileHelper::writeVector3(filePointer, v.vertPosition);
            FileHelper::writeVector3(filePointer, v.vertNormal);
            FileHelper::writeVector2(filePointer, v.texCoord1);
            FileHelper::writeVector3(filePointer, v.vertTangent);
            FileHelper::writeVector3(filePointer, v.vertBitangent);
            FileHelper::writeVector4(filePointer, v.optionalData1);
        }
        
        FileHelper::writeInt(filePointer, indicesCount);
        
        for(int j = 0; j < indicesCount; j++) {
            FileHelper::writeInt(filePointer, indices[j]);
        }
    }
}

void MeshRW::writeSkinMeshData(ofstream *filePointer, Mesh* mesh, shared_ptr< AnimatedMeshNode > aNode)
{
    SkinMesh *skinnedMesh = (SkinMesh*)mesh;

    int meshBufferCount = mesh->getMeshBufferCount();
    
    FileHelper::writeInt(filePointer, meshBufferCount);
    
    for(int i = 0; i < meshBufferCount; i++) {
        
        vector< vertexDataHeavy > vertices = mesh->getHeavyVerticesArray(i);
        unsigned short* indices = mesh->getIndicesArray(i);
        int verticesCount = (int)vertices.size();
        int indicesCount = mesh->getIndicesCount(i);
        
        FileHelper::writeInt(filePointer, verticesCount);
        
        for(int j = 0; j < verticesCount; j++) {
            vertexDataHeavy v = vertices[j];
            FileHelper::writeVector3(filePointer, v.vertPosition);
            FileHelper::writeVector3(filePointer, v.vertNormal);
            FileHelper::writeVector2(filePointer, v.texCoord1);
            FileHelper::writeVector3(filePointer, v.vertTangent);
            FileHelper::writeVector3(filePointer, v.vertBitangent);
            FileHelper::writeVector4(filePointer, v.optionalData1);
            FileHelper::writeVector4(filePointer, v.optionalData2);
            FileHelper::writeVector4(filePointer, v.optionalData3);
            FileHelper::writeVector4(filePointer, v.optionalData4);

        }
        
        FileHelper::writeInt(filePointer, indicesCount);
        
        for(int j = 0; j < indicesCount; j++) {
            FileHelper::writeInt(filePointer, indices[j]);
        }
        
     }

    int boneCount = aNode->getJointCount();
    FileHelper::writeInt(filePointer, boneCount);
    
    for(int j = 0; j < boneCount; j++) {
        if(j == 0)
            FileHelper::writeInt(filePointer, -1); // write -1 for pivot bone
        else if(j == 1)
            FileHelper::writeInt(filePointer, 0); // write 0 for hip bone
        else
            FileHelper::writeInt(filePointer, aNode->getJointNode(j)->getParent()->getID()); // write bone ParentId
        
        Mat4 jointMatrix = (*skinnedMesh->joints)[j]->LocalAnimatedMatrix;
        for(int k = 0;k < 16;k++) // write jointMatrix
            FileHelper::writeFloat(filePointer, jointMatrix[k]);
        
        unsigned short boneWeightCount = (*skinnedMesh->joints)[j]->PaintedVertices->size();
        //printf("\n weight %d %d %d",j,node->getJointNode(j)->getParent()->getID(),boneWeightCount);
        FileHelper::writeInt(filePointer, boneWeightCount); // write boneWeightCount
        
        for(int w = 0;w < boneWeightCount;w++){	// write vertexIndex and strength
            unsigned int vertexIndex = (*(*skinnedMesh->joints)[j]->PaintedVertices)[w]->vertexId;
            unsigned short strength = (*(*skinnedMesh->joints)[j]->PaintedVertices)[w]->weight * 255.0;
            FileHelper::writeInt(filePointer, vertexIndex);
            FileHelper::writeInt(filePointer, strength);
        }
        
        FileHelper::writeFloat(filePointer, (*skinnedMesh->joints)[j]->envelopeRadius);
        FileHelper::writeFloat(filePointer, (*skinnedMesh->joints)[j]->sphereRadius);
    }
    
}

Mesh* MeshRW::readMeshData(ifstream* filePointer)
{
    Mesh* mesh = new Mesh();
    
    int meshBufferCount = FileHelper::readInt(filePointer);
    
    for(int i = 0; i < meshBufferCount; i++) {
        
        int verticesCount = FileHelper::readInt(filePointer);
        for(int j = 0; j < verticesCount; j++) {
            vertexData v;
            v.vertPosition = FileHelper::readVector3(filePointer);
            v.vertNormal = FileHelper::readVector3(filePointer);
            v.texCoord1 = FileHelper::readVector2(filePointer);
            v.vertTangent = FileHelper::readVector3(filePointer);
            v.vertBitangent = FileHelper::readVector3(filePointer);
            v.optionalData1 = FileHelper::readVector4(filePointer);
            mesh->addVertex(&v); //TODO add directly to mesh buffer vertices
        }
        
        int indicesCount = FileHelper::readInt(filePointer);
        for(int j = 0; j < indicesCount; j++) {
            unsigned short index = FileHelper::readInt(filePointer);
            mesh->addToIndicesArray(index); //TODO add directly to mesh buffer indices
        }
        
    }
    
    mesh->Commit();
    
    return mesh;
}

Mesh* MeshRW::readSkinMeshData(ifstream *filePointer)
{
    SkinMesh* mesh = new SkinMesh();
    mesh->meshType = MESH_TYPE_HEAVY;
    
    int meshBufferCount = FileHelper::readInt(filePointer);
    
    for(int i = 0; i < meshBufferCount; i++) {
        
        int verticesCount = FileHelper::readInt(filePointer);
        
        for(int j = 0; j < verticesCount; j++) {
            vertexDataHeavy v;
            v.vertPosition = FileHelper::readVector3(filePointer);
            v.vertNormal = FileHelper::readVector3(filePointer);
            v.texCoord1 = FileHelper::readVector2(filePointer);
            v.vertTangent = FileHelper::readVector3(filePointer);
            v.vertBitangent = FileHelper::readVector3(filePointer);
            v.optionalData1 = FileHelper::readVector4(filePointer);
            v.optionalData2 = FileHelper::readVector4(filePointer);
            v.optionalData3 = FileHelper::readVector4(filePointer);
            v.optionalData4 = FileHelper::readVector4(filePointer);
            
            mesh->addHeavyVertex(&v); //TODO add directly to mesh buffer vertices
        }
        
        int indicesCount = FileHelper::readInt(filePointer);
        
        for(int j = 0; j < indicesCount; j++) {
            unsigned short index = FileHelper::readInt(filePointer);
            mesh->addToIndicesArray(index); //TODO add directly to mesh buffer indices
        }
        
    }

    int boneCount = FileHelper::readInt(filePointer);
    
    for(int i = 0; i < boneCount; i++) {
        
        int boneParentId = FileHelper::readInt(filePointer);
        
        float* boneMatrix = new float[16];
        
        for( int k = 0; k < 16; k ++)
            boneMatrix[k] = FileHelper::readFloat(filePointer);
        
        Joint* ibone;
        if (boneParentId >= 0)
            ibone = mesh->addJoint((*mesh->joints)[boneParentId]);
        else
            ibone = mesh->addJoint(NULL);
        
        ibone->LocalAnimatedMatrix = Mat4(boneMatrix);

        delete [] boneMatrix;
        
        int boneWeightCount = FileHelper::readInt(filePointer);
        
        for( int j = 0; j < boneWeightCount; ++j) {
            int vertexIndex = FileHelper::readInt(filePointer);
            int vertWeight = FileHelper::readInt(filePointer);
            
            shared_ptr<PaintedVertex> PaintedVertexInfo = make_shared<PaintedVertex>();
            PaintedVertexInfo->vertexId = vertexIndex;
            PaintedVertexInfo->weight = ((float)vertWeight) / 255.0f;
            PaintedVertexInfo->meshBufferIndex = -1;
            ibone->PaintedVertices->push_back(PaintedVertexInfo);
        }
        
        ibone->envelopeRadius = FileHelper::readFloat(filePointer);
        ibone->sphereRadius = FileHelper::readFloat(filePointer);
        
    }
    
    mesh->finalize();
    
    return mesh;
}





