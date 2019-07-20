//
//  SGActionManager.cpp
//  Iyan3D
//
//  Created by Karthik on 26/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#include "HeaderFiles/SGActionManager.h"
#include "HeaderFiles/SGEditorScene.h"

SGEditorScene* actionScene;

SGActionManager::SGActionManager(SceneManager* smgr, void* scene)
{
    this->smgr = smgr;
    actionScene = (SGEditorScene*)scene;
    mirrorSwitchState = MIRROR_OFF;
    actions.clear();
    currentAction = 0;
}

SGActionManager::~SGActionManager()
{
    changeKeysAction.drop();
    assetAction.drop();
    propertyAction.drop();
    scaleAction.drop();
    addJointAction.drop();
    actions.clear();
}

void SGActionManager::addAction(SGAction& action){
    
    removeActions();
    
    actions.push_back(action);
    while(actions.size() > MAXUNDO){
        actions.erase(actions.begin());
        currentAction--;
    }
    currentAction++;
}

void SGActionManager::finalizeAndAddAction(SGAction& action)
{
    if(!actionScene || !smgr)
        return;
    
    if(action.actionType == ACTION_CHANGE_JOINT_KEYS|| action.actionType == ACTION_CHANGE_NODE_JOINT_KEYS|| action.actionType == ACTION_CHANGE_NODE_KEYS) {
        action.objectIndex = actionScene->nodes[actionScene->selectedNodeId]->actionId;
        if(action.actionType != ACTION_CHANGE_NODE_KEYS)
            action.actionSpecificIntegers.push_back(actionScene->selectedJointId);
        action.frameId = actionScene->currentFrame;
    }
    addAction(action);
}

void SGActionManager::removeActions()
{
    
    if(!actionScene || !smgr)
        return;

    if(currentAction < 0)
        currentAction = 0;
    
    while(actions.size()>currentAction)
        actions.pop_back();
}


bool SGActionManager::isIKJoint(int jointId)
{
    return (jointId == HAND_LEFT || jointId == HAND_RIGHT || jointId == LEG_LEFT || jointId == LEG_RIGHT || jointId == HIP || jointId == LEG_LEFT1 || jointId == LEG_RIGHT1);
}


bool SGActionManager::changeObjectOrientation(Vector3 outputValue, Quaternion outputQuatValue)
{
    if(!actionScene || !smgr || (!actionScene->isNodeSelected && actionScene->selectedNodeIds.size() <= 0))
        return false;
    
    SGNode* selectedNode = (actionScene->selectedNodeIds.size() > 0) ? NULL : actionScene->nodes[actionScene->selectedNodeId];
    
    bool success = false;
    switch(actionScene->controlType){
        case MOVE:{
            if(actionScene->selectedNodeIds.size() > 0 && actionScene->getParentNode()) {
                success = true;
                actionScene->getParentNode()->setPosition(actionScene->getParentNode()->getPosition() + outputValue, true);
                actionScene->getParentNode()->updateAbsoluteTransformation();
                break;
            } else if(actionScene->isJointSelected){
                success = true;
                moveJoint(outputValue);
            }else if(actionScene->isNodeSelected){
                success = true;
                selectedNode->setPosition(selectedNode->node->getPosition() + outputValue, actionScene->currentFrame);
                selectedNode->setPositionOnNode(selectedNode->node->getPosition() + outputValue, true);
                if(selectedNode->getType() == NODE_LIGHT || selectedNode->getType() == NODE_ADDITIONAL_LIGHT)
                    actionScene->updateDirectionLine();
                break;
            }
            break;
        }
        case ROTATE:{
            outputValue *= RADTODEG;
            if(actionScene->selectedNodeIds.size() > 0 && actionScene->getParentNode()) {
                success = true;
                actionScene->getParentNode()->setRotation(outputQuatValue, true);
                break;
            } else if(actionScene->isJointSelected){
                success = true;
                rotateJoint(outputQuatValue);
             }else if(actionScene->isNodeSelected){
                success = true;
                 if(actionScene->directionIndicator->node->getVisible())
                     actionScene->directionIndicator->setRotationOnNode(outputQuatValue);
                 else if (actionScene->directionLine->node->getVisible()) {
                     actionScene->directionLine->setRotationOnNode(outputQuatValue);
                     selectedNode->setRotation(outputQuatValue, actionScene->currentFrame);
                 } else {
                     selectedNode->setRotation(outputQuatValue, actionScene->currentFrame);
                     selectedNode->setRotationOnNode(outputQuatValue, true);
                 }
                break;
            }
            break;
        }
        case SCALE: {
            break;
        }
        default:
            break;
    }
    actionScene->updater->updateControlsOrientaion();
    actionScene->updater->updateLightCamera();
    return success;
}

void SGActionManager::moveJoint(Vector3 outputValue, bool touchMove)
{
    if(actionScene->selectedNodeId == NOT_SELECTED || actionScene->selectedNodeId > actionScene->nodes.size())
        return;
    
    SGNode* selectedNode = actionScene->nodes[actionScene->selectedNodeId];
    SGJoint * selectedJoint = actionScene->selectedJoint;
    
    Vector3 target = (touchMove) ? outputValue : selectedJoint->jointNode->getAbsolutePosition() + outputValue;
    if((selectedNode->joints.size() == HUMAN_JOINTS_SIZE) && isIKJoint(actionScene->selectedJointId)){
        if(actionScene->selectedJointId <= HIP){
            shared_ptr<AnimatedMeshNode> rigNode = dynamic_pointer_cast<AnimatedMeshNode>(selectedNode->node);
            if(rigNode) {
                Vector3 newPosition = (touchMove) ? outputValue : rigNode->getPosition() + outputValue;
                selectedNode->setPosition(newPosition, actionScene->currentFrame);
                selectedNode->setPositionOnNode(newPosition);
                rigNode->setPosition(newPosition, true);
                rigNode->updateAbsoluteTransformation();
                for (actionScene->ikJointsPositoinMapItr = actionScene->ikJointsPositionMap.begin(); actionScene->ikJointsPositoinMapItr != actionScene->ikJointsPositionMap.end(); actionScene->ikJointsPositoinMapItr++){
                    int jointId = ((*actionScene->ikJointsPositoinMapItr).first);
                    selectedNode->MoveBone((shared_ptr<JointNode>)(selectedNode->joints[jointId]->jointNode),actionScene->ikJointsPositionMap.find((*actionScene->ikJointsPositoinMapItr).first)->second,actionScene->currentFrame);
                }
            }
        }else{
            if(selectedJoint)
                selectedNode->MoveBone(selectedJoint->jointNode,target,actionScene->currentFrame);
        }
    } else if(selectedNode->getType() == NODE_TEXT_SKIN){
        
        selectedJoint = selectedNode->joints[actionScene->selectedJointId];
        Vector3 jointLocalPos = selectedJoint->jointNode->getPosition();
        
        Vector3 moveDir = Vector3((actionScene->selectedControlId == X_MOVE ? 1.0 : 0.0),(actionScene->selectedControlId == Y_MOVE ? 1.0 : 0.0),(actionScene->selectedControlId == Z_MOVE ? 1.0 : 0.0));
        
        float trnsValue = (actionScene->selectedControlId == X_MOVE) ? outputValue.x : (actionScene->selectedControlId == Y_MOVE) ? outputValue.y : outputValue.z;
        moveDir = moveDir * trnsValue;
        
        Vector3 position = jointLocalPos + moveDir;
        selectedJoint->setPosition(position, actionScene->currentFrame);
        selectedJoint->setPositionOnNode(position);
        
    }
    else{
        shared_ptr<Node> parent = selectedJoint->jointNode->getParent();
        Vector3 jointPos = selectedJoint->jointNode->getAbsoluteTransformation().getTranslation();
        Quaternion rot = Quaternion(MathHelper::getRelativeParentRotation(selectedJoint->jointNode,(jointPos + outputValue)) * DEGTORAD);
        selectedNode->joints[parent->getID()]->setRotation(rot, actionScene->currentFrame);
        selectedNode->joints[parent->getID()]->setRotationOnNode(rot, true);
        
        if(actionScene->getMirrorState() == MIRROR_ON){
            int mirrorJointId = BoneLimitsHelper::getMirrorJointId(actionScene->selectedJointId);
            if(mirrorJointId != -1) {
                selectedJoint = selectedNode->joints[actionScene->selectedJointId];
                shared_ptr<JointNode> mirrorNode = (dynamic_pointer_cast<AnimatedMeshNode>(selectedNode->node))->getJointNode(mirrorJointId);
                rot = selectedNode->joints[selectedJoint->jointNode->getParent()->getID()]->jointNode->getRotation() * Quaternion(1.0, -1.0, -1.0);
                selectedNode->joints[mirrorNode->getParent()->getID()]->setRotation(rot,actionScene->currentFrame);
                selectedNode->joints[mirrorNode->getParent()->getID()]->setRotationOnNode(rot, true);
            }
        }
    }
// TODO For CPU Skin update mesh cache check performance
}

