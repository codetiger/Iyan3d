//
//  Mesh.h
//  SGEngine2
//
//  Created by Harishankar on 13/11/14.
//  Copyright (c) 2014 Smackall Games Pvt Ltd. All rights reserved.
//

#ifndef __SGEngine2__Mesh__
#define __SGEngine2__Mesh__

#include "../common/common.h"
#include "../common/BoundingBox.h"
#include <iostream>
#include <map>
#include "md5.h"

typedef enum {
    MESH_TYPE_HEAVY,
    MESH_TYPE_LITE
} MESH_TYPE;

using std::vector;

struct vertexData {
    Vector3 vertPosition;
    Vector3 vertNormal;
    Vector2 texCoord1;
    Vector3 vertTangent;
    Vector3 vertBitangent;
    Vector4 vertColor;
    bool operator==(const vertexData& rhs) {
        return (vertPosition == rhs.vertPosition && vertNormal == rhs.vertNormal && texCoord1 == rhs.texCoord1);
    }
};

struct vertexDataHeavy {
    Vector3 vertPosition;
    Vector3 vertNormal;
    Vector2 texCoord1;
    Vector3 vertTangent;
    Vector3 vertBitangent;
    Vector4 vertColor;
    Vector4 optionalData1;
    Vector4 optionalData2;
    Vector4 optionalData3;
    Vector4 optionalData4;
};

struct vertexCompare
{
    bool operator() (const vertexData& lhs, const vertexData& rhs) const
    {
        return (lhs.vertPosition.x != rhs.vertPosition.x || lhs.vertPosition.y != rhs.vertPosition.y || lhs.vertPosition.z != rhs.vertPosition.z);
    }
    
    bool operator() (const vertexDataHeavy& lhs, const vertexDataHeavy& rhs) const
    {
        return (lhs.vertPosition.x != rhs.vertPosition.x || lhs.vertPosition.y != rhs.vertPosition.y || lhs.vertPosition.z != rhs.vertPosition.z);
    }
};

class Mesh {
private:
    BoundingBox BBox;

    int instanceCount;
    vector< vector<vertexData> > meshBufferVerticesData;
    vector< vector<vertexDataHeavy> > meshBufferVerticesDataHeavy;
    vector< vector<unsigned short> > meshBufferIndices;
    vector< unsigned short > meshBufferMaterialIndices;

public:
    MESH_TYPE meshType;

    Mesh* clone();
    Mesh* convert2Lite();
    
    void addMeshBuffer(vector<vertexData> mbvd, vector<unsigned short> mbi, unsigned short materialIndex, bool updateBB = true);
    void addMeshBuffer(vector<vertexDataHeavy> mbvd, vector<unsigned short> mbi, unsigned short materialIndex, bool updateBB = true);
    
    void copyDataFromMesh(Mesh* otherMesh);
    void copyInstanceToMeshCache(Mesh *originalMesh, int instanceIndex);
    void removeVerticesOfAnInstance(int verticesCount, int indicesCount);

    void clearVerticesArray();
    void clearIndicesArray();
    
    vector<vertexData> getLiteVerticesArray(int meshBufferIndex);
    vector<vertexDataHeavy> getHeavyVerticesArray(int meshBufferIndex);

    vertexDataHeavy* getHeavyVerticesForMeshBuffer(int meshBufferIndex, int vertexIndex);
    vertexData* getLiteVerticesForMeshBuffer(int meshBufferIndex, int vertexIndex);

    vector< unsigned short > getIndicesArrayAtMeshBufferIndex(int meshBufferIndex);
    unsigned short* getIndicesArray(int meshBufferIndex);
    unsigned int getIndicesCount(int meshBufferIndex);
    unsigned int getVerticesCountInMeshBuffer(int meshBufferIndex);
    int getMeshBufferCount();
    int getMeshBufferMaterialIndices(int meshBufferIndex);

    void clearVertices();
    void clearIndices();
    BoundingBox* getBoundingBox();
    Vector3 getAngleWeight(Vector3& v1, Vector3& v2, Vector3& v3);
    
    Mesh();
    ~Mesh();
};


#endif /* defined(__SGEngine2__Mesh__) */
