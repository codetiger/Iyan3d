//
//  SGEditorScene.cpp
//  Iyan3D
//
//  Created by Karthik on 22/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//
//

#include "HeaderFiles/SGEditorScene.h"

string constants::BundlePath = " ";
string constants::CachesStoragePath = " ";
string constants::DocumentsStoragePath="";
string constants::impotedImagesPathAndroid="";
float constants::iOS_Version = 0;

SGEditorScene::SGEditorScene(SceneManager *smgr, int screenWidth, int screenHeight, int maxUniforms, int maxJoints)
{
    SceneHelper::screenWidth = screenWidth;
    SceneHelper::screenHeight = screenHeight;
    this->smgr = smgr;
    renHelper = new RenderHelper(smgr, this);

    this->smgr->setVAOSupport(renHelper->supportsVAO());

    viewCamera =  SceneHelper::initViewCamera(smgr, cameraTarget, cameraRadius);
    SceneHelper::initLightMesh(smgr);
    initVariables(smgr, maxUniforms, maxJoints);

    initTextures();
    rotationCircle = SceneHelper::createCircle(smgr);
    directionLine = SceneHelper::createLightDirLine(smgr);
    lightCircles = SceneHelper::createLightCircles(smgr);
    blueGrid = SceneHelper::createBlueLines(smgr);
    greenGrid = SceneHelper::createGreenLines(smgr);
    redGrid = SceneHelper::createRedLines(smgr);
    sceneControls.clear();
    sceneControls = SceneHelper::initControls(smgr);
    directionIndicator = SceneHelper::initIndicatorNode(smgr);
    BoneLimitsHelper::init();

    renderCamera = SceneHelper::initRenderCamera(smgr, DEFAULT_CAMERA_FOV);
}

SGEditorScene::~SGEditorScene()
{
    ShaderManager::lightPosition.clear();
    ShaderManager::lightColor.clear();
    ShaderManager::lightFadeDistances.clear();
    ShaderManager::lightTypes.clear();
    
    selectedNodeIds.clear();
    isKeySetForFrame.clear();
    
    removeAllNodes();
    nodes.clear();
    
    if(ikJointsPositionMap.size())
        ikJointsPositionMap.clear();
    
    for(int i = 0;i < jointSpheres.size();i++){
        if(jointSpheres[i])
            delete jointSpheres[i];
    }
    jointSpheres.clear();
    
    if(renderingTextureMap.size())
        renderingTextureMap.clear();
    
    if(sceneControls.size()) {
        for (int i = 0; i < sceneControls.size(); i++) {
            if(sceneControls[i]) {
                delete sceneControls[i];
                sceneControls[i] = NULL;
            }
        }
        sceneControls.clear();
    }
    
    if(directionLine)
        delete directionLine;
    if(lightCircles)
        delete lightCircles;
    if(greenGrid)
        delete greenGrid;
    if(blueGrid)
        delete blueGrid;
    if(redGrid)
        delete redGrid;    
    if(directionIndicator)
        delete directionIndicator;
    if(rotationCircle)
        delete rotationCircle;
    if(controlsPlane)
        delete controlsPlane;
    if(shaderMGR)
        delete shaderMGR;
    if(renHelper)
        delete renHelper;
    if(selectMan)
        delete selectMan;
    if(updater)
        delete updater;
    if(loader)
        delete loader;
    if(moveMan)
        delete moveMan;
    if(actionMan)
        delete actionMan;
    if(writer)
        delete writer;
    if(animMan)
        delete animMan;
    if(physicsHelper)
        delete physicsHelper;
    if(cmgr)
        delete cmgr;
    if(smgr)
        delete smgr;
}

void SGEditorScene::removeAllNodes()
{
    for(int i = 0; i < nodes.size(); i++) {
        loader->removeObject(i, true);
    }
}

