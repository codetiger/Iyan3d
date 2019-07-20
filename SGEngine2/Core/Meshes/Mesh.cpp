//
//  Mesh.cpp
//  SGEngine2
//
//  Created by Harishankar on 13/11/14.
//  Copyright (c) 2014 Smackall Games Pvt Ltd. All rights reserved.
//

#include "Mesh.h"

Mesh::Mesh() {
    meshType = MESH_TYPE_LITE;
    clearVerticesArray();
    clearIndicesArray();
    instanceCount = 0;
}

Mesh::~Mesh() {
    clearIndicesArray();
    clearVerticesArray();
    instanceCount = 0;
}

void Mesh::addMeshBuffer(vector<vertexData> mbvd, vector<unsigned short> mbi, unsigned short materialIndex, bool updateBB) {
    meshBufferVerticesData.push_back(mbvd);
    meshBufferIndices.push_back(mbi);
    meshBufferMaterialIndices.push_back(materialIndex);

    if (updateBB) {
        for (int i = 0; i < meshBufferVerticesData[meshBufferVerticesData.size() - 1].size(); i++) {
            vertexData v = meshBufferVerticesData[meshBufferVerticesData.size() - 1][i];
            BBox.addPointsToCalculateBoundingBox(v.vertPosition);
        }
    }

    BBox.calculateEdges();
}

void Mesh::addMeshBuffer(vector<vertexDataHeavy> mbvd, vector<unsigned short> mbi, unsigned short materialIndex, bool updateBB) {
    meshBufferVerticesDataHeavy.push_back(mbvd);
    meshBufferIndices.push_back(mbi);
    meshBufferMaterialIndices.push_back(materialIndex);

    if (updateBB) {
        for (int i = 0; i < meshBufferVerticesDataHeavy[meshBufferVerticesDataHeavy.size() - 1].size(); i++) {
            vertexDataHeavy v = meshBufferVerticesDataHeavy[meshBufferVerticesDataHeavy.size() - 1][i];
            BBox.addPointsToCalculateBoundingBox(v.vertPosition);
        }
    }

    BBox.calculateEdges();
}

void Mesh::copyDataFromMesh(Mesh* otherMesh) {
    if (otherMesh->meshType == MESH_TYPE_LITE) {
        for (int i = 0; i < otherMesh->getMeshBufferCount(); i++) {
            vector<vertexData>     mbvd          = otherMesh->getLiteVerticesArray(i);
            vector<unsigned short> mbi           = otherMesh->getIndicesArrayAtMeshBufferIndex(i);
            unsigned short         materialIndex = otherMesh->getMeshBufferMaterialIndices(i);
            addMeshBuffer(mbvd, mbi, materialIndex);
        }
    } else {
        for (int i = 0; i < otherMesh->getMeshBufferCount(); i++) {
            vector<vertexDataHeavy> mbhvd         = otherMesh->getHeavyVerticesArray(i);
            vector<unsigned short>  mbi           = otherMesh->getIndicesArrayAtMeshBufferIndex(i);
            unsigned short          materialIndex = otherMesh->getMeshBufferMaterialIndices(i);
            addMeshBuffer(mbhvd, mbi, materialIndex);
        }
    }
}

void Mesh::copyInstanceToMeshCache(Mesh* originalMesh, int instanceIndex) {
    if (originalMesh->meshType == MESH_TYPE_LITE) {
        if (instanceIndex == 1)
            copyInstanceToMeshCache(originalMesh, 0);

        unsigned int           verticesCount = originalMesh->getVerticesCountInMeshBuffer(0);
        vector<vertexData>     newVertices;
        vector<unsigned short> newIndices;

        int  mbCount                = getMeshBufferCount();
        int  lastMBSize             = (mbCount > 0) ? meshBufferVerticesData[mbCount - 1].size() : 0;
        bool insertInLastMeshBuffer = (mbCount > 0) ? ((lastMBSize + verticesCount) < MAX_VERTICES_COUNT) : false;

        for (int i = 0; i < verticesCount; i++) {
            vertexData* v1 = originalMesh->getLiteVerticesForMeshBuffer(0, i);
            vertexData  v2;
            v2.vertPosition  = v1->vertPosition;
            v2.vertNormal    = v1->vertNormal;
            v2.texCoord1     = v1->texCoord1;
            v2.vertTangent   = v1->vertTangent;
            v2.vertBitangent = v1->vertBitangent;
            v2.vertColor     = Vector4(v1->vertColor.x, v1->vertColor.y, v1->vertColor.z, instanceIndex);

            if (insertInLastMeshBuffer) {
                meshBufferVerticesData[mbCount - 1].push_back(v2);
            } else {
                newVertices.push_back(v2);
            }
        }

        for (int i = 0; i < originalMesh->getIndicesCount(0); i++) {
            unsigned short index = originalMesh->getIndicesArrayAtMeshBufferIndex(0)[i];
            if (insertInLastMeshBuffer) {
                meshBufferIndices[mbCount - 1].push_back(lastMBSize + index);
            } else {
                newIndices.push_back(index);
            }
        }

        if (!insertInLastMeshBuffer) {
            addMeshBuffer(newVertices, newIndices, 0);
        }

        instanceCount++;
    }
}

