#include "Font2OBJ.h"
#include "SkinMesh.h"
#include <math.h>
#include <algorithm>

using namespace std;

static Vector4 VertexColor;
SkinMesh *m;
static int boneIndex;

FT_Library library;

void addVertexDataFromTri(Tri t,Mesh *mesh);

typedef GLvoid (*GLUTesselatorFunction) ();

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
    static int count = 0;
    static Tri t;
    const GLdouble *d;
    d = (GLdouble *) vertex;
    
    if(count == 0) {
        t.a.x = d[0];
        t.a.y = d[1];
        t.a.z = d[2];
        count++;
    } else if(count == 1) {
        t.b.x = d[0];
        t.b.y = d[1];
        t.b.z = d[2];
        count++;
    } else if(count == 2) {
        t.c.x = d[0];
        t.c.y = d[1];
        t.c.z = d[2];
        addVertexDataFromTri(t, m);
        count = 0;
    }
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

bool sortVertices (vertexDataHeavy *v1,vertexDataHeavy *v2) {
    return (v1->vertPosition.x < v2->vertPosition.x);
}

void addVertexDataFromTri(Tri t, Mesh *mesh) {
    vector<vertexDataHeavy*> vertices;
    vertexDataHeavy vts;
    vts.vertPosition.x = t.a.x;
    vts.vertPosition.y = t.a.y;
    vts.vertPosition.z = t.a.z;
    vts.optionalData1 = Vector4(boneIndex, 0, 0, 0);
    vts.optionalData2 = Vector4(1.0f, 0, 0, 0);
    vts.optionalData3 = Vector4(0, 0, 0, 0);
    vts.optionalData4 = VertexColor;
    vertices.push_back(&vts);
    
    vertexDataHeavy vts1;
    vts1.vertPosition.x = t.b.x;
    vts1.vertPosition.y = t.b.y;
    vts1.vertPosition.z = t.b.z;
    vts1.optionalData1 = Vector4(boneIndex, 0, 0, 0);
    vts1.optionalData2 = Vector4(1.0f, 0, 0, 0);
    vts1.optionalData3 = Vector4(0, 0, 0, 0);
    vts1.optionalData4 = VertexColor;
    vertices.push_back(&vts1);
    
    vertexDataHeavy vts2;
    vts2.vertPosition.x = t.c.x;
    vts2.vertPosition.y = t.c.y;
    vts2.vertPosition.z = t.c.z;
    vts2.optionalData1 = Vector4(boneIndex, 0, 0, 0);
    vts2.optionalData2 = Vector4(1.0f, 0, 0, 0);
    vts2.optionalData3 = Vector4(0, 0, 0, 0);
    vts2.optionalData4 = VertexColor;
    vertices.push_back(&vts2);

    mesh->addHeavyVertex(vertices[0]);
    mesh->addToIndicesArray(mesh->getVerticesCount()-1);
    
//    shared_ptr<PaintedVertex> pVertex1 = make_shared<PaintedVertex>();
//    pVertex1->vertexId = mesh->getVerticesCount() - 1;
//    pVertex1->weight = 0.0;
//    (*((SkinMesh*)m)->joints)[boneIndex-1]->PaintedVertices->push_back(pVertex1);

    
    mesh->addHeavyVertex(vertices[1]);
    mesh->addToIndicesArray(mesh->getVerticesCount()-1);
    
//    shared_ptr<PaintedVertex> pVertex2 = make_shared<PaintedVertex>();
//    pVertex2->vertexId = mesh->getVerticesCount() - 1;
//    pVertex2->weight = 0.0;
//    (*((SkinMesh*)m)->joints)[boneIndex-1]->PaintedVertices->push_back(pVertex2);

    
    mesh->addHeavyVertex(vertices[2]);
    mesh->addToIndicesArray(mesh->getVerticesCount()-1);
    
//    shared_ptr<PaintedVertex> pVertex3 = make_shared<PaintedVertex>();
//    pVertex3->vertexId = mesh->getVerticesCount() - 1;
//    pVertex3->weight = 0.0;
//    (*((SkinMesh*)m)->joints)[boneIndex-1]->PaintedVertices->push_back(pVertex3);

    
    vertices.clear();
}