void SGActionManager::rotateJoint(Quaternion outputValue)
{
    SGNode* selectedNode = actionScene->nodes[actionScene->selectedNodeId];
    SGJoint * selectedJoint = actionScene->selectedJoint;

    selectedJoint->setRotation(outputValue, actionScene->currentFrame);
    selectedJoint->setRotationOnNode(outputValue, true);
    if(actionScene->getMirrorState() == MIRROR_ON){
        int mirrorJointId = BoneLimitsHelper::getMirrorJointId(actionScene->selectedJointId);
        if(mirrorJointId != -1) {
            Quaternion rot = Quaternion(outputValue.x, -outputValue.y, -outputValue.z, outputValue.w);
            selectedNode->joints[mirrorJointId]->setRotation(rot, actionScene->currentFrame);
            selectedNode->joints[mirrorJointId]->setRotationOnNode(rot, true);
        }
    }
    // TODO For CPU Skin update mesh cache check performance
}

void SGActionManager::storeActionKeys(bool finished)
{
    if(!actionScene || !smgr || !actionScene->isNodeSelected)
        return;
    
    SGNode * selectedNode = actionScene->nodes[actionScene->selectedNodeId];
    if(actionScene->selectedControlId != NOT_SELECTED || actionScene->moveNodeId != NOT_EXISTS || actionScene->controlType == SCALE) {
        actionScene->isControlSelected = (actionScene->moveNodeId != NOT_EXISTS || actionScene->selectedControlId == NOT_SELECTED) ? false : true;
        if(!finished && actionScene->controlType != SCALE)
            changeKeysAction.drop();
        changeKeysAction.objectIndex = selectedNode->actionId;
        
        if(!actionScene->isJointSelected){
            changeKeysAction.actionType = ACTION_CHANGE_NODE_KEYS;
            changeKeysAction.keys.push_back(selectedNode->getKeyForFrame(actionScene->currentFrame));
            if(finished) {
                if(selectedNode->getType() == NODE_LIGHT || selectedNode->getType() == NODE_ADDITIONAL_LIGHT)
                    actionScene->updater->updateLightProperties(actionScene->currentFrame);
            }
        }
        else if(actionScene->isJointSelected){
            if(selectedNode->joints.size() >= HUMAN_JOINTS_SIZE && actionScene->selectedJointId == HIP) {
                changeKeysAction.actionType = ACTION_CHANGE_NODE_JOINT_KEYS;
                changeKeysAction.keys.push_back(selectedNode->getKeyForFrame(actionScene->currentFrame));
                for(unsigned long i=0; i<selectedNode->joints.size(); i++){
                    changeKeysAction.keys.push_back(selectedNode->joints[i]->getKeyForFrame(actionScene->currentFrame));
                }
            }
            else {
                changeKeysAction.actionType = ACTION_CHANGE_JOINT_KEYS;
                for(unsigned long i=0; i<selectedNode->joints.size(); i++){
                    changeKeysAction.keys.push_back(selectedNode->joints[i]->getKeyForFrame(actionScene->currentFrame));
                }
            }
        }
        
        actionScene->updater->reloadKeyFrameMap();
        if(finished) {
            finalizeAndAddAction(changeKeysAction);
            changeKeysAction.drop();
        }
    }
}

void SGActionManager::storeActionKeysForMulti(bool finished)
{
    if(!actionScene || !smgr || !(actionScene->selectedNodeIds.size() > 0))
        return;
    if(actionScene->selectedControlId != NOT_SELECTED || actionScene->controlType == SCALE) {
        actionScene->isControlSelected = (actionScene->selectedControlId == NOT_SELECTED) ? false : true;
        if(!finished && actionScene->controlType != SCALE)
            changeKeysAction.drop();
        changeKeysAction.actionType = ACTION_CHANGE_MULTI_NODE_KEYS;
        SGNode * selectedNode;
        for(int i = 0; i < actionScene->selectedNodeIds.size(); i++){
            selectedNode = actionScene->nodes[actionScene->selectedNodeIds[i]];
            changeKeysAction.actionSpecificIntegers.push_back(selectedNode->actionId);
            changeKeysAction.keys.push_back(selectedNode->getKeyForFrame(actionScene->currentFrame));
            if(finished) {
                if(selectedNode->getType() == NODE_LIGHT || selectedNode->getType() == NODE_ADDITIONAL_LIGHT)
                    actionScene->updater->updateLightProperties(actionScene->currentFrame);
            }
        }
        if(finished){
            changeKeysAction.frameId = actionScene->currentFrame;
            addAction(changeKeysAction);
            changeKeysAction.drop();
        }
    }
}

