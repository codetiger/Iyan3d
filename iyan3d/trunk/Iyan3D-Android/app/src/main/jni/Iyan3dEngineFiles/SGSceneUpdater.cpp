//
//  SGSceneUpdater.cpp
//  Iyan3D
//
//  Created by Karthik on 26/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#define CONTROLS_MARKED_DISTANCE_FROM_CAMERA 6.17
#define CONTROLS_MARKED_SCALE 0.27
#define CONTROLS_MARKED_DISTANCE_FROM_NODE 1.019


#include "HeaderFiles/SGSceneUpdater.h"
#include "HeaderFiles/SGEditorScene.h"

SGEditorScene * updatingScene;

SGSceneUpdater::SGSceneUpdater(SceneManager* smgr, void* scene)
{
    this->smgr = smgr;
    updatingScene = (SGEditorScene*)scene;
}

SGSceneUpdater::~SGSceneUpdater()
{
    
}

void SGSceneUpdater::setDataForFrame(int frame)
{
    if(!updatingScene || !smgr)
        return;

    bool lightChanged = false;
    for (unsigned long i = 0; i < updatingScene->nodes.size(); i++) {
        Vector3 position = KeyHelper::getKeyInterpolationForFrame<int, SGPositionKey, Vector3>(frame, updatingScene->nodes[i]->positionKeys);
        Quaternion rotation = KeyHelper::getKeyInterpolationForFrame<int, SGRotationKey, Quaternion>(frame, updatingScene->nodes[i]->rotationKeys,true);
        Vector3 scale = KeyHelper::getKeyInterpolationForFrame<int, SGScaleKey, Vector3>(frame, updatingScene->nodes[i]->scaleKeys);
        int visibilityKeyindex = KeyHelper::getKeyIndex(updatingScene->nodes[i]->visibilityKeys, frame);
        
        SGNode* sgNode = updatingScene->nodes[i];
        
        if(visibilityKeyindex != -1){
           sgNode->props.isVisible = sgNode->visibilityKeys[visibilityKeyindex].visibility;
        }
        sgNode->setPositionOnNode(position);
        if(sgNode->getType() == NODE_ADDITIONAL_LIGHT) {
            if(sgNode->rotationKeys.size() > 0 && rotation.w >= 10.0) {
                sgNode->props.vertexColor = Vector3(rotation.x,rotation.y,rotation.z);
                sgNode->props.nodeSpecificFloat = rotation.w;
                lightChanged = true;
            }
        } else if(sgNode->getType() == NODE_LIGHT) {
            if(sgNode->scaleKeys.size() > 0) {
                lightChanged = true;
            }
        } else {
            sgNode->setRotationOnNode(rotation);
            sgNode->setScaleOnNode(scale);
        }
        for (int j = 0; j < sgNode->joints.size(); j++) {
            SGJoint *joint = sgNode->joints[j];
            Quaternion rotation = KeyHelper::getKeyInterpolationForFrame<int, SGRotationKey, Quaternion>(frame,joint->rotationKeys,true);
            bool changed = joint->setRotationOnNode(rotation);
            if(updatingScene->nodes[i]->getType() == NODE_TEXT) {
                Vector3 jointPosition = KeyHelper::getKeyInterpolationForFrame<int, SGPositionKey, Vector3>(frame, joint->positionKeys);
                changed = joint->setPositionOnNode(jointPosition);
                Vector3 jointScale = KeyHelper::getKeyInterpolationForFrame<int, SGScaleKey, Vector3>(frame, joint->scaleKeys);
                changed = joint->setScaleOnNode(jointScale);
            }
        }
        
    }
    updateControlsOrientaion(updatingScene);
    updateLightCamera();
    if(lightChanged && !updatingScene->isPlaying)
        updateLightProperties(frame);
}

