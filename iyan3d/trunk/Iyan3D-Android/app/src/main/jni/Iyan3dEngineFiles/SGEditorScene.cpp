//
//  SGEditorScene.cpp
//  Iyan3D
//
//  Created by Karthik on 22/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#include "SGEditorScene.h"

string constants::BundlePath = " ";
string constants::CachesStoragePath = " ";
string constants::DocumentsStoragePath="";
string constants::impotedImagesPathAndroid="";
float constants::iOS_Version = 0;

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

    renHelper = new RenderHelper(sceneMngr, this);
    selectMan = new SGSelectionManager(sceneMngr, this);
    updater = new SGSceneUpdater(sceneMngr, this);
    loader = new SGSceneLoader(sceneMngr, this);
    moveMan = new SGMovementManager(sceneMngr, this);
    
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
        smgr->Render();
        
        renHelper->drawGrid();
        renHelper->drawCircle();
        renHelper->drawMoveAxisLine();
        renHelper->renderControls();
        //        // rtt division atlast post and pre stage
        renHelper->postRTTDrawCall();
        renHelper->rttDrawCall();
        smgr->EndDisplay(); // draws all the rendering command
        
        moveMan->swipeToRotate();
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
        selectMan->highlightJointSpheres();
}

MIRROR_SWITCH_STATE SGEditorScene::getMirrorState()
{
    return mirrorSwitchState;
}

void SGEditorScene::initLightCamera(Vector3 position)
{
    lightCamera = smgr->createCameraNode("LightUniforms");
    lightCamera->setFOVInRadians(150.0 * DEGTORAD);
    lightCamera->setNearValue(5.0);
    updater->updateLightCam(position);
}

void SGEditorScene::clearSelections()
{
    isNodeSelected = isJointSelected = false;
    selectedNode = NULL;
    selectedJoint = NULL;
    selectedNodeId = selectedJointId = NOT_SELECTED;
}

void SGEditorScene::shaderCallBackForNode(int nodeID,string matName)
{
    for(int i = 0; i < nodes.size();i++){
        if(nodes[i]->node->getID() == nodeID){
            shaderMGR->setUniforms(nodes[i],matName);
            break;
        }
    }
}
bool SGEditorScene::isNodeTransparent(int nodeId)
{
    if(nodeId == -1)
        return false;
    else{
        for(int i = 0; i < nodes.size();i++){
            if(nodes[i]->node->getID() == nodeId){
                return (nodes[i]->props.transparency < 1.0) || nodes[i]->props.isSelected || (!nodes[i]->props.isVisible);
                break;
            }
        }
    }
}

void SGEditorScene::setJointsUniforms(int nodeID,string matName)
{
    shaderMGR->setUniforms(jointSpheres[nodeID - JOINT_SPHERES_START_ID],matName);
}
void SGEditorScene::setRotationCircleUniforms(int nodeID,string matName)
{
    shaderMGR->setUniforms(rotationCircle,matName);
}
bool SGEditorScene::isJointTransparent(int nodeID,string matName)
{
    return (jointSpheres[nodeID - JOINT_SPHERES_START_ID]->props.transparency < 1.0);
}
void SGEditorScene::setControlsUniforms(int nodeID,string matName)
{
    shaderMGR->setUniforms(sceneControls[nodeID - CONTROLS_START_ID],matName);
}
bool SGEditorScene::isControlsTransparent(int nodeID,string matName)
{
    return (sceneControls[nodeID - CONTROLS_START_ID]->props.transparency < 1.0);
}


