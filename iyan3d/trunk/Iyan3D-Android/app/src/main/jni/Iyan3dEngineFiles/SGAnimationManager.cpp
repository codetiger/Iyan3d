//
//  SGAnimationManager.cpp
//  Iyan3D
//
//  Created by Karthik on 26/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#include "HeaderFiles/SGAnimationManager.h"
#include "HeaderFiles/SGEditorScene.h"

SGEditorScene* animScene;

SGAnimationManager::SGAnimationManager(SceneManager* smgr, void* scene)
{
    this->smgr = smgr;
    animScene = (SGEditorScene*)scene;
}

SGAnimationManager::~SGAnimationManager(){
    
}


vector<Vector3> SGAnimationManager::storeTextInitialPositions(SGNode *sgnode)
{
    vector<Vector3> jointPositions;

    if(!animScene || !smgr)
        return jointPositions;

    int jointCount = (dynamic_pointer_cast<AnimatedMeshNode>(sgnode->node))->getJointCount();
    for (int i = 0; i < jointCount; i++) {
        jointPositions.push_back((dynamic_pointer_cast<AnimatedMeshNode>(sgnode->node))->getJointNode(i)->getPosition());
    }
    return jointPositions;
}

void SGAnimationManager::applyAnimations(string filePath , int nodeIndex)
{
    if(!animScene || !smgr)
        return;
        
    nodeIndex = (nodeIndex == -1) ? animScene->selectedNodeId : nodeIndex;
    animFilePath = filePath;
    animStartFrame = animScene->currentFrame;
    SGNode *sgNode = NULL;
    if(nodeIndex < animScene->nodes.size()) {
        sgNode = animScene->nodes[nodeIndex];
    }
    if(sgNode->getType() == NODE_RIG) {
        applySGRAnimations(filePath, sgNode, animScene->totalFrames, animScene->currentFrame, animTotalFrames);
    } else if (sgNode->getType() == NODE_TEXT) {
        applyTextAnimations(filePath, sgNode, animScene->totalFrames, animScene->currentFrame ,animScene->textJointsBasePos[nodeIndex], animTotalFrames);
    }
    animScene->updater->setDataForFrame(animScene->currentFrame);
    animScene->updater->reloadKeyFrameMap();
}

void SGAnimationManager::copyKeysOfNode(int fromNodeId, int toNodeId)
{
    animScene->selectMan->selectObject(toNodeId);
    animScene->nodes[toNodeId]->positionKeys = animScene->nodes[fromNodeId]->positionKeys;
    animScene->nodes[toNodeId]->rotationKeys = animScene->nodes[fromNodeId]->rotationKeys;
    animScene->nodes[toNodeId]->scaleKeys = animScene->nodes[fromNodeId]->scaleKeys;
    
    for (int i =0; i < animScene->nodes[toNodeId]->joints.size(); i++)
        animScene->nodes[toNodeId]->joints[i]->rotationKeys = animScene->nodes[fromNodeId]->joints[i]->rotationKeys;
}

