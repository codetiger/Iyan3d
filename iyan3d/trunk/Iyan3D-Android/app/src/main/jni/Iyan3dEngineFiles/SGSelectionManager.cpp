//
//  SGGestureManager.cpp
//  Iyan3D
//
//  Created by Karthik on 25/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#include "SGSelectionManager.h"
#include "SGEditorScene.h"

SGSelectionManager::SGSelectionManager(SceneManager* sceneMngr)
{
    this->smgr = sceneMngr;
}

SGSelectionManager::~SGSelectionManager()
{
    
}

void SGSelectionManager::checkSelection(void *scene, Vector2 touchPosition,bool isDisplayPrepared)
{
    SGEditorScene *currentScene = (SGEditorScene*)scene;
    
    if(!currentScene || !smgr)
        return;
    
    currentScene->renHelper->rttNodeJointSelection(currentScene, touchPosition);
    if(currentScene->shaderMGR->deviceType == METAL){
        currentScene->renHelper->rttNodeJointSelection(currentScene, touchPosition);
        getNodeColorFromTouchTexture(currentScene);
    }
    currentScene->isRTTCompleted = true;
    
    postNodeJointSelection(currentScene);
    // TODO setTransparencyForIntrudingObjects();
}

void SGSelectionManager::postNodeJointSelection(void *scene)
{
    SGEditorScene *currentScene = (SGEditorScene*)scene;
    
    if(!currentScene || !smgr)
        return;

    SGNode * currentSelectedNode = (currentScene->isNodeSelected) ? currentScene->nodes[currentScene->selectedNodeId] : NULL;
    if(currentScene->isJointSelected && currentScene->selectedJointId <= HIP && currentSelectedNode->joints.size() >= HUMAN_JOINTS_SIZE)
        currentScene->getIKJointPosition();
    
    if(currentScene->selectedNodeId > 0) {
        if(currentSelectedNode->joints.size() != HUMAN_JOINTS_SIZE)
            currentScene->setMirrorState(MIRROR_OFF);
    }
    
    if(currentScene->isJointSelected && currentScene->selectedJointId != NOT_SELECTED)
        highlightJointSpheres(currentScene);
    
    if(!currentScene->isNodeSelected || (currentSelectedNode->getType() != NODE_RIG && currentSelectedNode->getType() != NODE_TEXT))
        currentScene->renHelper->setJointSpheresVisibility(currentScene, false);
}

void SGSelectionManager::getNodeColorFromTouchTexture(void *scene)
{
    SGEditorScene *currentScene = (SGEditorScene*)scene;
    
    if(!currentScene || !smgr)
        return;

    Vector2 touchPixel = currentScene->nodeJointPickerPosition;
    if(selectNodeOrJointInPixel(currentScene, touchPixel))
        return;
    if(selectNodeOrJointInPixel(currentScene, Vector2(touchPixel.x, touchPixel.y+1.0)))
        return;
    if(selectNodeOrJointInPixel(currentScene, Vector2(touchPixel.x+1.0, touchPixel.y)))
        return;
    if(selectNodeOrJointInPixel(currentScene, Vector2(touchPixel.x, touchPixel.y-1.0)))
        return;
    if(selectNodeOrJointInPixel(currentScene, Vector2(touchPixel.x-1.0, touchPixel.y)))
        return;
    if(selectNodeOrJointInPixel(currentScene, Vector2(touchPixel.x-1.0, touchPixel.y-1.0)))
        return;
    if(selectNodeOrJointInPixel(currentScene, Vector2(touchPixel.x+1.0, touchPixel.y+1.0)))
        return;
    if(selectNodeOrJointInPixel(currentScene, Vector2(touchPixel.x+1.0, touchPixel.y-1.0)))
        return;
    selectNodeOrJointInPixel(currentScene, Vector2(touchPixel.x-1.0, touchPixel.y+1.0));
}

