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

void SGSelectionManager::checkSelection(Vector2 touchPosition,bool isMultiSelectEnabled, bool isDisplayPrepared)
{
    if(!selectionScene || !smgr)
        return;

   
    selectionScene->renHelper->rttNodeJointSelection(touchPosition,isMultiSelectEnabled);
    if(selectionScene->shaderMGR->deviceType == METAL){
        getNodeColorFromTouchTexture(isMultiSelectEnabled,false);
    }
    selectionScene->isRTTCompleted = true;
    selectionScene->updater->updateControlsOrientaion();
    postNodeJointSelection();
}

bool SGSelectionManager::checkCtrlSelection(Vector2 curTouchPos,bool isDisplayPrepared)
{
    if(!selectionScene || !smgr || (!selectionScene->hasNodeSelected() && selectionScene->selectedNodeIds.size() <= 0))
        return false;

    selectionScene->moveMan->prevTouchPoints[0] = curTouchPos;

    selectionScene->updater->updateControlsOrientaion();
    bool status = false;
    status = selectionScene->renHelper->rttControlSelectionAnim(curTouchPos);
    if(selectionScene->shaderMGR->deviceType == METAL){
        status = getCtrlColorFromTouchTextureAnim(curTouchPos);
    }
    
    if(!status && selectionScene->hasNodeSelected()) {
            if(selectionScene->selectedNodeIds.size() <= 0 && !selectionScene->isRigMode) {
                selectionScene->renHelper->rttNodeJointSelection(curTouchPos, true);
                if(selectionScene->shaderMGR->deviceType == METAL){
                    getNodeColorFromTouchTexture(false,true);
                }
            }
    }
    selectionScene->isRTTCompleted = true;
    
    if(selectionScene->selectedNodeIds.size() > 0)
        selectionScene->actionMan->storeActionKeysForMulti(false);
    else
        selectionScene->actionMan->storeActionKeys(false);

    return status;
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
    
    if(!selectionScene->isNodeSelected || (currentSelectedNode->getType() != NODE_RIG && currentSelectedNode->getType() != NODE_TEXT_SKIN))
        selectionScene->renHelper->setJointSpheresVisibility(false);
}

bool SGSelectionManager::getCtrlColorFromTouchTextureAnim(Vector2 touchPosition)
{
    if(!selectionScene || !smgr)
        return false;
 
    int controlStartIndex = (selectionScene->controlType == MOVE) ? X_MOVE : (selectionScene->controlType == ROTATE) ? X_ROTATE : X_SCALE;
    int controlEndIndex = (selectionScene->controlType == MOVE) ? Z_MOVE : (selectionScene->controlType == ROTATE) ? Z_ROTATE : Z_SCALE;
    float xCoord = (touchPosition.x/SceneHelper::screenWidth) * selectionScene->touchTexture->width;
    float yCoord = (touchPosition.y/SceneHelper::screenHeight) * selectionScene->touchTexture->height;
    SceneHelper::limitPixelCoordsWithinTextureRange(selectionScene->touchTexture->width, selectionScene->touchTexture->height,xCoord,yCoord);
    Vector3 pixel = smgr->getPixelColor(Vector2(xCoord,yCoord),selectionScene->touchTexture,Vector4(255,255,255,255));
    int selectedCol = (int)pixel.x;
    if(selectedCol != 255 && (selectedCol >= controlStartIndex && selectedCol <= controlEndIndex)){
        selectionScene->isControlSelected = true;
        selectionScene->selectedControlId = selectedCol;
        return true;
    }else if(selectedCol >= controlStartIndex && selectedCol <= controlEndIndex){
        Logger::log(ERROR,"SGAnimationScene::getCtrlColorFromTouchTextureAnim","Wrong RTT Color");
    }
    return false;
}

