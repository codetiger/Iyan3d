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
#import "SGEditorScene.h"

#define ASSET_ANIMATION 4
#define ASSET_RIGGED 1
#define ASSET_BACKGROUNDS 2
#define ASSET_ACCESSORIES 3
#define ASSET_OBJ 6
#define ASSET_CAMERA 7
#define ASSET_LIGHT 8
#define ASSET_IMAGE 9
#define ASSET_TEXT 10
#define ASSET_ADDITIONAL_LIGHT 900
#define ADD_OBJECT 100
#define DELETE_OBJECT 200

@implementation RenderViewManager

SGEditorScene *editorScene;

- (void)setUpPaths
{
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
    NSString* cachesDir = [paths objectAtIndex:0];
    constants::CachesStoragePath = (char*)[cachesDir cStringUsingEncoding:NSASCIIStringEncoding];
}

- (void)setupLayer:(UIView*)renderView
{
    _renderView = (RenderingView*)renderView;
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
- (void)setupFrameBuffer:(SceneManager*) sceneMngr
{
    smgr = sceneMngr;
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

- (void) setUpCallBacks:(void *)scene
{
    editorScene = (SGEditorScene*)scene;
    if(!editorScene)
        return;
    
    smgr->ShaderCallBackForNode = &shaderCallBackForNode;
    smgr->isTransparentCallBack = &isTransparentCallBack;
    //    animationScene->downloadMissingAssetCallBack = &downloadMissingAssetCallBack;
    //    animationScene->fileWriteCallBack = &fileWriteCallBack;

}

void shaderCallBackForNode(int nodeID, string matName, string callbackFuncName)
{
    if(!editorScene)
        return;
    
    if (callbackFuncName.compare("setUniforms") == 0)
        editorScene->shaderCallBackForNode(nodeID, matName);
    else if (callbackFuncName.compare("setJointSpheresUniforms") == 0)
        editorScene->setJointsUniforms(nodeID, matName);
    else if (callbackFuncName.compare("setCtrlUniforms") == 0)
        editorScene->setControlsUniforms(nodeID, matName);
    else if (callbackFuncName.compare("RotationCircle") == 0)
        editorScene->setRotationCircleUniforms(nodeID, matName);
}

bool isTransparentCallBack(int nodeId, string callbackFuncName)
{
    if(!editorScene)
        return false;
    
        if (callbackFuncName.compare("setUniforms") == 0)
            return editorScene->isNodeTransparent(nodeId);
        else if (callbackFuncName.compare("setJointSpheresUniforms") == 0)
            return editorScene->isJointTransparent(nodeId, callbackFuncName);
        else if (callbackFuncName.compare("setCtrlUniforms") == 0)
            return editorScene->isControlsTransparent(nodeId, callbackFuncName);
        else if (callbackFuncName.compare("setOBJUniforms") == 0)
            return false;
        else if (callbackFuncName.compare("RotationCircle") == 0)
            return false;
        return false;
}

- (void) addCameraLight
{
    [self loadNodeInScene:ASSET_CAMERA AssetId:0 AssetName:ConversionHelper::getWStringForString("CAMERA") Width:0 Height:0 isTempNode:false];
    [self.delegate updateAssetListInScenes:ASSET_CAMERA assetId:0 actionType:(int)ADD_OBJECT];
    [self loadNodeInScene:ASSET_LIGHT AssetId:0 AssetName:ConversionHelper::getWStringForString("LIGHT") Width:0 Height:0 isTempNode:false];
    [self.delegate updateAssetListInScenes:ASSET_LIGHT assetId:0 actionType:(int)ADD_OBJECT];
    
}

- (bool)loadNodeInScene:(int)type AssetId:(int)assetId AssetName:(wstring)name Width:(int)imgWidth Height:(int)imgHeight isTempNode:(bool)isTempNode
{
    // TODO a lot to implement
    ActionType assetAddType = IMPORT_ASSET_ACTION;
    
    if(editorScene) {
        editorScene->loader->removeTempNodeIfExists();
        if(isTempNode)
            editorScene->isPreviewMode = false;
    }
    
    switch (type) {
        case ASSET_CAMERA: {
            editorScene->loader->loadNode(NODE_CAMERA, assetId, name, 0, 0, assetAddType);
            break;
        }
        case ASSET_LIGHT: {
            editorScene->loader->loadNode(NODE_LIGHT, assetId, name, 0, 0, assetAddType);
            break;
        }
        case ASSET_BACKGROUNDS:
        case ASSET_ACCESSORIES: {
//            [self showTipsViewForAction:OBJECT_IMPORTED];
            SGNode* sgNode = editorScene->loader->loadNode(NODE_SGM, assetId, name, 0, 0, assetAddType);
            if(sgNode)
                sgNode->isTempNode = isTempNode;
            if(!isTempNode){
                [self.delegate updateAssetListInScenes:ASSET_BACKGROUNDS assetId:assetId actionType:(int)ADD_OBJECT];
            }
            break;
        }
        case ASSET_RIGGED: {
//            [self showTipsViewForAction:OBJECT_IMPORTED_HUMAN];
            SGNode* sgNode = editorScene->loader->loadNode(NODE_RIG, assetId, name, 0, 0, assetAddType);
            if(sgNode)
                sgNode->isTempNode = isTempNode;
            if(!isTempNode){
                [self.delegate updateAssetListInScenes:ASSET_RIGGED assetId:assetId actionType:(int)ADD_OBJECT];
            }
            break;
        }
        case ASSET_OBJ: {
//            [self showTipsViewForAction:OBJECT_IMPORTED];
            SGNode* sgNode = editorScene->loader->loadNode(NODE_OBJ, assetId, name, 0, 0, assetAddType);
            if(sgNode)
                sgNode->isTempNode = isTempNode;
            if(!isTempNode){
                [self.delegate updateAssetListInScenes:ASSET_OBJ assetId:assetId actionType:(int)ADD_OBJECT];
            }
            break;
        }
        case ASSET_IMAGE: {
//            [self showTipsViewForAction:OBJECT_IMPORTED];
            SGNode* sgNode = editorScene->loader->loadNode(NODE_IMAGE, 0, name, imgWidth, imgHeight, assetAddType,Vector4(imgWidth,imgHeight,0,0));
            if(sgNode)
                sgNode->isTempNode = isTempNode;
            if(!isTempNode){
                [self.delegate updateAssetListInScenes:ASSET_IMAGE assetId:assetId actionType:(int)ADD_OBJECT];
            }
            break;
        }
        case ASSET_ANIMATION: {
            /*
            NSArray* paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
            NSString* documentsDirectory = [paths objectAtIndex:0];
            NSString* filePath = [NSString stringWithFormat:@"%@/%d.sga", documentsDirectory, assetId];
            std::string* sgaFilePath = new std::string([filePath UTF8String]);
            animationScene->animFilePath = "";
            animationScene->applySGAOnNode(sgaFilePath);
            delete sgaFilePath;
            animationScene->storeAddOrRemoveAssetAction(ACTION_APPLY_ANIM, 0);
            [self.frameCarouselView reloadData];
            [self showTipsViewForAction:ANIMATION_APPLIED];
            isFileSaved = !(animationScene->changeAction = true);
             */
            break;
        }
        case ASSET_TEXT: {
//            [self showTipsViewForAction:OBJECT_IMPORTED];
            /*
            SGNode* textNode = editorScene->loader->loadNode(editorScene, NODE_TEXT, 0, name, imgWidth, imgHeight, assetAddType, textColor, [fontFilePath UTF8String]);
            if (textNode == NULL) {
                [self.view endEditing:YES];
                UIAlertView* loadNodeAlert = [[UIAlertView alloc] initWithTitle:@"Information" message:@"The font style you chose does not support the characters you entered." delegate:self cancelButtonTitle:@"Ok" otherButtonTitles:nil];
                [loadNodeAlert show];
                return false;
            }
             */
            break;
        }
        case ASSET_ADDITIONAL_LIGHT: {
            //TODO enum for max lights
            if(ShaderManager::lightPosition.size() < 5) {
                editorScene->loader->loadNode(NODE_ADDITIONAL_LIGHT, assetId , name, imgWidth , imgHeight , assetAddType , Vector4(1.0));
            } else {
                NSLog(@"Max lights 5");
            }
            
            break;
        }
        default: {
            break;
        }
    }
    return true;
}

- (bool) removeNodeFromScene:(int)nodeIndex
{
    editorScene->loader->removeObject(nodeIndex);
    [self.delegate updateAssetListInScenes:NODE_UNDEFINED assetId:nodeIndex actionType:DELETE_OBJECT];
}

- (void)addGesturesToSceneView
{
    UIPanGestureRecognizer* panRecognizer = [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(panGesture:)];
    panRecognizer.delegate = self;
    panRecognizer.minimumNumberOfTouches = 1;
    panRecognizer.maximumNumberOfTouches = 2;
    UITapGestureRecognizer* tapRecognizer = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(tapGesture:)];
    tapRecognizer.delegate = self;
    UILongPressGestureRecognizer *longpressRecogniser = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(handleLongPressGestures:)];
    longpressRecogniser.minimumPressDuration = 1.0f;
    longpressRecogniser.allowableMovement = 100.0f;
    
    
    if (![self.delegate isMetalSupportedDevice]) {
        [self.renderView addGestureRecognizer:panRecognizer];
        [self.renderView addGestureRecognizer:tapRecognizer];
        [self.renderView addGestureRecognizer:longpressRecogniser];
    }
    else {
        [self.renderView addGestureRecognizer:panRecognizer];
        [self.renderView addGestureRecognizer:tapRecognizer];
        [self.renderView addGestureRecognizer:longpressRecogniser];
    }
}

