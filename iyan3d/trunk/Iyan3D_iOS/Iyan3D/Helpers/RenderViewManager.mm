//
//  RenderViewManager.m
//  Iyan3D
//
//  Created by Karthik on 24/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#import "RenderViewManager.h"
#import "Logger.h"
#import <OpenGLES/ES2/glext.h>


@implementation RenderViewManager

- (void)setupLayer:(UIView*)renderView
{
    screenScale = [[UIScreen mainScreen] scale];
    _eaglLayer = (CAEAGLLayer*)renderView.layer;
    _eaglLayer.opaque = YES;
    _eaglLayer.contentsScale = screenScale;
}
- (void)setupContext
{
    EAGLRenderingAPI api = kEAGLRenderingAPIOpenGLES2;
    _context = [[EAGLContext alloc] initWithAPI:api];
    if (!_context)
        Logger::log(INFO, "AnimatinEditor", "Failed to initialize OpenGLES 2.0 context");
        
        if (![EAGLContext setCurrentContext:_context])
            Logger::log(INFO, "AnimatinEditor", "Failed to set current OpenGL context");
            }
- (void)setupRenderBuffer
{
    glGenRenderbuffers(1, &_colorRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, _colorRenderBuffer);
    [_context renderbufferStorage:GL_RENDERBUFFER fromDrawable:_eaglLayer];
}
- (void)setupDepthBuffer:(UIView*)renderView
{
    glGenRenderbuffers(1, &_depthRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, _depthRenderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24_OES, renderView.frame.size.width * screenScale, renderView.frame.size.height * screenScale);
}
- (void)setupFrameBuffer:(SceneManager*) smgr
{
    glGenFramebuffers(1, &_frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, _frameBuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, _colorRenderBuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _depthRenderBuffer);
    smgr->setFrameBufferObjects(_frameBuffer, _colorRenderBuffer, _depthRenderBuffer);
}
- (void)presentRenderBuffer
{
    [_context presentRenderbuffer:_colorRenderBuffer];
}

@end