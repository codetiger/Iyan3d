//
//  Mesh.cpp
//  SGEngine2
//
//  Created by Harishankar on 13/11/14.
//  Copyright (c) 2014 Smackall Games Pvt Ltd. All rights reserved.
//

#include "Mesh.h"

Mesh::Mesh() {
    tempVerticesData.clear();
    tempVerticesDataHeavy.clear();
    tempIndicesData.clear();
    meshType = MESH_TYPE_LITE;
    clearVerticesArray();
    clearIndicesArray();
    instanceCount = 0;
}

Mesh::~Mesh() {
    tempVerticesData.clear();
    tempIndicesData.clear();
    tempVerticesDataHeavy.clear();
    clearIndicesArray();
    clearVerticesArray();
    instanceCount = 0;
}

void Mesh::copyDataFromMesh(Mesh* otherMesh) {
    if(otherMesh->meshType == MESH_TYPE_LITE) {
        for(int i = 0; i < otherMesh->getVerticesCount(); i++) {
            addVertex(otherMesh->getLiteVertexByIndex(i));
        }
    } else {
        for(int i = 0; i < otherMesh->getVerticesCount(); i++) {
            addHeavyVertex(otherMesh->getHeavyVertexByIndex(i));
        }
    }
    
    for(int i = 0; i < otherMesh->getTotalIndicesCount(); i++) {
        addToIndicesArray(otherMesh->getTotalIndicesArray()[i]);
    }
    
    Commit();
}

void Mesh::copyInstanceToMeshCache(Mesh *originalMesh, int instanceIndex) {
    if(originalMesh->meshType == MESH_TYPE_LITE) {
        
        if(instanceIndex == 1)
            copyInstanceToMeshCache(originalMesh, 0);
        
        unsigned int vertexCount = getVerticesCount();
        
        for(int i = 0; i < originalMesh->getVerticesCount(); i++) {
            vertexData v;
            vertexData *vertx = originalMesh->getLiteVertexByIndex(i);
            v.vertPosition = vertx->vertPosition;
            v.vertNormal = vertx->vertNormal;
            v.texCoord1 = vertx->texCoord1;
            v.optionalData1 = Vector4(vertx->optionalData1.x, vertx->optionalData1.y, vertx->optionalData1.z, instanceIndex);
            addVertex(&v);
        }
        
        for( int i = 0; i < originalMesh->getTotalIndicesCount(); i ++) {
            unsigned int indexToAdd = originalMesh->getTotalIndicesArray()[i] + vertexCount;
            addToIndicesArray(indexToAdd);
        }
    }
    
    instanceCount++;
    Commit();
}

void Mesh::removeVerticesOfAnInstance(int verticesCount, int indicesCount) {
    int endIndex = (getVerticesCount() > verticesCount) ? getVerticesCount() - verticesCount : getVerticesCount();
    
    for(int i = getVerticesCount()-1; i > endIndex; i --)
        tempVerticesData.erase(tempVerticesData.begin() + i);

    endIndex = (getTotalIndicesCount() > indicesCount) ? getTotalIndicesCount() - indicesCount : getTotalIndicesCount();
    for(int i = getTotalIndicesCount()-1; i > endIndex; i--)
        tempIndicesData.erase(tempIndicesData.begin() + i);
    
    Commit();
}

void Mesh::addVertex(vertexData* vertex, bool updateBB){
    vertexData vtx;
    vtx.vertPosition = vertex->vertPosition;
    vtx.vertNormal = vertex->vertNormal;
    vtx.texCoord1 = vertex->texCoord1;
    vtx.optionalData1 = vertex->optionalData1;
    tempVerticesData.push_back(vtx);
    if(updateBB)
        BBox.addPointsToCalculateBoundingBox(vertex->vertPosition);
}

