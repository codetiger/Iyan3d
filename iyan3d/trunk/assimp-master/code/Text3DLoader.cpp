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
#include "Text3DLoader.h"
#include "StreamReader.h"
#include "fast_atof.h"
#include <assimp/IOSystem.hpp>
#include <assimp/DefaultLogger.hpp>
#include <assimp/scene.h>

#include <fstream>
#include <iostream>

using namespace Assimp;

static const aiImporterDesc desc = {
    "Text3D Importer",
    "",
    "",
    "http://www.iyan3dapp.com/",
    aiImporterFlags_SupportBinaryFlavour,
    0,
    0,
    0,
    0,
    "ttf otf"
};

vector < aiVector3D > vertices;
vector < aiVector3D > textCoords;

typedef GLvoid (*GLUTesselatorFunction) ();

void sgBeginCallback(GLenum which)
{
}

void sgEndCallback(void)
{
}

void sgFlagCallback( GLboolean )
{
    
}

void sgErrorCallback(GLenum errorCode)
{
    const GLubyte *estring;
    
    estring = gluErrorString(errorCode);
    fprintf (stderr, "Tessellation Error: %s\n", estring);
}

void sgVertexCallback(GLvoid *vertex)
{
    const GLdouble *d;
    d = (GLdouble *) vertex;
    vertices.push_back(aiVector3D(-d[0], d[1], d[2]));
}

void sgCombineCallback(GLdouble coords[3], GLdouble *vertex_data[4], GLfloat weight[4], GLdouble **dataOut)
{
    GLdouble *vertex;
    
    vertex = (GLdouble *) malloc(6 * sizeof(GLdouble));
    vertex[0] = coords[0];
    vertex[1] = coords[1];
    vertex[2] = coords[2];
    *dataOut = vertex;
}

// ------------------------------------------------------------------------------------------------
// Constructor to be privately used by Importer
Text3DImporter::Text3DImporter()
{
}

// ------------------------------------------------------------------------------------------------
// Destructor, private as well
Text3DImporter::~Text3DImporter()
{
}

// ------------------------------------------------------------------------------------------------
// Setup configuration properties for the loader
void Text3DImporter::SetupProperties(const Importer* pImp)
{
    text2Convert = pImp->GetPropertyWString("TEXT3D_TEXT");
    shouldHaveBones = pImp->GetPropertyBool("TEXT3D_HASBONES");
    fontPath = pImp->GetPropertyString("TEXT3D_FONTPATH");
    bezierSteps = pImp->GetPropertyInteger("TEXT3D_BEZIERSTEPS");
    extrude = -pImp->GetPropertyFloat("TEXT3D_EXTRUDE");
    bevelRadius = pImp->GetPropertyFloat("TEXT3D_BEVELRADIUS");
    bevelSegments = pImp->GetPropertyInteger("TEXT3D_BEVELSEGMENTS");
}

// ------------------------------------------------------------------------------------------------
// Returns whether the class can handle the format of the given file.
bool Text3DImporter::CanRead( const string& pFile, IOSystem* pIOHandler, bool checkSig) const
{
    const string extension = GetExtension(pFile);

    if (extension == "otf" || extension == "ttf")
        return true;
    
    return false;
}

// ------------------------------------------------------------------------------------------------
const aiImporterDesc* Text3DImporter::GetInfo () const
{
    return &desc;
}

Vectoriser* Text3DImporter::generateVertices(FT_Face face, wchar_t ch, double strength)
{
    FT_UInt charIndex = FT_Get_Char_Index(face, ch);
    if(FT_Load_Glyph(face, charIndex, FT_LOAD_DEFAULT)) {
        printf("Error Loading Char from Font");
        return NULL;
    }
    
    FT_Glyph glyph;
    if(FT_Get_Glyph(face->glyph, &glyph)) {
        printf("Error Loading Char from Font");
        return NULL;
    }
    
    if(glyph->format != FT_GLYPH_FORMAT_OUTLINE) {
        printf("Error Loading Char from Font");
        return NULL;
    }
    
    if(strength != 0.0) {
        FT_Outline_Embolden(&face->glyph->outline, strength * 64.0);
        FT_Outline_Translate(&face->glyph->outline, -strength * 64.0 * 0.5, -strength * 64.0 * 0.5);
    }
    FT_Orientation f = FT_Outline_Get_Orientation(&face->glyph->outline);
    
    Vectoriser *v = new Vectoriser(face->glyph->outline, bezierSteps, f==true);
    FT_Done_Glyph(glyph);
    
    return v;
}