void SGSceneUpdater::setKeysForFrame(int frame)
{
    if(!updatingScene || !smgr)
        return;

    for (unsigned long i = 0; i < updatingScene->nodes.size(); i++) {
        SGNode *sgNode = updatingScene->nodes[i];
        int posKeyindex = KeyHelper::getKeyIndex(sgNode->positionKeys, frame);
        if(posKeyindex != -1){
            sgNode->setPositionOnNode(sgNode->positionKeys[posKeyindex].position);
        }
        int rotKeyindex = KeyHelper::getKeyIndex(sgNode->rotationKeys, frame);
        int scaleKeyindex = KeyHelper::getKeyIndex(sgNode->scaleKeys, frame);
        if(sgNode->getType() != NODE_LIGHT && sgNode->getType() != NODE_ADDITIONAL_LIGHT) {
            
            if(rotKeyindex != -1){
                sgNode->setRotationOnNode(sgNode->rotationKeys[rotKeyindex].rotation);
            }
            
            if(scaleKeyindex != -1){
                sgNode->setScaleOnNode(sgNode->scaleKeys[scaleKeyindex].scale);
            }
            
            int visibilityKeyindex = KeyHelper::getKeyIndex(sgNode->visibilityKeys, frame);
            if(visibilityKeyindex != -1){
                sgNode->props.isVisible =  sgNode->visibilityKeys[visibilityKeyindex].visibility;
            }
        }
        
        for (int j = 0; j < sgNode->joints.size(); j++) {
            SGJoint *joint = sgNode->joints[j];
            
            rotKeyindex = KeyHelper::getKeyIndex(joint->rotationKeys, frame);
            if(rotKeyindex != -1){
                joint->setRotationOnNode(joint->rotationKeys[rotKeyindex].rotation);
            }
            
            if(sgNode->getType() == NODE_TEXT) {
                
                posKeyindex = KeyHelper::getKeyIndex(joint->positionKeys, frame);
                if(posKeyindex != -1){
                    joint->setPositionOnNode(joint->positionKeys[posKeyindex].position);
                }
                
                scaleKeyindex = KeyHelper::getKeyIndex(joint->scaleKeys, frame);
                if(scaleKeyindex != -1){
                    joint->setScaleOnNode(joint->scaleKeys[scaleKeyindex].scale);
                }
            }
        }
    }
    updateControlsOrientaion();
    updateLightCamera();
}


void SGSceneUpdater::updateControlsMaterial()
{
    if(!updatingScene || !smgr)
        return;

    updatingScene->renHelper->setControlsVisibility();
    if(!updatingScene->isControlSelected)
        return;
    int controlStartToVisible = (updatingScene->controlType == MOVE) ? X_MOVE : X_ROTATE;
    int controlEndToVisible = (updatingScene->controlType == MOVE) ? Z_MOVE : Z_ROTATE;
    for(int i = controlStartToVisible;i <= controlEndToVisible;i++){
        if(i == updatingScene->selectedControlId)
            updatingScene->sceneControls[i]->node->setMaterial(smgr->getMaterialByIndex(SHADER_COLOR));
        else
            updatingScene->sceneControls[i]->node->setMaterial(smgr->getMaterialByIndex(SHADER_COLOR));
    }
}