void SGActionManager::switchFrame(int frame)
{
    if(!actionScene || !smgr)
        return;

    if(!actionScene->isPlaying) {
        SGAction switchFrameAction;
        switchFrameAction.actionType = ACTION_SWITCH_FRAME;
        switchFrameAction.actionSpecificIntegers.push_back(actionScene->previousFrame);
        switchFrameAction.frameId = actionScene->currentFrame;
        addAction(switchFrameAction);
    }

    actionScene->updatePhysics(frame);
    actionScene->selectMan->removeChildren(actionScene->getParentNode());
    actionScene->updater->setDataForFrame(frame);
    actionScene->selectMan->updateParentPosition();
}

void SGActionManager::removeActionsWithActionId(int actionId)
{
    for( int i = actions.size()-1; i >= 0; i--) {
        if(actions[i].objectIndex == actionId) {
            actions.erase(actions.begin()+i);
            if(currentAction >= i)
                currentAction--;
        }
    }
}

void SGActionManager::storePropertyChangeAction(int selectedNodeId, Vector4 value, PROP_INDEX pIndex, bool isChanged, bool isSceneProp)
{
    if((selectedNodeId == NOT_SELECTED || selectedNodeId >= actionScene->nodes.size()) && !isSceneProp)
        return;
    
    SGNode *sgNode = (isSceneProp) ? NULL : actionScene->nodes[selectedNodeId];
    
    if(propertyAction.actionType == ACTION_EMPTY) {
        
        propertyAction.actionType = ACTION_NUMERICAL_PROPERTY_CHANGE;
        propertyAction.objectIndex = (isSceneProp) ? NOT_EXISTS : sgNode->actionId;
        propertyAction.frameId = actionScene->currentFrame;
        propertyAction.actionSpecificIntegers.push_back((int)pIndex);
        propertyAction.actionSpecificIntegers.push_back(actionScene->selectedMeshBufferId);
        Vector4 prevVal = (isSceneProp) ? actionScene->shaderMGR->getProperty(pIndex).value : sgNode->getProperty(pIndex, actionScene->selectedMeshBufferId).value;
        propertyAction.actionSpecificFlags.push_back(isSceneProp);
        propertyAction.actionSpecificFloats.push_back(prevVal.x);
        propertyAction.actionSpecificFloats.push_back(prevVal.y);
        propertyAction.actionSpecificFloats.push_back(prevVal.z);
        propertyAction.actionSpecificFloats.push_back(prevVal.w);
    }
    
    if(isChanged) {
        propertyAction.actionSpecificFloats.push_back(value.x);
        propertyAction.actionSpecificFloats.push_back(value.y);
        propertyAction.actionSpecificFloats.push_back(value.z);
        propertyAction.actionSpecificFloats.push_back(value.w);
        finalizeAndAddAction(propertyAction);
        propertyAction.drop();
    }

}

void SGActionManager::storeTexturesChangeAction(int selectedNodeId, std::string fileName, Vector4 color, PROP_INDEX pIndex, bool hasMeshColor, bool isChanged, bool isSceneProp)
{
    if(propertyAction.actionType == ACTION_EMPTY) {
        
        propertyAction.actionType = ACTION_TEXTURE_CHANGE;
        propertyAction.objectIndex = (isSceneProp) ? NOT_SELECTED : (actionScene->nodes[selectedNodeId]->actionId);
        
        propertyAction.actionSpecificIntegers.push_back(actionScene->selectedMeshBufferId);
        propertyAction.actionSpecificIntegers.push_back(pIndex);
        
        std::string prevFileName = (isSceneProp) ? actionScene->shaderMGR->getProperty(pIndex).fileName : actionScene->nodes[selectedNodeId]->getProperty(pIndex, actionScene->selectedMeshBufferId).fileName;
        propertyAction.actionSpecificStrings.push_back(ConversionHelper::getWStringForString(prevFileName));
        
        
        Vector4 vColor = (isSceneProp) ? Vector4(0.0) : actionScene->nodes[selectedNodeId]->getProperty(VERTEX_COLOR, actionScene->selectedMeshBufferId).value;
        propertyAction.actionSpecificFloats.push_back(vColor.x);
        propertyAction.actionSpecificFloats.push_back(vColor.y);
        propertyAction.actionSpecificFloats.push_back(vColor.z);
        
        propertyAction.actionSpecificFlags.push_back(isSceneProp);
        bool isVColor = (isSceneProp) ? false : actionScene->nodes[selectedNodeId]->getProperty(IS_VERTEX_COLOR, actionScene->selectedMeshBufferId).value.x;
        propertyAction.actionSpecificFlags.push_back(isVColor);
        
    }
    
    if(isChanged) {
        propertyAction.actionSpecificStrings.push_back(ConversionHelper::getWStringForString(fileName));
        
        Vector4 vColor = (isSceneProp) ? Vector4(0.0) : color;
        propertyAction.actionSpecificFloats.push_back(vColor.x);
        propertyAction.actionSpecificFloats.push_back(vColor.y);
        propertyAction.actionSpecificFloats.push_back(vColor.z);
        
        bool isVColor = (isSceneProp) ? false : hasMeshColor;
        propertyAction.actionSpecificFlags.push_back(isVColor);
        finalizeAndAddAction(propertyAction);
        propertyAction.drop();
    }
}

void SGActionManager::updatePropsOnUndoRedo(PROP_INDEX pIndex, Vector4 value, int selectedNodeId, int frame)
{
    switch (pIndex) {
        case LIGHT_TYPE: {
            for(int pI = LIGHT_POINT; pI < LIGHT_DIRECTIONAL + 1; pI++) {
                if(pI - LIGHT_CONSTANT == (int)value.x)
                    actionScene->nodes[selectedNodeId]->getProperty((PROP_INDEX)pI).value.x = 1.0;
                else
                    actionScene->nodes[selectedNodeId]->getProperty((PROP_INDEX)pI).value.x = 0.0;
            }
            actionScene->updater->updateLightProperties(actionScene->currentFrame);
            break;
        }
        case PHYSICS_KIND: {
            for(int pI = (int)PHYSICS_STATIC; pI <= (int)PHYSICS_JELLY; pI++) {
                if(pI == value.x)
                    actionScene->nodes[selectedNodeId]->getProperty((PROP_INDEX)pI).value.x = 1.0;
                else
                    actionScene->nodes[selectedNodeId]->getProperty((PROP_INDEX)pI).value.x = 1.0;
            }
            
            int pType = value.x;
            
            if(pType != PHYSICS_NONE) {
                actionScene->syncSceneWithPhysicsWorld();
                actionScene->setPropsOfObject(actionScene->nodes[selectedNodeId], pType);
            }
            
            break;
        }
        case CAM_RESOLUTION: {
            for(int pI = THOUSAND_EIGHTY; pI < TWO_FORTY + 1; pI++) {
                if(pI - CAM_CONSTANT == value.x)
                    actionScene->nodes[NODE_CAMERA]->getProperty((PROP_INDEX)pI).value.x = 1.0;
                else
                    actionScene->nodes[NODE_CAMERA]->getProperty((PROP_INDEX)pI).value.x = 0.0;
            }
            break;
        }
        case VISIBILITY: {
            actionScene->nodes[selectedNodeId]->setVisibility(value.x, frame);
            break;
        }
            
            
        default:
            break;
    }
}