double Text3DImporter::AddCharacter(FT_Face face, wchar_t ch, double offset)
{
    static FT_UInt prevCharIndex = 0, curCharIndex = 0;
    static FT_Pos  prev_rsb_delta = 0;
    
    curCharIndex = FT_Get_Char_Index( face, ch );
    if(FT_Load_Glyph( face, curCharIndex, FT_LOAD_DEFAULT )){
        printf("Error Loading Char from Font");
        return -10000.0;
    }
    
    FT_Glyph glyph;
    if(FT_Get_Glyph( face->glyph, &glyph )){
        printf("Error Loading Char from Font");
        return -10000.0;
    }
    
    if(glyph->format != FT_GLYPH_FORMAT_OUTLINE) {
        printf("Error Loading Char from Font");
        return -10000.0;
    }
    
    short nCountour = 0;
    nCountour = face->glyph->outline.n_contours;
    
    if(FT_HAS_KERNING( face ) && prevCharIndex) {
        FT_Vector  kerning;
        FT_Get_Kerning( face, prevCharIndex, curCharIndex, FT_KERNING_DEFAULT, &kerning );
        offset += kerning.x >> 6;
    }
    
    if ( prev_rsb_delta - face->glyph->lsb_delta >= 32 )
        offset -= 1.0;
    else if ( prev_rsb_delta - face->glyph->lsb_delta < -32 )
        offset += 1.0;
    
    prev_rsb_delta = face->glyph->rsb_delta;
    
    Vectoriser* vectoriser = generateVertices(face, ch);
    for(size_t c = 0; c < vectoriser->ContourCount(); c++) {
        const Contour* contour = vectoriser->GetContour(c);
        
        if(contour->GetDirection()) {
            GLUtesselator* tobj = gluNewTess();
            
            gluTessCallback(tobj, GLU_TESS_VERTEX, (GLUTesselatorFunction) sgVertexCallback);
            gluTessCallback(tobj, GLU_TESS_BEGIN, (GLUTesselatorFunction) sgBeginCallback);
            gluTessCallback(tobj, GLU_TESS_END, (GLUTesselatorFunction) sgEndCallback);
            gluTessCallback(tobj, GLU_TESS_ERROR, (GLUTesselatorFunction) sgErrorCallback);
            gluTessCallback(tobj, GLU_TESS_COMBINE, (GLUTesselatorFunction) sgCombineCallback);
            gluTessCallback(tobj, GLU_TESS_EDGE_FLAG, (GLUTesselatorFunction) sgFlagCallback);
            
            gluTessProperty(tobj, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_ODD);
            gluTessNormal(tobj, 0.0, 0.0, 0.0);
            
            gluTessBeginPolygon(tobj, NULL);
            for(size_t c = 0; c < vectoriser->ContourCount(); ++c) {
                const Contour* contour = vectoriser->GetContour(c);
                gluTessBeginContour(tobj);
                for(size_t p = 0; p < contour->PointCount(); ++p) {
                    const double* d1 = contour->GetPoint(p);
                    double *d = new double[3];
                    d[0] = d1[0]/64.0 + offset;
                    d[1] = d1[1]/64.0;
                    d[2] = bevelRadius;
                    gluTessVertex(tobj, (GLdouble *)d, (GLvoid *)d);
                }
                
                gluTessEndContour(tobj);
            }
            gluTessEndPolygon(tobj);
            
            gluTessBeginPolygon(tobj, NULL);
            for(size_t c = 0; c < vectoriser->ContourCount(); ++c) {
                const Contour* contour = vectoriser->GetContour(c);
                gluTessBeginContour(tobj);
                for(int p = (int)contour->PointCount() - 1; p >= 0; --p) {
                    const double* d1 = contour->GetPoint(p);
                    double *d = new double[3];
                    d[0] = d1[0]/64.0 + offset;
                    d[1] = d1[1]/64.0;
                    d[2] = extrude - bevelRadius;
                    gluTessVertex(tobj, (GLdouble *)d, (GLvoid *)d);
                }
                
                gluTessEndContour(tobj);
            }
            gluTessEndPolygon(tobj);
            gluDeleteTess(tobj);
        }
    }
    
    FT_Done_Glyph(glyph);
    delete vectoriser;
    vectoriser = NULL;
    
    prevCharIndex = curCharIndex;
    double chSize = face->glyph->advance.x >> 6;
    return offset + chSize;
}

