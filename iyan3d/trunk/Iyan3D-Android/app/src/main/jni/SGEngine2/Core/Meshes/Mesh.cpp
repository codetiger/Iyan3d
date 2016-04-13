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
}

Mesh::~Mesh() {
    tempVerticesData.clear();
    tempIndicesData.clear();
    tempVerticesDataHeavy.clear();
    clearIndicesArray();
    clearVerticesArray();
}
void Mesh::addVertex(vertexData* vertex){
    vertexData vtx;
    vtx.vertPosition = vertex->vertPosition;
    vtx.vertNormal = vertex->vertNormal;
    vtx.texCoord1 = vertex->texCoord1;
    vtx.optionalData1 = vertex->optionalData1;
    tempVerticesData.push_back(vtx);
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

void Mesh::generateUV() {
    BoundingBox *bb = getBoundingBox();
    float largeExtend = bb->getXExtend() > bb->getYExtend() ? bb->getXExtend() : bb->getYExtend();
    
    const u32 vtxcnt = getVerticesCount();
    for (int i = 0; i!= vtxcnt;i++) {
        Vector3 pos = (meshType == MESH_TYPE_LITE) ? getLiteVertexByIndex(i)->vertPosition : getHeavyVertexByIndex(i)->vertPosition;
        
        Vector2 tex;
        tex.x = (pos.x - bb->getMinEdge().x) / largeExtend;
        tex.y = (pos.y - bb->getMinEdge().y) / largeExtend;
        
        if (meshType == MESH_TYPE_LITE)
            getLiteVertexByIndex(i)->texCoord1 = tex;
        else
            getHeavyVertexByIndex(i)->texCoord1 = tex;
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

vector<vertexData> Mesh::getTotalLiteVerticesArray()
{
    return tempVerticesData;
}

vector<vertexDataHeavy> Mesh::getTotalHeavyVerticesArray()
{
    return tempVerticesDataHeavy;
}

unsigned int Mesh::getVerticesCountInMeshBuffer(int index){
    if(meshType == MESH_TYPE_HEAVY)
        return (unsigned int)meshBufferVerticesDataHeavy[index].size();
    else
        return (unsigned int)meshBufferVerticesData[index].size();
}

unsigned int Mesh::getVerticesCount(){
    if(meshType == MESH_TYPE_HEAVY)
        return (unsigned int)tempVerticesDataHeavy.size();
    else
        return (unsigned int)tempVerticesData.size();
}

BoundingBox* Mesh::getBoundingBox(){
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

unsigned int Mesh::getIndicesCount(int index){
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

void Mesh::removeDoublesInHeavyMesh(bool usePos,bool useTcoords,bool useNormals){
    
    //This method should only be used for 3D Text
    map<string,unsigned int> vertMap;
    vector<vertexDataHeavy> verticesDataDup;
    tempIndicesData.clear();
    
    const u32 vtxcnt = getVerticesCount();
    for(u32 i=0; i<vtxcnt; i++){
        vertexDataHeavy *vert =  getHeavyVertexByIndex(i);
        string unique;
        if(usePos)
            unique = to_string(vert->vertPosition.x) + to_string(vert->vertPosition.y) + to_string(vert->vertPosition.z);
        if(useTcoords)
            unique += (to_string(vert->texCoord1.x) + to_string(vert->texCoord1.y));
        if(useNormals)
            unique += (to_string(vert->vertNormal.x) + to_string(vert->vertNormal.y) + to_string(vert->vertNormal.z));
        /*
         Normal Approximation
         
         int t = atan(vert->vertNormal.y/vert->vertNormal.x) / 4.0f;
         int p = acos(vert->vertNormal.z/vert->vertNormal.x) / 4.0f;
         unique += (to_string(t) + to_string(p));

         */
        if(vertMap.find(unique) != vertMap.end()){
            addToIndicesArray((unsigned short)vertMap[unique]);
        }else{
            verticesDataDup.push_back(*vert);
            vertMap.insert(std::pair<string,unsigned int>(unique,(unsigned int)verticesDataDup.size()-1));
            addToIndicesArray((unsigned short)verticesDataDup.size()-1);
        }
    }
    tempVerticesDataHeavy.clear();
    tempVerticesDataHeavy = verticesDataDup;
}

void Mesh::fixOrientation()
{
//    float dif = BBox.getXExtend()/2.0;
    const u32 vtxcnt = getVerticesCount();
    for (int i = 0; i!= vtxcnt;i++) {
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
    for (int i = 0; i!= vtxcnt;i++) {
        Vector3 pos = (meshType == MESH_TYPE_LITE) ? getLiteVertexByIndex(i)->vertPosition : getHeavyVertexByIndex(i)->vertPosition;
        
        if (meshType == MESH_TYPE_LITE)
            getLiteVertexByIndex(i)->vertPosition = Vector3(pos.x, pos.y, pos.z) + offset;
        else
            getHeavyVertexByIndex(i)->vertPosition = Vector3(pos.x, pos.y, pos.z) + offset;
    }
}

void Mesh::recalculateNormalsT(bool smooth)
{
    const u32 vtxcnt = getVerticesCount();
    const u32 idxcnt = getTotalIndicesCount();
    
    if (!smooth)
    {
        for (int i = 0; i!= vtxcnt; ++i) {
            (meshType == MESH_TYPE_LITE) ? getLiteVertexByIndex(i)->vertNormal : getHeavyVertexByIndex(i)->vertNormal = Vector3(-2.0,-2.0,-2.0);
        }
        
        for (u32 i=0; i<idxcnt; i+=3)
        {
            int index[3];
            index[0] = reinterpret_cast<u32*>(getHighPolyIndicesArray())[i+0];
            index[1] = reinterpret_cast<u32*>(getHighPolyIndicesArray())[i+1];
            index[2] = reinterpret_cast<u32*>(getHighPolyIndicesArray())[i+2];
            Vector3& v1 = (meshType == MESH_TYPE_LITE) ? getLiteVertexByIndex(index[0])->vertPosition : getHeavyVertexByIndex(index[0])->vertPosition;
            Vector3& v2 = (meshType == MESH_TYPE_LITE) ? getLiteVertexByIndex(index[1])->vertPosition : getHeavyVertexByIndex(index[1])->vertPosition;
            Vector3& v3 = (meshType == MESH_TYPE_LITE) ? getLiteVertexByIndex(index[2])->vertPosition : getHeavyVertexByIndex(index[2])->vertPosition;
            Vector3 dir1 = (v2 - v1).normalize();
            Vector3 dir2 = (v3 - v1).normalize();
            Vector3 normal = dir1.crossProduct(dir2).normalize();
            
            Vector3& n1 = (meshType == MESH_TYPE_LITE) ? getLiteVertexByIndex(index[0])->vertNormal : getHeavyVertexByIndex(index[0])->vertNormal;
            Vector3& n2 = (meshType == MESH_TYPE_LITE) ? getLiteVertexByIndex(index[1])->vertNormal : getHeavyVertexByIndex(index[1])->vertNormal;
            Vector3& n3 = (meshType == MESH_TYPE_LITE) ? getLiteVertexByIndex(index[2])->vertNormal : getHeavyVertexByIndex(index[2])->vertNormal;
            
            if(n1.x == -2.0)
                n1 = (normal);
            else
                n1 = getLinearInterpolation(1,n1,3,normal,2);
            
            if(n2.x == -2.0)
                n2 = (normal);
            else
                n2 = getLinearInterpolation(1,n2,3,normal,2);
            
            if(n3.x == -2.0)
                n3 = (normal);
            else
                n3 = getLinearInterpolation(1,n3,3,normal,2);
        }
    }
    else
    {
        u32 i;
        for ( i = 0; i!= vtxcnt; ++i ) {
            (meshType == MESH_TYPE_LITE) ? getLiteVertexByIndex(i)->vertNormal : getHeavyVertexByIndex(i)->vertNormal = Vector3(0.0, 0.0, 0.0);
        }
        
        for ( i=0; i<idxcnt; i+=3)
        {
            int index[3];
            index[0] = reinterpret_cast<u32*>(getHighPolyIndicesArray())[i+0];
            index[1] = reinterpret_cast<u32*>(getHighPolyIndicesArray())[i+1];
            index[2] = reinterpret_cast<u32*>(getHighPolyIndicesArray())[i+2];
            
            Vector3& v1 = (meshType == MESH_TYPE_LITE) ? getLiteVertexByIndex(index[0])->vertPosition : getHeavyVertexByIndex(index[0])->vertPosition;
            Vector3& v2 = (meshType == MESH_TYPE_LITE) ? getLiteVertexByIndex(index[1])->vertPosition : getHeavyVertexByIndex(index[1])->vertPosition;
            Vector3& v3 = (meshType == MESH_TYPE_LITE) ? getLiteVertexByIndex(index[2])->vertPosition : getHeavyVertexByIndex(index[2])->vertPosition;
            Vector3 dir1 = (v2 - v1).normalize();
            Vector3 dir2 = (v3 - v1).normalize();
            
            Vector3 normal = dir1.crossProduct(dir2).normalize();
            
            Vector3 weight(1.f,1.f,1.f);
            weight = getAngleWeight(v1,v2,v3);
            
            Vector3& n1 = (meshType == MESH_TYPE_LITE) ? getLiteVertexByIndex(index[0])->vertNormal : getHeavyVertexByIndex(index[0])->vertNormal;
            Vector3& n2 = (meshType == MESH_TYPE_LITE) ? getLiteVertexByIndex(index[1])->vertNormal : getHeavyVertexByIndex(index[1])->vertNormal;
            Vector3& n3 = (meshType == MESH_TYPE_LITE) ? getLiteVertexByIndex(index[2])->vertNormal : getHeavyVertexByIndex(index[2])->vertNormal;
            
            n1 += (normal * weight.x);
            n2 += (normal * weight.y);
            n3 += (normal * weight.z);
        }
        for ( i = 0; i!= vtxcnt; ++i ) {
            Vector3 &vertexNormal = (meshType == MESH_TYPE_LITE) ? getLiteVertexByIndex(i)->vertNormal : getHeavyVertexByIndex(i)->vertNormal;
            vertexNormal.normalize();
        }
    }
}

Vector3 Mesh::getAngleWeight(Vector3& v1,Vector3& v2,Vector3& v3)
{
    // Calculate this triangle's weight for each of its three vertices
    // start by calculating the lengths of its sides
    Vector3 asq =  Vector3(v2.x - v3.x,v2.y - v3.y,v2.z - v3.z);
    const f32 a = asq.x * asq.x + asq.y * asq.y + asq.z * asq.z;
    const f32 asqrt = sqrtf(a);
    
    asq = Vector3(v1.x - v3.x,v1.y - v3.y,v1.z - v3.z);
    const f32 b = asq.x * asq.x + asq.y * asq.y + asq.z * asq.z;
    const f32 bsqrt = sqrtf(b);
    
    asq = Vector3(v1.x - v2.x,v1.y - v2.y,v1.z - v2.z);
    const f32 c = asq.x * asq.x + asq.y * asq.y + asq.z * asq.z;
    const f32 csqrt = sqrtf(c);
    
    // use them to find the angle at each vertex
    return Vector3(
                   acosf((b + c - a) / (2.f * bsqrt * csqrt)),
                   acosf((-b + c + a) / (2.f * asqrt * csqrt)),
                   acosf((b - c + a) / (2.f * bsqrt * asqrt)));
}

