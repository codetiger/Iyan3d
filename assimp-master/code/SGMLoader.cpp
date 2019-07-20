/*
---------------------------------------------------------------------------
Open Asset Import Library (assimp)
---------------------------------------------------------------------------

Copyright (c) 2006-2016, assimp team

All rights reserved.

Redistribution and use of this software in source and binary forms,
with or without modification, are permitted provided that the following
conditions are met:

* Redistributions of source code must retain the above
  copyright notice, this list of conditions and the
  following disclaimer.

* Redistributions in binary form must reproduce the above
  copyright notice, this list of conditions and the
  following disclaimer in the documentation and/or other
  materials provided with the distribution.

* Neither the name of the assimp team, nor the names of its
  contributors may be used to endorse or promote products
  derived from this software without specific prior
  written permission of the assimp team.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
---------------------------------------------------------------------------
*/

/** @file  SGMLoader.cpp
 *  @brief Implementation of the SGM importer class
 */


#ifndef ASSIMP_BUILD_NO_SGM_IMPORTER

// internal headers
#include "SGMLoader.h"
#include "StreamReader.h"
#include "fast_atof.h"
#include <assimp/IOSystem.hpp>
#include <assimp/DefaultLogger.hpp>
#include <assimp/scene.h>

#include <fstream>

using namespace Assimp;

static const aiImporterDesc desc = {
    "SGM Importer",
    "",
    "",
    "http://www.iyan3dapp.com/",
    aiImporterFlags_SupportBinaryFlavour,
    0,
    0,
    0,
    0,
    "sgm"
};

// ------------------------------------------------------------------------------------------------
// Constructor to be privately used by Importer
SGMImporter::SGMImporter()
{}

// ------------------------------------------------------------------------------------------------
// Destructor, private as well
SGMImporter::~SGMImporter()
{}

// ------------------------------------------------------------------------------------------------
// Returns whether the class can handle the format of the given file.
bool SGMImporter::CanRead( const std::string& pFile, IOSystem* pIOHandler, bool checkSig) const
{
    const std::string extension = GetExtension(pFile);

    if (extension == "sgm" || extension == "sgr")
        return true;
    
    return false;
}

// ------------------------------------------------------------------------------------------------
const aiImporterDesc* SGMImporter::GetInfo () const
{
    return &desc;
}

std::string SGMImporter::getFileName(const std::string& s)
{
    
    char sep = '/';
    
    size_t i = s.rfind(sep, s.length());
    if (i != std::string::npos) {
        return(s.substr(i+1, s.length() - i));
    } else
        return s;
    
    return("");
}

void SGMImporter::replace(std::string& str, const std::string& from, const std::string& to)
{
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return;
    
    str.replace(start_pos, from.length(), to);
}

// ------------------------------------------------------------------------------------------------
// Imports the given file into the given scene structure.
void SGMImporter::InternReadFile( const std::string& pFile,
    aiScene* pScene, IOSystem* pIOHandler)
{
    const std::string extension = GetExtension(pFile);

    if(extension == "sgm") {
        loadSGM(pFile, pScene);
    } else if(extension == "sgr") {
        loadSGR(pFile, pScene);
    }
}

