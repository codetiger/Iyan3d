//
//  SGGestureManager.cpp
//  Iyan3D
//
//  Created by Karthik on 25/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#include "HeaderFiles/SGSelectionManager.h"
#include "HeaderFiles/SGEditorScene.h"

SGEditorScene *selectionScene;
SGSelectionManager::SGSelectionManager(SceneManager* sceneMngr, void* scene)
{
    this->smgr = sceneMngr;
    selectionScene = (SGEditorScene*)scene;
}

SGSelectionManager::~SGSelectionManager()
{
    
}

void SGSelectionManager::checkSelection(Vector2 touchPosition,bool isDisplayPrepared)
{
    if(!selectionScene || !smgr)
        return;
    
    selectionScene->renHelper->rttNodeJointSelection(touchPosition);
    if(selectionScene->shaderMGR->deviceType == METAL){
        selectionScene->renHelper->rttNodeJointSelection(touchPosition);
        getNodeColorFromTouchTexture();
    }
    selectionScene->isRTTCompleted = true;
    selectionScene->updater->updateControlsOrientaion();
    postNodeJointSelection();
}

void SGSelectionManager::checkCtrlSelection(Vector2 curTouchPos,bool isDisplayPrepared)
{
    if(!selectionScene || !smgr || !selectionScene->isNodeSelected)
        return;

    selectionScene->moveMan->prevTouchPoints[0] = curTouchPos;
    selectionScene->updater->updateControlsOrientaion();
    
    selectionScene->renHelper->rttControlSelectionAnim(curTouchPos);
    if(selectionScene->shaderMGR->deviceType == METAL){
        selectionScene->renHelper->rttControlSelectionAnim(curTouchPos);
        getCtrlColorFromTouchTextureAnim(curTouchPos);
    }
    selectionScene->isRTTCompleted = true;
    
    selectionScene->actionMan->storeActionKeys(false);
}

void SGSelectionManager::postNodeJointSelection()
{
    if(!selectionScene || !smgr)
        return;

    SGNode * currentSelectedNode = (selectionScene->isNodeSelected) ? selectionScene->nodes[selectionScene->selectedNodeId] : NULL;
    if(selectionScene->isJointSelected && selectionScene->selectedJointId <= HIP && currentSelectedNode->joints.size() >= HUMAN_JOINTS_SIZE)
        selectionScene->getIKJointPosition();
    
    if(selectionScene->selectedNodeId > 0) {
        if(currentSelectedNode->joints.size() != HUMAN_JOINTS_SIZE)
            selectionScene->actionMan->setMirrorState(MIRROR_OFF);
    }
    
    if(selectionScene->isJointSelected && selectionScene->selectedJointId != NOT_SELECTED)
        highlightJointSpheres();
    
    if(!selectionScene->isNodeSelected || (currentSelectedNode->getType() != NODE_RIG && currentSelectedNode->getType() != NODE_TEXT))
        selectionScene->renHelper->setJointSpheresVisibility(false);
}

void SGSelectionManager::getCtrlColorFromTouchTextureAnim(Vector2 touchPosition)
{
    if(!selectionScene || !smgr)
        return;
    
    int controlStartIndex = (selectionScene->controlType == MOVE) ? X_MOVE : X_ROTATE;
    int controlEndIndex = (selectionScene->controlType == MOVE) ? Z_MOVE : Z_ROTATE;
    float xCoord = (touchPosition.x/SceneHelper::screenWidth) * selectionScene->touchTexture->width;
    float yCoord = (touchPosition.y/SceneHelper::screenHeight) * selectionScene->touchTexture->height;
    SceneHelper::limitPixelCoordsWithinTextureRange(selectionScene->touchTexture->width, selectionScene->touchTexture->height,xCoord,yCoord);
    Vector3 pixel = smgr->getPixelColor(Vector2(xCoord,yCoord),selectionScene->touchTexture,Vector4(255,255,255,255));
    int selectedCol = (int)pixel.x;
    if(selectedCol != 255 && (selectedCol >= controlStartIndex && selectedCol <= controlEndIndex)){
        selectionScene->isControlSelected = true;
        selectionScene->selectedControlId = selectedCol;
    }else if(selectedCol >= controlStartIndex && selectedCol <= controlEndIndex){
        Logger::log(ERROR,"SGAnimationScene::getCtrlColorFromTouchTextureAnim","Wrong RTT Color");
    }
}