void SGEditorScene::initVariables(SceneManager* sceneMngr, int maxUniforms, int maxJoints)
{
    cmgr = new CollisionManager();
    shaderMGR = new ShaderManager(sceneMngr, maxUniforms, maxJoints);
    selectMan = new SGSelectionManager(sceneMngr, this);
    updater = new SGSceneUpdater(sceneMngr, this);
    loader = new SGSceneLoader(sceneMngr, this);
    moveMan = new SGMovementManager(sceneMngr, this);
    actionMan = new SGActionManager(sceneMngr, this);
    writer = new SGSceneWriter(sceneMngr, this);
    animMan = new SGAnimationManager(sceneMngr, this);
    physicsHelper = new PhysicsHelper(this);

    camPreviewOrigin = camPreviewEnd = Vector2(0.0, 0.0);
    isMultipleSelection = false;
    isJointSelected = isNodeSelected = isControlSelected = shadowsOff = false;
    freezeRendering = isPlaying = isPreviewMode = false;
    selectedNodeId = selectedJointId = selectedMeshBufferId = riggingNodeId = NOT_EXISTS;
    selectedNode = NULL;
    selectedJoint = NULL;
    jointSpheres.clear();
    controlType = MOVE;
    selectedControlId = NOT_EXISTS;
    selectedNode = NULL;
    controlsPlane = new Plane3D();
    nodes.clear();
    totalFrames = 24;
    assetIDCounter = 0;
    currentFrame = previousFrame = 0;
    
    camPreviewOrigin.x = 0.0;
    camPreviewOrigin.y = SceneHelper::screenHeight - (SceneHelper::screenHeight * CAM_PREV_PERCENT);
    float camPrevRatio = RESOLUTION[0][1] / ((SceneHelper::screenHeight) * CAM_PREV_PERCENT);
    camPreviewEnd.x = camPreviewOrigin.x + RESOLUTION[0][0] / camPrevRatio;
    camPreviewEnd.y = camPreviewOrigin.y + RESOLUTION[0][1] / camPrevRatio;
    camPreviewScale = 1.0;
    previousDistance = Vector2(0.0, 0.0);
    actionObjectsSize = 0;
    moveNodeId = NOT_SELECTED;

    shaderMGR->addOrUpdateProperty(AMBIENT_LIGHT, Vector4(0), UNDEFINED, SLIDER_TYPE, "Ambient Light", "Scene Properties");
    shaderMGR->addOrUpdateProperty(ENVIRONMENT_TEXTURE, Vector4(0), UNDEFINED, IMAGE_TYPE, "Environment Matcap", "Scene Properties", "");
}

void SGEditorScene::initTextures()
{
    const int MTLStorageModeShared  = 0, MTLStorageModePrivate = 2, MTLStorageModeMemoryless = 3;

    touchTexture = smgr->createRenderTargetTexture("TouchTexture", TEXTURE_RGBA8, TEXTURE_BYTE, TOUCH_TEXTURE_WIDTH, TOUCH_TEXTURE_HEIGHT, MTLStorageModeShared);

    previewTexture = smgr->createRenderTargetTexture("previewTexture", TEXTURE_RGBA8, TEXTURE_BYTE, PREVIEW_TEXTURE_WIDTH, PREVIEW_TEXTURE_HEIGHT, MTLStorageModeShared);
    thumbnailTexture = smgr->createRenderTargetTexture("thumbnailTexture", TEXTURE_RGBA8, TEXTURE_BYTE, THUMBNAIL_TEXTURE_WIDTH, THUMBNAIL_TEXTURE_HEIGHT, MTLStorageModeShared);
    whiteBorderTexture = smgr->loadTexture("whiteborder",constants::BundlePath + "/whiteborder.png",TEXTURE_RGBA8,TEXTURE_BYTE, true);
    
    shaderMGR->shadowTexture = smgr->createRenderTargetTexture("shadowTexture", TEXTURE_DEPTH32, TEXTURE_BYTE, SHADOW_TEXTURE_WIDTH, SHADOW_TEXTURE_HEIGHT, MTLStorageModePrivate);
    shaderMGR->environmentTex = NULL;
    
    renderingTextureMap[RESOLUTION[0][0]] = smgr->createRenderTargetTexture("RenderTexture", TEXTURE_RGBA8, TEXTURE_BYTE,RESOLUTION[0][0] , RESOLUTION[0][1], MTLStorageModeShared);
    renderingTextureMap[RESOLUTION[1][0]] = smgr->createRenderTargetTexture("RenderTexture", TEXTURE_RGBA8, TEXTURE_BYTE,RESOLUTION[1][0] , RESOLUTION[1][1], MTLStorageModeShared);
    renderingTextureMap[RESOLUTION[2][0]] = smgr->createRenderTargetTexture("RenderTexture", TEXTURE_RGBA8, TEXTURE_BYTE,RESOLUTION[2][0] , RESOLUTION[2][1], MTLStorageModeShared);
    renderingTextureMap[RESOLUTION[3][0]] = smgr->createRenderTargetTexture("RenderTexture", TEXTURE_RGBA8, TEXTURE_BYTE,RESOLUTION[3][0] , RESOLUTION[3][1], MTLStorageModeShared);
    renderingTextureMap[RESOLUTION[4][0]] = smgr->createRenderTargetTexture("RenderTexture", TEXTURE_RGBA8, TEXTURE_BYTE,RESOLUTION[4][0] , RESOLUTION[4][1], MTLStorageModeShared);
}