void SGSceneUpdater::updateControlsOrientaion(bool forRTT)
{
    if(!updatingScene || !smgr)
        return;

    updatingScene->renHelper->setControlsVisibility(updatingScene);
    
    if(!updatingScene->isNodeSelected)
        return;
    int controlStartIndex = (updatingScene->controlType == MOVE) ? X_MOVE : X_ROTATE;
    int controlEndIndex = (updatingScene->controlType == MOVE) ? Z_MOVE : Z_ROTATE;
    Vector3 nodePos;
    if(updatingScene->isJointSelected && updatingScene->selectedJoint)
        nodePos = updatingScene->selectedJoint->jointNode->getAbsoluteTransformation().getTranslation();
    else if( updatingScene->isNodeSelected && updatingScene->selectedNode)
        nodePos = updatingScene->selectedNode->node->getAbsoluteTransformation().getTranslation();
    
    float distanceFromCamera = nodePos.getDistanceFrom(smgr->getActiveCamera()->getPosition());
    float ctrlScale = ((distanceFromCamera / CONTROLS_MARKED_DISTANCE_FROM_CAMERA) * CONTROLS_MARKED_SCALE);
    
    ctrlScale = forRTT ? (ctrlScale * 1.5) : ctrlScale;
    
    float ctrlDistanceFromNode = ((distanceFromCamera / CONTROLS_MARKED_DISTANCE_FROM_CAMERA) * CONTROLS_MARKED_DISTANCE_FROM_NODE);
    
    ctrlDistanceFromNode = forRTT ? (ctrlDistanceFromNode/1.5) : ctrlDistanceFromNode;
    
    for(int i = controlStartIndex;i <= controlEndIndex;i++){
        SGNode* currentControl = updatingScene->sceneControls[i];
        currentControl->node->setScale(Vector3(ctrlScale));
        switch(i%3) {
            case 0:
                currentControl->node->setPosition(Vector3(nodePos) + Vector3(ctrlDistanceFromNode,0,0));
                currentControl->node->setRotationInDegrees(Vector3(0.0,180.0,0.0));
                break;
            case 1:
                currentControl->node->setPosition(Vector3(nodePos) + Vector3(0,ctrlDistanceFromNode,0));
                currentControl->node->setRotationInDegrees(Vector3(0.0));
                break;
            case 2:
                currentControl->node->setPosition(Vector3(nodePos) + Vector3(0,0,ctrlDistanceFromNode));
                currentControl->node->setRotationInDegrees(Vector3(0.0));
                break;
        }
        
        if(updatingScene->isJointSelected && updatingScene->selectedNode->getType() == NODE_TEXT) {
            Vector3 rot = updatingScene->selectedNode->node->getRotationInRadians();
            Vector3 delta;
            switch(i%3) {
                case 0:
                    //rot.y = (180.0 - (rot.y * RADTODEG)) * DEGTORAD;
                    delta = Vector3(-ctrlDistanceFromNode, 0, 0);
                    break;
                case 1:
                    delta = Vector3(0, ctrlDistanceFromNode, 0);
                    break;
                case 2:
                    delta = Vector3(0, 0, ctrlDistanceFromNode);
                    break;
            }
            Mat4 rotMat;
            rotMat.setRotationRadians(rot);
            rotMat.rotateVect(delta);
            
            currentControl->node->setRotationInRadians(rot);
            currentControl->node->setPosition(Vector3(nodePos) + delta);
        }
        
        currentControl->node->updateAbsoluteTransformation();
    }
}

void SGSceneUpdater::changeCameraView(CAMERA_VIEW_MODE mode)
{
    Vector3 cmapos = CameraViewHelper::getCameraPosition(mode);
    shared_ptr<CameraNode> activeCam = updatingScene->viewCamera;
    activeCam->setPosition(CameraViewHelper::getCameraPosition(mode));
    activeCam->updateAbsoluteTransformation();
    updatingScene->cameraTarget = Vector3(0.0);
    updatingScene->cameraRadius = 20.0;
    activeCam->setTarget(updatingScene->cameraTarget);
    activeCam->setUpVector(Vector3(0.0,1.0,0.0));
    activeCam->updateAbsoluteTransformation();
    Vector3 camForward = (activeCam->getTarget() - activeCam->getPosition()).normalize();
    Vector3 camRight = (camForward.crossProduct(activeCam->getUpVector())).normalize();
    Vector3 camUpReal = (camRight.crossProduct(camForward)).normalize();
    activeCam->setUpVector(camUpReal);
    updateLightCamera();
}

void SGSceneUpdater::updateLightCamera()
{
    if(!updatingScene || !smgr)
        return;

    if(updatingScene->nodes.size() > NODE_LIGHT) {
        updatingScene->nodes[NODE_LIGHT]->faceUserCamera(updatingScene->viewCamera,updatingScene->currentFrame);
        updateLightCam(updatingScene->nodes[NODE_LIGHT]->node->getAbsolutePosition());
    }
}

void SGSceneUpdater::updateLightCam(Vector3 position)
{
    if(!updatingScene || !smgr)
        return;

    updatingScene->lightCamera->setPosition(position);
    updatingScene->lightCamera->setTarget(Vector3(0.0));
    ShaderManager::lighCamViewMatrix = updatingScene->lightCamera->getViewMatrix();
    ShaderManager::lighCamProjMatrix = updatingScene->lightCamera->getProjectionMatrix();
    ShaderManager::camPos = smgr->getActiveCamera()->getAbsolutePosition();
}