void SGSelectionManager::getNodeColorFromTouchTexture()
{
    if(!selectionScene || !smgr)
        return;

    Vector2 touchPixel = selectionScene->nodeJointPickerPosition;
    if(selectNodeOrJointInPixel(touchPixel))
        return;
    if(selectNodeOrJointInPixel(Vector2(touchPixel.x, touchPixel.y+1.0)))
        return;
    if(selectNodeOrJointInPixel(Vector2(touchPixel.x+1.0, touchPixel.y)))
        return;
    if(selectNodeOrJointInPixel(Vector2(touchPixel.x, touchPixel.y-1.0)))
        return;
    if(selectNodeOrJointInPixel(Vector2(touchPixel.x-1.0, touchPixel.y)))
        return;
    if(selectNodeOrJointInPixel(Vector2(touchPixel.x-1.0, touchPixel.y-1.0)))
        return;
    if(selectNodeOrJointInPixel(Vector2(touchPixel.x+1.0, touchPixel.y+1.0)))
        return;
    if(selectNodeOrJointInPixel(Vector2(touchPixel.x+1.0, touchPixel.y-1.0)))
        return;
    selectNodeOrJointInPixel(Vector2(touchPixel.x-1.0, touchPixel.y+1.0));
}

bool SGSelectionManager::selectNodeOrJointInPixel(Vector2 touchPixel)
{
    if(!selectionScene || !smgr)
        return false;

    float xCoord = (touchPixel.x/SceneHelper::screenWidth) * selectionScene->touchTexture->width;
    float yCoord = (touchPixel.y/SceneHelper::screenHeight) * selectionScene->touchTexture->height;
    
    SceneHelper::limitPixelCoordsWithinTextureRange(selectionScene->touchTexture->width,selectionScene->touchTexture->height,xCoord,yCoord);
    Vector3 pixel = smgr->getPixelColor(Vector2(xCoord,yCoord),selectionScene->touchTexture);
    bool status = updateNodeSelectionFromColor(pixel);
    if(status)
        selectionScene->updater->reloadKeyFrameMap();
    return status;
}

bool SGSelectionManager::updateNodeSelectionFromColor(Vector3 pixel)
{
    if(!selectionScene || !smgr)
        return false;

    int prevSelectedNodeId = selectionScene->selectedNodeId;
    unselectObject(prevSelectedNodeId);
    
    int nodeId = (int) pixel.x,jointId = pixel.y;
    if(nodeId != 255 && nodeId >= selectionScene->nodes.size()){
        Logger::log(ERROR, "SganimationSceneRTT","Wrong Color from RTT texture colorx:" + to_string(nodeId));
        return false;
    }
    
    selectionScene->isNodeSelected = (selectionScene->selectedNodeId = (nodeId != 255) ? nodeId : NOT_EXISTS) != NOT_EXISTS ? true:false;
    if(selectionScene->selectedNodeId != NOT_EXISTS){
        highlightSelectedNode();
        if(selectionScene->nodes[selectionScene->selectedNodeId]->getType() == NODE_RIG || selectionScene->nodes[selectionScene->selectedNodeId]->getType() == NODE_TEXT){
            selectionScene->isJointSelected = (selectionScene->selectedJointId = (jointId != 255) ? jointId : NOT_EXISTS) != NOT_EXISTS ? true:false;
            if(selectionScene->isJointSelected)
                selectionScene->renHelper->displayJointsBasedOnSelection();
        }
    }
    if(selectionScene->isNodeSelected || selectionScene->isJointSelected) {
        selectionScene->updater->updateControlsOrientaion(selectionScene);
        if(selectionScene->isJointSelected)
            highlightJointSpheres();
        return true;
    } else
        return false;
}

