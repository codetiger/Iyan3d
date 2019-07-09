//
//  OGLTexture.h
//  SGEngine2
//
//  Created by Vivek on 29/12/14.
//  Copyright (c) 2014 Smackall Games Pvt Ltd. All rights reserved.
//

#ifndef __SGEngine2__OGLTexture__
#define __SGEngine2__OGLTexture__

#include <iostream>
#include "../common/common.h"
#include "Texture.h"
#include "../../RenderManager/ImageLoaderOBJCWrapper.h"
#include "../../RenderManager/PngFileManager.h"

#ifdef ANDROID
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#elif IOS
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#endif


class OGLTexture:public Texture{
public:
    GLuint OGLTextureName, rttFrameBuffer, rttDepthBuffer;
    char texturePathconverted[];
    OGLTexture();
    ~OGLTexture();
    void removeTexture();
    bool loadTexture(string texturePath,string textureName,TEXTURE_DATA_FORMAT format,TEXTURE_DATA_TYPE texelType, bool smoothTexture, int blurRadius = 0);
    bool loadTextureFromVideo(string videoFileName,TEXTURE_DATA_FORMAT format,TEXTURE_DATA_TYPE texelType);
    void updateTexture(string filePath, int frame);
#ifdef  ANDROID
    void updateTexture(unsigned char* imageData);
#endif
    GLenum getOGLTextureFormat(TEXTURE_DATA_FORMAT format);
    GLenum getOGLTextureType(TEXTURE_DATA_TYPE type);
    void createRenderTargetTexture(string textureName , TEXTURE_DATA_FORMAT format, TEXTURE_DATA_TYPE texelType, int width, int height);
    void createTexture(TEXTURE_DATA_FORMAT format, TEXTURE_DATA_TYPE texelType, int width, int height, const GLvoid* data);
};
#endif /* defined(__SGEngine2__OGLTexture__) */
