//
//  PreviewHelper.m
//  Iyan3D
//
//  Created by karthik on 13/07/15.
//  Copyright (c) 2015 Smackall Games. All rights reserved.
//

#import "PreviewHelper.h"
#import "Utility.h"
#import "AppHelper.h"
#import "AssetCellView.h"

#define ASSET_CAMERA 7
#define ASSET_LIGHT 8

@implementation PreviewHelper

SGPreviewScene* previewSceneObj;
RenderingView *renderView;

//+ (PreviewHelper *)getPreviewHelper
//{
//    static PreviewHelper *previewHelper;
//    @synchronized(self) {
//        if (!previewHelper)
//            previewHelper = [[self alloc] init];
//    }
//    return previewHelper;
//}


-(SGPreviewScene*) initScene:(RenderingView*)renderingView viewType:(VIEW_TYPE)viewType
{
    currentView = viewType;
    isMetalSupported = false;
    
    [self initMetal];
    renderView = renderingView;
    screenScale = [[UIScreen mainScreen] scale];
    DEVICE_TYPE deviceType = isMetalSupported ? METAL : OPENGLES2;
    
    if(!isMetalSupported) {
    [self setupLayer];
    [self setupContext];
    }
    
    SceneManager * sceneMgr = new SceneManager(renderingView.frame.size.width,renderingView.frame.size.height,screenScale,deviceType,[[[NSBundle mainBundle] resourcePath] UTF8String],(__bridge void*)renderingView);
    
    previewSceneObj = new SGPreviewScene(deviceType, sceneMgr, renderingView.frame.size.width, renderingView.frame.size.height, viewType);
    previewSceneObj->screenScale = screenScale;
    
    if(!isMetalSupported) {
    [self setupDepthBuffer];
    [self setupRenderBuffer];
    [self setupFrameBuffer];
    sceneMgr->setFrameBufferObjects(_frameBuffer,_colorRenderBuffer,_depthRenderBuffer);
    }
    
    sceneMgr->ShaderCallBackForNode = &nodeShaderCallBack;
    sceneMgr->isTransparentCallBack = &transparentCallBack;
    [self addGesturesToSceneView];
    return previewSceneObj;
}

- (void) initMetal
{
#if !(TARGET_IPHONE_SIMULATOR)
    if(iOSVersion >= 8.0)
        isMetalSupported = (MTLCreateSystemDefaultDevice() != NULL)?true:false;
#endif

}

-(void) addGesturesToSceneView
{
    UIPanGestureRecognizer *panRecognizer = [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(panGesture:)];
    panRecognizer.delegate = self;
    panRecognizer.minimumNumberOfTouches = 1;
    panRecognizer.maximumNumberOfTouches = 2;
    [renderView addGestureRecognizer:panRecognizer];
}

- (void)setupLayer {
    _eaglLayer = (CAEAGLLayer*) renderView.layer;
    _eaglLayer.opaque = YES;
    _eaglLayer.contentsScale = screenScale;
}
- (void)setupContext {
    EAGLRenderingAPI api = kEAGLRenderingAPIOpenGLES2;
    _context = [[EAGLContext alloc] initWithAPI:api];
    if (!_context)
        Logger::log(INFO,"AnimatinEditor","Failed to initialize OpenGLES 2.0 context");
    
    if (![EAGLContext setCurrentContext:_context])
        Logger::log(INFO,"AnimatinEditor" ,  "Failed to set current OpenGL context");
}
-(void) setCurrentContext
{
    if (![EAGLContext setCurrentContext:_context])
        Logger::log(INFO,"AnimatinEditor" ,  "Failed to set current OpenGL context");
}
- (void)setupRenderBuffer {
    glGenRenderbuffers(1, &_colorRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, _colorRenderBuffer);
    [_context renderbufferStorage:GL_RENDERBUFFER fromDrawable:_eaglLayer];
}
- (void)setupDepthBuffer {
    glGenRenderbuffers(1, &_depthRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, _depthRenderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24_OES, renderView.frame.size.width * screenScale, renderView.frame.size.height * screenScale);
}
- (void)setupFrameBuffer {
    glGenFramebuffers(1, &_frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, _frameBuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, _colorRenderBuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _depthRenderBuffer);
}

-(void) presentRenderBuffer{
    if(!isMetalSupported)
    [_context presentRenderbuffer:_colorRenderBuffer];
    //glFlush();
}

-(void)removeEngine
{
    if(playTimer) {
        [playTimer invalidate];
        playTimer = nil;
    }
    if(previewSceneObj){
        delete previewSceneObj;
        previewSceneObj = NULL;
    }
    
    if(!isMetalSupported) {
    if([EAGLContext currentContext] == _context)
        [EAGLContext setCurrentContext:nil];
    }
    
    _context = nil;
    _eaglLayer = nil;
    if (renderView)
        renderView = nil;
    
}

void nodeShaderCallBack(int nodeID,string matName,string callbackFuncName)
{
    if(callbackFuncName.compare("setUniforms") == 0 && previewSceneObj)
        previewSceneObj->shaderCallBackForNode(nodeID,matName);
}
bool transparentCallBack(int nodeId,string callbackFuncName)
{
    if(callbackFuncName.compare("setUniforms") == 0 && previewSceneObj)
        return previewSceneObj->isNodeTransparent(nodeId);
    else if(callbackFuncName.compare("setOBJUniforms") == 0)
        return  false;
    return false;
}

