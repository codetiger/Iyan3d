//
//  Helper.h
//  Iyan3D
//
//  Created by Vivek on 19/03/15.
//  Copyright (c) 2015 Smackall Games. All rights reserved.
//

#ifndef __Iyan3D__Helper__
#define __Iyan3D__Helper__

#include <stdio.h>
#include <iostream>
#include "../Core/common/common.h"

#ifdef ANDROID
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#elif IOS
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#elif UBUNTU

#endif


class Helper{
public:
    static GLenum getOGLES2DataType(DATA_TYPE type);
    static DATA_TYPE getSGEngineDataType(GLenum type);
    static Vector2 screenToOpenglCoords(Vector2 originCoord,float screenWidth,float screenHeight);
};
#endif /* defined(__Iyan3D__Helper__) */
