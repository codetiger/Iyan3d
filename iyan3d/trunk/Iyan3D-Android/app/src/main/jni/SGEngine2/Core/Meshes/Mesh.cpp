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
    
    shouldRemoveDoubles = true;
    shouldOptimizeIndicesOrder = true;
    shouldCalculateTangents = true;
    shouldCalculateNormals = false;
    shouldGenerateUV = false;
    shouldSplitBuffers = false;
    normalSmoothThreshold = 1.0;
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

void Mesh::addMeshBuffer(vector<vertexData> mbvd, vector<unsigned short> mbi, unsigned short materialIndex)
{
    meshBufferVerticesData.push_back(mbvd);
    meshBufferIndices.push_back(mbi);
    meshBufferMaterialIndices.push_back(materialIndex);
    
    for(int i = 0; i < meshBufferVerticesData[meshBufferVerticesData.size() - 1].size(); i++) {
        vertexData v = meshBufferVerticesData[meshBufferVerticesData.size() - 1][i];
        BBox.addPointsToCalculateBoundingBox(v.vertPosition);
    }
}

void Mesh::addMeshBuffer(vector<vertexDataHeavy> mbvd, vector<unsigned short> mbi, unsigned short materialIndex)
{
    meshBufferVerticesDataHeavy.push_back(mbvd);
    meshBufferIndices.push_back(mbi);
    meshBufferMaterialIndices.push_back(materialIndex);
    
    for(int i = 0; i < meshBufferVerticesDataHeavy[meshBufferVerticesDataHeavy.size() - 1].size(); i++) {
        vertexDataHeavy v = meshBufferVerticesDataHeavy[meshBufferVerticesDataHeavy.size() - 1][i];
        BBox.addPointsToCalculateBoundingBox(v.vertPosition);
    }
}

void Mesh::copyDataFromMesh(Mesh* otherMesh)
{
    if(otherMesh->meshType == MESH_TYPE_LITE) {
        for(int i = 0; i < otherMesh->getVerticesCount(); i++)
            addVertex(otherMesh->getLiteVertexByIndex(i));
    } else {
        for(int i = 0; i < otherMesh->getVerticesCount(); i++)
            addHeavyVertex(otherMesh->getHeavyVertexByIndex(i));
    }
    
    for(int i = 0; i < otherMesh->getTotalIndicesCount(); i++)
        addToIndicesArray(otherMesh->getTotalIndicesArray()[i]);
}

