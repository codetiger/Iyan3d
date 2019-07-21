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

#define JOINT_MARKED_DISTANCE_FROM_CAMERA 6.17
#define JOINT_MARKED_SCALE 0.27


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

void SGSceneUpdater::setDataForFrame(int frame, bool updateBB)
{
    bool shouldUpdateBB = (!updatingScene->isPlaying) && updateBB;
    if(!updatingScene || !smgr)
        return;

    bool lightChanged = false;
    
    updatingScene->shadowsOff = ShaderManager::shadowsOff;
    
    for (unsigned long i = 0; i < updatingScene->nodes.size(); i++) {

        if(updatingScene->nodes[i]->getType() == NODE_VIDEO) {
            Texture* nodeTex = updatingScene->nodes[i]->materialProps[0]->getTextureOfType(NODE_TEXTURE_TYPE_COLORMAP);
        nodeTex->updateTexture(ConversionHelper::getStringForWString(updatingScene->nodes[i]->name), frame);
        }

        Vector3 position = KeyHelper::getKeyInterpolationForFrame<int, SGPositionKey, Vector3>(frame, updatingScene->nodes[i]->positionKeys);
        Quaternion rotation = KeyHelper::getKeyInterpolationForFrame<int, SGRotationKey, Quaternion>(frame, updatingScene->nodes[i]->rotationKeys,true);
        Vector3 scale = KeyHelper::getKeyInterpolationForFrame<int, SGScaleKey, Vector3>(frame, updatingScene->nodes[i]->scaleKeys);
        int visibilityKeyindex = KeyHelper::getKeyIndex(updatingScene->nodes[i]->visibilityKeys, frame);

        SGNode* sgNode = updatingScene->nodes[i];

        if(visibilityKeyindex != -1)
            sgNode->getProperty(VISIBILITY).value.x = sgNode->visibilityKeys[visibilityKeyindex].visibility;

        sgNode->setPositionOnNode(position, shouldUpdateBB);
        
        if(sgNode->getType() == NODE_LIGHT || sgNode->getType() == NODE_ADDITIONAL_LIGHT) {
        	if(updatingScene->directionLine->node->getVisible() || updatingScene->lightCircles->node->getVisible())
                updatingScene->updateDirectionLine();

            if(sgNode->scaleKeys.size() > 0) {
                sgNode->getProperty(VERTEX_COLOR).value = Vector4(scale.x, scale.y, scale.z, 0);
                lightChanged = true;
            }
        } else {
            sgNode->setRotationOnNode(rotation, shouldUpdateBB);
            sgNode->setScaleOnNode(scale, shouldUpdateBB);
        }
        
        for (int j = 0; j < sgNode->joints.size(); j++) {
            SGJoint *joint = sgNode->joints[j];
            Quaternion rotation = KeyHelper::getKeyInterpolationForFrame<int, SGRotationKey, Quaternion>(frame,joint->rotationKeys,true);
            bool changed = joint->setRotationOnNode(rotation, shouldUpdateBB);
            
            if(updatingScene->nodes[i]->getType() == NODE_TEXT_SKIN) {
                Vector3 jointPosition = KeyHelper::getKeyInterpolationForFrame<int, SGPositionKey, Vector3>(frame, joint->positionKeys);
                changed = joint->setPositionOnNode(jointPosition, shouldUpdateBB);
                Vector3 jointScale = KeyHelper::getKeyInterpolationForFrame<int, SGScaleKey, Vector3>(frame, joint->scaleKeys);
                changed = joint->setScaleOnNode(jointScale, shouldUpdateBB);
            }
        }

        if(sgNode->node->type == NODE_TYPE_SKINNED && sgNode->node->skinType == CPU_SKIN) {
            (dynamic_pointer_cast<AnimatedMeshNode>(sgNode->node))->updateMeshCache();
        }

    }
    updateControlsOrientaion(updatingScene);
    updateJointSpheresPosition();
    updateLightCamera();
    if(lightChanged)
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
                sgNode->getProperty(VISIBILITY).value.x = sgNode->visibilityKeys[visibilityKeyindex].visibility;
            }
        }
        
        for (int j = 0; j < sgNode->joints.size(); j++) {
            SGJoint *joint = sgNode->joints[j];
            
            rotKeyindex = KeyHelper::getKeyIndex(joint->rotationKeys, frame);
            if(rotKeyindex != -1){
                joint->setRotationOnNode(joint->rotationKeys[rotKeyindex].rotation);
            }
            
            if(sgNode->getType() == NODE_TEXT_SKIN) {
                
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
    updatingScene->updateDirectionLine();
    updateLightCamera();
}

void SGSceneUpdater::updateControlsMaterial()
{
    if(!updatingScene || !smgr)
        return;

    updatingScene->renHelper->setControlsVisibility();
    if(!updatingScene->isControlSelected)
        return;
    int controlStartToVisible = (updatingScene->controlType == MOVE) ? X_MOVE : (updatingScene->controlType == ROTATE) ? X_ROTATE : X_SCALE;
    int controlEndToVisible = (updatingScene->controlType == MOVE) ? Z_MOVE : (updatingScene->controlType == ROTATE) ? Z_ROTATE : Z_SCALE;
    for(int i = controlStartToVisible;i <= controlEndToVisible;i++){
        if(i == updatingScene->selectedControlId)
            updatingScene->sceneControls[i]->node->setMaterial(smgr->getMaterialByIndex(SHADER_COLOR));
        else {
            updatingScene->sceneControls[i]->getProperty(VERTEX_COLOR).value = Vector4(Vector3(-1.0), 0.0);
            updatingScene->sceneControls[i]->node->setMaterial(smgr->getMaterialByIndex(SHADER_MESH));
        }
    }
}

void SGSceneUpdater::updateControlsOrientaion(bool forRTT)
{
    if(!updatingScene || !smgr)
        return;

    updatingScene->renHelper->setControlsVisibility(true);
    bool isNodeSelected = updatingScene->hasNodeSelected();
    bool isJointSelected = updatingScene->hasJointSelected();
    SGNode* selectedNode = updatingScene->getSelectedNode();
    SGJoint* selectedJoint = updatingScene->getSelectedJoint();

    if(isJointSelected && updatingScene->controlType == SCALE)
        updatingScene->controlType = MOVE;

    if((!isNodeSelected || !selectedNode) && (!isJointSelected || !selectedJoint) && updatingScene->selectedNodeIds.size() <= 0)
        return;
    
    int controlStartIndex = (updatingScene->controlType == MOVE) ? X_MOVE : (updatingScene->controlType == ROTATE) ? X_ROTATE : X_SCALE;
    int controlEndIndex = (updatingScene->controlType == MOVE) ? Z_MOVE : (updatingScene->controlType == ROTATE) ? Z_ROTATE : Z_SCALE;
    
    Vector3 nodePos;
    if(updatingScene->selectedNodeIds.size() > 0 && updatingScene->getParentNode())
        nodePos = updatingScene->getPivotPoint(false);
    else if(isJointSelected && selectedJoint)
        nodePos = (selectedJoint->jointNode) ? selectedJoint->jointNode->getAbsoluteTransformation().getTranslation() : Vector3(0.0);
    else if(isNodeSelected && selectedNode)
        nodePos = (selectedNode->node) ? selectedNode->node->getAbsoluteTransformation().getTranslation() : Vector3(0.0);
    
    float distanceFromCamera = nodePos.getDistanceFrom(smgr->getActiveCamera()->getPosition());
    float ctrlScale = ((distanceFromCamera / CONTROLS_MARKED_DISTANCE_FROM_CAMERA) * CONTROLS_MARKED_SCALE);
    ctrlScale = forRTT ? (ctrlScale * 1.5) : ctrlScale * 1.3;
    
    float ctrlDistanceFromNode = ((distanceFromCamera / CONTROLS_MARKED_DISTANCE_FROM_CAMERA) * CONTROLS_MARKED_DISTANCE_FROM_NODE);
    ctrlDistanceFromNode = forRTT ? (ctrlDistanceFromNode/1.2) : ctrlDistanceFromNode;
    
    for(int i = controlStartIndex;i <= controlEndIndex;i++){
        SGNode* currentControl = updatingScene->sceneControls[i];
        currentControl->node->setScale(Vector3(ctrlScale));
        switch(i%3) {
            case 0:
                currentControl->node->setPosition(Vector3(nodePos) + Vector3(ctrlDistanceFromNode, 0, 0));
                currentControl->node->setRotation(Quaternion(Vector3(0.0, 180.0, 0.0) * DEGTORAD));
                break;
            case 1:
                currentControl->node->setPosition(Vector3(nodePos) + Vector3(0, ctrlDistanceFromNode, 0));
                currentControl->node->setRotation(Quaternion(Vector3(0.0, 0.0, (i == Y_ROTATE) ? 180.0 : 0.0) * DEGTORAD));
                break;
            case 2:
                currentControl->node->setPosition(Vector3(nodePos) + Vector3(0, 0, ctrlDistanceFromNode));
                currentControl->node->setRotation(Quaternion(Vector3(0.0, (i == Z_ROTATE) ? 90.0 : 0.0, (i == Z_ROTATE) ? -90.0 : 0.0) * DEGTORAD));
                break;
        }
        
        if(isJointSelected && selectedNode->getType() == NODE_TEXT_SKIN) {
            Vector3 delta;
            switch(i%3) {
                case 0:
                    delta = Vector3(ctrlDistanceFromNode, 0, 0);
                    break;
                case 1:
                    delta = Vector3(0, ctrlDistanceFromNode, 0);
                    break;
                case 2:
                    delta = Vector3(0, 0, ctrlDistanceFromNode);
                    break;
            }

            Quaternion rot = selectedNode->node->getRotation();

            Mat4 rotMat = rot.getMatrix();
            rotMat.rotateVect(delta);

            Quaternion finalRotation = MathHelper::RotateNodeInWorld(currentControl->node->getRotation(), rot);

            currentControl->node->setRotation(finalRotation);
            currentControl->node->setPosition(nodePos + delta);
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
        updateLightCam(updatingScene->nodes[NODE_LIGHT]->node->getPosition());
    }
}

void SGSceneUpdater::updateJointSpheres()
{
    if(updatingScene->selectedNodeId == NOT_SELECTED || (updatingScene->nodes[updatingScene->selectedNodeId]->getType() != NODE_RIG && updatingScene->nodes[updatingScene->selectedNodeId]->getType() != NODE_TEXT_SKIN))
        return;
    
    shared_ptr<AnimatedMeshNode> animNode = dynamic_pointer_cast<AnimatedMeshNode>(updatingScene->nodes[updatingScene->selectedNodeId]->node);
    int bonesCount = animNode->getJointCount();
    
    updateJointSpheresPosition();
    

    for(int i = 0;i < bonesCount;i++) {
        
            Vector3 pos = updatingScene->jointSpheres[i]->node->getPosition();
            float distanceFromCamera = pos.getDistanceFrom(smgr->getActiveCamera()->getPosition());
            float jointScale = ((distanceFromCamera / JOINT_MARKED_DISTANCE_FROM_CAMERA) * JOINT_MARKED_SCALE);
            jointScale = jointScale * 1.5;
            
            Vector3 sphereScale = Vector3(jointScale);
            
            updatingScene->jointSpheres[i]->node->setScale(sphereScale);
    }

}

void SGSceneUpdater::updateJointSpheresPosition()
{
    if(updatingScene->selectedNodeId == NOT_SELECTED || (updatingScene->nodes[updatingScene->selectedNodeId]->getType() != NODE_RIG && updatingScene->nodes[updatingScene->selectedNodeId]->getType() != NODE_TEXT_SKIN))
        return;
    
    shared_ptr<AnimatedMeshNode> animNode = dynamic_pointer_cast<AnimatedMeshNode>(updatingScene->nodes[updatingScene->selectedNodeId]->node);
    int bonesCount = animNode->getJointCount();

    for(int i = 0;i < bonesCount;i++) {
        
        shared_ptr<JointNode> jointNode = animNode->getJointNode(i);
        updatingScene->jointSpheres[i]->node->setPosition(jointNode->getAbsolutePosition());
    }
}

void SGSceneUpdater::updateLightCam(Vector3 position)
{
    if(!updatingScene || !smgr)
        return;

    Vector3 lightPos = position;
    Vector3 target = Vector3(0.0);
    
    if(updatingScene->nodes.size() <= 2)
        return;
    if(updatingScene->nodes[NODE_LIGHT]->getProperty(LIGHT_TYPE).value.x == (int)DIRECTIONAL_LIGHT) {
        
        Quaternion rotation = KeyHelper::getKeyInterpolationForFrame<int, SGRotationKey, Quaternion>(updatingScene->currentFrame, updatingScene->nodes[NODE_LIGHT]->rotationKeys,true);

        Vector3 direction = Vector3(0.0, 1.0, 0.0);
        Mat4 rotMat;
        rotMat.setRotation(rotation);
        rotMat.rotateVect(direction);
        direction = direction.normalize();
        
        position = target - (direction * 50);
    }
    
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
            
            Vector3 posOrDir = KeyHelper::getKeyInterpolationForFrame<int, SGPositionKey, Vector3>(frameId, sgNode->positionKeys);
            

            Quaternion rotation = KeyHelper::getKeyInterpolationForFrame<int, SGRotationKey, Quaternion>(frameId,sgNode->rotationKeys,true);
            
            if(sgNode->getProperty(LIGHT_TYPE).value.x == (int)DIRECTIONAL_LIGHT) {
                posOrDir = Vector3(0.0, -1.0, 0.0);
                Mat4 rotMat;
                rotMat.setRotation(rotation);
                rotMat.rotateVect(posOrDir);
            }

            Vector3 scale = KeyHelper::getKeyInterpolationForFrame<int, SGScaleKey, Vector3>(frameId, sgNode->scaleKeys);
            
            Vector3 lightColor = Vector3(scale.x,scale.y,scale.z);
            float fadeDistance = (sgNode->getType() == NODE_LIGHT) ? 999.0 : sgNode->getProperty(SPECIFIC_FLOAT).value.x;
            
            if(index < ShaderManager::lightPosition.size())
                ShaderManager::lightPosition[index] = posOrDir;
            else
                ShaderManager::lightPosition.push_back(posOrDir);
            
            if(index < ShaderManager::lightColor.size())
                ShaderManager::lightColor[index] = Vector3(lightColor.x,lightColor.y,lightColor.z);
            else
                ShaderManager::lightColor.push_back(Vector3(lightColor.x,lightColor.y,lightColor.z));
            
            if(index < ShaderManager::lightFadeDistances.size())
                ShaderManager::lightFadeDistances[index] = fadeDistance;
            else
                ShaderManager::lightFadeDistances.push_back(fadeDistance);
            
            if(index < ShaderManager::lightTypes.size())
                ShaderManager::lightTypes[index] = sgNode->getProperty(LIGHT_TYPE).value.x;
            else
                ShaderManager::lightTypes.push_back(sgNode->getProperty(LIGHT_TYPE).value.x);
            
            sgNode->getProperty(VERTEX_COLOR).value = Vector4(lightColor.x, lightColor.y, lightColor.z, 1.0);
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
        if(ShaderManager::lightTypes.size() > 1)
            ShaderManager::lightTypes.pop_back();
    }
}

void SGSceneUpdater::resetMaterialTypes(bool isToonShader)
{
    if(!updatingScene || !smgr)
        return;

    commonType = SHADER_MESH;
    commonSkinType = SHADER_SKIN;
    vertexColorTextType = SHADER_TEXT_SKIN;
    vertexColorType = SHADER_MESH;
    
    for(int index = 0; index < updatingScene->nodes.size(); index++)
    {
        SGNode *sgNode = updatingScene->nodes[index];
        if(updatingScene->selectedNodeId == index || std::find(updatingScene->selectedNodeIds.begin(), updatingScene->selectedNodeIds.end(), index) != updatingScene->selectedNodeIds.end()) {
            //DO NOTHING
        } else {
                switch (sgNode->getType()) {
                    case NODE_CAMERA: {
                        sgNode->node->setMaterial(smgr->getMaterialByIndex(SHADER_MESH));
                        break;
                    }
                    case NODE_OBJ:
                    case NODE_SGM:
                    case NODE_IMAGE:
                    case NODE_TEXT:
                    case NODE_VIDEO:{
                        sgNode->node->setMaterial(smgr->getMaterialByIndex(commonType));
                        break;
                    }
                        
                    case NODE_LIGHT:
                    case NODE_ADDITIONAL_LIGHT:{
                        sgNode->node->setMaterial(smgr->getMaterialByIndex(SHADER_COLOR));
                        break;
                    }
                    case NODE_RIG: {
                        if(sgNode->node->skinType == CPU_SKIN)
                            sgNode->node->setMaterial(smgr->getMaterialByIndex(commonType));
                        else
                            sgNode->node->setMaterial(smgr->getMaterialByIndex(commonSkinType));
                        break;
                    }
                    case NODE_TEXT_SKIN: {
                        sgNode->node->setMaterial(smgr->getMaterialByIndex(vertexColorTextType));
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

    if(updatingScene->selectedNodeId != NOT_SELECTED && updatingScene->selectedNodeIds.size() <= 0) {
        SGNode *selectedMesh = updatingScene->nodes[updatingScene->selectedNodeId];
        bool searchPos = true,searchRot = false,searchScale = false;
        updatingScene->isKeySetForFrame.clear();
        searchRot = true;
        searchScale = (selectedMesh->getType() > NODE_CAMERA) ? true:false;
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
            if(selectedMesh->getType() == NODE_TEXT_SKIN) {
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
    updatingScene->nodes[NODE_CAMERA]->getProperty(FOV).value.x = fov;
    updatingScene->nodes[NODE_CAMERA]->getProperty(CAM_RESOLUTION).value.x = resolutionType;
    
    for(int pI = THOUSAND_EIGHTY; pI < TWO_FORTY + 1; pI++) {
        if(pI - CAM_CONSTANT == resolutionType)
            updatingScene->nodes[NODE_CAMERA]->getProperty((PROP_INDEX)pI).value.x = 1.0;
        else
            updatingScene->nodes[NODE_CAMERA]->getProperty((PROP_INDEX)pI).value.x = 0.0;
    }
    
    updatingScene->renderCamera->setFOVInRadians(updatingScene->nodes[NODE_CAMERA]->getProperty(FOV).value.x * PI / 180.0f);
}