void SGActionManager::changeMeshProperty(float refraction, float reflection, bool isLighting, bool isVisible, bool isChanged)
{
    if(!actionScene || !smgr || !actionScene->isNodeSelected)
        return;
    
    SGNode *selectedNode = actionScene->nodes[actionScene->selectedNodeId];
    int matIndex = (actionScene->selectedMeshBufferId == NOT_SELECTED) ? 0 : actionScene->selectedMeshBufferId;
    
    std::map<PROP_INDEX, Property> physicsProps = selectedNode->getProperty(HAS_PHYSICS).subProps;
    
    selectedNode->setMeshProperties(matIndex, refraction, reflection, isLighting, isVisible, selectedNode->getProperty(HAS_PHYSICS).value.x, physicsProps[PHYSICS_KIND].value.x, physicsProps[FORCE_MAGNITUDE].value.x, actionScene->currentFrame);
    
    actionScene->updater->setDataForFrame(actionScene->currentFrame);
}

void SGActionManager::changeUVScale(int nodeIndex, int meshBufferIndex, float scale)
{
    if(actionScene && nodeIndex < actionScene->nodes.size()) {
        actionScene->nodes[nodeIndex]->getProperty(TEXTURE_SCALE, meshBufferIndex).value.x = scale;//Vector4(scale, 0, 0, true), MATERIAL_PROPS);
    }
}

void SGActionManager::changeCameraProperty(float fov , int resolutionType, bool isChanged)
{
    if(!actionScene || !smgr)
        return;

    actionScene->updater->setCameraProperty(fov, resolutionType);
    
}

void SGActionManager::changeLightProperty(float red , float green, float blue, float shadow,float distance, int lightType, bool isChanged)
{
    if(!actionScene || !smgr || actionScene->selectedNodeId == NOT_EXISTS)
        return;
    
    ShaderManager::lightChanged = true;
    
    SGNode *selectedNode = actionScene->nodes[actionScene->selectedNodeId];
    
    if(selectedNode->getType() == NODE_ADDITIONAL_LIGHT) {
        selectedNode->getProperty(SPECIFIC_FLOAT).value.x = (distance);
    }
    
    if(selectedNode->getType() == NODE_LIGHT || selectedNode->getType() == NODE_ADDITIONAL_LIGHT) {
        ShaderManager::shadowDensity = shadow;
        selectedNode->getProperty(SHADOW_DARKNESS).value.x = shadow;
        selectedNode->getProperty(LIGHT_TYPE).value.x = (int)lightType;

        for(int pI = LIGHT_POINT; pI < LIGHT_DIRECTIONAL + 1; pI++) {
            if(pI - LIGHT_CONSTANT == (int)lightType)
                selectedNode->getProperty((PROP_INDEX)pI).value.x = 1.0;
            else
                selectedNode->getProperty((PROP_INDEX)pI).value.x = 0.0;
        }
    }
    
    //nodes[selectedNodeId]->getProperty(VERTEX_COLOR).value = Vector3(red,green,blue);
    Vector3 mainLightColor = Vector3(red,green,blue);
    selectedNode->setScale(mainLightColor, actionScene->currentFrame);
    
    actionScene->updater->updateLightProperties(actionScene->currentFrame);
    //    updateLightWithRender();
}

void SGActionManager::storeLightPropertyChangeAction(float red , float green , float blue , float shadowDensity, float distance, int lightType)
{
    if(!actionScene || !smgr || actionScene->selectedNodeId == NOT_EXISTS)
        return;
    SGNode* selectedNode = actionScene->nodes[actionScene->selectedNodeId];
    changeKeysAction.keys.push_back(selectedNode->getKeyForFrame(actionScene->currentFrame));
    
    propertyAction.actionSpecificFloats.push_back(shadowDensity);
    propertyAction.actionSpecificFloats.push_back(distance);
    propertyAction.actionSpecificIntegers.push_back(lightType);
    
    if(propertyAction.actionSpecificFloats[0] != shadowDensity || (actionScene->nodes[actionScene->selectedNodeId]->getType() == NODE_ADDITIONAL_LIGHT && propertyAction.actionSpecificFloats[1] != distance) || propertyAction.actionSpecificIntegers[0] != lightType) {
        finalizeAndAddAction(propertyAction);
    } else
        finalizeAndAddAction(changeKeysAction);
    
    if(actionScene->nodes[actionScene->selectedNodeId]->getProperty(LIGHT_TYPE).value.x == POINT_LIGHT)
        actionScene->updateDirectionLine();
    
    changeKeysAction.drop();
    propertyAction.drop();
}

void SGActionManager::setMirrorState(MIRROR_SWITCH_STATE flag)
{
    if(!actionScene || !smgr)
        return;

    mirrorSwitchState = flag;
    if(actionScene->isJointSelected)
        actionScene->selectMan->highlightMeshBufferAndJointSpheres();
}

bool SGActionManager::switchMirrorState()
{
    if(!actionScene || !smgr || actionScene->selectedNodeId == NOT_EXISTS)
        return false;

    SGAction action;
    action.actionType = ACTION_CHANGE_MIRROR_STATE;
    action.actionSpecificFlags.push_back(mirrorSwitchState);
    finalizeAndAddAction(action);
    setMirrorState((MIRROR_SWITCH_STATE)!mirrorSwitchState);
    return mirrorSwitchState;
}

MIRROR_SWITCH_STATE SGActionManager::getMirrorState()
{
    return mirrorSwitchState;
}