void SGSelectionManager::highlightSelectedNode()
{
    if(!selectionScene || !smgr)
        return;
    SGNode* currentSelectedNode = selectionScene->nodes[selectionScene->selectedNodeId];
    
    currentSelectedNode->props.prevMatName = currentSelectedNode->node->material->name;
    currentSelectedNode->props.isSelected = true;
    currentSelectedNode->props.isLighting = false;
    if(currentSelectedNode->getType() == NODE_RIG)
        currentSelectedNode->node->setMaterial(smgr->getMaterialByIndex(SHADER_COLOR_SKIN));
    else if (currentSelectedNode->getType() == NODE_TEXT)
        currentSelectedNode->node->setMaterial(smgr->getMaterialByIndex(SHADER_COLOR_TEXT));
    else
        currentSelectedNode->node->setMaterial(smgr->getMaterialByIndex(SHADER_COLOR));
    
    currentSelectedNode->props.transparency = NODE_SELECTION_TRANSPARENCY;
    selectionScene->selectedNode = currentSelectedNode;
    
    bool status = true;
    if(currentSelectedNode->getType() == NODE_RIG)
        status = selectionScene->renHelper->displayJointSpheresForNode(dynamic_pointer_cast<AnimatedMeshNode>(currentSelectedNode->node));
    else if (currentSelectedNode->getType() == NODE_TEXT)
        status = selectionScene->renHelper->displayJointSpheresForNode(dynamic_pointer_cast<AnimatedMeshNode>(currentSelectedNode->node), currentSelectedNode->props.fontSize/3.0);
    if(!status)
        unselectObject(selectionScene->selectedNodeId);
}

void SGSelectionManager::highlightJointSpheres()
{
    if(!selectionScene || !smgr)
        return;

    if(selectionScene->isNodeSelected  || selectionScene->isJointSelected){
        selectionScene->renHelper->setJointSpheresVisibility(true);
        for(int i = 0; i< selectionScene->jointSpheres.size(); i++)
            if(selectionScene->jointSpheres[i]){
                int mirrorjointId = BoneLimitsHelper::getMirrorJointId(selectionScene->selectedJointId);
                if((i == selectionScene->selectedJointId) || (selectionScene->getMirrorState() && (i == mirrorjointId))) {
                    selectionScene->jointSpheres[i]->props.vertexColor = constants::selectionColor;
                }else{
                    selectionScene->jointSpheres[i]->props.vertexColor = constants::sgrJointDefaultColor;
                }
            }
    }
    selectionScene->updater->updateControlsOrientaion(selectionScene);
}

void SGSelectionManager::selectObject(int objectId)
{
    if(!selectionScene || !smgr)
        return;

    unselectObject(selectionScene->selectedNodeId);
    
    if(objectId < 0 || objectId >= selectionScene->nodes.size() || objectId == selectionScene->selectedNodeId)
        return;
    
    
    
    selectionScene->selectedNodeId = objectId;
    selectionScene->selectedNode = selectionScene->nodes[selectionScene->selectedNodeId];

    selectionScene->isNodeSelected = selectionScene->selectedNode->props.isSelected = true;
    
    selectionScene->selectedNode->props.prevMatName = selectionScene->selectedNode->node->material->name;
    
    if(selectionScene->selectedNode->getType() == NODE_RIG)
        selectionScene->selectedNode->node->setMaterial(smgr->getMaterialByIndex(SHADER_COLOR_SKIN));
    else if (selectionScene->selectedNode->getType() == NODE_TEXT)
        selectionScene->selectedNode->node->setMaterial(smgr->getMaterialByIndex(SHADER_COLOR_TEXT));
    else
        selectionScene->selectedNode->node->setMaterial(smgr->getMaterialByIndex(SHADER_COLOR));
    
    selectionScene->selectedNode->props.transparency = NODE_SELECTION_TRANSPARENCY;
    
    bool status = true;
    if(selectionScene->selectedNode->getType() == NODE_RIG)
        status = selectionScene->renHelper->displayJointSpheresForNode(dynamic_pointer_cast<AnimatedMeshNode>(selectionScene->selectedNode->node));
    else if (selectionScene->selectedNode->getType() == NODE_TEXT)
        status = selectionScene->renHelper->displayJointSpheresForNode(dynamic_pointer_cast<AnimatedMeshNode>(selectionScene->selectedNode->node), selectionScene->selectedNode->props.fontSize/3.0);
    if(!status)
        unselectObject(selectionScene->selectedNodeId);
    
    if(selectionScene->selectedNode->getType() == NODE_RIG || selectionScene->selectedNode->getType() == NODE_TEXT){
        if(selectionScene->selectedNodeId != NOT_SELECTED)
            highlightJointSpheres();
        
    }else if(selectionScene->selectedNode->getType() == NODE_CAMERA){
        selectionScene->selectedNode->props.perVertexColor = false;
    }
    if(!selectionScene->isNodeSelected || (selectionScene->selectedNode->getType() != NODE_RIG && selectionScene->selectedNode->getType() != NODE_TEXT))
        selectionScene->renHelper->setJointSpheresVisibility(false);
    
    selectionScene->updater->updateControlsOrientaion(selectionScene);
    selectionScene->updater->reloadKeyFrameMap();
}