void SGMImporter::loadSGM(const std::string& pFile, aiScene* pScene)
{
    std::ifstream f(pFile, std::ios::in | std::ios::binary);
    if (!f)
        throw DeadlyImportError("File is either empty or corrupt: " + pFile);
    
    unsigned char versionIdentifier, hasUV;
    f.read((char*)&versionIdentifier, sizeof(unsigned char));
    
    if (versionIdentifier == 0 || versionIdentifier == 3) { // newer version
        f.read((char*)&hasUV, sizeof(unsigned char));
    } else {
        hasUV = versionIdentifier;
    }
    
    pScene->mRootNode = new aiNode();
    pScene->mRootNode->mName.Set("sgm mesh");
    pScene->mRootNode->mNumChildren = 1;
    pScene->mRootNode->mChildren = new aiNode*[1];
    
    aiNode* meshNode = pScene->mRootNode->mChildren[0] = new aiNode();
    meshNode->mName.Set("Mesh Node");
    meshNode->mParent = pScene->mRootNode;
    meshNode->mNumMeshes = 1;
    meshNode->mMeshes = new unsigned int[1];
    meshNode->mMeshes[0] = 0;
    
    pScene->mNumMeshes = 1;
    pScene->mMeshes = new aiMesh*[1];
    pScene->mMeshes[0] = new aiMesh();
    pScene->mMeshes[0]->mPrimitiveTypes = aiPrimitiveType_TRIANGLE;
    
    pScene->mNumMaterials = pScene->mNumMeshes;
    pScene->mMaterials = new aiMaterial*[pScene->mNumMaterials];
    pScene->mMaterials[0] = new aiMaterial();
    
    if (versionIdentifier == 0 || versionIdentifier == 3) {
        SSGMCountHeaderHighPoly counts;
        f.read((char*)&counts, sizeof(SSGMCountHeaderHighPoly));
        
        SSGMVectHeader* verts = new SSGMVectHeader[counts.vertCount];
        for (int i = 0; i < counts.vertCount; i++)
            f.read((char*)&verts[i], sizeof(SSGMVectHeader));
        
        SSGMUVHeader* texs = new SSGMUVHeader[counts.colCount];
        SSGMColHeader* col = new SSGMColHeader[counts.colCount];
        
        if (hasUV == UV_MAPPED) {
            for (int i = 0; i < counts.colCount; i++)
                f.read((char*)&texs[i], sizeof(SSGMUVHeader));
        } else if (hasUV == VERTEX_COLORED) {
            for (int i = 0; i < counts.colCount; i++)
                f.read((char*)&col[i], sizeof(SSGMColHeader));
        }
        
        pScene->mMeshes[0]->mNumVertices = counts.indCount;
        pScene->mMeshes[0]->mVertices = new aiVector3D[counts.indCount];
        pScene->mMeshes[0]->mNormals = new aiVector3D[counts.indCount];
        
        if (hasUV == UV_MAPPED) {
            pScene->mMeshes[0]->mTextureCoords[0] = new aiVector3D[counts.indCount];
        } else if (hasUV == VERTEX_COLORED) {
            pScene->mMeshes[0]->mColors[0] = new aiColor4D[counts.indCount];
        }
        
        for (int i = 0; i < counts.indCount; i++) {
            SSGMIndexHeaderHighPoly ind;
            f.read((char*)&ind, sizeof(SSGMIndexHeaderHighPoly));
            
            pScene->mMeshes[0]->mVertices[i].x = verts[ind.vtInd].vx;
            pScene->mMeshes[0]->mVertices[i].y = verts[ind.vtInd].vy;
            pScene->mMeshes[0]->mVertices[i].z = verts[ind.vtInd].vz;
            
            pScene->mMeshes[0]->mNormals[i].x = verts[ind.vtInd].nx;
            pScene->mMeshes[0]->mNormals[i].y = verts[ind.vtInd].ny;
            pScene->mMeshes[0]->mNormals[i].z = verts[ind.vtInd].nz;
            
            if (hasUV == UV_MAPPED) {
                pScene->mMeshes[0]->mTextureCoords[0][i].x = texs[ind.colInd].s;
                pScene->mMeshes[0]->mTextureCoords[0][i].y = texs[ind.colInd].t;
            } else if (hasUV == VERTEX_COLORED) {
                pScene->mMeshes[0]->mColors[0][i].r = col[ind.colInd].r / 255.0;
                pScene->mMeshes[0]->mColors[0][i].g = col[ind.colInd].g / 255.0;
                pScene->mMeshes[0]->mColors[0][i].b = col[ind.colInd].b / 255.0;
                pScene->mMeshes[0]->mColors[0][i].a = 0.0;
            }
        }
        
        pScene->mMeshes[0]->mNumFaces = counts.indCount / 3;
        pScene->mMeshes[0]->mFaces = new aiFace[counts.indCount / 3];
        
        int inds = 0;
        for (int i = 0; i < counts.indCount / 3; i++) {
            pScene->mMeshes[0]->mFaces[i].mNumIndices = 3;
            pScene->mMeshes[0]->mFaces[i].mIndices = new unsigned int[3];
            
            for (int j = 0; j < 3; j++)
                pScene->mMeshes[0]->mFaces[i].mIndices[j] = inds++;
        }
        
        delete[] verts;
        delete[] texs;
        delete[] col;
    } else {
        SSGMCountHeaderLowPoly counts;
        f.read((char*)&counts, sizeof(SSGMCountHeaderLowPoly));
        
        SSGMVectHeader* verts = new SSGMVectHeader[counts.vertCount];
        for (int i = 0; i < counts.vertCount; i++)
            f.read((char*)&verts[i], sizeof(SSGMVectHeader));
        
        SSGMUVHeader* texs = new SSGMUVHeader[counts.colCount];
        SSGMColHeader* col = new SSGMColHeader[counts.colCount];
        
        if (hasUV == UV_MAPPED) {
            for (int i = 0; i < counts.colCount; i++)
                f.read((char*)&texs[i], sizeof(SSGMUVHeader));
        } else if (hasUV == VERTEX_COLORED) {
            for (int i = 0; i < counts.colCount; i++)
                f.read((char*)&col[i], sizeof(SSGMColHeader));
        }
        
        pScene->mMeshes[0]->mNumVertices = counts.indCount;
        pScene->mMeshes[0]->mVertices = new aiVector3D[counts.indCount];
        pScene->mMeshes[0]->mNormals = new aiVector3D[counts.indCount];
        
        if (hasUV == UV_MAPPED) {
            pScene->mMeshes[0]->mTextureCoords[0] = new aiVector3D[counts.indCount];
        } else if (hasUV == VERTEX_COLORED) {
            pScene->mMeshes[0]->mColors[0] = new aiColor4D[counts.indCount];
        }
        
        for (int i = 0; i < counts.indCount; i++) {
            SSGMIndexHeaderLowPoly ind;
            f.read((char*)&ind, sizeof(SSGMIndexHeaderLowPoly));
            
            pScene->mMeshes[0]->mVertices[i].x = verts[ind.vtInd].vx;
            pScene->mMeshes[0]->mVertices[i].y = verts[ind.vtInd].vy;
            pScene->mMeshes[0]->mVertices[i].z = verts[ind.vtInd].vz;
            
            pScene->mMeshes[0]->mNormals[i].x = verts[ind.vtInd].nx;
            pScene->mMeshes[0]->mNormals[i].y = verts[ind.vtInd].ny;
            pScene->mMeshes[0]->mNormals[i].z = verts[ind.vtInd].nz;
            
            if (hasUV == UV_MAPPED) {
                pScene->mMeshes[0]->mTextureCoords[0][i].x = texs[ind.colInd].s;
                pScene->mMeshes[0]->mTextureCoords[0][i].y = texs[ind.colInd].t;
            } else if (hasUV == VERTEX_COLORED) {
                pScene->mMeshes[0]->mColors[0][i].r = col[ind.colInd].r / 255.0;
                pScene->mMeshes[0]->mColors[0][i].g = col[ind.colInd].g / 255.0;
                pScene->mMeshes[0]->mColors[0][i].b = col[ind.colInd].b / 255.0;
                pScene->mMeshes[0]->mColors[0][i].a = 0.0;
            }
        }
        
        pScene->mMeshes[0]->mNumFaces = counts.indCount / 3;
        pScene->mMeshes[0]->mFaces = new aiFace[counts.indCount / 3];
        
        int inds = 0;
        for (int i = 0; i < counts.indCount / 3; i++) {
            pScene->mMeshes[0]->mFaces[i].mNumIndices = 3;
            pScene->mMeshes[0]->mFaces[i].mIndices = new unsigned int[3];
            
            for (int j = 0; j < 3; j++)
                pScene->mMeshes[0]->mFaces[i].mIndices[j] = inds++;
        }
        
        delete[] verts;
        delete[] texs;
        delete[] col;
    }
    
    std::string textureFileName = getFileName(pFile);
    replace(textureFileName, ".sgm", "-cm.png");
    
    aiString s;
    s.Set(textureFileName);
    pScene->mMaterials[0]->AddProperty(&s, AI_MATKEY_TEXTURE_DIFFUSE(0));
}