bool SGSelectionManager::getNodeColorFromTouchTexture(bool isMultiSelected,bool touchMove)
{
    if(!selectionScene || !smgr)
        return false;

    Vector2 touchPixel = selectionScene->nodeJointPickerPosition;
    if(selectNodeOrJointInPixel(touchPixel,isMultiSelected, touchMove))
        return true;
    if(selectNodeOrJointInPixel(Vector2(touchPixel.x, touchPixel.y+1.0),isMultiSelected, touchMove))
        return true;
    if(selectNodeOrJointInPixel(Vector2(touchPixel.x+1.0, touchPixel.y),isMultiSelected, touchMove))
        return true;
    if(selectNodeOrJointInPixel(Vector2(touchPixel.x, touchPixel.y-1.0),isMultiSelected, touchMove))
        return true;
    if(selectNodeOrJointInPixel(Vector2(touchPixel.x-1.0, touchPixel.y),isMultiSelected, touchMove))
        return true;
    if(selectNodeOrJointInPixel(Vector2(touchPixel.x-1.0, touchPixel.y-1.0),isMultiSelected, touchMove))
        return true;
    if(selectNodeOrJointInPixel(Vector2(touchPixel.x+1.0, touchPixel.y+1.0),isMultiSelected, touchMove))
        return true;
    if(selectNodeOrJointInPixel(Vector2(touchPixel.x+1.0, touchPixel.y-1.0),isMultiSelected, touchMove))
        return true;
    return selectNodeOrJointInPixel(Vector2(touchPixel.x-1.0, touchPixel.y+1.0),isMultiSelected, touchMove);
}

bool SGSelectionManager::selectNodeOrJointInPixel(Vector2 touchPixel,bool isMultiSelectEnabled, bool touchMove)
{
    
    if(!selectionScene || !smgr)
        return false;

    float xCoord = (touchPixel.x/SceneHelper::screenWidth) * selectionScene->touchTexture->width;
    float yCoord = (touchPixel.y/SceneHelper::screenHeight) * selectionScene->touchTexture->height;
    
    SceneHelper::limitPixelCoordsWithinTextureRange(selectionScene->touchTexture->width,selectionScene->touchTexture->height,xCoord,yCoord);
    Vector3 pixel = smgr->getPixelColor(Vector2(xCoord,yCoord),selectionScene->touchTexture);
    bool status = updateNodeSelectionFromColor(pixel, isMultiSelectEnabled,touchMove);
    if(status) {
        selectionScene->updater->reloadKeyFrameMap();
    }
    return status;
}

bool SGSelectionManager::updateNodeSelectionFromColor(Vector3 pixel,bool isMultipleSelectionEnabled, bool touchMove)
{
    
    if(!selectionScene || !smgr)
        return false;
    
    int nodeId = (int) pixel.x,jointId = pixel.y;
    
    if(nodeId != 255 && nodeId >= selectionScene->nodes.size()){
        Logger::log(ERROR, "SganimationSceneRTT","Wrong Color from RTT texture colorx:" + to_string(nodeId));
        return false;
    } else if (nodeId == 255 && jointId == 255 && !touchMove) {
        if(selectionScene->selectedNodeIds.size() > 0) {
            unselectObjects();
        } else {
            int prevSelectedNodeId = selectionScene->selectedNodeId;
            unselectObject(prevSelectedNodeId);
        }
    }
    
    if(touchMove)
        selectionScene->moveNodeId = (nodeId != 255) ? nodeId : NOT_EXISTS;
    else {
        selectionScene->moveNodeId = NOT_EXISTS;
        if(((selectionScene->isNodeSelected && selectionScene->selectedNodeId != nodeId) || selectionScene->selectedNodeIds.size() > 0) && jointId == 255 && !selectionScene->isJointSelected && isMultipleSelectionEnabled)
            return multipleSelections(nodeId);
        else {
            unselectObject(selectionScene->selectedNodeId);
            selectionScene->isNodeSelected = (selectionScene->selectedNodeId = (nodeId != 255) ? nodeId : NOT_EXISTS) != NOT_EXISTS ? true:false;
        }
    }
    if((selectionScene->selectedNodeId != NOT_EXISTS || (touchMove && selectionScene->moveNodeId != NOT_EXISTS))  && selectionScene->selectedNodeIds.size() <= 0) {
        if(!touchMove)
            highlightSelectedNode();
        if(selectionScene->nodes[selectionScene->selectedNodeId]->getType() == NODE_RIG || selectionScene->nodes[selectionScene->selectedNodeId]->getType() == NODE_TEXT_SKIN){
            if(touchMove)
                selectionScene->moveJointId = (jointId != 255) ? jointId : NOT_EXISTS;
            else {
                selectionScene->moveJointId = NOT_EXISTS;
                selectionScene->isJointSelected = (selectionScene->selectedJointId = (jointId != 255) ? jointId : NOT_EXISTS) != NOT_EXISTS ? true:false;
            }
            if(selectionScene->isJointSelected && !touchMove) {
                selectionScene->renHelper->displayJointsBasedOnSelection();
            }
        }
    }
    if((selectionScene->isNodeSelected || selectionScene->isJointSelected) && !touchMove) {
        selectionScene->updater->updateControlsOrientaion(selectionScene);
        if(selectionScene->isJointSelected)
            highlightJointSpheres();
        return true;
    } else if(selectionScene->moveJointId != NOT_EXISTS || selectionScene->moveNodeId != NOT_EXISTS)
        return true;
    else
        return false;
}