void SGAnimationManager::applySGRAnimations(string filePath, SGNode *sgNode, int &totalFrames , int currentFrame , int &animFrames)
{
    ifstream sgraFile(filePath,ios::in | ios::binary);
    FileHelper::resetSeekPosition();
    FileHelper::readShort(&sgraFile); // version number
    FileHelper::readShort(&sgraFile); // type
    short numberOfJoints = FileHelper::readShort(&sgraFile);
    FileHelper::readShort(&sgraFile); // Number of frames
    short numberOfKeyFrames = FileHelper::readShort(&sgraFile);
    short lastFrameId = FileHelper::readShort(&sgraFile);   
    
    if((currentFrame + lastFrameId+1) > totalFrames)
        totalFrames = currentFrame + lastFrameId+1;
    animFrames = totalFrames;
    
    Vector3 nodeInitPos = sgNode->node->getAbsolutePosition();
    int posCount = 0;
    //printf("Node Init Pos %f %f %f ",nodeInitPos.x, nodeInitPos.y, nodeInitPos.z);
    for (int i = 0; i < numberOfKeyFrames; i++) {
        short keyFrameId = currentFrame + FileHelper::readShort(&sgraFile);
        
        ActionKey nodeKey;
        
        short activeKeys = FileHelper::readShort(&sgraFile);
        int keyType = 0;
        
        for (short j = 0; j < activeKeys; j++) {
            
            keyType = FileHelper::readShort(&sgraFile);
            
            if(keyType == 1) {
                nodeKey.isPositionKey = true;
                Vector3 position;
                position.x = (posCount==0) ? nodeInitPos.x : (nodeInitPos.x +  FileHelper::readFloat(&sgraFile));
                position.y = (posCount==0) ? nodeInitPos.y : (nodeInitPos.y + FileHelper::readFloat(&sgraFile));
                position.z = (posCount==0) ? nodeInitPos.z : (nodeInitPos.z + FileHelper::readFloat(&sgraFile));
                if(posCount == 0){
                    FileHelper::readFloat(&sgraFile);
                    FileHelper::readFloat(&sgraFile);
                    FileHelper::readFloat(&sgraFile);
                }
                posCount++;
                nodeKey.position = position;
            }
            if(keyType == 2) {
                nodeKey.isRotationKey = true;
                Quaternion rotation;
                rotation.x = FileHelper::readFloat(&sgraFile);
                rotation.y = FileHelper::readFloat(&sgraFile);
                rotation.z = FileHelper::readFloat(&sgraFile);
                rotation.w = FileHelper::readFloat(&sgraFile);
                nodeKey.rotation = rotation;
            }
            if(keyType == 3) {
                nodeKey.isScaleKey = true;
                Vector3 scale;
                scale.x = FileHelper::readFloat(&sgraFile);
                scale.y = FileHelper::readFloat(&sgraFile);
                scale.z = FileHelper::readFloat(&sgraFile);
                nodeKey.scale = scale;
            }
        }
        
        sgNode->setKeyForFrame(keyFrameId, nodeKey);
        
        for (int jointId = 1; jointId < numberOfJoints; jointId++) {
            
            ActionKey jointKey;
            
            short activeKeys = FileHelper::readShort(&sgraFile);
            int keyType = 0;
            for (short j = 0; j < activeKeys; j++) {
                keyType = FileHelper::readShort(&sgraFile);
                if(keyType == 1) {
                    jointKey.isPositionKey = true;
                    Vector3 position;
                    position.x = FileHelper::readFloat(&sgraFile);
                    position.y = FileHelper::readFloat(&sgraFile);
                    position.z = FileHelper::readFloat(&sgraFile);
                    jointKey.position = position;
                }
                if(keyType == 2) {
                    jointKey.isRotationKey = true;
                    Quaternion rotation;
                    rotation.x = FileHelper::readFloat(&sgraFile);
                    rotation.y = FileHelper::readFloat(&sgraFile);
                    rotation.z = FileHelper::readFloat(&sgraFile);
                    rotation.w = FileHelper::readFloat(&sgraFile);
                    jointKey.rotation = rotation;
                }
                if(keyType == 3) {
                    jointKey.isScaleKey = true;
                    Vector3 scale;
                    scale.x = FileHelper::readFloat(&sgraFile);
                    scale.y = FileHelper::readFloat(&sgraFile);
                    scale.z = FileHelper::readFloat(&sgraFile);
                    jointKey.scale = scale;
                }
            }
            
            sgNode->joints[jointId]->setKeyForFrame(keyFrameId, jointKey);
            
        }
    }
    
    sgraFile.close();
}

