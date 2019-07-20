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

#import <QuartzCore/CAMetalLayer.h>
#import <Metal/Metal.h>
#import <UIKit/UIKit.h>
#include "Texture.h"

class MTLTexture : public Texture {
public:
    id<MTLTexture> texture;

    MTLTexture();
    ~MTLTexture();
    bool           loadTexture(string name, string texturePath, TEXTURE_DATA_FORMAT format, TEXTURE_DATA_TYPE texelType, bool smoothTexture, int blurRadius = 0);
    bool           loadTextureFromVideo(string videoFileName, TEXTURE_DATA_FORMAT format, TEXTURE_DATA_TYPE texelType);
    void           updateTexture(string fileName, int frame);
    int            getBytesPerRow(int width, TEXTURE_DATA_FORMAT format);
    int            getBytesPerRow();
    MTLPixelFormat getMTLPixelFormat(TEXTURE_DATA_FORMAT format);
    int            getMTLPixelBytesForFormat(TEXTURE_DATA_FORMAT format);
    int            getBitsPerCompomentForFormat(TEXTURE_DATA_FORMAT format);
    void           createRenderTargetTexture(string textureName, TEXTURE_DATA_FORMAT format, TEXTURE_DATA_TYPE texelType, int width, int height, int storageMode);
};
#endif /* defined(__SGEngine2__MTLTexture__) */
