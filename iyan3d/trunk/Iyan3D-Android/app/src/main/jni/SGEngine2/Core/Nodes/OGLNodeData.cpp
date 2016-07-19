//
//  OGLNodeData.cpp
//  SGEngine2
//
//  Created by Vivek on 27/12/14.
//  Copyright (c) 2014 Smackall Games Pvt Ltd. All rights reserved.
//

#include "OGLNodeData.h"

//these ugly typenames are defined in GLES2/gl2ext.h
#ifdef ANDROID
#include <EGL/egl.h>
PFNGLDELETEVERTEXARRAYSOESPROC glDeleteVertexArraysOES;

void initialiseDeleteVAOFunc()
{
    glDeleteVertexArraysOES = (PFNGLDELETEVERTEXARRAYSOESPROC)eglGetProcAddress ( "glDeleteVertexArraysOES" );
}
#endif

OGLNodeData::OGLNodeData()
{
    VAOCreated = false;
    IndexBufLocations.clear();
    vertexBufLocations.clear();
    vertexArrayLocations.clear();
}

OGLNodeData::~OGLNodeData()
{

#ifdef ANDROID
    if(vertexArrayLocations.size() > 0)
            initialiseDeleteVAOFunc();
#endif

    for(int i = 0; i < vertexBufLocations.size();i++)
        glDeleteBuffers(1,&vertexBufLocations[i]);
    
    for(int i = 0; i < IndexBufLocations.size();i++)
        glDeleteBuffers(1,&IndexBufLocations[i]);

    for(int i = 0; i < vertexArrayLocations.size();i++)
        glDeleteVertexArraysOES(1, &vertexArrayLocations[i]);

    vertexBufLocations.clear();
    IndexBufLocations.clear();
    vertexArrayLocations.clear();
}

void OGLNodeData::removeVertexBuffers()
{
    for(int i = 0; i < vertexBufLocations.size();i++)
        glDeleteBuffers(1,&vertexBufLocations[i]);
    vertexBufLocations.clear();
}
