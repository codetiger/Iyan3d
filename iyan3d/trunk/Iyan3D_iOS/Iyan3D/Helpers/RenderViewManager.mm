//
//  RenderViewManager.m
//  Iyan3D
//
//  Created by Karthik on 24/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#import "ZipArchive.h"
#import "RenderViewManager.h"
#import "Logger.h"
#import <OpenGLES/ES2/glext.h>
#import "SGEditorScene.h"
#import "AppHelper.h"
#import "AppDelegate.h"
#import "SceneImporter.h"

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
    smgr = sceneMngr;
}

- (void)setupLayer:(UIView*)renderView
{
    _renderView = (RenderingView*)renderView;
    screenScale = [[AppHelper getAppHelper] userDefaultsBoolForKey:@"ScreenScaleDisable"] ? 1.0 : [[UIScreen mainScreen] scale];
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
    [self check_gl_error];
}
- (void)setupDepthBuffer:(UIView*)renderView
{
    glGenRenderbuffers(1, &_depthRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, _depthRenderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24_OES, renderView.frame.size.width * screenScale, renderView.frame.size.height * screenScale);
    [self check_gl_error];
}
- (void)setupFrameBuffer
{
    glGenFramebuffers(1, &_frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, _frameBuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, _colorRenderBuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _depthRenderBuffer);
    smgr->setFrameBufferObjects(_frameBuffer, _colorRenderBuffer, _depthRenderBuffer);
    [self check_gl_error];
}

-(void) check_gl_error
{
    GLenum err (glGetError());
    
    while(err!=GL_NO_ERROR) {
        string error;
        
        switch(err) {
            case GL_INVALID_OPERATION:      error="INVALID_OPERATION";      break;
            case GL_INVALID_ENUM:           error="INVALID_ENUM";           break;
            case GL_INVALID_VALUE:          error="INVALID_VALUE";          break;
            case GL_OUT_OF_MEMORY:          error="OUT_OF_MEMORY";          break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:  error="INVALID_FRAMEBUFFER_OPERATION";  break;
        }
        
        cerr << "GL_" << error.c_str() <<endl;
        err=glGetError();
    }
}
- (void)presentRenderBuffer
{
    if(_context)
        [_context presentRenderbuffer:_colorRenderBuffer];
}

- (void) setUpCallBacks:(void *)scene
{
    editorScene = (SGEditorScene*)scene;
    if(!editorScene)
        return;
    
    smgr->ShaderCallBackForNode = &shaderCallBackForNode;
    smgr->isTransparentCallBack = &isTransparentCallBack;
//    SceneManager::getTransparencyCB = &getTransparency;
    //    animationScene->downloadMissingAssetCallBack = &downloadMissingAssetCallBack;
    //    animationScene->fileWriteCallBack = &fileWriteCallBack;

}

