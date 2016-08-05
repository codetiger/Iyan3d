//
//  MeshOptimizeHelper.cpp
//  Iyan3D
//
//  Created by Harishankarn on 04/08/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//


#include "MeshOptimizeHelper.h"

#include <math.h>
#include <vector>
#include <limits>
#include <algorithm>

//-----------------------------------------------------------------------------
//  OptimizeFaces
//-----------------------------------------------------------------------------
//  Parameters:
//      indexList
//          input index list
//      indexCount
//          the number of indices in the list
//      vertexCount
//          the largest index value in indexList
//      newIndexList
//          a pointer to a preallocated buffer the same size as indexList to
//          hold the optimized index list
//      lruCacheSize
//          the size of the simulated post-transform cache (max:64)
//-----------------------------------------------------------------------------

MeshOptimizeHelper::MeshOptimizeHelper() {
    ComputeVertexScores();
}

MeshOptimizeHelper::~MeshOptimizeHelper() {
    
}

float MeshOptimizeHelper::ComputeVertexCacheScore(int cachePosition, int vertexCacheSize)
{
    const float FindVertexScore_CacheDecayPower = 1.5f;
    const float FindVertexScore_LastTriScore = 0.75f;
    
    float score = 0.0f;
    if ( cachePosition >= 0 ) {
        if ( cachePosition < 3 )
            score = FindVertexScore_LastTriScore;
        else
        {
            const float scaler = 1.0f / ( vertexCacheSize - 3 );
            score = 1.0f - ( cachePosition - 3 ) * scaler;
            score = powf ( score, FindVertexScore_CacheDecayPower );
        }
    }
    
    return score;
}

float MeshOptimizeHelper::ComputeVertexValenceScore(uint numActiveFaces)
{
    const float FindVertexScore_ValenceBoostScale = 2.0f;
    const float FindVertexScore_ValenceBoostPower = 0.5f;
    
    float score = 0.f;
    
    // Bonus points for having a low number of tris still to
    // use the vert, so we get rid of lone verts quickly.
    float valenceBoost = powf ( static_cast<float>(numActiveFaces),
                               -FindVertexScore_ValenceBoostPower );
    score += FindVertexScore_ValenceBoostScale * valenceBoost;
    
    return score;
}

void MeshOptimizeHelper::ComputeVertexScores()
{
    for (int cacheSize = 0; cacheSize <= kMaxVertexCacheSize; ++cacheSize)
        for (int cachePos=0; cachePos<cacheSize; ++cachePos)
            s_vertexCacheScores[cacheSize][cachePos] = ComputeVertexCacheScore(cachePos, cacheSize);
    
    for (uint valence=0; valence<kMaxPrecomputedVertexValenceScores; ++valence)
        s_vertexValenceScores[valence] = ComputeVertexValenceScore(valence);
    
    s_vertexScoresComputed = true;
}

float MeshOptimizeHelper::FindVertexScore(uint numActiveFaces, uint cachePosition, uint vertexCacheSize)
{
    
    if ( numActiveFaces == 0 )
        return -1.0f;
    
    float score = 0.f;
    if (cachePosition < vertexCacheSize)
        score += s_vertexCacheScores[vertexCacheSize][cachePosition];
    
    if (numActiveFaces < kMaxPrecomputedVertexValenceScores)
        score += s_vertexValenceScores[numActiveFaces];
    else
        score += ComputeVertexValenceScore(numActiveFaces);
    
    return score;
}