-(void) addCameraLight
{
    [self loadNodeInScene:ASSET_CAMERA AssetId:0 AssetName:L"CAMERA"];
    [self loadNodeInScene:ASSET_LIGHT AssetId:0 AssetName:L"LIGHT"];
}

-(int) getBoneCount
{
    if(previewSceneObj)
    return (int)previewSceneObj->nodes[previewSceneObj->nodes.size() - 1]->joints.size();
    else
        return 0;
}

-(bool) loadNodeInScene:(int)type AssetId:(int)assetId AssetName:(wstring)name
{
    return [self loadNodeInScene:type AssetId:assetId AssetName:name FontSize:0 BevelValue:0 TextColor:Vector4(0) FontPath:""];
}

-(bool) loadNodeInScene:(int)type AssetId:(int)assetId AssetName:(wstring)name FontSize:(float)fontSize BevelValue:(float)bevelValue TextColor:(Vector4)textColor FontPath:(string)fontFilePath
{
    if(previewSceneObj) {
    switch(type){
        case ASSET_CAMERA:
            previewSceneObj->loadNode(NODE_CAMERA, 0, L"CAMERA");
            break;
        case ASSET_LIGHT:
            previewSceneObj->loadNode(NODE_LIGHT, 0, L"LIGHT");
            break;
        case BACKGROUNDS:
        case ACCESSORIES:{
            SGNode* sgNode = previewSceneObj->loadNode(NODE_SGM,assetId,name);
            if(sgNode == NULL)
                return false;
            break;
        }
        case CHARACTERS:{
            SGNode* sgNode = previewSceneObj->loadNode(NODE_RIG,assetId,name);
            if(sgNode == NULL)
                return false;
            break;
        }
        case OBJ_FILE:{
            previewSceneObj->loadNode(NODE_OBJ, assetId,name);
            break;
        }
        case ANIMATIONS:{
            [playTimer invalidate];
            playTimer = nil;
            [self performSelectorOnMainThread:@selector(applyAnimationToNode:) withObject:[NSNumber numberWithInt:assetId] waitUntilDone:YES];
            break;
        }
        case FONT:{
            SGNode *textNode = previewSceneObj->loadNode(NODE_TEXT, 0,name,fontSize,bevelValue,textColor,fontFilePath);
            if(textNode == NULL){
                UIAlertView *loadNodeAlert = [[UIAlertView alloc]initWithTitle:@"Information" message:@"The font style you chose does not support the characters you entered." delegate:self cancelButtonTitle:@"Ok" otherButtonTitles:nil];
                [loadNodeAlert show];
                return false;
            }
            break;
        }
        default:{
            break;
        }
    }
    return true;
    } else {
        return true;
    }
}

- (void) applyAnimationToNode:(NSNumber*) assetId
{
    @synchronized(self) {
        previewSceneObj->applyAnimations([assetId intValue]);
    }
    playTimer = [NSTimer scheduledTimerWithTimeInterval:(1.0f/24.0f) target:self selector:@selector(playAnimation) userInfo:nil repeats:YES];
    [[NSRunLoop mainRunLoop] addTimer:playTimer forMode:NSDefaultRunLoopMode];
}

-(void) playAnimation
{
    if(previewSceneObj) {
    if(previewSceneObj->currentFrame == previewSceneObj->totalFrames)
        previewSceneObj->currentFrame = 0;
    
    if(previewSceneObj->currentFrame < previewSceneObj->totalFrames)
        previewSceneObj->playAnimation();
    
    previewSceneObj->currentFrame++;
    }
}

- (void) panGesture:(UIPanGestureRecognizer *)rec
{
    if(previewSceneObj) {
    //isPanned = true;
    vector<Vector2> p(2);
    CGPoint velocity = (!isMetalSupported) ? [rec velocityInView:renderView]:[rec velocityInView:renderView];
    NSUInteger touchCount = rec.numberOfTouches;
    for(int i=0; i<touchCount ;i++){
        if(!isMetalSupported){
            p[i].x = [rec locationOfTouch:i inView:renderView].x;
            p[i].y = [rec locationOfTouch:i inView:renderView].y;
        }else{
            p[i].x = [rec locationOfTouch:i inView:renderView].x;
            p[i].y = [rec locationOfTouch:i inView:renderView].y;
        }
    }
    switch(rec.state){
        case UIGestureRecognizerStateBegan:{
            switch(touchCount){
                case 1: {
                    previewSceneObj->swipeToRotate(-velocity.x/50.0,-velocity.y/50.0);
                    break;
                }
                case 2: {
                    if(currentView == ALL_ANIMATION_VIEW)
                        previewSceneObj->pinchBegan(p[0] * screenScale, p[1] * screenScale);
                    break;
                }
                default:
                    break;
            }
            break;
        }
        case UIGestureRecognizerStateChanged:{
            switch(touchCount){
                case 1: {
                    previewSceneObj->swipeToRotate(-velocity.x/50.0 , -velocity.y/50.0);
                    break;
                }
                case 2: {
                    if(currentView == ALL_ANIMATION_VIEW)
                        previewSceneObj->pinchZoom(p[0] * screenScale , p[1] * screenScale);
                    break;
                }
                default:
                    break;
            }
            break;
        }
        default:{
            previewSceneObj->swipeEnd();
            break;
        }
    }
    }
}


@end
