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
    [self loadNodeInScene:ASSET_CAMERA AssetId:0 AssetName:ConversionHelper::getWStringForString("CAMERA") Width:0 Height:0];
    [self loadNodeInScene:ASSET_LIGHT AssetId:0 AssetName:ConversionHelper::getWStringForString("LIGHT") Width:0 Height:0];
    
}

- (bool)loadNodeInScene:(int)type AssetId:(int)assetId AssetName:(wstring)name Width:(int)imgWidth Height:(int)imgHeight
{
    ActionType assetAddType = IMPORT_ASSET_ACTION;
    
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
            editorScene->loader->loadNode(NODE_SGM, assetId, name, 0, 0, assetAddType);
            break;
        }
        case ASSET_RIGGED: {
//            [self showTipsViewForAction:OBJECT_IMPORTED_HUMAN];
            editorScene->loader->loadNode(NODE_RIG, assetId, name, 0, 0, assetAddType);
            break;
        }
        case ASSET_OBJ: {
//            [self showTipsViewForAction:OBJECT_IMPORTED];
            editorScene->loader->loadNode(NODE_OBJ, assetId, name, 0, 0, assetAddType);
            break;
        }
        case ASSET_IMAGE: {
//            [self showTipsViewForAction:OBJECT_IMPORTED];
            editorScene->loader->loadNode(NODE_IMAGE, 0, name, imgWidth, imgHeight, assetAddType,Vector4(imgWidth,imgHeight,0,0));
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
    
}

- (void)addGesturesToSceneView
{
    UIPanGestureRecognizer* panRecognizer = [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(panGesture:)];
    panRecognizer.delegate = self;
    panRecognizer.minimumNumberOfTouches = 1;
    panRecognizer.maximumNumberOfTouches = 2;
    UITapGestureRecognizer* tapRecognizer = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(tapGesture:)];
    tapRecognizer.delegate = self;
    if (![self.delegate isMetalSupportedDevice]) {
        [self.renderView addGestureRecognizer:panRecognizer];
        [self.renderView addGestureRecognizer:tapRecognizer];
    }
    else {
        [self.renderView addGestureRecognizer:panRecognizer];
        [self.renderView addGestureRecognizer:tapRecognizer];
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
                    _touchBeganPosition = p[0] * screenScale;
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
                editorScene->moveMan->panBegan(p[0] * screenScale, p[1] * screenScale);
            }
            switch (touchCount) {
                case 1: {
                    //animationScene->touchMove(p[0] * screenScale, p[1] * screenScale, SCREENWIDTH * screenScale, SCREENHEIGHT * screenScale);
                    editorScene->moveMan->swipeProgress(-velocity.x / 50.0, -velocity.y / 50.0);
                    break;
                }
                case 2: {
                    editorScene->moveMan->panProgress(p[0] * screenScale, p[1] * screenScale);
                    editorScene->updater->updateLightCamera();
                    break;
                }
                default:
                    break;
            }
            break;
        }
        default: {
//            if (editorScene->isControlSelected && editorScene->selectedNodeId != NOT_SELECTED && editorScene->nodes[editorScene->selectedNodeId]->getType() == NODE_RIG) {
//                if (editorScene->isJointSelected)
//                    [self showTipsViewForAction:JOINT_MOVED];
//                else
//                    [self showTipsViewForAction:OBJECT_RIG_MOVED];
//            }
//            else if (animationScene->isControlSelected)
//                [self showTipsViewForAction:OBJECT_MOVED];
            
            editorScene->moveMan->touchEnd(p[0] * screenScale);
            [self.delegate reloadFrames];
            break;
        }
    }
    editorScene->updater->updateControlsOrientaion();
    if (editorScene->actions.size() > 0 && editorScene->currentAction > 0 && !_isPlaying) {
        //[self.undoButton setEnabled:YES];
    }
}

@end