- (void) setupAutoRigCallBacks
{
    editorScene->rigMan->objLoaderCallBack = &objLoaderCallBack;
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

void shaderCallBackForNode(int nodeID, string matName, int materialIndex, string callbackFuncName)
{
    if(!editorScene)
        return;
        
    if (callbackFuncName.compare("setUniforms") == 0)
        editorScene->shaderCallBackForNode(nodeID, matName, materialIndex);
    else if (callbackFuncName.compare("setJointSpheresUniforms") == 0)
        editorScene->setJointsUniforms(nodeID, matName);
    else if (callbackFuncName.compare("setCtrlUniforms") == 0)
        editorScene->setControlsUniforms(nodeID, matName);
    else if (callbackFuncName.compare("RotationCircle") == 0)
        editorScene->setRotationCircleUniforms(nodeID, matName);
    else if (callbackFuncName.compare("LightCircles") == 0 || callbackFuncName.compare("LightLine") == 0)
        editorScene->setLightLineUniforms(nodeID, matName);
    else if (callbackFuncName.compare("GreenLines") == 0)
        editorScene->setGridLinesUniforms(nodeID, 3, matName);
    else if (callbackFuncName.compare("BlueLines") == 0)
        editorScene->setGridLinesUniforms(nodeID, 2, matName);
    else if (callbackFuncName.compare("RedLines") == 0)
        editorScene->setGridLinesUniforms(nodeID, 1, matName);
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

float getTransparency(int nodeId, string callbackFuncName)
{
    return editorScene->getNodeTransparency(nodeId);
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
        else if (callbackFuncName.compare("RotationCircle") == 0 || callbackFuncName.compare("LightCircles") == 0)
            return false;
        else if (callbackFuncName.compare("GreenLines") == 0)
            return false;
        else if (callbackFuncName.compare("BlueLines") == 0)
            return false;
        else if (callbackFuncName.compare("RedLines") == 0)
            return false;
        else if (callbackFuncName.compare("LightLine") == 0)
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

- (bool) loadNodeInScene:(int)type AssetId:(int)assetId AssetName:(wstring)name TextureName:(NSString*)textureName Width:(int)imgWidth Height:(int)imgHeight isTempNode:(bool)isTempNode More:(NSMutableDictionary*)moreDetail ActionType:(ActionType)assetAddType VertexColor:(Vector4)vertexColor
{
   
    string textureNameStr = std::string([textureName UTF8String]);
    
    if(editorScene) {
        editorScene->loader->removeTempNodeIfExists();
        if(isTempNode)
            editorScene->isPreviewMode = false;
    }
    
    switch (type) {
        case ASSET_CAMERA: {
            editorScene->loader->loadNode(NODE_CAMERA, assetId, "", "", name, 0, 0, assetAddType,vertexColor, "", isTempNode);
            break;
        }
        case ASSET_LIGHT: {
            editorScene->loader->loadNode(NODE_LIGHT, assetId, "", "", name, 0, 0, assetAddType, vertexColor, "", isTempNode);
            break;
        }
        case ASSET_BACKGROUNDS:
        case ASSET_ACCESSORIES: {
            
            string meshPath = FileHelper::getCachesDirectory() + to_string(assetId) + ".sgm";
            Vector3 mColor = Vector3(vertexColor.x, vertexColor.y, vertexColor.z);
            editorScene->loader->removeTempNodeIfExists();
            SceneImporter *loader = new SceneImporter();
            loader->importNodesFromFile(editorScene, ConversionHelper::getStringForWString(name), meshPath, [textureName UTF8String], false, mColor, isTempNode);
            
            if(!isTempNode){
                if(assetAddType != UNDO_ACTION && assetAddType != REDO_ACTION)
                    editorScene->actionMan->storeAddOrRemoveAssetAction(ACTION_NODE_ADDED, assetId);
                [self.delegate updateAssetListInScenes];
            }

            /*
            SGNode* sgNode = editorScene->loader->loadNode(NODE_SGM, assetId, meshPath, textureNameStr, name, 0, 0, assetAddType, vertexColor, "", isTempNode);
            if(sgNode)
                sgNode->isTempNode = isTempNode;
            if(!isTempNode){
                if(assetAddType != UNDO_ACTION && assetAddType != REDO_ACTION)
                    editorScene->actionMan->storeAddOrRemoveAssetAction(ACTION_NODE_ADDED, assetId);
              [self.delegate updateAssetListInScenes];
            }
             */
            break;
        }
        case ASSET_RIGGED: {
            
            string meshPath = FileHelper::getCachesDirectory() + to_string(assetId) + ".sgr";
            Vector3 mColor = Vector3(vertexColor.x, vertexColor.y, vertexColor.z);
            editorScene->loader->removeTempNodeIfExists();
            SceneImporter *loader = new SceneImporter();
            loader->importNodesFromFile(editorScene, ConversionHelper::getStringForWString(name), meshPath, [textureName UTF8String], false, mColor, isTempNode);
            
            if(!isTempNode){
                if(assetAddType != UNDO_ACTION && assetAddType != REDO_ACTION)
                    editorScene->actionMan->storeAddOrRemoveAssetAction(ACTION_NODE_ADDED, assetId);
                [self.delegate updateAssetListInScenes];
            }

            /*
            string meshPath = FileHelper::getCachesDirectory() + to_string(assetId) + ".sgr";
            SGNode* sgNode = editorScene->loader->loadNode(NODE_RIG, assetId, meshPath, textureNameStr, name, 0, 0, assetAddType, vertexColor, "", isTempNode);
            if(sgNode)
                sgNode->isTempNode = isTempNode;
            if(!isTempNode){
                if(assetAddType != UNDO_ACTION && assetAddType != REDO_ACTION)
                    editorScene->actionMan->storeAddOrRemoveAssetAction(ACTION_NODE_ADDED, assetId);
                [self.delegate updateAssetListInScenes];
            }
             */
            
            break;
        }
        case ASSET_OBJ: {
            string meshPath = FileHelper::getDocumentsDirectory() + "Resources/Obj/" + to_string(assetId) + ".obj";
            SGNode* sgNode = editorScene->loader->loadNode(NODE_OBJ, assetId, meshPath, "", name, 0, 0, assetAddType, vertexColor, "", isTempNode);
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
            SGNode* sgNode = editorScene->loader->loadNode(nType, 0, "", "", name, imgWidth, imgHeight, assetAddType, Vector4(imgWidth, imgHeight, 0, 0), "", isTempNode);
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
            float red = [[moreDetail objectForKey:@"red"]floatValue];
            float green = [[moreDetail objectForKey:@"green"]floatValue];
            float blue = [[moreDetail objectForKey:@"blue"]floatValue];
            float alpha = [[moreDetail objectForKey:@"alpha"]floatValue];
            NSString* fontFileName = [moreDetail objectForKey:@"fontFileName"];
            Vector4 textColor = Vector4(red,green,blue,alpha);
            NODE_TYPE nodeType = (type == ASSET_TEXT) ? NODE_TEXT : NODE_TEXT_SKIN;
            
            std::string fontFilePath = FileHelper::getCachesDirectory() + [fontFileName UTF8String];
            
            SceneImporter * loader = new SceneImporter();
            loader->import3DText(editorScene, name, fontFilePath, 4, 4, imgHeight / 50.0f, 4, (type == ASSET_TEXT_RIG), isTempNode);
            
            if(!isTempNode){
                if(assetAddType != UNDO_ACTION && assetAddType != REDO_ACTION)
                    editorScene->actionMan->storeAddOrRemoveAssetAction(ACTION_NODE_ADDED, assetId);
                [self.delegate updateAssetListInScenes];
            }

            /*
            SGNode* textNode = editorScene->loader->loadNode(nodeType, 0, "", textureNameStr,name, imgWidth, imgHeight, assetAddType, textColor, [fontFilePath UTF8String],isTempNode);
            if (textNode == NULL) {
                UIAlertView* loadNodeAlert = [[UIAlertView alloc] initWithTitle:NSLocalizedString(@"Information", nil) message:NSLocalizedString(@"fontstyle_char_unsupported", nil) delegate:self cancelButtonTitle:NSLocalizedString(@"Ok", nil) otherButtonTitles:nil];
                [loadNodeAlert show];
                [self.delegate showOrHideProgress:0];
                if(editorScene && editorScene->loader)
                    editorScene->loader->removeTempNodeIfExists();
                return false;
            }
            if(textNode)
                textNode->isTempNode = isTempNode;
            if(!isTempNode){
                if(assetAddType != UNDO_ACTION && assetAddType != REDO_ACTION)
                    editorScene->actionMan->storeAddOrRemoveAssetAction(ACTION_TEXT_IMAGE_ADD, assetId);
                [self.delegate updateAssetListInScenes];
            }
             */
            
            break;
        }
        case ASSET_ADDITIONAL_LIGHT: {
            //TODO enum for max lights
            if(ShaderManager::lightPosition.size() < 5) {
                editorScene->loader->loadNode(NODE_ADDITIONAL_LIGHT, assetId, "",  "", name, imgWidth, imgHeight, assetAddType, Vector4(1.0), "", isTempNode);
                if(assetAddType != UNDO_ACTION && assetAddType != REDO_ACTION){
                    editorScene->actionMan->storeAddOrRemoveAssetAction(ACTION_NODE_ADDED, assetId , "Light"+ to_string(assetId-ASSET_ADDITIONAL_LIGHT));

                }
                [self.delegate updateAssetListInScenes];
            } else {
                NSLog(@"Max lights 5");
            }            
            break;
        }
        case ASSET_PARTICLES: {
           SGNode* particle =  editorScene->loader->loadNode(NODE_PARTICLES, assetId, "",  "", name, imgWidth, imgHeight, assetAddType, Vector4(1.0), "", isTempNode);
            if(particle)
                particle->isTempNode = isTempNode;
            if(!isTempNode){
                if(assetAddType != UNDO_ACTION && assetAddType != REDO_ACTION)
                    editorScene->actionMan->storeAddOrRemoveAssetAction(ACTION_NODE_ADDED, assetId);
                [self.delegate updateAssetListInScenes];
            }
        }
        default: {
            break;
        }
    }
    [self.delegate showOrHideProgress:0];
    return true;
}

- (bool) removeNodeFromScene:(int)nodeIndex IsUndoOrRedo:(BOOL)isUndoOrRedo
{
    if(editorScene->nodes.size()>nodeIndex){
        if(editorScene->isNodeInSelection(editorScene->nodes[nodeIndex]))
            editorScene->selectMan->multipleSelections(nodeIndex);
        if(editorScene->isNodeSelected && editorScene->selectedNodeId != nodeIndex)
            editorScene->selectMan->unselectObject(editorScene->selectedNodeId);
        editorScene->selectedNodeId = nodeIndex;
        if(!isUndoOrRedo){
            if(editorScene->nodes[nodeIndex]->getType() == NODE_TEXT_SKIN || editorScene->nodes[nodeIndex]->getType() == NODE_TEXT || editorScene->nodes[nodeIndex]->getType() == NODE_IMAGE || editorScene->nodes[nodeIndex]->getType() == NODE_VIDEO)
                editorScene->actionMan->storeAddOrRemoveAssetAction(ACTION_TEXT_IMAGE_DELETE, 0);
            else if (editorScene->nodes[nodeIndex]->getType() == NODE_OBJ || editorScene->nodes[nodeIndex]->getType() == NODE_SGM || editorScene->nodes[nodeIndex]->getType() == NODE_RIG || editorScene->nodes[nodeIndex]->getType() == NODE_ADDITIONAL_LIGHT|| editorScene->nodes[nodeIndex]->getType() == NODE_PARTICLES)
                editorScene->actionMan->storeAddOrRemoveAssetAction(ACTION_NODE_DELETED, 0);
        }
        [self performSelectorOnMainThread:@selector(removeNodeInMainThread:) withObject:[NSNumber numberWithInt:nodeIndex] waitUntilDone:YES];
    }
}

- (void) removeNodeInMainThread:(NSNumber*) indNum
{
    int nodeIndex = [indNum intValue];
    editorScene->loader->removeObject(nodeIndex);
    [self.delegate updateAssetListInScenes];
    [self.delegate showOrHideProgress:0];

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
    
    [self.renderView addGestureRecognizer:tapRecognizer];
    [self.renderView addGestureRecognizer:panRecognizer];
    [self.renderView addGestureRecognizer:longpressRecogniser];
}

- (void)tapGesture:(UITapGestureRecognizer*)rec
{
//    isTapped = true;
    CGPoint position;
    position = [rec locationInView:self.renderView];
    
    [[AppHelper getAppHelper] toggleHelp:nil Enable:NO];
    
    if(editorScene->renHelper->isMovingCameraPreview(Vector2(position.x, position.y) * screenScale)){
        editorScene->setLightingOn();
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
            if(editorScene) {
                ShaderManager::shadowsOff = true;
                editorScene->setLightingOff();
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
            ShaderManager::shadowsOff = false;
            editorScene->moveMan->touchEnd(p[0] * screenScale);
            [self.delegate reloadFrames];
            break;
        }
    }
    editorScene->updater->updateControlsOrientaion();
    if (!_isPlaying) {
        Vector3 trans = editorScene->getTransformValue();
        [self.delegate updateXYZValuesHide:NO X:trans.x Y:trans.y Z:trans.z];
        [self.delegate undoRedoButtonState:DEACTIVATE_BOTH];
    }
}

- (void)handleLongPressGestures:(UILongPressGestureRecognizer *)sender
{
    if(editorScene && editorScene->isRigMode)
        return;
    
    if (sender.state == UIGestureRecognizerStateBegan)
    {
        _longPress=true;
        CGPoint position;
        position = [sender locationInView:self.renderView];
        _longPresPosition=CGRectMake(position.x, position.y, 1, 1);

        if (!_isPlaying && !_isPanned)
        {

            if(editorScene->selectedNodeIds.size() > 0) {
                if(editorScene->allNodesRemovable() || editorScene->allNodesClonable())
                    [self.delegate presentPopOver:_longPresPosition];
                _longPress=false;
            } else {
                CGPoint position;
                position = [sender locationInView:self.renderView];
                _checkTapSelection = true;
                _tapPosition = Vector2(position.x, position.y) * screenScale;
                editorScene->setLightingOn();
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

#pragma i3d file creation methods

-(NSMutableArray*) getFileteredFilePathsFrom:(NSMutableArray*) filePaths
{
    NSMutableArray * filtFilePaths = [[NSMutableArray alloc] init];
    NSString *docDirPath = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0];
    NSString *cacheDirPath = [NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES) objectAtIndex:0];
    NSString *objDirPath = [docDirPath stringByAppendingPathComponent:@"Resources/Objs"];
    NSString *rigDirPath = [docDirPath stringByAppendingPathComponent:@"Resources/Rigs"];
    NSString *texDirPath = [docDirPath stringByAppendingPathComponent:@"Resources/Textures"];
    NSString *sgmDirPath = [docDirPath stringByAppendingPathComponent:@"Resources/Sgm"];
    NSString *vidDirPath = [docDirPath stringByAppendingPathComponent:@"Resources/Videos"];
    
    NSFileManager * fm = [NSFileManager defaultManager];
    
    for(int i = 0; i < [filePaths count]; i++) {
        if([[filePaths objectAtIndex:i] isEqualToString:@""])
            continue;
        NSString *filePath1 = [NSString stringWithFormat:@"%@/%@",cacheDirPath,[filePaths objectAtIndex:i]];
        NSString *filePath2 = [NSString stringWithFormat:@"%@/%@",objDirPath,[filePaths objectAtIndex:i]];
        NSString *filePath3 = [NSString stringWithFormat:@"%@/%@",rigDirPath,[filePaths objectAtIndex:i]];
        NSString *filePath4 = [NSString stringWithFormat:@"%@/%@",texDirPath,[filePaths objectAtIndex:i]];
        NSString *filePath5 = [NSString stringWithFormat:@"%@/%@", sgmDirPath, [filePaths objectAtIndex:i]];
        NSString *filePath6 = [NSString stringWithFormat:@"%@/%@", vidDirPath, [filePaths objectAtIndex:i]];
        NSString *filePath7 = [NSString stringWithFormat:@"%@/%@", docDirPath, [filePaths objectAtIndex:i]];
        NSString *filePath8 = [NSString stringWithFormat:@"%s/%@",constants::BundlePath.c_str(), [filePaths objectAtIndex:i]];
        
        
        if([fm fileExistsAtPath:filePath1]) {
            if(![filtFilePaths containsObject:filePath1])
                [filtFilePaths addObject:filePath1];
        } else if([fm fileExistsAtPath:filePath2]){
            if(![filtFilePaths containsObject:filePath2])
                [filtFilePaths addObject:filePath2];
        } else if([fm fileExistsAtPath:filePath3]){
            if(![filtFilePaths containsObject:filePath3])
                [filtFilePaths addObject:filePath3];
        } else if([fm fileExistsAtPath:filePath4]){
            if(![filtFilePaths containsObject:filePath4])
                [filtFilePaths addObject:filePath4];
        } else if([fm fileExistsAtPath:filePath5]){
            if(![filtFilePaths containsObject:filePath5])
                [filtFilePaths addObject:filePath5];
        } else if([fm fileExistsAtPath:filePath6]){
            if(![filtFilePaths containsObject:filePath6])
                [filtFilePaths addObject:filePath6];
        } else if([fm fileExistsAtPath:filePath7]){
            if(![filtFilePaths containsObject:filePath7])
                [filtFilePaths addObject:filePath7];
        } else if([fm fileExistsAtPath:filePath8]){
            if(![filtFilePaths containsObject:filePath8])
                [filtFilePaths addObject:filePath8];
        }
    }
    return filtFilePaths;
}

- (NSMutableArray*) getFileNamesFromScene:(bool) forBackup
{
    if(!editorScene)
        return nil;
    
    NSMutableArray *fileNamesToZip = [[NSMutableArray alloc] init];
    vector<string> textureFileNames = editorScene->getUserFileNames(forBackup);
    
    for(int i = 0; i < textureFileNames.size(); i++) {
        [fileNamesToZip addObject:[NSString stringWithCString:textureFileNames[i].c_str() encoding:NSUTF8StringEncoding]];
    }

    return fileNamesToZip;
}

- (void) createi3dFileWithThumb:(NSString*) thumbPath
{
    NSMutableArray* fileNames = [self getFileNamesFromScene:true];
    NSString *docDirPath = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0];
    NSString* sgbFilePath = [self.delegate getSGBPath];
    NSString* sgbName = [[sgbFilePath lastPathComponent] stringByDeletingPathExtension];

    NSString* zipfile = [docDirPath stringByAppendingPathComponent:[NSString stringWithFormat:@"Projects/%@.i3d", sgbName]];
    NSMutableArray *userFiles = [self getFileteredFilePathsFrom:fileNames];
    [userFiles addObject:thumbPath];
    ZipArchive* zip = [[ZipArchive alloc] init];
    BOOL ret = [zip CreateZipFile2:zipfile];
    
    if([[NSFileManager defaultManager] fileExistsAtPath:sgbFilePath])
        ret = [zip addFileToZip:sgbFilePath newname:[sgbFilePath lastPathComponent]];
    
    for(int i = 0; i < [userFiles count]; i++)
        ret = [zip addFileToZip:[userFiles objectAtIndex:i] newname:[[userFiles objectAtIndex:i] lastPathComponent]];
    
    if( ![zip CloseZipFile2] )
        zipfile = @"";
}

@end