void MeshOptimizeHelper::OptimizeFaces(const unsigned int* indexList, unsigned int indexCount, unsigned int vertexCount, unsigned int* newIndexList, unsigned int lruCacheSize)
{
    std::vector<OptimizeVertexData> vertexDataList;
    vertexDataList.resize(vertexCount);
    
    // compute face count per vertex
    for (unsigned int i=0; i<indexCount; ++i) {
        unsigned int index = indexList[i];
        OptimizeVertexData& vertexData = vertexDataList[index];
        vertexData.activeFaceListSize++;
    }
    
    std::vector<unsigned int> activeFaceList;
    
    const unsigned int kEvictedCacheIndex = std::numeric_limits<unsigned int>::max();
    
    {
        // allocate face list per vertex
        unsigned int curActiveFaceListPos = 0;
        for (unsigned int i=0; i<vertexCount; ++i)
        {
            OptimizeVertexData& vertexData = vertexDataList[i];
            vertexData.cachePos0 = kEvictedCacheIndex;
            vertexData.cachePos1 = kEvictedCacheIndex;
            vertexData.activeFaceListStart = curActiveFaceListPos;
            curActiveFaceListPos += vertexData.activeFaceListSize;
            vertexData.score = FindVertexScore(vertexData.activeFaceListSize, vertexData.cachePos0, lruCacheSize);
            vertexData.activeFaceListSize = 0;
        }
        activeFaceList.resize(curActiveFaceListPos);
    }
    
    // fill out face list per vertex
    for (unsigned int i = 0; i < indexCount; i += 3) {
        for (unsigned int j = 0; j < 3; ++j) {
            unsigned int index = indexList[i+j];
            OptimizeVertexData& vertexData = vertexDataList[index];
            activeFaceList[vertexData.activeFaceListStart + vertexData.activeFaceListSize] = i;
            vertexData.activeFaceListSize++;
        }
    }
    
    std::vector<unsigned int> processedFaceList;
    processedFaceList.resize(indexCount);
    
    unsigned int vertexCacheBuffer[(kMaxVertexCacheSize+3)*2];
    unsigned int* cache0 = vertexCacheBuffer;
    unsigned int* cache1 = vertexCacheBuffer+(kMaxVertexCacheSize+3);
    unsigned int entriesInCache0 = 0;
    
    uint bestFace = 0;
    float bestScore = -1.f;
    
    const float maxValenceScore = FindVertexScore(1, kEvictedCacheIndex, lruCacheSize) * 3.f;
    
    for (uint i = 0; i < indexCount; i += 3) {
        if (bestScore < 0.f)  {
            for (uint j = 0; j < indexCount; j += 3) {
                if (processedFaceList[j] == 0) {
                    uint face = j;
                    float faceScore = 0.f;
                    for (uint k = 0; k < 3; ++k) {
                        unsigned int index = indexList[face+k];
                        OptimizeVertexData& vertexData = vertexDataList[index];
                        faceScore += vertexData.score;
                    }
                    
                    if (faceScore > bestScore) {
                        bestScore = faceScore;
                        bestFace = face;
                        
                        if (bestScore >= maxValenceScore)
                            break;
                    }
                }
            }
        }
        
        processedFaceList[bestFace] = 1;
        unsigned int entriesInCache1 = 0;
        
        // add bestFace to LRU cache and to newIndexList
        for (uint v = 0; v < 3; ++v)
        {
            unsigned int index = indexList[bestFace+v];
            newIndexList[i+v] = index;
            
            OptimizeVertexData& vertexData = vertexDataList[index];
            
            if (vertexData.cachePos1 >= entriesInCache1)
            {
                vertexData.cachePos1 = entriesInCache1;
                cache1[entriesInCache1++] = index;
                
                if (vertexData.activeFaceListSize == 1)
                {
                    --vertexData.activeFaceListSize;
                    continue;
                }
            }
            
            uint* begin = &activeFaceList[vertexData.activeFaceListStart];
            uint* end = &(activeFaceList[vertexData.activeFaceListStart + vertexData.activeFaceListSize - 1]) + 1;
            uint* it = std::find(begin, end, bestFace);
            std::swap(*it, *(end-1));
            --vertexData.activeFaceListSize;
            vertexData.score = FindVertexScore(vertexData.activeFaceListSize, vertexData.cachePos1, lruCacheSize);
            
        }
        
        // move the rest of the old verts in the cache down and compute their new scores
        for (uint c0 = 0; c0 < entriesInCache0; ++c0)
        {
            unsigned int index = cache0[c0];
            OptimizeVertexData& vertexData = vertexDataList[index];
            
            if (vertexData.cachePos1 >= entriesInCache1)
            {
                vertexData.cachePos1 = entriesInCache1;
                cache1[entriesInCache1++] = index;
                vertexData.score = FindVertexScore(vertexData.activeFaceListSize, vertexData.cachePos1, lruCacheSize);
            }
        }
        
        // find the best scoring triangle in the current cache (including up to 3 that were just evicted)
        bestScore = -1.f;
        for (unsigned int c1 = 0; c1 < entriesInCache1; ++c1)
        {
            unsigned int index = cache1[c1];
            OptimizeVertexData& vertexData = vertexDataList[index];
            vertexData.cachePos0 = vertexData.cachePos1;
            vertexData.cachePos1 = kEvictedCacheIndex;
            for (unsigned int j = 0; j < vertexData.activeFaceListSize; ++j)
            {
                unsigned int face = activeFaceList[vertexData.activeFaceListStart+j];
                float faceScore = 0.f;
                for (unsigned int v = 0; v < 3; v++)
                {
                    unsigned int faceIndex = indexList[face+v];
                    OptimizeVertexData& faceVertexData = vertexDataList[faceIndex];
                    faceScore += faceVertexData.score;
                }
                if (faceScore > bestScore)
                {
                    bestScore = faceScore;
                    bestFace = face;
                }
            }
        }
        
        std::swap(cache0, cache1);
        entriesInCache0 = std::min(entriesInCache1, lruCacheSize);
    }
}
