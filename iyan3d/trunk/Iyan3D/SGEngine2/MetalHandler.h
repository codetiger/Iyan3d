//
//  OGLES2RenderManager.h
//  SGEngine2
//
//  Created by Harishankar on 15/11/14.
//  Copyright (c) 2014 Smackall Games Pvt Ltd. All rights reserved.
//
#ifdef IOS
#ifndef __SGEngine2__MetalHandler__
#define __SGEngine2__MetalHandler__


#if !(TARGET_IPHONE_SIMULATOR)
    #import <QuartzCore/CAMetalLayer.h>
    #import <Metal/Metal.h>
#endif
#import <UIKit/UIKit.h>
#include "Logger.h"
#include "common.h"
#include "Node.h"
#import "ImageLoaderOBJCWrapper.h"
#import "MetalWrapper.h"
#import "MetalWrapperForMaterial.h"

class MetalHandler{

public:
#if !(TARGET_IPHONE_SIMULATOR)
    static MTLRenderPipelineDescriptor* FMGetPipelineDesc(NSString* VertFuncName,NSString* FragFuncName);
    static void setDevice(id <MTLDevice> device);
    static void setMTLLayer(CAMetalLayer* mtlLayer);
    static void setMTLLibrary(id <MTLLibrary> mtlLib);
    static id <MTLDevice> getMTLDevice();
#endif
};
#endif /* defined(__SGEngine2__MetalHandler__) */

#endif