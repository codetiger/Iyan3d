//
//  OGLTexture.h
//  SGEngine2
//
//  Created by Vivek on 29/12/14.
//  Copyright (c) 2014 Smackall Games Pvt Ltd. All rights reserved.
//

#ifndef __SGEngine2__DummyTexture__
#define __SGEngine2__DummyTexture__

#include <iostream>
#include "../common/common.h"
#include "Texture.h"

class DummyTexture:public Texture{
public:
    DummyTexture();
    ~DummyTexture();
    bool loadTexture(string texturePath,string textureName,TEXTURE_DATA_FORMAT format,TEXTURE_DATA_TYPE texelType);
    void createRenderTargetTexture(string textureName , TEXTURE_DATA_FORMAT format, TEXTURE_DATA_TYPE texelType, int width, int height);
    void updateTexture(string filePath, int frame);
    bool loadTextureFromVideo(string videoFileName,TEXTURE_DATA_FORMAT format,TEXTURE_DATA_TYPE texelType);
};
#endif /* defined(__SGEngine2__DummyTexture__) */