void SGEditorScene::renderAll()
{
    if(freezeRendering) {
        ShaderManager::isRendering = true;
        return;
    }

    bool displayPrepared = smgr->PrepareDisplay(SceneHelper::screenWidth, SceneHelper::screenHeight, true, true, false, Vector4(0.1, 0.1, 0.1, 1.0));

    if(displayPrepared) {
        rotationCircle->node->setVisible(false);
        renHelper->drawGrid();
        
        if((selectedNodeId != NOT_EXISTS && nodes[selectedNodeId]->getType() == NODE_CAMERA) || isPlaying)
            ShaderManager::isRendering = true;
        else
            ShaderManager::isRendering = false;
        
        smgr->Render(false);

        renHelper->drawCircle();
        renHelper->drawMoveAxisLine();
        renHelper->renderControls();
        
//        if(touchTexture)
//            smgr->draw2DImage(touchTexture, Vector2(0.0, 100.0), Vector2(256.0, 356.0), smgr->getMaterialByIndex(SHADER_DRAW_2D_IMAGE), false);
        
        //        // rtt division atlast post and pre stage
        renHelper->postRTTDrawCall();
        renHelper->rttDrawCall();
        smgr->EndDisplay(); // draws all the rendering command

        moveMan->swipeToRotate();
        setTransparencyForObjects();
    }
}

void SGEditorScene::setPropsOfObject(SGNode *sgNode, int pType)
{
    if(physicsHelper)
        physicsHelper->calculateAndSetPropsOfObject(sgNode, pType);
}

void SGEditorScene::syncSceneWithPhysicsWorld()
{
    if(physicsHelper)
        physicsHelper->syncPhysicsWorld();
}

void SGEditorScene::updatePhysics(int frame)
{
    physicsHelper->updatePhysicsUpToFrame(frame);
}

void SGEditorScene::enableDirectionIndicator()
{
    directionIndicator->node->setVisible(true);
    directionIndicator->node->setPosition(nodes[selectedNodeId]->getNodePosition());
    directionIndicator->assetId = selectedNodeId;
    controlType = ROTATE;
}

void SGEditorScene::updateDirectionLine()
{
    bool status = (selectedNodeId != NOT_EXISTS && (nodes[selectedNodeId]->getType() == NODE_LIGHT || nodes[selectedNodeId]->getType() == NODE_ADDITIONAL_LIGHT));
    
    if(!status) {
        directionLine->node->setVisible(false);
        lightCircles->node->setVisible(false);
        return;
    }
    
     bool isDirectionLight = (nodes[selectedNodeId]->getProperty(LIGHT_TYPE).value.x == (int)DIRECTIONAL_LIGHT);
        
        directionLine->node->setVisible(isDirectionLight);
        lightCircles->node->setVisible(!isDirectionLight);
    
    Vector3 position = KeyHelper::getKeyInterpolationForFrame<int, SGPositionKey, Vector3>(currentFrame, nodes[selectedNodeId]->positionKeys);

    if(isDirectionLight) {
        Quaternion rotation = KeyHelper::getKeyInterpolationForFrame<int, SGRotationKey, Quaternion>(currentFrame, nodes[selectedNodeId]->rotationKeys,true);

        directionLine->node->setPosition(position);
        directionLine->node->setRotation(rotation);
    } else {
        lightCircles->node->setPosition(position);
        lightCircles->node->setScale(Vector3(nodes[selectedNodeId]->getProperty(SPECIFIC_FLOAT).value.x));
        lightCircles->node->updateAbsoluteTransformation();
    }
}

void SGEditorScene::setTransparencyForObjects()
{
    if(!smgr && nodes.size() < 3)
        return;

    isPreviewMode = false;

    for(int index = 0; index < nodes.size(); index++) {
        if(nodes[index]->isTempNode) {
            isPreviewMode = true;
            nodes[index]->getProperty(TRANSPARENCY).value.x = 1.0;
        } else {
            nodes[index]->getProperty(TRANSPARENCY).value.x = 0.2;
        }
    }

    if(!isPreviewMode) {
        for(int index = 0; index < nodes.size(); index++)
            if(nodes[index]->getProperty(VISIBILITY).value.x)
                nodes[index]->getProperty(TRANSPARENCY).value.x = 1.0;
    }
}

Vector4 SGEditorScene::getCameraPreviewLayout()
{
    int cRT = nodes[NODE_CAMERA]->getProperty(CAM_RESOLUTION).value.x;
    float camPrevRatio = RESOLUTION[cRT][1] / ((SceneHelper::screenHeight) * CAM_PREV_PERCENT * camPreviewScale);
    float limitX = topRight.x - (RESOLUTION[cRT][0] / camPrevRatio);
    float limitY = SceneHelper::screenHeight - (RESOLUTION[cRT][1] / camPrevRatio);
    
    if(previousDistance != renHelper->cameraPreviewMoveDist && camPreviewOrigin.x >= topLeft.x && camPreviewOrigin.y >= topLeft.y && camPreviewOrigin.x <= limitX && camPreviewOrigin.y <= limitY) {
        camPreviewOrigin.x -= renHelper->cameraPreviewMoveDist.x;
        camPreviewOrigin.y -= renHelper->cameraPreviewMoveDist.y;
    } else {
        camPreviewOrigin.x = (camPreviewOrigin.x < topLeft.x) ? topLeft.x : camPreviewOrigin.x;
        camPreviewOrigin.y = (camPreviewOrigin.y < topLeft.y) ? topLeft.y : camPreviewOrigin.y;
        camPreviewOrigin.x = (camPreviewOrigin.x > limitX) ? limitX : camPreviewOrigin.x;
        camPreviewOrigin.y = (camPreviewOrigin.y > limitY) ? limitY : camPreviewOrigin.y;
    }
    camPreviewEnd.x = camPreviewOrigin.x + RESOLUTION[cRT][0] / camPrevRatio;
    camPreviewEnd.y = camPreviewOrigin.y + RESOLUTION[cRT][1] / camPrevRatio;
    previousDistance = renHelper->cameraPreviewMoveDist;
    return Vector4(camPreviewOrigin.x, camPreviewOrigin.y, camPreviewEnd.x, camPreviewEnd.y);
}