void SGAnimationManager::applyTextAnimations(string filePath, SGNode *sgNode, int &totalFrames, int currentFrame , vector<Vector3> jointBasePositions, int &animFrames)
{
    if(jointBasePositions.size() < sgNode->joints.size())
        return;
    
    ifstream sgraFile(filePath,ios::in | ios::binary);
    
    FileHelper::resetSeekPosition();
    FileHelper::readShort(&sgraFile); // Version Number
    FileHelper::readShort(&sgraFile); // Type
    short numberOfJoints = FileHelper::readShort(&sgraFile);
    FileHelper::readShort(&sgraFile); // Number of frames
    short numberOfKeyFrames = FileHelper::readShort(&sgraFile);
    short lastFrameId = FileHelper::readShort(&sgraFile);
    
    if((currentFrame + lastFrameId+1) > totalFrames)
        totalFrames = currentFrame + lastFrameId+1;
    animFrames = totalFrames;
    Vector3 nodeInitPos = sgNode->node->getAbsolutePosition();
    int posCount = 0;
    
    for (int i = 0; i < numberOfKeyFrames; i++) {
        short keyFrameId = currentFrame + FileHelper::readShort(&sgraFile);
        
        ActionKey nodeKey;
        
        short activeKeys = FileHelper::readShort(&sgraFile);
        int keyType = 0;
        
        for (short j = 0; j < activeKeys; j++) {
            
            keyType = FileHelper::readShort(&sgraFile);
            if(keyType == 1) {
                nodeKey.isPositionKey = true;
                Vector3 position;
                position.x = (posCount==0) ? nodeInitPos.x : (nodeInitPos.x +  FileHelper::readFloat(&sgraFile));
                position.y = (posCount==0) ? nodeInitPos.y : (nodeInitPos.y + FileHelper::readFloat(&sgraFile));
                position.z = (posCount==0) ? nodeInitPos.z : (nodeInitPos.z + FileHelper::readFloat(&sgraFile));
                if(posCount == 0){
                    FileHelper::readFloat(&sgraFile);
                    FileHelper::readFloat(&sgraFile);
                    FileHelper::readFloat(&sgraFile);
                }
                posCount++;
                nodeKey.position = position;
            }
            if(keyType == 2) {
                nodeKey.isRotationKey = true;
                Quaternion rotation;
                rotation.x = FileHelper::readFloat(&sgraFile);
                rotation.y = FileHelper::readFloat(&sgraFile);
                rotation.z = FileHelper::readFloat(&sgraFile);
                rotation.w = FileHelper::readFloat(&sgraFile);
                nodeKey.rotation = rotation;
            }
            if(keyType == 3) {
                nodeKey.isScaleKey = true;
                Vector3 scale;
                scale.x = FileHelper::readFloat(&sgraFile);
                scale.y = FileHelper::readFloat(&sgraFile);
                scale.z = FileHelper::readFloat(&sgraFile);
                nodeKey.scale = scale;
            }
        }
        
        sgNode->setKeyForFrame(keyFrameId, nodeKey);
        
        for (int jointId = 1; jointId < numberOfJoints; jointId++) {
            
            ActionKey jointKey;
            
            short activeKeys = FileHelper::readShort(&sgraFile);
            int keyType = 0;
            for (short j = 0; j < activeKeys; j++) {
                keyType = FileHelper::readShort(&sgraFile);
                if(keyType == 1) {
                    jointKey.isPositionKey = true;
                    Vector3 position;
                    position.x = FileHelper::readFloat(&sgraFile);
                    position.y = FileHelper::readFloat(&sgraFile);
                    position.z = FileHelper::readFloat(&sgraFile);
                    jointKey.position = position;
                }
                if(keyType == 2) {
                    jointKey.isRotationKey = true;
                    Quaternion rotation;
                    rotation.x = FileHelper::readFloat(&sgraFile);
                    rotation.y = FileHelper::readFloat(&sgraFile);
                    rotation.z = FileHelper::readFloat(&sgraFile);
                    rotation.w = FileHelper::readFloat(&sgraFile);
                    jointKey.rotation = rotation;
                }
                if(keyType == 3) {
                    jointKey.isScaleKey = true;
                    Vector3 scale;
                    scale.x = FileHelper::readFloat(&sgraFile);
                    scale.y = FileHelper::readFloat(&sgraFile);
                    scale.z = FileHelper::readFloat(&sgraFile);
                    jointKey.scale = scale;
                }
            }
            
            //sgNode->joints[jointId]->setKeyForFrame(keyFrameId, jointKey);
            
            int j = jointId;
            while (j < (int)sgNode->joints.size()) {
                
                ActionKey actualKey;
                actualKey.isPositionKey = jointKey.isPositionKey;
                actualKey.isRotationKey = jointKey.isRotationKey;
                actualKey.isScaleKey = jointKey.isScaleKey;
                actualKey.rotation = jointKey.rotation;actualKey.scale = jointKey.scale;
                
                actualKey.position.x = jointBasePositions[j].x + jointKey.position.x;
                actualKey.position.y = jointBasePositions[j].y + jointKey.position.y;
                actualKey.position.z = jointBasePositions[j].z + jointKey.position.z;
                
                sgNode->joints[j]->setKeyForFrame(keyFrameId, actualKey);
                
                j += (numberOfJoints -1);
            }
            
        }
    }
    
    sgraFile.close();
    
}

bool SGAnimationManager::removeAnimationForSelectedNodeAtFrame(int selectedFrame)
{
    if(!animScene || !smgr)
        return false;

    bool isKeySetForNode = true;
    if(animScene->selectedNodeId != NOT_SELECTED) {
        SGNode *selectedNode = animScene->nodes[animScene->selectedNodeId];
        
        if(animScene->isKeySetForFrame.find(selectedFrame) == animScene->isKeySetForFrame.end())
        {
            return false;
        }
        if(selectedFrame == 0){
            Vector3 initPos = Vector3(0.0);
            int type = selectedNode->getType();
            if(type <= NODE_LIGHT){
                initPos = (type == NODE_CAMERA) ? Vector3(RENDER_CAM_INIT_POS_X,RENDER_CAM_INIT_POS_Y,RENDER_CAM_INIT_POS_Z):Vector3(-LIGHT_INIT_POS_X,LIGHT_INIT_POS_Y,LIGHT_INIT_POS_Z);
            }
            
            selectedNode->setPosition(initPos, 0);
            selectedNode->setRotation(selectedNode->nodeInitialRotation, 0);
            if(animScene->selectedNodeId != NODE_LIGHT)
                selectedNode->setScale( Vector3(1.0), 0);
            
            for(unsigned long i = 0; i < selectedNode->joints.size(); i++){
                selectedNode->joints[i]->setRotation((selectedNode->jointsInitialRotations[i]), 0);
            }
            
            animScene->updater->setDataForFrame(selectedFrame);
            //            updateLightWithRender();
            animScene->updater->updateControlsOrientaion();
        }
        else {
            selectedNode->removeAnimationInCurrentFrame(selectedFrame);
            animScene->updater->setDataForFrame(selectedFrame);
            animScene->updater->reloadKeyFrameMap();
            animScene->updater->updateControlsOrientaion();
        }
        return isKeySetForNode;
    }
    else
        return isKeySetForNode;
}

void SGAnimationManager::removeAppliedAnimation(int startFrame, int endFrame)
{
    if(!animScene || !smgr)
        return;

    animScene->updater->reloadKeyFrameMap();
    for (int i = startFrame; i < endFrame; i++) {
        removeAnimationForSelectedNodeAtFrame(i);
    }
    animScene->updater->reloadKeyFrameMap();
    animScene->selectMan->unselectObject(animScene->selectedNodeId);
}