float Font2OBJ::AddCharacterSideFace(FT_Face face, wchar_t ch, unsigned short bezierSteps, float offset, float extrude, SkinMesh* mesh, float bevelRadius, int bevelSegments) {
    static FT_UInt prevCharIndex = 0, curCharIndex = 0;
    static FT_Pos  prev_rsb_delta = 0;
    
    curCharIndex = FT_Get_Char_Index( face, ch );
    if(FT_Load_Glyph( face, curCharIndex, FT_LOAD_DEFAULT )){
        Logger::log(INFO,"Font2Obj","FT_Load_Glyph failed");
        return -10000.0;
    }
    
    FT_Glyph glyph;
    if(FT_Get_Glyph( face->glyph, &glyph )){
        Logger::log(INFO,"Font2Obj","FT_Get_Glyph failed");
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
        offset -= 1.0f;
    else if ( prev_rsb_delta - face->glyph->lsb_delta < -32 )
        offset += 1.0f;
    
    prev_rsb_delta = face->glyph->rsb_delta;

    Vectoriser *vectoriser = new Vectoriser(face->glyph->outline, bezierSteps);
    for(size_t c = 0; c < vectoriser->ContourCount(); c++) {
        const Contour* contour = vectoriser->GetContour(c);
        
        for(size_t p = 0; p < contour->PointCount(); p++) {
            const double* d1 = contour->GetPoint(p);
            const double* d2 = contour->GetPoint(p + 1);
            
            if(p != contour->PointCount() - 1)
                d2 = contour->GetPoint(p + 1);
            else
                d2 = contour->GetPoint(0);
            
            Tri t1;
            t1.a.x = (d1[0]/64.0f) + offset;
            t1.a.y = d1[1]/64.0f;
            t1.a.z = 0.0f;
            t1.c.x = (d2[0]/64.0f) + offset;
            t1.c.y = d2[1]/64.0f;
            t1.c.z = 0.0f;
            t1.b.x = (d1[0]/64.0f) + offset;
            t1.b.y = d1[1]/64.0f;
            t1.b.z = extrude;
            addVertexDataFromTri(t1,mesh);
            
            Tri t2;
            t2.a.x = (d1[0]/64.0f) + offset;
            t2.a.y = d1[1]/64.0f;
            t2.a.z = extrude;
            t2.b.x = (d2[0]/64.0f) + offset;
            t2.b.y = d2[1]/64.0f;
            t2.b.z = extrude;
            t2.c.x = (d2[0]/64.0f) + offset;
            t2.c.y = d2[1]/64.0f;
            t2.c.z = 0.0f;
            addVertexDataFromTri(t2,mesh);
        }
    }
    
    delete vectoriser;
    vectoriser = NULL;
    
    FT_Done_Glyph(glyph);
    
    prevCharIndex = curCharIndex;
    float chSize = face->glyph->advance.x >> 6;
    return offset + chSize;
}

float Font2OBJ::AddCharacter(FT_Face face, wchar_t ch, unsigned short bezierSteps, float offset, float extrude, SkinMesh* mesh, float bevelRadius, int bevelSegments) {
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

    FT_Outline_Embolden(&face->glyph->outline, - bevelRadius * 64);
    FT_Outline_Translate(&face->glyph->outline, bevelRadius * 32, bevelRadius * 32);

    short nCountour = 0;
    nCountour = face->glyph->outline.n_contours;

	if(FT_HAS_KERNING( face ) && prevCharIndex) {
		FT_Vector  kerning;
		FT_Get_Kerning( face, prevCharIndex, curCharIndex, FT_KERNING_DEFAULT, &kerning );
		offset += kerning.x >> 6;
	}

	if ( prev_rsb_delta - face->glyph->lsb_delta >= 32 )
		offset -= 1.0f;
	else if ( prev_rsb_delta - face->glyph->lsb_delta < -32 )
		offset += 1.0f;

	prev_rsb_delta = face->glyph->rsb_delta;                       

    Vectoriser *vectoriser = new Vectoriser(face->glyph->outline, bezierSteps);

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
//            gluTessProperty(tobj, GLU_TESS_TOLERANCE, 0);
            gluTessNormal(tobj, 0.0f, 0.0f, 0.0f);
            
            gluTessBeginPolygon(tobj, NULL);
            for(size_t c = 0; c < vectoriser->ContourCount(); ++c) {
                const Contour* contour = vectoriser->GetContour(c);
                gluTessBeginContour(tobj);
                for(size_t p = 0; p < contour->PointCount(); ++p) {
                    const double* d1 = contour->GetPoint(p);
                    double *d = new double[3];
                    d[0] = d1[0] / 64.0f + offset;
                    d[1] = d1[1] / 64.0f;
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
                    d[0] = d1[0] / 64.0f + offset;
                    d[1] = d1[1] / 64.0f;
                    d[2] = extrude - bevelRadius;
                    gluTessVertex(tobj, (GLdouble *)d, (GLvoid *)d);
                }
                
                gluTessEndContour(tobj);
            }
            gluTessEndPolygon(tobj);
            gluDeleteTess(tobj);
		}
	}
    //delete d;

    delete vectoriser;
    vectoriser = NULL;

    FT_Done_Glyph(glyph);
    
    prevCharIndex = curCharIndex;
    float chSize = face->glyph->advance.x >> 6;
    return offset + chSize;
}

