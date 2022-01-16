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
#import "Constants.h"
#import "Vector3.h"

@protocol RenderViewManagerDelegate

- (bool) isMetalSupportedDevice;
- (void) reloadFrames;
- (void) presentPopOver:(CGRect )arect;
- (void) updateAssetListInScenes;
- (void) stopPlaying;
- (void)undoRedoButtonState:(int)state;

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
    int touchCountTracker,lightCount;
    
}
@property (strong, atomic) RenderingView *renderView;
@property (strong, atomic) id <RenderViewManagerDelegate> delegate;
@property (nonatomic) bool isPlaying;
@property (nonatomic) bool isPanned;
@property (nonatomic) bool makePanOrPinch;
@property (nonatomic) bool checkTapSelection;
@property (nonatomic) bool longPress;
@property (nonatomic) Vector2 tapPosition;
@property (nonatomic) CGRect longPresPosition;
@property (nonatomic) vector<Vector2> touchMovePosition;
@property (nonatomic) bool checkCtrlSelection;
- (void)setUpPaths:(SceneManager*)sceneMngr;
- (void)setupLayer:(UIView*)renderView;
- (void)setupContext;
- (void)setupRenderBuffer;
- (void)setupDepthBuffer:(UIView*)renderView;
- (void)setupFrameBuffer;
- (void)presentRenderBuffer;
- (void) setUpCallBacks:(void*)scene;
- (void) addCameraLight;
-(void) showPopOver:(int) selectedNodeId;
- (bool)loadNodeInScene:(int)type AssetId:(int)assetId AssetName:(wstring)name TextureName:(NSString*)textureName Width:(int)imgWidth Height:(int)imgHeight isTempNode:(bool)isTempNode More:(NSMutableDictionary*)moreDetail ActionType:(ActionType)assetAddType VertexColor:(Vector4)vertexColor;
- (bool) removeNodeFromScene:(int)nodeIndex IsUndoOrRedo:(BOOL)isUndoOrRedo;
- (void)addGesturesToSceneView;
- (void)panOrPinchProgress;

@end


#endif /* RenderViewManager_h */
