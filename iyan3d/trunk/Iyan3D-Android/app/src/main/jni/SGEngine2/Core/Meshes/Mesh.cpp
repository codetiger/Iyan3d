//
//  Mesh.cpp
//  SGEngine2
//
//  Created by Harishankar on 13/11/14.
//  Copyright (c) 2014 Smackall Games Pvt Ltd. All rights reserved.
//

#include "Mesh.h"

Mesh::Mesh()
{
    tempVerticesData.clear();
    tempVerticesDataHeavy.clear();
    tempIndicesData.clear();
    meshType = MESH_TYPE_LITE;
    clearVerticesArray();
    clearIndicesArray();
    instanceCount = 0;
}

Mesh::~Mesh()
{
    tempVerticesData.clear();
    tempIndicesData.clear();
    tempVerticesDataHeavy.clear();
    clearIndicesArray();
    clearVerticesArray();
    instanceCount = 0;
}

void Mesh::addMeshBuffer(vector<vertexData> mbvd, vector<unsigned short> mbi, unsigned short materialIndex, bool updateBB)
{
    meshBufferVerticesData.push_back(mbvd);
    meshBufferIndices.push_back(mbi);
    meshBufferMaterialIndices.push_back(materialIndex);
    
    if(updateBB) {
        for(int i = 0; i < meshBufferVerticesData[meshBufferVerticesData.size() - 1].size(); i++) {
            vertexData v = meshBufferVerticesData[meshBufferVerticesData.size() - 1][i];
            BBox.addPointsToCalculateBoundingBox(v.vertPosition);
        }
    }
}

void Mesh::addMeshBuffer(vector<vertexDataHeavy> mbvd, vector<unsigned short> mbi, unsigned short materialIndex, bool updateBB)
{
    meshBufferVerticesDataHeavy.push_back(mbvd);
    meshBufferIndices.push_back(mbi);
    meshBufferMaterialIndices.push_back(materialIndex);
    
    if(updateBB) {
        for(int i = 0; i < meshBufferVerticesDataHeavy[meshBufferVerticesDataHeavy.size() - 1].size(); i++) {
            vertexDataHeavy v = meshBufferVerticesDataHeavy[meshBufferVerticesDataHeavy.size() - 1][i];
            BBox.addPointsToCalculateBoundingBox(v.vertPosition);
        }
    }
}

void Mesh::copyDataFromMesh(Mesh* otherMesh)
{
    if(otherMesh->meshType == MESH_TYPE_LITE) {
        for( int i = 0; i < otherMesh->getMeshBufferCount(); i++) {
            vector< vertexData > mbvd = otherMesh->getLiteVerticesArray(i);
            vector< unsigned short > mbi = otherMesh->getIndicesArrayAtMeshBufferIndex(i);
            unsigned short materialIndex = otherMesh->getMeshBufferMaterialIndices(i);
            addMeshBuffer(mbvd, mbi, materialIndex);
        }
    } else {
        for( int i = 0; i < otherMesh->getMeshBufferCount(); i++) {
            vector< vertexDataHeavy > mbhvd = otherMesh->getHeavyVerticesArray(i);
            vector< unsigned short > mbi = otherMesh->getIndicesArrayAtMeshBufferIndex(i);
            unsigned short materialIndex = otherMesh->getMeshBufferMaterialIndices(i);
            addMeshBuffer(mbhvd, mbi, materialIndex);
        }
    }
    
}

void Mesh::copyInstanceToMeshCache(Mesh *originalMesh, int instanceIndex)
{
    if(originalMesh->meshType == MESH_TYPE_LITE) {
        
        if(instanceIndex == 1)
            copyInstanceToMeshCache(originalMesh, 0);
        
        unsigned int vertexCount = getVerticesCount();
        
        for( int i = 0; i < originalMesh->getMeshBufferCount(); i++) {
            vector< vertexData > mbvd = originalMesh->getLiteVerticesArray(i);
            for( int j = 0; j < mbvd.size(); j++) {
                vertexData &v = mbvd[j];
                v.vertColor = Vector4(v.vertColor.x, v.vertColor.y, v.vertColor.z, instanceIndex);
            }
            vector< unsigned short > mbi = originalMesh->getIndicesArrayAtMeshBufferIndex(i);
            unsigned short materialIndex = originalMesh->getMeshBufferMaterialIndices(i);
            addMeshBuffer(mbvd, mbi, materialIndex);
        }
    }
    
    instanceCount++;
}