void Text3DImporter::AddBevel(FT_Face face, wchar_t ch, double offset)
{
    static FT_UInt prevCharIndex = 0, curCharIndex = 0;
    static FT_Pos  prev_rsb_delta = 0;
    
    curCharIndex = FT_Get_Char_Index( face, ch );
    if(FT_Load_Glyph( face, curCharIndex, FT_LOAD_DEFAULT )){
        printf("Error Loading Char from Font");
        return;
    }
    
    FT_Glyph glyph;
    if(FT_Get_Glyph( face->glyph, &glyph )){
        printf("Error Loading Char from Font");
        return;
    }
    
    if(glyph->format != FT_GLYPH_FORMAT_OUTLINE) {
        printf("Error Loading Char from Font");
        return;
    }
    
    short nCountour = 0;
    nCountour = face->glyph->outline.n_contours;
    
    if(FT_HAS_KERNING( face ) && prevCharIndex) {
        FT_Vector  kerning;
        FT_Get_Kerning( face, prevCharIndex, curCharIndex, FT_KERNING_DEFAULT, &kerning );
        offset += kerning.x >> 6;
    }
    
    if ( prev_rsb_delta - face->glyph->lsb_delta >= 32 )
        offset -= 1.0;
    else if ( prev_rsb_delta - face->glyph->lsb_delta < -32 )
        offset += 1.0;
    
    prev_rsb_delta = face->glyph->rsb_delta;
    
    for (int i = 0; i < bevelSegments; i++) {
        double bevelX = bevelRadius * 1.5 * sinf(M_PI * 0.5 * i / (double)bevelSegments);
        double nextBevelX = bevelRadius * 1.5  * sinf(M_PI * 0.5 * (i+1) / (double)bevelSegments);
        
        Vectoriser* vectoriser1 = generateVertices(face, ch, bevelX);
        Vectoriser* vectoriser2 = generateVertices(face, ch, nextBevelX);
        
        for(size_t c = 0; c < vectoriser1->ContourCount(); c++) {
            const Contour* contour1 = vectoriser1->GetContour(c);
            const Contour* contour2 = vectoriser2->GetContour(c);
            
            for(size_t p = 0; p < contour1->PointCount(); p++) {
                const double* d1 = contour1->GetPoint(p);
                const double* d2 = contour1->GetPoint(p + 1);
                
                if(p == contour1->PointCount() - 1)
                    d2 = contour1->GetPoint(0);
                
                aiVector3D v1 = aiVector3D(d1[0]/64.0, d1[1]/64.0, 0.0);
                aiVector3D v2 = aiVector3D(d2[0]/64.0, d2[1]/64.0, 0.0);
                
                const double* d3 = contour2->GetPoint(p);
                const double* d4 = contour2->GetPoint(p + 1);
                
                if(p == contour2->PointCount() - 1)
                    d4 = contour2->GetPoint(0);
                
                aiVector3D v3 = aiVector3D(d3[0]/64.0, d3[1]/64.0, 0.0);
                aiVector3D v4 = aiVector3D(d4[0]/64.0, d4[1]/64.0, 0.0);
                
                double bevelY = bevelRadius * cosf(M_PI * 0.5 * i / (double)bevelSegments);
                double nextBevelY = bevelRadius * cosf(M_PI * 0.5 * (i+1) / (double)bevelSegments);
                
                vertices.push_back(aiVector3D(-(v1.x + offset), v1.y, bevelY));
                vertices.push_back(aiVector3D(-(v3.x + offset), v3.y, nextBevelY));
                vertices.push_back(aiVector3D(-(v2.x + offset), v2.y, bevelY));
                vertices.push_back(aiVector3D(-(v3.x + offset), v3.y, nextBevelY));
                vertices.push_back(aiVector3D(-(v4.x + offset), v4.y, nextBevelY));
                vertices.push_back(aiVector3D(-(v2.x + offset), v2.y, bevelY));

                double eBevelY = extrude - (bevelRadius * cosf(M_PI * 0.5 * i / (double)bevelSegments));
                double eNextBevelY = extrude - (bevelRadius * cosf(M_PI * 0.5 * (i+1) / (double)bevelSegments));
                
                vertices.push_back(aiVector3D(-(v1.x + offset), v1.y, eBevelY));
                vertices.push_back(aiVector3D(-(v2.x + offset), v2.y, eBevelY));
                vertices.push_back(aiVector3D(-(v3.x + offset), v3.y, eNextBevelY));
                vertices.push_back(aiVector3D(-(v3.x + offset), v3.y, eNextBevelY));
                vertices.push_back(aiVector3D(-(v2.x + offset), v2.y, eBevelY));
                vertices.push_back(aiVector3D(-(v4.x + offset), v4.y, eNextBevelY));
            }
        }
        delete vectoriser1;
        delete vectoriser2;
        vectoriser1 = NULL;
        vectoriser2 = NULL;
    }
    
    FT_Done_Glyph(glyph);
    
    prevCharIndex = curCharIndex;
}

