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

@protocol RenderViewManagerDelegate

- (bool) isMetalSupportedDevice;
- (void) reloadFrames;

@end


@interface RenderViewManager : NSObject <UIGestureRecognizerDelegate>
{
    CAEAGLLayer* _eaglLayer;
    EAGLContext* _context;
    GLuint _colorRenderBuffer;
    GLuint _depthRenderBuffer;
    GLuint _frameBuffer;
    float screenScale;
    SceneManager *smgr;
}
@property (strong, atomic) RenderingView *renderView;
@property (strong, atomic) id <RenderViewManagerDelegate> delegate;
@property (nonatomic) bool isPlaying;
@property (nonatomic) bool isPanned;
@property (nonatomic) bool checkTapSelection;
@property (nonatomic) Vector2 tapPosition;
@property (nonatomic) Vector2 touchBeganPosition;
@property (nonatomic) bool checkCtrlSelection;
- (void)setUpPaths;
- (void)setupLayer:(UIView*)renderView;
- (void)setupContext;
- (void)setupRenderBuffer;
- (void)setupDepthBuffer:(UIView*)renderView;
- (void)setupFrameBuffer:(SceneManager*)smgr;
- (void)presentRenderBuffer;
- (void) setUpCallBacks:(void*)scene;
- (void) addCameraLight;
- (bool)loadNodeInScene:(int)type AssetId:(int)assetId AssetName:(wstring)name Width:(int)imgWidth Height:(int)imgHeight;
- (bool) removeNodeFromScene:(int)nodeIndex;
- (void)addGesturesToSceneView;

@end


#endif /* RenderViewManager_h */
