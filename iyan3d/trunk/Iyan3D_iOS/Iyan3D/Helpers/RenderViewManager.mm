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
#import "AppHelper.h"
#import "AppDelegate.h"

#define UNDEFINED_OBJECT -1
#define ASSET_RIGGED 1
#define ASSET_BACKGROUNDS 2
#define ASSET_ACCESSORIES 3
#define ASSET_ANIMATION 4
#define ASSET_OBJ 6
#define ASSET_CAMERA 7
#define ASSET_LIGHT 8
#define ASSET_IMAGE 9
#define ASSET_TEXT_RIG 10
#define ASSET_TEXT 11
#define ASSET_VIDEO 12
#define ASSET_PARTICLES 13
#define ASSET_ADDITIONAL_LIGHT 900
#define ADD_OBJECT 100
#define DELETE_OBJECT 200

@implementation RenderViewManager

SGEditorScene *editorScene;

- (void)setUpPaths:(SceneManager*)sceneMngr
{
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
    NSString* cachesDir = [paths objectAtIndex:0];
    constants::CachesStoragePath = (char*)[cachesDir cStringUsingEncoding:NSASCIIStringEncoding];
    touchCountTracker = 0;
    lightCount = 1;
    smgr = sceneMngr;
}

- (void)setupLayer:(UIView*)renderView
{
    _renderView = (RenderingView*)renderView;
    screenScale = [[UIScreen mainScreen] scale];
    if(![self.delegate isMetalSupportedDevice]) {
        _eaglLayer = (CAEAGLLayer*)renderView.layer;
        _eaglLayer.opaque = YES;
        _eaglLayer.contentsScale = screenScale;
    }
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
- (void)setupFrameBuffer
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

- (void) setupAutoRigCallBacks
{
    //editorScene->rigMan->boneLimitsCallBack = &boneLimitsCallBack;
    editorScene->rigMan->objLoaderCallBack = &objLoaderCallBack;
    //editorScene->rigMan->notRespondingCallBack = &notRespondingCallBackAutoRigScene;
}


void objLoaderCallBack(int status)
{
    
    /* TODO show alert
    AppDelegate *appDelegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
    AutoRigViewController *autoRigVC = (AutoRigViewController*)[[appDelegate window] rootViewController];
    if(status == OBJ_CROSSED_MAX_VERTICES_LIMIT){
        [autoRigVC.view endEditing:YES];
        UIAlertView *objLoadMsg = [[UIAlertView alloc]initWithTitle:@"Information" message:@"The obj file is too heavy. Maximum vertices limit is 2.7 Millions." delegate:autoRigVC cancelButtonTitle:@"OK" otherButtonTitles:nil];
        [objLoadMsg show];
    }else if(status == OBJ_NOT_LOADED){
        UIAlertView *objLoadMsg = [[UIAlertView alloc]initWithTitle:@"Information" message:@"Please load any obj file to rig." delegate:autoRigVC cancelButtonTitle:@"OK" otherButtonTitles:nil];
        [objLoadMsg show];
    }else{
        [autoRigVC.view endEditing:YES];
        UIAlertView *objLoadMsg = [[UIAlertView alloc]initWithTitle:@"Information" message:@"Obj file corrupted please try another file." delegate:autoRigVC cancelButtonTitle:@"OK" otherButtonTitles:nil];
        [objLoadMsg show];
    }
     */
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
    else if(callbackFuncName.compare("ObjUniforms") == 0){
        editorScene->rigMan->objNodeCallBack(matName);
    }else if(callbackFuncName.compare("jointUniforms") == 0){
        editorScene->rigMan->jointNodeCallBack(nodeID,matName);
    }else if(callbackFuncName.compare("BoneUniforms") == 0){
        editorScene->rigMan-> boneNodeCallBack(nodeID,matName);
    }else if(callbackFuncName.compare("envelopeUniforms") == 0){
        editorScene->rigMan->setEnvelopeUniforms(nodeID,matName);
    }else if(callbackFuncName.compare("sgrUniforms") == 0){
        editorScene->rigMan->setSGRUniforms(nodeID,matName);
    }
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
        else if(callbackFuncName.compare("ObjUniforms") == 0)
            return editorScene->rigMan->isOBJTransparent(callbackFuncName);
        else if(callbackFuncName.compare("jointUniforms") == 0)
            return false;
        else if(callbackFuncName.compare("BoneUniforms") == 0)
            return false;
        else if(callbackFuncName.compare("envelopeUniforms") == 0)
            return false;
        else if(callbackFuncName.compare("sgrUniforms") == 0)
            return editorScene->rigMan->isSGRTransparent(nodeId,callbackFuncName);

}

- (void) addCameraLight
{
    [self loadNodeInScene:ASSET_CAMERA AssetId:0 AssetName:ConversionHelper::getWStringForString("CAMERA") TextureName:(@"") Width:0 Height:0 isTempNode:false More:nil ActionType:IMPORT_ASSET_ACTION VertexColor:Vector4(0)];
    [self.delegate updateAssetListInScenes];
    [self loadNodeInScene:ASSET_LIGHT AssetId:0 AssetName:ConversionHelper::getWStringForString("LIGHT") TextureName:(@"") Width:0 Height:0 isTempNode:false More:nil ActionType:IMPORT_ASSET_ACTION VertexColor:Vector4(0)];
    [self.delegate updateAssetListInScenes];
    
}

- (bool)loadNodeInScene:(int)type AssetId:(int)assetId AssetName:(wstring)name TextureName:(NSString*)textureName Width:(int)imgWidth Height:(int)imgHeight isTempNode:(bool)isTempNode More:(NSMutableDictionary*)moreDetail ActionType:(ActionType)assetAddType VertexColor:(Vector4)vertexColor
{
    // TODO a lot to implement
   
    NSLog(@"Texture Name : %@ " , textureName);
    string textureNameStr = *new std::string([textureName UTF8String]);
    
    if(editorScene) {
        editorScene->loader->removeTempNodeIfExists();
        if(isTempNode)
            editorScene->isPreviewMode = false;
    }
    
    switch (type) {
        case ASSET_CAMERA: {
            editorScene->loader->loadNode(NODE_CAMERA,assetId ,"",name, 0, 0, assetAddType,vertexColor,"",isTempNode);
            break;
        }
        case ASSET_LIGHT: {
            editorScene->loader->loadNode(NODE_LIGHT, assetId,"",name, 0, 0, assetAddType,vertexColor,"",isTempNode);
            break;
        }
        case ASSET_BACKGROUNDS:
        case ASSET_ACCESSORIES: {
//            [self showTipsViewForAction:OBJECT_IMPORTED];
            SGNode* sgNode = editorScene->loader->loadNode(NODE_SGM, assetId,textureNameStr ,name, 0, 0, assetAddType,vertexColor,"",isTempNode);
            if(sgNode)
                sgNode->isTempNode = isTempNode;
            if(!isTempNode){
                if(assetAddType != UNDO_ACTION && assetAddType != REDO_ACTION)
                    editorScene->actionMan->storeAddOrRemoveAssetAction(ACTION_NODE_ADDED, assetId);
              [self.delegate updateAssetListInScenes];
            }
            break;
        }
        case ASSET_RIGGED: {
//            [self showTipsViewForAction:OBJECT_IMPORTED_HUMAN];
            SGNode* sgNode = editorScene->loader->loadNode(NODE_RIG, assetId,textureNameStr,name, 0, 0, assetAddType,vertexColor,"",isTempNode);
            if(sgNode)
                sgNode->isTempNode = isTempNode;
            if(!isTempNode){
                if(assetAddType != UNDO_ACTION && assetAddType != REDO_ACTION)
                    editorScene->actionMan->storeAddOrRemoveAssetAction(ACTION_NODE_ADDED, assetId);
                [self.delegate updateAssetListInScenes];
            }
            break;
        }
        case ASSET_OBJ: {
//            [self showTipsViewForAction:OBJECT_IMPORTED];
            SGNode* sgNode = editorScene->loader->loadNode(NODE_OBJ, assetId,"",name, 0, 0, assetAddType,vertexColor,"",isTempNode);
            if(sgNode)
                sgNode->isTempNode = isTempNode;
            if(!isTempNode){
                [self.delegate updateAssetListInScenes];
                                editorScene->actionMan->storeAddOrRemoveAssetAction(ACTION_NODE_ADDED, assetId);
            }
            break;
        }
        case ASSET_VIDEO:
        case ASSET_IMAGE: {
            NODE_TYPE nType = (type == ASSET_VIDEO) ? NODE_VIDEO : NODE_IMAGE;
//            [self showTipsViewForAction:OBJECT_IMPORTED];
            SGNode* sgNode = editorScene->loader->loadNode(nType, 0,"",name, imgWidth, imgHeight, assetAddType,Vector4(imgWidth,imgHeight,0,0),"",isTempNode);
            if(sgNode)
                sgNode->isTempNode = isTempNode;
            if(!isTempNode){
                if(assetAddType != UNDO_ACTION && assetAddType != REDO_ACTION)
                    editorScene->actionMan->storeAddOrRemoveAssetAction(ACTION_TEXT_IMAGE_ADD, assetId);
                [self.delegate updateAssetListInScenes];
            }
            break;
        }
        case ASSET_ANIMATION: {
            
            NSArray* paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
            NSString* documentsDirectory = [paths objectAtIndex:0];
            NSString* filePath = [NSString stringWithFormat:@"%@/%d.sga", documentsDirectory, assetId];
            std::string* sgaFilePath = new std::string([filePath UTF8String]);
            editorScene->animMan->applyAnimations(*sgaFilePath, editorScene->selectedNodeId);
            delete sgaFilePath;
            //animationScene->storeAddOrRemoveAssetAction(ACTION_APPLY_ANIM, 0);
            //[self.frameCarouselView reloadData];
            //[self showTipsViewForAction:ANIMATION_APPLIED];
            //isFileSaved = !(animationScene->changeAction = true);
            
            break;
        }
        case ASSET_TEXT:
        case ASSET_TEXT_RIG: {
//            [self showTipsViewForAction:OBJECT_IMPORTED];
            float red = [[moreDetail objectForKey:@"red"]floatValue];
            float green = [[moreDetail objectForKey:@"green"]floatValue];
            float blue = [[moreDetail objectForKey:@"blue"]floatValue];
            float alpha = [[moreDetail objectForKey:@"alpha"]floatValue];
            NSString* fontFilePath = [moreDetail objectForKey:@"fontFileName"];
            Vector4 textColor = Vector4(red,green,blue,alpha);
            NODE_TYPE nodeType = (type == ASSET_TEXT) ? NODE_TEXT : NODE_TEXT_SKIN;
            SGNode* textNode = editorScene->loader->loadNode(nodeType, 0,"",name, imgWidth, imgHeight, assetAddType, textColor, [fontFilePath UTF8String],isTempNode);
            if (textNode == NULL) {
                UIAlertView* loadNodeAlert = [[UIAlertView alloc] initWithTitle:@"Information" message:@"The font style you chose does not support the characters you entered." delegate:self cancelButtonTitle:@"Ok" otherButtonTitles:nil];
                [loadNodeAlert show];
                return false;
            }
            if(textNode)
                textNode->isTempNode = isTempNode;
            if(!isTempNode){
                if(assetAddType != UNDO_ACTION && assetAddType != REDO_ACTION)
                    editorScene->actionMan->storeAddOrRemoveAssetAction(ACTION_TEXT_IMAGE_ADD, assetId);
                [self.delegate updateAssetListInScenes];
            }
            break;
        }
        case ASSET_ADDITIONAL_LIGHT: {
            //TODO enum for max lights
            if(ShaderManager::lightPosition.size() < 5) {
                editorScene->loader->loadNode(NODE_ADDITIONAL_LIGHT, assetId ,"",name, imgWidth , imgHeight , assetAddType , Vector4(1.0),"",isTempNode);
                if(assetAddType != UNDO_ACTION && assetAddType != REDO_ACTION)
                    editorScene->actionMan->storeAddOrRemoveAssetAction(ACTION_NODE_ADDED, ASSET_ADDITIONAL_LIGHT + lightCount , "Light"+ to_string(lightCount));
                [self.delegate updateAssetListInScenes];
                lightCount++;
            } else {
                NSLog(@"Max lights 5");
            }            
            break;
        }
        case ASSET_PARTICLES: {
            editorScene->loader->loadNode(NODE_PARTICLES, assetId, "");
        }
        default: {
            break;
        }
    }
    return true;
}

- (bool) removeNodeFromScene:(int)nodeIndex IsUndoOrRedo:(BOOL)isUndoOrRedo
{
    if(editorScene->nodes.size()>nodeIndex){
        if(editorScene->isNodeInSelection(editorScene->nodes[nodeIndex]))
            editorScene->selectMan->multipleSelections(nodeIndex);
        editorScene->selectedNodeId = nodeIndex;
        if(!isUndoOrRedo){
            if(editorScene->nodes[nodeIndex]->getType() == NODE_TEXT_SKIN || editorScene->nodes[nodeIndex]->getType() == NODE_TEXT || editorScene->nodes[nodeIndex]->getType() == NODE_IMAGE)
                editorScene->actionMan->storeAddOrRemoveAssetAction(ACTION_TEXT_IMAGE_DELETE, 0);
            else if (editorScene->nodes[nodeIndex]->getType() == NODE_OBJ || editorScene->nodes[nodeIndex]->getType() == NODE_SGM || editorScene->nodes[nodeIndex]->getType() == NODE_RIG || editorScene->nodes[nodeIndex]->getType() == NODE_ADDITIONAL_LIGHT)
                editorScene->actionMan->storeAddOrRemoveAssetAction(ACTION_NODE_DELETED, 0);
        }
        editorScene->loader->removeObject(nodeIndex);
        [self.delegate updateAssetListInScenes];
    }
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
//    isTapped = true;
    CGPoint position;
    position = [rec locationInView:self.renderView];
    
    if(editorScene->renHelper->isMovingCameraPreview(Vector2(position.x, position.y) * screenScale)){
        editorScene->camPreviewScale = (editorScene->camPreviewScale == 1.0) ? 2.0 : 1.0;
        return;
    }
    if (!editorScene->isPlaying && !_isPanned) {
        _checkTapSelection = true;
        _tapPosition = Vector2(position.x, position.y) * screenScale;
        editorScene->isRTTCompleted = true;
        
    }
    if(editorScene->isPlaying){
        [self.delegate stopPlaying];
        return;
    }
    [self.delegate undoRedoButtonState:DEACTIVATE_BOTH];
    editorScene->setLightingOn();
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
            touchCountTracker = (int)touchCount;
            editorScene->moveMan->touchBegan(p[0] * screenScale);
            switch (touchCount) {
                case 1: {
                    if(!editorScene->renHelper->isMovingPreview) {
                    _checkCtrlSelection = true;
                    _touchMovePosition.clear();
                    _touchMovePosition.push_back(p[0] * screenScale);
                    editorScene->moveMan->swipeProgress(-velocity.x / 50.0, -velocity.y / 50.0);
                    }
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
            if (touchCountTracker != touchCount) {
                if(editorScene->isControlSelected)
                    return;
                _makePanOrPinch = false;
                editorScene->moveMan->panBegan(p[0] * screenScale, p[1] * screenScale);
                touchCountTracker = (int)touchCount;
            }
            switch (touchCount) {
                case 1: {
                    if(editorScene->isRigMode)
                        editorScene->moveMan->touchMoveRig(p[0] * screenScale, p[1] * screenScale, SCREENWIDTH * screenScale, SCREENHEIGHT * screenScale);
                    else
                        editorScene->moveMan->touchMove(p[0] * screenScale, p[1] * screenScale, SCREENWIDTH * screenScale, SCREENHEIGHT * screenScale);
                    if(!editorScene->renHelper->isMovingPreview)
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
    if (!_isPlaying) {
        [self.delegate undoRedoButtonState:DEACTIVATE_BOTH];
    }
}

- (void)handleLongPressGestures:(UILongPressGestureRecognizer *)sender{
    if (sender.state == UIGestureRecognizerStateBegan)
    {
        _longPress=true;
        CGPoint position;
        position = [sender locationInView:self.renderView];
        _longPresPosition=CGRectMake(position.x, position.y, 1, 1);

        if (!_isPlaying && !_isPanned)
        {

            if(editorScene->selectedNodeIds.size() > 0) {
                if(editorScene->allNodesRemovable())
                    [self.delegate presentPopOver:_longPresPosition];
                _longPress=false;
            } else {
                CGPoint position;
                position = [sender locationInView:self.renderView];
                _checkTapSelection = true;
                _tapPosition = Vector2(position.x, position.y) * screenScale;
                editorScene->isRTTCompleted = true;
            }
            
        }
        
    }
}

- (void) panOrPinchProgress
{
    editorScene->moveMan->panProgress(_touchMovePosition[0], _touchMovePosition[1]);
    editorScene->updater->updateLightCamera();
}

-(void) showPopOver:(int) selectedNodeId{
    if(_longPress){
        
        [self.delegate showOptions:_longPresPosition];
        _longPress=false;
    }
}

@end