void Mesh::addHeavyVertex(vertexDataHeavy* vertex){
    vertexDataHeavy vtx;
    vtx.vertPosition = vertex->vertPosition;
    vtx.vertNormal = vertex->vertNormal;
    vtx.texCoord1 = vertex->texCoord1;
    vtx.optionalData1 = vertex->optionalData1;
    vtx.optionalData2 = vertex->optionalData2;
    vtx.optionalData3 = vertex->optionalData3;
    vtx.optionalData4 = vertex->optionalData4;
    tempVerticesDataHeavy.push_back(vtx);
    BBox.addPointsToCalculateBoundingBox(vertex->vertPosition);
}

void Mesh::addToIndicesArray(unsigned int index) {
    tempIndicesData.push_back(index);
}

void Mesh::Commit() {
    removeDoublesInMesh();
    reCalculateTangents();
    
    clearVerticesArray();
    clearIndicesArray();
    vector<unsigned int> indicesDataCopy;
    std::map<string, int> indicesDataMap;
    
    if(meshType == MESH_TYPE_LITE) {
        int meshBufferCount = (int)((tempVerticesData.size() - 1) / MAX_VERTICES_COUNT) + 1;
        for (int meshBufferIndex = 0; meshBufferIndex < meshBufferCount; meshBufferIndex++) {
            meshBufferIndices.push_back(vector< unsigned short >());
            
            int vertexCount = (tempVerticesData.size() >= (meshBufferIndex+1)*MAX_VERTICES_COUNT) ? MAX_VERTICES_COUNT : (int)tempVerticesData.size() - meshBufferIndex*MAX_VERTICES_COUNT;

            vector< vertexData > vData(tempVerticesData.begin() + (meshBufferIndex * MAX_VERTICES_COUNT), tempVerticesData.begin() + (meshBufferIndex * MAX_VERTICES_COUNT) + vertexCount);
            meshBufferVerticesData.push_back(vData);

            for (int i = 0; i < (int)tempIndicesData.size() - 2; i += 3) {
                if((tempIndicesData[i] >= ((meshBufferIndex)*MAX_VERTICES_COUNT) && tempIndicesData[i] < ((meshBufferIndex+1)*MAX_VERTICES_COUNT)) &&
                   (tempIndicesData[i+1] >= ((meshBufferIndex)*MAX_VERTICES_COUNT) && tempIndicesData[i+1] < ((meshBufferIndex+1)*MAX_VERTICES_COUNT)) &&
                   (tempIndicesData[i+2] >= ((meshBufferIndex)*MAX_VERTICES_COUNT) && tempIndicesData[i+2] < ((meshBufferIndex+1)*MAX_VERTICES_COUNT))) {
                    meshBufferIndices[meshBufferIndex].push_back(tempIndicesData[i] - (meshBufferIndex)*MAX_VERTICES_COUNT);
                    meshBufferIndices[meshBufferIndex].push_back(tempIndicesData[i+1] - (meshBufferIndex)*MAX_VERTICES_COUNT);
                    meshBufferIndices[meshBufferIndex].push_back(tempIndicesData[i+2] - (meshBufferIndex)*MAX_VERTICES_COUNT);
                } else if((tempIndicesData[i] >= ((meshBufferIndex)*MAX_VERTICES_COUNT) && tempIndicesData[i] < ((meshBufferIndex+1)*MAX_VERTICES_COUNT)) ||
                          (tempIndicesData[i+1] >= ((meshBufferIndex)*MAX_VERTICES_COUNT) && tempIndicesData[i+1] < ((meshBufferIndex+1)*MAX_VERTICES_COUNT)) ||
                          (tempIndicesData[i+2] >= ((meshBufferIndex)*MAX_VERTICES_COUNT) && tempIndicesData[i+2] < ((meshBufferIndex+1)*MAX_VERTICES_COUNT))) {
                    string uniqueid = to_string(tempIndicesData[i]) + to_string(tempIndicesData[i+1]) + to_string(tempIndicesData[i+2]);
                    if(indicesDataMap.find(uniqueid) == indicesDataMap.end()) {
                        indicesDataCopy.push_back(tempIndicesData[i]);
                        indicesDataCopy.push_back(tempIndicesData[i+1]);
                        indicesDataCopy.push_back(tempIndicesData[i+2]);
                        indicesDataMap.insert(std::pair<string,int> (uniqueid, i));
                    }
                }
            }
        }
        
        int meshBufferIndex = meshBufferCount - 1;
        if((meshBufferVerticesData[meshBufferIndex].size() + indicesDataCopy.size()) > MAX_VERTICES_COUNT) {
            meshBufferVerticesData.push_back(vector< vertexData >());
            meshBufferIndices.push_back(vector< unsigned short >());
            meshBufferIndex++;
        }
        
        for(int i = 0; i < (int)indicesDataCopy.size(); i++) {
            if((int)meshBufferVerticesData[meshBufferIndex].size() == MAX_VERTICES_COUNT) {
                meshBufferVerticesData.push_back(vector< vertexData >());
                meshBufferIndices.push_back(vector< unsigned short >());
                meshBufferIndex++;
            }
            
            meshBufferVerticesData[meshBufferIndex].push_back(tempVerticesData[indicesDataCopy[i]]);
            meshBufferIndices[meshBufferIndex].push_back(meshBufferVerticesData[meshBufferIndex].size()-1);
        }

        indicesDataCopy.clear();
        indicesDataMap.clear();
        
    } else {
        int meshBufferCount = (int)((tempVerticesDataHeavy.size() - 1) / MAX_VERTICES_COUNT) + 1;
        for (int meshBufferIndex = 0; meshBufferIndex < meshBufferCount; meshBufferIndex++) {
            meshBufferIndices.push_back(vector< unsigned short >());
            
            int vertexCount = (tempVerticesDataHeavy.size() >= (meshBufferIndex+1)*MAX_VERTICES_COUNT) ? MAX_VERTICES_COUNT : (int)tempVerticesDataHeavy.size() - meshBufferIndex*MAX_VERTICES_COUNT;

            vector< vertexDataHeavy > vData(tempVerticesDataHeavy.begin() + (meshBufferIndex * MAX_VERTICES_COUNT), tempVerticesDataHeavy.begin() + (meshBufferIndex * MAX_VERTICES_COUNT) + vertexCount);
            meshBufferVerticesDataHeavy.push_back(vData);
            
            for (int i = 0; i < (int)tempIndicesData.size() - 2; i += 3) {
                if(tempIndicesData[i] >= (meshBufferIndex)*MAX_VERTICES_COUNT &&
                   tempIndicesData[i+1] >= (meshBufferIndex)*MAX_VERTICES_COUNT &&
                   tempIndicesData[i+2] >= (meshBufferIndex)*MAX_VERTICES_COUNT &&
                   tempIndicesData[i] < (meshBufferIndex+1)*MAX_VERTICES_COUNT &&
                   tempIndicesData[i+1] < (meshBufferIndex+1)*MAX_VERTICES_COUNT &&
                   tempIndicesData[i+2] < (meshBufferIndex+1)*MAX_VERTICES_COUNT) {
                    meshBufferIndices[meshBufferIndex].push_back(tempIndicesData[i] - (meshBufferIndex)*MAX_VERTICES_COUNT);
                    meshBufferIndices[meshBufferIndex].push_back(tempIndicesData[i+1] - (meshBufferIndex)*MAX_VERTICES_COUNT);
                    meshBufferIndices[meshBufferIndex].push_back(tempIndicesData[i+2] - (meshBufferIndex)*MAX_VERTICES_COUNT);
                } else if((tempIndicesData[i] >= (meshBufferIndex)*MAX_VERTICES_COUNT && tempIndicesData[i] < (meshBufferIndex+1)*MAX_VERTICES_COUNT) ||
                          (tempIndicesData[i+1] >= (meshBufferIndex)*MAX_VERTICES_COUNT && tempIndicesData[i+1] < (meshBufferIndex+1)*MAX_VERTICES_COUNT) ||
                          (tempIndicesData[i+2] >= (meshBufferIndex)*MAX_VERTICES_COUNT && tempIndicesData[i+2] < (meshBufferIndex+1)*MAX_VERTICES_COUNT)) {
                    string uniqueid = to_string(tempIndicesData[i]) + to_string(tempIndicesData[i+1]) + to_string(tempIndicesData[i+2]);
                    if(indicesDataMap.find(uniqueid) == indicesDataMap.end()) {
                        indicesDataCopy.push_back(tempIndicesData[i]);
                        indicesDataCopy.push_back(tempIndicesData[i+1]);
                        indicesDataCopy.push_back(tempIndicesData[i+2]);
                        indicesDataMap.insert(std::pair<string,int> (uniqueid, i));
                    }
                }
            }
        }
        
        int meshBufferIndex = meshBufferCount - 1;
        if((meshBufferVerticesDataHeavy[meshBufferIndex].size() + indicesDataCopy.size()) > MAX_VERTICES_COUNT) {
            meshBufferVerticesDataHeavy.push_back(vector< vertexDataHeavy >());
            meshBufferIndices.push_back(vector< unsigned short >());
            meshBufferIndex++;
        }
        
        for(int i = 0; i < (int)indicesDataCopy.size(); i++) {
            if((int)meshBufferVerticesDataHeavy[meshBufferIndex].size() == MAX_VERTICES_COUNT) {
                meshBufferVerticesDataHeavy.push_back(vector< vertexDataHeavy >());
                meshBufferIndices.push_back(vector< unsigned short >());
                meshBufferIndex++;
            }
            
            meshBufferVerticesDataHeavy[meshBufferIndex].push_back(tempVerticesDataHeavy[indicesDataCopy[i]]);
            meshBufferIndices[meshBufferIndex].push_back(meshBufferVerticesDataHeavy[meshBufferIndex].size()-1);
        }
        
        indicesDataCopy.clear();
        indicesDataMap.clear();
    }
    BBox.calculateEdges();
}

