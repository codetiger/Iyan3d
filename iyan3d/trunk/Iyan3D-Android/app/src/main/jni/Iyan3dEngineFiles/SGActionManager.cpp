//
//  SGActionManager.cpp
//  Iyan3D
//
//  Created by Karthik on 26/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#include "SGActionManager.h"
#include "SGEditorScene.h"

SGEditorScene* actionScene;

SGActionManager::SGActionManager(SceneManager* smgr, void* scene)
{
    this->smgr = smgr;
    actionScene = (SGEditorScene*)scene;
}

bool SGActionManager::isIKJoint(int jointId)
{
    return (jointId == HAND_LEFT || jointId == HAND_RIGHT || jointId == LEG_LEFT || jointId == LEG_RIGHT || jointId == HIP || jointId == LEG_LEFT1 || jointId == LEG_RIGHT1);
}


bool SGActionManager::changeObjectOrientation(Vector3 outputValue)
{
    if(!actionScene || !smgr || !actionScene->isNodeSelected)
        return false;
    
    SGNode* selectedNode = actionScene->nodes[actionScene->selectedNodeId];
    
    bool success = false;
    switch(actionScene->controlType){
        case MOVE:{
            if(actionScene->isJointSelected){
                success = true;
                moveJoint(outputValue);
            }else if(actionScene->isNodeSelected){
                success = true;
                selectedNode->setPosition(selectedNode->node->getPosition() + outputValue, actionScene->currentFrame);
                selectedNode->setPositionOnNode(selectedNode->node->getPosition() + outputValue);
                break;
            }
            break;
        }
        case ROTATE:{
            outputValue*=RADTODEG;
            if(actionScene->isJointSelected){
                success = true;
                rotateJoint(outputValue);
             }else if(actionScene->isNodeSelected){
                success = true;
                Quaternion r = Quaternion(outputValue * DEGTORAD);
                selectedNode->setRotation(r, actionScene->currentFrame);
                selectedNode->setRotationOnNode(r);
                break;
            }
            break;
        }
        default:
            break;
    }
    actionScene->updater->updateControlsOrientaion();
    actionScene->updater->updateLightCamera();
    return success;
}

void SGActionManager::moveJoint(Vector3 outputValue)
{
    
    SGNode* selectedNode = actionScene->nodes[actionScene->selectedNodeId];
    SGJoint * selectedJoint = actionScene->selectedJoint;
    
    Vector3 target = selectedJoint->jointNode->getAbsolutePosition() + outputValue;
    if((selectedNode->joints.size() == HUMAN_JOINTS_SIZE) && isIKJoint(actionScene->selectedJointId)){
        if(actionScene->selectedJointId <= HIP){
            shared_ptr<AnimatedMeshNode> rigNode = dynamic_pointer_cast<AnimatedMeshNode>(selectedNode->node);
            if(rigNode) {
                selectedNode->setPosition(rigNode->getPosition() + outputValue, actionScene->currentFrame);
                selectedNode->setPositionOnNode(rigNode->getPosition() + outputValue);
                rigNode->setPosition(rigNode->getPosition() + outputValue);
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
    } else if(selectedNode->getType() == NODE_TEXT){
        
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
        selectedNode->joints[parent->getID()]->setRotationOnNode(rot);
        
        if(actionScene->getMirrorState() == MIRROR_ON){
            int mirrorJointId = BoneLimitsHelper::getMirrorJointId(actionScene->selectedJointId);
            if(mirrorJointId != -1) {
                selectedJoint = selectedNode->joints[actionScene->selectedJointId];
                shared_ptr<JointNode> mirrorNode = (dynamic_pointer_cast<AnimatedMeshNode>(selectedNode->node))->getJointNode(mirrorJointId);
                rot = Quaternion(selectedNode->joints[selectedJoint->jointNode->getParent()->getID()]->jointNode->getRotationInDegrees()*Vector3(1.0,-1.0,-1.0)*DEGTORAD);
                selectedNode->joints[mirrorNode->getParent()->getID()]->setRotation(rot,actionScene->currentFrame);
                selectedNode->joints[mirrorNode->getParent()->getID()]->setRotationOnNode(rot);
            }
        }
    }
// TODO For CPU Skin update mesh cache
}

void SGActionManager::rotateJoint(Vector3 outputValue)
{
    SGNode* selectedNode = actionScene->nodes[actionScene->selectedNodeId];
    SGJoint * selectedJoint = actionScene->selectedJoint;

    selectedJoint->setRotation(Quaternion(outputValue*DEGTORAD), actionScene->currentFrame);
    selectedJoint->setRotationOnNode(Quaternion(outputValue*DEGTORAD));
    if(actionScene->getMirrorState() == MIRROR_ON){
        int mirrorJointId = BoneLimitsHelper::getMirrorJointId(actionScene->selectedJointId);
        if(mirrorJointId != -1) {
            Quaternion rot = Quaternion(outputValue*Vector3(1.0,-1.0,-1.0)*DEGTORAD);
            selectedNode->joints[mirrorJointId]->setRotation(rot, actionScene->currentFrame);
            selectedNode->joints[mirrorJointId]->setRotationOnNode(rot);
        }
    }
    // TODO For CPU Skin update mesh cache
}