void Mesh::removeVerticesOfAnInstance(int verticesCount, int indicesCount) {
    std::vector<vertexData>::iterator beginIt;
    std::vector<vertexData>::iterator endIt;
    int                               lastMBIndex = meshBufferVerticesData.size() - 1;
    int                               lastMBSize  = meshBufferVerticesData[lastMBIndex].size();

    beginIt = (lastMBSize > verticesCount) ? (meshBufferVerticesData[lastMBIndex].begin() + (lastMBSize - verticesCount)) : meshBufferVerticesData[lastMBIndex].begin();
    endIt   = meshBufferVerticesData[lastMBIndex].end();

    meshBufferVerticesData[lastMBIndex].erase(beginIt, endIt);

    std::vector<unsigned short>::iterator beginIndIt;
    std::vector<unsigned short>::iterator endIndIt;
    lastMBIndex = meshBufferIndices.size() - 1;
    lastMBSize  = meshBufferIndices[lastMBIndex].size();

    beginIndIt = (lastMBSize > indicesCount) ? (meshBufferIndices[lastMBIndex].begin() + (lastMBSize - indicesCount)) : meshBufferIndices[lastMBIndex].begin();
    endIndIt   = meshBufferIndices[lastMBIndex].end();

    meshBufferIndices[lastMBIndex].erase(beginIndIt, endIndIt);

    instanceCount--;
}

Mesh* Mesh::clone() {
    Mesh* m                        = new Mesh();
    m->meshType                    = meshType;
    m->meshBufferIndices           = meshBufferIndices;
    m->meshBufferVerticesData      = meshBufferVerticesData;
    m->meshBufferVerticesDataHeavy = meshBufferVerticesDataHeavy;
    m->meshBufferMaterialIndices   = meshBufferMaterialIndices;

    return m;
}

Mesh* Mesh::convert2Lite() {
    Mesh* m     = new Mesh();
    m->meshType = MESH_TYPE_LITE;
    m->meshBufferIndices.clear();
    m->meshBufferVerticesData.clear();

    for (int i = 0; i < getMeshBufferCount(); i++) {
        vector<vertexData>     mbvd;
        vector<unsigned short> mbi           = getIndicesArrayAtMeshBufferIndex(i);
        int                    materialIndex = getMeshBufferMaterialIndices(i);

        for (int j = 0; j < meshBufferVerticesDataHeavy[i].size(); j++) {
            vertexData      v;
            vertexDataHeavy vH = meshBufferVerticesDataHeavy[i][j];
            v.vertPosition     = vH.vertPosition;
            v.vertNormal       = vH.vertNormal;
            v.texCoord1        = vH.texCoord1;
            v.vertColor        = vH.optionalData4;
            mbvd.push_back(v);
        }
        m->addMeshBuffer(mbvd, mbi, materialIndex);
    }

    return m;
}

void Mesh::clearVerticesArray() {
    if (meshType == MESH_TYPE_HEAVY) {
        for (int i = 0; i < meshBufferVerticesDataHeavy.size(); i++) {
            meshBufferVerticesDataHeavy[i].clear();
        }
        meshBufferVerticesDataHeavy.clear();
    } else {
        for (int i = 0; i < meshBufferVerticesData.size(); i++) {
            meshBufferVerticesData[i].clear();
        }
        meshBufferVerticesData.clear();
    }
}

void Mesh::clearIndicesArray() {
    for (int i = 0; i < meshBufferIndices.size(); i++) {
        meshBufferIndices[i].clear();
    }
    meshBufferIndices.clear();
}

vector<vertexData> Mesh::getLiteVerticesArray(int meshBufferIndex) {
    return meshBufferVerticesData[meshBufferIndex];
}

vector<vertexDataHeavy> Mesh::getHeavyVerticesArray(int meshBufferIndex) {
    return meshBufferVerticesDataHeavy[meshBufferIndex];
}

vertexDataHeavy* Mesh::getHeavyVerticesForMeshBuffer(int meshBufferIndex, int vertexIndex) {
    return &(meshBufferVerticesDataHeavy[meshBufferIndex])[vertexIndex];
}

vertexData* Mesh::getLiteVerticesForMeshBuffer(int meshBufferIndex, int vertexIndex) {
    return &(meshBufferVerticesData[meshBufferIndex])[vertexIndex];
}

unsigned int Mesh::getVerticesCountInMeshBuffer(int meshBufferIndex) {
    if (meshType == MESH_TYPE_HEAVY && meshBufferVerticesDataHeavy.size())
        return (unsigned int)meshBufferVerticesDataHeavy[meshBufferIndex].size();
    else if (meshBufferVerticesData.size())
        return (unsigned int)meshBufferVerticesData[meshBufferIndex].size();

    return 0;
}

BoundingBox* Mesh::getBoundingBox() {
    return &BBox;
}

vector<unsigned short> Mesh::getIndicesArrayAtMeshBufferIndex(int meshBufferIndex) {
    return meshBufferIndices[meshBufferIndex];
}

unsigned short* Mesh::getIndicesArray(int meshBufferIndex) {
    return &(meshBufferIndices[meshBufferIndex][0]);
}

unsigned int Mesh::getIndicesCount(int meshBufferIndex) {
    return (unsigned int)(meshBufferIndices[meshBufferIndex]).size();
}

int Mesh::getMeshBufferCount() {
    return (int)meshBufferIndices.size();
}

int Mesh::getMeshBufferMaterialIndices(int meshBufferIndex) {
    return meshBufferMaterialIndices[meshBufferIndex];
}

void Mesh::clearVertices() {
    for (int i = 0; i < meshBufferVerticesData.size(); i++) {
        meshBufferVerticesData[i].clear();
    }
    meshBufferVerticesData.clear();

    for (int i = 0; i < meshBufferVerticesDataHeavy.size(); i++) {
        meshBufferVerticesDataHeavy[i].clear();
    }
    meshBufferVerticesDataHeavy.clear();
}

void Mesh::clearIndices() {
    for (int i = 0; i < meshBufferIndices.size(); i++) {
        meshBufferIndices[i].clear();
    }
    meshBufferIndices.clear();
}
