//
//  3dTextMesh.cpp
//  Iyan3D
//
//  Created by Sankar on 23/01/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#include "SkinMesh.h"
#include <math.h>
#include <algorithm>
#include "TextMesh3d.h"

using namespace std;
SkinMesh *mesh;
int boneIndex;
Vector4 vertexColor;
typedef GLvoid (*GLUTesselatorFunction) ();

void AddMeshVertex(SkinMesh* mesh, Vector4 v)
{
    vertexDataHeavy vts;
    vts.vertPosition.x = v.x;
    vts.vertPosition.y = v.y;
    vts.vertPosition.z = v.z;
    vts.optionalData1 = Vector4(boneIndex, 0, 0, 0);
    vts.optionalData2 = Vector4(1.0, 0, 0, 0);
    vts.optionalData3 = Vector4(0, 0, 0, 0);
    vts.optionalData4 = vertexColor;
    mesh->addHeavyVertex(&vts);
    mesh->addToIndicesArray(mesh->getVerticesCount()-1);
}

void beginCallback(GLenum which)
{
}

void endCallback(void)
{
}

void flagCallback( GLboolean )
{
    
}

void errorCallback(GLenum errorCode)
{
    const GLubyte *estring;
    
    estring = gluErrorString(errorCode);
    fprintf (stderr, "Tessellation Error: %s\n", estring);
}

void vertexCallback(GLvoid *vertex)
{
    const GLdouble *d;
    d = (GLdouble *) vertex;
    AddMeshVertex(mesh, Vector4(d[0], d[1], d[2], 0.0));
}

void combineCallback(GLdouble coords[3], GLdouble *vertex_data[4], GLfloat weight[4], GLdouble **dataOut)
{
    GLdouble *vertex;
    
    vertex = (GLdouble *) malloc(6 * sizeof(GLdouble));
    vertex[0] = coords[0];
    vertex[1] = coords[1];
    vertex[2] = coords[2];
    *dataOut = vertex;
}

Vectoriser* TextMesh3d::generateVertices(FT_Face face, wchar_t ch, unsigned short bezierSteps, double strength)
{
    FT_UInt charIndex = FT_Get_Char_Index(face, ch);
    if(FT_Load_Glyph(face, charIndex, FT_LOAD_DEFAULT)) {
        Logger::log(INFO, "TextMesh3d", "FT_Load_Glyph failed");
        return NULL;
    }
    
    FT_Glyph glyph;
    if(FT_Get_Glyph(face->glyph, &glyph)) {
        Logger::log(INFO, "TextMesh3d", "FT_Load_Glyph failed");
        return NULL;
    }
    
    if(glyph->format != FT_GLYPH_FORMAT_OUTLINE) {
        Logger::log(INFO, "TextMesh3d", "Invalid Glyph Format");
        return NULL;
    }

    if(strength != 0.0) {
        FT_Outline_Embolden(&face->glyph->outline, strength * 64.0);
        FT_Outline_Translate(&face->glyph->outline, -strength * 64.0 * 0.5, -strength * 64.0 * 0.5);
    }
    FT_Orientation f = FT_Outline_Get_Orientation(&face->glyph->outline);
//    printf("Orientation: %d\n", f);

    Vectoriser *v = new Vectoriser(face->glyph->outline, bezierSteps, f==true);
    FT_Done_Glyph(glyph);
    
    return v;
}