void SGActionManager::storeAddOrRemoveAssetAction(int actionType, int assetId, string optionalFilePath)
{
    if(!actionScene || !smgr)
        return;

    if (actionType == ACTION_APPLY_ANIM) {
        
        assetAction.drop();
        assetAction.actionType = ACTION_APPLY_ANIM;
        assetAction.actionSpecificStrings.push_back(ConversionHelper::getWStringForString(actionScene->animMan->animFilePath));
        assetAction.objectIndex = actionScene->nodes[actionScene->selectedNodeId]->actionId;
        assetAction.frameId = actionScene->animMan->animStartFrame;
        assetAction.actionSpecificIntegers.push_back(actionScene->animMan->animTotalFrames);
        addAction(assetAction);
    }
    else if(actionType == ACTION_ADD_BONE){
        assetAction.drop();
        assetAction.actionType = ACTION_ADD_BONE;
        SGNode * selectedNode = actionScene->nodes[actionScene->selectedNodeId];
        assetAction.frameId = selectedNode->assetId;
        assetAction.objectIndex = selectedNode->actionId;
        assetAction.actionSpecificStrings.push_back(ConversionHelper::getWStringForString(actionScene->nodes[actionScene->selectedNodeId]->oriTextureName));
        StoreDeleteObjectKeys(actionScene->selectedNodeId);
        assetAction.actionSpecificStrings.push_back(ConversionHelper::getWStringForString(actionScene->nodes[actionScene->selectedNodeId]->oriTextureName));
        assetAction.actionSpecificStrings.push_back(ConversionHelper::getWStringForString(actionScene->nodes[actionScene->selectedNodeId]->getProperty(TEXTURE).fileName));
        assetAction.actionSpecificFloats.push_back(actionScene->nodes[actionScene->selectedNodeId]->getProperty(ORIG_VERTEX_COLOR).value.x);
        assetAction.actionSpecificFloats.push_back(actionScene->nodes[actionScene->selectedNodeId]->getProperty(ORIG_VERTEX_COLOR).value.y);
        assetAction.actionSpecificFloats.push_back(actionScene->nodes[actionScene->selectedNodeId]->getProperty(ORIG_VERTEX_COLOR).value.z);
        assetAction.actionSpecificFloats.push_back(actionScene->nodes[actionScene->selectedNodeId]->getProperty(VERTEX_COLOR).value.x);
        assetAction.actionSpecificFloats.push_back(actionScene->nodes[actionScene->selectedNodeId]->getProperty(VERTEX_COLOR).value.y);
        assetAction.actionSpecificFloats.push_back(actionScene->nodes[actionScene->selectedNodeId]->getProperty(VERTEX_COLOR).value.z);
        assetAction.actionSpecificFlags.push_back(actionScene->nodes[actionScene->selectedNodeId]->getProperty(IS_VERTEX_COLOR).value.x);
        assetAction.options = actionScene->nodes[actionScene->selectedNodeId]->options;

        addAction(assetAction);
    }
    else if(actionType == ACTION_TEXTURE_CHANGE){
        assetAction.drop();
        assetAction.actionType = ACTION_TEXTURE_CHANGE;
        assetAction.objectIndex = (actionScene->nodes[actionScene->selectedNodeId]->actionId);
        assetAction.actionSpecificIntegers.push_back(actionScene->selectedMeshBufferId);
        assetAction.actionSpecificStrings.push_back(ConversionHelper::getWStringForString(actionScene->nodes[actionScene->selectedNodeId]->oriTextureName));
        assetAction.actionSpecificStrings.push_back(ConversionHelper::getWStringForString(actionScene->nodes[actionScene->selectedNodeId]->getProperty(TEXTURE).fileName));
        assetAction.actionSpecificFloats.push_back(actionScene->nodes[actionScene->selectedNodeId]->getProperty(ORIG_VERTEX_COLOR).value.x);
        assetAction.actionSpecificFloats.push_back(actionScene->nodes[actionScene->selectedNodeId]->getProperty(ORIG_VERTEX_COLOR).value.y);
        assetAction.actionSpecificFloats.push_back(actionScene->nodes[actionScene->selectedNodeId]->getProperty(ORIG_VERTEX_COLOR).value.z);
        assetAction.actionSpecificFloats.push_back(actionScene->nodes[actionScene->selectedNodeId]->getProperty(VERTEX_COLOR).value.x);
        assetAction.actionSpecificFloats.push_back(actionScene->nodes[actionScene->selectedNodeId]->getProperty(VERTEX_COLOR).value.y);
        assetAction.actionSpecificFloats.push_back(actionScene->nodes[actionScene->selectedNodeId]->getProperty(VERTEX_COLOR).value.z);
        addAction(assetAction);
    }
}

void SGActionManager::StoreDeleteObjectKeys(int nodeIndex)
{
    if(!actionScene || !smgr)
        return;
    
    SGNode *sgNode = actionScene->nodes[nodeIndex];

    if(sgNode->positionKeys.size())
        assetAction.nodePositionKeys = sgNode->positionKeys;
    if(sgNode->rotationKeys.size())
        assetAction.nodeRotationKeys = sgNode->rotationKeys;
    if(sgNode->scaleKeys.size())
        assetAction.nodeSCaleKeys = sgNode->scaleKeys;
    if(sgNode->visibilityKeys.size())
        assetAction.nodeVisibilityKeys = sgNode->visibilityKeys;
    for (int i = 0; i < (int)sgNode->joints.size(); i++) {
        //assetAction.jointsRotationKeys[i] = nodes[nodeIndex]->joints[i]->rotationKeys;
        assetAction.jointRotKeys[i] = sgNode->joints[i]->rotationKeys;
        if(sgNode->getType() == NODE_TEXT_SKIN){
            assetAction.jointPosKeys[i] = sgNode->joints[i]->positionKeys;
            assetAction.jointScaleKeys[i] = sgNode->joints[i]->scaleKeys;
        }
    }
}

void SGActionManager::changeObjectScale(Vector3 scale, bool isChanged)
{
    if((actionScene->isJointSelected  && actionScene->nodes[actionScene->selectedNodeId]->getType() != NODE_TEXT_SKIN) || (!actionScene->isNodeSelected && actionScene->selectedNodeIds.size() <= 0)) return;

    if(changeKeysAction.actionType == ACTION_EMPTY) {
        if(actionScene->selectedNodeIds.size() > 0)
            storeActionKeysForMulti(false);
        else
            storeActionKeys(false);
    }


    if(actionScene->selectedNodeIds.size() > 0) {
        Vector3 pScale = actionScene->getParentNode()->getScale();
        actionScene->getParentNode()->setScale(scale);
        actionScene->getParentNode()->updateAbsoluteTransformation();
    } else if(actionScene->isJointSelected && actionScene->nodes[actionScene->selectedNodeId]->getType() == NODE_TEXT_SKIN){
        actionScene->nodes[actionScene->selectedNodeId]->joints[actionScene->selectedJointId]->setScale(scale, actionScene->currentFrame);
    } else {
        actionScene->nodes[actionScene->selectedNodeId]->setScale(scale, actionScene->currentFrame);
    }

    if(isChanged) {
        if(actionScene->selectedNodeIds.size() > 0) {
            actionScene->selectMan->removeChildren(actionScene->getParentNode(), true);
            actionScene->selectMan->updateParentPosition();
            storeActionKeysForMulti(true);
        } else {
            storeActionKeys(true);
        }

        if(actionScene->selectedNodeIds.size() <= 0)
            actionScene->updater->setDataForFrame(actionScene->currentFrame);
    }

    if(actionScene->selectedNodeIds.size() <= 0)
        actionScene->updater->setDataForFrame(actionScene->currentFrame,false);

    
    actionScene->updater->reloadKeyFrameMap();
    actionScene->updater->updateControlsOrientaion();
}

void SGActionManager::removeDemoAnimation()
{
    SGAction &recentAction = actions[currentAction-1];
    int objectIndex = recentAction.objectIndex;
        int indexOfAction = getObjectIndex(objectIndex);
        actionScene->selectedNodeId = indexOfAction;
        actionScene->animMan->removeAppliedAnimation(recentAction.frameId, recentAction.actionSpecificIntegers[0]);
        actions.erase(actions.begin()+(currentAction-1));
        currentAction--;
    
}

