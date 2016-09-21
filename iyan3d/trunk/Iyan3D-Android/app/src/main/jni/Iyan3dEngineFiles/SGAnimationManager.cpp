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

SGAnimationManager::~SGAnimationManager()
{
    
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

    if(nodeIndex < animScene->nodes.size())
        sgNode = animScene->nodes[nodeIndex];
    
    if(sgNode->getType() == NODE_RIG)
        applySGRAnimations(filePath, sgNode, animScene->totalFrames, animScene->currentFrame, animTotalFrames);
    else if (sgNode->getType() == NODE_TEXT_SKIN)
        applyTextAnimations(filePath, sgNode, animScene->totalFrames, animScene->currentFrame ,animScene->textJointsBasePos[nodeIndex], animTotalFrames);
    
    animScene->updater->setDataForFrame(animScene->currentFrame);
    animScene->updater->reloadKeyFrameMap();
}

void SGAnimationManager::copyKeysOfNode(int fromNodeId, int toNodeId)
{
    if(fromNodeId < 0 || fromNodeId >= animScene->nodes.size() || toNodeId < 0 || toNodeId >= animScene->nodes.size())
        return;
    animScene->selectMan->selectObject(toNodeId, NOT_SELECTED,false);
    animScene->nodes[toNodeId]->positionKeys = animScene->nodes[fromNodeId]->positionKeys;
    animScene->nodes[toNodeId]->rotationKeys = animScene->nodes[fromNodeId]->rotationKeys;
    animScene->nodes[toNodeId]->scaleKeys = animScene->nodes[fromNodeId]->scaleKeys;
    if(animScene->nodes[fromNodeId]->joints.size() > 0){
    for (int i =0; i < animScene->nodes[fromNodeId]->joints.size(); i++)
        animScene->nodes[toNodeId]->joints[i]->rotationKeys = animScene->nodes[fromNodeId]->joints[i]->rotationKeys;
    }
}