bool SGSelectionManager::multipleSelections(int nodeId)
{
    if(nodeId != 255 && std::find(selectionScene->selectedNodeIds.begin(), selectionScene->selectedNodeIds.end(), nodeId) == selectionScene->selectedNodeIds.end()) {
        selectionScene->isMultipleSelection = true;
        if(std::find(selectionScene->selectedNodeIds.begin(), selectionScene->selectedNodeIds.end(), selectionScene->selectedNodeId) == selectionScene->selectedNodeIds.end() && selectionScene->selectedNodeId != NOT_EXISTS)
            selectionScene->selectedNodeIds.push_back(selectionScene->selectedNodeId);
        selectionScene->selectedNodeIds.push_back(nodeId);
        removeChildren(getParentNode(), true);
        if(parentNode)
            smgr->RemoveNode(parentNode);
        sphereMesh = CSGRMeshFileLoader::createSGMMesh(constants::BundlePath + "/sphere.sgm", selectionScene->shaderMGR->deviceType);
        parentNode = smgr->createNodeFromMesh(sphereMesh, "setUniforms");
        getParentNode()->setVisible(false);
        getParentNode()->setID(PIVOT_ID);
        getParentNode()->setMaterial(smgr->getMaterialByIndex(SHADER_COLOR));
        updateParentPosition();
        highlightSelectedNode(nodeId);
        selectionScene->renHelper->setJointSpheresVisibility(false);
        if(selectionScene->controlType == SCALE && !selectionScene->allObjectsScalable()) {
            selectionScene->controlType = MOVE;
            selectionScene->updater->updateControlsOrientaion();
        }
        selectionScene->clearSelections();
    } else if(nodeId != 255 && std::find(selectionScene->selectedNodeIds.begin(), selectionScene->selectedNodeIds.end(), nodeId) != selectionScene->selectedNodeIds.end()){
        removeChildren(getParentNode(),true);
        if(parentNode)
            smgr->RemoveNode(parentNode);
        for(int i = 0; i < selectionScene->selectedNodeIds.size(); i++) {
            if(selectionScene->selectedNodeIds[i] == nodeId)
                selectionScene->selectedNodeIds.erase(selectionScene->selectedNodeIds.begin() + i);
        }
        if(selectionScene->selectedNodeIds.size() == 0) {
            unselectObject(nodeId);
            unselectObjects();
            return true;
        } else
            unselectObject(nodeId);
        
        sphereMesh = CSGRMeshFileLoader::createSGMMesh(constants::BundlePath + "/sphere.sgm", selectionScene->shaderMGR->deviceType);
        parentNode = smgr->createNodeFromMesh(sphereMesh, "setUniforms");
        getParentNode()->setVisible(false);
        getParentNode()->setID(PIVOT_ID);
        getParentNode()->setMaterial(smgr->getMaterialByIndex(SHADER_COLOR));
        updateParentPosition();
        selectionScene->clearSelections();
    }
    return true;
}