void Mesh::removeVerticesOfAnInstance(int verticesCount, int indicesCount)
{
    int endIndex = (getVerticesCount() > verticesCount) ? getVerticesCount() - verticesCount : getVerticesCount();
    
    for(int i = getVerticesCount()-1; i > endIndex; i --)
        tempVerticesData.erase(tempVerticesData.begin() + i);

    endIndex = (getTotalIndicesCount() > indicesCount) ? getTotalIndicesCount() - indicesCount : getTotalIndicesCount();

    for(int i = getTotalIndicesCount()-1; i > endIndex; i--)
        tempIndicesData.erase(tempIndicesData.begin() + i);
    
    Commit();
}

Mesh* Mesh::clone()
{
    Mesh *m = new Mesh();
    m->meshType = meshType;
    
    m->tempIndicesData = tempIndicesData;
    m->tempVerticesData = tempVerticesData;
    m->tempVerticesDataHeavy = tempVerticesDataHeavy;
    
    m->Commit();
    return m;
}

Mesh* Mesh::convert2Lite()
{
    Mesh *m = new Mesh();
    m->meshType = MESH_TYPE_LITE;
    m->meshBufferIndices.clear();
    m->meshBufferVerticesData.clear();
    
    for( int i = 0; i < getMeshBufferCount(); i++) {
        vector< vertexData > mbvd;
        vector< unsigned short > mbi = getIndicesArrayAtMeshBufferIndex(i);
        int materialIndex = getMeshBufferMaterialIndices(i);
        
        for( int j = 0; j < meshBufferVerticesDataHeavy[i].size(); j++){
            vertexData v;
            vertexDataHeavy vH = meshBufferVerticesDataHeavy[i][j];
            v.vertPosition = vH.vertPosition;
            v.vertNormal = vH.vertNormal;
            v.texCoord1 = vH.texCoord1;
            v.vertColor = vH.optionalData4;
            mbvd.push_back(v);
        }
        m->addMeshBuffer(mbvd, mbi, materialIndex);
    }
    
    m->Commit();
    return m;
}

void Mesh::Commit()
{
    BBox.calculateEdges();
}

void Mesh::clearVerticesArray()
{
    if(meshType == MESH_TYPE_HEAVY) {
        for(int i = 0; i < meshBufferVerticesDataHeavy.size(); i++) {
            meshBufferVerticesDataHeavy[i].clear();
        }
        meshBufferVerticesDataHeavy.clear();
    } else {
        for(int i = 0; i < meshBufferVerticesData.size(); i++) {
            meshBufferVerticesData[i].clear();
        }
        meshBufferVerticesData.clear();
    }
}

void Mesh::clearIndicesArray()
{
    for(int i = 0; i < meshBufferIndices.size(); i++) {
        meshBufferIndices[i].clear();
    }
    meshBufferIndices.clear();
}

vertexData* Mesh::getLiteVertexByIndex(unsigned int index)
{
    return &tempVerticesData[index];
}

vertexDataHeavy* Mesh::getHeavyVertexByIndex(unsigned int index)
{
    return &tempVerticesDataHeavy[index];
}

vector<vertexData> Mesh::getLiteVerticesArray(int index)
{
    return meshBufferVerticesData[index];
}

vector<vertexDataHeavy> Mesh::getHeavyVerticesArray(int index)
{
    return meshBufferVerticesDataHeavy[index];
}

vertexDataHeavy* Mesh::getHeavyVerticesForMeshBuffer(int meshBufferIndex, int vertexIndex)
{
    return &(meshBufferVerticesDataHeavy[meshBufferIndex])[vertexIndex];
}