void SGSelectionManager::unselectObject(int objectId)
{
    if(!selectionScene || !smgr)
        return;

    if(objectId != selectionScene->selectedNodeId)
        return;
    
    if(objectId >= 0 && objectId < selectionScene->nodes.size())
    {
        selectionScene->nodes[objectId]->props.transparency = 1.0;
        selectionScene->nodes[objectId]->props.isSelected = false;
        selectionScene->selectedJoint = NULL;
        selectionScene->selectedNode = NULL;
        selectionScene->selectedJointId = NOT_EXISTS;
        selectionScene->selectedNodeId = NOT_EXISTS;
        selectionScene->isJointSelected = selectionScene->isNodeSelected = false;
        selectionScene->updater->resetMaterialTypes(false);
        selectionScene->renHelper->setJointSpheresVisibility(false);
        
        if(selectionScene->nodes[objectId]->getType() == NODE_CAMERA)
            selectionScene->nodes[objectId]->props.perVertexColor = true;
    }
    selectionScene->clearSelections();
    selectionScene->updater->updateControlsOrientaion(selectionScene);
    selectionScene->updater->reloadKeyFrameMap();
}

void SGSelectionManager::checkSelectionForAutoRig(Vector2 touchPosition)
{
    if(!selectionScene || !smgr || !selectionScene->isRigMode)
        return;
    
    switch(selectionScene->rigMan->sceneMode){
        case RIG_MODE_OBJVIEW:
            break;
        case RIG_MODE_MOVE_JOINTS:{
            selectionScene->rigMan->clearNodeSelections();
            selectionScene->renHelper->AttachSkeletonModeRTTSelection(touchPosition);
            if(selectionScene->shaderMGR->deviceType == METAL){
                selectionScene->renHelper->AttachSkeletonModeRTTSelection(touchPosition);
                readSkeletonSelectionTexture();
            }
            break;
        }
        case RIG_MODE_EDIT_ENVELOPES:{
            selectionScene->rigMan->clearNodeSelections();
            selectionScene->renHelper->AttachSkeletonModeRTTSelection(touchPosition);
            if(selectionScene->shaderMGR->deviceType == METAL){
                selectionScene->renHelper->AttachSkeletonModeRTTSelection(touchPosition);
                readSkeletonSelectionTexture();
            }
            break;
        }
        case RIG_MODE_PREVIEW:{
            selectionScene->renHelper->rttSGRNodeJointSelection(touchPosition);
            if(selectionScene->shaderMGR->deviceType == METAL){
                selectionScene->renHelper->rttSGRNodeJointSelection(touchPosition);
                readSGRSelectionTexture();
            }
            break;
        }
        default:
            break;
    }
    
    if(selectionScene->rigMan->sceneMode == RIG_MODE_MOVE_JOINTS && selectionScene->rigMan->selectedNodeId > 0)
        selectionScene->rigMan->selectedJointId = selectionScene->rigMan->selectedNodeId;
    //    if((isSkeletonSelected || isNodeSelected) && controlType == SCALE)
    //        controlType = MOVE;
    selectionScene->updater->updateControlsOrientaion();
}

void SGSelectionManager::readSkeletonSelectionTexture()
{
    if(!selectionScene || !smgr || !selectionScene->isRigMode)
        return;
    
    Vector2 touchPixel = selectionScene->rigMan->touchPosForSkeletonSelection;
    if(selectSkeletonJointInPixel(touchPixel))
        return;
    if(selectSkeletonJointInPixel(Vector2(touchPixel.x, touchPixel.y+1.0)))
        return;
    if(selectSkeletonJointInPixel(Vector2(touchPixel.x+1.0, touchPixel.y)))
        return;
    if(selectSkeletonJointInPixel(Vector2(touchPixel.x, touchPixel.y-1.0)))
        return;
    if(selectSkeletonJointInPixel(Vector2(touchPixel.x-1.0, touchPixel.y)))
        return;
    if(selectSkeletonJointInPixel(Vector2(touchPixel.x-1.0, touchPixel.y-1.0)))
        return;
    if(selectSkeletonJointInPixel(Vector2(touchPixel.x+1.0, touchPixel.y+1.0)))
        return;
    if(selectSkeletonJointInPixel(Vector2(touchPixel.x+1.0, touchPixel.y-1.0)))
        return;
    selectSkeletonJointInPixel(Vector2(touchPixel.x-1.0, touchPixel.y+1.0));
}