void SGSelectionManager::updateParentPosition()
{
    if(!getParentNode())
        return;
    
    storeGlobalPositions();
    addSelectedChildren(getParentNode());
    static_pointer_cast<Node>(getParentNode())->updateBoundingBox();
    Vector3 pivot = selectionScene->getPivotPoint(true);
    getParentNode()->setPosition(pivot);
    getParentNode()->updateAbsoluteTransformation();
    storeRelativePositions();
    static_pointer_cast<Node>(getParentNode())->updateBoundingBox();
    selectionScene->updater->updateControlsOrientaion();
}

void SGSelectionManager::unselectObjects()
{    
    for(int i = 0; i < selectionScene->selectedNodeIds.size(); i++)
        unselectObject(selectionScene->selectedNodeIds[i]);
    removeChildren(getParentNode(), true);
    if(parentNode) {
        smgr->RemoveNode(parentNode);
        parentNode.reset();
    }
    selectionScene->isMultipleSelection = false;
    selectionScene->selectedNodeIds.clear();
    globalPositions.clear();
    relPositions.clear();
    selectionScene->updater->resetMaterialTypes(false);
}

shared_ptr<Node> SGSelectionManager::getParentNode()
{
    return parentNode;
}

void SGSelectionManager::storeGlobalPositions()
{
    globalPositions.clear();
    for(int i = 0; i < selectionScene->selectedNodeIds.size(); i++)
    {
        globalPositions.push_back(selectionScene->nodes[selectionScene->selectedNodeIds[i]]->node->getAbsolutePosition());
    }
}

void SGSelectionManager::storeRelativePositions()
{
    relPositions.clear();
    for (int i = 0; i < selectionScene->selectedNodeIds.size(); i++) {
        Mat4 parentGlobalMat = selectionScene->nodes[selectionScene->selectedNodeIds[i]]->node->getParent()->getAbsoluteTransformation();
        Vector3 relativePos = MathHelper::getRelativePosition(parentGlobalMat, globalPositions[i]);
        relPositions.push_back(relativePos);
        selectionScene->nodes[selectionScene->selectedNodeIds[i]]->node->setPosition(relativePos);
    }
}

void SGSelectionManager::addSelectedChildren(shared_ptr<Node> toParent)
{
    for(int i = 0; i < selectionScene->selectedNodeIds.size(); i++)
        selectionScene->nodes[selectionScene->selectedNodeIds[i]]->node->setParent((toParent) ? toParent : getParentNode());
}

void SGSelectionManager::removeChildren(shared_ptr<Node> fromParent, bool resetKeys)
{
    vector<Vector3> positions;
    vector<Quaternion> rotations;
    vector<Vector3> scales;

    if(resetKeys) {
        for(int i = 0; i < selectionScene->selectedNodeIds.size(); i++) {
            selectionScene->nodes[selectionScene->selectedNodeIds[i]]->node->updateAbsoluteTransformation();
            positions.push_back(selectionScene->nodes[selectionScene->selectedNodeIds[i]]->node->getAbsolutePosition());
            Vector3 delta = selectionScene->nodes[selectionScene->selectedNodeIds[i]]->node->getAbsoluteTransformation().getRotationInDegree();
                rotations.push_back(Quaternion(delta * DEGTORAD));
                scales.push_back(selectionScene->nodes[selectionScene->selectedNodeIds[i]]->node->getAbsoluteTransformation().getScale());
        }
    }

    if(fromParent)
        fromParent->detachAllChildren();
    else if(getParentNode())
        getParentNode()->detachAllChildren();
    
    for(int i = 0; i < selectionScene->selectedNodeIds.size(); i++) {
        selectionScene->nodes[selectionScene->selectedNodeIds[i]]->node->setParent(shared_ptr<Node>());
        if(resetKeys) {
            selectionScene->nodes[selectionScene->selectedNodeIds[i]]->setPosition(positions[i], selectionScene->currentFrame);
            if(selectionScene->nodes[selectionScene->selectedNodeIds[i]]->getType() != NODE_LIGHT) {
                selectionScene->nodes[selectionScene->selectedNodeIds[i]]->setRotation(rotations[i], selectionScene->currentFrame);
                selectionScene->nodes[selectionScene->selectedNodeIds[i]]->setScale(scales[i], selectionScene->currentFrame);
            }
        }
    }
    if(selectionScene->selectedNodeIds.size())
        selectionScene->updater->setDataForFrame(selectionScene->currentFrame);
}

