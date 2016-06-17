//
//  Texture.h
//  SGEngine2
//
//  Created by Vivek on 30/12/14.
//  Copyright (c) 2014 Smackall Games Pvt Ltd. All rights reserved.
//

#ifndef SGEngine2_Texture_h
#define SGEngine2_Texture_h

#include <iostream>
#include "../common/common.h"

class Texture{
public:
    Texture(){
        
    }
    virtual ~Texture(){
        
    }
    int width,height;
    TEXTURE_DATA_FORMAT texelFormat;
    TEXTURE_DATA_TYPE texelType;
    string textureName;
    virtual bool loadTexture(string texturePath,string textureName,TEXTURE_DATA_FORMAT format,TEXTURE_DATA_TYPE texelType) = 0;
    virtual bool loadTextureFromVideo(string videoFileName,TEXTURE_DATA_FORMAT format,TEXTURE_DATA_TYPE texelType) = 0;
    virtual void updateTexture(string filePath, int frame) = 0;
    #ifdef  ANDROID
    virtual void updateTexture(unsigned char* imageData) = 0;
    #endif
    virtual void createRenderTargetTexture(string textureName,TEXTURE_DATA_FORMAT format,TEXTURE_DATA_TYPE texelType,int width,int height) = 0;
    bool operator==(Texture *texture){
        if(textureName.compare(texture->textureName) == 0 && width == texture->width && height == texture->height && texture->texelFormat == texelFormat && texture->texelType == texelType){
            return true;
        }
        return false;
    }
};

#endif
