//
//  SGEditorScene.cpp
//  Iyan3D
//
//  Created by Karthik on 22/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#include "SGEditorScene.h"

SGEditorScene::SGEditorScene(DEVICE_TYPE device,SceneManager *smgr,int screenWidth,int screenHeight)
{
    SceneHelper::screenWidth = screenWidth;
    SceneHelper::screenHeight = screenHeight;
    this->smgr = smgr;
    viewCamera =  SceneHelper::initViewCamera(smgr, cameraTarget, cameraRadius);

    initVariables(smgr, device);
    initTextures();
    
    jointSphereMesh = CSGRMeshFileLoader::createSGMMesh(constants::BundlePath + "/sphere.sgm",smgr->device);
    rotationCircle = SceneHelper::createCircle(smgr);
    BoneLimitsHelper::init();
    AutoRigJointsDataHelper::getTPoseJointsData(tPoseJoints);
    sceneControls = SceneHelper::initControls(smgr);
    renderCamera = SceneHelper::initRenderCamera(smgr, cameraFOV);
}

SGEditorScene::~SGEditorScene()
{
    
}

void SGEditorScene::initVariables(SceneManager* sceneMngr, DEVICE_TYPE devType)
{
    cmgr = new CollisionManager();
    shaderMGR = new ShaderManager(sceneMngr, devType);

    renHelper = new RenderHelper(sceneMngr);
    isJointSelected = isNodeSelected = isControlSelected = freezeRendering = isPlaying = false;
    selectedNodeId = selectedJointId = NOT_EXISTS;
    selectedNode = NULL;
    selectedJoint = NULL;
    jointSpheres.clear();
    tPoseJoints.clear();
    controlType = MOVE;
    selectedControlId = NOT_EXISTS;
    selectedNode = NULL;
    controlsPlane = new Plane3D();
    objLoader = new OBJMeshFileLoader;
    mirrorSwitchState = MIRROR_OFF;
    nodes.clear();
    actions.clear();
    totalFrames = 24;
    cameraFOV = 72.0;
    cameraResolutionType = 0;
}

void SGEditorScene::initTextures()
{
    bgTexture = smgr->loadTexture("bgtex",constants::BundlePath +  "/bgImageforall.png",TEXTURE_RGBA8,TEXTURE_BYTE);
    touchTexture = smgr->createRenderTargetTexture("TouchTexture", TEXTURE_RGBA8, TEXTURE_BYTE, TOUCH_TEXTURE_WIDTH, TOUCH_TEXTURE_HEIGHT);
    watermarkTexture = smgr->loadTexture("waterMarkTexture",constants::BundlePath + "/watermark.png",TEXTURE_RGBA8,TEXTURE_BYTE);

    previewTexture = smgr->createRenderTargetTexture("previewTexture", TEXTURE_RGBA8, TEXTURE_BYTE, PREVIEW_TEXTURE_WIDTH, PREVIEW_TEXTURE_HEIGHT);
    thumbnailTexture = smgr->createRenderTargetTexture("thumbnailTexture", TEXTURE_RGBA8, TEXTURE_BYTE, THUMBNAIL_TEXTURE_WIDTH, THUMBNAIL_TEXTURE_HEIGHT);
    shadowTexture = smgr->createRenderTargetTexture("shadowTexture", TEXTURE_DEPTH32, TEXTURE_BYTE, SHADOW_TEXTURE_WIDTH, SHADOW_TEXTURE_HEIGHT);
    whiteBorderTexture = smgr->loadTexture("whiteborder",constants::BundlePath + "/whiteborder.png",TEXTURE_RGBA8,TEXTURE_BYTE);
    
    renderingTextureMap[RESOLUTION[0][0]] = smgr->createRenderTargetTexture("RenderTexture", TEXTURE_RGBA8, TEXTURE_BYTE,RESOLUTION[0][0] , RESOLUTION[0][1]);
    renderingTextureMap[RESOLUTION[1][0]] = smgr->createRenderTargetTexture("RenderTexture", TEXTURE_RGBA8, TEXTURE_BYTE,RESOLUTION[1][0] , RESOLUTION[1][1]);
    renderingTextureMap[RESOLUTION[2][0]] = smgr->createRenderTargetTexture("RenderTexture", TEXTURE_RGBA8, TEXTURE_BYTE,RESOLUTION[2][0] , RESOLUTION[2][1]);

}

void SGEditorScene::renderAll()
{
    if(freezeRendering)
        return;
    
    bool displayPrepared = smgr->PrepareDisplay(SceneHelper::screenWidth, SceneHelper::screenHeight, true, true, false,
                                                Vector4(0, 0, 0, 255));
    
    if(displayPrepared) {
        rotationCircle->node->setVisible(false);
        smgr->draw2DImage(bgTexture, Vector2(0, 0), Vector2(SceneHelper::screenWidth,  SceneHelper::screenHeight), true,
                          smgr->getMaterialByIndex(SHADER_DRAW_2D_IMAGE));
        //smgr->Render();
        
        renHelper->drawGrid();
        renHelper->drawCircle(this);
        renHelper->drawMoveAxisLine(this);
        renHelper->renderControls(this);
        //        // rtt division atlast post and pre stage
        renHelper->postRTTDrawCall(this);
        renHelper->rttDrawCall(this);
        smgr->EndDisplay(); // draws all the rendering command
        
        //        if(fabs(xAcceleration) > 0.0 || fabs(yAcceleration) > 0.0) {
        //            swipeToRotate();
        //            updateLightCamera();
        //        }
        //
        //        if(selectedNodeId != NOT_SELECTED)
        //            setTransparencyForIntrudingObjects();
    }
}