void Text3DImporter::AddCharacterSideFace(FT_Face face, wchar_t ch, double offset)
{
    static FT_UInt prevCharIndex = 0, curCharIndex = 0;
    static FT_Pos  prev_rsb_delta = 0;
    
    curCharIndex = FT_Get_Char_Index( face, ch );
    if(FT_Load_Glyph( face, curCharIndex, FT_LOAD_DEFAULT )){
        printf("Error Loading Char from Font");
        return;
    }
    
    FT_Glyph glyph;
    if(FT_Get_Glyph( face->glyph, &glyph )){
        printf("Error Loading Char from Font");
        return;
    }
    
    if(glyph->format != FT_GLYPH_FORMAT_OUTLINE) {
        printf("Error Loading Char from Font");
        return;
    }
    
    short nCountour = 0;
    nCountour = face->glyph->outline.n_contours;
    
    if(FT_HAS_KERNING( face ) && prevCharIndex) {
        FT_Vector  kerning;
        FT_Get_Kerning( face, prevCharIndex, curCharIndex, FT_KERNING_DEFAULT, &kerning );
        offset += kerning.x >> 6;
    }
    
    if ( prev_rsb_delta - face->glyph->lsb_delta >= 32 )
        offset -= 1.0;
    else if ( prev_rsb_delta - face->glyph->lsb_delta < -32 )
        offset += 1.0;
    
    prev_rsb_delta = face->glyph->rsb_delta;
    
    Vectoriser* vectoriser = generateVertices(face, ch, bevelRadius * 1.5);
    for(size_t c = 0; c < vectoriser->ContourCount(); c++) {
        const Contour* contour = vectoriser->GetContour(c);
        
        for(size_t p = 0; p < contour->PointCount(); p++) {
            const double* d1 = contour->GetPoint(p);
            const double* d2 = contour->GetPoint(p + 1);
            
            if(p != contour->PointCount() - 1)
                d2 = contour->GetPoint(p + 1);
            else
                d2 = contour->GetPoint(0);
            
            vertices.push_back(aiVector3D(-(d1[0]/64.0 + offset), d1[1]/64.0, 0.0));
            vertices.push_back(aiVector3D(-(d1[0]/64.0 + offset), d1[1]/64.0, extrude));
            vertices.push_back(aiVector3D(-(d2[0]/64.0 + offset), d2[1]/64.0, 0.0));
            vertices.push_back(aiVector3D(-(d1[0]/64.0 + offset), d1[1]/64.0, extrude));
            vertices.push_back(aiVector3D(-(d2[0]/64.0 + offset), d2[1]/64.0, extrude));
            vertices.push_back(aiVector3D(-(d2[0]/64.0 + offset), d2[1]/64.0, 0.0));
        }
    }
    
    delete vectoriser;
    vectoriser = NULL;
    
    FT_Done_Glyph(glyph);
    
    prevCharIndex = curCharIndex;
}