bool SGSelectionManager::selectSkeletonJointInPixel(Vector2 touchPixel)
{
    if(!selectionScene || !smgr || !selectionScene->isRigMode || selectionScene->rigMan->sceneMode == RIG_MODE_PREVIEW)
        return false;

    int prevSelectedNodeId = selectionScene->rigMan->selectedNodeId;
    
    float xCoord = (touchPixel.x/SceneHelper::screenWidth) * selectionScene->touchTexture->width;
    float yCoord = (touchPixel.y/SceneHelper::screenHeight) * selectionScene->touchTexture->height;
    SceneHelper::limitPixelCoordsWithinTextureRange(selectionScene->touchTexture->width,selectionScene->touchTexture->height,xCoord,yCoord);
    Vector3 pixel = smgr->getPixelColor(Vector2(xCoord,yCoord),selectionScene->touchTexture);
    
    selectionScene->rigMan->isSkeletonJointSelected = false;
    selectionScene->rigMan->isNodeSelected = true;
    selectionScene->rigMan->selectedJoint = NULL;
    selectionScene->rigMan->selectedNode = NULL;
    
    std::map<int, RigKey>& rigKeys = selectionScene->rigMan->rigKeys;
    
    if(pixel.x != 255.0 && pixel.x != 0.0){
        selectionScene->rigMan->selectedNodeId = (int)pixel.x;
        selectionScene->rigMan->selectedNode = rigKeys[selectionScene->rigMan->selectedNodeId].referenceNode;
        selectionScene->rigMan->isSkeletonJointSelected = true;
    }else{
        selectionScene->rigMan->selectedNodeId = 0;
        selectionScene->rigMan->selectedNode = rigKeys[0].referenceNode;
    }
    updateSkeletonSelectionColors(prevSelectedNodeId);
    selectionScene->updater->updateControlsOrientaion();
    
    return selectionScene->rigMan->selectedNode;
}

void SGSelectionManager::updateSkeletonSelectionColors(int prevSelectedBoneId)
{
    if(!selectionScene || !smgr || !selectionScene->isRigMode)
        return;
    
    std::map<int, RigKey> rigKeys = selectionScene->rigMan->rigKeys;
    
    if(prevSelectedBoneId > 0 && prevSelectedBoneId < selectionScene->tPoseJoints.size() && selectionScene->rigMan->findInRigKeys(prevSelectedBoneId))
        rigKeys[prevSelectedBoneId].sphere->props.vertexColor = Vector3(SGR_JOINT_DEFAULT_COLOR_R,SGR_JOINT_DEFAULT_COLOR_G,SGR_JOINT_DEFAULT_COLOR_B);
    
    if((selectionScene->rigMan->sceneMode == RIG_MODE_MOVE_JOINTS || selectionScene->rigMan->sceneMode == RIG_MODE_EDIT_ENVELOPES) && prevSelectedBoneId > 0) {
        int jointId = BoneLimitsHelper::getMirrorJointId(prevSelectedBoneId);
        if((!selectionScene->getMirrorState() || jointId != NOT_EXISTS) && selectionScene->rigMan->findInRigKeys(jointId))
            rigKeys[jointId].sphere->props.vertexColor = Vector3(SGR_JOINT_DEFAULT_COLOR_R,SGR_JOINT_DEFAULT_COLOR_G,SGR_JOINT_DEFAULT_COLOR_B);
    }
    
    if(selectionScene->rigMan->selectedNodeId > 0 && selectionScene->rigMan->findInRigKeys(selectionScene->rigMan->selectedNodeId))
        rigKeys[selectionScene->rigMan->selectedNodeId].sphere->props.vertexColor = Vector3(SELECTION_COLOR_R,SELECTION_COLOR_G,SELECTION_COLOR_B);
    
    if((selectionScene->rigMan->sceneMode == RIG_MODE_MOVE_JOINTS || selectionScene->rigMan->sceneMode == RIG_MODE_EDIT_ENVELOPES) && selectionScene->rigMan->selectedNodeId > 0) {
        int jointId = BoneLimitsHelper::getMirrorJointId(selectionScene->rigMan->selectedNodeId);
        if(jointId != NOT_EXISTS && selectionScene->getMirrorState() && selectionScene->rigMan->findInRigKeys(jointId))
            rigKeys[jointId].sphere->props.vertexColor = Vector3(SELECTION_COLOR_R,SELECTION_COLOR_G,SELECTION_COLOR_B);
        else if(jointId != NOT_EXISTS && selectionScene->rigMan->findInRigKeys(jointId)) {
            rigKeys[jointId].sphere->props.vertexColor = Vector3(SGR_JOINT_DEFAULT_COLOR_R,SGR_JOINT_DEFAULT_COLOR_G,SGR_JOINT_DEFAULT_COLOR_B);
        }
        
    }
    
    if(selectionScene->rigMan->selectedNodeId == 0 || selectionScene->rigMan->selectedNodeId == NOT_EXISTS) {
        for (int i = 1; i < selectionScene->tPoseJoints.size(); i++) {
            int id = selectionScene->tPoseJoints[i].id;
            if(selectionScene->rigMan->findInRigKeys(id))
                rigKeys[id].sphere->props.vertexColor = Vector3(SGR_JOINT_DEFAULT_COLOR_R,SGR_JOINT_DEFAULT_COLOR_G,SGR_JOINT_DEFAULT_COLOR_B);
        }
    }
    
    if(selectionScene->rigMan->sceneMode == RIG_MODE_EDIT_ENVELOPES){
        selectionScene->renHelper->drawEnvelopes(selectionScene->rigMan->envelopes, selectionScene->rigMan->selectedNodeId);
        if(selectionScene->getMirrorState() == MIRROR_ON && selectionScene->rigMan->selectedNodeId > 0 && BoneLimitsHelper::getMirrorJointId(selectionScene->rigMan->selectedNodeId) != -1)
            selectionScene->renHelper->drawEnvelopes(selectionScene->rigMan->envelopes, BoneLimitsHelper::getMirrorJointId(selectionScene->rigMan->selectedNodeId));
        
        selectionScene->updater->updateEnvelopes();
        selectionScene->updater->updateOBJVertexColor();
    }
    
}