bool SGSelectionManager::selectNodeOrJointInPixel(void *scene, Vector2 touchPixel)
{
    SGEditorScene *currentScene = (SGEditorScene*)scene;
    if(!currentScene || !smgr)
        return;

    float xCoord = (touchPixel.x/SceneHelper::screenWidth) * currentScene->touchTexture->width;
    float yCoord = (touchPixel.y/SceneHelper::screenHeight) * currentScene->touchTexture->height;
    
    SceneHelper::limitPixelCoordsWithinTextureRange(currentScene->touchTexture->width,currentScene->touchTexture->height,xCoord,yCoord);
    Vector3 pixel = smgr->getPixelColor(Vector2(xCoord,yCoord),currentScene->touchTexture);
    bool status = updateNodeSelectionFromColor(currentScene, pixel);
    if(status)
        currentScene->reloadKeyFrameMap();
    return status;
}

bool SGSelectionManager::updateNodeSelectionFromColor(void *scene, Vector3 pixel)
{
    SGEditorScene *currentScene = (SGEditorScene*)scene;
    if(!currentScene || !smgr)
        return;

    int prevSelectedNodeId = currentScene->selectedNodeId;
    unselectObject(currentScene, prevSelectedNodeId);
    
    int nodeId = (int) pixel.x,jointId = pixel.y;
    if(nodeId != 255 && nodeId >= currentScene->nodes.size()){
        Logger::log(ERROR, "SganimationSceneRTT","Wrong Color from RTT texture colorx:" + to_string(nodeId));
        return;
    }
    
    currentScene->isNodeSelected = (currentScene->selectedNodeId = (nodeId != 255) ? nodeId : NOT_EXISTS) != NOT_EXISTS ? true:false;
    if(currentScene->selectedNodeId != NOT_EXISTS){
        highlightSelectedNode(currentScene);
        if(currentScene->nodes[currentScene->selectedNodeId]->getType() == NODE_RIG || currentScene->nodes[currentScene->selectedNodeId]->getType() == NODE_TEXT){
            currentScene->isJointSelected = (currentScene->selectedJointId = (jointId != 255) ? jointId : NOT_EXISTS) != NOT_EXISTS ? true:false;
            // TODO if(currentScene->isJointSelected)
                // displayJointsBasedOnSelection(currentScene);
        }
    }
    if(currentScene->isNodeSelected || currentScene->isJointSelected) {
        // TODO updateControlsOrientaion();
        if(currentScene->isJointSelected)
            highlightJointSpheres(currentScene);
        return true;
    } else
        return false;
}

void SGSelectionManager::highlightSelectedNode(void *scene)
{
    SGEditorScene *currentScene = (SGEditorScene*)scene;
    if(!currentScene || !smgr)
        return;
    SGNode* currentSelectedNode = currentScene->nodes[currentScene->selectedNodeId];
    
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
    currentScene->selectedNode = currentSelectedNode;
    
    bool status = true;
    if(currentSelectedNode->getType() == NODE_RIG)
        status = currentScene->renHelper->displayJointSpheresForNode(currentScene, dynamic_pointer_cast<AnimatedMeshNode>(currentSelectedNode->node));
    else if (currentSelectedNode->getType() == NODE_TEXT)
        status = currentScene->renHelper->displayJointSpheresForNode(currentScene, dynamic_pointer_cast<AnimatedMeshNode>(currentSelectedNode->node), currentSelectedNode->props.fontSize/3.0);
    if(!status)
        unselectObject(currentScene, currentScene->selectedNodeId);
}

void SGSelectionManager::highlightJointSpheres(void *scene)
{
    SGEditorScene *currentScene = (SGEditorScene*)scene;
    if(!currentScene || !smgr)
        return;

    if(currentScene->isNodeSelected  || currentScene->isJointSelected){
        currentScene->renHelper->setJointSpheresVisibility(currentScene, true);
        for(int i = 0; i< currentScene->jointSpheres.size(); i++)
            if(currentScene->jointSpheres[i]){
                int mirrorjointId = BoneLimitsHelper::getMirrorJointId(currentScene->selectedJointId);
                if((i == currentScene->selectedJointId) || (currentScene->getMirrorState() && (i == mirrorjointId))) {
                    currentScene->jointSpheres[i]->props.vertexColor = constants::selectionColor;
                }else{
                    currentScene->jointSpheres[i]->props.vertexColor = constants::sgrJointDefaultColor;
                }
            }
    }
    // TODO updateControlsOrientaion();
}