- (void)tapGesture:(UITapGestureRecognizer*)rec
{
// TODO add missing
    
//    isTapped = true;
    if (!_isPlaying && !_isPanned) {
        CGPoint position;
        position = [rec locationInView:self.renderView];
        _checkTapSelection = true;
        _tapPosition = Vector2(position.x, position.y) * screenScale;
        editorScene->isRTTCompleted = true;
        [self.delegate reloadFrames];
//        [self setupEnableDisableControls];
        
//        if (editorScene->actions.size() > 0 && editorScene->currentAction > 0)
//            [self.undoButton setEnabled:YES];
    }
//    animationScene->setLightingOn();
}

- (void)panGesture:(UIPanGestureRecognizer*)rec
{
    // TODO add functions for two finger pan
    _isPanned = true;
    vector<Vector2> p(2);
    CGPoint velocity = (![self.delegate isMetalSupportedDevice]) ? [rec velocityInView:self.renderView] : [rec velocityInView:self.renderView];
    NSUInteger touchCount = rec.numberOfTouches;
    for (int i = 0; i < touchCount; i++) {
        if (![self.delegate isMetalSupportedDevice]) {
            p[i].x = [rec locationOfTouch:i inView:self.renderView].x;
            p[i].y = [rec locationOfTouch:i inView:self.renderView].y;
        }
        else {
            p[i].x = [rec locationOfTouch:i inView:self.renderView].x;
            p[i].y = [rec locationOfTouch:i inView:self.renderView].y;
        }
    }
    switch (rec.state) {
        case UIGestureRecognizerStateBegan: {
            editorScene->moveMan->touchBegan(p[0] * screenScale);
            switch (touchCount) {
                case 1: {
                    _checkCtrlSelection = true;
                    _touchMovePosition.clear();
                    _touchMovePosition.push_back(p[0] * screenScale);
                    editorScene->moveMan->swipeProgress(-velocity.x / 50.0, -velocity.y / 50.0);
                    break;
                }
                case 2: {
                    editorScene->moveMan->panBegan(p[0] * screenScale, p[1] * screenScale);
                    editorScene->updater->updateLightCamera();
                    break;
                }
                default:
                    break;
            }
            break;
        }
        case UIGestureRecognizerStateChanged: {
            if (editorScene->isControlSelected && touchCount == 2) {
                _makePanOrPinch = false;
                editorScene->moveMan->panBegan(p[0] * screenScale, p[1] * screenScale);
            }
            switch (touchCount) {
                case 1: {
                    editorScene->moveMan->touchMove(p[0] * screenScale, p[1] * screenScale, SCREENWIDTH * screenScale, SCREENHEIGHT * screenScale);
                    editorScene->moveMan->swipeProgress(-velocity.x / 50.0, -velocity.y / 50.0);
                    break;
                }
                case 2: {
                    _touchMovePosition.clear();
                    _touchMovePosition.push_back(p[0] * screenScale);
                    _touchMovePosition.push_back(p[1] * screenScale);
                    _makePanOrPinch = true;
                    break;
                }
                default:
                    break;
            }
            break;
        }
        default: {
            _makePanOrPinch = false;
            _isPanned = false;
            editorScene->moveMan->touchEnd(p[0] * screenScale);
            [self.delegate reloadFrames];
            break;
        }
    }
    editorScene->updater->updateControlsOrientaion();
    if (editorScene->actionMan->actions.size() > 0 && editorScene->actionMan->currentAction > 0 && !_isPlaying) {
        //[self.undoButton setEnabled:YES];
    }
}

