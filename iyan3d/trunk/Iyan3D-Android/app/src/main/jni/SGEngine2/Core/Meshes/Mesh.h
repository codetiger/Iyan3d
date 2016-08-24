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
#include "MeshOptimizeHelper.h"

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

    vector<vertexData> tempVerticesData;
    vector<vertexDataHeavy> tempVerticesDataHeavy;
    vector<unsigned int> tempIndicesData;
    
    bool shouldRemoveDoubles, shouldOptimizeIndicesOrder, shouldCalculateTangents, shouldCalculateNormals, shouldGenerateUV;
    bool shouldSplitBuffers;
    float normalSmoothThreshold;

    void reCalculateTangents();
    void calculateTanget(Vector3 &tangent, Vector3 &bitangent, Vector3 vt1, Vector3 vt2, Vector3 vt3, Vector2 tc1, Vector2 tc2, Vector2 tc3);
   
    void checkUVSeam();
    void generateUV();
    void recalculateNormals();

public:
    MESH_TYPE meshType;

    Mesh* clone();
    Mesh* convert2Lite();
    
    void addMeshBuffer(vector<vertexData> mbvd, vector<unsigned short> mbi, unsigned short materialIndex);
    void addMeshBuffer(vector<vertexDataHeavy> mbvd, vector<unsigned short> mbi, unsigned short materialIndex);
    
    void copyDataFromMesh(Mesh* otherMesh);
    void copyInstanceToMeshCache(Mesh *originalMesh, int instanceIndex);
    void removeVerticesOfAnInstance(int verticesCount, int indicesCount);
    void addVertex(vertexData* vertex, bool updateBB = true);
    void addHeavyVertex(vertexDataHeavy* vertex);
    void addToIndicesArray(unsigned int index);
    void Commit(bool forceSplitBuffers = false);

    void clearVerticesArray();
    void clearIndicesArray();
    
    void flipMeshHorizontal();
    void moveVertices(Vector3 offset);
    vertexData* getLiteVertexByIndex(unsigned int index);
    vertexDataHeavy* getHeavyVertexByIndex(unsigned int index);
    vector<vertexData> getLiteVerticesArray(int index);
    vector<vertexDataHeavy> getHeavyVerticesArray(int index);
    vector<vertexData> getTotalLiteVerticesArray();
    vector<vertexDataHeavy> getTotalHeavyVerticesArray();

    vertexDataHeavy* getHeavyVerticesForMeshBuffer(int meshBufferIndex, int vertexIndex);
    vertexData* getLiteVerticesForMeshBuffer(int meshBufferIndex, int vertexIndex);

    unsigned short* getIndicesArray(int index);
    unsigned int * getHighPolyIndicesArray();
    unsigned int getIndicesCount(int index);
    unsigned int getTotalIndicesCount();
    vector< unsigned int > getTotalIndicesArray();
    unsigned int getVerticesCountInMeshBuffer(int index);
    unsigned int getVerticesCount();
    int getMeshBufferCount();
    int getMeshBufferMaterialIndices(int meshBufferIndex);

    void clearVertices();
    void clearIndices();
    BoundingBox* getBoundingBox();
    Vector3 getAngleWeight(Vector3& v1, Vector3& v2, Vector3& v3);
    
    void removeDoublesInMesh();
    void reOrderMeshIndices();
    void setOptimization(bool removeDoubles, bool optimizeIndicesOrder, bool calculateTangents = false, bool shouldCalculateNormals = false, bool shouldGenerateUV = false, float smoothThreshold = 1.0);
    
    Mesh();
    ~Mesh();
};


#endif /* defined(__SGEngine2__Mesh__) */
