// Copyright (C) 2013 Harishankar Narayanan
// SGR Rigged Mesh loader

#include "CSGRMeshFileLoader.h"
#define UV_MAPPED 1
#define VERTEX_COLORED 2

AnimatedMesh* CSGRMeshFileLoader::LoadMesh(string filepath)
{

    ifstream f(filepath, ios::in | ios::binary);
    unsigned short versionIdentifier;
    unsigned int vertCount = 0;
    f.read((char*)&versionIdentifier, sizeof(unsigned short));
    
    if (versionIdentifier == 0 || versionIdentifier == 1) // for large Mesh identification
        f.read((char*)&vertCount, sizeof(unsigned int));
    else
        vertCount = versionIdentifier;

    SkinMesh* animMesh = new SkinMesh();
    if(versionIdentifier == 1)
        animMesh->versionId = versionIdentifier;

    for (int i = 0; i < vertCount; i++) {
        SSGRVectHeader vert;
        f.read((char*)&vert, sizeof(SSGRVectHeader));

        vertexDataHeavy vts;
        vts.vertPosition.x = vert.vx;
        vts.vertPosition.y = vert.vy;
        vts.vertPosition.z = vert.vz;
        vts.vertNormal.x = vert.nx;
        vts.vertNormal.y = vert.ny;
        vts.vertNormal.z = vert.nz;

        vts.texCoord1.x = vert.s;
        vts.texCoord1.y = 1 - vert.t;

        vts.optionalData1 = vts.optionalData2 = vts.optionalData3 = vts.optionalData4 = Vector4(0.0);
        animMesh->addHeavyVertex(&vts);
    }
    unsigned int indicesCount = 0;
    if (versionIdentifier > 1) {
        unsigned short indCount;
        f.read((char*)&indCount, sizeof(unsigned short));
        indicesCount = (unsigned int)indCount;
    }
    else {
        unsigned int indCount;
        f.read((char*)&indCount, sizeof(unsigned int));
        indicesCount = indCount;
    }
    
    for (int i = 0; i < indicesCount; i++) {
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
        animMesh->addToIndicesArray(vertexIndex);
    }
    unsigned short boneCount;
    f.read((char*)&boneCount, sizeof(unsigned short));
    for (int i = 0; i < boneCount; i++) {
        short boneParentInd;
        f.read((char*)&boneParentInd, sizeof(short));

        float* boneMatrix = new float[16];
        f.read((char*)boneMatrix, sizeof(float) * 16);

        Joint* ibone;
        if (boneParentInd >= 0)
            ibone = animMesh->addJoint((*animMesh->joints)[boneParentInd]);
        else
            ibone = animMesh->addJoint(NULL);

        ibone->LocalAnimatedMatrix = Mat4(boneMatrix);
        
        delete[] boneMatrix;
        
        unsigned short boneWeightCount;
        f.read((char*)&boneWeightCount, sizeof(unsigned short));

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
            shared_ptr<PaintedVertex> PaintedVertexInfo = make_shared<PaintedVertex>();
            PaintedVertexInfo->vertexId = vertexIndex;
            PaintedVertexInfo->weight = ((float)vertWeight) / 255.0f;
            PaintedVertexInfo->meshBufferIndex = -1;
            ibone->PaintedVertices->push_back(PaintedVertexInfo);
        }
        
        //TODO Read envelope radius
        if(versionIdentifier == 1) {
            float sphereRadius;
            f.read((char*)&sphereRadius, sizeof(float));
            float envRadius;
            f.read((char*)&envRadius, sizeof(float));
            
            ibone->envelopeRadius = envRadius;
            ibone->sphereRadius = sphereRadius;
        }
        
    }
    animMesh->finalize();
    return animMesh;
}

Mesh* CSGRMeshFileLoader::createSGMMesh(string filepath)
{

    ifstream f(filepath, ios::in | ios::binary);
    Mesh* sgmMesh = new Mesh();

    if (!f)
        return 0;
    unsigned char versionIdentifier, hasUV;
    f.read((char*)&versionIdentifier, sizeof(unsigned char));
    if (versionIdentifier == 0 || versionIdentifier == 3) { // newer version
        f.read((char*)&hasUV, sizeof(unsigned char));
    }
    else {
        hasUV = versionIdentifier;
    }
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
        }
        else if (hasUV == VERTEX_COLORED) {
            for (int i = 0; i < counts.colCount; i++)
                f.read((char*)&col[i], sizeof(SSGMColHeader));
        }
        for (int i = 0; i < counts.indCount; i++) {
            SSGMIndexHeaderHighPoly ind;
            f.read((char*)&ind, sizeof(SSGMIndexHeaderHighPoly));
            vertexData vts;
            vts.vertPosition.x = verts[ind.vtInd].vx;
            vts.vertPosition.y = verts[ind.vtInd].vy;
            vts.vertPosition.z = verts[ind.vtInd].vz;
            vts.vertNormal.x = verts[ind.vtInd].nx;
            vts.vertNormal.y = verts[ind.vtInd].ny;
            vts.vertNormal.z = verts[ind.vtInd].nz;
            if (hasUV == UV_MAPPED) {
                vts.texCoord1.x = texs[ind.colInd].s;
                vts.texCoord1.y = 1 - texs[ind.colInd].t;
                vts.optionalData1 = Vector4(255, 255, 255, 0);
            }
            else if (hasUV == VERTEX_COLORED) {
                vts.optionalData1 = Vector4(col[ind.colInd].r / 255.0, col[ind.colInd].g / 255.0, col[ind.colInd].b / 255.0, 0);
            }
            sgmMesh->addVertex(&vts);
            sgmMesh->addToIndicesArray(i);
        }
        delete[] verts;
        delete[] texs;
        delete[] col;
    }
    else {
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
        }
        else if (hasUV == VERTEX_COLORED) {
            for (int i = 0; i < counts.colCount; i++)
                f.read((char*)&col[i], sizeof(SSGMColHeader));
        }
        for (int i = 0; i < counts.indCount; i++) {
            SSGMIndexHeaderLowPoly ind;
            f.read((char*)&ind, sizeof(SSGMIndexHeaderLowPoly));
            vertexData vts;
            vts.vertPosition.x = verts[ind.vtInd].vx;
            vts.vertPosition.y = verts[ind.vtInd].vy;
            vts.vertPosition.z = verts[ind.vtInd].vz;
            vts.vertNormal.x = verts[ind.vtInd].nx;
            vts.vertNormal.y = verts[ind.vtInd].ny;
            vts.vertNormal.z = verts[ind.vtInd].nz;
            if (hasUV == UV_MAPPED) {
                vts.texCoord1.x = texs[ind.colInd].s;
                vts.texCoord1.y = 1 - texs[ind.colInd].t;
                vts.optionalData1 = Vector4(255, 255, 255, 0);
            }
            else if (hasUV == VERTEX_COLORED) {
                vts.optionalData1 = Vector4(col[ind.colInd].r / 255.0, col[ind.colInd].g / 255.0, col[ind.colInd].b / 255.0, 0);
            }
            sgmMesh->addVertex(&vts);
            sgmMesh->addToIndicesArray(i);
        }
        delete[] verts;
        delete[] texs;
        delete[] col;
    }
    
    return sgmMesh;
}