void SGSelectionManager::readSGRSelectionTexture()
{
    if(!selectionScene || !smgr || !selectionScene->isRigMode)
        return;
    Vector2 touchPosForSGRSelection = selectionScene->rigMan->touchPosForSkeletonSelection;
    SGNode *sgrSGNode = selectionScene->rigMan->getRiggedNode();
    
    float xCoord = (touchPosForSGRSelection.x/SceneHelper::screenWidth) * selectionScene->touchTexture->width;
    float yCoord = (touchPosForSGRSelection.y/SceneHelper::screenHeight) * selectionScene->touchTexture->height;
    SceneHelper::limitPixelCoordsWithinTextureRange(selectionScene->touchTexture->width,selectionScene->touchTexture->height,xCoord,yCoord);
    Vector3 pixel = smgr->getPixelColor(Vector2(xCoord,yCoord), selectionScene->touchTexture);
    updateSGRSelection(pixel.x,pixel.y,dynamic_pointer_cast<AnimatedMeshNode>(sgrSGNode->node));
}
void SGSelectionManager::updateSGRSelection(int selectedNodeColor,int selectedJointColor, shared_ptr<AnimatedMeshNode> animNode)
{
    int prevJointId = selectionScene->rigMan->selectedJointId;
    if(selectedNodeColor != 255){
        selectionScene->rigMan->isNodeSelected = true;
        selectionScene->rigMan->selectedNodeId = selectedNodeColor;
        selectionScene->rigMan->selectedNode = selectionScene->rigMan->getRiggedNode();
        selectionScene->rigMan->selectedNode->props.transparency = NODE_SELECTION_TRANSPARENCY;
        if(selectedJointColor >= 1 && selectedJointColor < animNode->getJointCount()){
            selectionScene->rigMan->selectedJointId = selectedJointColor;
            selectionScene->rigMan->isSGRJointSelected = true;
            selectionScene->rigMan->selectedJoint = selectionScene->rigMan->selectedNode->joints[selectionScene->rigMan->selectedJointId];
            highlightJointSpheres();
        }
        bool status = selectionScene->renHelper->displayJointSpheresForNode(animNode);
        if(!status)
            updateSGRSelection(255, 0, animNode);
    }else{
        selectionScene->rigMan->clearNodeSelections();
        selectionScene->renHelper->setJointSpheresVisibility(false);
        selectionScene->rigMan->selectedNodeId = 0;
        selectionScene->rigMan->selectedJointId = NOT_SELECTED;
        selectionScene->rigMan->selectedNode->props.transparency = 1.0;
    }
    highlightJointSpheres();
}