int SGActionManager::undo(int &returnValue2)
{
    if(!actionScene || !smgr)
        return -1;


    if(actionScene->selectedNodeIds.size() > 0)
        actionScene->selectMan->unselectObjects();
    
    int returnValue = DO_NOTHING;
    
    SGAction &recentAction = actions[currentAction-1];
    int indexOfAction = 0;
        
    if(currentAction <= 0) {
        currentAction = 0;
        return (actions.size() > 0) ? DEACTIVATE_UNDO:DEACTIVATE_BOTH;
    }
    returnValue2 = indexOfAction = getObjectIndex(recentAction.objectIndex);
    
    switch(recentAction.actionType){
        case ACTION_CHANGE_NODE_KEYS:
            actionScene->nodes[indexOfAction]->setKeyForFrame(recentAction.frameId, recentAction.keys[0]);
            actionScene->updater->reloadKeyFrameMap();
            break;
        case ACTION_CHANGE_MULTI_NODE_KEYS:
            for (int i =0; i < recentAction.actionSpecificIntegers.size()/2; i++) {
                if(recentAction.actionSpecificIntegers[i] != NOT_EXISTS){
                    actionScene->nodes[getObjectIndex(recentAction.actionSpecificIntegers[i])]->setKeyForFrame(recentAction.frameId, recentAction.keys[i]);
                    actionScene->updater->reloadKeyFrameMap();
                }
            }
            break;
        case ACTION_CHANGE_JOINT_KEYS: {
            for(int i = 0; i <(int)actionScene->nodes[indexOfAction]->joints.size(); i++){
                actionScene->nodes[indexOfAction]->joints[i]->setKeyForFrame(recentAction.frameId, recentAction.keys[i]);
            }
            actionScene->updater->reloadKeyFrameMap();
            break;
            
        }
        case ACTION_CHANGE_NODE_JOINT_KEYS:{
            actionScene->nodes[indexOfAction]->setKeyForFrame(recentAction.frameId, recentAction.keys[0]);
            for(unsigned long i=0; i < actionScene->nodes[indexOfAction]->joints.size(); i++){
                actionScene->nodes[indexOfAction]->joints[i]->setKeyForFrame(recentAction.frameId, recentAction.keys[i+1]);
            }
            actionScene->updater->reloadKeyFrameMap();
            break;
        }
        case ACTION_SWITCH_FRAME:{
            actionScene->currentFrame = recentAction.actionSpecificIntegers[0];
            returnValue = SWITCH_FRAME;
            break;
        }
        case ACTION_NUMERICAL_PROPERTY_CHANGE: {
            bool isSceneProp = recentAction.actionSpecificFlags[0];
            if((indexOfAction != NOT_SELECTED && indexOfAction < actionScene->nodes.size()) || isSceneProp) {
                PROP_INDEX pIndex = (PROP_INDEX)recentAction.actionSpecificIntegers[0];
                int matIndex = recentAction.actionSpecificIntegers[1];
                Vector4 value;
                value.x = recentAction.actionSpecificFloats[0];
                value.y = recentAction.actionSpecificFloats[1];
                value.z = recentAction.actionSpecificFloats[2];
                value.w = recentAction.actionSpecificFloats[3];
                if(isSceneProp)
                    actionScene->shaderMGR->getProperty(pIndex).value = value;
                else
                    actionScene->nodes[indexOfAction]->getProperty(pIndex, matIndex).value = value;
                
                updatePropsOnUndoRedo(pIndex, value, indexOfAction, recentAction.frameId);
            }
            break;
        }
        case ACTION_CHANGE_MIRROR_STATE:{
            actionScene->selectMan->unselectObject(actionScene->selectedNodeId);
            setMirrorState((MIRROR_SWITCH_STATE)(bool)recentAction.actionSpecificFlags[0]);
            returnValue = SWITCH_MIRROR;
            break;
        }
        case ACTION_ADD_BONE: {
            if(recentAction.actionSpecificIntegers.size() && recentAction.actionSpecificIntegers[0] == NODE_TYPE_INSTANCED) {
                returnValue2 = recentAction.frameId;
            } else {
                returnValue = ADD_ASSET_BACK;
                returnValue2 = recentAction.frameId;
            }
            break;
    }
        case ACTION_APPLY_ANIM: {
            actionScene->selectMan->unselectObject(actionScene->selectedNodeId);
            actionScene->selectedNodeId = indexOfAction;
            actionScene->animMan->removeAppliedAnimation(recentAction.frameId, recentAction.actionSpecificIntegers[0]);
            returnValue = RELOAD_FRAMES;
            break;
        }
        case ACTION_TEXTURE_CHANGE:{
            bool isSceneProp = recentAction.actionSpecificFlags[0];
            if(!isSceneProp)
                actionScene->selectMan->selectObject(indexOfAction, recentAction.actionSpecificIntegers[0], false);
            std::string fileName = ConversionHelper::getStringForWString(recentAction.actionSpecificStrings[0]).c_str();
            Vector3 vColor;
            vColor.x = recentAction.actionSpecificFloats[0];
            vColor.y = recentAction.actionSpecificFloats[1];
            vColor.z = recentAction.actionSpecificFloats[2];
            PROP_INDEX pIndex = (PROP_INDEX)recentAction.actionSpecificIntegers[1];
            if(isSceneProp)
                actionScene->setEnvironmentTexture(fileName, false);
            else {
                actionScene->changeTexture(fileName, vColor, false, true, pIndex);
                actionScene->selectMan->unselectObject(indexOfAction);
            }
            break;
        }
        default:
            return DO_NOTHING;
    }
    
    if(recentAction.actionType != ACTION_ADD_BONE)
        currentAction--;
   
    if(recentAction.actionType != ACTION_SWITCH_FRAME) {
        actionScene->selectMan->removeChildren(actionScene->getParentNode());
        actionScene->updater->setKeysForFrame(recentAction.frameId);
        actionScene->updater->setKeysForFrame(recentAction.frameId);
        actionScene->selectMan->updateParentPosition();
    }
        // CPU SKIN update
    
    if(recentAction.actionType == ACTION_CHANGE_NODE_KEYS && (actionScene->nodes[indexOfAction]->getType() == NODE_LIGHT || actionScene->nodes[indexOfAction]->getType() == NODE_ADDITIONAL_LIGHT))
        actionScene->updater->updateLightProperties(recentAction.frameId);
    
    return returnValue;

}

