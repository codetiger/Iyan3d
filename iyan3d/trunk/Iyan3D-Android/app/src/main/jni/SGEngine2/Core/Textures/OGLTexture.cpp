//
//  OGLTexture.cpp
//  SGEngine2
//
//  Created by Vivek on 29/12/14.
//  Copyright (c) 2014 Smackall Games Pvt Ltd. All rights reserved.
//

#ifdef ANDROID
#include <malloc.h>
//#include "../../../../../../../../../../Android/Sdk/ndk-bundle/platforms/android-21/arch-arm/usr/include/android/log.h"
#include "../../../opengl.h"
#endif

#include "OGLTexture.h"


OGLTexture::OGLTexture(){
    texelFormat = TEXTURE_RGBA8;
    texelType = TEXTURE_BYTE;
    OGLTextureName = NULL;
    rttFrameBuffer = NOT_EXISTS;
    rttDepthBuffer = NOT_EXISTS;
}
OGLTexture::~OGLTexture(){
    if(rttFrameBuffer != NOT_EXISTS)
        glDeleteBuffers(1,&rttFrameBuffer);
    if(rttDepthBuffer != NOT_EXISTS)
        glDeleteBuffers(1,&rttDepthBuffer);
    if(OGLTextureName)
        glDeleteTextures(1, &OGLTextureName);
    OGLTextureName = NULL;
}
void OGLTexture::removeTexture(){
    if(OGLTextureName)
        glDeleteTextures(1, &OGLTextureName);
    OGLTextureName = NULL;
}
bool OGLTexture::loadTexture(string name,string texturePath,TEXTURE_DATA_FORMAT format,TEXTURE_DATA_TYPE texelType)
{
    textureName = name;
    texelFormat = format;
    this->texelType = texelType;

    unsigned char *imageData;
#ifndef IOS
    imageData = PNGFileManager::read_png_file(texturePath.c_str() , width , height);
#else
    imageData = loadPNGImage(texturePath , width , height);
#endif

    if(!imageData)
        return false;

//    Logger::log(INFO, "OGLTEX LOADTEX", "width " + to_string(width) + " height " + to_string(height));
    glGenTextures(1, &OGLTextureName);
    glBindTexture(GL_TEXTURE_2D, OGLTextureName);
    glTexImage2D(GL_TEXTURE_2D, 0,getOGLTextureFormat(format), width, height, 0, getOGLTextureFormat(format), getOGLTextureType(texelType), imageData);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    free(imageData);
    return true;
}

bool OGLTexture::loadTextureFromVideo(string videoFileName,TEXTURE_DATA_FORMAT format,TEXTURE_DATA_TYPE texelType)
{

    textureName = videoFileName;
    texelFormat = format;
    this->texelType = texelType;
    unsigned char *imageData;
    #ifdef ANDROID
        imageData = PNGFileManager::getImageDataFromVideo(videoFileName, 0, width, height);
    #else
        imageData = getImageDataFromVideo(videoFileName, 0, width, height);
    #endif

    if(!imageData)
        return false;

    glGenTextures(1, &OGLTextureName);
    glBindTexture(GL_TEXTURE_2D, OGLTextureName);
    glTexImage2D(GL_TEXTURE_2D, 0,getOGLTextureFormat(format), width, height, 0, getOGLTextureFormat(format), getOGLTextureType(texelType), imageData);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    free(imageData);
    return true;
}


void OGLTexture::updateTexture(string fileName, int frame)
{
    unsigned char *imageData;
    #ifdef ANDROID
        imageData = PNGFileManager::getImageDataFromVideo(fileName, frame, width, height);
    #else
        imageData = getImageDataFromVideo(fileName, frame, width, height);
    #endif
    if(!imageData)
        return false;

    glBindTexture(GL_TEXTURE_2D, OGLTextureName);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, getOGLTextureFormat(texelFormat), getOGLTextureType(texelType), imageData);
    free(imageData);
}

GLenum OGLTexture::getOGLTextureFormat(TEXTURE_DATA_FORMAT format){
    GLenum oglFormat = GL_RGBA;
    switch(format){
        case TEXTURE_RG:
            oglFormat = GL_RG8_EXT;
            break;
        case TEXTURE_R8:
            oglFormat = GL_RED_EXT;
            break;
    }
    return oglFormat;
}
GLenum OGLTexture::getOGLTextureType(TEXTURE_DATA_TYPE type){
    GLenum oglType = GL_UNSIGNED_BYTE;
// switch
    return oglType;
}
void OGLTexture::createRenderTargetTexture(string name, TEXTURE_DATA_FORMAT format, TEXTURE_DATA_TYPE type, int textureWidth, int textureHeight)
{
    textureName = name;
    texelFormat = TEXTURE_RGBA8; // default type as now
    texelType = TEXTURE_BYTE; // default type as now
    width = textureWidth;
    height = textureHeight;
    if(rttFrameBuffer == NOT_EXISTS)
        glGenFramebuffers(1,&rttFrameBuffer);
    if(rttDepthBuffer == NOT_EXISTS)
        glGenRenderbuffers(1,&rttDepthBuffer);

    glBindFramebuffer(GL_FRAMEBUFFER,rttFrameBuffer);
    createTexture(texelFormat, texelType, textureWidth, textureHeight, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,OGLTextureName,0);

    glBindRenderbuffer(GL_RENDERBUFFER,rttDepthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24_OES,width,height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER,rttDepthBuffer);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        Logger::log(ERROR,"OGLTexture CreateRenderBuffer","FB Not Complete");
}
void OGLTexture::createTexture(TEXTURE_DATA_FORMAT format, TEXTURE_DATA_TYPE type, int textureWidth, int textureHeight, const GLvoid *data)
{
    glGenTextures(1, &OGLTextureName);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, OGLTextureName);
    glTexImage2D(GL_TEXTURE_2D, 0, getOGLTextureFormat(format), textureWidth, textureHeight, 0, getOGLTextureFormat(format), getOGLTextureType(type), data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}