- (void)handleLongPressGestures:(UILongPressGestureRecognizer *)sender{
    
    if (sender.state == UIGestureRecognizerStateBegan)
    {
        _longPress=true;
        
        if (!_isPlaying && !_isPanned)
        {
            CGPoint position;
            position = [sender locationInView:self.renderView];
            _checkTapSelection = true;
            _tapPosition = Vector2(position.x, position.y) * screenScale;
            editorScene->isRTTCompleted = true;
            
        }
        CGPoint position;
        position = [sender locationInView:self.renderView];
        _longPresPosition=CGRectMake(position.x, position.y, 1, 1);
        
        
        
    }

}

- (void) panOrPinchProgress
{
    editorScene->moveMan->panProgress(_touchMovePosition[0], _touchMovePosition[1]);
    editorScene->updater->updateLightCamera();
}


-(void) showPopOver:(int) selectedNodeId{
    if(_longPress){
        if(editorScene->selectedNodeId != NOT_SELECTED){
            if (editorScene->nodes[editorScene->selectedNodeId]->getType() != NODE_CAMERA && editorScene->nodes[editorScene->selectedNodeId]->getType() != NODE_LIGHT && editorScene->nodes[editorScene->selectedNodeId]->getType() != NODE_ADDITIONAL_LIGHT)
            {
                [self.delegate presentPopOver:_longPresPosition];
                _longPress=false;
            }
        
        }
        _longPress=false;
    }
    
    
}

@end