float Font2OBJ::AddBevel(FT_Face face, wchar_t ch, unsigned short bezierSteps, float offset, float extrude, SkinMesh* mesh, float bevelRadius, int bevelSegments) {
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
        offset -= 1.0f;
    else if ( prev_rsb_delta - face->glyph->lsb_delta < -32 )
        offset += 1.0f;
    
    prev_rsb_delta = face->glyph->rsb_delta;
    
    for (int i = 0; i < bevelSegments; i++) {
        double bevelX = bevelRadius - (bevelRadius * sinf(i * 90.0f * (PI / 180.0f)/ bevelSegments));
        FT_Outline outline;
        FT_Outline_New(library, face->glyph->outline.n_points, face->glyph->outline.n_contours, &outline);
        FT_Outline_Copy(&face->glyph->outline, &outline);
        FT_Outline_Embolden(&outline, - bevelX * 64.0f);
        FT_Outline_Translate(&outline, bevelX * 32.0f, bevelX * 32.0f);
        Vectoriser *vectoriser = new Vectoriser(outline, bezierSteps);
        
        double nextBevelX = bevelRadius - (bevelRadius * sinf((i+1) * 90.0f * (PI / 180.0f)/ bevelSegments));
        FT_Outline nextOutline;
        FT_Outline_New(library, face->glyph->outline.n_points, face->glyph->outline.n_contours, &nextOutline);
        FT_Outline_Copy(&face->glyph->outline, &nextOutline);
        FT_Outline_Embolden(&nextOutline, - nextBevelX * 64.0f);
        FT_Outline_Translate(&nextOutline, nextBevelX * 32.0f, nextBevelX * 32.0f);
        Vectoriser *nextVectoriser = new Vectoriser(nextOutline, bezierSteps);

        for(size_t c = 0; c < vectoriser->ContourCount(); c++) {
            const Contour* contour = vectoriser->GetContour(c);
            const Contour* nextContour = nextVectoriser->GetContour(c);

            for(size_t p0 = 0, p1 = 0; p0 < contour->PointCount() && p1 < nextContour->PointCount(); p0++, p1++) {
                const double* d1 = contour->GetPoint(p0);
                const double* d2 = contour->GetPoint(p0 + 1);
                
                if(p0 == contour->PointCount() - 1)
                    d2 = contour->GetPoint(0);

                const double* d3 = nextContour->GetPoint(p1);
                const double* d4 = nextContour->GetPoint(p1 + 1);
                
                if(p1 == nextContour->PointCount() - 1)
                    d4 = nextContour->GetPoint(0);
                
                float bevelY = bevelRadius * cosf(i * 90.0f * (PI / 180.0f)/ bevelSegments);
                float nextBevelY = bevelRadius * cosf((i+1) * 90.0f * (PI / 180.0f)/ bevelSegments);

                Tri t1;
                t1.a.x = d1[0]/64.0f + offset;
                t1.a.y = d1[1]/64.0f;
                t1.a.z = d1[2]/64.0f + bevelY;
                t1.b.x = d3[0]/64.0f + offset;
                t1.b.y = d3[1]/64.0f;
                t1.b.z = d3[2]/64.0f + nextBevelY;
                t1.c.x = d2[0]/64.0f + offset;
                t1.c.y = d2[1]/64.0f;
                t1.c.z = d2[2]/64.0f + bevelY;
                addVertexDataFromTri(t1, mesh);
                
                Tri t2;
                t2.a.x = d3[0]/64.0f + offset;
                t2.a.y = d3[1]/64.0f;
                t2.a.z = d3[2]/64.0f + nextBevelY;
                t2.b.x = d4[0]/64.0f + offset;
                t2.b.y = d4[1]/64.0f;
                t2.b.z = d4[2]/64.0f + nextBevelY;
                t2.c.x = d2[0]/64.0f + offset;
                t2.c.y = d2[1]/64.0f;
                t2.c.z = d2[2]/64.0f + bevelY;
                addVertexDataFromTri(t2, mesh);

                float eBevelY = extrude - (bevelRadius * cosf(i * 90.0f * (PI / 180.0f)/ bevelSegments));
                float eNextBevelY = extrude - (bevelRadius * cosf((i+1) * 90.0f * (PI / 180.0f)/ bevelSegments));

                Tri t3;
                t3.a.x = d1[0]/64.0f + offset;
                t3.a.y = d1[1]/64.0f;
                t3.a.z = d1[2]/64.0f + eBevelY;
                t3.c.x = d3[0]/64.0f + offset;
                t3.c.y = d3[1]/64.0f;
                t3.c.z = d3[2]/64.0f + eNextBevelY;
                t3.b.x = d2[0]/64.0f + offset;
                t3.b.y = d2[1]/64.0f;
                t3.b.z = d2[2]/64.0f + eBevelY;
                addVertexDataFromTri(t3, mesh);
                
                Tri t4;
                t4.a.x = d3[0]/64.0f + offset;
                t4.a.y = d3[1]/64.0f;
                t4.a.z = d3[2]/64.0f + eNextBevelY;
                t4.c.x = d4[0]/64.0f + offset;
                t4.c.y = d4[1]/64.0f;
                t4.c.z = d4[2]/64.0f + eNextBevelY;
                t4.b.x = d2[0]/64.0f + offset;
                t4.b.y = d2[1]/64.0f;
                t4.b.z = d2[2]/64.0f + eBevelY;
                addVertexDataFromTri(t4, mesh);
            }
        }

        delete vectoriser;
        vectoriser = NULL;
        delete nextVectoriser;
        nextVectoriser = NULL;
    }
    
    FT_Done_Glyph(glyph);
    
    prevCharIndex = curCharIndex;
    float chSize = face->glyph->advance.x >> 6;
    return offset + chSize;
}

