//
//  MeshOptimizeHelper.hpp
//  Iyan3D
//
//  Created by Harishankarn on 04/08/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#ifndef MeshOptimizeHelper_hpp
#define MeshOptimizeHelper_hpp

#include "Mesh.h"
#include <stdio.h>

struct OptimizeVertexData
{
    float   score;
    unsigned int    activeFaceListStart;
    unsigned int    activeFaceListSize;
    unsigned int  cachePos0;
    unsigned int  cachePos1;
    OptimizeVertexData() : score(0.f), activeFaceListStart(0), activeFaceListSize(0), cachePos0(0), cachePos1(0) { }
};

class MeshOptimizeHelper {
    
private:
    const int kMaxVertexCacheSize = 64;
    const uint kMaxPrecomputedVertexValenceScores = 64;
    float s_vertexCacheScores[64+1][64];
    float s_vertexValenceScores[64];
    bool s_vertexScoresComputed = false;

    float ComputeVertexCacheScore(int cachePosition, int vertexCacheSize);
    float ComputeVertexValenceScore(unsigned int numActiveFaces);
    void ComputeVertexScores();
    float FindVertexScore(unsigned int numActiveFaces, unsigned int cachePosition, unsigned int vertexCacheSize);
    
public:
    MeshOptimizeHelper();
    ~MeshOptimizeHelper();
    void OptimizeFaces(const unsigned int* indexList, unsigned int indexCount, unsigned int vertexCount, unsigned int* newIndexList, unsigned int lruCacheSize);
};

#endif /* MeshOptimizeHelper_hpp */
