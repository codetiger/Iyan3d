//
//  PreviewHelper.h
//  Iyan3D
//
//  Created by karthik on 13/07/15.
//  Copyright (c) 2015 Smackall Games. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "RenderingView.h"
#import "Constants.h"
#import "SGPreviewScene.h"
#import "SGNode.h"

@interface PreviewHelper : NSObject <UIGestureRecognizerDelegate>
{
    NSTimer *playTimer;
    BOOL isMetalSupported;
    CAEAGLLayer* _eaglLayer;
    EAGLContext* _context;
    GLuint _colorRenderBuffer;
    GLuint _depthRenderBuffer;
    GLuint _frameBuffer;
    float screenScale;
    VIEW_TYPE currentView;
}

//+(PreviewHelper *)getPreviewHelper;
-(SGPreviewScene*) initScene:(RenderingView*)renderingView viewType:(VIEW_TYPE)viewType;
-(void) presentRenderBuffer;
-(void) setCurrentContext;
-(void) addCameraLight;
-(int) getBoneCount;
-(bool) loadNodeInScene:(int)type AssetId:(int)assetId AssetName:(wstring)name;

-(bool) loadNodeInScene:(int)type AssetId:(int)assetId AssetName:(wstring)name FontSize:(float)fontSize BevelValue:(float)bevelValue TextColor:(Vector4)textColor FontPath:(string)fontFilePath;
-(void) removeEngine;

@end