void SGSelectionManager::selectObject(void *scene, int objectId)
{
    SGEditorScene *currentScene = (SGEditorScene*)scene;
    if(!currentScene || !smgr)
        return;

    unselectObject(currentScene, currentScene->selectedNodeId);
    
    if(objectId < 0 || objectId >= currentScene->nodes.size() || objectId == currentScene->selectedNodeId)
        return;
    
    
    
    currentScene->selectedNodeId = objectId;
    currentScene->isNodeSelected = currentScene->selectedNode->props.isSelected = true;
    
    currentScene->selectedNode = currentScene->nodes[currentScene->selectedNodeId];
    
    currentScene->selectedNode->props.prevMatName = currentScene->selectedNode->node->material->name;
    
    if(currentScene->selectedNode->getType() == NODE_RIG)
        currentScene->selectedNode->node->setMaterial(smgr->getMaterialByIndex(SHADER_COLOR_SKIN));
    else if (currentScene->selectedNode->getType() == NODE_TEXT)
        currentScene->selectedNode->node->setMaterial(smgr->getMaterialByIndex(SHADER_COLOR_TEXT));
    else
        currentScene->selectedNode->node->setMaterial(smgr->getMaterialByIndex(SHADER_COLOR));
    
    currentScene->selectedNode->props.transparency = NODE_SELECTION_TRANSPARENCY;
    
    bool status = true;
    if(currentScene->selectedNode->getType() == NODE_RIG)
        status = currentScene->renHelper->displayJointSpheresForNode(currentScene, dynamic_pointer_cast<AnimatedMeshNode>(currentScene->selectedNode->node));
    else if (currentScene->selectedNode->getType() == NODE_TEXT)
        status = currentScene->renHelper->displayJointSpheresForNode(currentScene, dynamic_pointer_cast<AnimatedMeshNode>(currentScene->selectedNode->node), currentScene->selectedNode->props.fontSize/3.0);
    if(!status)
        unselectObject(currentScene, currentScene->selectedNodeId);
    
    if(currentScene->selectedNode->getType() == NODE_RIG || currentScene->selectedNode->getType() == NODE_TEXT){
        if(currentScene->selectedNodeId != NOT_SELECTED)
            highlightJointSpheres(currentScene);
        
    }else if(currentScene->selectedNode->getType() == NODE_CAMERA){
        currentScene->selectedNode->props.perVertexColor = false;
    }
    if(!currentScene->isNodeSelected || (currentScene->selectedNode->getType() != NODE_RIG && currentScene->selectedNode->getType() != NODE_TEXT))
        currentScene->renHelper->setJointSpheresVisibility(currentScene, false);
    
    // TODO updateControlsOrientaion();
    currentScene->reloadKeyFrameMap();
}

void SGSelectionManager::unselectObject(void *scene, int objectId)
{
    SGEditorScene *currentScene = (SGEditorScene*)scene;
    if(!currentScene || !smgr)
        return;

    if(objectId != currentScene->selectedNodeId)
        return;
    
    if(objectId >= 0 && objectId < currentScene->nodes.size())
    {
        currentScene->nodes[objectId]->props.transparency = 1.0;
        currentScene->nodes[objectId]->props.isSelected = false;
        currentScene->selectedJoint = NULL;
        currentScene->selectedNode = NULL;
        currentScene->selectedJointId = NOT_EXISTS;
        currentScene->selectedNodeId = NOT_EXISTS;
        currentScene->isJointSelected = currentScene->isNodeSelected = false;
        // TODO currentScene->resetMaterialTypes(false);
        currentScene->renHelper->setJointSpheresVisibility(currentScene, false);
        
        if(currentScene->nodes[objectId]->getType() == NODE_CAMERA)
            currentScene->nodes[objectId]->props.perVertexColor = true;
    }
    // TODO clearSelections();
    // TODO updateControlsOrientaion();
    currentScene->reloadKeyFrameMap();
}