void Mesh::clearVerticesArray() {
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

void Mesh::clearIndicesArray() {
    for(int i = 0; i < meshBufferIndices.size(); i++) {
        meshBufferIndices[i].clear();
    }
    meshBufferIndices.clear();
}

void Mesh::reCalculateTangents() {
    if(meshType == MESH_TYPE_LITE) {
        for (int i = 0; i + 2 < tempIndicesData.size(); i += 3) {
            Vector3 tangent = Vector3(0.0), bitangent = Vector3(0.0);
            vertexData *v1 = getLiteVertexByIndex(tempIndicesData[i + 0]);
            vertexData *v2 = getLiteVertexByIndex(tempIndicesData[i + 1]);
            vertexData *v3 = getLiteVertexByIndex(tempIndicesData[i + 2]);

            if(v1 && v2 && v3) {
                calculateTanget(tangent, bitangent, v1->vertPosition, v2->vertPosition, v3->vertPosition, v1->texCoord1, v2->texCoord1, v3->texCoord1);
            
                v1->vertTangent = tangent;
                v2->vertTangent = tangent;
                v3->vertTangent = tangent;
                v1->vertBitangent = bitangent;
                v2->vertBitangent = bitangent;
                v3->vertBitangent = bitangent;
            }
        }
    } else {
        for (int i = 0; i < tempIndicesData.size(); i += 3) {
            Vector3 tangent = Vector3(0.0), bitangent = Vector3(0.0);
            vertexDataHeavy *v1 = getHeavyVertexByIndex(tempIndicesData[i + 0]);
            vertexDataHeavy *v2 = getHeavyVertexByIndex(tempIndicesData[i + 1]);
            vertexDataHeavy *v3 = getHeavyVertexByIndex(tempIndicesData[i + 2]);
            
            if(v1 && v2 && v3) {
                calculateTanget(tangent, bitangent, v1->vertPosition, v2->vertPosition, v3->vertPosition, v1->texCoord1, v2->texCoord1, v3->texCoord1);
                
                v1->vertTangent = tangent;
                v2->vertTangent = tangent;
                v3->vertTangent = tangent;
                v1->vertBitangent = bitangent;
                v2->vertBitangent = bitangent;
                v3->vertBitangent = bitangent;
            }
        }
    }
}

void Mesh::calculateTanget(Vector3 &tangent, Vector3 &bitangent, Vector3 vt1, Vector3 vt2, Vector3 vt3, Vector2 tc1, Vector2 tc2, Vector2 tc3) {
    Vector3 v1 = vt1 - vt2;
    Vector3 v2 = vt3 - vt1;
    Vector3 normal = v2.crossProduct(v1);
    normal.normalize();
    
    f32 deltaX1 = tc1.x - tc2.x;
    f32 deltaX2 = tc3.x - tc1.x;
    bitangent = (v1 * deltaX2) - (v2 * deltaX1);
    bitangent.normalize();
    
    f32 deltaY1 = tc1.y - tc2.y;
    f32 deltaY2 = tc3.y - tc1.y;
    tangent = (v1 * deltaY2) - (v2 * deltaY1);
    tangent.normalize();
    
    Vector3 txb = tangent.crossProduct(bitangent);
    if (txb.dotProduct(normal) < 0.0f) {
        tangent *= -1.0f;
        bitangent *= -1.0f;
    }
}

void Mesh::generateUV() {
    BoundingBox *bb = getBoundingBox();
    float largeExtend = bb->getXExtend() > bb->getYExtend() ? bb->getXExtend() : bb->getYExtend();
    largeExtend = largeExtend > bb->getZExtend() ? largeExtend : bb->getZExtend();
    
    const u32 vtxcnt = getVerticesCount();
    for (int i = 0; i != vtxcnt; i++) {
        Vector3 pos = (meshType == MESH_TYPE_LITE) ? getLiteVertexByIndex(i)->vertPosition : getHeavyVertexByIndex(i)->vertPosition;
        Vector3 normal = (meshType == MESH_TYPE_LITE) ? getLiteVertexByIndex(i)->vertNormal : getHeavyVertexByIndex(i)->vertNormal;

        Vector3 r = Vector3(fabs(normal.x), fabs(normal.y), fabs(normal.z));
        
        Vector2 tex;
        tex.x = 0.0;
        tex.y = 0.0;

        if(r.z <= 0.1) {
            tex.x = (pos.z - bb->getMinEdge().z) / largeExtend;
            tex.y = atan2f(r.y, r.x);
        } else {
            tex.x = (pos.x - bb->getMinEdge().x) / largeExtend;
            tex.y = (pos.y - bb->getMinEdge().y) / largeExtend;
        }
        
        if (meshType == MESH_TYPE_LITE)
            getLiteVertexByIndex(i)->texCoord1 = tex;
        else
            getHeavyVertexByIndex(i)->texCoord1 = tex;
    }
}

vertexData* Mesh::getLiteVertexByIndex(unsigned int index) {
    return &tempVerticesData[index];
}

vertexDataHeavy* Mesh::getHeavyVertexByIndex(unsigned int index) {
    return &tempVerticesDataHeavy[index];
}

vector<vertexData> Mesh::getLiteVerticesArray(int index) {
    return meshBufferVerticesData[index];
}

vector<vertexDataHeavy> Mesh::getHeavyVerticesArray(int index) {
    return meshBufferVerticesDataHeavy[index];
}

vector<vertexData> Mesh::getTotalLiteVerticesArray() {
    return tempVerticesData;
}

vector<vertexDataHeavy> Mesh::getTotalHeavyVerticesArray() {
    return tempVerticesDataHeavy;
}

unsigned int Mesh::getVerticesCountInMeshBuffer(int index) {
    if(meshType == MESH_TYPE_HEAVY)
        return (unsigned int)meshBufferVerticesDataHeavy[index].size();
    else
        return (unsigned int)meshBufferVerticesData[index].size();
}

unsigned int Mesh::getVerticesCount() {
    if(meshType == MESH_TYPE_HEAVY)
        return (unsigned int)tempVerticesDataHeavy.size();
    else
        return (unsigned int)tempVerticesData.size();
}

BoundingBox* Mesh::getBoundingBox() {
    return &BBox;
}

unsigned int * Mesh::getHighPolyIndicesArray() {
    return &tempIndicesData[0];
}

unsigned short* Mesh::getIndicesArray(int index) {
    return &(meshBufferIndices[index][0]);
}

unsigned int Mesh::getIndicesCount(int index) {
    return (unsigned int )(meshBufferIndices[index]).size();
}

unsigned int Mesh::getTotalIndicesCount() {
    return (int)tempIndicesData.size();
}

vector< unsigned int > Mesh::getTotalIndicesArray() {
    return tempIndicesData;
}

int Mesh::getMeshBufferCount() {
    return (int)meshBufferIndices.size();
}

void Mesh::clearVertices() {
    tempVerticesData.clear();
}

void Mesh::clearIndices() {
    tempIndicesData.clear();
}

void Mesh::removeDoublesInMesh() {
    std::map<std::string, unsigned int> vertMap;
    vector<unsigned int> tIndicesData;
    
    if(meshType == MESH_TYPE_LITE) {
        vector<vertexData> verticesDataDup;

        for(u32 i = 0; i < tempIndicesData.size(); i++) {
            vertexData *vert =  getLiteVertexByIndex(tempIndicesData[i]);
            string unique = to_string(vert->vertPosition.x) + to_string(vert->vertPosition.y) + to_string(vert->vertPosition.z) + to_string(vert->texCoord1.x) + to_string(vert->texCoord1.y) + to_string(vert->vertNormal.x) + to_string(vert->vertNormal.y) + to_string(vert->vertNormal.z) + to_string(vert->optionalData1.x) + to_string(vert->optionalData1.y) + to_string(vert->optionalData1.z) + to_string(vert->optionalData1.w);
            MD5 md5;
            unique = md5.digestString((char*)unique.c_str());

            if(vertMap.find(unique) != vertMap.end()) {
                tIndicesData.push_back((unsigned int)vertMap[unique]);
            } else {
                verticesDataDup.push_back(*vert);
                vertMap.insert(std::pair<std::string, unsigned int>(unique, (unsigned int)verticesDataDup.size() - 1));
                tIndicesData.push_back((unsigned int)verticesDataDup.size() - 1);
            }
        }
        tempVerticesData.clear();
        tempVerticesData = verticesDataDup;
        tempIndicesData.clear();
        tempIndicesData = tIndicesData;
    } else {
        vector<vertexDataHeavy> verticesDataDup;
        
        for(u32 i = 0; i < tempIndicesData.size(); i++) {
            vertexDataHeavy *vert =  getHeavyVertexByIndex(tempIndicesData[i]);
            string unique = to_string(vert->vertPosition.x) + to_string(vert->vertPosition.y) + to_string(vert->vertPosition.z) + to_string(vert->texCoord1.x) + to_string(vert->texCoord1.y) + to_string(vert->vertNormal.x) + to_string(vert->vertNormal.y) + to_string(vert->vertNormal.z) + to_string(vert->optionalData1.x) + to_string(vert->optionalData1.y) + to_string(vert->optionalData1.z) + to_string(vert->optionalData1.w);
            MD5 md5;
            unique = md5.digestString((char*)unique.c_str());
            
            if(vertMap.find(unique) != vertMap.end()) {
                tIndicesData.push_back((unsigned int)vertMap[unique]);
            } else {
                verticesDataDup.push_back(*vert);
                vertMap.insert(std::pair<std::string, unsigned int>(unique, (unsigned int)verticesDataDup.size() - 1));
                tIndicesData.push_back((unsigned int)verticesDataDup.size() - 1);
            }
        }
        tempVerticesDataHeavy.clear();
        tempVerticesDataHeavy = verticesDataDup;
        tempIndicesData.clear();
        tempIndicesData = tIndicesData;
    }
}

void Mesh::fixOrientation() {
    const u32 vtxcnt = getVerticesCount();
    for (int i = 0; i != vtxcnt; i++) {
        Vector3 pos = (meshType == MESH_TYPE_LITE) ? getLiteVertexByIndex(i)->vertPosition : getHeavyVertexByIndex(i)->vertPosition;
        
        if (meshType == MESH_TYPE_LITE)
            getLiteVertexByIndex(i)->vertPosition = Vector3(-pos.x, pos.y, pos.z);
        else
            getHeavyVertexByIndex(i)->vertPosition = Vector3(-pos.x, pos.y, pos.z);
    }
}

void Mesh::moveVertices(Vector3 offset) {
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

void Mesh::recalculateNormals() {
    for (int i = 0; i < getVerticesCount(); ++i) {
        Vector3 &n = (meshType == MESH_TYPE_LITE) ? getLiteVertexByIndex(i)->vertNormal : getHeavyVertexByIndex(i)->vertNormal;
        n = Vector3(0.0, 0.0, 0.0);
    }

    for (int i = 0; i < getTotalIndicesCount(); i += 3) {
        Vector3& v1 = (meshType == MESH_TYPE_LITE) ? getLiteVertexByIndex(tempIndicesData[i+0])->vertPosition : getHeavyVertexByIndex(tempIndicesData[i+0])->vertPosition;
        Vector3& v2 = (meshType == MESH_TYPE_LITE) ? getLiteVertexByIndex(tempIndicesData[i+1])->vertPosition : getHeavyVertexByIndex(tempIndicesData[i+1])->vertPosition;
        Vector3& v3 = (meshType == MESH_TYPE_LITE) ? getLiteVertexByIndex(tempIndicesData[i+2])->vertPosition : getHeavyVertexByIndex(tempIndicesData[i+2])->vertPosition;
        Vector3 dir1 = (v2 - v1).normalize();
        Vector3 dir2 = (v3 - v1).normalize();
        Vector3 normal = dir1.crossProduct(dir2).normalize();

        Vector3 weight(1.f, 1.f, 1.f);
        weight = getAngleWeight(v1, v2, v3);
        
        Vector3& n1 = (meshType == MESH_TYPE_LITE) ? getLiteVertexByIndex(tempIndicesData[i+0])->vertNormal : getHeavyVertexByIndex(tempIndicesData[i+0])->vertNormal;
        Vector3& n2 = (meshType == MESH_TYPE_LITE) ? getLiteVertexByIndex(tempIndicesData[i+1])->vertNormal : getHeavyVertexByIndex(tempIndicesData[i+1])->vertNormal;
        Vector3& n3 = (meshType == MESH_TYPE_LITE) ? getLiteVertexByIndex(tempIndicesData[i+2])->vertNormal : getHeavyVertexByIndex(tempIndicesData[i+2])->vertNormal;
        n1 += normal * weight.x;
        n2 += normal * weight.y;
        n3 += normal * weight.z;
    }
    
    for (int i = 0; i < getVerticesCount(); ++i) {
        Vector3 &n = (meshType == MESH_TYPE_LITE) ? getLiteVertexByIndex(i)->vertNormal : getHeavyVertexByIndex(i)->vertNormal;
        n = n.normalize();
    }
}

Vector3 Mesh::getAngleWeight(Vector3& v1, Vector3& v2, Vector3& v3) {
    Vector3 asq =  Vector3(v2.x - v3.x,v2.y - v3.y,v2.z - v3.z);
    const f32 a = asq.x * asq.x + asq.y * asq.y + asq.z * asq.z;
    const f32 asqrt = sqrtf(a);
    
    asq = Vector3(v1.x - v3.x,v1.y - v3.y,v1.z - v3.z);
    const f32 b = asq.x * asq.x + asq.y * asq.y + asq.z * asq.z;
    const f32 bsqrt = sqrtf(b);
    
    asq = Vector3(v1.x - v2.x,v1.y - v2.y,v1.z - v2.z);
    const f32 c = asq.x * asq.x + asq.y * asq.y + asq.z * asq.z;
    const f32 csqrt = sqrtf(c);
    
    return Vector3(acosf((b + c - a) / (2.f * bsqrt * csqrt)),
                   acosf((-b + c + a) / (2.f * asqrt * csqrt)),
                   acosf((b - c + a) / (2.f * bsqrt * asqrt)));
}
