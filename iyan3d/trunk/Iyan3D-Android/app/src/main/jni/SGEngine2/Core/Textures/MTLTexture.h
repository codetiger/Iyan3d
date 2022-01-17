//
//  MTLTexture.h
//  SGEngine2
//
//  Created by Vivek on 29/11/14.
//  Copyright (c) 2014 Smackall Games Pvt Ltd. All rights reserved.
//

#ifndef __SGEngine2__MTLTexture__
#define __SGEngine2__MTLTexture__

#include "../common/common.h"
#include "../../RenderManager/ImageLoaderOBJCWrapper.h"

#ifdef IOS
#if !(TARGET_IPHONE_SIMULATOR)
    #import <QuartzCore/CAMetalLayer.h>
    #import <Metal/Metal.h>
#endif
#import <UIKit/UIKit.h>
#endif
#include "Texture.h"

class MTLTexture:public Texture{
public:
#if !(TARGET_IPHONE_SIMULATOR)
    id <MTLTexture> texture;
    
    MTLTexture();
    ~MTLTexture();
    bool loadTexture(string name,string texturePath,TEXTURE_DATA_FORMAT format,TEXTURE_DATA_TYPE texelType);
    bool loadTextureFromVideo(string videoFileName,TEXTURE_DATA_FORMAT format,TEXTURE_DATA_TYPE texelType);
    void updateTexture(string fileName, int frame);
    int getBytesPerRow(int width,TEXTURE_DATA_FORMAT format);
    int getBytesPerRow();
    MTLPixelFormat getMTLPixelFormat(TEXTURE_DATA_FORMAT format);
    int getMTLPixelBytesForFormat(TEXTURE_DATA_FORMAT format);
    int getBitsPerCompomentForFormat(TEXTURE_DATA_FORMAT format);
    void createRenderTargetTexture(string textureName,TEXTURE_DATA_FORMAT format,TEXTURE_DATA_TYPE texelType,int width,int height);
#endif
};
#endif /* defined(__SGEngine2__MTLTexture__) */