Vector4 SGEditorScene::getCameraPreviewLayout()
{
    float camPrevWidth = (SceneHelper::screenWidth) * CAM_PREV_PERCENT;
    float camPrevHeight = (SceneHelper::screenHeight) * CAM_PREV_PERCENT;
    float camPrevRatio = RESOLUTION[cameraResolutionType][1] / camPrevHeight;
    
    float originX = SceneHelper::screenWidth - camPrevWidth * CAM_PREV_GAP_PERCENT_FROM_SCREEN_EDGE;
    float originY = SceneHelper::screenHeight - camPrevHeight * CAM_PREV_GAP_PERCENT_FROM_SCREEN_EDGE;
    float endX = originX + RESOLUTION[cameraResolutionType][0] / camPrevRatio;
    float endY = originY + RESOLUTION[cameraResolutionType][1] / camPrevRatio;
    return Vector4(originX,originY,endX,endY);
}

void SGEditorScene::findAndInsertInIKPositionMap(int jointId)
{
    shared_ptr<AnimatedMeshNode> rigNode = dynamic_pointer_cast<AnimatedMeshNode>(selectedNode->node);
    if(ikJointsPositionMap.find(jointId) == ikJointsPositionMap.end())
        ikJointsPositionMap.insert(pair<int,Vector3>(jointId,rigNode->getJointNode(jointId)->getAbsolutePosition()));
    else
        ikJointsPositionMap.find(jointId)->second = rigNode->getJointNode(jointId)->getAbsolutePosition();
}

void SGEditorScene::getIKJointPosition()
{
    shared_ptr<AnimatedMeshNode> rigNode = dynamic_pointer_cast<AnimatedMeshNode>(selectedNode->node);
    if(rigNode) {
        findAndInsertInIKPositionMap(LEG_RIGHT);
        findAndInsertInIKPositionMap(LEG_LEFT);
        findAndInsertInIKPositionMap(HAND_RIGHT);
        findAndInsertInIKPositionMap(HAND_LEFT);
    }
}

void SGEditorScene::setMirrorState(MIRROR_SWITCH_STATE flag)
{
    mirrorSwitchState = flag;
    if(isJointSelected)
        selectMan->highlightJointSpheres(this);
}

MIRROR_SWITCH_STATE SGEditorScene::getMirrorState()
{
    return mirrorSwitchState;
}

void SGEditorScene::reloadKeyFrameMap()
{
    if(selectedNodeId != NOT_SELECTED) {
        SGNode *selectedMesh = nodes[selectedNodeId];
        bool searchPos = true,searchRot = false,searchScale = false;
        isKeySetForFrame.clear();
        searchRot = (nodes[selectedNodeId]->getType() == NODE_LIGHT) ? false:true;
        searchScale = (nodes[selectedNodeId]->getType() > NODE_LIGHT) ? true:false;
        if(searchPos){
            for(unsigned long i = 0; i < selectedMesh->positionKeys.size(); i++)
                isKeySetForFrame.insert(pair<int,int>(selectedMesh->positionKeys[i].id,selectedMesh->positionKeys[i].id));
        }
        if(searchRot){
            for(unsigned long i = 0; i < selectedMesh->rotationKeys.size(); i++)
                isKeySetForFrame.insert(pair<int,int>(selectedMesh->rotationKeys[i].id,selectedMesh->rotationKeys[i].id));
        }
        if(searchScale){
            for(unsigned long i = 0; i < selectedMesh->scaleKeys.size(); i++)
                isKeySetForFrame.insert(pair<int,int>(selectedMesh->scaleKeys[i].id,selectedMesh->scaleKeys[i].id));
        }
        
        for( unsigned long i = 0; i < selectedMesh->joints.size(); i++){
            for(unsigned long j = 0; j < selectedMesh->joints[i]->rotationKeys.size(); j++) {
                isKeySetForFrame.insert(pair<int,int>(selectedMesh->joints[i]->rotationKeys[j].id,selectedMesh->joints[i]->rotationKeys[j].id));
            }
            if(selectedMesh->getType() == NODE_TEXT) {
                for(unsigned long j = 0; j < selectedMesh->joints[i]->positionKeys.size(); j++)
                    isKeySetForFrame.insert(pair<int,int>(selectedMesh->joints[i]->positionKeys[j].id,selectedMesh->joints[i]->positionKeys[j].id));
                for(unsigned long j = 0; j < selectedMesh->joints[i]->scaleKeys.size(); j++)
                    isKeySetForFrame.insert(pair<int,int>(selectedMesh->joints[i]->scaleKeys[j].id,selectedMesh->joints[i]->scaleKeys[j].id));
            }
        }
    }
    else
        isKeySetForFrame.clear();
}