AnimatedMesh* Font2OBJ::getTextMesh(std::wstring text, u16 beizerSteps, float extrude, int height, char* filePath, Vector4 color, DEVICE_TYPE device, float bevelRadius, int bevelSegments){

    if (FT_Init_FreeType( &library )){
        printf("FT_Init_FreeType failed\n");
        return NULL;
    }
    FT_Face face;
    if (FT_New_Face( library, filePath, 0, &face )){
        printf("FT_New_Face failed (there is probably a problem with your font file\n");
        return NULL;
    }
    FT_Set_Char_Size( face, height << 6, height << 6, 96, 96);
    
    SkinMesh *mesh = new SkinMesh();
    m = mesh;
    VertexColor = color;

    Joint* rootBone = mesh->addJoint(NULL);
    mesh->finalize();
    float offset = 0, prevOffset = 0;
    for(int i = 0; i < text.length(); i++){
        Joint* joint = NULL;
        if(text[i] != ' ') {
            joint = mesh->addJoint(rootBone);
            boneIndex = joint->Index + 1;
        }
        SkinMesh *tempMesh = new SkinMesh();
        
       	AddCharacterSideFace(face, text[i], beizerSteps, offset, -extrude, tempMesh, bevelRadius, bevelSegments);
       	AddBevel(face, text[i], beizerSteps, offset, -extrude, tempMesh, bevelRadius, bevelSegments);
        tempMesh->removeDoublesInHeavyMesh(true, false, false);
        unsigned int oldVewrtCount = mesh->getVerticesCount();
        
        for (int j = 0; j < tempMesh->getVerticesCount(); j++) {
            vertexDataHeavy *vtx = tempMesh->getHeavyVertexByIndex(j);
            mesh->addHeavyVertex(vtx);
            
//            shared_ptr<PaintedVertex> pVertex1 = make_shared<PaintedVertex>();
//            pVertex1->vertexId = mesh->getVerticesCount() - 1;
//            pVertex1->weight = 0.0;
//            joint->PaintedVertices->push_back(pVertex1);
        }

        for (int j = 0; j < tempMesh->getTotalIndicesCount(); j++) {
            unsigned int in = tempMesh->getHighPolyIndicesArray()[j];
            mesh->addToIndicesArray(in + oldVewrtCount);
        }
        
        delete tempMesh;
        
       	offset = AddCharacter(face, text[i], beizerSteps, offset, -extrude, mesh, bevelRadius, bevelSegments);

        if(text[i] != ' ') {
            float diff = prevOffset - offset;
            joint->LocalAnimatedMatrix[12] = -offset - diff/2.0f;
            joint->LocalAnimatedMatrix[13] = height / 2;
            joint->LocalAnimatedMatrix[14] = -extrude / 2;
        }
        
        if(offset == -10000.0)
            return NULL;
        
        prevOffset = offset;
    }
    mesh->finalize();
    rootBone->LocalAnimatedMatrix[12] = offset / 2;

    if(mesh->getVerticesCount() <= 0)
        return NULL;

    FT_Done_Face(face);
    FT_Done_FreeType(library);
    
    mesh->pivotToOrigin();
    mesh->recalculateNormalsT(true);
    return mesh;
}

