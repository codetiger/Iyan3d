// Copyright (C) 2012 Harishankar Narayanan
// SGR Rigged Mesh loader

#ifndef __C_SGR_MESH_LOADER_H_INCLUDED__
#define __C_SGR_MESH_LOADER_H_INCLUDED__

#include <iostream>
#include "../Core/Meshes/SkinMesh.h"
#ifdef ANDROID
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#elif IOS
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/gl.h>
#elif UBUNTU
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <fstream>
#endif

class CSGRMeshFileLoader {
    struct SSGRVectHeader {
        float vx, vy, vz, nx, ny, nz, s, t;
    };

    struct SSGMCountHeaderLowPoly {
        u16 vertCount, indCount, colCount;
    };
    struct SSGMCountHeaderHighPoly {
        unsigned int vertCount, indCount, colCount;
    };

    struct SSGMVectHeader {
        float vx, vy, vz, nx, ny, nz;
    };
    struct SSGMUVHeader {
        float s, t;
    };
    struct SSGMIndexHeaderLowPoly {
        u_int16_t vtInd, colInd;
    };
    struct SSGMIndexHeaderHighPoly {
        unsigned int vtInd, colInd;
    };
    struct SSGMColHeader {
        unsigned char r, g, b;
    };

public:
    static AnimatedMesh* LoadMesh(string filepath, DEVICE_TYPE device = OPENGLES2);
    static Mesh* createSGMMesh(string filepath, DEVICE_TYPE device = OPENGLES2);
};

#endif // __C_SGR_MESH_LOADER_H_INCLUDED__