int SGActionManager::redo()
{
    if(!actionScene || !smgr)
        return -1;

    if(actionScene->selectedNodeIds.size() > 0)
        actionScene->selectMan->unselectObjects();

    int returnValue = DO_NOTHING;
    
    if(currentAction == actions.size()) {
        if(currentAction <= 0) {
            currentAction = 0;
            return DEACTIVATE_BOTH;
        }
        else
            return DEACTIVATE_REDO;
    }
    

    SGAction &recentAction = actions[currentAction];
    int indexOfAction = getObjectIndex(recentAction.objectIndex);
    
    SGNode* sgNode;
    if(indexOfAction < actionScene->nodes.size())
        sgNode = actionScene->nodes[indexOfAction];

    
    switch(recentAction.actionType){
        case ACTION_CHANGE_NODE_KEYS:{
            sgNode->setKeyForFrame(recentAction.frameId, recentAction.keys[1]);
            actionScene->updater->reloadKeyFrameMap();
        }
            break;
        case ACTION_CHANGE_MULTI_NODE_KEYS:
        {
            int size = (int)recentAction.actionSpecificIntegers.size()/2;
            for (int i = size; i < (int)recentAction.actionSpecificIntegers.size(); i++) {
                if(recentAction.actionSpecificIntegers[i] != NOT_EXISTS){
                    actionScene->nodes[getObjectIndex(recentAction.actionSpecificIntegers[i])]->setKeyForFrame(recentAction.frameId, recentAction.keys[i]);
                    actionScene->updater->reloadKeyFrameMap();
                }
            }
        }
            break;
        case ACTION_CHANGE_JOINT_KEYS:{
            int jointsCnt = (int)sgNode->joints.size();
            for(unsigned long i=0; i < jointsCnt; i++){
                sgNode->joints[i]->setKeyForFrame(recentAction.frameId, recentAction.keys[jointsCnt+i]);
            }
            actionScene->updater->reloadKeyFrameMap();
            break;
        }
        case ACTION_CHANGE_NODE_JOINT_KEYS:{
            int jointsCnt = (int)sgNode->joints.size();
            sgNode->setKeyForFrame(recentAction.frameId, recentAction.keys[jointsCnt +1]);
            for(unsigned long i=1; i <= sgNode->joints.size(); i++){
                sgNode->joints[i-1]->setKeyForFrame(recentAction.frameId, recentAction.keys[jointsCnt+i+1]);
            }
            break;
        }
        case ACTION_SWITCH_FRAME:
            actionScene->currentFrame = recentAction.frameId;
            break;
           
        case ACTION_NUMERICAL_PROPERTY_CHANGE: {
            bool isSceneProp = recentAction.actionSpecificFlags[0];
            if((indexOfAction != NOT_SELECTED && indexOfAction < actionScene->nodes.size()) || isSceneProp) {
                PROP_INDEX pIndex = (PROP_INDEX)recentAction.actionSpecificIntegers[0];
                int matIndex = recentAction.actionSpecificIntegers[1];
                Vector4 value;
                value.x = recentAction.actionSpecificFloats[4];
                value.y = recentAction.actionSpecificFloats[5];
                value.z = recentAction.actionSpecificFloats[6];
                value.w = recentAction.actionSpecificFloats[7];
                if(isSceneProp)
                    actionScene->shaderMGR->getProperty(pIndex).value = value;
                else
                    actionScene->nodes[indexOfAction]->getProperty(pIndex, matIndex).value = value;
                
                updatePropsOnUndoRedo(pIndex, value, indexOfAction, recentAction.frameId);
            }
            break;
        }
        case ACTION_CHANGE_MIRROR_STATE:
            actionScene->selectMan->unselectObject(actionScene->selectedNodeId);
            setMirrorState((MIRROR_SWITCH_STATE)!recentAction.actionSpecificFlags[0]);
            returnValue = SWITCH_MIRROR;
            break;
       case ACTION_ADD_BONE:
            actionScene->selectMan->unselectObject(actionScene->selectedNodeId);
            actionScene->selectedNodeId = indexOfAction;
            returnValue = DELETE_ASSET;
            break;
        case ACTION_APPLY_ANIM: {
            actionScene->selectedNodeId = indexOfAction;
            actionScene->currentFrame = recentAction.frameId;
            actionScene->updater->setDataForFrame(actionScene->currentFrame);
            actionScene->animMan->applyAnimations(ConversionHelper::getStringForWString(recentAction.actionSpecificStrings[0]), actionScene->selectedNodeId);
            actionScene->updater->reloadKeyFrameMap();
            actionScene->selectMan->unselectObject(actionScene->selectedNodeId);
            break;
        }
        case ACTION_TEXTURE_CHANGE:{
            
            bool isSceneProp = recentAction.actionSpecificFlags[0];
            if(!isSceneProp)
                actionScene->selectMan->selectObject(indexOfAction, recentAction.actionSpecificIntegers[0], false);
            std::string fileName = ConversionHelper::getStringForWString(recentAction.actionSpecificStrings[1]).c_str();
            Vector3 vColor;
            vColor.x = recentAction.actionSpecificFloats[3];
            vColor.y = recentAction.actionSpecificFloats[4];
            vColor.z = recentAction.actionSpecificFloats[5];
            PROP_INDEX pIndex = (PROP_INDEX)recentAction.actionSpecificIntegers[1];
            if(isSceneProp)
                actionScene->setEnvironmentTexture(fileName, false);
            else {
                actionScene->changeTexture(fileName, vColor, false, true, pIndex);
                actionScene->selectMan->unselectObject(indexOfAction);
            }
            break;
        }
        default:
            return DO_NOTHING;
    }
    
    if(recentAction.actionType != ACTION_SWITCH_FRAME){
        actionScene->selectMan->removeChildren(actionScene->getParentNode());
        actionScene->updater->setKeysForFrame(actionScene->currentFrame);
        actionScene->selectMan->updateParentPosition();
    }
    actionScene->updater->updateControlsOrientaion();
    
    if(recentAction.actionType == ACTION_CHANGE_NODE_KEYS && (sgNode->getType() == NODE_LIGHT || sgNode->getType() == NODE_ADDITIONAL_LIGHT))
        actionScene->updater->updateLightProperties(recentAction.frameId);
    
    currentAction++;
    
    return returnValue;
}

int SGActionManager::getObjectIndex(int actionIndex)
{
        for(int i = 0; i < (int)actionScene->nodes.size(); i++) {
        if(actionScene->nodes[i]->actionId == actionIndex) {
            return i;
        }
    }
    return 0;
}