void SGSceneUpdater::updateLightProperties(int frameId)
{
    if(!updatingScene || !smgr)
        return;

    int index = 0;
    for(int i = 0; i < updatingScene->nodes.size(); i++) {
        SGNode* sgNode = updatingScene->nodes[i];
        if(sgNode->getType() == NODE_LIGHT || sgNode->getType() == NODE_ADDITIONAL_LIGHT) {
            Vector3 position = KeyHelper::getKeyInterpolationForFrame<int, SGPositionKey, Vector3>(frameId, sgNode->positionKeys);
            Quaternion rotation = KeyHelper::getKeyInterpolationForFrame<int, SGRotationKey, Quaternion>(frameId,sgNode->rotationKeys,true);
            Vector3 scale = KeyHelper::getKeyInterpolationForFrame<int, SGScaleKey, Vector3>(frameId, sgNode->scaleKeys);
            
            Vector3 lightColor = (sgNode->getType() == NODE_LIGHT) ? Vector3(scale.x,scale.y,scale.z) : Vector3(rotation.x, rotation.y, rotation.z);
            float fadeDistance = (sgNode->getType() == NODE_LIGHT) ? 999.0 : rotation.w;
            
            if(index < ShaderManager::lightPosition.size())
                ShaderManager::lightPosition[index] = position;
            else
                ShaderManager::lightPosition.push_back(position);
            
            if(index < ShaderManager::lightColor.size())
                ShaderManager::lightColor[index] = Vector3(lightColor.x,lightColor.y,lightColor.z);
            else
                ShaderManager::lightColor.push_back(Vector3(lightColor.x,lightColor.y,lightColor.z));
            
            if(index < ShaderManager::lightFadeDistances.size())
                ShaderManager::lightFadeDistances[index] = fadeDistance;
            else
                ShaderManager::lightFadeDistances.push_back(fadeDistance);
            
            sgNode->props.vertexColor = Vector3(lightColor.x,lightColor.y,lightColor.z);
            index++;
        }
    }
    
    while (index < ShaderManager::lightPosition.size()) {
        if(ShaderManager::lightPosition.size() > 1)
            ShaderManager::lightPosition.pop_back();
        if(ShaderManager::lightColor.size() > 1)
            ShaderManager::lightColor.pop_back();
        if(ShaderManager::lightFadeDistances.size() > 1)
            ShaderManager::lightFadeDistances.pop_back();
    }
}

void SGSceneUpdater::resetMaterialTypes(bool isToonShader)
{
    if(!updatingScene || !smgr)
        return;

    int count = (int)ShaderManager::lightPosition.size();
    
    switch (count) {
            
        case 1: {
            commonType = SHADER_COMMON_L1;
            commonSkinType = SHADER_COMMON_SKIN_L1;
            vertexColorTextType = SHADER_VERTEX_COLOR_SHADOW_SKIN_L1;
            break;
        }
        case 2: {
            commonType = SHADER_COMMON_L2;
            commonSkinType = SHADER_COMMON_SKIN_L2;
            vertexColorTextType = SHADER_VERTEX_COLOR_SHADOW_SKIN_L2;
            break;
        }
        case 3: {
            commonType = SHADER_COMMON_L3;
            commonSkinType = SHADER_COMMON_SKIN_L3;
            vertexColorTextType = SHADER_VERTEX_COLOR_SHADOW_SKIN_L3;
            break;
        }
        case 4: {
            commonType = SHADER_COMMON_L4;
            commonSkinType = SHADER_COMMON_SKIN_L4;
            vertexColorTextType = SHADER_VERTEX_COLOR_SHADOW_SKIN_L4;
            break;
        }
        case 5: {
            commonType = SHADER_COMMON_L5;
            commonSkinType = SHADER_COMMON_SKIN_L5;
            vertexColorTextType = SHADER_VERTEX_COLOR_SHADOW_SKIN_L5;
            break;
        }
        default:
            break;
    }
    
    for(int index = 0; index < updatingScene->nodes.size(); index++)
    {
        SGNode *sgNode = updatingScene->nodes[index];
        if(updatingScene->selectedNodeId == index) {
            //DO NOTHING
        } else {
            switch (sgNode->getType()) {
                case NODE_CAMERA: {
                    sgNode->node->setMaterial(smgr->getMaterialByIndex(SHADER_VERTEX_COLOR_L1));
                    break;
                }
                case NODE_LIGHT:
                case NODE_SGM:
                case NODE_OBJ:
                case NODE_IMAGE: {
                    sgNode->node->setMaterial(smgr->getMaterialByIndex((isToonShader && sgNode->getType() != NODE_LIGHT) ? SHADER_TOON : commonType));
                    break;
                }
                case NODE_RIG: {
                    sgNode->node->setMaterial(smgr->getMaterialByIndex((isToonShader) ? SHADER_TOON_SKIN :commonSkinType));
                    break;
                }
                case NODE_TEXT: {
                    sgNode->node->setMaterial(smgr->getMaterialByIndex((isToonShader) ? SHADER_VERTEX_COLOR_SKIN_TOON: vertexColorTextType));
                    break;
                }
                    
                default:
                    break;
            }
        }
    }
    
}