void Mesh::copyInstanceToMeshCache(Mesh *originalMesh, int instanceIndex)
{
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
            v.vertColor = Vector4(vertx->vertColor.x, vertx->vertColor.y, vertx->vertColor.z, instanceIndex);
            addVertex(&v);
        }
        
        for( int i = 0; i < originalMesh->getTotalIndicesCount(); i ++) {
            unsigned int indexToAdd = originalMesh->getTotalIndicesArray()[i] + vertexCount;
            addToIndicesArray(indexToAdd);
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

void Mesh::addVertex(vertexData* vertex, bool updateBB)
{
    vertexData vtx;
    vtx.vertPosition = vertex->vertPosition;
    vtx.vertNormal = vertex->vertNormal;
    vtx.texCoord1 = vertex->texCoord1;
    vtx.vertColor = vertex->vertColor;
    tempVerticesData.push_back(vtx);
    if(updateBB)
        BBox.addPointsToCalculateBoundingBox(vertex->vertPosition);
    
    shouldRemoveDoubles = true;
    shouldCalculateTangents = true;
    shouldSplitBuffers = true;
}

void Mesh::addHeavyVertex(vertexDataHeavy* vertex)
{
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

    shouldRemoveDoubles = true;
    shouldCalculateTangents = true;
    shouldSplitBuffers = true;
}

void Mesh::addToIndicesArray(unsigned int index)
{
    tempIndicesData.push_back(index);
    
    shouldOptimizeIndicesOrder = true;
    shouldSplitBuffers = true;
}

Mesh* Mesh::clone()
{
    Mesh *m = new Mesh();
    m->meshType = meshType;
    m->setOptimization(false, false);
    
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
  
    m->tempIndicesData = tempIndicesData;
    m->tempVerticesData.clear();
    m->tempVerticesDataHeavy.clear();
    
    for (int i = 0; i < getVerticesCount(); i++) {
        vertexDataHeavy *vtx = getHeavyVertexByIndex(i);
        vertexData v;
        v.vertPosition = vtx->vertPosition;
        v.vertNormal = vtx->vertNormal;
        v.texCoord1 = vtx->texCoord1;
        v.vertColor = vtx->optionalData4;
        m->addVertex(&v);
    }
    
    m->Commit();
    return m;
}

void Mesh::setOptimization(bool rmDoubles, bool optimIndOrd, bool calcTangents, bool calcNormals, bool genUV, float smoothThreshold)
{
    shouldRemoveDoubles = rmDoubles;
    shouldOptimizeIndicesOrder = optimIndOrd;
    shouldCalculateTangents = calcTangents;
    normalSmoothThreshold = smoothThreshold;
    shouldCalculateNormals = calcNormals;
    shouldGenerateUV = genUV;
}

void Mesh::Commit(bool forceSplitBuffers)
{
    if(shouldCalculateNormals)
        recalculateNormals();
    
    if (shouldRemoveDoubles)
        removeDoublesInMesh();
    
    if (shouldOptimizeIndicesOrder)
        reOrderMeshIndices();
    
    if (shouldCalculateTangents)
        reCalculateTangents();
    
    if(shouldGenerateUV)
        generateUV();
    
    if(shouldSplitBuffers || forceSplitBuffers) {
        clearVerticesArray();
        clearIndicesArray();
        vector<unsigned int> indicesDataCopy;
        std::map<string, int> indicesDataMap;
        
        if(meshType == MESH_TYPE_LITE) {
            if(tempVerticesData.size() > MAX_VERTICES_COUNT) {
                int meshBufferCount = (int)((tempVerticesData.size() - 1) / MAX_VERTICES_COUNT) + 1;
                
                for (int meshBufferIndex = 0; meshBufferIndex < meshBufferCount; meshBufferIndex++) {
                    meshBufferIndices.push_back(vector< unsigned short >());
                    meshBufferMaterialIndices.push_back(0);
                    
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
                    meshBufferMaterialIndices.push_back(0);
                    meshBufferIndex++;
                }
                
                for(int i = 0; i < (int)indicesDataCopy.size(); i++) {
                    if((int)meshBufferVerticesData[meshBufferIndex].size() == MAX_VERTICES_COUNT) {
                        meshBufferVerticesData.push_back(vector< vertexData >());
                        meshBufferIndices.push_back(vector< unsigned short >());
                        meshBufferMaterialIndices.push_back(0);
                        meshBufferIndex++;
                    }
                    
                    meshBufferVerticesData[meshBufferIndex].push_back(tempVerticesData[indicesDataCopy[i]]);
                    meshBufferIndices[meshBufferIndex].push_back(meshBufferVerticesData[meshBufferIndex].size()-1);
                }
                
                indicesDataCopy.clear();
                indicesDataMap.clear();
            } else {
                vector< vertexData > vData(tempVerticesData.begin(), tempVerticesData.end());
                meshBufferVerticesData.push_back(vData);
                
                vector< unsigned short > indData(tempIndicesData.begin(), tempIndicesData.end());
                meshBufferIndices.push_back(indData);
                
                meshBufferMaterialIndices.push_back(0);
            }
            
        } else {
            
            if(tempVerticesDataHeavy.size() > MAX_VERTICES_COUNT) {
                int meshBufferCount = (int)((tempVerticesDataHeavy.size() - 1) / MAX_VERTICES_COUNT) + 1;
                for (int meshBufferIndex = 0; meshBufferIndex < meshBufferCount; meshBufferIndex++) {
                    meshBufferIndices.push_back(vector< unsigned short >());
                    meshBufferMaterialIndices.push_back(0);
                    
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
                    meshBufferMaterialIndices.push_back(0);
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
                
            } else {
                vector< vertexDataHeavy > vData(tempVerticesDataHeavy.begin(), tempVerticesDataHeavy.end());
                meshBufferVerticesDataHeavy.push_back(vData);
                
                vector< unsigned short > indData(tempIndicesData.begin(), tempIndicesData.end());
                meshBufferIndices.push_back(indData);
                
                meshBufferMaterialIndices.push_back(0);
            }
        }
    }
    shouldSplitBuffers = false;
    BBox.calculateEdges();
    
//    printf("\nMesh Info: ");
//    for (int i = 0; i < getMeshBufferCount(); i++) {
//        printf("MeshBuffer: %d \nIndices \n", i);
//        for (int j = 0; j < getIndicesCount(i); j++) {
//            printf(" %d ", getIndicesArray(i)[j]);
//        }
//    }
//    printf("\n\n\n");
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

void Mesh::reCalculateTangents()
{
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

void Mesh::calculateTanget(Vector3 &tangent, Vector3 &bitangent, Vector3 vt1, Vector3 vt2, Vector3 vt3, Vector2 tc1, Vector2 tc2, Vector2 tc3)
{
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

void Mesh::generateUV()
{
    BoundingBox *bb = getBoundingBox();
    float largeExtend = bb->getXExtend() > bb->getYExtend() ? bb->getXExtend() : bb->getYExtend();
    largeExtend = largeExtend > bb->getZExtend() ? largeExtend : bb->getZExtend();
    
    const u32 vtxcnt = getVerticesCount();
    for (int i = 0; i < vtxcnt; i++) {
        Vector3 pos = (meshType == MESH_TYPE_LITE) ? getLiteVertexByIndex(i)->vertPosition : getHeavyVertexByIndex(i)->vertPosition;
        Vector3 diff = (pos - bb->getCenter()).normalize();

        Vector2 tex;
        tex.x = (atan2 (diff.z, diff.y) + M_PI ) / (2.0 * M_PI);
        tex.y = (asin(diff.x) + (M_PI / 2.0)) / M_PI;

        if (meshType == MESH_TYPE_LITE)
            getLiteVertexByIndex(i)->texCoord1 = tex;
        else
            getHeavyVertexByIndex(i)->texCoord1 = tex;
    }
    
    checkUVSeam();
    shouldGenerateUV = false;
}

void Mesh::checkUVSeam()
{
    const static float LOWER_LIMIT = 0.1;
    const static float UPPER_LIMIT = 0.9;
    const static float LOWER_EPSILON = 10e-3;
    const static float UPPER_EPSILON = 1.0-10e-3;

    for (int i = 0; i < getTotalIndicesCount() - 2; i += 3) {
        unsigned int small = 3, large = small;
        bool zero = false, one = false, round_to_zero = false;

        for(int j = 0; j < 3; j++) {
            Vector2* tex;
            if(meshType == MESH_TYPE_LITE)
                tex = &tempVerticesData[tempIndicesData[i+j]].texCoord1;
            else
                tex = &tempVerticesDataHeavy[tempIndicesData[i+j]].texCoord1;
            
            if (tex->x < LOWER_LIMIT) {
                small = j;
                
                if (tex->x <= LOWER_EPSILON)
                    zero = true;
                else
                    round_to_zero = true;
            }
            
            if (tex->x > UPPER_LIMIT) {
                large = j;
                
                if (tex->x >= UPPER_EPSILON)
                    one = true;
            }
        }
        
        if (small != 3 && large != 3) {
            for(int j = 0; j < 3; ++j) {
                Vector2* tex;
                if(meshType == MESH_TYPE_LITE)
                    tex = &tempVerticesData[tempIndicesData[i+j]].texCoord1;
                else
                    tex = &tempVerticesDataHeavy[tempIndicesData[i+j]].texCoord1;

                if (tex->x > UPPER_LIMIT && !zero)
                    tex->x = 0.0;
                else if (tex->x < LOWER_LIMIT && !one)
                    tex->x = 1.0;
                else if (one && zero) {
                    if (round_to_zero && tex->x >=  UPPER_EPSILON)
                        tex->x = 0.0;
                    else if (!round_to_zero && tex->x <= LOWER_EPSILON)
                        tex->x = 1.0;
                }
            }
        }
    }
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
    if(meshType == MESH_TYPE_HEAVY)
        return (unsigned int)meshBufferVerticesDataHeavy[index].size();
    else
        return (unsigned int)meshBufferVerticesData[index].size();
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

void Mesh::removeDoublesInMesh()
{
//    printf("Original Vertices Count: %d ", getVerticesCount());
    map< unsigned int, unsigned int > remapIndices;

    if(meshType == MESH_TYPE_LITE) {
        vector<vertexData> verticesData;
        
        int totalVertCount = tempVerticesData.size();
        for(int i = 0; i < totalVertCount; i++) {
            vertexData v1 = tempVerticesData[i];
            unsigned int newIndices = i;
            bool alreadyInserted = false;
            
            int vertDataCount = verticesData.size();
            for(int j = 0; j < vertDataCount; j++) {
                vertexData v2 = verticesData[j];
                
                if(v1.vertPosition == v2.vertPosition && v1.texCoord1 == v2.texCoord1 && v1.vertColor == v2.vertColor) {
                    if(v1.vertNormal == v2.vertNormal) {
                        newIndices = j;
                        alreadyInserted = true;
                        j = vertDataCount;
                    } else {
                        v1.vertNormal.normalize();
                        v2.vertNormal.normalize();
                        
                        float diff = v1.vertNormal.dotProduct(v2.vertNormal);
                        
                        if(diff >= normalSmoothThreshold) {
                            newIndices = j;
                            alreadyInserted = true;
                            j = vertDataCount;
                        }
                    }
                }
            }
            
            if(!alreadyInserted) {
                newIndices = verticesData.size();
                verticesData.push_back(v1);
            }
            
            remapIndices.insert(pair<unsigned int, unsigned int > (i, newIndices));
        }

        tempVerticesData.clear();
        tempVerticesData = verticesData;

    } else {
        vector<vertexDataHeavy> verticesData;
        
        int totalVertCount = tempVerticesDataHeavy.size();
        for(int i = 0; i < totalVertCount; i++) {
            vertexDataHeavy v1 = tempVerticesDataHeavy[i];
            unsigned int newIndices = i;
            bool alreadyInserted = false;
            
            int vertDataCount = verticesData.size();
            for(int j = 0; j < vertDataCount; j++) {
                vertexDataHeavy v2 = verticesData[j];
                
                if(v1.vertPosition == v2.vertPosition && v1.texCoord1 == v2.texCoord1 && v1.optionalData1 == v2.optionalData1 && v1.optionalData2 == v2.optionalData2 && v1.optionalData3 == v2.optionalData3 && v1.optionalData4 == v2.optionalData4) {
                    if(v1.vertNormal == v2.vertNormal) {
                        newIndices = j;
                        alreadyInserted = true;
                        j = vertDataCount;
                    } else {
                        v1.vertNormal.normalize();
                        v2.vertNormal.normalize();
                        
                        float diff = v1.vertNormal.dotProduct(v2.vertNormal);
                        
                        if(diff >= normalSmoothThreshold) {
                            newIndices = j;
                            alreadyInserted = true;
                            j = vertDataCount;
                        }
                    }
                }
            }
            
            if(!alreadyInserted) {
                newIndices = verticesData.size();
                verticesData.push_back(v1);
            }
            
            remapIndices.insert(pair<unsigned int, unsigned int > (i, newIndices));
        }

        tempVerticesDataHeavy.clear();
        tempVerticesDataHeavy = verticesData;
    }
    
    
    for(int k = 0; k < tempIndicesData.size(); k++) {
        if(remapIndices.find(tempIndicesData[k]) != remapIndices.end())
            tempIndicesData[k] = remapIndices[tempIndicesData[k]];
    }
    

//    printf(" after RemoveDoubles: %d\n", getVerticesCount());
    normalSmoothThreshold = 1.0;
    shouldRemoveDoubles = false;
    
    recalculateNormals();
}

void Mesh::reOrderMeshIndices()
{
    int indicesCount = getTotalIndicesCount();

//    printf("\n\nBefore Optimization: %d\n", getVerticesCount());
//    for (int i = 0; i < indicesCount; i++)
//        printf("%d ", getHighPolyIndicesArray()[i]);
    
    MeshOptimizeHelper *optimizer = new MeshOptimizeHelper();
    unsigned int *optimizedIndices = new unsigned int[indicesCount];
    optimizer->OptimizeFaces(getHighPolyIndicesArray(), indicesCount, getVerticesCount(), optimizedIndices, 32);
    
    tempIndicesData.clear();
    
    for (int i = 0; i < indicesCount; i++)
        addToIndicesArray(optimizedIndices[i]);

    shouldOptimizeIndicesOrder = false;
//    printf("\nAfter Optimization\n");
//    for (int i = 0; i < indicesCount; i++)
//        printf("%d ", getHighPolyIndicesArray()[i]);
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

void Mesh::recalculateNormals()
{
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
    
    shouldCalculateNormals = false;
}

Vector3 Mesh::getAngleWeight(Vector3& v1, Vector3& v2, Vector3& v3)
{
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