void SGEditorScene::findAndInsertInIKPositionMap(int jointId)
{
    shared_ptr<AnimatedMeshNode> rigNode = dynamic_pointer_cast<AnimatedMeshNode>(selectedNode->node);
    if(ikJointsPositionMap.find(jointId) == ikJointsPositionMap.end())
        ikJointsPositionMap.insert(pair<int,Vector3>(jointId,rigNode->getJointNode(jointId)->getAbsolutePosition()));
    else
        ikJointsPositionMap.find(jointId)->second = rigNode->getJointNode(jointId)->getAbsolutePosition();
}

vector<string> SGEditorScene::getUserFileNames(bool forBackup)
{
    vector<string> userFileNames;
    for(int index = 0; index < nodes.size(); index++) {
        SGNode *sgNode = nodes[index];
        NODE_TYPE nType = sgNode->getType();
        
        if((sgNode->assetId > 0 && sgNode->getProperty(TEXTURE).fileName.find(to_string(sgNode->assetId)) ==  string::npos) || (sgNode->assetId == 0 && sgNode->getProperty(TEXTURE).fileName.length() > 0))
            userFileNames.push_back(sgNode->getProperty(TEXTURE).fileName + ".png");
        
        if(sgNode->assetId >= 40000 && sgNode->assetId < 50000) {
            userFileNames.push_back(to_string(sgNode->assetId) + ".sgr");
            if(sgNode->getProperty(TEXTURE).fileName.find(to_string(sgNode->assetId)) !=  string::npos)
                userFileNames.push_back(to_string(sgNode->assetId) + "-cm.png");
        } else if (sgNode->assetId >= 20000 && sgNode->assetId < 30000) {
            userFileNames.push_back(to_string(sgNode->assetId) + ".sgm");
            userFileNames.push_back(to_string(sgNode->assetId) + ".obj");
            if(sgNode->getProperty(TEXTURE).fileName.find(to_string(sgNode->assetId)) !=  string::npos)
                userFileNames.push_back(to_string(sgNode->assetId) + "-cm.png");
        } else if ((nType == NODE_TEXT || nType == NODE_TEXT_SKIN)) {
            userFileNames.push_back(sgNode->optionalFilePath);
        } else if (sgNode->assetId >= 60001 && sgNode->assetId < 60007)
            userFileNames.push_back(to_string(sgNode->assetId) + ".sgm");
        else if (sgNode->getType() == NODE_IMAGE)
            userFileNames.push_back(sgNode->getProperty(TEXTURE).fileName + ".png");
        else if (sgNode->getType() == NODE_PARTICLES) {
            userFileNames.push_back(to_string(sgNode->assetId) + ".json");
            userFileNames.push_back(to_string(sgNode->assetId) + ".sgm");
            userFileNames.push_back(sgNode->getProperty(TEXTURE).fileName + ".png");
        } else if (sgNode->getType() == NODE_VIDEO && forBackup) {
            userFileNames.push_back(ConversionHelper::getStringForWString(sgNode->name));
        }
    }
    return userFileNames;
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

MIRROR_SWITCH_STATE SGEditorScene::getMirrorState()
{
    return actionMan->getMirrorState();
}

void SGEditorScene::initLightCamera(Vector3 position)
{
    lightCamera = smgr->createCameraNode("LightUniforms");
    //lightCamera->setFOVInRadians(150.0 * DEGTORAD);
    //lightCamera->setNearValue(5.0);
    updater->updateLightCam(position);
}

void SGEditorScene::clearSelections()
{
    isNodeSelected = isJointSelected = false;
    selectedNode = NULL;
    selectedJoint = NULL;
    selectedNodeId = selectedJointId = selectedMeshBufferId = NOT_SELECTED;
}

void SGEditorScene::shaderCallBackForNode(int nodeID, string matName, int materialIndex)
{
    for(int i = 0; i < nodes.size(); i++) {
        if(nodes[i]->node->getID() == nodeID) {
            shaderMGR->setUniforms(nodes[i], matName, materialIndex);
            break;
        }
    }
}

float SGEditorScene::getNodeTransparency(int nodeId)
{
    for(int i = 0; i < nodes.size();i++){
        if(nodes[i]->node->getID() == nodeId){
            return nodes[i]->getProperty(TRANSPARENCY).value.x;
            break;
        }
    }
    return -1.0;
}

bool SGEditorScene::isNodeTransparent(int nodeId)
{
    if(nodeId == -1)
        return false;
    else {
        for(int i = 0; i < nodes.size(); i++) {
            if(nodes[i]->node->getID() == nodeId) {
                if(nodes[i]->getType() == NODE_PARTICLES || nodes[i]->node->hasTransparency)
                    return true;
                
                return (nodes[i]->getProperty(TRANSPARENCY).value.x < 1.0) || nodes[i]->getProperty(SELECTED).value.x || (!nodes[i]->getProperty(VISIBILITY).value.x);
                break;
            }
        }
    }
}

void SGEditorScene::setLightLineUniforms(int nodeID, string matName)
{
    shaderMGR->setUniforms((nodeID == LIGHT_CIRCLES) ? lightCircles : directionLine, matName);
}

void SGEditorScene::setJointsUniforms(int nodeID,string matName)
{
    shaderMGR->setUniforms(jointSpheres[nodeID - JOINT_SPHERES_START_ID],matName);
}

void SGEditorScene::setRotationCircleUniforms(int nodeID,string matName)
{
    shaderMGR->setUniforms((nodeID == LIGHT_CIRCLES) ? lightCircles : rotationCircle,matName);
}

void SGEditorScene::setGridLinesUniforms(int nodeId, int rgb, string matName)
{
    shaderMGR->setUniforms((rgb == 1) ? redGrid : (rgb == 2) ? blueGrid : (rgb == 3) ? greenGrid : directionLine, matName);
}

bool SGEditorScene::isJointTransparent(int nodeID,string matName)
{
    return (jointSpheres[nodeID - JOINT_SPHERES_START_ID]->getProperty(TRANSPARENCY).value.x < 1.0);
}

void SGEditorScene::setControlsUniforms(int nodeID,string matName)
{
    if(nodeID == FORCE_INDICATOR_ID)
        shaderMGR->setUniforms(directionIndicator, matName);
    else
        shaderMGR->setUniforms(sceneControls[nodeID - CONTROLS_START_ID],matName);
}

bool SGEditorScene::isControlsTransparent(int nodeID,string matName)
{
    if(nodeID == FORCE_INDICATOR_ID)
        return false;
    return true;//(sceneControls[nodeID - CONTROLS_START_ID]->getProperty(TRANSPARENCY).value.x < 1.0);
}

bool SGEditorScene::hasNodeSelected()
{
    return isNodeSelected;
}

bool SGEditorScene::hasMeshBufferSelected()
{
    return (selectedMeshBufferId != NOT_EXISTS);
}

bool SGEditorScene::hasJointSelected()
{
    return isJointSelected;
}

SGNode* SGEditorScene::getSelectedNode()
{
    if (isNodeSelected)
        return selectedNode;
    return NULL;
}

SGJoint* SGEditorScene::getSelectedJoint()
{
    if (isJointSelected)
        return selectedJoint;
    return NULL;    
}

shared_ptr<Node> SGEditorScene::getParentNode()
{
    return selectMan->getParentNode();
}

void SGEditorScene::setEnvironmentTexture(std::string textureFilePath, bool isPreview)
{
    freezeRendering = true;
    
    if(shaderMGR->environmentTex) {
        smgr->RemoveTexture(shaderMGR->environmentTex);
        shaderMGR->environmentTex = NULL;
    }

    string envTexFilePath = FileHelper::getTexturesDirectory() + textureFilePath;
        shaderMGR->environmentTex = smgr->loadTexture("Env Texture", envTexFilePath, TEXTURE_RGBA8, TEXTURE_BYTE, true, 10);
    
    if(!isPreview) {
        shaderMGR->addOrUpdateProperty(ENVIRONMENT_TEXTURE, Vector4(0), UNDEFINED, IMAGE_TYPE, "Environment Matcap", "Scene Properties", textureFilePath);
    }
    
    for( int i = 2; i < nodes.size(); i++) {
        if(nodes[i]->getType() != NODE_IMAGE && nodes[i]->getType() != NODE_VIDEO) {
            for(int j = 0; j < nodes[i]->materialProps.size(); j++)
                nodes[i]->materialProps[j]->setTextureForType(shaderMGR->environmentTex, NODE_TEXTURE_TYPE_REFLECTIONMAP);
        }
    }
    freezeRendering = false;
}

bool SGEditorScene::loadSceneData(std::string *filePath)
{
    return loader->loadSceneData(filePath);
}

void SGEditorScene::saveSceneData(std::string *filePath)
{
    if(selectedNodeId != NOT_SELECTED)
        selectMan->unselectObject(selectedNodeId);
    
    writer->saveSceneData(filePath);
}

bool SGEditorScene::checkNodeSize()
{
    if(nodes.size() < NODE_LIGHT + 1)
        return false;
    return true;
}

void SGEditorScene::saveThumbnail(char* targetPath)
{
    if(!checkNodeSize())
        return;
    
    bool displayPrepared = smgr->PrepareDisplay(thumbnailTexture->width, thumbnailTexture->height, false, true, false, Vector4(0.0));
    if(!displayPrepared)
        return;
    
    selectMan->unselectObjects();
    renHelper->setControlsVisibility(false);
    renHelper->setJointSpheresVisibility(false);
    rotationCircle->node->setVisible(false);

    for(unsigned long i = NODE_CAMERA; i < nodes.size(); i++) {
        if(!(nodes[i]->getProperty(VISIBILITY).value.x))
            nodes[i]->node->setVisible(false);
    }
    
    selectMan->unselectObject(selectedNodeId);
    smgr->setRenderTarget(thumbnailTexture, true, true, false, Vector4(0.1, 0.1, 0.1, 1.0));
    renHelper->drawGrid();
    
    smgr->Render(false);
    smgr->EndDisplay();
    smgr->writeImageToFile(thumbnailTexture, targetPath, NO_FLIP);
    
    smgr->setRenderTarget(NULL, true, true, false, Vector4(0.0));
    
    for(unsigned long i = NODE_CAMERA; i < nodes.size(); i++) {
        if(!(nodes[i]->getProperty(VISIBILITY).value.x))
            nodes[i]->node->setVisible(true);
    }
    
//    if(selectedNodeId != NOT_SELECTED)
//        nodes[selectedNodeId]->getProperty(SELECTED).value.x = true;
//    
//    selectMan->selectObject(selectedNodeId, selectedMeshBufferId, false);
    renHelper->setControlsVisibility(true);
    smgr->EndDisplay();
    
}

int SGEditorScene::undo(int &returnValue2)
{
    return actionMan->undo(returnValue2);
}

int SGEditorScene::redo()
{
    return actionMan->redo();
}

void SGEditorScene::setLightingOn()
{
    ShaderManager::sceneLighting = true;
}

void SGEditorScene::setLightingOff()
{
    ShaderManager::sceneLighting = false;
}

void SGEditorScene::popLightProps()
{
    if(ShaderManager::lightPosition.size() > 1) {
        ShaderManager::lightPosition.pop_back();
        ShaderManager::lightColor.pop_back();
        ShaderManager::lightFadeDistances.pop_back();
        ShaderManager::lightTypes.pop_back();
    }
    
}

void SGEditorScene::clearLightProps()
{
    while(ShaderManager::lightPosition.size() > 1)
        popLightProps();
}

void SGEditorScene::changeTexture(string textureFileName, Vector3 vertexColor, bool isTemp, bool isUndoRedo, PROP_INDEX pIndex)
{
    if(!isNodeSelected || selectedNodeId == NOT_SELECTED)
        return;
    
    int matIndex = (selectedMeshBufferId == NOT_SELECTED) ? 0 : selectedMeshBufferId;
    
    string texturePath = FileHelper::getTexturesDirectory() + textureFileName;

    node_texture_type texType;
    if(pIndex == TEXTURE)
        texType = NODE_TEXTURE_TYPE_COLORMAP;
    else if (pIndex == BUMP_MAP)
        texType = NODE_TEXTURE_TYPE_NORMALMAP;
    else
        texType = NODE_TEXTURE_TYPE_REFLECTIONMAP;

    if(textureFileName.length() > 4 && nodes[selectedNodeId]->checkFileExists(texturePath)) {
        nodes[selectedNodeId]->getProperty(IS_VERTEX_COLOR, matIndex).value.x = (pIndex == TEXTURE) ? false : nodes[selectedNodeId]->getProperty(IS_VERTEX_COLOR, matIndex).value.x; // Vector4(false, 0, 0, 0), MATERIAL_PROPS);
        
        if(nodes[selectedNodeId]->node->type == NODE_TYPE_INSTANCED) {
            loader->copyMeshFromOriginalNode(nodes[selectedNodeId]);
            loader->removeNodeFromInstances(nodes[selectedNodeId]);
            nodes[selectedNodeId]->node->type = NODE_TYPE_MESH;
            nodes[selectedNodeId]->node->original = shared_ptr<Node>();
        } else if (nodes[selectedNodeId]->instanceNodes.size() > 0) {
            loader->setFirstInstanceAsMainNode(nodes[selectedNodeId]);
        }
        
        bool smoothTexture = nodes[selectedNodeId]->getProperty(TEXTURE_SMOOTH, selectedMeshBufferId).value.x;
        Texture *nodeTex = smgr->loadTexture(textureFileName, texturePath, TEXTURE_RGBA8, TEXTURE_BYTE, smoothTexture);
        nodes[selectedNodeId]->materialProps[matIndex]->setTextureForType(nodeTex, texType);
        
        if(!isTemp || isUndoRedo){
            nodes[selectedNodeId]->getProperty(pIndex, matIndex).fileName = textureFileName;
        }
    } else {
        
        if(pIndex == TEXTURE) {
        nodes[selectedNodeId]->getProperty(VERTEX_COLOR, matIndex).value = Vector4(vertexColor, 0);
        nodes[selectedNodeId]->getProperty(IS_VERTEX_COLOR, matIndex).value = Vector4(true, 0, 0, 0);
        }
        
        if(!isTemp || isUndoRedo){
            nodes[selectedNodeId]->getProperty(pIndex, matIndex).fileName = "";
        }
        
        if(pIndex == BUMP_MAP) {
            Texture *tex = nodes[selectedNodeId]->materialProps[matIndex]->getTextureOfType(texType);
            if(tex)
                smgr->RemoveTexture(tex);
            nodes[selectedNodeId]->materialProps[matIndex]->setTextureForType(NULL, texType);

        }
    }
    
    if(!isTemp || isUndoRedo){
        nodes[selectedNodeId]->oriTextureName = nodes[selectedNodeId]->getProperty(TEXTURE, matIndex).fileName;
        nodes[selectedNodeId]->getProperty(ORIG_VERTEX_COLOR, matIndex).value = nodes[selectedNodeId]->getProperty(VERTEX_COLOR, matIndex).value;
    }
}

void SGEditorScene::removeTempTextureAndVertex(int selectedNode, int selectedMaterialIndex, PROP_INDEX pIndex)
{
    if(selectedNode == NOT_EXISTS && pIndex != ENVIRONMENT_TEXTURE)
        return;
    
    string StoragePath = constants::CachesStoragePath + "/";

    node_texture_type texType;
    if(pIndex == TEXTURE)
        texType = NODE_TEXTURE_TYPE_COLORMAP;
    else if (pIndex == BUMP_MAP)
        texType = NODE_TEXTURE_TYPE_NORMALMAP;
    else
        texType = NODE_TEXTURE_TYPE_REFLECTIONMAP;

    if(pIndex == ENVIRONMENT_TEXTURE) {
        setEnvironmentTexture(shaderMGR->getProperty(ENVIRONMENT_TEXTURE).fileName, false);
    }
    else {
        string textureFileName = FileHelper::getTexturesDirectory() + nodes[selectedNode]->getProperty(pIndex, selectedMeshBufferId).fileName;

        if(selectedMaterialIndex == -1)
            selectedMaterialIndex = 0;
        
        if(selectedMaterialIndex >= 0 && selectedMaterialIndex < nodes[selectedNode]->materialProps.size()) {
            Texture *tex = nodes[selectedNode]->materialProps[selectedMaterialIndex]->getTextureOfType(texType);
            if(tex)
                smgr->RemoveTexture(tex);
            nodes[selectedNode]->materialProps[selectedMaterialIndex]->setTextureForType(NULL, texType);
            
            if(pIndex == TEXTURE) {
                if(nodes[selectedNode]->getProperty(TEXTURE, selectedMeshBufferId).fileName.length() > 3 && nodes[selectedNode]->checkFileExists(textureFileName)) {
                    nodes[selectedNode]->getProperty(IS_VERTEX_COLOR, selectedMeshBufferId).value.x = false; // Vector4(false, 0, 0, 0), MATERIAL_PROPS);
                    Texture *nodeTex = smgr->loadTexture(nodes[selectedNode]->getProperty(TEXTURE, selectedMeshBufferId).fileName, textureFileName, TEXTURE_RGBA8, TEXTURE_BYTE, nodes[selectedNode]->getProperty(TEXTURE_SMOOTH, selectedMeshBufferId).value.x);
                    nodes[selectedNode]->materialProps[selectedMaterialIndex]->setTextureForType(nodeTex, NODE_TEXTURE_TYPE_COLORMAP);
                } else {
                    nodes[selectedNode]->getProperty(TEXTURE, selectedMeshBufferId).fileName = "";
                    nodes[selectedNode]->getProperty(VERTEX_COLOR, selectedMeshBufferId).value = nodes[selectedNode]->getProperty(ORIG_VERTEX_COLOR).value;
                    nodes[selectedNode]->getProperty(IS_VERTEX_COLOR, selectedMeshBufferId).value.x = true; // Vector4(true, 0, 0, 0), MATERIAL_PROPS);
                }
            } else if(pIndex == BUMP_MAP) {
                if(nodes[selectedNode]->getProperty(BUMP_MAP).fileName.length() > 3  && nodes[selectedNode]->checkFileExists(textureFileName)) {
                    Texture *nodeTex = smgr->loadTexture(nodes[selectedNode]->getProperty(BUMP_MAP).fileName, textureFileName, TEXTURE_RGBA8, TEXTURE_BYTE, true);
                    nodes[selectedNode]->materialProps[selectedMaterialIndex]->setTextureForType(nodeTex, NODE_TEXTURE_TYPE_NORMALMAP);
                } else {
                    nodes[selectedNode]->getProperty(BUMP_MAP).fileName = "";
                }
            }
        }
    }
}

bool SGEditorScene::canEditRigBones(SGNode *sgNode)
{
    if(sgNode->getType() == NODE_RIG) {
        shared_ptr<AnimatedMeshNode> animNode = dynamic_pointer_cast<AnimatedMeshNode>(sgNode->node);
        if(((SkinMesh*)animNode->mesh)->versionId == 1)
            return true;
    }
    return false;
}

bool SGEditorScene::isNodeInSelection(SGNode *sgNode)
{
    bool status = false;
    for(int i = 0; i < selectedNodeIds.size(); i++) {
        if(nodes[selectedNodeIds[i]] == sgNode) {
            status = true;
            break;
        }
    }
    return status;
}

bool SGEditorScene::allObjectsScalable()
{
    bool status = true;
    for(int i = 0; i < selectedNodeIds.size(); i++) {
        NODE_TYPE nType = nodes[selectedNodeIds[i]]->getType();
        if(nType == NODE_CAMERA || nType == NODE_LIGHT || nType == NODE_ADDITIONAL_LIGHT || nType == NODE_PARTICLES) {
            status = false;
            break;
        }
    }
    return status;
}

bool SGEditorScene::allNodesRemovable()
{
    bool status = true;
    for(int i = 0; i < selectedNodeIds.size(); i++) {
        NODE_TYPE nType = nodes[selectedNodeIds[i]]->getType();
        if(nType == NODE_CAMERA || nType == NODE_LIGHT) {
            status = false;
            break;
        }
    }
    return status;
}

bool SGEditorScene::allNodesClonable()
{
    bool status = (bool)selectedNodeIds.size();
    for(int i = 0; i < selectedNodeIds.size(); i++) {
        NODE_TYPE nType = nodes[selectedNodeIds[i]]->getType();
        if(nType == NODE_CAMERA || nType == NODE_LIGHT || nType == NODE_ADDITIONAL_LIGHT) {
            status = false;
            break;
        }
    }
    return status;
}

bool SGEditorScene::addTempNodeToScene()
{
    bool hasChangedStatus = false;
    for(int i = 0; i < nodes.size(); i++) {
        if(nodes[i]->isTempNode) {
            hasChangedStatus = true;
            nodes[i]->isTempNode = false;
        }
    }
    
    return hasChangedStatus;
}

Vector3 SGEditorScene::getSelectedNodeScale()
{
    if(selectedNodeIds.size() > 0 && getParentNode())
        return getParentNode()->getScale();
    else if(isNodeSelected && selectedNode) {
        if(isJointSelected && selectedNode->getType() == NODE_TEXT_SKIN)
            return selectedNode->joints[selectedJointId]->jointNode->getScale();
        else
            return selectedNode->node->getScale();
    }
    
    return Vector3(1.0);
}

Vector3 SGEditorScene::getTransformValue()
{
    if((!hasNodeSelected() || !isControlSelected) && selectedNodeIds.size() == 0)
        return Vector3(-999.0 ,-999.0, -999.0);
    
    shared_ptr<Node> curNode = (selectedNodeIds.size() > 0) ? getParentNode() : getSelectedNode()->node;
    
    if(controlType == MOVE)
        return curNode->getAbsolutePosition();
    else if (controlType == ROTATE) {
        Vector3 rot;
        curNode->getRotation().toEuler(rot);
        return rot;
    } else if (controlType == SCALE)
        return curNode->getScale();
    
    return Vector3(-999.0 ,-999.0, -999.0);
}

Vector3 SGEditorScene::getPivotPoint(bool initial)
{
    if(getParentNode() && initial)
        return getParentNode()->getBoundingBox().getCenter();
    else if (getParentNode())
        return getParentNode()->getAbsolutePosition();
    return Vector3(0.0);
}

void SGEditorScene::setMirrorState(MIRROR_SWITCH_STATE flag)
{
    actionMan->setMirrorState(flag);
    if(isJointSelected)
        selectMan->highlightMeshBufferAndJointSpheres();
}

bool SGEditorScene::switchMirrorState()
{
    SGAction action;
    action.actionType = ACTION_CHANGE_MIRROR_STATE;
    action.actionSpecificFlags.push_back(getMirrorState());
    actionMan->addAction(action);
    setMirrorState((MIRROR_SWITCH_STATE)!getMirrorState());
    if(isJointSelected)
        selectMan->highlightMeshBufferAndJointSpheres();
    return getMirrorState();
}

SceneManager* SGEditorScene::getSceneManager() {
    return smgr;
}