vertexData* Mesh::getLiteVerticesForMeshBuffer(int meshBufferIndex, int vertexIndex)
{
    return &(meshBufferVerticesData[meshBufferIndex])[vertexIndex];
}

vector<vertexData> Mesh::getTotalLiteVerticesArray()
{
    return tempVerticesData;
}

vector<vertexDataHeavy> Mesh::getTotalHeavyVerticesArray()
{
    return tempVerticesDataHeavy;
}

unsigned int Mesh::getVerticesCountInMeshBuffer(int index)
{
    if(meshType == MESH_TYPE_HEAVY && meshBufferVerticesDataHeavy.size())
        return (unsigned int)meshBufferVerticesDataHeavy[index].size();
    else if(meshBufferVerticesData.size())
        return (unsigned int)meshBufferVerticesData[index].size();
    
    return 0;
}

unsigned int Mesh::getVerticesCount()
{
    if(meshType == MESH_TYPE_HEAVY)
        return (unsigned int)tempVerticesDataHeavy.size();
    else
        return (unsigned int)tempVerticesData.size();
}

BoundingBox* Mesh::getBoundingBox()
{
    return &BBox;
}

unsigned int * Mesh::getHighPolyIndicesArray()
{
    return &tempIndicesData[0];
}

vector< unsigned short > Mesh::getIndicesArrayAtMeshBufferIndex(int index)
{
    return meshBufferIndices[index];
}

unsigned short* Mesh::getIndicesArray(int index)
{
    return &(meshBufferIndices[index][0]);
}

unsigned int Mesh::getIndicesCount(int index)
{
    return (unsigned int )(meshBufferIndices[index]).size();
}

unsigned int Mesh::getTotalIndicesCount()
{
    return (int)tempIndicesData.size();
}

vector< unsigned int > Mesh::getTotalIndicesArray()
{
    return tempIndicesData;
}

int Mesh::getMeshBufferCount()
{
    return (int)meshBufferIndices.size();
}

int Mesh::getMeshBufferMaterialIndices(int meshBufferIndex)
{
    return meshBufferMaterialIndices[meshBufferIndex];
}

void Mesh::clearVertices()
{
    tempVerticesData.clear();
}

void Mesh::clearIndices()
{
    tempIndicesData.clear();
}

void Mesh::flipMeshHorizontal()
{
    const u32 vtxcnt = getVerticesCount();
    for (int i = 0; i != vtxcnt; i++) {
        Vector3 pos = (meshType == MESH_TYPE_LITE) ? getLiteVertexByIndex(i)->vertPosition : getHeavyVertexByIndex(i)->vertPosition;
        
        if (meshType == MESH_TYPE_LITE)
            getLiteVertexByIndex(i)->vertPosition = Vector3(-pos.x, pos.y, pos.z);
        else
            getHeavyVertexByIndex(i)->vertPosition = Vector3(-pos.x, pos.y, pos.z);
    }
}

void Mesh::moveVertices(Vector3 offset)
{
    const u32 vtxcnt = getVerticesCount();
    BBox.clearPoints();

    for (int i = 0; i!= vtxcnt;i++) {
        
        Vector3 pos = (meshType == MESH_TYPE_LITE) ? getLiteVertexByIndex(i)->vertPosition : getHeavyVertexByIndex(i)->vertPosition;
        
        if (meshType == MESH_TYPE_LITE) {
            getLiteVertexByIndex(i)->vertPosition = Vector3(pos.x, pos.y, pos.z) + offset;
            BBox.addPointsToCalculateBoundingBox(Vector3(pos.x, pos.y, pos.z) + offset);
        }
        else {
            getHeavyVertexByIndex(i)->vertPosition = Vector3(pos.x, pos.y, pos.z) + offset;
            BBox.addPointsToCalculateBoundingBox(Vector3(pos.x, pos.y, pos.z) + offset);
        }
    }
}

