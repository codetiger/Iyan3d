//
//  DummyTexture.cpp
//  SGEngine2
//
//  Created by Vivek on 29/12/14.
//  Copyright (c) 2014 Smackall Games Pvt Ltd. All rights reserved.
//

#include "DummyTexture.h"

DummyTexture::DummyTexture(){
    texelFormat = TEXTURE_RGBA8;
    texelType = TEXTURE_BYTE;
}

DummyTexture::~DummyTexture(){
}

bool DummyTexture::loadTexture(string name,string texturePath,TEXTURE_DATA_FORMAT format,TEXTURE_DATA_TYPE texelType){
    textureName = name;
    texelFormat = format;
    this->texelType = texelType;
    return true;
}
void DummyTexture::createRenderTargetTexture(string name, TEXTURE_DATA_FORMAT format, TEXTURE_DATA_TYPE type, int textureWidth, int textureHeight)
{
    textureName = name;
    texelFormat = TEXTURE_RGBA8; // default type as now
    texelType = TEXTURE_BYTE; // default type as now
    width = textureWidth;
    height = textureHeight;
}
