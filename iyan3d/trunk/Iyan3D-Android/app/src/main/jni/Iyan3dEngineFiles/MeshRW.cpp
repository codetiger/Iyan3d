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
        unsigned int indicesCount = mesh->getIndicesCount(i);
        
        FileHelper::writeInt(filePointer, mesh->getMeshBufferMaterialIndices(i));
        
        FileHelper::writeInt(filePointer, verticesCount);
        
        for(int j = 0; j < verticesCount; j++) {
            vertexData v = vertices[j];
            FileHelper::writeVector3(filePointer, v.vertPosition);
            FileHelper::writeVector3(filePointer, v.vertNormal);
            FileHelper::writeVector2(filePointer, v.texCoord1);
            FileHelper::writeVector3(filePointer, v.vertTangent);
            FileHelper::writeVector3(filePointer, v.vertBitangent);
            FileHelper::writeVector4(filePointer, v.vertColor);
        }
        
        FileHelper::writeUnsignedInt(filePointer, indicesCount);
        
        for(unsigned int j = 0; j < indicesCount; j++) {
            FileHelper::writeInt(filePointer, indices[j]);
        }
    }
}

void MeshRW::writeSkinMeshData(ofstream *filePointer, SkinMesh* skinnedMesh, shared_ptr< AnimatedMeshNode > aNode)
{
    int meshBufferCount = skinnedMesh->getMeshBufferCount();
    
    FileHelper::writeInt(filePointer, meshBufferCount);
    
    for(int i = 0; i < meshBufferCount; i++) {
        
        vector< vertexDataHeavy > vertices = skinnedMesh->getHeavyVerticesArray(i);
        vector< unsigned short > indicesArr = skinnedMesh->getIndicesArrayAtMeshBufferIndex(i);
        int verticesCount = (int)vertices.size();
                
        FileHelper::writeInt(filePointer, skinnedMesh->getMeshBufferMaterialIndices(i));
        
        FileHelper::writeInt(filePointer, verticesCount);
        
        for(int j = 0; j < verticesCount; j++) {
            vertexDataHeavy v = vertices[j];
            FileHelper::writeVector3(filePointer, v.vertPosition);
            FileHelper::writeVector3(filePointer, v.vertNormal);
            FileHelper::writeVector2(filePointer, v.texCoord1);
            FileHelper::writeVector3(filePointer, v.vertTangent);
            FileHelper::writeVector3(filePointer, v.vertBitangent);
            FileHelper::writeVector4(filePointer, v.vertColor);
            FileHelper::writeVector4(filePointer, v.optionalData1);
            FileHelper::writeVector4(filePointer, v.optionalData2);
            FileHelper::writeVector4(filePointer, v.optionalData3);
            FileHelper::writeVector4(filePointer, v.optionalData4);
        }
        
        FileHelper::writeUnsignedInt(filePointer, indicesArr.size());
        
        for(unsigned int j = 0; j < indicesArr.size(); j++) {
            FileHelper::writeInt(filePointer, indicesArr[j]);
        }
    }
    
    int boneCount = skinnedMesh->joints->size();
    FileHelper::writeInt(filePointer, boneCount);
    
    for(int j = 0; j < boneCount; j++) {
        Joint* parent = (*skinnedMesh->joints)[j]->Parent;
        if(parent)
            FileHelper::writeInt(filePointer, parent->Index);
        else
            FileHelper::writeInt(filePointer, -1);
        
        Mat4 origMatrix = (*skinnedMesh->joints)[j]->originalJointMatrix;
        for(int k = 0; k < 16; k ++)
            FileHelper::writeFloat(filePointer, origMatrix[k]);
        
        unsigned short boneWeightCount = (*skinnedMesh->joints)[j]->PaintedVertices->size();
        FileHelper::writeInt(filePointer, boneWeightCount);
        
        for(int w = 0; w < boneWeightCount; w++) {
            unsigned int vertexIndex = (*(*skinnedMesh->joints)[j]->PaintedVertices)[w]->vertexId;
            unsigned short strength = (*(*skinnedMesh->joints)[j]->PaintedVertices)[w]->weight * 255.0;
            unsigned short meshBufferIndex = (*(*skinnedMesh->joints)[j]->PaintedVertices)[w]->meshBufferIndex;
            
            FileHelper::writeInt(filePointer, vertexIndex);
            FileHelper::writeInt(filePointer, strength);
            FileHelper::writeShort(filePointer, meshBufferIndex);
            
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
        
        vector<vertexData> mbvd;
        vector<unsigned short> mbi;
        
        int materialIndex = FileHelper::readInt(filePointer);
        int verticesCount = FileHelper::readInt(filePointer);
        
        for(int j = 0; j < verticesCount; j++) {
            vertexData v;
            v.vertPosition = FileHelper::readVector3(filePointer);
            v.vertNormal = FileHelper::readVector3(filePointer);
            v.texCoord1 = FileHelper::readVector2(filePointer);
            v.vertTangent = FileHelper::readVector3(filePointer);
            v.vertBitangent = FileHelper::readVector3(filePointer);
            v.vertColor = FileHelper::readVector4(filePointer);
            mbvd.push_back(v);
        }
        
        unsigned int indicesCount = FileHelper::readUnsignedInt(filePointer);

        for(unsigned int j = 0; j < indicesCount; j++) {
            unsigned short index = FileHelper::readInt(filePointer);
            mbi.push_back(index);
        }
        
        mesh->addMeshBuffer(mbvd, mbi, materialIndex);
    }
    
    return mesh;
}

Mesh* MeshRW::readSkinMeshData(ifstream *filePointer)
{
    
    SkinMesh* mesh = new SkinMesh();
    mesh->meshType = MESH_TYPE_HEAVY;
    
    int meshBufferCount = FileHelper::readInt(filePointer);
    
    for(int i = 0; i < meshBufferCount; i++) {
        
        vector<vertexDataHeavy> mbvd;
        vector<unsigned short> mbi;

        int materialIndex = FileHelper::readInt(filePointer);
        int verticesCount = FileHelper::readInt(filePointer);
        
        for(int j = 0; j < verticesCount; j++) {
            vertexDataHeavy v;
            v.vertPosition = FileHelper::readVector3(filePointer);
            v.vertNormal = FileHelper::readVector3(filePointer);
            v.texCoord1 = FileHelper::readVector2(filePointer);
            v.vertTangent = FileHelper::readVector3(filePointer);
            v.vertBitangent = FileHelper::readVector3(filePointer);
            v.vertColor = FileHelper::readVector4(filePointer);
            v.optionalData1 = FileHelper::readVector4(filePointer);
            v.optionalData2 = FileHelper::readVector4(filePointer);
            v.optionalData3 = FileHelper::readVector4(filePointer);
            v.optionalData4 = FileHelper::readVector4(filePointer);
            
            mbvd.push_back(v);
        }
        
        unsigned int indicesCount = FileHelper::readUnsignedInt(filePointer);
        
        for(unsigned int j = 0; j < indicesCount; j++) {
            unsigned short index = FileHelper::readInt(filePointer);
            mbi.push_back(index);
        }
        
        mesh->addMeshBuffer(mbvd, mbi, materialIndex);
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
        ibone->originalJointMatrix = ibone->LocalAnimatedMatrix;
        delete [] boneMatrix;
        
        int boneWeightCount = FileHelper::readInt(filePointer);
        
        for( int j = 0; j < boneWeightCount; ++j) {
            int vertexIndex = FileHelper::readInt(filePointer);
            int vertWeight = FileHelper::readInt(filePointer);
            int meshBufferIndex = FileHelper::readShort(filePointer);
            
            shared_ptr<PaintedVertex> PaintedVertexInfo = make_shared<PaintedVertex>();
            PaintedVertexInfo->vertexId = vertexIndex;
            PaintedVertexInfo->weight = ((float)vertWeight) / 255.0f;
            PaintedVertexInfo->meshBufferIndex = meshBufferIndex;
            ibone->PaintedVertices->push_back(PaintedVertexInfo);
            
        }
        
        ibone->envelopeRadius = FileHelper::readFloat(filePointer);
        ibone->sphereRadius = FileHelper::readFloat(filePointer);
        
    }
    
    mesh->finalize();
    
    return mesh;
}