void SGSelectionManager::highlightSelectedNode(int nodeId)
{
    if(!selectionScene || !smgr)
        return;
    
    int selectedNodeId = (nodeId == -1) ? selectionScene->selectedNodeId : nodeId;
    SGNode* currentSelectedNode = selectionScene->nodes[selectedNodeId];
    
    //currentSelectedNode->props.prevMatName = currentSelectedNode->node->material->name;
    currentSelectedNode->props.isSelected = true;
    //currentSelectedNode->props.isLighting = false;
    if(currentSelectedNode->getType() == NODE_RIG)
        currentSelectedNode->node->setMaterial(smgr->getMaterialByIndex(SHADER_COLOR_SKIN));
    else if (currentSelectedNode->getType() == NODE_TEXT_SKIN)
        currentSelectedNode->node->setMaterial(smgr->getMaterialByIndex(SHADER_COLOR_TEXT));
    else
        currentSelectedNode->node->setMaterial(smgr->getMaterialByIndex(SHADER_COLOR));
    
    currentSelectedNode->props.transparency = NODE_SELECTION_TRANSPARENCY;
    selectionScene->selectedNode = currentSelectedNode;
    
    bool status = true;
    if(selectionScene->selectedNodeIds.size() <= 0) {
        if(currentSelectedNode->getType() == NODE_RIG)
            status = selectionScene->renHelper->displayJointSpheresForNode(dynamic_pointer_cast<AnimatedMeshNode>(currentSelectedNode->node));
        else if (currentSelectedNode->getType() == NODE_TEXT_SKIN)
            status = selectionScene->renHelper->displayJointSpheresForNode(dynamic_pointer_cast<AnimatedMeshNode>(currentSelectedNode->node), currentSelectedNode->props.fontSize/3.0);
    }
    if(!status)
        unselectObject(selectionScene->selectedNodeId);
}

void SGSelectionManager::highlightJointSpheres()
{
    if(!selectionScene || !smgr)
        return;
    bool isNodeSelected = selectionScene->hasNodeSelected();
    bool isJointSelected = selectionScene->hasJointSelected();
    int selectedJointId = (selectionScene->isRigMode) ? selectionScene->rigMan->selectedJointId : selectionScene->selectedJointId;
    
    if((isNodeSelected  || isJointSelected) && selectionScene->selectedNodeIds.size() <= 0) {
        selectionScene->renHelper->setJointSpheresVisibility(true);
        for(int i = 0; i< selectionScene->jointSpheres.size(); i++)
            if(selectionScene->jointSpheres[i]){
                int mirrorjointId = BoneLimitsHelper::getMirrorJointId(selectedJointId);
                if((i == selectedJointId) || (selectionScene->getMirrorState() && (i == mirrorjointId))) {
                    selectionScene->jointSpheres[i]->props.vertexColor = constants::selectionColor;
                }else{
                    selectionScene->jointSpheres[i]->props.vertexColor = constants::sgrJointDefaultColor;
                }
            }
    } else {
        selectionScene->renHelper->setJointSpheresVisibility(false);
    }
    selectionScene->updater->updateControlsOrientaion(selectionScene);
}