// ------------------------------------------------------------------------------------------------
// Imports the given file into the given scene structure.
void Text3DImporter::InternReadFile( const string& pFile, aiScene* pScene, IOSystem* pIOHandler)
{
    const string extension = GetExtension(pFile);
    
    FT_Library library;
    if (FT_Init_FreeType(&library)) {
        printf("FT_Init_FreeType failed\n");
        return;
    }
    
    FT_Face face;
    if (FT_New_Face(library, fontPath.c_str(), 0, &face)) {
        printf("FT_New_Face failed (there is probably a problem with your font file\n");
        return;
    }
    FT_Set_Char_Size(face, 16 * 64.0, 16 * 64.0, 96, 96);

    pScene->mRootNode = new aiNode();
    pScene->mRootNode->mName.Set("sgm mesh");
    pScene->mRootNode->mNumChildren = shouldHaveBones ? 2 : 1;
    pScene->mRootNode->mChildren = new aiNode*[shouldHaveBones ? 2 : 1];
    
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

    vector < aiBone* > boneList;
    vector < aiMatrix4x4 > matrixList;
    
    double offset = 0, prevOffset = 0;
    int prevVertCount = 0;
    for(int i = 0; i < text2Convert.length(); i++){
       	AddCharacterSideFace(face, text2Convert[i], offset);
       	AddBevel(face, text2Convert[i], offset);
        offset = AddCharacter(face, text2Convert[i], offset);
        
        if(offset == -10000.0)
            return;
        
        if(shouldHaveBones && text2Convert[i] != L' ') {
            double diff = prevOffset - offset;
            aiMatrix4x4 mat;
            mat.a4 = -offset - diff/2.0;
            mat.b4 = 16.0 / 2.0;
            mat.c4 = extrude / 2.0;
            matrixList.push_back(mat);

            aiBone* bone = new aiBone();
            aiString s;
            s.Set("Char:" + to_string(text2Convert[i]) + to_string(i));
            bone->mName = s;
            
            bone->mNumWeights = vertices.size() - prevVertCount;
            bone->mWeights = new aiVertexWeight[vertices.size() - prevVertCount];
            for (int j = 0; j < vertices.size() - prevVertCount; j++) {
                bone->mWeights[j].mVertexId = prevVertCount + j;
                bone->mWeights[j].mWeight = 1.0;
            }
            boneList.push_back(bone);
        }

        aiVector3D center;
        for (int j = prevVertCount; j < vertices.size(); j++)
            center += vertices[j];
        
        center /= (vertices.size() - prevVertCount);
        for (int j = prevVertCount; j < vertices.size(); j++) {
            aiVector3D textCoord;
            
            const aiVector3D diff = (vertices[j]-center).Normalize();
            textCoord = aiVector3D((atan2(diff.x, diff.z) + AI_MATH_PI_F) / AI_MATH_TWO_PI_F, (asin(diff.y) + AI_MATH_HALF_PI_F) / AI_MATH_PI_F, 0.0);
            
            textCoords.push_back(textCoord);
        }

        prevVertCount = vertices.size();
        prevOffset = offset;
    }

    FT_Done_Face(face);
    FT_Done_FreeType(library);
    
    pScene->mMeshes[0]->mNumVertices = vertices.size();
    pScene->mMeshes[0]->mVertices = new aiVector3D[vertices.size()];
    pScene->mMeshes[0]->mTextureCoords[0] = new aiVector3D[vertices.size()];
    
    for (int i = 0; i < vertices.size(); i++) {
        pScene->mMeshes[0]->mVertices[i] = vertices[i];
        pScene->mMeshes[0]->mVertices[i].x += offset / 2.0;
        pScene->mMeshes[0]->mVertices[i].y -= 16.0 / 2.0;
        pScene->mMeshes[0]->mVertices[i].z -= extrude / 2.0;
        
        pScene->mMeshes[0]->mTextureCoords[0][i] = textCoords[i];
    }

    pScene->mMeshes[0]->mNumFaces = vertices.size() / 3;
    pScene->mMeshes[0]->mFaces = new aiFace[vertices.size() / 3];
    int count = 0;
    for (int i = 0; i < vertices.size() / 3; i++) {
        pScene->mMeshes[0]->mFaces[i].mNumIndices = 3;
        pScene->mMeshes[0]->mFaces[i].mIndices = new unsigned int[3];
        for (int j = 0; j < 3; j++)
            pScene->mMeshes[0]->mFaces[i].mIndices[j] = count++;
    }

    pScene->mMeshes[0]->mMaterialIndex = 0;
    pScene->mNumMaterials = 1;
    pScene->mMaterials = new aiMaterial*[1];
    pScene->mMaterials[0] = new aiMaterial();
    
    int mapping = aiTextureMapping_SPHERE;
    pScene->mMaterials[0]->AddProperty<int>(&mapping, 1, AI_MATKEY_MAPPING(aiTextureType_DIFFUSE, 0));
    
    if(shouldHaveBones) {
        aiNode* rootBone = pScene->mRootNode->mChildren[1] = new aiNode();
        rootBone->mName.Set("Armature");
        rootBone->mParent = pScene->mRootNode;
        rootBone->mTransformation.a4 = offset / 2.0;
        rootBone->mTransformation.b4 = -16.0 / 2.0;
        rootBone->mTransformation.c4 = -extrude / 2.0;
        
        rootBone->mNumChildren = boneList.size();
        rootBone->mChildren = new aiNode*[boneList.size()];
        
        pScene->mMeshes[0]->mNumBones = boneList.size();
        pScene->mMeshes[0]->mBones = new aiBone*[boneList.size()];
        
        for (int i = 0; i < boneList.size(); i++) {
            pScene->mMeshes[0]->mBones[i] = boneList[i];

            rootBone->mChildren[i] = new aiNode();
            rootBone->mChildren[i]->mName = boneList[i]->mName;
            rootBone->mChildren[i]->mTransformation = matrixList[i];
            rootBone->mChildren[i]->mParent = rootBone;
        }
    }
    
    vertices.clear();
    textCoords.clear();
}

#endif // !! ASSIMP_BUILD_NO_SGM_IMPORTER
