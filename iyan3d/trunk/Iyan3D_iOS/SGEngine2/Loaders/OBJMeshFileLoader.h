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
#else
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/gl.h>
#endif


//! Meshloader capable of loading obj meshes.
class OBJMeshFileLoader {
public:
    //! Constructor
    OBJMeshFileLoader();

    //! destructor
    virtual ~OBJMeshFileLoader();

    //! returns true if the file maybe is able to be loaded by this class
    //! based on the file extension (e.g. ".obj")
    virtual bool isALoadableFileExtension(const string& filename) const;

    //! creates/loads an animated mesh from the file.
    //! \return Pointer to the created mesh. Returns 0 if loading failed.
    //! If you no longer need the mesh, you should call AnimatedMesh::drop().
    //! See IReferenceCounted::drop() for more information.
    Mesh* createMesh(string fileName, int& status, DEVICE_TYPE device = OPENGLES2);

private:
    map<size_t, int> VertMap;

    // helper method for material reading

    // returns a pointer to the first printable character available in the buffer
    const c8* goFirstWord(const c8* buf, const c8* const bufEnd, bool acrossNewlines = true);
    // returns a pointer to the first printable character after the first non-printable
    const c8* goNextWord(const c8* buf, const c8* const bufEnd, bool acrossNewlines = true);
    // returns a pointer to the next printable character after the first line break
    const c8* goNextLine(const c8* buf, const c8* const bufEnd);
    // copies the current word from the inBuf to the outBuf
    u32 copyWord(c8* outBuf, const c8* inBuf, u32 outBufLength, const c8* const pBufEnd);
    // copies the current line from the inBuf to the outBuf
    string copyLine(const c8* inBuf, const c8* const bufEnd);

    // combination of goNextWord followed by copyWord
    const c8* goAndCopyNextWord(c8* outBuf, const c8* inBuf, u32 outBufLength, const c8* const pBufEnd);

    //! Read the material from the given file
    void readMTL(const c8* fileName, const string& relPath);

    //! Find and return the material with the given name
    int strtol10(const char* in, const char** out = 0);
    //! Read RGB color
    const c8* readColor(const c8* bufPtr, Vector4& color, const c8* const pBufEnd);
    //! Read 3d vector of floats
    const c8* readVec3(const c8* bufPtr, Vector3& vec, const c8* const pBufEnd);
    float fast_atof(const char* floatAsString, const char** out = 0);

    //! Read 2d vector of floats
    const c8* readUV(const c8* bufPtr, Vector2& vec, const c8* const pBufEnd);
    //! Read boolean value represented as 'on' or 'off'
    const c8* readBool(const c8* bufPtr, bool& tf, const c8* const bufEnd);

    // reads and convert to integer the vertex indices in a line of obj file's face statement
    // -1 for the index if it doesn't exist
    // indices are changed to 0-based index instead of 1-based from the obj file
    bool retrieveVertexIndices(c8* vertexData, int* idx, const c8* bufEnd, u32 vbsize, u32 vtsize, u32 vnsize);
    void recalculateNormalsT(Mesh* mesh);
    Vector3 getAngleWeight(Vector3& v1,
        Vector3& v2,
        Vector3& v3);

    void cleanUp();
    u32 strtoul10(const char* in, const char** out = 0);
    int isdigit(int c) { return c >= '0' && c <= '9'; }
    int isspace(int c) { return c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v'; }
    int isupper(int c) { return c >= 'A' && c <= 'Z'; }
};

#endif