void SGSceneUpdater::reloadKeyFrameMap()
{
    if(!updatingScene || !smgr)
        return;

    if(updatingScene->selectedNodeId != NOT_SELECTED) {
        SGNode *selectedMesh = updatingScene->nodes[updatingScene->selectedNodeId];
        bool searchPos = true,searchRot = false,searchScale = false;
        updatingScene->isKeySetForFrame.clear();
        searchRot = (selectedMesh->getType() == NODE_LIGHT) ? false:true;
        searchScale = (selectedMesh->getType() > NODE_LIGHT) ? true:false;
        if(searchPos){
            for(unsigned long i = 0; i < selectedMesh->positionKeys.size(); i++)
                updatingScene->isKeySetForFrame.insert(pair<int,int>(selectedMesh->positionKeys[i].id,selectedMesh->positionKeys[i].id));
        }
        if(searchRot){
            for(unsigned long i = 0; i < selectedMesh->rotationKeys.size(); i++)
                updatingScene->isKeySetForFrame.insert(pair<int,int>(selectedMesh->rotationKeys[i].id,selectedMesh->rotationKeys[i].id));
        }
        if(searchScale){
            for(unsigned long i = 0; i < selectedMesh->scaleKeys.size(); i++)
                updatingScene->isKeySetForFrame.insert(pair<int,int>(selectedMesh->scaleKeys[i].id,selectedMesh->scaleKeys[i].id));
        }
        
        for( unsigned long i = 0; i < selectedMesh->joints.size(); i++){
            for(unsigned long j = 0; j < selectedMesh->joints[i]->rotationKeys.size(); j++) {
                updatingScene->isKeySetForFrame.insert(pair<int,int>(selectedMesh->joints[i]->rotationKeys[j].id,selectedMesh->joints[i]->rotationKeys[j].id));
            }
            if(selectedMesh->getType() == NODE_TEXT) {
                for(unsigned long j = 0; j < selectedMesh->joints[i]->positionKeys.size(); j++)
                    updatingScene->isKeySetForFrame.insert(pair<int,int>(selectedMesh->joints[i]->positionKeys[j].id,selectedMesh->joints[i]->positionKeys[j].id));
                for(unsigned long j = 0; j < selectedMesh->joints[i]->scaleKeys.size(); j++)
                    updatingScene->isKeySetForFrame.insert(pair<int,int>(selectedMesh->joints[i]->scaleKeys[j].id,selectedMesh->joints[i]->scaleKeys[j].id));
            }
        }
    }
    else
        updatingScene->isKeySetForFrame.clear();
}

void SGSceneUpdater::setCameraProperty(float fov , int resolutionType)
{
    if(!updatingScene || !smgr)
        return;
    updatingScene->cameraFOV = fov;
    updatingScene->cameraResolutionType = resolutionType;
    updatingScene->renderCamera->setFOVInRadians(updatingScene->cameraFOV * PI / 180.0f);
}