void TextMesh3d::AddCharacterSideFace(FT_Face face, wchar_t ch, unsigned short bezierSteps, double offset, float extrude, SkinMesh* mesh, double bevelRadius)
{
    static FT_UInt prevCharIndex = 0, curCharIndex = 0;
    static FT_Pos  prev_rsb_delta = 0;
    
    curCharIndex = FT_Get_Char_Index( face, ch );
    if(FT_Load_Glyph( face, curCharIndex, FT_LOAD_DEFAULT )){
        Logger::log(INFO,"Font2Obj","FT_Load_Glyph failed");
        return -10000.0;
    }
    
    FT_Glyph glyph;
    if(FT_Get_Glyph( face->glyph, &glyph )){
        Logger::log(INFO,"Font2Obj","FT_Load_Glyph failed");
        return -10000.0;
    }
    
    if(glyph->format != FT_GLYPH_FORMAT_OUTLINE) {
        Logger::log(INFO,"Font2Obj","Invalid Glyph Format");
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
    
    Vectoriser* vectoriser = generateVertices(face, ch, bezierSteps, bevelRadius * 1.5);
    for(size_t c = 0; c < vectoriser->ContourCount(); c++) {
        const Contour* contour = vectoriser->GetContour(c);
        
        for(size_t p = 0; p < contour->PointCount(); p++) {
            const double* d1 = contour->GetPoint(p);
            const double* d2 = contour->GetPoint(p + 1);
            
            if(p != contour->PointCount() - 1)
                d2 = contour->GetPoint(p + 1);
            else
                d2 = contour->GetPoint(0);
            
            AddMeshVertex(mesh, Vector4(d1[0]/64.0 + offset, d1[1]/64.0, 0.0, 0.0));
            AddMeshVertex(mesh, Vector4(d1[0]/64.0 + offset, d1[1]/64.0, extrude, 0.0));
            AddMeshVertex(mesh, Vector4(d2[0]/64.0 + offset, d2[1]/64.0, 0.0, 0.0));
            AddMeshVertex(mesh, Vector4(d1[0]/64.0 + offset, d1[1]/64.0, extrude, 0.0));
            AddMeshVertex(mesh, Vector4(d2[0]/64.0 + offset, d2[1]/64.0, extrude, 0.0));
            AddMeshVertex(mesh, Vector4(d2[0]/64.0 + offset, d2[1]/64.0, 0.0, 0.0));
        }
    }

    delete vectoriser;
    vectoriser = NULL;
    
    FT_Done_Glyph(glyph);
    
    prevCharIndex = curCharIndex;
}

double TextMesh3d::AddCharacter(FT_Face face, wchar_t ch, unsigned short bezierSteps, double offset, float extrude, SkinMesh* mesh, double bevelRadius)
{
    static FT_UInt prevCharIndex = 0, curCharIndex = 0;
    static FT_Pos  prev_rsb_delta = 0;
    
    curCharIndex = FT_Get_Char_Index( face, ch );
    if(FT_Load_Glyph( face, curCharIndex, FT_LOAD_DEFAULT )){
        Logger::log(INFO,"Font2Obj","FT_Load_Glyph failed");
        return -10000.0;
    }
    
    FT_Glyph glyph;
    if(FT_Get_Glyph( face->glyph, &glyph )){
        Logger::log(INFO,"Font2Obj","FT_Load_Glyph failed");
        return -10000.0;
    }
    
    if(glyph->format != FT_GLYPH_FORMAT_OUTLINE) {
        Logger::log(INFO,"Font2Obj","Invalid Glyph Format");
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
    
    Vectoriser* vectoriser = generateVertices(face, ch, bezierSteps);
    for(size_t c = 0; c < vectoriser->ContourCount(); c++) {
        const Contour* contour = vectoriser->GetContour(c);
        
        if(contour->GetDirection()) {
            GLUtesselator* tobj = gluNewTess();
            
            gluTessCallback(tobj, GLU_TESS_VERTEX, (GLUTesselatorFunction) vertexCallback);
            gluTessCallback(tobj, GLU_TESS_BEGIN, (GLUTesselatorFunction) beginCallback);
            gluTessCallback(tobj, GLU_TESS_END, (GLUTesselatorFunction) endCallback);
            gluTessCallback(tobj, GLU_TESS_ERROR, (GLUTesselatorFunction) errorCallback);
            gluTessCallback(tobj, GLU_TESS_COMBINE, (GLUTesselatorFunction) combineCallback);
            gluTessCallback(tobj, GLU_TESS_EDGE_FLAG, (GLUTesselatorFunction) flagCallback);
            
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

void TextMesh3d::AddBevel(FT_Face face, wchar_t ch, unsigned short bezierSteps, double offset, float extrude, SkinMesh* mesh, double bevelRadius, int bevelSegments, int height)
{
    static FT_UInt prevCharIndex = 0, curCharIndex = 0;
    static FT_Pos  prev_rsb_delta = 0;
    
    curCharIndex = FT_Get_Char_Index( face, ch );
    if(FT_Load_Glyph( face, curCharIndex, FT_LOAD_DEFAULT )){
        Logger::log(INFO,"Font2Obj","FT_Load_Glyph failed");
        return -10000.0;
    }
    
    FT_Glyph glyph;
    if(FT_Get_Glyph( face->glyph, &glyph )){
        Logger::log(INFO,"Font2Obj","FT_Load_Glyph failed");
        return -10000.0;
    }
    
    if(glyph->format != FT_GLYPH_FORMAT_OUTLINE) {
        Logger::log(INFO,"Font2Obj","Invalid Glyph Format");
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
    
    for (int i = 0; i < bevelSegments; i++) {
        double bevelX = bevelRadius * 1.5 * sinf(PI * 0.5 * i / (double)bevelSegments);
        double nextBevelX = bevelRadius * 1.5  * sinf(PI * 0.5 * (i+1) / (double)bevelSegments);

        Vectoriser* vectoriser1 = generateVertices(face, ch, bevelSegments, bevelX);
        Vectoriser* vectoriser2 = generateVertices(face, ch, bevelSegments, nextBevelX);

        for(size_t c = 0; c < vectoriser1->ContourCount(); c++) {
            const Contour* contour1 = vectoriser1->GetContour(c);
            const Contour* contour2 = vectoriser2->GetContour(c);
            
            for(size_t p = 0; p < contour1->PointCount(); p++) {
                const double* d1 = contour1->GetPoint(p);
                const double* d2 = contour1->GetPoint(p + 1);
                
                if(p == contour1->PointCount() - 1)
                    d2 = contour1->GetPoint(0);
                
                Vector4 v1 = Vector4(d1[0]/64.0, d1[1]/64.0, 0.0, 0.0);
                Vector4 v2 = Vector4(d2[0]/64.0, d2[1]/64.0, 0.0, 0.0);
                
                const double* d3 = contour2->GetPoint(p);
                const double* d4 = contour2->GetPoint(p + 1);
                
                if(p == contour2->PointCount() - 1)
                    d4 = contour2->GetPoint(0);

                Vector4 v3 = Vector4(d3[0]/64.0, d3[1]/64.0, 0.0, 0.0);
                Vector4 v4 = Vector4(d4[0]/64.0, d4[1]/64.0, 0.0, 0.0);

                double bevelY = bevelRadius * cosf(PI * 0.5 * i / (double)bevelSegments);
                double nextBevelY = bevelRadius * cosf(PI * 0.5 * (i+1) / (double)bevelSegments);

                AddMeshVertex(mesh, Vector4(v1.x + offset, v1.y, bevelY, 0.0));
                AddMeshVertex(mesh, Vector4(v3.x + offset, v3.y, nextBevelY, 0.0));
                AddMeshVertex(mesh, Vector4(v2.x + offset, v2.y, bevelY, 0.0));
                AddMeshVertex(mesh, Vector4(v3.x + offset, v3.y, nextBevelY, 0.0));
                AddMeshVertex(mesh, Vector4(v4.x + offset, v4.y, nextBevelY, 0.0));
                AddMeshVertex(mesh, Vector4(v2.x + offset, v2.y, bevelY, 0.0));

                double eBevelY = extrude - (bevelRadius * cosf(PI * 0.5 * i / (double)bevelSegments));
                double eNextBevelY = extrude - (bevelRadius * cosf(PI * 0.5 * (i+1) / (double)bevelSegments));
                
                AddMeshVertex(mesh, Vector4(v1.x + offset, v1.y, eBevelY, 0.0));
                AddMeshVertex(mesh, Vector4(v2.x + offset, v2.y, eBevelY, 0.0));
                AddMeshVertex(mesh, Vector4(v3.x + offset, v3.y, eNextBevelY, 0.0));
                AddMeshVertex(mesh, Vector4(v3.x + offset, v3.y, eNextBevelY, 0.0));
                AddMeshVertex(mesh, Vector4(v2.x + offset, v2.y, eBevelY, 0.0));
                AddMeshVertex(mesh, Vector4(v4.x + offset, v4.y, eNextBevelY, 0.0));
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

AnimatedMesh* TextMesh3d::get3DTextAnimatedMesh(wstring text, u16 beizerSteps, float extrude, int height, char* filepath, Vector4 color, DEVICE_TYPE device, double bevelRadius, int bevelSegments)
{
    FT_Library library;
    if (FT_Init_FreeType(&library)) {
        printf("FT_Init_FreeType failed\n");
        return NULL;
    }

    FT_Face face;
    if (FT_New_Face(library, filepath, 0, &face)) {
        printf("FT_New_Face failed (there is probably a problem with your font file\n");
        return NULL;
    }
    FT_Set_Char_Size(face, height * 64.0, height * 64.0, 96, 96);
    
    mesh = new SkinMesh();
    Joint* rootBone = mesh->addJoint(NULL);
    
    mesh->finalize();
    double offset = 0, prevOffset = 0;
    vertexColor = color;
    
    for(int i = 0; i < text.length(); i++){
        Joint* joint = NULL;
        if(text[i] != ' ') {
            joint = mesh->addJoint(rootBone);
            boneIndex = joint->Index + 1;
        }
        SkinMesh *tempMesh = new SkinMesh();
        
       	AddCharacterSideFace(face, text[i], beizerSteps, offset, -extrude, tempMesh, bevelRadius);
       	AddBevel(face, text[i], beizerSteps, offset, -extrude, tempMesh, bevelRadius, bevelSegments, height);
        tempMesh->removeDoublesInMesh();
        unsigned int oldVertCount = mesh->getVerticesCount();
        
        for (int j = 0; j < tempMesh->getVerticesCount(); j++) {
            vertexDataHeavy *vtx = tempMesh->getHeavyVertexByIndex(j);
            mesh->addHeavyVertex(vtx);
        }
        
        for (int j = 0; j < tempMesh->getTotalIndicesCount(); j++) {
            unsigned int in = tempMesh->getHighPolyIndicesArray()[j];
            mesh->addToIndicesArray(in + oldVertCount);
        }
        
        delete tempMesh;
        
       	offset = AddCharacter(face, text[i], beizerSteps, offset, -extrude, mesh, bevelRadius);
        if(text[i] != ' ') {
            double diff = prevOffset - offset;
            joint->LocalAnimatedMatrix.setElement(12, -offset - diff/2.0);
            joint->LocalAnimatedMatrix.setElement(13, height / 2);
            joint->LocalAnimatedMatrix.setElement(14, -extrude / 2);
        }
        
        if(offset == -10000.0)
            return NULL;
        
        prevOffset = offset;
    }
    mesh->finalize();
    rootBone->LocalAnimatedMatrix.setElement(12, offset/2);
    rootBone->LocalAnimatedMatrix.setElement(13, -height/2);
    rootBone->LocalAnimatedMatrix.setElement(14, extrude/2);

    FT_Done_Face(face);
    FT_Done_FreeType(library);
    
    if(mesh->getVerticesCount() <= 0)
        return NULL;

    mesh->fixOrientation();
    mesh->recalculateNormals();
    mesh->generateUV();

    return mesh;
}

Mesh* TextMesh3d::get3DTextMesh(wstring text, u16 beizerSteps, float extrude, int height, char* filepath, Vector4 color, DEVICE_TYPE device, double bevelRadius, int bevelSegments)
{
    FT_Library library;
    if (FT_Init_FreeType(&library)) {
        printf("FT_Init_FreeType failed\n");
        return NULL;
    }
    
    FT_Face face;
    if (FT_New_Face(library, filepath, 0, &face)) {
        printf("FT_New_Face failed (there is probably a problem with your font file\n");
        return NULL;
    }
    FT_Set_Char_Size(face, height * 64.0, height * 64.0, 96, 96);
    
    mesh = new SkinMesh();
    mesh->finalize();
    double offset = 0, prevOffset = 0;
    vertexColor = color;
    
    for(int i = 0; i < text.length(); i++){
        SkinMesh *tempMesh = new SkinMesh();
       	AddCharacterSideFace(face, text[i], beizerSteps, offset, -extrude, tempMesh, bevelRadius);
       	AddBevel(face, text[i], beizerSteps, offset, -extrude, tempMesh, bevelRadius, bevelSegments, height);
        tempMesh->removeDoublesInMesh();
        unsigned int oldVertCount = mesh->getVerticesCount();
        
        for (int j = 0; j < tempMesh->getVerticesCount(); j++) {
            vertexDataHeavy *vtx = tempMesh->getHeavyVertexByIndex(j);
            mesh->addHeavyVertex(vtx);
        }
        
        for (int j = 0; j < tempMesh->getTotalIndicesCount(); j++) {
            unsigned int in = tempMesh->getHighPolyIndicesArray()[j];
            mesh->addToIndicesArray(in + oldVertCount);
        }
        
        delete tempMesh;
        
       	offset = AddCharacter(face, text[i], beizerSteps, offset, -extrude, mesh, bevelRadius);

        if(offset == -10000.0)
            return NULL;
        
        prevOffset = offset;
    }
    FT_Done_Face(face);
    FT_Done_FreeType(library);
    
    if(mesh->getVerticesCount() <= 0)
        return NULL;

    mesh->fixOrientation();
    mesh->moveVertices(Vector3(offset, -height, extrude) / 2.0);
    mesh->recalculateNormals();
    
    Mesh* m = new Mesh();
    for (int i = 0; i < mesh->getVerticesCount(); i++) {
        vertexDataHeavy *vtx = mesh->getHeavyVertexByIndex(i);
        vertexData v;
        v.vertPosition = vtx->vertPosition;
        v.vertNormal = vtx->vertNormal;
        v.texCoord1 = vtx->texCoord1;
        v.optionalData1 = vtx->optionalData4;
        m->addVertex(&v);
    }
    
    MeshOptimizeHelper *optimizer = new MeshOptimizeHelper();
    unsigned int *optimizedIndices = new unsigned int[mesh->getTotalIndicesCount()];
    optimizer->OptimizeFaces(mesh->getHighPolyIndicesArray(), mesh->getTotalIndicesCount(), mesh->getVerticesCount(), optimizedIndices, 32);
    
    for (int i = 0; i < mesh->getTotalIndicesCount(); i++)
        m->addToIndicesArray(optimizedIndices[i]);

    m->generateUV();

    return m;
}
