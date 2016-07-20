//
//  OBJMeshFileLoader.h
//  SGEngine2
//
//  Created by Vivek shivam on 30/03/2015.
//  Copyright (c) 2014 Smackall Games Pvt Ltd. All rights reserved.
//

#ifndef __C_OBJ_MESH_FILE_LOADER_H_INCLUDED__
#define __C_OBJ_MESH_FILE_LOADER_H_INCLUDED__

#include <iostream>
#include "../Core/common/common.h"
#include "../Core/Meshes/SkinMesh.h"
#include <fstream>
#include <map>

#ifdef ANDROID
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#elif IOS
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/gl.h>
#elif UBUNTU
#endif

class OBJMeshFileLoader {
public:

    OBJMeshFileLoader();
    virtual ~OBJMeshFileLoader();

    virtual bool isALoadableFileExtension(const string& filename) const;

    Mesh* createMesh(string fileName, int& status, DEVICE_TYPE device = OPENGLES2);

private:
    map<size_t, int> VertMap;

    const c8* goFirstWord(const c8* buf, const c8* const bufEnd, bool acrossNewlines = true);
    const c8* goNextWord(const c8* buf, const c8* const bufEnd, bool acrossNewlines = true);
    const c8* goNextLine(const c8* buf, const c8* const bufEnd);
    u32 copyWord(c8* outBuf, const c8* inBuf, u32 outBufLength, const c8* const pBufEnd);
    string copyLine(const c8* inBuf, const c8* const bufEnd);
    const c8* goAndCopyNextWord(c8* outBuf, const c8* inBuf, u32 outBufLength, const c8* const pBufEnd);
    void readMTL(const c8* fileName, const string& relPath);
    int strtol10(const char* in, const char** out = 0);
    const c8* readColor(const c8* bufPtr, Vector4& color, const c8* const pBufEnd);
    const c8* readVec3(const c8* bufPtr, Vector3& vec, const c8* const pBufEnd);
    float fast_atof(const char* floatAsString, const char** out = 0);
    const c8* readUV(const c8* bufPtr, Vector2& vec, const c8* const pBufEnd);
    const c8* readBool(const c8* bufPtr, bool& tf, const c8* const bufEnd);
    bool retrieveVertexIndices(c8* vertexData, int* idx, const c8* bufEnd, u32 vbsize, u32 vtsize, u32 vnsize);

    void cleanUp();
    u32 strtoul10(const char* in, const char** out = 0);
    int isdigit(int c) { return c >= '0' && c <= '9'; }
    int isspace(int c) { return c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v'; }
    int isupper(int c) { return c >= 'A' && c <= 'Z'; }
};

#endif