bool SGActionManager::changeSkeletonPosition(Vector3 outputValue)
{
    if(!actionScene || !smgr || !actionScene->isRigMode)
        return false;
    
    bool isNodeSelected = actionScene->hasNodeSelected();
    int selectedNodeId = actionScene->rigMan->selectedNodeId;
    SGNode* selectedNode = actionScene->getSelectedNode();
    std::map<int, RigKey>& rigKeys = actionScene->rigMan->rigKeys;
    
    if(!isNodeSelected || actionScene->rigMan->sceneMode == RIG_MODE_EDIT_ENVELOPES || actionScene->rigMan->sceneMode == RIG_MODE_PREVIEW)
        return false;
    
    if(selectedNodeId > 0){
        Mat4 parentGlobalMat = selectedNode->node->getParent()->getAbsoluteTransformation();
        selectedNode->node->setPosition(MathHelper::getRelativePosition(parentGlobalMat, selectedNode->node->getAbsolutePosition() + outputValue));
        selectedNode->node->updateAbsoluteTransformation();
        actionScene->updater->updateSkeletonBone(rigKeys, selectedNodeId);
        
        if(getMirrorState() == MIRROR_ON){
            int mirrorJointId = BoneLimitsHelper::getMirrorJointId(selectedNodeId);
            if(mirrorJointId != -1){
                {
                    shared_ptr<Node> mirrorNode = rigKeys[mirrorJointId].referenceNode->node;
                    if(selectedNode && selectedNode->node && mirrorNode) {
                        mirrorNode->setPosition(selectedNode->node->getPosition() * Vector3(-1.0,1.0,1.0));
                        mirrorNode->updateAbsoluteTransformation();
                        actionScene->updater->updateSkeletonBone(rigKeys, mirrorJointId);
                    }
                }
            }
        }
        actionScene->updater->updateSkeletonBones();
        return true;
    }
    else if(selectedNodeId == 0){
        selectedNode->node->setPosition(selectedNode->node->getAbsolutePosition() + outputValue);
        selectedNode->node->updateAbsoluteTransformation();
        return true;
    }
    return false;
}

bool SGActionManager::changeSkeletonRotation(Quaternion outputValue)
{
    if(!actionScene || !smgr || !actionScene->isRigMode || actionScene->rigMan->sceneMode != RIG_MODE_MOVE_JOINTS)
        return false;
    
    bool isNodeSelected = actionScene->hasNodeSelected();
    int selectedNodeId = actionScene->rigMan->selectedNodeId;
    SGNode* selectedNode = actionScene->getSelectedNode();
    
    if (actionScene->rigMan->sceneMode == RIG_MODE_MOVE_JOINTS && selectedNodeId > 0){
        selectedNode->node->setRotation(outputValue);
        selectedNode->node->updateAbsoluteTransformation();
        actionScene->updater->updateSkeletonBone(actionScene->rigMan->rigKeys, selectedNodeId);
        if(getMirrorState() == MIRROR_ON) {
            int mirrorJointId = BoneLimitsHelper::getMirrorJointId(selectedNodeId);
            if(mirrorJointId != -1){
                shared_ptr<Node> mirrorNode = actionScene->rigMan->rigKeys[mirrorJointId].referenceNode->node;
                mirrorNode->setRotation(outputValue * Quaternion(1.0, -1.0, -1.0));
                mirrorNode->updateAbsoluteTransformation();
                actionScene->updater->updateSkeletonBone(actionScene->rigMan->rigKeys, mirrorJointId);
                //mirrorNode.reset();
            }
        }
        //        updateEnvelopes();
        return true;
    } else if(selectedNode) {
        selectedNode->node->setRotation(outputValue);
        if(isNodeSelected)
            selectedNode->node->updateAbsoluteTransformationOfChildren();
        else
            selectedNode->node->updateAbsoluteTransformation();
        
        return true;
    }
    return false;
}

bool SGActionManager::changeSGRPosition(Vector3 outputValue)
{
    if(!actionScene || !smgr || !actionScene->isRigMode || actionScene->rigMan->sceneMode != RIG_MODE_PREVIEW)
        return false;

    bool isJointSelected = actionScene->hasJointSelected();
    bool isNodeSelected = actionScene->hasNodeSelected();
    SGNode* selectedNode = actionScene->getSelectedNode();
    SGJoint* selectedJoint = actionScene->getSelectedJoint();
    int selectedJointId = actionScene->rigMan->selectedJointId;
    SGNode* sgrSGNode = actionScene->rigMan->getRiggedNode();
    
    if(isJointSelected){
        Vector3 target = selectedJoint->jointNode->getAbsolutePosition() + outputValue;
        selectedJoint->jointNode->getParent()->setRotation(MathHelper::getRelativeParentRotation(selectedJoint->jointNode, target));
        (dynamic_pointer_cast<JointNode>(selectedJoint->jointNode->getParent()))->updateAbsoluteTransformationOfChildren();
        if(getMirrorState() == MIRROR_ON){
            int mirrorJointId = BoneLimitsHelper::getMirrorJointId(selectedJointId);
            if(mirrorJointId != -1){
                shared_ptr<JointNode> mirrorNode = (dynamic_pointer_cast<AnimatedMeshNode>(sgrSGNode->node))->getJointNode(mirrorJointId);
                mirrorNode->getParent()->setRotation(selectedJoint->jointNode->getParent()->getRotation() * Quaternion(1.0, -1.0, -1.0));
                (dynamic_pointer_cast<JointNode>(mirrorNode->getParent()))->updateAbsoluteTransformationOfChildren();
            }
        }
        return true;
    }
    else if(isNodeSelected){
        selectedNode->node->setPosition(selectedNode->node->getAbsolutePosition() + outputValue);
        selectedNode->node->updateAbsoluteTransformationOfChildren();
        return true;
    }
    return false;
}

bool SGActionManager::changeSGRRotation(Quaternion outputValue)
{
    if(!actionScene || !smgr || !actionScene->isRigMode || actionScene->rigMan->sceneMode != RIG_MODE_PREVIEW)
        return false;
    
    bool isJointSelected = actionScene->hasJointSelected();
    bool isNodeSelected = actionScene->hasNodeSelected();
    SGNode* selectedNode = actionScene->getSelectedNode();
    SGJoint* selectedJoint = actionScene->getSelectedJoint();
    int selectedJointId = actionScene->rigMan->selectedJointId;
    SGNode* sgrSGNode = actionScene->rigMan->getRiggedNode();
    
    outputValue *= RADTODEG;
    if(isJointSelected){
        selectedJoint->jointNode->setRotation(outputValue);
        selectedJoint->jointNode->updateAbsoluteTransformationOfChildren();
        if(getMirrorState() == MIRROR_ON){
            int mirrorJointId = BoneLimitsHelper::getMirrorJointId(selectedJointId);
            if(mirrorJointId != -1){
                shared_ptr<JointNode> mirrorNode = (dynamic_pointer_cast<AnimatedMeshNode>(sgrSGNode->node))->getJointNode(mirrorJointId);
                mirrorNode->setRotation(outputValue * Quaternion(1.0, -1.0, -1.0));
                mirrorNode->updateAbsoluteTransformationOfChildren();
                //mirrorNode.reset();
            }
        }
        return true;
    }
    else if(selectedNode){
        selectedNode->node->setRotation(outputValue);
        if(isNodeSelected)
            selectedNode->node->updateAbsoluteTransformation();
        else
            selectedNode->node->updateAbsoluteTransformationOfChildren();
        return true;
    }
    return false;
}