void SGSelectionManager::selectObject(int objectId ,bool isMultiSelectionEnabled)
{
    if(!selectionScene || !smgr)
        return;
    if(((selectionScene->isNodeSelected && selectionScene->selectedNodeId != objectId) || selectionScene->selectedNodeIds.size() > 0) && !selectionScene->isJointSelected &&isMultiSelectionEnabled) {
        multipleSelections(objectId);
        return;
    } else
        unselectObject(selectionScene->selectedNodeId);
    
    if(objectId < 0 || objectId >= selectionScene->nodes.size() || objectId == selectionScene->selectedNodeId)
        return;
    
    selectionScene->selectedNodeId = objectId;
    selectionScene->selectedNode = selectionScene->nodes[selectionScene->selectedNodeId];

    selectionScene->isNodeSelected = selectionScene->selectedNode->props.isSelected = true;
    
    //selectionScene->selectedNode->props.prevMatName = selectionScene->selectedNode->node->material->name;
    
    if(selectionScene->selectedNode->getType() == NODE_RIG)
        selectionScene->selectedNode->node->setMaterial(smgr->getMaterialByIndex(SHADER_COLOR_SKIN));
    else if (selectionScene->selectedNode->getType() == NODE_TEXT_SKIN)
        selectionScene->selectedNode->node->setMaterial(smgr->getMaterialByIndex(SHADER_COLOR_TEXT));
    else
        selectionScene->selectedNode->node->setMaterial(smgr->getMaterialByIndex(SHADER_COLOR));
    
    selectionScene->selectedNode->props.transparency = NODE_SELECTION_TRANSPARENCY;
    
    bool status = true;
    if(selectionScene->selectedNode->getType() == NODE_RIG)
        status = selectionScene->renHelper->displayJointSpheresForNode(dynamic_pointer_cast<AnimatedMeshNode>(selectionScene->selectedNode->node));
    else if (selectionScene->selectedNode->getType() == NODE_TEXT_SKIN)
        status = selectionScene->renHelper->displayJointSpheresForNode(dynamic_pointer_cast<AnimatedMeshNode>(selectionScene->selectedNode->node), selectionScene->selectedNode->props.fontSize/3.0);
    if(!status)
        unselectObject(selectionScene->selectedNodeId);
    
    if(selectionScene->selectedNode->getType() == NODE_RIG || selectionScene->selectedNode->getType() == NODE_TEXT_SKIN){
        if(selectionScene->selectedNodeId != NOT_SELECTED)
            highlightJointSpheres();
        
    }else if(selectionScene->selectedNode->getType() == NODE_CAMERA){
        selectionScene->selectedNode->props.perVertexColor = false;
    }
    if(!selectionScene->isNodeSelected || (selectionScene->selectedNode->getType() != NODE_RIG && selectionScene->selectedNode->getType() != NODE_TEXT_SKIN))
        selectionScene->renHelper->setJointSpheresVisibility(false);
    
    selectionScene->updater->updateControlsOrientaion(selectionScene);
    selectionScene->updater->reloadKeyFrameMap();
}

void SGSelectionManager::unselectObject(int objectId)
{
    if(!selectionScene || !smgr)
        return;
    if(objectId > selectionScene->nodes.size())
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
                readSkeletonSelectionTexture();
            }
            break;
        }
        case RIG_MODE_EDIT_ENVELOPES:{
            selectionScene->rigMan->clearNodeSelections();
            selectionScene->renHelper->AttachSkeletonModeRTTSelection(touchPosition);
            if(selectionScene->shaderMGR->deviceType == METAL){
                readSkeletonSelectionTexture();
            }
            break;
        }
        case RIG_MODE_PREVIEW:{
            selectionScene->renHelper->rttSGRNodeJointSelection(touchPosition);
            if(selectionScene->shaderMGR->deviceType == METAL){
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
    
    std::map<int, RigKey>& rigKeys = selectionScene->rigMan->rigKeys;
    
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
            if(selectionScene->rigMan->findInRigKeys(id)){
                rigKeys[id].sphere->props.vertexColor = Vector3(SGR_JOINT_DEFAULT_COLOR_R,SGR_JOINT_DEFAULT_COLOR_G,SGR_JOINT_DEFAULT_COLOR_B);
            }
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
        Vector3 jScale = selectionScene->rigMan->selectedNode->joints[1]->jointNode->getScale();
        printf(" joint scale %f %f %f ", jScale.x, jScale.y, jScale.z);
        if(!status)
            updateSGRSelection(255, 0, animNode);
    }else{
        if(selectionScene->rigMan->selectedNode)
            selectionScene->rigMan->selectedNode->props.transparency = 1.0;
        selectionScene->rigMan->clearNodeSelections();
        selectionScene->renHelper->setJointSpheresVisibility(false);
        selectionScene->rigMan->selectedNodeId = 0;
        selectionScene->rigMan->selectedJointId = NOT_SELECTED;
    }
    highlightJointSpheres();
}