aiMatrix4x4 makeAssimpMat(float *mtrix)
{
    aiMatrix4x4 assimpMatrix;
    
    assimpMatrix.a1 = mtrix[0];
    assimpMatrix.b1 = mtrix[1];
    assimpMatrix.c1 = mtrix[2];
    assimpMatrix.d1 = mtrix[3];
    
    assimpMatrix.a2 = mtrix[4];
    assimpMatrix.b2 = mtrix[5];
    assimpMatrix.c2 = mtrix[6];
    assimpMatrix.d2 = mtrix[7];
    
    assimpMatrix.a3 = mtrix[8];
    assimpMatrix.b3 = mtrix[9];
    assimpMatrix.c3 = mtrix[10];
    assimpMatrix.d3 = mtrix[11];
    
    assimpMatrix.a4 = mtrix[12];
    assimpMatrix.b4 = mtrix[13];
    assimpMatrix.c4 = mtrix[14];
    assimpMatrix.d4 = mtrix[15];

    return assimpMatrix;
}

struct BoneStruct {
    std::string name;
    aiMatrix4x4 transform;
    std::string parentName;
};

void addChildren(aiNode* parent, std::vector <BoneStruct> boneList)
{
    std::string parentName = std::string(parent->mName.C_Str());
    
    std::vector <aiNode* > children;
    for (int i = 0; i < boneList.size(); i++) {

        if(boneList[i].parentName == parentName) {
            aiNode* child = new aiNode();
            aiString s;
            s.Set(boneList[i].name);
            child->mName = s;
            
            child->mTransformation = boneList[i].transform;
            child->mParent = parent;
            
            children.push_back(child);
        }
    }

    parent->mNumChildren = children.size();
    if(children.size()) {
        parent->mChildren = new aiNode*[children.size()];

        for (int i = 0; i < children.size(); i++) {
            parent->mChildren[i] = children[i];
            addChildren(parent->mChildren[i], boneList);
        }
    }
}