void SGAnimationManager::copyPropsOfNode(int fromNodeId, int toNodeId, bool excludeKeys)
{
    
    animScene->selectMan->unselectObject(fromNodeId);
    animScene->nodes[toNodeId]->options = animScene->nodes[fromNodeId]->options;
//    for(int i = 0; i < animScene->nodes[fromNodeId]->materialProps.size(); i++) {
//        if(animScene->nodes[toNodeId]->materialProps.size() <= i)
//            animScene->nodes[toNodeId]->materialProps.push_back(new MaterialProperty(animScene->nodes[toNodeId]->getType()));
//        memcpy(animScene->nodes[toNodeId]->materialProps[i], animScene->nodes[fromNodeId]->materialProps[i], sizeof(MaterialProperty));
//    }
    //animScene->nodes[toNodeId]->materialProps = animScene->nodes[fromNodeId]->materialProps;
    
    if(!excludeKeys) {
        ActionKey key;
        key.isPositionKey = true;
        if(animScene->nodes[fromNodeId]->positionKeys.size() > 0)
            key.position = KeyHelper::getKeyInterpolationForFrame<int, SGPositionKey, Vector3>(animScene->currentFrame, animScene->nodes[fromNodeId]->positionKeys);
        key.isRotationKey = true;
        if(animScene->nodes[fromNodeId]->rotationKeys.size() >0)
            key.rotation = KeyHelper::getKeyInterpolationForFrame<int, SGRotationKey, Quaternion>(animScene->currentFrame, animScene->nodes[fromNodeId]->rotationKeys,true);
        key.isScaleKey = true;
        if(animScene->nodes[fromNodeId]->scaleKeys.size() >0)
            key.scale = KeyHelper::getKeyInterpolationForFrame<int, SGScaleKey, Vector3>(animScene->currentFrame, animScene->nodes[fromNodeId]->scaleKeys);
        
        animScene->nodes[toNodeId]->setKeyForFrame(0, key);
        
        if(animScene->actionMan->actions.size() > 0) {
            SGAction &addAction = (animScene->actionMan->actions[animScene->actionMan->actions.size() -1]);
            
            SGNode *sgNode = animScene->nodes[toNodeId];
            
            if(sgNode->positionKeys.size())
                addAction.nodePositionKeys = sgNode->positionKeys;
            if(sgNode->rotationKeys.size())
                addAction.nodeRotationKeys = sgNode->rotationKeys;
            if(sgNode->scaleKeys.size())
                addAction.nodeSCaleKeys = sgNode->scaleKeys;
            if(sgNode->visibilityKeys.size())
                addAction.nodeVisibilityKeys = sgNode->visibilityKeys;
            for (int i = 0; i < (int)sgNode->joints.size(); i++) {
                addAction.jointRotKeys[i] = sgNode->joints[i]->rotationKeys;
                if(sgNode->getType() == NODE_TEXT_SKIN){
                    addAction.jointPosKeys[i] = sgNode->joints[i]->positionKeys;
                    addAction.jointScaleKeys[i] = sgNode->joints[i]->scaleKeys;
                }
            }
            
            if(addAction.actionType == ACTION_NODE_ADDED) {
                addAction.actionSpecificStrings[0] = (ConversionHelper::getWStringForString(sgNode->oriTextureName));
                addAction.actionSpecificStrings[1] = (ConversionHelper::getWStringForString(sgNode->getProperty(TEXTURE).fileName));
                addAction.actionSpecificFloats[0] = (sgNode->getProperty(ORIG_VERTEX_COLOR).value.x);
                addAction.actionSpecificFloats[1] = (sgNode->getProperty(ORIG_VERTEX_COLOR).value.y);
                addAction.actionSpecificFloats[2] = (sgNode->getProperty(ORIG_VERTEX_COLOR).value.z);
                addAction.actionSpecificFloats[3] = (sgNode->getProperty(VERTEX_COLOR).value.x);
                addAction.actionSpecificFloats[4] = (sgNode->getProperty(VERTEX_COLOR).value.y);
                addAction.actionSpecificFloats[5] = (sgNode->getProperty(VERTEX_COLOR).value.z);
                addAction.actionSpecificFlags[0] = (sgNode->getProperty(IS_VERTEX_COLOR).value.x);
                addAction.options = animScene->nodes[fromNodeId]->options;
            }
        }
    }
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
            } else if(keyType == 2) {
                nodeKey.isRotationKey = true;
                Quaternion rotation;
                rotation.x = FileHelper::readFloat(&sgraFile);
                rotation.y = FileHelper::readFloat(&sgraFile);
                rotation.z = FileHelper::readFloat(&sgraFile);
                rotation.w = FileHelper::readFloat(&sgraFile);
                nodeKey.rotation = rotation;
            } else if(keyType == 3) {
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
                } else if(keyType == 2) {
                    jointKey.isRotationKey = true;
                    Quaternion rotation;
                    rotation.x = FileHelper::readFloat(&sgraFile);
                    rotation.y = FileHelper::readFloat(&sgraFile);
                    rotation.z = FileHelper::readFloat(&sgraFile);
                    rotation.w = FileHelper::readFloat(&sgraFile);
                    jointKey.rotation = rotation;
                } else if(keyType == 3) {
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

bool SGAnimationManager::storeAnimations(int assetId)
{
    NODE_TYPE currentType = animScene->nodes[animScene->selectedNodeId]->getType();
    animScene->updater->setDataForFrame(animScene->currentFrame);
    string extension = (currentType == NODE_RIG) ? ".sgra" : ".sgta";
    
#ifdef IOS
    string filePath =  FileHelper::getDocumentsDirectory()+ "Resources/Animations/" + to_string(assetId) + extension;
    string thumbnailPath = FileHelper::getDocumentsDirectory() + "Resources/Animations/" + to_string(assetId) + ".png";
#elif ANDROID
    string filePath =  constants::DocumentsStoragePath + "/animations/" + to_string(assetId) + extension;
    string thumbnailPath = constants::DocumentsStoragePath + "/animations/" + to_string(assetId) + ".png";
#elif UBUNTU
    string filePath =  "/animations/" + to_string(assetId) + extension;
    string thumbnailPath = "/animations/" + to_string(assetId) + ".png";
#endif
    int oldResolution = animScene->nodes[NODE_CAMERA]->getProperty(CAM_RESOLUTION).value.x;

    animScene->nodes[NODE_CAMERA]->getProperty(CAM_RESOLUTION).value.x = 2;
    animScene->renHelper->renderAndSaveImage((char*)thumbnailPath.c_str(), true, animScene->currentFrame);
    if(animScene->shaderMGR->deviceType == METAL)
        animScene->renHelper->renderAndSaveImage((char*)thumbnailPath.c_str(), true, animScene->currentFrame);
    animScene->nodes[NODE_CAMERA]->getProperty(CAM_RESOLUTION).value.x = oldResolution;
    
    vector<int> totalKeyFrames;
    for (int frame = 0; frame < animScene->totalFrames; frame++) {
        if(animScene->isKeySetForFrame.find(frame) != animScene->isKeySetForFrame.end())
            totalKeyFrames.push_back(frame);
    }
    
    if(currentType == NODE_RIG)
        SGAnimationManager::storeSGRAnimations(filePath,animScene->nodes[animScene->selectedNodeId], animScene->totalFrames,totalKeyFrames);
    else
        SGAnimationManager::storeTextAnimations(filePath, animScene->nodes[animScene->selectedNodeId], animScene->totalFrames , animScene->textJointsBasePos[animScene->selectedNodeId],totalKeyFrames);
    
    if(FILE *file = fopen(filePath.c_str(), "r")) {
        fclose(file);
        return true;
    }
    return false;
}

void SGAnimationManager::storeSGRAnimations(string filePath , SGNode *sgNode , int totalFrames , vector<int>totalKeyFrames)
{
    //string filePath = FileHelper::getDocumentsDirectory() + "sample.sgra";
    ofstream sgraFile(filePath,ios::out | ios::binary);
    FileHelper::resetSeekPosition();
    FileHelper::writeShort(&sgraFile, 1);
    FileHelper::writeShort(&sgraFile, 0);
    FileHelper::writeShort(&sgraFile, sgNode->joints.size());
    FileHelper::writeShort(&sgraFile, totalFrames);
    FileHelper::writeShort(&sgraFile, (short)totalKeyFrames.size());
    short lastKeyFrameId = totalKeyFrames[totalKeyFrames.size()-1];
    FileHelper::writeShort(&sgraFile, lastKeyFrameId);
    
    Vector3 nodeInitPos = Vector3(0.0);
    int posCount = 0;
    for(int i = 0; i < totalKeyFrames.size(); i++)
    {
        short frame = totalKeyFrames[i];
        ActionKey nodeKey = sgNode->getKeyForFrame(frame);
        short activeKeys = 0;
        if(nodeKey.isPositionKey) activeKeys++;
        if(nodeKey.isRotationKey) activeKeys++;
        if(nodeKey.isScaleKey) activeKeys++;
        
        FileHelper::writeShort(&sgraFile, frame); // Frame Id
        
        FileHelper::writeShort(&sgraFile, activeKeys);
        
        if(nodeKey.isPositionKey) {
            int type = 1;
            
            Vector3 offsetPos = Vector3();
            FileHelper::writeShort(&sgraFile, type);
            FileHelper::writeFloat(&sgraFile, (nodeKey.position.x - nodeInitPos.x));
            FileHelper::writeFloat(&sgraFile, (nodeKey.position.y - nodeInitPos.y));
            FileHelper::writeFloat(&sgraFile, (nodeKey.position.z - nodeInitPos.z));
            if(posCount == 0) nodeInitPos = nodeKey.position;
            posCount++;
        }
        if(nodeKey.isRotationKey) {
            int type = 2;
            FileHelper::writeShort(&sgraFile, type);
            FileHelper::writeFloat(&sgraFile, nodeKey.rotation.x);
            FileHelper::writeFloat(&sgraFile, nodeKey.rotation.y);
            FileHelper::writeFloat(&sgraFile, nodeKey.rotation.z);
            FileHelper::writeFloat(&sgraFile, nodeKey.rotation.w);
        }
        if(nodeKey.isScaleKey) {
            int type = 3;
            FileHelper::writeShort(&sgraFile, type);
            FileHelper::writeFloat(&sgraFile, nodeKey.scale.x);
            FileHelper::writeFloat(&sgraFile, nodeKey.scale.y);
            FileHelper::writeFloat(&sgraFile, nodeKey.scale.z);
        }
        
        for(int jointId = 1; jointId < sgNode->joints.size(); jointId++) {
            
            ActionKey frameKey = sgNode->joints[jointId]->getKeyForFrame(frame);
            short activeKeys = 0;
            if(frameKey.isPositionKey) activeKeys++;
            if(frameKey.isRotationKey) activeKeys++;
            if(frameKey.isScaleKey) activeKeys++;
            
            FileHelper::writeShort(&sgraFile, activeKeys);
            
            if(frameKey.isPositionKey) {
                int type = 1;
                FileHelper::writeShort(&sgraFile, type);
                FileHelper::writeFloat(&sgraFile, frameKey.position.x);
                FileHelper::writeFloat(&sgraFile, frameKey.position.y);
                FileHelper::writeFloat(&sgraFile, frameKey.position.z);
            }
            if(frameKey.isRotationKey) {
                int type = 2;
                FileHelper::writeShort(&sgraFile, type);
                FileHelper::writeFloat(&sgraFile, frameKey.rotation.x);
                FileHelper::writeFloat(&sgraFile, frameKey.rotation.y);
                FileHelper::writeFloat(&sgraFile, frameKey.rotation.z);
                FileHelper::writeFloat(&sgraFile, frameKey.rotation.w);
            }
            if(frameKey.isScaleKey) {
                int type = 3;
                FileHelper::writeShort(&sgraFile, type);
                FileHelper::writeFloat(&sgraFile, frameKey.scale.x);
                FileHelper::writeFloat(&sgraFile, frameKey.scale.y);
                FileHelper::writeFloat(&sgraFile, frameKey.scale.z);
            }
            
        }
    }
    
    sgraFile.close();
}

void SGAnimationManager::storeTextAnimations(string filePath, SGNode *sgNode, int totalFrames , vector<Vector3> jointBasePositions, vector<int>totalKeyFrames)
{
    ofstream sgtaFile(filePath,ios::out | ios::binary);
    FileHelper::resetSeekPosition();
    FileHelper::writeShort(&sgtaFile, 1);
    FileHelper::writeShort(&sgtaFile, 1);
    FileHelper::writeShort(&sgtaFile, sgNode->joints.size());
    FileHelper::writeShort(&sgtaFile, totalFrames);
    FileHelper::writeShort(&sgtaFile, (short)totalKeyFrames.size());
    short lastKeyFrameId = totalKeyFrames[totalKeyFrames.size()-1];
    FileHelper::writeShort(&sgtaFile, lastKeyFrameId);
    
    Vector3 nodeInitPos = Vector3(0.0);
    int posCount = 0;
    
    for(int i = 0; i < totalKeyFrames.size(); i++)
    {
        short frame = totalKeyFrames[i];
        ActionKey nodeKey = sgNode->getKeyForFrame(frame);
        short activeKeys = 0;
        if(nodeKey.isPositionKey) activeKeys++;
        if(nodeKey.isRotationKey) activeKeys++;
        if(nodeKey.isScaleKey) activeKeys++;
        
        FileHelper::writeShort(&sgtaFile, frame); // Frame Id
        
        FileHelper::writeShort(&sgtaFile, activeKeys);
        
        if(nodeKey.isPositionKey) {
            int type = 1;
            FileHelper::writeShort(&sgtaFile, type);
            FileHelper::writeFloat(&sgtaFile, (nodeKey.position.x - nodeInitPos.x));
            FileHelper::writeFloat(&sgtaFile, (nodeKey.position.y - nodeInitPos.y));
            FileHelper::writeFloat(&sgtaFile, (nodeKey.position.z - nodeInitPos.z));
            if(posCount == 0) nodeInitPos = nodeKey.position;
            posCount++;
        }
        if(nodeKey.isRotationKey) {
            int type = 2;
            FileHelper::writeShort(&sgtaFile, type);
            FileHelper::writeFloat(&sgtaFile, nodeKey.rotation.x);
            FileHelper::writeFloat(&sgtaFile, nodeKey.rotation.y);
            FileHelper::writeFloat(&sgtaFile, nodeKey.rotation.z);
            FileHelper::writeFloat(&sgtaFile, nodeKey.rotation.w);
        }
        if(nodeKey.isScaleKey) {
            int type = 3;
            FileHelper::writeShort(&sgtaFile, type);
            FileHelper::writeFloat(&sgtaFile, nodeKey.scale.x);
            FileHelper::writeFloat(&sgtaFile, nodeKey.scale.y);
            FileHelper::writeFloat(&sgtaFile, nodeKey.scale.z);
        }
        
        for(int jointId = 1; jointId < sgNode->joints.size(); jointId++) {
            
            ActionKey frameKey = sgNode->joints[jointId]->getKeyForFrame(frame);
            short activeKeys = 0;
            if(frameKey.isPositionKey) activeKeys++;
            if(frameKey.isRotationKey) activeKeys++;
            if(frameKey.isScaleKey) activeKeys++;
            
            FileHelper::writeShort(&sgtaFile, activeKeys);
            
            if(frameKey.isPositionKey) {
                int type = 1;
                
                Vector3 offsetPos = Vector3(frameKey.position.x-jointBasePositions[jointId].x,frameKey.position.y-jointBasePositions[jointId].y,frameKey.position.z-jointBasePositions[jointId].z);
                
                FileHelper::writeShort(&sgtaFile, type);
                FileHelper::writeFloat(&sgtaFile, offsetPos.x);
                FileHelper::writeFloat(&sgtaFile, offsetPos.y);
                FileHelper::writeFloat(&sgtaFile, offsetPos.z);
            }
            if(frameKey.isRotationKey) {
                int type = 2;
                FileHelper::writeShort(&sgtaFile, type);
                FileHelper::writeFloat(&sgtaFile, frameKey.rotation.x);
                FileHelper::writeFloat(&sgtaFile, frameKey.rotation.y);
                FileHelper::writeFloat(&sgtaFile, frameKey.rotation.z);
                FileHelper::writeFloat(&sgtaFile, frameKey.rotation.w);
            }
            if(frameKey.isScaleKey) {
                int type = 3;
                FileHelper::writeShort(&sgtaFile, type);
                FileHelper::writeFloat(&sgtaFile, frameKey.scale.x);
                FileHelper::writeFloat(&sgtaFile, frameKey.scale.y);
                FileHelper::writeFloat(&sgtaFile, frameKey.scale.z);
            }
            
        }
    }
    
    sgtaFile.close();
}


