//
//  RenderViewManager.h
//  Iyan3D
//
//  Created by Karthik on 24/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#ifndef RenderViewManager_h
#define RenderViewManager_h

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

#if !(TARGET_IPHONE_SIMULATOR)
#import <QuartzCore/CAMetalLayer.h>
#import <Metal/Metal.h>
#endif

#import <GameKit/GameKit.h>
#import "RenderingView.h"
#import "SceneManager.h"



@interface RenderViewManager : NSObject
{
    CAEAGLLayer* _eaglLayer;
    EAGLContext* _context;
    GLuint _colorRenderBuffer;
    GLuint _depthRenderBuffer;
    GLuint _frameBuffer;
    float screenScale;
}


- (void)setupLayer:(UIView*)renderView;
- (void)setupContext;
- (void)setupRenderBuffer;
- (void)setupDepthBuffer:(UIView*)renderView;
- (void)setupFrameBuffer:(SceneManager*)smgr;
- (void)presentRenderBuffer;

@end


#endif /* RenderViewManager_h */