void SGMImporter::loadSGR(const std::string& pFile, aiScene* pScene)
{
    std::ifstream f(pFile, std::ios::in | std::ios::binary);
    if (!f)
        throw DeadlyImportError("File is either empty or corrupt: " + pFile);
    
    unsigned short versionIdentifier;
    unsigned int vertCount = 0;
    f.read((char*)&versionIdentifier, sizeof(unsigned short));
    
    if (versionIdentifier == 0 || versionIdentifier == 1) // for large Mesh identification
        f.read((char*)&vertCount, sizeof(unsigned int));
    else
        vertCount = versionIdentifier;
    
    pScene->mRootNode = new aiNode();
    pScene->mRootNode->mName.Set("Scene");
    pScene->mRootNode->mNumChildren = 2;
    pScene->mRootNode->mChildren = new aiNode*[2];
    
    aiNode* meshNode = pScene->mRootNode->mChildren[0] = new aiNode();
    meshNode->mName.Set("Mesh Node");
    meshNode->mParent = pScene->mRootNode;
    
    meshNode->mNumMeshes = 1;
    meshNode->mMeshes = new unsigned int[1];
    meshNode->mMeshes[0] = 0;
    
    pScene->mNumMeshes = 1;
    pScene->mMeshes = new aiMesh*[1];
    pScene->mMeshes[0] = new aiMesh();
    pScene->mMeshes[0]->mPrimitiveTypes = aiPrimitiveType_TRIANGLE;
    
    pScene->mNumMaterials = pScene->mNumMeshes;
    pScene->mMaterials = new aiMaterial*[pScene->mNumMaterials];
    pScene->mMaterials[0] = new aiMaterial();
    
    pScene->mMeshes[0]->mNumVertices = vertCount;
    pScene->mMeshes[0]->mVertices = new aiVector3D[vertCount];
    pScene->mMeshes[0]->mNormals = new aiVector3D[vertCount];
    pScene->mMeshes[0]->mTextureCoords[0] = new aiVector3D[vertCount];

    for (int i = 0; i < vertCount; i++) {
        SSGRVectHeader vert;
        f.read((char*)&vert, sizeof(SSGRVectHeader));
        
        pScene->mMeshes[0]->mVertices[i].x = vert.vx;
        pScene->mMeshes[0]->mVertices[i].y = vert.vy;
        pScene->mMeshes[0]->mVertices[i].z = vert.vz;
        
        pScene->mMeshes[0]->mNormals[i].x = vert.nx;
        pScene->mMeshes[0]->mNormals[i].y = vert.ny;
        pScene->mMeshes[0]->mNormals[i].z = vert.nz;
        
        pScene->mMeshes[0]->mTextureCoords[0][i].x = vert.s;
        pScene->mMeshes[0]->mTextureCoords[0][i].y = vert.t;
    }
    
    unsigned int indicesCount = 0;
    if (versionIdentifier > 1) {
        unsigned short indCount;
        f.read((char*)&indCount, sizeof(unsigned short));
        indicesCount = (unsigned int)indCount;
    } else {
        unsigned int indCount;
        f.read((char*)&indCount, sizeof(unsigned int));
        indicesCount = indCount;
    }
    
    pScene->mMeshes[0]->mNumFaces = indicesCount / 3;
    pScene->mMeshes[0]->mFaces = new aiFace[indicesCount / 3];
    
    for (int i = 0; i < indicesCount / 3; i++) {
        pScene->mMeshes[0]->mFaces[i].mNumIndices = 3;
        pScene->mMeshes[0]->mFaces[i].mIndices = new unsigned int[3];
        
        for (int j = 0; j < 3; j++) {
            unsigned int vertexIndex = 0;
            if (versionIdentifier > 1) {
                unsigned short ind;
                f.read((char*)&ind, sizeof(unsigned short));
                vertexIndex = (unsigned int)ind;
            } else {
                unsigned int ind;
                f.read((char*)&ind, sizeof(unsigned int));
                vertexIndex = ind;
            }
            pScene->mMeshes[0]->mFaces[i].mIndices[j] = vertexIndex;
        }
    }

    std::string textureFileName = getFileName(pFile);
    replace(textureFileName, ".sgr", "-cm.png");
    
    aiString s;
    s.Set(textureFileName);
    pScene->mMaterials[0]->AddProperty(&s, AI_MATKEY_TEXTURE_DIFFUSE(0));

    std::vector <BoneStruct> boneList;
    std::vector <aiBone*> aiBoneList;
    
    unsigned short boneCount;
    f.read((char*)&boneCount, sizeof(unsigned short));
    for (int i = 0; i < boneCount; i++) {
        short boneParentInd;
        f.read((char*)&boneParentInd, sizeof(short));
        
        float* boneMatrix = new float[16];
        f.read((char*)boneMatrix, sizeof(float) * 16);
        
        BoneStruct b;
        b.name = "sg00" + std::to_string(i);
        
        if (boneParentInd >= 0) {
            b.parentName = "sg00" + std::to_string(boneParentInd);
        } else {
            b.parentName = "Armature";
        }
        
        b.transform = makeAssimpMat(boneMatrix);
        boneList.push_back(b);
        
        delete[] boneMatrix;
        
        unsigned short boneWeightCount;
        f.read((char*)&boneWeightCount, sizeof(unsigned short));
        
        aiBone *bone = NULL;
        
        if(boneWeightCount > 0) {
            bone = new aiBone();
            s.Set("sg00" + std::to_string(i));
            bone->mName = s;
            bone->mNumWeights = boneWeightCount;
            bone->mWeights = new aiVertexWeight[boneWeightCount];
        }
        
        for (int j = 0; j < boneWeightCount; ++j) {
            unsigned int vertexIndex = 0;
            if (versionIdentifier > 1) {
                unsigned short ind;
                f.read((char*)&ind, sizeof(unsigned short));
                vertexIndex = (unsigned int)ind;
            }
            else {
                unsigned int ind;
                f.read((char*)&ind, sizeof(unsigned int));
                vertexIndex = ind;
            }
            unsigned short vertWeight;
            f.read((char*)&vertWeight, sizeof(unsigned short));
            
            bone->mWeights[j].mVertexId = vertexIndex;
            bone->mWeights[j].mWeight = ((float)vertWeight) / 255.0f;
        }
        
        if(bone)
            aiBoneList.push_back(bone);
    }

    pScene->mMeshes[0]->mNumBones = aiBoneList.size();
    pScene->mMeshes[0]->mBones = new aiBone*[aiBoneList.size()];
    
    for (int i = 0; i < aiBoneList.size(); i++)
        pScene->mMeshes[0]->mBones[i] = aiBoneList[i];

    aiNode* armatureNode = pScene->mRootNode->mChildren[1] = new aiNode();
    s.Set("Armature");
    armatureNode->mName = s;
    armatureNode->mParent = pScene->mRootNode;
    
    addChildren(armatureNode, boneList);
}

#endif // !! ASSIMP_BUILD_NO_SGM_IMPORTER
